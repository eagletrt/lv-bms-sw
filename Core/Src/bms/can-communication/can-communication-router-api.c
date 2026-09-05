/*!
 * \file can-communication-router-api.c
 * \date 2026-07-23
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \authors Thomas Moretti [majorfrost173@gmail.com]
 *
 * \brief Implementation of the hardware-agnostic CAN messages router.
 */

#include "can-communication-router-api.h"
#include "can-primary-api.h"

EAGLETRT_STATIC void prv_dispatch_rx(uint32_t frame_id, union CanPrimaryMessages message) {
    EAGLETRT_API_UNUSED(message);
    switch (frame_id) {
        default:
            break;
    }
}

enum CanCommunicationReturnCode can_communication_router_api_receive_primary(const struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    if (!can_primary_api_id_is_valid(frame->id)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    union CanPrimaryMessages message = { 0 };
    if (can_primary_api_deserialize_from_id(frame->id, (uint8_t *)frame->data, &message) != 0) {
        return CAN_COMMUNICATION_RC_ERROR;
    }

    prv_dispatch_rx(frame->id, message);

    return CAN_COMMUNICATION_RC_OK;
}
