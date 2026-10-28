/*!
 * \file bootloader-api.h
 * \author Matteo Grande [mattwexx2929@gmail.com]
 * \date 2026-10-13
 * \brief Implementation of the APIs of the bootloader module
 */

#include "bootloader-api.h"

#include "watchdogs-api.h"
#include "eagletrt.h"

/*!
 * \brief Internal module handler
 * \details Hidden from external linkage to enforce API-only access
 */
EAGLETRT_STATIC struct BootloaderHandler bootloader_handler;

/*!
 * \brief module's watchdog timeout callback
 */
void prv_bootloader_api_flash_timeout(void) {
    bootloader_handler.timedout = true;
}

enum BootloaderReturnCode bootloader_api_init(struct WatchdogHandler *watchdog_pool,
                                              reboot_callback reboot) {
    memset(&bootloader_handler, 0, sizeof(bootloader_handler));

    if (watchdog_pool == nullptr || reboot == nullptr) {
        return BOOTLOADER_RC_NULL_POINTER;
    }
    bootloader_handler.watchdog_pool = watchdog_pool;
    bootloader_handler.reboot = reboot;

    watchdogs_api_init_watchdog(&bootloader_handler.watchdog, BOOTLOADER_WATCHDOG_TIMEOUT, prv_bootloader_api_flash_timeout);

    bootloader_handler.flashing = false;
    bootloader_handler.timedout = false;

    return BOOTLOADER_RC_OK;
}

enum BootloaderReturnCode bootloader_api_reboot(void) {
    if (bootloader_handler.reboot == nullptr) {
        return BOOTLOADER_RC_NULL_POINTER;
    }

    bootloader_handler.reboot();
    return BOOTLOADER_RC_OK;
}

enum BootloaderReturnCode bootloader_api_watchdog_restart(void) {
    bootloader_handler.timedout = false;
    watchdogs_api_watchdog_restart(
        bootloader_handler.watchdog_pool,
        &bootloader_handler.watchdog);

    return BOOTLOADER_RC_OK;
}

enum BootloaderReturnCode bootloader_api_watchdog_pet(void) {
    watchdogs_api_watchdog_pet(
        bootloader_handler.watchdog_pool,
        &bootloader_handler.watchdog);

    return BOOTLOADER_RC_OK;
}

bool bootloader_api_is_timedout(void) {
    return bootloader_handler.timedout;
}

bool bootloader_api_is_flashing(void) {
    return bootloader_handler.flashing;
}

enum BootloaderReturnCode bootloader_api_set_flashing(bool flashing) {
    bootloader_handler.flashing = flashing;

    return BOOTLOADER_RC_OK;
}
