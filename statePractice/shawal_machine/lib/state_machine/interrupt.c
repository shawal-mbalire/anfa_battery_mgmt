#include "interrupt.h"

// --- PLATFORM-SPECIFIC IMPLEMENTATION ---
// The following functions are placeholders. You must provide the real
// implementation for your target hardware. For an ARM Cortex-M processor,
// this might use inline assembly (`__disable_irq()`, `__enable_irq()`)
// or functions from a hardware abstraction layer (HAL).

void disable_interrupts(void) {
    // On a real system, this would disable global interrupts.
    // printf("[interrupt] Disabling interrupts\n");
}

void enable_interrupts(void) {
    // On a real system, this would re-enable global interrupts.
    // printf("[interrupt] Enabling interrupts\n");
}
