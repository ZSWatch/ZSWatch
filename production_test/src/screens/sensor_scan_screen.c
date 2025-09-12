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
#include <stddef.h>
#include "sensor_scan_screen.h"

LOG_MODULE_REGISTER(sensor_scan_screen, LOG_LEVEL_INF);

static lv_obj_t *sensor_screen;
static lv_obj_t *title_label;
static lv_obj_t *test_list;
static lv_obj_t *summary_label;

typedef struct {
    const char *name;
    const test_result_t *result_ptr;
    lv_obj_t *label_obj;
    lv_obj_t *status_obj;
    lv_obj_t *container_obj;
} test_item_ui_t;

static const char *get_status_text(test_result_t result)
{
    switch (result) {
        case TEST_RESULT_PENDING:
            return "PENDING";
        case TEST_RESULT_RUNNING:
            return "TESTING";
        case TEST_RESULT_PASSED:
            return "PASS";
        case TEST_RESULT_FAILED:
            return "FAIL";
        default:
            return "UNKNOWN";
    }
}

static lv_color_t get_status_color(test_result_t result)
{
    switch (result) {
        case TEST_RESULT_PENDING:
            return lv_color_hex(0x888888);  // Gray
        case TEST_RESULT_RUNNING:
            return lv_color_hex(0x0099FF);  // Blue
        case TEST_RESULT_PASSED:
            return lv_color_hex(0x00AA00);   // Green
        case TEST_RESULT_FAILED:
            return lv_color_hex(0xFF0000);   // Red
        default:
            return lv_color_hex(0x888888);
    }
}

static lv_obj_t *create_test_item(lv_obj_t *parent, const char *name, test_result_t *result_ptr,
                                  test_item_ui_t *item_ui)
{
    // Container for each test item
    lv_obj_t *item_container = lv_obj_create(parent);
    lv_obj_set_size(item_container, LV_PCT(100), 15);
    lv_obj_set_style_bg_color(item_container, lv_color_hex(0x222222), LV_PART_MAIN);
    lv_obj_set_style_border_width(item_container, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(item_container, 2, LV_PART_MAIN);
    lv_obj_set_flex_flow(item_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(item_container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(item_container, 2, LV_PART_MAIN);
    lv_obj_clear_flag(item_container, LV_OBJ_FLAG_SCROLLABLE);

    // Test name label
    lv_obj_t *name_label = lv_label_create(item_container);
    lv_label_set_text(name_label, name);
    lv_obj_set_style_text_color(name_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(name_label, &lv_font_montserrat_12, LV_PART_MAIN);

    // Status label
    lv_obj_t *status_label = lv_label_create(item_container);
    test_result_t result = *result_ptr;
    lv_label_set_text(status_label, get_status_text(result));
    lv_obj_set_style_text_color(status_label, get_status_color(result), LV_PART_MAIN);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_12, LV_PART_MAIN);

    // Store UI references
    item_ui->name = name;
    item_ui->result_ptr = result_ptr;
    item_ui->label_obj = name_label;
    item_ui->status_obj = status_label;
    item_ui->container_obj = item_container;

    // Hide items that are still pending
    if (result == TEST_RESULT_PENDING) {
        lv_obj_add_flag(item_container, LV_OBJ_FLAG_HIDDEN);
    }

    return item_container;
}

void sensor_scan_screen_init(void)
{
    LOG_DBG("Initializing sensor scan screen");

    // Create sensor scan screen
    sensor_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(sensor_screen, lv_color_hex(0x000000), LV_PART_MAIN);

#ifdef CONFIG_BOARD_NATIVE_SIM
    // Add circle border for native sim
    lv_obj_t *circle_border = lv_obj_create(sensor_screen);
    lv_obj_set_size(circle_border, 240, 240);
    lv_obj_center(circle_border);
    lv_obj_set_style_bg_opa(circle_border, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(circle_border, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(circle_border, lv_color_hex(0xFF6600), LV_PART_MAIN);
    lv_obj_set_style_radius(circle_border, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(circle_border, LV_OPA_80, LV_PART_MAIN);
#endif

    // Title
    title_label = lv_label_create(sensor_screen);
    lv_label_set_text(title_label, "HARDWARE STATUS");
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 25);

    // Test list container
    test_list = lv_obj_create(sensor_screen);
    lv_obj_set_size(test_list, 200, 150);
    lv_obj_align(test_list, LV_ALIGN_CENTER, 0, 5);
    lv_obj_set_style_bg_color(test_list, lv_color_hex(0x111111), LV_PART_MAIN);
    lv_obj_set_style_border_width(test_list, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(test_list, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_flex_flow(test_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(test_list, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(test_list, 1, LV_PART_MAIN);
    lv_obj_clear_flag(test_list, LV_OBJ_FLAG_SCROLLABLE);

    // Summary label
    summary_label = lv_label_create(sensor_screen);
    lv_label_set_text(summary_label, "Scanning hardware...");
    lv_obj_set_style_text_color(summary_label, lv_color_hex(0x00AAFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(summary_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(summary_label, LV_ALIGN_BOTTOM_MID, 0, -25);

    LOG_DBG("Sensor scan screen initialized");
}

void sensor_scan_screen_show(const test_metadata_t *metadata,
                             size_t num_tests)
{
    LOG_DBG("Showing sensor scan screen with %zu tests", num_tests);

    lv_obj_clean(test_list);

    test_item_ui_t test_items[num_tests];

    for (size_t i = 0; i < num_tests; i++) {
        create_test_item(test_list,
                         metadata[i].name,
                         (test_result_t *)metadata[i].result_ptr,
                         &test_items[i]);
    }

    // Calculate and display summary
    int total_tests = 0;
    int passed_tests = 0;

    for (size_t i = 0; i < num_tests; i++) {
        test_result_t result = *metadata[i].result_ptr;
        if (result != TEST_RESULT_PENDING) {
            total_tests++;
            if (result == TEST_RESULT_PASSED) {
                passed_tests++;
            }
        }
    }

    // Update summary
    static char summary_text[64];
    if (total_tests == 0) {
        lv_label_set_text(summary_label, "Scanning hardware...");
        lv_obj_set_style_text_color(summary_label, lv_color_hex(0x00AAFF), LV_PART_MAIN);
    } else {
        snprintf(summary_text, sizeof(summary_text), "RESULT: %d/%d tests passed", passed_tests, total_tests);
        lv_label_set_text(summary_label, summary_text);
        lv_obj_set_style_text_color(summary_label,
                                    passed_tests == total_tests ? lv_color_hex(0x00AA00)
                                    : lv_color_hex(0xFF6600),
                                    LV_PART_MAIN);
    }

    lv_scr_load(sensor_screen);
}
