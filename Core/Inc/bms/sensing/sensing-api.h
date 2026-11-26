/*!
 * \file            sensing-api.h
 * \date            2026-11-25
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           TBD.
 */

#ifndef SENSING_API_H
#define SENSING_API_H

#include "sensing.h"
#include "types.h"

enum SensingReturnCode sensing_api_init(sensing_adc_start_callback start);

/*!
 * \defgroup         adc_sense Scaled readings of the analog sense front-ends.
 *
 * \details          Each getter undoes the 10 k / 18 k or 10 k / 1k3 attenuation of
 *                   its sense line (\c DEFINES_SENSE_*_GAIN in defines.h) so it
 *                   returns the voltage present on the rail itself, not the one
 *                   measured on the MCU pin.
 *
 * \note             The two current getters go one step further and also undo the
 *                   transfer function of the Hall sensor behind each line, see the
 *                   \c sense_current and \c sense_i_out groups in defines.h. The
 *                   *_sense_voltage getters expose the sensor output itself, which
 *                   is what you want when checking the zero-current offset on the
 *                   bench.
 *
 * \{
 */

volt sensing_api_get_vin(void);                  /*!< Fused input voltage in V. */
volt sensing_api_get_vin_unfused(void);          /*!< Unfused input voltage in V. */
volt sensing_api_get_vsup(void);                 /*!< Supply voltage in V. */
volt sensing_api_get_vout(void);                 /*!< Output voltage in V. */
volt sensing_api_get_lvms_out(void);             /*!< LVMS output voltage in V. */
volt sensing_api_get_mcu_5v(void);               /*!< 5 V rail voltage in V. */
volt sensing_api_get_charger_voltage(void);      /*!< Charger voltage in V. */
volt sensing_api_get_i_out_sense_voltage(void);  /*!< I_OUT_SENSED, the pack current sensor output, in V. */
volt sensing_api_get_i_chrg_sense_voltage(void); /*!< I_CHRG, the ACS724 output, in V. */
ampere sensing_api_get_charger_current(void);    /*!< Charger current in A, positive into the battery. */
ampere sensing_api_get_output_current(void);     /*!< Pack output current in A, positive out of the pack (see DEFINES_SENSE_I_OUT_DIRECTION). */

/*! \} */

enum SensingReturnCode sensing_api_routine(uint8_t task_id);

#endif /*! SENSING_API_H */
 
