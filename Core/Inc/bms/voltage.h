/*!
 * \file            voltage.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Voltage module data structures.
 */

#ifndef VOLTAGE_H
#define VOLTAGE_H

#include <stddef.h>
#include <stdbool.h>

#include "defines.h"
#include "types.h"

#define VOLTAGE_MIN_V (2.5f) /*!< Minimum allowed cell voltage in V */
#define VOLTAGE_MAX_V (4.2f) /*!< Maximum allowed cell voltage in V */

/*!
 * \brief            Return codes for the voltage module functions.
 */
enum VoltageReturnCode {
    VOLTAGE_RC_OK,            /*!< Function executed successfully */
    VOLTAGE_RC_NULL_POINTER,  /*!< Unexpected NULL pointer */
    VOLTAGE_RC_OUT_OF_BOUNDS, /*!< A value is greater/lower than the maximum/minimum allowed voltage */
};

/*!
 * \brief            Voltage module handler structure.
 */
struct VoltageHandler {
    volt voltages[DEFINES_SERIES_COUNT]; /*!< An array of voltages in V */
};

#endif /*! VOLTAGE_H */
