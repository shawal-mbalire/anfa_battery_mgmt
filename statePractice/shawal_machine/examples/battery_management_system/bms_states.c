#include "bms_states.h"
#include "hw_abstraction.h"
#include "../../lib/state_machine/logger.h"
#include <stdio.h>

// Helper function to handle unhandled events
void HandleUnhandledEvent(Event event, const char* state_name) {
    printf("Unhandled event %d in state: %s\n", event, state_name);
    // In a real system, this would log to non-volatile memory
}

// --- IDLE State Functions ---
void bms_idle_entry(void) {
    non_blocking_log("Entering BMS_IDLE_STATE.\n");
    // Disable all major outputs for a safe state
    set_gpio("ENABLE_LED_BUCK (PB13)", LOW);
    set_gpio("ENABLE_BAT_OUTPUT (PE3)", HIGH);  // Keep MCU powered
    set_gpio("MPPT_BQ_CHARGE_ENABLE (PE10)", HIGH);  // Active low, disable charger
    set_gpio("ENABLE_USB_60W (PD9)", LOW);
    set_gpio("ENABLE_USB_100W (PD5)", LOW);
    set_gpio("ENABLE_12V_RS485 (PD7)", LOW);
    set_gpio("ENABLE_DIAG_12V_RS485 (PB4)", LOW);
    set_gpio("RS485_RECEIVER_ENABLE (PA7)", LOW);  // Active low, enable receiver
    set_gpio("CAN_ENABLE (PD14)", HIGH);
}

void bms_idle_main(void) {
    // Continuously monitor for power sources and events
    adc_usb_c_voltage = read_adc("ADC_Con_1");
    adc_solar_voltage = read_adc("ADC_Solar_In");

    // Post events if power sources are detected
    if (adc_usb_c_voltage > 100) {
        PostBmsEvent(POWER_SOURCE_CONNECTED);
    } else if (adc_solar_voltage > 50) {
        PostBmsEvent(SOLAR_POWER_CONNECTED);
    }
    
    printf("BMS_IDLE_STATE: Polling - USB-C: %d, Solar: %d\n", adc_usb_c_voltage, adc_solar_voltage);
}

void bms_idle_exit(void) {
    non_blocking_log("Exiting BMS_IDLE_STATE.\n");
}

void bms_idle_transition(Event event) {
    switch (event) {
        case POWER_SOURCE_CONNECTED:
        case SOLAR_POWER_CONNECTED:
            ChangeState(&BMS_CHARGING_STATE);
            break;
        case LIGHT_SWITCH_TOGGLED:
            ChangeState(&BMS_LED_CONTROL_STATE);
            break;
        case SYSTEM_FAULT:
        case BMS_INTERRUPT:
        case USB_FAULT:
        case BATTERY_TEMP_FAULT:
            ChangeState(&BMS_FAULT_STATE);
            break;
        case GO_TO_SLEEP:
            ChangeState(&BMS_DEEP_SLEEP_STATE);
            break;
        default:
            if (event != EVENT_MAX) {
                HandleUnhandledEvent(event, "BMS_IDLE_STATE");
            }
            break;
    }
}

// --- CHARGING State Functions ---
void bms_charging_entry(void) {
    non_blocking_log("Entering BMS_CHARGING_STATE.\n");
    // Enable charger and current sense
    set_gpio("MPPT_BQ_QON (PE0)", HIGH);
    set_gpio("MPPT_BQ_CHARGE_ENABLE (PE10)", LOW);  // Active low, enable
    set_gpio("ENABLE_CURRENT_SENSE (PE1)", HIGH);

    // Configure power source
    if (adc_usb_c_voltage > 100) {
        non_blocking_log("Configuring for USB-C charging...\n");
        if (adc_usb_c_voltage < 200) {
            set_gpio("ENABLE_USB_60W (PD9)", HIGH);
            i2c_write("BQ25798", 60);
        } else {
            set_gpio("ENABLE_USB_100W (PD5)", HIGH);
            set_gpio("ENABLE_BUCK_BOOST_100W (PD6)", HIGH);
            i2c_write("BQ25798", 100);
        }
    } else if (adc_solar_voltage > 50) {
        non_blocking_log("Configuring for Solar charging...\n");
        i2c_write("BQ25798", 50);
    }
}

