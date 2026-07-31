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

#include "current-api.h"
#include "temperature-api.h"

/* FFF mocks for SPI callbacks passed to init */
FAKE_VALUE_FUNC(enum BmsMonitorReturnCode, send_fake, uint8_t *const, const size_t);
FAKE_VALUE_FUNC(enum BmsMonitorReturnCode, send_receive_fake, uint8_t *const, uint8_t *, const size_t, const size_t);
FAKE_VALUE_FUNC(enum BmsMonitorReturnCode, ntc_read_fake, size_t, raw_ampere *);

extern struct BmsMonitorHandler bms_monitor_handler;
extern bool prv_bms_monitor_api_is_cells_bitmask_valid(uint8_t cells);

void setUp(void) {
    RESET_FAKE(send_fake);
    RESET_FAKE(send_receive_fake);
    RESET_FAKE(ntc_read_fake);
    memset(&bms_monitor_handler, 0, sizeof(bms_monitor_handler));
    current_api_init();
    temperature_api_init();
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
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_NULL_POINTER, bms_monitor_api_init(NULL, NULL, NULL), "both NULL not detected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_NULL_POINTER, bms_monitor_api_init(send_fake, NULL, NULL), "send_receive NULL not detected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_NULL_POINTER, bms_monitor_api_init(NULL, send_receive_fake, NULL), "send NULL not detected");
}

void test_bms_monitor_api_init_ok(void) {
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_OK, bms_monitor_api_init(send_fake, send_receive_fake, NULL));
}

void test_bms_monitor_api_init_config(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    TEST_ASSERT_EQUAL(1U, bms_monitor_handler.requested_configuration.REFON);
}

void test_bms_monitor_api_init_send(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    TEST_ASSERT_EQUAL_PTR(send_fake, bms_monitor_handler.send);
}

void test_bms_monitor_api_init_send_receive(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    TEST_ASSERT_EQUAL_PTR(send_receive_fake, bms_monitor_handler.send_receive);
}

void test_bms_monitor_api_init_ltc_handler(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    TEST_ASSERT_EQUAL(DEFINES_LTC_COUNT, bms_monitor_handler.ltc_handler.count);
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_write_configuration Test for bms_monitor_api_write_configuration function.
 * \{
 */

void test_bms_monitor_api_write_configuration_ok(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_OK, bms_monitor_api_write_configuration());
}

void test_bms_monitor_api_write_configuration_send_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_write_configuration());
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_read_configuration Test for bms_monitor_api_read_configuration function.
 * \{
 */

void test_bms_monitor_api_read_configuration_decode_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_DECODE_ERROR, bms_monitor_api_read_configuration());
}

void test_bms_monitor_api_read_configuration_send_receive_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_read_configuration());
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_start_volt_covertion Test for bms_monitor_api_start_volt_covertion function.
 * \{
 */

void test_bms_monitor_api_start_volt_conversion_ok(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_OK, bms_monitor_api_start_volt_covertion());
}

void test_bms_monitor_api_start_volt_conversion_send_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_start_volt_covertion());
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_start_open_wire_covertion Test for bms_monitor_api_start_open_wire_covertion function.
 * \{
 */

void test_bms_monitor_api_start_open_wire_covertion_ok(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_OK, bms_monitor_api_start_open_wire_covertion(LTC6810_2_PUP_INACTIVE));
}

void test_bms_monitor_api_start_open_wire_covertion_send_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_start_open_wire_covertion(LTC6810_2_PUP_INACTIVE));
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_read_voltages Test for bms_monitor_api_read_voltages function.
 * \{
 */

void test_bms_monitor_api_read_voltages_decode_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_DECODE_ERROR,
                      bms_monitor_api_read_voltages(BMS_MONITOR_VOLTAGE_REGISTER_A));
}

void test_bms_monitor_api_read_voltages_send_receive_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR, bms_monitor_api_read_voltages(BMS_MONITOR_VOLTAGE_REGISTER_A));
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_read_currents Test for bms_monitor_api_read_currents function.
 * \{
 */

void test_bms_monitor_api_read_currents_null_callback(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_NULL_POINTER, bms_monitor_api_read_currents(), "no callback set not detected");
}

static enum BmsMonitorReturnCode ntc_read_fake_custom(size_t channel, raw_ampere *raw) {
    (void)channel;
    *raw = 512U; /* mid-scale 10-bit ADC value */
    return BMS_MONITOR_RC_OK;
}

