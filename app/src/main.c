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

#include <zephyr/device.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zsw_clock.h>
#include <lvgl.h>
#include <sys/time.h>
#include <zephyr/zbus/zbus.h>
#include <zsw_cpu_freq.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/fatal.h>
#include <zephyr/input/input.h>
#include <zephyr/retention/bootmode.h>
#include <zephyr/sys/reboot.h>
#include "dfu.h"
#include "ui/zsw_ui.h"
#include "ble/ble_comm.h"
#include "ble/ble_aoa.h"
#include "battery/battery.h"
#include "events/accel_event.h"
#include "events/ble_event.h"
#include "sensors/zsw_imu.h"
#include "sensors/zsw_magnetometer.h"
#include "sensors/zsw_pressure_sensor.h"
#include "sensors/zsw_light_sensor.h"
#include "sensors/zsw_environment_sensor.h"
#include <zsw_retained_ram_storage.h>
#include "drivers/zsw_vibration_motor.h"
#include "drivers/zsw_display_control.h"
#include "managers/zsw_power_manager.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_notification_manager.h"
#include "applications/watchface/watchface_app.h"
#include <filesystem/zsw_rtt_flash_loader.h>
#include <filesystem/zsw_filesystem.h>
#include "ui/popup/zsw_popup_window.h"
#include "ble/ble_ams.h"
#include "ble/ble_ancs.h"
#include "ble/ble_cts.h"

LOG_MODULE_REGISTER(main, CONFIG_ZSW_APP_LOG_LEVEL);

#define TASK_WDT_FEED_INTERVAL_MS  3000

typedef enum ui_state {
    INIT_STATE,
    WATCHFACE_STATE,
    APPLICATION_MANAGER_STATE,
} ui_state_t;

static struct input_worker_item_t {
    struct k_work work;
    struct input_event event;
} input_worker_item;

struct input_event last_input_event;

static void run_input_work(struct k_work *item);
static void run_init_work(struct k_work *item);

static void run_wdt_work(struct k_work *item);
static void enable_bluetoth(void);
static void print_retention_ram(void);
static void enocoder_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static void click_feedback(struct _lv_indev_drv_t *drv, uint8_t e);
static void open_notification_popup(void *data);
static void async_turn_off_buttons_allocation(void *unused);
static void open_application_manager_page(void *app_name);
static void handle_screen_gesture(lv_dir_t event_code);

static void on_application_manager_close(void);
static void on_popup_notifcation_closed(uint32_t id);
static void on_zbus_notification_callback(const struct zbus_channel *chan);
static void on_zbus_ble_data_callback(const struct zbus_channel *chan);
static void on_input_subsys_callback(struct input_event *evt);
static void on_watchface_app_event_callback(watchface_app_evt_t evt);
static void on_lvgl_screen_gesture_event_callback(lv_event_t *e);

static int kernal_wdt_id;

static lv_group_t *input_group;
static lv_group_t *temp_group;

static lv_indev_drv_t enc_drv;
static lv_indev_t *enc_indev;
static uint8_t last_pressed;

static bool pending_not_open;
static bool is_buttons_for_lvgl = false;

static ui_state_t watch_state = INIT_STATE;

K_WORK_DELAYABLE_DEFINE(wdt_work, run_wdt_work);
K_WORK_DEFINE(init_work, run_init_work);
K_WORK_DEFINE(input_work, run_input_work);

ZBUS_CHAN_DECLARE(ble_comm_data_chan);
ZBUS_LISTENER_DEFINE(main_ble_comm_lis, on_zbus_ble_data_callback);
ZBUS_LISTENER_DEFINE(main_notification_lis, on_zbus_notification_callback);

