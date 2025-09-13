# Contributing to State Machine Library

Thank you for your interest in contributing to the State Machine Library! This document will help you get started with contributions.

## Table of Contents

- [Getting Started](#getting-started)
- [Development Environment](#development-environment)
- [Project Structure](#project-structure)
- [Coding Standards](#coding-standards)
- [How to Contribute](#how-to-contribute)
- [Examples](#examples)
- [Testing](#testing)
- [Documentation](#documentation)
- [Pull Request Process](#pull-request-process)

## Getting Started

### Prerequisites

- GCC compiler (version 7.0 or higher)
- Make build system
- Git for version control

### Quick Setup

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd shawal_machine
   ```

2. Build the examples:
   ```bash
   # Build and run the simple example
   cd examples/simple_state_machine
   make run
   
   # Build and run the battery management system example
   cd ../battery_management_system
   make run
   ```

## Development Environment

### Build System
The project uses Makefiles for building. Each example has its own Makefile that:
- Compiles the state machine library
- Builds the specific example
- Provides clean and run targets

### Directory Structure
```
shawal_machine/
├── lib/state_machine/           # Core state machine library
│   ├── state_machine.h/.c       # Main state machine implementation
│   ├── event_queue.h/.c         # Event queue management
│   ├── logger.h/.c              # Logging utilities
│   └── interrupt.h/.c           # Interrupt handling
├── examples/
│   ├── simple_state_machine/    # Basic example
│   └── battery_management_system/ # Complex real-world example
├── README.md                    # Project overview
└── CONTRIBUTING.md              # This file
```

## Project Structure

### Core Library (`lib/state_machine/`)

The state machine library consists of:

- **state_machine.h/.c**: Core state machine implementation with state transitions
- **event_queue.h/.c**: FIFO event queue for managing state machine events
- **logger.h/.c**: Non-blocking logging utilities
- **interrupt.h/.c**: Interrupt handling support

### Examples

#### Simple State Machine (`examples/simple_state_machine/`)
A minimal example demonstrating:
- Basic two-state system (IDLE ↔ ACTIVE)
- Event handling (button press, timeout)
- State transitions with entry/exit actions

#### Battery Management System (`examples/battery_management_system/`)
A comprehensive example showing:
- Multi-state system (Idle, Charging, Discharging, LED Control, Fault, Deep Sleep)
- Hardware abstraction layer
- Complex event handling and fault management
- Real-world embedded system patterns

## Coding Standards

### C Style Guidelines

1. **Naming Conventions**:
   - Functions: `snake_case`
   - Variables: `snake_case`
   - Constants/Macros: `UPPER_CASE`
   - Types: `PascalCase` or `snake_case_t`

2. **File Organization**:
   - Header guards for all `.h` files
   - Include system headers before local headers
   - Group related functions together

3. **Documentation**:
   - Comment all public functions
   - Explain complex algorithms
   - Use clear, descriptive variable names

### Example Code Structure

```c
#include <stdio.h>
#include "state_machine.h"

/*
 * Function description
 * @param param1 Description of parameter
 * @return Description of return value
 */
void example_function(int param1) {
    // Implementation
}
```

## How to Contribute

### Types of Contributions

1. **Bug Fixes**: Fix issues in existing code
2. **New Features**: Add functionality to the core library
3. **Examples**: Create new example applications
4. **Documentation**: Improve README, comments, or guides
5. **Testing**: Add test cases or improve build system

### Getting Started with Contributions

1. **Fork the repository** on your Git platform
2. **Create a feature branch** from `main`:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make your changes** following the coding standards
4. **Test your changes** thoroughly
5. **Commit your changes** with clear messages:
   ```bash
   git commit -m "Add feature: description of what you added"
   ```
6. **Push to your fork** and **create a pull request**

## Examples

### Creating a New Example

To create a new example:

1. Create a new directory under `examples/`:
   ```bash
   mkdir examples/your_example_name
   ```

2. Create the necessary files:
   - `README.md` - Describe your example
   - `Makefile` - Build configuration
   - `main.c` - Main application code
   - Additional source files as needed

3. Use this Makefile template:
   ```makefile
   CC = gcc
   CFLAGS = -Wall -g -I../../lib/state_machine
   
   LIB_SOURCES = ../../lib/state_machine/state_machine.c \
                 ../../lib/state_machine/event_queue.c \
                 ../../lib/state_machine/logger.c \
                 ../../lib/state_machine/interrupt.c
   
   APP_SOURCES = main.c  # Add your source files here
   
   SOURCES = $(LIB_SOURCES) $(APP_SOURCES)
   OBJECTS = $(SOURCES:.c=.o)
   EXECUTABLE = your_example_name
   
   .PHONY: all clean run
   
   all: $(EXECUTABLE)
   
   $(EXECUTABLE): $(OBJECTS)
   	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS)
   
   %.o: %.c
   	$(CC) $(CFLAGS) -c $< -o $@
   
   clean:
   	rm -f $(OBJECTS) $(EXECUTABLE)
   
   run: all
   	./$(EXECUTABLE)
   ```

### Example Template

Your `main.c` should follow this pattern:

```c
#include <stdio.h>
#include "../../lib/state_machine/state_machine.h"
#include "../../lib/state_machine/event_queue.h"
#include "../../lib/state_machine/logger.h"

// External state machine instance
extern StateMachine g_stateMachine;

// Your state definitions
// (see existing examples for reference)

int main(void) {
    printf("=== Your Example Name ===\n");
    
    // Initialize state machine
    g_stateMachine.current_state = &YOUR_INITIAL_STATE;
    
    // Execute entry action
    if (g_stateMachine.current_state->entry_action) {
        g_stateMachine.current_state->entry_action();
    }
    
    // Your main loop here
    
    return 0;
}
```

## Testing

### Building and Testing

Always test your changes:

1. **Build all examples**:
   ```bash
   cd examples/simple_state_machine && make clean && make
   cd ../battery_management_system && make clean && make
   ```

2. **Run examples** to ensure they work:
   ```bash
   make run
   ```

3. **Check for warnings** during compilation
4. **Test edge cases** in your implementations

### Adding Tests

When adding new functionality:
- Create examples that demonstrate the feature
- Test error conditions
- Verify memory usage is reasonable
- Ensure no memory leaks (if dynamic allocation is used)

## Documentation

### Code Documentation

- Document all public functions in header files
- Use clear, descriptive comments
- Explain the purpose and usage of each module

### README Updates

When adding features:
- Update the main README.md if needed
- Create/update example README files
- Document any new build requirements

## Pull Request Process

### Before Submitting

1. **Test thoroughly** - ensure all examples build and run
2. **Follow coding standards** - consistent style with existing code
3. **Update documentation** - add/update relevant documentation
4. **Clean commits** - clear commit messages and logical changes

### Pull Request Template

When creating a pull request, include:

```
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Example addition

## Testing
- [ ] Built and tested locally
- [ ] All examples compile
- [ ] No new warnings

## Additional Notes
Any additional information about the changes
```

### Review Process

1. **Automated checks** - ensure code compiles
2. **Code review** - maintainer reviews for quality and consistency
3. **Testing** - verify examples work as expected
4. **Documentation** - check that documentation is updated

## Questions and Support

- **Issues**: Use the issue tracker for bug reports and feature requests
- **Discussions**: Use discussions for questions about usage
- **Documentation**: Check existing examples and README files

Thank you for contributing to the State Machine Library!