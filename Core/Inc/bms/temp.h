/*!
 * \file            temp.h
 * \date            2026-03-24
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature module data structures.
 */

#ifndef TEMP_H
#define TEMP_H

#include <stddef.h>
#include <stdbool.h>

#include "defs.h"
#include "types.h"

#define TEMP_CHARGE_MIN_C (0.f)                /*!< Minimum allowed cell temperature in °C in charge */
#define TEMP_CHARGE_MAX_C (60.f)               /*!< Maximum allowed cell temperature in °C in charge */
#define TEMP_DISCHARGE_MIN_C (-40.f)           /*!< Minimum allowed cell temperature in °C in discharge */
#define TEMP_DISCHARGE_MAX_C TEMP_CHARGE_MAX_C /*!< Maximum allowed cell temperature in °C in discharge */

/*!
 * \brief            Return codes for the temperature module functions.
 */
enum TempReturnCode {
    TEMP_RC_OK,            /*!< Function executed successfully */
    TEMP_RC_NULL_POINTER,  /*!< Unexpected NULL pointer */
    TEMP_RC_OUT_OF_BOUNDS, /*!< A value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief            Temperature module handler structure.
 */
struct TempHandler {
    celsius temperatures[DEFS_CELLS_COUNT]; /*!< An array of temperatures in °C */
};

#endif /*! TEMP_H */
