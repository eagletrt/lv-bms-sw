#include "unity.h"
#include "application.h"
#include "can-communication-api.h"
#include "identity-api.h"
#include "voltage-api.h"
#include "temperature-api.h"
#include "../../../Src/bms/application/application.c"

static state_t test_state;
static bool test_failed;
static unsigned monitor_count;
static unsigned relay_count;
static unsigned feedback_count;
static unsigned heartbeat_count;
static unsigned discharge_count;
static unsigned frame_count;
static uint16_t frame_ids[16];

void fsm_monitor_step(void) {
    ++monitor_count;
}

void fsm_relay_supervise(void) {
    ++relay_count;
}

static void prv_test_feedback(void) {
    ++feedback_count;
}

static void prv_test_heartbeat(void) {
    ++heartbeat_count;
}

static void prv_test_discharge(void) {
    ++discharge_count;
}

static enum CanCommunicationReturnCode prv_test_receive(const struct CanCommunicationFrame *frame) {
    EAGLETRT_API_UNUSED(frame);
    return CAN_COMMUNICATION_RC_OK;
}

static enum CanCommunicationReturnCode prv_test_send(const struct CanCommunicationFrame *frame) {
    if (frame_count < sizeof(frame_ids) / sizeof(frame_ids[0])) {
        frame_ids[frame_count] = frame->id;
    }
    ++frame_count;
    return CAN_COMMUNICATION_RC_OK;
}

static void prv_test_dispatch(uint8_t task_id) {
    application_task_list[task_id].function(task_id);
}

void setUp(void) {
    test_state = STATE_IDLE;
    test_failed = false;
    monitor_count = 0U;
    relay_count = 0U;
    feedback_count = 0U;
    heartbeat_count = 0U;
    discharge_count = 0U;
    frame_count = 0U;
    memset(frame_ids, 0, sizeof(frame_ids));
    application_services = (struct ApplicationServices){
        .state = &test_state,
        .failed = &test_failed,
        .feedback = prv_test_feedback,
        .heartbeat = prv_test_heartbeat,
        .discharge = prv_test_discharge
    };
    struct CanCommunicationNetworkConfig configs[CAN_COMMUNICATION_NETWORK_COUNT] = {
        [CAN_COMMUNICATION_NETWORK_PRIMARY] = {
            .send = prv_test_send,
            .on_receive = prv_test_receive }
    };
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_api_init(configs));
    TEST_ASSERT_EQUAL(IDENTITY_RC_OK, identity_api_init());
    TEST_ASSERT_EQUAL(VOLTAGE_RC_OK, voltage_api_init());
    TEST_ASSERT_EQUAL(TEMPERATURE_RC_OK, temperature_api_init());
}

void tearDown(void) {
    memset(&application_services, 0, sizeof(application_services));
}

void test_board_callbacks_are_dispatched(void) {
    prv_test_dispatch(APPLICATION_TASK_FEEDBACK);
    TEST_ASSERT_EQUAL_UINT(1U, feedback_count);
    TEST_ASSERT_EQUAL_UINT(0U, heartbeat_count);
    prv_test_dispatch(APPLICATION_TASK_HEARTBEAT);
    TEST_ASSERT_EQUAL_UINT(1U, heartbeat_count);
    prv_test_dispatch(APPLICATION_TASK_DISCHARGE);
    TEST_ASSERT_EQUAL_UINT(1U, discharge_count);
}

void test_fatal_gates_monitor_relay_and_telemetry(void) {
    test_state = STATE_FATAL;
    for (uint8_t task_id = 0U; task_id < APPLICATION_TASK_DISCHARGE; ++task_id) {
        prv_test_dispatch(task_id);
    }
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK,
                      can_communication_api_process_tx(CAN_COMMUNICATION_NETWORK_PRIMARY));
    TEST_ASSERT_EQUAL_UINT(0U, monitor_count);
    TEST_ASSERT_EQUAL_UINT(0U, relay_count);
    TEST_ASSERT_EQUAL_UINT(0U, frame_count);
    TEST_ASSERT_EQUAL_UINT(1U, feedback_count);
    TEST_ASSERT_EQUAL_UINT(1U, heartbeat_count);
}

void test_balancing_keeps_monitor_and_relay_running(void) {
    test_state = STATE_BALANCING;
    prv_test_dispatch(APPLICATION_TASK_MONITOR);
    prv_test_dispatch(APPLICATION_TASK_RELAY);
    TEST_ASSERT_EQUAL_UINT(1U, monitor_count);
    TEST_ASSERT_EQUAL_UINT(1U, relay_count);
}

void test_scheduler_failure_gates_all_callbacks(void) {
    test_failed = true;
    for (uint8_t task_id = 0U; task_id < APPLICATION_TASK_COUNT; ++task_id) {
        prv_test_dispatch(task_id);
    }
    TEST_ASSERT_EQUAL_UINT(0U, monitor_count);
    TEST_ASSERT_EQUAL_UINT(0U, relay_count);
    TEST_ASSERT_EQUAL_UINT(0U, feedback_count);
    TEST_ASSERT_EQUAL_UINT(0U, heartbeat_count);
    TEST_ASSERT_EQUAL_UINT(0U, discharge_count);
}

void test_telemetry_callbacks_send_expected_frames(void) {
    const uint16_t ids[] = {
        CAN_PRIMARY_MESSAGE_FRAME_ID_LVACFSM,
        CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVERSION,
        CAN_PRIMARY_MESSAGE_FRAME_ID_LVACLIBCANVERSION,
        CAN_PRIMARY_MESSAGE_FRAME_ID_LVACCELLVOLTAGE,
        CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVOLTAGEINFO,
        CAN_PRIMARY_MESSAGE_FRAME_ID_LVACTEMPERATURE
    };
    for (uint8_t task_id = APPLICATION_TASK_STATE; task_id <= APPLICATION_TASK_TEMPERATURES; ++task_id) {
        frame_count = 0U;
        prv_test_dispatch(task_id);
        can_communication_api_process_tx(CAN_COMMUNICATION_NETWORK_PRIMARY);
        TEST_ASSERT_EQUAL_UINT(ids[task_id - APPLICATION_TASK_STATE], frame_ids[0]);
        const unsigned expected = (task_id == APPLICATION_TASK_VERSION ||
                                   task_id == APPLICATION_TASK_LIBCAN_VERSION ||
                                   task_id == APPLICATION_TASK_TEMPERATURES)
                                      ? 2U
                                      : 1U;
        TEST_ASSERT_EQUAL_UINT(expected, frame_count);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_board_callbacks_are_dispatched);
    RUN_TEST(test_fatal_gates_monitor_relay_and_telemetry);
    RUN_TEST(test_balancing_keeps_monitor_and_relay_running);
    RUN_TEST(test_scheduler_failure_gates_all_callbacks);
    RUN_TEST(test_telemetry_callbacks_send_expected_frames);
    return UNITY_END();
}
