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
#include <zephyr/logging/log.h>

#include "watchface_picker_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "applications/watchface/watchface_app.h"

LOG_MODULE_REGISTER(watchface_picker_app, CONFIG_ZSW_REMOTE_APP_LOG_LEVEL);

static void watchface_picker_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void watchface_picker_app_stop(void *user_data);

ZSW_LV_IMG_DECLARE(watchface_picker_icon);

static application_t app = {
    .name = "Face",
    .icon = ZSW_LV_IMG_USE(watchface_picker_icon),
    .start_func = watchface_picker_app_start,
    .stop_func = watchface_picker_app_stop,
    .category = ZSW_APP_CATEGORY_SYSTEM
};

static void on_watchface_selected(int index)
{
    watchface_change(index);
    zsw_app_manager_app_close_request(&app);
}

static void watchface_picker_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    watchface_picker_ui_show(root, on_watchface_selected);
    for (int i = 0; i < watchface_app_get_num_faces(); i++) {
        const char *name;
        const lv_img_dsc_t *img;
        watchface_app_get_face_info(i, &img, &name);
        watchface_picker_ui_add_watchface(img, name,  i);
    }

    watchface_picker_ui_set_selected(watchface_app_get_current_face());
}

static void watchface_picker_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    watchface_picker_ui_remove();
}

static int watchface_picker_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(watchface_picker_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
