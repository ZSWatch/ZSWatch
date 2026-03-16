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
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include <string.h>
#include <stdio.h>

#include "zsw_voice_memo_popup.h"
#include "events/zsw_voice_memo_event.h"
#include "managers/zsw_recording_manager.h"
#include "managers/zsw_power_manager.h"
#include "ble/gadgetbridge/ble_gadgetbridge.h"
#include "ui/zsw_ui.h"

LOG_MODULE_REGISTER(zsw_voice_memo_popup, LOG_LEVEL_INF);

#define DISP_WIDTH   240
#define DISP_HEIGHT  240
#define AUTO_DISMISS_MS 20000

ZBUS_CHAN_DECLARE(voice_memo_result_chan);

static lv_obj_t *popup_container;
static lv_timer_t *dismiss_timer;
static char popup_filename[32];

static struct k_work show_popup_work;
static char pending_title[128];
static char pending_filename[32];
static char pending_action_type[20];
static char pending_datetime[32];

static void dismiss_popup(void)
{
    if (dismiss_timer) {
        lv_timer_delete(dismiss_timer);
        dismiss_timer = NULL;
    }
    if (popup_container) {
        lv_obj_del(popup_container);
        popup_container = NULL;
    }
}

static void dismiss_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    dismiss_popup();
}

static void undo_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    LOG_INF("Undo requested for: %s", popup_filename);
    zsw_recording_manager_delete(popup_filename);
    ble_gadgetbridge_send_voice_memo_undo(popup_filename);
    dismiss_popup();
}

