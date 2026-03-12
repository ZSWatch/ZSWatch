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
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <math.h>

#include "managers/zsw_app_manager.h"
#include "managers/zsw_microphone_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "ble/gadgetbridge/ble_gadgetbridge.h"
#include "zsw_audio_codec.h"
#include "voice_memo_ui.h"
#include "voice_memo_store.h"

LOG_MODULE_REGISTER(voice_memo_app, CONFIG_ZSW_VOICE_MEMO_LOG_LEVEL);

/* ---------- Configuration ---------- */
#define CODEC_RING_BUF_SIZE    2048   /* 64 ms — enough for ~6 Opus frames */
#define CODEC_THREAD_STACK     16384  /* Opus CELT encoder needs ~12-14 KB on ARM Cortex-M33 */
#define CODEC_THREAD_PRIO      K_PRIO_PREEMPT(5)
#define MAX_OPUS_FRAME_BYTES   160
#define UI_UPDATE_INTERVAL_MS  200
#define OVERFLOW_LOG_INTERVAL_MS 1000

/* ---------- Icon ---------- */
ZSW_LV_IMG_DECLARE(music);

/* ---------- Forward declarations ---------- */
static void voice_memo_app_start(lv_obj_t *root, lv_group_t *group);
static void voice_memo_app_stop(void);
static bool voice_memo_app_back(void);
static void voice_memo_app_ui_unavailable(void);
static void voice_memo_app_ui_available(void);

/* ---------- App registration ---------- */
static application_t app = {
    .name = "Voice Memo",
    .icon = ZSW_LV_IMG_USE(music),
    .start_func = voice_memo_app_start,
    .stop_func = voice_memo_app_stop,
    .back_func = voice_memo_app_back,
    .ui_unavailable_func = voice_memo_app_ui_unavailable,
    .ui_available_func = voice_memo_app_ui_available,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

/* ---------- State ---------- */
static bool is_recording;
static bool codec_thread_running;
static bool ui_visible;
static bool quick_record_pending_exit;  /* Auto-exit after quick-record stop */
static bool recording_screen_shown;
static bool auto_stop_pending;
static uint32_t recording_start_time;
static uint32_t peak_level;

/* Ring buffer for PCM data from mic to codec thread (dynamically allocated) */
static struct ring_buf pcm_ring_buf;
static uint8_t *pcm_ring_buf_data;
static struct k_spinlock pcm_ring_buf_lock;
static uint32_t ring_buf_dropped_bytes;
static uint32_t last_overflow_log_ms;

/* Codec thread */
static K_THREAD_STACK_DEFINE(codec_stack, CODEC_THREAD_STACK);
static struct k_thread codec_thread_data;
static k_tid_t codec_thread_id;
static struct k_sem codec_sem;

/* Work for UI updates (context-switch from codec thread) */
static uint32_t ui_elapsed_ms;
static uint8_t ui_level;

/* Timer for periodic UI updates */
static lv_timer_t *ui_timer;
static struct k_work auto_stop_work;

/* Cached list buffer to avoid large stack use in LVGL callback context */
static voice_memo_entry_t *list_entries;

static int stop_recording(void);
static int abort_recording(void);
static void refresh_list(void);
static void show_list_and_refresh_async(void *unused);

static void maybe_exit_after_quick_record(void)
{
    if (quick_record_pending_exit) {
        quick_record_pending_exit = false;
        if (app.current_state != ZSW_APP_STATE_STOPPED) {
            zsw_app_manager_exit_app();
        }
    }
}

static void show_list_and_refresh_async(void *unused)
{
    ARG_UNUSED(unused);

    if (app.current_state != ZSW_APP_STATE_STOPPED) {
        recording_screen_shown = false;
        voice_memo_ui_show_list();
        refresh_list();
    }

    maybe_exit_after_quick_record();
}

static void auto_stop_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);

    if (!is_recording && !codec_thread_running) {
        auto_stop_pending = false;
        return;
    }

    (void)stop_recording();
    auto_stop_pending = false;
    lv_async_call(show_list_and_refresh_async, NULL);
}

