/*!
 * \file            balancing.h
 * \date            2026-09-08
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Balancing module data structures.
 */

#ifndef BALANCING_H
#define BALANCING_H

#include <stdbool.h>
#include <stdint.h>

#include "types.h"

#define BALANCING_THRESHOLD_V (0.05F)      /*!< Imbalance tolerated above the minimum cell voltage in V */
#define BALANCING_RUN_PERIOD_MS (1000U)    /*!< Discharge mask refresh period in ms, well under the LTC 30 s discharge timeout */

/*!
 * \brief            Return codes for the balancing module functions.
 */
enum BalancingReturnCode {
    BALANCING_RC_OK,            /*!< Function executed successfully */
    BALANCING_RC_NULL_POINTER,  /*!< Unexpected NULL pointer */
    BALANCING_RC_OUT_OF_BOUNDS  /*!< A value is outside the allowed cell voltage range */
};

/*!
 * \brief            Balancing module handler structure.
 */
struct BalancingHandler {
    volt target;       /*!< The voltage each discharged cell should reach in V, captured as the minimum cell voltage at start */
    volt threshold;    /*!< The imbalance tolerated above target in V */
    uint32_t last_run; /*!< Tick of the last executed run, for cadence gating */
    bool is_active;    /*!< Indicates if balancing is active (true) or not (false) */
    bool odd_phase;    /*!< Discharge odd-indexed cells this cycle, even-indexed the next */
};

#endif /*! BALANCING_H */
