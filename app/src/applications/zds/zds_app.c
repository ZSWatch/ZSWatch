#include <zds/zds_ui.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>

#include "events/accel_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

// Functions needed for all applications
static void zds_app_start(lv_obj_t *root, lv_group_t *group);
static void zds_app_stop(void);

static void zbus_accel_data_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(accel_data_chan);
ZBUS_LISTENER_DEFINE_WITH_ENABLE(zds_app_accel_lis, zbus_accel_data_callback, false);

ZSW_LV_IMG_DECLARE(zephyr_icon_round);

static application_t app = {
    .name = "ZDS App",
    .icon = ZSW_LV_IMG_USE(zephyr_icon_round),
    .start_func = zds_app_start,
    .stop_func = zds_app_stop,
    .category = ZSW_APP_CATEGORY_RANDOM
};

static void zds_app_start(lv_obj_t *root, lv_group_t *group)
{
    zds_ui_show(root);
    zbus_chan_add_obs(&accel_data_chan, &zds_app_accel_lis, K_MSEC(100));
}

static void zds_app_stop(void)
{
    zbus_chan_rm_obs(&accel_data_chan, &zds_app_accel_lis, K_MSEC(100));
    zds_ui_remove();
}

static int zds_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

static void zbus_accel_data_callback(const struct zbus_channel *chan)
{
    const struct accel_event *event = zbus_chan_const_msg(chan);
    if (event->data.data.gesture == ZSW_IMU_EVT_GESTURE_WRIST_SHAKE) {
        zds_ui_reset_rotation();
    }
}

SYS_INIT(zds_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
