#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "watchface_picker_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "applications/watchface/watchface_app.h"

LOG_MODULE_REGISTER(watchface_picker_app, CONFIG_ZSW_REMOTE_APP_LOG_LEVEL);

static void watchface_picker_app_start(lv_obj_t *root, lv_group_t *group);
static void watchface_picker_app_stop(void);

ZSW_LV_IMG_DECLARE(watchface_picker_icon);

static application_t app = {
    .name = "Watchface Picker",
    .icon = ZSW_LV_IMG_USE(watchface_picker_icon),
    .start_func = watchface_picker_app_start,
    .stop_func = watchface_picker_app_stop
};

static void on_watchface_selected(int index)
{
    watchface_change(index);
    zsw_app_manager_app_close_request(&app);
}

static void watchface_picker_app_start(lv_obj_t *root, lv_group_t *group)
{
    watchface_picker_ui_show(root, on_watchface_selected);
    for (int i = 0; i < watchface_app_get_num_faces(); i++) {
        watchface_picker_ui_add_watchface(watchface_app_get_face_info(i), i);
    }

    watchface_picker_ui_set_selected(watchface_app_get_current_face());
}

static void watchface_picker_app_stop(void)
{
    watchface_picker_ui_remove();
}

static int watchface_picker_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(watchface_picker_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
