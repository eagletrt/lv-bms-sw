/*!
 * \file            sensing.h
 * \date            2026-11-25
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           TBD.
 */

#ifndef SENSING_H
#define SENSING_H

#include <stdint.h>

#include "watchdogs.h"

constexpr uint32_t sensing_adc_watchdog_timeout_ms = 0U;

enum SensingReturnCode {
    SENSING_RC_OK,
    SENSING_RC_NULL_POINTER,
    SENSING_RC_NOT_INITIALIZED,
};

enum SensingAdcChannel : uint8_t {
    SENSING_ADC_CHANNEL_MCU_5V_SENSE = 0U, /*!< CH0  - PA0  - 5 V rail sense */
    SENSING_ADC_CHANNEL_VSUP_SENSE,        /*!< CH1  - PA1  - supply voltage sense */
    SENSING_ADC_CHANNEL_VOUT_FB,           /*!< CH2  - PA2  - output voltage feedback */
    SENSING_ADC_CHANNEL_OUTPUT_EN_FB,      /*!< CH6  - PA6  - relay coil feedback */
    SENSING_ADC_CHANNEL_OUT_FUSE_FB,       /*!< CH7  - PA7  - output fuse feedback */
    SENSING_ADC_CHANNEL_VIN_SENSE,         /*!< CH8  - PA8  - fused input voltage sense */
    SENSING_ADC_CHANNEL_MCU_TEMPSENSOR,    /*!< CH9  - internal MCU temperature sensor */
    SENSING_ADC_CHANNEL_VREFINT,           /*!< CH10 - internal voltage reference */
    SENSING_ADC_CHANNEL_VDDA,              /*!< CH15 - internal VDDA */
    SENSING_ADC_CHANNEL_VSSA,              /*!< CH16 - internal VSSA */
    SENSING_ADC_CHANNEL_I_OUT_SENSE,       /*!< CH17 - PB0  - output current sense */
    SENSING_ADC_CHANNEL_LVMS_OUT_SENSE,    /*!< CH18 - PB1  - LVMS output sense */
    SENSING_ADC_CHANNEL_NTC_SENSE,         /*!< CH19 - PB2  - NTC multiplexer output */
    SENSING_ADC_CHANNEL_VIN_UNFUSED_SENSE, /*!< CH20 - PB10 - unfused input voltage sense */
    SENSING_ADC_CHANNEL_I_CHRG_SENSE,      /*!< CH21 - PB11 - charger current sense */
    SENSING_ADC_CHANNEL_V_CHRG_SENSE,      /*!< CH22 - PB12 - charger voltage sense */
    SENSING_ADC_CHANNEL_COUNT              /*!< The number of conversions in a single scan */
};

enum SensingAdcState : uint8_t {
    SENSING_ADC_STATE_IDLE = 0U,  /*!< */
    SENSING_ADC_STATE_CONVERTING, /*!< */
    SENSING_ADC_STATE_CONVERTED   /*!< */
};
                    
/*!
 * \brief           Callback that starts a single scan conversion of an MCU ADC with DMA
 *
 * \details         When the scan is complete \ref sensing_api_adc_conversion_complete
 *                  must be called (e.g. from HAL_ADC_ConvCpltCallback)
 *
 * \param[in]       adc The ADC to start
 * \param[in]       buffer The buffer where the DMA has to store the samples (16 bit each)
 * \param[in]       count The number of samples to convert
 *
 * \returns         True if the conversion was started, false otherwise
 */
typedef bool (*sensing_adc_start_callback)(void);

struct SensingHandler {
    sensing_adc_start_callback start;
    enum SensingAdcState adc_state;
    struct Watchdog adc_watchdog;
    bool is_initialized;
    bool watchdog_started;
};

#endif /*! SENSING_H */
