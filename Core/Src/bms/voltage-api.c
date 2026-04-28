/*!
 * \file            voltage-api.c
 * \date            2026-03-24
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Voltage module APIs.
 */

#include "voltage.h"
#include "voltage-api.h"

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "types.h"
#include "defs.h"
#include "eagletrt.h"
#include "eagletrt-api.h"

#ifdef CONFIG_VOLTAGE_MODULE_ENABLE

EAGLETRT_STATIC struct VoltageHandler htemp; /*!< Private voltage handler instance. */

void voltage_api_init(void) {
    memset(&htemp, 0U, sizeof(htemp));
}

enum VoltageReturnCode voltage_api_update_voltage(size_t index, volt voltage) {
    if (index >= DEFS_CELLS_COUNT) {
        return VOLTAGE_RC_OUT_OF_BOUNDS;
    }

    htemp.voltages[index] = voltage;
    return VOLTAGE_RC_OK;
}

enum VoltageReturnCode voltage_api_update_voltages(size_t index, const volt *voltages, size_t size) {
    if (voltages == NULL) {
        return VOLTAGE_RC_NULL_POINTER;
    }

    if ((index + size) > DEFS_CELLS_COUNT) {
        return VOLTAGE_RC_OUT_OF_BOUNDS;
    }

    for (size_t i = 0U; i < size; ++i) {
        htemp.voltages[index + i] = voltages[i];
    }
    return VOLTAGE_RC_OK;
}

volt voltage_api_get_min(void) {
    volt min = htemp.voltages[0U];
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        min = EAGLETRT_API_MIN(min, htemp.voltages[i]);
    }

    return min;
}

volt voltage_api_get_max(void) {
    volt max = htemp.voltages[0U];
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        max = EAGLETRT_API_MAX(max, htemp.voltages[i]);
    }

    return max;
}

volt voltage_api_get_average(void) {
    volt average = 0.f;
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        average += htemp.voltages[i];
    }

    return average / DEFS_CELLS_COUNT;
}

volt voltage_api_get_sum(void) {
    volt sum = 0.f;
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        sum += htemp.voltages[i];
    }

    return sum;
}

enum VoltageReturnCode voltage_api_dump_voltages(volt *out, size_t start, size_t size) {
    if (out == NULL) {
        return VOLTAGE_RC_NULL_POINTER;
    }

    if ((start + size) > DEFS_CELLS_COUNT) {
        return VOLTAGE_RC_OUT_OF_BOUNDS;
    }

    memcpy(out, htemp.voltages + start, size * sizeof(*out));
    return VOLTAGE_RC_OK;
}

#endif /*! CONFIG_VOLTAGE_MODULE_ENABLE */
