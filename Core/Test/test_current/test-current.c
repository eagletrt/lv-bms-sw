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
    current_handler.cell_out_current = 3.2F;
    TEST_ASSERT_EQUAL_INT(CURRENT_RC_OK, current_api_init());
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.F, current_handler.cell_out_current, "cell_out_current not zero after init!");
}

/*! \} */

/*!
 * \defgroup		current_api_set_cells_output_current Test for current_api_set_cells_output_current function.
 * \{
 */

void check_current_api_set_cells_output_current_stores_value(void) {
    TEST_ASSERT_EQUAL_INT(CURRENT_RC_OK, current_api_set_cells_output_current(7.5F));
    TEST_ASSERT_EQUAL_FLOAT(7.5F, current_handler.cell_out_current);
}

void check_current_api_set_cells_output_current_keeps_sign(void) {
    /* Charging shows up as a negative pack current and must survive untouched. */
    TEST_ASSERT_EQUAL_INT(CURRENT_RC_OK, current_api_set_cells_output_current(-2.25F));
    TEST_ASSERT_EQUAL_FLOAT(-2.25F, current_handler.cell_out_current);
}

void check_current_api_set_cells_output_current_overwrites(void) {
    current_api_set_cells_output_current(1.F);
    current_api_set_cells_output_current(4.F);
    TEST_ASSERT_EQUAL_FLOAT(4.F, current_handler.cell_out_current);
}

/*! \} */

/*!
 * \defgroup		current_api_get_cells_output_current Test for current_api_get_cells_output_current function.
 * \{
 */

void check_current_api_get_cells_output_current(void) {
    current_handler.cell_out_current = 12.5F;
    TEST_ASSERT_EQUAL_FLOAT(12.5F, current_api_get_cells_output_current());
}

void check_current_api_get_cells_output_current_round_trip(void) {
    current_api_set_cells_output_current(-0.125F);
    TEST_ASSERT_EQUAL_FLOAT(-0.125F, current_api_get_cells_output_current());
}

/*! \} */

/*!
 * \defgroup		current_api_get_power Test for current_api_get_power function.
 * \{
 */

void check_current_api_get_power(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    memcpy(voltage_handler.voltages, voltages, DEFINES_CELLS_SERIES_COUNT * sizeof(*voltages));
    current_handler.cell_out_current = 3.2F;
    TEST_ASSERT_EQUAL_FLOAT(20.7F * 3.2F, current_api_get_power());
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(check_current_api_init);

    RUN_TEST(check_current_api_set_cells_output_current_stores_value);
    RUN_TEST(check_current_api_set_cells_output_current_keeps_sign);
    RUN_TEST(check_current_api_set_cells_output_current_overwrites);

    RUN_TEST(check_current_api_get_cells_output_current);
    RUN_TEST(check_current_api_get_cells_output_current_round_trip);

    RUN_TEST(check_current_api_get_power);

    return UNITY_END();
}
