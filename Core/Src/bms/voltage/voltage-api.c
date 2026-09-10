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
#include "defines.h"
#include "eagletrt.h"
#include "eagletrt-api.h"
#include "can-primary-api.h"
#include "can-communication-api.h"

#ifdef CONFIG_VOLTAGE_MODULE_ENABLE

EAGLETRT_STATIC struct VoltageHandler voltage_handler; /*!< Private voltage handler instance. */

enum VoltageReturnCode voltage_api_init(void) {
    memset(&voltage_handler, 0U, sizeof(voltage_handler));
    return VOLTAGE_RC_OK;
}

enum VoltageReturnCode voltage_api_update_voltage(size_t index, volt voltage) {
    if (index >= DEFINES_CELLS_SERIES_COUNT) {
        return VOLTAGE_RC_OUT_OF_BOUNDS;
    }

    voltage_handler.voltages[index] = voltage;
    return VOLTAGE_RC_OK;
}

enum VoltageReturnCode voltage_api_update_voltages(size_t index, const volt *voltages, size_t size) {
    if (voltages == NULL) {
        return VOLTAGE_RC_NULL_POINTER;
    }

    if ((index + size) > DEFINES_CELLS_SERIES_COUNT) {
        return VOLTAGE_RC_OUT_OF_BOUNDS;
    }

    for (size_t i = 0U; i < size; ++i) {
        voltage_handler.voltages[index + i] = voltages[i];
    }
    return VOLTAGE_RC_OK;
}

volt voltage_api_get_min(void) {
    volt min = voltage_handler.voltages[0U];
    for (size_t i = 0; i < DEFINES_CELLS_SERIES_COUNT; ++i) {
        min = EAGLETRT_API_MIN(min, voltage_handler.voltages[i]);
    }

    return min;
}

volt voltage_api_get_max(void) {
    volt max = voltage_handler.voltages[0U];
    for (size_t i = 0; i < DEFINES_CELLS_SERIES_COUNT; ++i) {
        max = EAGLETRT_API_MAX(max, voltage_handler.voltages[i]);
    }

    return max;
}

volt voltage_api_get_average(void) {
    volt average = 0.F;
    for (size_t i = 0; i < DEFINES_CELLS_SERIES_COUNT; ++i) {
        average += voltage_handler.voltages[i];
    }

    return average / DEFINES_CELLS_SERIES_COUNT;
}

volt voltage_api_get_sum(void) {
    volt sum = 0.F;
    for (size_t i = 0; i < DEFINES_CELLS_SERIES_COUNT; ++i) {
        sum += voltage_handler.voltages[i];
    }

    return sum;
}

enum VoltageReturnCode voltage_api_dump_voltages(volt *out, size_t start, size_t size) {
    if (out == NULL) {
        return VOLTAGE_RC_NULL_POINTER;
    }

    if ((start + size) > DEFINES_CELLS_SERIES_COUNT) {
        return VOLTAGE_RC_OUT_OF_BOUNDS;
    }

    memcpy(out, voltage_handler.voltages + start, size * sizeof(*out));
    return VOLTAGE_RC_OK;
}

enum VoltageReturnCode voltage_api_periodically_send_cell_voltages(uint32_t tick_ms) {
    if (tick_ms - voltage_handler.last_tick_cell_voltage_ms < can_primary_cycle_time_lvaccellvoltage) {
        return VOLTAGE_RC_OK;
    }
    voltage_handler.last_tick_cell_voltage_ms = tick_ms;

    volt cells[DEFINES_CELLS_SERIES_COUNT];
    voltage_api_dump_voltages(cells, 0U, DEFINES_CELLS_SERIES_COUNT);

    union CanPrimaryMessages message = {
        .lvaccellvoltage = {
            .voltage1 = cells[0],
            .voltage2 = cells[1],
            .voltage3 = cells[2],
            .voltage4 = cells[3],
            .voltage5 = cells[4],
            .voltage6 = cells[5],
        }
    };

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_LVACCELLVOLTAGE,
        .length = can_primary_byte_size_lvaccellvoltage,
    };

    if (can_primary_api_serialize_from_id(frame.id, &message, frame.data) != -1) {
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx_buffer(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }

    return VOLTAGE_RC_OK;
}

enum VoltageReturnCode voltage_api_periodically_send_voltage_information(uint32_t tick_ms) {
    if (tick_ms - voltage_handler.last_tick_voltage_information_ms < can_primary_cycle_time_lvacvoltageinfo) {
        return VOLTAGE_RC_OK;
    }
    voltage_handler.last_tick_voltage_information_ms = tick_ms;

    union CanPrimaryMessages message = {
        .lvacvoltageinfo = {
            .total = voltage_api_get_sum(),
            .min = voltage_api_get_min(),
            .max = voltage_api_get_max(),
            .average = voltage_api_get_average(),
        }
    };

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVOLTAGEINFO,
        .length = can_primary_byte_size_lvacvoltageinfo,
    };

    if (can_primary_api_serialize_from_id(frame.id, &message, frame.data) != -1) {
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx_buffer(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }

    return VOLTAGE_RC_OK;
}

#endif /*! CONFIG_VOLTAGE_MODULE_ENABLE */
