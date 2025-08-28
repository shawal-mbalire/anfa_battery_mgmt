/*
 * bq76907.h
 *
 *  Driver skeleton for TI BQ76907 (Battery Monitor / Protector)
 *  NOTE: Register addresses and bit fields MUST be verified against the official datasheet (bq76907.pdf)
 *  and the application/user guide SLUAAM4 (sluaam4.pdf) for recommended configuration workflow.
 *  This file is modeled after bq25798.h for structural consistency in this project.
 *
 *  Created on: Aug 25, 2025
 *      Author: Auto-generated (GitHub Copilot Assist) – PLEASE VALIDATE
 */

#ifndef INC_BQ76907_H_
#define INC_BQ76907_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_HAL_STUBS) || defined(BQ76907_NO_HAL)
#include "hal_stubs.h"
#else
#include "stm32g0xx_hal.h"  /* Real HAL */
#endif
#include <stdint.h>
#include "bm_errors.h"

/* ============================= IMPORTANT VALIDATION NOTICE =============================
 * The BQ76907 register map, I2C address, and bit definitions below are placeholders
 * based on typical TI battery monitor conventions and MUST be cross-checked with the
 * actual datasheet (bq76907.pdf in repo). Replace all entries marked TODO_VERIFY.
 * ======================================================================================= */

/* --- I2C Configuration (TODO_VERIFY) --- */
#define BQ76907_I2C_ADDRESS_7BIT              (0x08)    /* TODO_VERIFY */
#define BQ76907_I2C_ADDRESS                   (BQ76907_I2C_ADDRESS_7BIT << 1)

/* --- Core Register Map (Partial / TODO_FILL) ---
 * Add ONLY after verifying each address. Names chosen to mirror likely functions.
 */
#define BQ76907_REG_SYS_STAT                  0x00  /* TODO_VERIFY */
#define BQ76907_REG_CELLBAL1                  0x01  /* TODO_VERIFY */
#define BQ76907_REG_CELLBAL2                  0x02  /* TODO_VERIFY */
#define BQ76907_REG_SYS_CTRL1                 0x04  /* TODO_VERIFY */
#define BQ76907_REG_SYS_CTRL2                 0x05  /* TODO_VERIFY */
#define BQ76907_REG_PROTECT1                  0x06  /* TODO_VERIFY */
#define BQ76907_REG_PROTECT2                  0x07  /* TODO_VERIFY */
#define BQ76907_REG_PROTECT3                  0x08  /* TODO_VERIFY */
#define BQ76907_REG_OV_TRIP                   0x09  /* TODO_VERIFY */
#define BQ76907_REG_UV_TRIP                   0x0A  /* TODO_VERIFY */
#define BQ76907_REG_CC_CFG                    0x0B  /* TODO_VERIFY */
#define BQ76907_REG_DEVICE_ID                 0x0C  /* TODO_VERIFY */
#define BQ76907_REG_ADC_CONTROL               0x0D  /* TODO_VERIFY */
#define BQ76907_REG_VCELL1_H                  0x0E  /* TODO_VERIFY */
#define BQ76907_REG_VCELL1_L                  0x0F  /* TODO_VERIFY */
/* ... Continue sequential VCELLx registers when validated ... */
#define BQ76907_REG_PACK_V_H                  0x2A  /* TODO_VERIFY */
#define BQ76907_REG_PACK_V_L                  0x2B  /* TODO_VERIFY */
#define BQ76907_REG_TS1_H                     0x2C  /* TODO_VERIFY */
#define BQ76907_REG_TS1_L                     0x2D  /* TODO_VERIFY */

