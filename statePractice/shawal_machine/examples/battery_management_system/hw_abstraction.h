#ifndef HW_ABSTRACTION_H
#define HW_ABSTRACTION_H

// --- MICROCONTROLLER PIN & PERIPHERAL SIMULATION ---
// This section simulates the hardware interactions based on your pin layout.
// In a real project, these would be replaced by actual hardware abstraction layer (HAL) functions.

// Define constants for digital output pin states
#define HIGH 1
#define LOW 0

// Simulated pin states (these would be actual GPIO registers in real hardware)
extern int pin_ENABLE_LED_BUCK;
extern int pin_ENABLE_BAT_OUTPUT;
extern int pin_ENABLE_CURRENT_SENSE;
extern int pin_MPPT_BQ_QON;
extern int pin_MPPT_BQ_CHARGE_ENABLE;
extern int pin_LED_DISABLE;
extern int pin_ENABLE_BUCK_BOOST_100W;
extern int pin_ENABLE_USB_100W;
extern int pin_ENABLE_USB_60W;
extern int pin_ENABLE_12V_RS485;
extern int pin_ENABLE_DIAG_12V_RS485;
extern int pin_RS485_RECEIVER_ENABLE;
extern int pin_CAN_ENABLE;

// Simulated ADC readings (0-4095)
extern int adc_usb_c_voltage;
extern int adc_solar_voltage;
extern int adc_battery_ntc;
extern int adc_sys_current;

// Hardware abstraction functions
void set_gpio(const char* pin_name, int state);
int read_gpio(const char* pin_name);
int read_adc(const char* channel_name);
void i2c_write(const char* device_name, int data);
int i2c_read(const char* device_name);

// Initialize hardware simulation
void hw_init(void);

#endif // HW_ABSTRACTION_H