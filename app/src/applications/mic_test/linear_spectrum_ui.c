/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2025 Jakob Krantz.
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

#include "linear_spectrum_ui.h"
#include "spectrum_analyzer.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(linear_spectrum_ui, LOG_LEVEL_DBG);

// UI state
static struct {
    lv_obj_t *container;
    lv_obj_t *bars[LINEAR_SPECTRUM_BARS];
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    bool initialized;
} linear_ui;

// Color gradient for linear spectrum (blue to red)
static uint32_t get_linear_bar_color(uint8_t magnitude)
{
    if (magnitude < 64) {
        // Blue to cyan
        uint8_t intensity = magnitude * 4;
        return (0x00 << 16) | (intensity << 8) | 0xFF;
    } else if (magnitude < 128) {
        // Cyan to green
        uint8_t fade = (magnitude - 64) * 4;
        return (0x00 << 16) | (0xFF << 8) | (255 - fade);
    } else if (magnitude < 192) {
        // Green to yellow
        uint8_t red_fade = (magnitude - 128) * 4;
        return (red_fade << 16) | (0xFF << 8) | 0x00;
    } else {
        // Yellow to red
        uint8_t green_fade = 255 - ((magnitude - 192) * 4);
        return (0xFF << 16) | (green_fade << 8) | 0x00;
    }
}

int linear_spectrum_ui_init(lv_obj_t *parent, int16_t x, int16_t y,
                            uint16_t width, uint16_t height)
{
    if (!parent) {
        LOG_ERR("Parent object is NULL");
        return -EINVAL;
    }

    if (linear_ui.initialized) {
        LOG_WRN("Linear spectrum UI already initialized");
        return -EALREADY;
    }

    // Store configuration
    linear_ui.x = x;
    linear_ui.y = y;
    linear_ui.width = width;
    linear_ui.height = height;

    // Create container for spectrum bars - make it visible for debugging
    linear_ui.container = lv_obj_create(parent);
    lv_obj_set_size(linear_ui.container, width, height);
    lv_obj_set_pos(linear_ui.container, x, y);
    lv_obj_set_style_bg_color(linear_ui.container, lv_color_hex(0x222222), LV_PART_MAIN); // Dark background
    lv_obj_set_style_bg_opa(linear_ui.container, LV_OPA_50, LV_PART_MAIN); // Semi-transparent
    lv_obj_set_style_border_width(linear_ui.container, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(linear_ui.container, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // White border for visibility

    // Calculate bar width and spacing
    uint16_t bar_width = (width - 2) / LINEAR_SPECTRUM_BARS;
    if (bar_width < 2) bar_width = 2;
    uint16_t spacing = 1;

    // Create individual bars as rectangles
    for (int i = 0; i < LINEAR_SPECTRUM_BARS; i++) {
        linear_ui.bars[i] = lv_obj_create(linear_ui.container);
        
        // Position bars horizontally with proper spacing
        int16_t bar_x = i * (bar_width + spacing);
        lv_obj_set_pos(linear_ui.bars[i], bar_x, height - 20); // Start from bottom
        lv_obj_set_size(linear_ui.bars[i], bar_width, 20); // Start with visible height
        
        // Style the bars with very bright colors and borders for visibility
        lv_obj_set_style_bg_color(linear_ui.bars[i], lv_color_hex(0xFF0000), LV_PART_MAIN); // Bright red
        lv_obj_set_style_bg_opa(linear_ui.bars[i], LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(linear_ui.bars[i], 1, LV_PART_MAIN);
        lv_obj_set_style_border_color(linear_ui.bars[i], lv_color_hex(0xFFFFFF), LV_PART_MAIN); // White border
        lv_obj_set_style_radius(linear_ui.bars[i], 0, LV_PART_MAIN);
    }

    linear_ui.initialized = true;
    LOG_INF("Linear spectrum UI initialized with %d bars (%dx%d)", 
            LINEAR_SPECTRUM_BARS, width, height);
    return 0;
}

void linear_spectrum_ui_update(const uint8_t *magnitudes, size_t num_bars)
{
    if (!linear_ui.initialized || !linear_ui.container) {
        LOG_WRN("Linear spectrum UI not initialized");
        return;
    }

    if (!magnitudes) {
        LOG_ERR("Invalid magnitudes pointer");
        return;
    }

    // Map input bars to our display bars
    for (int i = 0; i < LINEAR_SPECTRUM_BARS; i++) {
        uint8_t magnitude;
        
        if (num_bars <= LINEAR_SPECTRUM_BARS) {
            // If we have fewer input bars, repeat/interpolate
            int src_index = (i * num_bars) / LINEAR_SPECTRUM_BARS;
            magnitude = magnitudes[src_index < num_bars ? src_index : num_bars - 1];
        } else {
            // If we have more input bars, average groups
            int start_idx = (i * num_bars) / LINEAR_SPECTRUM_BARS;
            int end_idx = ((i + 1) * num_bars) / LINEAR_SPECTRUM_BARS;
            uint32_t sum = 0;
            int count = 0;
            
            for (int j = start_idx; j < end_idx && j < num_bars; j++) {
                sum += magnitudes[j];
                count++;
            }
            magnitude = count > 0 ? (uint8_t)(sum / count) : 0;
        }

        // Calculate bar height (minimum 5, maximum 80% of container height)
        uint16_t max_height = (linear_ui.height * 80) / 100;
        uint16_t bar_height = 5 + (magnitude * (max_height - 5)) / 255;

        // Update bar size and position (grow upward from bottom)
        lv_obj_set_size(linear_ui.bars[i], 
                        lv_obj_get_width(linear_ui.bars[i]), bar_height);
        lv_obj_set_pos(linear_ui.bars[i], 
                       lv_obj_get_x(linear_ui.bars[i]), 
                       linear_ui.height - bar_height);

        // Update bar color based on magnitude
        uint32_t color = get_linear_bar_color(magnitude);
        lv_obj_set_style_bg_color(linear_ui.bars[i], lv_color_hex(color), LV_PART_MAIN);
    }
}

void linear_spectrum_ui_remove(void)
{
    if (linear_ui.container) {
        lv_obj_del(linear_ui.container);
        linear_ui.container = NULL;
    }

    // Clear bar references
    for (int i = 0; i < LINEAR_SPECTRUM_BARS; i++) {
        linear_ui.bars[i] = NULL;
    }

    linear_ui.initialized = false;
    LOG_INF("Linear spectrum UI removed");
}
