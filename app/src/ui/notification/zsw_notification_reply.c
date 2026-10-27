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

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <lvgl.h>
#include <stdlib.h>
#include <string.h>

#include "ui/notification/zsw_notification_reply.h"
#include "ui/zsw_overlay_manager.h"
#include "ui/zsw_ui.h"
#include "ble/gadgetbridge/ble_gadgetbridge.h"
#include "drivers/zsw_vibration_motor.h"
#include "events/zsw_notification_event.h"
#include "managers/zsw_notification_manager.h"
#include "managers/zsw_power_manager.h"
#include "managers/zsw_recording_manager.h"

LOG_MODULE_REGISTER(zsw_notification_reply, CONFIG_ZSW_NOTIFICATION_REPLY_LOG_LEVEL);

#define UI_UPDATE_INTERVAL_MS   150
#define MAX_RECORDING_MS        (30 * 1000)
/* The recording manager drops shorter recordings without telling the phone. */
#define MIN_RECORDING_MS        1000
#define TRANSCRIBE_TIMEOUT_MS   (90 * 1000)
#define SEND_TIMEOUT_MS         (10 * 1000)
#define SENT_CLOSE_DELAY_MS     1500
#define TEXT_TOP                46
#define BUTTON_ROW_SPACE        76
#define BUTTON_HEIGHT           38
#define BUTTON_BOTTOM           -32
#define LEVEL_RING_SIZE         96

LV_FONT_DECLARE(lv_font_montserrat_14_full);
ZSW_LV_IMG_DECLARE(mic_icon);

typedef enum {
    /* The recording view is drawn first, the microphone starts on the next tick. */
    STATE_STARTING,
    STATE_RECORDING,
    STATE_TRANSCRIBING,
    STATE_DRAFT,
    STATE_SENDING,
    STATE_SENT,
    STATE_FAILED,
} reply_state_t;

static void popup_show(lv_obj_t *parent, lv_group_t *group);
static void popup_close(void);
static void render(void);

static zsw_overlay_t overlay = {
    .name = "Reply",
    /* Above incoming notifications and Chat answers, below the quick recording. */
    .priority = 25,
    .blocks_input = true,
    .show = popup_show,
    .close = popup_close,
};

static struct {
    reply_state_t state;
    uint32_t id;
    char sender[ZSW_NOTIFICATION_MGR_MAX_FIELD_LEN];
    /* Recording the expected draft comes from. */
    char filename[32];
    int64_t state_since;
    /* Draft text on the LVGL heap, only while there is one. */
    char *draft;
    char status[96];
    /* A failed transcription can be recorded again, a failed send can not. */
    bool can_record_again;

    lv_obj_t *cont;
    lv_obj_t *body;
    lv_obj_t *time_label;
    lv_obj_t *level_arc;
    lv_timer_t *timer;
} reply;

static void on_reply_event(const struct zbus_channel *chan);
ZBUS_LISTENER_DEFINE(zsw_notification_reply_lis, on_reply_event);
ZBUS_CHAN_DECLARE(zsw_notification_reply_chan);
ZBUS_CHAN_ADD_OBS(zsw_notification_reply_chan, zsw_notification_reply_lis, 1);

static struct k_work event_work;

static void set_state(reply_state_t state)
{
    reply.state = state;
    reply.state_since = k_uptime_get();
}

static void fail(const char *message, bool can_record_again)
{
    strncpy(reply.status, message, sizeof(reply.status) - 1);
    reply.status[sizeof(reply.status) - 1] = '\0';
    reply.can_record_again = can_record_again;
    set_state(STATE_FAILED);
}

static void free_draft(void)
{
    if (reply.draft) {
        lv_free(reply.draft);
        reply.draft = NULL;
    }
}

/* Starting the microphone while the view is first drawn overflows its buffer. */
static void request_recording(void)
{
    free_draft();
    reply.filename[0] = '\0';
    set_state(STATE_STARTING);
}

