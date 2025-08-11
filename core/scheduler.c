#include "scheduler.h"

static uint32_t tick_counter = 0;
static uint8_t task_due_flags[NUM_TASKS] = {0};

// Function prototypes for tasks
void read_bms_status_task(void);
void check_charger_state_task(void);


void scheduler_init(void) {
    // Initialize timer (e.g., SysTick)
}

// This would be called by the timer ISR
void scheduler_update(void) {
    tick_counter++;
    if ((tick_counter % 10) == 0) { // 100ms task
        set_task_due(TASK_READ_BMS);
    }
    if ((tick_counter % 50) == 0) { // 500ms task
        set_task_due(TASK_CHECK_CHARGER);
    }
}


void scheduler_run(void) {
    if (is_timer_task_due(TASK_READ_BMS)) {
        read_bms_status_task();
    }
    if (is_timer_task_due(TASK_CHECK_CHARGER)) {
        check_charger_state_task();
    }
    // ... other tasks
}

uint8_t is_timer_task_due(task_id_t task_id) {
    if (task_due_flags[task_id]) {
        task_due_flags[task_id] = 0;
        return 1;
    }
    return 0;
}

void set_task_due(task_id_t task_id) {
    task_due_flags[task_id] = 1;
}
