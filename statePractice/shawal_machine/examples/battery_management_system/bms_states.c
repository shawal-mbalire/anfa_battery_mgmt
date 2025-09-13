#include "bms_states.h"
#include <stdio.h>

/*
 * BMS State Definitions and Common Functions
 * 
 * This file contains:
 * - Common utility functions used by all states
 * - State structure definitions
 * - External function declarations for states in other files
 */

// Helper function to handle unhandled events
void HandleUnhandledEvent(Event event, const char* state_name) {
    printf("Unhandled event %d in state: %s\n", event, state_name);
    // In a real system, this would log to non-volatile memory
}

// External function declarations for power states (in bms_power_states.c)
extern void bms_idle_entry(void);
extern void bms_idle_main(void);
extern void bms_idle_exit(void);
extern void bms_idle_transition(Event event);

extern void bms_charging_entry(void);
extern void bms_charging_main(void);
extern void bms_charging_exit(void);
extern void bms_charging_transition(Event event);

extern void bms_discharging_entry(void);
extern void bms_discharging_main(void);
extern void bms_discharging_exit(void);
extern void bms_discharging_transition(Event event);

// External function declarations for control states (in bms_control_states.c)
extern void bms_led_control_entry(void);
extern void bms_led_control_main(void);
extern void bms_led_control_exit(void);
extern void bms_led_control_transition(Event event);

extern void bms_fault_entry(void);
extern void bms_fault_main(void);
extern void bms_fault_exit(void);
extern void bms_fault_transition(Event event);

extern void bms_deep_sleep_entry(void);
extern void bms_deep_sleep_main(void);
extern void bms_deep_sleep_exit(void);
extern void bms_deep_sleep_transition(Event event);

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