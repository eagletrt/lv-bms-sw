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

/*! Voltage reported to the feedback module for a digital pin read as high in V. */
#define GPIO_FEEDBACK_LOGIC_HIGH_V (3.3F)

/*! Voltage reported to the feedback module for a digital pin read as low in V. */
#define GPIO_FEEDBACK_LOGIC_LOW_V (0.F)

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/*!
 * \brief            Open or close the master output relay K1.
 *
 * \details          Energising connects the pack to VOUT through the 20 A fuse; only after pre-flight, MX_GPIO_Init leaves it open (safe reset state).
 *
 * \param[in]        closed True to energise the coil and connect the output, false
 *                   to release it.
 */
void gpio_set_master_relay(bool closed);

/*!
 * \brief            Tell whether the master relay coil is currently energised.
 *
 * \details          Reads the drive pin, not the contacts; confirm follow-through on the negated EN and VOUT feedbacks.
 *
 * \returns          bool True if the coil is being driven.
 */
bool gpio_is_master_relay_closed(void);

/*!
 * \brief            Sample the digital feedback pins and publish them to the
 *                   feedback module.
 *
 * \details          Only plain logic inputs; the analog feedbacks refresh in adc_routine().
 */
void gpio_update_digital_feedbacks(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */
