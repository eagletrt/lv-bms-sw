/*!
 * \file bootloader.h
 * \author Matteo Grande [mattwexx2929@gmail.com]
 * \date 2026-10-13
 * \brief Type definitions for the bootloader module of the cooling board
 */

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "watchdogs.h"
#include "can-primary.h"

#define BOOTLOADER_CAN_FLASH_ID_MIN CAN_PRIMARY_MESSAGE_FRAME_ID_ECUFLASHINGTX
#define BOOTLOADER_CAN_FLASH_ID_MAX CAN_PRIMARY_MESSAGE_FRAME_ID_STEERINGBOARDFLASHINGRX

#define BOOTLOADER_WATCHDOG_TIMEOUT 500U

/*!
 * \brief Reboot callback
 */
typedef void (*reboot_callback)(void);

/*!
 * \brief Return codes for the bootloader module functions
 */
enum BootloaderReturnCode {
    BOOTLOADER_RC_OK = 0,       /*!< The function executed succesfully */
    BOOTLOADER_RC_NULL_POINTER, /*!< An argument of pointer type wa null or a null pointer */
    BOOTLOADER_RC_ERROR         /*!< The function encountered an error */
};

/*!
 * \brief The handler containing the handlers for timebase and the state of the
 * module
 */
struct BootloaderHandler {
    struct WatchdogHandler *watchdog_pool; /*!< Watchdogs pool */
    struct Watchdog watchdog;              /*!< Watchdog used by the module */
    reboot_callback reboot;                /*!< MCU reboot callback */
    bool flashing;                         /*!< Wheather the MCU is flashing or not */
    bool timedout;                         /*!< Wheather the watchdog has timedout or not */
};

#endif // BOOTLOADER_H
