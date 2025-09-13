# State Machine Library - Main Makefile
# This Makefile provides convenient targets for building all examples

.PHONY: all clean help examples simple bms

# Default target
all: examples

# Build all examples
examples: simple bms

# Build simple state machine example
simple:
	@echo "Building simple state machine example..."
	$(MAKE) -C examples/simple_state_machine

# Build battery management system example  
bms:
	@echo "Building battery management system example..."
	$(MAKE) -C examples/battery_management_system

# Run simple example
run-simple: simple
	@echo "Running simple state machine example..."
	$(MAKE) -C examples/simple_state_machine run

# Run BMS example
run-bms: bms
	@echo "Running battery management system example..."
	$(MAKE) -C examples/battery_management_system run

# Clean all examples
clean:
	@echo "Cleaning all examples..."
	$(MAKE) -C examples/simple_state_machine clean
	$(MAKE) -C examples/battery_management_system clean

# Help target
help:
	@echo "State Machine Library - Available targets:"
	@echo ""
	@echo "  all           - Build all examples (default)"
	@echo "  examples      - Build all examples"  
	@echo "  simple        - Build simple state machine example"
	@echo "  bms           - Build battery management system example"
	@echo "  run-simple    - Build and run simple example"
	@echo "  run-bms       - Build and run BMS example"
	@echo "  clean         - Clean all build artifacts"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  make simple       # Build simple example"
	@echo "  make run-bms      # Build and run BMS example"
	@echo "  make clean        # Clean everything"
