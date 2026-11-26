/*!
 * \file            scheduler.h
 * \date            2026-10-16
 *
 * \brief           LV BMS Tasks module data structures and constants.
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "fsm.h"
#include "tasks.h"
#include "timebase.h"
#include "watchdogs.h"

#define SCHEDULER_MAIN_PERIOD_MS (2U)                /*!< Step period of the BMS monitor FSM */
#define SCHEDULER_MONITOR_PERIOD_MS (2U)             /*!< Step period of the BMS monitor FSM */
#define SCHEDULER_ADC_MUX_SETTLE_PERIOD_MS (2U)      /*!< Time given to the multiplexer and to the NTC divider to settle after an address change. */
#define SCHEDULER_ADC_SCAN_TIMEOUT_MS (200U)         /*!< A scan that does not complete within this time is aborted and restarted. Has to stay above the worst-case blocking time of the debug print. */
#define SCHEDULER_RELAY_PERIOD_MS (50U)              /*!< Supervision period of the master relay */
#define SCHEDULER_HEARTBEAT_PERIOD_MS (500U)         /*!< Toggling period of the heartbeat LED */
#define SCHEDULER_FEEDBACK_PERIOD_MS (10U)           /*!< Sampling period of the digital feedbacks */
#define SCHEDULER_DISCHARGE_ROUTINE_PERIOD_MS (10U)  /*!< Routine period of the discharge demo */
#define SCHEDULER_DISCHARGE_STEP_PERIOD_MS (5000U)   /*!< Dwell on each cell during the discharge sweep */
#define SCHEDULER_BALANCING_ROUTINE_PERIOD_MS (10U)  /*!< Routine period of the balancing demo */
#define SCHEDULER_BALANCING_STATE_TIMEOUT_MS (3000U) /*!< Discharge mask refresh period in ms, well under the LTC 30 s discharge timeout */
#define SCHEDULER_BALANCING_RUN_PERIOD_MS (1000U)    /*!< The period of time without receiving balancing state updates in ms */

/*!
 * \brief            Return codes for the scheduler module functions.
 */
enum SchedulerReturnCode {
    SCHEDULER_RC_OK,           /*!< Function executed successfully */
    SCHEDULER_RC_NULL_POINTER, /*!< Unexpected NULL pointer */
    SCHEDULER_RC_INVALID_ID
};

/*!
 * \brief            Identifiers of the scheduled scheduler tasks.
 */
enum SchedulerTaskId {
    SCHEDULER_TASK_MAIN_FSM,            /*!< */
    SCHEDULER_TASK_MONITOR_FSM,         /*!< */
    SCHEDULER_TASK_ADC_ROUTINE,         /*!< */
    SCHEDULER_TASK_ADC_SETTLE,          /*!< */
    SCHEDULER_TASK_ADC_TIMEOUT,         /*!< */
    SCHEDULER_TASK_RELAY,               /*!< */
    SCHEDULER_TASK_FEEDBACK,            /*!< */
    SCHEDULER_TASK_HEARTBEAT,           /*!< */
    SCHEDULER_TASK_STATE,               /*!< */
    SCHEDULER_TASK_VERSION,             /*!< */
    SCHEDULER_TASK_LIBCAN_VERSION,      /*!< */
    SCHEDULER_TASK_CELL_VOLTAGES,       /*!< */
    SCHEDULER_TASK_VOLTAGE_INFORMATION, /*!< */
    SCHEDULER_TASK_TEMPERATURES,        /*!< */
    SCHEDULER_TASK_DISCHARGE_ROUTINE,   /*!< */
    SCHEDULER_TASK_DISCHARGE_STEP,      /*!< */
    SCHEDULER_TASK_BALANCING_ROUTINE,   /*!< */
    SCHEDULER_TASK_BALANCING_REFRESH,   /*!< */
    SCHEDULER_TASK_BALANCING_TIMEOUT,   /*!< */
    SCHEDULER_TASK_COUNT                /*!< The number of LV BMS tasks */
};

/*!
 * \brief            BMS tasks handler structure.
 */
struct SchedulerHandler {
    struct TasksHandler task_handler;        /*!< The tasks handler structure */
    struct WatchdogHandler watchdog_handler; /*!< The watchdog handler structure */
    void (*feedback)(uint8_t task_id);       /*!< Samples the digital feedbacks */
    void (*heartbeat)(uint8_t task_id);      /*!< Toggles the heartbeat LED */
};

#endif /*! SCHEDULER_H */
