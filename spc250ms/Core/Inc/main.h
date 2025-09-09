/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

#include "stm32g0xx_ll_ucpd.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_dma.h"

#include "stm32g0xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BOOTLOADER_TX_Pin GPIO_PIN_10
#define BOOTLOADER_TX_GPIO_Port GPIOC
#define BOOTLOADER_RX_Pin GPIO_PIN_11
#define BOOTLOADER_RX_GPIO_Port GPIOC
#define MCU_RESET_Pin GPIO_PIN_2
#define MCU_RESET_GPIO_Port GPIOF
#define RS485_TX_Pin GPIO_PIN_0
#define RS485_TX_GPIO_Port GPIOC
#define RS485_RX_Pin GPIO_PIN_1
#define RS485_RX_GPIO_Port GPIOC
#define BI_Pin GPIO_PIN_3
#define BI_GPIO_Port GPIOC
#define ADC_LED_VOLTAGE_4_Pin GPIO_PIN_0
#define ADC_LED_VOLTAGE_4_GPIO_Port GPIOA
#define ADC_LED_VOLTAGE_3_Pin GPIO_PIN_1
#define ADC_LED_VOLTAGE_3_GPIO_Port GPIOA
#define ADC_LED_VOLTAGE_2_Pin GPIO_PIN_2
#define ADC_LED_VOLTAGE_2_GPIO_Port GPIOA
#define ADC_LED_VOLTAGE_1_Pin GPIO_PIN_3
#define ADC_LED_VOLTAGE_1_GPIO_Port GPIOA
#define ADC_BATTERY_NTC_2_Pin GPIO_PIN_4
#define ADC_BATTERY_NTC_2_GPIO_Port GPIOA
#define ADC_SYS_CURRENT_Pin GPIO_PIN_5
#define ADC_SYS_CURRENT_GPIO_Port GPIOA
#define RS485_ENABLE_Pin GPIO_PIN_7
#define RS485_ENABLE_GPIO_Port GPIOA
#define DC_USB_C_60W_I_SENSE_Pin GPIO_PIN_4
#define DC_USB_C_60W_I_SENSE_GPIO_Port GPIOC
#define DC_USB_C_60W_V_SINK_Pin GPIO_PIN_5
#define DC_USB_C_60W_V_SINK_GPIO_Port GPIOC
#define ADC_USB_C_60W_IN_Pin GPIO_PIN_0
#define ADC_USB_C_60W_IN_GPIO_Port GPIOB
#define ADC_USB_C_100W_V_BUS_Pin GPIO_PIN_1
#define ADC_USB_C_100W_V_BUS_GPIO_Port GPIOB
#define ADC_USB_C_100W_V_SOURCE_Pin GPIO_PIN_2
#define ADC_USB_C_100W_V_SOURCE_GPIO_Port GPIOB
#define BMS_INTERRUPT_Pin GPIO_PIN_7
#define BMS_INTERRUPT_GPIO_Port GPIOE
#define PWR_GOOD_VBAT_OUT_Pin GPIO_PIN_8
#define PWR_GOOD_VBAT_OUT_GPIO_Port GPIOE
#define MPPT_BQ_INTERRUPT_Pin GPIO_PIN_9
#define MPPT_BQ_INTERRUPT_GPIO_Port GPIOE
#define MPPT_BQ_CHARGE_ENABLE_Pin GPIO_PIN_10
#define MPPT_BQ_CHARGE_ENABLE_GPIO_Port GPIOE
#define ADC_USB_C_100W_I_SENSE_Pin GPIO_PIN_10
#define ADC_USB_C_100W_I_SENSE_GPIO_Port GPIOB
#define ADC_USB_C_100W_V_SINK_Pin GPIO_PIN_11
#define ADC_USB_C_100W_V_SINK_GPIO_Port GPIOB
#define ADC_USB_C_60W_V_BUS_Pin GPIO_PIN_12
#define ADC_USB_C_60W_V_BUS_GPIO_Port GPIOB
#define ENABLE_LED_BUCK_Pin GPIO_PIN_13
#define ENABLE_LED_BUCK_GPIO_Port GPIOB
#define PWM_LED_CURRENT_1_Pin GPIO_PIN_6
#define PWM_LED_CURRENT_1_GPIO_Port GPIOC
#define FLAG_USB_100W_Pin GPIO_PIN_8
#define FLAG_USB_100W_GPIO_Port GPIOD
#define ENABLE_USB_60W_Pin GPIO_PIN_9
#define ENABLE_USB_60W_GPIO_Port GPIOD
#define FAULT_12V_RS485_Pin GPIO_PIN_10
#define FAULT_12V_RS485_GPIO_Port GPIOD
#define FLAG_USB_60W_Pin GPIO_PIN_11
#define FLAG_USB_60W_GPIO_Port GPIOD
#define CAN_ENABLE_Pin GPIO_PIN_14
#define CAN_ENABLE_GPIO_Port GPIOD
#define DISABLE_LED_CURRENT_Pin GPIO_PIN_15
#define DISABLE_LED_CURRENT_GPIO_Port GPIOD
#define USB_DATA_N_Pin GPIO_PIN_11
#define USB_DATA_N_GPIO_Port GPIOA
#define USB_DATA_P_Pin GPIO_PIN_12
#define USB_DATA_P_GPIO_Port GPIOA
#define PWM_LED_CURRENT_3_Pin GPIO_PIN_15
#define PWM_LED_CURRENT_3_GPIO_Port GPIOA
#define ENABLE_USB_100W_Pin GPIO_PIN_5
#define ENABLE_USB_100W_GPIO_Port GPIOD
#define ENABLE_BUCK_BOOST_100W_Pin GPIO_PIN_6
#define ENABLE_BUCK_BOOST_100W_GPIO_Port GPIOD
#define ENABLE_12V_RS485_Pin GPIO_PIN_7
#define ENABLE_12V_RS485_GPIO_Port GPIOD
#define PWM_LED_CURRENT_2_Pin GPIO_PIN_3
#define PWM_LED_CURRENT_2_GPIO_Port GPIOB
#define ENABLE_DIAG_12V_RS485_Pin GPIO_PIN_4
#define ENABLE_DIAG_12V_RS485_GPIO_Port GPIOB
#define MPPT_BQ_QON_Pin GPIO_PIN_0
#define MPPT_BQ_QON_GPIO_Port GPIOE
#define ENABLE_CURRENT_SENSE_Pin GPIO_PIN_1
#define ENABLE_CURRENT_SENSE_GPIO_Port GPIOE
#define ENABLE_BAT_OUTPUT_Pin GPIO_PIN_3
#define ENABLE_BAT_OUTPUT_GPIO_Port GPIOE
#define PWM_LED_VOLTAGE_FEEDBACK_Pin GPIO_PIN_6
#define PWM_LED_VOLTAGE_FEEDBACK_GPIO_Port GPIOB
#define PWM_LED_CURRENT_4_Pin GPIO_PIN_7
#define PWM_LED_CURRENT_4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
