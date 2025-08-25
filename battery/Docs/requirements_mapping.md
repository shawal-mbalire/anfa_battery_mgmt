# Requirements Coverage Mapping

## BQ76907 (Battery Monitor / Protector)
| Requirement | Status | Implementation / API | Notes |
|-------------|--------|----------------------|-------|
| 4s Cell Monitoring (voltages) | PARTIAL | `BQ76907_readCellVoltages`, `cellVoltage_mV[4]`, `BQ76907_readPackVoltage` | Register addresses & scaling are placeholders (`TODO_VERIFY`). |
| FET Control (CHG/DSG) | PARTIAL | `BQ76907_fetEnable`, `BQ76907_setFETOptions` | Bit masks placeholder; need real register map & bits. |
| Cell Balancing (host) | PARTIAL | `BQ76907_evaluateAndBalance`, `BQ76907_setActiveBalancingMask` | Heuristic with hysteresis; no hardware timing integration yet. |
| Voltage Protection | PARTIAL | `BQ76907_configVoltageProtection`, `setCOV/CUVThreshold` | Scaling & encoding TBD; assumes single‑byte thresholds. |
| Current Protection | PARTIAL | `BQ76907_configCurrentProtection`, OC threshold setters | Needs confirmation of raw units & register width. |
| Temperature Protection (IC) | PARTIAL | `BQ76907_configTemperatureProtection`, `setInternalOTThreshold` | Need to confirm distinct meaning of INT_OT vs Max Internal Temp. |
| Recovery Mechanisms | STUB | `BQ76907_protectionRecovery` | Requires bit definitions & sequencing. |
| Power Config / Sleep | STUB | `BQ76907_enterSleep/exitSleep`, `sleepEnable/Disable`, `setPowerConfig` | Bit semantics unverified. |
| PASSQ / SOC | STUB | `BQ76907_readPASSQ` | Need SOC algorithm or register definition. |
| Alarm Handling | STUB | `read/clearAlarmStatus`, `setAlarmEnable` | Need confirm clear behavior (write 1 vs 0). |
| Balancing Interval | HOST ONLY | `balanceInterval_ms` in config | Not yet used by a scheduler. |

## BQ25798 (Charger / Power Path)
| Requirement | Status | Implementation / API | Notes |
|-------------|--------|----------------------|-------|
| Charging Profile (pre / fast / taper) | PARTIAL | `BQ25798_updateChargeProfile` | Thresholds & scaling placeholders; need proper encoding. |
| Set Charge Voltage / Current | PARTIAL | `BQ25798_setChargeVoltage`, `BQ25798_setChargeCurrent` | Encode helpers currently identity; adjust for real LSB. |
| Input Current Limit | PARTIAL | `BQ25798_setInputCurrentLimit` | Same scaling caveat. |
| Charger Enable / Disable | PARTIAL | `BQ25798_chargerEnable` | Bit masks placeholder; confirm CHG_EN position. |
| Monitoring (Input/Output V/I) | IMPLEMENTED (raw) | `readBusVoltage/Current`, `readBatteryVoltage/Current` | Raw values assumed 1 LSB = 1 mV/mA. Verify. |
| Thermal Guard (10–45°C window) | PARTIAL | `BQ25798_thermalGuard`, `readDieTemperature` | Temperature scaling placeholder; need correct conversion & window logic. |
| Protection (OV/UV/OC) | PARTIAL | Fault status reads + implicit hardware limits | No dynamic mitigation beyond profile yet. |
| Part Identification | IMPLEMENTED | `BQ25798_confirmPart` | Masks added; still needs documented bit layout confirmation. |
| MPPT Control | NOT STARTED | — | Would use `BQ25798_REG_MPPT_CTRL`; not implemented. |
| JEITA / NTC Management | NOT STARTED | — | NTC registers not configured. |

## Key Placeholders / TODO_VERIFY Summary
- All scaling encode functions for BQ25798 (`BQ25798_encode*`).
- BQ76907 register addresses & bit masks across the header.
- Balancing register semantics (active mask vs enable bits).
- Protection threshold raw encoding for both devices.
- Alarm clear behavior and PASSQ (SOC) register meaning.
- Temperature ADC scaling for both ICs.

## Recommended Next Validation Steps
1. Populate a register definition table for both ICs (address, width, reset value, LSB, description).
2. Replace placeholder bit masks with real names and add decomposition comments near magic bytes in init sequences.
3. Implement scaling functions with constants (e.g., `#define BQ25798_VREG_LSB_mV 10` etc.).
4. Add a periodic task for BQ76907 balancing using `balanceInterval_ms` and integrate with main loop.
5. Introduce a self-test routine to read back and verify key configuration registers after `applyConfig` / `init`.
6. Add logging or LED patterns for critical fault flags (OC/OV/thermal) to facilitate debugging.
7. Extend thermal guard to incorporate hysteresis and to optionally reduce (not just disable) charge current near limits.

## Risk Notes
| Area | Risk | Mitigation |
|------|------|------------|
| Placeholder Register Addresses | Wrong writes could configure unintended functions | Verify against datasheet before deploying to hardware. |
| Scaling Assumptions | Misreported voltage/current/temperature | Add asserts or compile-time checks once constants known. |
| Balancing Heuristic | Over-balancing or unbalanced pack if thresholds mis-set | Guard with min voltage and delta hysteresis (already in place), add max active time cap. |
| Thermal Control | Wrong scaling could disable charger unnecessarily | Implement proper temperature conversion and add debug output. |

## Glossary / Reference Hooks
- CUV/COV: Cell Under/Over Voltage thresholds (mV)
- OCD/SCD: Over Current (Discharge) / Short Circuit detection
- PASSQ: Likely SOC / pass-through quality metric (verify)
- CB: Cell Balancing
- LSB: Least Significant Bit weight of an ADC/register

---
Generated automatically; update once datasheet values are confirmed.
