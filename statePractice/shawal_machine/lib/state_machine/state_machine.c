#include "state_machine.h"
#include "logger.h"
#include <stddef.h>

// Global state machine instance
StateMachine g_stateMachine;

// The state change function that handles entry and exit actions
void ChangeState(StateHandler new_state) {
    if (g_stateMachine.current_state && g_stateMachine.current_state->exit_action != NULL) {
        g_stateMachine.current_state->exit_action();
    }
    g_stateMachine.current_state = new_state;
    if (g_stateMachine.current_state && g_stateMachine.current_state->entry_action != NULL) {
        g_stateMachine.current_state->entry_action();
    }
}

// Forward declarations for all state actions
void idle_entry(void);
void idle_main(void);
void idle_exit(void);
void idle_transition(Event event);

void active_entry(void);
void active_main(void);
void active_exit(void);
void active_transition(Event event);

void error_entry(void);
void error_main(void);

// Define the state instances
const StateActions IDLE_STATE = {
    .entry_action = idle_entry,
    .main_action = idle_main,
    .exit_action = idle_exit,
    .transition_signal = idle_transition
};

const StateActions ACTIVE_STATE = {
    .entry_action = active_entry,
    .main_action = active_main,
    .exit_action = active_exit,
    .transition_signal = active_transition
};

const StateActions ERROR_STATE = {
    .entry_action = error_entry,
    .main_action = error_main,
    .exit_action = NULL, // No exit action
    .transition_signal = NULL // Error state does not transition
};

// IDLE State Logic
void idle_entry(void) {
    non_blocking_log("Entering IDLE State\n");
}
void idle_main(void) {
    // non_blocking_log("Executing IDLE main loop...\n");
}
void idle_exit(void) {
    non_blocking_log("Exiting IDLE State\n");
}
void idle_transition(Event event) {
    switch (event) {
        case EVENT_BUTTON_PRESS:
            non_blocking_log("IDLE: Button press -> ACTIVE\n");
            ChangeState(&ACTIVE_STATE);
            break;
        case EVENT_QUEUE_FULL:
            non_blocking_log("IDLE: Event queue full -> ERROR\n");
            ChangeState(&ERROR_STATE);
            break;
        default:
            // Ignore other events
            break;
    }
}

// ACTIVE State Logic
void active_entry(void) {
    non_blocking_log("Entering ACTIVE State\n");
}
void active_main(void) {
    // non_blocking_log("Executing ACTIVE main loop...\n");
}
void active_exit(void) {
    non_blocking_log("Exiting ACTIVE State\n");
}
void active_transition(Event event) {
    switch (event) {
        case EVENT_TIMEOUT:
            non_blocking_log("ACTIVE: Timeout -> IDLE\n");
            ChangeState(&IDLE_STATE);
            break;
        case EVENT_QUEUE_FULL:
            non_blocking_log("ACTIVE: Event queue full -> ERROR\n");
            ChangeState(&ERROR_STATE);
            break;
        default:
            // Ignore other events
            break;
    }
}

// ERROR State Logic
void error_entry(void) {
    non_blocking_log("--- ENTERING ERROR STATE ---\n");
    // In a real system, you might blink an error LED, log the fault,
    // and then potentially trigger a system reset.
}

void error_main(void) {
    // The system stays here. Only a reset can exit this state.
}
