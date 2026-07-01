/*!
 * \file            feedback-api.h
 * \date            2026-06-30
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Feedback management APIs.
 */

#ifndef FEEDBACK_API_H
#define FEEDBACK_API_H

#include "eagletrt-api.h"

#include "config.h"
#include "feedback.h"

#ifdef CONFIG_FEEDBACK_MODULE_ENABLE

/*!
 * \brief           Initialize the feedback module.
 * \retval          FEEDBACK_RC_OK on success.
 */
enum FeedbackReturnCode feedback_api_init(void);

/*!
 * \brief           Get the value of an analog feedback in V.
 * \param[in]       feedback The desired feedback.
 * \returns         volt The feedback analog value in V.
 */
volt feedback_api_get_analog(enum Feedback feedback);

/*!
 * \brief           Get the status of a single feedback.
 * \param[in]       feedback The desired feedback.
 * \retval          FEEDBACK_STATUS_LOW if has low logical value.
 * \retval          FEEDBACK_STATUS_ERROR if it isn't in a valid state.
 * \retval          FEEDBACK_STATUS_HIGH if has a high logical value.
 */
enum FeedbackStatus feedback_api_get_status(enum Feedback feedback);

#else /*! CONFIG_FEEDBACK_MODULE_ENABLE */

#define feedback_api_init() (FEEDBACK_RC_OK)
#define feedback_api_get_analog(feedback) (0.F)
#define feedback_api_get_status(feedback) (FEEDBACK_STATUS_ERROR)

#endif /*! CONFIG_FEEDBACK_MODULE_ENABLE */

#endif /*! FEEDBACK_API_H */
