/*!
 * \file identity-api.h
 * \author Thomas Moretti [majorfrost173@gmail.com]
 * \date 2026-08-07
 * \brief Public API for the module dedicated to providing information about the LV BMS firmware and its dependencies.
 */

#ifndef IDENTITY_API_H
#define IDENTITY_API_H

#include "identity.h"
#include "can-primary.h"

/*!
 * \brief Initialize the identity module
 *
 * \retval IDENTITY_RC_OK if the module was successfully initalizated
 */
enum IdentityReturnCode identity_api_init(void);

/*!
 * \brief Send information about the board state
 *
 * \param[in] status The current FSM status
 *
 * \retval IDENTITY_RC_OK if the state was sent successfully
 */
enum IdentityReturnCode identity_api_send_state(enum CanPrimaryLvacfsmStatus status);

/*!
 * \brief Periodically send information about the board state
 *
 * \param[in] status The current FSM status
 * \param[in] tick The current time tick (in ms)
 *
 * \retval IDENTITY_RC_OK if the state was sent successfully
 */
enum IdentityReturnCode identity_api_periodically_send_state(enum CanPrimaryLvacfsmStatus status, uint32_t tick);

/*!
 * \brief Periodically send the version of the board firmware and information related to it
 *
 * \param[in] tick The current time tick (in ms)
 *
 * \retval IDENTITY_RC_OK if the version was sent successfully
 */
enum IdentityReturnCode identity_api_periodically_send_version(uint32_t tick);

/*!
 * \brief Periodically send the version of the libcan dependency and information related to it
 *
 * \param[in] tick The current time tick (in ms)
 *
 * \retval IDENTITY_RC_OK if the libcan version was sent successfully
 */
enum IdentityReturnCode identity_api_periodically_send_libcan_version(uint32_t tick);

#endif // IDENTITY_API_H
