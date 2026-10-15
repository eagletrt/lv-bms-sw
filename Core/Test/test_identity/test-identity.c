/*!
 * \file test_identity.c
 * \authors Alessandro Bridi
 * \authors Thomas Moretti [majorfrost173@gmail.com]
 * \date 2026-08-07
 * \brief Unit tests for the identity module.
 */

#include <unity.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "fff.h"
#include "identity-api.h"
#include "can-communication-api.h"
#include "can-primary.h"
#include "can-primary-api.h"
#include "eagletrt-api.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, mock_can_send, const struct CanCommunicationFrame *);
FAKE_VALUE_FUNC(enum CanCommunicationReturnCode, mock_on_receive, const struct CanCommunicationFrame *);

#define TEST_MAX_CAPTURED_FRAMES (4U)

/*!
 * \brief Frames flushed onto the primary bus during a single test.
 */
EAGLETRT_STATIC struct CanCommunicationFrame test_captured_frames[TEST_MAX_CAPTURED_FRAMES];
EAGLETRT_STATIC uint32_t test_captured_count;

/*!
 * \brief Send callback: records every frame the module flushes to the primary bus.
 *
 * \param[in] frame The frame being transmitted.
 *
 * \retval CAN_COMMUNICATION_RC_OK always.
 */
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_capture_send(const struct CanCommunicationFrame *frame) {
    if (frame != NULL && test_captured_count < TEST_MAX_CAPTURED_FRAMES) {
        test_captured_frames[test_captured_count] = *frame;
        test_captured_count++;
    }
    return CAN_COMMUNICATION_RC_OK;
}

/*!
 * \brief Drain the primary network's tx queue, invoking \ref prv_capture_send per frame.
 */
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

    identity_api_init();
}

void tearDown(void) {
}

/* --- Test Cases --- */

/*!
 * \defgroup identity_api_init Tests for identity_api_init
 * \{
 */

void test_identity_api_init_returns_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(IDENTITY_RC_OK, identity_api_init(), "Identity initialization must report success.");
}
/*! \} */

/*!
 * \defgroup identity_api_send_state Tests for identity_api_send_state
 * \{
 */

void test_send_state_returns_ok(void) {
    enum IdentityReturnCode rc = identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_IDLE);

    TEST_ASSERT_EQUAL_MESSAGE(IDENTITY_RC_OK, rc, "Sending the state must report success.");
}

void test_send_state_queues_exactly_one_frame(void) {
    identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_FLASH);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, test_captured_count, "Sending the state must queue exactly one primary frame.");
}

void test_send_state_targets_lvacfsm_frame_id(void) {
    identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_IDLE);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACFSM, test_captured_frames[0].id, "The state must be reported on the LVACFSM frame.");
}

void test_send_state_encodes_status(void) {
    identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_INIT);
    prv_flush_primary();

    union CanPrimaryMessages msg = { 0 };
    EAGLETRT_API_UNUSED(can_primary_api_deserialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACFSM, test_captured_frames[0].data, &msg));

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(CAN_PRIMARY_LVACFSM_STATUS_INIT, msg.lvacfsm.status, "The status must be encoded into the LVACFSM frame.");
}

/*! \} */

/*!
 * \defgroup identity_api_periodically_send_state Tests for identity_api_periodically_send_state
 * \{
 */

void test_send_state_success(void) {
    enum IdentityReturnCode rc = identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_IDLE);

    TEST_ASSERT_EQUAL(IDENTITY_RC_OK, rc);
}

void test_send_state_without_clock(void) {
    identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_FLASH);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32(1U, test_captured_count);
}

void test_send_state_sends_one_frame(void) {
    identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_INIT);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32(1U, test_captured_count);
}

void test_send_state_on_every_call(void) {
    // First call emits; a second call also queues its own frame because timing
    // now belongs to the application scheduler, not the sender.
    identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_ERROR);
    identity_api_send_state(CAN_PRIMARY_LVACFSM_STATUS_IDLE);
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32(2U, test_captured_count);
}
/*! \} */

/*!
 * \defgroup identity_api_periodically_send_version Tests for identity_api_periodically_send_version
 * \{
 */

void test_send_version_without_clock(void) {
    identity_api_send_version();
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32(2U, test_captured_count);
}

void test_send_version_emits_two_frames(void) {
    identity_api_send_version();
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2U, test_captured_count, "A version report must emit both the version and version-info frames.");
}

void test_send_version_first_frame_is_lvacversion(void) {
    identity_api_send_version();
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVERSION, test_captured_frames[0].id, "The first version frame must be the LVACVERSION frame.");
}

