CC = gcc
CFLAGS = -Wall -g -I../../lib/state_machine

# Library sources
LIB_SOURCES = ../../lib/state_machine/state_machine.c \
              ../../lib/state_machine/event_queue.c \
              ../../lib/state_machine/logger.c \
              ../../lib/state_machine/interrupt.c

# Application sources
APP_SOURCES = main.c \
              states.c

# All sources
SOURCES = $(LIB_SOURCES) $(APP_SOURCES)

# Corresponding object files
OBJECTS = $(SOURCES:.c=.o)

# The name of the executable
EXECUTABLE = simple_state_machine

.PHONY: all clean run

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS)

# This rule handles object files for both lib and local directories
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

run: all
	./$(EXECUTABLE)

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build the simple state machine example"
	@echo "  clean    - Remove object files and executable"
	@echo "  run      - Build and run the example"
	@echo "  help     - Show this help message"