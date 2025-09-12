#include "hw_abstraction.h"
#include <stdio.h>
#include <string.h>

// Simulated pin states
int pin_ENABLE_LED_BUCK = LOW;
int pin_ENABLE_BAT_OUTPUT = LOW;
int pin_ENABLE_CURRENT_SENSE = LOW;
int pin_MPPT_BQ_QON = LOW;
int pin_MPPT_BQ_CHARGE_ENABLE = HIGH; // Active Low
int pin_LED_DISABLE = LOW;
int pin_ENABLE_BUCK_BOOST_100W = LOW;
int pin_ENABLE_USB_100W = LOW;
int pin_ENABLE_USB_60W = LOW;
int pin_ENABLE_12V_RS485 = LOW;
int pin_ENABLE_DIAG_12V_RS485 = LOW;
int pin_RS485_RECEIVER_ENABLE = LOW; // Active Low
int pin_CAN_ENABLE = LOW;

// Simulated ADC readings (0-4095)
int adc_usb_c_voltage = 0;
int adc_solar_voltage = 0;
int adc_battery_ntc = 0;
int adc_sys_current = 0;

// Simulates setting a digital output pin state
void set_gpio(const char* pin_name, int state) {
    printf("GPIO: Setting %s to %s\n", pin_name, state == HIGH ? "HIGH" : "LOW");
    // In real hardware, this would write to the actual GPIO register
    // Example: HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, state);
}

// Simulates reading a digital input pin state
int read_gpio(const char* pin_name) {
    // In real hardware, this would read from the actual GPIO register
    // Example: return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10);
    return LOW; // Returning a dummy value for the simulation
}

// Simulates reading an ADC channel
int read_adc(const char* channel_name) {
    // In real hardware, this would trigger ADC conversion and read the result
    // Example: HAL_ADC_Start(&hadc1); HAL_ADC_PollForConversion(&hadc1, 100); return HAL_ADC_GetValue(&hadc1);
    
    // For simulation, return varying values to demonstrate state transitions
    static int counter = 0;
    counter++;
    
    if (strcmp(channel_name, "ADC_Con_1") == 0) {
        // Simulate USB-C voltage detection after some time
        return (counter > 50 && counter < 150) ? 150 : 0;
    } else if (strcmp(channel_name, "ADC_Solar_In") == 0) {
        // Simulate solar voltage detection
        return (counter > 350 && counter < 450) ? 75 : 0;
    } else if (strcmp(channel_name, "NTC_2_PACK") == 0) {
        // Simulate normal battery temperature
        return 2048; // Mid-range ADC value
    } else if (strcmp(channel_name, "I_SYS_OUT") == 0) {
        // Simulate normal system current
        return 500; // Normal current level
    }
    
    return 0; // Default return
}

// Simulates writing to an I2C device
void i2c_write(const char* device_name, int data) {
    printf("I2C: Writing %d to %s\n", data, device_name);
    // In real hardware: HAL_I2C_Mem_Write(&hi2c1, device_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
}

// Simulates reading from an I2C device
int i2c_read(const char* device_name) {
    printf("I2C: Reading from %s\n", device_name);
    // In real hardware: HAL_I2C_Mem_Read(&hi2c1, device_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    return 0; // Returning a dummy value for normal operation
}

// Initialize hardware simulation
void hw_init(void) {
    printf("Hardware simulation initialized\n");
    // In real hardware, this would initialize GPIO, ADC, I2C, PWM, etc.
    // Example:
    // HAL_GPIO_Init();
    // HAL_ADC_Init(&hadc1);
    // HAL_I2C_Init(&hi2c1);
    // HAL_TIM_PWM_Init(&htim1);
}