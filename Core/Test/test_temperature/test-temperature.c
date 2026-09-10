/*!
 * \file            test-temperature.c
 * \date            2026-03-31
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the temperature module.
 */

#include "unity.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "fff.h"

#include "temperature.h"
#include "temperature-api.h"

extern struct TemperatureHandler temperature_handler;
#include "types.h"
#include "defines.h"
#include "can-communication-api.h"
#include "can-primary.h"
#include "can-primary-api.h"
#include "eagletrt-api.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, mock_can_send, const struct CanCommunicationFrame *);
FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, mock_on_receive, const struct CanCommunicationFrame *);

#define TEST_MAX_CAPTURED_FRAMES (8U)

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

    temperature_api_init();
}

void tearDown(void) {
}

/*!
 * \defgroup        temperature_api_init Test for temperature_api_init function.
 * \{
 */

void check_temperature_api_init(void) {
    celsius temperatures[DEFINES_CELLS_NTC_COUNT] = { 0 };
    memset(temperature_handler.temperatures, 0xFF, DEFINES_CELLS_NTC_COUNT * sizeof(*temperature_handler.temperatures));
    temperature_api_init();
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(temperatures, temperature_handler.temperatures, DEFINES_CELLS_NTC_COUNT);
}

/*! \} */

/*!
 * \defgroup        temperature_api_update_temperature Test for temperature_api_update_temperature function.
 * \{
 */

void check_temperature_api_update_temperature_with_valid_parameters(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OK, temperature_api_update_temperature(0U, 7.F), "temperature_api_update_temperature failed!");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(7.F, temperature_handler.temperatures[0U], "Stored temperature is different!");
}

void check_temperature_api_update_temperature_when_index_is_out_of_bounds(void) {
    celsius temperatures[DEFINES_CELLS_NTC_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_update_temperature(DEFINES_CELLS_NTC_COUNT, 7.F), "temperature_api_update_temperature returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFINES_CELLS_NTC_COUNT, "Previously stored values have been modified!");
}

/*! \} */

/*!
 * \defgroup        temperature_api_update_temperatures Test for temperature_api_update_temperatures function.
 * \{
 */

void check_temperature_api_update_temperatures_with_valid_parameters(void) {
    const celsius temperatures[DEFINES_CELLS_SERIES_COUNT] = { 1.F, 2.F, 3.F, 4.F, 5.F, 6.F };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OK, temperature_api_update_temperatures(0U, temperatures, DEFINES_CELLS_SERIES_COUNT), "temperature_api_update_temperatures failed!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFINES_CELLS_SERIES_COUNT, "Stored temperatures are different!");
}

void check_temperature_api_update_temperatures_with_null_temperatures(void) {
    celsius temperatures[DEFINES_CELLS_NTC_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_NULL_POINTER, temperature_api_update_temperatures(0U, NULL, DEFINES_CELLS_SERIES_COUNT), "temperature_api_update_temperatures returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFINES_CELLS_NTC_COUNT, "Previously stored values have been modified!");
}

void check_temperature_api_update_temperatures_when_index_is_out_of_bounds(void) {
    const celsius mock_temperatures[DEFINES_CELLS_SERIES_COUNT] = { 1.F, 2.F, 3.F, 4.F, 5.F, 6.F };
    celsius temperatures[DEFINES_CELLS_NTC_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_update_temperatures(DEFINES_CELLS_NTC_COUNT, mock_temperatures, DEFINES_CELLS_SERIES_COUNT), "temperature_api_update_temperatures returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFINES_CELLS_NTC_COUNT, "Previously stored values have been modified!");
}

void check_temperature_api_update_temperatures_when_size_is_too_big(void) {
#define SIZE (DEFINES_CELLS_NTC_COUNT + 1U)
    const celsius mock_temperatures[1U] = { 1.F };
    celsius temperatures[DEFINES_CELLS_NTC_COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_update_temperatures(0U, mock_temperatures, SIZE), "temperature_api_update_temperatures returned a different value!");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(temperatures, temperature_handler.temperatures, DEFINES_CELLS_NTC_COUNT, "Previously stored values have been modified!");
#undef SIZE
}

/*! \} */

/*!
 * \defgroup        temperature_api_get_min Test for temperature_api_get_min function.
 * \{
 */

void check_temperature_api_get_min(void) {
    const celsius temperatures[DEFINES_CELLS_NTC_COUNT] = {
        1.F, 2.F, 3.F, 4.F, 5.F, 6.F, 7.F, 8.F, 9.F, 10.F, 11.F, 12.F
    };
    memcpy(temperature_handler.temperatures, temperatures, DEFINES_CELLS_NTC_COUNT * sizeof(*temperatures));
    TEST_ASSERT_EQUAL_FLOAT(1.F, temperature_api_get_min());
}

/*! \} */

/*!
 * \defgroup        temperature_api_get_max Test for temperature_api_get_max function.
 * \{
 */

void check_temperature_api_get_max(void) {
    const celsius temperatures[DEFINES_CELLS_SERIES_COUNT] = { 1.F, 2.F, 3.F, 4.F, 5.F, 6.F };
    memcpy(temperature_handler.temperatures, temperatures, DEFINES_CELLS_SERIES_COUNT * sizeof(*temperatures));
    TEST_ASSERT_EQUAL_FLOAT(temperatures[5U], temperature_api_get_max());
}

/*! \} */

/*!
 * \defgroup        temperature_api_get_average Test for temperature_api_get_average function.
 * \{
 */

void check_temperature_api_get_average(void) {
    const celsius temperatures[DEFINES_CELLS_NTC_COUNT] = {
        1.F, 2.F, 3.F, 4.F, 5.F, 6.F, 7.F, 8.F, 9.F, 10.F, 11.F, 12.F
    };
    memcpy(temperature_handler.temperatures, temperatures, DEFINES_CELLS_NTC_COUNT * sizeof(*temperatures));
    TEST_ASSERT_EQUAL_FLOAT(6.5F, temperature_api_get_average());
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
    TEST_ASSERT_EQUAL_INT(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_dump_temperatures(dump, DEFINES_CELLS_NTC_COUNT, COUNT));
#undef COUNT
}

void check_temperature_api_dump_temperatures_when_size_is_too_big(void) {
#define COUNT (3U)
    celsius dump[COUNT] = { 0 };
    TEST_ASSERT_EQUAL_INT(TEMPERATURE_RC_OUT_OF_BOUNDS, temperature_api_dump_temperatures(dump, DEFINES_CELLS_NTC_COUNT, COUNT));
#undef COUNT
}

/*! \} */

/*!
 * \defgroup        temperature_api_periodically_send_temperatures Tests for periodically_send_temperatures
 * \{
 */

void check_periodically_send_temperatures_returns_ok(void) {
    const celsius temperatures[DEFINES_CELLS_NTC_COUNT] = {
        10.F, 20.F, 30.F, 40.F, 50.F, 60.F, 70.F, 80.F, 90.F, 100.F, 110.F, 120.F
    };
    memcpy(temperature_handler.temperatures, temperatures, sizeof(temperatures));

    enum TemperatureReturnCode rc = temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    TEST_ASSERT_EQUAL_INT_MESSAGE(TEMPERATURE_RC_OK, rc, "Periodic send must return OK.");
}

void check_periodically_send_temperatures_does_not_send_before_cycle(void) {
    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature - 1);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, test_captured_count, "No frame must be emitted before the cycle time has elapsed.");
}

void check_periodically_send_temperatures_sends_once_cycle_elapsed(void) {
    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(3U, test_captured_count, "Three frames (2 mux + info) must be emitted once the cycle time has elapsed.");
}

void check_periodically_send_temperatures_does_not_resend_within_same_cycle(void) {
    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature + 1);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(3U, test_captured_count, "A second call within the same cycle must not emit additional frames.");
}

