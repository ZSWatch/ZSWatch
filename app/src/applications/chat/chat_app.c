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
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <stdio.h>
#include <string.h>

#include "managers/zsw_app_manager.h"
#include "managers/zsw_recording_manager.h"
#include "managers/zsw_recording_manager_store.h"
#include "managers/zsw_speaker_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "ble/gadgetbridge/ble_gadgetbridge.h"
#include "events/zsw_chat_event.h"
#include "chat_ui.h"

LOG_MODULE_REGISTER(chat_app, CONFIG_ZSW_CHAT_LOG_LEVEL);

#define UI_UPDATE_INTERVAL_MS           200
#define MAX_QUESTION_DURATION_MS        6000
#define CHAT_REPLY_PATH                 "/user/chat/reply_current.wav"
#define CHAT_REPLY_DIR                  "/user/chat"
#define CHAT_QUESTION_DIR               "/user/chat/questions"
#define CHAT_QUESTION_CODEC             "opus_zsw"
#define CHAT_QUESTION_SAMPLE_RATE       16000

ZSW_LV_IMG_DECLARE(music);

static void chat_app_start(lv_obj_t *root, lv_group_t *group);
static void chat_app_stop(void);
static bool chat_app_back(void);

static application_t app = {
    .name = "Chat",
    .icon = ZSW_LV_IMG_USE(music),
    .start_func = chat_app_start,
    .stop_func = chat_app_stop,
    .back_func = chat_app_back,
    .category = ZSW_APP_CATEGORY_ROOT,
};

/* ---------- Zbus declarations ---------- */

ZBUS_CHAN_DECLARE(chat_state_chan);
ZBUS_CHAN_DECLARE(chat_transcript_chan);
ZBUS_CHAN_DECLARE(chat_reply_ready_chan);
ZBUS_CHAN_DECLARE(chat_error_chan);

static void on_state_event(const struct zbus_channel *chan);
static void on_transcript_event(const struct zbus_channel *chan);
static void on_reply_ready_event(const struct zbus_channel *chan);
static void on_error_event(const struct zbus_channel *chan);

ZBUS_LISTENER_DEFINE(chat_app_state_lis, on_state_event);
ZBUS_LISTENER_DEFINE(chat_app_transcript_lis, on_transcript_event);
ZBUS_LISTENER_DEFINE(chat_app_reply_ready_lis, on_reply_ready_event);
ZBUS_LISTENER_DEFINE(chat_app_error_lis, on_error_event);

/* ---------- State ---------- */

typedef enum {
    APP_CHAT_IDLE,
    APP_CHAT_LISTENING,
    APP_CHAT_UPLOADING,
    APP_CHAT_THINKING,
    APP_CHAT_SPEAKING,
    APP_CHAT_ERROR,
} app_chat_state_t;

static app_chat_state_t chat_state;
static uint32_t session_id;
static lv_timer_t *ui_timer;

/* Absolute path to the last completed chat question file. Chat recordings are
 * moved out of /user/recordings so the voice-memo sync never picks them up. */
static char last_question_path[96];

/* k_work items for context-switching from zbus */
static struct k_work state_work;
static struct k_work transcript_work;
static struct k_work reply_ready_work;
static struct k_work error_work;

/* k_timer to enforce the per-session max recording duration */
static struct k_timer max_duration_timer;
static struct k_work max_duration_work;

/* Cached event data (copied in zbus callbacks) */
static struct zsw_chat_state_event cached_state_event;
static struct zsw_chat_transcript_event cached_transcript_event;
static struct zsw_chat_reply_ready_event cached_reply_ready_event;
static struct zsw_chat_error_event cached_error_event;

/* ---------- Forward declarations ---------- */

static void set_chat_state(app_chat_state_t new_state);
static void start_recording(void);
static void stop_recording(void);
static void cancel_session(void);
static void start_reply_playback(const char *path);
static void cleanup_reply_file(void);
static void cleanup_question_file(void);
static int ensure_dir_exists(const char *path);

/* ---------- Async helpers for LVGL context ---------- */

static void state_update_async(void *data);
static void transcript_update_async(void *data);
static void reply_ready_async(void *data);
static void error_update_async(void *data);

/* ---------- Speaker callback ---------- */

static void speaker_event_cb(zsw_speaker_event_t event, void *user_data)
{
    ARG_UNUSED(user_data);

    if (event == ZSW_SPEAKER_EVENT_PLAYBACK_FINISHED ||
        event == ZSW_SPEAKER_EVENT_PLAYBACK_ERROR) {
        LOG_INF("Playback %s",
                event == ZSW_SPEAKER_EVENT_PLAYBACK_FINISHED ? "finished" : "error");
        ble_gadgetbridge_send_chat_playback_done(session_id);
        cleanup_reply_file();
        set_chat_state(APP_CHAT_IDLE);
    }
}

