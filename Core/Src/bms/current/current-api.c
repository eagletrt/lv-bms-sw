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
#include "voltage-api.h"

#ifdef CONFIG_CURRENT_MODULE_ENABLE

EAGLETRT_STATIC struct CurrentHandler current_handler;

enum CurrentReturnCode current_api_init(void) {
    current_handler = (struct CurrentHandler){ 0 };

    return CURRENT_RC_OK;
}

enum CurrentReturnCode current_api_set_cells_output_current(ampere current) {
    current_handler.cell_out_current = current;

    return CURRENT_RC_OK;
}

ampere current_api_get_cells_output_current(void) {
    return current_handler.cell_out_current;
}

kilowatt current_api_get_power() {
    return voltage_api_get_sum() * current_handler.cell_out_current;
}

#endif /*! CONFIG_CURRENT_MODULE_ENABLE */
