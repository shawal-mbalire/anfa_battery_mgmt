#include <stdio.h>
#include <stdbool.h>
#include "../../lib/state_machine/state_machine.h"
#include "../../lib/state_machine/logger.h"
#include "bms_states.h"
#include "bms_events.h"
#include "hw_abstraction.h"

/*
 * Battery Management System using State Machine Library
 *
 * This example demonstrates a comprehensive battery management system built using
 * the reusable state machine library. The system manages:
 * - Charging from USB-C (60W/100W) or Solar sources
 * - LED lighting control with fault detection
 * - Battery monitoring and safety
 * - Power management and sleep modes
 * - Comprehensive fault handling
 *
 * The state machine ensures safe operation and immediate response to fault conditions.
 */

// External state machine instance from library
extern StateMachine g_stateMachine;

// Process BMS-specific events using the library's state machine
void ProcessBmsEvent(Event event) {
    if (g_stateMachine.current_state != NULL && g_stateMachine.current_state->transition_signal != NULL) {
        g_stateMachine.current_state->transition_signal(event);
    }
}

// Main system loop
void SystemMainLoop(void) {
    Event event;
    while(1) {
        // Execute current state's main action
        if (g_stateMachine.current_state && g_stateMachine.current_state->main_action) {
            g_stateMachine.current_state->main_action();
        }
        
        // Process any pending events
        event = GetBmsEvent();
        if (event != EVENT_MAX) {
            ProcessBmsEvent(event);
        }
        
        // In a real system, add small delay or sleep for power saving
        // _delay_ms(10);
    }
}

// Simulation function to demonstrate system behavior
void RunSimulation(void) {
    static int simulation_step = 0;
    simulation_step++;
    
    switch (simulation_step) {
        case 5:
            printf("\n--- Simulation: USB-C power source connected ---\n");
            PostBmsEvent(POWER_SOURCE_CONNECTED);
            break;
        case 15:
            printf("\n--- Simulation: Charging complete ---\n");
            PostBmsEvent(CHARGE_COMPLETE);
            break;
        case 25:
            printf("\n--- Simulation: Light switch toggled ON ---\n");
            PostBmsEvent(LIGHT_SWITCH_TOGGLED);
            break;
        case 35:
            printf("\n--- Simulation: Solar power connected ---\n");
            PostBmsEvent(SOLAR_POWER_CONNECTED);
            break;
        case 45:
            printf("\n--- Simulation: Light switch toggled OFF ---\n");
            PostBmsEvent(LIGHT_SWITCH_TOGGLED);
            break;
        case 55:
            printf("\n--- Simulation: Battery temperature fault ---\n");
            PostBmsEvent(BATTERY_TEMP_FAULT);
            break;
        case 65:
            printf("\n--- Simulation: System reset (back to IDLE) ---\n");
            ChangeState(&BMS_IDLE_STATE);
            break;
        case 75:
            printf("\n--- Simulation: Going to deep sleep ---\n");
            PostBmsEvent(GO_TO_SLEEP);
            break;
        case 85:
            printf("\n--- Simulation: Wake up from sleep ---\n");
            PostBmsEvent(WAKE_UP);
            break;
        case 95:
            printf("\n--- Simulation complete ---\n");
            simulation_step = 0; // Reset for continuous demo
            break;
    }
}

int main(void) {
    printf("=== Battery Management System Initialization ===\n");
    
    // Initialize hardware abstraction layer
    hw_init();
    
    // Initialize the state machine to IDLE state
    g_stateMachine.current_state = &BMS_IDLE_STATE;
    
    non_blocking_log("System initialized. Starting in BMS_IDLE_STATE...\n");
    
    // Execute initial entry action
    if (g_stateMachine.current_state->entry_action) {
        g_stateMachine.current_state->entry_action();
    }
    
    printf("\n=== Starting Battery Management System ===\n");
    
    // Main system loop with simulation
    int loop_counter = 0;
    while(1) {
        // Execute current state's main action
        if (g_stateMachine.current_state && g_stateMachine.current_state->main_action) {
            g_stateMachine.current_state->main_action();
        }
        
        // Process any pending events
        Event event = GetBmsEvent();
        if (event != EVENT_MAX) {
            ProcessBmsEvent(event);
        }
        
        // Run simulation events
        if (loop_counter % 10 == 0) {  // Slow down simulation
            RunSimulation();
        }
        
        // Simulate processing delay
        for (volatile long i = 0; i < 100000; i++);
        
        loop_counter++;
        
        // Exit after demonstration
        if (loop_counter > 1000) {
            break;
        }
    }
    
    printf("\n=== Battery Management System Demo Complete ===\n");
    return 0;
}