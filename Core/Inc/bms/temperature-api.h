/*!
 * \file            temperature-api.h
 * \date            2026-03-31
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature module APIs.
 */

#ifndef TEMPERATURE_API_H
#define TEMPERATURE_API_H

#include "temperature.h"
#include "config.h"

#ifdef CONFIG_TEMPERATURE_MODULE_ENABLE

/*!
 * \brief            Initialize the temperature module.
 */
void temperature_api_init(void);

/*!
 * \brief            Update a single temperature temperature.
 * \param[in]        index The index of the temperature to update.
 * \param[in]        temperature The new temperature in °C.
 * \retval           TEMPERATURE_RC_OK on success.
 * \retval           TEMPERATURE_RC_OUT_OF_BOUNDS if index is greater than the total number of temperatures.
 */
enum TemperatureReturnCode temperature_api_update_temperature(size_t index, celsius temperature);

/*!
 * \brief            Update multiple temperature temperatures.
 * \param[in]        index The start index of the temperatures to update.
 * \param[in]        temperatures A pointer to the array of temperatures to copy.
 * \param[in]        size The number of elements to copy.
 * \retval           TEMPERATURE_RC_OK on success.
 * \retval           TEMPERATURE_RC_NULL_POINTER if temperatures is NULL.
 * \retval           TEMPERATURE_RC_OUT_OF_BOUNDS if index is greater than the total number of temperatures or if the size is too big.
 */
enum TemperatureReturnCode temperature_api_update_temperatures(size_t index, const celsius *temperatures, size_t size);

/*!
 * \brief            Get the minimum cell temperature.
 * \returns           celsius The minimum temperature in °C.
 */
celsius temperature_api_get_min(void);

/*!
 * \brief            Get the maximum cell temperature.
 * \returns          celsius The maximum temperature in °C.
 */
celsius temperature_api_get_max(void);

/*!
 * \brief            Get the average cell temperature.
 * \returns          celsius The average temperature in °C.
 */
celsius temperature_api_get_average(void);

/*!
 * \brief            Copy a list of adjacent temperatures.
 * \param[out]       out A pointer to the array where the temperatures are copied into.
 * \param[in]        strart The index of the first index to copy.
 * \param[in]        size The number of temperatures that should be copied.
 * \retval           TEMPERATURE_RC_OK on success.
 * \retval           TEMPERATURE_RC_NULL_POINTER if out is NULL.
 * \retval           TEMPERATURE_RC_OUT_OF_BOUNDS if index is greater than the total number of temperatures or if the size is too big.
 */
enum TemperatureReturnCode temperature_api_dump_temperatures(celsius *out, size_t start, size_t size);

#else /*! CONFIG_TEMPERATURE_MODULE_ENABLE */

#define temperature_api_init() EAGLETRT_API_NOP()
#define temperature_api_update_temperature(index, temperature) (TEMPERATURE_RC_OK)
#define temperature_api_update_temperatures(index, temperatures, size) (TEMPERATURE_RC_OK)
#define temperature_api_get_min() (0.f)
#define temperature_api_get_max() (0.f)
#define temperature_api_get_average() (0.f)
#define temperature_api_dump_temperatures(out, start, size) (TEMPERATURE_RC_OK)

#endif /*! CONFIG_TEMPERATURE_MODULE_ENABLE */

#endif /*! TEMPERATURE_API_H */
