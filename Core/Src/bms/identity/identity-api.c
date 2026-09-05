/*!
 * \file identity-api.c
 * \author Thomas Moretti [majorfrost173@gmail.com]
 * \date 2026-08-07
 * \brief Implementation of the APIs of the module dedicated to providing information about the LV BMS firmware and its dependencies.
 */

#include "identity-api.h"

#include <string.h>
#include <time.h>
#include "eagletrt.h"
#include "can-communication-api.h"
#include "can-primary-api.h"

/*!
 * \brief Internal module handler
 * \details Hidden from external linkage to enforce API-only access
 */
EAGLETRT_STATIC struct IdentityHandler identity_handler;

enum IdentityReturnCode identity_api_init(void) {
    memset(&identity_handler, 0, sizeof(identity_handler));

    struct tm timeinfo;
    strptime(__DATE__ " " __TIME__, "%b %d %Y %H:%M:%S", &timeinfo);
    identity_handler = (struct IdentityHandler){
        .firmware_build_time = mktime(&timeinfo),
        .last_send_tick_status = 0,
        .last_send_tick_version = 0,
        .last_send_tick_libcan_version = 0
    };

    return IDENTITY_RC_OK;
}

enum IdentityReturnCode identity_api_send_state(enum CanPrimaryLvacfsmStatus status) {
    union CanPrimaryMessages message = { .lvacfsm = { .status = status } };
    struct CanCommunicationFrame frame = { .id = CAN_PRIMARY_MESSAGE_FRAME_ID_LVACFSM };
    if (can_primary_api_serialize_from_id(frame.id, &message, frame.data) != -1) {
        frame.length = can_primary_byte_size_lvacfsm;
        EAGLETRT_API_UNUSED(can_communication_api_add_to_tx_buffer(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
    }
    return IDENTITY_RC_OK;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
enum IdentityReturnCode identity_api_periodically_send_state(enum CanPrimaryLvacfsmStatus status, uint32_t tick) {
    if (tick - identity_handler.last_send_tick_status >= can_primary_cycle_time_lvacfsm) {
        identity_handler.last_send_tick_status = tick;

        identity_api_send_state(status);
    }
    return IDENTITY_RC_OK;
}

enum IdentityReturnCode identity_api_periodically_send_version(uint32_t tick) {
    if (tick - identity_handler.last_send_tick_version >= can_primary_cycle_time_lvacversion) {
        identity_handler.last_send_tick_version = tick;

        union CanPrimaryMessages message;
        message.lvacversion = (struct CanPrimaryLvacversion){
            .major = IDENTITY_VERSION_MAJOR,
            .minor = IDENTITY_VERSION_MINOR,
            .patch = IDENTITY_VERSION_PATCH
        };

        struct CanCommunicationFrame frame;
        frame = (struct CanCommunicationFrame){
            .id = CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVERSION
        };

        if (can_primary_api_serialize_from_id(frame.id, &message, frame.data) != -1) {
            frame.length = can_primary_byte_size_lvacversion;
            EAGLETRT_API_UNUSED(can_communication_api_add_to_tx_buffer(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
        }

        message.lvacversioninfo = (struct CanPrimaryLvacversioninfo){
            .buildtime = identity_handler.firmware_build_time,
            .commithash = IDENTITY_VERSION_INFO_COMMIT_HASH,
            .dirty = IDENTITY_VERSION_INFO_DIRTY
        };
        frame = (struct CanCommunicationFrame){
            .id = CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVERSIONINFO
        };

        if (can_primary_api_serialize_from_id(frame.id, &message, frame.data) != -1) {
            frame.length = can_primary_byte_size_lvacversioninfo;
            EAGLETRT_API_UNUSED(can_communication_api_add_to_tx_buffer(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
        }
    }
    return IDENTITY_RC_OK;
}

enum IdentityReturnCode identity_api_periodically_send_libcan_version(uint32_t tick) {
    if (tick - identity_handler.last_send_tick_libcan_version >= can_primary_cycle_time_lvaclibcanversion) {
        identity_handler.last_send_tick_libcan_version = tick;

        union CanPrimaryMessages message;
        message.lvaclibcanversion = (struct CanPrimaryLvaclibcanversion){
            .major = can_version_major,
            .minor = can_version_minor,
            .patch = can_version_patch
        };

        struct CanCommunicationFrame frame;
        frame = (struct CanCommunicationFrame){
            .id = CAN_PRIMARY_MESSAGE_FRAME_ID_LVACLIBCANVERSION
        };

        if (can_primary_api_serialize_from_id(frame.id, &message, frame.data) != -1) {
            frame.length = can_primary_byte_size_lvaclibcanversion;
            EAGLETRT_API_UNUSED(can_communication_api_add_to_tx_buffer(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
        }

        message.lvaclibcanversioninfo = (struct CanPrimaryLvaclibcanversioninfo){
            .generationtime = can_generation_time,
            .commithash = 0,
            .dirty = 0
        };
        frame = (struct CanCommunicationFrame){
            .id = CAN_PRIMARY_MESSAGE_FRAME_ID_LVACLIBCANVERSIONINFO
        };

        if (can_primary_api_serialize_from_id(frame.id, &message, frame.data) != -1) {
            frame.length = can_primary_byte_size_lvaclibcanversioninfo;
            EAGLETRT_API_UNUSED(can_communication_api_add_to_tx_buffer(CAN_COMMUNICATION_NETWORK_PRIMARY, &frame));
        }
    }
    return IDENTITY_RC_OK;
}
