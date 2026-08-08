/*!
 * \file can-communication-router-api.h
 * \date 2026-07-23
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \authors Thomas Moretti [majorfrost173@gmail.com]
 *
 * \brief Public API for the hardware-agnostic CAN messages router.
 */

#ifndef CAN_COMMUNICATION_ROUTER_API_H
#define CAN_COMMUNICATION_ROUTER_API_H

#include "can-communication.h"

/*!
 * \brief Router function for incoming CAN frames on primary network.
 *
 * \param[in] frame The frame just popped off the RX queue.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_RECEIVE_HANDLER_ERROR if dispatch fails.
 */
enum CanCommunicationReturnCode can_communication_router_api_receive_primary(const struct CanCommunicationFrame *frame);

#endif // CAN_COMMUNICATION_ROUTER_API_H
