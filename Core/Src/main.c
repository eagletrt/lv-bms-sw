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
#include "defines.h"

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
#define HEARTBEAT_PERIOD_MS (500U)     /*!< Toggling period of the heartbeat LED. */
#define FEEDBACK_POLL_PERIOD_MS (10U)  /*!< Sampling period of the digital feedbacks. */
#define CONSOLE_RX_BUFFER_SIZE (8U)    /*!< Longest console command accepted, in characters. */
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
/*!
 * \brief Console state used by the NTC multiplexer command.
 *
 * \details The serial console accepts one command, typed on the same USART the
 *          logger prints on:
 *            - a number 0..15 followed by ENTER pins the multiplexer to that
 *              channel, so a single NTC can be watched;
 *            - 'a' releases it and the multiplexer goes back to walking every
 *              channel by itself.
 *          Anything else is ignored. The actual pinning is done by the ADC
 *          module, which owns the address lines, so the command and the scan
 *          loop no longer fight over them.
 */
EAGLETRT_STATIC uint8_t console_rx_char;
EAGLETRT_STATIC char console_rx_buffer[CONSOLE_RX_BUFFER_SIZE];
EAGLETRT_STATIC uint8_t console_rx_index = 0U;

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

    /*! Arm the console. Needs USART1_IRQn, enabled in HAL_UART_MspInit(). */
    HAL_UART_Receive_IT(&huart1, &console_rx_char, 1U);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    uint32_t heartbeat_tick = HAL_GetTick();
    uint32_t feedback_tick = HAL_GetTick();

    adc_start_read();

    while (1) {
        const uint32_t tick = HAL_GetTick();

        fsm_data.tick = tick;
        current_state = run_state(current_state, &fsm_data);

        /* Drive the ADC scan and the NTC multiplexer. One scan samples one
           multiplexer channel, so the whole pack is refreshed every
           DEFINES_NTC_MUX_USED_CHANNEL_COUNT scans. The snapshot of the board is
           printed by the FSM debug interface (prv_print_debug). */
        adc_routine(tick);

        if (tick - feedback_tick >= FEEDBACK_POLL_PERIOD_MS) {
            feedback_tick = tick;
            gpio_update_digital_feedbacks();
        }

        if (tick - heartbeat_tick >= HEARTBEAT_PERIOD_MS) {
            heartbeat_tick = tick;
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
            /* LED1 mirrors the health of the board: off while the FSM runs
               normally, steady on once it has fallen into the fatal state. */
            HAL_GPIO_WritePin(LED1_GPIO_Port,
                              LED1_Pin,
                              (current_state == STATE_FATAL) ? GPIO_PIN_SET : GPIO_PIN_RESET);
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
    if (huart->Instance != USART1) {
        return;
    }

    if (console_rx_char >= '0' && console_rx_char <= '9') {
        if (console_rx_index < (CONSOLE_RX_BUFFER_SIZE - 1U)) {
            console_rx_buffer[console_rx_index++] = (char)console_rx_char;
        }
    } else if (console_rx_char == 'a' || console_rx_char == 'A') {
        /*! Release the multiplexer straight away, no ENTER needed. */
        adc_clear_mux_hold();
        console_rx_index = 0U;
    } else if (console_rx_char == '\r' || console_rx_char == '\n') {
        if (console_rx_index > 0U) {
            uint32_t channel = 0U;
            for (uint8_t i = 0U; i < console_rx_index; ++i) {
                channel = (channel * 10U) + (uint32_t)(console_rx_buffer[i] - '0');
            }

            /*! Out of range means "go back to automatic" rather than nothing,
                so a typo cannot leave the multiplexer stuck. */
            if (channel < DEFINES_NTC_MUX_CHANNEL_COUNT) {
                adc_set_mux_hold((size_t)channel);
            } else {
                adc_clear_mux_hold();
            }

            console_rx_index = 0U;
        }
    } else {
        console_rx_index = 0U;
    }

    HAL_UART_Receive_IT(&huart1, &console_rx_char, 1U);
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
