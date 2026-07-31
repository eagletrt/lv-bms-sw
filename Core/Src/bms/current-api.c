/*!
 * \file            current-api.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Current module data structures.
 */

#include "current.h"
#include "current-api.h"

#include <string.h>

#include "defines.h"
#include "types.h"
#include "eagletrt.h"
#include "voltage-api.h"

#ifdef CONFIG_CURRENT_MODULE_ENABLE

EAGLETRT_STATIC struct CurrentHandler current_handler;

/*!
 * \brief           Recompute the output current as the sum of all NTC currents.
 */
EAGLETRT_STATIC void prv_current_api_compute_output_current(void) {
    ampere sum = 0.F;
    for (size_t i = 0U; i < DEFINES_NTC_COUNT; ++i) {
        sum += current_handler.currents[i];
    }
    current_handler.output_current = sum;
}

enum CurrentReturnCode current_api_init(void) {
    current_handler.output_current = 0.F;
    for (size_t i = 0; i < DEFINES_NTC_COUNT; ++i) {
        current_handler.currents[i] = 0.F;
    }

    return CURRENT_RC_OK;
}

enum CurrentReturnCode current_api_update_current(size_t index, ampere current) {
    if (index >= DEFINES_NTC_COUNT) {
        return CURRENT_RC_OUT_OF_BOUNDS;
    }

    current_handler.currents[index] = current;
    prv_current_api_compute_output_current();

    return CURRENT_RC_OK;
}

enum CurrentReturnCode current_api_update_currents(size_t index, ampere *currents, size_t size) {
    if (currents == NULL) {
        return CURRENT_RC_NULL_POINTER;
    }

    if ((index + size) > DEFINES_CELLS_COUNT) {
        return CURRENT_RC_OUT_OF_BOUNDS;
    }

    for (size_t i = 0U; i < size; ++i) {
        current_handler.currents[index + i] = currents[i];
    }
    prv_current_api_compute_output_current();

    return CURRENT_RC_OK;
}

enum CurrentReturnCode current_api_dump_currents(ampere *out, size_t start, size_t size) {
    if (out == NULL) {
        return CURRENT_RC_NULL_POINTER;
    }

    if ((start + size) > DEFINES_CELLS_COUNT) {
        return CURRENT_RC_OUT_OF_BOUNDS;
    }

    memcpy(out, current_handler.currents + start, size * sizeof(*out));

    return CURRENT_RC_OK;
}

ampere current_api_get_output_current(void) {
    return current_handler.output_current;
}

kilowatt current_api_get_power() {
    return voltage_api_get_sum() * current_handler.output_current;
}

#endif /*! CONFIG_CURRENT_MODULE_ENABLE */
