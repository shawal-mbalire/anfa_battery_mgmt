#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef enum {
    TASK_READ_BMS,
    TASK_CHECK_CHARGER,
    // ... other tasks
    NUM_TASKS
} task_id_t;

void scheduler_init(void);
void scheduler_run(void);
uint8_t is_timer_task_due(task_id_t task_id);
void set_task_due(task_id_t task_id);


#endif // SCHEDULER_H
