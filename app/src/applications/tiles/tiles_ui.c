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

#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "tiles_ui.h"
#include "ui/zsw_ui.h"
#include "ui/utils/zsw_ui_utils.h"

#define RING_SIZE       156
#define RING_WIDTH      10
#define DOT_SIZE        6

LV_FONT_DECLARE(lv_font_montserrat_14_full);
LV_FONT_DECLARE(ui_font_aliean_25);
LV_FONT_DECLARE(ui_font_aliean_47);

static tiles_open_cb_t open_cb;
static tiles_music_cb_t music_cb;

static lv_obj_t *root_page;
static lv_obj_t *tileview;
static lv_obj_t *dots[TILE_COUNT];
static int num_tiles;

static struct {
    lv_obj_t *icon;
    lv_obj_t *temperature;
    lv_obj_t *unit;
    lv_obj_t *text;
} weather;

static struct {
    lv_obj_t *ring;
    lv_obj_t *steps;
    lv_obj_t *goal;
} activity;

static struct {
    lv_obj_t *bpm;
    lv_obj_t *age;
    lv_obj_t *chart;
    lv_chart_series_t *series;
} hr;

static struct {
    lv_obj_t *track;
    lv_obj_t *artist;
    lv_obj_t *play_pause;
} music;

static struct {
    lv_obj_t *ring;
    lv_obj_t *percent;
    lv_obj_t *status;
} battery;

static lv_color_t gray(void)
{
    return lv_color_hex(0x8C8C8C);
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    return label;
}

/* The digit fonts only have 0-9 and ':', other text such as "--" or "-3" uses Montserrat. */
static void set_number(lv_obj_t *label, const lv_font_t *digit_font, const char *text)
{
    bool digits = true;

    for (const char *c = text; *c; c++) {
        digits = digits && ((*c >= '0' && *c <= '9') || *c == ':');
    }
    lv_obj_set_style_text_font(label, digits ? digit_font : &lv_font_montserrat_28, 0);
    lv_label_set_text(label, text);
}

static void tile_clicked(lv_event_t *e)
{
    open_cb((tiles_tile_t)(uintptr_t)lv_event_get_user_data(e));
}

