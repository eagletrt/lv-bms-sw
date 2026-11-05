/*!
 * \file            test-relay.c
 * \date            2026-11-05
 *
 * \brief           Test functions for the master relay supervisor.
 */

#include "unity.h"

#include <stdint.h>

#include "relay.h"
#include "relay-api.h"

static struct RelayHandler handler;

/*! Feed \p count identical verdicts, return the last action. */
static enum RelayAction prv_feed(bool healthy, unsigned count) {
    enum RelayAction action = RELAY_ACTION_NONE;
    for (unsigned i = 0U; i < count; ++i) {
        action = relay_api_update(&handler, healthy);
    }
    return action;
}

/*! Bring the relay to the closed position the way the firmware does at power-up. */
static void prv_close(void) {
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_CLOSE, prv_feed(true, RELAY_CLOSE_CONFIRMATIONS));
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));
}

void setUp(void) {
    relay_api_init(&handler);
}

void tearDown(void) {
}

/*!
 * \defgroup		relay_api_init Test for relay_api_init and relay_api_force_open functions.
 * \{
 */

void check_relay_api_init_starts_open(void) {
    handler.closed = true;
    handler.tripped = true;
    handler.good_streak = 7U;
    handler.bad_streak = 3U;

    TEST_ASSERT_EQUAL_INT(RELAY_RC_OK, relay_api_init(&handler));
    TEST_ASSERT_FALSE(relay_api_is_closed(&handler));
    TEST_ASSERT_FALSE(handler.tripped);
    TEST_ASSERT_EQUAL_UINT8(0U, handler.good_streak);
    TEST_ASSERT_EQUAL_UINT8(0U, handler.bad_streak);
}

void check_relay_api_with_null_pointer(void) {
    TEST_ASSERT_EQUAL_INT(RELAY_RC_NULL_POINTER, relay_api_init(NULL));
    TEST_ASSERT_EQUAL_INT(RELAY_RC_NULL_POINTER, relay_api_force_open(NULL));
    TEST_ASSERT_FALSE(relay_api_is_due(NULL, 1000U));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, relay_api_update(NULL, true));
    TEST_ASSERT_FALSE(relay_api_is_closed(NULL));
}

void check_relay_api_force_open_restarts_closing_from_scratch(void) {
    prv_close();
    prv_feed(false, RELAY_OPEN_CONFIRMATIONS - 1U);

    TEST_ASSERT_EQUAL_INT(RELAY_RC_OK, relay_api_force_open(&handler));
    TEST_ASSERT_FALSE(relay_api_is_closed(&handler));
    TEST_ASSERT_EQUAL_UINT8(0U, handler.bad_streak);

    /* Forcing it open is not a fault: the power-up rule applies, not the re-close one. */
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(true, RELAY_CLOSE_CONFIRMATIONS - 1U));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_CLOSE, relay_api_update(&handler, true));
}

/*! \} */

/*!
 * \defgroup		relay_api_is_due Test for relay_api_is_due function.
 * \{
 */

void check_relay_api_is_due_waits_for_the_period(void) {
    TEST_ASSERT_FALSE(relay_api_is_due(&handler, RELAY_CHECK_PERIOD_MS - 1U));
    TEST_ASSERT_TRUE(relay_api_is_due(&handler, RELAY_CHECK_PERIOD_MS));

    /* The period restarts from the tick that was granted. */
    TEST_ASSERT_FALSE(relay_api_is_due(&handler, RELAY_CHECK_PERIOD_MS + RELAY_CHECK_PERIOD_MS - 1U));
    TEST_ASSERT_TRUE(relay_api_is_due(&handler, RELAY_CHECK_PERIOD_MS + RELAY_CHECK_PERIOD_MS));
}

void check_relay_api_is_due_survives_tick_wrap_around(void) {
    handler.check_tick = UINT32_MAX - 10U;

    TEST_ASSERT_FALSE(relay_api_is_due(&handler, UINT32_MAX));
    TEST_ASSERT_FALSE(relay_api_is_due(&handler, RELAY_CHECK_PERIOD_MS - 12U));
    TEST_ASSERT_TRUE(relay_api_is_due(&handler, RELAY_CHECK_PERIOD_MS - 10U));
}

/*! \} */

/*!
 * \defgroup		relay_api_update Test for relay_api_update function.
 * \{
 */

void check_relay_api_update_stays_open_until_the_pack_looks_good(void) {
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(false, 50U));
    TEST_ASSERT_FALSE(relay_api_is_closed(&handler));
}

void check_relay_api_update_closes_after_the_power_up_confirmations(void) {
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(true, RELAY_CLOSE_CONFIRMATIONS - 1U));
    TEST_ASSERT_FALSE(relay_api_is_closed(&handler));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_CLOSE, relay_api_update(&handler, true));
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));
}

void check_relay_api_update_power_up_confirmations_must_be_consecutive(void) {
    prv_feed(true, RELAY_CLOSE_CONFIRMATIONS - 1U);
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, relay_api_update(&handler, false));

    /* The count starts over after the bad verdict. */
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(true, RELAY_CLOSE_CONFIRMATIONS - 1U));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_CLOSE, relay_api_update(&handler, true));
}

