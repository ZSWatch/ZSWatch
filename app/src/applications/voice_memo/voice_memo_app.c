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
#include <string.h>

#include "managers/zsw_app_manager.h"
#include "managers/zsw_recording_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "events/zsw_voice_memo_event.h"
#include "voice_memo_ui.h"

LOG_MODULE_REGISTER(voice_memo_app, CONFIG_ZSW_VOICE_MEMO_LOG_LEVEL);

#define UI_UPDATE_INTERVAL_MS  200

ZSW_LV_IMG_DECLARE(music);

static void voice_memo_app_start(lv_obj_t *root, lv_group_t *group);
static void voice_memo_app_stop(void);
static bool voice_memo_app_back(void);
static void voice_memo_app_ui_unavailable(void);
static void voice_memo_app_ui_available(void);

static application_t app = {
    .name = "Voice Memo",
    .icon = ZSW_LV_IMG_USE(music),
    .start_func = voice_memo_app_start,
    .stop_func = voice_memo_app_stop,
    .back_func = voice_memo_app_back,
    .ui_unavailable_func = voice_memo_app_ui_unavailable,
    .ui_available_func = voice_memo_app_ui_available,
    .category = ZSW_APP_CATEGORY_ROOT,
};

ZBUS_CHAN_DECLARE(voice_memo_recording_chan);
ZBUS_CHAN_DECLARE(voice_memo_result_chan);

static void on_recording_event(const struct zbus_channel *chan);
static void on_result_event(const struct zbus_channel *chan);

ZBUS_LISTENER_DEFINE(voice_memo_app_recording_lis, on_recording_event);
ZBUS_LISTENER_DEFINE(voice_memo_app_result_lis, on_result_event);

static bool recording_screen_shown;

static lv_timer_t *ui_timer;
static zsw_recording_entry_t *list_entries;

static struct k_work recording_event_work;
static struct k_work result_event_work;

static void refresh_list(void);
static void recording_stopped_async(void *data);
static void result_refresh_async(void *data);

static void recording_event_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    if (app.current_state != ZSW_APP_STATE_STOPPED && !zsw_recording_manager_is_recording()) {
        lv_async_call(recording_stopped_async, NULL);
    }
}

static void recording_stopped_async(void *data)
{
    ARG_UNUSED(data);
    if (app.current_state != ZSW_APP_STATE_STOPPED) {
        recording_screen_shown = false;
        voice_memo_ui_show_list();
        refresh_list();
    }
}

static void on_recording_event(const struct zbus_channel *chan)
{
    const struct zsw_voice_memo_recording_event *evt = zbus_chan_const_msg(chan);
    if (evt->state == ZSW_VOICE_MEMO_RECORDING_STOPPED ||
        evt->state == ZSW_VOICE_MEMO_RECORDING_ABORTED) {
        k_work_submit(&recording_event_work);
    }
}

static void result_event_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    if (app.current_state != ZSW_APP_STATE_STOPPED) {
        lv_async_call(result_refresh_async, NULL);
    }
}

static void result_refresh_async(void *data)
{
    ARG_UNUSED(data);
    if (app.current_state != ZSW_APP_STATE_STOPPED) {
        refresh_list();
    }
}

static void on_result_event(const struct zbus_channel *chan)
{
    ARG_UNUSED(chan);
    k_work_submit(&result_event_work);
}

static void on_start_recording(void)
{
    int ret = zsw_recording_manager_start();
    if (ret == 0) {
        recording_screen_shown = true;
        voice_memo_ui_show_recording();
    } else {
        LOG_ERR("Failed to start recording: %d", ret);
    }
}

static void on_stop_recording(void)
{
    zsw_recording_manager_stop();
    recording_screen_shown = false;
    voice_memo_ui_show_list();
    refresh_list();
}

static void on_delete(const char *filename)
{
    zsw_recording_manager_delete(filename);
    refresh_list();
}

static void on_back_during_recording(bool save)
{
    if (save) {
        zsw_recording_manager_stop();
    } else {
        zsw_recording_manager_abort();
    }
    recording_screen_shown = false;
    voice_memo_ui_show_list();
    refresh_list();
}

static void refresh_list(void)
{
    if (!list_entries) {
        list_entries = k_malloc(sizeof(*list_entries) * ZSW_RECORDING_MAX_FILES);
        if (!list_entries) {
            LOG_ERR("Failed to allocate voice memo list buffer");
            voice_memo_ui_update_list(NULL, 0, 0, 0);
            return;
        }
    }

    int count = zsw_recording_manager_list(list_entries, ZSW_RECORDING_MAX_FILES);
    uint32_t free_bytes = 0;
    zsw_recording_manager_get_free_space(&free_bytes);

    if (count < 0) {
        count = 0;
    } else if (count > ZSW_RECORDING_MAX_FILES) {
        count = ZSW_RECORDING_MAX_FILES;
    }

    voice_memo_ui_update_list(list_entries, count, free_bytes / 1024,
                              (CONFIG_ZSW_OPUS_BITRATE / 8 + 1023) / 1024);
}

static const voice_memo_ui_callbacks_t ui_callbacks = {
    .on_start_recording = on_start_recording,
    .on_stop_recording = on_stop_recording,
    .on_delete = on_delete,
    .on_back_during_recording = on_back_during_recording,
};

static void ui_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    if (app.current_state != ZSW_APP_STATE_UI_VISIBLE) {
        return;
    }

    if (!zsw_recording_manager_is_recording()) {
        if (recording_screen_shown) {
            recording_screen_shown = false;
            voice_memo_ui_show_list();
            refresh_list();
        }
        return;
    }

    voice_memo_ui_update_time(zsw_recording_manager_get_elapsed_ms(),
                              ZSW_RECORDING_MAX_DURATION_S -
                              zsw_recording_manager_get_elapsed_ms() / 1000);
    voice_memo_ui_update_level(zsw_recording_manager_get_audio_level());
}

static void voice_memo_app_start(lv_obj_t *root, lv_group_t *group)
{
    ARG_UNUSED(group);

    recording_screen_shown = false;

    voice_memo_ui_show(root, &ui_callbacks);
    refresh_list();

    ui_timer = lv_timer_create(ui_timer_cb, UI_UPDATE_INTERVAL_MS, NULL);

    LOG_INF("Voice Memo app started");
}

static void voice_memo_app_stop(void)
{
    if (zsw_recording_manager_is_recording()) {
        zsw_recording_manager_stop();
    }

    if (ui_timer) {
        lv_timer_delete(ui_timer);
        ui_timer = NULL;
    }

    if (list_entries) {
        k_free(list_entries);
        list_entries = NULL;
    }

    voice_memo_ui_remove();
    LOG_INF("Voice Memo app stopped");
}

static bool voice_memo_app_back(void)
{
    if (zsw_recording_manager_is_recording()) {
        voice_memo_ui_show_back_confirm();
        return true;
    }
    return false;
}

static void voice_memo_app_ui_unavailable(void)
{
}

static void voice_memo_app_ui_available(void)
{
    if (zsw_recording_manager_is_recording()) {
        voice_memo_ui_show_recording();
    } else {
        refresh_list();
    }
}

static int voice_memo_app_add(void)
{
    zsw_app_manager_add_application(&app);
    k_work_init(&recording_event_work, recording_event_work_fn);
    k_work_init(&result_event_work, result_event_work_fn);
    return 0;
}

SYS_INIT(voice_memo_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
