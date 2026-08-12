/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
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
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN Private defines */

enum AdcRead {
    ADC_READ_T5,
    ADC_READ_T4,
    ADC_READ_T3,
    ADC_READ_OUTPUT_EN_FB,
    ADC_READ_OUT_FUSE_FB,
    ADC_READ_T0,
    ADC_READ_I_OUT,
    ADC_READ_LVMS_OUT,
    ADC_READ_T7,
    ADC_READ_T6,
    ADC_READ_T2,
    ADC_READ_T1,
    ADC_READ_COUNT
};

/* USER CODE END Private defines */

void MX_ADC1_Init(void);

/* USER CODE BEGIN Prototypes */

void adc_start_read(void);

volatile float *get_adc_voltages(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
