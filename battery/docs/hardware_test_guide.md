# Hardware Functional Test & Verification Guide

Integrated validation plan for the STM32G0 + BQ25798 (charger) + BQ76907 (monitor/protector) stack. Focus is on staged bring‑up, safety, measurement integrity, protection behavior, and interaction between the two TI devices mediated by the MCU firmware (`main.c`).

> Treat all threshold values in this guide as placeholders until datasheet scaling constants are verified. Update after register / scaling confirmation work completes.

---
## 1. Safety First
| Risk | Mitigation |
|------|------------|
| Over-voltage on cell inputs | Start with bench supply & cell simulator (programmable sources) before real pack. |
| Short / high current surge | Use inline fuse / current-limited bench supply (ILIM slightly above expected). |
| Thermal runaway / hot components | IR camera or thermocouple on charger IC during initial high current tests. |
| Logic misconfiguration (wrong register map) | Power with reduced cell voltages (e.g., 3.5 V per cell) and conservative current limits. |
| Accidental deep discharge while debugging | Add low-voltage audible/visual alarm threshold > real protection UV. |

Pre-flight checklist:
- Confirm continuity / no shorts across pack connector.
- Verify correct pull-ups on I2C and logic levels (3.3 V).
- Validate orientation and value of sense resistor(s).
- Confirm thermistor value matches assumed Beta in firmware (if used).

---
## 2. Required Equipment
| Equipment | Purpose |
|-----------|---------|
| Programmable bench PSU (4–25 V) | Simulate adapter / input source. |
| Cell simulator or 4 isolated supplies / battery emulator | Independent per-cell voltage control for imbalance & fault injection. |
| Electronic load (CC/CV/CP) | Simulate system / discharge current paths. |
| DMM (2+), oscilloscope | Cross-check voltages, observe switching / timing. |
| IR thermometer / camera | Monitor charger thermals under load. |
| Logic analyzer (optional) | Inspect I2C transactions & timing. |

---
## 3. Test Phases Overview
| Phase | Goal |
|-------|------|
| P0 | Power and communication sanity (I2C, device IDs). |
| P1 | Static measurement accuracy (cell, pack, bus, current). |
| P2 | Charger control & profile adjustments. |
| P3 | Protection fault detection (OV, UV, OCD, SCD, OT). |
| P4 | Balancing algorithm activation & stability. |
| P5 | Low-power / idle behavior (future). |
| P6 | Integrated scenarios (charge with induced imbalance & fault). |

Each phase builds on prior confirmations; do not proceed if safety-critical anomalies are found.

---
## 4. Common Pass/Fail Criteria Template
| Aspect | Pass Criteria | Notes |
|--------|--------------|-------|
| Register access | 100% successful reads/writes w/ expected echo | No unexpected NACKs after init. |
| Measurement delta | |Δ(measured - reference)| within provisional tolerance (e.g. ±25 mV / ±50 mA) | Tighten after scaling verified. |
| Fault latency | Event flag set within datasheet max latency + margin | Use scope timestamp if needed. |
| Recovery | Flag auto-clears or manual clear sequence works per datasheet | Document method. |
| No unintended trips | No false fault flags during nominal operation window | Monitor log for 10+ cycles. |

---
## 5. Detailed Test Cases
### P0: Power & Communication
| ID | Test | Steps | Expected |
|----|------|-------|----------|
| P0.1 | MCU + I2C bring-up | Power board @ nominal input (e.g., 12 V). Firmware boots. | `[MAIN]` init prints; no hard faults. |
| P0.2 | Charger ID | Issue `BQ25798_confirmPart()` or read ID reg. | Correct part/rev bits, no error. |
| P0.3 | Monitor ID | Read `DEVICE_ID` (BQ76907). | Matches datasheet. |
| P0.4 | Basic status sweep | First `UpdateCharger` / `UpdateMonitor` cycles run. | `[CHG]` & `[MON]` lines appear on intervals. |

### P1: Measurement Accuracy
| ID | Test | Steps | Expected |
|----|------|-------|----------|
| P1.1 | Cell voltage linearity | Sweep one simulated cell 3.3→4.1 V in 0.1 V steps; others fixed. Log readings. | Monotonic increase; error within tolerance. |
| P1.2 | Pack voltage consistency | Compare sum(cell_i) vs reported pack voltage. | Difference < combined measurement tolerance. |
| P1.3 | Bus voltage read | Adjust adapter PSU 8→20 V in 2 V steps. | `voltageBus` tracks within tolerance. |
| P1.4 | Charge current sense | Apply different load currents while charging (0.2C, 0.5C...). | `currentBattery` tracks load (sign correct). |
| P1.5 | Input current limit effect | Set low input current limit, increase load above limit. | Charger throttles / holds below limit. |

