/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

#include "eagletrt.h"
#include "eagletrt-api.h"

#include "defines.h"
#include "feedback-api.h"
#include "temperature-api.h"

/*! Full scale of a #ADC_RESOLUTION_BITS conversion. */
#define ADC_FULL_SCALE ((float)((1UL << ADC_RESOLUTION_BITS) - 1UL))

/*! Fallback supply voltage used before the first VREFINT conversion is available. */
#define ADC_VDDA_NOMINAL_V (3.3F)

/*! Internal temperature sensor average slope in uV/°C (STM32C0 datasheet "Avg_Slope"). */
#define ADC_MCU_TEMPSENSOR_AVG_SLOPE_UV_C (2530.F)

/*!
 * \brief           Lifecycle of a single ADC scan.
 */
enum AdcScanState {
    ADC_SCAN_STATE_SETTLING,   /*!< The multiplexer address just changed, waiting for it to settle. */
    ADC_SCAN_STATE_CONVERTING, /*!< A DMA scan is in flight. */
    ADC_SCAN_STATE_COMPLETE,   /*!< The scan completed, its data is waiting to be processed. */
};

EAGLETRT_STATIC EAGLETRT_VOLATILE uint16_t adc_buffer[ADC_READ_COUNT] = { 0 };                /*!< Raw DMA destination, written by the DMA only. */
EAGLETRT_STATIC float voltages[ADC_READ_COUNT] = { 0.F };                                     /*!< Converted pin voltages in V, written by adc_routine() only. */
EAGLETRT_STATIC EAGLETRT_VOLATILE enum AdcScanState adc_scan_state = ADC_SCAN_STATE_SETTLING; /*!< The state of the current scan. */
EAGLETRT_STATIC size_t adc_mux_channel = 0U;                                                  /*!< The multiplexer channel currently selected. */
EAGLETRT_STATIC uint32_t adc_state_tick = 0U;                                                 /*!< Tick at which the current state was entered in ms. */
EAGLETRT_STATIC volt adc_vdda = ADC_VDDA_NOMINAL_V;                                           /*!< Supply voltage measured through VREFINT in V. */
EAGLETRT_STATIC float ntc_voltages[DEFINES_NTC_MUX_USED_CHANNEL_COUNT] = { 0.F };             /*!< Last NTC voltage read on each multiplexer channel, in V. */
EAGLETRT_STATIC bool adc_mux_hold = false;                                                    /*!< True while the multiplexer is pinned to one channel for debugging. */

/*! Window in which the measured +5 V rail is trusted for the ACS724 conversion. */
#define ADC_ACS724_SUPPLY_MIN_V (4.0F)
#define ADC_ACS724_SUPPLY_MAX_V (5.5F)

#define ADC_RAW_VALUE_TO_VOLT(VALUE, VREF) ((float)(VALUE) / ADC_FULL_SCALE * (VREF))

/*!
 * \brief           Drive the NTC multiplexer address lines.
 *
 * \param[in]       channel The multiplexer channel to select, wrapped to
 *                  #DEFINES_NTC_MUX_CHANNEL_COUNT.
 */
