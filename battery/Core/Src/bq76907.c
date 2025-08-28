/*
 * bq76907.c
 * Skeleton implementation for BQ76907 driver.
 * IMPORTANT: All register addresses & scaling factors MUST be VERIFIED against the datasheet.
 * Each placeholder remains tagged in the header with TODO_VERIFY until confirmed.
 */
#include "bq76907.h"

/* -------- Internal Helper: Combine two bytes (MSB first) -------- */
static inline uint16_t u16_be(uint8_t hi, uint8_t lo){ return ((uint16_t)hi << 8) | lo; }

/**
 * @brief Initialise BQ76907 driver context.
 * Performs a minimal communication test by reading the DEVICE_ID register.
 * NOTE: No configuration (protections / thresholds) is applied here yet.
 * @return 0 on success, non-zero error code (1 = I2C read failure).
 */
uint8_t BQ76907_init(BQ76907 *dev, I2C_HandleTypeDef *hi2c){
    dev->i2cHandle = hi2c;

    // Basic: read device ID (placeholder register) for sanity
    uint8_t id = 0; // TODO: Replace with verified DEVICE_ID register address usage
    if (BQ76907_ReadRegister(dev, BQ76907_REG_DEVICE_ID, &id) != HAL_OK){
        return 1; // comm error
    }
    // Optionally compare against expected ID (once known) else return distinct code
    // if ((id & 0xFF) != EXPECTED_ID) return 2;

    // Enable ADC / measurements (placeholder control sequence)
    // TODO: Write verified ADC enable bits
    // BQ76907_WriteRegister(dev, BQ76907_REG_ADC_CONTROL, 0x01); // hypothetical

    return 0; // success
}

/**
 * @brief Read system status register and decode fault / ready bits.
 * Updates dev->status structure with parsed flags.
 */
HAL_StatusTypeDef BQ76907_readSystemStatus(BQ76907 *dev){
    uint8_t regVal;
    HAL_StatusTypeDef st = BQ76907_ReadRegister(dev, BQ76907_REG_SYS_STAT, &regVal);
    if (st == HAL_OK){
        dev->status.cc_ready  = (regVal & BQ76907_SYS_STAT_CC_READY) ? 1:0;
        dev->status.dev_ready = (regVal & BQ76907_SYS_STAT_DEVICE_XREADY) ? 1:0;
        dev->status.ov_fault  = (regVal & BQ76907_SYS_STAT_OV_FLAG) ? 1:0;
        dev->status.uv_fault  = (regVal & BQ76907_SYS_STAT_UV_FLAG) ? 1:0;
        dev->status.scd_fault = (regVal & BQ76907_SYS_STAT_SCD_FLAG)? 1:0;
        dev->status.ocd_fault = (regVal & BQ76907_SYS_STAT_OCD_FLAG)? 1:0;
        dev->status.ot_fault  = (regVal & BQ76907_SYS_STAT_OVERTEMP_FLAG)?1:0;
    }
    return st;
}

/**
 * @brief Read all configured cell voltage registers (4s pack per requirements).
 * Adjust base address arithmetic once actual register map confirmed.
 */
HAL_StatusTypeDef BQ76907_readCellVoltages(BQ76907 *dev){
    for (uint8_t cell = 0; cell < 4; ++cell){
        uint8_t hi, lo;
        uint8_t baseHigh = BQ76907_REG_VCELL1_H + (cell * 2);
        HAL_StatusTypeDef st = BQ76907_ReadRegister(dev, baseHigh, &hi);
        if (st != HAL_OK) return st;
        st = BQ76907_ReadRegister(dev, baseHigh + 1, &lo);
        if (st != HAL_OK) return st;
        uint16_t raw = u16_be(hi, lo);
        dev->cellVoltage_mV[cell] = BQ76907_scaleCellVoltage(raw);
    }
    return HAL_OK;
}

/**
 * @brief Read pack voltage (two-byte) and scale to millivolts.
 */
