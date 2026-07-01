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

#define FEEDBACK_VREF (3.3F) /*!< Voltage reference */

/*!
 * \defgroup        feedback_threshold Thresholds for the analog feedbacks in V.
 *
 * \details         If the voltage of a feedback is greater than the HIGH threshold it is considered as logically high,
 *                  if it is lower than the LOW threshold is considered low otherwise it is in an implausible state and
 *                  considered as error.
 *
 * \{
 */

#define FEEDBACK_THRESHOLD_HIGH_V (1.9f) /*!< Feedback high threshold in V */
#define FEEDBACK_THRESHOLD_LOW_V (0.7f)  /*!< Feedback low threshold in V */

/*!
 * \}
 */

/*!
 * \brief            Return codes for the feedback module functions.
 */
enum FeedbackReturnCode {
    FEEDBACK_RC_OK, /*!< Function executed successfully */
};

/*!
 * \brief           Type definition of the feedback identifiers.
 */
enum Feedback {
    FEEDBACK_SUPPLY_EN,
    FEEDBACK_SUPPLY_DELAY,
    FEEDBACK_CHRG_STATUS,
    FEEDBACK_CHRG_VIN_VALID,
    FEEDBACK_OUT_EN,
    FEEDBACK_OUTPUT_DELAY,
    FEEDBACK_OUT_FUSE,
    FEEDBACK_VOUT,
    FEEDBACK_COUNT, /*!< The number of feedbacks */
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
