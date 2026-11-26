/*!
 * \file            discharge.h
 * \date            2026-10-27
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Discharge module data structures.
 */

#ifndef DISCHARGE_H
#define DISCHARGE_H

#include <types.h>

enum DischargeReturnCode {
    DISCHARGE_RC_OK,
    DISCHARGE_RC_NULL_POINTER,
};

struct DischargeHandler {
    bool is_active;
    bool toggle_request;
    uint8_t current_cell;
};

#endif /*! DISCHARGE_H */
