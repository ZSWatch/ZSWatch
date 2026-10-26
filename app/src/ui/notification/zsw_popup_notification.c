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
#include <string.h>

#include "ui/notification/zsw_popup_notification.h"
#include "ui/notification/zsw_notification_reply.h"
#include "ui/zsw_overlay_manager.h"
#include "ui/zsw_ui.h"
#include "ui/utils/zsw_ui_utils.h"

#define EXTENDED_TIMEOUT_MS 20000
#define BODY_TOP            70
#define BUTTON_HEIGHT       38
#define BUTTON_BOTTOM       -32
#define BUTTON_ROW_SPACE    78

static void popup_show(lv_obj_t *parent, lv_group_t *group);
static void popup_close(void);
static bool popup_back(void);

static on_close_notif_cb_t on_close_cb;
static bool dismissed;
// Copied, as the popup may wait behind another overlay.
static zsw_not_mngr_notification_t notif;
static lv_obj_t *panel;
static lv_obj_t *countdown;

static zsw_overlay_t overlay = {
    .name = "Notification",
    .priority = 10,
    .blocks_input = true,
    .swipe_is_back = true,
    .show = popup_show,
    .close = popup_close,
    .back = popup_back,
};

LV_FONT_DECLARE(lv_font_montserrat_14_full)
ZSW_LV_IMG_DECLARE(mic_icon);

void zsw_notification_popup_show(const zsw_not_mngr_notification_t *notification, on_close_notif_cb_t close_cb,
                                 uint32_t close_after_seconds)
{
    notif = *notification;
    on_close_cb = close_cb;
    overlay.timeout_ms = close_after_seconds * 1000;
    zsw_overlay_show(&overlay);
}

static void countdown_anim_cb(void *arc, int32_t value)
{
    lv_arc_set_value(arc, value);
}

/* A ring around the edge drains until the popup closes by itself. */
static void start_countdown(uint32_t timeout_ms)
{
    if (!countdown) {
        return;
    }
    lv_anim_delete(countdown, countdown_anim_cb);
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, countdown);
    lv_anim_set_exec_cb(&anim, countdown_anim_cb);
    lv_anim_set_values(&anim, 1000, 0);
    lv_anim_set_duration(&anim, timeout_ms);
    lv_anim_start(&anim);
}

static void extend_timeout(void)
{
    if (overlay.timeout_ms > 0) {
        zsw_overlay_set_timeout(&overlay, EXTENDED_TIMEOUT_MS);
        start_countdown(EXTENDED_TIMEOUT_MS);
    }
}

static void on_pressed(lv_event_t *e)
{
    LV_UNUSED(e);
    extend_timeout();
}

static void on_dismiss_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    zsw_notification_popup_remove();
}

static void on_reply_clicked(lv_event_t *e)
{
    LV_UNUSED(e);
    // Replaces this popup, the notification stays until the phone removes it.
    zsw_notification_reply_start(notif.id);
}

static lv_obj_t *create_button(lv_obj_t *parent, const void *icon, const char *text, lv_color_t color,
                               lv_color_t text_color, lv_event_cb_t cb, int x, int width)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, width, BUTTON_HEIGHT);
    lv_obj_set_style_radius(btn, BUTTON_HEIGHT / 2, 0);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_pad_column(btn, 4, 0);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_ext_click_area(btn, 6);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, x, BUTTON_BOTTOM);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(lv_group_get_default(), btn);

    if (icon) {
        zsw_ui_utils_create_icon(btn, icon, 20, text_color);
    }
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, text_color, 0);
    return btn;
}

static lv_obj_t *create_text(lv_obj_t *parent, const char *text, lv_color_t color, lv_label_long_mode_t mode,
                             int width)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_width(label, width);
    lv_label_set_long_mode(label, mode);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14_full, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_base_dir(label, LV_BASE_DIR_AUTO, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    return label;
}

