/*!
 * \file            bms_monitor.h
 * \date            2026-07-19
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           BMS monitor data strucutres.
 */

#ifndef BMS_MONITOR_H
#define BMS_MONITOR_H

#include "ltc6810-2.h"

#include "types.h"
#include "defines.h"

#define BMS_MONITOR_ADC_RESOLUTION (12)

/*!
 * \brief            Return codes for the BMS monitor functions.
 */
enum BmsMonitorReturnCode {
    BMS_MONITOR_RC_OK,                  /*!< Function executed successfully */
    BMS_MONITOR_RC_NULL_POINTER,        /*!< Unexpected NULL pointer */
    BMS_MONITOR_RC_ENCODE_ERROR,        /*!< Data could not be encoded correctly */
    BMS_MONITOR_RC_DECODE_ERROR,        /*!< Data could not be decoded correctly */
    BMS_MONITOR_RC_COMMUNICATION_ERROR, /*!< Communication error with the LTC */
    BMS_MONITOR_RC_INVALID_ARGUMENT,    /*!< Given argument isn't a valid one */
    BMS_MONITOR_RC_BUSY,                /*!< The monitor or the peripheral is busy */
    BMS_MONITOR_RC_ERROR                /*!< A generic error */
};

/*!
 * \brief             List of voltage registers.
 */
enum BmsMonitorVoltageRegister {
    BMS_MONITOR_VOLTAGE_REGISTER_A = LTC6810_2_CVAR,
    BMS_MONITOR_VOLTAGE_REGISTER_B = LTC6810_2_CVBR,
    BMS_MONITOR_VOLTAGE_REGISTER_COUNT = LTC6810_2_CVXR_COUNT,
};

/*!
 * \brief             List of open wire procedure operations.
 */
enum BmsMonitorOpenWireOperation {
    BMS_MANAGER_OPEN_WIRE_OPERATION_PUD = LTC6810_2_PUP_INACTIVE,
    BMS_MANAGER_OPEN_WIRE_OPERATION_PUP = LTC6810_2_PUP_ACTIVE
};

/**
 * \brief            Callback used to send data via SPI.
 *
 * \param[in]        data A pointer to the data to send.
 * \param[in]        size The length of the data in bytes.
 *
 * \retval           BMS_MONITOR_RC_OK on success.
 * \retval           BMS_MONITOR_RC_NULL_POINTER data is NULL.
 * \retval           BMS_MONITOR_RC_COMMUNICATION_ERROR if there is an error during the transmission of the data.
 */
typedef enum BmsMonitorReturnCode (*bms_monitor_send_callback)(uint8_t *const data, const size_t size);

/**
 * \brief            Callback used to send and receive data via SPI.
 *
 * \param[in]        data A pointer to the data to send.
 * \param[out]       out A pointer where the received data is stored.
 * \param[in]        size The length of the sent data in bytes.
 * \param[in]        out_size The length of the received data in bytes.
 *
 * \retval           BMS_MONITOR_RC_OK on success.
 * \retval           BMS_MONITOR_RC_NULL_POINTER data or out is NULL.
 * \retval           BMS_MONITOR_RC_COMMUNICATION_ERROR if there is an error during the transmission of the data.
 */
typedef enum BmsMonitorReturnCode (*bms_monitor_send_receive_callback)(uint8_t *const data, uint8_t *out, const size_t size, const size_t out_size);

/*!
 * \brief            Callback used to read the raw NTC current for a given channel.
 *
 * \param[in]        channel The NTC channel index (0 to DEFINES_NTC_COUNT-1).
 * \param[out]       raw Pointer to store the raw ADC value.
 *
 * \retval           BMS_MONITOR_RC_OK on success.
 * \retval           BMS_MONITOR_RC_COMMUNICATION_ERROR if read failed.
 */
typedef enum BmsMonitorReturnCode (*bms_monitor_ntc_read_callback)(size_t channel, raw_ampere *raw);

/*!
 * \brief            BMS monitor handler structure.
 */
struct BmsMonitorHandler {
    bms_monitor_send_callback send;                 /*!< A pointer to the callback used to send the data via SPI */
    bms_monitor_send_receive_callback send_receive; /*!< A pointer to the callback used to send the data via SPI */
    struct Ltc68102Handler ltc_handler;             /*!< The LTC handler strucure */
    struct Ltc68102Cfgr actual_configuration;       /*!< The actual configuration register read from the LTC */
    struct Ltc68102Cfgr requested_configuration;    /*!< The requested configuration register of the LTC */
    volt pup[2U][DEFINES_CELLS_SERIES_COUNT];       /*!< An array of voltages read with pull-up and pull-down for the open-wire check */
    bms_monitor_ntc_read_callback ntc_read;         /*!< Callback to read NTC current for a given channel */
};

#endif /*! BMS_MONITOR_H */
