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
#include <zephyr/zbus/zbus.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "ui/zsw_ui.h"
#include "managers/zsw_app_manager.h"
#include "events/activity_event.h"

LOG_MODULE_REGISTER(app_manager, LOG_LEVEL_INF);

#define MAX_APPS        25
#define INVALID_APP_ID  0xFF

typedef struct {
    const char *name;
    const void *icon;
    zsw_app_category_t category;
    lv_color_t folder_color;
} app_folder_t;

static void draw_app_and_folder_view(void);
static void draw_apps_in_folder(zsw_app_category_t category);
static void app_clicked(lv_event_t *e);
static void folder_clicked(lv_event_t *e);
static void folder_back_clicked(lv_event_t *e);
static void async_app_start(lv_timer_t *timer);
static void async_app_close(lv_timer_t *timer);
static void transition_app_to_ui_hidden(application_t *app);
static void transition_app_to_ui_visible(application_t *app);
static void zbus_activity_event_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(activity_state_data_chan);
ZBUS_LISTENER_DEFINE(app_manager_activity_state_event_lis, zbus_activity_event_callback);

ZSW_LV_IMG_DECLARE(close_icon);
ZSW_LV_IMG_DECLARE(folder_icon);

static application_t *apps[MAX_APPS];
static uint8_t num_apps;
static uint8_t num_visible_apps;
static uint8_t current_app;
static lv_obj_t *root_obj;
static lv_group_t *group_obj;
static on_app_manager_cb_fn close_cb_func;
static lv_obj_t *grid;
static uint8_t last_index; // Last focused position in root view
static bool app_launch_only;
static bool is_deleting_app_picker;
static lv_timer_t *async_app_start_timer;
static lv_timer_t *async_app_close_timer;
static zsw_app_category_t current_folder = ZSW_APP_CATEGORY_INVALID;
static bool screen_is_on = true;

// TODO: Add icons for app folders
static const app_folder_t app_folders[ZSW_APP_CATEGORY_COUNT] = {
    [ZSW_APP_CATEGORY_ROOT] = {
        .name = "Root",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .folder_color = LV_COLOR_MAKE(0x9E, 0x9E, 0x9E),
        .category = ZSW_APP_CATEGORY_ROOT
    },
    [ZSW_APP_CATEGORY_TOOLS] = {
        .name = "Tools",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .folder_color = LV_COLOR_MAKE(0xFF, 0x98, 0x00),
        .category = ZSW_APP_CATEGORY_TOOLS
    },
    [ZSW_APP_CATEGORY_FITNESS] = {
        .name = "Fitness",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .folder_color = LV_COLOR_MAKE(0x4C, 0xAF, 0x50),
        .category = ZSW_APP_CATEGORY_FITNESS
    },
    [ZSW_APP_CATEGORY_SYSTEM] = {
        .name = "System",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .folder_color = LV_COLOR_MAKE(0xF4, 0x43, 0x36),
        .category = ZSW_APP_CATEGORY_SYSTEM
    },
    [ZSW_APP_CATEGORY_RANDOM] = {
        .name = "Random",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .folder_color = LV_COLOR_MAKE(0xE9, 0x1E, 0x63),
        .category = ZSW_APP_CATEGORY_RANDOM
    },
    [ZSW_APP_CATEGORY_SENSORS] = {
        .name = "Sensors",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .folder_color = LV_COLOR_MAKE(0x00, 0xBC, 0xD4),
        .category = ZSW_APP_CATEGORY_SENSORS
    },
    [ZSW_APP_CATEGORY_GAMES] = {
        .name = "Games",
        .icon = ZSW_LV_IMG_USE(folder_icon),
        .folder_color = LV_COLOR_MAKE(0x21, 0x96, 0xF3),
        .category = ZSW_APP_CATEGORY_GAMES
    },
};

static void delete_root_object(void)
{
    if (grid != NULL) {
        // When deleting the grid, we get callbacks for each row being deleted.
        // Because LVGL will refocus one a new row when one is deleted.
        // This causes the wrong last opened app index to be changed.
        is_deleting_app_picker = true;
        lv_obj_del(grid);
        grid = NULL;
        is_deleting_app_picker = false;
    }
}

static void row_focused(lv_event_t *e)
{
    lv_obj_t *row = lv_event_get_target(e);
    if (row && lv_obj_get_child_cnt(row) > 0 && !is_deleting_app_picker) {
        // Track focus position in root view only
        if (current_folder == ZSW_APP_CATEGORY_INVALID) {
            // Find the index of this row in the grid
            lv_obj_t *parent = lv_obj_get_parent(row);
            if (parent) {
                for (int i = 0; i < lv_obj_get_child_cnt(parent); i++) {
                    if (lv_obj_get_child(parent, i) == row) {
                        last_index = i;
                        break;
                    }
                }
            }
        }

        lv_obj_t *title_label = lv_obj_get_user_data(row);
        if (title_label) {
            lv_obj_set_style_text_color(title_label, lv_color_white(), LV_PART_MAIN);
        }
    }
}

