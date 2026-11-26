/*!
 * \file            scheduler-api.h
 * \date            2026-10-16
 *
 * \brief           LV BMS tasks module APIs.
 */

#ifndef SCHEDULER_API_H
#define SCHEDULER_API_H

#include "scheduler.h"
#include "timebase.h"

/*!
 * \brief            Initialize the scheduler module.
 *
 * \param[in]        timebase_handler TBD.
 * \param[in]        feedback Samples the digital feedbacks.
 * \param[in]        heartbeat Toggles the heartbeat LED.
 *
 * \retval           SCHEDULER_RC_OK if the services were registered.
 * \retval           SCHEDULER_RC_NULL_POINTER if any argument is NULL.
 */
enum TasksReturnCode scheduler_api_init(uint32_t (*get_tick_callback)(void),
                                        void (*feedback)(uint8_t task_id),
                                        void (*heartbeat)(uint8_t task_id));

/*!
 * \brief            Enable a single LV BMS task.
 *
 * \details          It's a `tasks_api_enable_task` function wrapper.
 *
 * \retval           TASKS_RC_OK The operation was successful
 * \retval           TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval           TASKS_RC_INVALID_ID The given identifier does not exist
 * \retval           TASKS_RC_ERROR An error occurred during the operation
 * \retval           TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_enable_task(enum SchedulerTaskId task_id);

/*!
 * \brief            Enable a single LV BMS task.
 *
 * \details          It's a `tasks_api_pause_task` function wrapper.
 *
 * \retval           TASKS_RC_OK The operation was successful
 * \retval           TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval           TASKS_RC_INVALID_ID The given identifier does not exist
 * \retval           TASKS_RC_ERROR An error occurred during the operation
 * \retval           TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_pause_task(enum SchedulerTaskId task_id);

/*!
 * \brief            Disable a single LV BMS task.
 *
 * \details          It's a `tasks_api_disable_task` function wrapper.
 *
 * \retval           TASKS_RC_OK The operation was successful
 * \retval           TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval           TASKS_RC_INVALID_ID The given identifier does not exist
 * \retval           TASKS_RC_ERROR An error occurred during the operation
 * \retval           TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_disable_task(enum SchedulerTaskId task_id);

/*!
 * \brief            Update a single LV BMS task information.
 *
 * \details          It's a `tasks_api_update_task` function wrapper.
 *
 * \retval           TASKS_RC_OK The operation was successful
 * \retval           TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval           TASKS_RC_INVALID_ID The given identifier does not exist
 * \retval           TASKS_RC_ERROR An error occurred during the operation
 * \retval           TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_update_task(
    enum SchedulerTaskId task_id,
    uint16_t new_interval,
    uint16_t new_start,
    uint8_t repeats);

/*!
 * \brief            Routine to be called in the main loop to execute the scheduled tasks.
 *
 * \details          It's a `tasks_api_routine` function wrapper.
 *
 * \retval           TASKS_RC_OK The operation was successful
 * \retval           TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval           TASKS_RC_ERROR An error occurred during the operation
 * \retval           TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_routine(void);

#endif /*! SCHEDULER_API_H */
