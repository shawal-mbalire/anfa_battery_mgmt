/*
 * bq25798.c
 *
 *  Created on: Aug 21, 2025
 *      Author: shawal
 */
#include "bq25798.h"
#include "stm32g0xx_hal.h" /* Ensure HAL declarations visible here */

uint8_t  BQ25798_init(BQ25798 *device, I2C_HandleTypeDef *i2cHandle){
	device->i2cHandle = i2cHandle;

	BQ25798_PartInfo idInfo;
	BQ25798_Result idRes = BQ25798_confirmPart(device, &idInfo);
	if (idRes != BQ25798_OK) {
		return 255; /* legacy error code path; consider replacing with enum everywhere */
	}

	/* Configuration writes (placeholders; TODO: replace magic values with masks) */
	uint8_t reg;
	reg = 0xD1; /* Recharge control: 4S, 256ms deglitch, 100mV below VREG (verify decomposition) */
	BQ25798_WriteRegister(device, BQ25798_REG_RECHARGE_CTRL, &reg);
	reg = 0x10; /* Charger Ctrl 1: disable watchdog, host mode, I2C watchdog reset (verify bits) */
	BQ25798_WriteRegister(device, BQ25798_REG_CHARGER_CTRL_1, &reg);

	/* Voltage / current limit setters using scaling helpers */
	/* VSYSMIN left as previously set (complex mapping TBD). Keep existing raw 0x70 placeholder. */
	uint8_t vsys_min = 0x70; /* TODO_VERIFY: derive proper encode for VSYSMIN */
	BQ25798_WriteRegister(device, BQ25798_REG_MIN_SYS_VOLTAGE, &vsys_min);

	/* Charge voltage: 14600 mV -> raw 1460 (0x05B4) */
	uint16_t vreg_raw = BQ25798_encodeChargeVoltage_mV(14600);
	BQ25798_Write16(device, BQ25798_REG_CHARGE_VOLTAGE_LIMIT, vreg_raw);

	/* Charge current: choose 5000 mA (raw 500 = 0x01F4); correcting earlier probable typo 0x03F4 */
	uint16_t ichg_raw = BQ25798_encodeChargeCurrent_mA(5000);
	BQ25798_Write16(device, BQ25798_REG_CHARGE_CURRENT_LIMIT, ichg_raw);

	/* Input voltage limit: 3600 mV -> 0x24 */
	uint8_t vindpm = BQ25798_encodeInputVoltageLimit_mV(3600);
	BQ25798_WriteRegister(device, BQ25798_REG_INPUT_VOLTAGE_LIMIT, &vindpm);

	/* Input current limit: 3300 mA -> raw 330 (0x014A) */
	uint16_t iin_raw = BQ25798_encodeInputCurrent_mA(3300);
	BQ25798_Write16(device, BQ25798_REG_INPUT_CURRENT_LIMIT, iin_raw);

	uint8_t prechg = 0x03; /* precharge current config placeholder */
	BQ25798_WriteRegister(device, BQ25798_REG_PRECHARGE_CTRL, &prechg);

	reg = 0x8C; /* Charger Ctrl 0: enable charger etc. */
	BQ25798_WriteRegister(device, BQ25798_REG_CHARGER_CTRL_0, &reg);
	reg = 0x80; /* ADC control: all ADCs on */
	BQ25798_WriteRegister(device, BQ25798_REG_ADC_CTRL, &reg);
	return 0;
}

/* ================= Part Info & Helpers ================= */
void BQ25798_decodePartInfo(uint8_t raw, BQ25798_PartInfo *out){
	if (!out) return;
	out->raw      = raw;
	out->part3bit = (raw & BQ25798_PART_INFO_PART_MASK)  >> BQ25798_PART_INFO_PART_SHIFT; /* legacy 3-bit */
	out->part5bit = (raw & BQ25798_PART_INFO_PART5_MASK) >> BQ25798_PART_INFO_PART5_SHIFT; /* proposed full */
	out->rev      = (raw & BQ25798_PART_INFO_REV_MASK);
}

