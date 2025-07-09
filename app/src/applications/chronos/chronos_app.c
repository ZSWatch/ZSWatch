#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "chronos_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(chronos_app, LOG_LEVEL_INF);

// Functions needed for all applications
static void chronos_app_start(lv_obj_t *root, lv_group_t *group);
static void chronos_app_stop(void);

ZSW_LV_IMG_DECLARE(chronos_logo_icon);

static application_t app = {
    .name = "Chronos",
    .icon = ZSW_LV_IMG_USE(chronos_logo_icon),
    .start_func = chronos_app_start,
    .stop_func = chronos_app_stop
};

static void chronos_app_start(lv_obj_t *root, lv_group_t *group)
{
    chronos_ui_init(root);
}

static void chronos_app_stop(void)
{
    chronos_ui_deinit();
}

static int chronos_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(chronos_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
