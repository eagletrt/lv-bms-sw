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

void tearDown() {
}

/*!
 * \defgroup		current_api_init Test for current_api_init function.
 * \{
 */

void check_current_api_init(void) {
    ampere current = 0.F;
    current_api_init();
    TEST_ASSERT_EQUAL_FLOAT(current, current_handler.current);
}

/*! \} */

/*!
 * \defgroup		current_api_update_current Test for current_api_update_current function
 */

void check_current_api_update_current_with_valid_parameters(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(CURRENT_RC_OK, current_api_update_current(3.2F), "current_api_update_current failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.2F, current_handler.current, "Stored current is different!");
}

/*! \{ */

/*!
 * \defgroup		current_api_get_current Test for current_api_get_current function.
 */

void check_current_api_get_current(void) {
    current_handler.current = 3.2F;
    TEST_ASSERT_EQUAL_FLOAT(3.2F, current_api_get_current());
}

/*!  */

/*!
 * \defgroup		current_api_get_power Test for current_api_get_power function.
 */

void check_current_api_get_power(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    memcpy(voltage_handler.voltages, voltages, DEFINES_CELLS_SERIES_COUNT * sizeof(*voltages));
    current_handler.current = 3.2F;
    TEST_ASSERT_EQUAL_FLOAT(20.7F * 3.2F, current_api_get_power());
}

/*!  */

int main(void) {
    UNITY_BEGIN();

    /*!
	 * \defgroup	 current_api_init Test for current_api_init function.
	 * \{
	 */

    RUN_TEST(check_current_api_init);

    /*! \} */

    /*!
	 * \defgroup	 current_api_update_current Test for current_api_update_current function
	 */

    RUN_TEST(check_current_api_update_current_with_valid_parameters);

    /*! \} */

    /*!
	 * \defgroup	 current_api_api_get_current Test for current_api_api_get_current function
	 */

    RUN_TEST(check_current_api_get_current);

    /*! \} */

    /*!
	 * \defgroup	 current_api_api_get_power Test for current_api_api_get_power function
	 */

    RUN_TEST(check_current_api_get_power);

    /*! \} */

    UNITY_END();
}
