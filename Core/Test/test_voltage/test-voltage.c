/*!
 * \file            test-voltage.c
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the voltage module.
 */

#include "unity.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "fff.h"

#include "voltage.h"
#include "voltage-api.h"

extern struct VoltageHandler voltage_handler;
#include "types.h"
#include "defines.h"
#include "can-communication-api.h"
#include "can-primary.h"
#include "can-primary-api.h"
#include "eagletrt-api.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, mock_can_send, const struct CanCommunicationFrame *);
FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, mock_on_receive, const struct CanCommunicationFrame *);

#define TEST_MAX_CAPTURED_FRAMES (4U)

EAGLETRT_STATIC struct CanCommunicationFrame test_captured_frames[TEST_MAX_CAPTURED_FRAMES];
EAGLETRT_STATIC uint32_t test_captured_count;

EAGLETRT_STATIC enum CanCommunicationReturnCode prv_capture_send(const struct CanCommunicationFrame *frame) {
    if (frame != NULL && test_captured_count < TEST_MAX_CAPTURED_FRAMES) {
        test_captured_frames[test_captured_count] = *frame;
        test_captured_count++;
    }
    return CAN_COMMUNICATION_RC_OK;
}

EAGLETRT_STATIC void prv_flush_primary(void) {
    EAGLETRT_API_UNUSED(can_communication_api_process_tx(CAN_COMMUNICATION_NETWORK_PRIMARY));
}

void setUp(void) {
    RESET_FAKE(mock_can_send);
    RESET_FAKE(mock_on_receive);
    FFF_RESET_HISTORY();

    mock_can_send_fake.custom_fake = prv_capture_send;
    mock_on_receive_fake.return_val = CAN_COMMUNICATION_RC_OK;

    memset(test_captured_frames, 0, sizeof(test_captured_frames));
    test_captured_count = 0U;

    struct CanCommunicationNetworkConfig configs[CAN_COMMUNICATION_NETWORK_COUNT];
    for (size_t i = 0; i < CAN_COMMUNICATION_NETWORK_COUNT; i++) {
        configs[i].send = mock_can_send;
        configs[i].on_receive = mock_on_receive;
        configs[i].cs_enter = NULL;
        configs[i].cs_exit = NULL;
    }
    can_communication_api_init(configs);

    voltage_api_init();
}

void tearDown(void) {
}

/*!
 * \defgroup        voltage_api_init Test for voltage_api_init function.
 * \{
 */

void check_voltage_api_init(void) {
    volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 0 };
    memset(voltage_handler.voltages, 0xFF, DEFINES_CELLS_SERIES_COUNT * sizeof(*voltage_handler.voltages));
    voltage_api_init();
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(voltages, voltage_handler.voltages, DEFINES_CELLS_SERIES_COUNT);
}

/*! \} */

/*!
 * \defgroup        voltage_api_update_voltage Test for voltage_api_update_voltage function.
 * \{
 */

void check_voltage_api_update_voltage_with_valid_parameters(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OK, voltage_api_update_voltage(0U, 7.F), "voltage_api_update_voltage failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(7.F, voltage_handler.voltages[0U], "Stored voltage is different!");
}

void check_voltage_api_update_voltage_when_index_is_out_of_bounds(void) {
    volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_update_voltage(DEFINES_CELLS_SERIES_COUNT, 7.F), "voltage_api_update_voltage returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_CELLS_SERIES_COUNT, "Previously stored values have been modified!");
}

/*! \} */

/*!
 * \defgroup        voltage_api_update_voltages Test for voltage_api_update_voltages function.
 * \{
 */

void check_voltage_api_update_voltages_with_valid_parameters(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OK, voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT), "voltage_api_update_voltages failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_CELLS_SERIES_COUNT, "Stored voltages are different!");
}

