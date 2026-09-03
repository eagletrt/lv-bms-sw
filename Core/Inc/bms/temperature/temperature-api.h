/*!
 * \file            temperature-api.h
 * \date            2026-03-31
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Temperature module APIs.
 */

#ifndef TEMPERATURE_API_H
#define TEMPERATURE_API_H

#include <stdint.h>

#include "temperature.h"
#include "config.h"

#ifdef CONFIG_TEMPERATURE_MODULE_ENABLE

/*!
 * \brief            Initialize the temperature module.
 *
 * \retval           TEMPERATURE_RC_OK on success.
 */
enum TemperatureReturnCode temperature_api_init(void);

/*!
 * \brief            Update a single temperature temperature.
 *
 * \param[in]        index The index of the temperature to update.
 * \param[in]        temperature The new temperature in °C.
 *
 * \retval           TEMPERATURE_RC_OK on success.
 * \retval           TEMPERATURE_RC_OUT_OF_BOUNDS if index is greater than the total number of temperatures.
 */
enum TemperatureReturnCode temperature_api_update_temperature(size_t index, celsius temperature);

/*!
 * \brief            Update multiple temperature temperatures.
 *
 * \param[in]        index The start index of the temperatures to update.
 * \param[in]        temperatures A pointer to the array of temperatures to copy.
 * \param[in]        size The number of elements to copy.
 *
 * \retval           TEMPERATURE_RC_OK on success.
 * \retval           TEMPERATURE_RC_NULL_POINTER if temperatures is NULL.
 * \retval           TEMPERATURE_RC_OUT_OF_BOUNDS if index is greater than the total number of temperatures or if the size is too big.
 */
enum TemperatureReturnCode temperature_api_update_temperatures(size_t index, const celsius *temperatures, size_t size);

/*!
 * \brief            Flag the health of a single NTC channel.
 *
 * \details          Set by whatever samples the NTC, since only that layer knows
 *                  the pull-up rail the reading has to be compared against.
 *                  Channels that are not #TEMPERATURE_STATUS_OK are excluded from
 *                  the min, max and average of the pack.
 *
 * \param[in]        index The index of the channel.
 * \param[in]        status The health to record.
 *
 * \retval           TEMPERATURE_RC_OK on success.
 * \retval           TEMPERATURE_RC_OUT_OF_BOUNDS if index is out of range.
 */
enum TemperatureReturnCode temperature_api_update_temperature_status(size_t index, enum TemperatureStatus status);

/*!
 * \brief            Get the health of a single NTC channel.
 *
 * \param[in]        index The index of the channel.
 *
 * \returns          enum TemperatureStatus The recorded health, TEMPERATURE_STATUS_OPEN if index is out of range.
 */
enum TemperatureStatus temperature_api_get_channel_status(size_t index);

/*!
 * \brief            Get every faulty NTC channel as a bitmask.
 *
 * \details          Bit n is set when channel n is not #TEMPERATURE_STATUS_OK,
 *                  the same shape as bms_monitor_api_check_open_wire().
 *
 * \returns          uint32_t The bitmask of faulty channels, 0 if all are healthy.
 */
uint32_t temperature_api_get_fault_bitmask(void);

/*!
 * \brief            Get the minimum cell temperature.
 *
 * \note             Channels flagged open or shorted are skipped. Returns 0 °C if
 *                  no channel is healthy.
 *
 * \returns           celsius The minimum temperature in °C.
 */
celsius temperature_api_get_min(void);

/*!
 *
 * \brief            Get the maximum cell temperature.
 * \returns          celsius The maximum temperature in °C.
 */
celsius temperature_api_get_max(void);

/*!
 *
 * \brief            Get the average cell temperature.
 * \returns          celsius The average temperature in °C.
 */
celsius temperature_api_get_average(void);

/*!
 * \brief            Copy a list of adjacent temperatures.
 *
 * \param[out]       out A pointer to the array where the temperatures are copied into.
 * \param[in]        strart The index of the first index to copy.
 * \param[in]        size The number of temperatures that should be copied.
 *
 * \retval           TEMPERATURE_RC_OK on success.
 * \retval           TEMPERATURE_RC_NULL_POINTER if out is NULL.
 * \retval           TEMPERATURE_RC_OUT_OF_BOUNDS if index is greater than the total number of temperatures or if the size is too big.
 */
enum TemperatureReturnCode temperature_api_dump_temperatures(celsius *out, size_t start, size_t size);

/*!
 * \brief            Convert an NTC divider voltage to a temperature.
 *
 * \details          Single calibrated conversion shared by every NTC source
 *                   (MCU ADC and LTC GPIOs) so all channels read on the same
 *                   scale. The input voltage is clamped to the fitted range.
 *
 * \param[in]        value NTC voltage in V.
 * \returns          celsius The temperature in °C.
 */
celsius temperature_api_volt_to_celsius(volt value);

#else /*! CONFIG_TEMPERATURE_MODULE_ENABLE */

#define temperature_api_init() (TEMPERATURE_RC_OK)
#define temperature_api_update_temperature(index, temperature) (TEMPERATURE_RC_OK)
#define temperature_api_update_temperature_status(index, status) (TEMPERATURE_RC_OK)
#define temperature_api_get_channel_status(index) (TEMPERATURE_STATUS_OK)
#define temperature_api_get_fault_bitmask() (0U)
#define temperature_api_update_temperatures(index, temperatures, size) (TEMPERATURE_RC_OK)
#define temperature_api_get_min() (0.F)
#define temperature_api_get_max() (0.F)
#define temperature_api_get_average() (0.F)
#define temperature_api_dump_temperatures(out, start, size) (TEMPERATURE_RC_OK)
#define temperature_api_volt_to_celsius(value) (0.F)

#endif /*! CONFIG_TEMPERATURE_MODULE_ENABLE */

#endif /*! TEMPERATURE_API_H */
