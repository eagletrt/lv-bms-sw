/*!
 * \file            test-balancing.c
 * \date            2026-09-08
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the balancing module.
 */

#include "unity.h"

#include <string.h>

#include "balancing.h"
#include "balancing-api.h"

#include "config.h"
#include "defines.h"
#include "types.h"

#include "voltage-api.h"
#include "bms-monitor.h"
#include "bms-monitor-api.h"

extern struct BalancingHandler balancing_handler;
extern struct BmsMonitorHandler bms_monitor_handler;
extern uint8_t prv_balancing_api_compute_mask(bool odd_parity);

void setUp(void) {
    memset(&bms_monitor_handler, 0, sizeof(bms_monitor_handler));
    voltage_api_init();
    balancing_api_init();
}

void tearDown(void) {
}

/*!
 * \defgroup        balancing_api_init Test for balancing_api_init function.
 * \{
 */

void test_balancing_api_init_resets_handler(void) {
    balancing_handler.target = 4.2F;
    balancing_handler.threshold = 1.F;
    balancing_handler.last_run = 42U;
    balancing_handler.is_active = true;
    balancing_handler.odd_phase = true;

    TEST_ASSERT_EQUAL(BALANCING_RC_OK, balancing_api_init());

    TEST_ASSERT_EQUAL_FLOAT(0.F, balancing_handler.target);
    TEST_ASSERT_EQUAL_FLOAT(0.F, balancing_handler.threshold);
    TEST_ASSERT_EQUAL_UINT32(0U, balancing_handler.last_run);
    TEST_ASSERT_FALSE(balancing_handler.is_active);
    TEST_ASSERT_FALSE(balancing_handler.odd_phase);
}

/*! \} */

/*!
 * \defgroup        balancing_api_start Test for balancing_api_start function.
 * \{
 */

void test_balancing_api_start_valid(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.80F, 3.75F, 3.70F, 3.85F, 3.72F, 3.79F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);

    TEST_ASSERT_EQUAL(BALANCING_RC_OK, balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V));

    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 3.70F, balancing_handler.target);
    TEST_ASSERT_EQUAL_FLOAT(BALANCING_THRESHOLD_V, balancing_handler.threshold);
    TEST_ASSERT_TRUE(balancing_handler.is_active);
    TEST_ASSERT_TRUE(balancing_api_is_active());
}

void test_balancing_api_start_refuses_invalid_target(void) {
    TEST_ASSERT_EQUAL(BALANCING_RC_OUT_OF_BOUNDS, balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V));
    TEST_ASSERT_EQUAL(BALANCING_RC_OUT_OF_BOUNDS, balancing_api_start(2.0F, BALANCING_THRESHOLD_V));
    TEST_ASSERT_EQUAL(BALANCING_RC_OUT_OF_BOUNDS, balancing_api_start(4.3F, BALANCING_THRESHOLD_V));
    TEST_ASSERT_FALSE(balancing_api_is_active());
}

/*! \} */

/*!
 * \defgroup        balancing_api_stop Test for balancing_api_stop function.
 * \{
 */

void test_balancing_api_stop_clears_discharge(void) {
    (void)bms_monitor_api_set_discharge(0b00010101U);

    TEST_ASSERT_EQUAL(BALANCING_RC_OK, balancing_api_stop());

    TEST_ASSERT_EQUAL_UINT8(0U, bms_monitor_handler.requested_configuration.DCC);
    TEST_ASSERT_FALSE(balancing_api_is_active());
}

/*! \} */

/*!
 * \defgroup        prv_balancing_api_compute_mask Test for mask computation.
 * \{
 */

void test_prv_compute_mask_even_parity(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.80F, 3.70F, 3.80F, 3.70F, 3.80F, 3.70F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);
    (void)balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V);

    TEST_ASSERT_EQUAL_UINT8(0b00010101U, prv_balancing_api_compute_mask(false));
}

void test_prv_compute_mask_odd_parity(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.70F, 3.80F, 3.70F, 3.80F, 3.70F, 3.80F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);
    (void)balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V);

    TEST_ASSERT_EQUAL_UINT8(0b00101010U, prv_balancing_api_compute_mask(true));
}

void test_prv_compute_mask_never_adjacent(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.90F, 3.90F, 3.70F, 3.70F, 3.70F, 3.70F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);
    (void)balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V);

    const uint8_t even_mask = prv_balancing_api_compute_mask(false);
    const uint8_t odd_mask = prv_balancing_api_compute_mask(true);

    TEST_ASSERT_EQUAL_UINT8(0b00000001U, even_mask);
    TEST_ASSERT_EQUAL_UINT8(0b00000010U, odd_mask);
    TEST_ASSERT_EQUAL_UINT8(0U, even_mask & (even_mask << 1U));
    TEST_ASSERT_EQUAL_UINT8(0U, odd_mask & (odd_mask << 1U));
}

