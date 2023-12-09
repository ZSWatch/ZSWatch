#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include "calendar_ui.h"
#include "managers/zsw_app_manager.h"

static void calendar_app_start(lv_obj_t *root, lv_group_t *group);
static void calendar_app_stop(void);

LV_IMG_DECLARE(calendar);

static application_t app = {
    .name = "Calendar",
    .icon = &calendar,
    .start_func = calendar_app_start,
    .stop_func = calendar_app_stop
};

static void calendar_app_start(lv_obj_t *root, lv_group_t *group)
{
    calendar_ui_show(root);
}

static void calendar_app_stop(void)
{
    calendar_ui_remove();
}

static int calendar_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(calendar_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
