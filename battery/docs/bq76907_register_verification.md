# BQ76907 Register & Bit Field Verification Checklist

This file is a structured checklist to TRIPLE CROSS-CHECK every placeholder currently in `bq76907.h` / `bq76907.c` against the official TI datasheet (local copy: `bq76907.pdf`).

> INSTRUCTIONS: For each item fill the three verification columns with initials/date once verified independently (e.g., engineer A, B, C). Only remove a TODO when all three confirmations are logged.

| Category | Symbol / Item | Expected (Current Placeholder) | Datasheet Source (Page / Table) | V1 | V2 | V3 | Final Value (after confirm) | Notes |
|----------|---------------|--------------------------------|----------------------------------|----|----|----|-----------------------------|-------|
| I2C Addr | BQ76907_I2C_ADDRESS_7BIT | 0x08 | | | | | | Verify variant (ADDR pin config?) |
| Register | BQ76907_REG_SYS_STAT | 0x00 | | | | | | Confirm readable/clear-on-read behavior |
| Register | BQ76907_REG_CELLBAL1 | 0x01 | | | | | | Check cell coverage (cells 1-3?) |
| Register | BQ76907_REG_CELLBAL2 | 0x02 | | | | | | Cells 4-5? or more? |
| Register | BQ76907_REG_SYS_CTRL1 | 0x04 | | | | | | Sleep/ADC bits? |
| Register | BQ76907_REG_SYS_CTRL2 | 0x05 | | | | | | CC enable? |
| Register | BQ76907_REG_PROTECT1 | 0x06 | | | | | | OVP/UVP delays? |
| Register | BQ76907_REG_PROTECT2 | 0x07 | | | | | | Short-circuit thresholds |
| Register | BQ76907_REG_PROTECT3 | 0x08 | | | | | | Timing config |
| Register | BQ76907_REG_OV_TRIP | 0x09 | | | | | | Units / LSB? |
| Register | BQ76907_REG_UV_TRIP | 0x0A | | | | | | Units / LSB? |
| Register | BQ76907_REG_CC_CFG | 0x0B | | | | | | Coulomb counter format |
| Register | BQ76907_REG_DEVICE_ID | 0x0C | | | | | | ID + revision mask |
| Register | BQ76907_REG_ADC_CONTROL | 0x0D | | | | | | Start/continuous modes |
| Register | BQ76907_REG_VCELL1_H | 0x0E | | | | | | Sequence ordering |
| Register | BQ76907_REG_VCELL1_L | 0x0F | | | | | | Endianness |
| Register | BQ76907_REG_PACK_V_H | 0x2A | | | | | | Derived vs. measured |
| Register | BQ76907_REG_PACK_V_L | 0x2B | | | | | | Scaling |
| Register | BQ76907_REG_TS1_H | 0x2C | | | | | | Thermistor formula |
| Register | BQ76907_REG_TS1_L | 0x2D | | | | | | Signed/unsigned? |
| Bitfield | BQ76907_SYS_STAT_CC_READY | bit7 | | | | | | Clear action? |
| Bitfield | BQ76907_SYS_STAT_DEVICE_XREADY | bit6 | | | | | | Name matches datasheet? |
| Bitfield | BQ76907_SYS_STAT_OV_FLAG | bit5 | | | | | | Latch/reset behavior |
| Bitfield | BQ76907_SYS_STAT_UV_FLAG | bit4 | | | | | |  | 
| Bitfield | BQ76907_SYS_STAT_SCD_FLAG | bit3 | | | | | |  | 
| Bitfield | BQ76907_SYS_STAT_OCD_FLAG | bit2 | | | | | |  | 
| Bitfield | BQ76907_SYS_STAT_OVERTEMP_FLAG | bit1 | | | | | |  | 
| Scaling  | Cell voltage LSB | 1 mV assumed | | | | | | Confirm actual (e.g. 1.0 / 0.5 mV) |
| Scaling  | Pack voltage LSB | 1 mV assumed | | | | | | Confirm relationship to cell sum |
| Scaling  | Temperature raw | raw->0.1C direct | | | | | | Likely NTC + coefficient conversion |
| Behavior | Sleep entry sequence | SYS_CTRL1 write | | | | | | Any required delays? |
| Behavior | Wake source | I2C / voltage change | | | | | | Clarify conditions |

## Verification Procedure
1. Open datasheet section: Electrical Characteristics (for scaling) & Register Map.
2. For each register:
   - Confirm address, access (R/W, R, W, RC, etc.). Add to Notes.
   - Record reset value; optionally add a RESET column if desired.
   - Confirm multi-byte ordering (big-endian vs little-endian).
3. For each bitfield:
   - Confirm bit position & meaning.
   - Note if write 1 to clear (W1C) or read-to-clear (RC).
4. For scaling:
   - Extract exact LSB in mV / °C, include formula.
   - Update driver scaling helper implementations.
5. For power modes:
   - Identify timing constraints, minimum sleep current, wake latency.

## After Verification
- Update `bq76907.h` macros and remove `TODO_VERIFY` comments.
- Implement real scaling in `bq76907.c`.
- Add low-power APIs (`enterSleep`, `exitSleep`).
- Commit with message: `feat(bq76907): fill verified register map + scaling`. 

## Optional Extensions
- Add cell balancing control API.
- Add protection threshold configuration functions.
- Add coulomb counter read & accumulation logic.

---
Generated template – fill before relying on the driver in production.
