#ifndef BQ25798_H
#define BQ25798_H

#include <stdint.h>

/**
 * @brief Initializes the BQ25798 with a default charging profile.
 * This can be customized based on the PROG pin resistance.
 */
void bq25798_init_charge_profile(void);

/**
 * @brief Reads the charger status registers to get the current state.
 * This includes charging status, faults, and input source information.
 */
void bq25798_read_status_registers(void);

/**
 * @brief Enables the charging process.
 * Requires the CE pin to be low and EN_CHG bit to be set.
 */
void bq25798_start_charging(void);

/**
 * @brief Sets the input current limit.
 * @param current_ma The input current limit in mA.
 */
void bq25798_set_input_current_limit(uint16_t current_ma);

/**
 * @brief Enables or disables the Maximum Power Point Tracking (MPPT) feature for solar panels.
 * @param enable 1 to enable, 0 to disable.
 */
void bq25798_set_mppt_enabled(uint8_t enable);


#endif // BQ25798_H
