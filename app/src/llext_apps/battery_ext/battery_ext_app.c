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

/*
 * LLEXT battery monitor app — validates:
 *   - Background zbus listener (battery_sample_data_chan)
 *   - Complex LVGL UI (chart widget with live data)
 *   - Image icon compiled into .rodata → XIP
 *   - UI state gating (only update chart when visible)
 *
 * Simplified vs. built-in battery_app: single chart page,
 * no history persistence, no PMIC-specific pages.
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>
#include <lvgl.h>

#include "managers/zsw_app_manager.h"
#include "events/battery_event.h"

/* ---------- Image (compiled into .rodata → lives in XIP) ---------- */
#include "images/battery_app_icon.c"

/* ---------- Forward declarations ---------- */
static void battery_ext_start(lv_obj_t *root, lv_group_t *group);
static void battery_ext_stop(void);
static void zbus_battery_callback(const struct zbus_channel *chan);

/* ---------- Zbus runtime observer ---------- */
ZBUS_CHAN_DECLARE(battery_sample_data_chan);

static struct zbus_observer_data battery_ext_obs_data = {
    .enabled = true,
};

static struct zbus_observer battery_ext_listener = {
#if defined(CONFIG_ZBUS_OBSERVER_NAME)
    .name = "bat_ext_lis",
#endif
    .type = ZBUS_OBSERVER_LISTENER_TYPE,
    .data = &battery_ext_obs_data,
    .callback = zbus_battery_callback,
};

/* ---------- UI state ---------- */
static lv_obj_t *root_page;
static lv_obj_t *chart;
static lv_chart_series_t *percent_series;
static lv_chart_series_t *voltage_series;
static lv_obj_t *status_label;

#define MAX_CHART_POINTS 50

/* ---------- App registration ---------- */
static application_t app = {
    .name = "Battery EXT",
    .icon = &battery_app_icon,
    .start_func = battery_ext_start,
    .stop_func = battery_ext_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

/* ---------- UI ---------- */

static void battery_ext_start(lv_obj_t *root, lv_group_t *group)
{
    LV_UNUSED(group);
    printk("battery_ext: start\n");

    root_page = lv_obj_create(root);
    lv_obj_remove_style_all(root_page);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_align(root_page, LV_ALIGN_CENTER);
    lv_obj_remove_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    /* Title */
    lv_obj_t *title = lv_label_create(root_page);
    lv_label_set_text(title, "Battery EXT");
    lv_obj_set_style_text_color(title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_align(title, LV_ALIGN_TOP_MID);
    lv_obj_set_y(title, 5);

    /* Chart */
    chart = lv_chart_create(root_page);
    lv_obj_set_size(chart, 180, 120);
    lv_obj_set_align(chart, LV_ALIGN_CENTER);
    lv_obj_set_y(chart, -5);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, MAX_CHART_POINTS);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 3000, 4500);
    lv_chart_set_div_line_count(chart, 5, 0);

    /* Chart styling */
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_color(chart, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_border_opa(chart, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(chart, 1, LV_PART_MAIN);
    lv_obj_set_style_line_color(chart, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_line_opa(chart, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_size(chart, 0, 0, LV_PART_INDICATOR);

    /* Series */
    percent_series = lv_chart_add_series(chart, lv_color_hex(0x00BCFF),
                                         LV_CHART_AXIS_PRIMARY_Y);
    voltage_series = lv_chart_add_series(chart, lv_color_hex(0x1EB931),
                                         LV_CHART_AXIS_SECONDARY_Y);

    /* Axis labels */
    lv_obj_t *pct_label = lv_label_create(root_page);
    lv_label_set_text(pct_label, "%");
    lv_obj_set_style_text_color(pct_label, lv_color_hex(0x00BCFF), LV_PART_MAIN);
    lv_obj_set_align(pct_label, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(pct_label, 10);
    lv_obj_set_y(pct_label, -35);

    lv_obj_t *v_label = lv_label_create(root_page);
    lv_label_set_text(v_label, "V");
    lv_obj_set_style_text_color(v_label, lv_color_hex(0x1EB931), LV_PART_MAIN);
    lv_obj_set_align(v_label, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(v_label, -10);
    lv_obj_set_y(v_label, -35);

    /* Status label at bottom */
    status_label = lv_label_create(root_page);
    lv_label_set_text(status_label, "Waiting...");
    lv_obj_set_style_text_color(status_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_align(status_label, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(status_label, -20);

    /* Read initial battery sample */
    struct battery_sample_event sample;

    if (zbus_chan_read(&battery_sample_data_chan, &sample, K_MSEC(100)) == 0) {
        lv_chart_set_next_value(chart, percent_series, sample.percent);
        lv_chart_set_next_value(chart, voltage_series, sample.mV);
        lv_label_set_text_fmt(status_label, "%d%% / %d.%02dV",
                              sample.percent,
                              sample.mV / 1000,
                              (sample.mV % 1000) / 10);
    }
}

static void battery_ext_stop(void)
{
    printk("battery_ext: stop\n");
    lv_obj_delete(root_page);
    root_page = NULL;
    chart = NULL;
    percent_series = NULL;
    voltage_series = NULL;
    status_label = NULL;
}

/* ---------- Background: zbus battery listener ---------- */

static void zbus_battery_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event = zbus_chan_const_msg(chan);

    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE && chart != NULL) {
        lv_chart_set_next_value(chart, percent_series, event->percent);
        lv_chart_set_next_value(chart, voltage_series, event->mV);
        lv_label_set_text_fmt(status_label, "%d%% / %d.%02dV %s",
                              event->percent,
                              event->mV / 1000,
                              (event->mV % 1000) / 10,
                              event->is_charging ? "CHG" : "");
    }
}

/* ---------- Entry point ---------- */

application_t *app_entry(void)
{
    printk("battery_ext: app_entry called\n");

    /* Runtime zbus registration (replaces compile-time ZBUS_CHAN_ADD_OBS) */
    int ret = zbus_chan_add_obs(&battery_sample_data_chan,
                                &battery_ext_listener, K_MSEC(100));
    if (ret != 0) {
        printk("battery_ext: failed to add zbus observer: %d\n", ret);
    }

    return &app;
}
EXPORT_SYMBOL(app_entry);
