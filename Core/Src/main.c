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
#include "ltc6810-2.h"
#include "spi.h"
#include "stm32c0xx_hal.h"
#include "stm32c0xx_hal_gpio.h"
#include "stm32c0xx_hal_spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ltc6810-2-api.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_ENABLE_FUNC_PRINT
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifdef UART_ENABLE_FUNC_PRINT
#define UART_ENTERING_FUNC()
#define UART_EXITING_FUNC()
#else /*! UART_ENABLE_FUNC_PRINT */
#define UART_ENTERING_FUNC() EAGLETRT_API_NOP()
#define UART_EXITING_FUNC() EAGLETRT_API_NOP()
#endif /*! UART_ENABLE_FUNC_PRINT */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void uart_printf(const char *fmt, ...);
static void ltc_delay_discharge_demo(struct Ltc68102Handler *handler);
static void ltc_read_voltages(struct Ltc68102Handler *handler);
static void ltc_write_config(struct Ltc68102Handler *handler, struct Ltc68102Cfgr *cfg, uint8_t *encoded);
static size_t ltc_read_config(struct Ltc68102Handler *handler, struct Ltc68102Cfgr *cfg, uint8_t *decoded);
static void ltc_read_id(struct Ltc68102Handler *handler);
static void ltc_read_status_palle(struct Ltc68102Handler *handler);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

    struct Ltc68102Handler ltc_handler = { 0 };
    ltc6810_2_api_init(&ltc_handler, 1);

    // struct Ltc68102Cfgr cfg = { .REFON = 1 };
    // uint8_t cfg_tx[32] = { 0 };
    // memset(cfg_tx, 0xFF, sizeof(cfg_tx));

    // size_t cfg_len = ltc6810_2_api_wrcfg_encode_broadcast(&ltc_handler, &cfg, cfg_tx);
    // HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
    // HAL_SPI_Transmit(&hspi1, cfg_tx, cfg_len, 30);
    // HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    // not working on adjacent cells!!!!
    // struct Ltc68102Cfgr requested_config = {
    //     .REFON = 1,
    //     .DCC = 0b0000000,
    //     .DCTO = 0
    // };
    // struct Ltc68102Cfgr actual_config = { 0 };
    //
    // uint8_t encoded[LTC6810_2_WRITE_BUFFER_SIZE(1)];
    // ltc6810_2_api_unmute_encode_broadcast(&ltc_handler, encoded);
    // HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
    // HAL_SPI_Transmit(&hspi1, encoded, LTC6810_2_WRITE_BUFFER_SIZE(1), 10);
    // HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

    uint32_t t = 0;

    HAL_GPIO_WritePin(SUPPLY_EN_GPIO_Port, SUPPLY_EN_Pin, GPIO_PIN_SET);

    while (1) {
        if (HAL_GetTick() - t >= 1000) {
            // ltc_read_id(&ltc_handler);
            // struct Ltc68102Cfgr requested_config = {
            //     .REFON = 1,
            //     .DCC = 0b1010101,
            //     .DCTO = 0
            // };
            // ltc_write_config(&ltc_handler, &requested_config, NULL);
            ltc_read_voltages(&ltc_handler);
            t = HAL_GetTick();
        }

        // ltc_read_status_palle(&ltc_handler);
        // ltc_delay_discharge_demo(&ltc_handler);
        // uint8_t encoded[LTC6810_2_WRITE_BUFFER_SIZE(1)];
        // uint8_t decoded[LTC6810_2_DATA_BUFFER_SIZE(1)];
        // ltc_write_config(&ltc_handler, &requested_config, encoded);
        //
        // HAL_Delay(1);
        //
        // ltc_read_config(&ltc_handler, &actual_config, decoded);
        // ltc_read_voltages(&ltc_handler);
        //
        // HAL_Delay(1);
        //
        // if (HAL_GetTick() - t > 1000) {
        // uart_printf("Encoded: ");
        // for (size_t i = 0; i < LTC6810_2_WRITE_BUFFER_SIZE(1); ++i) {
        //     uart_printf("%02X ", encoded[i]);
        // }
        // uart_printf("\r\n");
        // uart_printf("Decoded: ");
        // for (size_t i = 0; i < LTC6810_2_DATA_BUFFER_SIZE(1); ++i) {
        //     uart_printf("%02X ", decoded[i]);
        // }
        // uart_printf("\r\n");
        // uart_printf("\r\n");
        //
        // uart_printf("Write: REFON=%u DCC=%u DCTO=%u\r\n", requested_config.REFON, requested_config.DCC, requested_config.DCTO);
        // uart_printf("Read: REFON=%u DCC=%u DCTO=%u\r\n", actual_config.REFON, actual_config.DCC, actual_config.DCTO);
        // uart_printf("\r\n");
        //     t = HAL_GetTick();
        // }

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
void uart_printf(const char *fmt, ...) {
    static constexpr size_t bufflen = 1024U;
    static uint8_t buff[bufflen];
    va_list args;
    va_start(args, fmt);

    int len = vsnprintf((char *)buff, bufflen, fmt, args);
    va_end(args);

    if (len > 0) {
        HAL_UART_Transmit(&huart1, buff, (uint16_t)len, HAL_MAX_DELAY);
    }
}

typedef enum {
    LTC_STATE_OFF,
    LTC_STATE_ON
} DischargeState_t;

static uint32_t last_discharge_time = 0;
static DischargeState_t current_state = LTC_STATE_OFF;

static void ltc_delay_discharge_demo(struct Ltc68102Handler *handler) {
    UART_ENTERING_FUNC();

    uint32_t now = HAL_GetTick();

    static uint8_t tx[LTC6810_2_WRITE_BUFFER_SIZE(1)];
    static size_t tx_len = 0;

    switch (current_state) {
        case LTC_STATE_OFF:
            if (now - last_discharge_time >= 1000) {
                struct Ltc68102Cfgr cfg = {
                    .REFON = 1,
                    .DCC = 0x07,
                    .DCTO = LTC6810_2_DCTO_30S
                };

                memset(tx, 0xFF, sizeof(tx));
                tx_len = ltc6810_2_api_wrcfg_encode_broadcast(handler, &cfg, tx);

                last_discharge_time = now;
                current_state = LTC_STATE_ON;
                uart_printf("=== Discharge ON ====================== %u\n\r", tx_len);
            }
            break;

        case LTC_STATE_ON:
            if (now - last_discharge_time >= 1000) {
                struct Ltc68102Cfgr cfg = {
                    .REFON = 1,
                    .DCC = 0x00,
                    .DCTO = LTC6810_2_DCTO_OFF
                };

                memset(tx, 0xFF, sizeof(tx));
                tx_len = ltc6810_2_api_wrcfg_encode_broadcast(handler, &cfg, tx);

                last_discharge_time = now;
                current_state = LTC_STATE_OFF;
                uart_printf("===  Discharge OFF ====================== %u\n\r", tx_len);
            }
            break;
    }

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx, tx_len, 5);
    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

    UART_EXITING_FUNC();
}