/* ---------- Audio level calculation ---------- */
static uint8_t calc_audio_level(const int16_t *samples, size_t count)
{
    int32_t peak = 0;
    for (size_t i = 0; i < count; i++) {
        int32_t abs_val = samples[i] < 0 ? -samples[i] : samples[i];
        if (abs_val > peak) {
            peak = abs_val;
        }
    }
    /* Map to 0-100 with some smoothing */
    uint8_t level = (uint8_t)(peak * 100 / 32768);
    if (level > 100) {
        level = 100;
    }
    return level;
}

/* ---------- Microphone callback ---------- */
static void mic_data_callback(zsw_mic_event_t event, zsw_mic_event_data_t *data,
                               void *user_data)
{
    ARG_UNUSED(user_data);

    if (event != ZSW_MIC_EVENT_RECORDING_DATA || !is_recording) {
        return;
    }

    const int16_t *pcm = (const int16_t *)data->raw_block.data;
    size_t pcm_bytes = data->raw_block.size;

    /* Compute audio level for UI */
    peak_level = calc_audio_level(pcm, pcm_bytes / sizeof(int16_t));

    /* Put PCM data into ring buffer */
    k_spinlock_key_t key = k_spin_lock(&pcm_ring_buf_lock);
    uint32_t written = ring_buf_put(&pcm_ring_buf, (const uint8_t *)pcm, pcm_bytes);
    if (written < pcm_bytes) {
        uint32_t now = k_uptime_get_32();
        uint32_t used = ring_buf_size_get(&pcm_ring_buf);
        ring_buf_dropped_bytes += (pcm_bytes - written);

        if ((now - last_overflow_log_ms) >= OVERFLOW_LOG_INTERVAL_MS) {
            LOG_WRN("voice_memo PCM overflow: used=%u/%u dropped=%u bytes",
                    used, CODEC_RING_BUF_SIZE, ring_buf_dropped_bytes);
            last_overflow_log_ms = now;
            ring_buf_dropped_bytes = 0;
        }
    }
    k_spin_unlock(&pcm_ring_buf_lock, key);

    /* Wake codec thread */
    k_sem_give(&codec_sem);
}

/* ---------- Codec thread ---------- */
static void codec_thread_fn(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    int16_t pcm_frame[CONFIG_ZSW_OPUS_FRAME_SIZE_SAMPLES];
    uint8_t opus_frame[MAX_OPUS_FRAME_BYTES];
    const size_t frame_bytes = CONFIG_ZSW_OPUS_FRAME_SIZE_SAMPLES * sizeof(int16_t);

    LOG_INF("Codec thread started");

    while (codec_thread_running) {
        /* Wait for data */
        k_sem_take(&codec_sem, K_MSEC(100));

        if (!is_recording || !codec_thread_running) {
            continue;
        }

        /* Process all available complete frames */
        while (true) {
            uint32_t got;
            k_spinlock_key_t key = k_spin_lock(&pcm_ring_buf_lock);

            if (ring_buf_size_get(&pcm_ring_buf) < frame_bytes) {
                k_spin_unlock(&pcm_ring_buf_lock, key);
                break;
            }

            got = ring_buf_get(&pcm_ring_buf, (uint8_t *)pcm_frame, frame_bytes);
            k_spin_unlock(&pcm_ring_buf_lock, key);

            if (got < frame_bytes) {
                break;
            }

            /* Encode */
            int encoded = zsw_audio_codec_encode(pcm_frame,
                                                  CONFIG_ZSW_OPUS_FRAME_SIZE_SAMPLES,
                                                  opus_frame, sizeof(opus_frame));
            if (encoded < 0) {
                LOG_ERR("Opus encode error: %d", encoded);
                continue;
            }

            /* Write to flash store */
            int ret = voice_memo_store_write_frame(opus_frame, encoded);
            if (ret < 0) {
                LOG_ERR("Store write error: %d", ret);
                /* Continue encoding to avoid ring buf overflow */
            }
        }

        /* Check max duration */
        uint32_t elapsed = k_uptime_get_32() - recording_start_time;
        if (!auto_stop_pending &&
            elapsed >= (uint32_t)CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MAX_DURATION_S * 1000) {
            LOG_INF("[TEST] voice_memo: max duration reached");
            auto_stop_pending = true;
            k_work_submit(&auto_stop_work);
        }

        /* Check free space — auto-stop if below minimum */
        uint32_t free_bytes = 0;
        if (!auto_stop_pending && voice_memo_store_get_free_space(&free_bytes) == 0) {
            if (free_bytes < (uint32_t)CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MIN_FREE_SPACE_KB * 1024) {
                LOG_WRN("[TEST] voice_memo: low space auto-stop, free=%u KB",
                        free_bytes / 1024);
                auto_stop_pending = true;
                k_work_submit(&auto_stop_work);
            }
        }
    }

    LOG_INF("Codec thread stopped");
}

