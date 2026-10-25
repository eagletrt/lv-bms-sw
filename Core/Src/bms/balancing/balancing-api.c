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
#include "tasks-api.h"

#ifdef CONFIG_BALANCING_MODULE_ENABLE

EAGLETRT_STATIC struct BalancingHandler balancing_handler; /*!< Private balancing handler instance. */

/*!
 * \brief            Identifiers of the internal balancing tasks.
 */
enum BalancingTaskId {
    BALANCING_TASK_TIMEOUT, /*!< Timeout from the tick of the last executed state set */
    BALANCING_TASK_REFRESH, /*!< Periodic cadence gating of the executed runs */
    BALANCING_TASK_COUNT /*!< Number of balancing tasks */
};

EAGLETRT_STATIC void prv_balancing_api_refresh(uint8_t task_id);

/*!
 * \brief            Stop balancing when the state-update timeout expires.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
EAGLETRT_STATIC void prv_balancing_api_timeout(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    if (balancing_api_stop() == BALANCING_RC_OK) {
        balancing_handler.result = BALANCING_RC_STATE_TIMEOUT;
    }
}

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
 * \param[in]        odd_parity true to consider odd-indexed cells, false for even-indexed ones.
 *
 * \returns          uint8_t The cell bitmask, bit 0 being the first cell.
 */
EAGLETRT_STATIC uint8_t prv_balancing_api_compute_mask(bool odd_parity) {
    volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 0.F };
    if (voltage_api_dump_voltages(voltages, 0U, DEFINES_CELLS_SERIES_COUNT) != VOLTAGE_RC_OK) {
        return 0U;
    }

    uint8_t mask = 0U;
    for (size_t i = 0U; i < DEFINES_CELLS_SERIES_COUNT; ++i) {
        if (prv_balancing_api_cell_need_discharge(voltages[i]) && (((i & 1U) != 0U) == odd_parity)) {
            mask |= (uint8_t)(1U << i);
        }
    }

    return mask;
}

enum BalancingReturnCode balancing_api_init(balancing_get_tick_callback get_tick) {
    if (get_tick == NULL) {
        return BALANCING_RC_NULL_POINTER;
    }

    arena_allocator_api_free(&balancing_handler.tasks.arena_handler);
    memset(&balancing_handler, 0, sizeof(balancing_handler));
    TaskList tasks = {
        [BALANCING_TASK_TIMEOUT] = {
            .task_id = BALANCING_TASK_TIMEOUT,
            .state = TASKS_STATE_DISABLED,
            .repeats = 1U,
            .function = prv_balancing_api_timeout,
            .start = BALANCING_STATE_TIMEOUT_MS
        },
        [BALANCING_TASK_REFRESH] = {
            .task_id = BALANCING_TASK_REFRESH,
            .state = TASKS_STATE_DISABLED,
            .function = prv_balancing_api_refresh,
            .interval = BALANCING_RUN_PERIOD_MS,
            .start = BALANCING_RUN_PERIOD_MS
        }
    };
    if (tasks_api_init(&balancing_handler.tasks, tasks, BALANCING_TASK_COUNT, get_tick) != TASKS_RC_OK) {
        balancing_handler.result = BALANCING_RC_SCHEDULER_ERROR;
        return BALANCING_RC_SCHEDULER_ERROR;
    }

    return BALANCING_RC_OK;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
enum BalancingReturnCode balancing_api_start(volt threshold) {
    // TODO: add threshold check.

    balancing_handler.target = voltage_api_get_min();
    balancing_handler.threshold = threshold;
    balancing_handler.odd_phase = false;
    balancing_handler.result = BALANCING_RC_OK;
    if (tasks_api_enable_task(&balancing_handler.tasks, BALANCING_TASK_REFRESH) != TASKS_RC_OK ||
        tasks_api_update_task(&balancing_handler.tasks, BALANCING_TASK_TIMEOUT,
                              0U, BALANCING_STATE_TIMEOUT_MS, 1U) != TASKS_RC_OK ||
        tasks_api_enable_task(&balancing_handler.tasks, BALANCING_TASK_TIMEOUT) != TASKS_RC_OK) {
        EAGLETRT_API_UNUSED(balancing_api_stop());
        balancing_handler.result = BALANCING_RC_SCHEDULER_ERROR;
        return BALANCING_RC_SCHEDULER_ERROR;
    }
    balancing_handler.is_active = true;

    return BALANCING_RC_OK;
}

enum BalancingReturnCode balancing_api_stop(void) {
    EAGLETRT_API_UNUSED(bms_monitor_api_set_discharge(0U));
    balancing_handler.is_active = false;
    if (tasks_api_disable_task(&balancing_handler.tasks, BALANCING_TASK_REFRESH) != TASKS_RC_OK ||
        tasks_api_disable_task(&balancing_handler.tasks, BALANCING_TASK_TIMEOUT) != TASKS_RC_OK) {
        balancing_handler.result = BALANCING_RC_SCHEDULER_ERROR;
        return BALANCING_RC_SCHEDULER_ERROR;
    }
    return BALANCING_RC_OK;
}

/*!
 * \brief            Recompute the discharge mask and push it to the monitor.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
EAGLETRT_STATIC void prv_balancing_api_refresh(uint8_t task_id) {
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

enum BalancingReturnCode balancing_api_run(void) {
    if (balancing_handler.result == BALANCING_RC_SCHEDULER_ERROR ||
        tasks_api_routine(&balancing_handler.tasks) != TASKS_RC_OK) {
        EAGLETRT_API_UNUSED(bms_monitor_api_set_discharge(0U));
        balancing_handler.is_active = false;
        balancing_handler.result = BALANCING_RC_SCHEDULER_ERROR;
    }
    return balancing_handler.result;
}

bool balancing_api_is_active(void) {
    return balancing_handler.is_active;
}

enum BalancingReturnCode balancing_api_set_state_handle(bool active, volt threshold) {
    if (active && !balancing_handler.is_active) {
        return balancing_api_start(threshold);
    }
    if (!active && balancing_handler.is_active) {
        return balancing_api_stop();
    }
    if (active) {
        if (tasks_api_update_task(&balancing_handler.tasks, BALANCING_TASK_TIMEOUT,
                                  0U, BALANCING_STATE_TIMEOUT_MS, 1U) != TASKS_RC_OK) {
            EAGLETRT_API_UNUSED(balancing_api_stop());
            balancing_handler.result = BALANCING_RC_SCHEDULER_ERROR;
            return BALANCING_RC_SCHEDULER_ERROR;
        }
    }

    return BALANCING_RC_OK;
}

#endif /*! CONFIG_BALANCING_MODULE_ENABLE */
