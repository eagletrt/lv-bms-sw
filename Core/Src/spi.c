/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
#include "logger-api.h"
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void) {

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
    if (spiHandle->Instance == SPI1) {
        /* USER CODE BEGIN SPI1_MspInit 0 */

        /* USER CODE END SPI1_MspInit 0 */

        /** Initializes the peripherals clocks
  */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2S1;
        PeriphClkInit.I2s1ClockSelection = RCC_I2S1CLKSOURCE_SYSCLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        /* SPI1 clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**SPI1 GPIO Configuration
    PA15     ------> SPI1_NSS
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
        GPIO_InitStruct.Pin = GPIO_PIN_15;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN SPI1_MspInit 1 */

        /* USER CODE END SPI1_MspInit 1 */
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle) {

    if (spiHandle->Instance == SPI1) {
        /* USER CODE BEGIN SPI1_MspDeInit 0 */

        /* USER CODE END SPI1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /**SPI1 GPIO Configuration
    PA15     ------> SPI1_NSS
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

        /* USER CODE BEGIN SPI1_MspDeInit 1 */

        /* USER CODE END SPI1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

enum BmsMonitorReturnCode spi_bms_monitor_send(uint8_t *const data, const size_t size) {
    logger_api_log(LOGGER_LEVEL_DEBUG, "SPI SEND: %2d. ", size);
    for (uint16_t i = 0; i < size; ++i) {
        logger_api_log(LOGGER_LEVEL_DEBUG, "%02hx ", data[i]);
    }
    logger_api_log(LOGGER_LEVEL_DEBUG, "\r\n");
    enum BmsMonitorReturnCode code = BMS_MONITOR_RC_ERROR;
    uint16_t timeout = size * 5U;

    // TODO: Non-blocking or set a decent enough timeout
    const HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, data, size, timeout);
    switch (status) {
        case HAL_TIMEOUT:
        case HAL_ERROR:
            code = BMS_MONITOR_RC_COMMUNICATION_ERROR;
            break;
        case HAL_BUSY:
            code = BMS_MONITOR_RC_BUSY;
            break;
        case HAL_OK:
            code = BMS_MONITOR_RC_OK;
            break;
        default:
            code = BMS_MONITOR_RC_ERROR;
            break;
    }
    return code;
}

enum BmsMonitorReturnCode spi_bms_monitor_send_receive(uint8_t *const data, uint8_t *out, const size_t size, const size_t out_size) {
    enum BmsMonitorReturnCode code = BMS_MONITOR_RC_ERROR;
    uint16_t timeout_tx = size * 5U;
    uint16_t timeout_rx = out_size * 5U;

    // TODO: Non-blocking or set a decent enough timeout
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, data, size, timeout_tx);
    switch (status) {
        case HAL_TIMEOUT:
        case HAL_ERROR:
            code = BMS_MONITOR_RC_COMMUNICATION_ERROR;
            break;
        case HAL_BUSY:
            code = BMS_MONITOR_RC_BUSY;
            break;
        case HAL_OK:
            code = BMS_MONITOR_RC_OK;
            break;
        default:
            code = BMS_MONITOR_RC_ERROR;
            break;
    }

    if (code != BMS_MONITOR_RC_OK) {
        return code;
    }

    status = HAL_SPI_Receive(&hspi1, out, out_size, timeout_rx);
    switch (status) {
        case HAL_TIMEOUT:
        case HAL_ERROR:
            code = BMS_MONITOR_RC_COMMUNICATION_ERROR;
            break;
        case HAL_BUSY:
            code = BMS_MONITOR_RC_BUSY;
            break;
        case HAL_OK:
            code = BMS_MONITOR_RC_OK;
            break;
        default:
            code = BMS_MONITOR_RC_ERROR;
            break;
    }

    logger_api_log(LOGGER_LEVEL_DEBUG, "SPI SEND-RECEIVE: %2d. ", size);
    for (uint16_t i = 0; i < size; ++i) {
        logger_api_log(LOGGER_LEVEL_DEBUG, "%02hx ", data[i]);
    }

    logger_api_log(LOGGER_LEVEL_DEBUG, "%2d. ", out_size);
    for (uint16_t i = 0; i < out_size; ++i) {
        logger_api_log(LOGGER_LEVEL_DEBUG, "%02hx ", out[i]);
    }
    logger_api_log(LOGGER_LEVEL_DEBUG, "\r\n");

    return BMS_MONITOR_RC_OK;
}

/* USER CODE END 1 */
