#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "calculator_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "smf_calculator_thread.h"

LOG_MODULE_REGISTER(calculator_app, LOG_LEVEL_DBG);

// Functions needed for all applications
static void calculator_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void calculator_app_stop(void *user_data);

ZSW_LV_IMG_DECLARE(statistic_icon);

static application_t app = {
    .name = "Calc",
    .icon = ZSW_LV_IMG_USE(statistic_icon),
    .start_func = calculator_app_start,
    .stop_func = calculator_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static void calculator_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    calculator_ui_show(root);
}

static void calculator_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    calculator_ui_remove();
}

static int calculator_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(calculator_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
