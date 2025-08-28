#ifndef BM_ERRORS_H
#define BM_ERRORS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" { 
#endif

/* Generic battery management result codes */
typedef enum {
    BM_OK = 0,
    BM_ERR_I2C = -1,
    BM_ERR_ID_MISMATCH = -2,
    BM_ERR_TIMEOUT = -3,
    BM_ERR_RANGE = -4,
    BM_ERR_STATE = -5,
    BM_ERR_CONFIG = -6,
    BM_ERR_COMM_CRC = -7,
    BM_ERR_PROTECTION_TRIP = -8,
    BM_ERR_UNKNOWN = -9
} BM_Result;

/* Optional classification (can extend later) */
typedef enum {
    BM_SRC_GENERIC = 0,
    BM_SRC_BQ25798 = 1,
    BM_SRC_BQ76907 = 2
} BM_ErrorSource;

/* Compact error entry for ring buffer */
typedef struct {
    uint32_t tick;     /* HAL_GetTick() snapshot */
    int8_t   code;     /* BM_Result (negative or 0) */
    uint8_t  detail;   /* Optional detail (e.g. HAL status, sub-step) */
    uint8_t  source;   /* BM_ErrorSource */
    uint8_t  reg;      /* Register involved (if any) */
    uint16_t value;    /* Value attempted or raw read */
} BM_ErrorEntry;

#ifndef BM_ERROR_LOG_DEPTH
#define BM_ERROR_LOG_DEPTH 16
#endif

/* Helper macro to push an error into a device-specific ring buffer.
 * Device struct MUST expose: BM_ErrorEntry errorLog[BM_ERROR_LOG_DEPTH]; uint8_t errorHead; int8_t lastError; */
#define BM_PUSH_ERROR(devPtr, src, errCode, detailByte, regAddr, val) \
    do { \
        if ((devPtr)) { \
            uint8_t idx = (devPtr)->errorHead % BM_ERROR_LOG_DEPTH; \
            (devPtr)->errorLog[idx].tick = BM_GET_TICK(); \
            (devPtr)->errorLog[idx].code = (int8_t)(errCode); \
            (devPtr)->errorLog[idx].detail = (uint8_t)(detailByte); \
            (devPtr)->errorLog[idx].source = (uint8_t)(src); \
            (devPtr)->errorLog[idx].reg = (uint8_t)(regAddr); \
            (devPtr)->errorLog[idx].value = (uint16_t)(val); \
            (devPtr)->errorHead = (uint8_t)((devPtr)->errorHead + 1); \
            (devPtr)->lastError = (int8_t)(errCode); \
        } \
    } while(0)

/* Abstract tick accessor so we can fall back if HAL not present */
#ifndef BM_GET_TICK
#include "stm32g0xx_hal.h"
#define BM_GET_TICK() HAL_GetTick()
#endif

#ifdef __cplusplus
}
#endif

#endif /* BM_ERRORS_H */
