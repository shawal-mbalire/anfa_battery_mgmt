# Simple State Machine Example

This example demonstrates the basic usage of the state machine library with a minimal implementation showing fundamental concepts.

## Overview

This example creates a simple state machine with two states:
- **IDLE**: The initial state, waiting for button presses
- **ACTIVE**: An active state that responds to timeout events

The example demonstrates:
- Basic state transitions
- Event handling
- Entry and exit actions
- State machine initialization and execution

## States

### IDLE State
- **Entry Action**: Logs "Entering IDLE state"
- **Main Action**: Continuously logs "In IDLE state"
- **Transitions**: 
  - `EVENT_BUTTON_PRESS` → ACTIVE state

### ACTIVE State  
- **Entry Action**: Logs "Entering ACTIVE state" 
- **Main Action**: Continuously logs "In ACTIVE state"
- **Transitions**:
  - `EVENT_TIMEOUT` → IDLE state

## Events

- `EVENT_BUTTON_PRESS`: Simulates a button press to activate the system
- `EVENT_TIMEOUT`: Simulates a timeout to return to idle

## Building and Running

```bash
make            # Build the example
make run        # Build and run the example
make clean      # Clean up build artifacts
```

## Expected Output

```
Initial run...
Entering IDLE state
In IDLE state
Event received: EVENT_BUTTON_PRESS
Exiting IDLE state
Entering ACTIVE state  
In ACTIVE state
Event received: EVENT_TIMEOUT
Exiting ACTIVE state
Entering IDLE state
Example complete.
```

## Learning Objectives

This example teaches:
1. How to define states with entry/exit actions
2. How to implement state transitions
3. How to use the event queue system
4. Basic state machine initialization and execution
5. Proper project structure for state machine applications

This serves as a foundation for more complex state machines like the battery management system example.