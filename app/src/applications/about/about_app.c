#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include "about_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

static void about_app_start(lv_obj_t *root, lv_group_t *group);
static void about_app_stop(void);

LV_IMG_DECLARE(templates);

static application_t app = {
    .name = "About",
    .icon = &templates,
    .start_func = about_app_start,
    .stop_func = about_app_stop
};

static void about_app_start(lv_obj_t *root, lv_group_t *group)
{
    about_ui_show(root);
}

static void about_app_stop(void)
{
    about_ui_remove();
}

static int about_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(about_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
