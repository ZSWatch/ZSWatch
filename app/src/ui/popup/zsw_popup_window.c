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

#include <stdbool.h>
#include <string.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/spinlock.h>

#include "managers/zsw_power_manager.h"
#include "ui/popup/zsw_popup_window.h"
#include "ui/zsw_ui_theme.h"
#include "ui/zsw_ui_widgets.h"

LOG_MODULE_REGISTER(zsw_popup_window, LOG_LEVEL_INF);

#define POPUP_QUEUE_SIZE         8
#define POPUP_TITLE_MAX_LEN      64
#define POPUP_BODY_MAX_LEN       192

#define POPUP_COLOR              ZSW_COLOR_ACCENT
#define BUTTON_SIZE              44
#define BUTTON_BOTTOM            -22
#define COUNTDOWN_WIDTH          4
#define COUNTDOWN_STEPS          120

typedef struct popup_request {
    char title[POPUP_TITLE_MAX_LEN];
    char body[POPUP_BODY_MAX_LEN];
    on_close_popup_cb_t close_cb;
    uint32_t close_after_seconds;
    bool display_yes_no;
} popup_request_t;

static void on_popup_button_pressed(lv_event_t *e);
static void on_popup_close_button_pressed(lv_event_t *e);
static void close_popup_timer(lv_timer_t *timer);
static void finalize_current_popup(bool confirmed);
static void show_popup_request(popup_request_t *req);
static void copy_text_to_request(char *dst, size_t dst_size, const char *src, const char *name);
static int popup_queue_init(void);

static lv_obj_t *mbox;
static lv_obj_t *yes_btn;
static lv_obj_t *no_btn;
static lv_timer_t *auto_close_timer;
static bool popup_active;
static struct k_spinlock popup_lock;

static popup_request_t current_popup;

K_MSGQ_DEFINE(pending_popup_msgq, sizeof(popup_request_t), POPUP_QUEUE_SIZE, 4);

static int popup_queue_init(void)
{
    k_msgq_purge(&pending_popup_msgq);
    return 0;
}

SYS_INIT(popup_queue_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void copy_text_to_request(char *dst, size_t dst_size, const char *src, const char *name)
{
    size_t src_len;

    if (!src) {
        dst[0] = '\0';
        return;
    }

    src_len = strlen(src);
    if (src_len >= dst_size) {
        LOG_WRN("Popup %s truncated (%u >= %u)", name, (uint32_t)src_len, (uint32_t)dst_size);
    }

    strncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';
}

static void countdown_anim_exec(void *ring, int32_t value)
{
    // The value only changes a few times per second, in between this returns without redrawing.
    lv_arc_set_value(ring, value * ZSW_UI_RING_MAX / COUNTDOWN_STEPS);
}

/* A ring around the edge drains until the popup closes by itself. */
static void start_countdown(uint32_t seconds)
{
    lv_obj_t *ring = zsw_ui_ring_create(mbox, POPUP_COLOR);
    lv_obj_set_style_arc_width(ring, COUNTDOWN_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ring, COUNTDOWN_WIDTH, LV_PART_INDICATOR);
    lv_arc_set_value(ring, ZSW_UI_RING_MAX);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, ring);
    lv_anim_set_exec_cb(&anim, countdown_anim_exec);
    lv_anim_set_values(&anim, COUNTDOWN_STEPS, 0);
    lv_anim_set_duration(&anim, seconds * 1000);
    lv_anim_start(&anim);
}

