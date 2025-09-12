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
#include "microphone_test_screen.h"
#include "applications/mic/spectrum_analyzer.h"

LOG_MODULE_REGISTER(mic_test_screen, LOG_LEVEL_INF);

#define NUM_SPECTRUM_BARS       30

static lv_obj_t *mic_screen;
static lv_obj_t *title_label;
static lv_obj_t *instruction_label;
static lv_obj_t *spectrum_bars[NUM_SPECTRUM_BARS];
static lv_obj_t *level_label;
static lv_obj_t *timeout_label;

void microphone_test_screen_init(void)
{
    LOG_DBG("Initializing microphone test screen");

    // Create microphone test screen
    mic_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(mic_screen, lv_color_hex(0x000000), LV_PART_MAIN);

#ifdef CONFIG_BOARD_NATIVE_SIM
    // Add circle border for native sim to show round display bounds
    lv_obj_t *circle_border = lv_obj_create(mic_screen);
    lv_obj_set_size(circle_border, 240, 240);
    lv_obj_center(circle_border);
    lv_obj_set_style_bg_opa(circle_border, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(circle_border, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(circle_border, lv_color_hex(0xFF6600), LV_PART_MAIN);
    lv_obj_set_style_radius(circle_border, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(circle_border, LV_OPA_80, LV_PART_MAIN);
#endif

    title_label = lv_label_create(mic_screen);
    lv_label_set_text(title_label, "MICROPHONE TEST");
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 30);

    instruction_label = lv_label_create(mic_screen);
    lv_label_set_text(instruction_label, "Speak or make noise\nnear microphone");
    lv_obj_set_style_text_color(instruction_label, lv_color_hex(0x00AAFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(instruction_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_align(instruction_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(instruction_label, LV_ALIGN_CENTER, 0, -30);

    // Create spectrum bars (simplified FFT version of the mic_app)
    int bar_width = 4;
    int bar_spacing = 1;
    int total_width = (bar_width + bar_spacing) * NUM_SPECTRUM_BARS - bar_spacing;
    int start_x = (240 - total_width) / 2;

    for (int i = 0; i < NUM_SPECTRUM_BARS; i++) {
        spectrum_bars[i] = lv_bar_create(mic_screen);
        lv_obj_set_size(spectrum_bars[i], bar_width, 60);
        lv_obj_set_pos(spectrum_bars[i], start_x + i * (bar_width + bar_spacing), 90);
        lv_bar_set_range(spectrum_bars[i], 0, 255);
        lv_bar_set_value(spectrum_bars[i], 0, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(spectrum_bars[i], lv_color_hex(0x222222), LV_PART_MAIN);
        lv_obj_set_style_bg_color(spectrum_bars[i], lv_color_hex(0x00AAFF), LV_PART_INDICATOR);
        lv_obj_clear_flag(spectrum_bars[i], LV_OBJ_FLAG_SCROLLABLE);
    }

    level_label = lv_label_create(mic_screen);
    lv_label_set_text(level_label, "Audio: 0%");
    lv_obj_set_style_text_color(level_label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_font(level_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_align(level_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(level_label, LV_ALIGN_CENTER, 0, 40);

    timeout_label = lv_label_create(mic_screen);
    lv_label_set_text(timeout_label, "");
    lv_obj_set_style_text_color(timeout_label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_font(timeout_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(timeout_label, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void microphone_test_screen_show(void)
{
    lv_label_set_text(level_label, "Warming up...");
    lv_label_set_text(timeout_label, "");
    lv_obj_set_style_text_color(timeout_label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_color(level_label, lv_color_hex(0xFFAA00), LV_PART_MAIN);

    lv_scr_load(mic_screen);
}

void microphone_test_screen_update_countdown(int seconds_remaining)
{
    static char timeout_text[32];
    snprintf(timeout_text, sizeof(timeout_text), "%ds remaining", seconds_remaining);
    lv_label_set_text(timeout_label, timeout_text);
}

void microphone_test_screen_update_spectrum(const uint8_t *magnitudes, size_t count)
{
    // Update spectrum bars with FFT data
    int bars_to_update = count > NUM_SPECTRUM_BARS ? NUM_SPECTRUM_BARS : count;

    for (int i = 0; i < bars_to_update; i++) {
        lv_bar_set_value(spectrum_bars[i], magnitudes[i], LV_ANIM_OFF);
    }

    // Calculate overall audio activity level from spectrum
    int total_magnitude = 0;
    for (int i = 0; i < bars_to_update; i++) {
        total_magnitude += magnitudes[i];
    }

    int avg_magnitude = total_magnitude / bars_to_update;
    int activity_percent = (avg_magnitude * 100) / 255;

    static char status_text[32];
    snprintf(status_text, sizeof(status_text), "Activity: %d%%", activity_percent);
    lv_label_set_text(level_label, status_text);

    // Change text color based on activity
    if (activity_percent > 15) {
        lv_obj_set_style_text_color(level_label, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
    } else if (activity_percent > 5) {
        lv_obj_set_style_text_color(level_label, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN);
    } else {
        lv_obj_set_style_text_color(level_label, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    }
}