/* ---------- Start/Stop recording ---------- */
static int start_recording(void)
{
    int ret;

    if (is_recording) {
        return -EALREADY;
    }

    /* Allocate ring buffer from heap */
    if (!pcm_ring_buf_data) {
        pcm_ring_buf_data = k_malloc(CODEC_RING_BUF_SIZE);
        if (!pcm_ring_buf_data) {
            LOG_ERR("Failed to allocate %d bytes for ring buffer", CODEC_RING_BUF_SIZE);
            return -ENOMEM;
        }
        ring_buf_init(&pcm_ring_buf, CODEC_RING_BUF_SIZE, pcm_ring_buf_data);
    }

    /* Initialize codec if needed */
    ret = zsw_audio_codec_init();
    if (ret < 0) {
        LOG_ERR("Codec init failed: %d", ret);
        return ret;
    }
    zsw_audio_codec_reset();

    /* Initialize store */
    ret = voice_memo_store_init();
    if (ret < 0) {
        LOG_ERR("Store init failed: %d", ret);
        return ret;
    }

    /* Start recording to file */
    ret = voice_memo_store_start_recording();
    if (ret < 0) {
        LOG_ERR("Store start failed: %d", ret);
        return ret;
    }

    /* Clear ring buffer */
    ring_buf_reset(&pcm_ring_buf);

    /* Start codec thread */
    codec_thread_running = true;
    is_recording = true;
    recording_screen_shown = true;
    auto_stop_pending = false;
    recording_start_time = k_uptime_get_32();
    ring_buf_dropped_bytes = 0;
    last_overflow_log_ms = 0;

    k_sem_init(&codec_sem, 0, 1);
    codec_thread_id = k_thread_create(&codec_thread_data, codec_stack,
                                       CODEC_THREAD_STACK,
                                       codec_thread_fn, NULL, NULL, NULL,
                                       CODEC_THREAD_PRIO, 0, K_NO_WAIT);
    k_thread_name_set(codec_thread_id, "voice_codec");

    /* Start microphone */
    zsw_mic_config_t mic_cfg;
    zsw_microphone_manager_get_default_config(&mic_cfg);
    mic_cfg.output = ZSW_MIC_OUTPUT_RAW;
    mic_cfg.duration_ms = 0;  /* Unlimited */

    ret = zsw_microphone_manager_start_recording(&mic_cfg, mic_data_callback, NULL);
    if (ret < 0) {
        LOG_ERR("Mic start failed: %d", ret);
        is_recording = false;
        codec_thread_running = false;
        k_thread_abort(codec_thread_id);
        voice_memo_store_stop_recording(NULL, NULL);
        return ret;
    }

    LOG_INF("[TEST] voice_memo: pipeline started");
    return 0;
}

