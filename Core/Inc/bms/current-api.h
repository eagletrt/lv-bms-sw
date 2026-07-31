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
 * \brief            Update a single current current.
 *
 * \param[in]        index The index of the current to update.
 * \param[in]        current The new current in A.
 *
 * \retval           CURRENT_RC_OK on success.
 * \retval           CURRENT_RC_OUT_OF_BOUNDS if index is greater than the total number of currents.
 */
enum CurrentReturnCode current_api_update_current(size_t index, ampere current);

/*!
 * \brief            Update multiple current currents.
 *
 * \param[in]        index The start index of the currents to update.
 * \param[in]        currents A pointer to the array of currents to copy.
 * \param[in]        size The number of elements to copy.
 *
 * \retval           CURRENT_RC_OK on success.
 * \retval           CURRENT_RC_NULL_POINTER if currents is NULL.
 * \retval           CURRENT_RC_OUT_OF_BOUNDS if index is greater than the total number of currents or if the size is too big.
 */
enum CurrentReturnCode current_api_update_currents(size_t index, ampere *currents, size_t size);

/*!
 * \brief            Copy a list of adjacent currents.
 *
 * \param[out]       out A pointer to the array where the currents are copied into.
 * \param[in]        strart The index of the first index to copy.
 * \param[in]        size The number of currents that should be copied.
 *
 * \retval           CURRENT_RC_OK on success.
 * \retval           CURRENT_RC_NULL_POINTER if out is NULL.
 * \retval           CURRENT_RC_OUT_OF_BOUNDS if index is greater than the total number of currents or if the size is too big.
 */
enum CurrentReturnCode current_api_dump_currents(ampere *out, size_t start, size_t size);

/*!
 * \brief            Get the output current in A.
 *
 * \returns          ampere The current in A.
 */
ampere current_api_get_output_current(void);

/*!
 * \brief            Get the power in kW.
 *
 * \returns          kilowatt The power in kW.
 */
kilowatt current_api_get_power(void);

#else /*! CONFIG_CURRENT_MODULE_ENABLE */

#define current_api_init() (CURRENT_RC_OK)
#define current_api_get_output_current() (0.F)
#define current_api_get_power() (0.F)

#endif /*! CONFIG_CURRENT_MODULE_ENABLE */

#endif /*! CURRENT_API_H */
