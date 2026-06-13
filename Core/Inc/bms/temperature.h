/*!
 * \file            temperature.h
 * \date            2026-03-24
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature module data structures.
 */

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <stddef.h>
#include <stdbool.h>

#include "defines.h"
#include "types.h"

#define TEMPERATURE_CHARGE_MIN_C (0.F)                       /*!< Minimum allowed cell temperature in °C in charge */
#define TEMPERATURE_CHARGE_MAX_C (60.F)                      /*!< Maximum allowed cell temperature in °C in charge */
#define TEMPERATURE_DISCHARGE_MIN_C (-40.F)                  /*!< Minimum allowed cell temperature in °C in discharge */
#define TEMPERATURE_DISCHARGE_MAX_C TEMPERATURE_CHARGE_MAX_C /*!< Maximum allowed cell temperature in °C in discharge */

/*!
 * \brief            Return codes for the temperature module functions.
 */
enum TemperatureReturnCode {
    TEMPERATURE_RC_OK,            /*!< Function executed successfully */
    TEMPERATURE_RC_NULL_POINTER,  /*!< Unexpected NULL pointer */
    TEMPERATURE_RC_OUT_OF_BOUNDS, /*!< A value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief            Temperature module handler structure.
 */
struct TemperatureHandler {
    celsius temperatures[DEFINES_CELLS_SERIES_COUNT]; /*!< An array of temperatures in °C */
};

#endif /*! TEMPERATURE_H */
