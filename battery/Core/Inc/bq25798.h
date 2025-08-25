/*
 * bq25798.h
 *
 *  Created on: Aug 21, 2025
 *      Author: Shawal Mbalire
 */

#ifndef INC_BQ25798_H_
#define INC_BQ25798_H_

#include "stm32g0xx_hal.h" /* needed for I2C */
#include <stdint.h>

/* --- I2C Configuration --- */
#define BQ25798_I2C_ADDRESS_7BIT           (0x6B) // page 53
#define BQ25798_I2C_ADDRESS                (0x6B << 1) // 7 bit  left shifted by one to include the R/W bit.
#define BQ25798_PART_INFO_REG_VALUE        ( 0x31 ) // 0011 0001b, combining Part Number (011b) and Device Revision (001b)
#define BQ25798_PART_NUM_VAL               ( 0x3  ) // 011b, for BQ25798 Part Number
#define BQ25798_DEV_REV_VAL                ( 0x1  ) // 001b, for BQ25798 Device Revision

/* --- Register Addresses and Descriptions (Control Registers) --- */
#define BQ25798_REG_MIN_SYS_VOLTAGE        (0x00)  /**< Minimal System Voltage (VSYSMIN) register. POR value is tied to PROG pin. */
#define BQ25798_REG_CHARGE_VOLTAGE_LIMIT   (0x01)  /**< Charge Voltage Limit (VREG) register. Sets battery charge regulation voltage. */
#define BQ25798_REG_CHARGE_CURRENT_LIMIT   (0x03)  /**< Charge Current Limit (IREG) register. Sets battery charge current limit. */
#define BQ25798_REG_INPUT_VOLTAGE_LIMIT    (0x05)  /**< Input Voltage Limit (VINDPM) register. Sets minimum acceptable input voltage. */
#define BQ25798_REG_INPUT_CURRENT_LIMIT    (0x06)  /**< Input Current Limit register. Sets absolute input current limit. */
#define BQ25798_REG_PRECHARGE_CTRL         (0x08)  /**< Precharge Control register. Controls precharge to fast charge transition. */
#define BQ25798_REG_TERMINATION_CTRL       (0x09)  /**< Termination Control register. Controls charger termination and reset. */
#define BQ25798_REG_RECHARGE_CTRL          (0x0A)  /**< Re-charge Control register. Controls battery recharge offset and deglitch time. */
#define BQ25798_REG_VOTG_REGULATION        (0x0B)  /**< VOTG Regulation register. Sets the OTG mode regulation voltage. */
#define BQ25798_REG_IOTG_REGULATION        (0x0D)  /**< IOTG Regulation register. Sets pre-charge safety timer and OTG current limit. */
#define BQ25798_REG_TIMER_CTRL             (0x0E)  /**< Timer Control register. Controls various charging timers. */
#define BQ25798_REG_CHARGER_CTRL_0         (0x0F)  /**< Charger Control 0 register. Controls charger enable, HIZ mode, etc. */
#define BQ25798_REG_CHARGER_CTRL_1         (0x10)  /**< Charger Control 1 register. Controls watchdog timer, VINDPM backup, etc. */
#define BQ25798_REG_CHARGER_CTRL_2         (0x11)  /**< Charger Control 2 register. Controls USB D+/D- detection and HV DCP modes. */
#define BQ25798_REG_CHARGER_CTRL_3         (0x12)  /**< Charger Control 3 register. Controls OTG mode, PFM, and other functions. */
#define BQ25798_REG_CHARGER_CTRL_4         (0x13)  /**< Charger Control 4 register. Controls ACFET/RBFET gates and protection features. */
#define BQ25798_REG_CHARGER_CTRL_5         (0x14)  /**< Charger Control 5 register. Controls ship FET, IBAT sensing, and current regulation. */
#define BQ25798_REG_MPPT_CTRL              (0x15)  /**< MPPT Control register. Controls Maximum Power Point Tracking features. */
#define BQ25798_REG_TEMP_CTRL              (0x16)  /**< Temperature Control register. Sets thermal regulation and shutdown thresholds. */
#define BQ25798_REG_NTC_CTRL_0             (0x17)  /**< NTC Control 0 register. Sets JEITA charge voltage/current based on temperature. */
#define BQ25798_REG_NTC_CTRL_1             (0x18)  /**< NTC Control 1 register. Sets JEITA comparator and OTG temperature thresholds. */

