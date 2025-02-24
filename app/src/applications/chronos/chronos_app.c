#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "chronos_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(template_app, CONFIG_ZSW_TEMPLATE_APP_LOG_LEVEL);

// Functions needed for all applications
static void template_app_start(lv_obj_t *root, lv_group_t *group);
static void template_app_stop(void);


ZSW_LV_IMG_DECLARE(image_chronos_icon);

static application_t app = {
    .name = "Chronos",
    .icon = ZSW_LV_IMG_USE(image_chronos_icon),
    .start_func = template_app_start,
    .stop_func = template_app_stop
};


static void template_app_start(lv_obj_t *root, lv_group_t *group)
{
    chronos_ui_init(root);
    
}

static void template_app_stop(void)
{

    chronos_ui_deinit();
}


static int template_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(template_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
