/*!
 * \file            discharge-api.h
 * \date            2026-10-27
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Discharge module APIs.
 */

#ifndef DISCHARGE_API_H
#define DISCHARGE_API_H

#include <types.h>

#include "discharge.h"

/*!
 * \brief            Initialize the discharge module.
 *
 * \retval           DISCHARGE_RC_OK on success.
 */
enum DischargeReturnCode discharge_api_init(void);

/*!
 * \brief            Make a discharge toggle request; this will be consumed by
 *                   the discharge routine task.
 *
 * \reval            DISCHARGE_RC_OK on success.
 */
enum DischargeReturnCode discharge_api_routine_toggle(void);

/*!
 * \defgroup         discharge_api_tasks Discharge module's tasks.
 * \{
 */

/*!
 * \brief            TBD.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
void discharge_api_routine_task(uint8_t task_id);

/*!
 * \brief            TBD.
 *
 * \param[in]        task_id The ID of the task that is being executed.
 */
void discharge_api_step_task(uint8_t task_id);

/*!
 * \} */

#endif /*! DISCHARGE_API_H */
