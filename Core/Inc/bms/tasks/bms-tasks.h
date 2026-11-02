/*!
 * \file            bms-tasks.h
 * \date            2026-10-16
 *
 * \brief           LV BMS Tasks module data structures and constants.
 */

#ifndef BMS_TASKS_H
#define BMS_TASKS_H

#include "fsm.h"
#include "tasks.h"
#include "timebase.h"

#define BMS_TASKS_MONITOR_PERIOD_MS (2U)             /*!< Step period of the BMS monitor FSM */
#define BMS_TASKS_RELAY_PERIOD_MS (50U)              /*!< Supervision period of the master relay */
#define BMS_TASKS_HEARTBEAT_PERIOD_MS (500U)         /*!< Toggling period of the heartbeat LED */
#define BMS_TASKS_FEEDBACK_PERIOD_MS (10U)           /*!< Sampling period of the digital feedbacks */
#define BMS_TASKS_DISCHARGE_ROUTINE_PERIOD_MS (10U)  /*!< Routine period of the discharge demo */
#define BMS_TASKS_DISCHARGE_STEP_PERIOD_MS (5000U)   /*!< Dwell on each cell during the discharge sweep */
#define BMS_TASKS_BALANCING_ROUTINE_PERIOD_MS (10U)  /*!< Routine period of the balancing demo */
#define BMS_TASKS_BALANCING_STATE_TIMEOUT_MS (3000U) /*!< Discharge mask refresh period in ms, well under the LTC 30 s discharge timeout */
#define BMS_TASKS_BALANCING_RUN_PERIOD_MS (1000U)    /*!< The period of time without receiving balancing state updates in ms */

/*!
 * \brief            Return codes for the bms-tasks module functions.
 */
enum BmsTasksReturnCode {
    BMS_TASKS_RC_OK,           /*!< Function executed successfully */
    BMS_TASKS_RC_NULL_POINTER, /*!< Unexpected NULL pointer */
    BMS_TASKS_RC_INVALID_ID
};

/*!
 * \brief            Identifiers of the scheduled bms-tasks tasks.
 */
enum BmsTasksTaskId {
    BMS_TASKS_TASK_MONITOR,             /*!< */
    BMS_TASKS_TASK_RELAY,               /*!< */
    BMS_TASKS_TASK_FEEDBACK,            /*!< */
    BMS_TASKS_TASK_HEARTBEAT,           /*!< */
    BMS_TASKS_TASK_STATE,               /*!< */
    BMS_TASKS_TASK_VERSION,             /*!< */
    BMS_TASKS_TASK_LIBCAN_VERSION,      /*!< */
    BMS_TASKS_TASK_CELL_VOLTAGES,       /*!< */
    BMS_TASKS_TASK_VOLTAGE_INFORMATION, /*!< */
    BMS_TASKS_TASK_TEMPERATURES,        /*!< */
    BMS_TASKS_TASK_DISCHARGE_ROUTINE,   /*!< */
    BMS_TASKS_TASK_DISCHARGE_STEP,      /*!< */
    BMS_TASKS_TASK_BALANCING_ROUTINE,   /*!< */
    BMS_TASKS_TASK_BALANCING_REFRESH,   /*!< */
    BMS_TASKS_TASK_BALANCING_TIMEOUT,   /*!< */
    BMS_TASKS_TASK_COUNT                /*!< The number of LV BMS tasks */
};

/*!
 * \brief            BMS tasks handler structure.
 */
struct BmsTasksHandler {
    struct TasksHandler task_handler;         /*!< The tasks handler structure */
    void (*feedback)(uint8_t task_id);        /*!< Samples the digital feedbacks */
    void (*heartbeat)(uint8_t task_id);       /*!< Toggles the heartbeat LED */
};

#endif /*! BMS_TASKS_H */
