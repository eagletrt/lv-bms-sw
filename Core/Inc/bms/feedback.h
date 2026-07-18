/*!
 * \file            feedback.h
 * \date            2026-06-30
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Feedback management module.
 */

#ifndef FEEDBACK_H
#define FEEDBACK_H

#include "types.h"

#define FEEDBACK_CYCLE_TIME_MS (1U) /*!< The period with which feedbacks are updated */

/*!
 * \defgroup        feedback_threshold Thresholds for the analog feedbacks in V.
 *
 * \details         If the voltage of a feedback is greater than the HIGH threshold it is considered as logically high,
 *                  if it is lower than the LOW threshold is considered low otherwise it is in an implausible state and
 *                  considered as error.
 *
 * \{
 */

#define FEEDBACK_THRESHOLD_HIGH_V (1.9F) /*!< Feedback high threshold in V */
#define FEEDBACK_THRESHOLD_LOW_V (0.7F)  /*!< Feedback low threshold in V */

/*!
 * \}
 */

/*!
 * \brief            Return codes for the feedback module functions.
 */
enum FeedbackReturnCode {
    FEEDBACK_RC_OK,            /*!< Function executed successfully */
    FEEDBACK_RC_OUT_OF_BOUNDS, /*!< A value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief           Type definition of the feedback identifiers.
 *
 * \details         The charger IC provides two related signals that must be evaluated together:
 *                  - FEEDBACK_CHARGE_STATUS_NEGATED (STAT)
 *                  - FEEDBACK_CHARGE_VIN_VALID_NEGATED (ACOK)
 *
 *                  | IN      | Charging State                               | ACON | STAT            |
 *                  |---------|----------------------------------------------|------|-----------------|
 *                  | Absent  | NA                                           | Hi-Z | Hi-Z            |
 *                  | Present | In charging                                  | Low  | Low             |
 *                  | Present | Charging complete, charging disabled         | Low  | Hi-Z            |
 *                  | Present | NTC fault, safety timer expires, battery OVP | Low  | Blinking at 2Hz |
 */
enum Feedback {
    FEEDBACK_SUPPLY_ENABLE_NEGATED,    /*!< Indicates if the supply of the board is enabled */
    FEEDBACK_SUPPLY_DELAY,             /*!< The voltage of the RC circuit used during flashing (MCU) */
    FEEDBACK_CHARGE_STATUS_NEGATED,    /*!< Charger IC feedback; it has to be evaluated with FEEDBACK_CHARGE_VIN_VALID_NEGATED */
    FEEDBACK_CHARGE_VIN_VALID_NEGATED, /*!< Charger IC feedback; it has to be evaluated with FEEDBACK_CHARGE_STATUS_NEGATED */
    FEEDBACK_OUTPUT_ENABLE_NEGATED,    /*!< Indicates the relay's coil status */
    FEEDBACK_OUTPUT_DELAY,             /*!< The voltage of the RC circuit used during flashing (LV) */
    FEEDBACK_OUTPUT_FUSE,              /*!< Indicates if the fuse is blown */
    FEEDBACK_VOUT,                     /*!< Indicated the output voltage */
    FEEDBACK_COUNT,                    /*!< The number of feedbacks */
};

/*!
 * \brief           Status of the feedbacks.
 */
enum FeedbackStatus {
    FEEDBACK_STATUS_LOW,   /*!< Feedback status considered as low */
    FEEDBACK_STATUS_ERROR, /*!< Feedback status considered as an error */
    FEEDBACK_STATUS_HIGH   /*!< Feedback status considered as high */
};

/*!
 * \brief            Feedback module handler structure.
 */
struct FeedbackHandler {
    volt analog[FEEDBACK_COUNT]; /*!< An array of raw voltages of the feedbacks. */
};

#endif /*! FEEDBACK */
