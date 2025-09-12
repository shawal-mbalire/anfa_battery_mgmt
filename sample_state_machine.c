/**
 * @file main.c
 * @brief Table-driven state machine for power management system.
 *
 * This file implements a robust, event-driven state machine to manage the
 * complex charging and fault logic of the system. The architecture is
 * based on a table-driven design for clarity, low overhead, and deterministic
 * behavior.
 *
 * A key principle of this design is the separation of concerns:
 * - The state machine handles high-level system behavior and transitions.
 * - Peripheral drivers (for ADC, PWM, GPIO, etc.) handle low-level hardware
 * control.
 * - Action functions act as a bridge, called by the state machine to invoke
 * the appropriate driver functions.
 */

#include "peripheral_drivers.h" // Assume this header exists with your drivers
#include <stdbool.h>
#include <stdio.h> // For logging, replace with your system's logging mechanism

// ============================================================================
// 1. STATE AND EVENT ENUMERATIONS
// ============================================================================

/**
 * @enum SystemState
 * @brief Enumeration of all possible high-level system states.
 * @note This enum directly maps to the states in the UML Statechart.
 */
typedef enum {
  STATE_IDLE,          // System is inactive, in low-power mode
  STATE_CHARGING_60W,  // Substate of CHARGING
  STATE_CHARGING_100W, // Substate of CHARGING
  STATE_CHARGING_MPPT, // Substate of CHARGING
  STATE_DISCHARGING,   // System is operating from the battery
  STATE_FAULT,         // A critical fault has occurred, requiring shutdown
  STATE_STANDBY,       // A waiting or intermediate state
  STATE_HIERARCHICAL_CHARGING // A pseudo-state to handle hierarchical
                              // transitions
} SystemState;

/**
 * @enum SystemEvent
 * @brief Enumeration of all events that can trigger a state transition.
 * @note These events are logical signals from your drivers.
 */
typedef enum {
  EVENT_NONE,              // No event has occurred
  EVENT_60W_CONNECTED,     // USB-C 60W power detected (pin PD11)
  EVENT_100W_CONNECTED,    // USB-C 100W power detected (pin PD8)
  EVENT_MPPT_CONNECTED,    // MPPT (solar) input power detected
  EVENT_BQ_CHARGING_DONE,  // BQ25798 signals charging is complete
  EVENT_BQ_CHARGING_ERROR, // BQ25798 signals a charging error
  EVENT_BMS_FAULT,         // BQ76907 signals a critical fault (pin PE7)
  EVENT_POWER_GOOD_VBAT,   // PE8 signals V_BAT_OUT is ready
  EVENT_ADC_TEMP_HIGH,     // ADC reading from NTC exceeds threshold
  EVENT_ADC_CURRENT_HIGH,  // ADC reading from current sense exceeds threshold
  EVENT_TIMEOUT,           // A periodic timer has expired
  EVENT_FAULT              // A generic fault event to be handled
} SystemEvent;

/**
 * @enum FaultReason
 * @brief Enumeration of specific reasons for a fault event.
 */
typedef enum {
  FAULT_REASON_NONE,
  FAULT_REASON_BMS,
  FAULT_REASON_BQ_ERROR,
  FAULT_REASON_ADC_TEMP,
  FAULT_REASON_ADC_CURRENT,
  FAULT_REASON_PERIPHERAL_BUS
} FaultReason;

/**
 * @enum FaultLevel
 * @brief Defines the severity of a fault.
 */
typedef enum { FAULT_LEVEL_NON_FATAL, FAULT_LEVEL_FATAL } FaultLevel;

// Global variable to store the fault reason and severity
static volatile FaultReason current_fault_reason = FAULT_REASON_NONE;
static volatile FaultLevel current_fault_level = FAULT_LEVEL_NON_FATAL;

// ============================================================================
// 2. ACTION FUNCTIONS
// ============================================================================

// These functions encapsulate the specific actions for each state transition.
// They are responsible for calling your dedicated peripheral drivers,
// using the specific pinouts from your spc250 project report.

