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

// State Machine methods - directly use State function pointers
void state_machine_init(StateMachine* sm) {
    sm->states = &all_states;
    sm->current_state_id = 0;  // Start with WAIT state
    sm->current_state = &all_states.WAIT_BUTTON;
    printf("State Machine initialized to WAIT state\n");
}

void state_machine_set_state(StateMachine* sm, int state_id) {
    sm->current_state_id = state_id;
    switch(state_id) {
        case 0: sm->current_state = &all_states.WAIT_BUTTON; break;
        case 1: sm->current_state = &all_states.BLINK_LED; break;
        case 2: sm->current_state = &all_states.PAUSE_LED; break;
        case 3: sm->current_state = &all_states.BOOM; break;
    }
}

// Direct dispatch methods - no enum needed!
void state_machine_dispatch_entry(StateMachine* sm) {
    if(sm->current_state && sm->current_state->Entry)
        sm->current_state->Entry();
}

void state_machine_dispatch_main(StateMachine* sm) {
    if(sm->current_state && sm->current_state->Main)
        sm->current_state->Main();
}

void state_machine_dispatch_exit(StateMachine* sm) {
    if(sm->current_state && sm->current_state->Exit)
        sm->current_state->Exit();
}

void state_machine_dispatch_transition(StateMachine* sm) {
    if(sm->current_state && sm->current_state->Transition)
        sm->current_state->Transition();
}

int main() {
    StateMachine sm;
    
    // Initialize state machine
    state_machine_init(&sm);
    
    printf("\n=== WAIT State (ID: 0) ===\n");
    state_machine_dispatch_entry(&sm);
    state_machine_dispatch_main(&sm);
    state_machine_dispatch_transition(&sm);
    
    printf("\n=== BLINK State (ID: 1) ===\n");
    state_machine_set_state(&sm, 1);
    state_machine_dispatch_entry(&sm);
    state_machine_dispatch_main(&sm);
    state_machine_dispatch_exit(&sm);
    
    printf("\n=== PAUSE State (ID: 2) ===\n");
    state_machine_set_state(&sm, 2);
    state_machine_dispatch_entry(&sm);
    state_machine_dispatch_main(&sm);
    
    printf("\n=== BOOM State (ID: 3) ===\n");
    state_machine_set_state(&sm, 3);
    state_machine_dispatch_entry(&sm);
    state_machine_dispatch_main(&sm);
    
    return 0;
}