#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include "app_version.h"
#include <version.h>
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
    char version[50];
    char sdk_version[50];
    char build_time[50];
    char fs_stats[50];
    snprintf(fs_stats, sizeof(fs_stats), "%d Files (X MB / %d MB)", NUM_RAW_FS_FILES, 16);
    snprintf(build_time, sizeof(build_time), "%s %s", __DATE__, __TIME__);
    snprintf(sdk_version, sizeof(sdk_version), "NCS: %s - Zephyr: %s", BANNER_VERSION, KERNEL_VERSION_STRING);
    snprintf(version, sizeof(version), "v%s-%s", APP_VERSION_STRING, STRINGIFY(APP_BUILD_VERSION));
    about_ui_show(root, CONFIG_ZSWATCH_PCB_REV, version, build_time, sdk_version, fs_stats, zsw_app_manager_get_num_apps());
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