static void run_input_work(struct k_work *item)
{
    struct input_worker_item_t *container = CONTAINER_OF(item, struct input_worker_item_t, work);

    LOG_DBG("Input worker code: %u", container->event.code);

    // Don't process the press if it caused wakeup.
    if (zsw_power_manager_reset_idle_timout()) {
        return;
    }

    switch (container->event.code) {
        // Button event
        // Always allow force restart.
        case (INPUT_KEY_Y): {
            LOG_INF("Force restart");

            retained.off_count += 1;
            zsw_retained_ram_update();
            sys_reboot(SYS_REBOOT_COLD);

            break;
        }
        // Button event
        case INPUT_KEY_3: {
            if (zsw_notification_popup_is_shown()) {
                zsw_notification_popup_remove();
            } else if (watch_state == APPLICATION_MANAGER_STATE) {
                zsw_app_manager_exit_app();

                return;
            }

            break;
        }
        case INPUT_KEY_1: {
            if ((watch_state == WATCHFACE_STATE) && !zsw_notification_popup_is_shown()) {
                zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_CLICK);
                lv_async_call(open_application_manager_page, NULL);
            }

            break;
        }
    }

    // Handle the input events. We have to take care about the screen orientation for the touch events.
    lv_dir_t gesture_code = LV_DIR_NONE;
    switch (container->event.code) {
        // Watch: Slide from right to left.
        case INPUT_BTN_NORTH: {
            gesture_code = LV_DIR_LEFT;
            break;
        }
        // Watch: Slide from left to right.
        case INPUT_BTN_SOUTH: {
            gesture_code = LV_DIR_RIGHT;
            break;
        }
        // Watch: Slide from bottom to top.
        case INPUT_BTN_WEST: {
            gesture_code = LV_DIR_TOP;
            break;
        }
        // Watch: Slide from top to bottom.
        case INPUT_BTN_EAST: {
            gesture_code = LV_DIR_BOTTOM;
            break;
        }
    }

    if (is_buttons_for_lvgl && (gesture_code == LV_DIR_NONE)) {
        // Handled by LVGL
        last_input_event.code = container->event.code;
        return;
    }
}

static void run_init_work(struct k_work *item)
{
    lv_indev_t *touch_indev;

    // Not to self, PWM consumes like 250uA...
    // Need to disable also when screen is off.
    lv_obj_set_style_bg_color(lv_scr_act(), zsw_color_dark_gray(), LV_PART_MAIN | LV_STATE_DEFAULT);
    zsw_display_control_init();
    zsw_display_control_sleep_ctrl(true);
    print_retention_ram();
    zsw_notification_manager_init();
    enable_bluetoth();
    zsw_imu_init();
    zsw_magnetometer_init();
    zsw_pressure_sensor_init();
    zsw_light_sensor_init();
    zsw_environment_sensor_init();
    zsw_clock_init(retained.current_time_seconds, retained.timezone);

    INPUT_CALLBACK_DEFINE(NULL, on_input_subsys_callback);

    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = enocoder_read;
    enc_drv.feedback_cb = click_feedback;
    enc_indev = lv_indev_drv_register(&enc_drv);

    input_group = lv_group_create();
    temp_group = lv_group_create();
    lv_group_set_default(input_group);
    lv_indev_set_group(enc_indev, input_group);

    touch_indev = lv_indev_get_next(NULL);
    while (touch_indev) {
        if (lv_indev_get_type(touch_indev) == LV_INDEV_TYPE_POINTER) {
            // TODO First fix so not all presses everywhere are registered as clicks and cause vibration
            // Clicking anywehere with this below added right now will cause a vibration, which
            // is not what we want
            // touch_indev->driver->feedback_cb = click_feedback;
            break;
        }
        touch_indev = lv_indev_get_next(touch_indev);
    }

    watch_state = WATCHFACE_STATE;

    lv_obj_add_event_cb(lv_scr_act(), on_lvgl_screen_gesture_event_callback, LV_EVENT_GESTURE, NULL);

    watchface_app_start(input_group, on_watchface_app_event_callback);

#ifdef CONFIG_SPI_FLASH_LOADER
    if (NUM_RAW_FS_FILES != zsw_filesytem_get_num_rawfs_files()) {
        LOG_ERR("Number of rawfs files does not match the number of files in the file table: %d / %d",
                zsw_filesytem_get_num_rawfs_files(), NUM_RAW_FS_FILES);
        zsw_popup_show("Warning", "Missing files in external flash\nPlease run:\nwest upload_fs", NULL, 5, false);
    }
#endif

#if defined(CONFIG_TASK_WDT) && !defined(CONFIG_BOARD_NATIVE_POSIX)
    const struct device *hw_wdt_dev = DEVICE_DT_GET(DT_ALIAS(watchdog0));
    if (!device_is_ready(hw_wdt_dev)) {
        LOG_DBG("Hardware watchdog %s is not ready; ignoring it.",
                hw_wdt_dev->name);
        hw_wdt_dev = NULL;
    }

    task_wdt_init(hw_wdt_dev);
    kernal_wdt_id = task_wdt_add(TASK_WDT_FEED_INTERVAL_MS * 5, NULL, NULL);

    k_work_schedule(&wdt_work, K_NO_WAIT);
#endif
}

