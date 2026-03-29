/*!
 * \file            temp.h
 * \date            2026-03-24
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature reading module.
 */

#ifndef TEMP_H
#define TEMP_H

#include <stddef.h>

#include "config.h"
#include "defs.h"

#define TEMP_MIN_C (0.f) /*!< Minimum allowed cell temperature in °C */
#define TEMP_MAX_C (0.f) /*!< Maximum allowed cell temperature in °C */

/*!
 * \brief            Return codes for the temperature module functions.
 */
enum TempReturnCode {
    TEMP_RETURN_CODE_OK,            /*!< Function executed successfully */
    TEMP_RETURN_CODE_INVALID_ARG,   /*!< Bad argument passed to a function */
    TEMP_RETURN_CODE_OUT_OF_BOUNDS, /*!< A value is greater/lower than the maximum/minimum allowed value */
};

/*!
 * \brief            Temperature module handler structure.
 */
struct TempHandler {
    /*! TODO: add a pointer to the configuration handler (SPEC). */
    celsius temperatures[CONFIG_CELLBOARD_COUNT]; /*!< An array of temperatures in °C */
};

#ifdef CONFIG_TEMPERATURE_MODULE_ENABLE

/*!
 * \brief            Initialize the temperature module.
 * \retval           TempReturnCode TEMP_RETURN_CODE_OK on success, an error otherwise:
 *                   - TEMP_RETURN_CODE_INVALID_ARG
 */
enum TempReturnCode temp_init(void);

/*!
 * \brief            Update a single temperature value.
 * \param[in]        idx The index of the value to update.
 * \param[in]        value The new value in °C.
 * \retval           TempReturnCode TEMP_RETURN_CODE_OK on success, an error otherwise:
 *                   - TEMP_OUT_OF_BOUNDS if index is greater than the total number of values.
 */
enum TempReturnCode temp_update_value(size_t idx, celsius value);

/*!
 * \brief            Update multiple temperature values.
 * \param[in]        idx The start index of the values to update.
 * \param[in]        values A pointer to the array of values to copy.
 * \param[in]        size The number of elements to copy.
 * \retval           TempReturnCode TEMP_RETURN_CODE_OK on success, an error otherwise:
 *                   - TEMP_RETURN_CODE_INVALID_ARG if values is NULL;
 *                   - TEMP_RETURN_CODE_OUT_OF_BOUNDS if index is greater than the total number of values.
 */
enum TempReturnCode temp_update_values(size_t idx, celsius *values, size_t size);

/*!
 * \brief            Get a pointer to the array where cells temperatures are stored.
 * \retval           celsius* The pointer to the array.
 */
const celsius *temp_get_temperatures(void);

/*!
 * \brief            Get the minimum cell temperature.
 * \retval           celsius The minimum value in °C.
 */
celsius temp_get_min(void);

/*!
 * \brief            Get the maximum cell temperature.
 * \retval           celsius The maximum value in °C.
 */
celsius temp_get_max(void);

/*!
 * \brief            Get the average cell temperature.
 * \retval           celsius The average value in °C.
 */
celsius temp_get_avg(void);

#else /*! CONFIG_TEMPERATURE_MODULE_ENABLE */

#define temp_init() (TEMP_RETURN_CODE_OK)
#define temp_update_valuetemp_update_value(idx, value) (TEMP_RETURN_CODE_OK)
#define temp_update_valuetemp_update_values(idx, values) (TEMP_RETURN_CODE_OK)
#define temp_get_temperatures() (NULL)
#define temp_get_min() (NULL)
#define temp_get_max() (NULL)

#endif /*! CONFIG_TEMPERATURE_MODULE_ENABLE */

#endif /*! TEMP_H */
