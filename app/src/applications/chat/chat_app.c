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

/*
 * Chat by voice; the phone answers.
 * Questions are ordinary recordings named "chat_..." (new question) or
 * "chatr_..." (reply to the shown answer), so they sync like voice memos.
 * The phone keeps the conversation; the watch only shows the latest answer.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "managers/zsw_recording_manager.h"
#include "managers/zsw_power_manager.h"
#include "drivers/zsw_vibration_motor.h"
#include "events/zsw_voice_memo_event.h"
#include "ui/zsw_ui.h"
#include "ui/zsw_overlay_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(chat_app, CONFIG_ZSW_CHAT_APP_LOG_LEVEL);

#define UI_UPDATE_INTERVAL_MS  200
#define ANSWER_TIMEOUT_MS      (120 * 1000)
#define POPUP_DISMISS_MS       (30 * 1000)
#define POPUP_TOP              46

LV_FONT_DECLARE(lv_font_montserrat_14_full);

typedef enum {
    STATE_IDLE,
    STATE_RECORDING,
    STATE_THINKING,
    STATE_ANSWER,
    STATE_TIMED_OUT,
} chat_state_t;

static void on_answer_event(const struct zbus_channel *chan);
ZBUS_LISTENER_DEFINE(chat_app_answer_lis, on_answer_event);

static chat_state_t state;
static int64_t thinking_since;
static char answer[ZSW_CHAT_MAX_ANSWER];
static K_MUTEX_DEFINE(answer_lock);
static struct k_work answer_work;

static struct {
    lv_obj_t *cont;
    lv_obj_t *body;
    lv_obj_t *time_label;
    lv_timer_t *timer;
    int64_t shown_at;
} popup;

static void popup_render(void);
static void popup_show(lv_obj_t *parent, lv_group_t *group);
static void popup_close(void);

static zsw_overlay_t overlay = {
    .name = "Chat",
    .priority = 20,
    .blocks_input = true,
    .show = popup_show,
    .close = popup_close,
};

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    return label;
}

static int begin_recording(bool reply)
{
    int ret = zsw_recording_manager_start_prefixed(reply ? "chatr_" : "chat_");
    if (ret < 0) {
        LOG_ERR("Failed to start recording: %d", ret);
        zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_QUICK_REC_ERROR);
        return ret;
    }
    state = STATE_RECORDING;
    return 0;
}

static void end_recording(void)
{
    zsw_recording_manager_stop();
    thinking_since = k_uptime_get();
    state = STATE_THINKING;
}

static void send_question(void)
{
    end_recording();
    popup_render();
}

static void reply_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    if (begin_recording(true) == 0) {
        popup_render();
    }
}

static void send_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    send_question();
}

static lv_obj_t *create_view(lv_obj_t *parent)
{
    lv_obj_t *view = lv_obj_create(parent);
    lv_obj_set_size(view, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_style_bg_opa(view, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(view, 0, 0);
    lv_obj_set_style_pad_all(view, 0, 0);
    lv_obj_clear_flag(view, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(view);
    return view;
}

static void build_answer_view(lv_obj_t *parent, int top, int bottom_space)
{
    lv_obj_t *scroll = lv_obj_create(parent);
    lv_obj_set_size(scroll, 196, lv_disp_get_ver_res(NULL) - top - bottom_space);
    lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(scroll, 0, 0);
    lv_obj_set_style_pad_all(scroll, 0, 0);
    lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, top);

    lv_obj_t *text = lv_label_create(scroll);
    k_mutex_lock(&answer_lock, K_FOREVER);
    lv_label_set_text(text, answer);
    k_mutex_unlock(&answer_lock);
    lv_obj_set_width(text, 196);
    lv_label_set_long_mode(text, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(text, &lv_font_montserrat_14_full, 0);
    lv_obj_set_style_text_color(text, lv_color_white(), 0);
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);
    /* Room to scroll the last line above the round bezel. */
    lv_obj_set_style_pad_bottom(text, 20, 0);
}

static void popup_close(void)
{
    if (state == STATE_RECORDING) {
        zsw_recording_manager_abort();
        state = STATE_ANSWER;
    }
    lv_timer_delete(popup.timer);
    lv_obj_del(popup.cont);
    memset(&popup, 0, sizeof(popup));
}

static void popup_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    if (state == STATE_RECORDING && !zsw_recording_manager_is_recording()) {
        /* Max duration reached; the manager already saved the file. */
        send_question();
    } else if (state == STATE_THINKING && k_uptime_get() - thinking_since > ANSWER_TIMEOUT_MS) {
        state = STATE_TIMED_OUT;
        popup_render();
    } else if (state == STATE_RECORDING && popup.time_label) {
        uint32_t elapsed_s = zsw_recording_manager_get_elapsed_ms() / 1000;
        lv_label_set_text_fmt(popup.time_label, "%u:%02u", elapsed_s / 60, elapsed_s % 60);
    } else if ((state == STATE_ANSWER || state == STATE_TIMED_OUT) &&
               k_uptime_get() - popup.shown_at > POPUP_DISMISS_MS) {
        zsw_overlay_close(&overlay);
    }
}

static void popup_close_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    zsw_overlay_close(&overlay);
}

static lv_obj_t *popup_button(const char *text, lv_color_t color, lv_event_cb_t cb, int x, int width)
{
    lv_obj_t *btn = lv_btn_create(popup.body);
    lv_obj_set_size(btn, width, 40);
    lv_obj_set_style_radius(btn, 20, 0);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, x, -26);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(lv_group_get_default(), btn);

    lv_obj_t *label = create_label(btn, text, &lv_font_montserrat_14, lv_color_white());
    lv_obj_center(label);
    return btn;
}

