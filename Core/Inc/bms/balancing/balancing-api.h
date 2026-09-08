/*!
 * \file            balancing-api.h
 * \date            2026-09-08
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Cells balancing module APIs.
 */

#ifndef BALANCING_API_H
#define BALANCING_API_H

#include "balancing.h"
#include "config.h"
#include "eagletrt-api.h"

#ifdef CONFIG_BALANCING_MODULE_ENABLE

/*!
 * \brief            Initialize the balancing module.
 *
 * \retval           BALANCING_RC_OK on success.
 */
enum BalancingReturnCode balancing_api_init(void);

/*!
 * \brief            Start balancing.
 *
 * \details          Every cell sitting more than \p threshold above \p target
 *                   is discharged until the whole pack falls back inside the
 *                   threshold. Pass voltage_api_get_min() as target to balance
 *                   the pack down to its lowest cell.
 *
 * \param[in]        target The voltage each discharged cell should reach in V.
 * \param[in]        threshold The imbalance tolerated above \p target in V.
 *
 * \retval           BALANCING_RC_OK on success.
 * \retval           BALANCING_RC_OUT_OF_BOUNDS if target is outside the valid
 *                   cell voltage range, e.g. before the monitor has produced
 *                   its first readings.
 */
enum BalancingReturnCode balancing_api_start(volt target, volt threshold);

/*!
 * \brief            Stop balancing and release every discharge FET.
 *
 * \retval           BALANCING_RC_OK on success.
 */
enum BalancingReturnCode balancing_api_stop(void);

/*!
 * \brief            Run one balancing cycle.
 *
 * \details          Cooperative task, same shape as adc_routine(): meant to be
 *                   called on every loop iteration, self-gated on \p tick, one
 *                   small step per call, never blocking. Recomputes the
 *                   discharge mask from the last measured voltages, alternates
 *                   between even and odd cell parities so adjacent cells never
 *                   discharge together, and pushes the mask to the monitor.
 *                   #BALANCING_RUN_PERIOD_MS is much shorter than the LTC
 *                   30 s discharge timeout, so the mask stays fresh.
 *
 * \param[in]        tick The current tick in ms.
 *
 * \retval           BALANCING_RC_OK on success.
 */
enum BalancingReturnCode balancing_api_run(uint32_t tick);

/*!
 * \brief            Check whether balancing is active.
 *
 * \returns          true if balancing is active, false otherwise.
 */
bool balancing_api_is_active(void);

#else /*! CONFIG_BALANCING_MODULE_ENABLE */

#define balancing_api_init() EAGLETRT_API_NOP()
#define balancing_api_start(target, threshold) (BALANCING_RC_OK)
#define balancing_api_stop() (BALANCING_RC_OK)
#define balancing_api_run(tick) (BALANCING_RC_OK)
#define balancing_api_is_active() (false)

#endif /*! CONFIG_BALANCING_MODULE_ENABLE */

#endif /*! BALANCING_API_H */
