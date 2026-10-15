#include "application.h"
#include "identity-api.h"
#include "tasks.h"
#include "voltage-api.h"
#include "temperature-api.h"

struct ApplicationServices application_services;
struct TimebaseHandler application_timebase;

EAGLETRT_STATIC void prv_application_task(uint8_t task_id) {
    if (application_services.state == NULL || application_services.failed == NULL ||
        *application_services.failed) {
        return;
    }
    if (task_id == APPLICATION_TASK_FEEDBACK) {
        if (application_services.feedback != NULL) {
            application_services.feedback();
        }
        return;
    }
    if (task_id == APPLICATION_TASK_HEARTBEAT) {
        if (application_services.heartbeat != NULL) {
            application_services.heartbeat();
        }
        return;
    }
    if (task_id == APPLICATION_TASK_DISCHARGE) {
        if (application_services.discharge != NULL) {
            application_services.discharge();
        }
        return;
    }
    if (*application_services.state != STATE_IDLE && *application_services.state != STATE_BALANCING) {
        return;
    }
    switch (task_id) {
        case APPLICATION_TASK_MONITOR:
            fsm_monitor_step();
            break;
        case APPLICATION_TASK_RELAY:
            fsm_relay_supervise();
            break;
        case APPLICATION_TASK_STATE:
            EAGLETRT_API_UNUSED(identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_IDLE));
            break;
        case APPLICATION_TASK_VERSION:
            EAGLETRT_API_UNUSED(identity_api_send_version());
            break;
        case APPLICATION_TASK_LIBCAN_VERSION:
            EAGLETRT_API_UNUSED(identity_api_send_libcan_version());
            break;
        case APPLICATION_TASK_CELL_VOLTAGES:
            EAGLETRT_API_UNUSED(voltage_api_send_cell_voltages());
            break;
        case APPLICATION_TASK_VOLTAGE_INFORMATION:
            EAGLETRT_API_UNUSED(voltage_api_send_voltage_information());
            break;
        case APPLICATION_TASK_TEMPERATURES:
            EAGLETRT_API_UNUSED(temperature_api_send_temperatures());
            break;
        default:
            break;
    }
}

#define APPLICATION_TASK_ENTRY(id, period) \
    [id] = { .task_id = (id), .state = TASKS_STATE_ENABLED, .function = prv_application_task, .interval = (period), .start = (period) }

TaskList application_task_list = {
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_MONITOR, 2U),
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_RELAY, 50U),
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_FEEDBACK, APPLICATION_FEEDBACK_PERIOD_MS),
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_HEARTBEAT, APPLICATION_HEARTBEAT_PERIOD_MS),
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_STATE, can_primary_cycle_time_lvacfsm),
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_VERSION, can_primary_cycle_time_lvacversion),
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_LIBCAN_VERSION, can_primary_cycle_time_lvaclibcanversion),
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_CELL_VOLTAGES, can_primary_cycle_time_lvaccellvoltage),       /*!< Timing for LvacCellVoltage. */
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_VOLTAGE_INFORMATION, can_primary_cycle_time_lvacvoltageinfo), /*!< Timing for LvacVoltageInfo. */
    APPLICATION_TASK_ENTRY(APPLICATION_TASK_TEMPERATURES, can_primary_cycle_time_lvactemperature),        /*!< Timing for periodic temperature CAN send. */
    [APPLICATION_TASK_DISCHARGE] = {
        .task_id = APPLICATION_TASK_DISCHARGE,
        .state = TASKS_STATE_DISABLED,
        .function = prv_application_task,
        .interval = APPLICATION_DISCHARGE_PERIOD_MS,
        .start = APPLICATION_DISCHARGE_PERIOD_MS } /*!< Dwell from the selection of the current cell. */
};
