#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/**
 * @brief Initializes the I2C peripheral.
 * Note: The BQ7690x supports optional I2C CRC, which can be enabled in its configuration.
 */
void i2c_init(void);

/**
 * @brief Reads data from an I2C device.
 * @param device_addr The 7-bit I2C address of the device.
 * @param reg_addr The register address to read from.
 * @param data Pointer to a buffer to store the read data.
 * @param len Number of bytes to read.
 * @return 0 on success, non-zero on failure.
 */
uint8_t i2c_read(uint8_t device_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
 * @brief Writes data to an I2C device.
 * @param device_addr The 7-bit I2C address of the device.
 * @param reg_addr The register address to write to.
 * @param data Pointer to the data to write.
 * @param len Number of bytes to write.
 * @return 0 on success, non-zero on failure.
 */
uint8_t i2c_write(uint8_t device_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);

#endif // I2C_H
