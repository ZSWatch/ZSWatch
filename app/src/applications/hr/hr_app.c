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
#include <lvgl.h>
#include <zephyr/random/random.h>

#include "lvgl_editor_gen.h"
#include "hr_app_overview_gen.h"
#include "hr_app_debug_gen.h"
#include "hr_app_graph_gen.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#ifndef CONFIG_ARCH_POSIX
#include "drivers/zsw_hr.h"
#endif

LOG_MODULE_REGISTER(hr_app, LOG_LEVEL_DBG);

// Activity labels for display
static const char *activity_labels[] = {"Rest", "Other", "Walk", "Run", "Bike"};
#define NUM_ACTIVITIES (sizeof(activity_labels) / sizeof(activity_labels[0]))

// Skin contact labels
static const char *skin_contact_labels[] = {"Unknown", "Off Skin", "On Subject", "On Skin"};
#define NUM_SKIN_STATES (sizeof(skin_contact_labels) / sizeof(skin_contact_labels[0]))

// App state
static lv_obj_t *root_page;
static lv_obj_t *tv;
static lv_obj_t *page_overview;
static lv_obj_t *page_debug;
static lv_obj_t *page_graph;

// Page indicator LEDs
static lv_obj_t *ui_page_indicator;
static lv_obj_t *led1;
static lv_obj_t *led2;
static lv_obj_t *led3;

// Chart and series for graph page
static lv_obj_t *hr_chart;
static lv_chart_series_t *hr_series;
static lv_chart_series_t *conf_series;

// Timer for fake data / real sensor updates
static lv_timer_t *update_timer;

// Use fake data for testing
#ifdef CONFIG_ARCH_POSIX
#define USE_FAKE_DATA 1
#endif

#if USE_FAKE_DATA
// Fake data generation state
static int fake_hr_base = 72;
static int fake_conf_base = 75;
static int fake_counter = 0;
#endif

// Forward declarations
static void hr_app_start(lv_obj_t *root, lv_group_t *group);
static void hr_app_stop(void);
static void on_tileview_change(lv_event_t *e);
static void create_page_indicator(lv_obj_t *container);
static void set_indicator_page(int page);
static void update_timer_cb(lv_timer_t *timer);
static void update_ui_from_sample(int hr, int hr_conf, int spo2, int spo2_conf, 
                                   int rr, int rr_conf, int skin_contact, int activity);

// Application registration
ZSW_LV_IMG_DECLARE(heart_beat);

static application_t app = {
    .name = "HR",
    .icon = ZSW_LV_IMG_USE(heart_beat),
    .start_func = hr_app_start,
    .stop_func = hr_app_stop,
    .category = ZSW_APP_CATEGORY_ROOT,
};

