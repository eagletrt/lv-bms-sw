/*!
 * \file            bms_monitor_api.c
 * \date            2026-07-19
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           BMS monitor APIs.
 */

#include "bms-monitor.h"
#include "bms-monitor-api.h"

#include <stdint.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "eagletrt.h"
#include "defines.h"
#include "ltc6810-2.h"
#include "ltc6810-2-api.h"
#include "bms-monitor.h"
#include "bms-monitor-api.h"
#include "types.h"
#include "voltage-api.h"
#include "temperature-api.h"
#include "current-api.h"

#ifdef CONFIG_BMS_MONITOR_MODULE_ENABLE

EAGLETRT_STATIC struct BmsMonitorHandler bms_monitor_handler;

/*!
 * \brief           Check if there are adjacent or invalid cells.
 *
 * \returns         bool True if valid, false otherwise.
 */
EAGLETRT_STATIC_INLINE bool prv_bms_monitor_api_is_cells_bitmask_valid(uint8_t cells) {
    return cells & (cells << 1U) || cells & 0b11000000;
}

/*!
 * \brief           Compute the cell temperature based on the NTC current.
 *
 * \details         The conversion is based on two physical principles:
 *
 *                  1. Ohm's Law:
 *                     R_NTC = V_DD / I
 *                     Determines the NTC resistance (R_NTC) from the fixed supply voltage (V_DD)
 *                     and the measured current (I).
 *
 *                  2. Beta Parameter Model:
 *                     1 / T = (1 / T0) + (1 / Beta) * ln(R_NTC / R0)
 *                     Models the semiconductor's exponential non-linear drop in resistance
 *                     as temperature increases, yielding absolute temperature (T) in K.
 *
 *                  3. Kelvin to Celsius scale convertion:
 *                     T_celsius = T - 273.15
 *
 * \param[in]       current Measured current flowing through the NTC sensor in C.
 *
 * \return          Calculated cell temperature in °C.
 */
EAGLETRT_STATIC_INLINE __attribute__((unused)) celsius prv_bms_monitor_api_compute_temperature(ampere current) {
    float ntc_resistance = DEFINES_NTC_VDD / current;

    float steinhart = logf(ntc_resistance / DEFINES_NTC_R0) / DEFINES_NTC_BETA;
    steinhart += (1.0F / DEFINES_NTC_T0_KELVIN);

    return (1.0F / steinhart) - 273.15F;
}

enum BmsMonitorReturnCode bms_monitor_api_init(bms_monitor_send_callback send, bms_monitor_send_receive_callback send_receive, bms_monitor_ntc_read_callback ntc_read) {
    if (send == NULL || send_receive == NULL) {
        return BMS_MONITOR_RC_NULL_POINTER;
    }

    memset(&bms_monitor_handler, 0, sizeof(bms_monitor_handler));

    /*! Set callbacks */
    bms_monitor_handler.send = send;
    bms_monitor_handler.send_receive = send_receive;
    bms_monitor_handler.ntc_read = ntc_read;
    /*! Initalize the LTC driver */
    ltc6810_2_api_init(&bms_monitor_handler.ltc_handler, DEFINES_LTC_COUNT);
    bms_monitor_handler.requested_configuration.REFON = 1U;

    return BMS_MONITOR_RC_OK;
}

enum BmsMonitorReturnCode bms_monitor_api_write_configuration(void) {
    uint8_t command[LTC6810_2_WRITE_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };

    size_t byte_size = ltc6810_2_api_wrcfg_encode_broadcast(
        &bms_monitor_handler.ltc_handler,
        &bms_monitor_handler.requested_configuration,
        command);
    if (byte_size != LTC6810_2_WRITE_BUFFER_SIZE(DEFINES_LTC_COUNT)) {
        return BMS_MONITOR_RC_ENCODE_ERROR;
    }

    enum BmsMonitorReturnCode code = bms_monitor_handler.send(command, byte_size);
    if (code != BMS_MONITOR_RC_OK) {
        return code;
    }

    return code;
}

enum BmsMonitorReturnCode bms_monitor_api_read_configuration(void) {
    uint8_t command[LTC6810_2_WRITE_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };

    size_t byte_count = ltc6810_2_api_rdcfg_encode_broadcast(&bms_monitor_handler.ltc_handler, command);
    if (byte_count != LTC6810_2_READ_BUFFER_SIZE) {
        return BMS_MONITOR_RC_ENCODE_ERROR;
    }

    uint8_t data[LTC6810_2_DATA_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };

    enum BmsMonitorReturnCode code = bms_monitor_handler.send_receive(
        command,
        data,
        byte_count,
        LTC6810_2_DATA_BUFFER_SIZE(DEFINES_LTC_COUNT));
    if (code != BMS_MONITOR_RC_OK) {
        return code;
    }

    size_t byte_size = ltc6810_2_api_rdcfg_decode_broadcast(
        &bms_monitor_handler.ltc_handler,
        data,
        &bms_monitor_handler.actual_configuration);
    if (byte_size != LTC6810_2_DATA_BUFFER_SIZE(DEFINES_LTC_COUNT)) {
        return BMS_MONITOR_RC_DECODE_ERROR;
    }

    return BMS_MONITOR_RC_OK;
}

enum BmsMonitorReturnCode bms_monitor_api_start_volt_conversion(void) {
    uint8_t command[LTC6810_2_WRITE_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };

    size_t byte_size = ltc6810_2_api_adcv_encode_broadcast(
        &bms_monitor_handler.ltc_handler,
        LTC6810_2_MD_27KHZ,
        LTC6810_2_DCP_DISABLED,
        LTC6810_2_CH_ALL,
        command);
    if (byte_size != LTC6810_2_POLL_BUFFER_SIZE) {
        return BMS_MONITOR_RC_ENCODE_ERROR;
    }

    enum BmsMonitorReturnCode code = bms_monitor_handler.send(command, byte_size);
    if (code != BMS_MONITOR_RC_OK) {
        return code;
    }

    return code;
}

enum BmsMonitorReturnCode bms_monitor_api_start_open_wire_conversion(enum Ltc68102Pup pull_up) {
    uint8_t command[LTC6810_2_WRITE_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };

    size_t byte_size = ltc6810_2_api_adow_encode_broadcast(
        &bms_monitor_handler.ltc_handler,
        LTC6810_2_MD_27KHZ,
        pull_up,
        LTC6810_2_DCP_DISABLED,
        LTC6810_2_CH_ALL,
        command);
    if (byte_size != LTC6810_2_POLL_BUFFER_SIZE) {
        return BMS_MONITOR_RC_ENCODE_ERROR;
    }

    enum BmsMonitorReturnCode code = bms_monitor_handler.send(command, byte_size);
    if (code != BMS_MONITOR_RC_OK) {
        return code;
    }

    return BMS_MONITOR_RC_OK;
}

enum BmsMonitorReturnCode bms_monitor_api_read_voltages(enum BmsMonitorVoltageRegister reg) {
    uint8_t command[LTC6810_2_WRITE_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };
    uint8_t data[LTC6810_2_DATA_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };
    raw_volt voltages[LTC6810_2_CELL_COUNT] = { 0 };

    size_t byte_size = ltc6810_2_api_rdcv_encode_broadcast(
        &bms_monitor_handler.ltc_handler,
        (enum Ltc68102Cvxr)reg,
        command);
    if (byte_size != LTC6810_2_READ_BUFFER_SIZE) {
        return BMS_MONITOR_RC_ENCODE_ERROR;
    }

    enum BmsMonitorReturnCode code = bms_monitor_handler.send_receive(command, data, byte_size, LTC6810_2_DATA_BUFFER_SIZE(DEFINES_CELLS_SERIES_COUNT));
    if (code != BMS_MONITOR_RC_OK) {
        return code;
    }

    byte_size = ltc6810_2_api_rdcv_decode_broadcast(&bms_monitor_handler.ltc_handler, data, voltages);
    if (byte_size != LTC6810_2_DATA_BUFFER_SIZE(DEFINES_LTC_COUNT)) {
        return BMS_MONITOR_RC_DECODE_ERROR;
    }

    for (size_t i = 0U; i < LTC6810_2_CELL_COUNT; ++i) {
        voltage_api_update_voltage(i, BMS_MONITOR_API_RAW_VOLTAGE_TO_VOLT(voltages[i]));
    }

    return BMS_MONITOR_RC_OK;
}

enum BmsMonitorReturnCode bms_monitor_api_read_currents(void) {
    raw_ampere raw_currents[DEFINES_NTC_COUNT] = { 0 };

    if (bms_monitor_handler.ntc_read == NULL) {
        return BMS_MONITOR_RC_NULL_POINTER;
    }

    for (size_t i = 0U; i < DEFINES_NTC_COUNT; ++i) {
        enum BmsMonitorReturnCode code = bms_monitor_handler.ntc_read(i, &raw_currents[i]);
        if (code != BMS_MONITOR_RC_OK) {
            return code;
        }
    }

