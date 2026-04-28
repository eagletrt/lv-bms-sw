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

#include "defs.h"
#include "types.h"

#define VOLTAGE_MIN_V (2.5f) /*!< Minmum allowed cell voltage in V */
#define VOLTAGE_MAX_V (4.2f) /*!< Maxmum allowed cell voltage in V */

/*!
 * \brief            Return codes for the voltage module functions.
 */
enum VoltageReturnCode {
    VOLTAGE_RC_OK,            /*!< Function executed successfully */
    VOLTAGE_RC_NULL_POINTER,  /*!< Unexpected NULL pointer */
    VOLTAGE_RC_OUT_OF_BOUNDS, /*!< A value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief            Voltage module handler structure.
 */
struct VoltageHandler {
    volt voltages[DEFS_CELLS_COUNT]; /*!< An array of voltages in V */
};

#endif /*! VOLTAGE_H */