static void run_wdt_work(struct k_work *item)
{
    task_wdt_feed(kernal_wdt_id);
    k_work_schedule(&wdt_work, K_MSEC(TASK_WDT_FEED_INTERVAL_MS));
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

    return 0;
}

static void enable_bluetoth(void)
{
    int err;

#ifdef CONFIG_BOARD_NATIVE_POSIX
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

    ble_ams_init();
    ble_cts_init();
    ble_ancs_init();
}

static void print_retention_ram(void)
{
    LOG_DBG("Boot count: %u\n", retained.boots);
    LOG_DBG("uptime_latest: %" PRIu64 "\n", retained.uptime_latest);
    LOG_DBG("Active Ticks: %" PRIu64 "\n", retained.uptime_sum);
}

static void open_notification_popup(void *data)
{
    zsw_not_mngr_notification_t *not = zsw_notification_manager_get_newest();
    if (not != NULL) {
        lv_group_set_default(temp_group);
        lv_indev_set_group(enc_indev, temp_group);
        zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_NOTIFICATION);
        zsw_notification_popup_show(not->sender, not->body, not->src, not->id, on_popup_notifcation_closed, 10);
        is_buttons_for_lvgl = true;
    }
    pending_not_open = false;
}

static void open_application_manager_page(void *app_name)
{
    watchface_app_stop();
    is_buttons_for_lvgl = true;
    watch_state = APPLICATION_MANAGER_STATE;
    zsw_app_manager_show(on_application_manager_close, lv_scr_act(), input_group, (char *)app_name);
}

static void close_popup_notification(lv_timer_t *timer)
{
    uint32_t id;

    id = (uint32_t)timer->user_data;
    // Notification was dismissed, hence consider it read.
    zsw_notification_manager_remove(id);

    lv_group_set_default(input_group);
    lv_indev_set_group(enc_indev, input_group);
    if (watch_state == WATCHFACE_STATE) {
        is_buttons_for_lvgl = 0;
    } else {
        is_buttons_for_lvgl = 1;
    }
}

static void on_popup_notifcation_closed(uint32_t id)
{
    lv_timer_t *timer;
    // Changing back input group directly here causes LVGL
    // to not remove the notifcation for some reason.
    // Maybe a bug, or something done wrong.
    // Anyway doing it after a while instead seems to fix
    // the problem.
    timer = lv_timer_create(close_popup_notification, 500,  (void *)id);
    lv_timer_set_repeat_count(timer, 1);
}

static void on_application_manager_close(void)
{
    zsw_app_manager_delete();
    watch_state = WATCHFACE_STATE;
    watchface_app_start(input_group, on_watchface_app_event_callback);
    lv_async_call(async_turn_off_buttons_allocation, NULL);
}

static void async_turn_off_buttons_allocation(void *unused)
{
    is_buttons_for_lvgl = false;
}

static void on_input_subsys_callback(struct input_event *evt)
{
    // Currently you have to define a keycode as binding between buttons and longpress. We skip this binding codes for now.
    // Also touch events will be skipped, because they are handled by LVGL.
    // TODO: Charger is also ignored for now.
    // TODO: Replace this filtering with a propper device filtering setup for the input handler
    if ((evt->code == INPUT_ABS_X) || (evt->code == INPUT_ABS_Y) || (evt->code == INPUT_BTN_TOUCH) ||
        (evt->code == INPUT_KEY_KP0) || (evt->code == INPUT_KEY_POWER) || (evt->value == 1)) {
        return;
    }

    input_worker_item.event = *evt;
    input_worker_item.work = input_work;
    k_work_submit(&input_worker_item.work);
}