void check_voltage_api_update_voltages_with_null_voltages(void) {
    volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_NULL_POINTER, voltage_api_update_voltages(0U, NULL, DEFINES_CELLS_SERIES_COUNT), "voltage_api_update_voltages returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_CELLS_SERIES_COUNT, "Previously stored values have been modified!");
}

void check_voltage_api_update_voltages_when_index_is_out_of_bounds(void) {
    const volt VOLTAGES[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_update_voltages(DEFINES_CELLS_SERIES_COUNT, VOLTAGES, DEFINES_CELLS_SERIES_COUNT), "voltage_api_update_voltages returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_CELLS_SERIES_COUNT, "Previously stored values have been modified!");
}

void check_voltage_api_update_voltages_when_size_is_too_big(void) {
#define COUNT (9U)
    const volt VOLTAGES[COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F, 3.8F, 3.9F, 4.F };
    volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_update_voltages(0U, VOLTAGES, COUNT), "voltage_api_update_voltages returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(voltages, voltage_handler.voltages, DEFINES_CELLS_SERIES_COUNT, "Previously stored values have been modified!");
#undef COUNT
}

/*! \} */

/*!
 * \defgroup        voltage_api_get_min Test for voltage_api_get_min function.
 * \{
 */

void check_voltage_api_get_min(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    memcpy(voltage_handler.voltages, voltages, DEFINES_CELLS_SERIES_COUNT * sizeof(*voltages));
    TEST_ASSERT_EQUAL_FLOAT(voltages[0U], voltage_api_get_min());
}

/*! \} */

/*!
 * \defgroup        voltage_api_get_max Test for voltage_api_get_max function.
 * \{
 */

void check_voltage_api_get_max(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    memcpy(voltage_handler.voltages, voltages, DEFINES_CELLS_SERIES_COUNT * sizeof(*voltages));
    TEST_ASSERT_EQUAL_FLOAT(voltages[5U], voltage_api_get_max());
}

/*! \} */

/*!
 * \defgroup        voltage_api_get_average Test for voltage_api_get_average function.
 * \{
 */

void check_voltage_api_get_average(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    memcpy(voltage_handler.voltages, voltages, DEFINES_CELLS_SERIES_COUNT * sizeof(*voltages));
    TEST_ASSERT_EQUAL_FLOAT(3.45F, voltage_api_get_average());
}

/*! \} */

/*!
 * \defgroup        voltage_api_get_sum Test for voltage_api_get_sum function.
 * \{
 */

void check_voltage_api_get_sum(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.2F, 3.3F, 3.4F, 3.5F, 3.6F, 3.7F };
    memcpy(voltage_handler.voltages, voltages, DEFINES_CELLS_SERIES_COUNT * sizeof(*voltages));
    TEST_ASSERT_EQUAL_FLOAT(20.7F, voltage_api_get_sum());
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
    TEST_ASSERT_EQUAL_INT(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_dump_voltages(dump, DEFINES_CELLS_SERIES_COUNT, COUNT));
#undef COUNT
}

void check_voltage_api_dump_voltages_when_size_is_too_big(void) {
#define COUNT (8U)
    volt dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(VOLTAGE_RC_OUT_OF_BOUNDS, voltage_api_dump_voltages(dump, 0U, COUNT));
#undef COUNT
}

/*! \} */

/*!
 * \defgroup        voltage_api_periodically_send_cell_voltages Tests for periodically_send_cell_voltages
 * \{
 */

void check_periodically_send_cell_voltages_returns_ok(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.1F, 3.2F, 3.3F, 3.4F, 3.5F, 3.6F };
    memcpy(voltage_handler.voltages, voltages, sizeof(voltages));

    enum VoltageReturnCode rc = voltage_api_periodically_send_cell_voltages(can_primary_cycle_time_lvaccellvoltage);
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OK, rc, "Periodic send must return OK.");
}

