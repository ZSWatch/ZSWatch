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

#include <lvgl.h>
#include <zephyr/logging/log.h>

#include "zsw_recording_overlay.h"
#include "managers/zsw_recording_manager.h"
#include "ui/zsw_ui.h"

LOG_MODULE_REGISTER(zsw_recording_overlay, LOG_LEVEL_INF);

#define OVERLAY_UPDATE_MS  200
#define DISP_WIDTH         240

static lv_obj_t *overlay_container;
static lv_obj_t *red_dot;
static lv_obj_t *time_label;
static lv_obj_t *level_bar;
static lv_timer_t *update_timer;
static lv_timer_t *blink_timer;
static bool dot_visible;

static void blink_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    dot_visible = !dot_visible;
    lv_obj_set_style_opa(red_dot, dot_visible ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
}

static void overlay_tap_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (zsw_recording_manager_is_recording()) {
        zsw_recording_manager_stop();
    }
    zsw_recording_overlay_hide();
}

static void overlay_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);

    if (!zsw_recording_manager_is_recording()) {
        zsw_recording_overlay_hide();
        return;
    }

    uint32_t elapsed = zsw_recording_manager_get_elapsed_ms();
    uint32_t secs = elapsed / 1000;
    uint32_t mins = secs / 60;
    secs %= 60;
    lv_label_set_text_fmt(time_label, "%02u:%02u", mins, secs);

    uint8_t level = zsw_recording_manager_get_audio_level();
    lv_bar_set_value(level_bar, level, LV_ANIM_ON);
}

void zsw_recording_overlay_show(void)
{
    if (overlay_container) {
        return;
    }

    overlay_container = lv_obj_create(lv_layer_top());
    lv_obj_set_size(overlay_container, 120, 36);
    lv_obj_align(overlay_container, LV_ALIGN_TOP_MID, 0, 8);
    lv_obj_set_style_bg_color(overlay_container, lv_color_make(0x20, 0x20, 0x20), 0);
    lv_obj_set_style_bg_opa(overlay_container, LV_OPA_90, 0);
    lv_obj_set_style_radius(overlay_container, 18, 0);
    lv_obj_set_style_border_width(overlay_container, 0, 0);
    lv_obj_set_style_pad_all(overlay_container, 4, 0);
    lv_obj_set_flex_flow(overlay_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(overlay_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(overlay_container, 6, 0);

    /* Pulsing red dot */
    red_dot = lv_obj_create(overlay_container);
    lv_obj_set_size(red_dot, 12, 12);
    lv_obj_set_style_radius(red_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(red_dot, zsw_color_red(), 0);
    lv_obj_set_style_bg_opa(red_dot, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(red_dot, 0, 0);

    dot_visible = true;
    blink_timer = lv_timer_create(blink_timer_cb, 500, NULL);

    /* Elapsed time */
    time_label = lv_label_create(overlay_container);
    lv_label_set_text(time_label, "00:00");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);

    /* Audio level bar */
    level_bar = lv_bar_create(overlay_container);
    lv_obj_set_size(level_bar, 30, 6);
    lv_bar_set_range(level_bar, 0, 100);
    lv_bar_set_value(level_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(level_bar, zsw_color_dark_gray(), 0);
    lv_obj_set_style_bg_color(level_bar, zsw_color_blue(), LV_PART_INDICATOR);

    /* Tap to stop */
    lv_obj_add_flag(overlay_container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(overlay_container, overlay_tap_cb, LV_EVENT_CLICKED, NULL);

    /* Periodic update timer */
    update_timer = lv_timer_create(overlay_timer_cb, OVERLAY_UPDATE_MS, NULL);

    LOG_INF("Recording overlay shown");
}

void zsw_recording_overlay_hide(void)
{
    if (update_timer) {
        lv_timer_delete(update_timer);
        update_timer = NULL;
    }

    if (blink_timer) {
        lv_timer_delete(blink_timer);
        blink_timer = NULL;
    }

    if (overlay_container) {
        lv_obj_del(overlay_container);
        overlay_container = NULL;
        red_dot = NULL;
        time_label = NULL;
        level_bar = NULL;
    }

    LOG_INF("Recording overlay hidden");
}
