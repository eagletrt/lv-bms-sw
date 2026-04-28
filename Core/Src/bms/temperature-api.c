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
#include "defs.h"
#include "eagletrt.h"
#include "eagletrt-api.h"

#ifdef CONFIG_TEMPERATURE_MODULE_ENABLE

EAGLETRT_STATIC struct TemperatureHandler htemp; /*!< Private temperature handler instance. */

void temperature_api_init(void) {
    memset(&htemp, 0U, sizeof(htemp));
}

enum TemperatureReturnCode temperature_api_update_temperature(size_t index, celsius temperature) {
    if (index >= DEFS_CELLS_COUNT) {
        return TEMPERATURE_RC_OUT_OF_BOUNDS;
    }

    htemp.temperatures[index] = temperature;
    return TEMPERATURE_RC_OK;
}

enum TemperatureReturnCode temperature_api_update_temperatures(size_t index, const celsius *temperatures, size_t size) {
    if (temperatures == NULL) {
        return TEMPERATURE_RC_NULL_POINTER;
    }

    if ((index + size) > DEFS_CELLS_COUNT) {
        return TEMPERATURE_RC_OUT_OF_BOUNDS;
    }

    for (size_t i = 0U; i < size; ++i) {
        htemp.temperatures[index + i] = temperatures[i];
    }
    return TEMPERATURE_RC_OK;
}

celsius temperature_api_get_min(void) {
    celsius min = htemp.temperatures[0U];
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        min = EAGLETRT_API_MIN(min, htemp.temperatures[i]);
    }

    return min;
}

celsius temperature_api_get_max(void) {
    celsius max = htemp.temperatures[0U];
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        max = EAGLETRT_API_MAX(max, htemp.temperatures[i]);
    }

    return max;
}

celsius temperature_api_get_average(void) {
    celsius average = 0.f;
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        average += htemp.temperatures[i];
    }

    return average / DEFS_CELLS_COUNT;
}

enum TemperatureReturnCode temperature_api_dump_temperatures(celsius *out, size_t start, size_t size) {
    if (out == NULL) {
        return TEMPERATURE_RC_NULL_POINTER;
    }

    if ((start + size) > DEFS_CELLS_COUNT) {
        return TEMPERATURE_RC_OUT_OF_BOUNDS;
    }

    memcpy(out, htemp.temperatures + start, size * sizeof(*out));
    return TEMPERATURE_RC_OK;
}

#endif /*! CONFIG_TEMPERATURE_MODULE_ENABLE */