/* --- Additional Configuration / Protection / Control Registers (ALL TODO_VERIFY) --- */
#define BQ76907_REG_POWER_CONFIG              0x30  /* Power Config */
#define BQ76907_REG_DA_CONFIG                 0x31  /* Device / Analog Config (e.g. thermistor usage) */
#define BQ76907_REG_REGOUT_CONFIG             0x32  /* REGOUT voltage configuration */
#define BQ76907_REG_VCELL_MODE                0x33  /* Number of series cells enabled */
#define BQ76907_REG_ALARM_MASK_DEFAULT        0x34  /* Default Alarm Mask */
#define BQ76907_REG_FET_OPTIONS               0x35  /* FET option bits */
#define BQ76907_REG_ENABLED_PROTECTIONS_A     0x36  /* Protections A enable */
#define BQ76907_REG_ENABLED_PROTECTIONS_B     0x37  /* Protections B enable */
#define BQ76907_REG_DSG_FET_PROTECTIONS_A     0x38  /* Discharge FET protection behavior A */
#define BQ76907_REG_CHG_FET_PROTECTIONS_A     0x39  /* Charge FET protection behavior A */
#define BQ76907_REG_LATCH_LIMIT               0x3A  /* Latch limit config */
#define BQ76907_REG_MAX_INTERNAL_TEMP         0x3B  /* Maximum internal temperature threshold */
#define BQ76907_REG_CUV_THRESHOLD             0x3C  /* Cell undervoltage threshold */
#define BQ76907_REG_COV_THRESHOLD             0x3D  /* Cell overvoltage threshold */
#define BQ76907_REG_OCD_CHG_THRESHOLD         0x3E  /* Overcurrent in charge threshold */
#define BQ76907_REG_OCD_DISCH1_THRESHOLD      0x3F  /* Overcurrent in discharge 1 threshold */
#define BQ76907_REG_OCD_DISCH2_THRESHOLD      0x40  /* Overcurrent in discharge 2 threshold */
#define BQ76907_REG_INT_OT_THRESHOLD          0x41  /* Internal overtemperature threshold */
#define BQ76907_REG_VOLTAGE_TIME              0x42  /* Voltage time config */
#define BQ76907_REG_ALARM_ENABLE              0x43  /* Alarm enable bits */
#define BQ76907_REG_ALARM_STATUS              0x44  /* Alarm status (write to clear) */
#define BQ76907_REG_SAFETY_STATUS_A           0x45  /* Safety Status A */
#define BQ76907_REG_SAFETY_STATUS_B           0x46  /* Safety Status B */
#define BQ76907_REG_PASSQ                     0x47  /* PASSQ / SOC related */
#define BQ76907_REG_CB_ACTIVE_CELLS           0x48  /* Cell balancing active cells mask */
#define BQ76907_REG_PROT_RECOVERY             0x49  /* Protection recovery control */

/* --- Command / Mode Control (logical registers or command writes) --- */
#define BQ76907_CMD_SET_CFGUPDATE             0x60  /* Enter Config Update (write sequence) */
#define BQ76907_CMD_EXIT_CFGUPDATE            0x61  /* Exit Config Update (write sequence) */

/* --- Bit Masks (Placeholders) --- */
#define BQ76907_FET_OPTIONS_CHG_FET_EN        (1u<<0) /* TODO_VERIFY */
#define BQ76907_FET_OPTIONS_DSG_FET_EN        (1u<<1) /* TODO_VERIFY */
#define BQ76907_POWER_CONFIG_SLEEP_EN         (1u<<2) /* TODO_VERIFY */
#define BQ76907_POWER_CONFIG_SLEEP_DIS        (1u<<3) /* TODO_VERIFY */

