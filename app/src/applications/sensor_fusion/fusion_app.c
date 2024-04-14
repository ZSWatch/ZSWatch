#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>

#include "fusion_ui.h"
#include "sensors/zsw_imu.h"
#include "sensor_fusion/zsw_sensor_fusion.h"
#include "events/zsw_periodic_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

static void zbus_fetch_fusion_data_callback(const struct zbus_channel *chan);
static void fusion_app_start(lv_obj_t *root, lv_group_t *group);
static void fusion_app_stop(void);
static void on_close_fusion(void);

ZSW_LV_IMG_DECLARE(move);

ZBUS_CHAN_DECLARE(periodic_event_100ms_chan);
ZBUS_LISTENER_DEFINE(accel_app_lis, zbus_fetch_fusion_data_callback);

static application_t app = {
    .name = "Sensor Fusion",
    .icon = ZSW_LV_IMG_USE(move),
    .start_func = fusion_app_start,
    .stop_func = fusion_app_stop
};

static void fusion_app_start(lv_obj_t *root, lv_group_t *group)
{
    fusion_ui_show(root, on_close_fusion);
    zsw_periodic_chan_add_obs(&periodic_event_100ms_chan, &accel_app_lis);
    zsw_sensor_fusion_init();
}

static void fusion_app_stop(void)
{
    zsw_periodic_chan_rm_obs(&periodic_event_100ms_chan, &accel_app_lis);
    zsw_sensor_fusion_deinit();
    fusion_ui_remove();
}

static void zbus_fetch_fusion_data_callback(const struct zbus_channel *chan)
{
    sensor_fusion_t fusion;

    zsw_sensor_fusion_fetch_all(&fusion);

    fusion_ui_set_values(fusion.roll, fusion.pitch, fusion.yaw);
}

static void on_close_fusion(void)
{
    zsw_app_manager_app_close_request(&app);
}

static int accel_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(accel_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