static void popup_show(lv_obj_t *parent, lv_group_t *group)
{
    // Created on the screen instead of parent (lv_layer_top), so swipes on it
    // still reach the screen gesture handlers.
    LV_UNUSED(parent);
    LV_UNUSED(group);
    const char *sender = notif.sender[0] != '\0' ? notif.sender : notif.title;

    dismissed = false;

    panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_center(panel);
    // Opaque and square, so LVGL skips redrawing the screen below.
    lv_obj_set_style_radius(panel, 0, 0);
    lv_obj_set_style_bg_color(panel, lv_color_hex(0x0C0C1A), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(panel, on_pressed, LV_EVENT_PRESSED, NULL);

    if (overlay.timeout_ms > 0) {
        countdown = lv_arc_create(panel);
        lv_obj_set_size(countdown, lv_disp_get_hor_res(NULL) - 4, lv_disp_get_ver_res(NULL) - 4);
        lv_obj_center(countdown);
        lv_arc_set_rotation(countdown, 270);
        lv_arc_set_bg_angles(countdown, 0, 360);
        lv_arc_set_range(countdown, 0, 1000);
        lv_arc_set_value(countdown, 1000);
        lv_obj_remove_style(countdown, NULL, LV_PART_KNOB);
        lv_obj_clear_flag(countdown, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_arc_width(countdown, 3, LV_PART_MAIN);
        lv_obj_set_style_arc_width(countdown, 3, LV_PART_INDICATOR);
        lv_obj_set_style_arc_opa(countdown, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_arc_color(countdown, zsw_color_blue(), LV_PART_INDICATOR);
        start_countdown(overlay.timeout_ms);
    }

    // App icon and name, the same line.
    lv_obj_t *source = lv_obj_create(panel);
    lv_obj_set_size(source, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(source, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(source, 0, 0);
    lv_obj_set_style_pad_all(source, 0, 0);
    lv_obj_set_style_pad_column(source, 6, 0);
    lv_obj_set_flex_flow(source, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(source, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(source, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(source, LV_ALIGN_TOP_MID, 0, 22);

    lv_obj_t *icon = lv_img_create(source);
    lv_img_set_src(icon, zsw_ui_utils_icon_from_notification(notif.src));
    lv_obj_t *app = lv_label_create(source);
    lv_label_set_text(app, notif.src == NOTIFICATION_SRC_NONE && notif.title[0] != '\0' ? notif.title :
                      zsw_ui_utils_source_from_notification(notif.src));
    lv_obj_set_style_text_font(app, &lv_font_montserrat_14_full, 0);
    lv_obj_set_style_text_color(app, lv_palette_main(LV_PALETTE_GREY), 0);

    lv_obj_t *title = create_text(panel, sender, zsw_color_blue(), LV_LABEL_LONG_DOT, 176);
    lv_obj_set_height(title, lv_font_get_line_height(&lv_font_montserrat_14_full));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 44);

    // The body scrolls when it does not fit, sideways swipes still dismiss.
    lv_obj_t *body_scroll = lv_obj_create(panel);
    lv_obj_set_size(body_scroll, 196, lv_disp_get_ver_res(NULL) - BODY_TOP - BUTTON_ROW_SPACE);
    lv_obj_set_style_bg_opa(body_scroll, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(body_scroll, 0, 0);
    lv_obj_set_style_pad_all(body_scroll, 0, 0);
    lv_obj_set_scroll_dir(body_scroll, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(body_scroll, LV_SCROLLBAR_MODE_OFF);
    lv_obj_align(body_scroll, LV_ALIGN_TOP_MID, 0, BODY_TOP);
    lv_obj_add_event_cb(body_scroll, on_pressed, LV_EVENT_PRESSED, NULL);

    lv_obj_t *body = create_text(body_scroll, notif.body, lv_color_white(), LV_LABEL_LONG_WRAP, 196);
    lv_obj_set_style_pad_bottom(body, 8, 0);

    lv_color_t neutral = lv_color_make(0x3a, 0x3a, 0x4e);
    if (notif.can_reply && IS_ENABLED(CONFIG_ZSW_NOTIFICATION_REPLY)) {
        create_button(panel, NULL, LV_SYMBOL_CLOSE, neutral, lv_color_white(), on_dismiss_clicked, -44, BUTTON_HEIGHT);
        lv_obj_t *reply = create_button(panel, ZSW_LV_IMG_USE(mic_icon), "Reply", zsw_color_blue(), lv_color_black(),
                                        on_reply_clicked, 24, 92);
        lv_group_focus_obj(reply);
    } else {
        create_button(panel, NULL, LV_SYMBOL_CLOSE, neutral, lv_color_white(), on_dismiss_clicked, 0, BUTTON_HEIGHT);
    }
}

void zsw_notification_popup_remove(void)
{
    dismissed = true;
    zsw_overlay_close(&overlay);
}

bool zsw_notification_popup_is_shown(void)
{
    return zsw_overlay_is_shown(&overlay);
}

static bool popup_back(void)
{
    zsw_notification_popup_remove();
    return true;
}

static void popup_close(void)
{
    if (countdown) {
        lv_anim_delete(countdown, countdown_anim_cb);
        countdown = NULL;
    }
    if (panel) {
        lv_obj_del(panel);
        panel = NULL;
    }

    // Only a dismissal removes the notification, a timeout keeps it.
    if (dismissed) {
        on_close_cb(notif.id);
    }
}
