# State Machine Library for Embedded Systems

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()

A robust, event-driven state machine library in C designed for embedded systems. This library provides a clean, modular approach to implementing state machines without the complexity of large switch statements or blocking operations.

## 🚀 Quick Start

```bash
# Clone the repository
git clone <repository-url>
cd shawal_machine

# Build and run the simple example
cd examples/simple_state_machine
make run

# Build and run the battery management system example  
cd ../battery_management_system
make run
```

## 📋 Table of Contents

- [Features](#features)
- [Examples](#examples)
- [Installation](#installation)
- [Usage](#usage)
- [API Reference](#api-reference)
- [Architecture](#architecture)
- [Contributing](#contributing)
- [License](#license)

## ✨ Features

- **Event-Driven Architecture**: Reactive system design that responds to events rather than polling
- **Non-Blocking Operations**: FIFO event queue prevents blocking in interrupt contexts
- **Memory Efficient**: Zero dynamic allocation, `const` state definitions stored in Flash/ROM
- **Modular Design**: Clean separation of state logic using function pointers
- **Entry/Main/Exit Actions**: Well-defined state lifecycle management
- **Real-World Examples**: Complete battery management system implementation
- **Embedded-Friendly**: Designed for resource-constrained microcontrollers

## 🎯 Examples

### Simple State Machine
A minimal two-state example demonstrating basic concepts:

```c
// IDLE state ⟷ ACTIVE state
// Events: EVENT_BUTTON_PRESS, EVENT_TIMEOUT
```

**Use Case**: Perfect for learning the library fundamentals.

### Battery Management System (BMS)
A comprehensive real-world implementation featuring:

- **6 States**: Idle, Charging, Discharging, LED Control, Fault, Deep Sleep
- **Hardware Abstraction**: I2C, GPIO, ADC interfaces
- **Fault Handling**: Temperature monitoring, communication faults
- **Power Management**: Deep sleep mode with wake-up capability
- **Multiple Power Sources**: USB-C (60W/100W) and Solar charging

**Use Case**: Production-ready embedded system demonstrating advanced patterns.

## 🛠 Installation

### Prerequisites

- GCC compiler (version 7.0+)
- Make build system
- Git

### Build Instructions

```bash
# Build specific example
cd examples/simple_state_machine
make

# Clean build artifacts
make clean

# Build and run
make run
```

## 📚 Usage

### Basic State Machine Implementation

1. **Define your states and events**:

```c
#include "state_machine.h"

// State function declarations
void my_idle_entry(void);
void my_idle_main(void);
void my_idle_exit(void);
void my_idle_transition(Event event);

// State definition
const StateActions MY_IDLE_STATE = {
    .entry_action = my_idle_entry,
    .main_action = my_idle_main,
    .exit_action = my_idle_exit,
    .transition_signal = my_idle_transition
};
```

2. **Implement state functions**:

```c
void my_idle_entry(void) {
    printf("Entering IDLE state\n");
    // Initialize hardware, start timers, etc.
}

void my_idle_main(void) {
    // Continuous actions while in this state
    // e.g., blink LED, read sensors
}

void my_idle_exit(void) {
    printf("Exiting IDLE state\n");
    // Cleanup, stop timers, etc.
}

void my_idle_transition(Event event) {
    switch (event) {
        case EVENT_BUTTON_PRESS:
            ChangeState(&MY_ACTIVE_STATE);
            break;
        // Handle other events...
    }
}
```

3. **Initialize and run**:

```c
int main(void) {
    // Initialize state machine
    g_stateMachine.current_state = &MY_IDLE_STATE;
    
    // Execute entry action
    if (g_stateMachine.current_state->entry_action) {
        g_stateMachine.current_state->entry_action();
    }
    
    // Main loop
    while(1) {
        // Execute current state's main action
        if (g_stateMachine.current_state->main_action) {
            g_stateMachine.current_state->main_action();
        }
        
        // Process events
        Event event = PopEvent();
        if (event != EVENT_MAX) {
            if (g_stateMachine.current_state->transition_signal) {
                g_stateMachine.current_state->transition_signal(event);
            }
        }
    }
    
    return 0;
}
```

## 📖 API Reference

### Core Functions

| Function | Description |
|----------|-------------|
| `ChangeState(StateHandler new_state)` | Transition to a new state with proper entry/exit handling |
| `PushEvent(Event event)` | Add event to the queue (interrupt-safe) |
| `PopEvent()` | Retrieve and remove event from queue |
| `non_blocking_log(const char* message)` | Non-blocking logging utility |

### State Structure

```c
typedef struct StateActions_s {
    void (*entry_action)(void);        // Called once when entering state
    void (*main_action)(void);         // Called repeatedly while in state  
    void (*exit_action)(void);         // Called once when exiting state
    void (*transition_signal)(Event event); // Handles state transitions
} StateActions;
```

### Predefined Events

```c
typedef enum {
    EVENT_BUTTON_PRESS,    // Button press detected
    EVENT_TIMEOUT,         // Timer expired
    EVENT_DOOR_OPEN,       // Door sensor triggered
    EVENT_QUEUE_FULL,      // Event queue overflow
    EVENT_MAX              // Sentinel value (no event)
} Event;
```

## 🏗 Architecture

### Event-Driven Design

The library uses an event-driven architecture where:

1. **Events** are generated by interrupts, timers, or external signals
2. **Event Queue** stores events in a non-blocking FIFO buffer
3. **State Machine** processes events and triggers state transitions
4. **States** encapsulate specific system behaviors

### Memory Layout

```
Flash/ROM:              RAM:
┌─────────────────┐    ┌─────────────────┐
│ State Definitions│    │ Event Queue     │
│ (const data)    │    │ State Machine   │
│ Program Code    │    │ Variables       │
└─────────────────┘    └─────────────────┘
```

### Key Benefits

- **Modularity**: Each state is self-contained
- **Maintainability**: Easy to add/modify states
- **Efficiency**: Function pointer calls, no large switch statements
- **Safety**: Non-blocking operations, predictable memory usage
- **Scalability**: Easily extensible for complex systems

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

### Quick Contribution Steps

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Make your changes and test thoroughly
4. Commit: `git commit -m "Add amazing feature"`
5. Push: `git push origin feature/amazing-feature`
6. Create a Pull Request

### Areas for Contribution

- **New Examples**: Industry-specific implementations
- **Platform Support**: Additional microcontroller support
- **Testing**: Unit tests and integration tests
- **Documentation**: Tutorials, API docs, diagrams
- **Optimization**: Performance improvements, memory reduction

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🔗 Related Projects

- [TinyStateMachine](https://github.com/example/tiny-state-machine) - Minimal state machine for 8-bit MCUs
- [EmbeddedSM](https://github.com/example/embedded-sm) - State machine with RTOS integration

## 📞 Support

- **Issues**: [GitHub Issues](../../issues)
- **Discussions**: [GitHub Discussions](../../discussions)
- **Documentation**: Check example READMEs and code comments

---

⭐ **If this library helped you, please give it a star!** ⭐
