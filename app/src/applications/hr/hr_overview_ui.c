/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

#include "hr_overview_ui.h"
#include <stdio.h>
#include <zephyr/sys/util.h>
#include "lvgl_editor_gen.h"

static lv_obj_t *root;
static lv_obj_t *bpm_label;
static lv_obj_t *age_label;
static lv_obj_t *status_label;
static lv_obj_t *mini_chart;
static lv_chart_series_t *mini_series;
static lv_obj_t *history_label;
static lv_obj_t *measure_button;
static lv_obj_t *measure_label;
static lv_obj_t *settings_button;
static lv_obj_t *settings_modal;
static lv_obj_t *modal_hint;
static lv_group_t *input_group;
static struct zsw_hr_manager_snapshot current;

static char background_name[24];
static const char *const mode_names[] = {"Off", background_name, "Continuous"};
static const char *const mode_short[] = {"Off", "Background", "Continuous"};

#define TEXT_DIM   0x9A9A9A
#define HR_RED     0xFF4D5E
#define MINI_HOURS 3
#define MINI_POINTS 36

static lv_obj_t *label(lv_obj_t *parent, const char *text, int y, const lv_font_t *font)
{
    lv_obj_t *obj = lv_label_create(parent);
    lv_label_set_text(obj, text);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
    return obj;
}

static lv_obj_t *dim_label(lv_obj_t *parent, const char *text, int y, const lv_font_t *font)
{
    lv_obj_t *obj = label(parent, text, y, font);
    lv_obj_set_style_text_color(obj, lv_color_hex(TEXT_DIM), 0);
    return obj;
}

static lv_obj_t *button(lv_obj_t *parent, const char *text, int y, int width)
{
    lv_obj_t *obj = lv_button_create(parent);
    lv_obj_set_size(obj, width, 32);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x303030), 0);
    lv_obj_set_style_text_font(obj, montserrat_14_full, 0);
    lv_obj_t *caption = lv_label_create(obj);
    lv_label_set_text(caption, text);
    lv_obj_center(caption);
    if (input_group) {
        lv_group_add_obj(input_group, obj);
    }
    return obj;
}

static void close_settings(lv_event_t *event)
{
    LV_UNUSED(event);
    if (settings_modal) {
        lv_obj_delete(settings_modal);
    }
    settings_modal = NULL;
    if (input_group && settings_button) {
        lv_group_add_obj(input_group, settings_button);
        lv_group_add_obj(input_group, measure_button);
        lv_group_focus_obj(settings_button);
    }
}

static void choose_mode(lv_event_t *event)
{
    zsw_hr_setting_t mode = (zsw_hr_setting_t)(uintptr_t)lv_event_get_user_data(event);
    int rc = zsw_hr_manager_set_mode(mode);
    if (rc) {
        lv_label_set_text(modal_hint, "Could not save mode");
    } else {
        close_settings(NULL);
    }
}

