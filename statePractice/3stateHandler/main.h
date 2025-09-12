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

// State actions
typedef enum { ENTRY, MAIN, EXIT, TRANSITION } StateAction;

// State Machine - simplified. includes method pointers so it behaves like an object.
typedef struct StateMachine StateMachine;
typedef void (*SM_Init)(StateMachine* sm);
typedef void (*SM_Dispatch)(StateMachine* sm, StateAction action);
typedef void (*SM_SetState)(StateMachine* sm, State* new_state);

struct StateMachine {
    State* current_state;    // Pointer to current active state
    /* methods */
    SM_Init init;
    SM_Dispatch dispatch;
    SM_SetState set_state;
};

// Global states instance
extern States all_states;

// Implementation functions (bound into the StateMachine instance)
void state_machine_init_impl(StateMachine* sm);
void state_machine_dispatch_impl(StateMachine* sm, StateAction action);
void state_machine_set_state_impl(StateMachine* sm, State* new_state);

// Convenience constructor: binds methods into the struct and initializes it
void state_machine_new(StateMachine* sm);