static void popup_render(void)
{
    if (!popup.cont) {
        return;
    }
    if (popup.body) {
        lv_obj_del(popup.body);
    }
    popup.time_label = NULL;
    popup.shown_at = k_uptime_get();
    popup.body = create_view(popup.cont);

    lv_obj_t *focus = NULL;
    switch (state) {
        case STATE_RECORDING: {
            build_answer_view(popup.body, POPUP_TOP, 74);
            lv_obj_t *dot = lv_obj_create(popup.body);
            lv_obj_set_size(dot, 10, 10);
            lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
            lv_obj_set_style_bg_color(dot, zsw_color_red(), 0);
            lv_obj_set_style_border_width(dot, 0, 0);
            lv_obj_align(dot, LV_ALIGN_BOTTOM_MID, -74, -41);
            popup.time_label = create_label(popup.body, "0:00", &lv_font_montserrat_14, lv_color_white());
            lv_obj_align(popup.time_label, LV_ALIGN_BOTTOM_MID, -44, -38);
            focus = popup_button(LV_SYMBOL_OK " Send", zsw_color_blue(), send_btn_cb, 36, 92);
            break;
        }
        case STATE_THINKING: {
            lv_obj_t *spinner = lv_spinner_create(popup.body);
            lv_obj_set_size(spinner, 40, 40);
            lv_obj_set_style_arc_width(spinner, 4, LV_PART_MAIN);
            lv_obj_set_style_arc_width(spinner, 4, LV_PART_INDICATOR);
            lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -6);
            lv_obj_t *label = create_label(popup.body, "Thinking...", &lv_font_montserrat_14,
                                           lv_color_make(0xbb, 0xbb, 0xbb));
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 30);
            focus = popup_button(LV_SYMBOL_CLOSE, lv_color_make(0x3a, 0x3a, 0x4e), popup_close_cb, 0, 40);
            break;
        }
        case STATE_ANSWER:
            build_answer_view(popup.body, POPUP_TOP, 74);
            popup_button(LV_SYMBOL_CLOSE, lv_color_make(0x3a, 0x3a, 0x4e), popup_close_cb, -55, 40);
            focus = popup_button(LV_SYMBOL_AUDIO " Reply", zsw_color_blue(), reply_btn_cb, 20, 100);
            break;
        case STATE_TIMED_OUT: {
            lv_obj_t *label = create_label(popup.body, "No answer yet.\nIs the phone nearby\nwith OpenRouter set up?",
                                           &lv_font_montserrat_14, lv_color_white());
            lv_obj_align(label, LV_ALIGN_CENTER, 0, -10);
            focus = popup_button(LV_SYMBOL_CLOSE, lv_color_make(0x3a, 0x3a, 0x4e), popup_close_cb, 0, 40);
            break;
        }
        case STATE_IDLE:
            zsw_overlay_close(&overlay);
            return;
    }
    if (focus) {
        lv_group_focus_obj(focus);
    }
}

static void popup_show(lv_obj_t *parent, lv_group_t *group)
{
    LV_UNUSED(group);
    popup.cont = create_view(parent);
    lv_obj_set_style_bg_color(popup.cont, lv_color_make(0x1a, 0x1a, 0x2e), 0);
    lv_obj_set_style_bg_opa(popup.cont, LV_OPA_90, 0);

    lv_obj_t *title = create_label(popup.cont, "Chat", &lv_font_montserrat_14, zsw_color_blue());
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    popup.timer = lv_timer_create(popup_timer_cb, UI_UPDATE_INTERVAL_MS, NULL);
    popup_render();
}

static void answer_async(void *data)
{
    ARG_UNUSED(data);
    /* A reply being recorded keeps going; the new answer shows once it is sent. */
    if (state != STATE_RECORDING) {
        state = STATE_ANSWER;
    }
    if (zsw_overlay_is_shown(&overlay)) {
        popup_render();
    } else {
        zsw_overlay_show(&overlay);
    }
}

static void answer_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    zsw_power_manager_reset_idle_timout();
    zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_NOTIFICATION);
    lv_async_call(answer_async, NULL);
}

static void on_answer_event(const struct zbus_channel *chan)
{
    const struct zsw_chat_answer_event *evt = zbus_chan_const_msg(chan);

    LOG_INF("Answer for %s (%u bytes)", evt->filename, strlen(evt->text));
    if (k_mutex_lock(&answer_lock, K_MSEC(100)) == 0) {
        strncpy(answer, evt->text, sizeof(answer) - 1);
        answer[sizeof(answer) - 1] = '\0';
        k_mutex_unlock(&answer_lock);
    }
    k_work_submit(&answer_work);
}

static void render_async(void *data)
{
    ARG_UNUSED(data);
    popup_render();
}

static int cmd_chat(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    static const char *const actions[] = {"new", "reply", "send"};

    if (strcmp(argv[1], "state") == 0) {
        shell_print(sh, "state=%d popup=%d answer=%s", state, popup.cont != NULL, answer);
        return 0;
    }
    for (int i = 0; i < ARRAY_SIZE(actions); i++) {
        if (strcmp(argv[1], actions[i]) != 0) {
            continue;
        }
        int ret = 0;
        if (i < 2) {
            ret = begin_recording(i == 1);
        } else if (state == STATE_RECORDING) {
            end_recording();
        }
        if (ret == 0) {
            lv_async_call(render_async, NULL);
        }
        return ret;
    }
    shell_error(sh, "Use new|reply|send|state");
    return -EINVAL;
}

SHELL_CMD_ARG_REGISTER(chat, NULL, "Chat: new|reply|send|state", cmd_chat, 2, 0);

static int chat_init(void)
{
    k_work_init(&answer_work, answer_work_fn);
    return 0;
}

SYS_INIT(chat_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