static void show_popup_request(popup_request_t *req)
{
    __ASSERT_NO_MSG(req != NULL);

    lv_obj_t *close_btn = NULL;

    current_popup = *req;
    zsw_power_manager_reset_idle_timout();

    // A full screen sheet instead of a box: opaque, so nothing under it is drawn.
    mbox = zsw_ui_screen_create(lv_layer_top(), POPUP_COLOR);
    lv_obj_add_flag(mbox, LV_OBJ_FLAG_CLICKABLE);

    if (current_popup.close_after_seconds > 0) {
        start_countdown(current_popup.close_after_seconds);
    }

    lv_obj_t *title = zsw_ui_label_create(mbox, current_popup.title, ZSW_FONT_TITLE, ZSW_COLOR_TEXT);
    lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
    lv_obj_set_width(title, 140);
    lv_obj_set_height(title, lv_font_get_line_height(ZSW_FONT_TITLE));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

    // The body scrolls when it does not fit
    lv_obj_t *body_scroll = lv_obj_create(mbox);
    lv_obj_remove_style_all(body_scroll);
    lv_obj_set_size(body_scroll, 184, 100);
    lv_obj_align(body_scroll, LV_ALIGN_TOP_MID, 0, 62);
    lv_obj_set_scroll_dir(body_scroll, LV_DIR_VER);

    lv_obj_t *body = zsw_ui_label_create(body_scroll, current_popup.body, ZSW_FONT_TEXT, ZSW_COLOR_TEXT_DIM);
    lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(body, LV_PCT(100));
    lv_obj_set_style_base_dir(body, LV_BASE_DIR_AUTO, LV_PART_MAIN);

    if (current_popup.display_yes_no) {
        no_btn = zsw_ui_round_button_create(mbox, LV_SYMBOL_CLOSE, BUTTON_SIZE, ZSW_COLOR_SURFACE_HIGH);
        lv_obj_align(no_btn, LV_ALIGN_BOTTOM_MID, -30, BUTTON_BOTTOM);
        yes_btn = zsw_ui_round_button_create(mbox, LV_SYMBOL_OK, BUTTON_SIZE, ZSW_COLOR_GREEN);
        lv_obj_align(yes_btn, LV_ALIGN_BOTTOM_MID, 30, BUTTON_BOTTOM);
        lv_obj_add_event_cb(yes_btn, on_popup_button_pressed, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(no_btn, on_popup_button_pressed, LV_EVENT_CLICKED, NULL);
        zsw_ui_slide_in(no_btn, 0);
        zsw_ui_slide_in(yes_btn, 40);
    } else {
        close_btn = zsw_ui_round_button_create(mbox, LV_SYMBOL_OK, BUTTON_SIZE, POPUP_COLOR);
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, BUTTON_BOTTOM);
        lv_obj_add_event_cb(close_btn, on_popup_close_button_pressed, LV_EVENT_CLICKED, NULL);
        zsw_ui_slide_in(close_btn, 0);
        lv_group_focus_obj(close_btn);
    }

    if (current_popup.close_after_seconds > 0) {
        auto_close_timer = lv_timer_create(close_popup_timer, current_popup.close_after_seconds * 1000, NULL);
        lv_timer_set_repeat_count(auto_close_timer, 1);
    } else {
        auto_close_timer = NULL;
    }
}

static void finalize_current_popup(bool confirmed)
{
    on_close_popup_cb_t finished_cb = NULL;

    if (!popup_active) {
        return;
    }

    finished_cb = current_popup.close_cb;

    if (mbox) {
        if (auto_close_timer) {
            lv_timer_del(auto_close_timer);
            auto_close_timer = NULL;
        }
        lv_obj_delete_async(mbox);
        mbox = NULL;
    }

    yes_btn = NULL;
    no_btn = NULL;

    if (finished_cb) {
        finished_cb(confirmed);
    }

    if (mbox == NULL) {
        popup_request_t next_popup;
        bool has_next = false;

        k_spinlock_key_t key = k_spin_lock(&popup_lock);
        if (k_msgq_get(&pending_popup_msgq, &next_popup, K_NO_WAIT) == 0) {
            has_next = true;
        } else {
            popup_active = false;
        }
        k_spin_unlock(&popup_lock, key);

        if (has_next) {
            show_popup_request(&next_popup);
        }
    }
}

void zsw_popup_show(char *title, char *body, on_close_popup_cb_t close_cb, uint32_t close_after_seconds,
                    bool display_yes_no)
{
    popup_request_t req = {
        .close_cb = close_cb,
        .close_after_seconds = close_after_seconds,
        .display_yes_no = display_yes_no,
    };

    copy_text_to_request(req.title, sizeof(req.title), title, "title");
    copy_text_to_request(req.body, sizeof(req.body), body, "body");

    k_spinlock_key_t key = k_spin_lock(&popup_lock);
    if (popup_active) {
        int ret = k_msgq_put(&pending_popup_msgq, &req, K_NO_WAIT);
        k_spin_unlock(&popup_lock, key);
        if (ret != 0) {
            LOG_WRN("Popup queue full, dropping popup");
        }
        return;
    }

    popup_active = true;
    k_spin_unlock(&popup_lock, key);

    show_popup_request(&req);
}

void zsw_popup_remove(void)
{
    if (!popup_active) {
        return;
    }

    finalize_current_popup(false);
}

static void on_popup_button_pressed(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target_obj(e);
    bool is_yes_btn = (target == yes_btn);

    finalize_current_popup(is_yes_btn);
}

static void on_popup_close_button_pressed(lv_event_t *e)
{
    LV_UNUSED(e);
    finalize_current_popup(false);
}

static void close_popup_timer(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    finalize_current_popup(false);
}
