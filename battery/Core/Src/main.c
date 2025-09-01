/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bq25798.h" // Include the BQ25798 driver header
#include "bq76907.h" // Battery monitor / protector (placeholder driver)
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BQ_UPDATE_INTERVAL_MS   500 // Update BQ25798 status every 500 milliseconds
#define BQ76907_UPDATE_INTERVAL_MS  750 // Update cell monitor every 750 ms (staggered)
#define BALANCE_INTERVAL_MS     5000 // Evaluate balancing every 5 seconds
#define BALANCE_THRESHOLD_MV    25   // Start balancing if delta > 25mV (placeholder)
#define BALANCE_HYSTERESIS_MV   10   // Stop when delta < 10mV (placeholder)
#define ERROR_LED_BLINK_RATE_MS 200 // Blink the error LED every 200 milliseconds
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
BQ25798 bq25798_charger;              // Charger instance
BQ76907 bq76907_monitor;              // Monitor instance (placeholder implementation)
static uint32_t last_bq_update_tick = 0;          // Last charger status update
static uint32_t last_bq76907_update_tick = 0;     // Last monitor update
static uint32_t last_balance_eval_tick = 0;       // Last balancing evaluation
static uint32_t last_error_led_toggle_tick = 0;   // Last time the error LED was toggled

// Forward static helpers
static void UpdateCharger(void);
static void UpdateMonitor(void);
static void EvaluateBalancing(void);
static uint16_t findMaxCell(uint16_t *vals, uint8_t count);
static uint16_t findMinCell(uint16_t *vals, uint8_t count);
static void applyCellBalancingMask(uint8_t mask);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  printf("[FUNC] main BEGIN\n");

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  printf("[MAIN] Init start\n");

  // Initialize the BQ25798 charger
  uint32_t t0 = HAL_GetTick();
  if (BQ25798_init(&bq25798_charger, &hi2c1) != 0) {
    Error_Handler();
  }
  printf("[MAIN] Charger init done (+%lums)\n", (unsigned long)(HAL_GetTick()-t0));
  // Initialize the BQ76907 monitor (placeholder init – returns 0 on success)
  t0 = HAL_GetTick();
  if (BQ76907_init(&bq76907_monitor, &hi2c1) != 0) {
    // If monitor init fails we continue charger operation but could flag a warning
    // For now treat as critical
    Error_Handler();
  }
  printf("[MAIN] Monitor init done (+%lums)\n", (unsigned long)(HAL_GetTick()-t0));

  // Apply a placeholder safe configuration snapshot (values TBD after datasheet validation)
  BQ76907_Config cfg = {
      .cellCount = 4,
      .uvThreshold_mV = 2500, .ovThreshold_mV = 4200,
      .ocCharge_mA = 3000, .ocDischarge1_mA = 5000, .ocDischarge2_mA = 8000,
      .internalOT_C = 85, .maxInternalTemp_C = 90,
      .balanceInterval_ms = 5000,
      .voltageTimeUnits = 0x00,
      .fetOptions = 0x00,
      .protectionsA = 0x00, .protectionsB = 0x00,
      .dsgFetProtA = 0x00, .chgFetProtA = 0x00,
      .latchLimit = 0x00,
      .alarmMaskDefault = 0x00, .alarmEnableMask = 0x00,
      .daConfig = 0x00, .regoutConfig = 0x00, .powerConfig = 0x00
  };
  t0 = HAL_GetTick();
  if (BQ76907_applyConfig(&bq76907_monitor, &cfg) == HAL_OK){
      BQ76907_logConfig(&bq76907_monitor);
  printf("[MAIN] Monitor config applied (+%lums)\n", (unsigned long)(HAL_GetTick()-t0));
  } else {
  printf("[MAIN] Monitor config apply FAILED\n");
  }

  // Initialize timers for immediate first update
  uint32_t now = HAL_GetTick();
  last_bq_update_tick       = now;
  last_bq76907_update_tick  = now;
  last_balance_eval_tick    = now;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint32_t tick = HAL_GetTick();
    if ((tick - last_bq_update_tick) >= BQ_UPDATE_INTERVAL_MS) {
      last_bq_update_tick = tick;
      UpdateCharger();
    }
    if ((tick - last_bq76907_update_tick) >= BQ76907_UPDATE_INTERVAL_MS) {
      last_bq76907_update_tick = tick;
      UpdateMonitor();
    }
    if ((tick - last_balance_eval_tick) >= BALANCE_INTERVAL_MS) {
      last_balance_eval_tick = tick;
      EvaluateBalancing();
    }

      // --- Non-blocking Error LED (Orange LED) handling ---
      // This is for demonstration, assuming GPIO_PIN_5 (orange LED) is for a general fault indicator.
      // You would typically turn this on or blink it in your Error_Handler or if a specific fault is detected.
      if (bq25798_charger.faultStatus1.tshut_stat == 1) {
          if ((HAL_GetTick() - last_error_led_toggle_tick) >= ERROR_LED_BLINK_RATE_MS) {
              last_error_led_toggle_tick = HAL_GetTick();
              HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // Toggle Orange LED
          }
      } else {
          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Keep Orange LED OFF (assuming active low)
      }
    /* main loop continues indefinitely */


      /* Add other non-blocking tasks here */
      // e.g., Read sensors, process user input, update displays, communicate over other peripherals
      // These tasks will execute continuously while the BQ25798 update timer is counting down.

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  /* System clock configured */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// ---------------- Internal helper implementations ----------------

