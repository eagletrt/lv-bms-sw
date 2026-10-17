/*!
 * \file            application.h
 * \date            2026-10-16
 *
 * \brief           Application module data structures and constants.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "fsm.h"
#include "tasks.h"
#include "timebase.h"

#define APPLICATION_MONITOR_PERIOD_MS (2U)      /*!< Step period of the BMS monitor FSM. */
#define APPLICATION_RELAY_PERIOD_MS (50U)       /*!< Supervision period of the master relay. */
#define APPLICATION_HEARTBEAT_PERIOD_MS (500U)  /*!< Toggling period of the heartbeat LED. */
#define APPLICATION_FEEDBACK_PERIOD_MS (10U)    /*!< Sampling period of the digital feedbacks. */
#define APPLICATION_DISCHARGE_PERIOD_MS (5000U) /*!< Dwell on each cell during the discharge sweep. */

/*!
 * \brief            Return codes for the application module functions.
 */
enum ApplicationReturnCode {
    APPLICATION_RC_OK,           /*!< Function executed successfully */
    APPLICATION_RC_NULL_POINTER, /*!< Unexpected NULL pointer */
};

/*!
 * \brief            Identifiers of the scheduled application tasks.
 */
enum ApplicationTaskId {
    APPLICATION_TASK_MONITOR,
    APPLICATION_TASK_RELAY,
    APPLICATION_TASK_FEEDBACK,
    APPLICATION_TASK_HEARTBEAT,
    APPLICATION_TASK_STATE,
    APPLICATION_TASK_VERSION,
    APPLICATION_TASK_LIBCAN_VERSION,
    APPLICATION_TASK_CELL_VOLTAGES,
    APPLICATION_TASK_VOLTAGE_INFORMATION,
    APPLICATION_TASK_TEMPERATURES,
    APPLICATION_TASK_DISCHARGE,
    APPLICATION_TASK_COUNT
};

/*!
 * \brief            Board services the application module cannot reach for itself.
 */
struct ApplicationServices {
    const state_t *state;               /*!< Current FSM state, read by the task gate */
    const bool *failed;                 /*!< Scheduler failure flag, read by the task gate */
    void (*feedback)(uint8_t task_id);  /*!< Samples the digital feedbacks */
    void (*heartbeat)(uint8_t task_id); /*!< Toggles the heartbeat LED */
    void (*discharge)(uint8_t task_id); /*!< Steps the discharge sweep */
};

extern struct ApplicationServices application_services;
extern struct TimebaseHandler application_timebase;
extern TaskList application_task_list;

#endif /*! APPLICATION_H */
