/*!
 * \file            balancing-api.c
 * \date            2026-09-08
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Cells balancing module APIs.
 */

#include "balancing.h"
#include "balancing-api.h"

#include <stdint.h>
#include <string.h>

#include "config.h"
#include "defines.h"
#include "eagletrt.h"
#include "eagletrt.h"
#include "voltage-api.h"
#include "bms-monitor-api.h"

#ifdef CONFIG_BALANCING_MODULE_ENABLE

EAGLETRT_STATIC struct BalancingHandler balancing_handler; /*!< Private balancing handler instance. */

/*!
 * \brief            Check whether a cell should be discharged.
 *
 * \param[in]        voltage The cell voltage in V.
 *
 * \returns          true if the cell sits more than threshold above target.
 */
EAGLETRT_STATIC bool prv_balancing_api_cell_need_discharge(volt voltage) {
    return (voltage > balancing_handler.target) &&
           ((voltage - balancing_handler.target) > balancing_handler.threshold);
}

/*!
 * \brief            Compute the bitmask of cells to discharge.
 *
 * \param[in]        odd_parity true to consider odd-indexed cells, false for even-indexed ones.
 *
 * \returns          uint8_t The cell bitmask, bit 0 being the first cell.
 */
EAGLETRT_STATIC uint8_t prv_balancing_api_compute_mask(bool odd_parity) {
    volt voltages[DEFINES_CELLS_SERIES_COUNT] = { 0.F };
    if (voltage_api_dump_voltages(voltages, 0U, DEFINES_CELLS_SERIES_COUNT) != VOLTAGE_RC_OK) {
        return 0U;
    }

    uint8_t mask = 0U;
    for (size_t i = 0U; i < DEFINES_CELLS_SERIES_COUNT; ++i) {
        if (prv_balancing_api_cell_need_discharge(voltages[i]) && (((i & 1U) != 0U) == odd_parity)) {
            mask |= (uint8_t)(1U << i);
        }
    }

    return mask;
}

enum BalancingReturnCode balancing_api_init(void) {
    memset(&balancing_handler, 0, sizeof(balancing_handler));
    return BALANCING_RC_OK;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
enum BalancingReturnCode balancing_api_start(volt target, volt threshold) {
    if (target < VOLTAGE_MIN_V || target > VOLTAGE_MAX_V) {
        return BALANCING_RC_OUT_OF_BOUNDS;
    }

    balancing_handler.target = target;
    balancing_handler.threshold = threshold;
    balancing_handler.odd_phase = false;
    balancing_handler.last_run = 0U;
    balancing_handler.is_active = true;

    return BALANCING_RC_OK;
}

enum BalancingReturnCode balancing_api_stop(void) {
    (void)bms_monitor_api_set_discharge(0U);
    balancing_handler.is_active = false;
    return BALANCING_RC_OK;
}

enum BalancingReturnCode balancing_api_run(uint32_t tick) {
    if (!balancing_handler.is_active) {
        return BALANCING_RC_OK;
    }

    if ((tick - balancing_handler.last_run) < BALANCING_RUN_PERIOD_MS) {
        return BALANCING_RC_OK;
    }
    balancing_handler.last_run = tick;

    balancing_handler.odd_phase = !balancing_handler.odd_phase;
    uint8_t mask = prv_balancing_api_compute_mask(balancing_handler.odd_phase);

    if (mask == 0U) {
        balancing_handler.odd_phase = !balancing_handler.odd_phase;
        mask = prv_balancing_api_compute_mask(balancing_handler.odd_phase);
        if (mask == 0U) {
            (void)balancing_api_stop();
            return BALANCING_RC_OK;
        }
    }

    (void)bms_monitor_api_set_discharge(mask);
    return BALANCING_RC_OK;
}

bool balancing_api_is_active(void) {
    return balancing_handler.is_active;
}

#endif /*! CONFIG_BALANCING_MODULE_ENABLE */
