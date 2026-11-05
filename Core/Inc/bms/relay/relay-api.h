/*!
 * \file            relay-api.h
 * \date            2026-11-05
 *
 * \brief           Master relay supervisor APIs.
 */

#ifndef RELAY_API_H
#define RELAY_API_H

#include "relay.h"

#include <stdbool.h>
#include <stdint.h>

/*!
 * \brief            Initialize the supervisor with the relay open.
 *
 * \param[out]       handler The handler to initialize.
 *
 * \retval           RELAY_RC_OK on success.
 * \retval           RELAY_RC_NULL_POINTER if \p handler is NULL.
 */
enum RelayReturnCode relay_api_init(struct RelayHandler *handler);

/*!
 * \brief            Record that the relay has been forced open from outside the
 *                   supervision (init, fatal state).
 *
 * \details          Clears the streaks so the next closing starts from scratch.
 *                   It does not count as a fault: the power-up closing rule applies.
 *
 * \param[in,out]    handler The handler.
 *
 * \retval           RELAY_RC_OK on success.
 * \retval           RELAY_RC_NULL_POINTER if \p handler is NULL.
 */
enum RelayReturnCode relay_api_force_open(struct RelayHandler *handler);

/*!
 * \brief            Tell whether a new verdict is due, and start the next period.
 *
 * \param[in,out]    handler The handler.
 * \param[in]        tick The current tick in ms, wrap-around safe.
 *
 * \retval           true if at least #RELAY_CHECK_PERIOD_MS passed since the last
 *                   verdict (the period restarts from \p tick).
 * \retval           false otherwise, or if \p handler is NULL.
 */
bool relay_api_is_due(struct RelayHandler *handler, uint32_t tick);

/*!
 * \brief            Feed one verdict about the pack and get what to do with the relay.
 *
 * \details          A closed relay opens once #RELAY_OPEN_CONFIRMATIONS bad
 *                   verdicts have come in a row. An open one closes once
 *                   #RELAY_CLOSE_CONFIRMATIONS (#RELAY_RECLOSE_CONFIRMATIONS after
 *                   a fault) good ones have. Any verdict of the opposite kind
 *                   restarts the count. The handler tracks the relay position
 *                   itself, assuming the caller carries the returned action out.
 *
 * \param[in,out]    handler The handler.
 * \param[in]        healthy True if the pack passed every check this time.
 *
 * \returns          enum RelayAction The action to carry out, RELAY_ACTION_NONE
 *                   if \p handler is NULL.
 */
enum RelayAction relay_api_update(struct RelayHandler *handler, bool healthy);

/*!
 * \brief            Tell whether the relay is commanded closed.
 *
 * \param[in]        handler The handler.
 *
 * \retval           true if closed, false if open or if \p handler is NULL.
 */
bool relay_api_is_closed(const struct RelayHandler *handler);

#endif /*! RELAY_API_H */
