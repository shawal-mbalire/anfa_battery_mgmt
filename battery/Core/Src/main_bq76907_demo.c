/*
 * main_bq76907_demo.c
 *  Demonstration file showing how the new BQ76907 driver API could be consumed.
 *  This does NOT replace the existing main.c; you can selectively compile it.
 */
#include "stm32g0xx_hal.h"
#include "bq76907.h"

// Externs from the project (assumes I2C handle is named hi2c1 in i2c.c)
extern I2C_HandleTypeDef hi2c1;

static BQ76907 batteryMon;

static void Demo_LogResults(void){
    // Placeholder: replace with real logging (UART, SWO, etc.)
    // For now, function just a breakpoint location.
    __NOP();
}

void BQ76907_Demo_Task(void){
    if (BQ76907_readSystemStatus(&batteryMon) == HAL_OK){
        BQ76907_readCellVoltages(&batteryMon);
        BQ76907_readPackVoltage(&batteryMon);
        BQ76907_readTemperature1(&batteryMon);
        Demo_LogResults();
    }
}

void BQ76907_Demo_Init(void){
    uint8_t initResult = BQ76907_init(&batteryMon, &hi2c1);
    (void)initResult; // Set breakpoint to inspect
}

/* If you want this to be a standalone main (instead of project main.c),
 * you can rename and integrate. Provided here only as structural guidance. */
#ifdef BQ76907_DEMO_STANDALONE
int main(void){
    HAL_Init();
    // SystemClock_Config(); // user must add
    // MX_I2C1_Init();       // user must add

    BQ76907_Demo_Init();

    while (1){
        BQ76907_Demo_Task();
        HAL_Delay(1000);
    }
}
#endif