void check_periodically_send_cell_voltages_does_not_send_before_cycle(void) {
    voltage_api_periodically_send_cell_voltages(can_primary_cycle_time_lvaccellvoltage - 1);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, test_captured_count, "No frame must be emitted before the cycle time has elapsed.");
}

void check_periodically_send_cell_voltages_sends_once_cycle_elapsed(void) {
    voltage_api_periodically_send_cell_voltages(can_primary_cycle_time_lvaccellvoltage);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, test_captured_count, "One frame must be emitted once the cycle time has elapsed.");
}

void check_periodically_send_cell_voltages_does_not_resend_within_same_cycle(void) {
    voltage_api_periodically_send_cell_voltages(can_primary_cycle_time_lvaccellvoltage);
    voltage_api_periodically_send_cell_voltages(can_primary_cycle_time_lvaccellvoltage + 1);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, test_captured_count, "A second call within the same cycle must not emit another frame.");
}

void check_periodically_send_cell_voltages_frame_id(void) {
    voltage_api_periodically_send_cell_voltages(can_primary_cycle_time_lvaccellvoltage);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACCELLVOLTAGE, test_captured_frames[0].id, "The frame must be LvacCellVoltage.");
}

void check_periodically_send_cell_voltages_encodes_payload(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.1F, 3.2F, 3.3F, 3.4F, 3.5F, 3.6F };
    memcpy(voltage_handler.voltages, voltages, sizeof(voltages));

    voltage_api_periodically_send_cell_voltages(can_primary_cycle_time_lvaccellvoltage);
    prv_flush_primary();

    union CanPrimaryMessages msg = { 0 };
    EAGLETRT_API_UNUSED(can_primary_api_deserialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACCELLVOLTAGE, test_captured_frames[0].data, &msg));

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.1F, msg.lvaccellvoltage.voltage1, "voltage1 must match cell 0.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.2F, msg.lvaccellvoltage.voltage2, "voltage2 must match cell 1.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.3F, msg.lvaccellvoltage.voltage3, "voltage3 must match cell 2.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.4F, msg.lvaccellvoltage.voltage4, "voltage4 must match cell 3.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.5F, msg.lvaccellvoltage.voltage5, "voltage5 must match cell 4.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.6F, msg.lvaccellvoltage.voltage6, "voltage6 must match cell 5.");
}

/*! \} */

/*!
 * \defgroup        voltage_api_periodically_send_voltage_information Tests for periodically_send_voltage_information
 * \{
 */

void check_periodically_send_voltage_information_returns_ok(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.1F, 3.2F, 3.3F, 3.4F, 3.5F, 3.6F };
    memcpy(voltage_handler.voltages, voltages, sizeof(voltages));

    enum VoltageReturnCode rc = voltage_api_periodically_send_voltage_information(can_primary_cycle_time_lvacvoltageinfo);
    TEST_ASSERT_EQUAL_INT_MESSAGE(VOLTAGE_RC_OK, rc, "Periodic send must return OK.");
}

void check_periodically_send_voltage_information_does_not_send_before_cycle(void) {
    voltage_api_periodically_send_voltage_information(can_primary_cycle_time_lvacvoltageinfo - 1);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, test_captured_count, "No frame must be emitted before the cycle time has elapsed.");
}

void check_periodically_send_voltage_information_sends_once_cycle_elapsed(void) {
    voltage_api_periodically_send_voltage_information(can_primary_cycle_time_lvacvoltageinfo);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, test_captured_count, "One frame must be emitted once the cycle time has elapsed.");
}

void check_periodically_send_voltage_information_does_not_resend_within_same_cycle(void) {
    voltage_api_periodically_send_voltage_information(can_primary_cycle_time_lvacvoltageinfo);
    voltage_api_periodically_send_voltage_information(can_primary_cycle_time_lvacvoltageinfo + 1);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, test_captured_count, "A second call within the same cycle must not emit another frame.");
}

