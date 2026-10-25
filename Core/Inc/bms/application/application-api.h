/*!
 * \file            application-api.h
 * \date            2026-10-16
 *
 * \brief           Application module APIs.
 */

#ifndef APPLICATION_API_H
#define APPLICATION_API_H

#include "application.h"

/*!
 * \brief            Initialize the application module.
 *
 * \param[in]        state The current FSM state.
 * \param[in]        failed The scheduler failure flag.
 * \param[in]        feedback Samples the digital feedbacks.
 * \param[in]        heartbeat Toggles the heartbeat LED.
 * \param[in]        discharge Steps the discharge sweep.
 *
 * \retval           APPLICATION_RC_OK if the services were registered.
 * \retval           APPLICATION_RC_NULL_POINTER if any argument is NULL.
 */
enum ApplicationReturnCode application_api_init(
    const state_t *state,
    const bool *failed,
    void (*feedback)(uint8_t task_id),
    void (*heartbeat)(uint8_t task_id),
    void (*discharge)(uint8_t task_id));

#endif /*! APPLICATION_API_H */
