/*!
 * \file            test-voltage-api.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the voltage module.
 */

#include "unity.h"

#include <string.h>

#include "voltage.h"
#include "voltage-api.h"
#include "types.h"
#include "defines.h"

extern struct VoltageHandler voltage_handler;

void setUp(void) {
    voltage_api_init();
}

void tearDown() {
}

/*!
 * \defgroup		voltage_api_init Test for voltage_api_init function.
 * \{
 */

void check_voltage_api_init(void) {
    volt voltages[DEFINES_SERIES_COUNT] = { 0 };
    memset(voltage_handler.voltages, 0xFF, DEFINES_SERIES_COUNT * sizeof(*voltage_handler.voltages));
    voltage_api_init();
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(voltages, voltage_handler.voltages, DEFINES_SERIES_COUNT);
}

/*! \} */

/*!
 * \defgroup        voltage_api_update_voltage Test for voltage_api_update_voltage function.
 * \{
 */

void check_voltage_api_update_voltage_with_valid_parameters(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OK, voltage_api_update_voltage(0U, 7.f), "voltage_api_update_voltage failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(7.f, voltage_handler.voltages[0U], "Stored voltage is different!");
}

void check_voltage_api_update_voltage_when_index_is_out_of_bounds(void) {
    volt voltages[DEFINES_SERIES_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_update_voltage(DEFINES_SERIES_COUNT, 7.f), "voltage_api_update_voltage returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_SERIES_COUNT, "Previously stored values have been modified!");
}

/*! \} */

/*!
 * \defgroup        voltage_api_update_voltages Test for voltage_api_update_voltages function.
 * \{
 */

void check_voltage_api_update_voltages_with_valid_parameters(void) {
    const volt voltages[DEFINES_SERIES_COUNT] = { 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OK, voltage_api_update_voltages(0U, voltages, DEFINES_SERIES_COUNT), "voltage_api_update_voltages failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_SERIES_COUNT, "Stored voltages are different!");
}

void check_voltage_api_update_voltages_with_null_voltages(void) {
    volt voltages[DEFINES_SERIES_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_NULL_POINTER, voltage_api_update_voltages(0U, NULL, DEFINES_SERIES_COUNT), "voltage_api_update_voltages returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_SERIES_COUNT, "Previously stored values have been modified!");
}

void check_voltage_api_update_voltages_when_index_is_out_of_bounds(void) {
    const volt VOLTAGES[DEFINES_SERIES_COUNT] = { 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f };
    volt voltages[DEFINES_SERIES_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_update_voltages(DEFINES_SERIES_COUNT, VOLTAGES, DEFINES_SERIES_COUNT), "voltage_api_update_voltages returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_SERIES_COUNT, "Previously stored values have been modified!");
}

void check_voltage_api_update_voltages_when_size_is_too_big(void) {
#define COUNT (9U)
    const volt VOLTAGES[COUNT] = { 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f, 3.8f, 3.9f, 4.f };
    volt voltages[DEFINES_SERIES_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_update_voltages(0U, VOLTAGES, COUNT), "voltage_api_update_voltages returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_SERIES_COUNT, "Previously stored values have been modified!");
#undef COUNT
}

/*! \} */

/*!
 * \defgroup        voltage_api_get_min Test for voltage_api_get_min function.
 * \{
 */

void check_voltage_api_get_min(void) {
    const volt voltages[DEFINES_SERIES_COUNT] = { 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f };
    memcpy(voltage_handler.voltages, voltages, DEFINES_SERIES_COUNT * sizeof(*voltages));
    TEST_ASSERT_EQUAL_FLOAT(voltages[0U], voltage_api_get_min());
}

/*! \} */

/*!
 * \defgroup        voltage_api_get_max Test for voltage_api_get_max function.
 * \{
 */

void check_voltage_api_get_max(void) {
    const volt voltages[DEFINES_SERIES_COUNT] = { 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f };
    memcpy(voltage_handler.voltages, voltages, DEFINES_SERIES_COUNT * sizeof(*voltages));
    TEST_ASSERT_EQUAL_FLOAT(voltages[5U], voltage_api_get_max());
}

/*! \} */

/*!
 * \defgroup        voltage_api_get_average Test for voltage_api_get_average function.
 * \{
 */

void check_voltage_api_get_average(void) {
    const volt voltages[DEFINES_SERIES_COUNT] = { 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f };
    memcpy(voltage_handler.voltages, voltages, DEFINES_SERIES_COUNT * sizeof(*voltages));
    TEST_ASSERT_EQUAL_FLOAT(3.45f, voltage_api_get_average());
}

/*! \} */

/*!
 * \defgroup        voltage_api_get_sum Test for voltage_api_get_sum function.
 * \{
 */

void check_voltage_api_get_sum(void) {
    const volt voltages[DEFINES_SERIES_COUNT] = { 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f };
    memcpy(voltage_handler.voltages, voltages, DEFINES_SERIES_COUNT * sizeof(*voltages));
    TEST_ASSERT_EQUAL_FLOAT(20.7f, voltage_api_get_sum());
}

/*! \} */

/*!
 * \defgroup        voltage_api_dump_voltages Test for voltage_api_dump_voltages function.
 * \{
 */

void check_voltage_api_dump_voltages_with_valid_parameters(void) {
#define COUNT (3U)
    volt dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OK, voltage_api_dump_voltages(dump, 2U, COUNT), "voltage_api_dump_voltages failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltage_handler.voltages + 2U, dump, COUNT, "voltages are different!");
#undef COUNT
}

void check_voltage_api_dump_voltages_with_null_out(void) {
    TEST_ASSERT_EQUAL_INT(VOLTAGE_RC_NULL_POINTER, voltage_api_dump_voltages(NULL, 0U, 0U));
}

void check_voltage_api_dump_voltages_when_start_is_out_of_bounds(void) {
#define COUNT (3U)
    volt dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_dump_voltages(dump, DEFINES_SERIES_COUNT, COUNT));
#undef COUNT
}

void check_voltage_api_dump_voltages_when_size_is_too_big(void) {
#define COUNT (8U)
    volt dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_dump_voltages(dump, 0U, COUNT));
#undef COUNT
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*!
     * \defgroup		voltage_api_init Test for voltage_api_init function.
     * \{
     */

    RUN_TEST(check_voltage_api_init);

    /*! \} */

    /*!
     * \defgroup        voltage_api_update_voltage Test for voltage_api_update_voltage function.
     * \{
     */

    RUN_TEST(check_voltage_api_update_voltage_with_valid_parameters);
    RUN_TEST(check_voltage_api_update_voltage_when_index_is_out_of_bounds);

    /*! \} */

    /*!
     * \defgroup        voltage_api_update_voltages Test for voltage_api_update_voltages function.
     * \{
     */

    RUN_TEST(check_voltage_api_update_voltages_with_valid_parameters);
    RUN_TEST(check_voltage_api_update_voltages_with_null_voltages);
    RUN_TEST(check_voltage_api_update_voltages_when_index_is_out_of_bounds);
    RUN_TEST(check_voltage_api_update_voltages_when_size_is_too_big);

    /*! \} */

    /*!
	 * \defgroup        voltage_api_get_min Test for voltage_api_get_min function.
	 * \{
	 */

    RUN_TEST(check_voltage_api_get_min);

    /*! \} */

    /*!
	 * \defgroup        voltage_api_get_max Test for voltage_api_get_max function.
	 * \{
	 */

    RUN_TEST(check_voltage_api_get_max);

    /*! \} */

    /*!
	 * \defgroup        voltage_api_get_average Test for voltage_api_get_average function.
	 * \{
	 */

    RUN_TEST(check_voltage_api_get_average);

    /*! \} */

    /*!
	 * \defgroup        voltage_api_get_sum Test for voltage_api_get_sum function.
	 * \{
	 */

    RUN_TEST(check_voltage_api_get_sum);

    /*! \} */

    /*!
     * \defgroup        voltage_api_dump_voltages Test for voltage_api_dump_voltages function.
     * \{
     */

    RUN_TEST(check_voltage_api_dump_voltages_with_valid_parameters);
    RUN_TEST(check_voltage_api_dump_voltages_with_null_out);
    RUN_TEST(check_voltage_api_dump_voltages_when_start_is_out_of_bounds);
    RUN_TEST(check_voltage_api_dump_voltages_when_size_is_too_big);

    /*! \} */

    UNITY_END();
}
