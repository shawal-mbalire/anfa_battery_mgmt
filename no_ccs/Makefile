# Makefile for BMS Firmware

# Compiler and flags
CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -g -O2 -ffunction-sections -fdata-sections -Wall -std=c11 -Iinclude
LDFLAGS = -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -Wl,--gc-sections -T mspm0g3507.ld -Wl,--defsym=_HEAP_SIZE=0x400 -Wl,--defsym=_STACK_SIZE=0x400

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

debug:
	openocd -f interface/xds110.cfg -f board/ti_mspm0_launchpad.cfg
