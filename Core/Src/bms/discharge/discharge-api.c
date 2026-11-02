/*!
 * \file            discharge-api.h
 * \date            2026-10-27
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Discharge module APIs.
 */

#include "bms-tasks-api.h"
#include "bms-tasks.h"
#include "discharge.h"
#include "discharge-api.h"

#include "balancing-api.h"
#include "bms-monitor-api.h"
#include "defines.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "logger.h"
#include "logger-api.h"

EAGLETRT_STATIC struct DischargeHandler discharge_handler;

enum DischargeReturnCode discharge_api_init(void) {
    discharge_handler.is_active = false;
    discharge_handler.toggle_request = false;
    return DISCHARGE_RC_OK;
}

enum DischargeReturnCode discharge_api_toggle(void) {
    discharge_handler.toggle_request = true;
    return DISCHARGE_RC_OK;
}

void discharge_api_routine_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    if (balancing_api_is_active()) {
        return;
    }

    if (discharge_handler.toggle_request) {
        discharge_handler.toggle_request = false;
        discharge_handler.is_active = !discharge_handler.is_active;
        discharge_handler.current_cell = 0U;

        if (discharge_handler.is_active) {
            bms_tasks_api_enable_task(BMS_TASKS_TASK_DISCHARGE_STEP);
        } else {
            bms_tasks_api_disable_task(BMS_TASKS_TASK_DISCHARGE_STEP);
        }

        EAGLETRT_API_UNUSED(bms_monitor_api_set_discharge(discharge_handler.is_active
                                                              ? (uint8_t)(1U << discharge_handler.current_cell)
                                                              : 0U));
        logger_api_log(LOGGER_LEVEL_INFO,
                       "[DCHG] %s",
                       discharge_handler.is_active ? "sweep on, cell 1" : "sweep off");
    }
}

void discharge_api_step_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    if (discharge_handler.is_active && !balancing_api_is_active()) {
        EAGLETRT_API_UNUSED(bms_monitor_api_set_discharge((uint8_t)(1U << discharge_handler.current_cell)));
        logger_api_log(LOGGER_LEVEL_INFO, "[DCHG] cell %d", (int)(discharge_handler.current_cell + 1U));
        discharge_handler.current_cell =
            (uint8_t)((discharge_handler.current_cell + 1U) % DEFINES_CELLS_SERIES_COUNT);
    }
}
