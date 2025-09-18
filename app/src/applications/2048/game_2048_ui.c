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

#include <2048/game_2048_ui.h>
#include <lvgl.h>
#include <lv_100ask_2048.h>

static void game_2048_event_cb(lv_event_t *e);

static lv_obj_t *root_page = NULL;

void game_2048_ui_show(lv_obj_t *root)
{
    assert(root_page == NULL);
    lv_obj_t *obj_2048;

    // Create the root container
    root_page = lv_obj_create(root);
    // Remove the default border
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    // Make root container fill the screen
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    // Don't want it to be scollable. Putting anything close the edges
    // then LVGL automatically makes the page scrollable and shows a scroll bar.
    // Does not look very good on the round display.
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_style_bg_color(root_page, lv_color_hex(0xb3a397), LV_PART_MAIN); // Match 2048 game background color

    obj_2048 = lv_100ask_2048_create(root_page);
    lv_obj_set_size(obj_2048, 200, 200);
    lv_obj_center(obj_2048);
    lv_obj_set_style_outline_width(obj_2048, 0, LV_STATE_FOCUS_KEY | LV_STATE_DEFAULT);

    // Score
    lv_obj_t *label = lv_label_create(root_page);
    lv_label_set_recolor(label, true);
    lv_label_set_text_fmt(label, "#ff00ff %d #", lv_100ask_2048_get_score(obj_2048));
    lv_obj_align_to(label, obj_2048, LV_ALIGN_OUT_TOP_MID, -20, 0);
    lv_obj_add_event_cb(obj_2048, game_2048_event_cb, LV_EVENT_ALL, label);
}

void game_2048_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}

static void game_2048_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj_2048 = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        if (lv_100ask_2048_get_best_tile(obj_2048) >= 2048) {
            lv_label_set_text(label, "#00b329 YOU WIN! #");
        } else if (lv_100ask_2048_get_status(obj_2048)) {
            lv_label_set_text(label, "#ff0000 GAME OVER! #");
        } else {
            lv_label_set_text_fmt(label, "#ff00ff %d #", lv_100ask_2048_get_score(obj_2048));
        }
    }
}
