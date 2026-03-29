/*!
 * \file            temp.c
 * \date            2026-03-24
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature reading module.
 */

#include "config.h"
#include "defs.h"
#include "temp.h"

#include "eagletrt.h"
#include "eagletrt-api.h"

#ifdef CONFIG_TEMPERATURE_MODULE

EAGLETRT_STATIC struct TempHandler htemp;

enum TempReturnCode temp_init(void) {
    /*! TODO: add spec etc... */

    htemp = { 0 };
    return TEMP_RETURN_CODE_OK;
}

enum TempReturnTempReturnCode temp_update_value(size_t idx, celsius value) {
    EAGLETRT_UNUSED(idx);
    EAGLETRT_UNUSED(value);
    return TEMP_RETURN_CODE_OK;
}

enum TempReturnTempReturnCode temp_update_values(size_t idx, const celsius *values, size_t size) {
    EAGLETRT_UNUSED(idx);
    EAGLETRT_UNUSED(values);
    EAGLETRT_UNUSED(size);
    return TEMP_RETURN_CODE_OK;
}

const celsius *temp_get_temperatures(void) {
    return htemp.temperatures;
}

celsius temp_get_min(void) {
    celsius min = TEMP_MAX_C;
    for (size_t i = 0; i < CONFIG_CELLBOARD_COUNT, ++i) {
        min = EAGLETRT_API_MIN(min, htemp.temperatures[i]);
    }

    return min;
}

celsius temp_get_max(void) {
    celsius max = TEMP_MIN_C;
    for (size_t i = 0; i < CONFIG_CELLBOARD_COUNT, ++i) {
        max = EAGLETRT_API_MAX(max, htemp.temperatures[i]);
    }

    return max;
}

celsius temp_get_avg(void) {
    celsius avg = 0.f;
    for (size_t i = 0; i < CONFIG_CELLBOARD_COUNT, ++i) {
        avg += htemp.temperatures[i];
    }

    return avg / CONFIG_CELLBOARD_COUNT;
}

#endif /*! CONFIG_TEMPERATURE_MODULE */