void check_periodically_send_voltage_information_frame_id(void) {
    voltage_api_periodically_send_voltage_information(can_primary_cycle_time_lvacvoltageinfo);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVOLTAGEINFO, test_captured_frames[0].id, "The frame must be LvacVoltageInfo.");
}

void check_periodically_send_voltage_information_encodes_payload(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.1F, 3.2F, 3.3F, 3.4F, 3.5F, 3.6F };
    memcpy(voltage_handler.voltages, voltages, sizeof(voltages));

    voltage_api_periodically_send_voltage_information(can_primary_cycle_time_lvacvoltageinfo);
    prv_flush_primary();

    union CanPrimaryMessages msg = { 0 };
    EAGLETRT_API_UNUSED(can_primary_api_deserialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVOLTAGEINFO, test_captured_frames[0].data, &msg));

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(20.1F, msg.lvacvoltageinfo.total, "Total voltage must be the sum of all cells.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.1F, msg.lvacvoltageinfo.min, "Min voltage must be the lowest cell.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.6F, msg.lvacvoltageinfo.max, "Max voltage must be the highest cell.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.35F, msg.lvacvoltageinfo.average, "Average voltage must be the mean of all cells.");
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*!
     * \addtogroup        voltage_api_init
     * \{
     */

    RUN_TEST(check_voltage_api_init);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_update_voltage
     * \{
     */

    RUN_TEST(check_voltage_api_update_voltage_with_valid_parameters);
    RUN_TEST(check_voltage_api_update_voltage_when_index_is_out_of_bounds);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_update_voltages
     * \{
     */

    RUN_TEST(check_voltage_api_update_voltages_with_valid_parameters);
    RUN_TEST(check_voltage_api_update_voltages_with_null_voltages);
    RUN_TEST(check_voltage_api_update_voltages_when_index_is_out_of_bounds);
    RUN_TEST(check_voltage_api_update_voltages_when_size_is_too_big);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_get_min
     * \{
     */

    RUN_TEST(check_voltage_api_get_min);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_get_max
     * \{
     */

    RUN_TEST(check_voltage_api_get_max);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_get_average
     * \{
     */

    RUN_TEST(check_voltage_api_get_average);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_get_sum
     * \{
     */

    RUN_TEST(check_voltage_api_get_sum);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_dump_voltages
     * \{
     */

    RUN_TEST(check_voltage_api_dump_voltages_with_valid_parameters);
    RUN_TEST(check_voltage_api_dump_voltages_with_null_out);
    RUN_TEST(check_voltage_api_dump_voltages_when_start_is_out_of_bounds);
    RUN_TEST(check_voltage_api_dump_voltages_when_size_is_too_big);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_periodically_send_cell_voltages
     * \{
     */

    RUN_TEST(check_periodically_send_cell_voltages_returns_ok);
    RUN_TEST(check_periodically_send_cell_voltages_does_not_send_before_cycle);
    RUN_TEST(check_periodically_send_cell_voltages_sends_once_cycle_elapsed);
    RUN_TEST(check_periodically_send_cell_voltages_does_not_resend_within_same_cycle);
    RUN_TEST(check_periodically_send_cell_voltages_frame_id);
    RUN_TEST(check_periodically_send_cell_voltages_encodes_payload);

    /*! \} */

    /*!
     * \addtogroup        voltage_api_periodically_send_voltage_information
     * \{
     */

    RUN_TEST(check_periodically_send_voltage_information_returns_ok);
    RUN_TEST(check_periodically_send_voltage_information_does_not_send_before_cycle);
    RUN_TEST(check_periodically_send_voltage_information_sends_once_cycle_elapsed);
    RUN_TEST(check_periodically_send_voltage_information_does_not_resend_within_same_cycle);
    RUN_TEST(check_periodically_send_voltage_information_frame_id);
    RUN_TEST(check_periodically_send_voltage_information_encodes_payload);

    /*! \} */

    UNITY_END();
}
