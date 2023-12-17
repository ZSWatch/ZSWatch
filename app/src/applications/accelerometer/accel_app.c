#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>

#include "accel_ui.h"
#include "sensors/zsw_imu.h"
#include "events/zsw_periodic_event.h"
#include "managers/zsw_app_manager.h"

static void zbus_fetch_accel_data_callback(const struct zbus_channel *chan);
static void accel_app_start(lv_obj_t *root, lv_group_t *group);
static void accel_app_stop(void);
static void on_close_accel(void);

LV_IMG_DECLARE(move);

ZBUS_CHAN_DECLARE(periodic_event_100ms_chan);
ZBUS_LISTENER_DEFINE(accel_app_lis, zbus_fetch_accel_data_callback);

static application_t app = {
    .name = "Accelerometer",
    .icon = &move,
    .start_func = accel_app_start,
    .stop_func = accel_app_stop
};

static void accel_app_start(lv_obj_t *root, lv_group_t *group)
{
    accel_ui_show(root, on_close_accel);
    zsw_periodic_chan_add_obs(&periodic_event_100ms_chan, &accel_app_lis);
}

static void accel_app_stop(void)
{
    zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &accel_app_lis);
    accel_ui_remove();
}

static void zbus_fetch_accel_data_callback(const struct zbus_channel *chan)
{
    int16_t x;
    int16_t y;
    int16_t z;
    int res = zsw_imu_fetch_accel(&x, &y, &z);
    if (res == 0) {
        accel_ui_set_values(x, y, z);
    }
}

static void on_close_accel(void)
{
    zsw_app_manager_app_close_request(&app);
}

static int accel_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(accel_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
