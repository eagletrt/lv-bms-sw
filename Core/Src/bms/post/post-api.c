/*!
 * \file post-api.c
 * \author Thomas Moretti [majorfrost173@gmail.com]
 * \date 2026-08-08
 * \brief Module dedicated to running a power-on self-test (POST)
 */

#include "post-api.h"
#include "can-communication-api.h"
#include "current-api.h"
#include "voltage-api.h"
#include "feedback-api.h"
#include "temperature-api.h"

enum PostReturnCode post_api_run(struct PostInitData *post_init_data) {
    if (post_init_data == nullptr ||
        post_init_data->can_network_configurations[CAN_COMMUNICATION_NETWORK_PRIMARY].on_receive == nullptr ||
        post_init_data->can_network_configurations[CAN_COMMUNICATION_NETWORK_PRIMARY].send == nullptr) {
        return POST_RC_ERROR;
    }

    enum PostReturnCode post_return_code = POST_RC_OK;

    if (can_communication_api_init(post_init_data->can_network_configurations) != CAN_COMMUNICATION_RC_OK) {
        post_return_code = POST_RC_UNINITIALIZED_MODULE;
    }

    if (current_api_init() != CURRENT_RC_OK) {
        post_return_code = POST_RC_UNINITIALIZED_MODULE;
    }

    if (voltage_api_init() != VOLTAGE_RC_OK) {
        post_return_code = POST_RC_UNINITIALIZED_MODULE;
    }

    if (feedback_api_init() != FEEDBACK_RC_OK) {
        post_return_code = POST_RC_UNINITIALIZED_MODULE;
    }

    if (temperature_api_init() != TEMPERATURE_RC_OK) {
        post_return_code = POST_RC_UNINITIALIZED_MODULE;
    }

    return post_return_code;
}
