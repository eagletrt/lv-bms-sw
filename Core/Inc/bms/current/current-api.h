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

#include <stddef.h>

#include "config.h"
#include "types.h"

#ifdef CONFIG_CURRENT_MODULE_ENABLE

/*!
 * \brief            Initialize the current module.
 *
 * \retval           CURRENT_RC_OK on success.
 */
enum CurrentReturnCode current_api_init(void);

/*!
 * \brief            Store the current flowing out of the cells.
 *
 * \details          Fed by the board layer from the pack Hall sensor after every
 *                   ADC scan. Positive means current leaving the pack into the
 *                   load, negative means current going into it.
 *
 * \param[in]        current The current in A.
 *
 * \retval           CURRENT_RC_OK on success.
 */
enum CurrentReturnCode current_api_set_cells_output_current(ampere current);

/*!
 * \brief            Get the output current in A.
 *
 * \returns          ampere The current in A.
 */
ampere current_api_get_cells_output_current(void);

/*!
 * \brief            Get the power in kW.
 *
 * \returns          kilowatt The power in kW.
 */
kilowatt current_api_get_power(void);

#else /*! CONFIG_CURRENT_MODULE_ENABLE */

#define current_api_init() (CURRENT_RC_OK)
#define current_api_set_cells_output_current(current) (CURRENT_RC_OK)
#define current_api_get_cells_output_current() (0.F)
#define current_api_get_power() (0.F)

#endif /*! CONFIG_CURRENT_MODULE_ENABLE */

#endif /*! CURRENT_API_H */
