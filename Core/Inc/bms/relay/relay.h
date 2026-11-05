/*!
 * \file            relay.h
 * \date            2026-11-05
 *
 * \brief           Master relay supervisor data structures.
 */

#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>
#include <stdint.h>

/*!
 * \defgroup        relay_timing Timing of the master relay supervision.
 *
 * \details         The supervisor looks at the pack every #RELAY_CHECK_PERIOD_MS and
 *                  needs several agreeing verdicts in a row before it moves the
 *                  relay, in either direction.
 *
 *                  Opening K1 takes the whole car down (it feeds every low voltage
 *                  load), so a single odd sample must never be enough: the inputs
 *                  of the verdict are one-shot ADC conversions and LTC readings,
 *                  and any of them can spike for a reason that has nothing to do
 *                  with the pack (EMI from the inverters, a load step sagging the
 *                  cells, a mux still settling, a corrupted SPI frame that
 *                  happens to carry a valid PEC...). The pack limits it enforces
 *                  (cell voltage, cell spread, open wire, temperature) all move
 *                  over seconds, so a fault that persists for
 *                  #RELAY_OPEN_CONFIRMATIONS checks (250 ms) is opened just as
 *                  safely as one opened on the first sample.
 *
 *                  Closing again waits longer after a fault than at power-up, so
 *                  a marginal reading cannot chatter a mechanical relay.
 *
 * \{
 */

#define RELAY_CHECK_PERIOD_MS (50U)       /*!< How often the pack is judged, in ms */
#define RELAY_OPEN_CONFIRMATIONS (5U)     /*!< Consecutive bad verdicts needed to open a closed relay */
#define RELAY_CLOSE_CONFIRMATIONS (3U)    /*!< Consecutive good verdicts needed to close the relay at power-up */
#define RELAY_RECLOSE_CONFIRMATIONS (10U) /*!< Consecutive good verdicts needed to close it again after a fault opened it */

/*! \} */

/*!
 * \brief            Return codes for the relay module functions.
 */
enum RelayReturnCode {
    RELAY_RC_OK,           /*!< Function executed successfully */
    RELAY_RC_NULL_POINTER, /*!< Unexpected NULL pointer */
};

/*!
 * \brief            What the caller has to do with the relay after a verdict.
 */
enum RelayAction {
    RELAY_ACTION_NONE,  /*!< Leave the relay where it is */
    RELAY_ACTION_OPEN,  /*!< Open the relay */
    RELAY_ACTION_CLOSE, /*!< Close the relay */
};

/*!
 * \brief            Relay supervisor handler structure.
 */
struct RelayHandler {
    uint32_t check_tick; /*!< Tick of the last verdict, for the period gate */
    uint8_t good_streak; /*!< Consecutive good verdicts while the relay is open */
    uint8_t bad_streak;  /*!< Consecutive bad verdicts while the relay is closed */
    bool closed;         /*!< True while the relay is commanded closed */
    bool tripped;        /*!< True from a fault opening until the relay closes again */
};

#endif /*! RELAY_H */