    for (size_t i = 0U; i < DEFINES_NTC_COUNT; ++i) {
        ampere current = BMS_MONITOR_API_RAW_CURRENT_TO_AMPERE(raw_currents[i]);
        current_api_update_current(i, current);
    }

    return BMS_MONITOR_RC_OK;
}

enum BmsMonitorReturnCode bms_monitor_api_read_temperatures(void) {
    ampere currents[DEFINES_NTC_COUNT] = { 0.F };
    current_api_dump_currents(currents, 0U, DEFINES_NTC_COUNT);

    for (size_t i = 0U; i < DEFINES_NTC_COUNT; ++i) {
        celsius temperature = prv_bms_monitor_api_compute_temperature(currents[i]);
        temperature_api_update_temperature(i, temperature);
    }

    return BMS_MONITOR_RC_OK;
}

enum BmsMonitorReturnCode bms_monitor_api_read_open_wire_voltages(enum BmsMonitorVoltageRegister reg, enum BmsMonitorOpenWireOperation operation) {
    uint8_t command[LTC6810_2_WRITE_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };
    uint8_t data[LTC6810_2_DATA_BUFFER_SIZE(DEFINES_LTC_COUNT)] = { 0 };
    raw_volt voltages[LTC6810_2_CELL_COUNT] = { 0 };

    size_t byte_size = ltc6810_2_api_rdcv_encode_broadcast(
        &bms_monitor_handler.ltc_handler,
        (enum Ltc68102Cvxr)reg,
        command);
    if (byte_size != LTC6810_2_READ_BUFFER_SIZE) {
        return BMS_MONITOR_RC_ENCODE_ERROR;
    }

    enum BmsMonitorReturnCode code = bms_monitor_handler.send_receive(
        command,
        data,
        byte_size,
        LTC6810_2_DATA_BUFFER_SIZE(DEFINES_LTC_COUNT));
    if (code != BMS_MONITOR_RC_OK) {
        return code;
    }

    byte_size = ltc6810_2_api_rdcv_decode_broadcast(&bms_monitor_handler.ltc_handler, data, voltages);
    if (byte_size != LTC6810_2_DATA_BUFFER_SIZE(DEFINES_LTC_COUNT)) {
        return BMS_MONITOR_RC_DECODE_ERROR;
    }

    for (size_t i = 0U; i < LTC6810_2_CELL_COUNT; ++i) {
        bms_monitor_handler.pup[operation][i] = BMS_MONITOR_API_RAW_VOLTAGE_TO_VOLT(voltages[i]);
    }

    return BMS_MONITOR_RC_OK;
}

enum BmsMonitorReturnCode bms_monitor_api_set_discharge(uint8_t cells) {
    if (prv_bms_monitor_api_is_cells_bitmask_valid(cells)) {
        return BMS_MONITOR_RC_INVALID_ARGUMENT;
    }

    bms_monitor_handler.requested_configuration.DCTO = (cells == 0U) ? LTC6810_2_DCTO_OFF : LTC6810_2_DCTO_30S;
    bms_monitor_handler.requested_configuration.DCC = cells;

    return BMS_MONITOR_RC_OK;
}

uint16_t bms_monitor_api_get_discharge(void) {
    return bms_monitor_handler.actual_configuration.DCC;
}

uint32_t bms_monitor_api_check_open_wire(void) {
    uint32_t open_wire = 0U;
    constexpr volt open_wire_epsilon = 0.000005F;
    constexpr volt open_wire_threshold_volt = LTC6810_2_OPEN_WIRE_THRESHOLD_MV * 1000;

    if (fabs(bms_monitor_handler.pup[LTC6810_2_PUP_ACTIVE][0U]) <= open_wire_epsilon) {
        open_wire = EAGLETRT_API_BIT_SET(open_wire, 0U);
    }

    if (fabs(bms_monitor_handler.pup[LTC6810_2_PUP_INACTIVE][DEFINES_CELLS_SERIES_COUNT - 1U]) <= open_wire_epsilon) {
        open_wire = EAGLETRT_API_BIT_SET(open_wire, DEFINES_CELLS_SERIES_COUNT - 1U);
    }

    for (size_t i = 1U; i < DEFINES_CELLS_SERIES_COUNT; ++i) {
        const volt delta_v = bms_monitor_handler.pup[LTC6810_2_PUP_ACTIVE][i] - bms_monitor_handler.pup[LTC6810_2_PUP_INACTIVE][i];
        if (delta_v < open_wire_threshold_volt) {
            open_wire = EAGLETRT_API_BIT_SET(open_wire, i);
        }
    }

    return open_wire;
}

#endif /*! CONFIG_BMS_MONITOR_MODULE_ENABLE */
