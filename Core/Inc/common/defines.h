/*!
 * \file            defines.h
 * \date            2026-03-31
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           LV BMS board definition.
 */

#ifndef DEFINES_H
#define DEFINES_H

/*!
 * \defgroup        cells Cell-related definitions.
 * \{
 */

#define DEFINES_CELLS_SERIES_COUNT (6U)                                                 /*!< The number of cells in series */
#define DEFINES_CELLS_PARALLEL_COUNT (3U)                                               /*!< The number of cells in parallel */
#define DEFINES_CELLS_COUNT (DEFINES_CELLS_SERIES_COUNT * DEFINES_CELLS_PARALLEL_COUNT) /*!< The number of all cells */
#define DEFINES_CELLS_NTC_COUNT (12U)                                                   /*!< The number of NTCs connected to the cells */

/*!
 * \}
 */

/*!
 * \defgroup        ntc NTC-related definitions.
 *
 * \warning         Currently they are possible values, not the actual ones. Datasheet is missinig
 *
 * \{
 */

#define DEFINES_NTC_COUNT (12U)         /*!< The number of NTCs (12 across the 18 cells). */
#define DEFINES_NTC_VDD (3.3F)          /*!< Supply voltage for the NTC circuit in V. */
#define DEFINES_NTC_R0 (10000.F)        /*!< Nominal NTC resistance at 25 °C in Ohm. */
#define DEFINES_NTC_BETA (3950.F)       /*!< NTC Beta parameter in K. */
#define DEFINES_NTC_T0_KELVIN (298.15F) /*!< Reference temperature in K. */

/*!
 * \defgroup        ntc_mux NTC analog multiplexer.
 *
 * \details         Every NTC of the pack is routed through a single 16:1 analog
 *                  multiplexer whose output is wired to NTC_SENSE_MCU
 *                  (PB2 / ADC1_IN19). The channel is selected with the four
 *                  address lines MUX_A0..MUX_A3:
 *
 *                  | Line   | Pin  | Weight |
 *                  |--------|------|--------|
 *                  | MUX_A0 | PD0  | 1      |
 *                  | MUX_A1 | PA3  | 2      |
 *                  | MUX_A2 | PA4  | 4      |
 *                  | MUX_A3 | PA5  | 8      |
 *
 *                  Mux channel \c n carries NTC \c n, so the mux channel index is
 *                  also the index used by the temperature module. Only the first
 *                  #DEFINES_CELLS_NTC_COUNT channels are populated; the remaining
 *                  ones are left unread.
 *
 * \{
 */

#define DEFINES_NTC_MUX_CHANNEL_COUNT (16U)                  /*!< Number of channels the multiplexer can address. */
#define DEFINES_NTC_MUX_USED_CHANNEL_COUNT DEFINES_NTC_COUNT /*!< Number of multiplexer channels actually wired to an NTC. */

/*! \} */

/*!
 * \brief           Number of LTC GPIO/auxiliary channels (GPIO1..GPIO4), read
 *                  through the LTC auxiliary ADC.
 *
 * \note            Since the NTCs moved onto the MCU multiplexer these channels
 *                  are no longer temperature sources: they are read as plain
 *                  voltages only.
 */
#define DEFINES_LTC_GPIO_COUNT (4U)

/*!
 * \}
 */

/*!
 * \defgroup        sense Analog sense front-end gains.
 *
 * \details         Each constant is the gain of the analog front-end between the
 *                  measured quantity and the MCU pin, i.e.
 *                  <tt>V_pin = quantity * gain</tt>. The ADC layer divides the
 *                  measured pin voltage by these to recover the physical value.
 *
 * \warning         These are placeholders set to unity because the divider and
 *                  shunt values are not in this repository: until they are filled
 *                  in from the schematic every adc_get_* getter below returns the
 *                  voltage measured at the MCU pin, not the rail value. Only these
 *                  constants have to change, no code.
 *
 * \{
 */

#define DEFINES_SENSE_VIN_GAIN (1.F)         /*!< VIN_SENSE_MCU divider ratio [V/V]. */
#define DEFINES_SENSE_VIN_UNFUSED_GAIN (1.F) /*!< VIN_UNFUSED_SENSE_MCU divider ratio [V/V]. */
#define DEFINES_SENSE_VSUP_GAIN (1.F)        /*!< VSUP_SENSE_MCU divider ratio [V/V]. */
#define DEFINES_SENSE_VOUT_GAIN (1.F)        /*!< VOUT_FB_MCU divider ratio [V/V]. */
#define DEFINES_SENSE_LVMS_OUT_GAIN (1.F)    /*!< LVMS_OUT_SENSE divider ratio [V/V]. */
#define DEFINES_SENSE_MCU_5V_GAIN (1.F)      /*!< MCU_5V_SENSE divider ratio [V/V]. */
#define DEFINES_SENSE_V_CHRG_GAIN (1.F)      /*!< V_CHRG_SENSE_MCU divider ratio [V/V]. */
#define DEFINES_SENSE_I_OUT_GAIN (1.F)       /*!< I_OUT_SENSE transimpedance [V/A]. */
#define DEFINES_SENSE_I_CHRG_GAIN (1.F)      /*!< I_CHRG_SENSE_MCU transimpedance [V/A]. */

/*! \} */
#define DEFINES_LTC_COUNT (1U) /*!< The number of LTCs */

#endif /*! DEFINES_H */
