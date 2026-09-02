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
#include "dma.h"
#include "fdcan.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "fsm.h"
#include "post.h"
#include "can-communication-router-api.h"
#include "eagletrt-api.h"
#include "arena-allocator-api.h"
#include "pal-api.h"
#include "logger-api.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LOGGER_ENABLED (true)          /*!< Logger status: true to enable active logging, false to mute entirely. */
#define LOGGER_RX_CAPACITY (1U)        /*!< Receive queue depth. Set to 1 because the logger is transmit-only but needs to be > 0 because of arena allocator. */
#define LOGGER_TX_CAPACITY (10U)       /*!< Maximum number of log message packets allowed to sit in the outbound transmission queue. */
#define LOGGER_UART_MAX_MSG_SIZE (64U) /*!< Maximum allocation allowed for an individual log string. */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

state_t current_state = STATE_INIT;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

EAGLETRT_STATIC struct ArenaAllocatorHandler arena_allocator_handler;
EAGLETRT_STATIC struct PalHandler logger_pal_handler;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*!
 * \brief Initializes the low-level memory allocation and logging framework.
 */
EAGLETRT_STATIC void prv_main_init_logging_configuration() {
    arena_allocator_api_init(&arena_allocator_handler);

    EAGLETRT_API_UNUSED(pal_api_init(&logger_pal_handler,
                                     LOGGER_RX_CAPACITY,
                                     LOGGER_TX_CAPACITY,
                                     LOGGER_UART_MAX_MSG_SIZE,
                                     NULL,
                                     usart_logger_transmit,
                                     NULL,
                                     NULL,
                                     &arena_allocator_handler));
}
static uint8_t mux_channel = 3;
static uint8_t uart_rx;
static char uart_rx_buffer[8];
static uint8_t uart_rx_index = 0;

static void mux_select_channel(uint8_t channel) {
    channel &= 0x0F;

    HAL_GPIO_WritePin(MUX_A0_MCU_GPIO_Port,
                      MUX_A0_MCU_Pin,
                      (channel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MUX_A1_MCU_GPIO_Port,
                      MUX_A1_MCU_Pin,
                      (channel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MUX_A2_MCU_GPIO_Port,
                      MUX_A2_MCU_Pin,
                      (channel & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MUX_A3_MCU_GPIO_Port,
                      MUX_A3_MCU_Pin,
                      (channel & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
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
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_FDCAN1_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */

    mux_select_channel(0);

    HAL_UART_Receive_IT(&huart1, &uart_rx, 1);

    prv_main_init_logging_configuration();
    EAGLETRT_API_UNUSED(logger_api_init(&logger_pal_handler, LOGGER_ENABLED));

    HAL_FDCAN_Start(&hfdcan1);

    struct PostInitData post_init_data = {
        .can_network_configurations = {
            [CAN_COMMUNICATION_NETWORK_PRIMARY] = {
                .cs_enter = __disable_irq,
                .cs_exit = __enable_irq,
                .on_receive = can_communication_router_api_receive_primary,
                .send = fdcan_send_primary,
            },
        },
        .bms_monitor_send = spi_bms_monitor_send,
        .bms_monitor_send_receive = spi_bms_monitor_send_receive,
        .bms_monitor_ntc_read = nullptr
    };

    struct FsmData fsm_data = {
        .tick = HAL_GetTick()
    };

    current_state = run_state(STATE_INIT, &post_init_data);

    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);

    HAL_GPIO_WritePin(SUPPLY_EN_GPIO_Port, SUPPLY_EN_Pin, GPIO_PIN_SET);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    /* USER CODE BEGIN WHILE */
    uint32_t t = HAL_GetTick();
    uint32_t t_print = HAL_GetTick();

    char uart_tx_buffer[256];

    while (1) {
        uint32_t tick = HAL_GetTick();

        /* LED */
        if (tick - t >= 250) {
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
            t = tick;
        }

        /*! Fa avanzare la macchina a stati ADC (settling -> DMA -> complete),
            gestisce da sola il mux e aggiorna feedback/temperature. */
        adc_routine(tick);

        /*! Stampa la tabella completa ogni 500 ms. */
        if (tick - t_print >= 500) {
            int len = 0;

            len += snprintf(uart_tx_buffer + len, sizeof(uart_tx_buffer) - len, "VDDA=%.2fV VIN=%.2fV VOUT=%.2fV IOUT=%.2fA MCU_T=%.1fC MUXCH=%02u\r\n", adc_get_vdda(), adc_get_vin(), adc_get_vout(), adc_get_output_current(), adc_get_mcu_temperature(), (unsigned)adc_get_current_ntc_channel());

            len += snprintf(uart_tx_buffer + len, sizeof(uart_tx_buffer) - len, "NTC:");
            for (size_t ch = 0; ch < DEFINES_NTC_MUX_USED_CHANNEL_COUNT; ++ch) {
                len += snprintf(uart_tx_buffer + len, sizeof(uart_tx_buffer) - len, " [%02u]=%.3fV", (unsigned)ch, adc_get_ntc_voltage(ch));
            }
            len += snprintf(uart_tx_buffer + len, sizeof(uart_tx_buffer) - len, "\r\n");

            HAL_UART_Transmit(&huart1, (uint8_t *)uart_tx_buffer, len, 100);

            t_print = tick;
        }

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
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        /*
         * Accetta numeri ASCII.
         *
         * Esempio:
         * 0 + INVIO
         * 5 + INVIO
         * 12 + INVIO
         */

        if (uart_rx >= '0' && uart_rx <= '9') {
            if (uart_rx_index < sizeof(uart_rx_buffer) - 1) {
                uart_rx_buffer[uart_rx_index++] = uart_rx;
            }
        } else if (uart_rx == '\r' || uart_rx == '\n') {
            if (uart_rx_index > 0) {
                uart_rx_buffer[uart_rx_index] = '\0';

                uint8_t new_channel =
                    (uint8_t)atoi(uart_rx_buffer);

                if (new_channel <= 15) {
                    mux_channel = new_channel;

                    mux_select_channel(mux_channel);
                }

                uart_rx_index = 0;
            }
        }

        HAL_UART_Receive_IT(&huart1, &uart_rx, 1);
    }
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
