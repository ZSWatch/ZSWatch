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
#include "drivers/zsw_display_control.h"

#include "qr_code_ui.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

#ifdef CONFIG_ZSW_LLEXT_APPS
#include <zephyr/llext/symbol.h>
#else
ZSW_LV_IMG_DECLARE(qr_code_icon);
#endif

// Functions needed for all applications
static void qr_code_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void qr_code_app_stop(void *user_data);

static application_t app = {
    .name = "QR",
#ifdef CONFIG_ZSW_LLEXT_APPS
    /* icon set at runtime in app_entry() — PIC linker drops static relocation */
#else
    .icon = ZSW_LV_IMG_USE(qr_code_icon),
#endif
    .start_func = qr_code_app_start,
    .stop_func = qr_code_app_stop,
    .category = ZSW_APP_CATEGORY_RANDOM,
};

static uint8_t original_brightness;

static void qr_code_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    original_brightness = zsw_display_control_get_brightness();
    zsw_display_control_set_brightness(100);
    qr_code_ui_show(root);
}

static void qr_code_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    zsw_display_control_set_brightness(original_brightness);
    qr_code_ui_remove();
}

#ifdef CONFIG_ZSW_LLEXT_APPS
application_t *app_entry(void)
{
    /* Set icon at runtime — static relocation is lost by the PIC linker */
    app.icon = "S:qr_code_icon.bin";
    zsw_app_manager_add_application(&app);
    return &app;
}
EXPORT_SYMBOL(app_entry);
#else
static int qr_code_app_add(void)
{
    zsw_app_manager_add_application(&app);

    return 0;
}

SYS_INIT(qr_code_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#endif
