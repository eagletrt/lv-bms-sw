/*!
 * \file            temperature-api.c
 * \date            2026-03-24
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature module APIs.
 */

#include "temperature.h"
#include "temperature-api.h"

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "types.h"
#include "defines.h"
#include "eagletrt.h"
#include "eagletrt-api.h"

#ifdef CONFIG_TEMPERATURE_MODULE_ENABLE

EAGLETRT_STATIC struct TemperatureHandler temperature_handler; /*!< Private temperature handler instance. */

/*! NTC voltage-to-temperature polynomial fit (valid over [MIN, MAX] volts). */
#define TEMPERATURE_NTC_MIN_LIMIT_V (0.0F)
#define TEMPERATURE_NTC_MAX_LIMIT_V (3.0F)
#define TEMPERATURE_NTC_COEFF_0 (148.305319086073000)
#define TEMPERATURE_NTC_COEFF_1 (-317.553729396941300)
#define TEMPERATURE_NTC_COEFF_2 (444.564306449468700)
#define TEMPERATURE_NTC_COEFF_3 (-378.912004657724100)
#define TEMPERATURE_NTC_COEFF_4 (180.457759604731300)
#define TEMPERATURE_NTC_COEFF_5 (-44.504609710405890)
#define TEMPERATURE_NTC_COEFF_6 (4.399756702462762)

celsius temperature_api_volt_to_celsius(volt value) {
    value = EAGLETRT_API_CLAMP(value, (volt)TEMPERATURE_NTC_MIN_LIMIT_V, (volt)TEMPERATURE_NTC_MAX_LIMIT_V);
    const double val = value;
    const double val2 = val * val;
    const double val3 = val2 * val;
    const double val4 = val2 * val2;
    const double val5 = val4 * val;
    const double val6 = val3 * val3;
    return (celsius)(TEMPERATURE_NTC_COEFF_0 +
                     (TEMPERATURE_NTC_COEFF_1 * val) +
                     (TEMPERATURE_NTC_COEFF_2 * val2) +
                     (TEMPERATURE_NTC_COEFF_3 * val3) +
                     (TEMPERATURE_NTC_COEFF_4 * val4) +
                     (TEMPERATURE_NTC_COEFF_5 * val5) +
                     (TEMPERATURE_NTC_COEFF_6 * val6));
}

enum TemperatureReturnCode temperature_api_init(void) {
    memset(&temperature_handler, 0U, sizeof(temperature_handler));
    return TEMPERATURE_RC_OK;
}

enum TemperatureReturnCode temperature_api_update_temperature(size_t index, celsius temperature) {
    if (index >= DEFINES_CELLS_NTC_COUNT) {
        return TEMPERATURE_RC_OUT_OF_BOUNDS;
    }

    temperature_handler.temperatures[index] = temperature;
    return TEMPERATURE_RC_OK;
}

enum TemperatureReturnCode temperature_api_update_temperatures(size_t index, const celsius *temperatures, size_t size) {
    if (temperatures == NULL) {
        return TEMPERATURE_RC_NULL_POINTER;
    }

    if ((index + size) > DEFINES_CELLS_NTC_COUNT) {
        return TEMPERATURE_RC_OUT_OF_BOUNDS;
    }

    for (size_t i = 0U; i < size; ++i) {
        temperature_handler.temperatures[index + i] = temperatures[i];
    }
    return TEMPERATURE_RC_OK;
}

celsius temperature_api_get_min(void) {
    celsius min = temperature_handler.temperatures[0U];
    for (size_t i = 0; i < DEFINES_CELLS_NTC_COUNT; ++i) {
        min = EAGLETRT_API_MIN(min, temperature_handler.temperatures[i]);
    }

    return min;
}

celsius temperature_api_get_max(void) {
    celsius max = temperature_handler.temperatures[0U];
    for (size_t i = 0; i < DEFINES_CELLS_NTC_COUNT; ++i) {
        max = EAGLETRT_API_MAX(max, temperature_handler.temperatures[i]);
    }

    return max;
}

celsius temperature_api_get_average(void) {
    celsius average = 0.F;
    for (size_t i = 0; i < DEFINES_CELLS_NTC_COUNT; ++i) {
        average += temperature_handler.temperatures[i];
    }

    return average / DEFINES_CELLS_NTC_COUNT;
}

enum TemperatureReturnCode temperature_api_dump_temperatures(celsius *out, size_t start, size_t size) {
    if (out == NULL) {
        return TEMPERATURE_RC_NULL_POINTER;
    }

    if ((start + size) > DEFINES_CELLS_NTC_COUNT) {
        return TEMPERATURE_RC_OUT_OF_BOUNDS;
    }

    memcpy(out, temperature_handler.temperatures + start, size * sizeof(*out));
    return TEMPERATURE_RC_OK;
}

#endif /*! CONFIG_TEMPERATURE_MODULE_ENABLE */