void bms_charging_main(void) {
    // Monitor charging status and safety
    int charge_status = i2c_read("BQ25798_STATUS");
    int bms_faults = i2c_read("BQ76907_FAULTS");
    adc_battery_ntc = read_adc("NTC_2_PACK");
    
    printf("BMS_CHARGING_STATE: Monitoring temperature. NTC: %d\n", adc_battery_ntc);
    
    // Check for fault conditions
    if (adc_battery_ntc > 3000 || adc_battery_ntc < 1000) {  // Temperature out of range
        PostBmsEvent(BATTERY_TEMP_FAULT);
    }
}

void bms_charging_exit(void) {
    non_blocking_log("Exiting BMS_CHARGING_STATE.\n");
    // Disable charging components
    set_gpio("MPPT_BQ_CHARGE_ENABLE (PE10)", HIGH);
    set_gpio("ENABLE_CURRENT_SENSE (PE1)", LOW);
    set_gpio("ENABLE_USB_60W (PD9)", LOW);
    set_gpio("ENABLE_USB_100W (PD5)", LOW);
    set_gpio("ENABLE_BUCK_BOOST_100W (PD6)", LOW);
}

void bms_charging_transition(Event event) {
    switch (event) {
        case POWER_SOURCE_DISCONNECTED:
        case CHARGE_COMPLETE:
            ChangeState(&BMS_IDLE_STATE);
            break;
        case SYSTEM_FAULT:
        case BMS_INTERRUPT:
        case USB_FAULT:
        case BATTERY_TEMP_FAULT:
        case CHARGER_COMM_FAULT:
        case BMS_COMM_FAULT:
            ChangeState(&BMS_FAULT_STATE);
            break;
        default:
            if (event != EVENT_MAX) {
                HandleUnhandledEvent(event, "BMS_CHARGING_STATE");
            }
            break;
    }
}

// --- DISCHARGING State Functions ---
void bms_discharging_entry(void) {
    non_blocking_log("Entering BMS_DISCHARGING_STATE.\n");
    set_gpio("ENABLE_CURRENT_SENSE (PE1)", HIGH);
}

void bms_discharging_main(void) {
    // Monitor battery conditions
    int bms_faults = i2c_read("BQ76907_FAULTS");
    adc_sys_current = read_adc("I_SYS_OUT");
    
    if (adc_sys_current > 1000) {  // High current threshold
        PostBmsEvent(HIGH_CURRENT_DETECTED);
    }
    
    printf("BMS_DISCHARGING_STATE: System current: %d\n", adc_sys_current);
}

void bms_discharging_exit(void) {
    non_blocking_log("Exiting BMS_DISCHARGING_STATE.\n");
    set_gpio("ENABLE_CURRENT_SENSE (PE1)", LOW);
}

void bms_discharging_transition(Event event) {
    switch (event) {
        case POWER_SOURCE_CONNECTED:
        case SOLAR_POWER_CONNECTED:
            ChangeState(&BMS_CHARGING_STATE);
            break;
        case LIGHT_SWITCH_TOGGLED:
            ChangeState(&BMS_LED_CONTROL_STATE);
            break;
        case BATTERY_CELL_UVLO:
        case HIGH_CURRENT_DETECTED:
        case BMS_INTERRUPT:
            ChangeState(&BMS_FAULT_STATE);
            break;
        default:
            if (event != EVENT_MAX) {
                HandleUnhandledEvent(event, "BMS_DISCHARGING_STATE");
            }
            break;
    }
}

// --- LED Control State Functions ---
void bms_led_control_entry(void) {
    non_blocking_log("Entering BMS_LED_CONTROL_STATE.\n");
    // Enable LED and 12V outputs
    set_gpio("ENABLE_LED_BUCK (PB13)", HIGH);
    set_gpio("LED_DISABLE (PD15)", LOW);
    set_gpio("ENABLE_12V_RS485 (PD7)", HIGH);
    set_gpio("ENABLE_DIAG_12V_RS485 (PB4)", HIGH);
    // PWM channels would be configured here for LED brightness
}

void bms_led_control_main(void) {
    printf("BMS_LED_CONTROL_STATE: LEDs and 12V outputs active.\n");
    // Monitor LED voltages and 12V rail faults
    if (read_gpio("FAULT_12V_RS485") == LOW) {
        PostBmsEvent(FAULT_RS485_12V);
    }
}

void bms_led_control_exit(void) {
    non_blocking_log("Exiting BMS_LED_CONTROL_STATE.\n");
    // Shut down LED and 12V outputs
    set_gpio("ENABLE_LED_BUCK (PB13)", LOW);
    set_gpio("ENABLE_12V_RS485 (PD7)", LOW);
    set_gpio("ENABLE_DIAG_12V_RS485 (PB4)", LOW);
}

