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

enum DischargeReturnCode discharge_api_init(void);

enum DischargeReturnCode discharge_api_toggle(void);

void discharge_api_routine_task(uint8_t task_id);

void discharge_api_step_task(uint8_t task_id);

#endif /*! DISCHARGE_API_H */
