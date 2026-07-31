/*!
 * \file            test-current-api.c
 * \date            2026-04-29
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the current module.
 */

#include "unity.h"

#include <string.h>

#include "current.h"
#include "current-api.h"
#include "voltage.h"
#include "voltage-api.h"
#include "types.h"
#include "defines.h"

extern struct CurrentHandler current_handler;
extern struct VoltageHandler voltage_handler;

void setUp(void) {
    current_api_init();
    voltage_api_init();
}

void tearDown(void) {
}

/*!
 * \defgroup		current_api_init Test for current_api_init function.
 * \{
 */

void check_current_api_init(void) {
    ampere currents[DEFINES_NTC_COUNT] = { 0 };
    memset(current_handler.currents, 0xFF, DEFINES_NTC_COUNT * sizeof(*current_handler.currents));
    current_handler.output_current = 3.2F;
    current_api_init();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.F, current_handler.output_current, "output_current not zero after init!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(currents, current_handler.currents, DEFINES_NTC_COUNT, "currents not zero after init!");
}

/*! \} */

/*!
 * \defgroup		current_api_update_current Test for current_api_update_current function.
 * \{
 */

void check_current_api_update_current_with_valid_parameters(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(CURRENT_RC_OK, current_api_update_current(0U, 7.F), "current_api_update_current failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(7.F, current_handler.currents[0U], "Stored current is different!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(7.F, current_api_get_output_current(), "output current not recomputed from currents!");
}

void check_current_api_update_current_when_index_is_out_of_bounds(void) {
    ampere currents[DEFINES_NTC_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(CURRENT_RC_OUT_OF_BOUNDS, current_api_update_current(DEFINES_NTC_COUNT, 7.F), "current_api_update_current returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(currents, current_handler.currents, DEFINES_NTC_COUNT, "Previously stored values have been modified!");
}

/*! \} */

/*!
 * \defgroup		current_api_update_currents Test for current_api_update_currents function.
 * \{
 */

void check_current_api_update_currents_with_valid_parameters(void) {
    const ampere currents[DEFINES_CELLS_SERIES_COUNT] = { 1.F, 2.F, 3.F, 4.F, 5.F, 6.F };
    TEST_ASSERT_EQUAL_INT_MESSAGE(CURRENT_RC_OK, current_api_update_currents(0U, currents, DEFINES_CELLS_SERIES_COUNT), "current_api_update_currents failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(currents, current_handler.currents, DEFINES_CELLS_SERIES_COUNT, "Stored currents are different!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(21.F, current_api_get_output_current(), "output current not recomputed from currents!");
}

void check_current_api_update_currents_with_null_currents(void) {
    ampere currents[DEFINES_NTC_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(CURRENT_RC_NULL_POINTER, current_api_update_currents(0U, NULL, DEFINES_CELLS_SERIES_COUNT), "current_api_update_currents returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(currents, current_handler.currents, DEFINES_NTC_COUNT, "Previously stored values have been modified!");
}

void check_current_api_update_currents_when_index_is_out_of_bounds(void) {
    const ampere mock_currents[DEFINES_CELLS_SERIES_COUNT] = { 1.F, 2.F, 3.F, 4.F, 5.F, 6.F };
    ampere currents[DEFINES_NTC_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(CURRENT_RC_OUT_OF_BOUNDS, current_api_update_currents(DEFINES_CELLS_COUNT, mock_currents, DEFINES_CELLS_SERIES_COUNT), "current_api_update_currents returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(currents, current_handler.currents, DEFINES_NTC_COUNT, "Previously stored values have been modified!");
}

void check_current_api_update_currents_when_size_is_too_big(void) {
    const ampere mock_currents[1U] = { 1.F };
    ampere currents[DEFINES_NTC_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(CURRENT_RC_OUT_OF_BOUNDS, current_api_update_currents(DEFINES_CELLS_COUNT, mock_currents, 1U), "current_api_update_currents returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(currents, current_handler.currents, DEFINES_NTC_COUNT, "Previously stored values have been modified!");
}

/*! \} */

/*!
 * \defgroup		current_api_dump_currents Test for current_api_dump_currents function.
 * \{
 */

void check_current_api_dump_currents_with_valid_parameters(void) {
#define COUNT (3U)
    ampere dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(CURRENT_RC_OK, current_api_dump_currents(dump, 2U, COUNT), "current_api_dump_currents failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(current_handler.currents + 2U, dump, COUNT, "currents are different!");
#undef COUNT
}

void check_current_api_dump_currents_with_null_out(void) {
    TEST_ASSERT_EQUAL_INT(CURRENT_RC_NULL_POINTER, current_api_dump_currents(NULL, 0U, 0U));
}

void check_current_api_dump_currents_when_start_is_out_of_bounds(void) {
#define COUNT (3U)
    ampere dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(CURRENT_RC_OUT_OF_BOUNDS, current_api_dump_currents(dump, DEFINES_CELLS_COUNT, COUNT));
#undef COUNT
}

void check_current_api_dump_currents_when_size_is_too_big(void) {
#define COUNT (3U)
    ampere dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(CURRENT_RC_OUT_OF_BOUNDS, current_api_dump_currents(dump, DEFINES_CELLS_COUNT, COUNT));
#undef COUNT
}

/*! \} */

/*!
 * \defgroup		current_api_get_output_current Test for current_api_get_output_current function.
 * \{
 */

void check_current_api_get_output_current(void) {
    current_api_update_current(0U, 2.F);
    current_api_update_current(1U, 3.F);
    TEST_ASSERT_EQUAL_FLOAT(5.F, current_api_get_output_current());
}

/*! \} */

/*!
 * \defgroup		current_api_get_power Test for current_api_get_power function.
 * \{
 */

void check_current_api_get_power(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    memcpy(voltage_handler.voltages, voltages, DEFINES_CELLS_SERIES_COUNT * sizeof(*voltages));
    current_handler.output_current = 3.2F;
    TEST_ASSERT_EQUAL_FLOAT(20.7F * 3.2F, current_api_get_power());
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*!
	 * \defgroup	 current_api_init Test for current_api_init function.
	 * \{
	 */

    RUN_TEST(check_current_api_init);

    /*! \} */

    /*!
	 * \defgroup	 current_api_update_current Test for current_api_update_current function.
	 * \{
	 */

    RUN_TEST(check_current_api_update_current_with_valid_parameters);
    RUN_TEST(check_current_api_update_current_when_index_is_out_of_bounds);

    /*! \} */

    /*!
	 * \defgroup	 current_api_update_currents Test for current_api_update_currents function.
	 * \{
	 */

    RUN_TEST(check_current_api_update_currents_with_valid_parameters);
    RUN_TEST(check_current_api_update_currents_with_null_currents);
    RUN_TEST(check_current_api_update_currents_when_index_is_out_of_bounds);
    RUN_TEST(check_current_api_update_currents_when_size_is_too_big);

    /*! \} */

    /*!
	 * \defgroup	 current_api_dump_currents Test for current_api_dump_currents function.
	 * \{
	 */

    RUN_TEST(check_current_api_dump_currents_with_valid_parameters);
    RUN_TEST(check_current_api_dump_currents_with_null_out);
    RUN_TEST(check_current_api_dump_currents_when_start_is_out_of_bounds);
    RUN_TEST(check_current_api_dump_currents_when_size_is_too_big);

    /*! \} */

    /*!
	 * \defgroup	 current_api_get_output_current Test for current_api_get_output_current function.
	 * \{
	 */

    RUN_TEST(check_current_api_get_output_current);

    /*! \} */

    /*!
	 * \defgroup	 current_api_get_power Test for current_api_get_power function.
	 * \{
	 */

    RUN_TEST(check_current_api_get_power);

    /*! \} */

    UNITY_END();
}
