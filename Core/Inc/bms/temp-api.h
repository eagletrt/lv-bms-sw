/*!
 * \file            temp-api.h
 * \date            2026-03-31
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature module APIs.
 */

#ifndef TEMP_API_H
#define TEMP_API_H

#include "temp.h"
#include "config.h"

#ifdef CONFIG_TEMPERATURE_MODULE_ENABLE

/*!
 * \brief            Initialize the temperature module.
 */
void temp_api_init(void);

/*!
 * \brief            Update a single temperature value.
 * \param[in]        idx The index of the value to update.
 * \param[in]        value The new value in °C.
 * \retval           TEMP_RC_OK on success.
 * \retval           TEMP_RC_OUT_OF_BOUNDS if index is greater than the total number of values.
 */
enum TempReturnCode temp_api_update_value(size_t idx, celsius value);

/*!
 * \brief            Update multiple temperature values.
 * \param[in]        idx The start index of the values to update.
 * \param[in]        values A pointer to the array of values to copy.
 * \param[in]        size The number of elements to copy.
 * \retval           TEMP_RC_OK on success.
 * \retval           TEMP_RC_NULL_PTR if values is NULL.
 * \retval           TEMP_RC_OUT_OF_BOUNDS if index is greater than the total number of values or if the size is too big.
 */
enum TempReturnCode temp_api_update_values(size_t idx, const celsius *values, size_t size);

/*!
 * \brief            Get the minimum cell temperature.
 * \retval           celsius The minimum value in °C.
 */
celsius temp_api_get_min(void);

/*!
 * \brief            Get the maximum cell temperature.
 * \retval           celsius The maximum value in °C.
 */
celsius temp_api_get_max(void);

/*!
 * \brief            Get the average cell temperature.
 * \retval           celsius The average value in °C.
 */
celsius temp_api_get_avg(void);

/*!
 * \brief            Copy a list of adjacent temperatures.
 * \param[out]       out A pointer to the array where the values are copied into.
 * \param[in]        strart The index of the first index to copy.
 * \param[in]        size The number of values that should be copied.
 * \retval           TEMP_RC_OK on success.
 * \retval           TEMP_RC_NULL_PTR if out is NULL.
 * \retval           TEMP_RC_OUT_OF_BOUNDS if index is greater than the total number of values or if the size is too big.
 */
enum TempReturnCode temp_api_dump_values(celsius *out, size_t start, size_t size);

#else /*! CONFIG_TEMPERATURE_MODULE_ENABLE */

#define temp_api_init() (NULL)
#define temp_api_update_value(idx, value) (TEMP_RC_OK)
#define temp_api_update_values(idx, values, size) (TEMP_RC_OK)
#define temp_api_get_min() (0.f)
#define temp_api_get_max() (0.f)
#define temp_api_get_avg() (0.f)
#define temp_api_dump_values(out, start, size) (TEMP_RC_OK)

#endif /*! CONFIG_TEMPERATURE_MODULE_ENABLE */

#endif /*! TEMP_API_H */
