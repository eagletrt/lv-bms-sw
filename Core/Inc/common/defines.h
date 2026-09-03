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
 * \defgroup        sense Analog sense front-end.
 *
 * \details         Every sense line shares the same passive network (schematic
 *                  sheet "Feedback Voltage Divider"): a 10 kOhm series resistor
 *                  taken from the network RN1/RN2/RN3, a resistor to GND that
 *                  sets the range, a 100 nF cap to GND at the MCU pin (CN1/CN2/CN3)
 *                  and a BAT54S clamp to 3V3/GND (sheet "OVP").
 *
 *                  The shunt leg is what picks the full scale:
 *
 *                  | Shunt | Ratio  | Source at 3.3 V on the pin | Used for            |
 *                  |-------|--------|----------------------------|---------------------|
 *                  | 18 k  | 0.6429 | 5.13 V                     | 5 V-range signals   |
 *                  | 1.5 k | 0.1304 | 25.3 V                     | 24 V-range signals  |
 *
 *                  The 24 V shunt was 1k3 on the first build and is now 1.5 k,
 *                  matching the "ADC sensibility" note on the sheet (24 V ->
 *                  3.13 V at the pin). Confirmed against a live board: with 1.5 k
 *                  VSUP decodes to 23.89 V while the six cells sum to 24.20 V,
 *                  with 1k3 it read 27.08 V. The 5 V lines kept their 18 k, which
 *                  the same board confirms twice over: the 5 V rail reads 5.04 V
 *                  and the I_OUT sense node sits at exactly Vcc/2.
 *
 *                  So <tt>V_pin = V_source * ratio</tt> and the ADC layer divides
 *                  the measured pin voltage by the ratio to recover the source.
 *                  Both ranges leave the pin below the 3V3 + Vf clamp of the
 *                  BAT54S at full scale.
 *
 * \{
 */

#define DEFINES_SENSE_DIVIDER_SERIES_OHM (10000.F)   /*!< Series leg, one element of RN1/RN2/RN3. */
#define DEFINES_SENSE_DIVIDER_5V_SHUNT_OHM (18000.F) /*!< Shunt leg of the 5 V-range lines (R2, R4, R6, R9, R11). */
#define DEFINES_SENSE_DIVIDER_24V_SHUNT_OHM (1500.F) /*!< Shunt leg of the 24 V-range lines (R3, R5, R7, R8, R10, R12, R13). */

/*!
 * \brief           Attenuation of a sense line, V at the MCU pin per V at the source.
 *
 * \param           SHUNT The resistance of the leg to GND in Ohm.
 */
#define DEFINES_SENSE_DIVIDER_RATIO(SHUNT) ((SHUNT) / (DEFINES_SENSE_DIVIDER_SERIES_OHM + (SHUNT)))

#define DEFINES_SENSE_5V_RANGE_GAIN DEFINES_SENSE_DIVIDER_RATIO(DEFINES_SENSE_DIVIDER_5V_SHUNT_OHM)   /*!< 0.642857 */
#define DEFINES_SENSE_24V_RANGE_GAIN DEFINES_SENSE_DIVIDER_RATIO(DEFINES_SENSE_DIVIDER_24V_SHUNT_OHM) /*!< 0.130435 */

/*! \defgroup       sense_voltage Per-line attenuation of the voltage senses.
 *  \{
 */

#define DEFINES_SENSE_VIN_GAIN DEFINES_SENSE_24V_RANGE_GAIN         /*!< VIN_SENSED -> VIN_SENSE_MCU, R13 1.5k. */
#define DEFINES_SENSE_VIN_UNFUSED_GAIN DEFINES_SENSE_24V_RANGE_GAIN /*!< VIN_UNFUSED_SENSED -> VIN_UNFUSED_SENSE_MCU, R12 1.5k. */
#define DEFINES_SENSE_VSUP_GAIN DEFINES_SENSE_24V_RANGE_GAIN        /*!< VSUP_SENSED -> VSUP_SENSE_MCU, R7 1.5k. */
#define DEFINES_SENSE_VOUT_GAIN DEFINES_SENSE_24V_RANGE_GAIN        /*!< VOUTD -> VOUT_FB_MCU, R8 1.5k. */
#define DEFINES_SENSE_LVMS_OUT_GAIN DEFINES_SENSE_24V_RANGE_GAIN    /*!< LVMS_OUT_SENSED -> LVMS_OUT_SENSE_MCU, R5 1.5k. */
#define DEFINES_SENSE_V_CHRG_GAIN DEFINES_SENSE_24V_RANGE_GAIN      /*!< V_CHRG_SENSED -> V_CHRG_SENSE_MCU, R10 1.5k. */
#define DEFINES_SENSE_MCU_5V_GAIN DEFINES_SENSE_5V_RANGE_GAIN       /*!< +5V -> 5V_SENSE_MCU, R6 18k. */

/*! \} */

/*!
 * \defgroup        sense_current Current sense transfer functions.
 *
 * \details         The board carries a single current sensor, U4 on the
 *                  "Current sensor" sheet: an Allegro ACS724LLCTR-05AB in the
 *                  charger path (CHRG -> IP+, IP- -> F3 4 A -> CHRG_BAT), supplied
 *                  from the +5 V rail and driving the net I_CHRG. That net then
 *                  goes through the same 10 k / 18 k attenuator as every other
 *                  5 V-range line before reaching I_CHRG_SENSE_MCU (PB11).
 *
 *                  The -05AB is the bidirectional +/-5 A variant: it outputs
 *                  Vcc/2 at zero current and moves 400 mV per ampere. Both the
 *                  quiescent point and the sensitivity are ratiometric to Vcc, and
 *                  the board happens to measure that very rail on MCU_5V_SENSE, so
 *                  the conversion uses the measured supply rather than a nominal
 *                  5 V:
 *
 *                      V_sensed = V_pin / DIVIDER_GAIN
 *                      V_zero   = Vcc * ZERO_RATIO
 *                      S        = SENSITIVITY_V_A * (Vcc / SUPPLY_NOMINAL_V)
 *                      I        = (V_sensed - V_zero) / S
 *
 *                  Sanity check at Vcc = 5 V: +5 A -> 4.5 V sensed -> 2.89 V at the
 *                  pin, -5 A -> 0.5 V -> 0.32 V at the pin, 0 A -> 2.5 V -> 1.61 V.
 *                  The whole range fits under the 3V3 BAT54S clamp.
 *
 * \{
 */

#define DEFINES_SENSE_I_CHRG_DIVIDER_GAIN DEFINES_SENSE_5V_RANGE_GAIN /*!< I_CHRG -> I_CHRG_SENSE_MCU, R11 18k. */
#define DEFINES_SENSE_I_CHRG_SUPPLY_NOMINAL_V (5.F)                   /*!< Vcc the ACS724 sensitivity is specified at, in V. */
#define DEFINES_SENSE_I_CHRG_SENSITIVITY_V_A (0.400F)                 /*!< ACS724-05AB sensitivity at nominal Vcc, in V/A. */
#define DEFINES_SENSE_I_CHRG_ZERO_RATIO (0.5F)                        /*!< Quiescent output as a fraction of Vcc. */
#define DEFINES_SENSE_I_CHRG_RANGE_A (5.F)                            /*!< Nominal full scale of the sensor, +/- this value in A. */

/*!
 * \brief           Attenuation of the I_OUT_SENSED line, R4 18k.
 *
 * \warning         The divider is populated and reaches I_OUT_SENSE_MCU (PB0), but
 *                  nothing on the schematic drives I_OUT_SENSED: the only current
 *                  sensor on the board is the charger one above, and the output
 *                  path ("Board output" sheet) is just F14 20 A into the relay K1
 *                  with no shunt or Hall sensor. Until a sensor exists there is no
 *                  output current to compute, so only the raw node voltage is
 *                  exposed, through adc_get_i_out_sense_voltage().
 */
#define DEFINES_SENSE_I_OUT_DIVIDER_GAIN DEFINES_SENSE_5V_RANGE_GAIN

/*! \} */

/*! \} */
#define DEFINES_LTC_COUNT (1U) /*!< The number of LTCs */

#endif /*! DEFINES_H */
