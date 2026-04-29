/*!
 * \file            test-temperature-api.h
 * \date            2026-03-31
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the temperature module.
 */

#include "unity.h"

#include <string.h>

#include "temperature.h"
#include "temperature-api.h"
#include "types.h"
#include "defs.h"

extern struct TemperatureHandler temperature_handler;

void setUp(void) {
    temperature_api_init();
}

void tearDown() {
}

/*!
 * \defgroup		temperature_api_init Test for temperature_api_init function.
 * \{
 */

void check_temperature_api_init(void) {
    celsius temperatures[DEFS_CELLS_COUNT] = { 0 };
    memset(temperature_handler.temperatures, 0xFF, DEFS_CELLS_COUNT * sizeof(*temperature_handler.temperatures));
    temperature_api_init();
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(temperatures, temperature_handler.temperatures, DEFS_CELLS_COUNT);
}

/*! \} */

/*!
 * \defgroup        temperature_api_update_temperature Test for temperature_api_update_temperature function.
 * \{
 */

void check_temperature_api_update_temperature_with_valid_parameters(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OK, temperature_api_update_temperature(0U, 7.f), "temperature_api_update_temperature failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(7.f, temperature_handler.temperatures[0U], "Stored temperature is different!");
}

void check_temperature_api_update_temperature_when_index_is_out_of_bounds(void) {
    celsius temperatures[DEFS_CELLS_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_update_temperature(DEFS_CELLS_COUNT, 7.f), "temperature_api_update_temperature returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFS_CELLS_COUNT, "Previously stored values have been modified!");
}

/*! \} */

/*!
 * \defgroup        temperature_api_update_temperatures Test for temperature_api_update_temperatures function.
 * \{
 */

void check_temperature_api_update_temperatures_with_valid_parameters(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OK, temperature_api_update_temperatures(0U, temperatures, DEFS_CELLS_COUNT), "temperature_api_update_temperatures failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFS_CELLS_COUNT, "Stored temperatures are different!");
}

void check_temperature_api_update_temperatures_with_null_temperatures(void) {
    celsius temperatures[DEFS_CELLS_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_NULL_POINTER, temperature_api_update_temperatures(0U, NULL, DEFS_CELLS_COUNT), "temperature_api_update_temperatures returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFS_CELLS_COUNT, "Previously stored values have been modified!");
}

void check_temperature_api_update_temperatures_when_index_is_out_of_bounds(void) {
    const celsius mock_temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    celsius temperatures[DEFS_CELLS_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_update_temperatures(DEFS_CELLS_COUNT, mock_temperatures, DEFS_CELLS_COUNT), "temperature_api_update_temperatures returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFS_CELLS_COUNT, "Previously stored values have been modified!");
}

void check_temperature_api_update_temperatures_when_size_is_too_big(void) {
#define COUNT (9U)
    const celsius mock_temperatures[COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
    celsius temperatures[DEFS_CELLS_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_update_temperatures(0U, mock_temperatures, COUNT), "temperature_api_update_temperatures returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFS_CELLS_COUNT, "Previously stored values have been modified!");
#undef COUNT
}

/*! \} */

/*!
 * \defgroup        temperature_api_get_min Test for temperature_api_get_min function.
 * \{
 */

void check_temperature_api_get_min(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    memcpy(temperature_handler.temperatures, temperatures, DEFS_CELLS_COUNT * sizeof(*temperatures));
    TEST_ASSERT_EQUAL_FLOAT(temperatures[0U], temperature_api_get_min());
}

/*! \} */

/*!
 * \defgroup        temperature_api_get_max Test for temperature_api_get_max function.
 * \{
 */

void check_temperature_api_get_max(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    memcpy(temperature_handler.temperatures, temperatures, DEFS_CELLS_COUNT * sizeof(*temperatures));
    TEST_ASSERT_EQUAL_FLOAT(temperatures[5U], temperature_api_get_max());
}

/*! \} */

/*!
 * \defgroup        temperature_api_get_average Test for temperature_api_get_average function.
 * \{
 */

void check_temperature_api_get_average(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    memcpy(temperature_handler.temperatures, temperatures, DEFS_CELLS_COUNT * sizeof(*temperatures));
    TEST_ASSERT_EQUAL_FLOAT(3.5f, temperature_api_get_average());
}

/*! \} */

/*!
 * \defgroup        temperature_api_dump_temperatures Test for temperature_api_dump_temperatures function.
 * \{
 */

void check_temperature_api_dump_temperatures_with_valid_parameters(void) {
#define COUNT (3U)
    celsius dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OK, temperature_api_dump_temperatures(dump, 2U, COUNT), "temperature_api_dump_temperatures failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperature_handler.temperatures + 2U, dump, COUNT, "temperatures are different!");
#undef COUNT
}

void check_temperature_api_dump_temperatures_with_null_out(void) {
    TEST_ASSERT_EQUAL_INT(TEMPERATURE_RC_NULL_POINTER, temperature_api_dump_temperatures(NULL, 0U, 0U));
}

void check_temperature_api_dump_temperatures_when_start_is_out_of_bounds(void) {
#define COUNT (3U)
    celsius dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_dump_temperatures(dump, DEFS_CELLS_COUNT, COUNT));
#undef COUNT
}

void check_temperature_api_dump_temperatures_when_size_is_too_big(void) {
#define COUNT (8U)
    celsius dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_dump_temperatures(dump, 0U, COUNT));
#undef COUNT
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*!
     * \defgroup		temperature_api_init Test for temperature_api_init function.
     * \{
     */

    RUN_TEST(check_temperature_api_init);

    /*! \} */

    /*!
     * \defgroup        temperature_api_update_temperature Test for temperature_api_update_temperature function.
     * \{
     */

    RUN_TEST(check_temperature_api_update_temperature_with_valid_parameters);
    RUN_TEST(check_temperature_api_update_temperature_when_index_is_out_of_bounds);

    /*! \} */

    /*!
     * \defgroup        temperature_api_update_temperatures Test for temperature_api_update_temperatures function.
     * \{
     */

    RUN_TEST(check_temperature_api_update_temperatures_with_valid_parameters);
    RUN_TEST(check_temperature_api_update_temperatures_with_null_temperatures);
    RUN_TEST(check_temperature_api_update_temperatures_when_index_is_out_of_bounds);
    RUN_TEST(check_temperature_api_update_temperatures_when_size_is_too_big);

    /*! \} */

    /*!
	 * \defgroup        temperature_api_get_min Test for temperature_api_get_min function.
	 * \{
	 */

    RUN_TEST(check_temperature_api_get_min);

    /*! \} */

    /*!
	 * \defgroup        temperature_api_get_max Test for temperature_api_get_max function.
	 * \{
	 */

    RUN_TEST(check_temperature_api_get_max);

    /*! \} */

    /*!
	 * \defgroup        temperature_api_get_average Test for temperature_api_get_average function.
	 * \{
	 */

    RUN_TEST(check_temperature_api_get_average);

    /*! \} */

    /*!
     * \defgroup        temperature_api_dump_temperatures Test for temperature_api_dump_temperatures function.
     * \{
     */

    RUN_TEST(check_temperature_api_dump_temperatures_with_valid_parameters);
    RUN_TEST(check_temperature_api_dump_temperatures_with_null_out);
    RUN_TEST(check_temperature_api_dump_temperatures_when_start_is_out_of_bounds);
    RUN_TEST(check_temperature_api_dump_temperatures_when_size_is_too_big);

    /*! \} */

    UNITY_END();
}
