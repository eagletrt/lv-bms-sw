/*!
 * \file post.h
 * \author Thomas Moretti [majorfrost173@gmail.com]
 * \date 2026-06-18
 * \brief Module dedicated to running a power-on self-test (POST)
 */

#ifndef POST_H
#define POST_H

#include "can-communication.h"

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
};

#endif // POST_H
