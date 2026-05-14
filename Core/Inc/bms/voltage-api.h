/*!
 * \file            voltage-api.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Voltage module APIs.
 */

#ifndef VOLTAGE_API_H
#define VOLTAGE_API_H

#include "voltage.h"
#include "config.h"

#ifdef CONFIG_VOLTAGE_MODULE_ENABLE

/*!
 * \brief            Initialize the voltage module.
 */
void voltage_api_init(void);

/*!
 * \brief            Update a single voltage value.
 * \param[in]        index The index of the voltage to update.
 * \param[in]        voltage The new voltage in V.
 * \retval           VOLTAGE_RC_OK on success.
 * \retval           VOLTAGE_RC_OUT_OF_BOUNDS if index is greater than the total number of voltages.
 */
enum VoltageReturnCode voltage_api_update_voltage(size_t index, volt voltage);

/*!
 * \brief            Update multiple voltage values.
 * \param[in]        index The start index of the voltages to update.
 * \param[in]        voltages A pointer to the array of voltages to copy.
 * \param[in]        size The number of elements to copy.
 * \retval           VOLTAGE_RC_OK on success.
 * \retval           VOLTAGE_RC_NULL_POINTER if voltages is NULL.
 * \retval           VOLTAGE_RC_OUT_OF_BOUNDS if index is greater than the total number of voltages or if the size is too big.
 */
enum VoltageReturnCode voltage_api_update_voltages(size_t index, const volt *voltages, size_t size);

/*!
 * \brief            Get the minimum cell voltage.
 * \returns          volt The minimum voltage in V.
 */
volt voltage_api_get_min(void);

/*!
 * \brief            Get the maximum cell voltage.
 * \returns          volt The maximum voltage in V.
 */
volt voltage_api_get_max(void);

/*!
 * \brief            Get the average cell voltage.
 * \returns          volt The average voltage in V.
 */
volt voltage_api_get_average(void);

/*!
 * \brief            Get the sum of the cells voltages.
 * \returns          volt The cells voltages sum.
 */
volt voltage_api_get_sum(void);

/*!
 * \brief            Copy a list of adjacent voltages.
 * \param[out]       out A pointer to the array where the voltages are copied into.
 * \param[in]        strart The index of the first index to copy.
 * \param[in]        size The number of voltages that should be copied.
 * \retval           VOLTAGE_RC_OK on success.
 * \retval           VOLTAGE_RC_NULL_POINTER if out is NULL.
 * \retval           VOLTAGE_RC_OUT_OF_BOUNDS if index is greater than the total number of voltages or if the size is too big.
 */
enum VoltageReturnCode voltage_api_dump_voltages(volt *out, size_t start, size_t size);

#else /*! CONFIG_VOLTAGE_MODULE_ENABLE */

#define voltage_api_init() (EAGLETRT_API_NOP())
#define voltage_api_update_voltage(index, voltage) (VOLTAGE_RC_OK)
#define voltage_api_update_voltages(index, voltages, size) (VOLTAGE_RC_OK)
#define voltage_api_get_min() (0.F)
#define voltage_api_get_max() (0.F)
#define voltage_api_get_average() (0.F)
#define voltage_api_dump_voltages(out, start, size) (VOLTAGE_RC_OK)

#endif /*! CONFIG_VOLTAGE_MODULE_ENABLE */

#endif /*! VOLTAGE_API_H */
