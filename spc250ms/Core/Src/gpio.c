/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, MPPT_BQ_CHARGE_ENABLE_Pin|MPPT_BQ_QON_Pin|ENABLE_CURRENT_SENSE_Pin|ENABLE_BAT_OUTPUT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ENABLE_LED_BUCK_Pin|PWM_LED_CURRENT_2_Pin|ENABLE_DIAG_12V_RS485_Pin|PWM_LED_VOLTAGE_FEEDBACK_Pin
                          |PWM_LED_CURRENT_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PWM_LED_CURRENT_1_GPIO_Port, PWM_LED_CURRENT_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ENABLE_USB_60W_Pin|CAN_ENABLE_Pin|DISABLE_LED_CURRENT_Pin|ENABLE_USB_100W_Pin
                          |ENABLE_BUCK_BOOST_100W_Pin|ENABLE_12V_RS485_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PWM_LED_CURRENT_3_GPIO_Port, PWM_LED_CURRENT_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : MCU_RESET_Pin */
  GPIO_InitStruct.Pin = MCU_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MCU_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BI_Pin */
  GPIO_InitStruct.Pin = BI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BMS_INTERRUPT_Pin MPPT_BQ_INTERRUPT_Pin */
  GPIO_InitStruct.Pin = BMS_INTERRUPT_Pin|MPPT_BQ_INTERRUPT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PWR_GOOD_VBAT_OUT_Pin */
  GPIO_InitStruct.Pin = PWR_GOOD_VBAT_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PWR_GOOD_VBAT_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MPPT_BQ_CHARGE_ENABLE_Pin MPPT_BQ_QON_Pin ENABLE_CURRENT_SENSE_Pin ENABLE_BAT_OUTPUT_Pin */
  GPIO_InitStruct.Pin = MPPT_BQ_CHARGE_ENABLE_Pin|MPPT_BQ_QON_Pin|ENABLE_CURRENT_SENSE_Pin|ENABLE_BAT_OUTPUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : ENABLE_LED_BUCK_Pin PWM_LED_CURRENT_2_Pin ENABLE_DIAG_12V_RS485_Pin PWM_LED_VOLTAGE_FEEDBACK_Pin
                           PWM_LED_CURRENT_4_Pin */
  GPIO_InitStruct.Pin = ENABLE_LED_BUCK_Pin|PWM_LED_CURRENT_2_Pin|ENABLE_DIAG_12V_RS485_Pin|PWM_LED_VOLTAGE_FEEDBACK_Pin
                          |PWM_LED_CURRENT_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PWM_LED_CURRENT_1_Pin */
  GPIO_InitStruct.Pin = PWM_LED_CURRENT_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PWM_LED_CURRENT_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FLAG_USB_100W_Pin FAULT_12V_RS485_Pin FLAG_USB_60W_Pin */
  GPIO_InitStruct.Pin = FLAG_USB_100W_Pin|FAULT_12V_RS485_Pin|FLAG_USB_60W_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : ENABLE_USB_60W_Pin CAN_ENABLE_Pin DISABLE_LED_CURRENT_Pin ENABLE_USB_100W_Pin
                           ENABLE_BUCK_BOOST_100W_Pin ENABLE_12V_RS485_Pin */
  GPIO_InitStruct.Pin = ENABLE_USB_60W_Pin|CAN_ENABLE_Pin|DISABLE_LED_CURRENT_Pin|ENABLE_USB_100W_Pin
                          |ENABLE_BUCK_BOOST_100W_Pin|ENABLE_12V_RS485_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PWM_LED_CURRENT_3_Pin */
  GPIO_InitStruct.Pin = PWM_LED_CURRENT_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PWM_LED_CURRENT_3_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
