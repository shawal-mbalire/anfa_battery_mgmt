# Firmware Development Brief for BMS

## 1. Core Components and System Architecture

This document outlines the firmware development for a Battery Management System (BMS) and charging circuit.

- **Core Components:**
  - **BQ76907:** Battery monitor and protector IC.
  - **BQ25798:** Multi-cell battery charge controller.
- **Communication:** Both ICs are connected to a microcontroller unit (MCU) via an I²C bus.

### Architectural Challenge

The primary architectural challenge is the system's reliance on I²C for critical, real-time safety functions. While suitable for configuration and status reporting, I²C poses a significant risk for immediate, deterministic actions like over-current or over-voltage protection.

**Key Insight:** The BQ76907 has integrated hardware protection features that operate independently of the MCU. These should be the primary safety mechanism. The software's role is to manage and monitor, not to serve as the first line of defense.

## 2. Deconstructing the Core Problem

The main objective is to develop robust, reliable, and safe firmware for the BMS and charging system.

### Key Constraints

- **Safety-Critical Operation:** The system must prevent thermal runaway, over-voltage, under-voltage, and over-current conditions.
- **Real-Time Monitoring and Control:** The MCU must periodically read status and adjust charging parameters.
- **Communication Reliability:** All interactions with the ICs occur over the I²C bus, which introduces potential for communication errors, delays, and bus conflicts.
- **Power Efficiency:** The system, especially the BQ76907, needs to be managed for low power consumption.

**Hidden Assumption:** The MCU is not the sole controller of the system's safety. The BQ76907's hardware-based protections are the most reliable and fastest method for handling critical faults.

## 3. Proposed Lean Architecture

A **bare-metal, state-driven model** with a lightweight scheduler is the most robust architecture, not a full-blown RTOS.

- **Safety-First Hardware Abstraction Layer (HAL):** A thin HAL for the BQ76907 and BQ25798 to abstract I²C operations and provide a clean, safe API.
- **Event-Driven State Machine:** A top-level state machine to manage system states (e.g., `IDLE`, `CHARGING`, `DISCHARGING`, `FAULT`, `SHUTDOWN`).
- **Minimalist Cooperative Scheduler:** A simple, timer-driven cooperative scheduler (e.g., using SysTick at 100 Hz).
- **Interrupt-Driven I²C:** Non-blocking, interrupt-driven I²C to prevent the main loop from stalling.

This architecture prioritizes determinism and low overhead, which are crucial for safety-critical systems.

## 4. Quantifiable Metrics for Success

- **System Quiescent Current:** `< 20 µA` in `IDLE` state.
- **I²C Read Latency:** `< 1 ms` for critical read operations.
- **Charging/Discharging Efficiency:** Minimized thermal dissipation.
- **Fault Detection Time:** Predictable and well-defined fault detection time.

## 5. Actionable, Measurable Steps

### 5.1. Hardware Initialization

1. Initialize MCU clock system and GPIOs.
2. Configure I²C peripheral in interrupt-driven mode.
3. Initialize a system timer (SysTick) for periodic interrupts (e.g., every 10 ms).
4. In the SysTick ISR, increment a global tick counter and set flags for tasks.

### 5.2. Driver and HAL Implementation

- **Low-Level I²C Driver:** Write a non-blocking I²C driver for the MCU.
- **BQ76907 HAL:**
  - `bq76907_init_protection_settings()`: Configure hardware protection thresholds.
  - `bq76907_read_cell_voltages()`: Read all cell voltages.
  - `bq76907_read_status_registers()`: Read `FAULT` and `ALERT` registers.
  - `bq76907_enable_sleep_mode()`: Put the IC into a low-power state.
- **BQ25798 HAL:**
  - `bq25798_init_charge_profile()`: Set charging voltage and current limits.
  - `bq25798_read_status_registers()`: Get charging status.
  - `bq25798_start_charging()`: Enable the charging process.

### 5.3. Firmware Architecture

- **Main Loop:** An infinite `while(1)` loop that checks flags set by the SysTick ISR and executes tasks.
- **Top-Level State Machine:** Transitions based on events like "charger connected," "over-voltage fault," or "charging complete."

```c
// Simplified main loop structure
void main() {
    system_init();
    while(1) {
        if (is_timer_task_due(TASK_READ_BMS)) {
            read_bms_status();
        }
        if (is_timer_task_due(TASK_CHECK_CHARGER)) {
            check_charger_state();
        }
        // ... other tasks

        // Update main state machine
        update_system_state_machine();
    }
}
```

### 5.4. Memory Safety and Security

- **Memory Protection Unit (MPU):** Isolate bare-metal drivers from application logic.
- **Secure Bootloader:** Validate the firmware image before execution.

## 6. Validation Protocol

### 6.1. Unit and Integration Testing

- **I²C Simulator:** Develop a software I²C simulator to inject errors and simulate responses.
- **Driver Test Suite:** Write unit tests for HAL functions to verify I²C commands and response parsing.

### 6.2. Hardware-in-the-Loop (HIL) Testing

- **Fault Injection:** Use a controlled power supply and load to trigger fault conditions.
- **Measure Response Time:** Use a logic analyzer or oscilloscope to measure the time from fault to MCU response.

### 6.3. Power Profiling

- Use a power profiler to measure the quiescent current of the entire board.

### 6.4. Worst-Case Execution Time (WCET) Analysis

- Measure the execution time of critical tasks to ensure deterministic behavior.
