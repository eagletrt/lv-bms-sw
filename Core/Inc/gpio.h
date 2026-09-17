/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include <stdbool.h>

#include "types.h"

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/*!
 * \brief           Open or close the master output relay K1.
 *
 * \details         Closing it connects the pack to VOUT through the 20 A fuse, so
 *                  it is only ever done once the pre-flight checks in the FSM have
 *                  passed. MX_GPIO_Init() leaves it open, which is the safe state
 *                  at reset.
 *
 * \param[in]       closed True to energise the coil and connect the output, false
 *                  to release it.
 */
void gpio_set_master_relay(bool closed);

/*!
 * \brief           Tell whether the master relay coil is currently energised.
 *
 * \details         Reads the drive pin back, not the contacts. Use the
 *                  FEEDBACK_OUTPUT_ENABLE_NEGATED and FEEDBACK_VOUT feedbacks to
 *                  confirm the relay actually followed.
 *
 * \returns         bool True if the coil is being driven.
 */
bool gpio_is_master_relay_closed(void);

/*!
 * \brief           Sample the digital feedback pins and publish them to the
 *                  feedback module.
 *
 * \details         Covers the feedbacks that are plain logic inputs; the analog
 *                  ones are refreshed by adc_routine().
 */
void gpio_update_digital_feedbacks(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */
