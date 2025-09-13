#include "bms_states.h"
#include "hw_abstraction.h"
#include "../../lib/state_machine/logger.h"
#include <stdio.h>

/*
 * BMS Control and Safety States
 * 
 * This file contains the control and safety states:
 * - LED_CONTROL: LED lighting control with fault detection
 * - FAULT: Critical fault handling state
 * - DEEP_SLEEP: Low power sleep mode
 */

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