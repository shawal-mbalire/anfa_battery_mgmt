#ifndef STATES_H
#define STATES_H

#include "../../lib/state_machine/state_machine.h"

// State function declarations
void simple_idle_entry(void);
void simple_idle_main(void);
void simple_idle_exit(void);
void simple_idle_transition(Event event);

void simple_active_entry(void);
void simple_active_main(void);
void simple_active_exit(void);
void simple_active_transition(Event event);

// State definitions
extern const StateActions SIMPLE_IDLE_STATE;
extern const StateActions SIMPLE_ACTIVE_STATE;

#endif // STATES_H