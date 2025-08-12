#include "state_machine.h"
#include "bq25798.h" // Needed for charging functions

static system_state_t current_state = STATE_IDLE;

// Event flags that can be set by other parts of the system (e.g., drivers)
static uint8_t charger_connected_event = 0;
static uint8_t fault_detected_event = 0;

// Functions to signal events to the state machine
void state_machine_signal_charger_connected(void) {
    charger_connected_event = 1;
}

void state_machine_signal_fault(void) {
    fault_detected_event = 1;
}


void update_system_state_machine(void) {
    // The state machine checks for events and transitions between states.
    switch (current_state) {
        case STATE_IDLE:
            if (fault_detected_event) {
                current_state = STATE_FAULT;
                fault_detected_event = 0; // Clear event
            } else if (charger_connected_event) {
                current_state = STATE_CHARGING;
                bq25798_start_charging(); // Action: Start the charger
                charger_connected_event = 0; // Clear event
            }
            break;

        case STATE_CHARGING:
            if (fault_detected_event) {
                current_state = STATE_FAULT;
                // Action: Stop the charger
                fault_detected_event = 0; // Clear event
            }
            // Add logic to check for charge completion, etc.
            break;

        case STATE_DISCHARGING:
            // Logic for discharging state
            break;

        case STATE_FAULT:
            // Logic for fault state: stop charging, open FETs, etc.
            break;

        case STATE_SHUTDOWN:
            // Logic for shutdown state
            break;
    }
}

system_state_t get_system_state(void) {
    return current_state;
}
