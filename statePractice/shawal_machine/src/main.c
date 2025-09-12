#include "../lib/state_machine/state_machine.h"
#include "../lib/state_machine/event_queue.h"
#include "../lib/state_machine/logger.h"
#include <stddef.h>

extern StateMachine g_stateMachine;

void process_event(Event event) {
    if (g_stateMachine.current_state != NULL && g_stateMachine.current_state->transition_signal != NULL) {
        g_stateMachine.current_state->transition_signal(event);
    }
}

void main_loop(void) {
    Event event;
    while(1) {
        if (g_stateMachine.current_state && g_stateMachine.current_state->main_action) {
            g_stateMachine.current_state->main_action();
        }
        
        event = PopEvent();
        if (event != EVENT_MAX) {
            process_event(event);
        }
        
        // Add a small delay or sleep function to save power
        // in a real embedded system (e.g., `_delay_ms(10)`).
    }
}

int main(void) {
    // Initialize the state machine
    g_stateMachine.current_state = &IDLE_STATE;

    non_blocking_log("Initial run...\n");
    // Initial entry
    if (g_stateMachine.current_state->entry_action) {
        g_stateMachine.current_state->entry_action();
    }
    
    // Simulate events
    PushEvent(EVENT_BUTTON_PRESS);
    
    // Process the event
    Event event = PopEvent();
    if (event != EVENT_MAX) {
        process_event(event);
    }
    
    PushEvent(EVENT_TIMEOUT);

    // Process the event
    event = PopEvent();
    if (event != EVENT_MAX) {
        process_event(event);
    }
    
    return 0;
}