void handle_enter_idle(void) {
  // Disable all charging outputs to enter a safe, low-power state
  printf("STATE_IDLE: Shutting down all power peripherals.\n");
  gpio_set_state(GPIO_MPPT_BQ_CHARGE_ENABLE, GPIO_STATE_LOW);
  gpio_set_state(GPIO_ENABLE_USB_60W, GPIO_STATE_LOW);
  gpio_set_state(GPIO_ENABLE_USB_100W, GPIO_STATE_LOW);
  // Enter low-power mode and wait for an EXTI wakeup
}

void handle_start_charging_60w(void) {
  // Enable 60W charging and configure peripherals as per spc250.pdf
  printf("STATE_CHARGING_60W: Starting 60W charge (via pin PD11).\n");
  gpio_set_state(GPIO_ENABLE_USB_60W, GPIO_STATE_HIGH); // PD11
  gpio_set_state(GPIO_ENABLE_USB_100W, GPIO_STATE_LOW); // PD8
  gpio_set_state(GPIO_MPPT_BQ_CHARGE_ENABLE, GPIO_STATE_LOW);
  // Configure PWMs for LED indicators (PB7, PB3 from spc250.pdf)
  pwm_set_duty_cycle(PWM_LED_CURRENT_1, 50);
  // Wait for the BQ chip to report status
}

void handle_start_charging_100w(void) {
  // Enable 100W charging and configure peripherals as per spc250.pdf
  printf("STATE_CHARGING_100W: Starting 100W charge (via pin PD8).\n");
  gpio_set_state(GPIO_ENABLE_USB_100W, GPIO_STATE_HIGH); // PD8
  gpio_set_state(GPIO_ENABLE_USB_60W, GPIO_STATE_LOW);   // PD11
  gpio_set_state(GPIO_MPPT_BQ_CHARGE_ENABLE, GPIO_STATE_LOW);
  // Configure PWMs for LED indicators
  pwm_set_duty_cycle(PWM_LED_CURRENT_1, 100);
}

void handle_start_charging_mppt(void) {
  printf("STATE_CHARGING_MPPT: Starting MPPT charge.\n");
  gpio_set_state(GPIO_MPPT_BQ_CHARGE_ENABLE, GPIO_STATE_HIGH);
  gpio_set_state(GPIO_ENABLE_USB_60W, GPIO_STATE_LOW);
  gpio_set_state(GPIO_ENABLE_USB_100W, GPIO_STATE_LOW);
}

void handle_start_discharging(void) {
  printf("STATE_DISCHARGING: Enabling battery output (via pin PE3).\n");
  gpio_set_state(GPIO_ENABLE_BAT_OUTPUT, GPIO_STATE_HIGH); // PE3
}

/**
 * @brief The centralized fault handler. It logs the fault and takes action
 * based on severity.
 * @param reason The specific reason for the fault.
 */
void handle_system_fault(FaultReason reason, FaultLevel level) {
  printf("CRITICAL FAULT: Reason %d, Level %d. Shutting down all power.\n",
         reason, level);

  // Disable all power paths immediately to ensure safety
  gpio_set_state(GPIO_ENABLE_BAT_OUTPUT, GPIO_STATE_LOW);
  gpio_set_state(GPIO_MPPT_BQ_QON, GPIO_STATE_LOW);
  gpio_set_state(GPIO_ENABLE_USB_60W, GPIO_STATE_LOW);
  gpio_set_state(GPIO_ENABLE_USB_100W, GPIO_STATE_LOW);
  gpio_set_state(GPIO_MPPT_BQ_CHARGE_ENABLE, GPIO_STATE_LOW);

  // Log the fault and update global state
  current_fault_reason = reason;
  current_fault_level = level;

  // Enter the FAULT state immediately
  current_state = STATE_FAULT;

  // Non-fatal faults could enter a recovery sequence (e.g., reset a peripheral)
  // For this demonstration, both are unrecoverable.
  if (level == FAULT_LEVEL_FATAL) {
    printf("Fatal fault detected. System halt.\n");
    // This is the true unrecoverable state,
    // often waiting for a watchdog timer to reset the device.
    while (1)
      ;
  }
}

// Placeholder for other action functions
void no_action(void) {}

// ============================================================================
// 3. STATE TRANSITION TABLE
// ============================================================================