void test_bms_monitor_api_read_currents_ok(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, ntc_read_fake);
    ntc_read_fake_fake.custom_fake = ntc_read_fake_custom;

    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_read_currents(), "read currents failed");
    TEST_ASSERT_EQUAL_MESSAGE(DEFINES_NTC_COUNT, ntc_read_fake_fake.call_count, "callback not called for each NTC");

    /* raw=512 → 512 * VDD / (2^RES - 1) */
    ampere expected = 512.0F * (DEFINES_NTC_VDD / (float)((1U << BMS_MONITOR_ADC_RESOLUTION) - 1U));
    ampere currents[DEFINES_NTC_COUNT];
    current_api_dump_currents(currents, 0U, DEFINES_NTC_COUNT);
    for (size_t i = 0U; i < DEFINES_NTC_COUNT; ++i) {
        TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, currents[i], "current not stored correctly");
    }

    /* output_current = sum of all NTC currents = 16 * expected */
    ampere output = current_api_get_output_current();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected * (float)DEFINES_NTC_COUNT, output, "output current not computed");
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_read_temperatures Test for bms_monitor_api_read_temperatures function.
 * \{
 */

void test_bms_monitor_api_read_temperatures_ok(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);

    /* Pre-populate currents in current module */
    ampere currents[DEFINES_NTC_COUNT];
    for (size_t i = 0U; i < DEFINES_NTC_COUNT; ++i) {
        currents[i] = 0.005F;
    }
    current_api_update_currents(0U, currents, DEFINES_NTC_COUNT);

    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_read_temperatures(), "read temperatures failed");

    celsius temps[DEFINES_NTC_COUNT];
    temperature_api_dump_temperatures(temps, 0U, DEFINES_NTC_COUNT);
    for (size_t i = 0U; i < DEFINES_NTC_COUNT; ++i) {
        TEST_ASSERT_FALSE_MESSAGE(temps[i] == 0.0F, "temperature not stored correctly");
    }
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_read_open_wire_voltages Test for bms_monitor_api_read_open_wire_voltages function.
 * \{
 */

void test_bms_monitor_api_read_open_wire_voltages_decode_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_OK;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_DECODE_ERROR,
                      bms_monitor_api_read_open_wire_voltages(BMS_MONITOR_VOLTAGE_REGISTER_A, BMS_MANAGER_OPEN_WIRE_OPERATION_PUP));
}

void test_bms_monitor_api_read_open_wire_voltages_send_receive_error(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    send_receive_fake_fake.return_val = BMS_MONITOR_RC_COMMUNICATION_ERROR;
    TEST_ASSERT_EQUAL(BMS_MONITOR_RC_COMMUNICATION_ERROR,
                      bms_monitor_api_read_open_wire_voltages(BMS_MONITOR_VOLTAGE_REGISTER_A, BMS_MANAGER_OPEN_WIRE_OPERATION_PUP));
}

/*! \} */

/*!
 * \defgroup		bms_monitor_api_set_discharge Test for bms_monitor_api_set_discharge function.
 * \{
 */

void test_bms_monitor_api_set_discharge_invalid_adjacent(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b00000011U), "cells 0,1 adjacent not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b00000110U), "cells 1,2 adjacent not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b00011000U), "cells 3,4 adjacent not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b00110000U), "cells 4,5 adjacent not rejected");
}

void test_bms_monitor_api_set_discharge_invalid_high_bits(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b01000000U), "bit 6 not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b10000000U), "bit 7 not rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_INVALID_ARGUMENT, bms_monitor_api_set_discharge(0b11000000U), "bits 6,7 not rejected");
}

void test_bms_monitor_api_set_discharge_valid(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00000000U), "no cells rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00000001U), "single cell 0 rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00000100U), "single cell 2 rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00010000U), "single cell 4 rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00010101U), "cells 0,2,4 non-adjacent rejected");
    TEST_ASSERT_EQUAL_MESSAGE(BMS_MONITOR_RC_OK, bms_monitor_api_set_discharge(0b00101010U), "cells 1,3,5 non-adjacent rejected");
}

void test_bms_monitor_api_set_discharge_dcc(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    bms_monitor_api_set_discharge(0b00010101U);
    TEST_ASSERT_EQUAL(0b00010101U, bms_monitor_handler.requested_configuration.DCC);
}

void test_bms_monitor_api_set_discharge_dcto_off(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    bms_monitor_api_set_discharge(0b00000000U);
    TEST_ASSERT_EQUAL(LTC6810_2_DCTO_OFF, bms_monitor_handler.requested_configuration.DCTO);
}

void test_bms_monitor_api_set_discharge_dcto_30s(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
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
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    bms_monitor_handler.actual_configuration.DCC = 0b00010101U;
    TEST_ASSERT_EQUAL(0b00010101U, bms_monitor_api_get_discharge());
}

void test_bms_monitor_api_get_discharge_zero(void) {
    bms_monitor_api_init(send_fake, send_receive_fake, NULL);
    bms_monitor_handler.actual_configuration.DCC = 0b00000000U;
    TEST_ASSERT_EQUAL(0b00000000U, bms_monitor_api_get_discharge());
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
	 * \defgroup	 bms_monitor_api_start_open_wire_covertion Test for start_open_wire_covertion function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_start_open_wire_covertion_ok);
    RUN_TEST(test_bms_monitor_api_start_open_wire_covertion_send_error);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_read_voltages Test for bms_monitor_api_read_voltages function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_read_voltages_decode_error);
    RUN_TEST(test_bms_monitor_api_read_voltages_send_receive_error);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_read_currents Test for read_currents function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_read_currents_null_callback);
    RUN_TEST(test_bms_monitor_api_read_currents_ok);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_read_temperatures Test for read_temperatures function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_read_temperatures_ok);

    /*! \} */

    /*!
	 * \defgroup	 bms_monitor_api_read_open_wire_voltages Test for read_open_wire_voltages function.
	 * \{
	 */

    RUN_TEST(test_bms_monitor_api_read_open_wire_voltages_decode_error);
    RUN_TEST(test_bms_monitor_api_read_open_wire_voltages_send_receive_error);

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

    return UNITY_END();
}
