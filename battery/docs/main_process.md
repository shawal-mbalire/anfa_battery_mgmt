# Runtime Process Flow (`main.c`)

This document explains the control flow, timing loops, helper functions, and diagnostic prints implemented in `Core/Src/main.c`.

---
## 1. High-Level Lifecycle
1. HAL / system startup: `HAL_Init()` and `SystemClock_Config()` set the MCU core, bus clocks, and SysTick.
2. Peripheral init: `MX_GPIO_Init()` and `MX_I2C1_Init()` configure GPIO (LEDs, etc.) and the I2C bus used by both TI devices.
3. Device bring-up:
   - `BQ25798_init()` (charger) queried first.
   - `BQ76907_init()` (monitor) follows.
   - A provisional configuration (`BQ76907_applyConfig`) is applied to the monitor; results are logged.
4. Scheduler loop (cooperative, polling): main `while(1)` body time-slices work based on millisecond tick intervals.
5. Non-blocking LED + error handling logic executes each iteration.

The firmware purposefully avoids blocking delays inside the loop (except inside HAL/drivers as needed) to keep iteration latency low and predictable.

---
## 2. Timing Model
| Interval Constant | Purpose | Current Value |
|-------------------|---------|---------------|
| `BQ_UPDATE_INTERVAL_MS` | Charger (BQ25798) status & measurements refresh | 500 ms |
| `BQ76907_UPDATE_INTERVAL_MS` | Monitor (BQ76907) cell + pack metrics refresh | 750 ms (staggered vs charger) |
| `BALANCE_INTERVAL_MS` | Evaluate and (naively) apply cell balancing mask | 5000 ms |
| `ERROR_LED_BLINK_RATE_MS` | Blink cadence for charger thermal fault (tshut) | 200 ms |

Staggering the charger (500 ms) and monitor (750 ms) updates reduces I2C congestion and interleaves transactions naturally (LCM = 1500 ms). Balancing runs much less frequently to minimize FET toggling and current perturbations.

---
## 3. State & Globals
| Variable | Description |
|----------|-------------|
| `bq25798_charger` | Struct instance representing charger driver state (status bytes, measurements). |
| `bq76907_monitor` | Struct instance representing monitor driver state (cell voltages, faults). |
| `last_bq_update_tick` | Last tick timestamp when charger refresh executed. |
| `last_bq76907_update_tick` | Last tick timestamp for monitor refresh. |
| `last_balance_eval_tick` | Last tick for balancing evaluation. |
| `last_error_led_toggle_tick` | Last time the error LED (orange) was toggled. |

All timing comparisons use monotonically increasing `HAL_GetTick()` (millisecond SysTick).

---
## 4. Main Loop Scheduling Logic
Pseudocode representation:
```c
while (1) {
    tick = HAL_GetTick();
    if (tick - last_bq_update_tick      >= BQ_UPDATE_INTERVAL_MS)   { last_bq_update_tick = tick; UpdateCharger(); }
    if (tick - last_bq76907_update_tick >= BQ76907_UPDATE_INTERVAL_MS) { last_bq76907_update_tick = tick; UpdateMonitor(); }
    if (tick - last_balance_eval_tick   >= BALANCE_INTERVAL_MS)     { last_balance_eval_tick = tick; EvaluateBalancing(); }
    handleErrorLed();
    // room for more non-blocking tasks
}
```
No delay is inserted deliberately—loop cycles quickly when no task is due.

### 4.1 ASCII Timeline / Scheduler Diagram
```
Time (ms) --->

Charger (500 ms) : |----C----|----C----|----C----|----C----|
Monitor (750 ms) : |------M------|------M------|------M----
Balancing (5 s)  : |--------------------B--------------------| (repeat every 5000 ms)

Legend:
 C = UpdateCharger()
 M = UpdateMonitor()
 B = EvaluateBalancing()

Interleave Example (first 3 seconds):
0ms:  C M (both due at start)  
500ms: C          
750ms:    M       
1000ms: C         
1500ms: C M       
2000ms: C         
2250ms:    M      
2500ms: C         
3000ms: C M       (Balancing still pending until 5000ms)
```

---
## 5. Helper Functions
### 5.1 `UpdateCharger()`
Responsibilities:
- Read multiple charger status/fault registers (`readChargerStatus0..4`, `readFaultStatus0/1`).
- Acquire instantaneous bus & battery voltage/current via `BQ25798_read*` functions.
- Drive a status LED (GPIOC PIN 13) based on `vbat_present_stat` (battery presence / condition scenario placeholder).
- Log an aggregated status line through `BQ25798_logStatus()` followed by a concise timing + measurement summary.

