#include "bq76907.h"
#include "i2c.h"

// BQ76907 I2C address
#define BQ76907_I2C_ADDR 0x08

// Direct Commands - for reading telemetry data
#define CMD_CELL1_VOLTAGE 0x14
#define CMD_STATUS 0x00

// Subcommands - for control and configuration
#define SUBCMD_FET_ENABLE 0x0022
#define SUBCMD_RESET 0x0012
#define SUBCMD_CB_ACTIVE_CELLS 0x0083
#define SUBCMD_SET_CFGUPDATE 0x0090
#define SUBCMD_EXIT_CFGUPDATE 0x0092

// RAM Registers - for detailed configuration
#define RAM_ENABLED_PROTECTIONS_A 0x9024
#define RAM_VCELL_MODE 0x901B

/**
 * @brief Enters CONFIG_UPDATE mode to allow RAM writes.
 * This is required before writing to most RAM registers.
 */
static void enter_config_update(void) {
    uint8_t cmd[] = {SUBCMD_SET_CFGUPDATE & 0xFF, SUBCMD_SET_CFGUPDATE >> 8};
    // Write subcommand to 0x3E
    i2c_write(BQ76907_I2C_ADDR, 0x3E, cmd, 2);
}

/**
 * @brief Exits CONFIG_UPDATE mode to protect RAM from accidental writes.
 */
static void exit_config_update(void) {
    uint8_t cmd[] = {SUBCMD_EXIT_CFGUPDATE & 0xFF, SUBCMD_EXIT_CFGUPDATE >> 8};
    // Write subcommand to 0x3E
    i2c_write(BQ76907_I2C_ADDR, 0x3E, cmd, 2);
}

void bq76907_init_protection_settings(void) {
    enter_config_update();
    // Example: Enable CUV (undervoltage) protection by writing to RAM
    // This requires writing the register address and data to 0x3E,
    // then writing the checksum and length to 0x60.
    uint8_t data[] = {RAM_ENABLED_PROTECTIONS_A & 0xFF, RAM_ENABLED_PROTECTIONS_A >> 8, 0xE1};
    i2c_write(BQ76907_I2C_ADDR, 0x3E, data, 3);
    // Checksum and length calculation would be done here and written to 0x60
    exit_config_update();
}

void bq76907_read_cell_voltages(uint16_t *cell_voltages) {
    // Reads cell voltages using direct commands (starting from Cell 1)
    for (int i = 0; i < 4; i++) {
        uint8_t cmd = CMD_CELL1_VOLTAGE + (i * 2);
        uint8_t data[2];
        i2c_read(BQ76907_I2C_ADDR, cmd, data, 2);
        // Data is in little-endian format
        cell_voltages[i] = (data[1] << 8) | data[0];
    }
}

void bq76907_read_status_registers(void) {
    // Read FAULT and ALERT registers using a direct command
    uint8_t status_data[2];
    i2c_read(BQ76907_I2C_ADDR, CMD_STATUS, status_data, 2);
}

void bq76907_enable_sleep_mode(void) {
    // Subcommand to put the IC into a low-power state
    // The specific sleep mode is configured in RAM
}

void bq76907_set_cell_balancing(uint8_t balance_mask) {
    // Write to CB_ACTIVE_CELLS subcommand to start balancing
    // This also requires a checksum and length write to 0x60
    uint8_t cmd[] = {SUBCMD_CB_ACTIVE_CELLS & 0xFF, SUBCMD_CB_ACTIVE_CELLS >> 8, balance_mask};
    i2c_write(BQ76907_I2C_ADDR, 0x3E, cmd, 3);
    // Checksum and length calculation would be done here and written to 0x60
}
