/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32c0xx_hal.h"

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
#define SUPPLY_EN_FB_MCU_Pin GPIO_PIN_13
#define SUPPLY_EN_FB_MCU_GPIO_Port GPIOC
#define LED_SW_Pin GPIO_PIN_15
#define LED_SW_GPIO_Port GPIOC
#define V5V_SENSE_MCU_Pin GPIO_PIN_0
#define V5V_SENSE_MCU_GPIO_Port GPIOA
#define VSUP_SENSE_MCU_Pin GPIO_PIN_1
#define VSUP_SENSE_MCU_GPIO_Port GPIOA
#define VOUT_FB_MCU_Pin GPIO_PIN_2
#define VOUT_FB_MCU_GPIO_Port GPIOA
#define T1_Pin GPIO_PIN_3
#define T1_GPIO_Port GPIOA
#define T2_Pin GPIO_PIN_4
#define T2_GPIO_Port GPIOA
#define T3_Pin GPIO_PIN_5
#define T3_GPIO_Port GPIOA
#define OUTPUT_EN_FB_MCU_Pin GPIO_PIN_6
#define OUTPUT_EN_FB_MCU_GPIO_Port GPIOA
#define OUT_FUSE_FB_MCU_Pin GPIO_PIN_7
#define OUT_FUSE_FB_MCU_GPIO_Port GPIOA
#define I_OUT_SENSE_MCU_Pin GPIO_PIN_0
#define I_OUT_SENSE_MCU_GPIO_Port GPIOB
#define LVMS_OUT_SENSE_MCU_Pin GPIO_PIN_1
#define LVMS_OUT_SENSE_MCU_GPIO_Port GPIOB
#define T4_Pin GPIO_PIN_2
#define T4_GPIO_Port GPIOB
#define VIN_UNFUSED_SENSE_MCU_Pin GPIO_PIN_10
#define VIN_UNFUSED_SENSE_MCU_GPIO_Port GPIOB
#define I_CHRG_SENSE_MCU_Pin GPIO_PIN_11
#define I_CHRG_SENSE_MCU_GPIO_Port GPIOB
#define V_CHRG_SENSE_MCU_Pin GPIO_PIN_12
#define V_CHRG_SENSE_MCU_GPIO_Port GPIOB
#define SUPPLY_EN_Pin GPIO_PIN_13
#define SUPPLY_EN_GPIO_Port GPIOB
#define SUPPLY_DELAY_FB_Pin GPIO_PIN_14
#define SUPPLY_DELAY_FB_GPIO_Port GPIOB
#define SUPPLY_DELAY_SET_Pin GPIO_PIN_15
#define SUPPLY_DELAY_SET_GPIO_Port GPIOB
#define VIN_SENSE_MCU_Pin GPIO_PIN_8
#define VIN_SENSE_MCU_GPIO_Port GPIOA
#define OUTPUT_EN_Pin GPIO_PIN_9
#define OUTPUT_EN_GPIO_Port GPIOA
#define OUTPUT_DELAY_SET_Pin GPIO_PIN_6
#define OUTPUT_DELAY_SET_GPIO_Port GPIOC
#define OUTPUT_DELAY_FB_Pin GPIO_PIN_7
#define OUTPUT_DELAY_FB_GPIO_Port GPIOC
#define USART_RX_Pin GPIO_PIN_10
#define USART_RX_GPIO_Port GPIOA
#define CHRG_STATUS_FB_Pin GPIO_PIN_1
#define CHRG_STATUS_FB_GPIO_Port GPIOD
#define CHRG_VIN_VALID_FB_Pin GPIO_PIN_2
#define CHRG_VIN_VALID_FB_GPIO_Port GPIOD
#define CHARGE_EN_Pin GPIO_PIN_3
#define CHARGE_EN_GPIO_Port GPIOD
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_7
#define LED2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
