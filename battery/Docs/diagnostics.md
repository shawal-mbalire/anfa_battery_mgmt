# Diagnostics & Debug Aids

This document explains the lightweight diagnostics introduced during the polish pass.

## Logging Macro
`BQ_LOG(fmt, ...)` is used for all new debug output. By default it expands to `printf("[BQ] " fmt "\n", ...)`.

Override it globally (e.g., route to UART or RTT) by defining before including any driver header:
```c
#define BQ_LOG(fmt, ...) SEGGER_RTT_printf(0, "[BQ] " fmt "\n", ##__VA_ARGS__)
#include "bq25798.h"
#include "bq76907.h"
```

## State Dump Functions
| Function | Purpose |
|----------|---------|
| `BQ25798_debugDump()` | Prints bus/battery volts & currents plus key charger & fault bits |
| `BQ76907_debugDump()` | Prints pack & per‑cell voltages, temperature, and decoded status flags |
| `BQ25798_dumpErrors()` | Lists recent I2C / identity / generic errors captured in ring buffer |
| `BQ76907_dumpErrors()` | Lists recent monitor/protector errors |

Call them after periodic update logic, e.g.:
```c
if ((HAL_GetTick() - lastDump) > 1000) {
    BQ25798_debugDump(&charger);
    BQ76907_debugDump(&monitor);
    lastDump = HAL_GetTick();
}
```

## Typical Output (Example)
```
[BQ] BQ25798 Debug Dump:
[BQ]   VBUS  = 12054 mV
[BQ]   IBUS  =  1540 mA
[BQ]   VBAT  = 15234 mV
[BQ]   IBAT  =  1980 mA
[BQ]   CHG_STAT0: iindpm=0 vindpm=0 wd=0 pg=1 ac2=0 ac1=0 vbus=1
[BQ]   CHG_STAT1: chg=3 vbus=2 bc12=1
[BQ]   FAULT0: ibat_reg=0 vbus_ovp=0 vbat_ovp=0 ibus_ocp=0 ibat_ocp=0 conv_ocp=0 vac2_ovp=0 vac1_ovp=0
[BQ] BQ76907 Dump: Pack=15320 mV T=28.4C
[BQ]   Cell1=3830 mV
[BQ]   Cell2=3820 mV
[BQ]   Cell3=3835 mV
[BQ]   Cell4=3835 mV
[BQ]   Flags: OV=0 UV=0 OCD=0 SCD=0 OT=0 CC_RDY=1 DEV_RDY=1

[BQ] BQ25798 Error Log (most recent 2):
[BQ]   t=3456 code=-1 det=4 reg=0x35 val=0x0000
[BQ]   t=3460 code=-1 det=4 reg=0x35 val=0x0000
```

## Integrating With a UART
If `printf` is retargeted (e.g., via `_write()` in `syscalls.c`), output will already appear on your console. For raw UART without retarget, adapt `BQ_LOG` to use `HAL_UART_Transmit` into a scratch buffer.

## Minimal Overhead Mode
Define `BQ_LOG_DISABLE` and add this before headers to suppress all logs:
```c
#ifdef BQ_LOG_DISABLE
#undef BQ_LOG
#define BQ_LOG(...) do {} while(0)
#endif
```
(Or adjust driver headers to conditionally compile logging if needed.)

## Next Enhancements (Optional)
- Add a ring buffer and watermark for asynchronous logging.
- Provide JSON mode for easier host parsing.
- Implement a structured fault decoder with textual mapping.
- Add persistent error counters (aggregate) separate from rolling buffer.
- Introduce selective suppression for repeated identical errors.

## Scaling Self-Test Harness
`BQ25798_runScalingSelfTest()` (in `bq25798_scale_test.c`) performs a pure arithmetic verification of the encode/decode helpers:

Outputs look like:
```
[BQ][SCALE] Starting scaling self-test
[BQ][SCALE] VREG 14600mV   enc=1460(expect 1460) dec=14600(expect 14600) OK
[BQ][SCALE] ICHG 4000mA    enc=400(expect 400) dec=4000(expect 4000) OK
[BQ][SCALE] ICHG 1000mA    enc=100(expect 100) dec=1000(expect 1000) OK
[BQ][SCALE] ICHG 5000mA    enc=500(expect 500) dec=5000(expect 5000) OK
[BQ][SCALE] IIN  3300mA    enc=330(expect 330) dec=3300(expect 3300) OK
[BQ][SCALE] VINDPM 3600mV  enc=36(expect 36) dec=3600(expect 3600) OK
[BQ][SCALE] PCHG 120mA     enc=3(expect 3) dec=120(expect 120) OK
[BQ][SCALE] ...
[BQ][SCALE] Self-test complete: 0 failure(s)
```

Call it early in `main()` (before I2C init if desired) since it does not touch hardware. Remove or wrap with a debug conditional for production builds.

### Runtime Assertion Report
Use `BQ25798_runAssertionReport(&charger);` to print PASS/FAIL lines for each scaling rule plus a live PART_INFO comparison (if HAL/device available). Example line:
```
[BQ][ASSERT] PART_INFO raw=0x31 part3=3 part5=0x18 rev=1 : PASS (legacy=ok full=no)
```
This helps track mismatches if the expected 5-bit part field differs once confirmed.

### Ambiguous Datasheet Sample
A supplied sample listed raw `0x03F4` for 5000 mA charge current. Given the derived 10 mA/LSB scaling that would correspond to 101,2xx mA which is impossible; expected raw is `0x01F4` (decimal 500). The harness flags only if the consistent encoding fails; it does not assert against the suspected typo.

---
Polish diagnostics scaffolding – expand once core register map is verified.
