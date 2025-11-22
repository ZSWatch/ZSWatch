/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
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
#include <math.h>

#include "zsw_settings.h"
#include "zsw_cpu_freq.h"
#include "events/accel_event.h"
#include "events/battery_event.h"
#include "managers/zsw_power_manager.h"
#include "drivers/zsw_display_control.h"
#include "drivers/zsw_vibration_motor.h"

LOG_MODULE_REGISTER(zsw_power_manager, LOG_LEVEL_INF);

#ifdef CONFIG_ARCH_POSIX
#define IDLE_TIMEOUT_SECONDS    UINT32_MAX
#else
#define IDLE_TIMEOUT_SECONDS    10
#endif

#define POWER_MANAGEMENT_MIN_ACTIVE_PERIOD_SECONDS                  1
#define LOW_BATTERY_VOLTAGE_MV                                      3750

/* Tilt-based auto-off tuning */
/* How often to sample accelerometer while active (ms) */
#define TILT_SAMPLE_PERIOD_MS                                       500
/* Number of samples to average when learning reference orientation */
#define TILT_REF_SAMPLES                                            8
/* Require user inactivity for at least this long before tilt-off (ms) */
#define TILT_MIN_LVGL_IDLE_MS                                       1500
/* Cosine thresholds for "clearly facing" / "clearly away"
 * (dot product between current gravity vector and learned reference).
 *  cos(35deg) ~ 0.82, cos(70deg) ~ 0.34
 */
#define TILT_FACE_DOT_MIN                                           0.75f
#define TILT_AWAY_DOT_MAX                                           0.45f
/* How long tilt must stay in "away" region before turning off (ms) */
#define TILT_AWAY_HOLD_MS                                           800

static void update_and_publish_state(zsw_power_manager_state_t new_state);
static void handle_idle_timeout(struct k_work *item);
static void zbus_accel_data_callback(const struct zbus_channel *chan);
static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);
static void tilt_timeout(struct k_work *item);

K_WORK_DELAYABLE_DEFINE(idle_work, handle_idle_timeout);
K_WORK_DELAYABLE_DEFINE(tilt_work, tilt_timeout);

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

/* Tilt detection state */
static bool tilt_ref_valid;
static float tilt_ref_x;
static float tilt_ref_y;
static float tilt_ref_z;
static uint8_t tilt_ref_count;
static uint32_t tilt_away_start_ms;

static void tilt_reset_state(void)
{
    tilt_ref_valid = false;
    tilt_ref_x = 0.0f;
    tilt_ref_y = 0.0f;
    tilt_ref_z = 0.0f;
    tilt_ref_count = 0;
    tilt_away_start_ms = 0;
}

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

    // Publish inactive state immediately, before disabling display and XIP.
    update_and_publish_state(ZSW_ACTIVITY_STATE_INACTIVE);

    zsw_display_control_sleep_ctrl(false);

    zsw_cpu_set_freq(ZSW_CPU_FREQ_DEFAULT, true);

    // Screen inactive -> wait for NO_MOTION interrupt in order to power off display regulator.
    zsw_imu_feature_enable(ZSW_IMU_FEATURE_NO_MOTION, true);
    zsw_imu_feature_disable(ZSW_IMU_FEATURE_ANY_MOTION);
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

    /* Prepare tilt detection for this active session */
    tilt_reset_state();

    update_and_publish_state(ZSW_ACTIVITY_STATE_ACTIVE);

    k_work_schedule(&idle_work, K_SECONDS(idle_timeout_seconds));
    k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
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

