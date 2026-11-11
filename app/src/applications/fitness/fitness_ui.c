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

#include "fitness_ui.h"
#include "ui/zsw_ui.h"
#include <lvgl.h>
#include <assert.h>
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

#define FITNESS_COLOR       lv_color_hex(0x22C55E)
#define STEP_GOAL           10000
#define NUM_BARS            7
#define BAR_WIDTH           12
#define BAR_PITCH           22
#define BAR_MAX_HEIGHT      44
#define BAR_MIN_HEIGHT      4
#define BAR_ANIM_STAGGER_MS 30

static void on_bar_clicked(lv_event_t *e);
static void show_day(int day);

static lv_obj_t *root_page = NULL;
static lv_obj_t *steps_label;
static lv_obj_t *caption_label;
static lv_obj_t *goal_ring;
static lv_obj_t *bars[NUM_BARS];
static uint32_t day_steps[NUM_BARS];
static char **day_names;
static int num_days;
static int shown_day;

static int32_t bar_height(uint32_t steps)
{
    return BAR_MIN_HEIGHT + (LV_MIN(steps, STEP_GOAL) * (BAR_MAX_HEIGHT - BAR_MIN_HEIGHT)) / STEP_GOAL;
}

static void bar_anim_exec(void *bar, int32_t value)
{
    lv_obj_set_height(bar, value);
}

static void create_week_bars(lv_obj_t *parent)
{
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_remove_style_all(container);
    lv_obj_set_size(container, NUM_BARS * BAR_PITCH, BAR_MAX_HEIGHT + 18);
    lv_obj_align(container, LV_ALIGN_TOP_MID, 0, 128);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < NUM_BARS; i++) {
        bars[i] = lv_obj_create(container);
        lv_obj_remove_style_all(bars[i]);
        lv_obj_set_size(bars[i], BAR_WIDTH, BAR_MIN_HEIGHT);
        lv_obj_align(bars[i], LV_ALIGN_BOTTOM_LEFT, i * BAR_PITCH + (BAR_PITCH - BAR_WIDTH) / 2, -18);
        lv_obj_set_style_radius(bars[i], BAR_WIDTH / 2, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(bars[i], LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_bg_color(bars[i], ZSW_COLOR_SURFACE_HIGH, LV_PART_MAIN);
        // The bars are narrow: make them easier to hit
        lv_obj_set_ext_click_area(bars[i], (BAR_PITCH - BAR_WIDTH) / 2);
        lv_obj_add_flag(bars[i], LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(bars[i], on_bar_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        lv_obj_t *day = lv_label_create(container);
        lv_label_set_text(day, "");
        lv_obj_set_style_text_font(day, ZSW_FONT_CAPTION, LV_PART_MAIN);
        lv_obj_set_style_text_color(day, ZSW_COLOR_TEXT_DIM, LV_PART_MAIN);
        lv_obj_set_width(day, BAR_PITCH);
        lv_obj_set_style_text_align(day, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_obj_align(day, LV_ALIGN_BOTTOM_LEFT, i * BAR_PITCH, 0);
    }
}

static void update_bar_colors(void)
{
    for (int i = 0; i < num_days; i++) {
        lv_color_t color;

        if (i == shown_day) {
            color = FITNESS_COLOR;
        } else if (day_steps[i] >= STEP_GOAL) {
            color = lv_color_mix(FITNESS_COLOR, ZSW_COLOR_BG, LV_OPA_50);
        } else {
            color = ZSW_COLOR_SURFACE_HIGH;
        }
        lv_obj_set_style_bg_color(bars[i], color, LV_PART_MAIN);
    }
}

// Shows the steps of one day in the large number. The last day is today.
static void show_day(int day)
{
    shown_day = day;

    lv_label_set_text_fmt(steps_label, "%d", (int)day_steps[day]);
    if (day == num_days - 1) {
        lv_label_set_text_fmt(caption_label, "of %d steps today", STEP_GOAL);
    } else {
        lv_label_set_text_fmt(caption_label, "steps on %s", day_names[day]);
    }
    zsw_ui_ring_set_value(goal_ring, (LV_MIN(day_steps[day], STEP_GOAL) * ZSW_UI_RING_MAX) / STEP_GOAL, true);
    update_bar_colors();
}

static void on_bar_clicked(lv_event_t *e)
{
    int day = (int)(intptr_t)lv_event_get_user_data(e);

    if (day < num_days) {
        // A second tap on the same bar goes back to today
        show_day(day == shown_day ? num_days - 1 : day);
    }
}

void fitness_ui_show(lv_obj_t *root, uint16_t max_samples)
{
    assert(root_page == NULL);
    LV_UNUSED(max_samples);

    num_days = 0;
    shown_day = 0;

    root_page = zsw_ui_screen_create(root, FITNESS_COLOR);
    goal_ring = zsw_ui_ring_create(root_page, FITNESS_COLOR);
    zsw_ui_title_create(root_page, "STEPS", ZSW_COLOR_TEXT_DIM);

    steps_label = lv_label_create(root_page);
    lv_label_set_text(steps_label, "-");
    lv_obj_set_style_text_font(steps_label, ZSW_FONT_NUMBER, LV_PART_MAIN);
    lv_obj_set_style_text_color(steps_label, ZSW_COLOR_TEXT, LV_PART_MAIN);
    lv_obj_align(steps_label, LV_ALIGN_TOP_MID, 0, 42);

    caption_label = lv_label_create(root_page);
    lv_label_set_text(caption_label, "");
    lv_obj_set_style_text_font(caption_label, ZSW_FONT_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_color(caption_label, ZSW_COLOR_TEXT_DIM, LV_PART_MAIN);
    lv_obj_align(caption_label, LV_ALIGN_TOP_MID, 0, 98);

    create_week_bars(root_page);
}

void fitness_ui_set_weekly_steps(uint16_t *samples, char **weekday_names, uint16_t num_samples)
{
    assert(root_page != NULL);

    day_names = weekday_names;
    num_days = LV_MIN(num_samples, NUM_BARS);

    for (int i = 0; i < num_days; i++) {
        day_steps[i] = samples[i];
        lv_label_set_text_fmt(lv_obj_get_sibling(bars[i], 1), "%.1s", weekday_names[i]);

        // The bars grow one after the other
        lv_anim_t anim;
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, bars[i]);
        lv_anim_set_exec_cb(&anim, bar_anim_exec);
        lv_anim_set_values(&anim, BAR_MIN_HEIGHT, bar_height(day_steps[i]));
        lv_anim_set_duration(&anim, 300);
        lv_anim_set_delay(&anim, i * BAR_ANIM_STAGGER_MS);
        lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
        lv_anim_start(&anim);
    }

    show_day(num_days - 1);
}

void fitness_ui_set_daily_steps(uint32_t steps)
{
    if (root_page == NULL || num_days == 0) {
        return;
    }

    int today = num_days - 1;

    day_steps[today] = steps;
    lv_anim_delete(bars[today], bar_anim_exec);
    lv_obj_set_height(bars[today], bar_height(steps));
    if (shown_day == today) {
        show_day(today);
    }
}

void fitness_ui_remove(void)
{
    lv_obj_del(root_page);
    root_page = NULL;
}