static void row_unfocused(lv_event_t *e)
{
    lv_obj_t *row = lv_event_get_target(e);
    // Need to check child count as during delete we get this callback called, but without children.
    // The image is deleted already.
    if (row && lv_obj_get_child_cnt(row) > 0) {
        lv_obj_t *title_label = lv_obj_get_user_data(row);
        if (title_label) {
            lv_obj_set_style_text_color(title_label, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
        }
    }
}

static void folder_clicked(lv_event_t *e)
{
    zsw_app_category_t category = (zsw_app_category_t)(intptr_t)lv_event_get_user_data(e);
    current_folder = category;
    delete_root_object();
    draw_apps_in_folder(category);
}

static void folder_back_clicked(lv_event_t *e)
{
    current_folder = ZSW_APP_CATEGORY_INVALID;
    delete_root_object();
    draw_app_and_folder_view();
}

static void app_clicked(lv_event_t *e)
{
    int app_id = (int)(intptr_t)lv_event_get_user_data(e);
    current_app = app_id;

    // This function may be called within a lvgl callback such
    // as a button click. If we create a new ui in this callback
    // which registers a button press callback then that callback
    // may get called, but we don't want that. So delay the opening
    // of the new application some time.
    if (async_app_start_timer == NULL) {
        async_app_start_timer = lv_timer_create(async_app_start, 500,  NULL);
        lv_timer_set_repeat_count(async_app_start_timer, 1);
    }
}

static void async_app_start(lv_timer_t *timer)
{
    async_app_start_timer = NULL;
    LOG_DBG("Start %d", current_app);
    delete_root_object();

    application_t *app = apps[current_app];
    __ASSERT(screen_is_on, "Screen expected to be on when starting app.");
    app->current_state = ZSW_APP_STATE_UI_VISIBLE;

    app->start_func(root_obj, group_obj);
}

static void async_app_close(lv_timer_t *timer)
{
    if (current_app < num_apps) {
        LOG_DBG("Stop %d", current_app);
        bool back_button_consumed = false;
        if (apps[current_app]->back_func) {
            back_button_consumed = apps[current_app]->back_func();
        }

        if (!back_button_consumed) {
            apps[current_app]->current_state = ZSW_APP_STATE_STOPPED;
            apps[current_app]->stop_func();
            current_app = INVALID_APP_ID;
            if (app_launch_only) {
                zsw_app_manager_delete();
                close_cb_func();
            } else {
                if (current_folder != ZSW_APP_CATEGORY_INVALID) {
                    draw_apps_in_folder(current_folder);
                } else {
                    draw_app_and_folder_view();
                }
            }
        }
    } else {
        // No app running, check if we are in a folder
        if (current_folder != ZSW_APP_CATEGORY_INVALID) {
            LOG_DBG("Exit folder %d", current_folder);
            current_folder = ZSW_APP_CATEGORY_INVALID;
            delete_root_object();
            draw_app_and_folder_view();
        } else {
            LOG_DBG("Exit application manager");
            zsw_app_manager_delete();
            close_cb_func();
        }
    }
    async_app_close_timer = NULL;
}

static void transition_app_to_ui_hidden(application_t *app)
{
    if (app && app->current_state == ZSW_APP_STATE_UI_VISIBLE) {
        app->current_state = ZSW_APP_STATE_UI_HIDDEN;
        LOG_DBG("App '%s' UI now hidden", app->name);

        if (app->ui_unavailable_func) {
            app->ui_unavailable_func();
        }
    }
}

static void transition_app_to_ui_visible(application_t *app)
{
    if (app && app->current_state == ZSW_APP_STATE_UI_HIDDEN) {
        app->current_state = ZSW_APP_STATE_UI_VISIBLE;
        LOG_DBG("App '%s' UI now visible", app->name);

        if (app->ui_available_func) {
            app->ui_available_func();
        }
    }
}

static void zbus_activity_event_callback(const struct zbus_channel *chan)
{
    const struct activity_state_event *event = zbus_chan_const_msg(chan);
    bool new_screen_state = (event->state == ZSW_ACTIVITY_STATE_ACTIVE);

    if (screen_is_on == new_screen_state) {
        return; // No change
    }

    screen_is_on = new_screen_state;

    // Find currently running app and transition it
    if (current_app < num_apps) {
        application_t *running_app = apps[current_app];

        if (screen_is_on) {
            transition_app_to_ui_visible(running_app);
        } else {
            transition_app_to_ui_hidden(running_app);
        }
    }
}

static void async_app_manager_close(lv_timer_t *timer)
{
    LOG_DBG("Close app manager");
    close_cb_func();
}

static void app_manager_close_button_pressed(lv_event_t *e)
{
    lv_timer_t *timer = lv_timer_create(async_app_manager_close, 500,  NULL);
    lv_timer_set_repeat_count(timer, 1);
    // Next time we open focus on first app and not close button.
    last_index = 0;
}

static void scroll_event_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_target(e);
    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;
    lv_coord_t r = lv_obj_get_height(cont) * 5 / 9;

    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for (i = 0; i < child_cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /* Get the x of diff_y on a circle. */
        lv_coord_t x;
        /* If diff_y is out of the circle use the last point of the circle (the radius) */
        if (diff_y >= r) {
            x = r;
        } else {
            /* Use Pythagoras theorem to get x from radius and y */
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /* Use lvgl's built in sqrt root function */
            x = r - res.i - 20; /* Added - 20 here to pull all a bit more to the left side */
        }

        /* Translate the item by the calculated X coordinate */
        lv_obj_set_style_translate_x(child, x, 0);
        lv_obj_set_style_translate_y(child, -13, 0);

        /* Uncomment if to use some opacity with larger translations */
        //lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        //lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

static lv_obj_t *create_grid_container(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_flex_flow(&style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_START);
    lv_style_set_layout(&style, LV_LAYOUT_FLEX);

    lv_style_set_bg_opa(&style, LV_OPA_TRANSP);
    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

    grid = lv_obj_create(root_obj);
    lv_obj_add_style(grid, &style, 0);
    lv_obj_set_scrollbar_mode(root_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_side(grid, LV_BORDER_SIDE_NONE, 0);
    lv_obj_set_style_pad_row(grid, 2, 0);

    lv_obj_set_size(grid, LV_PCT(100), LV_PCT(100));
    lv_obj_center(grid);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(grid, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(grid, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(grid, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(grid, scroll_event_cb, LV_EVENT_SCROLL, NULL);

    return grid;
}

static lv_obj_t *create_application_list_entry(lv_obj_t *grid, const void *icon, const char *name, int app_id,
                                               const lv_color_t *folder_color)
{
    lv_obj_t *cont = lv_obj_create(grid);
    lv_obj_center(cont);
    lv_obj_set_style_border_side(cont, LV_BORDER_SIDE_NONE, 0);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_image_header_t header;
    lv_image_decoder_get_info(icon, &header);

    lv_obj_set_size(cont, LV_PCT(100), header.h + 6);
    lv_obj_clear_flag(cont,
                      LV_OBJ_FLAG_SCROLLABLE); // Needed, otherwise indev will first focus on this cont before it's contents.

    lv_obj_add_event_cb(cont, row_focused, LV_EVENT_FOCUSED, (void *)(intptr_t)app_id);
    lv_obj_add_event_cb(cont, row_unfocused, LV_EVENT_DEFOCUSED, (void *)(intptr_t)app_id);
    lv_group_add_obj(group_obj, cont);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_t *img_icon = lv_img_create(cont);
    lv_img_set_src(img_icon, icon);
    lv_obj_set_size(img_icon, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(img_icon, LV_ALIGN_LEFT_MID, 0, 0);

    if (folder_color) {
        lv_obj_set_style_image_recolor_opa(img_icon, LV_OPA_70, LV_PART_MAIN);
        lv_obj_set_style_image_recolor(img_icon, *folder_color, LV_PART_MAIN);
    }
    lv_obj_t *title = lv_label_create(cont);
    lv_label_set_text(title, name);
    lv_obj_set_size(title, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align_to(title, img_icon, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);
    lv_obj_set_style_text_align(title, LV_ALIGN_OUT_LEFT_MID, LV_PART_MAIN);

    lv_obj_set_user_data(cont, title);

    return cont;
}

static void draw_app_and_folder_view(void)
{
    lv_obj_t *entry;

    create_grid_container();

    int entry_index = 0;

    // Add apps without category/folder
    for (int i = 0; i < num_apps; i++) {
        if (!apps[i]->hidden && apps[i]->category == ZSW_APP_CATEGORY_ROOT) {
            entry = create_application_list_entry(grid, apps[i]->icon, apps[i]->name, i, NULL);
            lv_obj_add_event_cb(entry, app_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)i);
            entry_index++;
        }
    }

    // Add folders
    for (int i = 1; i < ZSW_APP_CATEGORY_COUNT; i++) {
        bool has_apps = false;
        for (int j = 0; j < num_apps; j++) {
            if (!apps[j]->hidden && apps[j]->category == i) {
                has_apps = true;
                break;
            }
        }

        if (has_apps) {
            entry = create_application_list_entry(grid, app_folders[i].icon, app_folders[i].name, -1, &app_folders[i].folder_color);
            lv_obj_add_event_cb(entry, folder_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)i);
            entry_index++;
        }
    }

    entry = create_application_list_entry(grid, ZSW_LV_IMG_USE(close_icon), "Close", num_visible_apps, NULL);
    lv_obj_add_event_cb(entry, app_manager_close_button_pressed, LV_EVENT_CLICKED, NULL);

    // Focus on the last focused entry
    int focus_index = last_index;
    if (focus_index >= lv_obj_get_child_cnt(grid)) {
        focus_index = 0;
    }

    lv_group_focus_obj(lv_obj_get_child(grid, focus_index));

    // Update the notifications position manually first time
    lv_obj_send_event(grid, LV_EVENT_SCROLL, NULL);

    // Be sure the focused notification is in the middle
    lv_obj_scroll_to_view(lv_obj_get_child(grid, focus_index), LV_ANIM_OFF);
}

static void draw_apps_in_folder(zsw_app_category_t category)
{
    lv_obj_t *entry;

    create_grid_container();

    // Add apps from this category
    for (int i = 0; i < num_apps; i++) {
        if (!apps[i]->hidden && apps[i]->category == category) {
            entry = create_application_list_entry(grid, apps[i]->icon, apps[i]->name, i, NULL);
            lv_obj_add_event_cb(entry, app_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        }
    }

    // Add back button
    entry = create_application_list_entry(grid, ZSW_LV_IMG_USE(close_icon), "Back", -1, NULL);
    lv_obj_add_event_cb(entry, folder_back_clicked, LV_EVENT_CLICKED, NULL);

    lv_group_focus_obj(lv_obj_get_child(grid, 0));

    // Update the notifications position manually first time
    lv_obj_send_event(grid, LV_EVENT_SCROLL, NULL);

    // Be sure the first notification is in the middle
    lv_obj_scroll_to_view(lv_obj_get_child(grid, 0), LV_ANIM_OFF);
}

int zsw_app_manager_show(on_app_manager_cb_fn close_cb, lv_obj_t *root, lv_group_t *group, char *app_name)
{
    int err = 0;
    bool app_found;
    close_cb_func = close_cb;
    root_obj = root;
    group_obj = group;
    app_launch_only = false;

    if (app_name == NULL) {
        draw_app_and_folder_view();
    } else {
        app_found = false;
        for (int i = 0; i < num_apps; i++) {
            if (strcmp(apps[i]->name, app_name) == 0) {
                app_launch_only = true;
                current_app = i;
                if (async_app_start_timer == NULL) {
                    async_app_start_timer = lv_timer_create(async_app_start, 1,  NULL);
                    lv_timer_set_repeat_count(async_app_start_timer, 1);
                }
            }
        }
    }

    if (app_name != NULL && !app_found) {
        err = -ENOENT;
    }

    return err;
}

void zsw_app_manager_delete(void)
{
    if (current_app < num_apps) {
        LOG_DBG("Stop force %d", current_app);
        apps[current_app]->current_state = ZSW_APP_STATE_STOPPED;
        apps[current_app]->stop_func();
    }
    delete_root_object();
}

void zsw_app_manager_add_application(application_t *app)
{
    __ASSERT_NO_MSG(num_apps < MAX_APPS);

    app->current_state = ZSW_APP_STATE_STOPPED;
    apps[num_apps] = app;
    num_apps++;
    if (!app->hidden) {
        app->private_list_index = num_visible_apps;
        num_visible_apps++;
    }
}

void zsw_app_manager_exit_app(void)
{
    if (async_app_close_timer != NULL) {
        return;
    }
    async_app_close_timer = lv_timer_create(async_app_close, 500,  NULL);
    lv_timer_set_repeat_count(async_app_close_timer, 1);
}

void zsw_app_manager_app_close_request(application_t *app)
{
    LOG_DBG("zsw_app_manager_app_close_request");
    zsw_app_manager_exit_app();
}

int zsw_app_manager_get_num_apps(void)
{
    return num_apps;
}

zsw_app_state_t zsw_app_manager_get_app_state(application_t *app)
{
    __ASSERT_NO_MSG(app != NULL);
    return app->current_state;
}

static int application_manager_init(void)
{
    memset(apps, 0, sizeof(apps));
    num_apps = 0;
    current_app = INVALID_APP_ID;
    async_app_start_timer = NULL;
    current_folder = ZSW_APP_CATEGORY_INVALID;
    last_index = 0;
    screen_is_on = true;

    // Subscribe to activity events to track screen state
    zbus_chan_add_obs(&activity_state_data_chan, &app_manager_activity_state_event_lis, K_MSEC(100));

    return 0;
}

SYS_INIT(application_manager_init, POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY);