static void begin_recording(void)
{
    int ret = zsw_recording_manager_start_prefixed("reply_");
    if (ret < 0) {
        LOG_ERR("Failed to start recording: %d", ret);
        zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_QUICK_REC_ERROR);
        reply.filename[0] = '\0';
        fail("Could not start the microphone.", true);
        return;
    }
    const char *filename = zsw_recording_manager_store_get_current_filename();
    strncpy(reply.filename, filename ? filename : "", sizeof(reply.filename) - 1);
    reply.filename[sizeof(reply.filename) - 1] = '\0';
    set_state(STATE_RECORDING);
}

static void finish_recording(void)
{
    if (zsw_recording_manager_get_elapsed_ms() < MIN_RECORDING_MS) {
        zsw_recording_manager_abort();
        fail("Too short. Hold on a little longer.", true);
        return;
    }
    zsw_recording_manager_stop();
    set_state(STATE_TRANSCRIBING);
}

static void send_draft(void)
{
    int ret = reply.draft ? ble_gadgetbridge_send_notification_reply(reply.id, reply.draft) : -EINVAL;
    if (ret < 0) {
        fail("Not connected to the phone.", false);
        return;
    }
    set_state(STATE_SENDING);
}

static void cancel_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    zsw_overlay_close(&overlay);
}

static void done_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    finish_recording();
    render();
}

static void record_again_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    request_recording();
    render();
}

static void send_btn_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    send_draft();
    render();
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

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, const lv_font_t *font, lv_color_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    return label;
}

static lv_obj_t *create_button(const char *text, lv_color_t color, lv_color_t text_color, lv_event_cb_t cb,
                               int x, int width)
{
    lv_obj_t *btn = lv_btn_create(reply.body);
    lv_obj_set_size(btn, width, BUTTON_HEIGHT);
    lv_obj_set_style_radius(btn, BUTTON_HEIGHT / 2, 0);
    lv_obj_set_style_bg_color(btn, color, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_ext_click_area(btn, 4);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, x, BUTTON_BOTTOM);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(lv_group_get_default(), btn);

    if (text) {
        lv_obj_t *label = create_label(btn, text, &lv_font_montserrat_14, text_color);
        lv_obj_center(label);
    }
    return btn;
}

static lv_obj_t *create_cancel_button(int x)
{
    return create_button(LV_SYMBOL_CLOSE, lv_color_make(0x3a, 0x3a, 0x4e), lv_color_white(), cancel_btn_cb, x,
                         BUTTON_HEIGHT);
}

static lv_obj_t *create_record_again_button(int x)
{
    lv_obj_t *btn = create_button(NULL, lv_color_make(0x3a, 0x3a, 0x4e), lv_color_white(), record_again_btn_cb, x,
                                  BUTTON_HEIGHT);
    lv_obj_center(zsw_ui_utils_create_icon(btn, ZSW_LV_IMG_USE(mic_icon), 22, lv_color_white()));
    return btn;
}

