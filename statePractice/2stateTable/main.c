/*
    * main.c
    *
    *State machine
    *  Author: Shawal Mbalire
    *  Description: This is the main invocation of the state machine
    
    we have a machine with states and EventSignals
    for each state, there are event trigger signals
    based on the current state and the signal, we transition to a new state
    and perform actions like turning on or off an LED
    these are event driven active objects with queueing of events.
    State table implementation
*/
#include "main.h"


// table function prototypes
void machine_init(Machine *machine);
void machine_button_pressed(Machine *machine);
void machine_blink_timeout(Machine *machine);
void machine_pause_timeout(Machine *machine);
void machine_ignore(Machine *machine);
// state table
StateHandler const state_table[STATES_MAX][SIGNALS_MAX] = {
/**********************INIT*******PRESSES*******RELEASED******TIMEOUT*/
/* WAIT_BUTTON */ { &machine_init  , &machine_button_pressed, &machine_ignore, &machine_ignore },
/* BLINK       */ { &machine_init  , &machine_ignore, &machine_ignore, &machine_blink_timeout },
/* PAUSE       */ { &machine_ignore, &machine_ignore, &machine_ignore, &machine_pause_timeout },
/* BOOM        */ { &machine_ignore, &machine_ignore, &machine_ignore, &machine_ignore }
};
static void dispatch(Machine *machine){
    // Q_ASSERT(machine->state < STATES_MAX)&&(machine->event < SIGNALS_MAX);
    if (machine->state >= STATES_MAX || machine->event >= SIGNALS_MAX) {
        printf("Error: Invalid state or event\n");
        return;
    }
    (*state_table[machine->state][machine->event])(machine);
}

int main(void) {
    Machine machine = {
        .led = { .is_on = 0, .port = "P1" },
        .state = WAIT_BUTTON,
        .event = INIT_SIGNAL
    };

    // Initialize the state machine
    dispatch(&machine);

    // Simulate events in a realistic sequence
    machine.event = BUTTON_PRESSED;
    dispatch(&machine);
    machine.event = TIMER_EXPIRED;
    dispatch(&machine);
    machine.event = TIMER_EXPIRED;
    dispatch(&machine); 
    // try all events
    machine.event = BUTTON_PRESSED;
    dispatch(&machine);
    machine.event = TIMER_EXPIRED;
    dispatch(&machine);
    machine.event = TIMER_EXPIRED;
    dispatch(&machine);

    return 0;
}

// state table functions with state and event handling
void machine_init(Machine *machine) {
    machine->state = WAIT_BUTTON;
    printf("Machine initialized to WAIT_BUTTON state\n");
}
void machine_button_pressed(Machine *machine) {
    if (machine->state == WAIT_BUTTON) {
        machine->state = BLINK_LED;
        machine->led.is_on = 1; // Turn on LED
        printf("Button pressed: Transition to BLINK_LED state, LED ON\n");
    }
}
void machine_blink_timeout(Machine *machine) {
    if (machine->state == BLINK_LED) {
        machine->state = PAUSE_LED;
        machine->led.is_on = 0; // Turn off LED
        printf("Blink timeout: Transition to PAUSE_LED state, LED OFF\n");
    }
}
void machine_pause_timeout(Machine *machine) {
    if (machine->state == PAUSE_LED) {
        machine->state = WAIT_BUTTON;
        printf("Pause timeout: Transition to WAIT_BUTTON state\n");
    }
}
void machine_ignore(Machine *machine) {
    // Ignore the event in the current state
    printf("Ignoring event in state %d\n", machine->state);
}   