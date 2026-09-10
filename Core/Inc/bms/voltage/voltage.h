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

#define VOLTAGE_MIN_V (2.5F) /*!< Minimum allowed cell voltage in V */
#define VOLTAGE_MAX_V (4.2F) /*!< Maximum allowed cell voltage in V */

/*!
 * \brief            Return codes for the voltage module functions.
 */
enum VoltageReturnCode {
    VOLTAGE_RC_OK,                 /*!< Function executed successfully */
    VOLTAGE_RC_NULL_POINTER,       /*!< Unexpected NULL pointer */
    VOLTAGE_RC_OUT_OF_BOUNDS,      /*!< A value is greater/lower than the maximum/minimum allowed voltage */
    VOLTAGE_RC_COMMUNICATION_ERROR /*!< Failed to queue a CAN frame */
};

/*!
 * \brief            Voltage module handler structure.
 */
struct VoltageHandler {
    volt voltages[DEFINES_CELLS_SERIES_COUNT]; /*!< An array of voltages in V */
    uint32_t last_tick_cell_voltage_ms;        /*!< Last tick for LvacCellVoltage */
    uint32_t last_tick_voltage_information_ms; /*!< Last tick for LvacVoltageInfo */
};

#endif /*! VOLTAGE_H */
