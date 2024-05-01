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

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/pm/device.h>
#include <zephyr/zbus/zbus.h>
#include <lvgl.h>
#include <zephyr/logging/log.h>
#include <events/activity_event.h>
#include <zsw_retained_ram_storage.h>
#include <zephyr/settings/settings.h>

#include "zsw_settings.h"
#include "zsw_cpu_freq.h"
#include "events/accel_event.h"
#include "events/battery_event.h"
#include "managers/zsw_power_manager.h"
#include "drivers/zsw_display_control.h"
#include "drivers/zsw_vibration_motor.h"

LOG_MODULE_REGISTER(zsw_power_manager, LOG_LEVEL_INF);

#ifdef CONFIG_BOARD_NATIVE_POSIX
#define IDLE_TIMEOUT_SECONDS    UINT32_MAX
#else
#define IDLE_TIMEOUT_SECONDS    CONFIG_POWER_MANAGEMENT_IDLE_TIMEOUT_SECONDS
#endif

#define POWER_MANAGEMENT_MIN_ACTIVE_PERIOD_SECONDS                  1
#define LOW_BATTERY_VOLTAGE_MV                                      3750

static void update_and_publish_state(zsw_power_manager_state_t new_state);
static void handle_idle_timeout(struct k_work *item);
static void zbus_accel_data_callback(const struct zbus_channel *chan);
static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);

K_WORK_DELAYABLE_DEFINE(idle_work, handle_idle_timeout);

ZBUS_CHAN_DECLARE(activity_state_data_chan);

ZBUS_LISTENER_DEFINE(power_manager_accel_lis, zbus_accel_data_callback);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_OBS_DECLARE(zsw_power_manager_bat_listener);
ZBUS_CHAN_ADD_OBS(battery_sample_data_chan, zsw_power_manager_bat_listener, 1);
ZBUS_LISTENER_DEFINE(zsw_power_manager_bat_listener, zbus_battery_sample_data_callback);

static uint32_t idle_timeout_seconds = IDLE_TIMEOUT_SECONDS;
static bool is_active = true;
static bool is_stationary;
static uint32_t last_wakeup_time;
static uint32_t last_pwr_off_time;
static zsw_power_manager_state_t state;

static void enter_inactive(void)
{
    // Minimum time the device should stay active before switching back to inactive.
    if ((k_uptime_get_32() - last_wakeup_time) < (POWER_MANAGEMENT_MIN_ACTIVE_PERIOD_SECONDS * 1000UL)) {
        return;
    }

    LOG_INF("Enter inactive");
    is_active = false;
    retained.wakeup_time += k_uptime_get_32() - last_wakeup_time;
    zsw_retained_ram_update();
    zsw_display_control_sleep_ctrl(false);

    zsw_cpu_set_freq(ZSW_CPU_FREQ_DEFAULT, true);

    // Screen inactive -> wait for NO_MOTION interrupt in order to power off display regulator.
    zsw_imu_feature_enable(ZSW_IMU_FEATURE_NO_MOTION, true);
    zsw_imu_feature_disable(ZSW_IMU_FEATURE_ANY_MOTION);

    update_and_publish_state(ZSW_ACTIVITY_STATE_INACTIVE);
}

static void enter_active(void)
{
    LOG_INF("Enter active");
    int ret;

    is_active = true;
    is_stationary = false;
    last_wakeup_time = k_uptime_get_32();

    // Running at max CPU freq consumes more power, but rendering we
    // want to do as fast as possible. Also to use 32MHz SPI, CPU has
    // to be running at 128MHz. Meaning this improves both rendering times
    // and the SPI transmit time.
    zsw_cpu_set_freq(ZSW_CPU_FREQ_FAST, true);

    ret = zsw_display_control_pwr_ctrl(true);
    zsw_display_control_sleep_ctrl(true);

    if (ret == 0) {
        retained.display_off_time += k_uptime_get_32() - last_pwr_off_time;
        zsw_retained_ram_update();
    }

    // Only used when display is not active.
    zsw_imu_feature_disable(ZSW_IMU_FEATURE_NO_MOTION);
    zsw_imu_feature_disable(ZSW_IMU_FEATURE_ANY_MOTION);

    update_and_publish_state(ZSW_ACTIVITY_STATE_ACTIVE);

    k_work_schedule(&idle_work, K_SECONDS(idle_timeout_seconds));
}

bool zsw_power_manager_reset_idle_timout(void)
{
    if (!is_active) {
        // If we are inactive, then this means we we should enter active.
        enter_active();
        return true;
    } else {
        // We are active, then just reschdule the inactivity timeout.
        k_work_reschedule(&idle_work, K_SECONDS(idle_timeout_seconds));
        return false;
    }
}

