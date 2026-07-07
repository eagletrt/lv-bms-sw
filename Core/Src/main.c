/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "fdcan.h"
#include "spi.h"
#include "stm32c0xx_hal.h"
#include "stm32c0xx_hal_gpio.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ltc6810-2-api.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void ltc_delay_discharge_demo(struct Ltc68102Handler *handler) {
    struct Ltc68102Cfgr cfg = { 0 };

    uint8_t tx[32];
    uint8_t rx[32];

    memset(tx, 0xFF, sizeof(tx));
    memset(rx, 0xFF, sizeof(rx));

    /*
     * Enable reference
     * Enable discharge on cells 1-6
     */
    cfg.REFON = 1;

    cfg.DCC = 0x0; // Cells 1-6 discharge enabled

    size_t tx_len =
        ltc6810_2_api_wrcfg_encode_broadcast(
            handler,
            &cfg,
            tx);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port,
                      SPI_LT_CS_Pin,
                      GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi1,
                     tx,
                     tx_len,
                     HAL_MAX_DELAY);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port,
                      SPI_LT_CS_Pin,
                      GPIO_PIN_SET);

    HAL_Delay(200);

    /*
     * Disable discharge
     */
    memset(tx, 0xFF, sizeof(tx));

    cfg.DCC = 0x0; // Cells 1-6 discharge disabled

    tx_len =
        ltc6810_2_api_wrcfg_encode_broadcast(
            handler,
            &cfg,
            tx);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port,
                      SPI_LT_CS_Pin,
                      GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi1,
                     tx,
                     tx_len,
                     HAL_MAX_DELAY);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port,
                      SPI_LT_CS_Pin,
                      GPIO_PIN_SET);
}

static void ltc_read_voltages(struct Ltc68102Handler *handler) {
    uint8_t tx[16];
    uint8_t rx[16];

    uint16_t cells[6];

    /*
     * Start ADC conversion
     */
    memset(tx, 0xFF, sizeof(tx));

    size_t tx_len =
        ltc6810_2_api_adcv_encode_broadcast(
            handler,
            LTC6810_2_MD_27KHZ,
            LTC6810_2_DCP_DISABLED,
            LTC6810_2_CH_ALL,
            tx);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port,
                      SPI_LT_CS_Pin,
                      GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi1,
                     tx,
                     tx_len,
                     HAL_MAX_DELAY);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port,
                      SPI_LT_CS_Pin,
                      GPIO_PIN_SET);

    /*
     * Wait conversion
     */
    HAL_Delay(10);

    /*
     * Read cell voltage group A
     */
    memset(tx, 0xFF, sizeof(tx));
    memset(rx, 0xFF, sizeof(rx));

    tx_len =
        ltc6810_2_api_rdcv_encode_broadcast(
            handler,
            LTC6810_2_CVAR,
            tx);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port,
                      SPI_LT_CS_Pin,
                      GPIO_PIN_RESET);

    HAL_SPI_TransmitReceive(&hspi1,
                            tx,
                            rx,
                            12,
                            HAL_MAX_DELAY);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port,
                      SPI_LT_CS_Pin,
                      GPIO_PIN_SET);

    /*
     * Skip command bytes
     */
    if (ltc6810_2_api_rdcv_decode_broadcast(
            handler,
            &rx[4],
            cells) != 0) {

        char msg[128];

        snprintf(msg,
                 sizeof(msg),
                 "CELL1=%umV CELL2=%umV CELL3=%umV CELL4=%umV CELL5=%umV CELL6=%umV\r\n",
                 cells[0],
                 cells[1],
                 cells[2],
                 cells[3],
                 cells[4],
                 cells[5]);

        HAL_UART_Transmit(&huart1,
                          (uint8_t *)msg,
                          strlen(msg),
                          HAL_MAX_DELAY);
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {

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
    MX_ADC1_Init();
    MX_FDCAN1_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */

    struct Ltc68102Handler ltc_handler;

    ltc6810_2_api_init(&ltc_handler, 1);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1) {

        // HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);

        // constexpr uint8_t payload_size = LTC6810_2_READ_BUFFER_SIZE + LTC6810_2_DATA_BUFFER_SIZE(1);

        // uint8_t tx_payload[payload_size];
        // uint8_t rx_payload[payload_size];
        // for (int i = 0; i < payload_size; i++) {
        //     tx_payload[i] = 0xFF;
        //     rx_payload[i] = 0xFF;
        // }
        // ltc6810_2_api_rdsid_encode_broadcast(&ltc_handler, tx_payload);

        // if (HAL_SPI_TransmitReceive(&hspi1, tx_payload, rx_payload, payload_size, 30) != HAL_OK) {
        //     while (1) {
        //         HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        //         HAL_Delay(100);
        //     }
        // }

        // HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

        // uint8_t decoded[6];
        // size_t dec = ltc6810_2_api_rdsid_decode_broadcast(&ltc_handler, &rx_payload[4], decoded);

        // char buffer[1024];
        // size_t len;

        // /* -------------------- TX -------------------- */

        // len = snprintf(buffer, sizeof(buffer), "Sending:");

        // for (size_t i = 0; i < payload_size && len < sizeof(buffer); i++) {
        //     len += snprintf(buffer + len,
        //                     sizeof(buffer) - len,
        //                     " 0x%02X",
        //                     tx_payload[i]);
        // }

        // len += snprintf(buffer + len,
        //                 sizeof(buffer) - len,
        //                 "\r\n");

        // HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len, HAL_MAX_DELAY);

        // /* -------------------- RX -------------------- */

        // len = snprintf(buffer, sizeof(buffer), "Receiving:");

        // for (size_t i = 0; i < payload_size && len < sizeof(buffer); i++) {
        //     len += snprintf(buffer + len,
        //                     sizeof(buffer) - len,
        //                     " 0x%02X",
        //                     rx_payload[i]);
        // }

        // len += snprintf(buffer + len,
        //                 sizeof(buffer) - len,
        //                 "\r\n");

        // HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len, HAL_MAX_DELAY);

        // /* -------------------- Decoded -------------------- */

        // len = snprintf(buffer,
        //                sizeof(buffer),
        //                "Decoded (%u):",
        //                (unsigned)dec);

        // for (size_t i = 0; i < 6 && len < sizeof(buffer); i++) {
        //     len += snprintf(buffer + len,
        //                     sizeof(buffer) - len,
        //                     " 0x%02X",
        //                     decoded[i]);
        // }

        // len += snprintf(buffer + len,
        //                 sizeof(buffer) - len,
        //                 "\r\n");

        // HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len, HAL_MAX_DELAY);
        //
        ltc_delay_discharge_demo(&ltc_handler);

        ltc_read_voltages(&ltc_handler);

        HAL_Delay(500);

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