/* ---------- UI callbacks ---------- */

static void on_record_start(void)
{
    if (chat_state == APP_CHAT_IDLE) {
        start_recording();
    }
}

static void on_record_stop(void)
{
    if (chat_state == APP_CHAT_LISTENING) {
        stop_recording();
    }
}

static void on_cancel(void)
{
    cancel_session();
}

static void on_retry(void)
{
    set_chat_state(APP_CHAT_IDLE);
}

static const chat_ui_callbacks_t ui_callbacks = {
    .on_record_start = on_record_start,
    .on_record_stop = on_record_stop,
    .on_cancel = on_cancel,
    .on_retry = on_retry,
};

/* ---------- Recording (delegates to zsw_recording_manager) ---------- */

static void max_duration_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    if (chat_state == APP_CHAT_LISTENING) {
        LOG_INF("Chat: max recording duration reached, stopping");
        stop_recording();
    }
}

static void max_duration_timer_cb(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    /* Run on system work queue (LVGL/app context) */
    k_work_submit(&max_duration_work);
}

static void start_recording(void)
{
    int ret = zsw_recording_manager_start(ZSW_RECORDING_CLIENT_CHAT);
    if (ret != 0) {
        LOG_ERR("Failed to start recording: %d", ret);
        set_chat_state(APP_CHAT_ERROR);
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            chat_ui_set_error("Mic unavailable");
        }
        return;
    }

    session_id++;
    last_question_path[0] = '\0';
    set_chat_state(APP_CHAT_LISTENING);

    /* Auto-stop after the chat-specific max duration */
    k_timer_start(&max_duration_timer, K_MSEC(MAX_QUESTION_DURATION_MS), K_NO_WAIT);

    LOG_INF("Chat recording started (session %u)", session_id);
}

static void stop_recording(void)
{
    k_timer_stop(&max_duration_timer);

    zsw_recording_result_t result = {0};
    int ret = zsw_recording_manager_stop(&result);
    if (ret != 0 || result.filename[0] == '\0' || result.size_bytes == 0) {
        LOG_WRN("Chat recording stop failed or empty (ret=%d)", ret);
        set_chat_state(APP_CHAT_IDLE);
        return;
    }

    char source_path[VOICE_MEMO_MAX_FILENAME + sizeof(VOICE_MEMO_DIR) + 16];
    char remote_path[sizeof(last_question_path)];

    snprintf(source_path, sizeof(source_path), "%s/%s.zsw_opus",
             VOICE_MEMO_DIR, result.filename);
    snprintf(remote_path, sizeof(remote_path), "%s/%s.zsw_opus",
             CHAT_QUESTION_DIR, result.filename);

    if (ensure_dir_exists(CHAT_QUESTION_DIR) != 0) {
        LOG_ERR("Chat question dir unavailable");
        zsw_recording_manager_delete(result.filename);
        set_chat_state(APP_CHAT_ERROR);
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            chat_ui_set_error("Storage error");
        }
        return;
    }

    ret = fs_rename(source_path, remote_path);
    if (ret != 0) {
        LOG_ERR("Failed to move chat question from %s to %s: %d",
                source_path, remote_path, ret);
        zsw_recording_manager_delete(result.filename);
        set_chat_state(APP_CHAT_ERROR);
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            chat_ui_set_error("Storage error");
        }
        return;
    }

    strncpy(last_question_path, remote_path, sizeof(last_question_path) - 1);
    last_question_path[sizeof(last_question_path) - 1] = '\0';

    LOG_INF("Chat: question saved (%s, %u ms, %u bytes)",
            remote_path, result.duration_ms, result.size_bytes);

    set_chat_state(APP_CHAT_UPLOADING);
    ble_gadgetbridge_send_chat_question_ready(session_id, remote_path,
                                              result.duration_ms,
                                              result.size_bytes,
                                              CHAT_QUESTION_SAMPLE_RATE,
                                              CHAT_QUESTION_CODEC);
}

/* ---------- Playback ---------- */

static void start_reply_playback(const char *path)
{
    zsw_speaker_config_t config = {
        .source = ZSW_SPEAKER_SOURCE_FILE,
        .file = {
            .path = path,
        },
    };

    int ret = zsw_speaker_manager_start(&config, speaker_event_cb, NULL);
    if (ret != 0) {
        LOG_ERR("Failed to start reply playback: %d", ret);
        set_chat_state(APP_CHAT_ERROR);
        if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
            chat_ui_set_error("Playback failed");
        }
        return;
    }

    set_chat_state(APP_CHAT_SPEAKING);
}

