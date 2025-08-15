#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include "drivers/zsw_display_control.h"

#include "x_ray_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

// Functions needed for all applications
static void x_ray_app_start(lv_obj_t *root, lv_group_t *group);
static void x_ray_app_stop(void);

ZSW_LV_IMG_DECLARE(circuit_icon);

static application_t app = {
    .name = "X-ray",
    .icon = ZSW_LV_IMG_USE(circuit_icon),
    .start_func = x_ray_app_start,
    .stop_func = x_ray_app_stop,
    .category = ZSW_APP_CATEGORY_RANDOM
};

static uint8_t original_brightness;

static void x_ray_app_start(lv_obj_t *root, lv_group_t *group)
{
    original_brightness = zsw_display_control_get_brightness();
    zsw_display_control_set_brightness(100);
    x_ray_ui_show(root);
}

static void x_ray_app_stop(void)
{
    zsw_display_control_set_brightness(original_brightness);
    x_ray_ui_remove();
}

static int x_ray_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(x_ray_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
