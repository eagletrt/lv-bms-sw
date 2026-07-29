/*!
 * \file            types.h
 * \date            2026-04-28
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Custom type definitions used inside the project.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/*!
 * \defgroup        types Type definitions for various units of measurements.
 * \{
 */

typedef float ampere;      /*!< Current value in A */
typedef float watt;        /*!< Power value in W */
typedef float kilowatt;    /*!< Power value in kW */
typedef uint16_t raw_volt; /*!< 10 bit resolutioin raw voltage value */
typedef float volt;        /*!< Voltage value in V */
typedef float celsius;     /*!< Temperature value in °C */

/*! \} */

#endif /*! TYPES_H */
