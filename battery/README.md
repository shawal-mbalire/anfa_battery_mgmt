# ANFA Battery Management (STM32G0 + BQ25798 + BQ76907)

Lightweight embedded firmware components for a 4‑series Li‑ion battery system using:
- TI BQ25798 (buck/boost charger + power path)
- TI BQ76907 (battery monitor / protector) – scaffold with verification placeholders
- STM32G0 HAL (I²C, GPIO, timing)

> Many register addresses, bit masks, and scaling factors are intentionally tagged `TODO_VERIFY`. They must be replaced with authoritative datasheet values before deploying to hardware.

## Repo Layout
```
Core/
  Inc/ bq25798.h, bq76907.h, ...
  Src/ bq25798.c, bq76907.c, main.c, ...
docs/
  architecture.md
  BRIEF.md
  bq76907_register_verification.md
  bq76907_workflow.md
  requirements_mapping.md
```

## Build
Project generated via STM32CubeIDE (Makefile present under `Debug/`). Build inside IDE or invoke the generated makefile.

## High-Level Flow
1. `main.c` initialises I²C and both drivers.
2. Charger identity confirmed via `BQ25798_confirmPart` (now supports 5‑bit part extraction placeholder).
3. Monitoring loop periodically:
   - Reads bus/battery voltages & currents
   - Reads cell voltages / pack voltage (BQ76907)
   - Evaluates balancing (`BQ76907_evaluateAndBalance`)
   - Updates dynamic charge profile (`BQ25798_updateChargeProfile`)
4. Thermal guard or protection wrappers may disable charging.

## Debugging Aids
- Unified logging macro `BQ_LOG` (defaults to `printf`). Override by defining `BQ_LOG(fmt, ...)` before including headers.
- `BQ25798_debugDump()` and `BQ76907_debugDump()` provide quick state summaries.

## Verification Checklist Snapshot
See `docs/requirements_mapping.md` and `docs/bq76907_register_verification.md` for full list. Key pending items:
- Confirm PART_INFO 5‑bit part value for BQ25798.
- Replace all `TODO_VERIFY` register addresses in `bq76907.h`.
- Implement proper scaling (LSB constants) for voltage/current/temp on both devices.

For a structured hardware bring-up and validation sequence, see `docs/hardware_test_guide.md`.

## Adding Real Scaling
Example pattern once LSB known:
```c
#define BQ25798_VREG_LSB_mV 10
static inline uint16_t BQ25798_encodeChargeVoltage_mV(uint16_t mV){
    return (uint16_t)(mV / BQ25798_VREG_LSB_mV);
}
```
Repeat for current & temperature; adjust readback if needed.

## Safety Note
Do not enable balancing or high charge currents until thresholds and FET control bits are verified. Incorrect register writes could disable protections or overcharge cells.

## Next Suggested Steps
1. Populate real register maps from datasheets.
2. Add self-test that reads back every written config register and asserts match.
3. Introduce error escalation strategy (e.g., LED blink patterns, persistent fault latch).
4. Flesh out JEITA / NTC logic (temperature dependent charge adjustments).

## Debug Print Instrumentation
This build intentionally keeps debugging simple: only plain `printf` calls inside `Core/Src/main.c` provide:
- Function entry/exit markers (`[FUNC] ... BEGIN/END`)
- Charger update snapshots (`[CHG]`)
- Monitor update snapshots and faults (`[MON]`)
- Balancing decisions (`[BAL]`)

See the detailed runtime flow documentation in `docs/main_process.md`.

No drivers or headers contain logging macros. To silence all output quickly while testing timing, add at the very top of `main.c`:
```c
#define printf(...) ((void)0)
```
or comment out specific lines. Avoid introducing new logging abstractions unless a long‑term tracing backend is required.

## License
See `Drivers/STM32G0xx_HAL_Driver/License.txt` for HAL licensing. Project-level new code is intended MIT (add a LICENSE file if public release planned).

---
Generated polish pass (placeholders remain until datasheet confirmation).