EAGLETRT_STATIC void prv_adc_mux_select(size_t channel) {
    channel %= DEFINES_NTC_MUX_CHANNEL_COUNT;

    HAL_GPIO_WritePin(MUX_A0_MCU_GPIO_Port, MUX_A0_MCU_Pin, (channel & 0x1U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_A1_MCU_GPIO_Port, MUX_A1_MCU_Pin, (channel & 0x2U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_A2_MCU_GPIO_Port, MUX_A2_MCU_Pin, (channel & 0x4U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MUX_A3_MCU_GPIO_Port, MUX_A3_MCU_Pin, (channel & 0x8U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/*!
 * \brief           Convert the whole raw scan into pin voltages.
 *
 * \details         VREFINT is converted first so that every other channel is
 *                  scaled with the actual supply voltage instead of the nominal
 *                  one. A VREFINT reading of zero (sensor not settled yet) keeps
 *                  the previous value.
 */
EAGLETRT_STATIC void prv_adc_convert_scan(void) {
    const uint16_t vrefint_raw = adc_buffer[ADC_READ_VREFINT];
    if (vrefint_raw != 0U) {
        adc_vdda = (volt)__LL_ADC_CALC_VREFANALOG_VOLTAGE(vrefint_raw, LL_ADC_RESOLUTION_12B) / 1000.F;
    }

    for (size_t i = 0U; i < (size_t)ADC_READ_COUNT; ++i) {
        voltages[i] = ADC_RAW_VALUE_TO_VOLT(adc_buffer[i], adc_vdda);
    }
}

/*!
 * \brief           Push the analog feedbacks of the completed scan into the feedback module.
 *
 * \note            The remaining feedbacks are digital and are updated by
 *                  gpio_update_digital_feedbacks().
 */
EAGLETRT_STATIC void prv_adc_update_feedbacks(void) {
    (void)feedback_api_set_analog(FEEDBACK_OUTPUT_ENABLE_NEGATED, voltages[ADC_READ_OUTPUT_EN_FB]);
    (void)feedback_api_set_analog(FEEDBACK_OUTPUT_FUSE, voltages[ADC_READ_OUT_FUSE_FB]);
    (void)feedback_api_set_analog(FEEDBACK_VOUT, voltages[ADC_READ_VOUT_FB]);
}

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* ADC1 init function */
void MX_ADC1_Init(void) {

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = { 0 };

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_SEQ_FIXED;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.LowPowerAutoPowerOff = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 16;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
    hadc1.Init.OversamplingMode = DISABLE;
    hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_1;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_6;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_7;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_8;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_VDDA;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_VSSA;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_17;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_18;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_19;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_20;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_21;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_22;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /*! Calibrate the ADC once, while it is still disabled: without this the
        offset error of the conversions is well above the accuracy the NTC
        divider needs. */
    if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    /*! Park the multiplexer on the first NTC so the very first scan is valid. */
    prv_adc_mux_select(adc_mux_channel);

    /* USER CODE END ADC1_Init 2 */
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
    if (adcHandle->Instance == ADC1) {
        /* USER CODE BEGIN ADC1_MspInit 0 */

        /* USER CODE END ADC1_MspInit 0 */

        /** Initializes the peripherals clocks
  */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
        PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        /* ADC1 clock enable */
        __HAL_RCC_ADC_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PB0     ------> ADC1_IN17
    PB1     ------> ADC1_IN18
    PB2     ------> ADC1_IN19
    PB10     ------> ADC1_IN20
    PB11     ------> ADC1_IN21
    PB12     ------> ADC1_IN22
    PA8     ------> ADC1_IN8
    */
        GPIO_InitStruct.Pin = MCU_5V_SENSE_Pin | VSUP_SENSE_MCU_Pin | VOUT_FB_MCU_Pin | OUTPUT_EN_FB_MCU_Pin | OUT_FUSE_FB_MCU_Pin | VIN_SENSE_MCU_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = I_OUT_SENSE_Pin | LVMS_OUT_SENSE_Pin | NTC_SENSE_MCU_Pin | VIN_UNFUSED_SENSE_MCU_Pin | I_CHRG_SENSE_MCU_Pin | V_CHRG_SENSE_MCU_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* ADC1 DMA Init */
        /* ADC1 Init */
        hdma_adc1.Instance = DMA1_Channel1;
        hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc1.Init.Mode = DMA_NORMAL;
        hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc1);

        /* ADC1 interrupt Init */
        HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC1_IRQn);
        /* USER CODE BEGIN ADC1_MspInit 1 */

        /* USER CODE END ADC1_MspInit 1 */
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle) {

    if (adcHandle->Instance == ADC1) {
        /* USER CODE BEGIN ADC1_MspDeInit 0 */

        /* USER CODE END ADC1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ADC_CLK_DISABLE();

        /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PB0     ------> ADC1_IN17
    PB1     ------> ADC1_IN18
    PB2     ------> ADC1_IN19
    PB10     ------> ADC1_IN20
    PB11     ------> ADC1_IN21
    PB12     ------> ADC1_IN22
    PA8     ------> ADC1_IN8
    */
        HAL_GPIO_DeInit(GPIOA, MCU_5V_SENSE_Pin | VSUP_SENSE_MCU_Pin | VOUT_FB_MCU_Pin | OUTPUT_EN_FB_MCU_Pin | OUT_FUSE_FB_MCU_Pin | VIN_SENSE_MCU_Pin);

        HAL_GPIO_DeInit(GPIOB, I_OUT_SENSE_Pin | LVMS_OUT_SENSE_Pin | NTC_SENSE_MCU_Pin | VIN_UNFUSED_SENSE_MCU_Pin | I_CHRG_SENSE_MCU_Pin | V_CHRG_SENSE_MCU_Pin);

        /* ADC1 DMA DeInit */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);

        /* ADC1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(ADC1_IRQn);
        /* USER CODE BEGIN ADC1_MspDeInit 1 */

        /* USER CODE END ADC1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

void adc_start_read(void) {
    /*! Publish the new state before arming the DMA, so that a completion
        interrupt firing straight away cannot have its COMPLETE overwritten. */
    adc_state_tick = HAL_GetTick();
    adc_scan_state = ADC_SCAN_STATE_CONVERTING;

    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, ADC_READ_COUNT) != HAL_OK) {
        /*! Nothing was started: fall back to SETTLING so adc_routine() retries. */
        adc_scan_state = ADC_SCAN_STATE_SETTLING;
    }
}

void adc_routine(uint32_t tick) {
    switch (adc_scan_state) {
        case ADC_SCAN_STATE_COMPLETE: {
            prv_adc_convert_scan();
            prv_adc_update_feedbacks();

            /*! The NTC channel of this scan belongs to the multiplexer channel
                that was selected while it ran; mux channel n carries NTC n. */
            if (adc_mux_channel < DEFINES_NTC_MUX_USED_CHANNEL_COUNT) {
                ntc_voltages[adc_mux_channel] = voltages[ADC_READ_NTC_SENSE];
            }

            if (adc_mux_channel < DEFINES_CELLS_NTC_COUNT) {
                (void)temperature_api_update_temperature(
                    adc_mux_channel,
                    temperature_api_volt_to_celsius(voltages[ADC_READ_NTC_SENSE]));
            }

            /*! Step to the next populated channel and let it settle, unless a
                debug hold is pinning the multiplexer to one channel. */
            if (!adc_mux_hold) {
                adc_mux_channel = (adc_mux_channel + 1U) % DEFINES_NTC_MUX_USED_CHANNEL_COUNT;
                prv_adc_mux_select(adc_mux_channel);
            }

            adc_state_tick = tick;
            adc_scan_state = ADC_SCAN_STATE_SETTLING;
            break;
        }

        case ADC_SCAN_STATE_SETTLING: {
            if ((tick - adc_state_tick) >= ADC_MUX_SETTLE_MS) {
                /*! adc_start_read() re-stamps adc_state_tick, which then times the
                    conversion instead of the settling. */
                adc_start_read();
            }
            break;
        }

        case ADC_SCAN_STATE_CONVERTING: {
            /*! A conversion that never completes (aborted DMA, overrun) would
                otherwise freeze the acquisition: recover by restarting it. */
            if ((tick - adc_state_tick) >= ADC_SCAN_TIMEOUT_MS) {
                (void)HAL_ADC_Stop_DMA(&hadc1);
                adc_state_tick = tick;
                adc_scan_state = ADC_SCAN_STATE_SETTLING;
            }
            break;
        }

        default:
            adc_scan_state = ADC_SCAN_STATE_SETTLING;
            break;
    }
}

volatile float *get_adc_voltages(void) {
    return voltages;
}

volt adc_get_voltage(enum AdcRead read) {
    if (read >= ADC_READ_COUNT) {
        return 0.F;
    }

    return voltages[read];
}

size_t adc_get_current_ntc_channel(void) {
    return adc_mux_channel;
}

volt adc_get_vdda(void) {
    return adc_vdda;
}

celsius adc_get_mcu_temperature(void) {
    /*! TS_CAL1 is the raw value of the sensor at 30 °C with Vref+ = 3.0 V. */
    const float calibration_mv = ((float)(*TEMPSENSOR_CAL1_ADDR) * (float)TEMPSENSOR_CAL_VREFANALOG) / ADC_FULL_SCALE;
    const float measured_mv = voltages[ADC_READ_MCU_TEMPSENSOR] * 1000.F;

    return (celsius)((((measured_mv - calibration_mv) * 1000.F) / ADC_MCU_TEMPSENSOR_AVG_SLOPE_UV_C) + (float)TEMPSENSOR_CAL1_TEMP);
}

volt adc_get_vin(void) {
    return voltages[ADC_READ_VIN_SENSE] / DEFINES_SENSE_VIN_GAIN;
}

volt adc_get_vin_unfused(void) {
    return voltages[ADC_READ_VIN_UNFUSED_SENSE] / DEFINES_SENSE_VIN_UNFUSED_GAIN;
}

volt adc_get_vsup(void) {
    return voltages[ADC_READ_VSUP_SENSE] / DEFINES_SENSE_VSUP_GAIN;
}

volt adc_get_vout(void) {
    return voltages[ADC_READ_VOUT_FB] / DEFINES_SENSE_VOUT_GAIN;
}

volt adc_get_lvms_out(void) {
    return voltages[ADC_READ_LVMS_OUT_SENSE] / DEFINES_SENSE_LVMS_OUT_GAIN;
}

volt adc_get_mcu_5v(void) {
    return voltages[ADC_READ_MCU_5V_SENSE] / DEFINES_SENSE_MCU_5V_GAIN;
}

volt adc_get_charger_voltage(void) {
    return voltages[ADC_READ_V_CHRG_SENSE] / DEFINES_SENSE_V_CHRG_GAIN;
}

volt adc_get_i_out_sense_voltage(void) {
    /*! No sensor drives this node on the current schematic, see
        DEFINES_SENSE_I_OUT_DIVIDER_GAIN: this is the node voltage, nothing more. */
    return voltages[ADC_READ_I_OUT_SENSE] / DEFINES_SENSE_I_OUT_DIVIDER_GAIN;
}

volt adc_get_i_chrg_sense_voltage(void) {
    return voltages[ADC_READ_I_CHRG_SENSE] / DEFINES_SENSE_I_CHRG_DIVIDER_GAIN;
}

ampere adc_get_charger_current(void) {
    /*! The ACS724 is ratiometric, so use the +5 V rail the board measures for
        both its quiescent point and its sensitivity. If that reading is not
        plausible (rail down, sense line open) fall back to the nominal value
        rather than dividing by something meaningless. */
    volt supply = adc_get_mcu_5v();
    if (supply < ADC_ACS724_SUPPLY_MIN_V || supply > ADC_ACS724_SUPPLY_MAX_V) {
        supply = DEFINES_SENSE_I_CHRG_SUPPLY_NOMINAL_V;
    }

    const volt zero = supply * DEFINES_SENSE_I_CHRG_ZERO_RATIO;
    const float sensitivity = DEFINES_SENSE_I_CHRG_SENSITIVITY_V_A * (supply / DEFINES_SENSE_I_CHRG_SUPPLY_NOMINAL_V);

    return (ampere)((adc_get_i_chrg_sense_voltage() - zero) / sensitivity);
}

void adc_set_mux_hold(size_t channel) {
    if (channel >= DEFINES_NTC_MUX_CHANNEL_COUNT) {
        return;
    }

    adc_mux_channel = channel;
    adc_mux_hold = true;
    prv_adc_mux_select(adc_mux_channel);
}

void adc_clear_mux_hold(void) {
    adc_mux_hold = false;
}

bool adc_is_mux_held(void) {
    return adc_mux_hold;
}

float adc_get_ntc_voltage(size_t mux_channel) {
    if (mux_channel >= DEFINES_NTC_MUX_USED_CHANNEL_COUNT) {
        return 0.F;
    }
    return ntc_voltages[mux_channel];
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance != ADC1) {
        return;
    }

    /*! Keep the ISR trivial: the raw buffer is stable until the next scan is
        started, so the (soft-float, NTC polynomial) conversion is deferred to
        adc_routine() in the main loop. */
    adc_scan_state = ADC_SCAN_STATE_COMPLETE;
}

/* USER CODE END 1 */