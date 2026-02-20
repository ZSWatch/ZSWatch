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
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <lvgl.h>
#include <math.h>

#include "activity_event.h"
#include "zsw_retained_ram_storage.h"
#include "zsw_settings.h"
#include "zsw_cpu_freq.h"
#include "accel_event.h"
#include "battery_event.h"
#include "zsw_power_manager.h"
#include "zsw_display_control.h"
#include "zsw_vibration_motor.h"

LOG_MODULE_REGISTER(zsw_power_manager, CONFIG_ZSW_PWR_MANAGER_LOG_LEVEL);

#ifdef CONFIG_ARCH_POSIX
#define IDLE_TIMEOUT_SECONDS    UINT32_MAX
#else
#define IDLE_TIMEOUT_SECONDS    60
#endif

#define POWER_MANAGEMENT_MIN_ACTIVE_PERIOD_SECONDS                  1
#define LOW_BATTERY_VOLTAGE_MV                                      3750

// How often to sample accelerometer and run the state machine
#define TILT_SAMPLE_PERIOD_MS                                       100
// Number of samples to average when learning reference orientation
#define TILT_REF_SAMPLES                                            4
// Require user inactivity for at least this long
#define TILT_MIN_LVGL_IDLE_MS                                       1500
// Cosine thresholds for "clearly facing" / "clearly away"
// (dot product between current gravity vector and learned reference).
#define TILT_FACE_DOT_MIN                                           0.75f
#define TILT_AWAY_DOT_MAX                                           0.45f
// How long watch must face away before tunning off
#define TILT_AWAY_HOLD_MS                                           800

static void enter_active(void);
static void enter_inactive(void);
static int settings_load_handler(const char *key, size_t len,
                                 settings_read_cb read_cb, void *cb_arg, void *param);
static void update_and_publish_state(zsw_power_manager_state_t new_state);
static void handle_idle_timeout(struct k_work *item);
static void tilt_request_reference_update(void);
static void update_last_activity_timestamp(void);
static void zbus_accel_data_callback(const struct zbus_channel *chan);
static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);
static void tilt_detection_work_handler(struct k_work *item);
static void tilt_update_reference(float ax, float ay, float az, float mag);
static void tilt_check_monitoring(float ux, float uy, float uz);

K_WORK_DELAYABLE_DEFINE(idle_work, handle_idle_timeout);
K_WORK_DELAYABLE_DEFINE(tilt_work, tilt_detection_work_handler);

ZBUS_CHAN_DECLARE(activity_state_data_chan);

ZBUS_LISTENER_DEFINE(power_manager_accel_lis, zbus_accel_data_callback);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_OBS_DECLARE(zsw_power_manager_bat_listener);
ZBUS_CHAN_ADD_OBS(battery_sample_data_chan, zsw_power_manager_bat_listener, 1);
ZBUS_LISTENER_DEFINE(zsw_power_manager_bat_listener, zbus_battery_sample_data_callback);

typedef enum {
    TILT_STATE_IDLE,
    TILT_STATE_LEARNING,
    TILT_STATE_MONITORING,
} tilt_state_t;

static uint32_t idle_timeout_seconds = IDLE_TIMEOUT_SECONDS;
static bool is_active = true;
static bool is_stationary;
static uint32_t last_wakeup_time;
static uint32_t last_pwr_off_time;
static uint32_t last_activity_time_ms;
static zsw_power_manager_state_t state;
static bool iniitialized = false;

static struct {
    tilt_state_t state;
    float ref_x;
    float ref_y;
    float ref_z;
    uint8_t ref_count;
    uint32_t away_start_ms;
} tilt;

int zsw_power_manager_init(void)
{
    int err;
    zsw_settings_display_always_on_t display_always_on = false;

    last_wakeup_time = k_uptime_get_32();
    last_pwr_off_time = k_uptime_get_32();
    last_activity_time_ms = last_wakeup_time;
    settings_subsys_init();
    err = settings_load_subtree_direct(ZSW_SETTINGS_DISPLAY_ALWAYS_ON, settings_load_handler, &display_always_on);
    if (err == 0 && display_always_on) {
        idle_timeout_seconds = UINT32_MAX;
    }

    iniitialized = true;
    enter_active();

    return 0;
}

bool zsw_power_manager_reset_idle_timout(void)
{
    __ASSERT(iniitialized, "Power manager not initialized");
    update_last_activity_timestamp();

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
    __ASSERT(iniitialized, "Power manager not initialized");
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
    __ASSERT(iniitialized, "Power manager not initialized");
    return state;
}

void zsw_power_manager_on_user_activity(void)
{
    __ASSERT(iniitialized, "Power manager not initialized");

    update_last_activity_timestamp();

    if (is_active) {
        tilt_request_reference_update();
    }
}

static void tilt_request_reference_update(void)
{
    tilt.state = TILT_STATE_LEARNING;
    tilt.ref_x = 0.0f;
    tilt.ref_y = 0.0f;
    tilt.ref_z = 0.0f;
    tilt.ref_count = 0;
    tilt.away_start_ms = 0;
}

static void update_last_activity_timestamp(void)
{
    last_activity_time_ms = k_uptime_get_32();
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

    // Cancel pending idle timeout since we're already going inactive
    k_work_cancel_delayable(&idle_work);

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
    update_last_activity_timestamp();

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

    tilt_request_reference_update();

    update_and_publish_state(ZSW_ACTIVITY_STATE_ACTIVE);

    k_work_schedule(&idle_work, K_SECONDS(idle_timeout_seconds));
    k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
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
    if (idle_timeout_seconds == UINT32_MAX) {
        return;
    }

    uint32_t last_lvgl_activity_ms = lv_disp_get_inactive_time(NULL);

    if (last_lvgl_activity_ms > idle_timeout_seconds * 1000) {
        enter_inactive();
    } else {
        k_work_schedule(&idle_work, K_MSEC(idle_timeout_seconds * 1000 - last_lvgl_activity_ms));
    }
}

