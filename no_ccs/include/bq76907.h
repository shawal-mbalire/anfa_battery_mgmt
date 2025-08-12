#ifndef BQ76907_H
#define BQ76907_H

#include <stdint.h>

/**
 * @brief Enters CONFIG_UPDATE mode and configures hardware protection thresholds.
 */
void bq76907_init_protection_settings(void);

/**
 * @brief Reads all cell voltages using direct I2C commands.
 * @param cell_voltages Pointer to an array to store the cell voltages in mV.
 */
void bq76907_read_cell_voltages(uint16_t *cell_voltages);

/**
 * @brief Reads the FAULT and ALERT status registers.
 */
void bq76907_read_status_registers(void);

/**
 * @brief Puts the BQ76907 into a low-power sleep state.
 */
void bq76907_enable_sleep_mode(void);

/**
 * @brief Enables cell balancing for specified cells.
 * @param balance_mask A bitmask indicating which cells to balance.
 */
void bq76907_set_cell_balancing(uint8_t balance_mask);

#endif // BQ76907_H