BQ25798_Result BQ25798_confirmPart(BQ25798 *device, BQ25798_PartInfo *infoOut){
	uint8_t v;
	if (BQ25798_ReadRegister(device, BQ25798_REG_PART_INFO, &v) != HAL_OK) return BQ25798_ERR_I2C;
	BQ25798_PartInfo tmp; BQ25798_decodePartInfo(v, &tmp);
	/* Validation strategy: accept if either legacy 3-bit matches AND rev matches, OR 5-bit matches (once verified). */
	int legacy_ok = (tmp.part3bit == BQ25798_PART_NUM_VAL) && (tmp.rev == BQ25798_DEV_REV_VAL);
	int full_ok   = (tmp.part5bit == BQ25798_EXPECTED_PARTNUM_5BIT) && (tmp.rev == BQ25798_EXPECTED_REVISION);
	if (!(legacy_ok || full_ok)) {
		if (infoOut) *infoOut = tmp; /* still return captured info for diagnostics */
		return BQ25798_ERR_ID_MISMATCH;
	}
	if (infoOut) *infoOut = tmp;
	return BQ25798_OK;
}

/* Legacy wrapper */
int BQ25798_confirmPart_Legacy(BQ25798 *device, uint8_t *rawVal, uint8_t *partNum, uint8_t *revNum){
	BQ25798_PartInfo info; BQ25798_Result r = BQ25798_confirmPart(device, &info);
	if (rawVal)  *rawVal  = info.raw;
	if (partNum) *partNum = info.part3bit; /* preserve previous behavior */
	if (revNum)  *revNum  = info.rev;
	return r; /* relies on enum being compatible with previous int usage (negative on error) */
}

/* ================= 16-bit Access Helpers ================= */
HAL_StatusTypeDef BQ25798_Write16(BQ25798 *device, uint8_t msbReg, uint16_t value){
	uint8_t buf[2] = { (uint8_t)(value >> 8), (uint8_t)(value & 0xFF) };
	return HAL_I2C_Mem_Write(device->i2cHandle, BQ25798_I2C_ADDRESS, msbReg, I2C_MEMADD_SIZE_8BIT, buf, 2, HAL_MAX_DELAY);
}
HAL_StatusTypeDef BQ25798_Read16(BQ25798 *device, uint8_t msbReg, uint16_t *value){
	uint8_t buf[2];
	HAL_StatusTypeDef st = HAL_I2C_Mem_Read(device->i2cHandle, BQ25798_I2C_ADDRESS, msbReg, I2C_MEMADD_SIZE_8BIT, buf, 2, HAL_MAX_DELAY);
	if (st == HAL_OK && value){ *value = (uint16_t)(buf[0] << 8 | buf[1]); }
	return st;
}

// Read and interpret REG1B_Charger_Status_0
HAL_StatusTypeDef readChargerStatus0(BQ25798 *device, uint8_t *status){
    HAL_StatusTypeDef ret_val = BQ25798_ReadRegister(device, BQ25798_REG_CHARGER_STATUS_0, status);
    if (ret_val == HAL_OK) {
        device->chargerStatus0.iindpm_stat       = (*status >> 7) & 0x01;
        device->chargerStatus0.vindpm_stat       = (*status >> 6) & 0x01;
        device->chargerStatus0.wd_stat           = (*status >> 5) & 0x01;
        device->chargerStatus0.pg_stat           = (*status >> 3) & 0x01;
        device->chargerStatus0.ac2_present_stat  = (*status >> 2) & 0x01;
        device->chargerStatus0.ac1_present_stat  = (*status >> 1) & 0x01;
        device->chargerStatus0.vbus_present_stat = (*status >> 0) & 0x01;
    }
    return ret_val;
}

