/*!
 * \file identity.h
 * \author Thomas Moretti [majorfrost173@gmail.com]
 * \date 2026-08-07
 * \brief Type definitions for the module dedicated to providing information about the LV BMS firmware and its dependencies.
 */

#ifndef IDENTITY_H
#define IDENTITY_H

#include <stdint.h>

#define IDENTITY_VERSION_MAJOR (0U)
#define IDENTITY_VERSION_MINOR (1U)
#define IDENTITY_VERSION_PATCH (0U)

#define IDENTITY_VERSION_INFO_COMMIT_HASH (0x67)
#define IDENTITY_VERSION_INFO_DIRTY (0U)

/*!
 * \brief Return codes for the identity module functions
 */
enum IdentityReturnCode {
    IDENTITY_RC_OK,
    IDENTITY_RC_ERROR
};

/*!
 * \brief The handler of the module containing information about the firmware and various time ticks
 */
struct IdentityHandler {
    uint32_t firmware_build_time;
    uint32_t last_send_tick_status;
    uint32_t last_send_tick_version;
    uint32_t last_send_tick_libcan_version;
};

#endif // IDENTITY_H
