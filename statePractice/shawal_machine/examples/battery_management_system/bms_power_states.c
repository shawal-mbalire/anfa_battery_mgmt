#include "bms_states.h"
#include "hw_abstraction.h"
#include "../../lib/state_machine/logger.h"
#include <stdio.h>

/*
 * BMS Power Management States
 * 
 * This file contains the core power management states:
 * - IDLE: System idle, monitoring for power sources
 * - CHARGING: Active charging from USB-C or Solar
 * - DISCHARGING: Battery discharging mode
 */

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
    
    // Suppress compiler warnings for unused variables in simulation
    (void)charge_status;
    (void)bms_faults;
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
    
    // Suppress compiler warning for unused variable in simulation
    (void)bms_faults;
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