/*!
 * \file            bms-tasks-api.h
 * \date            2026-10-16
 *
 * \brief           LV BMS tasks module APIs.
 */

#ifndef BMS_TASKS_API_H
#define BMS_TASKS_API_H

#include "bms-tasks.h"
#include "timebase.h"

/*!
 * \brief            Initialize the bms-tasks module.
 *
 * \param[in]        timebase_handler TBD.
 * \param[in]        feedback Samples the digital feedbacks.
 * \param[in]        heartbeat Toggles the heartbeat LED.
 *
 * \retval           BMS_TASKS_RC_OK if the services were registered.
 * \retval           BMS_TASKS_RC_NULL_POINTER if any argument is NULL.
 */
enum TasksReturnCode bms_tasks_api_init(uint32_t (*get_tick_callback)(void),
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
enum TasksReturnCode bms_tasks_api_enable_task(enum BmsTasksTaskId task_id);

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
enum TasksReturnCode bms_tasks_api_pause_task(enum BmsTasksTaskId task_id);

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
enum TasksReturnCode bms_tasks_api_disable_task(enum BmsTasksTaskId task_id);

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
enum TasksReturnCode bms_tasks_api_update_task(
    enum BmsTasksTaskId task_id,
    uint16_t new_interval,
    uint16_t new_start,
    uint8_t repeats);

#endif /*! BMS_TASKS_API_H */
