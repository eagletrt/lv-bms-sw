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

#include <stdbool.h>
#include <stddef.h>

#include "types.h"

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN Private defines */

/*!
 * \brief           Index of every conversion inside the ADC1 DMA buffer.
 *
 * \details         ADC1 runs with the sequencer in "not fully configurable"
 *                  mode (\c ADC_SCAN_SEQ_FIXED), which converts the enabled
 *                  channels in ascending channel-number order, no matter in
 *                  which order they were configured. This enum therefore has to
 *                  stay sorted by ADC channel number, and its length has to
 *                  match exactly the number of channels enabled in
 *                  MX_ADC1_Init(), otherwise the DMA transfer and the buffer go
 *                  out of sync.
 */
enum AdcRead {
    ADC_READ_MCU_5V_SENSE,      /*!< CH0  - PA0  - 5 V rail sense */
    ADC_READ_VSUP_SENSE,        /*!< CH1  - PA1  - supply voltage sense */
    ADC_READ_VOUT_FB,           /*!< CH2  - PA2  - output voltage feedback */
    ADC_READ_OUTPUT_EN_FB,      /*!< CH6  - PA6  - relay coil feedback */
    ADC_READ_OUT_FUSE_FB,       /*!< CH7  - PA7  - output fuse feedback */
    ADC_READ_VIN_SENSE,         /*!< CH8  - PA8  - fused input voltage sense */
    ADC_READ_MCU_TEMPSENSOR,    /*!< CH9  - internal MCU temperature sensor */
    ADC_READ_VREFINT,           /*!< CH10 - internal voltage reference */
    ADC_READ_VDDA,              /*!< CH15 - internal VDDA */
    ADC_READ_VSSA,              /*!< CH16 - internal VSSA */
    ADC_READ_I_OUT_SENSE,       /*!< CH17 - PB0  - output current sense */
    ADC_READ_LVMS_OUT_SENSE,    /*!< CH18 - PB1  - LVMS output sense */
    ADC_READ_NTC_SENSE,         /*!< CH19 - PB2  - NTC multiplexer output */
    ADC_READ_VIN_UNFUSED_SENSE, /*!< CH20 - PB10 - unfused input voltage sense */
    ADC_READ_I_CHRG_SENSE,      /*!< CH21 - PB11 - charger current sense */
    ADC_READ_V_CHRG_SENSE,      /*!< CH22 - PB12 - charger voltage sense */
    ADC_READ_COUNT              /*!< The number of conversions in a single scan */
};

#define ADC_RESOLUTION_BITS (12U)  /*!< ADC resolution in bits, has to match hadc1.Init.Resolution. */
#define ADC_MUX_SETTLE_MS (2U)     /*!< Time given to the multiplexer and to the NTC divider to settle after an address change. */
#define ADC_SCAN_TIMEOUT_MS (200U) /*!< A scan that does not complete within this time is aborted and restarted. Has to stay above the worst-case blocking time of the debug print. */

/* USER CODE END Private defines */

void MX_ADC1_Init(void);

/* USER CODE BEGIN Prototypes */

/*!
 * \brief           Start a single ADC scan of every enabled channel via DMA.
 *
 * \details         Prefer adc_routine(), which drives the scan and the NTC
 *                  multiplexer together; this is exposed for the initial kick
 *                  and for tests.
 */
void adc_start_read(void);

/*!
 * \brief           Run the ADC/multiplexer state machine.
 *
 * \details         Has to be called periodically from the main loop. It
 *                  processes a completed scan (updating the temperature and
 *                  feedback modules), steps the NTC multiplexer to the next
 *                  channel, waits #ADC_MUX_SETTLE_MS for it to settle and starts
 *                  the next scan. A scan stuck for more than
 *                  #ADC_SCAN_TIMEOUT_MS is aborted and restarted so a lost DMA
 *                  completion cannot stall the acquisition for good.
 *
 * \param[in]       tick The current tick in ms.
 */
void adc_routine(uint32_t tick);

/*!
 * \brief           Get the last converted voltages, indexed by enum AdcRead.
 *
 * \returns         A pointer to the internal array of #ADC_READ_COUNT voltages in V.
 */