// Read and interpret REG1C_Charger_Status_1
HAL_StatusTypeDef readChargerStatus1(BQ25798 *device, uint8_t *status){
    HAL_StatusTypeDef ret_val = BQ25798_ReadRegister(device, BQ25798_REG_CHARGER_STATUS_1, status);
    if (ret_val == HAL_OK) {
        device->chargerStatus1.chg_stat        = (*status >> 5) & 0x07;
        device->chargerStatus1.vbus_stat       = (*status >> 1) & 0x0F;
        device->chargerStatus1.bc12_done_stat  = (*status >> 0) & 0x01;
    }
    return ret_val;
}

// Read and interpret REG1D_Charger_Status_2
HAL_StatusTypeDef readChargerStatus2(BQ25798 *device, uint8_t *status){
    HAL_StatusTypeDef ret_val = BQ25798_ReadRegister(device, BQ25798_REG_CHARGER_STATUS_2, status);
    if (ret_val == HAL_OK) {
        device->chargerStatus2.ico_stat          = (*status >> 6) & 0x03;
        device->chargerStatus2.treg_stat         = (*status >> 2) & 0x01;
        device->chargerStatus2.dpdm_stat         = (*status >> 1) & 0x01;
        device->chargerStatus2.vbat_present_stat = (*status >> 0) & 0x01;
    }
    return ret_val;
}

// Read and interpret REG1E_Charger_Status_3
HAL_StatusTypeDef readChargerStatus3(BQ25798 *device, uint8_t *status){
    HAL_StatusTypeDef ret_val = BQ25798_ReadRegister(device, BQ25798_REG_CHARGER_STATUS_3, status);
    if (ret_val == HAL_OK) {
        device->chargerStatus3.acrb2_stat      = (*status >> 7) & 0x01;
        device->chargerStatus3.acrb1_stat      = (*status >> 6) & 0x01;
        device->chargerStatus3.adc_done_stat   = (*status >> 5) & 0x01;
        device->chargerStatus3.vsys_stat       = (*status >> 4) & 0x01;
        device->chargerStatus3.chg_tmr_stat    = (*status >> 3) & 0x01;
        device->chargerStatus3.trichg_tmr_stat = (*status >> 2) & 0x01;
        device->chargerStatus3.prechg_tmr_stat = (*status >> 1) & 0x01;
    }
    return ret_val;
}

// Read and interpret REG1F_Charger_Status_4
HAL_StatusTypeDef readChargerStatus4(BQ25798 *device, uint8_t *status){
    HAL_StatusTypeDef ret_val = BQ25798_ReadRegister(device, BQ25798_REG_CHARGER_STATUS_4, status);
    if (ret_val == HAL_OK) {
        device->chargerStatus4.vbatotg_low_stat = (*status >> 4) & 0x01;
        device->chargerStatus4.ts_cold_stat     = (*status >> 3) & 0x01;
        device->chargerStatus4.ts_cool_stat     = (*status >> 2) & 0x01;
        device->chargerStatus4.ts_warm_stat     = (*status >> 1) & 0x01;
        device->chargerStatus4.ts_hot_stat      = (*status >> 0) & 0x01;
    }
    return ret_val;
}

// Read and interpret REG20_FAULT_Status_0
HAL_StatusTypeDef readFaultStatus0(BQ25798 *device, uint8_t *status){
    HAL_StatusTypeDef ret_val = BQ25798_ReadRegister(device, BQ25798_REG_FAULT_STATUS_0, status);
    if (ret_val == HAL_OK) {
        device->faultStatus0.ibat_reg_stat  = (*status >> 7) & 0x01;
        device->faultStatus0.vbus_ovp_stat  = (*status >> 6) & 0x01;
        device->faultStatus0.vbat_ovp_stat  = (*status >> 5) & 0x01;
        device->faultStatus0.ibus_ocp_stat  = (*status >> 4) & 0x01;
        device->faultStatus0.ibat_ocp_stat  = (*status >> 3) & 0x01;
        device->faultStatus0.conv_ocp_stat  = (*status >> 2) & 0x01;
        device->faultStatus0.vac2_ovp_stat  = (*status >> 1) & 0x01;
        device->faultStatus0.vac1_ovp_stat  = (*status >> 0) & 0x01;
    }
    return ret_val;
}

