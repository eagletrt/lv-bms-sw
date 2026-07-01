/*!
 * \file            feedback-api.c
 * \date            2026-06-30
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Feedback management APIs.
 */

#include "eagletrt-api.h"

#include "config.h"
#include "eagletrt.h"
#include "feedback.h"
#include "feedback-api.h"

#ifdef CONFIG_FEEDBACK_MODULE_ENABLE

EAGLETRT_STATIC struct FeedbackHandler feedback_handler; /*!< */

/*!
 * \brief           Check if an analog value is in a valid feedback status.
 * \param[in]       analog The analog value to check in V.
 * \retval          true if valid.
 * \retval          false otherwise.
 */
EAGLETRT_STATIC_INLINE bool prv_feedback_is_valid_analog(volt analog) {
    return (analog >= FEEDBACK_THRESHOLD_LOW_V || analog <= FEEDBACK_THRESHOLD_HIGH_V) && analog < FEEDBACK_VREF;
}

enum FeedbackReturnCode feedback_api_init(void) {
    feedback_handler = (struct FeedbackHandler){ 0 };
    return FEEDBACK_RC_OK;
}

volt feedback_api_get_analog(enum Feedback feedback) {
    if (feedback >= FEEDBACK_COUNT) {
        return 0.F;
    }

    return feedback_handler.analog[feedback];
}

enum FeedbackStatus feedback_api_get_status(enum Feedback feedback) {
    if (feedback >= FEEDBACK_COUNT || !prv_feedback_is_valid_analog(feedback_handler.analog[feedback])) {
        return FEEDBACK_STATUS_ERROR;
    }

    return (feedback_handler.analog[feedback] <= FEEDBACK_THRESHOLD_LOW_V) ? FEEDBACK_STATUS_LOW : FEEDBACK_STATUS_HIGH;
}

#endif /*! CONFIG_FEEDBACK_MODULE_ENABLE */
