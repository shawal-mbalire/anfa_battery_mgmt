#include "system_init.h"
#include "i2c.h"
#include "bq76907.h"
#include "bq25798.h"

void system_init(void) {
    // Initialize MCU clock system and GPIOs
    // ...

    i2c_init();
    bq76907_init_protection_settings();
    bq25798_init_charge_profile();
}
