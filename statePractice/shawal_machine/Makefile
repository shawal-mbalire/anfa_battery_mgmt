CC = gcc
CFLAGS = -Wall -g -Ilib/state_machine

# Application sources
APP_SOURCES = src/main.c

# Library sources
LIB_SOURCES = $(wildcard lib/state_machine/*.c)

# All sources
SOURCES = $(APP_SOURCES) $(LIB_SOURCES)

# Corresponding object files
OBJECTS = $(SOURCES:.c=.o)

# The name of the executable
EXECUTABLE = state_machine_app

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS)

# This rule handles object files for both src and lib directories
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

run: all
	./$(EXECUTABLE)