static lv_obj_t *add_tile(tiles_tile_t type, const char *title)
{
    lv_obj_t *tile = lv_tileview_add_tile(tileview, num_tiles, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    lv_obj_add_flag(tile, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(tile, tile_clicked, LV_EVENT_SHORT_CLICKED, (void *)(uintptr_t)type);
    num_tiles++;

    lv_obj_t *label = create_label(tile, title, &lv_font_montserrat_14, gray());
    lv_obj_set_style_text_letter_space(label, 2, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 26);
    return tile;
}

static lv_obj_t *create_ring(lv_obj_t *parent, lv_color_t color)
{
    lv_obj_t *ring = lv_arc_create(parent);
    lv_obj_set_size(ring, RING_SIZE, RING_SIZE);
    lv_arc_set_rotation(ring, 270);
    lv_arc_set_bg_angles(ring, 0, 360);
    lv_obj_remove_style(ring, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ring, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(ring, RING_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ring, RING_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ring, lv_color_hex(0x24243a), LV_PART_MAIN);
    lv_obj_set_style_arc_color(ring, color, LV_PART_INDICATOR);
    lv_obj_align(ring, LV_ALIGN_CENTER, 0, 8);
    return ring;
}

/* A big number with its unit after it, centered as one. */
static lv_obj_t *create_value_row(lv_obj_t *parent, lv_obj_t **value, lv_obj_t **unit, const char *unit_text,
                                  const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_column(row, 4, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    *value = create_label(row, "", font, color);
    lv_obj_t *unit_label = create_label(row, unit_text, &lv_font_montserrat_16, gray());
    lv_obj_set_style_pad_bottom(unit_label, 6, 0);
    if (unit) {
        *unit = unit_label;
    }
    return row;
}

static void build_weather(void)
{
    lv_obj_t *tile = add_tile(TILE_WEATHER, "WEATHER");

    // The weather icons are small single color images, scaled up and tinted.
    weather.icon = lv_image_create(tile);
    lv_obj_set_size(weather.icon, 52, 52);
    lv_image_set_inner_align(weather.icon, LV_IMAGE_ALIGN_CONTAIN);
    lv_obj_set_style_image_recolor_opa(weather.icon, LV_OPA_COVER, 0);
    lv_obj_align(weather.icon, LV_ALIGN_CENTER, 0, -36);

    lv_obj_t *row = create_value_row(tile, &weather.temperature, &weather.unit, "°C", &ui_font_aliean_47,
                                     lv_color_white());
    lv_obj_align(row, LV_ALIGN_CENTER, 0, 24);

    weather.text = create_label(tile, "", &lv_font_montserrat_14_full, gray());
    lv_obj_set_width(weather.text, 170);
    lv_label_set_long_mode(weather.text, LV_LABEL_LONG_DOT);
    lv_obj_align(weather.text, LV_ALIGN_CENTER, 0, 66);
}

static void build_activity(void)
{
    lv_obj_t *tile = add_tile(TILE_ACTIVITY, "ACTIVITY");

    activity.ring = create_ring(tile, lv_color_hex(0x4CD964));
    activity.steps = create_label(tile, "", &ui_font_aliean_25, lv_color_white());
    lv_obj_align(activity.steps, LV_ALIGN_CENTER, 0, 0);
    activity.goal = create_label(tile, "", &lv_font_montserrat_12, gray());
    lv_obj_align(activity.goal, LV_ALIGN_CENTER, 0, 26);
}

static void build_heart_rate(void)
{
    lv_obj_t *tile = add_tile(TILE_HEART_RATE, "HEART RATE");

    lv_obj_t *row = create_value_row(tile, &hr.bpm, NULL, "bpm", &ui_font_aliean_47, lv_color_hex(0xFF5A5F));
    lv_obj_align(row, LV_ALIGN_CENTER, 0, -18);

    hr.age = create_label(tile, "", &lv_font_montserrat_12, gray());
    lv_obj_align(hr.age, LV_ALIGN_CENTER, 0, 18);

    hr.chart = lv_chart_create(tile);
    lv_obj_set_size(hr.chart, 150, 42);
    lv_obj_align(hr.chart, LV_ALIGN_CENTER, 0, 56);
    lv_chart_set_type(hr.chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(hr.chart, TILES_HR_POINTS);
    lv_chart_set_div_line_count(hr.chart, 0, 0);
    lv_obj_set_style_bg_opa(hr.chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hr.chart, 0, 0);
    lv_obj_set_style_pad_all(hr.chart, 0, 0);
    lv_obj_set_style_size(hr.chart, 0, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_width(hr.chart, 2, LV_PART_ITEMS);
    lv_obj_clear_flag(hr.chart, LV_OBJ_FLAG_CLICKABLE);
    hr.series = lv_chart_add_series(hr.chart, lv_color_hex(0xFF5A5F), LV_CHART_AXIS_PRIMARY_Y);
}

static void music_button_clicked(lv_event_t *e)
{
    music_cb((tiles_music_action_t)(uintptr_t)lv_event_get_user_data(e));
}

static lv_obj_t *create_music_button(lv_obj_t *parent, const char *symbol, tiles_music_action_t action, int x,
                                     int size)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, size, size);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn, action == TILES_MUSIC_PLAY_PAUSE ? zsw_color_blue() : lv_color_hex(0x2a2a40), 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_align(btn, LV_ALIGN_CENTER, x, 46);
    lv_obj_add_event_cb(btn, music_button_clicked, LV_EVENT_CLICKED, (void *)(uintptr_t)action);
    // Focusing a button would scroll the tileview to its tile.
    lv_group_remove_obj(btn);

    lv_obj_t *label = create_label(btn, symbol, &lv_font_montserrat_16,
                                   action == TILES_MUSIC_PLAY_PAUSE ? lv_color_black() : lv_color_white());
    lv_obj_center(label);
    return label;
}

static void build_music(void)
{
    lv_obj_t *tile = add_tile(TILE_MUSIC, "MUSIC");

    music.track = create_label(tile, "", &lv_font_montserrat_14_full, lv_color_white());
    lv_obj_set_width(music.track, 176);
    lv_label_set_long_mode(music.track, LV_LABEL_LONG_DOT);
    lv_obj_set_height(music.track, 2 * lv_font_get_line_height(&lv_font_montserrat_14_full));
    lv_obj_align(music.track, LV_ALIGN_CENTER, 0, -30);

    music.artist = create_label(tile, "", &lv_font_montserrat_14_full, gray());
    lv_obj_set_size(music.artist, 160, lv_font_get_line_height(&lv_font_montserrat_14_full));
    lv_label_set_long_mode(music.artist, LV_LABEL_LONG_DOT);
    lv_obj_align(music.artist, LV_ALIGN_CENTER, 0, 2);

    create_music_button(tile, LV_SYMBOL_PREV, TILES_MUSIC_PREVIOUS, -56, 40);
    music.play_pause = create_music_button(tile, LV_SYMBOL_PLAY, TILES_MUSIC_PLAY_PAUSE, 0, 52);
    create_music_button(tile, LV_SYMBOL_NEXT, TILES_MUSIC_NEXT, 56, 40);
}

static void build_battery(void)
{
    lv_obj_t *tile = add_tile(TILE_BATTERY, "BATTERY");

    battery.ring = create_ring(tile, lv_color_hex(0x4CD964));
    lv_arc_set_range(battery.ring, 0, 100);
    lv_obj_t *row = create_value_row(tile, &battery.percent, NULL, "%", &ui_font_aliean_47, lv_color_white());
    lv_obj_align(row, LV_ALIGN_CENTER, 0, 0);
    battery.status = create_label(tile, "", &lv_font_montserrat_12, gray());
    lv_obj_set_width(battery.status, 120);
    lv_obj_align(battery.status, LV_ALIGN_CENTER, 0, 38);
}

static void update_dots(void)
{
    lv_obj_t *tile = lv_tileview_get_tile_active(tileview);
    // None is active until the first layout, then it is the first tile.
    int32_t active = tile ? lv_obj_get_index(tile) : 0;

    for (int i = 0; i < num_tiles; i++) {
        lv_obj_set_style_bg_color(dots[i], i == active ? lv_color_white() : lv_color_hex(0x4a4a5a), 0);
    }
}

static void tile_changed(lv_event_t *e)
{
    LV_UNUSED(e);
    update_dots();
}

static void format_duration(char *buf, size_t size, int minutes)
{
    if (minutes >= 24 * 60) {
        snprintf(buf, size, "%dd %dh", minutes / (24 * 60), (minutes / 60) % 24);
    } else if (minutes >= 60) {
        snprintf(buf, size, "%dh %dm", minutes / 60, minutes % 60);
    } else {
        snprintf(buf, size, "%dm", minutes);
    }
}

void tiles_ui_update(const tiles_data_t *data)
{
    char buf[48];

    if (!root_page) {
        return;
    }

    if (data->weather_valid) {
        lv_color_t color;
        lv_image_set_src(weather.icon, zsw_ui_utils_icon_from_weather_code(data->weather_code, &color));
        lv_obj_set_style_image_recolor(weather.icon, lv_color_lighten(color, LV_OPA_50), 0);
        lv_obj_remove_flag(weather.icon, LV_OBJ_FLAG_HIDDEN);
        snprintf(buf, sizeof(buf), "%d", data->temperature_c);
        set_number(weather.temperature, &ui_font_aliean_47, buf);
        lv_label_set_text(weather.text, data->weather_text);
        lv_obj_remove_flag(weather.unit, LV_OBJ_FLAG_HIDDEN);
    } else {
        set_number(weather.temperature, &ui_font_aliean_47, "--");
        lv_obj_add_flag(weather.icon, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(weather.text, "Open Weather to update");
        lv_obj_add_flag(weather.unit, LV_OBJ_FLAG_HIDDEN);
    }

    lv_arc_set_range(activity.ring, 0, data->step_goal);
    lv_arc_set_value(activity.ring, MIN(data->steps, data->step_goal));
    snprintf(buf, sizeof(buf), "%u", data->steps);
    set_number(activity.steps, &ui_font_aliean_25, buf);
    lv_label_set_text_fmt(activity.goal, "of %u steps", data->step_goal);

    if (hr.bpm) {
        if (data->hr_bpm > 0) {
            snprintf(buf, sizeof(buf), "%d", data->hr_bpm);
            set_number(hr.bpm, &ui_font_aliean_47, buf);
            if (data->hr_age_s < 60) {
                lv_label_set_text(hr.age, "Just now");
            } else {
                format_duration(buf, sizeof(buf), data->hr_age_s / 60);
                lv_label_set_text_fmt(hr.age, "%s ago", buf);
            }
        } else {
            set_number(hr.bpm, &ui_font_aliean_47, "--");
            lv_label_set_text(hr.age, "No recent measurement");
        }
        int16_t min = INT16_MAX;
        int16_t max = 0;
        for (int i = 0; i < data->hr_num_points; i++) {
            min = MIN(min, data->hr_points[i]);
            max = MAX(max, data->hr_points[i]);
        }
        lv_chart_set_all_values(hr.chart, hr.series, LV_CHART_POINT_NONE);
        lv_chart_set_point_count(hr.chart, MAX(data->hr_num_points, 2));
        lv_chart_set_axis_range(hr.chart, LV_CHART_AXIS_PRIMARY_Y, MAX(min - 5, 0), max + 5);
        for (int i = 0; i < data->hr_num_points; i++) {
            lv_chart_set_next_value(hr.chart, hr.series, data->hr_points[i]);
        }
    }

    if (data->music_valid) {
        lv_label_set_text(music.track, data->track);
        lv_label_set_text(music.artist, data->artist);
    } else {
        lv_label_set_text(music.track, "Nothing playing");
        lv_label_set_text(music.artist, "");
    }
    lv_label_set_text(music.play_pause, data->music_playing ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);

    int percent = CLAMP(data->battery_percent, 0, 100);
    lv_arc_set_value(battery.ring, percent);
    lv_obj_set_style_arc_color(battery.ring, percent > 50 ? lv_color_hex(0x4CD964) :
                               percent > 20 ? lv_color_hex(0xFFB020) : lv_color_hex(0xFF5A5F), LV_PART_INDICATOR);
    snprintf(buf, sizeof(buf), "%d", percent);
    set_number(battery.percent, &ui_font_aliean_47, buf);
    if (data->battery_minutes_left > 0) {
        format_duration(buf, sizeof(buf), data->battery_minutes_left);
        lv_label_set_text_fmt(battery.status, data->battery_charging ? LV_SYMBOL_CHARGE " %s to full" : "%s left", buf);
    } else if (data->battery_charging) {
        lv_label_set_text(battery.status, LV_SYMBOL_CHARGE " Charging");
    } else {
        lv_label_set_text_fmt(battery.status, "%d mV", data->battery_mv);
    }
}

void tiles_ui_show(lv_obj_t *root, lv_group_t *group, tiles_open_cb_t on_open, tiles_music_cb_t on_music,
                   const tiles_data_t *data)
{
    LV_UNUSED(group);
    open_cb = on_open;
    music_cb = on_music;
    num_tiles = 0;
    memset(&hr, 0, sizeof(hr));

    root_page = lv_obj_create(root);
    lv_obj_set_size(root_page, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root_page, lv_color_hex(0x0C0C1A), 0);
    lv_obj_set_style_bg_opa(root_page, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(root_page, 0, 0);
    lv_obj_set_style_pad_all(root_page, 0, 0);
    lv_obj_set_style_radius(root_page, 0, 0);
    lv_obj_clear_flag(root_page, LV_OBJ_FLAG_SCROLLABLE);

    tileview = lv_tileview_create(root_page);
    lv_obj_set_size(tileview, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(tileview, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_event_cb(tileview, tile_changed, LV_EVENT_VALUE_CHANGED, NULL);

    build_weather();
    build_activity();
    if (data->hr_available) {
        build_heart_rate();
    }
    build_music();
    build_battery();

    lv_obj_t *dot_row = lv_obj_create(root_page);
    lv_obj_set_size(dot_row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(dot_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(dot_row, 0, 0);
    lv_obj_set_style_pad_all(dot_row, 0, 0);
    lv_obj_set_style_pad_column(dot_row, 6, 0);
    lv_obj_set_flex_flow(dot_row, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(dot_row, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(dot_row, LV_ALIGN_BOTTOM_MID, 0, -16);
    for (int i = 0; i < num_tiles; i++) {
        dots[i] = lv_obj_create(dot_row);
        lv_obj_set_size(dots[i], DOT_SIZE, DOT_SIZE);
        lv_obj_set_style_radius(dots[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(dots[i], 0, 0);
        lv_obj_clear_flag(dots[i], LV_OBJ_FLAG_CLICKABLE);
    }

    lv_tileview_set_tile_by_index(tileview, 0, 0, LV_ANIM_OFF);
    tiles_ui_update(data);
    update_dots();
}

void tiles_ui_remove(void)
{
    if (root_page) {
        lv_obj_del(root_page);
        root_page = NULL;
    }
}