void check_periodically_send_temperatures_first_frame_is_mux_group_0(void) {
    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACTEMPERATURE, test_captured_frames[0].id, "The first frame must be LvacTemperature.");
}

void check_periodically_send_temperatures_second_frame_is_mux_group_1(void) {
    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACTEMPERATURE, test_captured_frames[1].id, "The second frame must be LvacTemperature.");
}

void check_periodically_send_temperatures_third_frame_is_info(void) {
    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACTEMPERATUREINFO, test_captured_frames[2].id, "The third frame must be LvacTemperatureInfo.");
}

void check_periodically_send_temperatures_encodes_mux_group_0(void) {
    const celsius temperatures[DEFINES_CELLS_NTC_COUNT] = {
        2.5F, 2.8F, 3.0F, 3.3F, 3.6F, 4.0F, 2.5F, 2.8F, 3.0F, 3.3F, 3.6F, 4.0F
    };
    memcpy(temperature_handler.temperatures, temperatures, sizeof(temperatures));

    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    prv_flush_primary();

    union CanPrimaryMessages msg = { 0 };
    EAGLETRT_API_UNUSED(can_primary_api_deserialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACTEMPERATURE, test_captured_frames[0].data, &msg));

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, msg.lvactemperature.group, "Mux group must be 0 for the first frame.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(2.5F, msg.lvactemperature.group_payload.mux_0.voltage1, "voltage1 must match NTC 0.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(2.8F, msg.lvactemperature.group_payload.mux_0.voltage2, "voltage2 must match NTC 1.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.0F, msg.lvactemperature.group_payload.mux_0.voltage3, "voltage3 must match NTC 2.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.3F, msg.lvactemperature.group_payload.mux_0.voltage4, "voltage4 must match NTC 3.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.6F, msg.lvactemperature.group_payload.mux_0.voltage5, "voltage5 must match NTC 4.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(4.0F, msg.lvactemperature.group_payload.mux_0.voltage6, "voltage6 must match NTC 5.");
}

