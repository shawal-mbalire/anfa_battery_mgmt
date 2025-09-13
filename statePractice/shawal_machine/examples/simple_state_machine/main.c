#include <stdio.h>
#include <stdbool.h>
#include "states.h"
#include "../../lib/state_machine/state_machine.h"
#include "../../lib/state_machine/event_queue.h"
#include "../../lib/state_machine/logger.h"

/*
 * Simple State Machine Example
 *
 * This example demonstrates the basic usage of the state machine library
 * with a minimal implementation showing fundamental concepts:
 * - Two states: IDLE and ACTIVE
 * - Basic event handling with EVENT_BUTTON_PRESS and EVENT_TIMEOUT
 * - State transitions with entry/exit actions
 * - Event queue management
 */

// External state machine instance from library
extern StateMachine g_stateMachine;

// Process events using the library's state machine
void ProcessEvent(Event event) {
    if (g_stateMachine.current_state != NULL && g_stateMachine.current_state->transition_signal != NULL) {
        g_stateMachine.current_state->transition_signal(event);
    }
}

// Main system loop
void SystemMainLoop(void) {
    Event event;
    int loop_count = 0;
    
    while(loop_count < 500000) {  // Run for limited time for demonstration
        // Execute current state's main action
        if (g_stateMachine.current_state && g_stateMachine.current_state->main_action) {
            g_stateMachine.current_state->main_action();
        }
        
        // Process any pending events
        event = PopEvent();
        if (event != EVENT_MAX) {
            ProcessEvent(event);
        }
        
        loop_count++;
        
        // Simulate some events during the loop
        if (loop_count == 100000) {
            printf("\n--- Simulating button press ---\n");
            PushEvent(EVENT_BUTTON_PRESS);
        }
        else if (loop_count == 300000) {
            printf("\n--- Simulating timeout ---\n");
            PushEvent(EVENT_TIMEOUT);
        }
    }
}

int main(void) {
    printf("=== Simple State Machine Example ===\n");
    printf("This example demonstrates basic state machine concepts:\n");
    printf("- IDLE state: Waits for button press events\n");
    printf("- ACTIVE state: Waits for timeout events\n");
    printf("- Automatic transitions between states\n\n");
    
    // Initialize the state machine to IDLE state
    g_stateMachine.current_state = &SIMPLE_IDLE_STATE;
    
    non_blocking_log("System initialized. Starting in SIMPLE_IDLE_STATE...\n");
    
    // Execute initial entry action
    if (g_stateMachine.current_state->entry_action) {
        g_stateMachine.current_state->entry_action();
    }
    
    printf("\n=== Starting State Machine Loop ===\n");
    
    // Run the main system loop
    SystemMainLoop();
    
    printf("\n=== Simple State Machine Demo Complete ===\n");
    printf("Final state: %s\n", (g_stateMachine.current_state == &SIMPLE_IDLE_STATE) ? "SIMPLE_IDLE_STATE" : "SIMPLE_ACTIVE_STATE");
    
    return 0;
}