static void cleanup_reply_file(void)
{
    int ret = fs_unlink(CHAT_REPLY_PATH);
    if (ret != 0 && ret != -ENOENT) {
        LOG_WRN("Failed to delete reply file: %d", ret);
    }
}

static void cleanup_question_file(void)
{
    if (last_question_path[0] == '\0') {
        return;
    }
    int ret = fs_unlink(last_question_path);
    if (ret != 0 && ret != -ENOENT) {
        LOG_WRN("Failed to delete question file %s: %d",
                last_question_path, ret);
    }
    last_question_path[0] = '\0';
}

static int ensure_dir_exists(const char *path)
{
    int ret;
    struct fs_dir_t dir;

    fs_dir_t_init(&dir);
    ret = fs_opendir(&dir, path);
    if (ret == 0) {
        fs_closedir(&dir);
        return 0;
    }

    ret = fs_mkdir(path);
    if (ret != 0 && ret != -EEXIST) {
        LOG_ERR("Failed to create directory %s: %d", path, ret);
        return ret;
    }

    return 0;
}

/* ---------- Session management ---------- */

static void cancel_session(void)
{
    k_timer_stop(&max_duration_timer);

    if (chat_state == APP_CHAT_LISTENING) {
        zsw_recording_manager_abort();
    }
    if (chat_state == APP_CHAT_UPLOADING) {
        cleanup_question_file();
    }
    if (chat_state == APP_CHAT_SPEAKING) {
        zsw_speaker_manager_stop();
        cleanup_reply_file();
    }

    ble_gadgetbridge_send_chat_cancel(session_id);
    set_chat_state(APP_CHAT_IDLE);
    LOG_INF("Chat session %u cancelled", session_id);
}

/* ---------- State machine ---------- */

static void set_chat_state(app_chat_state_t new_state)
{
    chat_state = new_state;
    if (app.current_state != ZSW_APP_STATE_UI_VISIBLE) {
        return;
    }

    switch (new_state) {
    case APP_CHAT_IDLE:
        chat_ui_set_state(CHAT_UI_STATE_IDLE);
        break;
    case APP_CHAT_LISTENING:
        chat_ui_set_state(CHAT_UI_STATE_LISTENING);
        break;
    case APP_CHAT_UPLOADING:
        chat_ui_set_state(CHAT_UI_STATE_UPLOADING);
        break;
    case APP_CHAT_THINKING:
        chat_ui_set_state(CHAT_UI_STATE_THINKING);
        break;
    case APP_CHAT_SPEAKING:
        chat_ui_set_state(CHAT_UI_STATE_SPEAKING);
        break;
    case APP_CHAT_ERROR:
        chat_ui_set_state(CHAT_UI_STATE_ERROR);
        break;
    }
}

/* ---------- Zbus event handlers ---------- */

static void state_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    if (app.current_state != ZSW_APP_STATE_STOPPED) {
        lv_async_call(state_update_async, NULL);
    }
}

static void transcript_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    if (app.current_state != ZSW_APP_STATE_STOPPED) {
        lv_async_call(transcript_update_async, NULL);
    }
}

static void reply_ready_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    /* Phone has fetched the question and uploaded the reply; we can
     * remove the question file now. */
    cleanup_question_file();
    if (app.current_state != ZSW_APP_STATE_STOPPED) {
        lv_async_call(reply_ready_async, NULL);
    }
}

static void error_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    cleanup_question_file();
    if (app.current_state != ZSW_APP_STATE_STOPPED) {
        lv_async_call(error_update_async, NULL);
    }
}

static void state_update_async(void *data)
{
    ARG_UNUSED(data);
    if (app.current_state == ZSW_APP_STATE_STOPPED) {
        return;
    }

    switch (cached_state_event.state) {
    case ZSW_CHAT_STATE_IDLE:
        set_chat_state(APP_CHAT_IDLE);
        break;
    case ZSW_CHAT_STATE_UPLOADING:
        set_chat_state(APP_CHAT_UPLOADING);
        break;
    case ZSW_CHAT_STATE_TRANSCRIBING:
    case ZSW_CHAT_STATE_THINKING:
    case ZSW_CHAT_STATE_GENERATING_TTS:
    case ZSW_CHAT_STATE_UPLOADING_REPLY:
        set_chat_state(APP_CHAT_THINKING);
        break;
    case ZSW_CHAT_STATE_REPLY_READY:
        /* Handled via reply_ready event */
        break;
    case ZSW_CHAT_STATE_ERROR:
        set_chat_state(APP_CHAT_ERROR);
        break;
    }
}

static void transcript_update_async(void *data)
{
    ARG_UNUSED(data);
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        chat_ui_set_transcript(cached_transcript_event.transcript);
    }
}

