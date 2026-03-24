/*!
 * \file            config.h
 * \date            2026-03-24
 * \authors         Mirko Lana [mirko.lana@eagletrt.it]
 *
 * \brief           Project configuration.
 */

#ifndef CONFIG_H
#define CONFIG_H

/*!
 * \defgroup        modules Enable or disable the internal modules of the project.
 * @{
 */

#define CONFIG_TEMPERATURE_MODULE_ENABLE

/*!
 * @}
 */

/*!
 * \defgroup        constants Constants used throughout the project.
 * @{
 */

#define CONFIG_CELLBOARD_COUNT (6U) /*!< The number of cellboards */

/*!
 * @}
 */

#endif /*! CONFIG_H */
