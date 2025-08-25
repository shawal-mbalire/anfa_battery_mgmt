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

	uint8_t raw=0, part=0, rev=0;
	int rc = BQ25798_confirmPart(device, &raw, &part, &rev);
	if (rc != BQ25798_OK) {
		return 255; /* legacy error code path; consider replacing with enum */
	}

	/* Configuration writes (placeholders; TODO: replace magic values with masks) */
	uint8_t reg;
	reg = 0xD1; /* Recharge control: 4S, 256ms deglitch, 100mV below VREG (verify decomposition) */
	BQ25798_WriteRegister(device, BQ25798_REG_RECHARGE_CTRL, &reg);
	reg = 0x10; /* Charger Ctrl 1: disable watchdog, host mode, I2C watchdog reset (verify bits) */
	BQ25798_WriteRegister(device, BQ25798_REG_CHARGER_CTRL_1, &reg);

	/* Voltage / current limit setters (values are placeholders; implement proper encoders) */
	uint8_t vsys_min = 0x70; /* 14500mV? verify scaling */
	BQ25798_WriteRegister(device, BQ25798_REG_MIN_SYS_VOLTAGE, &vsys_min);

	BQ25798_Write16(device, BQ25798_REG_CHARGE_VOLTAGE_LIMIT, 0x05B4); /* 14600mV raw? verify */
	BQ25798_Write16(device, BQ25798_REG_CHARGE_CURRENT_LIMIT, 0x03F4); /* 3650mA raw? verify */
	uint8_t vindpm = 0x24; /* 3600mV? verify scaling */
	BQ25798_WriteRegister(device, BQ25798_REG_INPUT_VOLTAGE_LIMIT, &vindpm);
	BQ25798_Write16(device, BQ25798_REG_INPUT_CURRENT_LIMIT, 0x014A); /* 3300mA raw? verify */

	uint8_t prechg = 0x03; /* precharge current config placeholder */
	BQ25798_WriteRegister(device, BQ25798_REG_PRECHARGE_CTRL, &prechg);

	reg = 0x8C; /* Charger Ctrl 0: enable charger etc. */
	BQ25798_WriteRegister(device, BQ25798_REG_CHARGER_CTRL_0, &reg);
	reg = 0x80; /* ADC control: all ADCs on */
	BQ25798_WriteRegister(device, BQ25798_REG_ADC_CTRL, &reg);
	return 0;
}

/* ================= Part Info & Helpers ================= */
void BQ25798_decodePartInfo(uint8_t raw, uint8_t *partNum, uint8_t *revNum){
	if (partNum) *partNum = (raw & BQ25798_PART_INFO_PART_MASK) >> BQ25798_PART_INFO_PART_SHIFT;
	if (revNum)  *revNum  = (raw & BQ25798_PART_INFO_REV_MASK);
}

int BQ25798_confirmPart(BQ25798 *device, uint8_t *rawVal, uint8_t *partNum, uint8_t *revNum){
	uint8_t v;
	if (BQ25798_ReadRegister(device, BQ25798_REG_PART_INFO, &v) != HAL_OK) return BQ25798_ERR_I2C;
	uint8_t p, r; BQ25798_decodePartInfo(v, &p, &r);
	if (rawVal)  *rawVal = v;
	if (partNum) *partNum = p;
	if (revNum)  *revNum = r;
	if (p != BQ25798_PART_NUM_VAL || r != BQ25798_DEV_REV_VAL) return BQ25798_ERR_ID_MISMATCH;
	return BQ25798_OK;
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
	return HAL_I2C_Mem_Read(
			device -> i2cHandle,
			BQ25798_I2C_ADDRESS,
			reg,
			I2C_MEMADD_SIZE_8BIT,
			data,
			1,
			HAL_MAX_DELAY
	);
}

HAL_StatusTypeDef BQ25798_ReadRegisters(BQ25798 *device, uint8_t reg, uint8_t *data, uint8_t length){
	return HAL_I2C_Mem_Read(
			device -> i2cHandle,
			BQ25798_I2C_ADDRESS,
			reg,
			I2C_MEMADD_SIZE_8BIT,
			data,
			length,
			HAL_MAX_DELAY
	);

}
HAL_StatusTypeDef BQ25798_WriteRegister(BQ25798 *device, uint8_t reg, uint8_t *data){
	return HAL_I2C_Mem_Write(
			device -> i2cHandle,
			BQ25798_I2C_ADDRESS,
			reg,
			I2C_MEMADD_SIZE_8BIT,
			data,
			1,
			HAL_MAX_DELAY
	);

}
