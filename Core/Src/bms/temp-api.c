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

enum TempReturnCode temp_api_update_value(size_t idx, celsius value) {
    if (idx >= DEFS_CELLS_COUNT) {
        return TEMP_RC_OUT_OF_BOUNDS;
    }

    htemp.temperatures[idx] = value;
    return TEMP_RC_OK;
}

enum TempReturnCode temp_api_update_values(size_t idx, const celsius *values, size_t size) {
    if (values == NULL) {
        return TEMP_RC_NULL_PTR;
    }

    if ((idx + size) > DEFS_CELLS_COUNT) {
        return TEMP_RC_OUT_OF_BOUNDS;
    }

    for (size_t i = 0U; i < size; ++i) {
        htemp.temperatures[idx + i] = values[i];
    }
    return TEMP_RC_OK;
}

const celsius *temp_api_get_temperatures(void) {
    return htemp.temperatures;
}

celsius temp_api_get_min(void) {
    celsius min = TEMP_CHARGE_MAX_C;
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        min = EAGLETRT_API_MIN(min, htemp.temperatures[i]);
    }

    return min;
}

celsius temp_api_get_max(void) {
    celsius max = TEMP_DISCHARGE_MIN_C; /*! I'm using it because it's lower than TEMP_CHARGE_MIN_C */
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        max = EAGLETRT_API_MAX(max, htemp.temperatures[i]);
    }

    return max;
}

celsius temp_api_get_avg(void) {
    celsius avg = 0.f;
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        avg += htemp.temperatures[i];
    }

    return avg / DEFS_CELLS_COUNT;
}

enum TempReturnCode temp_api_dump_values(celsius *out, size_t start, size_t size) {
    if (out == NULL) {
        return TEMP_RC_NULL_PTR;
    }

    if ((start + size) >= DEFS_CELLS_COUNT) {
        return TEMP_RC_OUT_OF_BOUNDS;
    }

    memcpy(out, htemp.temperatures + start, size * sizeof(*out));
    return TEMP_RC_OK;
}

#endif /*! CONFIG_TEMPERATURE_MODULE */
