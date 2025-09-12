# Battery Management System Example

This example demonstrates how to use the state machine library to implement a comprehensive battery management system for an embedded device. The system manages charging, discharging, LED control, fault handling, and power management states.

## Overview

This battery management system is designed for a device that can:
- Charge from USB-C (60W/100W) or Solar power sources
- Control LED lighting with PWM brightness control
- Monitor battery health and temperature via BMS (Battery Management System)
- Handle various fault conditions safely
- Enter deep sleep mode for power conservation
- Provide RS485 communication for external control

## State Machine Architecture

The system implements 6 main states:

### 1. IDLE_STATE (Default/Safe State)
- **Purpose**: Default state upon system boot or after disconnecting power sources
- **Actions**: 
  - Disables all non-essential power outputs
  - Keeps battery output enabled for MCU operation
  - Continuously polls for power sources and user interactions
- **Transitions**: 
  - → CHARGING_STATE (when power source detected)
  - → LED_CONTROL_STATE (when light switch toggled)
  - → FAULT_STATE (on system faults)
  - → DEEP_SLEEP_STATE (on sleep command)

### 2. CHARGING_STATE
- **Purpose**: Manages battery charging from USB-C or Solar sources
- **Actions**:
  - Enables MPPT charger (BQ25798)
  - Configures appropriate power path (60W/100W USB or Solar)
  - Monitors battery temperature and BMS status via I2C
- **Transitions**:
  - → IDLE_STATE (when charging complete or power disconnected)
  - → FAULT_STATE (on charging faults, BMS interrupts, or temperature issues)

### 3. DISCHARGING_STATE
- **Purpose**: Active when battery is the sole power source
- **Actions**:
  - Monitors system current consumption
  - Watches for battery cell under/over voltage conditions
  - Tracks BMS fault conditions
- **Transitions**:
  - → CHARGING_STATE (when external power connected)
  - → LED_CONTROL_STATE (when lights are enabled)
  - → FAULT_STATE (on battery faults or overcurrent)

### 4. LED_CONTROL_STATE
- **Purpose**: Controls LED lighting and 12V outputs
- **Actions**:
  - Enables LED buck converter and 12V RS485 outputs
  - Controls LED brightness via PWM channels
  - Monitors LED voltages and 12V rail faults
- **Transitions**:
  - → IDLE_STATE (when lights are turned off)
  - → FAULT_STATE (on LED or 12V rail faults)

### 5. FAULT_STATE (Safety State)
- **Purpose**: Critical safety state for system protection
- **Actions**:
  - Immediately disables ALL power outputs
  - Logs fault information
  - Maintains safe state until manual intervention
- **Transitions**:
  - Stays locked until system reset (in real implementation)

### 6. DEEP_SLEEP_STATE
- **Purpose**: Low-power state for extended inactivity
- **Actions**:
  - Shuts down all peripherals
  - Configures wake-up interrupts
  - Puts MCU in low-power mode
- **Transitions**:
  - → IDLE_STATE (on wake-up interrupt)

## Hardware Interfaces Simulated

### Digital Outputs (GPIO)
- `ENABLE_LED_BUCK (PB13)`: LED buck converter enable
- `ENABLE_BAT_OUTPUT (PE3)`: Battery hot-swap controller
- `MPPT_BQ_QON (PE0)`: MPPT charger power control
- `MPPT_BQ_CHARGE_ENABLE (PE10)`: Charger enable (active low)
- `ENABLE_USB_60W/100W (PD9/PD5)`: USB power path selection
- `ENABLE_12V_RS485 (PD7)`: 12V rail for RS485 communication
- `CAN_ENABLE (PD14)`: CAN transceiver enable

### Analog Inputs (ADC)
- `ADC_Con_1`: USB-C voltage monitoring
- `ADC_Solar_In`: Solar panel voltage
- `NTC_2_PACK (PA4)`: Battery temperature sensor
- `I_SYS_OUT`: System current consumption
- `ADC_LED_VOLTAGE_x (PA0-PA3)`: LED string voltage monitoring

### Digital Communication (I2C)
- `BQ25798`: MPPT battery charger IC
- `BQ76907`: Battery Management System (BMS) IC

### PWM Outputs
- `PWM_LED_CURRENT_1/2/3/4 (PC6, PB3, PA15, PB7)`: LED brightness control

## Event-Driven Architecture

The system uses an interrupt-driven event model where:
- **Hardware interrupts** generate critical events (BMS faults, power detection)
- **Main loop polling** generates routine events (temperature monitoring, user input)
- **Event queue** ensures no events are lost and provides non-blocking operation

### Key Events
- `POWER_SOURCE_CONNECTED/DISCONNECTED`: External power detection
- `SOLAR_POWER_CONNECTED`: Solar panel detection
- `LIGHT_SWITCH_TOGGLED`: User lighting control
- `BMS_INTERRUPT`: Critical battery fault from BMS IC
- `BATTERY_TEMP_FAULT`: Temperature protection
- `CHARGE_COMPLETE`: Charging cycle finished
- `SYSTEM_FAULT`: Generic system error
- `GO_TO_SLEEP/WAKE_UP`: Power management

## Safety Features

1. **Immediate Fault Response**: Critical faults (BMS interrupts, temperature) immediately transition to FAULT_STATE
2. **All-Stop Safety**: FAULT_STATE disables ALL power outputs to prevent damage
3. **Temperature Monitoring**: Continuous battery temperature monitoring during charging
4. **Overcurrent Protection**: System current monitoring with automatic shutdown
5. **Voltage Protection**: BMS monitoring for cell under/over voltage conditions

## Building and Running

```bash
# From the examples/battery_management_system directory
make clean && make run
```

The example includes a simulation that demonstrates:
1. System startup in IDLE state
2. USB-C power connection and charging
3. Charge completion
4. LED control activation
5. Solar power transition
6. Fault condition handling
7. System recovery

## Adapting for Real Hardware

To use this example with real hardware:

1. **Replace GPIO simulation** in `hw_abstraction.c` with actual HAL calls
2. **Implement I2C communication** with real BQ25798 and BQ76907 drivers
3. **Add ADC reading functions** for voltage and current monitoring
4. **Configure interrupt handlers** for BMS and power detection
5. **Implement PWM control** for LED brightness
6. **Add sleep mode functionality** for deep sleep state

## Pin Mapping Reference

The example is designed for an STM32-based system with the following pin assignments:
- Power control pins on Port D and E
- LED control on Port B
- ADC inputs on Port A
- PWM outputs distributed across multiple ports
- I2C communication on I2C1/I2C2 peripherals

This provides a complete foundation for implementing a robust, safety-critical battery management system using the state machine library.