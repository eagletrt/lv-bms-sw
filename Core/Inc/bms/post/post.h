/*!
 * \file post.h
 * \author Thomas Moretti [majorfrost173@gmail.com]
 * \date 2026-06-18
 * \brief Module dedicated to running a power-on self-test (POST)
 */

#ifndef POST_H
#define POST_H

#include "can-communication.h"
#include "bms-monitor.h"
#include "fsm.h"

/*!
 * \brief Return codes for the POST module APIs
 */
enum PostReturnCode {
    POST_RC_OK = 0,               /*!< POST executed succesfully */
    POST_RC_UNINITIALIZED_MODULE, /*!< A module failed to be initalized during POST */
    POST_RC_ERROR                 /*!< POST encountered an error */
};

/*!
 * \brief Structure containing all the information needed at initialization time
 */
struct PostInitData {
    struct CanCommunicationNetworkConfig can_network_configurations[CAN_COMMUNICATION_NETWORK_COUNT];
    bms_monitor_send_callback bms_monitor_send;
    bms_monitor_send_receive_callback bms_monitor_send_receive;
    bms_monitor_ntc_read_callback bms_monitor_ntc_read;
    fsm_get_tick_callback get_tick;
};

#endif // POST_H