static int stop_recording(void)
{
    uint32_t duration_ms = 0;
    uint32_t size_bytes = 0;

    if (!is_recording && !codec_thread_running) {
        return -EINVAL;
    }

    /* Stop microphone first */
    zsw_microphone_stop_recording();

    /* Signal codec thread to stop */
    is_recording = false;
    k_sem_give(&codec_sem);  /* Wake it up */
    k_msleep(50);

    codec_thread_running = false;
    k_sem_give(&codec_sem);
    k_thread_join(codec_thread_id, K_MSEC(500));

    /* Flush and finalize store */
    voice_memo_store_flush();

    /* Save filename before stop (get_current_filename returns NULL after stop) */
    const char *rec_filename = voice_memo_store_get_current_filename();
    char saved_filename[VOICE_MEMO_MAX_FILENAME];
    if (rec_filename) {
        strncpy(saved_filename, rec_filename, sizeof(saved_filename) - 1);
        saved_filename[sizeof(saved_filename) - 1] = '\0';
    } else {
        saved_filename[0] = '\0';
    }

    voice_memo_store_stop_recording(&duration_ms, &size_bytes);

    /* Free dynamic buffers */
    if (pcm_ring_buf_data) {
        k_free(pcm_ring_buf_data);
        pcm_ring_buf_data = NULL;
    }
    auto_stop_pending = false;
    zsw_audio_codec_deinit();

    LOG_INF("[TEST] voice_memo: pipeline stopped, duration=%u ms, size=%u bytes",
            duration_ms, size_bytes);

    /* Notify companion app about the new recording over BLE */
    if (saved_filename[0] != '\0' && duration_ms > 0 && size_bytes > 0) {
        ble_gadgetbridge_send_voice_memo_new(saved_filename, duration_ms, size_bytes,
                                            voice_memo_store_get_unix_timestamp());
    }

    return 0;
}

/** Abort current recording: discard audio, delete temp file, free resources. */
static int abort_recording(void)
{
    if (!is_recording && !codec_thread_running) {
        return -EINVAL;
    }

    /* Stop microphone first */
    zsw_microphone_stop_recording();

    /* Signal codec thread to stop */
    is_recording = false;
    k_sem_give(&codec_sem);
    k_msleep(50);

    codec_thread_running = false;
    k_sem_give(&codec_sem);
    k_thread_join(codec_thread_id, K_MSEC(500));

    /* Abort the store — closes file and deletes it */
    voice_memo_store_abort_recording();

    /* Free dynamic buffers */
    if (pcm_ring_buf_data) {
        k_free(pcm_ring_buf_data);
        pcm_ring_buf_data = NULL;
    }
    auto_stop_pending = false;
    zsw_audio_codec_deinit();

    LOG_INF("[TEST] voice_memo: recording aborted (discarded)");
    return 0;
}

/* ---------- UI callbacks ---------- */
static void on_undo(const char *filename);
static void on_start_recording(void)
{
    int ret = start_recording();
    if (ret == 0) {
        recording_screen_shown = true;
        voice_memo_ui_show_recording();
    } else {
        LOG_ERR("Failed to start recording: %d", ret);
    }
}

static void on_stop_recording(void)
{
    stop_recording();
    recording_screen_shown = false;
    voice_memo_ui_show_list();
    refresh_list();
    maybe_exit_after_quick_record();
}

static void on_delete(const char *filename)
{
    voice_memo_store_delete(filename);
    refresh_list();
}

static void on_back_during_recording(bool save)
{
    if (save) {
        stop_recording();
        recording_screen_shown = false;
        voice_memo_ui_show_list();
        refresh_list();
    } else {
        /* Discard — abort the recording entirely */
        abort_recording();
        recording_screen_shown = false;
        voice_memo_ui_show_list();
        refresh_list();
    }

    maybe_exit_after_quick_record();
}

static void refresh_list(void)
{
    if (!list_entries) {
        list_entries = k_malloc(sizeof(*list_entries) *
                               CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MAX_FILES);
        if (!list_entries) {
            LOG_ERR("Failed to allocate voice memo list buffer");
            voice_memo_ui_update_list(NULL, 0, 0);
            return;
        }
    }

    int count = voice_memo_store_list(list_entries,
                                      CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MAX_FILES);
    uint32_t free_bytes = 0;
    voice_memo_store_get_free_space(&free_bytes);

    if (count < 0) {
        count = 0;
    } else if (count > CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MAX_FILES) {
        count = CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MAX_FILES;
    }

    voice_memo_ui_update_list(list_entries, count, free_bytes / 1024);
}