static void tilt_timeout(struct k_work *item)
{
    ARG_UNUSED(item);

    /* Only run tilt logic while active and when an idle timeout is configured. */
    if (!is_active || (idle_timeout_seconds == UINT32_MAX)) {
        LOG_DBG("Tilt: skip (is_active=%d, idle_timeout_seconds=%u)",
                is_active, idle_timeout_seconds);
        return;
    }

    uint32_t lvgl_idle = lv_disp_get_inactive_time(NULL);
    LOG_WRN("Tlvgl_idle=%u ms", lvgl_idle);
    /* Do not consider tilt-off while there is very recent LVGL activity. */
    if (lvgl_idle < TILT_MIN_LVGL_IDLE_MS) {
        LOG_DBG("Tilt: LVGL recently active (%u ms < %u ms), skip",
                lvgl_idle, (uint32_t)TILT_MIN_LVGL_IDLE_MS);
        k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
        return;
    }

    float ax, ay, az;
    if (zsw_imu_fetch_accel_f(&ax, &ay, &az) != 0) {
        /* Try again later if sampling fails. */
        LOG_DBG("Tilt: zsw_imu_fetch_accel_f failed");
        k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
        return;
    }

    /* Compute magnitude of current gravity vector. */
    float mag_sq = ax * ax + ay * ay + az * az;
    if (mag_sq <= 0.0f) {
        LOG_DBG("Tilt: invalid accel magnitude");
        k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
        return;
    }

    float mag = sqrtf(mag_sq);

    if (!tilt_ref_valid) {
        /* Learn reference orientation during the first few samples of active period. */
        tilt_ref_x += ax / mag;
        tilt_ref_y += ay / mag;
        tilt_ref_z += az / mag;
        tilt_ref_count++;

        LOG_DBG("Tilt: learning ref, count=%u", tilt_ref_count);

        if (tilt_ref_count >= TILT_REF_SAMPLES) {
            float ref_mag_sq = tilt_ref_x * tilt_ref_x +
                               tilt_ref_y * tilt_ref_y +
                               tilt_ref_z * tilt_ref_z;
            if (ref_mag_sq > 0.0f) {
                float ref_mag = sqrtf(ref_mag_sq);
                tilt_ref_x /= ref_mag;
                tilt_ref_y /= ref_mag;
                tilt_ref_z /= ref_mag;
                tilt_ref_valid = true;
                LOG_INF("Tilt: reference learned (%.3f, %.3f, %.3f)",
                        tilt_ref_x, tilt_ref_y, tilt_ref_z);
            } else {
                /* Reset learning if something went wrong. */
                tilt_reset_state();
                LOG_DBG("Tilt: reference learning failed, reset");
            }
        }

        k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
        return;
    }

    /* Compute unit vector of current gravity and dot product vs reference. */
    float ux = ax / mag;
    float uy = ay / mag;
    float uz = az / mag;

    float dot = ux * tilt_ref_x + uy * tilt_ref_y + uz * tilt_ref_z;

    LOG_DBG("Tilt: dot=%.3f, away_start=%u", (double)dot, tilt_away_start_ms);

    /* Fully facing: reset any away timer. */
    if (dot >= TILT_FACE_DOT_MIN) {
        tilt_away_start_ms = 0;
        k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
        return;
    }

    /* Clearly away: require sustained condition before turning off. */
    if (dot <= TILT_AWAY_DOT_MAX) {
        uint32_t now = k_uptime_get_32();
        if (tilt_away_start_ms == 0) {
            tilt_away_start_ms = now;
            LOG_INF("Tilt: away region entered, starting timer");
        } else if ((now - tilt_away_start_ms) >= TILT_AWAY_HOLD_MS) {
            LOG_INF("Tilt: away held for %u ms, entering inactive",
                    (uint32_t)(now - tilt_away_start_ms));
            enter_inactive();
            return;
        }
    } else {
        /* In-between region: neither clearly facing nor clearly away. */
        tilt_away_start_ms = 0;
        LOG_DBG("Tilt: in-between region, reset away timer");
    }

    k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
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
    zsw_settings_display_always_on_t display_always_on = false;

    last_wakeup_time = k_uptime_get_32();
    last_pwr_off_time = k_uptime_get_32();
    settings_subsys_init();
    err = settings_load_subtree_direct(ZSW_SETTINGS_DISPLAY_ALWAYS_ON, settings_load_handler, &display_always_on);
    if (err == 0 && display_always_on) {
        idle_timeout_seconds = UINT32_MAX;
    }

    /* Start in ACTIVE state after boot so that display and tilt logic
     * follow the same path as any other wakeup.
     */
    enter_active();

    return 0;
}

SYS_INIT(zsw_power_manager_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
