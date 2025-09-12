#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "event_queue.h"

// State actions and transition signal
typedef struct StateActions_s {
    void (*entry_action)(void);
    void (*main_action)(void);
    void (*exit_action)(void);
    void (*transition_signal)(Event event);
} StateActions;

// A pointer to the current set of actions
typedef const StateActions* StateHandler;

// Context and state machine instance
typedef struct {
    StateHandler current_state;
    // Add other context data here
} StateMachine;

// The state change function that handles entry and exit actions
void ChangeState(StateHandler new_state);

// State definitions
extern const StateActions IDLE_STATE;
extern const StateActions ACTIVE_STATE;
extern const StateActions ERROR_STATE;

#endif // STATE_MACHINE_H
