# Embedded Battery Management System (BMS) with Dynamic Charging Control

## 1. Introduction

This repository contains the firmware for a highly reliable and efficient Battery Management System (BMS) and dynamic charging circuit. The system is built around a microcontroller that interfaces with a **BQ76907** multi-cell battery monitor and a **BQ25798** charge controller via an I²C bus.

The core architectural philosophy of this project is **safety-first minimalism**. Our design prioritizes deterministic behavior, low power consumption, and provable reliability over unnecessary complexity.

## 2. Architecture Overview

The firmware employs a **bare-metal, cooperative scheduler** to manage concurrent tasks without the overhead and non-deterministic behavior of a full-fledged RTOS. This architecture is centered around a hierarchical state machine that governs the system's behavior, ensuring a clear and verifiable flow between operational states like `IDLE`, `CHARGING`, and `FAULT`.

A custom **Hardware Abstraction Layer (HAL)** is implemented to provide a clean, safe, and portable API for interacting with the BQ76907 and BQ25798 ICs. This layer handles all I²C communication, abstracting away the low-level details.

### Key Architectural Principles

- **Safety-Critical Isolation:** The MCU's role is to monitor and manage, while the hardware protection features of the BQ76907 serve as the primary, real-time safety mechanism.
- **Event-Driven Control:** A state machine ensures that system transitions are logical, testable, and driven by events.
- **Measurable Minimalism:** The codebase is lean and focuses on core functionality, with every component justified by performance, power, or safety metrics.

## 3. Hardware Components

| Component | Details |
| :--- | :--- |
| **Microcontroller Unit (MCU)** | MSPM0G3507SPMR |
| **Battery Monitor** | Texas Instruments BQ76907 |
| **Battery Charger** | Texas Instruments BQ25798 |

## 4. Firmware Structure

The repository is organized into the following directories:

| Path | Description |
| :--- | :--- |
| `main.syscfg` | System configuration file for the MCU. |
| `Makefile` | Makefile for building the project. |
| `core/` | Contains system-level initialization code and the cooperative scheduler. |
| `core/scheduler.c` | Implementation of the cooperative scheduler. |
| `core/system_init.c` | System initialization code. |
| `docs/` | Contains project documentation. |
| `docs/architecture.md` | Detailed description of the software architecture. |
| `docs/bq25798.pdf` | Datasheet for the BQ25798 charge controller. |
| `docs/bq76907.pdf` | Datasheet for the BQ76907 battery monitor. |
| `docs/bq7690xSoftwareguide.pdf` | Software guide for the BQ7690x series. |
| `docs/BRIEF.md` | Project brief and high-level design decisions. |
| `docs/i2c_guide.pdf` | Guide to the I²C protocol. |
| `drivers/` | Houses the bare-metal I²C driver and the dedicated HALs for the BQ76907 and BQ25798. |
| `drivers/bq25798.c` | Driver for the BQ25798 charge controller. |
| `drivers/bq76907.c` | Driver for the BQ76907 battery monitor. |
| `drivers/i2c.c` | I²C driver implementation. |
| `include/` | Contains header files for the project. |
| `include/bq25798.h` | Header file for the BQ25798 driver. |
| `include/bq76907.h` | Header file for the BQ76907 driver. |
| `include/i2c.h` | Header file for the I²C driver. |
| `include/main.h` | Main header file for the project. |
| `include/scheduler.h` | Header file for the cooperative scheduler. |
| `include/state_machine.h` | Header file for the state machine. |
| `include/system_init.h` | Header file for the system initialization code. |
| `src/` | Contains the main application logic, including the state machine implementation. |
| `src/main.c` | Main application file. |
| `src/state_machine.c` | Implementation of the state machine. |
| `tests/` | Contains unit tests and a hardware-in-the-loop (HIL) test harness. |
| `tests/test_bq76907.c` | Unit tests for the BQ76907 driver. |


## 5. Development and Validation

This project employs a rigorous, data-driven approach to development.

### Success Metrics

- **Quiescent Current:** `< 20 µA` in `IDLE` state.
- **I²C Latency:** `< 1 ms` for critical I²C read operations.
- **Fault Detection Time:** Deterministic fault handling.

### 5.1. Toolchain

| Tool | Details |
| :--- | :--- |
| **Compiler** | [Specify the C/C++ compiler and version, e.g., arm-none-eabi-gcc] |
| **Debugger** | [Specify the debugger, e.g., GDB with JTAG/SWD interface] |
| **Build System** | [Specify the build system, e.g., CMake or Make] |

### 5.2. Testing and Validation

- **Unit Tests:** Local tests for the HAL and I²C drivers.
- **HIL Testing:** A comprehensive HIL test rig is used to inject faults and measure the system's real-world response time and behavior.
- **Power Profiling:** The system's power consumption is continuously monitored.

## 6. How to Build and Run

[Include brief, actionable steps on how to set up the toolchain, clone the repository, build the project, and flash the firmware to the target board.]

For more detailed information, please refer to the project's documentation in the `docs/` directory.