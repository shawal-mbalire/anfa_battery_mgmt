#ifndef BMS_STATES_H
#define BMS_STATES_H

#include "../../lib/state_machine/state_machine.h"
#include "bms_events.h"

// BMS State declarations
extern const StateActions BMS_IDLE_STATE;
extern const StateActions BMS_CHARGING_STATE;
extern const StateActions BMS_DISCHARGING_STATE;
extern const StateActions BMS_LED_CONTROL_STATE;
extern const StateActions BMS_FAULT_STATE;
extern const StateActions BMS_DEEP_SLEEP_STATE;

// Helper function to handle unhandled events
void HandleUnhandledEvent(Event event, const char* state_name);

#endif // BMS_STATES_H