static void UpdateCharger(void) {
  uint32_t tStart = HAL_GetTick();
  printf("[FUNC] UpdateCharger BEGIN\n");
  printf("[CHG] Update begin\n");
  uint8_t b=0;
  readChargerStatus0(&bq25798_charger, &b);
  readChargerStatus1(&bq25798_charger, &b);
  readChargerStatus2(&bq25798_charger, &b);
  readChargerStatus3(&bq25798_charger, &b);
  readChargerStatus4(&bq25798_charger, &b);
  readFaultStatus0(&bq25798_charger, &b);
  readFaultStatus1(&bq25798_charger, &b);
  BQ25798_readBusVoltage(&bq25798_charger);
  BQ25798_readBusCurrent(&bq25798_charger);
  BQ25798_readBatteryVoltage(&bq25798_charger);
  BQ25798_readBatteryCurrent(&bq25798_charger);

  if (bq25798_charger.chargerStatus2.vbat_present_stat == 1) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
  } else {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  }
  /* Emit a concise status line */
  BQ25798_logStatus(&bq25798_charger);
  printf("[CHG] Update end (%lums) VBAT=%umV IBAT=%dmA BUS=%umV/%dmA Fault1.tshut=%u\n",
    (unsigned long)(HAL_GetTick()-tStart),
    (unsigned)bq25798_charger.voltageBattery,
    (unsigned)bq25798_charger.currentBattery,
    (unsigned)bq25798_charger.voltageBus,
    (unsigned)bq25798_charger.currentBus,
    (unsigned)bq25798_charger.faultStatus1.tshut_stat);
  printf("[FUNC] UpdateCharger END\n");
}

static void UpdateMonitor(void) {
  uint32_t tStart = HAL_GetTick();
  printf("[FUNC] UpdateMonitor BEGIN\n");
  printf("[MON] Update begin\n");
  // Read system status & cell voltages
  BQ76907_readSystemStatus(&bq76907_monitor);
  BQ76907_readCellVoltages(&bq76907_monitor);
  BQ76907_readPackVoltage(&bq76907_monitor);
  BQ76907_readTemperature1(&bq76907_monitor);
  BQ76907_logStatus(&bq76907_monitor);
  // Monitor fault indication (aggregate)
  uint8_t anyFault = bq76907_monitor.status.ov_fault || bq76907_monitor.status.uv_fault ||
           bq76907_monitor.status.ocd_fault || bq76907_monitor.status.scd_fault ||
           bq76907_monitor.status.ot_fault;
  static uint8_t lastFaultState = 0xFF; // ensure first print
  if (anyFault){
    // Flash LED rapidly to signal monitor-level fault (reuse PA5 / orange LED assumption)
    if ((HAL_GetTick() - last_error_led_toggle_tick) >= 150){
      last_error_led_toggle_tick = HAL_GetTick();
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    }
  }
  if (lastFaultState != anyFault){
    lastFaultState = anyFault;
    if (anyFault){
      printf("[MON] FAULT: OV=%u UV=%u OCD=%u SCD=%u OT=%u\n", bq76907_monitor.status.ov_fault,
             bq76907_monitor.status.uv_fault, bq76907_monitor.status.ocd_fault,
             bq76907_monitor.status.scd_fault, bq76907_monitor.status.ot_fault);
    } else {
      printf("[MON] FAULT CLEARED\n");
      // Ensure LED off (inactive state high per earlier assumption)
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    }
  }
  printf("[MON] Update end (%lums) Pack=%umV Cells=%u,%u,%u,%u mV Flags OV=%u UV=%u OCD=%u SCD=%u OT=%u\n",
    (unsigned long)(HAL_GetTick()-tStart),
    (unsigned)bq76907_monitor.packVoltage_mV,
    (unsigned)bq76907_monitor.cellVoltage_mV[0],
    (unsigned)bq76907_monitor.cellVoltage_mV[1],
    (unsigned)bq76907_monitor.cellVoltage_mV[2],
    (unsigned)bq76907_monitor.cellVoltage_mV[3],
    (unsigned)bq76907_monitor.status.ov_fault,
    (unsigned)bq76907_monitor.status.uv_fault,
    (unsigned)bq76907_monitor.status.ocd_fault,
    (unsigned)bq76907_monitor.status.scd_fault,
    (unsigned)bq76907_monitor.status.ot_fault);
  printf("[FUNC] UpdateMonitor END\n");
}