static void handle_screen_gesture(lv_dir_t event_code)
{
    if (watch_state == WATCHFACE_STATE && !zsw_notification_popup_is_shown()) {
        switch (event_code) {
            case LV_DIR_LEFT: {
                open_application_manager_page("Notification");
                break;
            }
            case LV_DIR_RIGHT: {
                watchface_change();
                break;
            }
            case LV_DIR_TOP: {
                open_application_manager_page("Settings");
                break;
            }
            case LV_DIR_BOTTOM: {
                open_application_manager_page(NULL);
                break;
            }
            default:
                __ASSERT(false, "Not a valid gesture code: %d", event_code);
        }
        lv_indev_wait_release(lv_indev_get_act());
    } else if (zsw_notification_popup_is_shown()) {
        zsw_notification_popup_remove();
    } else if (watch_state == APPLICATION_MANAGER_STATE && event_code == LV_DIR_RIGHT) {
#ifdef CONFIG_BOARD_NATIVE_POSIX
        // Until there is a better way to go back without access to buttons.
        zsw_app_manager_exit_app();
#endif
    }
}

static void on_lvgl_screen_gesture_event_callback(lv_event_t *e)
{
    lv_dir_t  dir;
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_GESTURE) {
        dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        handle_screen_gesture(dir);
    }
}

static void enocoder_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    if (!is_buttons_for_lvgl) {
        return;
    }

    if (last_input_event.code == INPUT_KEY_4) {
        data->key = LV_KEY_LEFT;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = 2;
    } else if (last_input_event.code == INPUT_KEY_1) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = 1;
    } else if (last_input_event.code == INPUT_KEY_2) {
        data->key = LV_KEY_RIGHT;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = 3;
    } else if (last_input_event.code == INPUT_KEY_3) {
        // Not used for now. TODO exit/back button.
    } else {
        if (last_pressed == 0xFF) {
            return;
        }
        data->state = LV_INDEV_STATE_REL;
        switch (last_pressed) {
            case 2:
                data->key = LV_KEY_RIGHT;
                break;
            case 1:
                data->key = LV_KEY_ENTER;
                break;
            case 3:
                data->key = LV_KEY_LEFT;
                break;
            default:
                break;
        }
        last_pressed = 0xFF;
    }

    last_input_event.code = 0xFF;
}

static void on_watchface_app_event_callback(watchface_app_evt_t evt)
{
    if (watch_state == WATCHFACE_STATE && !zsw_notification_popup_is_shown()) {
        switch (evt) {
            case WATCHFACE_APP_EVT_CLICK_BATT:
                open_application_manager_page("Battery");
                break;
            case WATCHFACE_APP_EVT_CLICK_STEP:
                break;
            case WATCHFACE_APP_EVT_CLICK_WEATHER:
                break;
            default:
                break;
        }
    }
}

static void click_feedback(struct _lv_indev_drv_t *drv, uint8_t e)
{
    if (e == LV_EVENT_CLICKED) {
        zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_CLICK);
    }
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
                struct timespec tspec;
                tspec.tv_sec = event->data.data.time.seconds;
                tspec.tv_nsec = 0;

                clock_settime(CLOCK_REALTIME, &tspec);
            }

            if (event->data.data.time.tz_offset != 0) {
                char tz[sizeof("UTC+01")] = { '\0' };
                char sign = (event->data.data.time.tz_offset < 0) ? '+' : '-';
                snprintf(tz, sizeof(tz), "UTC%c%d", sign, MIN(abs(event->data.data.time.tz_offset), 99));
                setenv("TZ", tz, 1);
                tzset();
            }
            break;
        }
        case BLE_COMM_DATA_TYPE_WEATHER:
            break;
        case BLE_COMM_DATA_TYPE_REMOTE_CONTROL:
            // TODO: Add correct enum
            if (event->data.data.remote_control.button == 4) {
                zsw_power_manager_reset_idle_timout();
                if (watch_state == APPLICATION_MANAGER_STATE) {
                    zsw_app_manager_delete();
                    zsw_app_manager_set_index(0);
                    is_buttons_for_lvgl = false;
                    watch_state = WATCHFACE_STATE;
                    watchface_app_start(input_group, on_watchface_app_event_callback);
                }
            } else {
                // TODO: Can we rework it with triggering input events?
                //button_set_fake_press((buttonId_t)event->data.data.remote_control.button, true);
            }

            break;
        default:
            break;
    }
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
