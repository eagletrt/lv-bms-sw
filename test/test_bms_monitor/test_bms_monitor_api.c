/*!
 * \file            test_bms_monitor_api.c
 * \date            2026-07-29
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the bms monitor module.
 */

#include "unity.h"
#include "fff.h"

#include <string.h>

#include "bms_monitor.h"
#include "bms_monitor_api.h"

#include "config.h"
#include "defines.h"
#include "types.h"

/* FFF mocks for SPI callbacks passed to init */
FAKE_VALUE_FUNC(enum BmsMonitorReturnCode, send_fake, uint8_t *const, const size_t);
FAKE_VALUE_FUNC(enum BmsMonitorReturnCode, send_receive_fake, uint8_t *const, uint8_t *, const size_t, const size_t);

extern struct BmsMonitorHandler bms_monitor_handler;
extern bool prv_bms_monitor_api_is_cells_bitmask_valid(uint8_t cells);

void setUp(void) {
    RESET_FAKE(send_fake);
    RESET_FAKE(send_receive_fake);
    memset(&bms_monitor_handler, 0, sizeof(bms_monitor_handler));
}

void tearDown(void) {
}

/*!
 * \defgroup		prv_bms_monitor_api_is_cells_bitmask_valid Test for validation helper.
 * \{
 */

void test_prv_is_cells_bitmask_valid_adjacent(void) {
    TEST_ASSERT_TRUE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00000011U), "cells 0 and 1 adjacent not detected");
    TEST_ASSERT_TRUE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00000110U), "cells 1 and 2 adjacent not detected");
    TEST_ASSERT_TRUE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00001100U), "cells 2 and 3 adjacent not detected");
    TEST_ASSERT_TRUE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00011000U), "cells 3 and 4 adjacent not detected");
    TEST_ASSERT_TRUE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00110000U), "cells 4 and 5 adjacent not detected");
}

void test_prv_is_cells_bitmask_valid_high_bits(void) {
    TEST_ASSERT_TRUE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b01000000U), "bit 6 not rejected");
    TEST_ASSERT_TRUE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b10000000U), "bit 7 not rejected");
    TEST_ASSERT_TRUE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b11000000U), "bits 6 and 7 not rejected");
}

void test_prv_is_cells_bitmask_valid_non_adjacent(void) {
    TEST_ASSERT_FALSE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00000000U), "no cells should be valid");
    TEST_ASSERT_FALSE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00000001U), "single cell 0 rejected");
    TEST_ASSERT_FALSE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00000100U), "single cell 2 rejected");
    TEST_ASSERT_FALSE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00010000U), "single cell 4 rejected");
    TEST_ASSERT_FALSE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00000101U), "cells 0 and 2 non-adjacent rejected");
    TEST_ASSERT_FALSE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00010001U), "cells 0 and 4 non-adjacent rejected");
    TEST_ASSERT_FALSE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00010101U), "cells 0,2,4 non-adjacent rejected");
    TEST_ASSERT_FALSE_MESSAGE(prv_bms_monitor_api_is_cells_bitmask_valid(0b00101010U), "cells 1,3,5 non-adjacent rejected");
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_init Test for bms_monitor_api_init function.
 * \{
 */

void test_bms_monitor_api_init_null(void) {
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_NULL_POINTER, bms_monitor_api_init(NULL, NULL), "both NULL not detected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_NULL_POINTER, bms_monitor_api_init(send_fake, NULL), "send_receive NULL not detected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_NULL_POINTER, bms_monitor_api_init(NULL, send_receive_fake), "send NULL not detected");
}

void test_bms_monitor_api_init_ok(void) {
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_OK, bms_monitor_api_init(send_fake, send_receive_fake));
}

void test_bms_monitor_api_init_config(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    TEST_ASSERT_EQUAL(1U, bms_monitor_handler.requested_configuration.REFON);
}

void test_bms_monitor_api_init_send(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    TEST_ASSERT_EQUAL_PTR(send_fake, bms_monitor_handler.send);
}

void test_bms_monitor_api_init_send_receive(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    TEST_ASSERT_EQUAL_PTR(send_receive_fake, bms_monitor_handler.send_receive);
}

void test_bms_monitor_api_init_ltc_handler(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    TEST_ASSERT_EQUAL(DEFINES_LTC_COUNT, bms_monitor_handler.ltc_handler.count);
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_set_discharge Test for bms_monitor_api_set_discharge function.
 * \{
 */

void test_bms_monitor_api_set_discharge_invalid_adjacent(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b00000011U), "cells 0,1 adjacent not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b00000110U), "cells 1,2 adjacent not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b00011000U), "cells 3,4 adjacent not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b00110000U), "cells 4,5 adjacent not rejected");
}

void test_bms_monitor_api_set_discharge_invalid_high_bits(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b01000000U), "bit 6 not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b10000000U), "bit 7 not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b11000000U), "bits 6,7 not rejected");
}

void test_bms_monitor_api_set_discharge_valid(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00000000U), "no cells rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00000001U), "single cell 0 rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00000100U), "single cell 2 rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00010000U), "single cell 4 rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00010101U), "cells 0,2,4 non-adjacent rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00101010U), "cells 1,3,5 non-adjacent rejected");
}

