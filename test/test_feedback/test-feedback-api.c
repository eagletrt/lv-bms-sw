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

/*! Get private instances and functions */
extern struct FeedbackHandler feedback_handler;
extern bool prv_feedback_is_valid_analog(volt analog);

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

    for (size_t i = 0U; i < FEEDBACK_COUNT; ++i) {
        feedback_handler.analog[i] = 1.9F;
    }

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
    feedback_handler.analog[FEEDBACK_CHARGE_VIN_VALID_NEGATED] = analog;
    TEST_ASSERT_EQUAL_FLOAT(analog, feedback_api_get_analog(FEEDBACK_CHARGE_VIN_VALID_NEGATED));
}

void check_feedback_api_get_analog_with_wrong_parameter(void) {
    for (size_t i = 0U; i < FEEDBACK_COUNT; ++i) {
        feedback_handler.analog[i] = 1.9F;
    }

    TEST_ASSERT_EQUAL_FLOAT(0.F, feedback_api_get_analog(FEEDBACK_COUNT));
}

/*! \} */

/*!
 * \defgroup		feedback_api_get_status Test for feedback_api_get_status function.
 * \{
 */

void check_feedback_api_get_status_with_valid_parameter(void) {
    feedback_handler.analog[FEEDBACK_CHARGE_VIN_VALID_NEGATED] = 1.9F;
    TEST_ASSERT_EQUAL_INT(FEEDBACK_STATUS_HIGH, feedback_api_get_status(FEEDBACK_CHARGE_VIN_VALID_NEGATED));
}

void check_feedback_api_get_status_with_wrong_parameter(void) {
    for (size_t i = 0U; i < FEEDBACK_COUNT; ++i) {
        feedback_handler.analog[i] = 0.7F;
    }

    TEST_ASSERT_EQUAL_INT(FEEDBACK_STATUS_ERROR, feedback_api_get_status(FEEDBACK_COUNT));
}

/*! \} */

/*!
 * \defgroup		prv_feedback_is_valid_analog Test for prv_feedback_is_valid_analog function.
 * \{
 */

void check_feedback_api_prv_feedback_is_valid_analog(void) {
    TEST_ASSERT_TRUE(prv_feedback_is_valid_analog(1.9F));
}

void check_feedback_api_prv_feedback_is_valid_analog_with_invalid_value(void) {
    TEST_ASSERT_FALSE(prv_feedback_is_valid_analog(1.1F));
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

    RUN_TEST(check_feedback_api_prv_feedback_is_valid_analog);
    RUN_TEST(check_feedback_api_prv_feedback_is_valid_analog_with_invalid_value);

    /*! \} */

    /*!
     * \defgroup		feedback_api_get_status Test for feedback_api_get_status function.
     * \{
     */

    RUN_TEST(check_feedback_api_get_status_with_valid_parameter);
    RUN_TEST(check_feedback_api_get_status_with_wrong_parameter);

    /*! \} */

    /*!
     * \defgroup		feedback_api_get_analog Test for feedback_api_get_analog function.
     * \{
     */

    RUN_TEST(check_feedback_api_get_analog_with_valid_parameter);
    RUN_TEST(check_feedback_api_get_analog_with_wrong_parameter);

    /*! \} */

    UNITY_END();
}