/* --- Register Addresses and Descriptions (Read Only) --- */
#define BQ25798_REG_ICO_CURRENT_LIMIT      (0x19)  /**< Read-only register for ICO or ILIM_HIZ current limit. */
#define BQ25798_REG_CHARGER_STATUS_0       (0x1B)  /**< Read-only register for charger status flags. */
#define BQ25798_REG_CHARGER_STATUS_1       (0x1C)  /**< Read-only register for charge, VBUS, and BC1 statuses. */
#define BQ25798_REG_CHARGER_STATUS_2       (0x1D)  /**< Read-only register for ICO, thermal regulation, D+/D- detection, and battery presence. */
#define BQ25798_REG_CHARGER_STATUS_3       (0x1E)  /**< Read-only register for FET, ADC, and timer statuses. */
#define BQ25798_REG_CHARGER_STATUS_4       (0x1F)  /**< Read-only register for battery voltage and TS temperature range status. */
#define BQ25798_REG_FAULT_STATUS_0         (0x20)  /**< Read-only register for various fault statuses. */
#define BQ25798_REG_FAULT_STATUS_1         (0x21)  /**< Read-only register for various fault statuses. */
#define BQ25798_REG_CHARGER_FLAG_0         (0x22)  /**< Read-only register for charger flags. */
#define BQ25798_REG_CHARGER_FLAG_1         (0x23)  /**< Read-only register for charger flags. */
#define BQ25798_REG_CHARGER_FLAG_2         (0x24)  /**< Read-only register for charger flags. */
#define BQ25798_REG_CHARGER_FLAG_3         (0x25)  /**< Read-only register for charger flags. */
#define BQ25798_REG_FAULT_FLAG_0           (0x26)  /**< Read-only register for various fault flags. */
#define BQ25798_REG_FAULT_FLAG_1           (0x27)  /**< Read-only register for various fault flags. */
#define BQ25798_REG_CHARGER_MASK_0         (0x28)  /**< Charger Mask 0 register. Masks charger interrupts. */
#define BQ25798_REG_CHARGER_MASK_1         (0x29)  /**< Charger Mask 1 register. Masks charger interrupts. */
#define BQ25798_REG_CHARGER_MASK_2         (0x2A)  /**< Charger Mask 2 register. Masks charger interrupts. */
#define BQ25798_REG_CHARGER_MASK_3         (0x2B)  /**< Charger Mask 3 register. Masks charger interrupts. */
#define BQ25798_REG_FAULT_MASK_0           (0x2C)  /**< FAULT Mask 0 register. Masks fault interrupts. */
#define BQ25798_REG_FAULT_MASK_1           (0x2D)  /**< FAULT Mask 1 register. Masks fault interrupts. */
#define BQ25798_REG_ADC_CTRL               (0x2E)  /**< ADC Control register. Controls ADC conversion rate and sample speed. */
#define BQ25798_REG_ADC_FUNC_DISABLE_0     (0x2F)  /**< ADC Function Disable 0 register. Enables/disables various ADCs. */
#define BQ25798_REG_ADC_FUNC_DISABLE_1     (0x30)  /**< ADC Function Disable 1 register. Enables/disables D+, D-, VAC1, VAC2 ADCs. */
#define BQ25798_REG_IBUS_ADC               (0x31)  /**< Read-only register for bus current ADC reading. */
#define BQ25798_REG_IBAT_ADC               (0x33)  /**< Read-only register for battery current ADC reading. */
#define BQ25798_REG_VBUS_ADC               (0x35)  /**< Read-only register for VBUS voltage ADC reading. */
#define BQ25798_REG_VAC1_ADC               (0x37)  /**< Read-only register for VAC1 voltage ADC reading. */
#define BQ25798_REG_VAC2_ADC               (0x39)  /**< Read-only register for VAC2 voltage ADC reading. */
#define BQ25798_REG_VBAT_ADC               (0x3B)  /**< Read-only register for battery voltage ADC reading. */
#define BQ25798_REG_VSYS_ADC               (0x3D)  /**< Read-only register for system voltage ADC reading. */
#define BQ25798_REG_TS_ADC                 (0x3F)  /**< Read-only register for NTC thermistor ADC reading. */
#define BQ25798_REG_TDIE_ADC               (0x41)  /**< Read-only register for die temperature ADC reading. */
#define BQ25798_REG_DP_ADC                 (0x43)  /**< Read-only register for D+ ADC reading. */
#define BQ25798_REG_DM_ADC                 (0x45)  /**< Read-only register for D- ADC reading. */
#define BQ25798_REG_DPDM_DRIVER            (0x47)  /**< Read-only register for D+/D- voltage control. */
#define BQ25798_REG_PART_INFO              (0x48)  /**< Read-only register for device part number and revision. */

// Structs
#include <stdint.h>

// REG1B_Charger_Status_0 Register (Offset = 1Bh)
typedef struct {
    uint8_t iindpm_stat;       // Bit 7: IINDPM status
    uint8_t vindpm_stat;       // Bit 6: VINDPM status
    uint8_t wd_stat;           // Bit 5: Watchdog timer status
    uint8_t pg_stat;           // Bit 3: Power Good Status
    uint8_t ac2_present_stat;  // Bit 2: VAC2 present status
    uint8_t ac1_present_stat;  // Bit 1: VAC1 present status
    uint8_t vbus_present_stat; // Bit 0: VBUS present status
} BQ25798_ChargerStatus0;
#include <stdint.h>

