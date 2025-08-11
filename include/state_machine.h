#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

typedef enum {
    STATE_IDLE,
    STATE_CHARGING,
    STATE_DISCHARGING,
    STATE_FAULT,
    STATE_SHUTDOWN
} system_state_t;

void update_system_state_machine(void);
system_state_t get_system_state(void);

#endif // STATE_MACHINE_H