HAL_StatusTypeDef BQ76907_readPackVoltage(BQ76907 *dev){
    uint8_t hi, lo;
    HAL_StatusTypeDef st = BQ76907_ReadRegister(dev, BQ76907_REG_PACK_V_H, &hi);
    if (st != HAL_OK) return st;
    st = BQ76907_ReadRegister(dev, BQ76907_REG_PACK_V_L, &lo);
    if (st != HAL_OK) return st;
    uint16_t raw = u16_be(hi, lo);
    dev->packVoltage_mV = BQ76907_scalePackVoltage(raw);
    return HAL_OK;
}

/**
 * @brief Read temperature sensor 1 raw value and convert to 0.1 degC units.
 */
HAL_StatusTypeDef BQ76907_readTemperature1(BQ76907 *dev){
    uint8_t hi, lo;
    HAL_StatusTypeDef st = BQ76907_ReadRegister(dev, BQ76907_REG_TS1_H, &hi);
    if (st != HAL_OK) return st;
    st = BQ76907_ReadRegister(dev, BQ76907_REG_TS1_L, &lo);
    if (st != HAL_OK) return st;
    int16_t raw = (int16_t)u16_be(hi, lo);
    dev->ts1_degC_x10 = BQ76907_scaleTemperature(raw);
    return HAL_OK;
}

/* Low level I2C wrappers */
/**
 * @brief Low-level single register read helper.
 */