void check_relay_api_update_does_not_repeat_the_close_action(void) {
    prv_close();
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(true, 100U));
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));
}

/*!
 * This is the regression the supervisor exists for: one odd sample used to open
 * the relay on the spot, and three good ones closed it again, so the whole car
 * lost its supply for a couple of hundred milliseconds.
 */
void check_relay_api_update_ignores_a_single_bad_verdict(void) {
    prv_close();

    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, relay_api_update(&handler, false));
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, relay_api_update(&handler, true));
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));
}

void check_relay_api_update_ignores_scattered_bad_verdicts(void) {
    prv_close();

    /* Never RELAY_OPEN_CONFIRMATIONS in a row, however many in total. */
    for (unsigned i = 0U; i < 100U; ++i) {
        TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(false, RELAY_OPEN_CONFIRMATIONS - 1U));
        TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, relay_api_update(&handler, true));
    }
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));
}

void check_relay_api_update_opens_on_a_persistent_fault(void) {
    prv_close();

    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(false, RELAY_OPEN_CONFIRMATIONS - 1U));
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_OPEN, relay_api_update(&handler, false));
    TEST_ASSERT_FALSE(relay_api_is_closed(&handler));
    TEST_ASSERT_TRUE(handler.tripped);
}

void check_relay_api_update_does_not_repeat_the_open_action(void) {
    prv_close();
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_OPEN, prv_feed(false, RELAY_OPEN_CONFIRMATIONS));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(false, 100U));
}

void check_relay_api_update_recloses_only_after_the_longer_confirmation(void) {
    prv_close();
    prv_feed(false, RELAY_OPEN_CONFIRMATIONS);

    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(true, RELAY_RECLOSE_CONFIRMATIONS - 1U));
    TEST_ASSERT_FALSE(relay_api_is_closed(&handler));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_CLOSE, relay_api_update(&handler, true));
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));
    TEST_ASSERT_FALSE(handler.tripped);
}

void check_relay_api_update_reclose_confirmations_must_be_consecutive(void) {
    prv_close();
    prv_feed(false, RELAY_OPEN_CONFIRMATIONS);

    prv_feed(true, RELAY_RECLOSE_CONFIRMATIONS - 1U);
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, relay_api_update(&handler, false));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(true, RELAY_RECLOSE_CONFIRMATIONS - 1U));
    TEST_ASSERT_FALSE(relay_api_is_closed(&handler));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_CLOSE, relay_api_update(&handler, true));
}

void check_relay_api_update_second_fault_needs_the_long_reclose_again(void) {
    prv_close();
    prv_feed(false, RELAY_OPEN_CONFIRMATIONS);
    prv_feed(true, RELAY_RECLOSE_CONFIRMATIONS);
    TEST_ASSERT_TRUE(relay_api_is_closed(&handler));

    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_OPEN, prv_feed(false, RELAY_OPEN_CONFIRMATIONS));
    TEST_ASSERT_EQUAL_INT(RELAY_ACTION_NONE, prv_feed(true, RELAY_CLOSE_CONFIRMATIONS));
    TEST_ASSERT_FALSE(relay_api_is_closed(&handler));
}

void check_relay_api_update_open_confirmation_covers_a_quarter_of_a_second(void) {
    /* The point of the debounce, spelled out in time. */
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(250U, RELAY_OPEN_CONFIRMATIONS * RELAY_CHECK_PERIOD_MS);
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(check_relay_api_init_starts_open);
    RUN_TEST(check_relay_api_with_null_pointer);
    RUN_TEST(check_relay_api_force_open_restarts_closing_from_scratch);

    RUN_TEST(check_relay_api_is_due_waits_for_the_period);
    RUN_TEST(check_relay_api_is_due_survives_tick_wrap_around);

    RUN_TEST(check_relay_api_update_stays_open_until_the_pack_looks_good);
    RUN_TEST(check_relay_api_update_closes_after_the_power_up_confirmations);
    RUN_TEST(check_relay_api_update_power_up_confirmations_must_be_consecutive);
    RUN_TEST(check_relay_api_update_does_not_repeat_the_close_action);
    RUN_TEST(check_relay_api_update_ignores_a_single_bad_verdict);
    RUN_TEST(check_relay_api_update_ignores_scattered_bad_verdicts);
    RUN_TEST(check_relay_api_update_opens_on_a_persistent_fault);
    RUN_TEST(check_relay_api_update_does_not_repeat_the_open_action);
    RUN_TEST(check_relay_api_update_recloses_only_after_the_longer_confirmation);
    RUN_TEST(check_relay_api_update_reclose_confirmations_must_be_consecutive);
    RUN_TEST(check_relay_api_update_second_fault_needs_the_long_reclose_again);
    RUN_TEST(check_relay_api_update_open_confirmation_covers_a_quarter_of_a_second);

    return UNITY_END();
}
