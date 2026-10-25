/*!
 * \file            identity-api.h
 * \authors         Thomas Moretti [majorfrost173@gmail.com]
 * \date            2026-08-07
 *
 * \brief           Public API for the module dedicated to providing information about the LV BMS firmware and its dependencies.
 */

#ifndef IDENTITY_API_H
#define IDENTITY_API_H

#include "identity.h"
#include "can-primary.h"

/*!
 * \brief            Initialize the identity module.
 *
 * \retval           IDENTITY_RC_OK on success.
 */
enum IdentityReturnCode identity_api_init(void);

/*!
 * \brief            Send information about the board state.
 *
 * \param[in]        status The current FSM status
 */
void identity_api_send_state(enum CanPrimaryLvacfsmStatus status);

/*!
 * \brief            Periodically send the version of the board firmware and information related to it.
 *
 * \note             The application task scheduler supplies the CAN cycle timing.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
void identity_api_send_version(uint8_t task_id);

/*!
 * \brief            Periodically send the version of the libcan dependency and information related to it.
 *
 * \note             The application task scheduler supplies the CAN cycle timing.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
void identity_api_send_libcan_version(uint8_t task_id);

#endif // IDENTITY_API_H
