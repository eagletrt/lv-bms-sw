/*!
 * \file            current-api.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Current module data structures.
 */

#include "current.h"
#include "current-api.h"

#include "types.h"
#include "eagletrt.h"
#include "eagletrt-api.h"
#include "voltage-api.h"

#ifdef CONFIG_CURRENT_MODULE_ENABLE

EAGLETRT_STATIC struct CurrentHandler current_handler;

void current_api_init(void) {
    current_handler.current = 0.F;
}

enum CurrentReturnCode current_api_update_current(ampere current) {
    current_handler.current = current;
    return CURRENT_RC_OK;
}

ampere current_api_get_current(void) {
    return current_handler.current;
}

kilowatt current_api_get_power() {
    return voltage_api_get_sum() * current_handler.current;
}

#endif /*! CONFIG_CURRENT_MODULE_ENABLE */
