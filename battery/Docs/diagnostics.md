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

---
Polish diagnostics scaffolding – expand once core register map is verified.