static void ltc_read_id(struct Ltc68102Handler *handler) {
    UART_ENTERING_FUNC();

    constexpr uint8_t tx_size = LTC6810_2_READ_BUFFER_SIZE;
    constexpr uint8_t rx_size = LTC6810_2_DATA_BUFFER_SIZE(1);

    uint8_t tx_payload[tx_size];
    uint8_t rx_payload[rx_size];
    memset(tx_payload, 0xFF, tx_size);
    memset(rx_payload, 0xFF, rx_size);

    ltc6810_2_api_rdsid_encode_broadcast(handler, tx_payload);
    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_payload, tx_size, 30);
    HAL_SPI_Receive(&hspi1, rx_payload, rx_size, 30);
    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

    uart_printf("--- ID --- \r\n");
    uart_printf("  Tx: ");
    for (size_t i = 0; i < tx_size; ++i) {
        uart_printf("%02X ", tx_payload[i]);
    }
    uart_printf("\r\n");
    uart_printf("  Rx: ");
    for (size_t i = 0; i < rx_size; ++i) {
        uart_printf("%02X ", rx_payload[i]);
    }
    uart_printf("\r\n");

    constexpr uint16_t id_size = 6;
    uint8_t decoded[id_size] = { 0 };
    ltc6810_2_api_rdsid_decode_broadcast(handler, rx_payload, decoded);
    uart_printf("  Id: ");
    for (size_t i = 0; i < id_size; ++i) {
        uart_printf("%02X ", decoded[i]);
    }
    uart_printf("\r\n\r\n");

    UART_EXITING_FUNC();
}

static void ltc_write_config(struct Ltc68102Handler *handler, struct Ltc68102Cfgr *cfg, uint8_t *encoded) {
    UART_ENTERING_FUNC();

    uint8_t cmd[LTC6810_2_WRITE_BUFFER_SIZE(1)];
    memset(cmd, 0xFF, LTC6810_2_WRITE_BUFFER_SIZE(1));

    size_t encode_byte_count = ltc6810_2_api_wrcfg_encode_broadcast(handler, cfg, cmd);
    if (encoded != NULL) {
        memcpy(encoded, cmd, encode_byte_count);
    }

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, cmd, encode_byte_count, 10);
    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

    UART_EXITING_FUNC();
}

static size_t ltc_read_config(struct Ltc68102Handler *handler, struct Ltc68102Cfgr *cfg, uint8_t *decoded) {
    UART_ENTERING_FUNC();

    uint8_t cmd[LTC6810_2_READ_BUFFER_SIZE];
    uint8_t data[LTC6810_2_DATA_BUFFER_SIZE(1)];
    memset(cmd, 0xFF, LTC6810_2_READ_BUFFER_SIZE);
    memset(data, 0xFF, LTC6810_2_DATA_BUFFER_SIZE(1));

    size_t encode_byte_count = ltc6810_2_api_rdcfg_encode_broadcast(handler, cmd);

    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, cmd, encode_byte_count, 5);
    HAL_SPI_Receive(&hspi1, data, LTC6810_2_DATA_BUFFER_SIZE(1), 10);
    HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

    memcpy(decoded, data, LTC6810_2_DATA_BUFFER_SIZE(1));

    return ltc6810_2_api_rdcfg_decode_broadcast(handler, data, cfg);
}