/* Structure capturing valued configuration registers */
typedef struct {
    uint8_t  cellCount;          /* Number of series cells (used for VCELL_MODE) */
    uint16_t uvThreshold_mV;     /* Cell undervoltage threshold */
    uint16_t ovThreshold_mV;     /* Cell overvoltage threshold */
    uint16_t ocCharge_mA;        /* Overcurrent charge threshold */
    uint16_t ocDischarge1_mA;    /* Overcurrent discharge 1 */
    uint16_t ocDischarge2_mA;    /* Overcurrent discharge 2 */
    uint8_t  internalOT_C;       /* Internal overtemperature threshold */
    uint8_t  maxInternalTemp_C;  /* Max internal temp (separate if distinct) */
    uint16_t balanceInterval_ms; /* Desired cell balancing interval (host side) */
    uint8_t  voltageTimeUnits;   /* Voltage time configuration raw value */
    uint8_t  fetOptions;         /* Initial FET options mask */
    uint8_t  protectionsA;       /* Enabled Protections A */
    uint8_t  protectionsB;       /* Enabled Protections B */
    uint8_t  dsgFetProtA;        /* DSG FET protections A mask */
    uint8_t  chgFetProtA;        /* CHG FET protections A mask */
    uint8_t  latchLimit;         /* Latch limit raw */
    uint8_t  alarmMaskDefault;   /* Default alarm mask */
    uint8_t  alarmEnableMask;    /* Alarm enable bits */
    uint8_t  daConfig;           /* DA Config bits (e.g., thermistor disabled) */
    uint8_t  regoutConfig;       /* REGOUT voltage select */
    uint8_t  powerConfig;        /* Power config base value */
} BQ76907_Config;

/* --- Bit Masks & Fields (Examples – adjust after validation) --- */
#define BQ76907_SYS_STAT_CC_READY             (1u << 7)  /* TODO_VERIFY */
#define BQ76907_SYS_STAT_DEVICE_XREADY        (1u << 6)  /* TODO_VERIFY */
#define BQ76907_SYS_STAT_OV_FLAG              (1u << 5)  /* TODO_VERIFY */
#define BQ76907_SYS_STAT_UV_FLAG              (1u << 4)  /* TODO_VERIFY */
#define BQ76907_SYS_STAT_SCD_FLAG             (1u << 3)  /* TODO_VERIFY */
#define BQ76907_SYS_STAT_OCD_FLAG             (1u << 2)  /* TODO_VERIFY */
#define BQ76907_SYS_STAT_OVERTEMP_FLAG        (1u << 1)  /* TODO_VERIFY */
#define BQ76907_SYS_STAT_RESERVED             (1u << 0)  /* TODO_VERIFY */

/* Data Structures */
typedef struct {
    uint8_t cc_ready;
    uint8_t dev_ready;
    uint8_t ov_fault;
    uint8_t uv_fault;
    uint8_t scd_fault;
    uint8_t ocd_fault;
    uint8_t ot_fault;
} BQ76907_SystemStatus;

/* Main driver object */
typedef struct {
    I2C_HandleTypeDef *i2cHandle;
    uint16_t packVoltage_mV;
    uint16_t cellVoltage_mV[4]; /* 4s pack per requirements */
    int16_t  ts1_degC_x10;
    BQ76907_SystemStatus status;
    BQ76907_Config       activeConfig; /* Snapshot of last applied configuration */

    /* Error handling */
    BM_ErrorEntry errorLog[BM_ERROR_LOG_DEPTH];
    uint8_t errorHead;
    int8_t  lastError;
} BQ76907;

/* ================= API PROTOTYPES ================= */

// Initialization
uint8_t BQ76907_init(BQ76907 *dev, I2C_HandleTypeDef *hi2c);

// Data acquisition
#ifndef BQ76907_NO_HAL
HAL_StatusTypeDef BQ76907_readSystemStatus(BQ76907 *dev);
HAL_StatusTypeDef BQ76907_readCellVoltages(BQ76907 *dev);
HAL_StatusTypeDef BQ76907_readPackVoltage(BQ76907 *dev);
HAL_StatusTypeDef BQ76907_readTemperature1(BQ76907 *dev);
#endif

// Low Level Access
HAL_StatusTypeDef BQ76907_ReadRegister (BQ76907 *dev, uint8_t reg, uint8_t *data);
HAL_StatusTypeDef BQ76907_ReadRegisters(BQ76907 *dev, uint8_t reg, uint8_t *data, uint8_t len);
HAL_StatusTypeDef BQ76907_WriteRegister(BQ76907 *dev, uint8_t reg, uint8_t data);