static void open_settings(lv_event_t *event)
{
    LV_UNUSED(event);
    if (settings_modal) {
        return;
    }
    if (input_group) {
        lv_group_remove_obj(settings_button);
        lv_group_remove_obj(measure_button);
    }
    settings_modal = lv_obj_create(root);
    lv_obj_set_size(settings_modal, 200, 196);
    lv_obj_center(settings_modal);
    lv_obj_remove_flag(settings_modal, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(settings_modal, 0, 0);
    lv_obj_set_style_bg_color(settings_modal, lv_color_hex(0x101010), 0);
    lv_obj_set_style_bg_opa(settings_modal, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(settings_modal, 0, 0);
    label(settings_modal, "Heart rate mode", 8, montserrat_14_full);
    lv_obj_t *first = NULL;
    for (unsigned i = 0; i < 3; i++) {
        lv_obj_t *choice = button(settings_modal, mode_names[i], 34 + i * 34, 176);
        if (!first) {
            first = choice;
        }
        if (i == current.mode) {
            lv_obj_set_style_bg_color(choice, lv_color_hex(0x285E9C), 0);
        }
        lv_obj_add_event_cb(choice, choose_mode, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }
    modal_hint = label(settings_modal, "Automatic: pauses when still", 138, montserrat_10);
    lv_obj_t *close = button(settings_modal, "Back", 158, 100);
    lv_obj_add_event_cb(close, close_settings, LV_EVENT_CLICKED, NULL);
    if (input_group && first) {
        lv_group_focus_obj(first);
    }
}

static void measure(lv_event_t *event)
{
    LV_UNUSED(event);
    int rc = current.manual ? zsw_hr_manager_cancel_measurement() : zsw_hr_manager_measure_now();
    if (rc && rc != -EALREADY) {
        lv_label_set_text(status_label, "Measurement unavailable");
    }
}

static void deleted(lv_event_t *event)
{
    LV_UNUSED(event);
    root = settings_modal = settings_button = mini_chart = NULL;
    mini_series = NULL;
}

lv_obj_t *hr_overview_ui_create(lv_obj_t *parent, lv_group_t *group)
{
    int interval_s = COND_CODE_1(CONFIG_ZSW_HR_MANAGER, (CONFIG_ZSW_HR_BACKGROUND_INTERVAL_SECONDS), (0));

    if (interval_s == 0) {
        snprintf(background_name, sizeof(background_name), "Background");
    } else if (interval_s % 60 == 0) {
        snprintf(background_name, sizeof(background_name), "Background - %d min", interval_s / 60);
    } else {
        snprintf(background_name, sizeof(background_name), "Background - %d s", interval_s);
    }
    input_group = group;
    settings_modal = NULL;
    root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(root, deleted, LV_EVENT_DELETE, NULL);
    settings_button = button(root, LV_SYMBOL_SETTINGS, 8, 30);
    lv_obj_set_style_text_font(settings_button, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_bg_opa(settings_button, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_width(settings_button, 0, 0);
    lv_obj_set_style_text_color(settings_button, lv_color_hex(TEXT_DIM), 0);
    lv_obj_set_height(settings_button, 30);
    lv_obj_add_event_cb(settings_button, open_settings, LV_EVENT_CLICKED, NULL);

    lv_obj_t *reading = lv_obj_create(root);
    lv_obj_remove_style_all(reading);
    lv_obj_set_size(reading, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(reading, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(reading, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(reading, 6, 0);
    lv_obj_align(reading, LV_ALIGN_TOP_MID, 0, 38);
    lv_obj_t *heart_img = lv_image_create(reading);
    lv_image_set_src(heart_img, heart);
    lv_obj_set_style_pad_bottom(heart_img, 12, 0);
    bpm_label = lv_label_create(reading);
    lv_obj_set_style_text_font(bpm_label, montserrat_48, 0);
    lv_obj_set_style_text_color(bpm_label, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(bpm_label, "--");
    lv_obj_t *unit = lv_label_create(reading);
    lv_obj_set_style_text_font(unit, montserrat_12, 0);
    lv_obj_set_style_text_color(unit, lv_color_hex(TEXT_DIM), 0);
    lv_obj_set_style_pad_bottom(unit, 8, 0);
    lv_label_set_text(unit, "bpm");

    age_label = dim_label(root, "No measurement yet", 92, montserrat_12);

    mini_chart = lv_chart_create(root);
    lv_obj_set_size(mini_chart, 170, 40);
    lv_obj_align(mini_chart, LV_ALIGN_TOP_MID, 0, 112);
    lv_obj_set_style_bg_opa(mini_chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(mini_chart, 0, 0);
    lv_obj_set_style_pad_all(mini_chart, 2, 0);
    lv_chart_set_div_line_count(mini_chart, 0, 0);
    lv_chart_set_type(mini_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(mini_chart, MINI_POINTS);
    lv_obj_set_style_line_width(mini_chart, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(mini_chart, 0, 0, LV_PART_INDICATOR);
    mini_series = lv_chart_add_series(mini_chart, lv_color_hex(HR_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_all_value(mini_chart, mini_series, LV_CHART_POINT_NONE);
    lv_obj_add_flag(mini_chart, LV_OBJ_FLAG_HIDDEN);
    history_label = dim_label(root, "No history yet", 154, montserrat_10);

    status_label = dim_label(root, "Waiting for movement", 168, montserrat_12);
    measure_button = button(root, "Measure now", 186, 120);
    lv_obj_set_height(measure_button, 28);
    lv_obj_set_style_radius(measure_button, 14, 0);
    lv_obj_set_style_bg_color(measure_button, lv_color_hex(0x6A1E2A), 0);
    measure_label = lv_obj_get_child(measure_button, 0);
    lv_obj_add_event_cb(measure_button, measure, LV_EVENT_CLICKED, NULL);
    return root;
}

void hr_overview_ui_update(const struct zsw_hr_manager_snapshot *snapshot, int64_t now_ms)
{
    if (!root) {
        return;
    }
    current = *snapshot;
    char text[64];
    if (current.has_sample) {
        lv_label_set_text_fmt(bpm_label, "%d", current.sample.heart_rate_bpm);
        int64_t seconds = LV_MAX(0, now_ms - current.sample.timestamp_ms) / 1000;
        if (seconds < 60) {
            snprintf(text, sizeof(text), "%lld s ago - %u%% conf", (long long)seconds, current.sample.heart_rate_confidence);
        } else if (seconds < 3600) {
            snprintf(text, sizeof(text), "%lld min ago - %u%% conf", (long long)(seconds / 60), current.sample.heart_rate_confidence);
        } else {
            snprintf(text, sizeof(text), "%lld h ago", (long long)(seconds / 3600));
        }
        lv_label_set_text(age_label, text);
    } else {
        lv_label_set_text(bpm_label, "--");
        lv_label_set_text(age_label, "No measurement yet");
    }
    const char *mode = mode_short[(unsigned)current.mode <= ZSW_HR_SETTING_CONTINUOUS ? current.mode : 0];
    switch (current.status) {
        case ZSW_HR_STATUS_OFF:
            lv_label_set_text(status_label, "Automatic measurement off");
            break;
        case ZSW_HR_STATUS_WAITING_MOTION:
            lv_label_set_text_fmt(status_label, "%s - waiting for movement", mode);
            break;
        case ZSW_HR_STATUS_WAITING_INTERVAL: {
            long long seconds = LV_MAX(0, current.next_measurement_ms - now_ms) / 1000;
            if (seconds >= 60) {
                lv_label_set_text_fmt(status_label, "%s - next in %lld min", mode, (seconds + 59) / 60);
            } else {
                lv_label_set_text_fmt(status_label, "%s - next in %lld s", mode, seconds);
            }
            break;
        }
        case ZSW_HR_STATUS_MEASURING:
            lv_label_set_text(status_label, "Measuring...");
            break;
        case ZSW_HR_STATUS_ERROR:
            lv_label_set_text(status_label, current.error == -ETIMEDOUT ? "No reliable reading" : "Sensor unavailable");
            break;
        default:
            lv_label_set_text(status_label, "HR hardware unavailable");
            break;
    }
    lv_label_set_text(measure_label, current.manual ? "Cancel" : "Measure now");
    if (current.status == ZSW_HR_STATUS_UNAVAILABLE) {
        lv_obj_add_state(measure_button, LV_STATE_DISABLED);
        lv_obj_add_state(settings_button, LV_STATE_DISABLED);
    } else {
        lv_obj_remove_state(measure_button, LV_STATE_DISABLED);
        lv_obj_remove_state(settings_button, LV_STATE_DISABLED);
    }
}

// Shows the last MINI_HOURS of the minute history, one point per few minutes.
void hr_overview_ui_set_history(const uint8_t *bpm, size_t count)
{
    if (!mini_chart) {
        return;
    }
    const size_t minutes = MINI_HOURS * 60;
    const size_t per_point = minutes / MINI_POINTS;
    const uint8_t *recent = count > minutes ? bpm + count - minutes : bpm;
    size_t span = MIN(count, minutes);
    int32_t low = INT32_MAX;
    int32_t high = 0;

    lv_chart_set_all_value(mini_chart, mini_series, LV_CHART_POINT_NONE);
    for (size_t p = 0; p < MINI_POINTS; p++) {
        // Right-align so the last point is the current minute.
        size_t offset = minutes - span;
        uint32_t sum = 0;
        uint32_t n = 0;
        for (size_t m = p * per_point; m < (p + 1) * per_point; m++) {
            if (m >= offset && recent[m - offset]) {
                sum += recent[m - offset];
                n++;
            }
        }
        if (n) {
            int32_t value = sum / n;
            lv_chart_set_value_by_id(mini_chart, mini_series, p, value);
            low = MIN(low, value);
            high = MAX(high, value);
        }
    }
    if (!high) {
        lv_obj_add_flag(mini_chart, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(history_label, "No history yet");
        return;
    }
    lv_chart_set_axis_min_value(mini_chart, LV_CHART_AXIS_PRIMARY_Y, low - 8);
    lv_chart_set_axis_max_value(mini_chart, LV_CHART_AXIS_PRIMARY_Y, high + 8);
    lv_chart_refresh(mini_chart);
    lv_obj_remove_flag(mini_chart, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text_fmt(history_label, "Last %d h: %d-%d bpm", MINI_HOURS, (int)low, (int)high);
}