Output tags:
- `[FUNC] UpdateCharger BEGIN/END` entry/exit.
- `[CHG] Update begin/end ...` with timing and measurement snapshot.

### 5.2 `UpdateMonitor()`
Responsibilities:
- Read system status (`BQ76907_readSystemStatus`).
- Refresh all cell voltages, pack voltage, and a temperature proxy.
- Call `BQ76907_logStatus()` (extended/multi-line internal dump, if implemented).
- Aggregate fault conditions into `anyFault` (OV, UV, OCD, SCD, OT).
- Edge-trigger print of FAULT or FAULT CLEARED.
- Emit one summary line including cell voltages and fault flag states.

Output tags:
- `[FUNC] UpdateMonitor BEGIN/END`.
- `[MON] Update begin` and `[MON] Update end ...` summary.
- `[MON] FAULT:` or `[MON] FAULT CLEARED` on state change only.

### 5.3 `EvaluateBalancing()`
Responsibilities:
- Gather min/max cell voltage (using static helpers `findMinCell` / `findMaxCell`).
- Compute delta (imbalance magnitude).
- If balancing inactive and delta above `BALANCE_THRESHOLD_MV`, compute a naive midpoint cutoff and set mask bits for cells above the cutoff.
- If active and delta falls below `BALANCE_HYSTERESIS_MV`, clear mask.
- Call `applyCellBalancingMask(mask)` when (de)activating.

Output tags:
- `[FUNC] EvaluateBalancing BEGIN/END`.
- `[BAL] Evaluate begin/end ... delta= active=`.

Limitations / placeholders:
- Simple midpoint algorithm — does not limit per-cell duty cycle, thermal model, or sequence order.
- Static thresholds: real design should parameterize and calibrate based on chemistry.

### 5.4 `applyCellBalancingMask(uint8_t mask)`
Responsibilities:
- (Future) Write mask into monitor registers controlling balancing FETs.
- Currently logs mask only.

Output tags:
- `[FUNC] applyCellBalancingMask BEGIN/END`.
- `[BAL] Apply mask=0x..`.

### 5.5 Utility Functions
- `findMaxCell` / `findMinCell`: single-pass reductions to get extremes from the `cellVoltage_mV` array.

---
## 6. Fault / LED Handling
Two visual/error indicators managed in the main loop:
1. Charger thermal shutdown flag (`bq25798_charger.faultStatus1.tshut_stat`): triggers a periodic toggle (200 ms) of Orange LED (GPIOA PIN 5) to indicate charger-level thermal issue.
2. Monitor aggregated fault (`anyFault` in `UpdateMonitor`): toggling LED at 150 ms cadence within `UpdateMonitor` (separate path) plus console FAULT lines.

Potential improvement: unifying LED patterns (e.g., pattern codes for different sources) to avoid contention between charger and monitor blink logic.

---
## 7. Configuration Snapshot (`BQ76907_Config`)
A provisional configuration struct is applied after initialization. All fields are placeholders marked for later datasheet verification (voltages, current thresholds, protections, FET behavior). The code prints configuration via `BQ76907_logConfig()` if application succeeds.

Key fields:
- `cellCount = 4` (4-series pack)
- UV / OV thresholds (mV) and discharge/charge OC limits
- Over-temp bounds and balancing interval placeholder
- Protection bitfields (`protectionsA/B`, FET options) all zeroed until validated

---
## 8. Diagnostic Print Conventions
| Tag Prefix | Meaning |
|------------|---------|
| `[FUNC]` | Lifecycle entry/exit for helper functions (profiling + watchdog of call frequency). |
| `[MAIN]` | One-time initialization progress in `main()` (device bring-up + config). |
| `[CHG]` | Charger measurement cycle summary. |
| `[MON]` | Monitor measurement cycle summary + fault transitions. |
| `[BAL]` | Balancing decision path (evaluate / apply). |

Disable quickly (temporary build): add at the top of `main.c` before includes:
```c
#define printf(...) ((void)0)
```

---
## 9. Extension Hooks (Future)
| Hook | Rationale |
|------|-----------|
| Telemetry aggregation buffer | Batch measurements for downstream comms (UART/CAN) without timing jitter. |
| Adaptive balancing scheduler | Spread balancing windows per cell to reduce thermal hotspots. |
| Fault escalation layer | Map raw faults to a prioritized alarm queue and debounce transient flags. |
| Parameter table | Move thresholds and timings to a single structure for runtime adaptation or profile loading. |

