#include "main.h"
#include "system_init.h"
#include "scheduler.h"
#include "bq76907.h"
#include "bq25798.h"
#include "state_machine.h"

int main() {
    system_init();
    scheduler_init();

    while(1) {
        // The scheduler runs tasks at regular intervals
        scheduler_run();

        // The state machine is updated continuously
        update_system_state_machine();
    }
    return 0;
}

void read_bms_status_task(void) {
    // This task would be called by the scheduler periodically.
    // It reads the status from the BMS IC and can signal events.
    // For example, if a fault is detected:
    // if (bq76907_has_fault()) {
    //     state_machine_signal_fault();
    // }
}

void check_charger_state_task(void) {
    // This task would also be called by the scheduler.
    // It could check if a charger is connected and signal the event.
    // if (is_charger_plugged_in()) {
    //     state_machine_signal_charger_connected();
    // }
}
