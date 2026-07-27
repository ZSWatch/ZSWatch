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
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "managers/zsw_power_manager.h"
#include "ui/popup/zsw_popup_window.h"

LOG_MODULE_REGISTER(zsw_popup_window, LOG_LEVEL_INF);

#define POPUP_QUEUE_SIZE         8
#define POPUP_TITLE_MAX_LEN      64
#define POPUP_BODY_MAX_LEN       192

typedef struct popup_request {
    /* k_fifo links items through the first word, so it must stay first. */
    void *fifo_reserved;
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
static void init_popup_queue(void);
static popup_request_t *acquire_popup_request_slot(void);
static void release_popup_request_slot(popup_request_t *req);
static void copy_text_to_request(char *dst, size_t dst_size, const char *src, const char *name);

static lv_obj_t *mbox;
static lv_obj_t *yes_btn;
static lv_obj_t *no_btn;
static lv_timer_t *auto_close_timer;

static bool popup_queue_initialized;
static popup_request_t popup_request_pool[POPUP_QUEUE_SIZE];
static popup_request_t *current_popup;

/* Separate free/pending FIFOs avoid dynamic allocation while keeping FIFO order. */
K_FIFO_DEFINE(free_popup_fifo);
K_FIFO_DEFINE(pending_popup_fifo);

static void init_popup_queue(void)
{
    if (popup_queue_initialized) {
        return;
    }

    for (int i = 0; i < POPUP_QUEUE_SIZE; i++) {
        k_fifo_put(&free_popup_fifo, &popup_request_pool[i]);
    }

    popup_queue_initialized = true;
}

static popup_request_t *acquire_popup_request_slot(void)
{
    return k_fifo_get(&free_popup_fifo, K_NO_WAIT);
}

static void release_popup_request_slot(popup_request_t *req)
{
    if (!req) {
        return;
    }

    req->title[0] = '\0';
    req->body[0] = '\0';
    req->close_cb = NULL;
    req->close_after_seconds = 0;
    req->display_yes_no = false;

    k_fifo_put(&free_popup_fifo, req);
}

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

static void show_popup_request(popup_request_t *req)
{
    lv_obj_t *close_btn = NULL;

    current_popup = req;
    zsw_power_manager_reset_idle_timout();

    mbox = lv_msgbox_create(lv_layer_top());
    lv_msgbox_add_title(mbox, current_popup->title);
    lv_msgbox_add_text(mbox, current_popup->body);
    if (current_popup->display_yes_no) {
        yes_btn = lv_msgbox_add_footer_button(mbox, "Yes");
        no_btn = lv_msgbox_add_footer_button(mbox, "No");
        lv_obj_add_event_cb(yes_btn, on_popup_button_pressed, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(no_btn, on_popup_button_pressed, LV_EVENT_CLICKED, NULL);
    } else {
        close_btn = lv_msgbox_add_header_button(mbox, LV_SYMBOL_CLOSE);
        lv_obj_add_event_cb(close_btn, on_popup_close_button_pressed, LV_EVENT_CLICKED, NULL);
    }

    lv_obj_set_scrollbar_mode(lv_layer_top(), LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(mbox, LV_SCROLLBAR_MODE_OFF);
    lv_obj_center(mbox);
    if (close_btn) {
        lv_group_focus_obj(close_btn);
    }
    lv_obj_set_size(mbox, 180, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(mbox, 5, 0);
    lv_obj_clear_flag(mbox, LV_OBJ_FLAG_SCROLLABLE);

    static lv_style_t style_indic_not_bg;
    lv_style_init(&style_indic_not_bg);
    lv_style_set_bg_color(&style_indic_not_bg, lv_color_hex(0x2C3333));
    lv_obj_add_style(mbox, &style_indic_not_bg, 0);

    static lv_style_t color_style;
    lv_style_init(&color_style);
    lv_style_set_text_color(&color_style, lv_color_hex(0xCBE4DE));
    lv_style_set_bg_color(&color_style, lv_color_hex(0x2C3333));
    if (close_btn) {
        lv_obj_add_style(close_btn, &color_style, 0);
    }

    auto_close_timer = lv_timer_create(close_popup_timer, current_popup->close_after_seconds * 1000,  NULL);
    lv_timer_set_repeat_count(auto_close_timer, 1);
}

static void finalize_current_popup(bool confirmed)
{
    popup_request_t *finished_popup = current_popup;

    if (mbox) {
        if (auto_close_timer) {
            lv_timer_del(auto_close_timer);
            auto_close_timer = NULL;
        }
        lv_msgbox_close(mbox);
        mbox = NULL;
    }

    yes_btn = NULL;
    no_btn = NULL;
    current_popup = NULL;

    if (finished_popup && finished_popup->close_cb) {
        /* Callback is fired before slot recycling to preserve request context. */
        finished_popup->close_cb(confirmed);
    }

    release_popup_request_slot(finished_popup);

    popup_request_t *next_popup = k_fifo_get(&pending_popup_fifo, K_NO_WAIT);
    if (next_popup) {
        show_popup_request(next_popup);
    }
}

void zsw_popup_show(char *title, char *body, on_close_popup_cb_t close_cb, uint32_t close_after_seconds,
                    bool display_yes_no)
{
    popup_request_t *req;

    init_popup_queue();

    req = acquire_popup_request_slot();
    if (!req) {
        LOG_WRN("Popup queue full, dropping popup");
        return;
    }

    copy_text_to_request(req->title, sizeof(req->title), title, "title");
    copy_text_to_request(req->body, sizeof(req->body), body, "body");
    req->close_cb = close_cb;
    req->close_after_seconds = close_after_seconds;
    req->display_yes_no = display_yes_no;

    if (mbox) {
        k_fifo_put(&pending_popup_fifo, req);
        return;
    }

    show_popup_request(req);
}

void zsw_popup_remove(void)
{
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
    finalize_current_popup(false);
}

static void close_popup_timer(lv_timer_t *timer)
{
    finalize_current_popup(false);
}
