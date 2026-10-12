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
    ampere cell_out_current; /*!< The output current from cells in A */
};

#endif /*! CURRENT_H */
