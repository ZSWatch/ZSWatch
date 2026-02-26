// Copyright (c) 2025 ZSWatch Project
// SPDX-License-Identifier: Apache-2.0

/**
 * Default app template pre-loaded in the editor.
 */

export const DEFAULT_APP_SOURCE = `\
/*
 * ZSWatch LLEXT App
 *
 * This app runs as a dynamically loaded extension on the watch.
 * Modify the start/stop functions to build your own app.
 *
 * Upload the built .llext to the watch via the companion app
 * or mcumgr CLI to /lvgl_lfs/apps/<name>/app.llext
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <lvgl.h>

#include "managers/zsw_app_manager.h"
#include "llext/zsw_llext_iflash.h"
#include "ui/utils/zsw_ui_utils.h"
#include "llext/zsw_llext_log.h"

LOG_MODULE_REGISTER(my_app, LOG_LEVEL_INF);

/* ---------- Forward declarations ---------- */
static void my_app_start(lv_obj_t *root, lv_group_t *group);
static void my_app_stop(void);

/* ---------- App registration ---------- */
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

    label = lv_label_create(root);
    lv_label_set_text(label, "Hello from\\nLLEXT App!");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

static void my_app_stop(void)
{
    LOG_INF("My App stopped");
}

/* ---------- LLEXT entry point ---------- */
static int my_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

int app_entry(void)
{
    LLEXT_TRAMPOLINE_APP_FUNCS(&app);
    my_app_add();
    return 0;
}
EXPORT_SYMBOL(app_entry);
`;

export const DEFAULT_FILES = {
  'my_app.c': DEFAULT_APP_SOURCE,
};