/**
 * @brief Structure defining a single state transition.
 */
typedef struct {
  SystemState current_state;
  SystemEvent event;
  void (*action)(void); // Pointer to the action function
  SystemState next_state;
} StateTransition_t;

/**
 * @brief The immutable state transition table.
 * @note This table is the single source of truth for all system behavior,
 * directly derived from the UML Statechart.
 */
const StateTransition_t state_transitions[] = {
    //   CURRENT STATE            EVENT                    ACTION NEXT STATE
    // ------------------------------------------------------------------------------------------------
    // Global/Hierarchical Transitions (all states inherit these)
    {STATE_HIERARCHICAL_CHARGING, EVENT_BQ_CHARGING_ERROR, handle_system_fault,
     STATE_FAULT},
    {STATE_HIERARCHICAL_CHARGING, EVENT_ADC_TEMP_HIGH, handle_system_fault,
     STATE_FAULT},
    {STATE_IDLE, EVENT_BMS_FAULT, handle_system_fault, STATE_FAULT},
    {STATE_CHARGING_60W, EVENT_BMS_FAULT, handle_system_fault, STATE_FAULT},
    {STATE_CHARGING_100W, EVENT_BMS_FAULT, handle_system_fault, STATE_FAULT},
    {STATE_CHARGING_MPPT, EVENT_BMS_FAULT, handle_system_fault, STATE_FAULT},
    {STATE_DISCHARGING, EVENT_BMS_FAULT, handle_system_fault, STATE_FAULT},
    {STATE_STANDBY, EVENT_BMS_FAULT, handle_system_fault, STATE_FAULT},

    // Transitions from IDLE
    {STATE_IDLE, EVENT_60W_CONNECTED, handle_start_charging_60w,
     STATE_CHARGING_60W},
    {STATE_IDLE, EVENT_100W_CONNECTED, handle_start_charging_100w,
     STATE_CHARGING_100W},
    {STATE_IDLE, EVENT_MPPT_CONNECTED, handle_start_charging_mppt,
     STATE_CHARGING_MPPT},
    {STATE_IDLE, EVENT_POWER_GOOD_VBAT, handle_start_discharging,
     STATE_DISCHARGING},

    // Transitions from CHARGING_60W
    {STATE_CHARGING_60W, EVENT_BQ_CHARGING_DONE, handle_enter_idle, STATE_IDLE},
    {STATE_CHARGING_60W, EVENT_100W_CONNECTED, handle_start_charging_100w,
     STATE_CHARGING_100W}, // Change charging source
    {STATE_CHARGING_60W, EVENT_MPPT_CONNECTED, handle_start_charging_mppt,
     STATE_CHARGING_MPPT},

    // Transitions from CHARGING_100W
    {STATE_CHARGING_100W, EVENT_BQ_CHARGING_DONE, handle_enter_idle,
     STATE_IDLE},
    {STATE_CHARGING_100W, EVENT_60W_CONNECTED, handle_start_charging_60w,
     STATE_CHARGING_60W},
    {STATE_CHARGING_100W, EVENT_MPPT_CONNECTED, handle_start_charging_mppt,
     STATE_CHARGING_MPPT},

    // Transitions from CHARGING_MPPT
    {STATE_CHARGING_MPPT, EVENT_BQ_CHARGING_DONE, handle_enter_idle,
     STATE_IDLE},
    {STATE_CHARGING_MPPT, EVENT_60W_CONNECTED, handle_start_charging_60w,
     STATE_CHARGING_60W},
    {STATE_CHARGING_MPPT, EVENT_100W_CONNECTED, handle_start_charging_100w,
     STATE_CHARGING_100W},

    // Transitions from DISCHARGING
    {STATE_DISCHARGING, EVENT_60W_CONNECTED, handle_start_charging_60w,
     STATE_CHARGING_60W},
    {STATE_DISCHARGING, EVENT_100W_CONNECTED, handle_start_charging_100w,
     STATE_CHARGING_100W},
    {STATE_DISCHARGING, EVENT_MPPT_CONNECTED, handle_start_charging_mppt,
     STATE_CHARGING_MPPT},

    // Transitions from STANDBY
    {STATE_STANDBY, EVENT_POWER_GOOD_VBAT, handle_start_discharging,
     STATE_DISCHARGING},
    {STATE_STANDBY, EVENT_60W_CONNECTED, handle_start_charging_60w,
     STATE_CHARGING_60W},
    {STATE_STANDBY, EVENT_100W_CONNECTED, handle_start_charging_100w,
     STATE_CHARGING_100W},
    {STATE_STANDBY, EVENT_MPPT_CONNECTED, handle_start_charging_mppt,
     STATE_CHARGING_MPPT},

    // Self-transitions and others
    {STATE_FAULT, EVENT_NONE, no_action, STATE_FAULT},
};

