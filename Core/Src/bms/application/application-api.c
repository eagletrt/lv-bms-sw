/*!
 * \file            application-api.c
 * \date            2026-10-16
 *
 * \brief           Application module APIs.
 */

#include "application-api.h"

#include "identity-api.h"
#include "temperature-api.h"
#include "voltage-api.h"

struct ApplicationServices application_services;
struct TimebaseHandler application_timebase;

/*!
 * \brief            Whether the board callbacks may run.
 *
 * \returns          bool True when the services are usable.
 */
EAGLETRT_STATIC bool prv_application_is_ready(void) {
    return application_services.state != NULL &&
           application_services.failed != NULL &&
           !*application_services.failed;
}

/*!
 * \brief            Whether a periodic module task may run.
 *
 * \returns          bool True when the module tasks may run.
 */
EAGLETRT_STATIC bool prv_application_is_running(void) {
    if (!prv_application_is_ready()) {
        return false;
    }
    return *application_services.state == STATE_IDLE || *application_services.state == STATE_BALANCING;
}

EAGLETRT_STATIC void prv_application_monitor_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    if (prv_application_is_running()) {
        fsm_monitor_step();
    }
}

EAGLETRT_STATIC void prv_application_relay_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    if (prv_application_is_running()) {
        fsm_relay_supervise();
    }
}

EAGLETRT_STATIC void prv_application_feedback_task(uint8_t task_id) {
    if (prv_application_is_ready() && application_services.feedback != NULL) {
        application_services.feedback(task_id);
    }
}

EAGLETRT_STATIC void prv_application_heartbeat_task(uint8_t task_id) {
    if (prv_application_is_ready() && application_services.heartbeat != NULL) {
        application_services.heartbeat(task_id);
    }
}

EAGLETRT_STATIC void prv_application_discharge_task(uint8_t task_id) {
    if (prv_application_is_ready() && application_services.discharge != NULL) {
        application_services.discharge(task_id);
    }
}

EAGLETRT_STATIC void prv_application_state_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    if (prv_application_is_running()) {
        identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_IDLE);
    }
}

EAGLETRT_STATIC void prv_application_version_task(uint8_t task_id) {
    if (prv_application_is_running()) {
        identity_api_send_version(task_id);
    }
}

EAGLETRT_STATIC void prv_application_libcan_version_task(uint8_t task_id) {
    if (prv_application_is_running()) {
        identity_api_send_libcan_version(task_id);
    }
}

EAGLETRT_STATIC void prv_application_cell_voltages_task(uint8_t task_id) {
    if (prv_application_is_running()) {
        voltage_api_send_cell_voltages(task_id);
    }
}

EAGLETRT_STATIC void prv_application_voltage_information_task(uint8_t task_id) {
    if (prv_application_is_running()) {
        voltage_api_send_voltage_information(task_id);
    }
}

EAGLETRT_STATIC void prv_application_temperatures_task(uint8_t task_id) {
    if (prv_application_is_running()) {
        temperature_api_send_temperatures(task_id);
    }
}

TaskList application_task_list = {
    [APPLICATION_TASK_MONITOR] = {
        .task_id = APPLICATION_TASK_MONITOR,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_monitor_task,
        .interval = APPLICATION_MONITOR_PERIOD_MS,
        .start = APPLICATION_MONITOR_PERIOD_MS,
    },
    [APPLICATION_TASK_RELAY] = {
        .task_id = APPLICATION_TASK_RELAY,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_relay_task,
        .interval = APPLICATION_RELAY_PERIOD_MS,
        .start = APPLICATION_RELAY_PERIOD_MS,
    },
    [APPLICATION_TASK_FEEDBACK] = {
        .task_id = APPLICATION_TASK_FEEDBACK,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_feedback_task,
        .interval = APPLICATION_FEEDBACK_PERIOD_MS,
        .start = APPLICATION_FEEDBACK_PERIOD_MS,
    },
    [APPLICATION_TASK_HEARTBEAT] = {
        .task_id = APPLICATION_TASK_HEARTBEAT,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_heartbeat_task,
        .interval = APPLICATION_HEARTBEAT_PERIOD_MS,
        .start = APPLICATION_HEARTBEAT_PERIOD_MS,
    },
    [APPLICATION_TASK_STATE] = {
        .task_id = APPLICATION_TASK_STATE,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_state_task,
        .interval = can_primary_cycle_time_lvacfsm,
        .start = can_primary_cycle_time_lvacfsm,
    },
    [APPLICATION_TASK_VERSION] = {
        .task_id = APPLICATION_TASK_VERSION,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_version_task,
        .interval = can_primary_cycle_time_lvacversion,
        .start = can_primary_cycle_time_lvacversion,
    },
    [APPLICATION_TASK_LIBCAN_VERSION] = {
        .task_id = APPLICATION_TASK_LIBCAN_VERSION,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_libcan_version_task,
        .interval = can_primary_cycle_time_lvaclibcanversion,
        .start = can_primary_cycle_time_lvaclibcanversion,
    },
    [APPLICATION_TASK_CELL_VOLTAGES] = {
        .task_id = APPLICATION_TASK_CELL_VOLTAGES,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_cell_voltages_task,
        .interval = can_primary_cycle_time_lvaccellvoltage,
        .start = can_primary_cycle_time_lvaccellvoltage,
    },
    [APPLICATION_TASK_VOLTAGE_INFORMATION] = {
        .task_id = APPLICATION_TASK_VOLTAGE_INFORMATION,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_voltage_information_task,
        .interval = can_primary_cycle_time_lvacvoltageinfo,
        .start = can_primary_cycle_time_lvacvoltageinfo,
    },
    [APPLICATION_TASK_TEMPERATURES] = {
        .task_id = APPLICATION_TASK_TEMPERATURES,
        .state = TASKS_STATE_ENABLED,
        .function = prv_application_temperatures_task,
        .interval = can_primary_cycle_time_lvactemperature,
        .start = can_primary_cycle_time_lvactemperature,
    },
    [APPLICATION_TASK_DISCHARGE] = {
        .task_id = APPLICATION_TASK_DISCHARGE,
        .state = TASKS_STATE_DISABLED,
        .function = prv_application_discharge_task,
        .interval = APPLICATION_DISCHARGE_PERIOD_MS,
        .start = APPLICATION_DISCHARGE_PERIOD_MS,
    },
};

enum ApplicationReturnCode application_api_init(
    const state_t *state,
    const bool *failed,
    void (*feedback)(uint8_t task_id),
    void (*heartbeat)(uint8_t task_id),
    void (*discharge)(uint8_t task_id)) {
    if (state == NULL || failed == NULL || feedback == NULL || heartbeat == NULL || discharge == NULL) {
        return APPLICATION_RC_NULL_POINTER;
    }

    application_services = (struct ApplicationServices){
        .state = state,
        .failed = failed,
        .feedback = feedback,
        .heartbeat = heartbeat,
        .discharge = discharge,
    };

    return APPLICATION_RC_OK;
}
