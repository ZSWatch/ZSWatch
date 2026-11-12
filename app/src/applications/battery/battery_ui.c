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

#include <battery/battery_ui.h>
#include "battery_ui.h"
#include "ui/utils/zsw_ui_utils.h"
#include <lvgl.h>
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"
#include <assert.h>
#include <string.h>

// The blue of the nPM1300, as the glow behind every page
#define BATTERY_COLOR       lv_color_hex(0x00A2C6)
#define PERCENT_COLOR       ZSW_COLOR_GREEN
#define VOLTAGE_COLOR       ZSW_COLOR_SKY

#define CHART_WIDTH         160
#define CHART_HEIGHT        120
#define CHART_MIN_MV        3000
#define CHART_MAX_MV        4300

#define INFO_LIST_WIDTH     176

static void overview_init(lv_obj_t *tile);
static void history_init(lv_obj_t *tile);
static void charger_init(lv_obj_t *tile);
static void rails_init(lv_obj_t *tile);
static void on_tileview_change(lv_event_t *e);
static void chart_event(lv_event_t *e);
static void scale_event(lv_event_t *e);

static lv_obj_t *root_page;
static lv_obj_t *tv;
static lv_obj_t *page_dots;

// Overview page
static lv_obj_t *overview_ring;
static lv_obj_t *overview_percent_label;
static lv_obj_t *overview_voltage_label;
static lv_obj_t *overview_status_label;
static lv_obj_t *overview_charge_icon;
static int overview_percent;

// History page
static lv_obj_t *history_chart;
static lv_chart_series_t *history_percent_series;
static lv_chart_series_t *history_voltage_series;
static lv_obj_t *history_value_label;

// Charger page
static lv_obj_t *charger_status_label;
static lv_obj_t *charger_error_label;
static lv_obj_t *charger_ttf_label;
static lv_obj_t *charger_tte_label;

// Power rails page
static lv_obj_t *rails_list;
static lv_obj_t *rail_dots[BATTERY_UI_MAX_RAILS];
static lv_obj_t *rail_value_labels[BATTERY_UI_MAX_RAILS];

static bool pmic_ui_enabled;
static int max_battery_samples;
static on_clear_history on_clear_history_cb;

