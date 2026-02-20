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
#include <zephyr/logging/log.h>

#include "app_picker_ui.h"
#include "app_picker_gen.h"
#include "lvgl_editor_gen.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(app_picker_ui, LOG_LEVEL_DBG);

/* Number of items displayed per page (6 outer + 1 center) */
#define ITEMS_PER_PAGE 7

/* Number of slots on the circular picker */
#define NUM_SLOTS 7

/* Maximum number of picker items (apps + folders) */
#define MAX_PICKER_ITEMS 32

ZSW_LV_IMG_DECLARE(close_icon);

typedef enum {
    PICKER_ITEM_APP,
    PICKER_ITEM_FOLDER
} picker_item_type_t;
typedef struct {
    picker_item_type_t type;
    union {
        application_t *app;
        struct {
            zsw_app_category_t category;
            const char *name;
            const void *icon;
            lv_color_t color;
        } folder;
    };
} picker_item_t;

static const zsw_app_folder_info_t *folder_info;

static lv_obj_t *picker_root;
static app_picker_on_app_selected_cb app_selected_cb;
static int current_page;
static int total_pages;

static int last_page = 0;
static zsw_app_category_t last_folder = ZSW_APP_CATEGORY_INVALID;

static lv_obj_t *app_container;
static lv_obj_t *app_slots[NUM_SLOTS];
static lv_obj_t *app_icons[NUM_SLOTS];
static lv_obj_t *app_labels[NUM_SLOTS];
static lv_obj_t *app_icon_bgs[NUM_SLOTS];
static lv_obj_t *nav_left;
static lv_obj_t *nav_right;
static lv_obj_t *folder_overlay;
static lv_obj_t *folder_grid;
static lv_obj_t *folder_title_label;
static lv_obj_t *folder_close_btn;
static lv_obj_t *folder_close_icon;

static picker_item_t picker_items[MAX_PICKER_ITEMS];
static int num_picker_items;

static picker_item_t *slot_items[NUM_SLOTS];

static zsw_app_category_t open_folder = ZSW_APP_CATEGORY_INVALID;

static void populate_page(int page_index);
static void update_nav_arrows(void);
static void build_picker_items(void);
static bool folder_has_apps(zsw_app_category_t category);
static void show_folder_contents(zsw_app_category_t category);
static void folder_app_clicked_cb(lv_event_t *e);
static void next_page(void);
static void prev_page(void);
static void on_swipe_event(lv_event_t *e);

/**
 * @brief Cache object references from the XML-generated UI
 */
static void cache_object_references(void)
{
    if (picker_root == NULL) {
        LOG_ERR("picker_root is NULL");
        return;
    }

    // TODO: When updating to newer LVGL use proper functions to get children by name
    app_container = lv_obj_get_child(picker_root, 0);
    nav_left = lv_obj_get_child(picker_root, 1);
    nav_right = lv_obj_get_child(picker_root, 2);
    folder_overlay = lv_obj_get_child(picker_root, 3);

    if (app_container == NULL) {
        LOG_ERR("app_container not found");
        return;
    }

    for (int i = 0; i < NUM_SLOTS; i++) {
        app_slots[i] = lv_obj_get_child(app_container, i);

        if (app_slots[i] != NULL) {
            app_icon_bgs[i] = lv_obj_get_child(app_slots[i], 0);
            app_labels[i] = lv_obj_get_child(app_slots[i], 1);

            if (app_icon_bgs[i] != NULL) {
                app_icons[i] = lv_obj_get_child(app_icon_bgs[i], 0);
            }

            lv_obj_set_user_data(app_slots[i], (void *)(intptr_t)i);
        }
    }

    if (folder_overlay != NULL) {
        folder_title_label = lv_obj_get_child(folder_overlay, 0);
        folder_grid = lv_obj_get_child(folder_overlay, 1);
        folder_close_btn = lv_obj_get_child(folder_overlay, 2);

        if (folder_close_btn != NULL) {
            folder_close_icon = lv_obj_get_child(folder_close_btn, 0);
            if (folder_close_icon != NULL) {
                lv_image_set_src(folder_close_icon, ZSW_LV_IMG_USE(close_icon));
            }
        }
    }
}

static bool folder_has_apps(zsw_app_category_t category)
{
    int num_apps = zsw_app_manager_get_num_apps();

    for (int i = 0; i < num_apps; i++) {
        application_t *app = zsw_app_manager_get_app(i);
        if (app && !app->hidden && app->category == category) {
            return true;
        }
    }

    return false;
}

