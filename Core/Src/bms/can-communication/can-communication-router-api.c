/*!
 * \file can-communication-router-api.c
 * \date 2026-07-23
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \authors Thomas Moretti [majorfrost173@gmail.com]
 *
 * \brief Implementation of the hardware-agnostic CAN messages router.
 */

#include "can-communication-router-api.h"

enum CanCommunicationReturnCode can_communication_router_api_receive_primary(const struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    // TODO: add libcan deserialization and dispatch logic here

    return CAN_COMMUNICATION_RC_OK;
}
