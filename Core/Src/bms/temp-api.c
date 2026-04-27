/*!
 * \file            temp-api.c
 * \date            2026-03-24
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature module APIs.
 */

#include "temp.h"
#include "temp-api.h"

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "types.h"
#include "defs.h"
#include "eagletrt.h"
#include "eagletrt-api.h"

#ifdef CONFIG_TEMPERATURE_MODULE_ENABLE

EAGLETRT_STATIC struct TempHandler htemp; /*!< Private temperature handler instance. */

void temp_api_init(void) {
    memset(&htemp, 0U, sizeof(htemp));
}

enum TempReturnCode temp_api_update_temperature(size_t idx, celsius temperature) {
    if (idx >= DEFS_CELLS_COUNT) {
        return TEMP_RC_OUT_OF_BOUNDS;
    }

    htemp.temperatures[idx] = temperature;
    return TEMP_RC_OK;
}

enum TempReturnCode temp_api_update_temperatures(size_t idx, const celsius *temperatures, size_t size) {
    if (temperatures == NULL) {
        return TEMP_RC_NULL_POINTER;
    }

    if ((idx + size) > DEFS_CELLS_COUNT) {
        return TEMP_RC_OUT_OF_BOUNDS;
    }

    for (size_t i = 0U; i < size; ++i) {
        htemp.temperatures[idx + i] = temperatures[i];
    }
    return TEMP_RC_OK;
}

celsius temp_api_get_min(void) {
    celsius min = htemp.temperatures[0];
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        min = EAGLETRT_API_MIN(min, htemp.temperatures[i]);
    }

    return min;
}

celsius temp_api_get_max(void) {
    celsius max = htemp.temperatures[0];
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        max = EAGLETRT_API_MAX(max, htemp.temperatures[i]);
    }

    return max;
}

celsius temp_api_get_average(void) {
    celsius average = 0.f;
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        average += htemp.temperatures[i];
    }

    return average / DEFS_CELLS_COUNT;
}

enum TempReturnCode temp_api_dump_temperatures(celsius *out, size_t start, size_t size) {
    if (out == NULL) {
        return TEMP_RC_NULL_POINTER;
    }

    if ((start + size) > DEFS_CELLS_COUNT) {
        return TEMP_RC_OUT_OF_BOUNDS;
    }

    memcpy(out, htemp.temperatures + start, size * sizeof(*out));
    return TEMP_RC_OK;
}

#endif /*! CONFIG_TEMPERATURE_MODULE */