static void create_page_indicator(lv_obj_t *container)
{
    ui_page_indicator = lv_obj_create(container);
    lv_obj_set_width(ui_page_indicator, 100);
    lv_obj_set_height(ui_page_indicator, 10);

    lv_obj_align(ui_page_indicator, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_clear_flag(ui_page_indicator, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(ui_page_indicator, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_page_indicator, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    led1 = lv_led_create(ui_page_indicator);
    lv_obj_align(led1, LV_ALIGN_CENTER, -10, 0);
    lv_obj_set_size(led1, 7, 7);
    lv_led_off(led1);

    led2 = lv_led_create(ui_page_indicator);
    lv_obj_align(led2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(led2, 7, 7);
    lv_led_off(led2);

    led3 = lv_led_create(ui_page_indicator);
    lv_obj_align(led3, LV_ALIGN_CENTER, 10, 0);
    lv_obj_set_size(led3, 7, 7);
    lv_led_off(led3);

    // Set initial page
    set_indicator_page(0);
}

static void set_indicator_page(int page)
{
    lv_color_t on_color = lv_color_hex(0xE6898B);
    lv_color_t off_color = lv_color_hex(0xFFFFFF);

    lv_led_set_color(led1, (page == 0) ? on_color : off_color);
    lv_led_set_color(led2, (page == 1) ? on_color : off_color);
    lv_led_set_color(led3, (page == 2) ? on_color : off_color);

    if (page == 0) {
        lv_led_on(led1);
        lv_led_off(led2);
        lv_led_off(led3);
    } else if (page == 1) {
        lv_led_off(led1);
        lv_led_on(led2);
        lv_led_off(led3);
    } else {
        lv_led_off(led1);
        lv_led_off(led2);
        lv_led_on(led3);
    }
}

static void on_tileview_change(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t *current = lv_tileview_get_tile_act(tv);
        lv_coord_t tile_x = lv_obj_get_x(current) / lv_obj_get_width(current);
        set_indicator_page(tile_x);
    }
}

static void find_chart_in_page(lv_obj_t *parent)
{
    // Find the chart widget in the graph page
    // The chart is created by LVGL Editor, we need to find it to add data
    uint32_t child_count = lv_obj_get_child_count(parent);
    for (uint32_t i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(parent, i);
        if (lv_obj_check_type(child, &lv_chart_class)) {
            hr_chart = child;
            // Get the series - they should be created by the XML
            hr_series = lv_chart_get_series_next(hr_chart, NULL);
            conf_series = lv_chart_get_series_next(hr_chart, hr_series);
            // Hide the data point dots, show only lines
            lv_obj_set_style_size(hr_chart, 0, 0, LV_PART_INDICATOR);
            
            // Ensure chart axis range matches our scale (XML may not set it correctly)
            lv_chart_set_range(hr_chart, LV_CHART_AXIS_PRIMARY_Y, 40, 130);
            lv_chart_set_range(hr_chart, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
            
            // Find and configure the Y-axis scale (created in XML with name "hr_scale")
            lv_obj_t *hr_scale = lv_obj_get_child_by_type(hr_chart, 0, &lv_scale_class);
            if (hr_scale) {
                lv_scale_set_range(hr_scale, 40, 130);
                lv_scale_set_total_tick_count(hr_scale, 5);
                lv_scale_set_major_tick_every(hr_scale, 1);
                lv_obj_set_style_line_width(hr_scale, 0, LV_PART_INDICATOR);
                lv_obj_set_style_line_width(hr_scale, 0, LV_PART_MAIN);
                LOG_DBG("Configured HR scale");
            }
            
            LOG_DBG("Found chart with series");
            return;
        }
        // Recurse into children
        find_chart_in_page(child);
    }
}

static void update_ui_from_sample(int hr, int hr_conf, int spo2, int spo2_conf,
                                   int rr, int rr_conf, int skin_contact, int activity)
{
    char buf[32];

    // Update integer subjects
    lv_subject_set_int(&hr_bpm, hr);
    lv_subject_set_int(&hr_confidence, hr_conf);
    lv_subject_set_int(&hr_spo2, spo2);
    lv_subject_set_int(&hr_spo2_confidence, spo2_conf);
    lv_subject_set_int(&hr_rr_interval, rr);
    lv_subject_set_int(&hr_rr_confidence, rr_conf);
    lv_subject_set_int(&hr_skin_contact, skin_contact);
    lv_subject_set_int(&hr_activity, activity);

    // Update string subjects for labels
    snprintf(buf, sizeof(buf), "%d", hr);
    lv_subject_copy_string(&hr_bpm_text, buf);

    snprintf(buf, sizeof(buf), "%d%%", hr_conf);
    lv_subject_copy_string(&hr_confidence_text, buf);

    snprintf(buf, sizeof(buf), "%d%%", spo2);
    lv_subject_copy_string(&hr_spo2_text, buf);

    snprintf(buf, sizeof(buf), "%d", rr);
    lv_subject_copy_string(&hr_rr_text, buf);

    // Activity text
    if (activity >= 0 && activity < (int)NUM_ACTIVITIES) {
        lv_subject_copy_string(&hr_activity_text, activity_labels[activity]);
    } else {
        lv_subject_copy_string(&hr_activity_text, "Unknown");
    }

    // Skin contact text
    if (skin_contact >= 0 && skin_contact < (int)NUM_SKIN_STATES) {
        lv_subject_copy_string(&hr_skin_text, skin_contact_labels[skin_contact]);
    } else {
        lv_subject_copy_string(&hr_skin_text, "Unknown");
    }

    // Update chart if available
    if (hr_chart && hr_series && conf_series) {
        lv_chart_set_next_value(hr_chart, hr_series, hr);
        lv_chart_set_next_value(hr_chart, conf_series, hr_conf);
    }
}

#if USE_FAKE_DATA
static void generate_fake_data(int *hr, int *hr_conf, int *spo2, int *spo2_conf,
                                int *rr, int *rr_conf, int *skin_contact, int *activity)
{
    // Generate realistic-looking fake HR data with some variation
    fake_counter++;
    
    // HR varies between 60-100 with some noise
    int hr_variation = (fake_counter % 20) - 10;  // -10 to +10
    *hr = fake_hr_base + hr_variation + (sys_rand32_get() % 5) - 2;
    if (*hr < 50) *hr = 50;
    if (*hr > 120) *hr = 120;
    
    // Confidence builds up over time, with occasional dips
    if (fake_counter < 10) {
        *hr_conf = 30 + fake_counter * 5;
    } else {
        *hr_conf = fake_conf_base + (sys_rand32_get() % 10) - 5;
    }
    if (*hr_conf < 0) *hr_conf = 0;
    if (*hr_conf > 100) *hr_conf = 100;
    
    // SpO2 stays stable around 97-99%
    *spo2 = 97 + (sys_rand32_get() % 3);
    *spo2_conf = 80 + (sys_rand32_get() % 15);
    
    // RR interval correlates with HR (60000/HR ≈ RR in ms)
    *rr = 60000 / (*hr) + (sys_rand32_get() % 50) - 25;
    *rr_conf = 65 + (sys_rand32_get() % 20);
    
    // Skin contact: mostly on skin (3), occasionally on subject (2)
    *skin_contact = (sys_rand32_get() % 10 < 8) ? 3 : 2;
    
    // Activity: cycle through different activities
    *activity = (fake_counter / 30) % NUM_ACTIVITIES;
}
#endif

static void update_timer_cb(lv_timer_t *timer)
{
#if USE_FAKE_DATA
    int hr, hr_conf, spo2, spo2_conf, rr, rr_conf, skin_contact, activity;
    generate_fake_data(&hr, &hr_conf, &spo2, &spo2_conf, &rr, &rr_conf, &skin_contact, &activity);
    update_ui_from_sample(hr, hr_conf, spo2, spo2_conf, rr, rr_conf, skin_contact, activity);
#else
    // Real sensor data path
    struct zsw_hr_sample sample;
    if (zsw_hr_get_latest(&sample) == 0) {
        update_ui_from_sample(
            sample.heart_rate_bpm,
            sample.heart_rate_confidence,
            sample.spo2_percent,
            sample.spo2_confidence,
            sample.respiration_rate,
            sample.respiration_confidence,
            sample.skin_contact ? 3 : 0,  // Convert bool to our scale
            sample.activity_class
        );
    }
#endif
}

static void hr_app_start(lv_obj_t *root, lv_group_t *group)
{
    LOG_INF("HR App starting");

    // Create root container
    root_page = lv_obj_create(root);
    lv_obj_set_style_border_width(root_page, 0, LV_PART_MAIN);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(root_page, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(root_page, 0, LV_PART_MAIN);

    // Create tileview for swipe navigation
    tv = lv_tileview_create(root_page);
    lv_obj_set_style_pad_all(tv, 0, LV_PART_MAIN);
    lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tv, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    // Create tiles (pages)
    lv_obj_t *tile_overview = lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR);
    lv_obj_t *tile_debug = lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR);
    lv_obj_t *tile_graph = lv_tileview_add_tile(tv, 2, 0, LV_DIR_HOR);

    // Create the UI for each page using LVGL Editor generated functions
    page_overview = hr_app_overview_create(tile_overview);
    page_debug = hr_app_debug_create(tile_debug);
    page_graph = hr_app_graph_create(tile_graph);

    // Find the chart widget in the graph page for data updates
    find_chart_in_page(page_graph);

    // Create page indicator dots
    create_page_indicator(root_page);

    // Add tileview change callback
    lv_obj_add_event_cb(tv, on_tileview_change, LV_EVENT_VALUE_CHANGED, NULL);

    // Initialize with some default data
    update_ui_from_sample(72, 78, 98, 85, 850, 72, 3, 2);

#if !USE_FAKE_DATA
    // Start the HR sensor
    struct zsw_hr_config config = {
        .mode = ZSW_HR_MODE_CONTINUOUS,
        .sample_interval_ms = 1000,
    };
    int ret = zsw_hr_start(&config);
    if (ret) {
        LOG_ERR("Failed to start HR sensor: %d", ret);
    }
#endif

    // Start update timer (1 second interval)
    update_timer = lv_timer_create(update_timer_cb, 1000, NULL);

    LOG_INF("HR App started");
}

static void hr_app_stop(void)
{
    LOG_INF("HR App stopping");

#if !USE_FAKE_DATA
    // Stop the HR sensor
    zsw_hr_stop();
#endif

    // Delete timer
    if (update_timer) {
        lv_timer_del(update_timer);
        update_timer = NULL;
    }

    // Delete root (will delete all children)
    if (root_page) {
        lv_obj_del(root_page);
        root_page = NULL;
    }

    // Reset pointers
    tv = NULL;
    page_overview = NULL;
    page_debug = NULL;
    page_graph = NULL;
    hr_chart = NULL;
    hr_series = NULL;
    conf_series = NULL;
    ui_page_indicator = NULL;
    led1 = NULL;
    led2 = NULL;
    led3 = NULL;

#if USE_FAKE_DATA
    fake_counter = 0;
#endif

    LOG_INF("HR App stopped");
}

static int hr_app_init(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(hr_app_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
