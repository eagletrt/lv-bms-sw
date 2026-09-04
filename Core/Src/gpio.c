/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

#include "feedback-api.h"

/*! Voltage reported to the feedback module for a digital pin read as high in V. */
#define GPIO_FEEDBACK_LOGIC_HIGH_V (3.3F)

/*! Voltage reported to the feedback module for a digital pin read as low in V. */
#define GPIO_FEEDBACK_LOGIC_LOW_V (0.F)

/*!
 * \brief           Translate a pin level into the analog level the feedback
 *                  module compares against its thresholds.
 *
 * \param[in]       port The GPIO port of the feedback pin.
 * \param[in]       pin The GPIO pin of the feedback.
 *
 * \returns         volt GPIO_FEEDBACK_LOGIC_HIGH_V or GPIO_FEEDBACK_LOGIC_LOW_V.
 */
static volt prv_gpio_read_feedback(GPIO_TypeDef *port, uint16_t pin) {
    return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET) ? GPIO_FEEDBACK_LOGIC_HIGH_V : GPIO_FEEDBACK_LOGIC_LOW_V;
}

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
void MX_GPIO_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, LED_SW_Pin | OUTPUT_DELAY_SET_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, MUX_A1_MCU_Pin | MUX_A2_MCU_Pin | MUX_A3_MCU_Pin | OUTPUT_EN_Pin | LTC_CS_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, SUPPLY_EN_Pin | SUPPLY_DELAY_FB_Pin | SUPPLY_DELAY_SET_Pin | LED2_Pin | LED1_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, MUX_A0_MCU_Pin | CHARGE_EN_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : SUPPLY_EN_FB_MCU_Pin OUTPUT_DELAY_FB_Pin */
    GPIO_InitStruct.Pin = SUPPLY_EN_FB_MCU_Pin | OUTPUT_DELAY_FB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : LED_SW_Pin OUTPUT_DELAY_SET_Pin */
    GPIO_InitStruct.Pin = LED_SW_Pin | OUTPUT_DELAY_SET_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pins : MUX_A1_MCU_Pin MUX_A2_MCU_Pin MUX_A3_MCU_Pin OUTPUT_EN_Pin
                           LTC_CS_Pin */
    GPIO_InitStruct.Pin = MUX_A1_MCU_Pin | MUX_A2_MCU_Pin | MUX_A3_MCU_Pin | OUTPUT_EN_Pin | LTC_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : SUPPLY_EN_Pin SUPPLY_DELAY_FB_Pin SUPPLY_DELAY_SET_Pin LED2_Pin
                           LED1_Pin */
    GPIO_InitStruct.Pin = SUPPLY_EN_Pin | SUPPLY_DELAY_FB_Pin | SUPPLY_DELAY_SET_Pin | LED2_Pin | LED1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : MUX_A0_MCU_Pin CHARGE_EN_Pin */
    GPIO_InitStruct.Pin = MUX_A0_MCU_Pin | CHARGE_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /*Configure GPIO pins : CHRG_STATUS_FB_Pin CHRG_VIN_VALID_FB_Pin */
    GPIO_InitStruct.Pin = CHRG_STATUS_FB_Pin | CHRG_VIN_VALID_FB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */

void gpio_set_master_relay(bool closed) {
    /*! OUTPUT_EN drives the gate of Q10, which sinks the coil of K1. Driving it
        high energises the coil and closes the contacts, connecting VBAT to VOUT.
        Low de-energises the coil and the relay opens on its own. */
    HAL_GPIO_WritePin(OUTPUT_EN_GPIO_Port, OUTPUT_EN_Pin, closed ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool gpio_is_master_relay_closed(void) {
    return HAL_GPIO_ReadPin(OUTPUT_EN_GPIO_Port, OUTPUT_EN_Pin) == GPIO_PIN_SET;
}

void gpio_update_digital_feedbacks(void) {
    (void)feedback_api_set_analog(FEEDBACK_SUPPLY_ENABLE_NEGATED, prv_gpio_read_feedback(SUPPLY_EN_FB_MCU_GPIO_Port, SUPPLY_EN_FB_MCU_Pin));
    (void)feedback_api_set_analog(FEEDBACK_OUTPUT_DELAY, prv_gpio_read_feedback(OUTPUT_DELAY_FB_GPIO_Port, OUTPUT_DELAY_FB_Pin));
    (void)feedback_api_set_analog(FEEDBACK_CHARGE_STATUS_NEGATED, prv_gpio_read_feedback(CHRG_STATUS_FB_GPIO_Port, CHRG_STATUS_FB_Pin));
    (void)feedback_api_set_analog(FEEDBACK_CHARGE_VIN_VALID_NEGATED, prv_gpio_read_feedback(CHRG_VIN_VALID_FB_GPIO_Port, CHRG_VIN_VALID_FB_Pin));
    /*! SUPPLY_DELAY_FB is configured as an output by the .ioc; reading it back
        still returns the level actually present on the pin (IDR). */
    (void)feedback_api_set_analog(FEEDBACK_SUPPLY_DELAY, prv_gpio_read_feedback(SUPPLY_DELAY_FB_GPIO_Port, SUPPLY_DELAY_FB_Pin));
}

/* USER CODE END 2 */