static void create_scroll_text(const char *text, lv_color_t color)
{
    lv_obj_t *scroll = lv_obj_create(reply.body);
    lv_obj_set_size(scroll, 196, lv_disp_get_ver_res(NULL) - TEXT_TOP - BUTTON_ROW_SPACE);
    lv_obj_set_style_bg_opa(scroll, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(scroll, 0, 0);
    lv_obj_set_style_pad_all(scroll, 0, 0);
    lv_obj_set_scroll_dir(scroll, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scroll, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_align(scroll, LV_ALIGN_TOP_MID, 0, TEXT_TOP);

    lv_obj_t *label = create_label(scroll, text, &lv_font_montserrat_14_full, color);
    lv_obj_set_width(label, 196);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_base_dir(label, LV_BASE_DIR_AUTO, 0);
    /* Room to scroll the last line above the round bezel. */
    lv_obj_set_style_pad_bottom(label, 12, 0);
}

static lv_obj_t *create_status_badge(const char *symbol, lv_color_t color)
{
    lv_obj_t *badge = lv_obj_create(reply.body);
    lv_obj_set_size(badge, 64, 64);
    lv_obj_set_style_radius(badge, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(badge, color, 0);
    lv_obj_set_style_border_width(badge, 0, 0);
    lv_obj_clear_flag(badge, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(badge, LV_ALIGN_CENTER, 0, -18);

    lv_obj_t *icon = create_label(badge, symbol, &lv_font_montserrat_28, lv_color_white());
    lv_obj_center(icon);
    return badge;
}

static void badge_zoom_anim_cb(void *obj, int32_t value)
{
    lv_obj_set_style_transform_scale(obj, value, 0);
}

static void render_recording(void)
{
    lv_obj_t *ring = lv_arc_create(reply.body);
    lv_obj_set_size(ring, LEVEL_RING_SIZE, LEVEL_RING_SIZE);
    lv_arc_set_rotation(ring, 270);
    lv_arc_set_bg_angles(ring, 0, 360);
    lv_arc_set_range(ring, 0, 100);
    lv_arc_set_value(ring, 0);
    lv_obj_remove_style(ring, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ring, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(ring, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_width(ring, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(ring, lv_color_make(0x3a, 0x3a, 0x4e), LV_PART_MAIN);
    lv_obj_set_style_arc_color(ring, zsw_color_red(), LV_PART_INDICATOR);
    lv_obj_align(ring, LV_ALIGN_CENTER, 0, -16);
    reply.level_arc = ring;

    lv_obj_t *mic = zsw_ui_utils_create_icon(reply.body, ZSW_LV_IMG_USE(mic_icon), 36, lv_color_white());
    lv_obj_align_to(mic, ring, LV_ALIGN_CENTER, 0, -10);

    reply.time_label = create_label(reply.body, "0:00", &lv_font_montserrat_14, lv_color_make(0xbb, 0xbb, 0xbb));
    lv_obj_align_to(reply.time_label, ring, LV_ALIGN_CENTER, 0, 22);

    create_cancel_button(-46);
    lv_obj_t *done = create_button(LV_SYMBOL_OK " Done", zsw_color_blue(), lv_color_black(), done_btn_cb, 22, 84);
    lv_group_focus_obj(done);
}

static void render_waiting(const char *text)
{
    lv_obj_t *spinner = lv_spinner_create(reply.body);
    lv_obj_set_size(spinner, 44, 44);
    lv_obj_set_style_arc_width(spinner, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(spinner, zsw_color_blue(), LV_PART_INDICATOR);
    lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -14);

    lv_obj_t *label = create_label(reply.body, text, &lv_font_montserrat_14, lv_color_make(0xbb, 0xbb, 0xbb));
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 26);
}

static void render_draft(void)
{
    create_scroll_text(reply.draft ? reply.draft : "", lv_color_white());
    create_cancel_button(-66);
    create_record_again_button(-20);
    lv_obj_t *send = create_button(LV_SYMBOL_UPLOAD " Send", zsw_color_blue(), lv_color_black(), send_btn_cb, 42,
                                   76);
    lv_group_focus_obj(send);
}

static void render_sent(void)
{
    lv_obj_t *badge = create_status_badge(LV_SYMBOL_OK, lv_color_make(0x2e, 0x9e, 0x5b));
    lv_obj_t *label = create_label(reply.body, reply.status[0] ? reply.status : "Sent", &lv_font_montserrat_14_full,
                                   lv_color_white());
    lv_obj_set_width(label, 180);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_align_to(label, badge, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /* A short pop so the confirmation is noticed. */
    lv_obj_set_style_transform_pivot_x(badge, 32, 0);
    lv_obj_set_style_transform_pivot_y(badge, 32, 0);
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, badge);
    lv_anim_set_exec_cb(&anim, badge_zoom_anim_cb);
    lv_anim_set_values(&anim, 128, 256);
    lv_anim_set_duration(&anim, 250);
    lv_anim_set_path_cb(&anim, lv_anim_path_overshoot);
    lv_anim_start(&anim);
}

static void render_failed(void)
{
    lv_obj_t *label = create_label(reply.body, reply.status, &lv_font_montserrat_14_full, lv_color_white());
    lv_obj_set_width(label, 180);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -10);

    if (reply.can_record_again) {
        create_cancel_button(-30);
        lv_group_focus_obj(create_record_again_button(30));
    } else {
        lv_group_focus_obj(create_cancel_button(0));
    }
}

static void render(void)
{
    if (!reply.cont) {
        return;
    }
    if (reply.body) {
        lv_obj_del(reply.body);
    }
    reply.time_label = NULL;
    reply.level_arc = NULL;
    reply.body = create_view(reply.cont);

    switch (reply.state) {
        case STATE_STARTING:
        case STATE_RECORDING:
            render_recording();
            break;
        case STATE_TRANSCRIBING:
            render_waiting("Transcribing...");
            create_cancel_button(0);
            break;
        case STATE_DRAFT:
            render_draft();
            break;
        case STATE_SENDING:
            render_waiting("Sending...");
            break;
        case STATE_SENT:
            render_sent();
            break;
        case STATE_FAILED:
            render_failed();
            break;
    }
}

static void timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    int64_t in_state = k_uptime_get() - reply.state_since;

    switch (reply.state) {
        case STATE_STARTING:
            begin_recording();
            if (reply.state != STATE_RECORDING) {
                render();
            }
            break;
        case STATE_RECORDING: {
            uint32_t elapsed_ms = zsw_recording_manager_get_elapsed_ms();
            if (!zsw_recording_manager_is_recording()) {
                /* Stopped by someone else, for example full storage. */
                fail("The recording stopped.", true);
                render();
                break;
            }
            if (elapsed_ms >= MAX_RECORDING_MS) {
                finish_recording();
                render();
                break;
            }
            uint32_t elapsed_s = elapsed_ms / 1000;
            lv_label_set_text_fmt(reply.time_label, "%u:%02u", elapsed_s / 60, elapsed_s % 60);
            lv_arc_set_value(reply.level_arc, zsw_recording_manager_get_audio_level());
            break;
        }
        case STATE_TRANSCRIBING:
            if (in_state > TRANSCRIBE_TIMEOUT_MS) {
                fail("No text from the phone. Is it nearby?", true);
                render();
            }
            break;
        case STATE_SENDING:
            /* Written to the phone, it just did not confirm, which an older companion app never does. */
            if (in_state > SEND_TIMEOUT_MS) {
                strncpy(reply.status, "Sent to the phone", sizeof(reply.status) - 1);
                set_state(STATE_SENT);
                render();
            }
            break;
        case STATE_SENT:
            if (in_state > SENT_CLOSE_DELAY_MS) {
                zsw_overlay_close(&overlay);
            }
            break;
        default:
            break;
    }
}

static void popup_show(lv_obj_t *parent, lv_group_t *group)
{
    LV_UNUSED(group);
    reply.cont = create_view(parent);
    /* Opaque and square, so LVGL skips redrawing the screen below. */
    lv_obj_set_style_radius(reply.cont, 0, 0);
    lv_obj_set_style_bg_color(reply.cont, lv_color_hex(0x0C0C1A), 0);
    lv_obj_set_style_bg_opa(reply.cont, LV_OPA_COVER, 0);

    lv_obj_t *title = create_label(reply.cont, "", &lv_font_montserrat_14_full, zsw_color_blue());
    lv_label_set_text_fmt(title, "Reply to %s", reply.sender);
    lv_obj_set_size(title, 150, lv_font_get_line_height(&lv_font_montserrat_14_full));
    lv_label_set_long_mode(title, LV_LABEL_LONG_DOT);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    reply.timer = lv_timer_create(timer_cb, UI_UPDATE_INTERVAL_MS, NULL);
    render();
}

static void popup_close(void)
{
    if (reply.state == STATE_RECORDING && zsw_recording_manager_is_recording()) {
        zsw_recording_manager_abort();
    }
    free_draft();
    lv_timer_delete(reply.timer);
    lv_obj_del(reply.cont);
    reply.timer = NULL;
    reply.cont = NULL;
    reply.body = NULL;
    reply.time_label = NULL;
    reply.level_arc = NULL;
    /* A late draft for this recording is ignored. */
    reply.filename[0] = '\0';
}

int zsw_notification_reply_start(uint32_t id)
{
    zsw_not_mngr_notification_t not;

    if (zsw_notification_manager_get(id, &not) != 0) {
        return -ENOENT;
    }
    if (!not.can_reply) {
        return -ENOTSUP;
    }
    if (zsw_overlay_is_shown(&overlay)) {
        zsw_overlay_close(&overlay);
    }

    reply.id = id;
    strncpy(reply.sender, not.sender[0] != '\0' ? not.sender : not.title, sizeof(reply.sender) - 1);
    reply.sender[sizeof(reply.sender) - 1] = '\0';
    reply.status[0] = '\0';
    request_recording();
    zsw_overlay_show(&overlay);

    return 0;
}

bool zsw_notification_reply_is_shown(void)
{
    return zsw_overlay_is_shown(&overlay);
}

static void handle_event_async(void *data)
{
    ARG_UNUSED(data);

    if (!zsw_overlay_is_shown(&overlay) ||
        zbus_chan_claim(&zsw_notification_reply_chan, K_MSEC(50)) != 0) {
        return;
    }

    /* Read in place, the event is too large to copy on the stack. */
    const struct zsw_notification_reply_event *evt = zbus_chan_const_msg(&zsw_notification_reply_chan);
    bool changed = false;

    switch (evt->type) {
        case ZSW_NOTIFICATION_REPLY_DRAFT:
        case ZSW_NOTIFICATION_REPLY_DRAFT_FAILED:
            if (reply.state != STATE_TRANSCRIBING || strcmp(evt->filename, reply.filename) != 0) {
                LOG_INF("Draft for %s ignored", evt->filename);
                break;
            }
            if (evt->type == ZSW_NOTIFICATION_REPLY_DRAFT && evt->text[0] != '\0') {
                free_draft();
                reply.draft = lv_strdup(evt->text);
            }
            if (reply.draft) {
                set_state(STATE_DRAFT);
            } else {
                fail(evt->text[0] != '\0' ? evt->text : "Could not transcribe that.", true);
            }
            changed = true;
            break;
        case ZSW_NOTIFICATION_REPLY_SENT:
            if (reply.state != STATE_SENDING || evt->id != reply.id) {
                break;
            }
            if (evt->ok) {
                reply.status[0] = '\0';
                set_state(STATE_SENT);
            } else {
                fail(evt->text[0] != '\0' ? evt->text : "The phone could not send it.", false);
            }
            changed = true;
            break;
    }
    zbus_chan_finish(&zsw_notification_reply_chan);

    if (changed) {
        render();
    }
}

static void event_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    /* The user may have lowered the wrist while waiting for the phone. */
    zsw_power_manager_reset_idle_timout();
    zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_CLICK);
    lv_async_call(handle_event_async, NULL);
}

static void on_reply_event(const struct zbus_channel *chan)
{
    ARG_UNUSED(chan);
    k_work_submit(&event_work);
}

static int zsw_notification_reply_init(void)
{
    k_work_init(&event_work, event_work_fn);
    return 0;
}

SYS_INIT(zsw_notification_reply_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#ifdef CONFIG_SHELL
static void shell_start_async(void *data)
{
    int ret = zsw_notification_reply_start((uint32_t)(uintptr_t)data);
    if (ret < 0) {
        LOG_WRN("Reply not started: %d", ret);
    }
}

static int cmd_reply_start(const struct shell *sh, size_t argc, char **argv)
{
    uint32_t id = strtoul(argv[1], NULL, 10);

    lv_async_call(shell_start_async, (void *)(uintptr_t)id);
    shell_print(sh, "Starting reply to %u", id);
    return 0;
}

static int cmd_reply_state(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    static const char *const names[] = {"starting", "recording", "transcribing", "draft", "sending", "sent",
                                        "failed"
                                       };
    shell_print(sh, "shown=%d state=%s id=%u file=%s draft=%s status=%s", zsw_overlay_is_shown(&overlay),
                names[reply.state], reply.id, reply.filename, reply.draft ? reply.draft : "", reply.status);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_reply,
                               SHELL_CMD_ARG(start, NULL, "Dictate a reply: reply start <notification id>",
                                             cmd_reply_start, 2, 0),
                               SHELL_CMD_ARG(state, NULL, "Show the reply state", cmd_reply_state, 1, 0),
                               SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(reply, &sub_reply, "Dictated notification replies", NULL);
#endif