static void build_picker_items(void)
{
    num_picker_items = 0;
    int num_apps = zsw_app_manager_get_num_apps();

    /* First, add all ROOT category apps */
    for (int i = 0; i < num_apps && num_picker_items < MAX_PICKER_ITEMS; i++) {
        application_t *app = zsw_app_manager_get_app(i);
        if (app && !app->hidden && app->category == ZSW_APP_CATEGORY_ROOT) {
            picker_items[num_picker_items].type = PICKER_ITEM_APP;
            picker_items[num_picker_items].app = app;
            num_picker_items++;
        }
    }

    /* Then, add folders that have apps (skip ROOT folder) */
    for (int cat = ZSW_APP_CATEGORY_TOOLS; cat < ZSW_APP_CATEGORY_COUNT && num_picker_items < MAX_PICKER_ITEMS; cat++) {
        if (folder_has_apps((zsw_app_category_t)cat)) {
            picker_items[num_picker_items].type = PICKER_ITEM_FOLDER;
            picker_items[num_picker_items].folder.category = (zsw_app_category_t)cat;
            picker_items[num_picker_items].folder.name = folder_info[cat].name;
            picker_items[num_picker_items].folder.icon = folder_info[cat].icon;
            picker_items[num_picker_items].folder.color = folder_info[cat].color;
            num_picker_items++;
        }
    }
}

