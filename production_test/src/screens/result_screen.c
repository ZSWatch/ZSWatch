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
#include "result_screen.h"

LOG_MODULE_REGISTER(result_screen, LOG_LEVEL_INF);

static lv_obj_t *result_screen;
static lv_obj_t *result_icon;
static lv_obj_t *result_label;
static lv_obj_t *details_label;
static lv_obj_t *action_label;

void result_screen_init(void)
{
    LOG_DBG("Initializing result screen");

    // Create result screen
    result_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(result_screen, lv_color_hex(0x000000), LV_PART_MAIN);

#ifdef CONFIG_BOARD_NATIVE_SIM
    // Add circle border for native sim
    lv_obj_t *circle_border = lv_obj_create(result_screen);
    lv_obj_set_size(circle_border, 240, 240);
    lv_obj_center(circle_border);
    lv_obj_set_style_bg_opa(circle_border, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(circle_border, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(circle_border, lv_color_hex(0xFF6600), LV_PART_MAIN);
    lv_obj_set_style_radius(circle_border, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_opa(circle_border, LV_OPA_80, LV_PART_MAIN);
#endif

    result_icon = lv_label_create(result_screen);
    lv_label_set_text(result_icon, "?");
    lv_obj_set_style_text_font(result_icon, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_align(result_icon, LV_ALIGN_TOP_MID, 0, 25);

    result_label = lv_label_create(result_screen);
    lv_label_set_text(result_label, "UNIT STATUS");
    lv_obj_set_style_text_font(result_label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_align(result_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(result_label, LV_ALIGN_TOP_MID, 0, 65);

    details_label = lv_label_create(result_screen);
    lv_label_set_text(details_label, "ALL TESTS OK");
    lv_obj_set_style_text_font(details_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_align(details_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(details_label, LV_ALIGN_TOP_MID, 0, 95);
    lv_label_set_long_mode(details_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(details_label, 180);

    action_label = lv_label_create(result_screen);
    lv_label_set_text(action_label, "Ready for shipping");
    lv_obj_set_style_text_color(action_label, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
    lv_obj_set_style_text_font(action_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_set_style_text_align(action_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align(action_label, LV_ALIGN_BOTTOM_MID, 0, -35);
    lv_label_set_long_mode(action_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(action_label, 180);
}

void result_screen_show(production_test_runner_context_t *context, const char **test_names, int num_tests)
{
    LOG_INF("Showing final result screen");

    // Count passed and total tests
    int total_tests = 0;
    int passed_tests = 0;
    int failed_count = 0;
    static char failed_tests_text[200] = {0};
    bool first_failure = true;

    // Check all test results
    test_result_t *results = (test_result_t *)&context->results;
    int num_results = sizeof(context->results) / sizeof(test_result_t);

    for (int i = 0; i < num_results && i < num_tests; i++) {
        if (results[i] != TEST_RESULT_PENDING) {
            total_tests++;
            if (results[i] == TEST_RESULT_PASSED) {
                passed_tests++;
            } else if (results[i] == TEST_RESULT_FAILED) {
                failed_count++;
                if (first_failure) {
                    snprintf(failed_tests_text, sizeof(failed_tests_text), "Failed: %s", test_names[i]);
                    first_failure = false;
                } else {
                    size_t len = strlen(failed_tests_text);
                    snprintf(failed_tests_text + len, sizeof(failed_tests_text) - len, ", %s", test_names[i]);
                }
            }
        }
    }

    // Determine overall result
    bool all_passed = (passed_tests == total_tests && total_tests > 0);

    if (all_passed) {
        lv_label_set_text(result_icon, LV_SYMBOL_OK);
        lv_obj_set_style_text_color(result_icon, lv_color_hex(0x00AA00), LV_PART_MAIN);

        lv_label_set_text(result_label, "UNIT PASSED");
        lv_obj_set_style_text_color(result_label, lv_color_hex(0x00AA00), LV_PART_MAIN);

        lv_label_set_text(details_label, "ALL TESTS OK");
        lv_obj_set_style_text_color(details_label, lv_color_hex(0x00AA00), LV_PART_MAIN);

        lv_label_set_text(action_label, "Ready for shipping\nPress any button to retest");
        lv_obj_set_style_text_color(action_label, lv_color_hex(0x00AA00), LV_PART_MAIN);

        LOG_INF("PRODUCTION TEST PASSED - %d/%d tests OK", passed_tests, total_tests);

    } else {
        // FAIL
        lv_label_set_text(result_icon, LV_SYMBOL_CLOSE);
        lv_obj_set_style_text_color(result_icon, lv_color_hex(0xFF0000), LV_PART_MAIN);

        lv_label_set_text(result_label, "UNIT FAILED");
        lv_obj_set_style_text_color(result_label, lv_color_hex(0xFF0000), LV_PART_MAIN);

        if (failed_count == 0) {
            snprintf(failed_tests_text, sizeof(failed_tests_text), "%d/%d tests failed",
                     total_tests - passed_tests, total_tests);
        }

        lv_label_set_text(details_label, failed_tests_text);
        lv_obj_set_style_text_color(details_label, lv_color_hex(0xFF0000), LV_PART_MAIN);

        lv_label_set_text(action_label, "Needs rework\nPress any button to retest");
        lv_obj_set_style_text_color(action_label, lv_color_hex(0xFF0000), LV_PART_MAIN);

        LOG_ERR("PRODUCTION TEST FAILED - %d/%d passed", passed_tests, total_tests);
    }

    lv_scr_load(result_screen);
}
