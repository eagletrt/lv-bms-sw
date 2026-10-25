/*!
 * \file            test-can-communication-router.c
 * \date            2026-09-25
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Test functions for the CAN messages router.
 */

#include "unity.h"
#include "eagletrt-api.h"

#include <string.h>

#include "can-communication.h"
#include "can-communication-router-api.h"
#include "can-primary.h"
#include "can-primary-api.h"
#include "balancing.h"
#include "balancing-api.h"
#include "voltage-api.h"

#include "config.h"
#include "defines.h"
#include "types.h"

extern struct BalancingHandler balancing_handler;

/*! The board time base, handed to balancing at init instead of a HAL_GetTick(). */
static uint32_t test_tick = 0U;

static uint32_t prv_test_get_tick(void) {
    return test_tick;
}

void setUp(void) {
    voltage_api_init();
    test_tick = 0U;
    balancing_api_init(prv_test_get_tick);
}

void tearDown(void) {
}

static struct CanCommunicationFrame prv_make_balancing_set_frame(bool start, volt threshold) {
    union CanPrimaryMessages message = { 0 };
    message.raspberrylvacbalancingset.start = start ? 1U : 0U;
    message.raspberrylvacbalancingset.threshold = threshold;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_RASPBERRYLVACBALANCINGSET,
        .length = can_primary_byte_size_raspberrylvacbalancingset,
    };
    if (can_primary_api_serialize_from_id(frame.id, &message, frame.data) == -1) {
        frame.length = 0U;
    }

    return frame;
}

void test_receive_primary_null_frame(void) {
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_NULL_POINTER, can_communication_router_api_receive_primary(NULL));
}

void test_receive_primary_invalid_id(void) {
    struct CanCommunicationFrame frame = { .id = 0x777U, .length = 8U };
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_INVALID_NETWORK, can_communication_router_api_receive_primary(&frame));
}

void test_receive_primary_balancing_set_starts(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.80F, 3.75F, 3.70F, 3.85F, 3.72F, 3.79F };
    EAGLETRT_API_UNUSED(voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT));

    struct CanCommunicationFrame frame = prv_make_balancing_set_frame(true, 0.05F);
    test_tick = 1234U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_router_api_receive_primary(&frame));

    TEST_ASSERT_TRUE(balancing_api_is_active());
    TEST_ASSERT_FLOAT_WITHIN(0.001F, 0.05F, balancing_handler.threshold);
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 3.70F, balancing_handler.target);
}

void test_receive_primary_balancing_set_stops(void) {
    const volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 3.80F, 3.75F, 3.70F, 3.85F, 3.72F, 3.79F };
    EAGLETRT_API_UNUSED(voltage_api_update_voltages(0U, voltages, DEFINES_CELLS_SERIES_COUNT));

    struct CanCommunicationFrame start_frame = prv_make_balancing_set_frame(true, 0.05F);
    EAGLETRT_API_UNUSED(can_communication_router_api_receive_primary(&start_frame));
    TEST_ASSERT_TRUE(balancing_api_is_active());

    struct CanCommunicationFrame stop_frame = prv_make_balancing_set_frame(false, 0.05F);
    test_tick = 2000U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_router_api_receive_primary(&stop_frame));
    TEST_ASSERT_FALSE(balancing_api_is_active());
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_receive_primary_null_frame);
    RUN_TEST(test_receive_primary_invalid_id);
    RUN_TEST(test_receive_primary_balancing_set_starts);
    RUN_TEST(test_receive_primary_balancing_set_stops);

    return UNITY_END();
}
