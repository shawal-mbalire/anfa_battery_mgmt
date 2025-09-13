#include "states.h"
#include "../../lib/state_machine/state_machine.h"
#include "../../lib/state_machine/logger.h"
#include <stdio.h>

/*
 * SIMPLE_IDLE State Implementation
 */
void simple_idle_entry(void) {
    non_blocking_log("Entering SIMPLE_IDLE state\n");
}

void simple_idle_main(void) {
    static int counter = 0;
    counter++;
    if (counter % 100000 == 0) {  // Reduce log frequency
        non_blocking_log("In SIMPLE_IDLE state - waiting for events...\n");
    }
}

void simple_idle_exit(void) {
    non_blocking_log("Exiting SIMPLE_IDLE state\n");
}

void simple_idle_transition(Event event) {
    switch (event) {
        case EVENT_BUTTON_PRESS:
            printf("Event received: EVENT_BUTTON_PRESS\n");
            ChangeState(&SIMPLE_ACTIVE_STATE);
            break;
        default:
            printf("Unhandled event %d in SIMPLE_IDLE state\n", event);
            break;
    }
}

/*
 * SIMPLE_ACTIVE State Implementation  
 */
void simple_active_entry(void) {
    non_blocking_log("Entering SIMPLE_ACTIVE state\n");
}

void simple_active_main(void) {
    static int counter = 0;
    counter++;
    if (counter % 100000 == 0) {  // Reduce log frequency
        non_blocking_log("In SIMPLE_ACTIVE state - system is active...\n");
    }
}

void simple_active_exit(void) {
    non_blocking_log("Exiting SIMPLE_ACTIVE state\n");
}

void simple_active_transition(Event event) {
    switch (event) {
        case EVENT_TIMEOUT:
            printf("Event received: EVENT_TIMEOUT\n");
            ChangeState(&SIMPLE_IDLE_STATE);
            break;
        default:
            printf("Unhandled event %d in SIMPLE_ACTIVE state\n", event);
            break;
    }
}

/*
 * State Definitions
 */
const StateActions SIMPLE_IDLE_STATE = {
    .entry_action = simple_idle_entry,
    .main_action = simple_idle_main,
    .exit_action = simple_idle_exit,
    .transition_signal = simple_idle_transition
};

const StateActions SIMPLE_ACTIVE_STATE = {
    .entry_action = simple_active_entry,
    .main_action = simple_active_main,
    .exit_action = simple_active_exit,
    .transition_signal = simple_active_transition
};