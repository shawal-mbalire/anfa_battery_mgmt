#include "bq25798.h"
#include "i2c.h"

// BQ25798 I2C address
#define BQ25798_I2C_ADDR 0x6B

// Register Addresses
#define REG_CHARGE_VOLTAGE_LIMIT 0x01
#define REG_CHARGE_CURRENT_LIMIT 0x03
#define REG_INPUT_VOLTAGE_LIMIT 0x05
#define REG_INPUT_CURRENT_LIMIT 0x06
#define REG_CHARGER_CONTROL_0 0x0F
#define REG_CHARGER_STATUS_0 0x1B

void bq25798_init_charge_profile(void) {
    // Set charging voltage and current limits based on battery configuration.
    // These are typically set based on the number of cells and their chemistry.
    // Example: Set 4.2V charge voltage for a single cell Li-ion battery.
    uint8_t voltage_data[] = {0x58, 0x10}; // 4.2V (LSB first)
    i2c_write(BQ25798_I2C_ADDR, REG_CHARGE_VOLTAGE_LIMIT, voltage_data, 2);
}

void bq25798_read_status_registers(void) {
    // Read charger status registers to get information about charging state,
    // faults, and input source.
    uint8_t status_data[1];
    i2c_read(BQ25798_I2C_ADDR, REG_CHARGER_STATUS_0, status_data, 1);
}

void bq25798_start_charging(void) {
    // Enable charging by setting the EN_CHG bit in the Charger Control 0 register.
    // The CE pin must also be low.
    uint8_t control_data[1];
    i2c_read(BQ25798_I2C_ADDR, REG_CHARGER_CONTROL_0, control_data, 1);
    control_data[0] |= (1 << 5); // Set EN_CHG bit
    i2c_write(BQ25798_I2C_ADDR, REG_CHARGER_CONTROL_0, control_data, 1);
}

void bq25798_set_input_current_limit(uint16_t current_ma) {
    // Set the input current limit in mA.
    uint8_t current_data[] = {current_ma & 0xFF, current_ma >> 8};
    i2c_write(BQ25798_I2C_ADDR, REG_INPUT_CURRENT_LIMIT, current_data, 2);
}

void bq25798_set_mppt_enabled(uint8_t enable) {
    // Enable or disable the MPPT feature for solar panels.
    // This is controlled by the EN_MPPT bit in the MPPT Control register (0x15).
}