void check_periodically_send_temperatures_encodes_mux_group_1(void) {
    const celsius temperatures[DEFINES_CELLS_NTC_COUNT] = {
        2.5F, 2.8F, 3.0F, 3.3F, 3.6F, 4.0F, 2.5F, 2.8F, 3.0F, 3.3F, 3.6F, 4.0F
    };
    memcpy(temperature_handler.temperatures, temperatures, sizeof(temperatures));

    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    prv_flush_primary();

    union CanPrimaryMessages msg = { 0 };
    EAGLETRT_API_UNUSED(can_primary_api_deserialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACTEMPERATURE, test_captured_frames[1].data, &msg));

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1U, msg.lvactemperature.group, "Mux group must be 1 for the second frame.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(2.5F, msg.lvactemperature.group_payload.mux_1.voltage7, "voltage7 must match NTC 6.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(2.8F, msg.lvactemperature.group_payload.mux_1.voltage8, "voltage8 must match NTC 7.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.0F, msg.lvactemperature.group_payload.mux_1.voltage9, "voltage9 must match NTC 8.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.3F, msg.lvactemperature.group_payload.mux_1.voltage10, "voltage10 must match NTC 9.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(3.6F, msg.lvactemperature.group_payload.mux_1.voltage11, "voltage11 must match NTC 10.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(4.0F, msg.lvactemperature.group_payload.mux_1.voltage12, "voltage12 must match NTC 11.");
}

void check_periodically_send_temperatures_encodes_info(void) {
    const celsius temperatures[DEFINES_CELLS_NTC_COUNT] = {
        10.F, 20.F, 30.F, 40.F, 50.F, 60.F, 70.F, 80.F, 90.F, 100.F, 110.F, 120.F
    };
    memcpy(temperature_handler.temperatures, temperatures, sizeof(temperatures));

    temperature_api_periodically_send_temperatures(can_primary_cycle_time_lvactemperature);
    prv_flush_primary();

    union CanPrimaryMessages msg = { 0 };
    EAGLETRT_API_UNUSED(can_primary_api_deserialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACTEMPERATUREINFO, test_captured_frames[2].data, &msg));

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(10.F, msg.lvactemperatureinfo.min, "Min temperature must be encoded.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(120.F, msg.lvactemperatureinfo.max, "Max temperature must be encoded.");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(65.F, msg.lvactemperatureinfo.average, "Average temperature must be encoded.");
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*!
     * \addtogroup        temperature_api_init
     * \{
     */

    RUN_TEST(check_temperature_api_init);

    /*! \} */

    /*!
     * \addtogroup        temperature_api_update_temperature
     * \{
     */

    RUN_TEST(check_temperature_api_update_temperature_with_valid_parameters);
    RUN_TEST(check_temperature_api_update_temperature_when_index_is_out_of_bounds);

    /*! \} */

    /*!
     * \addtogroup        temperature_api_update_temperatures
     * \{
     */

    RUN_TEST(check_temperature_api_update_temperatures_with_valid_parameters);
    RUN_TEST(check_temperature_api_update_temperatures_with_null_temperatures);
    RUN_TEST(check_temperature_api_update_temperatures_when_index_is_out_of_bounds);
    RUN_TEST(check_temperature_api_update_temperatures_when_size_is_too_big);

    /*! \} */

    /*!
     * \addtogroup        temperature_api_get_min
     * \{
     */

    RUN_TEST(check_temperature_api_get_min);

    /*! \} */

    /*!
     * \addtogroup        temperature_api_get_max
     * \{
     */

    RUN_TEST(check_temperature_api_get_max);

    /*! \} */

    /*!
     * \addtogroup        temperature_api_get_average
     * \{
     */

    RUN_TEST(check_temperature_api_get_average);

    /*! \} */

    /*!
     * \addtogroup        temperature_api_dump_temperatures
     * \{
     */

    RUN_TEST(check_temperature_api_dump_temperatures_with_valid_parameters);
    RUN_TEST(check_temperature_api_dump_temperatures_with_null_out);
    RUN_TEST(check_temperature_api_dump_temperatures_when_start_is_out_of_bounds);
    RUN_TEST(check_temperature_api_dump_temperatures_when_size_is_too_big);

    /*! \} */

    /*!
     * \addtogroup        temperature_api_periodically_send_temperatures
     * \{
     */

    RUN_TEST(check_periodically_send_temperatures_returns_ok);
    RUN_TEST(check_periodically_send_temperatures_does_not_send_before_cycle);
    RUN_TEST(check_periodically_send_temperatures_sends_once_cycle_elapsed);
    RUN_TEST(check_periodically_send_temperatures_does_not_resend_within_same_cycle);
    RUN_TEST(check_periodically_send_temperatures_first_frame_is_mux_group_0);
    RUN_TEST(check_periodically_send_temperatures_second_frame_is_mux_group_1);
    RUN_TEST(check_periodically_send_temperatures_third_frame_is_info);
    RUN_TEST(check_periodically_send_temperatures_encodes_mux_group_0);
    RUN_TEST(check_periodically_send_temperatures_encodes_mux_group_1);
    RUN_TEST(check_periodically_send_temperatures_encodes_info);

    /*! \} */

    UNITY_END();
}
