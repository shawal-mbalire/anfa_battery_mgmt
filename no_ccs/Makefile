# Makefile for BMS Firmware

# Compiler and flags
CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m4 -mthumb -Wall -O2
LDFLAGS = -nostdlib -T linker.ld

# Source files
SRCS =  src/main.c \
        src/state_machine.c \
        drivers/i2c.c \
        drivers/bq76907.c \
        drivers/bq25798.c \
        core/scheduler.c \
        core/system_init.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = bms_firmware

all: $(TARGET).elf

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET).elf