static void show_popup_async(void *data)
{
    ARG_UNUSED(data);

    dismiss_popup();

    strncpy(popup_filename, pending_filename, sizeof(popup_filename) - 1);
    popup_filename[sizeof(popup_filename) - 1] = '\0';

    popup_container = lv_obj_create(lv_layer_top());
    lv_obj_set_size(popup_container, DISP_WIDTH, DISP_HEIGHT);
    lv_obj_set_style_bg_color(popup_container, lv_color_make(0x1a, 0x1a, 0x2e), 0);
    lv_obj_set_style_bg_opa(popup_container, LV_OPA_90, 0);
    lv_obj_set_style_border_width(popup_container, 0, 0);
    lv_obj_set_style_radius(popup_container, 0, 0);
    lv_obj_set_style_pad_all(popup_container, 0, 0);
    lv_obj_center(popup_container);

    /* Determine action type label and icon */
    const char *type_icon = LV_SYMBOL_OK;
    const char *type_label = "Saved";
    lv_color_t type_color = lv_color_make(0x4c, 0xaf, 0x50); /* green */

    if (strncmp(pending_action_type, "calendar_event", sizeof(pending_action_type)) == 0) {
        type_icon = LV_SYMBOL_LIST;
        type_label = "Calendar Event";
        type_color = zsw_color_blue();
    } else if (strncmp(pending_action_type, "task", sizeof(pending_action_type)) == 0) {
        type_icon = LV_SYMBOL_OK;
        type_label = "Task";
        type_color = lv_color_make(0xff, 0x98, 0x00); /* orange */
    } else if (strncmp(pending_action_type, "reminder", sizeof(pending_action_type)) == 0) {
        type_icon = LV_SYMBOL_BELL;
        type_label = "Reminder";
        type_color = lv_color_make(0xab, 0x47, 0xbc); /* purple */
    }

    /* Type icon */
    lv_obj_t *icon = lv_label_create(popup_container);
    lv_label_set_text(icon, type_icon);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(icon, type_color, 0);
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 30);

    /* Type label (e.g. "Calendar Event") */
    lv_obj_t *header = lv_label_create(popup_container);
    lv_label_set_text(header, type_label);
    lv_obj_set_style_text_font(header, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(header, type_color, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 65);

    /* Title — the main action text, large and white */
    lv_obj_t *title_lbl = lv_label_create(popup_container);
    lv_label_set_text(title_lbl, pending_title);
    lv_obj_set_style_text_font(title_lbl, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(title_lbl, lv_color_white(), 0);
    lv_obj_set_style_text_align(title_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(title_lbl, DISP_WIDTH - 50);
    lv_label_set_long_mode(title_lbl, LV_LABEL_LONG_WRAP);
    lv_obj_align(title_lbl, LV_ALIGN_TOP_MID, 0, 90);

    /* Datetime line — if available, show below title */
    if (pending_datetime[0] != '\0') {
        /* Format: try to show "HH:MM" from ISO datetime like "2025-03-15T14:30:00" */
        char time_buf[64];
        const char *t_sep = strchr(pending_datetime, 'T');
        if (t_sep && strlen(t_sep) >= 6) {
            /* Show date and time: "Mar 15, 14:30" style */
            int year, month, day, hour, min;
            if (sscanf(pending_datetime, "%d-%d-%dT%d:%d", &year, &month, &day, &hour, &min) == 5) {
                static const char *months[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
                const char *mon_str = (month >= 1 && month <= 12) ? months[month] : "???";
                snprintf(time_buf, sizeof(time_buf), "%s %d, %02d:%02d", mon_str, day, hour, min);
            } else {
                snprintf(time_buf, sizeof(time_buf), "%s", pending_datetime);
            }
        } else {
            snprintf(time_buf, sizeof(time_buf), "%s", pending_datetime);
        }

        lv_obj_t *dt_lbl = lv_label_create(popup_container);
        lv_label_set_text(dt_lbl, time_buf);
        lv_obj_set_style_text_font(dt_lbl, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(dt_lbl, lv_color_make(0xbb, 0xbb, 0xbb), 0);
        lv_obj_set_style_text_align(dt_lbl, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_width(dt_lbl, DISP_WIDTH - 40);
        lv_obj_align(dt_lbl, LV_ALIGN_CENTER, 0, 20);
    }

    /* Undo button */
    lv_obj_t *undo_btn = lv_btn_create(popup_container);
    lv_obj_set_size(undo_btn, 120, 44);
    lv_obj_set_style_bg_color(undo_btn, zsw_color_red(), 0);
    lv_obj_set_style_radius(undo_btn, 22, 0);
    lv_obj_align(undo_btn, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_obj_add_event_cb(undo_btn, undo_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *undo_label = lv_label_create(undo_btn);
    lv_label_set_text(undo_label, "Undo");
    lv_obj_set_style_text_font(undo_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(undo_label, lv_color_white(), 0);
    lv_obj_center(undo_label);

    dismiss_timer = lv_timer_create(dismiss_timer_cb, AUTO_DISMISS_MS, NULL);
    lv_timer_set_repeat_count(dismiss_timer, 1);
}

static void show_popup_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    zsw_power_manager_reset_idle_timout();
    lv_async_call(show_popup_async, NULL);
}

static void on_voice_memo_result(const struct zbus_channel *chan)
{
    const struct zsw_voice_memo_result_event *evt = zbus_chan_const_msg(chan);

    strncpy(pending_title, evt->title, sizeof(pending_title) - 1);
    pending_title[sizeof(pending_title) - 1] = '\0';
    strncpy(pending_filename, evt->filename, sizeof(pending_filename) - 1);
    pending_filename[sizeof(pending_filename) - 1] = '\0';
    strncpy(pending_action_type, evt->action_type, sizeof(pending_action_type) - 1);
    pending_action_type[sizeof(pending_action_type) - 1] = '\0';
    strncpy(pending_datetime, evt->datetime, sizeof(pending_datetime) - 1);
    pending_datetime[sizeof(pending_datetime) - 1] = '\0';

    k_work_submit(&show_popup_work);
}

ZBUS_LISTENER_DEFINE(voice_memo_popup_result_lis, on_voice_memo_result);

void zsw_voice_memo_popup_init(void)
{
    k_work_init(&show_popup_work, show_popup_work_fn);
    LOG_INF("Voice memo popup initialized");
}
