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

#define DEFINES_NTC_COUNT (16U)         /*!< The number of NTCs */
#define DEFINES_NTC_VDD (3.3F)          /*!< Supply voltage for the NTC circuit in V. */
#define DEFINES_NTC_R0 (10000.F)        /*!< Nominal NTC resistance at 25 °C in Ohm. */
#define DEFINES_NTC_BETA (3950.F)       /*!< NTC Beta parameter in K. */
#define DEFINES_NTC_T0_KELVIN (298.15F) /*!< Reference temperature in K. */

/*!
 * \}
 */
#define DEFINES_LTC_COUNT (1U) /*!< The number of LTCs */

#endif /*! DEFINES_H */
