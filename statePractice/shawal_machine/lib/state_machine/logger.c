#include "logger.h"
#include <stdio.h>

#define LOG_BUFFER_SIZE 256
volatile char log_buffer[LOG_BUFFER_SIZE];
volatile int log_head = 0;
volatile int log_tail = 0;

void non_blocking_log(const char *format, ...) {
    char temp_buffer[128];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(temp_buffer, sizeof(temp_buffer), format, args);
    va_end(args);

    if (len < 0) return; // Error occurred

    // For simplicity, this example prints directly.
    // In a real embedded system, you would write to a circular buffer
    // and have an interrupt handle the UART transmission.
    printf("%s", temp_buffer);
}
