/**
 * @file speedometer_app.c
 * @author Leonardo Bispo
 *
 * @brief Speedometer app, report speed based on IMU Sensor fusion
 *
 */

#include <math.h>

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include "speedometer_ui.h"
#include "zsw_settings.h"
#include "managers/zsw_app_manager.h"
#include "../sensor_fusion/sensor_fusion.h"

#define SAMPLE_RATE_MS 1000

LOG_MODULE_REGISTER(speedometer_app, CONFIG_ZSW_SPEEDOMETER_APP_LOG_LEVEL);

// Functions needed for all applications
static void speedometer_app_start(lv_obj_t *root, lv_group_t *group);
static void speedometer_app_stop(void);
static void speedometer_calc_handler(struct k_work *item);

K_WORK_DELAYABLE_DEFINE(speedometer_calc, speedometer_calc_handler);
LV_IMG_DECLARE(speedometer);

static void speedometer_calc_handler(struct k_work *item)
{
    static float velocity_o = 0;
    static int64_t prev_time = 0;
    sensor_fusion_t fusion;

    /* update the time when first called */
    if (prev_time == 0) {
        prev_time = k_uptime_get();
    }

    /* Get all IMU sensors fused */
    sensor_fusion_fetch_all(&fusion);
    LOG_DBG("x:%.02f y: %.02f z: %.02f", fusion.x, fusion.y, fusion.z);

    /* Calculate the resultant 3D acceleration vector in g-force */
    float result_accel = sqrtf((fusion.x * fusion.x) + //
                               (fusion.y * fusion.y) + //
                               (fusion.z * fusion.z));
    LOG_DBG("vec result %.02fg", result_accel);

    /* Time derivative of the velocity function is acceleration -> d/dt v(t) = a(t) */
    /* Velocity(t) = V0 + at */
    int64_t time_f = k_uptime_get();
    float velocity_f = (result_accel) * 9.80665 * (time_f - prev_time) / 1000;

    /* If any of the acceleration vectors are negative it is deceleration */
    if ((fusion.x < 0) || (fusion.y < 0) || (fusion.z < 0)) {
        velocity_f = -velocity_f;
    }

    float speed = velocity_f - velocity_o;
    LOG_DBG("speed %.02fm/s", speed);

    /* Update velocity -1 */
    velocity_o = speed;
    prev_time = time_f;

    /// @todo need some speed delta rejection
    speedometer_set_value(speed * 3.6); // m/s to km/h

    k_work_schedule(&speedometer_calc, K_MSEC(SAMPLE_RATE_MS));
}

static application_t app = {
    .name = "Speedometer",
    .icon = &speedometer,
    .start_func = speedometer_app_start,
    .stop_func = speedometer_app_stop
};

static void speedometer_app_start(lv_obj_t *root, lv_group_t *group)
{
    // Screen always on
    zsw_settings_display_always_on_t display_always_on = true;
    settings_save_one(ZSW_SETTINGS_DISPLAY_ALWAYS_ON, &display_always_on,
                      sizeof(display_always_on));

    speedometer_ui_show(root);
    sensor_fusion_init();
    k_work_schedule(&speedometer_calc, K_MSEC(SAMPLE_RATE_MS));
}

static void speedometer_app_stop(void)
{
    // Screen always on - off
    zsw_settings_display_always_on_t display_always_on = false;
    settings_save_one(ZSW_SETTINGS_DISPLAY_ALWAYS_ON, &display_always_on,
                      sizeof(display_always_on));

    speedometer_ui_remove();
    sensor_fusion_deinit();
}

static int speedometer_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(speedometer_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
