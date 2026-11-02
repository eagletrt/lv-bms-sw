/*!
 * \file            balancing-api.c
 * \date            2026-09-08
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Cells balancing module APIs.
 */

#include "balancing.h"
#include "eagletrt-api.h"
#include "balancing-api.h"

#include <stdint.h>
#include <string.h>

#include "config.h"
#include "defines.h"
#include "eagletrt.h"
#include "voltage-api.h"
#include "bms-monitor-api.h"
#include "logger-api.h"
#include "logger.h"
#include "bms-tasks.h"
#include "bms-tasks-api.h"

#ifdef CONFIG_BALANCING_MODULE_ENABLE

EAGLETRT_STATIC struct BalancingHandler
    balancing_handler; /*!< Private balancing handler instance. */

/*!
 * \brief            Check whether a cell should be discharged.
 *
 * \param[in]        voltage The cell voltage in V.
 *
 * \returns          true if the cell sits more than threshold above target.
 */
EAGLETRT_STATIC bool prv_balancing_api_cell_need_discharge(volt voltage) {
    return (voltage > balancing_handler.target) &&
           ((voltage - balancing_handler.target) > balancing_handler.threshold);
}

/*!
 * \brief            Compute the bitmask of cells to discharge.
 *
 * \param[in]        odd_parity true to consider odd-indexed cells, false for
 *                   even-indexed ones.
 *
 * \returns          uint8_t The cell bitmask, bit 0 being the first cell.
 */
EAGLETRT_STATIC uint8_t prv_balancing_api_compute_mask(bool odd_parity) {
    volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 0.F };
    if (voltage_api_dump_voltages(voltages, 0U, DEFINES_CELLS_SERIES_COUNT) !=
        VOLTAGE_RC_OK) {
        return 0U;
    }

    uint8_t mask = 0U;
    for (size_t i = 0U; i < DEFINES_CELLS_SERIES_COUNT; ++i) {
        if (prv_balancing_api_cell_need_discharge(voltages[i]) &&
            (((i & 1U) != 0U) == odd_parity)) {
            mask |= (uint8_t)(1U << i);
        }
    }

    return mask;
}

enum BalancingReturnCode balancing_api_init(void) {
    balancing_handler.target = 0.F;
    balancing_handler.threshold = 0.F;
    balancing_handler.toggle_request = false;
    balancing_handler.is_active = false;
    balancing_handler.odd_phase = false;

    return BALANCING_RC_OK;
}

enum BalancingReturnCode balancing_api_start(volt threshold) {
    balancing_handler.target = voltage_api_get_min();
    balancing_handler.threshold = threshold;
    balancing_handler.odd_phase = false;
    balancing_handler.is_active = true;

    EAGLETRT_API_UNUSED(
        bms_tasks_api_enable_task(BMS_TASKS_TASK_BALANCING_REFRESH));
    EAGLETRT_API_UNUSED(
        bms_tasks_api_update_task(BMS_TASKS_TASK_BALANCING_TIMEOUT,
                                  0U,
                                  BMS_TASKS_BALANCING_STATE_TIMEOUT_MS,
                                  1U));
    EAGLETRT_API_UNUSED(
        bms_tasks_api_enable_task(BMS_TASKS_TASK_BALANCING_TIMEOUT));

    return BALANCING_RC_OK;
}

enum BalancingReturnCode balancing_api_stop(void) {
    EAGLETRT_API_UNUSED(bms_monitor_api_set_discharge(0U));
    balancing_handler.is_active = false;

    EAGLETRT_API_UNUSED(
        bms_tasks_api_disable_task(BMS_TASKS_TASK_BALANCING_REFRESH));
    EAGLETRT_API_UNUSED(
        bms_tasks_api_disable_task(BMS_TASKS_TASK_BALANCING_TIMEOUT));

    return BALANCING_RC_OK;
}

bool balancing_api_is_active(void) {
    return balancing_handler.is_active;
}

enum BalancingReturnCode balancing_api_set_state_handle(bool active,
                                                        volt threshold) {
    if (active) {
        if (!balancing_handler.is_active) {
            return balancing_api_start(threshold);
        }

        EAGLETRT_API_UNUSED(
            bms_tasks_api_update_task(BMS_TASKS_TASK_BALANCING_TIMEOUT,
                                      0U,
                                      BMS_TASKS_BALANCING_STATE_TIMEOUT_MS,
                                      1U));
    } else if (balancing_handler.is_active) {
        return balancing_api_stop();
    }

    return BALANCING_RC_OK;
}

enum BalancingReturnCode balancing_api_routine_toggle(void) {
    balancing_handler.toggle_request = true;
    return BALANCING_RC_OK;
}

/*!
 * \defgroup         balancing_api_tasks Balancing module's tasks
 * \{
 */

void balancing_api_timeout_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    EAGLETRT_API_UNUSED(balancing_api_stop());
}

void balancing_api_refresh_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    if (!balancing_handler.is_active) {
        return;
    }

    balancing_handler.odd_phase = !balancing_handler.odd_phase;
    uint8_t mask = prv_balancing_api_compute_mask(balancing_handler.odd_phase);

    if (mask == 0U) {
        balancing_handler.odd_phase = !balancing_handler.odd_phase;
        mask = prv_balancing_api_compute_mask(balancing_handler.odd_phase);
        if (mask == 0U) {
            EAGLETRT_API_UNUSED(balancing_api_stop());
            return;
        }
    }

    EAGLETRT_API_UNUSED(bms_monitor_api_set_discharge(mask));
}

void balancing_api_routine_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    if (!balancing_handler.toggle_request) {
        return;
    }
    balancing_handler.toggle_request = false;

    if (balancing_handler.is_active) {
        EAGLETRT_API_UNUSED(balancing_api_stop());
        logger_api_log(LOGGER_LEVEL_INFO, "[BAL] off");
    } else if (balancing_api_start(BALANCING_THRESHOLD_V) == BALANCING_RC_OK) {
        logger_api_log(LOGGER_LEVEL_INFO, "[BAL] on");
    } else {
        logger_api_log(LOGGER_LEVEL_WARN,
                       "[BAL] refused, pack voltages not valid");
    }
}

/*!
 * \} */

#endif /*! CONFIG_BALANCING_MODULE_ENABLE */
