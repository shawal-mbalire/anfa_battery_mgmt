/*
 * bq25798.c
 *
 *  Created on: Aug 21, 2025
 *      Author: shawal
 */
#include "bq25798.h"

uint8_t  BQ25798_init(BQ25798 *device, I2C_HandleTypeDef *i2cHandle){

	// set struct params
	device -> i2cHandle   = i2cHandle;

	uint8_t errorsNumber = 0;
	HAL_StatusTypeDef status;

	// check device part number
	uint8_t registerData;
	status = BQ25798_ReadRegister(device, BQ25798_PART_INFO_REG_VALUE, &registerData);
	errorsNumber += (status != HAL_OK);
	if (registerData != BQ25798_PART_INFO_REG_VALUE) return 255;

	registerData = 0xD1; // cells 4s, Reacharge voltage deglitch 256ms, recharge voltage 100mV below VREG
	BQ25798_WriteRegister(device,BQ25798_REG_RECHARGE_CTRL, &registerData);
	registerData = 0x10; // disable watchdog, enable host mode, i2c wathdog reset on
	BQ25798_WriteRegister(device, BQ25798_REG_CHARGER_CTRL_1, &registerData);

    // set limits Implemented at POR power on reset
	BQ25798_WriteRegister(device, BQ25798_REG_MIN_SYS_VOLTAGE, (uint8_t[]){0x70}); // 14500mV
	BQ25798_WriteRegister(device, BQ25798_REG_CHARGE_VOLTAGE_LIMIT, (uint8_t[]){0x05B4}); // 14600mV
	BQ25798_WriteRegister(device, BQ25798_REG_CHARGE_CURRENT_LIMIT, (uint8_t[]){0x03F4}); // 3650mA
	BQ25798_WriteRegister(device, BQ25798_REG_INPUT_VOLTAGE_LIMIT, (uint8_t[]){0x24}); // 3600mV
	BQ25798_WriteRegister(device, BQ25798_REG_INPUT_CURRENT_LIMIT, (uint8_t[]){0x14A}); //3300mA

    // TODO: configure ADCs and enable other things
	// all ADCs on by default
	// precharge current 120mA
	BQ25798_WriteRegister(device, BQ25798_REG_PRECHARGE_CTRL, (uint8_t[]){0x03});
	// charger control 0 ICO and HiZ on
	registerData = 0x8C; // enable charger, HIZ off, IIN
	BQ25798_WriteRegister(device, BQ25798_REG_CHARGER_CTRL_0, &registerData);
	// ADC control
	registerData = 0x80; // all ADCs on
	BQ25798_WriteRegister(device, BQ25798_REG_ADC_CTRL, &registerData);


	return errorsNumber;
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