static uint32_t last_print_time = 0;

static void ltc_read_voltages(struct Ltc68102Handler *handler) {
    UART_ENTERING_FUNC();

    constexpr uint16_t cells_count = 6;
    uint16_t cells[cells_count] = { 0 };

    // Start conversion
    {
        uint8_t cmd[LTC6810_2_POLL_BUFFER_SIZE];
        memset(cmd, 0xFF, LTC6810_2_POLL_BUFFER_SIZE);
        size_t cmd_len = ltc6810_2_api_adcv_encode_broadcast(handler, LTC6810_2_MD_7KHZ, LTC6810_2_DCP_DISABLED, LTC6810_2_CH_ALL, cmd);
        HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi1, cmd, cmd_len, 7);
        HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);
    }

    HAL_Delay(1);

    // Read first register
    {
        uint8_t cmd[LTC6810_2_READ_BUFFER_SIZE];
        uint8_t data[LTC6810_2_DATA_BUFFER_SIZE(1)];
        memset(cmd, 0xFF, LTC6810_2_READ_BUFFER_SIZE);
        memset(data, 0xFF, LTC6810_2_DATA_BUFFER_SIZE(1));

        ltc6810_2_api_rdcv_encode_broadcast(handler, LTC6810_2_CVAR, cmd);
        HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi1, cmd, LTC6810_2_READ_BUFFER_SIZE, 5);
        HAL_SPI_Receive(&hspi1, data, LTC6810_2_DATA_BUFFER_SIZE(1), 5);
        HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

        if (HAL_GetTick() - last_print_time > 100) {
            uart_printf(" --- RDCVA ---\r\n");
            uart_printf("  Rx: ");
            for (size_t i = 0; i < LTC6810_2_DATA_BUFFER_SIZE(1); ++i) {
                uart_printf("%02X ", data[i]);
            }
            uart_printf("\r\n");
            size_t dec_a = ltc6810_2_api_rdcv_decode_broadcast(handler, data, &cells[0]);
            uart_printf("  PEC: %s\r\n\r\n", dec_a ? "OK" : "FAIL"); /* Read group B: cells 4-6 -> cells[3..5] */
        }
    }

    // Read second register
    {
        uint8_t cmd[LTC6810_2_READ_BUFFER_SIZE];
        uint8_t data[LTC6810_2_DATA_BUFFER_SIZE(1)];
        memset(cmd, 0xFF, LTC6810_2_READ_BUFFER_SIZE);
        memset(cmd, 0xFF, LTC6810_2_DATA_BUFFER_SIZE(1));

        ltc6810_2_api_rdcv_encode_broadcast(handler, LTC6810_2_CVBR, cmd);
        HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_Transmit(&hspi1, cmd, LTC6810_2_READ_BUFFER_SIZE, 5);
        HAL_SPI_Receive(&hspi1, data, LTC6810_2_DATA_BUFFER_SIZE(1), 5);
        HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

        if (HAL_GetTick() - last_print_time > 100) {
            uart_printf(" --- RDCVB ---\r\n");
            uart_printf("  Rx: ");
            for (size_t i = 0; i < LTC6810_2_DATA_BUFFER_SIZE(1); ++i) {
                uart_printf("%02X ", data[i]);
            }
            uart_printf("\r\n");
            size_t dec_b = ltc6810_2_api_rdcv_decode_broadcast(handler, data, &cells[3]);
            uart_printf("  PEC: %s\r\n\r\n", dec_b ? "OK" : "FAIL");
        }
    }

    if (HAL_GetTick() - last_print_time > 100) {
        uart_printf(
            "CELL1=%fmV CELL2=%fmV CELL3=%fmV CELL4=%fmV CELL5=%fmV CELL6=%fmV\r\n",
            cells[0] * 0.0001F,
            cells[1] * 0.0001F,
            cells[2] * 0.0001F,
            cells[3] * 0.0001F,
            cells[4] * 0.0001F,
            cells[5] * 0.0001F);
        last_print_time = HAL_GetTick();
    }

    UART_EXITING_FUNC();
}

static void ltc_read_status_palle(struct Ltc68102Handler *handler) {
    UART_ENTERING_FUNC();

    EAGLETRT_API_UNUSED(handler);
    uart_printf("non fa un cazzo lol\r\n");
    // static constexpr size_t payload_size = 32;
    // static uint8_t *tx_payload[payload_size];
    // static uint8_t *rx_payload[payload_size];

    // memset(tx_payload, 0xFF, payload_size);
    // memset(rx_payload, 0xFF, payload_size);

    // HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_RESET);
    // HAL_SPI_TransmitReceive(&hspi1, tx, rx, 12, HAL_MAX_DELAY);
    // HAL_GPIO_WritePin(SPI_LT_CS_GPIO_Port, SPI_LT_CS_Pin, GPIO_PIN_SET);

    UART_EXITING_FUNC();
}
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
