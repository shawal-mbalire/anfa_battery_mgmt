/* Backup of original main.c focused on BQ25798 charger only.
 * This file is excluded from the build unless BQ25798_LEGACY_MAIN is defined.
 */
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "bq25798.h"

#ifndef BQ25798_LEGACY_MAIN
/* Stub to avoid empty translation unit warnings when not compiling legacy main. */
void bq25798_legacy_placeholder(void) {}
#else

#define BQ_UPDATE_INTERVAL_MS   500
#define ERROR_LED_BLINK_RATE_MS 200

static uint32_t last_bq_update_tick = 0;
static uint32_t last_error_led_toggle_tick = 0;
static BQ25798 bq25798_charger; /* static to limit linkage when legacy main enabled */

void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();

  if (BQ25798_init(&bq25798_charger, &hi2c1) != 0) {
      Error_Handler();
  }
  last_bq_update_tick = HAL_GetTick();

  while (1) {
      if ((HAL_GetTick() - last_bq_update_tick) >= BQ_UPDATE_INTERVAL_MS) {
          last_bq_update_tick = HAL_GetTick();
          uint8_t s=0;
          readChargerStatus0(&bq25798_charger, &s);
          readChargerStatus1(&bq25798_charger, &s);
          readChargerStatus2(&bq25798_charger, &s);
          readChargerStatus3(&bq25798_charger, &s);
          readChargerStatus4(&bq25798_charger, &s);
          readFaultStatus0(&bq25798_charger, &s);
          readFaultStatus1(&bq25798_charger, &s);
          BQ25798_readBusVoltage(&bq25798_charger);
          BQ25798_readBusCurrent(&bq25798_charger);
          BQ25798_readBatteryVoltage(&bq25798_charger);
          BQ25798_readBatteryCurrent(&bq25798_charger);
          if (bq25798_charger.chargerStatus2.vbat_present_stat == 1) {
              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
          } else {
              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
          }
      }
      if (bq25798_charger.faultStatus1.tshut_stat == 1) {
          if ((HAL_GetTick() - last_error_led_toggle_tick) >= ERROR_LED_BLINK_RATE_MS) {
              last_error_led_toggle_tick = HAL_GetTick();
              HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
          }
      } else {
          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
      }
  }
}
#endif /* BQ25798_LEGACY_MAIN */
