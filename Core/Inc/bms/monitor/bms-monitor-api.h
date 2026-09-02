/*!
 * \file            bms_monitor_api.h
 * \date            2026-07-19
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           BMS monitor APIs.
 */

#ifndef BMS_MONITOR_API_H
#define BMS_MONITOR_API_H

#include "bms-monitor.h"

#include "config.h"

#define BMS_MONITOR_API_RAW_VOLTAGE_TO_VOLT(raw_value) ((volt)((raw_value) * 0.0001F))
#define BMS_MONITOR_API_RAW_CURRENT_TO_AMPERE(raw_value) ((ampere)((raw_value) * (DEFINES_NTC_VDD / (float)((1U << BMS_MONITOR_ADC_RESOLUTION) - 1U))))

#ifdef CONFIG_BMS_MONITOR_MODULE_ENABLE

/*!
 * \brief           Initialize the BMS internal handler structure.
 *
 * \param[in]       send A pointer to the callback used to send data via SPI.
 * \param[in]       send_receive A pointer to the callback used to send and receive data via SPI.
 * \param[in]       ntc_read Callback that reads raw NTC ADC value for a given channel (can be NULL).
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_NULL_POINTER if send or send_receive is NULL.
 */
enum BmsMonitorReturnCode bms_monitor_api_init(bms_monitor_send_callback send, bms_monitor_send_receive_callback send_receive, bms_monitor_ntc_read_callback ntc_read);

/*!
 * \brief           Write the configuration registers of the LTC.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_ENOCDE_ERROR if there was an error while encoding the command.
 * \retval          BMS_MONITOR_RC_DEOCDE_ERROR if there was an error while decoding the data.
 * \retval          BMS_MONITOR_RC_COMMUNICATION_ERROR if there is an error during the transmission of the data.
 * \retval          BMS_MONITOR_RC_BBUSY the monitor or the peripheral is busy.
 */
enum BmsMonitorReturnCode bms_monitor_api_write_configuration(void);

/*!
 * \brief           Read the configuration registers of the LTC.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_ENOCDE_ERROR if there was an error while encoding the command.
 * \retval           BMS_MONITOR_RC_DEOCDE_ERROR if there was an error while decoding the data.
 * \retval          BMS_MONITOR_RC_COMMUNICATION_ERROR if there is an error during the transmission of the data.
 * \retval          BMS_MONITOR_RC_BBUSY the monitor or the peripheral is busy.
 */
enum BmsMonitorReturnCode bms_monitor_api_read_configuration(void);

/*!
 * \brief           Start the cell voltage ADC convertion.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_ENOCDE_ERROR if there was an error while encoding the command.
 * \retval          BMS_MONITOR_RC_COMMUNICATION_ERROR if there is an error during the transmission of the data.
 * \retval          BMS_MONITOR_BUSY the monitor or the peripheral is busy.
 */
enum BmsMonitorReturnCode bms_monitor_api_start_volt_conversion(void);

/*!
 * \brief           Start the open wire ADC convertion.
 *
 * \param[in]       pull_up Pull-up/pull-down option to select.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_ENOCDE_ERROR if there was an error while encoding the command.
 * \retval          BMS_MONITOR_RC_COMMUNICATION_ERROR if there is an error during the transmission of the data.
 * \retval          BMS_MONITOR_RC_BUSY the monitor or the peripheral is busy.
 */
enum BmsMonitorReturnCode bms_monitor_api_start_open_wire_conversion(enum Ltc68102Pup pull_up);

/**
 * \brief           Read the cells voltages from the BMS monitor.
 *
 * \param[in]       reg The register to read from.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_ENOCDE_ERROR if there was an error while encoding the command.
 * \retval          BMS_MONITOR_RC_DEOCDE_ERROR if there was an error while decoding the data.
 * \retval          BMS_MONITOR_RC_COMMUNICATION_ERROR if there is an error during the transmission of the data.
 * \retval          BMS_MONITOR_RC_BUSY the monitor or the peripheral is busy.
 */
enum BmsMonitorReturnCode bms_monitor_api_read_voltages(enum BmsMonitorVoltageRegister reg);

/*!
 * \brief           Read all NTC currents and store them via the current API.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_NULL_POINTER if the NTC read callback has not been set.
 */
enum BmsMonitorReturnCode bms_monitor_api_read_currents(void);

/*!
 * \brief           Read all NTC temperatures and store them via the temperature API.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 */
enum BmsMonitorReturnCode bms_monitor_api_read_temperatures(void);

/**
 * \brief           Read the cells voltages after the opern wire conversiont from the LTC.
 *
 * \param[in]       reg The register to read from.
 * \param[in]       operation The type of operation completed before the the readings.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_ENOCDE_ERROR if there was an error while encoding the command.
 * \retval          BMS_MONITOR_RC_DEOCDE_ERROR if there was an error while decoding the data.
 * \retval          BMS_MONITOR_RC_COMMUNICATION_ERROR if there is an error during the transmission of the data.
 * \retval          BMS_MONITOR_RC_BUSY the monitor or the peripheral is busy.
 */
enum BmsMonitorReturnCode bms_monitor_api_read_open_wire_voltages(enum BmsMonitorVoltageRegister reg, enum BmsMonitorOpenWireOperation operation);

