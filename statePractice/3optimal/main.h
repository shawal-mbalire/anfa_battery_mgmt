#include <stdio.h>
// State function signature
typedef void (*StateFunc)();
typedef struct {
    StateFunc Entry;
    StateFunc Main;
    StateFunc Exit;
    StateFunc Transition;
} State;
// available states
typedef struct {
    State WAIT_BUTTON;
    State BLINK_LED;
    State PAUSE_LED;
    State BOOM;
} States;

// State Machine
typedef struct {
    States* states;          // Pointer to all available states
    State* current_state;    // Pointer to current active state
    int current_state_id;    // 0=WAIT, 1=BLINK, 2=PAUSE, 3=BOOM
} StateMachine;

// Global states instance
extern States all_states;

// State Machine methods
void state_machine_init(StateMachine* sm);
void state_machine_dispatch_entry(StateMachine* sm);
void state_machine_dispatch_main(StateMachine* sm);
void state_machine_dispatch_exit(StateMachine* sm);
void state_machine_dispatch_transition(StateMachine* sm);
void state_machine_set_state(StateMachine* sm, int state_id);