static int calculate_page_count(void)
{
    if (num_picker_items == 0) {
        return 1;
    }
    return (num_picker_items + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
}

static void next_page(void)
{
    if (current_page < total_pages - 1) {
        populate_page(current_page + 1);
    }
}

static void prev_page(void)
{
    if (current_page > 0) {
        populate_page(current_page - 1);
    }
}

static void on_swipe_event(lv_event_t *e)
{
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

    if (open_folder != ZSW_APP_CATEGORY_INVALID) {
        return;
    }

    if (dir == LV_DIR_LEFT) {
        next_page();
    } else if (dir == LV_DIR_RIGHT) {
        prev_page();
    }
    lv_indev_wait_release(lv_indev_get_act());
}

static void populate_slot(int slot_index, picker_item_t *item)
{
    if (slot_index < 0 || slot_index >= NUM_SLOTS) {
        return;
    }

    slot_items[slot_index] = item;

    if (app_slots[slot_index] == NULL) {
        return;
    }

    if (item == NULL) {
        lv_obj_add_flag(app_slots[slot_index], LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_remove_flag(app_slots[slot_index], LV_OBJ_FLAG_HIDDEN);

    if (item->type == PICKER_ITEM_APP) {
        application_t *app = item->app;

        if (app_icons[slot_index]) {
            if (app->icon) {
                LOG_DBG("Slot %d app '%s': icon=%p '%s'", slot_index, app->name ? app->name : "?",
                        app->icon, (const char *)app->icon);
                lv_image_set_src(app_icons[slot_index], app->icon);
                lv_obj_remove_flag(app_icons[slot_index], LV_OBJ_FLAG_HIDDEN);
            } else {
                LOG_DBG("Slot %d app '%s': icon=NULL, hiding", slot_index, app->name ? app->name : "?");
                lv_obj_add_flag(app_icons[slot_index], LV_OBJ_FLAG_HIDDEN);
            }
            lv_obj_set_style_image_recolor_opa(app_icons[slot_index], LV_OPA_TRANSP, LV_PART_MAIN);
        }

        if (app_labels[slot_index] && app->name) {
            lv_label_set_text(app_labels[slot_index], app->name);
        }

        if (app_icon_bgs[slot_index]) {
            lv_obj_set_style_bg_color(app_icon_bgs[slot_index],
                                      lv_color_hex(0x495060), LV_PART_MAIN);
        }
    } else {
        if (app_icons[slot_index]) {
            if (item->folder.icon) {
                lv_image_set_src(app_icons[slot_index], item->folder.icon);
                lv_obj_remove_flag(app_icons[slot_index], LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(app_icons[slot_index], LV_OBJ_FLAG_HIDDEN);
            }
            lv_obj_set_style_image_recolor_opa(app_icons[slot_index], LV_OPA_70, LV_PART_MAIN);
            lv_obj_set_style_image_recolor(app_icons[slot_index], item->folder.color, LV_PART_MAIN);
        }

        if (app_labels[slot_index] && item->folder.name) {
            lv_label_set_text(app_labels[slot_index], item->folder.name);
        }

        if (app_icon_bgs[slot_index]) {
            lv_obj_set_style_bg_color(app_icon_bgs[slot_index],
                                      item->folder.color, LV_PART_MAIN);
        }
    }
}

static void populate_page(int page_index)
{
    int start_index = page_index * ITEMS_PER_PAGE;

    for (int i = 0; i < NUM_SLOTS; i++) {
        int item_index = start_index + i;
        picker_item_t *item = NULL;

        if (item_index < num_picker_items) {
            item = &picker_items[item_index];
        }

        populate_slot(i, item);
    }

    current_page = page_index;
    lv_subject_set_int(&app_picker_page_index, current_page);

    update_nav_arrows();
}

static void update_nav_arrows(void)
{
    if (nav_left) {
        lv_obj_remove_flag(nav_left, LV_OBJ_FLAG_HIDDEN);
        if (current_page > 0) {
            lv_obj_set_style_opa(nav_left, LV_OPA_100, LV_PART_MAIN);
            lv_obj_add_flag(nav_left, LV_OBJ_FLAG_CLICKABLE);
        } else {
            lv_obj_set_style_opa(nav_left, LV_OPA_30, LV_PART_MAIN);
            lv_obj_remove_flag(nav_left, LV_OBJ_FLAG_CLICKABLE);
        }
    }

    if (nav_right) {
        lv_obj_remove_flag(nav_right, LV_OBJ_FLAG_HIDDEN);
        if (current_page < total_pages - 1) {
            lv_obj_set_style_opa(nav_right, LV_OPA_100, LV_PART_MAIN);
            lv_obj_add_flag(nav_right, LV_OBJ_FLAG_CLICKABLE);
        } else {
            lv_obj_set_style_opa(nav_right, LV_OPA_30, LV_PART_MAIN);
            lv_obj_remove_flag(nav_right, LV_OBJ_FLAG_CLICKABLE);
        }
    }
}

static void folder_app_clicked_cb(lv_event_t *e)
{
    application_t *clicked_app = (application_t *)lv_obj_get_user_data(lv_event_get_target(e));
    if (clicked_app && app_selected_cb) {
        LOG_DBG("Folder app clicked: %s", clicked_app->name);
        app_selected_cb(clicked_app);
    }
}

static void show_folder_contents(zsw_app_category_t category)
{
    if (folder_overlay == NULL || folder_grid == NULL) {
        LOG_ERR("Folder overlay not available");
        return;
    }

    open_folder = category;

    if (folder_title_label) {
        lv_label_set_text(folder_title_label, folder_info[category].name);
    }

    lv_subject_set_int(&app_picker_folder_open, 1);

    /* Hide nav arrows when folder is open */
    if (nav_left) {
        lv_obj_add_flag(nav_left, LV_OBJ_FLAG_HIDDEN);
    }
    if (nav_right) {
        lv_obj_add_flag(nav_right, LV_OBJ_FLAG_HIDDEN);
    }

    /* Remove any existing children in folder grid */
    lv_obj_clean(folder_grid);

    int num_apps = zsw_app_manager_get_num_apps();

    for (int i = 0; i < num_apps; i++) {
        application_t *app = zsw_app_manager_get_app(i);
        if (app && !app->hidden && app->category == category) {
            lv_obj_t *app_btn = lv_obj_create(folder_grid);
            lv_obj_set_size(app_btn, 55, 70);
            lv_obj_set_style_bg_opa(app_btn, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_border_width(app_btn, 0, LV_PART_MAIN);
            lv_obj_set_style_pad_all(app_btn, 0, LV_PART_MAIN);
            lv_obj_remove_flag(app_btn, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_add_flag(app_btn, LV_OBJ_FLAG_CLICKABLE);

            lv_obj_t *icon_bg = lv_obj_create(app_btn);
            lv_obj_set_size(icon_bg, 55, 55);
            lv_obj_align(icon_bg, LV_ALIGN_TOP_MID, 0, 0);
            lv_obj_set_style_radius(icon_bg, 28, LV_PART_MAIN);
            lv_obj_set_style_bg_color(icon_bg, lv_color_hex(0x495060), LV_PART_MAIN);
            lv_obj_set_style_border_width(icon_bg, 0, LV_PART_MAIN);
            lv_obj_remove_flag(icon_bg, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_remove_flag(icon_bg, LV_OBJ_FLAG_CLICKABLE);

            if (app->icon) {
                lv_obj_t *icon = lv_image_create(icon_bg);
                lv_image_set_src(icon, app->icon);
                lv_obj_center(icon);
            }

            lv_obj_t *label = lv_label_create(app_btn);
            lv_label_set_text(label, app->name);
            lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);
            lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
            lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
            lv_obj_set_width(label, 60);

            lv_obj_set_user_data(app_btn, app);
            lv_obj_add_event_cb(app_btn, folder_app_clicked_cb, LV_EVENT_SHORT_CLICKED, NULL);
        }
    }

    lv_obj_remove_flag(folder_overlay, LV_OBJ_FLAG_HIDDEN);

    LOG_DBG("Showing folder: %s", folder_info[category].name);
}

void app_picker_on_app_clicked(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    int slot_index = (int)(intptr_t)lv_obj_get_user_data(target);

    if (slot_index < 0 || slot_index >= NUM_SLOTS || slot_items[slot_index] == NULL) {
        LOG_WRN("Click on invalid/empty slot %d", slot_index);
        return;
    }

    picker_item_t *item = slot_items[slot_index];

    if (item->type == PICKER_ITEM_APP) {
        LOG_DBG("App clicked: %s (slot %d)", item->app->name, slot_index);

        if (app_selected_cb) {
            app_selected_cb(item->app);
        }
    } else {
        LOG_DBG("Folder clicked: %s (slot %d)", item->folder.name, slot_index);
        show_folder_contents(item->folder.category);
    }
}

void app_picker_on_folder_close_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    LOG_DBG("Folder close clicked");

    if (folder_overlay) {
        lv_obj_add_flag(folder_overlay, LV_OBJ_FLAG_HIDDEN);
    }

    open_folder = ZSW_APP_CATEGORY_INVALID;
    lv_subject_set_int(&app_picker_folder_open, 0);

    update_nav_arrows();
}

void app_picker_on_nav_left_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    LOG_INF("Nav LEFT clicked - page=%d/%d", current_page, total_pages);
    prev_page();
}

void app_picker_on_nav_right_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    LOG_INF("Nav RIGHT clicked - page=%d/%d", current_page, total_pages);
    next_page();
}

lv_obj_t *app_picker_ui_create(lv_obj_t *root, lv_group_t *group,
                               app_picker_on_app_selected_cb on_app_selected,
                               const zsw_app_folder_info_t *folders)
{
    app_selected_cb = on_app_selected;
    folder_info = folders;
    open_folder = ZSW_APP_CATEGORY_INVALID;

    build_picker_items();
    total_pages = calculate_page_count();

    memset(slot_items, 0, sizeof(slot_items));

    picker_root = app_picker_create(root);

    if (picker_root == NULL) {
        LOG_ERR("Failed to create app picker UI");
        return NULL;
    }

    cache_object_references();

    lv_obj_add_event_cb(root, on_swipe_event, LV_EVENT_GESTURE, NULL);

    if (nav_left) {
        lv_obj_move_foreground(nav_left);
    }
    if (nav_right) {
        lv_obj_move_foreground(nav_right);
    }

    /* Restore last page, clamped to valid range */
    int restore_page = last_page;
    if (restore_page >= total_pages) {
        restore_page = total_pages > 0 ? total_pages - 1 : 0;
    }

    populate_page(restore_page);

    /* Restore last folder if it was open */
    if (last_folder != ZSW_APP_CATEGORY_INVALID && folder_has_apps(last_folder)) {
        show_folder_contents(last_folder);
    }

    LOG_DBG("App picker created: %d items, %d pages, restored page=%d folder=%d",
            num_picker_items, total_pages, restore_page, last_folder);

    return picker_root;
}

void app_picker_ui_delete(void)
{
    /* Save state before deleting */
    last_page = current_page;
    last_folder = open_folder;

    if (picker_root) {
        lv_obj_del(picker_root);
        picker_root = NULL;
    }

    app_container = NULL;
    memset(app_slots, 0, sizeof(app_slots));
    memset(app_icons, 0, sizeof(app_icons));
    memset(app_labels, 0, sizeof(app_labels));
    memset(app_icon_bgs, 0, sizeof(app_icon_bgs));
    memset(slot_items, 0, sizeof(slot_items));
    nav_left = NULL;
    nav_right = NULL;
    folder_overlay = NULL;
    folder_grid = NULL;
    folder_title_label = NULL;
    folder_close_btn = NULL;
    folder_close_icon = NULL;

    num_picker_items = 0;
    open_folder = ZSW_APP_CATEGORY_INVALID;

    app_selected_cb = NULL;
}

bool app_picker_ui_is_folder_open(void)
{
    return open_folder != ZSW_APP_CATEGORY_INVALID;
}

void app_picker_ui_close_folder(void)
{
    if (open_folder != ZSW_APP_CATEGORY_INVALID) {
        LOG_DBG("Closing folder via back button");

        if (folder_overlay) {
            lv_obj_add_flag(folder_overlay, LV_OBJ_FLAG_HIDDEN);
        }

        open_folder = ZSW_APP_CATEGORY_INVALID;
        last_folder = ZSW_APP_CATEGORY_INVALID;
        lv_subject_set_int(&app_picker_folder_open, 0);

        update_nav_arrows();
    }
}
