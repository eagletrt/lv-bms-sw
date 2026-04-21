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
 * \defgroup        temp_api_update_value Test for temp_api_update_value function.
 * \{
 */

void check_temp_api_update_value_success(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_value(0U, 7.f), "temp_api_update_value failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(htemp.temperatures[0U], 7.f, "Stored temperature is different!");
}

void check_temp_api_update_value_index_out_of_bounds(void) {
    TEST_ASSERT_EQUAL_INT(TEMP_RC_OUT_OF_BOUNDS, temp_api_update_value(DEFS_CELLS_COUNT, 7.f));
}

/*! \} */

/*!
 * \defgroup        temp_api_update_values Test for temp_api_update_values function.
 * \{
 */

void check_temp_api_update_values_success(void) {
    const celsius temperatures[] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_values(0U, temperatures, DEFS_CELLS_COUNT), "temp_api_update_values failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(htemp.temperatures, temperatures, DEFS_CELLS_COUNT, "Stored temperatures are different!");
}

void check_temp_api_update_values_null_temperatures(void) {
    TEST_ASSERT_EQUAL_INT(TEMP_RC_NULL_PTR, temp_api_update_values(0U, NULL, 0U));
}

void check_temp_api_update_values_index_out_of_bounds(void) {
    const celsius temperatures[] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OUT_OF_BOUNDS, temp_api_update_values(DEFS_CELLS_COUNT, temperatures, DEFS_CELLS_COUNT), "temp_api_update_values failed!");
}

void check_temp_api_update_values_size_too_big(void) {
    const celsius temperatures[] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OUT_OF_BOUNDS, temp_api_update_values(0U, temperatures, sizeof(temperatures) / sizeof(*temperatures)), "temp_api_update_values failed!");
}

/*! \} */

/*!
 * \defgroup        temp_api_get_min Test for temp_api_get_min function.
 * \{
 */

void check_temp_api_get_min(void) {
    const celsius temperatures[] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_values(0U, temperatures, DEFS_CELLS_COUNT), "temp_api_update_values failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(temperatures[0U], temp_api_get_min(), "Values are different!");
}

/*! \} */

/*!
 * \defgroup        temp_api_get_max Test for temp_api_get_max function.
 * \{
 */

void check_temp_api_get_max(void) {
    const celsius temperatures[] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_values(0U, temperatures, DEFS_CELLS_COUNT), "temp_api_update_values failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(temperatures[5U], temp_api_get_max(), "Values are different!");
}

/*! \} */

/*!
 * \defgroup        temp_api_get_avg Test for temp_api_get_avg function.
 * \{
 */

void check_temp_api_get_avg(void) {
    const celsius temperatures[] = { 1.f, 2.f, 3.f, 4.f, 5.f, 6.f };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_update_values(0U, temperatures, DEFS_CELLS_COUNT), "temp_api_update_values failed!");

    celsius avg = 0.f;
    for (size_t i = 0; i < DEFS_CELLS_COUNT; ++i) {
        avg += temperatures[i];
    }
    avg /= DEFS_CELLS_COUNT;

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(avg, temp_api_get_avg(), "Values are different!");
}

/*! \} */

/*!
 * \defgroup        temp_api_dump_values Test for temp_api_dump_values function.
 * \{
 */

void check_temp_api_dump_values_success(void) {
    celsius dump[3U];
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMP_RC_OK, temp_api_dump_values(dump, 2U, sizeof(dump) / sizeof(*dump)), "temp_api_dump_values failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(htemp.temperatures + 2U, dump, sizeof(dump) / sizeof(*dump), "Values are different!");
}

void check_temp_api_dump_values_null_out(void) {
    TEST_ASSERT_EQUAL_INT(TEMP_RC_NULL_PTR, temp_api_dump_values(NULL, 0U, 0U));
}

void check_temp_api_dump_values_start_out_of_bounds(void) {
    celsius dump[3U];
    TEST_ASSERT_EQUAL_INT(TEMP_RC_OUT_OF_BOUNDS, temp_api_dump_values(dump, DEFS_CELLS_COUNT, sizeof(dump) / sizeof(*dump)));
}

void check_temp_api_dump_values_size_too_big(void) {
    celsius dump[8U];
    TEST_ASSERT_EQUAL_INT(TEMP_RC_OUT_OF_BOUNDS, temp_api_dump_values(dump, 0U, sizeof(dump) / sizeof(*dump)));
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
     * \defgroup        temp_api_update_value Test for temp_api_update_value function.
     * \{
     */

    RUN_TEST(check_temp_api_update_value_success);
    RUN_TEST(check_temp_api_update_value_index_out_of_bounds);

    /*! \} */

    /*!
     * \defgroup        temp_api_update_values Test for temp_api_update_values function.
     * \{
     */

    RUN_TEST(check_temp_api_update_values_success);
    RUN_TEST(check_temp_api_update_values_null_temperatures);
    RUN_TEST(check_temp_api_update_values_index_out_of_bounds);
    RUN_TEST(check_temp_api_update_values_size_too_big);

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
	 * \defgroup        temp_api_get_avg Test for temp_api_get_avg function.
	 * \{
	 */

    RUN_TEST(check_temp_api_get_avg);

    /*! \} */

    /*!
     * \defgroup        temp_api_dump_values Test for temp_api_dump_values function.
     * \{
     */

    RUN_TEST(check_temp_api_dump_values_success);
    RUN_TEST(check_temp_api_dump_values_null_out);
    RUN_TEST(check_temp_api_dump_values_start_out_of_bounds);
    RUN_TEST(check_temp_api_dump_values_size_too_big);

    /*! \} */

    UNITY_END();
}