/*!
 * \brief           Set the cells to discharge.
 *
 * \param[in]       The bitmask representing wich where the n-bit is representing the n-th cell.
 *
 * \retval          BMS_MONITOR_RC_INVALID_ARGUMENT if there are two ore more adjacent cells in discharge.
 * \retval          BMS_MANAGER_RC_OK on success.
 */
enum BmsMonitorReturnCode bms_monitor_api_set_discharge(uint8_t cells);

/*!
 * \brief           Get the cells that are being currently discharged.
 *
 * \returns         uint16_t The bitmask representing wich where the n-bit is representing the n-th cell.
 */
uint16_t bms_monitor_api_get_discharge(void);

/*!
 * \brief Check for open wires
 *
 * \details To check for open wire the delta between the converted voltages values
 * is calculated for all the cells excluded the first, then the open wire
 * is detected if:
 *     - The first pull up voltage value is 0.0000 (an epsilon is used to avoid float precision errors)
 *     - The last pull-down voltage value is 0.0000 (same as above)
 *     - At least one delta voltage value is below the -400 mV threshold
 *
 * \returns bit_flag32 A bitmask where the n-th bit represents the n-th cell that has an open wire
 */
uint32_t bms_monitor_api_check_open_wire(void);

/*!
 * \brief           Start the GPIO (auxiliary) ADC conversion on the LTC.
 *
 * \details         Issues an ADAX command for all GPIOs. Pair it with
 *                  bms_monitor_api_read_gpios or bms_monitor_api_sample_gpios
 *                  after the conversion has completed.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_ENCODE_ERROR if the command could not be encoded.
 * \retval          BMS_MONITOR_RC_COMMUNICATION_ERROR on a transmission failure.
 */
enum BmsMonitorReturnCode bms_monitor_api_start_gpio_conversion(void);

/*!
 * \brief           Read the 4 LTC GPIO auxiliary voltages directly.
 *
 * \param[out]      out  Array receiving the GPIO1..GPIO4 voltages in V.
 * \param[in]       size Number of elements in \p out (must be >= DEFINES_LTC_GPIO_COUNT).
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_NULL_POINTER if \p out is NULL.
 * \retval          BMS_MONITOR_RC_INVALID_ARGUMENT if \p size is too small.
 * \retval          BMS_MONITOR_RC_ENCODE_ERROR / _DECODE_ERROR / _COMMUNICATION_ERROR on failure.
 */
enum BmsMonitorReturnCode bms_monitor_api_read_gpios(volt *out, size_t size);

/*!
 * \brief           Read the 4 LTC GPIO voltages and latch them inside the handler.
 *
 * \details         The NTCs of the pack are no longer wired to the LTC GPIOs:
 *                  they all sit behind the MCU analog multiplexer and are handled
 *                  by adc_routine(). These auxiliary channels are therefore kept
 *                  as plain voltages, readable with
 *                  bms_monitor_api_get_gpio_voltage(), and are not fed to the
 *                  temperature module any more.
 *
 * \retval          BMS_MONITOR_RC_OK on success.
 * \retval          BMS_MONITOR_RC_ENCODE_ERROR / _DECODE_ERROR / _COMMUNICATION_ERROR on failure.
 */
enum BmsMonitorReturnCode bms_monitor_api_sample_gpios(void);

/*!
 * \brief           Get one of the LTC GPIO voltages latched by bms_monitor_api_sample_gpios().
 *
 * \param[in]       index The GPIO index, 0..DEFINES_LTC_GPIO_COUNT-1.
 *
 * \returns         volt The GPIO voltage in V, 0 V if \p index is out of bounds.
 */
volt bms_monitor_api_get_gpio_voltage(size_t index);

#else /*! CONFIG_BMS_MONITOR_MODULE_ENABLE */

#define bms_monitor_api_init(send, send_receive, ntc_read) (BMS_MONITOR_RC_OK)
#define bsm_monitor_api_write_configuration() (BMS_MONITOR_RC_OK)
#define bms_monitor_api_read_configuration() (BMS_MONITOR_RC_OK)
#define bms_monitor_api_start_volt_covertion() (BMS_MONITOR_RC_OK)
#define bms_monitor_api_start_open_wire_covertion(pull_up) (BMS_MONITOR_RC_OK)
#define bms_monitor_api_read_voltages(reg) (BMS_MONITOR_RC_OK)
#define bms_monitor_api_read_open_wire_voltages(reg, operation) (BMS_MONITOR_RC_OK)
#define bms_monitor_api_set_discharge(cells) (BMS_MONITOR_OK)
#define bms_monitor_api_set_discharge() (0U)
#define bms_monitor_api_read_currents() (BMS_MONITOR_RC_OK)
#define bms_monitor_api_read_temperatures() (BMS_MONITOR_RC_OK)
#define bms_monitor_api_start_gpio_conversion() (BMS_MONITOR_RC_OK)
#define bms_monitor_api_read_gpios(out, size) (BMS_MONITOR_RC_OK)
#define bms_monitor_api_sample_gpios() (BMS_MONITOR_RC_OK)
#define bms_monitor_api_get_gpio_voltage(index) (0.F)

#endif /*! CONFIG_BMS_MONITOR_MODULE_ENABLE */

#endif /*! BMS_MONITOR_API_H */
