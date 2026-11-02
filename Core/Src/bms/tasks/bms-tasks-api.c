/*!
 * \file            bms_tasks-api.c
 * \date            2026-10-16
 *
 * \brief           LV BMS Tasks module APIs.
 */

#include "bms-tasks-api.h"
#include "balancing.h"
#include "bms-tasks.h"

#include "discharge-api.h"
#include "identity-api.h"
#include "temperature-api.h"
#include "voltage-api.h"
#include "tasks.h"
#include "tasks-api.h"
#include "balancing-api.h"

EAGLETRT_STATIC struct BmsTasksHandler bms_tasks_handler;

EAGLETRT_STATIC void prv_bms_tasks_monitor_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    fsm_monitor_step();
}

EAGLETRT_STATIC void prv_bms_tasks_relay_task(uint8_t task_id) {
    fsm_relay_supervise(task_id);
}

/*!
 * \brief            Sample the digital feedbacks.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
EAGLETRT_STATIC void prv_bms_tasks_feedback_task(uint8_t task_id) {
    if (bms_tasks_handler.feedback != NULL) {
        bms_tasks_handler.feedback(task_id);
    }
}

/*!
 * \brief            Toggle the heartbeat LED.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
EAGLETRT_STATIC void prv_bms_tasks_heartbeat_task(uint8_t task_id) {
    if (bms_tasks_handler.heartbeat != NULL) {
        bms_tasks_handler.heartbeat(task_id);
    }
}

/*!
 * \brief            Send the current FSM state over CAN.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
EAGLETRT_STATIC void prv_bms_tasks_state_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_IDLE);
}

EAGLETRT_STATIC TaskList bms_tasks_task_list = {
    [BMS_TASKS_TASK_MONITOR] = {
        .task_id = BMS_TASKS_TASK_MONITOR,
        .state = TASKS_STATE_ENABLED,
        .function = prv_bms_tasks_monitor_task,
        .interval = BMS_TASKS_MONITOR_PERIOD_MS,
        .start = BMS_TASKS_MONITOR_PERIOD_MS,
    },
    [BMS_TASKS_TASK_RELAY] = {
        .task_id = BMS_TASKS_TASK_RELAY,
        .state = TASKS_STATE_ENABLED,
        .function = prv_bms_tasks_relay_task,
        .interval = BMS_TASKS_RELAY_PERIOD_MS,
        .start = BMS_TASKS_RELAY_PERIOD_MS,
    },
    [BMS_TASKS_TASK_FEEDBACK] = {
        .task_id = BMS_TASKS_TASK_FEEDBACK,
        .state = TASKS_STATE_ENABLED,
        .function = prv_bms_tasks_feedback_task,
        .interval = BMS_TASKS_FEEDBACK_PERIOD_MS,
        .start = BMS_TASKS_FEEDBACK_PERIOD_MS,
    },
    [BMS_TASKS_TASK_HEARTBEAT] = {
        .task_id = BMS_TASKS_TASK_HEARTBEAT,
        .state = TASKS_STATE_ENABLED,
        .function = prv_bms_tasks_heartbeat_task,
        .interval = BMS_TASKS_HEARTBEAT_PERIOD_MS,
        .start = BMS_TASKS_HEARTBEAT_PERIOD_MS,
    },
    [BMS_TASKS_TASK_STATE] = {
        .task_id = BMS_TASKS_TASK_STATE,
        .state = TASKS_STATE_ENABLED,
        .function = prv_bms_tasks_state_task,
        .interval = can_primary_cycle_time_lvacfsm,
        .start = can_primary_cycle_time_lvacfsm,
    },
    [BMS_TASKS_TASK_VERSION] = {
        .task_id = BMS_TASKS_TASK_VERSION,
        .state = TASKS_STATE_ENABLED,
        .function = identity_api_send_version,
        .interval = can_primary_cycle_time_lvacversion,
        .start = can_primary_cycle_time_lvacversion,
    },
    [BMS_TASKS_TASK_LIBCAN_VERSION] = {
        .task_id = BMS_TASKS_TASK_LIBCAN_VERSION,
        .state = TASKS_STATE_ENABLED,
        .function = identity_api_send_libcan_version,
        .interval = can_primary_cycle_time_lvaclibcanversion,
        .start = can_primary_cycle_time_lvaclibcanversion,
    },
    [BMS_TASKS_TASK_CELL_VOLTAGES] = {
        .task_id = BMS_TASKS_TASK_CELL_VOLTAGES,
        .state = TASKS_STATE_ENABLED,
        .function = voltage_api_send_cell_voltages,
        .interval = can_primary_cycle_time_lvaccellvoltage,
        .start = can_primary_cycle_time_lvaccellvoltage,
    },
    [BMS_TASKS_TASK_VOLTAGE_INFORMATION] = {
        .task_id = BMS_TASKS_TASK_VOLTAGE_INFORMATION,
        .state = TASKS_STATE_ENABLED,
        .function = voltage_api_send_voltage_information,
        .interval = can_primary_cycle_time_lvacvoltageinfo,
        .start = can_primary_cycle_time_lvacvoltageinfo,
    },
    [BMS_TASKS_TASK_TEMPERATURES] = {
        .task_id = BMS_TASKS_TASK_TEMPERATURES,
        .state = TASKS_STATE_ENABLED,
        .function = temperature_api_send_temperatures,
        .interval = can_primary_cycle_time_lvactemperature,
        .start = can_primary_cycle_time_lvactemperature,
    },
    [BMS_TASKS_TASK_DISCHARGE_ROUTINE] = {
        .task_id = BMS_TASKS_TASK_DISCHARGE_ROUTINE,
        .state = TASKS_STATE_DISABLED,
        .function = discharge_api_routine_task,
        .interval = BMS_TASKS_DISCHARGE_ROUTINE_PERIOD_MS,
        .start = BMS_TASKS_DISCHARGE_ROUTINE_PERIOD_MS,
    },
    [BMS_TASKS_TASK_DISCHARGE_STEP] = {
        .task_id = BMS_TASKS_TASK_DISCHARGE_STEP,
        .state = TASKS_STATE_DISABLED,
        .function = discharge_api_step_task,
        .interval = BMS_TASKS_DISCHARGE_STEP_PERIOD_MS,
        .start = BMS_TASKS_DISCHARGE_STEP_PERIOD_MS,
    },
    [BMS_TASKS_TASK_BALANCING_ROUTINE] = {
        .task_id = BMS_TASKS_TASK_BALANCING_ROUTINE,
        .state = TASKS_STATE_DISABLED,
        .repeats = 1U,
        .function = balancing_api_routine_task,
        .start = 0U,
    },
    [BMS_TASKS_TASK_BALANCING_TIMEOUT] = {
        .task_id = BMS_TASKS_TASK_BALANCING_TIMEOUT,
        .state = TASKS_STATE_DISABLED,
        .repeats = 1U,
        .function = balancing_api_timeout_task,
        .start = BMS_TASKS_BALANCING_STATE_TIMEOUT_MS,
    },
    [BMS_TASKS_TASK_BALANCING_REFRESH] = {
        .task_id = BMS_TASKS_TASK_BALANCING_REFRESH,
        .state = TASKS_STATE_DISABLED,
        .repeats = 1U,
        .function = balancing_api_refresh_task,
        .start = BMS_TASKS_BALANCING_RUN_PERIOD_MS,
    }
};

enum TasksReturnCode bms_tasks_api_init(uint32_t (*get_tick_callback)(void),
                                        void (*feedback)(uint8_t task_id),
                                        void (*heartbeat)(uint8_t task_id)) {
    if (get_tick_callback == NULL || heartbeat == NULL || heartbeat == NULL) {
        return TASKS_RC_NULL_POINTER;
    }

    bms_tasks_handler.feedback = feedback;
    bms_tasks_handler.heartbeat = heartbeat;
    EAGLETRT_API_UNUSED(tasks_api_init(&bms_tasks_handler.task_handler,
                                       bms_tasks_task_list,
                                       BMS_TASKS_TASK_COUNT,
                                       get_tick_callback));

    return TASKS_RC_OK;
}

enum TasksReturnCode bms_tasks_api_enable_task(enum BmsTasksTaskId task_id) {
    if (task_id == BMS_TASKS_TASK_COUNT) {
        return TASKS_RC_INVALID_ID;
    }

    return tasks_api_enable_task(&bms_tasks_handler.task_handler, task_id);
}

enum TasksReturnCode bms_tasks_api_pause_task(enum BmsTasksTaskId task_id) {
    if (task_id == BMS_TASKS_TASK_COUNT) {
        return TASKS_RC_INVALID_ID;
    }

    return tasks_api_pause_task(&bms_tasks_handler.task_handler, task_id);
}

enum TasksReturnCode bms_tasks_api_disable_task(enum BmsTasksTaskId task_id) {
    if (task_id == BMS_TASKS_TASK_COUNT) {
        return TASKS_RC_INVALID_ID;
    }

    return tasks_api_disable_task(&bms_tasks_handler.task_handler, task_id);
}

enum TasksReturnCode bms_tasks_api_update_task(enum BmsTasksTaskId task_id,
                                               uint16_t new_interval,
                                               uint16_t new_start,
                                               uint8_t repeats) {
    if (task_id == BMS_TASKS_TASK_COUNT) {
        return TASKS_RC_INVALID_ID;
    }

    return tasks_api_update_task(&bms_tasks_handler.task_handler,
                                 task_id,
                                 new_interval,
                                 new_start,
                                 repeats);
}