HAL_StatusTypeDef BQ76907_ReadRegister(BQ76907 *dev, uint8_t reg, uint8_t *data){
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(dev->i2cHandle, BQ76907_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
    if (st != HAL_OK){
        BM_PUSH_ERROR(dev, BM_SRC_BQ76907, BM_ERR_I2C, (uint8_t)st, reg, 0);
    }
    return st;
}

/**
 * @brief Low-level burst read helper for sequential registers.
 */
HAL_StatusTypeDef BQ76907_ReadRegisters(BQ76907 *dev, uint8_t reg, uint8_t *data, uint8_t len){
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(dev->i2cHandle, BQ76907_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
    if (st != HAL_OK){
        BM_PUSH_ERROR(dev, BM_SRC_BQ76907, BM_ERR_I2C, (uint8_t)st, reg, 0);
    }
    return st;
}

/**
 * @brief Low-level single register write helper.
 */
HAL_StatusTypeDef BQ76907_WriteRegister(BQ76907 *dev, uint8_t reg, uint8_t data){
    HAL_StatusTypeDef st = HAL_I2C_Mem_Write(dev->i2cHandle, BQ76907_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    if (st != HAL_OK){
        BM_PUSH_ERROR(dev, BM_SRC_BQ76907, BM_ERR_I2C, (uint8_t)st, reg, data);
    }
    return st;
}

/* Scaling helpers (placeholder implementations) */
/**
 * @brief Convert raw cell voltage reading to millivolts.
 * Placeholder: assumes 1 LSB = 1 mV.
 */
uint16_t BQ76907_scaleCellVoltage(uint16_t raw){
    // Datasheet will define LSB (e.g., 1 mV / 0.5 mV). Assume 1 mV for now.
    return raw; // TODO_VERIFY
}
/**
 * @brief Convert raw pack voltage reading to millivolts.
 * Placeholder scaling identical to cell until verified.
 */
uint16_t BQ76907_scalePackVoltage(uint16_t raw){
    return raw; // TODO_VERIFY
}
/**
 * @brief Convert raw temperature to 0.1 degC units.
 * Placeholder: identity mapping until thermistor conversion is defined.
 */
int16_t BQ76907_scaleTemperature(int16_t raw){
    // Example: raw * 0.1C per LSB -> store *10. Adjust once known.
    return raw; // TODO_VERIFY
}

/* --- Low Power Control (placeholders – verify bit meanings in SYS_CTRL1) --- */
HAL_StatusTypeDef BQ76907_enterSleep(BQ76907 *dev){
    uint8_t v;
    HAL_StatusTypeDef st = BQ76907_ReadRegister(dev, BQ76907_REG_SYS_CTRL1, &v);
    if (st != HAL_OK) return st;
    v &= ~BQ76907_SYS_CTRL1_ADC_EN_BIT; /* disable ADC */
    v |=  BQ76907_SYS_CTRL1_SLEEP_BIT;  /* set sleep */
    return BQ76907_WriteRegister(dev, BQ76907_REG_SYS_CTRL1, v);
}

HAL_StatusTypeDef BQ76907_exitSleep(BQ76907 *dev){
    uint8_t v;
    HAL_StatusTypeDef st = BQ76907_ReadRegister(dev, BQ76907_REG_SYS_CTRL1, &v);
    if (st != HAL_OK) return st;
    v &= ~BQ76907_SYS_CTRL1_SLEEP_BIT;  /* clear sleep */
    v |=  BQ76907_SYS_CTRL1_ADC_EN_BIT; /* enable ADC */
    return BQ76907_WriteRegister(dev, BQ76907_REG_SYS_CTRL1, v);
}

/* ================= Configuration & Control Stubs ================= */
HAL_StatusTypeDef BQ76907_enterConfigUpdate(BQ76907 *dev){
    /* Typically a sequence: write command / set bit; placeholder single write */
    return BQ76907_WriteRegister(dev, BQ76907_CMD_SET_CFGUPDATE, 0x01); /* TODO_VERIFY actual mechanism */
}

HAL_StatusTypeDef BQ76907_exitConfigUpdate(BQ76907 *dev){
    return BQ76907_WriteRegister(dev, BQ76907_CMD_EXIT_CFGUPDATE, 0x01); /* TODO_VERIFY */
}

HAL_StatusTypeDef BQ76907_applyConfig(BQ76907 *dev, const BQ76907_Config *cfg){
    /* High level sequence: enter config, write registers, exit config */
    HAL_StatusTypeDef st = BQ76907_enterConfigUpdate(dev); if (st!=HAL_OK) return st;
    st = BQ76907_setPowerConfig(dev, cfg->powerConfig); if (st!=HAL_OK) return st;
    st = BQ76907_setDAConfig(dev, cfg->daConfig); if (st!=HAL_OK) return st;
    st = BQ76907_setRegoutConfig(dev, cfg->regoutConfig); if (st!=HAL_OK) return st;
    st = BQ76907_setVcellMode(dev, cfg->cellCount); if (st!=HAL_OK) return st;
    st = BQ76907_setAlarmMaskDefault(dev, cfg->alarmMaskDefault); if (st!=HAL_OK) return st;
    st = BQ76907_setFETOptions(dev, cfg->fetOptions); if (st!=HAL_OK) return st;
    st = BQ76907_enableProtectionsA(dev, cfg->protectionsA); if (st!=HAL_OK) return st;
    st = BQ76907_enableProtectionsB(dev, cfg->protectionsB); if (st!=HAL_OK) return st;
    st = BQ76907_setDSGFetProtectionsA(dev, cfg->dsgFetProtA); if (st!=HAL_OK) return st;
    st = BQ76907_setCHGFetProtectionsA(dev, cfg->chgFetProtA); if (st!=HAL_OK) return st;
    st = BQ76907_setLatchLimit(dev, cfg->latchLimit); if (st!=HAL_OK) return st;
    st = BQ76907_setMaxInternalTemp(dev, cfg->maxInternalTemp_C); if (st!=HAL_OK) return st;
    st = BQ76907_setCUVThreshold(dev, cfg->uvThreshold_mV); if (st!=HAL_OK) return st;
    st = BQ76907_setCOVThreshold(dev, cfg->ovThreshold_mV); if (st!=HAL_OK) return st;
    st = BQ76907_setOCChargeThreshold(dev, cfg->ocCharge_mA); if (st!=HAL_OK) return st;
    st = BQ76907_setOCDischarge1Threshold(dev, cfg->ocDischarge1_mA); if (st!=HAL_OK) return st;
    st = BQ76907_setOCDischarge2Threshold(dev, cfg->ocDischarge2_mA); if (st!=HAL_OK) return st;
    st = BQ76907_setInternalOTThreshold(dev, cfg->internalOT_C); if (st!=HAL_OK) return st;
    st = BQ76907_setVoltageTime(dev, cfg->voltageTimeUnits); if (st!=HAL_OK) return st;
    st = BQ76907_setAlarmEnable(dev, cfg->alarmEnableMask); if (st!=HAL_OK) return st;
    /* Additional host-side scheduling for balanceInterval_ms not written here */
    HAL_StatusTypeDef st2 = BQ76907_exitConfigUpdate(dev);
    if (st2 == HAL_OK) {
        dev->activeConfig = *cfg; /* snapshot */
        return st;
    }
    return (st==HAL_OK)? st2 : st; /* return first failure or exit failure */
}

/* Individual register writers (placeholder conversions) */
#define WRITE_RAW(dev, reg, val) BQ76907_WriteRegister(dev, reg, (uint8_t)(val))

HAL_StatusTypeDef BQ76907_setPowerConfig(BQ76907 *dev, uint8_t v){ return WRITE_RAW(dev, BQ76907_REG_POWER_CONFIG, v); }
HAL_StatusTypeDef BQ76907_setDAConfig(BQ76907 *dev, uint8_t v){ return WRITE_RAW(dev, BQ76907_REG_DA_CONFIG, v); }
HAL_StatusTypeDef BQ76907_setRegoutConfig(BQ76907 *dev, uint8_t v){ return WRITE_RAW(dev, BQ76907_REG_REGOUT_CONFIG, v); }
HAL_StatusTypeDef BQ76907_setVcellMode(BQ76907 *dev, uint8_t cellCount){ return WRITE_RAW(dev, BQ76907_REG_VCELL_MODE, cellCount); }
HAL_StatusTypeDef BQ76907_setAlarmMaskDefault(BQ76907 *dev, uint8_t mask){ return WRITE_RAW(dev, BQ76907_REG_ALARM_MASK_DEFAULT, mask); }
HAL_StatusTypeDef BQ76907_setFETOptions(BQ76907 *dev, uint8_t opts){ return WRITE_RAW(dev, BQ76907_REG_FET_OPTIONS, opts); }
HAL_StatusTypeDef BQ76907_enableProtectionsA(BQ76907 *dev, uint8_t mask){ return WRITE_RAW(dev, BQ76907_REG_ENABLED_PROTECTIONS_A, mask); }
HAL_StatusTypeDef BQ76907_enableProtectionsB(BQ76907 *dev, uint8_t mask){ return WRITE_RAW(dev, BQ76907_REG_ENABLED_PROTECTIONS_B, mask); }
HAL_StatusTypeDef BQ76907_setDSGFetProtectionsA(BQ76907 *dev, uint8_t mask){ return WRITE_RAW(dev, BQ76907_REG_DSG_FET_PROTECTIONS_A, mask); }
HAL_StatusTypeDef BQ76907_setCHGFetProtectionsA(BQ76907 *dev, uint8_t mask){ return WRITE_RAW(dev, BQ76907_REG_CHG_FET_PROTECTIONS_A, mask); }
HAL_StatusTypeDef BQ76907_setLatchLimit(BQ76907 *dev, uint8_t v){ return WRITE_RAW(dev, BQ76907_REG_LATCH_LIMIT, v); }
HAL_StatusTypeDef BQ76907_setMaxInternalTemp(BQ76907 *dev, uint8_t degC){ return WRITE_RAW(dev, BQ76907_REG_MAX_INTERNAL_TEMP, degC); }
HAL_StatusTypeDef BQ76907_setCUVThreshold(BQ76907 *dev, uint16_t mV){ /* scale to raw TODO */ return WRITE_RAW(dev, BQ76907_REG_CUV_THRESHOLD, (mV/10)); }
HAL_StatusTypeDef BQ76907_setCOVThreshold(BQ76907 *dev, uint16_t mV){ return WRITE_RAW(dev, BQ76907_REG_COV_THRESHOLD, (mV/10)); }
HAL_StatusTypeDef BQ76907_setOCChargeThreshold(BQ76907 *dev, uint16_t mA){ return WRITE_RAW(dev, BQ76907_REG_OCD_CHG_THRESHOLD, (mA/10)); }
HAL_StatusTypeDef BQ76907_setOCDischarge1Threshold(BQ76907 *dev, uint16_t mA){ return WRITE_RAW(dev, BQ76907_REG_OCD_DISCH1_THRESHOLD, (mA/10)); }
HAL_StatusTypeDef BQ76907_setOCDischarge2Threshold(BQ76907 *dev, uint16_t mA){ return WRITE_RAW(dev, BQ76907_REG_OCD_DISCH2_THRESHOLD, (mA/10)); }
HAL_StatusTypeDef BQ76907_setInternalOTThreshold(BQ76907 *dev, uint8_t degC){ return WRITE_RAW(dev, BQ76907_REG_INT_OT_THRESHOLD, degC); }
HAL_StatusTypeDef BQ76907_setVoltageTime(BQ76907 *dev, uint8_t raw){ return WRITE_RAW(dev, BQ76907_REG_VOLTAGE_TIME, raw); }
HAL_StatusTypeDef BQ76907_setAlarmEnable(BQ76907 *dev, uint8_t mask){ return WRITE_RAW(dev, BQ76907_REG_ALARM_ENABLE, mask); }

HAL_StatusTypeDef BQ76907_readAlarmStatus(BQ76907 *dev, uint8_t *status){ return BQ76907_ReadRegister(dev, BQ76907_REG_ALARM_STATUS, status); }
HAL_StatusTypeDef BQ76907_clearAlarmStatus(BQ76907 *dev, uint8_t maskToClear){ /* write 1s to clear bits? placeholder */ return WRITE_RAW(dev, BQ76907_REG_ALARM_STATUS, maskToClear); }
HAL_StatusTypeDef BQ76907_readSafetyStatusA(BQ76907 *dev, uint8_t *status){ return BQ76907_ReadRegister(dev, BQ76907_REG_SAFETY_STATUS_A, status); }
HAL_StatusTypeDef BQ76907_readSafetyStatusB(BQ76907 *dev, uint8_t *status){ return BQ76907_ReadRegister(dev, BQ76907_REG_SAFETY_STATUS_B, status); }

HAL_StatusTypeDef BQ76907_fetEnable(BQ76907 *dev, uint8_t chargeEnable, uint8_t dischargeEnable){
    uint8_t opts = 0;
    if (chargeEnable)    opts |= BQ76907_FET_OPTIONS_CHG_FET_EN;
    if (dischargeEnable) opts |= BQ76907_FET_OPTIONS_DSG_FET_EN;
    return BQ76907_setFETOptions(dev, opts);
}

HAL_StatusTypeDef BQ76907_sleepEnable(BQ76907 *dev){
    /* If separate from enterSleep bit sequence, set dedicated bit */
    return BQ76907_setPowerConfig(dev, dev->activeConfig.powerConfig | BQ76907_POWER_CONFIG_SLEEP_EN);
}
HAL_StatusTypeDef BQ76907_sleepDisable(BQ76907 *dev){
    return BQ76907_setPowerConfig(dev, dev->activeConfig.powerConfig & ~BQ76907_POWER_CONFIG_SLEEP_EN);
}

HAL_StatusTypeDef BQ76907_readPASSQ(BQ76907 *dev, uint8_t *val){
    return BQ76907_ReadRegister(dev, BQ76907_REG_PASSQ, val);
}
HAL_StatusTypeDef BQ76907_setActiveBalancingMask(BQ76907 *dev, uint8_t cellMask){
    return BQ76907_WriteRegister(dev, BQ76907_REG_CB_ACTIVE_CELLS, cellMask); /* CB_ACTIVE_CELLS */
}
HAL_StatusTypeDef BQ76907_protectionRecovery(BQ76907 *dev, uint8_t mask){
    return BQ76907_WriteRegister(dev, BQ76907_REG_PROT_RECOVERY, mask);
}

/* ================= Balancing Evaluator ================= */
uint8_t BQ76907_evaluateAndBalance(BQ76907 *dev, uint16_t deltaStart_mV, uint16_t deltaStop_mV, uint16_t minCell_mV){
    /* Read latest cell voltages */
    if (BQ76907_readCellVoltages(dev) != HAL_OK) return 0xFF; /* error sentinel */

    uint16_t maxV = 0, minV = 0xFFFF;
    for (uint8_t i=0;i<4;i++){
        uint16_t v = dev->cellVoltage_mV[i];
        if (v < minV) minV = v;
        if (v > maxV) maxV = v;
    }
    uint16_t spread = maxV - minV;
    static uint8_t balancingActive = 0; /* simple hysteresis state */

    if (!balancingActive){
        if (spread >= deltaStart_mV && minV >= minCell_mV){
            balancingActive = 1;
        }
    } else {
        if (spread <= deltaStop_mV || minV < minCell_mV){
            balancingActive = 0;
        }
    }

    uint8_t mask = 0;
    if (balancingActive){
        /* Balance all cells whose voltage is within (maxV - deltaStop_mV/2) of maxV (aggressive) */
        uint16_t targetFloor = (deltaStop_mV/2 > spread) ? maxV : (maxV - (deltaStop_mV/2));
        for (uint8_t i=0;i<4;i++){
            if (dev->cellVoltage_mV[i] >= targetFloor && dev->cellVoltage_mV[i] > minCell_mV){
                mask |= (1u << i);
            }
        }
    }
    if (BQ76907_setActiveBalancingMask(dev, mask) != HAL_OK) return 0xFF;
    return mask;
}

/* ================= Protection Convenience Wrappers ================= */
HAL_StatusTypeDef BQ76907_configVoltageProtection(BQ76907 *dev, uint16_t uv_mV, uint16_t ov_mV){
    HAL_StatusTypeDef st = BQ76907_setCUVThreshold(dev, uv_mV); if (st!=HAL_OK) return st;
    return BQ76907_setCOVThreshold(dev, ov_mV);
}
HAL_StatusTypeDef BQ76907_configCurrentProtection(BQ76907 *dev, uint16_t ocChg_mA, uint16_t ocDis1_mA, uint16_t ocDis2_mA){
    HAL_StatusTypeDef st = BQ76907_setOCChargeThreshold(dev, ocChg_mA); if (st!=HAL_OK) return st;
    st = BQ76907_setOCDischarge1Threshold(dev, ocDis1_mA); if (st!=HAL_OK) return st;
    return BQ76907_setOCDischarge2Threshold(dev, ocDis2_mA);
}
HAL_StatusTypeDef BQ76907_configTemperatureProtection(BQ76907 *dev, uint8_t ot_C, uint8_t maxInternal_C){
    HAL_StatusTypeDef st = BQ76907_setInternalOTThreshold(dev, ot_C); if (st!=HAL_OK) return st;
    return BQ76907_setMaxInternalTemp(dev, maxInternal_C);
}

/* ================= Debug Dump ================= */
void BQ76907_debugDump(const BQ76907 *dev){
    if (!dev){ BQ_LOG("BQ76907: (null)"); return; }
    BQ_LOG("BQ76907 Dump: Pack=%u mV T=%d.%uC", (unsigned)dev->packVoltage_mV,
           dev->ts1_degC_x10/10, (unsigned)(dev->ts1_degC_x10%10));
    for (int i=0;i<4;i++) BQ_LOG("  Cell%u=%u mV", i+1, (unsigned)dev->cellVoltage_mV[i]);
    BQ_LOG("  Flags: OV=%u UV=%u OCD=%u SCD=%u OT=%u CC_RDY=%u DEV_RDY=%u",
           dev->status.ov_fault,
           dev->status.uv_fault,
           dev->status.ocd_fault,
           dev->status.scd_fault,
           dev->status.ot_fault,
           dev->status.cc_ready,
           dev->status.dev_ready);
}

/* ================= Periodic Status Logger ================= */
void BQ76907_logStatus(BQ76907 *dev){
    if (!dev) return;
    /* Throttle: only every 2000 ms */
    static uint32_t lastTick = 0;
    uint32_t now = HAL_GetTick();
    if ((now - lastTick) < 2000u) return;
    lastTick = now;

    /* Ensure latest measurements (non-fatal if any fail; flags may be stale) */
    (void)BQ76907_readSystemStatus(dev);
    (void)BQ76907_readPackVoltage(dev);
    (void)BQ76907_readCellVoltages(dev);
    (void)BQ76907_readTemperature1(dev);

    BQ_LOG("76907 STAT Pack=%umV Cells=%u,%u,%u,%u mV T=%d.%uC F:OV=%u UV=%u OCD=%u SCD=%u OT=%u CC=%u DEV=%u",
        (unsigned)dev->packVoltage_mV,
        (unsigned)dev->cellVoltage_mV[0], (unsigned)dev->cellVoltage_mV[1],
        (unsigned)dev->cellVoltage_mV[2], (unsigned)dev->cellVoltage_mV[3],
        dev->ts1_degC_x10/10, (unsigned)(dev->ts1_degC_x10%10),
        dev->status.ov_fault,
        dev->status.uv_fault,
        dev->status.ocd_fault,
        dev->status.scd_fault,
        dev->status.ot_fault,
        dev->status.cc_ready,
        dev->status.dev_ready);
}

/* ================= Config Logger ================= */
void BQ76907_logConfig(const BQ76907 *dev){
    if (!dev){ BQ_LOG("76907 CFG: (null)"); return; }
    const BQ76907_Config *c = &dev->activeConfig;
    BQ_LOG("76907 CFG cellCount=%u UV=%umV OV=%umV OCchg=%umA OCd1=%umA OCd2=%umA OT=%uC MaxInt=%uC balInt=%ums voltTimeRaw=0x%02X",
        c->cellCount, c->uvThreshold_mV, c->ovThreshold_mV, c->ocCharge_mA,
        c->ocDischarge1_mA, c->ocDischarge2_mA, c->internalOT_C, c->maxInternalTemp_C,
        c->balanceInterval_ms, c->voltageTimeUnits);
    BQ_LOG("76907 CFG fetOpt=0x%02X protA=0x%02X protB=0x%02X dsgA=0x%02X chgA=0x%02X latch=0x%02X alarmMaskDef=0x%02X alarmEn=0x%02X daCfg=0x%02X regout=0x%02X pwr=0x%02X",
        c->fetOptions, c->protectionsA, c->protectionsB, c->dsgFetProtA, c->chgFetProtA,
        c->latchLimit, c->alarmMaskDefault, c->alarmEnableMask, c->daConfig, c->regoutConfig, c->powerConfig);
}

/* ================= Error API ================= */
int8_t  BQ76907_getLastError(const BQ76907 *dev){ return dev ? dev->lastError : (int8_t)BM_ERR_UNKNOWN; }
uint8_t BQ76907_getErrorCount(const BQ76907 *dev){ return dev ? (dev->errorHead > BM_ERROR_LOG_DEPTH ? BM_ERROR_LOG_DEPTH : dev->errorHead) : 0; }
void    BQ76907_dumpErrors(const BQ76907 *dev){
    if (!dev){ BQ_LOG("BQ76907 errors: (null)"); return; }
    uint8_t count = (dev->errorHead > BM_ERROR_LOG_DEPTH)? BM_ERROR_LOG_DEPTH : dev->errorHead;
    BQ_LOG("BQ76907 Error Log (most recent %u):", count);
    for (uint8_t i=0;i<count;i++){
        const BM_ErrorEntry *e = &dev->errorLog[i];
        BQ_LOG("  t=%lu code=%d det=%u reg=0x%02X val=0x%04X", (unsigned long)e->tick, (int)e->code, e->detail, e->reg, e->value);
    }
}

