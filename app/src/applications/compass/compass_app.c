#include <compass/compass_ui.h>
#include <application_manager.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <magnetometer.h>

LOG_MODULE_REGISTER(compass_app, LOG_LEVEL_DBG);

#define SENSOR_REFRESH_INTERVAL_MS  10

// Functions needed for all applications
static void compass_app_start(lv_obj_t *root, lv_group_t *group);
static void compass_app_stop(void);

// Functions related to app functionality
static void timer_callback(lv_timer_t *timer);

LV_IMG_DECLARE(move);

static application_t app = {
    .name = "Compass",
    .icon = &move,
    .start_func = compass_app_start,
    .stop_func = compass_app_stop
};

static lv_timer_t *refresh_timer;

static void compass_app_start(lv_obj_t *root, lv_group_t *group)
{
    compass_ui_show(root);
    refresh_timer = lv_timer_create(timer_callback, SENSOR_REFRESH_INTERVAL_MS,  NULL);
}

static void compass_app_stop(void)
{
    lv_timer_del(refresh_timer);
    compass_ui_remove();
}

static void timer_callback(lv_timer_t *timer)
{
    double heading = magnetometer_get_heading();
    compass_ui_set_heading(heading);
}

static int compass_app_add(const struct device *arg)
{
    application_manager_add_application(&app);
    return 0;
}

SYS_INIT(compass_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