volatile float *get_adc_voltages(void);

/*!
 * \brief           Get a single converted voltage measured at the MCU pin.
 *
 * \param[in]       read The conversion to read.
 *
 * \returns         volt The voltage in V, 0 V if \p read is out of bounds.
 */
volt adc_get_voltage(enum AdcRead read);

/*!
 * \brief           Get the multiplexer channel currently selected, i.e. the one
 *                  the next scan will sample.
 *
 * \returns         size_t The multiplexer channel index.
 */
size_t adc_get_current_ntc_channel(void);

/*!
 * \brief           Get the analog supply voltage measured through VREFINT.
 *
 * \details         The raw counts are scaled with this value instead of a fixed
 *                  3.3 V so that a supply that is off nominal does not skew every
 *                  reading, the NTC ones in particular.
 *
 * \returns         volt VDDA in V.
 */
volt adc_get_vdda(void);

/*!
 * \brief           Get the MCU junction temperature from the internal sensor.
 *
 * \returns         celsius The die temperature in °C.
 */
celsius adc_get_mcu_temperature(void);

/*!
 * \defgroup        adc_sense Scaled readings of the analog sense front-ends.
 *
 * \details         Each getter undoes the 10 k / 18 k or 10 k / 1k3 attenuation of
 *                  its sense line (\c DEFINES_SENSE_*_GAIN in defines.h) so it
 *                  returns the voltage present on the rail itself, not the one
 *                  measured on the MCU pin.
 *
 * \note            adc_get_charger_current() goes one step further and also undoes
 *                  the transfer function of the ACS724 that measures it. There is
 *                  no equivalent for the output current: nothing drives
 *                  I_OUT_SENSED on the schematic, so only the node voltage is
 *                  available, see \c DEFINES_SENSE_I_OUT_DIVIDER_GAIN.
 *
 * \{
 */

volt adc_get_vin(void);                  /*!< Fused input voltage in V. */
volt adc_get_vin_unfused(void);          /*!< Unfused input voltage in V. */
volt adc_get_vsup(void);                 /*!< Supply voltage in V. */
volt adc_get_vout(void);                 /*!< Output voltage in V. */
volt adc_get_lvms_out(void);             /*!< LVMS output voltage in V. */
volt adc_get_mcu_5v(void);               /*!< 5 V rail voltage in V. */
volt adc_get_charger_voltage(void);      /*!< Charger voltage in V. */
volt adc_get_i_out_sense_voltage(void);  /*!< I_OUT_SENSED node in V. No sensor drives it, see the note above. */
volt adc_get_i_chrg_sense_voltage(void); /*!< I_CHRG, the ACS724 output, in V. */
ampere adc_get_charger_current(void);    /*!< Charger current in A, positive into the battery. */

/*! \} */

/*!
 * \defgroup        adc_mux Manual control of the NTC multiplexer.
 *
 * \details         adc_routine() normally walks every populated channel on its
 *                  own. For bench work it is useful to freeze it on one channel
 *                  and watch a single NTC; that is what a hold does. It only
 *                  stops the stepping, the scans keep running, so the held
 *                  channel keeps being refreshed.
 *
 * \{
 */

/*!
 * \brief           Pin the multiplexer to one channel and stop the automatic stepping.
 *
 * \param[in]       channel The channel to hold, ignored if >= DEFINES_NTC_MUX_CHANNEL_COUNT.
 */
void adc_set_mux_hold(size_t channel);

/*!
 * \brief           Resume walking every populated multiplexer channel.
 */
void adc_clear_mux_hold(void);

/*!
 * \brief           Tell whether the multiplexer is currently held on one channel.
 *
 * \returns         bool True if held, false if cycling.
 */
bool adc_is_mux_held(void);

/*!
 * \brief           Get the last NTC voltage measured on a multiplexer channel.
 *
 * \param[in]       mux_channel The channel index.
 *
 * \returns         float The NTC divider voltage in V, 0 V if the channel is out of bounds.
 */
float adc_get_ntc_voltage(size_t mux_channel);

/*! \} */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
