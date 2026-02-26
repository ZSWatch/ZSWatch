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

/*
 * ZSWatch LLEXT App Template
 *
 * This is a minimal app that demonstrates how to create an LLEXT dynamic app
 * for ZSWatch. Modify the start/stop functions to build your own app.
 *
 * Build with the ZSWatch EDK:
 *   cmake -B build -DLLEXT_EDK_INSTALL_DIR=/path/to/zswatch-edk
 *   make -C build
 *
 * Upload the resulting .llext to the watch:
 *   mcumgr fs upload build/my_app.llext /lvgl_lfs/apps/my_app_ext/app.llext
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <lvgl.h>

#include "managers/zsw_app_manager.h"
#include "llext/zsw_llext_iflash.h"
#include "ui/utils/zsw_ui_utils.h"

/* Use the LLEXT logging macros (routes through firmware log system) */
#include "llext/zsw_llext_log.h"
LOG_MODULE_REGISTER(my_app, LOG_LEVEL_INF);

/* ---------- Forward declarations ---------- */
static void my_app_start(lv_obj_t *root, lv_group_t *group);
static void my_app_stop(void);

/* ---------- App registration ---------- */

/*
 * App icon: references an image on external flash.
 * ZSW_LV_IMG_USE(name) resolves to "S:name.bin" on hardware.
 * Use an existing icon name from the firmware image set, or upload your own.
 */
static application_t app = {
    .name = "My App",
    .icon = ZSW_LV_IMG_USE(templates),
    .start_func = my_app_start,
    .stop_func = my_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

/* ---------- UI ---------- */

static lv_obj_t *label;

static void my_app_start(lv_obj_t *root, lv_group_t *group)
{
    LOG_INF("My App started");

    /* Create a simple centered label */
    label = lv_label_create(root);
    lv_label_set_text(label, "Hello from\nLLEXT App!");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

static void my_app_stop(void)
{
    LOG_INF("My App stopped");
    /* No explicit cleanup needed — the app manager deletes root and its children */
}

/* ---------- LLEXT entry point ---------- */

static int my_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

int app_entry(void)
{
    /* Wrap function pointers with R9-restoring trampolines (required for PIC) */
    LLEXT_TRAMPOLINE_APP_FUNCS(&app);
    my_app_add();
    return 0;
}
EXPORT_SYMBOL(app_entry);