/**
 * @brief Update the tilt reference orientation by averaging samples.
 * @param ax, ay, az Raw accelerometer values in m/s²
 * @param mag Magnitude of the accelerometer vector
 */
static void tilt_update_reference(float ax, float ay, float az, float mag)
{
    // By checking magnitude we can avoid using samples taken during high motion.
    // 5.0 to 15.0 m/s² (approximately 0.5g to 1.5g) allows for sensor noise, but will
    // reject samples taken during higher motion.
    if ((mag < 5.0f) || (mag > 15.0f)) {
        LOG_DBG("Tilt: skip ref sample, mag=%.2f", (double)mag);
        return;
    }

    // Accumulate normalized samples
    tilt.ref_x += ax / mag;
    tilt.ref_y += ay / mag;
    tilt.ref_z += az / mag;
    tilt.ref_count++;
    LOG_DBG("Tilt: learning ref, count=%u", tilt.ref_count);

    if (tilt.ref_count < TILT_REF_SAMPLES) {
        return;
    }

    // Normalize the accumulated vector
    float ref_mag_sq = tilt.ref_x * tilt.ref_x +
                       tilt.ref_y * tilt.ref_y +
                       tilt.ref_z * tilt.ref_z;

    if (ref_mag_sq > 0.0f) {
        float ref_mag = sqrtf(ref_mag_sq);
        tilt.ref_x /= ref_mag;
        tilt.ref_y /= ref_mag;
        tilt.ref_z /= ref_mag;
        tilt.state = TILT_STATE_MONITORING;
        LOG_INF("Tilt: reference learned (%.3f, %.3f, %.3f)",
                tilt.ref_x, tilt.ref_y, tilt.ref_z);
    } else {
        LOG_WRN("Tilt: reference learning failed, reset");
        tilt_request_reference_update();
    }
}

/**
 * @brief Check if watch is tilted away from user and handle state transitions.
 * @param ux, uy, uz Normalized gravity vector
 */
static void tilt_check_monitoring(float ux, float uy, float uz)
{
    // Calculate dot product between current gravity vector and reference
    // 1.0 means they’re perfectly aligned (same direction).
    // 0.0 means they’re orthogonal (90° apart).
    float dot = ux * tilt.ref_x + uy * tilt.ref_y + uz * tilt.ref_z;

    LOG_DBG("Tilt: dot=%.3f, away_start=%u", (double)dot, tilt.away_start_ms);

    // Watch is facing user
    if (dot >= TILT_FACE_DOT_MIN) {
        tilt.away_start_ms = 0;
        return;
    }

    // Watch is tilted away
    if (dot <= TILT_AWAY_DOT_MAX) {
        uint32_t now = k_uptime_get_32();

        if (tilt.away_start_ms == 0) {
            tilt.away_start_ms = now;
            LOG_INF("Tilt: away region entered, starting timer");
        } else if ((now - tilt.away_start_ms) >= TILT_AWAY_HOLD_MS) {
            LOG_INF("Tilt: away held for %u ms, entering inactive",
                    (uint32_t)(now - tilt.away_start_ms));
            enter_inactive();
        }
        return;
    }

    // Between facing and away, just reset away timer
    tilt.away_start_ms = 0;
    LOG_DBG("Tilt: Between facing and away, reset away timer");
}

static void tilt_detection_work_handler(struct k_work *item)
{
    ARG_UNUSED(item);

    if (!is_active || (idle_timeout_seconds == UINT32_MAX)) {
        return;
    }

    float ax, ay, az;
    if (zsw_imu_fetch_accel_f(&ax, &ay, &az) != 0) {
        LOG_ERR("Tilt: zsw_imu_fetch_accel_f failed");
        k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
        return;
    }

    float mag_sq = ax * ax + ay * ay + az * az;
    if (mag_sq <= 0.0f) {
        k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
        return;
    }
    float mag = sqrtf(mag_sq);

    switch (tilt.state) {
        case TILT_STATE_IDLE:
            break;
        case TILT_STATE_LEARNING:
            tilt_update_reference(ax, ay, az, mag);
            break;
        case TILT_STATE_MONITORING: {
            // Check if minimum idle time elapsed
            uint32_t idle_elapsed_ms = k_uptime_get_32() - last_activity_time_ms;
            if (idle_elapsed_ms < TILT_MIN_LVGL_IDLE_MS) {
                LOG_DBG("Tilt: recent activity (%u ms < %u ms), skip",
                        idle_elapsed_ms, (uint32_t)TILT_MIN_LVGL_IDLE_MS);
                break;
            }
            // Normalize and check monitoring
            tilt_check_monitoring(ax / mag, ay / mag, az / mag);
            break;
        }
    }

    k_work_schedule(&tilt_work, K_MSEC(TILT_SAMPLE_PERIOD_MS));
}

static void zbus_accel_data_callback(const struct zbus_channel *chan)
{
    const struct accel_event *event = zbus_chan_const_msg(chan);

    switch (event->data.type) {
        case ZSW_IMU_EVT_TYPE_WRIST_WAKEUP: {
            if (!is_active) {
                LOG_INF("Wrist wakeup gesture detected");
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
