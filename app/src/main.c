/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

#include <zephyr/kernel.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>
#include <zephyr/logging/log.h>
#include <zephyr/input/input.h>
#include <zephyr/fatal.h>
#include <zephyr/sys/reboot.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/settings/settings.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/retention/bootmode.h>

#include <lvgl.h>

#include <zsw_clock.h>
#include <zsw_cpu_freq.h>
#include <zsw_retained_ram_storage.h>
#include <zsw_coredump.h>

#include "ui/zsw_ui.h"
#include "ui/popup/zsw_popup_window.h"
#include "zsw_ui_controller.h"

#include "ble/ble_comm.h"
#include "ble/ble_aoa.h"
#include "ble/ble_ams.h"
#include "ble/ble_ancs.h"
#include "ble/ble_cts.h"

#include "events/ble_event.h"

#include "sensors/zsw_imu.h"
#include "sensors/zsw_magnetometer.h"
#include "sensors/zsw_pressure_sensor.h"
#include "sensors/zsw_light_sensor.h"
#include "sensors/zsw_environment_sensor.h"

#include "drivers/zsw_vibration_motor.h"
#include "drivers/zsw_display_control.h"

#include "managers/zsw_power_manager.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_notification_manager.h"

#include "applications/watchface/watchface_app.h"

#include <filesystem/zsw_rtt_flash_loader.h>
#include <filesystem/zsw_filesystem.h>

#include "fuel_gauge/zsw_pmic.h"

LOG_MODULE_REGISTER(main, CONFIG_ZSW_APP_LOG_LEVEL);

static void run_init_work(struct k_work *item);
static void enable_bluetooth(void);
static void print_retention_ram(void);
static void open_notification_popup(void *data);
static void on_zbus_notification_callback(const struct zbus_channel *chan);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);

K_WORK_DEFINE(init_work, run_init_work);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(main_ble_comm_lis, on_zbus_ble_data_callback);
ZBUS_LISTENER_DEFINE(main_notification_lis, on_zbus_notification_callback);

static bool pending_not_open = false;

static void run_init_work(struct k_work *item)
{
#ifdef CONFIG_DEBUG_COREDUMP
    zsw_coredump_init();
#endif

    zsw_display_control_init();
    zsw_display_control_sleep_ctrl(true);

    zsw_notification_manager_init();

    enable_bluetooth();

    zsw_imu_init();
    zsw_magnetometer_init();
    zsw_pressure_sensor_init();
    zsw_light_sensor_init();
    zsw_environment_sensor_init();

    zsw_ui_controller_init();

    print_retention_ram();
#ifdef CONFIG_SPI_FLASH_LOADER
    if (NUM_RAW_FS_FILES != zsw_filesytem_get_num_rawfs_files()) {
        LOG_ERR("Number of rawfs files does not match the number of files in the file table: %d / %d",
                zsw_filesytem_get_num_rawfs_files(), NUM_RAW_FS_FILES);
        zsw_popup_show("Warning", "Missing files in external flash\nPlease run:\nwest upload_fs", NULL, 5, false);
    }
#endif

    LOG_INF("ZSWatch application started");
}

int main(void)
{
#ifdef CONFIG_SPI_FLASH_LOADER
    if (bootmode_check(ZSW_BOOT_MODE_RTT_FLASH_LOADER)) {
        LOG_WRN("SPI Flash Loader Boot Mode");
        zsw_rtt_flash_loader_start();
        return 0;
    } else if (bootmode_check(ZSW_BOOT_MODE_FLASH_ERASE)) {
        zsw_rtt_flash_loader_erase_external();
    }
#endif
    // The init code requires a bit of stack.
    // So in order to not increase CONFIG_MAIN_STACK_SIZE and loose
    // this RAM forever, instead re-use the system workqueue for init
    // it has the required amount of stack.
    k_work_submit(&init_work);

    // Workaround due to https://github.com/zephyrproject-rtos/zephyr/issues/71410
    // we need to run lv_task_handler from main thread and disable CONFIG_LV_Z_FLUSH_THREAD
#ifdef CONFIG_ARCH_POSIX
    int64_t next_update_in_ms;
    while (true) {
        next_update_in_ms = lv_task_handler();
        k_msleep(next_update_in_ms);
    }
#endif

    return 0;
}

