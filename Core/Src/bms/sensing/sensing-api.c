/*!
 * \file            sensing-api.c
 * \date            2026-11-25
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           TBD.
 */

#include "scheduler-api.h"
#include "scheduler.h"
#include "sensing.h"
#include "sensing-api.h"

#include "eagletrt.h"
#include "eagletrt-api.h"
#include "watchdogs-api.h"
#include "watchdogs.h"

EAGLETRT_STATIC struct SensingHandler sensing_handler;

EAGLETRT_STATIC void prv_sensing_api_adc_timeout_callback(void) {
}

EAGLETRT_STATIC enum SensingReturnCode prv_sensing_api_handle_adc_state(void) {
    if (sensing_handler.adc_state == SENSING_ADC_STATE_CONVERTED) {
        // TODO:
        sensing_handler.adc_state = SENSING_ADC_STATE_IDLE;
    }

    if (sensing_handler.adc_state == SENSING_ADC_STATE_IDLE) {
        sensing_handler.adc_state = SENSING_ADC_STATE_CONVERTING;
        // TODO:
    }
    return SENSING_RC_OK;
}

enum SensingReturnCode sensing_api_init(sensing_adc_start_callback start) {
    if (!sensing_handler.is_initialized) {
        return SENSING_RC_OK;
    }

    if (start == NULL) {
        return SENSING_RC_NULL_POINTER;
    }

    sensing_handler.start = start;
    sensing_handler.is_initialized = true;
    sensing_handler.watchdog_started = false;
    EAGLETRT_API_UNUSED(watchdogs_api_init_watchdog(
        &sensing_handler.adc_watchdog,
        sensing_adc_watchdog_timeout_ms,
        prv_sensing_api_adc_timeout_callback));

    return SENSING_RC_OK;
}

enum SensingReturnCode sensing_api_routine_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    if (!sensing_handler.is_initialized) {
        return SENSING_RC_NOT_INITIALIZED;
    }

    if (!sensing_handler.watchdog_started) {
        scheduler_api_watchdog_start(&sensing_handler.adc_watchdog);
        sensing_handler.watchdog_started = true;
    }

    return prv_sensing_api_handle_adc_state();
}