void test_prv_compute_mask_threshold_boundary(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.70F + BALANCING_THRESHOLD_V, 3.70F, 3.70F, 3.70F, 3.70F, 3.70F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);
    (void)balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V);

    TEST_ASSERT_EQUAL_UINT8(0U, prv_balancing_api_compute_mask(false));
}

/*! \} */

/*!
 * \defgroup        balancing_api_run Test for balancing_api_run function.
 * \{
 */

void test_balancing_api_run_inactive_noop(void) {
    TEST_ASSERT_EQUAL(BALANCING_RC_OK, balancing_api_run(10000U));
    TEST_ASSERT_EQUAL_UINT32(0U, balancing_handler.last_run);
    TEST_ASSERT_EQUAL_UINT8(0U, bms_monitor_handler.requested_configuration.DCC);
}

void test_balancing_api_run_cadence_gate(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.90F, 3.70F, 3.70F, 3.70F, 3.70F, 3.70F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);
    (void)balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V);

    TEST_ASSERT_EQUAL(BALANCING_RC_OK, balancing_api_run(BALANCING_RUN_PERIOD_MS - 1U));
    TEST_ASSERT_EQUAL_UINT8(0U, bms_monitor_handler.requested_configuration.DCC);

    TEST_ASSERT_EQUAL(BALANCING_RC_OK, balancing_api_run(BALANCING_RUN_PERIOD_MS));
    TEST_ASSERT_EQUAL_UINT32(BALANCING_RUN_PERIOD_MS, balancing_handler.last_run);
    TEST_ASSERT_EQUAL_UINT8(0b00000010U, bms_monitor_handler.requested_configuration.DCC);
}

void test_balancing_api_run_alternates_parity(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.90F, 3.90F, 3.70F, 3.70F, 3.70F, 3.70F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);
    (void)balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V);

    (void)balancing_api_run(BALANCING_RUN_PERIOD_MS);
    TEST_ASSERT_EQUAL_UINT8(0b00000100U, bms_monitor_handler.requested_configuration.DCC);

    (void)balancing_api_run(2U * BALANCING_RUN_PERIOD_MS);
    TEST_ASSERT_EQUAL_UINT8(0b00000010U, bms_monitor_handler.requested_configuration.DCC);
}

void test_balancing_api_run_completes_when_balanced(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.72F, 3.71F, 3.70F, 3.73F, 3.71F, 3.72F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);
    (void)balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V);

    (void)balancing_api_run(BALANCING_RUN_PERIOD_MS);

    TEST_ASSERT_FALSE(balancing_api_is_active());
    TEST_ASSERT_EQUAL_UINT8(0U, bms_monitor_handler.requested_configuration.DCC);
}

void test_balancing_api_run_parity_peek(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.90F, 3.70F, 3.70F, 3.70F, 3.70F, 3.70F };
    (void)voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT);
    (void)balancing_api_start(voltage_api_get_min(), BALANCING_THRESHOLD_V);

    (void)balancing_api_run(BALANCING_RUN_PERIOD_MS);

    TEST_ASSERT_TRUE(balancing_api_is_active());
    TEST_ASSERT_EQUAL_UINT8(0b00000010U, bms_monitor_handler.requested_configuration.DCC);
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*! \defgroup    init Run init tests.
     *  \{
     */
    RUN_TEST(test_balancing_api_init_resets_handler);
    /*! \} */

    /*! \defgroup    start Run start tests.
     *  \{
     */
    RUN_TEST(test_balancing_api_start_valid);
    RUN_TEST(test_balancing_api_start_refuses_invalid_target);
    /*! \} */

    /*! \defgroup    stop Run stop tests.
     *  \{
     */
    RUN_TEST(test_balancing_api_stop_clears_discharge);
    /*! \} */

    /*! \defgroup    compute_mask Run mask computation tests.
     *  \{
     */
    RUN_TEST(test_prv_compute_mask_even_parity);
    RUN_TEST(test_prv_compute_mask_odd_parity);
    RUN_TEST(test_prv_compute_mask_never_adjacent);
    RUN_TEST(test_prv_compute_mask_threshold_boundary);
    /*! \} */

    /*! \defgroup    run Run cycle tests.
     *  \{
     */
    RUN_TEST(test_balancing_api_run_inactive_noop);
    RUN_TEST(test_balancing_api_run_cadence_gate);
    RUN_TEST(test_balancing_api_run_alternates_parity);
    RUN_TEST(test_balancing_api_run_completes_when_balanced);
    RUN_TEST(test_balancing_api_run_parity_peek);
    /*! \} */

    return UNITY_END();
}