void bms_led_control_transition(Event event) {
    switch (event) {
        case LIGHT_SWITCH_TOGGLED:  // Toggle off
            ChangeState(&BMS_IDLE_STATE);
            break;
        case SYSTEM_FAULT:
        case BATTERY_TEMP_FAULT:
        case FAULT_RS485_12V:
            ChangeState(&BMS_FAULT_STATE);
            break;
        default:
            if (event != EVENT_MAX) {
                HandleUnhandledEvent(event, "BMS_LED_CONTROL_STATE");
            }
            break;
    }
}

// --- FAULT State Functions ---
void bms_fault_entry(void) {
    non_blocking_log("Entering BMS_FAULT_STATE.\n");
    // Immediately disable ALL power outputs for safety
    set_gpio("ENABLE_LED_BUCK (PB13)", LOW);
    set_gpio("ENABLE_BAT_OUTPUT (PE3)", LOW);
    set_gpio("MPPT_BQ_CHARGE_ENABLE (PE10)", HIGH);
    set_gpio("ENABLE_BUCK_BOOST_100W (PD6)", LOW);
    set_gpio("ENABLE_USB_100W (PD5)", LOW);
    set_gpio("ENABLE_USB_60W (PD9)", LOW);
    set_gpio("ENABLE_12V_RS485 (PD7)", LOW);
    set_gpio("ENABLE_DIAG_12V_RS485 (PB4)", LOW);
    
    printf("*** CRITICAL FAULT DETECTED - ALL OUTPUTS DISABLED ***\n");
}

void bms_fault_main(void) {
    printf("BMS_FAULT_STATE: System locked, awaiting reset.\n");
}

void bms_fault_exit(void) {
    non_blocking_log("Exiting BMS_FAULT_STATE.\n");
}

void bms_fault_transition(Event event) {
    // In a real system, only a hardware reset would exit this state
    // For simulation, we'll stay locked
    if (event != EVENT_MAX) {
        HandleUnhandledEvent(event, "BMS_FAULT_STATE");
    }
}

// --- DEEP SLEEP State Functions ---
void bms_deep_sleep_entry(void) {
    non_blocking_log("Entering BMS_DEEP_SLEEP_STATE.\n");
    // Shut down peripherals for low power
    set_gpio("ENABLE_BAT_OUTPUT (PE3)", LOW);
    set_gpio("CAN_ENABLE (PD14)", LOW);
    set_gpio("RS485_RECEIVER_ENABLE (PA7)", HIGH);  // Active low, disable
}

void bms_deep_sleep_main(void) {
    printf("BMS_DEEP_SLEEP_STATE: Sleeping...\n");
    // MCU would enter low-power mode here
}

void bms_deep_sleep_exit(void) {
    non_blocking_log("Exiting BMS_DEEP_SLEEP_STATE.\n");
    // Re-initialize peripherals
}

void bms_deep_sleep_transition(Event event) {
    switch (event) {
        case WAKE_UP:
            ChangeState(&BMS_IDLE_STATE);
            break;
        default:
            if (event != EVENT_MAX) {
                HandleUnhandledEvent(event, "BMS_DEEP_SLEEP_STATE");
            }
            break;
    }
}

// --- State Definitions ---
const StateActions BMS_IDLE_STATE = {
    .entry_action = bms_idle_entry,
    .main_action = bms_idle_main,
    .exit_action = bms_idle_exit,
    .transition_signal = bms_idle_transition
};

const StateActions BMS_CHARGING_STATE = {
    .entry_action = bms_charging_entry,
    .main_action = bms_charging_main,
    .exit_action = bms_charging_exit,
    .transition_signal = bms_charging_transition
};

const StateActions BMS_DISCHARGING_STATE = {
    .entry_action = bms_discharging_entry,
    .main_action = bms_discharging_main,
    .exit_action = bms_discharging_exit,
    .transition_signal = bms_discharging_transition
};

const StateActions BMS_LED_CONTROL_STATE = {
    .entry_action = bms_led_control_entry,
    .main_action = bms_led_control_main,
    .exit_action = bms_led_control_exit,
    .transition_signal = bms_led_control_transition
};

const StateActions BMS_FAULT_STATE = {
    .entry_action = bms_fault_entry,
    .main_action = bms_fault_main,
    .exit_action = bms_fault_exit,
    .transition_signal = bms_fault_transition
};

const StateActions BMS_DEEP_SLEEP_STATE = {
    .entry_action = bms_deep_sleep_entry,
    .main_action = bms_deep_sleep_main,
    .exit_action = bms_deep_sleep_exit,
    .transition_signal = bms_deep_sleep_transition
};