static void enable_bluetooth(void)
{
    int err;

#ifdef CONFIG_ARCH_POSIX
    bt_addr_le_t addr;

    err = bt_addr_le_from_str("DE:AD:BE:EF:BA:11", "random", &addr);
    if (err) {
        LOG_ERR("Invalid BT address (err %d)", err);
    }

    err = bt_id_create(&addr, NULL);
    if (err < 0) {
        LOG_ERR("Creating new ID failed (err %d)", err);
    }
#endif

    err = bt_enable(NULL);

#ifdef CONFIG_SETTINGS
    settings_load();
#endif
    if (err != 0) {
        LOG_ERR("Failed to enable Bluetooth, err: %d", err);
        return;
    }

    __ASSERT_NO_MSG(ble_comm_init() == 0);
    bleAoaInit();
#ifdef CONFIG_BT_AMS_CLIENT
    ble_ams_init();
    ble_cts_init();
#endif
#ifdef CONFIG_BT_ANCS_CLIENT
    ble_ancs_init();
#endif
}

static void on_close_popup_notification(uint32_t id)
{
    zsw_notification_manager_remove(id);

    zsw_ui_controller_clear_notification_mode();
}

static void open_notification_popup(void *data)
{
    zsw_not_mngr_notification_t *not = zsw_notification_manager_get_newest();
    if (not != NULL) {
        zsw_ui_controller_set_notification_mode();
        zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_NOTIFICATION);
        zsw_notification_popup_show(not->sender, not->body, not->src, not->id, on_close_popup_notification, 10);
    }
    pending_not_open = false;
}

static void on_zbus_notification_callback(const struct zbus_channel *chan)
{
    if (zsw_notification_popup_is_shown() || pending_not_open) {
        return;
    }

    if (zsw_power_manager_get_state() != ZSW_ACTIVITY_STATE_NOT_WORN_STATIONARY) {
        pending_not_open = true;
        zsw_power_manager_reset_idle_timout();
        lv_async_call(open_notification_popup, NULL);
    }
}

static void on_zbus_ble_data_callback(const struct zbus_channel *chan)
{
    const struct ble_data_event *event = zbus_chan_const_msg(chan);

    switch (event->data.type) {
        case BLE_COMM_DATA_TYPE_SET_TIME: {
            if (event->data.data.time.seconds > 0) {
                zsw_timeval_t ztm;
                memcpy(&ztm.tm, localtime((const time_t *)&event->data.data.time.seconds), sizeof(ztm.tm));
                zsw_clock_set_time(&ztm);
            }

            if (event->data.data.time.tz_offset != 0) {
                char tz[sizeof("UTC+01")] = { '\0' };
                char sign = (event->data.data.time.tz_offset < 0) ? '+' : '-';
                snprintf(tz, sizeof(tz), "UTC%c%d", sign, MIN(abs(event->data.data.time.tz_offset), 99));

#ifdef CONFIG_RTC
                // When using RTC, we need to adjust the current rtc_time according to the timezone.
                zsw_timeval_t ztm;
                zsw_clock_get_time(&ztm);
                ztm.tm.tm_year -= 1900;
                time_t current_rtc_time = mktime(rtc_time_to_tm(&ztm.tm));
                zsw_clock_set_timezone(tz);
                memcpy(&ztm.tm, localtime(&current_rtc_time), sizeof(ztm.tm)); // Adjust the time according to the new timezone
                zsw_clock_set_time(&ztm);
#else
                zsw_clock_set_timezone(tz);
#endif
            }
            break;
        }
        case BLE_COMM_DATA_TYPE_WEATHER:
            break;
        default:
            break;
    }
}

static void print_retention_ram(void)
{
    LOG_DBG("Boot count: %u\n", retained.boots);
    LOG_DBG("uptime_latest: %" PRIu64 "\n", retained.uptime_latest);
    LOG_DBG("Active Ticks: %" PRIu64 "\n", retained.uptime_sum);
}

#ifndef CONFIG_RESET_ON_FATAL_ERROR
void k_sys_fatal_error_handler(unsigned int reason, const z_arch_esf_t *esf)
{
    ARG_UNUSED(esf);
    ARG_UNUSED(reason);

    LOG_PANIC();

    LOG_ERR("Resetting system");
    sys_reboot(SYS_REBOOT_COLD);

    CODE_UNREACHABLE;
}
#endif
