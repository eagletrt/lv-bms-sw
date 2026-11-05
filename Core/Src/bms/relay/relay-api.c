/*!
 * \file            relay-api.c
 * \date            2026-11-05
 *
 * \brief           Master relay supervisor APIs.
 */

#include "relay.h"
#include "relay-api.h"

#include <stddef.h>
#include <string.h>

enum RelayReturnCode relay_api_init(struct RelayHandler *handler) {
    if (handler == NULL) {
        return RELAY_RC_NULL_POINTER;
    }

    memset(handler, 0, sizeof(*handler));
    return RELAY_RC_OK;
}

enum RelayReturnCode relay_api_force_open(struct RelayHandler *handler) {
    if (handler == NULL) {
        return RELAY_RC_NULL_POINTER;
    }

    handler->closed = false;
    handler->good_streak = 0U;
    handler->bad_streak = 0U;
    return RELAY_RC_OK;
}

bool relay_api_is_due(struct RelayHandler *handler, uint32_t tick) {
    if (handler == NULL || (tick - handler->check_tick) < RELAY_CHECK_PERIOD_MS) {
        return false;
    }

    handler->check_tick = tick;
    return true;
}

enum RelayAction relay_api_update(struct RelayHandler *handler, bool healthy) {
    if (handler == NULL) {
        return RELAY_ACTION_NONE;
    }

    if (handler->closed) {
        handler->good_streak = 0U;

        if (healthy) {
            handler->bad_streak = 0U;
            return RELAY_ACTION_NONE;
        }

        if (handler->bad_streak < RELAY_OPEN_CONFIRMATIONS) {
            ++handler->bad_streak;
        }
        if (handler->bad_streak < RELAY_OPEN_CONFIRMATIONS) {
            return RELAY_ACTION_NONE;
        }

        handler->closed = false;
        handler->tripped = true;
        handler->bad_streak = 0U;
        return RELAY_ACTION_OPEN;
    }

    handler->bad_streak = 0U;

    if (!healthy) {
        handler->good_streak = 0U;
        return RELAY_ACTION_NONE;
    }

    const uint8_t required = handler->tripped ? RELAY_RECLOSE_CONFIRMATIONS : RELAY_CLOSE_CONFIRMATIONS;
    if (handler->good_streak < required) {
        ++handler->good_streak;
    }
    if (handler->good_streak < required) {
        return RELAY_ACTION_NONE;
    }

    handler->closed = true;
    handler->tripped = false;
    handler->good_streak = 0U;
    return RELAY_ACTION_CLOSE;
}

bool relay_api_is_closed(const struct RelayHandler *handler) {
    return handler != NULL && handler->closed;
}
