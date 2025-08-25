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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BQ_UPDATE_INTERVAL_MS   500 // Update BQ25798 status every 500 milliseconds
#define ERROR_LED_BLINK_RATE_MS 200 // Blink the error LED every 200 milliseconds
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
BQ25798 bq25798_charger; // Declare a BQ25798 charger instance
static uint32_t last_bq_update_tick = 0; // Last time BQ25798 status was updated
static uint32_t last_error_led_toggle_tick = 0; // Last time the error LED was toggled
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  // Initialize the BQ25798 charger
  if (BQ25798_init(&bq25798_charger, &hi2c1) != 0) {
      // Initialization failed, immediately enter error state
      Error_Handler();
  }

  // Initialize last update tick to current time for immediate first update
  last_bq_update_tick = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // --- Non-blocking BQ25798 Status Update ---
      // Check if BQ_UPDATE_INTERVAL_MS has passed since the last update
      if ((HAL_GetTick() - last_bq_update_tick) >= BQ_UPDATE_INTERVAL_MS)
      {
          // Reset the timer for the next update
          last_bq_update_tick = HAL_GetTick();

          // Declare a status variable for each read operation
          uint8_t read_status_byte = 0;

          // Read all status registers and populate the bq25798_charger struct
          // Check return values for I2C errors for more robust error handling if needed
          readChargerStatus0(&bq25798_charger, &read_status_byte);
          readChargerStatus1(&bq25798_charger, &read_status_byte);
          readChargerStatus2(&bq25798_charger, &read_status_byte);
          readChargerStatus3(&bq25798_charger, &read_status_byte);
          readChargerStatus4(&bq25798_charger, &read_status_byte);
          readFaultStatus0(&bq25798_charger, &read_status_byte);
          readFaultStatus1(&bq25798_charger, &read_status_byte);

          // --- Read ADC values to update voltage and current data ---
          BQ25798_readBusVoltage(&bq25798_charger);
          BQ25798_readBusCurrent(&bq25798_charger);
          BQ25798_readBatteryVoltage(&bq25798_charger);
          BQ25798_readBatteryCurrent(&bq25798_charger);

          // --- Example: Act on the status flags (Green LED for battery presence) ---
          if (bq25798_charger.chargerStatus2.vbat_present_stat == 1) {
              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // Green LED ON (assuming active low)
          } else {
              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   // Green LED OFF
          }

          // You can add more logic here based on other status flags
          // For example, if (bq25798_charger.chargerStatus1.chg_stat == 0x3) { /* Fast charging */ }
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