### P2: Charger Control & Profile
| ID | Test | Steps | Expected |
|----|------|-------|----------|
| P2.1 | Set charge voltage | Program test voltage (e.g., 16.4 V for 4s @ 4.10 V) | Register reflects encoding; VBAT approaches target (within taper logic). |
| P2.2 | Set charge current | Program 1 A then 500 mA. | Current reduces accordingly without faults. |
| P2.3 | Thermal guard (sim) | Heat IC (hot air/controlled) while logging die temp placeholder. | Charging reduced or disabled near threshold (once scaling implemented). |
| P2.4 | Enable/disable charger | Toggle enable bit; observe current. | Current stops promptly when disabled. |

### P3: Protection & Fault Handling (Monitor)
| ID | Test | Steps | Expected |
|----|------|-------|----------|
| P3.1 | OV threshold | Raise one cell above provisional OV threshold. | OV flag set; optional FET action (future). |
| P3.2 | UV threshold | Lower one cell below UV threshold. | UV flag set; persists until recovery. |
| P3.3 | OCD | Apply discharge load pulse exceeding OCD limit. | OCD flag, discharge path disabled (future). |
| P3.4 | SCD | Very fast higher current pulse. | SCD flag latches per datasheet. |
| P3.5 | Over-temp | Heat thermistor / inject simulated value. | OT flag set. |
| P3.6 | Fault clear | Return to nominal conditions. | Flags clear per spec (auto or manual). |

### P4: Balancing
| ID | Test | Steps | Expected |
|----|------|-------|----------|
| P4.1 | Trigger balancing | Create 40 mV delta (raise one cell). | `[BAL]` shows active=1; mask selects high cell. |
| P4.2 | Stop on hysteresis | Reduce delta below hysteresis (e.g., <10 mV). | active=0; mask cleared. |
| P4.3 | Multi-cell selection | Two cells high above cutoff. | Mask includes both bits. |
| P4.4 | Suppress low-voltage balancing | All cells near 3.2 V (below safe). | (Future) No balancing engaged. |

### P5: Low Power (Future)
| ID | Test | Steps | Expected |
|----|------|-------|----------|
| P5.1 | Idle WFI | Implement WFI patch; no tasks due. | MCU current reduced vs spin. |
| P5.2 | STOP mode wake | Enter STOP, wake on tick/RTC. | Timers resume; next due tasks run. |

### P6: Integrated Scenario
| ID | Test | Steps | Expected |
|----|------|-------|----------|
| P6.1 | Charge + imbalance | Start charge with one cell 50 mV high. | Balance engages while charging remains stable. |
| P6.2 | Fault during charge | Induce OV while charging. | Charger responds (current reduce/off) + monitor OV flag. |
| P6.3 | Fault then recover | Clear OV by lowering cell voltage. | System resumes nominal charge; flag clears. |

---
## 6. Logging & Data Capture
| Item | Method | Notes |
|------|--------|-------|
| Console logs | Capture UART/semihost output to file with timestamps. | Tag build hash in header. |
| Raw register dump | Add optional debug function to snapshot key registers each cycle. | Useful for regression diff. |
| Environmental | Log ambient + device temperature. | Correlate with thermal behavior tests. |

Store results in a `test_runs/` directory with subfolders per date: `YYYY-MM-DD_phaseX/` containing raw logs + summary CSV.

---
## 7. Defect Classification
| Severity | Definition | Action |
|----------|-----------|--------|
| Critical | Safety risk or hardware damage possible | Halt testing, root cause before resume. |
| High | Incorrect protection / major functional deviation | Create issue, block dependent phases. |
| Medium | Measurement out of spec, but safe | Document, continue with caution. |
| Low | Cosmetic / log formatting | Schedule for cleanup; does not block. |

---
## 8. Traceability Mapping
Link each test ID (P*.x) back to requirement rows in `requirements_mapping.md`. Maintain a spreadsheet or add a section there recording Pass/Fail + date + reference log filename.

---
## 9. Expansion Hooks
Future additions once scaling & mask logic solidified:
- JEITA temperature zone tests (charge profile modulation).
- Adaptive balancing duty cycle limits.
- Coulomb counter calibration (charge/discharge cycle with precision shunt instrumentation).
- MPPT or input power optimization tests.

---
## 10. Quick Execution Matrix (Minimal Smoke Set)
Run before any deep regression to detect gross regressions:
| Test | Purpose |
|------|---------|
| P0.2 | Charger identity alive |
| P0.3 | Monitor identity alive |
| P1.1 | Cell voltage path linear |
| P2.2 | Charge current settable |
| P3.1 | OV detection pathway |
| P4.1 | Balancing engages |

All must PASS before running full suite.

---
## 11. Completion Criteria
Suite considered baseline-complete when:
- 100% of P0–P4 tests PASS (P5 optional, P6 partially PASS acceptable if future features not yet implemented).
- Measurement accuracy within provisional tolerances.
- No Critical/High open defects.

---
## 12. Revision Control
Tag repository when major milestone reached (e.g., `v0.3-hw-validated`). Attach compressed test evidence directory to release artifacts or internal storage.

---
*Generated initial version – update alongside driver scaling and protection implementation progress.*
