/*!
 * \file            current.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Current module data structures.
 */

#ifndef CURRENT_H
#define CURRENT_H

#include "defines.h"
#include "types.h"

#define CURRENT_MAX_A (45.F)        /*!< Maximum allowed current value in A */
#define CURRENT_MAX_POWER_KW (80.F) /*!< Maximum allowed power in kW */

/*!
 * \brief            Return codes for the voltage module functions.
 */
enum CurrentReturnCode {
    CURRENT_RC_OK,            /*!< Function executed successfully */
    CURRENT_RC_NULL_POINTER,  /*!< Unexpected NULL pointer */
    CURRENT_RC_OUT_OF_BOUNDS, /*!< A value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief            Current module handler structure.
 */
struct CurrentHandler {
    ampere output_current;              /*!< The output current value in A */
    ampere currents[DEFINES_NTC_COUNT]; /*!< An array of NTC currents in A  */
};

#endif /*! CURRENT_H */