const int NUM_TRANSITIONS =
    sizeof(state_transitions) / sizeof(StateTransition_t);

// ============================================================================
// 4. THE DISPATCHER AND MAIN LOOP
// ============================================================================

static SystemState current_state = STATE_IDLE;

// Helper to determine if a state is a child of STATE_HIERARCHICAL_CHARGING
static bool is_charging_state(SystemState state) {
  return state == STATE_CHARGING_60W || state == STATE_CHARGING_100W ||
         state == STATE_CHARGING_MPPT;
}

/**
 * @brief Dispatches an event to the state machine, handling hierarchy.
 * @param event The event to be processed.
 */
void dispatch_event(SystemEvent event) {
  // A separate check for fatal faults is faster and more reliable
  if (current_state == STATE_FAULT) {
    return;
  }

  // First, check for an explicit transition from the current state
  for (int i = 0; i < NUM_TRANSITIONS; ++i) {
    if (state_transitions[i].current_state == current_state &&
        state_transitions[i].event == event) {

      // Log the state transition for debugging and verification
      printf("Transition: %d -> %d on event %d\n", current_state,
             state_transitions[i].next_state, event);

      if (state_transitions[i].action != NULL) {
        state_transitions[i].action();
      }
      current_state = state_transitions[i].next_state;
      return;
    }
  }

  // Second, if no specific transition is found, check for a hierarchical
  // transition
  if (is_charging_state(current_state)) {
    for (int i = 0; i < NUM_TRANSITIONS; ++i) {
      if (state_transitions[i].current_state == STATE_HIERARCHICAL_CHARGING &&
          state_transitions[i].event == event) {

        // Log the hierarchical transition
        printf("Hierarchical Transition: %d -> %d on event %d\n", current_state,
               state_transitions[i].next_state, event);

        if (state_transitions[i].action != NULL) {
          state_transitions[i].action();
        }
        current_state = state_transitions[i].next_state;
        return;
      }
    }
  }

  // Finally, if no transition is found at any level, log a warning
  printf("WARNING: No valid transition for state %d with event %d.\n",
         current_state, event);
}

/**
 * @brief The main event loop.
 */
int main(void) {
  // System initialization (clocks, peripherals, drivers)
  // ...

  printf("System initialized. Starting in IDLE state.\n");
  current_state = STATE_IDLE;

  while (1) {
    // This is the heart of your bare-metal loop.
    // It should check for events from various sources and dispatch them.

    SystemEvent new_event = EVENT_NONE;

    // Example: Check if a BQ interrupt occurred (assuming ISR pushes to a
    // queue) You would get this from a global queue or a flag set by your EXTI
    // ISR.
    if (get_event_from_queue(&new_event)) {
      dispatch_event(new_event);
    }

    // Example: Periodic ADC polling for non-critical events like temp
    if (is_timer_polling_ready()) {
      if (is_adc_temp_too_high()) {
        // This is a non-fatal fault. We handle it as a specific event.
        dispatch_event(EVENT_ADC_TEMP_HIGH);
      }
    }

    // Example: Check for critical, immediate faults that must be handled
    // For a true real-time system, this check would happen in an ISR.
    if (gpio_get_state(GPIO_BMS_FAULT) == GPIO_STATE_HIGH) {
      handle_system_fault(FAULT_REASON_BMS, FAULT_LEVEL_FATAL);
    }

    // Enter a low-power mode to save energy while waiting for the next event
    enter_low_power_mode();
  }

  return 0;
}
