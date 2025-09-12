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
#include <lvgl.h>
#include "touch_test_screen.h"
#include "../production_test_runner.h"

LOG_MODULE_REGISTER(touch_test_screen, LOG_LEVEL_INF);

static lv_obj_t *touch_screen;
static lv_obj_t *title_label;
static lv_obj_t *instruction_label;
static lv_obj_t *timeout_label;
static lv_obj_t *touch_area;

static void touch_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        LOG_DBG("Touch detected on screen");

        lv_obj_set_style_bg_color(touch_area, lv_color_hex(0x00AA00), LV_PART_MAIN);
        lv_label_set_text(instruction_label, "Touched!");
        lv_obj_set_style_text_color(instruction_label, lv_color_hex(0x00AA00), LV_PART_MAIN);

        production_test_runner_test_passed();
    }
}

void touch_test_screen_init(void)
{
    LOG_DBG("Initializing touch test screen");

    // Create touch test screen
    touch_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(touch_screen, lv_color_hex(0x000000), LV_PART_MAIN);

#ifdef CONFIG_BOARD_NATIVE_SIM
    // Add circle border for native sim
    lv_obj_t *circle_border = lv_obj_create(touch_screen);
    lv_obj_set_size(circle_border, 240, 240);
    lv_obj_center(circle_border);
    lv_obj_set_style_bg_opa(circle_border, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(circle_border, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(circle_border, lv_color_hex(0xFF6600), LV_PART_MAIN);
    lv_obj_set_style_radius(circle_border, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(circle_border, LV_OPA_80, LV_PART_MAIN);
#endif

    title_label = lv_label_create(touch_screen);
    lv_label_set_text(title_label, "TOUCH SCREEN TEST");
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 50);

    touch_area = lv_obj_create(touch_screen);
    lv_obj_set_size(touch_area, 160, 70);
    lv_obj_align(touch_area, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_bg_color(touch_area, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_border_width(touch_area, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(touch_area, lv_color_hex(0x00AAFF), LV_PART_MAIN);
    lv_obj_set_style_radius(touch_area, 10, LV_PART_MAIN);
    lv_obj_add_flag(touch_area, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(touch_area, touch_event_handler, LV_EVENT_CLICKED, NULL);

    instruction_label = lv_label_create(touch_area);
    lv_label_set_text(instruction_label, "PRESS HERE");
    lv_obj_set_style_text_color(instruction_label, lv_color_hex(0x00AAFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(instruction_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_align(instruction_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_center(instruction_label);

    timeout_label = lv_label_create(touch_screen);
    lv_label_set_text(timeout_label, "");
    lv_obj_set_style_text_color(timeout_label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_font(timeout_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(timeout_label, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void touch_test_screen_show(void)
{
    lv_scr_load(touch_screen);
}

void touch_test_screen_update_countdown(int seconds_remaining)
{
    static char timeout_text[32];
    snprintf(timeout_text, sizeof(timeout_text), "%ds remaining", seconds_remaining);
    lv_label_set_text(timeout_label, timeout_text);
}