void test_send_version_second_frame_is_lvacversioninfo(void) {
    identity_api_send_version();
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVERSIONINFO, test_captured_frames[1].id, "The second version frame must be the LVACVERSIONINFO frame.");
}

void test_send_version_encodes_firmware_version(void) {
    identity_api_send_version();
    prv_flush_primary();

    union CanPrimaryMessages msg = { 0 };
    EAGLETRT_API_UNUSED(can_primary_api_deserialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACVERSION, test_captured_frames[0].data, &msg));

    TEST_ASSERT_EQUAL_UINT16_MESSAGE(IDENTITY_VERSION_MAJOR, msg.lvacversion.major, "The firmware major version must be encoded.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(IDENTITY_VERSION_MINOR, msg.lvacversion.minor, "The firmware minor version must be encoded.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(IDENTITY_VERSION_PATCH, msg.lvacversion.patch, "The firmware patch version must be encoded.");
}
/*! \} */

/*!
 * \defgroup identity_api_periodically_send_libcan_version Tests for identity_api_periodically_send_libcan_version
 * \{
 */

void test_send_libcan_version_without_clock(void) {
    identity_api_send_libcan_version();
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32(2U, test_captured_count);
}

void test_send_libcan_version_emits_two_frames(void) {
    identity_api_send_libcan_version();
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2U, test_captured_count, "A libcan version report must emit both the version and version-info frames.");
}

void test_send_libcan_version_first_frame_is_lvaclibcanversion(void) {
    identity_api_send_libcan_version();
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACLIBCANVERSION, test_captured_frames[0].id, "The first libcan frame must be the LVACLIBCANVERSION frame.");
}

void test_send_libcan_version_second_frame_is_lvaclibcanversioninfo(void) {
    identity_api_send_libcan_version();
    prv_flush_primary();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACLIBCANVERSIONINFO, test_captured_frames[1].id, "The second libcan frame must be the LVACLIBCANVERSIONINFO frame.");
}

void test_send_libcan_version_encodes_libcan_version(void) {
    identity_api_send_libcan_version();
    prv_flush_primary();

    union CanPrimaryMessages msg = { 0 };
    EAGLETRT_API_UNUSED(can_primary_api_deserialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_LVACLIBCANVERSION, test_captured_frames[0].data, &msg));

    TEST_ASSERT_EQUAL_UINT16_MESSAGE(can_version_major, msg.lvaclibcanversion.major, "The libcan major version must be encoded.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(can_version_minor, msg.lvaclibcanversion.minor, "The libcan minor version must be encoded.");
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(can_version_patch, msg.lvaclibcanversion.patch, "The libcan patch version must be encoded.");
}
/*! \} */

int main(void) {
    UNITY_BEGIN();

    /*!
     * \addtogroup identity_api_init
     * \{
     */
    RUN_TEST(test_identity_api_init_returns_ok);
    /*! \} */

    /*!
     * \addtogroup identity_api_send_state
     * \{
     */
    RUN_TEST(test_send_state_returns_ok);
    RUN_TEST(test_send_state_queues_exactly_one_frame);
    RUN_TEST(test_send_state_targets_lvacfsm_frame_id);
    RUN_TEST(test_send_state_encodes_status);
    /*! \} */

    /*!
     * \addtogroup identity_api_periodically_send_state
     * \{
     */
    RUN_TEST(test_send_state_success);
    RUN_TEST(test_send_state_without_clock);
    RUN_TEST(test_send_state_sends_one_frame);
    RUN_TEST(test_send_state_on_every_call);
    /*! \} */

    /*!
     * \addtogroup identity_api_periodically_send_version
     * \{
     */
    RUN_TEST(test_send_version_without_clock);
    RUN_TEST(test_send_version_emits_two_frames);
    RUN_TEST(test_send_version_first_frame_is_lvacversion);
    RUN_TEST(test_send_version_second_frame_is_lvacversioninfo);
    RUN_TEST(test_send_version_encodes_firmware_version);
    /*! \} */

    /*!
     * \addtogroup identity_api_periodically_send_libcan_version
     * \{
     */
    RUN_TEST(test_send_libcan_version_without_clock);
    RUN_TEST(test_send_libcan_version_emits_two_frames);
    RUN_TEST(test_send_libcan_version_first_frame_is_lvaclibcanversion);
    RUN_TEST(test_send_libcan_version_second_frame_is_lvaclibcanversioninfo);
    RUN_TEST(test_send_libcan_version_encodes_libcan_version);
    /*! \} */

    return UNITY_END();
}
