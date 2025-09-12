/* 
    * main.c
    *Optimal state machine implementation.
    *Does not use state table.
    *Reusable event processor.
    *Has entry and exit actions.
*/
#include "main.h"

// Optimization: Macro to generate state functions (reduces repetition)
#define DEFINE_STATE_FUNCTIONS(state_name, state_upper) \
    void state_name##_entry(){printf(state_upper " Entry\n");} \
    void state_name##_main(){printf(state_upper " Main\n");} \
    void state_name##_exit(){printf(state_upper " Exit\n");} \
    void state_name##_trans(){printf(state_upper " Trans\n");}

// Generate all state functions with one line each
DEFINE_STATE_FUNCTIONS(wait, "WAIT")
DEFINE_STATE_FUNCTIONS(blink, "BLINK")
DEFINE_STATE_FUNCTIONS(pause, "PAUSE")
DEFINE_STATE_FUNCTIONS(boom, "BOOM")

// Optimization: Macro for state initialization
#define INIT_STATE(name) {name##_entry, name##_main, name##_exit, name##_trans}

States all_states = {
    .WAIT_BUTTON = INIT_STATE(wait),
    .BLINK_LED = INIT_STATE(blink),
    .PAUSE_LED = INIT_STATE(pause),
    .BOOM = INIT_STATE(boom)
};

// State Machine core methods - single dispatch that switches among actions
// Implementation variants that will be bound to the StateMachine instance
void state_machine_init_impl(StateMachine* sm) {
    sm->current_state = &all_states.WAIT_BUTTON;  // Start with WAIT state
    printf("State Machine initialized to WAIT state\n");
}

void state_machine_set_state_impl(StateMachine* sm, State* new_state) {
    sm->current_state = new_state;
}

void state_machine_dispatch_impl(StateMachine* sm, StateAction action) {
    if (!sm->current_state) return;
    switch(action) {
        case ENTRY:
            if (sm->current_state->Entry) sm->current_state->Entry();
            break;
        case MAIN:
            if (sm->current_state->Main) sm->current_state->Main();
            break;
        case EXIT:
            if (sm->current_state->Exit) sm->current_state->Exit();
            break;
        case TRANSITION:
            if (sm->current_state->Transition) sm->current_state->Transition();
            break;
        default:
            break;
    }
}

// Bind the impls into the object and call init
void state_machine_new(StateMachine* sm) {
    sm->init = state_machine_init_impl;
    sm->dispatch = state_machine_dispatch_impl;
    sm->set_state = state_machine_set_state_impl;
    // call init method
    if (sm->init) sm->init(sm);
}

int main(void) {
    StateMachine sm = {0};
    state_machine_new(&sm); // binds methods and initializes

    printf("\n=== WAIT State ===\n");
    sm.dispatch(&sm, ENTRY);
    sm.dispatch(&sm, MAIN);
    sm.dispatch(&sm, TRANSITION);

    printf("\n=== BLINK State ===\n");
    sm.set_state(&sm, &all_states.BLINK_LED);
    sm.dispatch(&sm, ENTRY);
    sm.dispatch(&sm, MAIN);
    sm.dispatch(&sm, EXIT);

    printf("\n=== PAUSE State ===\n");
    sm.set_state(&sm, &all_states.PAUSE_LED);
    sm.dispatch(&sm, ENTRY);
    sm.dispatch(&sm, MAIN);

    printf("\n=== BOOM State ===\n");
    sm.set_state(&sm, &all_states.BOOM);
    sm.dispatch(&sm, ENTRY);
    sm.dispatch(&sm, MAIN);

    return 0;
}