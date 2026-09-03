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
 * \brief            Health of a single NTC channel.
 *
 * \details          Every cell NTC is the bottom leg of a divider whose top leg
 *                  is the pull-up on the multiplexer common node. That makes the
 *                  two failure modes trivial to tell apart from a real reading:
 *                  a disconnected NTC leaves the node sitting at the pull-up
 *                  rail, a shorted one pins it to ground. Both land far outside
 *                  the range the volt-to-celsius fit covers, so they have to be
 *                  reported rather than converted.
 */
enum TemperatureStatus {
    TEMPERATURE_STATUS_OK = 0,  /*!< The channel is reading a plausible NTC voltage. */
    TEMPERATURE_STATUS_OPEN,    /*!< The channel sits at the pull-up rail: NTC missing or wire broken. */
    TEMPERATURE_STATUS_SHORTED, /*!< The channel is pinned to ground: NTC or harness shorted. */
};

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
    celsius temperatures[DEFINES_CELLS_NTC_COUNT];            /*!< An array of temperatures in °C */
    enum TemperatureStatus statuses[DEFINES_CELLS_NTC_COUNT]; /*!< Health of each NTC channel */
};

#endif /*! TEMPERATURE_H */
