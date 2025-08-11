# Architecture

This document expands on the architectural decisions outlined in the `README.md` and `BRIEF.md`.

## State Machine

The system is controlled by a hierarchical state machine. The top-level states are:

- `IDLE`: The system is waiting for a charger to be connected or a load to be applied.
- `CHARGING`: The battery is being charged.
- `DISCHARGING`: The battery is powering a load.
- `FAULT`: A fault condition has been detected (e.g., over-voltage, under-voltage, over-current).
- `SHUTDOWN`: The system is in a low-power state.

## Cooperative Scheduler

A non-preemptive, cooperative scheduler is used to manage tasks. This ensures deterministic behavior and avoids the complexity of an RTOS. The scheduler is driven by a periodic timer interrupt (e.g., SysTick).

## Hardware Abstraction Layer (HAL)

The HAL provides a clean and portable interface to the BQ76907 and BQ25798 ICs. It abstracts the low-level I2C communication and provides a set of high-level functions for controlling the ICs.