static uint16_t findMaxCell(uint16_t *vals, uint8_t count) {
  uint16_t m = 0; for (uint8_t i=0;i<count;i++) if (vals[i] > m) m = vals[i]; return m;
}
static uint16_t findMinCell(uint16_t *vals, uint8_t count) {
  uint16_t m = 0xFFFF; for (uint8_t i=0;i<count;i++) if (vals[i] < m) m = vals[i]; return m;
}

static void EvaluateBalancing(void) {
  uint32_t tStart = HAL_GetTick();
  printf("[FUNC] EvaluateBalancing BEGIN\n");
  printf("[BAL] Evaluate begin\n");
  // Placeholder simple balancing: compute delta and decide a mask
  uint8_t cellCount = 4; // 4-series pack
  uint16_t vmax = findMaxCell(bq76907_monitor.cellVoltage_mV, cellCount);
  uint16_t vmin = findMinCell(bq76907_monitor.cellVoltage_mV, cellCount);
  uint16_t delta = vmax - vmin;
  static uint8_t balancingActive = 0;

  if (!balancingActive) {
    if (delta > BALANCE_THRESHOLD_MV) {
      uint16_t cutoff = vmin + (delta/2);
      uint8_t mask = 0;
      for (uint8_t i=0;i<cellCount;i++) {
        if (bq76907_monitor.cellVoltage_mV[i] > cutoff) mask |= (1u << i);
      }
      applyCellBalancingMask(mask);
      balancingActive = 1;
    }
  } else {
    if (delta < BALANCE_HYSTERESIS_MV) {
      applyCellBalancingMask(0); // turn off
      balancingActive = 0;
    }
  }
  printf("[BAL] Evaluate end (%lums) delta=%u mV active=%u\n",
    (unsigned long)(HAL_GetTick()-tStart),
    (unsigned)(vmax - vmin),
    (unsigned)balancingActive);
  printf("[FUNC] EvaluateBalancing END\n");
}

static void applyCellBalancingMask(uint8_t mask) {
  printf("[FUNC] applyCellBalancingMask BEGIN\n");
  printf("[BAL] Apply mask=0x%02X\n", mask);
  // Placeholder: would write mask bits into CELLBAL1/2 registers after verification.
  // Splitting across two registers if needed (e.g., lower 3 bits in CELLBAL1, next in CELLBAL2).
  (void)mask; // suppress unused warning until implemented
  printf("[FUNC] applyCellBalancingMask END\n");
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq(); // Disable interrupts to prevent further execution

  // In a critical error, often you halt or reset the system.
  // For a robust system, you might try a limited number of I2C reinitialization attempts.
  // For this example, we'll blink an LED indefinitely to signal an error.

  // Assuming an LED (e.g., on GPIOA, PIN_5) is available for error indication
  // Make sure this pin is initialized in MX_GPIO_Init()
  while (1)
  {
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // Toggle an LED to indicate error
      HAL_Delay(100); // Small delay for visible blinking
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