// Read and interpret REG21_FAULT_Status_1
HAL_StatusTypeDef readFaultStatus1(BQ25798 *device, uint8_t *status){
    HAL_StatusTypeDef ret_val = BQ25798_ReadRegister(device, BQ25798_REG_FAULT_STATUS_1, status);
    if (ret_val == HAL_OK) {
        device->faultStatus1.vsys_short_stat = (*status >> 7) & 0x01;
        device->faultStatus1.vsys_ovp_stat   = (*status >> 6) & 0x01;
        device->faultStatus1.otg_ovp_stat    = (*status >> 5) & 0x01;
        device->faultStatus1.otg_uvp_stat    = (*status >> 4) & 0x01;
        device->faultStatus1.tshut_stat      = (*status >> 2) & 0x01;
    }
    return ret_val;
}

// Read input voltage and current
HAL_StatusTypeDef BQ25798_readBusVoltage(BQ25798 *device){
	// Read raw values from voltage registers (16 bits)
	// we have a 16 bit ADC but 8bit RAM thus we read 2 addresses instead
	uint8_t registerData[2];
	HAL_StatusTypeDef status = BQ25798_ReadRegisters(device, BQ25798_REG_VBUS_ADC, registerData, 2); // bit resolution 1mV
	uint16_t voltageRaw = (registerData[0] << 8) | registerData[1];
	device->voltageBus = voltageRaw; // in mV
	return status;
}
HAL_StatusTypeDef BQ25798_readBusCurrent(BQ25798 *device){
	// Read raw values from voltage registers (16 bits)
	// we have a 16 bit ADC but 8bit RAM thus we read 2 addresses instead
	uint8_t registerData[2];
	HAL_StatusTypeDef status = BQ25798_ReadRegisters(device, BQ25798_REG_IBUS_ADC, registerData, 2); // bit resolution 1mV
	uint16_t currentRaw = (registerData[0] << 8) | registerData[1];
	device->currentBus = currentRaw; // in mA
	return status;
}
// Read output/ battery voltage and current

HAL_StatusTypeDef BQ25798_readBatteryVoltage(BQ25798 *device){
	// Read raw values from voltage registers (16 bits)
	// we have a 16 bit ADC but 8bit RAM thus we read 2 addresses instead
	uint8_t registerData[2];
	HAL_StatusTypeDef status = BQ25798_ReadRegisters(device, BQ25798_REG_VBAT_ADC, registerData, 2); // bit resolution 1mV
	uint16_t voltageRaw = (registerData[0] << 8) | registerData[1];
	device->voltageBattery = voltageRaw; // in mV
	return status;
}
HAL_StatusTypeDef BQ25798_readBatteryCurrent(BQ25798 *device){
	// Read raw values from voltage registers (16 bits)
	// we have a 16 bit ADC but 8bit RAM thus we read 2 addresses instead
	uint8_t registerData[2];
	HAL_StatusTypeDef status = BQ25798_ReadRegisters(device, BQ25798_REG_IBAT_ADC, registerData, 2); // bit resolution 1mA
	uint16_t currentRaw = (registerData[0] << 8) | registerData[1];
	device->currentBattery = currentRaw; // in mA
	return status;
}

// LOW LEVEL FUNCTIONS

