/*!
 * \file            current-api.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Current module data structures.
 */

#ifndef CURRENT_API_H
#define CURRENT_API_H

#include "current.h"

#include "config.h"
#include "eagletrt-api.h"

#ifdef CONFIG_CURRENT_MODULE_ENABLE

/*!
 * \brief            Initialize the current module.
 */
void current_api_init(void);

/*!
 * \brief            Update the current value.
 * \param[in]        current The new current in A.
 * \retval           CURRENT_RC_OK on success.
 */
enum CurrentReturnCode current_api_update_current(ampere current);

/*!
 * \brief            Get the current in A.
 * \returns          ampere The current in A.
 */
ampere current_api_get_current(void);

/*!
 * \brief            Get the power in kW.
 * \returns          kilowatt The power in kW.
 */
kilowatt current_api_get_power(void);

#else /*! CONFIG_CURRENT_MODULE_ENABLE */

#define current_api_init() (EAGLE_API_NOP())
#define current_api_update_current(current) (CURRENT_RC_OK)
#define current_api_get_current() (0.f)
#define current_api_get_power() (0.f)

#endif /*! CONFIG_CURRENT_MODULE_ENABLE */

#endif /*! CURRENT_API_H */
