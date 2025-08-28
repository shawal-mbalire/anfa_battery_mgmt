/* hal_stubs.h
 * Lightweight stand-in for STM32 HAL headers when building host-side tools/tests
 * Define USE_HAL_STUBS (or pass -DUSE_HAL_STUBS) before including driver headers.
 */
#ifndef HAL_STUBS_H
#define HAL_STUBS_H
typedef struct { int dummy; } I2C_HandleTypeDef;
typedef int HAL_StatusTypeDef;
#define HAL_OK 0
#define HAL_ERROR (-1)
#define HAL_MAX_DELAY 0xFFFFFFFFu
/* Minimal no-op macros */
#define __NOP() do {} while(0)
#endif
