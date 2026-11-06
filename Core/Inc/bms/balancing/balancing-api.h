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
 * \param[in]        get_tick Board time base in ms, must not be NULL.
 *
 * \retval           BALANCING_RC_OK on success.
 * \retval           BALANCING_RC_NULL_POINTER if get_tick is NULL.
 */
enum BalancingReturnCode balancing_api_init();

/*!
 * \brief            Start balancing.
 *
 * \details          Every cell sitting more than \p threshold above \p target
 *                   is discharged until the whole pack falls back inside the
 *                   threshold. Pass voltage_api_get_min() as target to balance
 *                   the pack down to its lowest cell.
 *
 * \param[in]        threshold The imbalance tolerated above \p target in V.
 *
 * \retval           BALANCING_RC_OK on success.
 * \retval           BALANCING_RC_SCHEDULER_ERROR if the task scheduler failed.
 */
enum BalancingReturnCode balancing_api_start(volt threshold);

/*!
 * \brief            Stop balancing and release every discharge FET.
 *
 * \retval           BALANCING_RC_OK on success.
 * \retval           BALANCING_RC_SCHEDULER_ERROR if the task scheduler failed.
 */
enum BalancingReturnCode balancing_api_stop(void);

/*!
 * \brief            Check whether balancing is active.
 *
 * \returns          true if balancing is active, false otherwise.
 */
bool balancing_api_is_active(void);

/*!
 * \brief            Handle the received balancing status message.
 *
 * \details          Restarts the status timeout using the time base given to
 *                   #balancing_api_init, so it must not be called before a
 *                   successful init.
 *
 * \param[in]        active    True to start balancing, false to stop it.
 * \param[in]        threshold The imbalance tolerated above target in V.
 *
 * \retval           BALANCING_RC_OK on success.
 * \retval           BALANCING_RC_SCHEDULER_ERROR if the task scheduler failed.
 */
enum BalancingReturnCode balancing_api_set_state_handle(bool active,
                                                        volt threshold);

/*!
 * \brief            Make a balancing toggle request; this will be consumed by
 *                   the balancing routine task.
 *
 * \reval            BALANCING_RC_OK on success.
 */
enum BalancingReturnCode balancing_api_routine_toggle(void);

/*!
 * \defgroup         balancing_api_tasks Balancing module's tasks.
 * \{
 */

/*!
 * \brief            Stop balancing when the state-update timeout expires.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
void balancing_api_timeout_task(uint8_t task_id);

/*!
 * \brief            Recompute the discharge mask and push it to the monitor.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
void balancing_api_refresh_task(uint8_t task_id);

/*!
 * \brief            TBD.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
void balancing_api_routine_task(uint8_t task_id);

/*!
 * \} */

#else /*! CONFIG_BALANCING_MODULE_ENABLE */

#define balancing_api_init(get_tick) (BALANCING_RC_OK)
#define balancing_api_start(threshold) (BALANCING_RC_OK)
#define balancing_api_stop() (BALANCING_RC_OK)
#define balancing_api_run() (BALANCING_RC_OK)
#define balancing_api_is_active() (false)
#define balancing_api_set_state_handle(active, threshold) (BALANCING_RC_OK)
#define balancing_api_routine_toggle() (BALANCING_RC_OK)
#define balancing_api_timeout_task() (EAGLETRT_API_NOP())
#define balancing_api_refresh_task() (EAGLETRT_API_NOP())
#define balancing_api_routine_task() (EAGLETRT_API_NOP())

#endif /*! CONFIG_BALANCING_MODULE_ENABLE */

#endif /*! BALANCING_API_H */