static const voice_memo_ui_callbacks_t ui_callbacks = {
    .on_start_recording = on_start_recording,
    .on_stop_recording = on_stop_recording,
    .on_delete = on_delete,
    .on_back_during_recording = on_back_during_recording,
    .on_undo = on_undo,
};

/* ---------- LVGL timer for periodic UI update ---------- */
static void ui_timer_cb(lv_timer_t *timer)
{
    LV_UNUSED(timer);
    if (app.current_state != ZSW_APP_STATE_UI_VISIBLE) {
        return;
    }

    if (!is_recording) {
        if (recording_screen_shown && !codec_thread_running) {
            show_list_and_refresh_async(NULL);
        }
        return;
    }

    ui_elapsed_ms = k_uptime_get_32() - recording_start_time;
    ui_level = (uint8_t)peak_level;
    voice_memo_ui_update_time(ui_elapsed_ms);
    voice_memo_ui_update_level(ui_level);
}

/* ---------- App lifecycle ---------- */
static void voice_memo_app_start(lv_obj_t *root, lv_group_t *group)
{
    ARG_UNUSED(group);

    ui_visible = true;
    recording_screen_shown = false;
    auto_stop_pending = false;

    /* Init store on first launch */
    voice_memo_store_init();

    voice_memo_ui_show(root, &ui_callbacks);

    /* Refresh the recording list */
    refresh_list();

    /* Periodic UI update timer */
    ui_timer = lv_timer_create(ui_timer_cb, UI_UPDATE_INTERVAL_MS, NULL);
    k_work_init(&auto_stop_work, auto_stop_work_fn);

    LOG_INF("Voice Memo app started");
}

static void voice_memo_app_stop(void)
{
    /* Stop recording if active */
    if (is_recording || codec_thread_running) {
        stop_recording();
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
    ui_visible = false;
    LOG_INF("Voice Memo app stopped");
}

static bool voice_memo_app_back(void)
{
    if (is_recording) {
        voice_memo_ui_show_back_confirm();
        return true;
    }
    return false;
}

static void voice_memo_app_ui_unavailable(void)
{
    ui_visible = false;
    /* Recording continues, just stop UI updates */
}

static void voice_memo_app_ui_available(void)
{
    ui_visible = true;
    if (is_recording) {
        voice_memo_ui_show_recording();
    } else {
        refresh_list();
    }
}

/* ---------- SYS_INIT registration ---------- */
static int voice_memo_app_add(void)
{
    zsw_app_manager_add_application(&app);
    k_work_init(&toast_work, show_toast_work_fn);
    return 0;
}

SYS_INIT(voice_memo_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/* ---------- Shell API (called from zsw_shell.c) ---------- */
int voice_memo_shell_start(void)
{
    return start_recording();
}

int voice_memo_shell_stop(void)
{
    int ret = stop_recording();

    if (ret == 0) {
        lv_async_call(show_list_and_refresh_async, NULL);
    }

    return ret;
}

/** Mark that the app should auto-exit after the next stop. */
void voice_memo_set_quick_record_exit(void)
{
    quick_record_pending_exit = true;
}

/* ---------- Round-trip confirmation toast ---------- */

static char pending_toast_title[128];
static char pending_toast_filename[VOICE_MEMO_MAX_FILENAME];
static struct k_work toast_work;

static void on_undo(const char *filename)
{
    LOG_INF("Undo requested for: %s", filename);
    ble_gadgetbridge_send_voice_memo_undo(filename);
}

static void show_toast_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    zsw_power_manager_reset_idle_timout();
    voice_memo_ui_show_result_toast(pending_toast_title, pending_toast_filename);
}

/**
 * Called from BLE context when the phone sends a voice_memo result.
 * Context-switches to the main thread to safely update LVGL UI.
 */
void voice_memo_show_result_toast(const char *title, const char *filename)
{
    strncpy(pending_toast_title, title, sizeof(pending_toast_title) - 1);
    pending_toast_title[sizeof(pending_toast_title) - 1] = '\0';
    strncpy(pending_toast_filename, filename, sizeof(pending_toast_filename) - 1);
    pending_toast_filename[sizeof(pending_toast_filename) - 1] = '\0';

    k_work_submit(&toast_work);
}