void battery_ui_show(lv_obj_t *root, on_clear_history clear_hist_cb, int max_samples, bool include_pmic_ui)
{
    assert(root_page == NULL);
    on_clear_history_cb = clear_hist_cb;
    pmic_ui_enabled = include_pmic_ui;
    max_battery_samples = max_samples;

    // One glow for all pages, it stays in place while the pages are swiped
    root_page = zsw_ui_screen_create(root, BATTERY_COLOR);

    tv = lv_tileview_create(root_page);
    lv_obj_set_size(tv, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tv, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(tv, LV_SCROLLBAR_MODE_OFF);

    overview_init(lv_tileview_add_tile(tv, 0, 0, LV_DIR_HOR));
    history_init(lv_tileview_add_tile(tv, 1, 0, LV_DIR_HOR));
    if (include_pmic_ui) {
        charger_init(lv_tileview_add_tile(tv, 2, 0, LV_DIR_HOR));
        rails_init(lv_tileview_add_tile(tv, 3, 0, LV_DIR_HOR));
    }
    page_dots = zsw_ui_page_dots_create(root_page, include_pmic_ui ? 4 : 2);
    lv_obj_add_event_cb(tv, on_tileview_change, LV_EVENT_VALUE_CHANGED, NULL);
}

void battery_ui_remove(void)
{
    assert(root_page != NULL);
    lv_obj_del(root_page);
    root_page = NULL;
    rails_list = NULL;
    memset(rail_dots, 0, sizeof(rail_dots));
}

static lv_color_t level_color(int percent)
{
    if (percent > 40) {
        return ZSW_COLOR_GREEN;
    } else if (percent > 15) {
        return ZSW_COLOR_ORANGE;
    }
    return ZSW_COLOR_RED;
}

static void charge_icon_anim_exec(void *icon, int32_t value)
{
    lv_obj_set_style_text_opa(icon, value, LV_PART_MAIN);
}

// The charge symbol fades in and out while charging: a few pixels, so it costs next to nothing.
static void set_charging(bool charging)
{
    if (charging == !lv_obj_has_flag(overview_charge_icon, LV_OBJ_FLAG_HIDDEN)) {
        return;
    }

    lv_anim_delete(overview_charge_icon, charge_icon_anim_exec);
    if (!charging) {
        lv_obj_add_flag(overview_charge_icon, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_remove_flag(overview_charge_icon, LV_OBJ_FLAG_HIDDEN);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, overview_charge_icon);
    lv_anim_set_exec_cb(&anim, charge_icon_anim_exec);
    lv_anim_set_values(&anim, LV_OPA_30, LV_OPA_COVER);
    lv_anim_set_duration(&anim, 900);
    lv_anim_set_reverse_duration(&anim, 900);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&anim);
}

static void overview_init(lv_obj_t *tile)
{
    lv_obj_t *screen = zsw_ui_page_create(tile);

    overview_percent = -1;
    overview_ring = zsw_ui_ring_create(screen, ZSW_COLOR_GREEN);
    zsw_ui_title_create(screen, "BATTERY", ZSW_COLOR_TEXT_DIM);

    overview_charge_icon = zsw_ui_label_create(screen, LV_SYMBOL_CHARGE, ZSW_FONT_TITLE, ZSW_COLOR_GREEN);
    lv_obj_align(overview_charge_icon, LV_ALIGN_TOP_MID, 0, 46);
    lv_obj_add_flag(overview_charge_icon, LV_OBJ_FLAG_HIDDEN);

    overview_percent_label = zsw_ui_label_create(screen, "--%", ZSW_FONT_NUMBER, ZSW_COLOR_TEXT);
    lv_obj_align(overview_percent_label, LV_ALIGN_CENTER, 0, -8);

    overview_voltage_label = zsw_ui_label_create(screen, "", ZSW_FONT_BODY, ZSW_COLOR_TEXT_DIM);
    lv_obj_align(overview_voltage_label, LV_ALIGN_CENTER, 0, 32);

    overview_status_label = zsw_ui_label_create(screen, "", ZSW_FONT_TEXT, ZSW_COLOR_TEXT);
    lv_obj_align(overview_status_label, LV_ALIGN_CENTER, 0, 58);
}

static lv_obj_t *axis_label_create(lv_obj_t *parent, const char *text, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(parent);

    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, ZSW_FONT_CAPTION, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, color, LV_PART_MAIN);

    return label;
}

