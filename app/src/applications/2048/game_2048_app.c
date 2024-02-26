#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <lv_100ask_2048.h>

#include "game_2048_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

// Functions needed for all applications
static void game_2048_app_start(lv_obj_t *root, lv_group_t *group);
static void game_2048_app_stop(void);

LV_IMG_DECLARE(icon_2048);

static application_t app = {
    .name = "2048",
    .icon = &icon_2048,
    .start_func = game_2048_app_start,
    .stop_func = game_2048_app_stop
};

static void game_2048_app_start(lv_obj_t *root, lv_group_t *group)
{
    game_2048_ui_show(root);
}

static void game_2048_app_stop(void)
{
    game_2048_ui_remove();
}

static int game_2048_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(game_2048_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
