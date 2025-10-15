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
#include <zephyr/zbus/zbus.h>

#include "fusion_ui.h"
#include "sensors/zsw_imu.h"
#include "sensor_fusion/zsw_sensor_fusion.h"
#include "events/zsw_periodic_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void zbus_fetch_fusion_data_callback(const struct zbus_channel *chan);
static void fusion_app_start(lv_obj_t *root, lv_group_t *group);
static void fusion_app_stop(void);
static void fusion_app_ui_unavailable(void);
static void fusion_app_ui_available(void);
static void on_close_fusion(void);
static void cube_timer_cb(lv_timer_t *t);
static void on_zero_yaw(void);

ZSW_LV_IMG_DECLARE(imu_sensor_icon);

ZBUS_CHAN_DECLARE(periodic_event_100ms_chan);
ZBUS_LISTENER_DEFINE(accel_app_lis, zbus_fetch_fusion_data_callback);

static application_t app = {
    .name = "Sensor Fusion",
    .icon = ZSW_LV_IMG_USE(imu_sensor_icon),
    .start_func = fusion_app_start,
    .stop_func = fusion_app_stop,
    .category = ZSW_APP_CATEGORY_SENSORS,
    .ui_unavailable_func = fusion_app_ui_unavailable,
    .ui_available_func = fusion_app_ui_available,
};

static lv_timer_t *cube_timer = NULL;
static uint32_t cube_timer_period_ms = 33;
static float yaw_zero_offset_rad = 0.0f;
static int32_t last_render_tick_ms = 0;
static uint32_t render_frames = 0;
static float last_render_hz = 0.0f;
static float latest_yaw_deg = 0.0f;

static void fusion_app_start(lv_obj_t *root, lv_group_t *group)
{
    fusion_ui_show(root, on_close_fusion, on_zero_yaw);
    zsw_periodic_chan_add_obs(&periodic_event_100ms_chan, &accel_app_lis);
    zsw_sensor_fusion_init();

    // Create periodic UI refresh for cube rendering
    if (!cube_timer) {
        cube_timer = lv_timer_create(cube_timer_cb, cube_timer_period_ms, NULL);
    }
}

static void fusion_app_stop(void)
{
    zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &accel_app_lis);
    zsw_sensor_fusion_deinit();
    if (cube_timer) {
        lv_timer_del(cube_timer);
        cube_timer = NULL;
    }
    fusion_ui_remove();
}

static void fusion_app_ui_unavailable(void)
{
    // Stop cube rendering when UI is not available
    if (cube_timer) {
        lv_timer_pause(cube_timer);
    }
}

static void fusion_app_ui_available(void)
{
    // Resume cube rendering when UI is available
    if (cube_timer) {
        lv_timer_resume(cube_timer);
    }
}

static void zbus_fetch_fusion_data_callback(const struct zbus_channel *chan)
{
    sensor_fusion_t fusion;

    zsw_sensor_fusion_fetch_all(&fusion);
    latest_yaw_deg = fusion.yaw;

    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        fusion_ui_set_values(fusion.roll, fusion.pitch, fusion.yaw);
    }
}

static void on_close_fusion(void)
{
    zsw_app_manager_app_close_request(&app);
}

static void cube_timer_cb(lv_timer_t *t)
{
    LV_UNUSED(t);
    zsw_quat_t q;
    if (zsw_sensor_fusion_get_quaternion(&q) == 0) {
        // This code allows to reset the yaw to zero using "zero" button
        // Apply yaw-zero correction: r_z(-offset) * q
        float half = -yaw_zero_offset_rad * 0.5f;
        float rw = cosf(half);
        float rz = sinf(half);
        // r = [w=rw, x=0, y=0, z=rz]
        zsw_quat_t qc;
        qc.w = rw * q.w - 0 * q.x - 0 * q.y - rz * q.z;
        qc.x = rw * q.x + 0 * q.w + 0 * q.z - rz * q.y;
        qc.y = rw * q.y - 0 * q.z + 0 * q.w + rz * q.x;
        qc.z = rw * q.z + 0 * q.y - 0 * q.x + rz * q.w;
        fusion_ui_set_quaternion(qc.w, qc.x, qc.y, qc.z);
    }

    // Render FPS
    render_frames++;
    int32_t now = (int32_t)k_uptime_get_32();
    if (last_render_tick_ms == 0) {
        last_render_tick_ms = now;
    }
    int32_t dt = now - last_render_tick_ms;
    if (dt >= 1000) {
        last_render_hz = (float)render_frames * 1000.0f / (float)dt;
        render_frames = 0;
        last_render_tick_ms = now;
        fusion_ui_set_stats(last_render_hz);
    }
}

static void on_zero_yaw(void)
{
    yaw_zero_offset_rad = latest_yaw_deg * (float)M_PI / 180.0f;
}

static int accel_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(accel_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
