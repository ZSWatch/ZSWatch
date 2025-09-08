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

#include "circular_spectrum_ui.h"
#include "spectrum_analyzer.h"
#include <zephyr/logging/log.h>
#include <math.h>

#define M_PI 3.14159265358979323846

LOG_MODULE_REGISTER(circular_spectrum_ui, LOG_LEVEL_DBG);

// UI state
static struct {
    lv_obj_t *container;
    lv_obj_t *bars[SPECTRUM_NUM_BARS_CIRCULAR];
    lv_point_precise_t line_points[SPECTRUM_NUM_BARS_CIRCULAR][2];
    int16_t center_x;
    int16_t center_y;
    uint16_t inner_radius;
    uint16_t outer_radius;
    bool initialized;
} spectrum_ui;

int circular_spectrum_ui_init(lv_obj_t *parent, int16_t center_x, int16_t center_y,
                              uint16_t inner_radius, uint16_t outer_radius)
{
    if (!parent) {
        LOG_ERR("Parent object is NULL");
        return -EINVAL;
    }

    if (spectrum_ui.initialized) {
        LOG_WRN("Circular spectrum UI already initialized");
        return -EALREADY;
    }

    // Store configuration
    spectrum_ui.center_x = center_x;
    spectrum_ui.center_y = center_y;
    spectrum_ui.inner_radius = inner_radius;
    spectrum_ui.outer_radius = outer_radius;

    // Create container for spectrum bars
    spectrum_ui.container = lv_obj_create(parent);
    lv_obj_set_size(spectrum_ui.container, 240, 240);
    lv_obj_center(spectrum_ui.container);
    lv_obj_set_style_bg_opa(spectrum_ui.container, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(spectrum_ui.container, 0, LV_PART_MAIN);

    // Create individual bars as lines
    for (int i = 0; i < SPECTRUM_NUM_BARS_CIRCULAR; i++) {
        spectrum_ui.bars[i] = lv_line_create(spectrum_ui.container);
        lv_obj_set_style_line_width(spectrum_ui.bars[i], 5, LV_PART_MAIN);
        lv_obj_set_style_line_color(spectrum_ui.bars[i], lv_color_hex(0xFF0000), LV_PART_MAIN);
        lv_obj_set_style_line_opa(spectrum_ui.bars[i], LV_OPA_COVER, LV_PART_MAIN);

        // Initialize with default line points (short line to make it visible)
        spectrum_ui.line_points[i][0].x = spectrum_ui.center_x;
        spectrum_ui.line_points[i][0].y = spectrum_ui.center_y;
        spectrum_ui.line_points[i][1].x = spectrum_ui.center_x + 20;
        spectrum_ui.line_points[i][1].y = spectrum_ui.center_y;
        lv_line_set_points(spectrum_ui.bars[i], spectrum_ui.line_points[i], 2);
    }

    spectrum_ui.initialized = true;
    LOG_INF("Circular spectrum UI initialized with %d bars", SPECTRUM_NUM_BARS_CIRCULAR);
    return 0;
}

void circular_spectrum_ui_update(const uint8_t *magnitudes, size_t num_bars)
{
    if (!spectrum_ui.initialized || !spectrum_ui.container) {
        LOG_WRN("Circular spectrum UI not initialized");
        return;
    }

    if (!magnitudes || num_bars != SPECTRUM_NUM_BARS_CIRCULAR) {
        LOG_ERR("Invalid parameters: magnitudes=%p, num_bars=%zu", magnitudes, num_bars);
        return;
    }

    // Update each frequency bar
    for (size_t i = 0; i < num_bars; i++) {
        // Calculate angle for this bar (evenly distributed around circle)
        float angle_rad = (float)i * (2.0f * M_PI) / (float)num_bars;

        // Calculate bar length based on magnitude
        uint16_t bar_length = (magnitudes[i] * (spectrum_ui.outer_radius - spectrum_ui.inner_radius)) / 255;
        uint16_t bar_end_radius = spectrum_ui.inner_radius + bar_length;

        // Calculate start and end points
        lv_value_precise_t start_x = spectrum_ui.center_x + (lv_value_precise_t)(spectrum_ui.inner_radius * cosf(angle_rad));
        lv_value_precise_t start_y = spectrum_ui.center_y + (lv_value_precise_t)(spectrum_ui.inner_radius * sinf(angle_rad));
        lv_value_precise_t end_x = spectrum_ui.center_x + (lv_value_precise_t)(bar_end_radius * cosf(angle_rad));
        lv_value_precise_t end_y = spectrum_ui.center_y + (lv_value_precise_t)(bar_end_radius * sinf(angle_rad));

        // Get color for this bar
        uint32_t color_rgb = spectrum_get_bar_color((uint8_t)i, magnitudes[i]);
        lv_color_t bar_color = lv_color_hex(color_rgb);

        // Update line points
        spectrum_ui.line_points[i][0].x = start_x;
        spectrum_ui.line_points[i][0].y = start_y;
        spectrum_ui.line_points[i][1].x = end_x;
        spectrum_ui.line_points[i][1].y = end_y;

        // Set line points and color
        lv_line_set_points(spectrum_ui.bars[i], spectrum_ui.line_points[i], 2);
        lv_obj_set_style_line_color(spectrum_ui.bars[i], bar_color, LV_PART_MAIN);
    }
}

void circular_spectrum_ui_remove(void)
{
    if (spectrum_ui.container) {
        lv_obj_del(spectrum_ui.container);
        spectrum_ui.container = NULL;
    }

    // Clear bar references
    for (int i = 0; i < SPECTRUM_NUM_BARS_CIRCULAR; i++) {
        spectrum_ui.bars[i] = NULL;
    }

    spectrum_ui.initialized = false;
    LOG_INF("Circular spectrum UI removed");
}