static void history_init(lv_obj_t *tile)
{
    lv_obj_t *screen = zsw_ui_page_create(tile);

    zsw_ui_title_create(screen, "HISTORY", ZSW_COLOR_TEXT_DIM);

    history_chart = lv_chart_create(screen);
    lv_obj_set_size(history_chart, CHART_WIDTH, CHART_HEIGHT);
    lv_obj_align(history_chart, LV_ALIGN_CENTER, -8, -2);
    lv_chart_set_type(history_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(history_chart, max_battery_samples);
    lv_chart_set_range(history_chart, LV_CHART_AXIS_SECONDARY_Y, CHART_MIN_MV, CHART_MAX_MV);
    lv_chart_set_div_line_count(history_chart, 5, 0);
    lv_obj_set_style_bg_opa(history_chart, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(history_chart, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(history_chart, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(history_chart, 0, LV_PART_MAIN);
    lv_obj_set_style_line_color(history_chart, ZSW_COLOR_SURFACE_HIGH, LV_PART_MAIN);
    lv_obj_set_style_line_width(history_chart, 1, LV_PART_MAIN);
    lv_obj_set_style_line_width(history_chart, 2, LV_PART_ITEMS);
    // Points only when there are few enough to tell them apart
    if (max_battery_samples < 20) {
        lv_obj_set_style_size(history_chart, 5, 5, LV_PART_INDICATOR);
    } else {
        lv_obj_set_style_size(history_chart, 0, 0, LV_PART_INDICATOR);
    }
    lv_obj_add_event_cb(history_chart, chart_event, LV_EVENT_LONG_PRESSED, NULL);

    history_percent_series = lv_chart_add_series(history_chart, PERCENT_COLOR, LV_CHART_AXIS_PRIMARY_Y);
    history_voltage_series = lv_chart_add_series(history_chart, VOLTAGE_COLOR, LV_CHART_AXIS_SECONDARY_Y);

    // Voltage axis to the right of the chart. The percent axis needs none: the grid is 0, 25, 50, 75, 100.
    lv_obj_t *scale = lv_scale_create(screen);
    lv_scale_set_mode(scale, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_scale_set_range(scale, CHART_MIN_MV, CHART_MAX_MV);
    lv_scale_set_total_tick_count(scale, 5);
    lv_scale_set_major_tick_every(scale, 1);
    lv_scale_set_label_show(scale, true);
    lv_obj_set_style_line_width(scale, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_width(scale, 0, LV_PART_MAIN);
    lv_obj_set_style_length(scale, 0, LV_PART_INDICATOR);
    lv_obj_set_style_text_font(scale, ZSW_FONT_CAPTION, LV_PART_INDICATOR);
    lv_obj_set_style_text_color(scale, VOLTAGE_COLOR, LV_PART_INDICATOR);
    lv_obj_set_height(scale, CHART_HEIGHT);
    lv_obj_align_to(scale, history_chart, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    // The scale holds millivolt, the labels show volt
    lv_obj_add_flag(scale, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_add_event_cb(scale, scale_event, LV_EVENT_DRAW_TASK_ADDED, NULL);

    lv_obj_t *percent_unit = axis_label_create(screen, "%", PERCENT_COLOR);
    lv_obj_align_to(percent_unit, history_chart, LV_ALIGN_OUT_LEFT_TOP, -4, -6);
    lv_obj_t *voltage_unit = axis_label_create(screen, "V", VOLTAGE_COLOR);
    lv_obj_align_to(voltage_unit, history_chart, LV_ALIGN_OUT_RIGHT_TOP, 8, -22);

    history_value_label = lv_label_create(screen);
    lv_label_set_text(history_value_label, "");
    lv_label_set_recolor(history_value_label, true);
    lv_obj_set_style_text_font(history_value_label, ZSW_FONT_TEXT, LV_PART_MAIN);
    lv_obj_align(history_value_label, LV_ALIGN_BOTTOM_MID, 0, -30);
}

static void charger_init(lv_obj_t *tile)
{
    lv_obj_t *screen = zsw_ui_page_create(tile);
    lv_obj_t *list = zsw_ui_info_list_create(screen, INFO_LIST_WIDTH);

    zsw_ui_title_create(screen, "CHARGER", ZSW_COLOR_TEXT_DIM);

    charger_status_label = zsw_ui_info_row_create(list, "Status", true);
    charger_error_label = zsw_ui_info_row_create(list, "Error", true);
    charger_ttf_label = zsw_ui_info_row_create(list, "Full in", true);
    charger_tte_label = zsw_ui_info_row_create(list, "Empty in", true);
}

static void rails_init(lv_obj_t *tile)
{
    lv_obj_t *screen = zsw_ui_page_create(tile);

    zsw_ui_title_create(screen, "POWER RAILS", ZSW_COLOR_TEXT_DIM);
    // The rows are added by battery_ui_set_rail()
    rails_list = zsw_ui_info_list_create(screen, INFO_LIST_WIDTH);
}

void battery_ui_add_measurement(int percent, int voltage)
{
    if (!root_page) {
        return;
    }

    lv_chart_set_next_value(history_chart, history_percent_series, percent);
    lv_chart_set_next_value(history_chart, history_voltage_series, voltage);
    lv_label_set_text_fmt(history_value_label, "#30D158 %d%%#   #2AA7E8 %.2f V#", percent, voltage / 1000.0);

    // Called once per stored sample when the app opens: only the first one animates the ring.
    lv_color_t color = level_color(percent);
    zsw_ui_ring_set_color(overview_ring, color);
    zsw_ui_ring_set_value(overview_ring, percent * ZSW_UI_RING_MAX / 100, overview_percent == -1);
    lv_obj_set_style_text_color(overview_charge_icon, color, LV_PART_MAIN);
    lv_label_set_text_fmt(overview_percent_label, "%d%%", percent);
    lv_label_set_text_fmt(overview_voltage_label, "%.2f V", voltage / 1000.0);
    overview_percent = percent;
}

static void format_duration(char *buf, size_t size, int seconds)
{
    int days, hours, minutes;

    zsw_ui_utils_seconds_to_day_hour_min(seconds, &days, &hours, &minutes);
    if (seconds <= 0) {
        lv_snprintf(buf, size, "-");
    } else if (days > 0) {
        lv_snprintf(buf, size, "%dd %dh", days, hours);
    } else if (hours > 0) {
        lv_snprintf(buf, size, "%dh %dm", hours, minutes);
    } else {
        lv_snprintf(buf, size, "%dm", minutes);
    }
}

void battery_ui_update(int ttf, int tte, const char *status_text, const char *error_text, int charging)
{
    char ttf_text[16];
    char tte_text[16];

    if (!root_page) {
        return;
    }

    format_duration(ttf_text, sizeof(ttf_text), ttf);
    format_duration(tte_text, sizeof(tte_text), tte);

    set_charging(charging);
    if (charging && ttf > 0) {
        lv_label_set_text_fmt(overview_status_label, "Full in %s", ttf_text);
    } else if (charging) {
        lv_label_set_text(overview_status_label, "Charging");
    } else if (tte > 0) {
        lv_label_set_text_fmt(overview_status_label, "%s left", tte_text);
    } else {
        lv_label_set_text(overview_status_label, "");
    }

    if (pmic_ui_enabled) {
        lv_label_set_text(charger_ttf_label, ttf_text);
        lv_label_set_text(charger_tte_label, tte_text);
        lv_label_set_text(charger_status_label, status_text);
        lv_label_set_text(charger_error_label, error_text);
        lv_obj_set_style_text_color(charger_status_label, charging ? ZSW_COLOR_GREEN : ZSW_COLOR_TEXT, LV_PART_MAIN);
    }
}

void battery_ui_set_rail(int index, const char *name, int millivolt, bool enabled)
{
    if (!root_page || !rails_list || index < 0 || index >= BATTERY_UI_MAX_RAILS) {
        return;
    }

    // Rails are reported in order, so the row for a new index goes last in the list
    if (rail_dots[index] == NULL) {
        rail_value_labels[index] = zsw_ui_info_row_create(rails_list, name, true);

        // A dot before the name tells if the rail is on
        rail_dots[index] = lv_obj_create(lv_obj_get_parent(rail_value_labels[index]));
        lv_obj_set_size(rail_dots[index], 8, 8);
        lv_obj_set_style_radius(rail_dots[index], LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_move_to_index(rail_dots[index], 0);
    }

    lv_obj_set_style_bg_color(rail_dots[index], enabled ? ZSW_COLOR_OK : ZSW_COLOR_OUTLINE, LV_PART_MAIN);
    lv_obj_set_style_text_color(rail_value_labels[index], enabled ? ZSW_COLOR_TEXT : ZSW_COLOR_TEXT_DISABLED,
                                LV_PART_MAIN);
    if (enabled) {
        lv_label_set_text_fmt(rail_value_labels[index], "%.2f V", millivolt / 1000.0);
    } else {
        lv_label_set_text(rail_value_labels[index], "Off");
    }
}

static void chart_event(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_LONG_PRESSED && on_clear_history_cb) {
        on_clear_history_cb();
    }
}

static void scale_event(lv_event_t *e)
{
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);

    if (!draw_task) {
        return;
    }

    lv_draw_dsc_base_t *base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    if (!base_dsc || base_dsc->part != LV_PART_INDICATOR) {
        return;
    }

    lv_draw_label_dsc_t *label_dsc = lv_draw_task_get_label_dsc(draw_task);
    if (!label_dsc || !label_dsc->text) {
        return;
    }

    if (label_dsc->text_local) {
        lv_free((void *)label_dsc->text);
    }

    char tmp_buffer[20] = {0};
    lv_snprintf(tmp_buffer, sizeof(tmp_buffer), "%.1f", (float) label_dsc->base.id2 / 1000.0);
    label_dsc->text = lv_strdup(tmp_buffer);
    label_dsc->text_local = 1;
}

static void on_tileview_change(lv_event_t *e)
{
    lv_obj_t *current = lv_tileview_get_tile_active(tv);

    zsw_ui_page_dots_set_page(page_dots, lv_obj_get_x(current) / lv_obj_get_width(current));
}
