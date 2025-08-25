# BQ76907 Recommended Operational Workflow (PLACEHOLDER – VERIFY)

> This workflow summarizes a typical sequence for bringing up and using a TI multi-cell battery monitor / protector like the BQ76907. Every step MUST be validated against the actual datasheet text (timings, required delays, unlock codes, bit names). Replace or adjust where different.

## 1. Power-Up / Hardware Preconditions
- Ensure VCx pins connected in ascending order and within absolute voltage limits.
- Thermistor(s) connected (TS1, etc.) if temperature-dependent protections are enabled.
- I2C pull-ups present; address selection pins configured.
- PACK / BAT / SR sense resistor sized for expected currents.

## 2. Basic Communication Bring-Up
1. Wait for device internal POR time (datasheet tPOR).  // TODO_VERIFY
2. Read DEVICE_ID register; store revision for conditional workarounds.
3. Optionally clear/ack any startup status flags (if read-to-clear behavior exists).

## 3. Configuration Phase
Suggested sequence to minimize unintended FET toggling:
1. Disable protections or set to benign thresholds during setup (e.g., set high OV, low UV). // TODO_VERIFY addresses
2. Program cell balancing constraints (disable balancing initially).
3. Set UV/OV thresholds and delays.
4. Configure SCD (short-circuit discharge) threshold & delay; then OCD (over-current discharge) parameters.
5. Configure temperature limits (OT/UT) if supported.
6. Enable or configure Coulomb Counter (if required) via CC_CFG.
7. Enable ADC continuous or one-shot mode (ADC_CONTROL). Delay for first valid conversion. // TODO_VERIFY delay
8. Re-enable protections with final thresholds.

## 4. Normal Monitoring Loop
Repeat at chosen sampling period:
1. Read SYS_STAT; log or react to any latched faults.
2. Read cell voltages (batch read sequential registers for efficiency).
3. Read pack voltage.
4. Read temperature(s) and convert using NTC table or Beta equation.
5. Optional: read Coulomb counter or accumulated SOC estimator.
6. Execute cell balancing algorithm (if criteria met) – ensure not during charge/discharge surge events.

## 5. Cell Balancing Strategy (High-Level)
- Trigger when highest cell - lowest cell > balance_threshold (e.g., 10–30 mV). // Tune
- Avoid balancing below a minimum cell voltage (e.g., < 3.4 V) or during high current.
- Time-slice balancing to limit thermals (e.g., duty cycle each cell).
- Stop balancing when delta < hysteresis margin.

## 6. Fault Handling
| Fault Type | Immediate Action | Follow-Up |
|------------|------------------|-----------|
| OV         | Optionally disable charge FET, log event | Resume when below re-arm threshold |
| UV         | Disable discharge FET (if required) | Wait for charge event / recovery |
| SCD        | Turn off discharge path quickly | Enforce cool-down time |
| OCD        | Turn off discharge path | Re-enable after timeout or manual clear |
| OT         | Disable charge or discharge depending (OTC/OTD) | Wait until temp recovers |

(Verify which faults auto-clear vs. require explicit flag clearing.)

## 7. Low Power (Sleep) Mode
1. Decide entry criteria (no load, minimal voltage change for X seconds).
2. Clear pending ADC operations if continuous mode running.
3. Write SYS_CTRL1: set SLEEP bit, clear ADC enable. // TODO_VERIFY bit positions
4. MCU may also enter its own low-power mode.
5. Wake sources may include: I2C access, cell voltage change > threshold, external interrupt pin. // TODO_VERIFY list
6. On wake: re-enable ADC, re-read configuration if any volatile registers.

## 8. Ship Mode (If Supported)
- Sequence often: assert ship bit(s) -> device reduces quiescent current drastically until charger/source applied.
- Recovery typically requires plug-in event or specific wake stimulus (not just I2C). // TODO_VERIFY details

## 9. Data Scaling & Conversion (To Fill)
| Quantity | Raw Format | LSB (Assumed) | Formula | Verify |
|---------|-----------|--------------|---------|--------|
| Cell Voltage | 16-bit | 1 mV | mV = raw * 1 | TODO |
| Pack Voltage | 16-bit | 1 mV | mV = raw * 1 | TODO |
| Temperature  | 16-bit | TBD  | Use NTC: T(K)=1/(1/T0 + (1/B)*ln(R/R0)) | TODO |
| CC / Current | TBD    | TBD  | Depends on sense resistor & scaling | TODO |

## 10. Recommended I2C Access Patterns
- Group sequential cell voltage reads into one burst for efficiency.
- Avoid polling high-rate; rely on ALERT/INT pin if available for fault notification.
- Exponential backoff on communication errors; attempt soft reset only after multiple failures.

## 11. Validation & Test Plan
| Test | Steps | Expected |
|------|-------|----------|
| ID Read | Power board, read ID | Matches datasheet/revision |
| UV Threshold | Configure low UV, lower a cell via simulator | UV flag set, FET action correct |
| OV Threshold | Configure modest OV, raise cell via supply | OV flag set, charge path disabled |
| SCD/OCD | Inject fault via load pulse | Flags latch, path disabled |
| Sleep Entry/Exit | Enter sleep, measure current | Quiescent current reduced |
| Balancing | Force imbalance, enable balancing | Correct cell(s) balancing |

## 12. Production Notes
- Log first 100 operational hours with extended debug traces.
- Include CRC or checksum on critical parameter blocks (if storing thresholds externally).
- Version tag configuration set so field updates can be tracked.

---
PLACEHOLDER content pending datasheet cross-check. Replace TODO items with verified data.