// Utility scaling helpers (implement when exact LSB values known)
uint16_t BQ76907_scaleCellVoltage(uint16_t raw);
uint16_t BQ76907_scalePackVoltage(uint16_t raw);
int16_t  BQ76907_scaleTemperature(int16_t raw);

/* Sleep / low power (placeholders; verify bits in SYS_CTRL1/SYS_CTRL2) */
#define BQ76907_SYS_CTRL1_SLEEP_BIT    (1u << 1) /* TODO_VERIFY */
#define BQ76907_SYS_CTRL1_ADC_EN_BIT   (1u << 0) /* TODO_VERIFY */
HAL_StatusTypeDef BQ76907_enterSleep(BQ76907 *dev);
HAL_StatusTypeDef BQ76907_exitSleep (BQ76907 *dev);

/* ================= Configuration & Control API (Stubs) ================= */
HAL_StatusTypeDef BQ76907_enterConfigUpdate(BQ76907 *dev);    /* SET_CFGUPDATE */
HAL_StatusTypeDef BQ76907_exitConfigUpdate (BQ76907 *dev);    /* EXIT_CFGUPDATE */
HAL_StatusTypeDef BQ76907_applyConfig      (BQ76907 *dev, const BQ76907_Config *cfg);

/* Individual register write helpers (each writes raw or scaled value) */
HAL_StatusTypeDef BQ76907_setPowerConfig         (BQ76907 *dev, uint8_t v);
HAL_StatusTypeDef BQ76907_setDAConfig            (BQ76907 *dev, uint8_t v);
HAL_StatusTypeDef BQ76907_setRegoutConfig        (BQ76907 *dev, uint8_t v);
HAL_StatusTypeDef BQ76907_setVcellMode           (BQ76907 *dev, uint8_t cellCount);
HAL_StatusTypeDef BQ76907_setAlarmMaskDefault    (BQ76907 *dev, uint8_t mask);
HAL_StatusTypeDef BQ76907_setFETOptions          (BQ76907 *dev, uint8_t opts);
HAL_StatusTypeDef BQ76907_enableProtectionsA     (BQ76907 *dev, uint8_t mask);
HAL_StatusTypeDef BQ76907_enableProtectionsB     (BQ76907 *dev, uint8_t mask);
HAL_StatusTypeDef BQ76907_setDSGFetProtectionsA  (BQ76907 *dev, uint8_t mask);
HAL_StatusTypeDef BQ76907_setCHGFetProtectionsA  (BQ76907 *dev, uint8_t mask);
HAL_StatusTypeDef BQ76907_setLatchLimit          (BQ76907 *dev, uint8_t v);
HAL_StatusTypeDef BQ76907_setMaxInternalTemp     (BQ76907 *dev, uint8_t degC);
HAL_StatusTypeDef BQ76907_setCUVThreshold        (BQ76907 *dev, uint16_t mV);
HAL_StatusTypeDef BQ76907_setCOVThreshold        (BQ76907 *dev, uint16_t mV);
HAL_StatusTypeDef BQ76907_setOCChargeThreshold   (BQ76907 *dev, uint16_t mA);
HAL_StatusTypeDef BQ76907_setOCDischarge1Threshold(BQ76907 *dev, uint16_t mA);
HAL_StatusTypeDef BQ76907_setOCDischarge2Threshold(BQ76907 *dev, uint16_t mA);
HAL_StatusTypeDef BQ76907_setInternalOTThreshold (BQ76907 *dev, uint8_t degC);
HAL_StatusTypeDef BQ76907_setVoltageTime         (BQ76907 *dev, uint8_t raw);
HAL_StatusTypeDef BQ76907_setAlarmEnable         (BQ76907 *dev, uint8_t mask);
HAL_StatusTypeDef BQ76907_readAlarmStatus        (BQ76907 *dev, uint8_t *status);
HAL_StatusTypeDef BQ76907_clearAlarmStatus       (BQ76907 *dev, uint8_t maskToClear);
HAL_StatusTypeDef BQ76907_readSafetyStatusA      (BQ76907 *dev, uint8_t *status);
HAL_StatusTypeDef BQ76907_readSafetyStatusB      (BQ76907 *dev, uint8_t *status);
HAL_StatusTypeDef BQ76907_fetEnable              (BQ76907 *dev, uint8_t chargeEnable, uint8_t dischargeEnable);
HAL_StatusTypeDef BQ76907_sleepEnable            (BQ76907 *dev); /* SLEEP_ENABLE wrapper */
HAL_StatusTypeDef BQ76907_sleepDisable           (BQ76907 *dev); /* SLEEP_DISABLE wrapper */
HAL_StatusTypeDef BQ76907_readPASSQ              (BQ76907 *dev, uint8_t *val); /* PASSQ / SOC calc placeholder */
HAL_StatusTypeDef BQ76907_setActiveBalancingMask (BQ76907 *dev, uint8_t cellMask); /* CB_ACTIVE_CELLS */
HAL_StatusTypeDef BQ76907_protectionRecovery     (BQ76907 *dev, uint8_t mask);   /* PROT_RECOVERY */