static void reply_ready_async(void *data)
{
    ARG_UNUSED(data);
    if (app.current_state == ZSW_APP_STATE_STOPPED) {
        return;
    }
    start_reply_playback(cached_reply_ready_event.path);
}

static void error_update_async(void *data)
{
    ARG_UNUSED(data);
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        chat_ui_set_error(cached_error_event.message);
        set_chat_state(APP_CHAT_ERROR);
    }
}

/* ---------- Zbus listeners ---------- */

static void on_state_event(const struct zbus_channel *chan)
{
    const struct zsw_chat_state_event *evt = zbus_chan_const_msg(chan);
    memcpy(&cached_state_event, evt, sizeof(cached_state_event));
    k_work_submit(&state_work);
}

static void on_transcript_event(const struct zbus_channel *chan)
{
    const struct zsw_chat_transcript_event *evt = zbus_chan_const_msg(chan);
    memcpy(&cached_transcript_event, evt, sizeof(cached_transcript_event));
    k_work_submit(&transcript_work);
}

static void on_reply_ready_event(const struct zbus_channel *chan)
{
    const struct zsw_chat_reply_ready_event *evt = zbus_chan_const_msg(chan);
    memcpy(&cached_reply_ready_event, evt, sizeof(cached_reply_ready_event));
    k_work_submit(&reply_ready_work);
}

static void on_error_event(const struct zbus_channel *chan)
{
    const struct zsw_chat_error_event *evt = zbus_chan_const_msg(chan);
    memcpy(&cached_error_event, evt, sizeof(cached_error_event));
    k_work_submit(&error_work);
}

/* ---------- UI timer ---------- */

static void ui_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    if (app.current_state != ZSW_APP_STATE_UI_VISIBLE) {
        return;
    }

    if (chat_state == APP_CHAT_LISTENING && zsw_recording_manager_is_recording()) {
        chat_ui_update_recording_time(zsw_recording_manager_get_elapsed_ms());
    }
}

/* ---------- App lifecycle ---------- */

static void chat_app_start(lv_obj_t *root, lv_group_t *group)
{
    ARG_UNUSED(group);

    chat_state = APP_CHAT_IDLE;
    last_question_path[0] = '\0';

    chat_ui_show(root, &ui_callbacks);
    ui_timer = lv_timer_create(ui_timer_cb, UI_UPDATE_INTERVAL_MS, NULL);

    (void)ensure_dir_exists(CHAT_REPLY_DIR);
    (void)ensure_dir_exists(CHAT_QUESTION_DIR);

    LOG_INF("Chat app started");
}

static void chat_app_stop(void)
{
    k_timer_stop(&max_duration_timer);

    if (chat_state == APP_CHAT_LISTENING) {
        zsw_recording_manager_abort();
    }
    if (chat_state == APP_CHAT_SPEAKING) {
        zsw_speaker_manager_stop();
    }

    if (ui_timer) {
        lv_timer_delete(ui_timer);
        ui_timer = NULL;
    }

    chat_ui_remove();
    chat_state = APP_CHAT_IDLE;

    LOG_INF("Chat app stopped");
}

static bool chat_app_back(void)
{
    if (chat_state != APP_CHAT_IDLE) {
        cancel_session();
        return true;
    }
    return false;
}

/* ---------- Init ---------- */

#if IS_ENABLED(CONFIG_APPLICATIONS_CHAT_AUTO_PLAY_TEST)
static void auto_play_test_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    struct fs_dirent ent;
    int ret = fs_stat(CHAT_REPLY_PATH, &ent);
    if (ret != 0) {
        LOG_ERR("AUTO-PLAY: %s not found (%d)", CHAT_REPLY_PATH, ret);
        return;
    }
    LOG_INF("AUTO-PLAY: starting playback of %s (%u bytes)",
            CHAT_REPLY_PATH, (unsigned)ent.size);
    start_reply_playback(CHAT_REPLY_PATH);
}
static K_WORK_DELAYABLE_DEFINE(auto_play_test_work, auto_play_test_work_fn);
#endif

static int chat_app_add(void)
{
    zsw_app_manager_add_application(&app);
    k_work_init(&state_work, state_work_fn);
    k_work_init(&transcript_work, transcript_work_fn);
    k_work_init(&reply_ready_work, reply_ready_work_fn);
    k_work_init(&error_work, error_work_fn);
    k_work_init(&max_duration_work, max_duration_work_fn);
    k_timer_init(&max_duration_timer, max_duration_timer_cb, NULL);
#if IS_ENABLED(CONFIG_APPLICATIONS_CHAT_AUTO_PLAY_TEST)
    k_work_schedule(&auto_play_test_work,
                    K_MSEC(CONFIG_APPLICATIONS_CHAT_AUTO_PLAY_DELAY_MS));
#endif
    return 0;
}

SYS_INIT(chat_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
