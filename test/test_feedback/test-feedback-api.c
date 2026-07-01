/*!
 * \file            feedback-api.h
 * \date            2026-06-30
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Feedback management APIs.
 */

#include "unity.h"

#include <string.h>

#include "feedback.h"
#include "feedback-api.h"

extern struct FeedbackHandler feedback_handler;

void setUp(void) {
    feedback_api_init();
}

void tearDown(void) {
}

/*!
 * \defgroup		feedback_api_init Test for feedback_api_init function.
 * \{
 */

void check_feedback_api_init(void) {
    volt analog[FEEDBACK_COUNT] = { 0.F };
    memset(feedback_handler.analog, 1.9F, FEEDBACK_COUNT * sizeof(*feedback_handler.analog));
    feedback_api_init();
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(analog, feedback_handler.analog, FEEDBACK_COUNT);
}

/*! \} */

/*!
 * \defgroup		feedback_api_get_analog Test for feedback_api_get_analog function.
 * \{
 */

void check_feedback_api_get_analog_with_valid_parameter(void) {
    volt analog = 1.9F;
    feedback_handler.analog[FEEDBACK_CHRG_VIN_VALID] = analog;
    TEST_ASSERT_EQUAL_FLOAT(analog, feedback_api_get_analog(FEEDBACK_CHRG_VIN_VALID));
}

void check_feedback_api_get_analog_with_wrong_parameter(void) {
    memset(feedback_handler.analog, 1.9F, FEEDBACK_COUNT * sizeof(*feedback_handler.analog));
    TEST_ASSERT_EQUAL_FLOAT(0.F, feedback_api_get_analog(FEEDBACK_COUNT));
}

/*! \} */

/*!
 * \defgroup		feedback_api_get_status Test for feedback_api_get_status function.
 * \{
 */

void check_feedback_api_get_status_with_valid_parameter(void) {
    feedback_handler.analog[FEEDBACK_CHRG_VIN_VALID] = 1.9F;
    TEST_ASSERT_EQUAL_FLOAT(FEEDBACK_STATUS_HIGH, feedback_api_get_status(FEEDBACK_CHRG_VIN_VALID));
}

void check_feedback_api_get_status_with_wrong_parameter(void) {
    memset(feedback_handler.analog, 0.7F, FEEDBACK_COUNT * sizeof(*feedback_handler.analog));
    TEST_ASSERT_EQUAL_FLOAT(FEEDBACK_STATUS_ERROR, feedback_api_get_status(FEEDBACK_COUNT));
}

void check_feedback_api_get_status_with_analog_value_too_high(void) {
    feedback_handler.analog[FEEDBACK_CHRG_VIN_VALID] = 3.8F;
    TEST_ASSERT_EQUAL_FLOAT(FEEDBACK_STATUS_ERROR, feedback_api_get_status(FEEDBACK_CHRG_VIN_VALID));
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*!
     * \defgroup		feedback_api_init Test for feedback_api_init function.
     * \{
     */

    RUN_TEST(check_feedback_api_init);

    /*! \} */

    /*!
     * \defgroup		feedback_api_get_analog Test for feedback_api_get_analog function.
     * \{
     */

    RUN_TEST(check_feedback_api_get_analog_with_valid_parameter);
    RUN_TEST(check_feedback_api_get_analog_with_wrong_parameter);

    /*! \} */

    /*!
     * \defgroup		feedback_api_get_status Test for feedback_api_get_status function.
     * \{
     */

    RUN_TEST(check_feedback_api_get_status_with_valid_parameter);
    RUN_TEST(check_feedback_api_get_status_with_wrong_parameter);
    RUN_TEST(check_feedback_api_get_status_with_analog_value_too_high);

    /*! \} */

    UNITY_END();
}
