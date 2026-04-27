/*!
 * \file            test-temp.h
 * \date            2026-03-31
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the temperature module.
 */

#include "unity.h"

#include <string.h>

#include "temp.h"
#include "temp-api.h"
#include "types.h"
#include "defs.h"

extern struct TempHandler htemp;

void setUp(void) {
    temp_api_init();
}

void tearDown() {
}

/*!
 * \defgroup		temp_api_init Test for temp_api_init function.
 * \{
 */

void check_temp_api_init(void) {
    celsius temperatures[DEFS_CELLS_COUNT] = { 0.f };
    memset(htemp.temperatures, 0xFF, DEFS_CELLS_COUNT * sizeof(*htemp.temperatures));
    temp_api_init();
    TEST_ASSERT_EQUAL_MEMORY(temperatures, htemp.temperatures, DEFS_CELLS_COUNT);
}

/*! \} */

/*!
 * \defgroup        temp_api_update_temperature Test for temp_api_update_temperature function.
 * \{
 */

void check_temp_api_update_temperature_success(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_temperature(0U, 7.f), "temp_api_update_temperature failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(7.f, htemp.temperatures[0U], "Stored temperature is different!");
}

void check_temp_api_update_temperature_index_out_of_bounds(void) {
    TEST_ASSERT_EQUAL_INT(TEMP_RC_OUT_OF_BOUNDS, temp_api_update_temperature(DEFS_CELLS_COUNT, 7.f));
}

/*! \} */

/*!
 * \defgroup        temp_api_update_temperatures Test for temp_api_update_temperatures function.
 * \{
 */

void check_temp_api_update_temperatures_success(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_temperatures(0U, temperatures, DEFS_CELLS_COUNT), "temp_api_update_temperatures failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, htemp.temperatures, DEFS_CELLS_COUNT, "Stored temperatures are different!");
}

void check_temp_api_update_temperatures_null_temperatures(void) {
    TEST_ASSERT_EQUAL_INT(TEMP_RC_NULL_POINTER, temp_api_update_temperatures(0U, NULL, 0U));
}

void check_temp_api_update_temperatures_index_out_of_bounds(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OUT_OF_BOUNDS, temp_api_update_temperatures(DEFS_CELLS_COUNT, temperatures, DEFS_CELLS_COUNT), "temp_api_update_temperatures failed!");
}

void check_temp_api_update_temperatures_size_too_big(void) {
#define COUNT (9U)
    const celsius temperatures[COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OUT_OF_BOUNDS, temp_api_update_temperatures(0U, temperatures, COUNT), "temp_api_update_temperatures failed!");
#undef COUNT
}

/*! \} */

/*!
 * \defgroup        temp_api_get_min Test for temp_api_get_min function.
 * \{
 */

void check_temp_api_get_min(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_temperatures(0U, temperatures, DEFS_CELLS_COUNT), "temp_api_update_temperatures failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(temperatures[0U], temp_api_get_min(), "temperatures are different!");
}

/*! \} */

/*!
 * \defgroup        temp_api_get_max Test for temp_api_get_max function.
 * \{
 */

void check_temp_api_get_max(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_temperatures(0U, temperatures, DEFS_CELLS_COUNT), "temp_api_update_temperatures failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(temperatures[5U], temp_api_get_max(), "temperatures are different!");
}

/*! \} */

/*!
 * \defgroup        temp_api_get_average Test for temp_api_get_average function.
 * \{
 */

void check_temp_api_get_average(void) {
    const celsius temperatures[DEFS_CELLS_COUNT] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_temperatures(0U, temperatures, DEFS_CELLS_COUNT), "temp_api_update_temperatures failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.5f, temp_api_get_average(), "temperatures are different!");
}

/*! \} */

/*!
 * \defgroup        temp_api_dump_temperatures Test for temp_api_dump_temperatures function.
 * \{
 */

void check_temp_api_dump_temperatures_success(void) {
#define COUNT (3U)
    celsius dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_dump_temperatures(dump, 2U, COUNT), "temp_api_dump_temperatures failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(htemp.temperatures + 2U, dump, COUNT, "temperatures are different!");
#undef COUNT
}

void check_temp_api_dump_temperatures_null_out(void) {
    TEST_ASSERT_EQUAL_INT(TEMP_RC_NULL_POINTER, temp_api_dump_temperatures(NULL, 0U, 0U));
}

void check_temp_api_dump_temperatures_start_out_of_bounds(void) {
#define COUNT (3U)
    celsius dump[3U] = { 0 };
    TEST_ASSERT_EQUAL_INT(TEMP_RC_OUT_OF_BOUNDS, temp_api_dump_temperatures(dump, DEFS_CELLS_COUNT, COUNT));
#undef COUNT
}

void check_temp_api_dump_temperatures_size_too_big(void) {
#define COUNT (8U)
    celsius dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(TEMP_RC_OUT_OF_BOUNDS, temp_api_dump_temperatures(dump, 0U, COUNT));
#undef COUNT
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*!
     * \defgroup		temp_api_init Test for temp_api_init function.
     * \{
     */

    RUN_TEST(check_temp_api_init);

    /*! \} */

    /*!
     * \defgroup        temp_api_update_temperature Test for temp_api_update_temperature function.
     * \{
     */

    RUN_TEST(check_temp_api_update_temperature_success);
    RUN_TEST(check_temp_api_update_temperature_index_out_of_bounds);

    /*! \} */

    /*!
     * \defgroup        temp_api_update_temperatures Test for temp_api_update_temperatures function.
     * \{
     */

    RUN_TEST(check_temp_api_update_temperatures_success);
    RUN_TEST(check_temp_api_update_temperatures_null_temperatures);
    RUN_TEST(check_temp_api_update_temperatures_index_out_of_bounds);
    RUN_TEST(check_temp_api_update_temperatures_size_too_big);

    /*! \} */

    /*!
	 * \defgroup        temp_api_get_min Test for temp_api_get_min function.
	 * \{
	 */

    RUN_TEST(check_temp_api_get_min);

    /*! \} */

    /*!
	 * \defgroup        temp_api_get_max Test for temp_api_get_max function.
	 * \{
	 */

    RUN_TEST(check_temp_api_get_max);

    /*! \} */

    /*!
	 * \defgroup        temp_api_get_average Test for temp_api_get_average function.
	 * \{
	 */

    RUN_TEST(check_temp_api_get_average);

    /*! \} */

    /*!
     * \defgroup        temp_api_dump_temperatures Test for temp_api_dump_temperatures function.
     * \{
     */

    RUN_TEST(check_temp_api_dump_temperatures_success);
    RUN_TEST(check_temp_api_dump_temperatures_null_out);
    RUN_TEST(check_temp_api_dump_temperatures_start_out_of_bounds);
    RUN_TEST(check_temp_api_dump_temperatures_size_too_big);

    /*! \} */

    UNITY_END();
}