/* Host-side balancing heuristic: evaluate cell voltages and apply mask.
 * Parameters:
 *  deltaStart_mV: minimum spread to start balancing
 *  deltaStop_mV : spread below which balancing stops (hysteresis)
 *  minCell_mV   : do not balance any cell below this voltage
 * Returns last applied mask (0 if none) or 0xFF on I2C error. */
uint8_t BQ76907_evaluateAndBalance(BQ76907 *dev, uint16_t deltaStart_mV, uint16_t deltaStop_mV, uint16_t minCell_mV);

/* Convenience high-level protection configuration wrappers */
HAL_StatusTypeDef BQ76907_configVoltageProtection(BQ76907 *dev, uint16_t uv_mV, uint16_t ov_mV);
HAL_StatusTypeDef BQ76907_configCurrentProtection(BQ76907 *dev, uint16_t ocChg_mA, uint16_t ocDis1_mA, uint16_t ocDis2_mA);
HAL_StatusTypeDef BQ76907_configTemperatureProtection(BQ76907 *dev, uint8_t ot_C, uint8_t maxInternal_C);

/* Debug / diagnostics */
void BQ76907_debugDump(const BQ76907 *dev); /* Emits a concise state summary via BQ_LOG */
/* Periodic concise status line; internally throttled by tick interval.
 * Example output:
 * [BQ] 76907 STAT Pack=15320mV Cells=3810,3820,3815,3875mV T=27.3C F:OV=0 UV=0 OCD=0 SCD=0 OT=0 Bal=0x0
 */
void BQ76907_logStatus(BQ76907 *dev);

/* One-shot dump of the currently applied (shadowed) configuration struct. */
void BQ76907_logConfig(const BQ76907 *dev);

/* Error / diagnostics API */
int8_t  BQ76907_getLastError(const BQ76907 *dev);
uint8_t BQ76907_getErrorCount(const BQ76907 *dev);
void    BQ76907_dumpErrors(const BQ76907 *dev);

#ifndef BQ_LOG
 #ifdef USE_ITM_LOG
  #include "itm_log.h"
  #define BQ_LOG(fmt, ...) do { ITM_Log_Printf("[BQ] " fmt, ##__VA_ARGS__); } while(0)
 #else
  #include <stdio.h>
  #define BQ_LOG(fmt, ...) do { printf("[BQ] " fmt "\n", ##__VA_ARGS__); } while(0)
 #endif
#endif

#ifndef BQ_VERIFY
#define BQ_VERIFY(devPtr, expr, code, detail) \
    do { if(!(expr)){ BQ_LOG("VERIFY FAIL: %s (code=%d)", #expr, (int)(code)); BM_PUSH_ERROR(devPtr, BM_SRC_BQ76907, code, (uint8_t)(detail), 0xFE, 0); } } while(0)
#endif


#ifdef __cplusplus
}
#endif

#endif /* INC_BQ76907_H_ */