void test_bms_monitor_api_set_discharge_dcc(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    bms_monitor_api_set_discharge(0b00010101U);
    TEST_ASSERT_EQUAL(0b00010101U, bms_monitor_handler.requested_configuration.DCC);
}

void test_bms_monitor_api_set_discharge_dcto_off(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    bms_monitor_api_set_discharge(0b00000000U);
    TEST_ASSERT_EQUAL(LTC6810_2_DCTO_OFF, bms_monitor_handler.requested_configuration.DCTO);
}

void test_bms_monitor_api_set_discharge_dcto_30s(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    bms_monitor_api_set_discharge(0b00000001U);
    TEST_ASSERT_EQUAL_MESSAGE(LTC6810_2_DCTO_30S, bms_monitor_handler.requested_configuration.DCTO, "DCTO not 30s with single cell");
    bms_monitor_api_set_discharge(0b00010101U);
    TEST_ASSERT_EQUAL_MESSAGE(LTC6810_2_DCTO_30S, bms_monitor_handler.requested_configuration.DCTO, "DCTO not 30s with multi cell");
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_get_discharge Test for bms_monitor_api_get_discharge function.
 * \{
 */

void test_bms_monitor_api_get_discharge(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    bms_monitor_handler.actual_configuration.DCC = 0b00010101U;
    TEST_ASSERT_EQUAL(0b00010101U, bms_monitor_api_get_discharge());
}

void test_bms_monitor_api_get_discharge_zero(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    bms_monitor_handler.actual_configuration.DCC = 0b00000000U;
    TEST_ASSERT_EQUAL(0b00000000U, bms_monitor_api_get_discharge());
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_write_configuration Test for bms_monitor_api_write_configuration function.
 * \{
 */

void test_bms_monitor_api_write_configuration_ok(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    send_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_OK, bms_monitor_api_write_configuration());
}

void test_bms_monitor_api_write_configuration_send_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    send_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_write_configuration());
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_read_configuration Test for bms_monitor_api_read_configuration function.
 * \{
 */

void test_bms_monitor_api_read_configuration_decode_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_DECODE_ERROR, bms_monitor_api_read_configuration());
}

void test_bms_monitor_api_read_configuration_send_receive_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_read_configuration());
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_start_volt_covertion Test for bms_monitor_api_start_volt_covertion function.
 * \{
 */

void test_bms_monitor_api_start_volt_conversion_ok(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    send_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_OK, bms_monitor_api_start_volt_covertion());
}

void test_bms_monitor_api_start_volt_conversion_send_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    send_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_start_volt_covertion());
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_read_voltages Test for bms_monitor_api_read_voltages function.
 * \{
 */

void test_bms_monitor_api_read_voltages_decode_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_DECODE_ERROR,
                      bms_monitor_api_read_voltages(BMS_MONITOR_VOLTAGE_REGISTER_A));
}

void test_bms_monitor_api_read_voltages_send_receive_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_read_voltages(BMS_MONITOR_VOLTAGE_REGISTER_A));
}

/*! \} */

DEFINE_FFF_GLOBALS

int main(void) {
    UNITY_BEGIN();

    /*!
	 * \defgroup	 prv_bms_monitor_api_is_cells_bitmask_valid Test for validation helper.
	 * \{
	 */

    RUN_TEST(test_prv_is_cells_bitmask_valid_adjacent);
    RUN_TEST(test_prv_is_cells_bitmask_valid_high_bits);
    RUN_TEST(test_prv_is_cells_bitmask_valid_non_adjacent);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_init Test for bms_monitor_api_init function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_init_null);
    RUN_TEST(test_bms_monitor_api_init_ok);
    RUN_TEST(test_bms_monitor_api_init_config);
    RUN_TEST(test_bms_monitor_api_init_send);
    RUN_TEST(test_bms_monitor_api_init_send_receive);
    RUN_TEST(test_bms_monitor_api_init_ltc_handler);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_set_discharge Test for bms_monitor_api_set_discharge function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_set_discharge_invalid_adjacent);
    RUN_TEST(test_bms_monitor_api_set_discharge_invalid_high_bits);
    RUN_TEST(test_bms_monitor_api_set_discharge_valid);
    RUN_TEST(test_bms_monitor_api_set_discharge_dcc);
    RUN_TEST(test_bms_monitor_api_set_discharge_dcto_off);
    RUN_TEST(test_bms_monitor_api_set_discharge_dcto_30s);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_get_discharge Test for bms_monitor_api_get_discharge function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_get_discharge);
    RUN_TEST(test_bms_monitor_api_get_discharge_zero);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_write_configuration Test for bms_monitor_api_write_configuration function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_write_configuration_ok);
    RUN_TEST(test_bms_monitor_api_write_configuration_send_error);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_read_configuration Test for bms_monitor_api_read_configuration function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_read_configuration_decode_error);
    RUN_TEST(test_bms_monitor_api_read_configuration_send_receive_error);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_start_volt_covertion Test for bms_monitor_api_start_volt_covertion function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_start_volt_conversion_ok);
    RUN_TEST(test_bms_monitor_api_start_volt_conversion_send_error);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_read_voltages Test for bms_monitor_api_read_voltages function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_read_voltages_decode_error);
    RUN_TEST(test_bms_monitor_api_read_voltages_send_receive_error);

    /*! \} */

    return UNITY_END();
}