HAL_StatusTypeDef BQ25798_ReadRegister(BQ25798 *device, uint8_t reg, uint8_t *data){
	HAL_StatusTypeDef st = HAL_I2C_Mem_Read(device->i2cHandle, BQ25798_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
	if (st != HAL_OK){
		BM_PUSH_ERROR(device, BM_SRC_BQ25798, BM_ERR_I2C, (uint8_t)st, reg, 0);
	}
	return st;
}

HAL_StatusTypeDef BQ25798_ReadRegisters(BQ25798 *device, uint8_t reg, uint8_t *data, uint8_t length){
	HAL_StatusTypeDef st = HAL_I2C_Mem_Read(device->i2cHandle, BQ25798_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
	if (st != HAL_OK){
		BM_PUSH_ERROR(device, BM_SRC_BQ25798, BM_ERR_I2C, (uint8_t)st, reg, 0);
	}
	return st;

}
HAL_StatusTypeDef BQ25798_WriteRegister(BQ25798 *device, uint8_t reg, uint8_t *data){
	HAL_StatusTypeDef st = HAL_I2C_Mem_Write(device->i2cHandle, BQ25798_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
	if (st != HAL_OK){
		BM_PUSH_ERROR(device, BM_SRC_BQ25798, BM_ERR_I2C, (uint8_t)st, reg, *data);
	}
	return st;

}

/* ================= High-Level Control / Profile Functions ================= */
HAL_StatusTypeDef BQ25798_setChargeVoltage(BQ25798 *dev, uint16_t mV){
    uint16_t raw = BQ25798_encodeChargeVoltage_mV(mV);
    return BQ25798_Write16(dev, BQ25798_REG_CHARGE_VOLTAGE_LIMIT, raw);
}
HAL_StatusTypeDef BQ25798_setChargeCurrent(BQ25798 *dev, uint16_t mA){
    uint16_t raw = BQ25798_encodeChargeCurrent_mA(mA);
    return BQ25798_Write16(dev, BQ25798_REG_CHARGE_CURRENT_LIMIT, raw);
}
HAL_StatusTypeDef BQ25798_setInputCurrentLimit(BQ25798 *dev, uint16_t mA){
    uint16_t raw = BQ25798_encodeInputCurrent_mA(mA);
    return BQ25798_Write16(dev, BQ25798_REG_INPUT_CURRENT_LIMIT, raw);
}
HAL_StatusTypeDef BQ25798_chargerEnable(BQ25798 *dev, uint8_t enable){
	uint8_t reg;
	if (BQ25798_ReadRegister(dev, BQ25798_REG_CHARGER_CTRL_0, &reg) != HAL_OK) return HAL_ERROR;
	if (enable) reg |= BQ25798_CHG_CTRL0_CHG_EN; else reg &= ~BQ25798_CHG_CTRL0_CHG_EN;
	return BQ25798_WriteRegister(dev, BQ25798_REG_CHARGER_CTRL_0, &reg);
}
/* Charge profile logic (placeholder thresholds):
 * - VBAT < 3000 mV: pre-charge 100 mA
 * - 3000 mV <= VBAT < 3650 mV: fast charge 5000 mA
 * - VBAT >= 3650 mV: reduce current (taper) to 1000 mA
 */
HAL_StatusTypeDef BQ25798_updateChargeProfile(BQ25798 *dev){
	if (BQ25798_readBatteryVoltage(dev) != HAL_OK) return HAL_ERROR;
	uint16_t vbatt = dev->voltageBattery; /* mV */
	uint16_t targetCurrent_mA;
	if (vbatt < 3000) targetCurrent_mA = 100;        /* pre-charge */
	else if (vbatt < 3650) targetCurrent_mA = 5000;  /* fast charge */
	else targetCurrent_mA = 1000;                    /* taper */
	return BQ25798_setChargeCurrent(dev, targetCurrent_mA);
}
HAL_StatusTypeDef BQ25798_readDieTemperature(BQ25798 *dev, int16_t *tempC_x10){
	uint8_t buf[2];
	if (BQ25798_ReadRegisters(dev, BQ25798_REG_TDIE_ADC, buf, 2) != HAL_OK) return HAL_ERROR;
	uint16_t raw = (buf[0] << 8) | buf[1];
	/* Placeholder: assume 1 LSB = 0.1C */
	if (tempC_x10) *tempC_x10 = (int16_t)raw; /* TODO: scale */
	return HAL_OK;
}
HAL_StatusTypeDef BQ25798_thermalGuard(BQ25798 *dev, int16_t tempC_x10_min, int16_t tempC_x10_max){
	int16_t t;
	if (BQ25798_readDieTemperature(dev, &t) != HAL_OK) return HAL_ERROR;
	if (t < tempC_x10_min || t > tempC_x10_max){
		return BQ25798_chargerEnable(dev, 0);
	} else {
		return BQ25798_chargerEnable(dev, 1);
	}
}

/* ================= Debug Dump ================= */
void BQ25798_debugDump(const BQ25798 *dev){
    if (!dev){ BQ_LOG("BQ25798: (null device)"); return; }
    BQ_LOG("BQ25798 Debug Dump:");
    BQ_LOG("  VBUS  = %u mV",   (unsigned)dev->voltageBus);
    BQ_LOG("  IBUS  = %u mA",   (unsigned)dev->currentBus);
    BQ_LOG("  VBAT  = %u mV",   (unsigned)dev->voltageBattery);
    BQ_LOG("  IBAT  = %u mA",   (unsigned)dev->currentBattery);
    BQ_LOG("  CHG_STAT0: iindpm=%u vindpm=%u wd=%u pg=%u ac2=%u ac1=%u vbus=%u",
	    dev->chargerStatus0.iindpm_stat,
	    dev->chargerStatus0.vindpm_stat,
	    dev->chargerStatus0.wd_stat,
	    dev->chargerStatus0.pg_stat,
	    dev->chargerStatus0.ac2_present_stat,
	    dev->chargerStatus0.ac1_present_stat,
	    dev->chargerStatus0.vbus_present_stat);
    BQ_LOG("  CHG_STAT1: chg=%u vbus=%u bc12=%u",
	    dev->chargerStatus1.chg_stat,
	    dev->chargerStatus1.vbus_stat,
	    dev->chargerStatus1.bc12_done_stat);
    BQ_LOG("  FAULT0: ibat_reg=%u vbus_ovp=%u vbat_ovp=%u ibus_ocp=%u ibat_ocp=%u conv_ocp=%u vac2_ovp=%u vac1_ovp=%u",
	    dev->faultStatus0.ibat_reg_stat,
	    dev->faultStatus0.vbus_ovp_stat,
	    dev->faultStatus0.vbat_ovp_stat,
	    dev->faultStatus0.ibus_ocp_stat,
	    dev->faultStatus0.ibat_ocp_stat,
	    dev->faultStatus0.conv_ocp_stat,
	    dev->faultStatus0.vac2_ovp_stat,
	    dev->faultStatus0.vac1_ovp_stat);
}

/* ================= Error API ================= */
int8_t BQ25798_getLastError(const BQ25798 *dev){ return dev ? dev->lastError : (int8_t)BM_ERR_UNKNOWN; }
uint8_t BQ25798_getErrorCount(const BQ25798 *dev){ return dev ? (dev->errorHead > BM_ERROR_LOG_DEPTH ? BM_ERROR_LOG_DEPTH : dev->errorHead) : 0; }
void BQ25798_dumpErrors(const BQ25798 *dev){
	if (!dev){ BQ_LOG("BQ25798 errors: (null)"); return; }
	uint8_t count = (dev->errorHead > BM_ERROR_LOG_DEPTH)? BM_ERROR_LOG_DEPTH : dev->errorHead;
	BQ_LOG("BQ25798 Error Log (most recent %u):", count);
	for (uint8_t i=0;i<count;i++){
		const BM_ErrorEntry *e = &dev->errorLog[i];
		BQ_LOG("  t=%lu code=%d det=%u reg=0x%02X val=0x%04X", (unsigned long)e->tick, (int)e->code, e->detail, e->reg, e->value);
	}
}