uint32_t zsw_power_manager_get_ms_to_inactive(void)
{
    if (!is_active) {
        return 0;
    }
    uint32_t time_since_lvgl_activity = lv_disp_get_inactive_time(NULL);
    uint32_t time_to_timeout = k_ticks_to_ms_ceil32(k_work_delayable_remaining_get(&idle_work));

    if (time_since_lvgl_activity >= idle_timeout_seconds * 1000) {
        return time_to_timeout;
    } else {
        return MAX(time_to_timeout, idle_timeout_seconds * 1000 - lv_disp_get_inactive_time(NULL));
    }
}

zsw_power_manager_state_t zsw_power_manager_get_state(void)
{
    return state;
}

static void update_and_publish_state(zsw_power_manager_state_t new_state)
{
    state = new_state;

    struct activity_state_event evt = {
        .state = state,
    };
    zbus_chan_pub(&activity_state_data_chan, &evt, K_MSEC(250));
}

static void handle_idle_timeout(struct k_work *item)
{
    uint32_t last_lvgl_activity_ms = lv_disp_get_inactive_time(NULL);

    if (last_lvgl_activity_ms > idle_timeout_seconds * 1000) {
        enter_inactive();
    } else {
        k_work_schedule(&idle_work, K_MSEC(idle_timeout_seconds * 1000 - last_lvgl_activity_ms));
    }
}

static void zbus_accel_data_callback(const struct zbus_channel *chan)
{
    const struct accel_event *event = zbus_chan_const_msg(chan);

    switch (event->data.type) {
        case ZSW_IMU_EVT_TYPE_WRIST_WAKEUP: {
            if (!is_active) {
                LOG_DBG("Wakeup gesture detected");
                enter_active();
            }
            break;
        }
        case ZSW_IMU_EVT_TYPE_NO_MOTION: {
            LOG_INF("Watch enterted stationary state");
            if (!is_active) {
                is_stationary = true;
                last_pwr_off_time = k_uptime_get();
                zsw_display_control_pwr_ctrl(false);
                zsw_imu_feature_enable(ZSW_IMU_FEATURE_ANY_MOTION, true);
                zsw_imu_feature_disable(ZSW_IMU_FEATURE_NO_MOTION);

                update_and_publish_state(ZSW_ACTIVITY_STATE_NOT_WORN_STATIONARY);
            }
            break;
        }
        case ZSW_IMU_EVT_TYPE_ANY_MOTION: {
            LOG_INF("Watch moved, init display");
            if (!is_active) {
                is_stationary = false;
                zsw_display_control_pwr_ctrl(true);
                zsw_display_control_sleep_ctrl(false);
                retained.display_off_time += k_uptime_get_32() - last_pwr_off_time;
                zsw_retained_ram_update();
                zsw_imu_feature_enable(ZSW_IMU_FEATURE_NO_MOTION, true);
                zsw_imu_feature_disable(ZSW_IMU_FEATURE_ANY_MOTION);

                update_and_publish_state(ZSW_ACTIVITY_STATE_INACTIVE);
            }
            break;
        }
        case ZSW_IMU_EVT_TYPE_GESTURE: {
            if ((event->data.data.gesture == BOSCH_BMI270_GESTURE_FLICK_OUT) &&
                (idle_timeout_seconds != UINT32_MAX)) {
                LOG_INF("Put device into standby");
                enter_inactive();
            }
            break;
        }
        default:
            break;
    }
}

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event = zbus_chan_const_msg(chan);

    if (event->mV <= LOW_BATTERY_VOLTAGE_MV) {
        zsw_vibration_set_enabled(false);
    } else {
        zsw_vibration_set_enabled(true);
    }
}

static int settings_load_handler(const char *key, size_t len,
                                 settings_read_cb read_cb, void *cb_arg, void *param)
{
    int rc;
    zsw_settings_display_always_on_t *display_always_on = (zsw_settings_display_always_on_t *)param;
    if (len != sizeof(zsw_settings_display_always_on_t)) {
        return -EINVAL;
    }

    rc = read_cb(cb_arg, display_always_on, sizeof(zsw_settings_display_always_on_t));
    if (rc >= 0) {
        return 0;
    }

    return -ENODATA;
}

static int zsw_power_manager_init(void)
{
    int err;
    zsw_settings_display_always_on_t display_always_on;

    last_wakeup_time = k_uptime_get_32();
    last_pwr_off_time = k_uptime_get_32();
    settings_subsys_init();
    err = settings_load_subtree_direct(ZSW_SETTINGS_DISPLAY_ALWAYS_ON, settings_load_handler, &display_always_on);
    if (err == 0 && display_always_on) {
        idle_timeout_seconds = UINT32_MAX;
    }

    zsw_cpu_set_freq(ZSW_CPU_FREQ_FAST, true);

    k_work_schedule(&idle_work, K_SECONDS(idle_timeout_seconds));
    return 0;
}

SYS_INIT(zsw_power_manager_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);