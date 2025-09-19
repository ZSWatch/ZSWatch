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
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <lvgl.h>
#include "button_test_screen.h"
#include "../production_test_runner.h"

LOG_MODULE_REGISTER(button_test_screen, LOG_LEVEL_INF);

#define NUM_BUTTONS 4

static lv_obj_t *button_screen;
static lv_obj_t *status_label;
static lv_obj_t *instruction_label;
static lv_obj_t *button_indicators[NUM_BUTTONS];

// Button layout positions matching physical placement
static const lv_point_t button_positions[NUM_BUTTONS] = {
    {188, 52},  // Button 1 (INPUT_KEY_1) - top right
    {52, 188},  // Button 2 (INPUT_KEY_2) - bottom left
    {188, 188}, // Button 3 (INPUT_KEY_3/INPUT_KEY_KP0) - bottom right
    {52, 52}    // Button 4 (INPUT_KEY_4) - top left
};

static bool button_states[NUM_BUTTONS] = {false, false, false, false};

void button_test_screen_init(void)
{
    LOG_DBG("Initializing button test screen");

    // Create button test screen
    button_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(button_screen, lv_color_hex(0x000000), LV_PART_MAIN);

#ifdef CONFIG_BOARD_NATIVE_SIM
    // Add circle border for native sim
    lv_obj_t *circle_border = lv_obj_create(button_screen);
    lv_obj_set_size(circle_border, 240, 240);
    lv_obj_center(circle_border);
    lv_obj_set_style_bg_opa(circle_border, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(circle_border, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(circle_border, lv_color_hex(0xFF6600), LV_PART_MAIN);
    lv_obj_set_style_radius(circle_border, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(circle_border, LV_OPA_80, LV_PART_MAIN);
#endif

    // Create button indicators
    for (int i = 0; i < NUM_BUTTONS; i++) {
        // Button indicator circle
        button_indicators[i] = lv_obj_create(button_screen);
        lv_obj_set_size(button_indicators[i], 40, 40);
        lv_obj_set_pos(button_indicators[i],
                       button_positions[i].x - 20,
                       button_positions[i].y - 20);

        // Initial style - gray circle
        lv_obj_set_style_bg_color(button_indicators[i], lv_color_hex(0x444444), LV_PART_MAIN);
        lv_obj_set_style_border_width(button_indicators[i], 2, LV_PART_MAIN);
        lv_obj_set_style_border_color(button_indicators[i], lv_color_hex(0x888888), LV_PART_MAIN);
        lv_obj_set_style_radius(button_indicators[i], LV_RADIUS_CIRCLE, LV_PART_MAIN);

        lv_obj_clear_flag(button_indicators[i], LV_OBJ_FLAG_SCROLLABLE);
    }

    status_label = lv_label_create(button_screen);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x00AAFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_label_set_text(status_label, "");

    instruction_label = lv_label_create(button_screen);
    lv_label_set_text(instruction_label, "Press ANY button\nto start test");
    lv_obj_set_style_text_color(instruction_label, lv_color_hex(0x00AAFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(instruction_label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_align(instruction_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(instruction_label, LV_ALIGN_CENTER, 0, 0);
}

void button_test_screen_show(void)
{
    lv_scr_load(button_screen);
}

void button_test_screen_start_test(void)
{
    lv_label_set_text(instruction_label, "PRESS ALL BUTTONS");
    lv_obj_set_style_text_color(status_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
}

void button_test_screen_update_button(uint32_t button_code)
{
    int button_index = -1;

    // Map button code to array index (matching physical layout)
    switch (button_code) {
        case INPUT_KEY_1:
            button_index = 0; // Top right
            break;
        case INPUT_KEY_2:
            button_index = 1; // Bottom left
            break;
        case INPUT_KEY_3:
        case INPUT_KEY_KP0:
            button_index = 2; // Bottom right (PMIC button or longpress conversion)
            break;
        case INPUT_KEY_4:
            button_index = 3; // Top left
            break;
        default:
            LOG_WRN("Unknown button code: %d", button_code);
            return;
    }

    if (button_index >= 0 && button_index < NUM_BUTTONS && !button_states[button_index]) {
        button_states[button_index] = true;

        // Set virtual button green
        lv_obj_set_style_bg_color(button_indicators[button_index], lv_color_hex(0x00AA00), LV_PART_MAIN);
        lv_obj_set_style_border_color(button_indicators[button_index], lv_color_hex(0x00FF00), LV_PART_MAIN);

        // Count pressed buttons
        int pressed_count = 0;
        for (int i = 0; i < NUM_BUTTONS; i++) {
            if (button_states[i]) {
                pressed_count++;
            }
        }

        // Update status
        static char status_text[32];
        snprintf(status_text, sizeof(status_text), "(%d/%d) %ds", pressed_count, NUM_BUTTONS,
                 production_test_runner_get_context()->countdown_seconds);
        lv_label_set_text(status_label, status_text);
    }
}

void button_test_screen_update_countdown(int seconds_remaining)
{
    int pressed_count = 0;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (button_states[i]) {
            pressed_count++;
        }
    }

    static char status_text[32];
    snprintf(status_text, sizeof(status_text), "(%d/%d) %ds", pressed_count, NUM_BUTTONS, seconds_remaining);
    lv_label_set_text(status_label, status_text);
}
