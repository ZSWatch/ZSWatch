/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <stdio.h>
#include "app_version.h"
#include <version.h>
#include <ncs_version.h>
#include "about_ui.h"
#include "managers/zsw_app_manager.h"
#include "llext/zsw_llext_iflash.h"
#include "ui/utils/zsw_ui_utils.h"
#include "filesystem/zsw_filesystem.h"

#ifdef CONFIG_ZSW_LLEXT_APPS
#include <zephyr/llext/symbol.h>
#endif

static void about_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void about_app_stop(void *user_data);

static application_t app = {
    .name = "About",
    .icon = ZSW_LV_IMG_USE(templates),
    .start_func = about_app_start,
    .stop_func = about_app_stop,
    .category = ZSW_APP_CATEGORY_SYSTEM,
};

static void about_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    char version[50];
    char sdk_version[50];
    char build_time[50];
    char fs_stats[50];

#if CONFIG_STORE_IMAGES_EXTERNAL_FLASH
    snprintf(fs_stats, sizeof(fs_stats), "%d Files (%.2f MB)", zsw_filesytem_get_num_rawfs_files(),
             zsw_filesytem_get_total_size() / 1000000.0);
#else
    snprintf(fs_stats, sizeof(fs_stats), "%d Files", NUM_RAW_FS_FILES);
#endif
    snprintf(build_time, sizeof(build_time), "%s %s", __DATE__, __TIME__);
    snprintf(sdk_version, sizeof(sdk_version), "NCS: %s - Zephyr: %s", NCS_VERSION_STRING, KERNEL_VERSION_STRING);
    snprintf(version, sizeof(version), "v%s-%s", APP_VERSION_STRING, STRINGIFY(APP_BUILD_VERSION));
    about_ui_show(root, CONFIG_BOARD_TARGET, version, build_time, sdk_version, fs_stats, zsw_app_manager_get_num_apps());
}

static void about_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    about_ui_remove();
}

static int about_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

#ifdef CONFIG_ZSW_LLEXT_APPS
application_t *app_entry(void)
{
    LLEXT_TRAMPOLINE_APP_FUNCS(&app);
    about_app_add();
    return &app;
}
EXPORT_SYMBOL(app_entry);
#else
SYS_INIT(about_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#endif