// REG1C_Charger_Status_1 Register (Offset = 1Ch)
typedef struct {
    uint8_t chg_stat;         // Bits 7-5: Charge Status
    uint8_t vbus_stat;        // Bits 4-1: VBUS Status
    uint8_t bc12_done_stat;   // Bit 0: BC1.2 status
} BQ25798_ChargerStatus1;
// REG1D_Charger_Status_2 Register
typedef struct {
    uint8_t ico_stat;          // Bits 7-6
    uint8_t treg_stat;         // Bit 2
    uint8_t dpdm_stat;         // Bit 1
    uint8_t vbat_present_stat; // Bit 0
} BQ25798_ChargerStatus2;

// REG1E_Charger_Status_3 Register
typedef struct {
    uint8_t acrb2_stat;      // Bit 7
    uint8_t acrb1_stat;      // Bit 6
    uint8_t adc_done_stat;   // Bit 5
    uint8_t vsys_stat;       // Bit 4
    uint8_t chg_tmr_stat;    // Bit 3
    uint8_t trichg_tmr_stat; // Bit 2
    uint8_t prechg_tmr_stat; // Bit 1
} BQ25798_ChargerStatus3;

// REG1F_Charger_Status_4 Register
typedef struct {
    uint8_t vbatotg_low_stat; // Bit 4
    uint8_t ts_cold_stat;     // Bit 3
    uint8_t ts_cool_stat;     // Bit 2
    uint8_t ts_warm_stat;     // Bit 1
    uint8_t ts_hot_stat;      // Bit 0
} BQ25798_ChargerStatus4;
// REG20_FAULT_Status_0 Register
typedef struct {
    uint8_t ibat_reg_stat;  // Bit 7
    uint8_t vbus_ovp_stat;  // Bit 6
    uint8_t vbat_ovp_stat;  // Bit 5
    uint8_t ibus_ocp_stat;  // Bit 4
    uint8_t ibat_ocp_stat;  // Bit 3
    uint8_t conv_ocp_stat;  // Bit 2
    uint8_t vac2_ovp_stat;  // Bit 1
    uint8_t vac1_ovp_stat;  // Bit 0
} BQ25798_FaultStatus0;

// REG21_FAULT_Status_1 Register
typedef struct {
    uint8_t vsys_short_stat; // Bit 7
    uint8_t vsys_ovp_stat;   // Bit 6
    uint8_t otg_ovp_stat;    // Bit 5
    uint8_t otg_uvp_stat;    // Bit 4
    uint8_t tshut_stat;      // Bit 2
} BQ25798_FaultStatus1;

typedef struct{
	I2C_HandleTypeDef *i2cHandle;
	uint16_t voltageBus;      // in mV
	uint16_t currentBus;     // in mA
	uint16_t voltageBattery;  // in mV
	uint16_t currentBattery; // in mA

	BQ25798_ChargerStatus0 chargerStatus0;
	BQ25798_ChargerStatus1 chargerStatus1;
	BQ25798_ChargerStatus2 chargerStatus2;
	BQ25798_ChargerStatus3 chargerStatus3;
	BQ25798_ChargerStatus4 chargerStatus4;
	BQ25798_FaultStatus0 faultStatus0;
	BQ25798_FaultStatus1 faultStatus1;
} BQ25798;

// INITIALISATION

uint8_t BQ25798_init(BQ25798 *device, I2C_HandleTypeDef *i2cHandle);

// DATA ACQUISATION
HAL_StatusTypeDef BQ25798_readBusVoltage(BQ25798 *device);
HAL_StatusTypeDef BQ25798_readBusCurrent(BQ25798 *device);
HAL_StatusTypeDef BQ25798_readBatteryVoltage(BQ25798 *device);
HAL_StatusTypeDef BQ25798_readBatteryCurrent(BQ25798 *device);


// LOW LEVEL FUNCTIONS
HAL_StatusTypeDef BQ25798_ReadRegisters(BQ25798 *device, uint8_t reg, uint8_t *data, uint8_t length);
HAL_StatusTypeDef BQ25798_ReadRegister (BQ25798 *device, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef BQ25798_WriteRegister(BQ25798 *device, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef readChargerStatus0(BQ25798 *device, uint8_t *status);
HAL_StatusTypeDef readChargerStatus1(BQ25798 *device, uint8_t *status);
HAL_StatusTypeDef readChargerStatus2(BQ25798 *device, uint8_t *status);
HAL_StatusTypeDef readChargerStatus3(BQ25798 *device, uint8_t *status);
HAL_StatusTypeDef readChargerStatus4(BQ25798 *device, uint8_t *status);
HAL_StatusTypeDef readFaultStatus0(BQ25798 *device, uint8_t *status);
HAL_StatusTypeDef readFaultStatus1(BQ25798 *device, uint8_t *status);


#endif /* INC_BQ25798_H_ */
