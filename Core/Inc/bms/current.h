/*!
 * \file            current.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Current module data structures.
 */

#ifndef CURRENT_H
#define CURRENT_H

#include "types.h"

#define CURRENT_MAX_A (45.F)        /*!< Maximum allowed current value in A */
#define CURRENT_MAX_POWER_KW (80.F) /*!< Maximum allowed power in kW */

/*!
 * \brief            Return codes for the voltage module functions.
 */
enum CurrentReturnCode {
    CURRENT_RC_OK, /*!< Function executed successfully */
};

/*!
 * \brief            Current module handler structure.
 */
struct CurrentHandler {
    ampere current; /*!< The current value in A */
};

#endif /*! CURRENT_H */