---
## 10. Known Limitations / TODOs
- All `TODO_VERIFY` values in drivers not yet validated against datasheets.
- No low-power (sleep) state transitions implemented; loop always active.
- Balancing logic naive; no current measurement correlation, no hysteresis on mask toggling beyond simple delta band.
- No persistence or logging of historical faults.
- Error_handler: infinite loop without attempt to soft recover.

---
## 11. Quick Trace Interpretation Example
Sample (illustrative) log fragment:
```
[MAIN] Init start
[MAIN] Charger init done (+12ms)
[MAIN] Monitor init done (+9ms)
[MAIN] Monitor config applied (+3ms)
[FUNC] UpdateCharger BEGIN
[CHG] Update begin
[CHG] Update end (2ms) VBAT=14405mV IBAT=120mA BUS=19012mV/250mA Fault1.tshut=0
[FUNC] UpdateCharger END
[FUNC] UpdateMonitor BEGIN
[MON] Update begin
[MON] Update end (3ms) Pack=14990mV Cells=3740,3744,3751,3755 mV Flags OV=0 UV=0 OCD=0 SCD=0 OT=0
[FUNC] UpdateMonitor END
[FUNC] EvaluateBalancing BEGIN
[BAL] Evaluate begin
[BAL] Evaluate end (0ms) delta=15 mV active=0
[FUNC] EvaluateBalancing END
```
Reading it:
- Charger/monitor cycles interleave (note different intervals).
- Balancing executed every 5 s; delta below 25 mV so inactive.
- No faults (all flags zero).

---
## 12. Change Impact of Adding More Prints
Adding more prints inside high-frequency paths increases:
- UART (or semihost) bandwidth usage
- Loop latency (blocking in `printf`)
Recommend grouping values into one line per cycle (already done) and avoiding prints inside tight per-cell loops except on state change.

---
## 13. Fast Checklist (for reviewers)
- [x] Non-blocking loop (no artificial delay)
- [x] Staggered periodic tasks
- [x] Minimal global shared state
- [x] Clear diagnostic tagging
- [ ] Validated thresholds (pending)
- [ ] Robust fault escalation (pending)
- [ ] Advanced balancing strategy (pending)

---
*Generated to mirror the current implementation on: 2025-09-01.*

---
## 14. Porting to Low-Power / Sleep Scheduling

Currently the loop spins continuously. To reduce energy:

### 14.1 Basic Tick Sleep (WFI)
Insert a `__WFI()` (Wait For Interrupt) when no task is immediately due:
```c
for(;;) {
    uint32_t now = HAL_GetTick();
    uint32_t next = soonest_due(now);
    if ((int32_t)(next - now) > 0) {
         // Optionally configure a wake-up timer here
         __WFI();
    }
    run_due_tasks(now);
}
```

### 14.2 Using RTC / LPTIM
1. Configure a low-power timer (LPTIM or RTC wake-up) for the minimum delta until any task deadline.
2. Enter STOP2 (or STOP1) mode instead of simple WFI.
3. On wake, reinitialize clocks if required (HSI vs MSI / PLL restoration) before I2C activity.

### 14.3 Deadline Calculation
Maintain next deadlines:
```c
uint32_t tChgNext, tMonNext, tBalNext; // updated after each task
uint32_t soonest_due(uint32_t now){
    uint32_t m = tChgNext;
    if ((int32_t)(tMonNext - m) < 0) m = tMonNext;
    if ((int32_t)(tBalNext - m) < 0) m = tBalNext;
    return m;
}
```

### 14.4 Clock & Wake Latency Considerations
- Ensure wake + clock stabilization < smallest interval (500 ms here) margin.
- If stabilization consumes significant time, consider coarsening intervals or pre-waking early.

### 14.5 I2C in Low-Power
- Avoid entering deep sleep while an I2C transfer is pending.
- In drivers, set a flag when starting a transaction; only sleep when all buses idle.

### 14.6 Printf Impact
`printf` keeps the MCU active; optionally buffer output and flush in batches, or suppress in low-power builds via macro.

### 14.7 Future Enhancement Path
| Step | Benefit |
|------|---------|
| Add deadline heap | Scales if more periodic tasks added. |
| Integrate error backoff | Sleep longer when faults stable, wake quickly on change interrupt. |
| Interrupt-driven cell alerts | Use alert pin from monitor to wake earlier than polling interval. |

### 14.8 Minimal Patch Outline
1. Convert global last_* into next_deadline style variables.  
2. After running a task, set its next deadline = now + interval.  
3. Compute earliest deadline, program low-power timer, enter STOP mode.  
4. On wake, update `now`, run any tasks with `now >= deadline`.  
5. Guard against tick wrap (use signed subtraction).  

---
