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
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <string.h>

#include "zsw_recording_manager.h"
#include "zsw_recording_manager_store.h"
#include "zsw_microphone_manager.h"
#include "zsw_audio_codec.h"
#include "events/zsw_voice_memo_event.h"

LOG_MODULE_REGISTER(zsw_recording_manager, CONFIG_ZSW_VOICE_MEMO_LOG_LEVEL);

#define CODEC_RING_BUF_SIZE    2048
// Opus encoder uses ~8-10 KB stack during opus_encode() on ARM.
#define CODEC_THREAD_STACK     12288
#define CODEC_THREAD_PRIO      K_PRIO_PREEMPT(5)
#define MAX_OPUS_FRAME_BYTES   160
#define OVERFLOW_LOG_INTERVAL_MS 1000

ZBUS_CHAN_DECLARE(voice_memo_recording_chan);

static bool is_recording;
static bool codec_thread_running;
static uint32_t recording_start_time;
static uint32_t peak_level;
static bool auto_stop_pending;

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

static struct k_work auto_stop_work;

/** Return audio level 0-100 as peak sample amplitude percentage of full-scale (INT16_MAX). */
static uint8_t calc_audio_level(const int16_t *samples, size_t count)
{
    int32_t peak = 0;
    for (size_t i = 0; i < count; i++) {
        int32_t abs_val = samples[i] < 0 ? -samples[i] : samples[i];
        if (abs_val > peak) {
            peak = abs_val;
        }
    }
    uint8_t level = (uint8_t)(peak * 100 / 32768);
    if (level > 100) {
        level = 100;
    }
    return level;
}

static void mic_data_callback(zsw_mic_event_t event, zsw_mic_event_data_t *data,
                              void *user_data)
{
    ARG_UNUSED(user_data);
    if (event != ZSW_MIC_EVENT_RECORDING_DATA || !is_recording) {
        return;
    }
    const int16_t *pcm = (const int16_t *)data->raw_block.data;
    size_t pcm_bytes = data->raw_block.size;
    peak_level = calc_audio_level(pcm, pcm_bytes / sizeof(int16_t));
    k_spinlock_key_t key = k_spin_lock(&pcm_ring_buf_lock);
    uint32_t written = ring_buf_put(&pcm_ring_buf, (const uint8_t *)pcm, pcm_bytes);
    if (written < pcm_bytes) {
        uint32_t now = k_uptime_get_32();
        uint32_t used = ring_buf_size_get(&pcm_ring_buf);
        ring_buf_dropped_bytes += (pcm_bytes - written);
        if ((now - last_overflow_log_ms) >= OVERFLOW_LOG_INTERVAL_MS) {
            LOG_WRN("PCM overflow: used=%u/%u dropped=%u bytes",
                    used, CODEC_RING_BUF_SIZE, ring_buf_dropped_bytes);
            last_overflow_log_ms = now;
            ring_buf_dropped_bytes = 0;
        }
    }
    k_spin_unlock(&pcm_ring_buf_lock, key);
    k_sem_give(&codec_sem);
}

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
        k_sem_take(&codec_sem, K_MSEC(100));
        if (!is_recording || !codec_thread_running) {
            continue;
        }
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
            int encoded = zsw_audio_codec_encode(pcm_frame,
                                                 CONFIG_ZSW_OPUS_FRAME_SIZE_SAMPLES,
                                                 opus_frame, sizeof(opus_frame));
            if (encoded < 0) {
                LOG_ERR("Opus encode error: %d", encoded);
                continue;
            }
            int ret = zsw_recording_manager_store_write_frame(opus_frame, encoded);
            if (ret < 0) {
                LOG_ERR("Store write error: %d, stopping recording", ret);
                if (!auto_stop_pending) {
                    auto_stop_pending = true;
                    k_work_submit(&auto_stop_work);
                }
                break;
            }
        }
        uint32_t elapsed = k_uptime_get_32() - recording_start_time;
        if (!auto_stop_pending &&
            elapsed >= (uint32_t)ZSW_RECORDING_MAX_DURATION_S * 1000) {
            LOG_INF("Voice memo: max duration reached");
            auto_stop_pending = true;
            k_work_submit(&auto_stop_work);
        }
        uint32_t free_bytes = 0;
        if (!auto_stop_pending && zsw_recording_manager_store_get_free_space(&free_bytes) == 0) {
            if (free_bytes < (uint32_t)ZSW_RECORDING_MIN_FREE_SPACE_KB * 1024) {
                LOG_WRN("Voice memo: low space auto-stop, free=%u KB",
                        free_bytes / 1024);
                auto_stop_pending = true;
                k_work_submit(&auto_stop_work);
            }
        }
    }
    LOG_INF("Codec thread stopped");
}

static void auto_stop_work_fn(struct k_work *work)
{
    ARG_UNUSED(work);
    if (!is_recording && !codec_thread_running) {
        auto_stop_pending = false;
        return;
    }
    (void)zsw_recording_manager_stop();
    auto_stop_pending = false;
}

int zsw_recording_manager_init(void)
{
    k_work_init(&auto_stop_work, auto_stop_work_fn);
    return zsw_recording_manager_store_init();
}

int zsw_recording_manager_start(void)
{
    int ret;

    if (is_recording) {
        return -EALREADY;
    }

    if (!pcm_ring_buf_data) {
        pcm_ring_buf_data = k_malloc(CODEC_RING_BUF_SIZE);
        if (!pcm_ring_buf_data) {
            LOG_ERR("Failed to allocate %d bytes for ring buffer", CODEC_RING_BUF_SIZE);
            return -ENOMEM;
        }
        ring_buf_init(&pcm_ring_buf, CODEC_RING_BUF_SIZE, pcm_ring_buf_data);
    }

    ret = zsw_audio_codec_init();
    if (ret < 0) {
        LOG_ERR("Codec init failed: %d", ret);
        return ret;
    }
    zsw_audio_codec_reset();

    ret = zsw_recording_manager_store_init();
    if (ret < 0) {
        LOG_ERR("Store init failed: %d", ret);
        zsw_audio_codec_deinit();
        return ret;
    }

    ret = zsw_recording_manager_store_start_recording();
    if (ret < 0) {
        LOG_ERR("Store start failed: %d", ret);
        zsw_audio_codec_deinit();
        return ret;
    }

    ring_buf_reset(&pcm_ring_buf);
    codec_thread_running = true;
    is_recording = true;
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

    zsw_mic_config_t mic_cfg;
    zsw_microphone_manager_get_default_config(&mic_cfg);
    mic_cfg.output = ZSW_MIC_OUTPUT_RAW;
    mic_cfg.duration_ms = 0;
    ret = zsw_microphone_manager_start_recording(&mic_cfg, mic_data_callback, NULL);
    if (ret < 0) {
        LOG_ERR("Mic start failed: %d", ret);
        is_recording = false;
        codec_thread_running = false;
        k_thread_abort(codec_thread_id);
        zsw_audio_codec_deinit();
        zsw_recording_manager_store_abort_recording();
        return ret;
    }

    LOG_INF("Voice memo pipeline started");

    struct zsw_voice_memo_recording_event evt = {
        .state = ZSW_VOICE_MEMO_RECORDING_STARTED,
    };
    zbus_chan_pub(&voice_memo_recording_chan, &evt, K_MSEC(50));

    return 0;
}

static void shutdown_pipeline(void)
{
    zsw_microphone_stop_recording();
    is_recording = false;
    k_sem_give(&codec_sem);
    k_msleep(50);
    codec_thread_running = false;
    k_sem_give(&codec_sem);
    k_thread_join(codec_thread_id, K_MSEC(500));

    if (pcm_ring_buf_data) {
        k_free(pcm_ring_buf_data);
        pcm_ring_buf_data = NULL;
    }
    auto_stop_pending = false;
    zsw_audio_codec_deinit();
}

int zsw_recording_manager_stop(void)
{
    uint32_t duration_ms = 0;
    uint32_t size_bytes = 0;

    if (!is_recording && !codec_thread_running) {
        return -EINVAL;
    }

    shutdown_pipeline();
    zsw_recording_manager_store_flush();

    const char *rec_filename = zsw_recording_manager_store_get_current_filename();
    char saved_filename[VOICE_MEMO_MAX_FILENAME];
    if (rec_filename) {
        strncpy(saved_filename, rec_filename, sizeof(saved_filename) - 1);
        saved_filename[sizeof(saved_filename) - 1] = '\0';
    } else {
        saved_filename[0] = '\0';
    }

    int store_ret = zsw_recording_manager_store_stop_recording(&duration_ms, &size_bytes);
    if (store_ret < 0) {
        LOG_ERR("Store stop failed: %d", store_ret);
    }

    LOG_INF("Voice memo pipeline stopped, duration=%u ms, size=%u bytes",
            duration_ms, size_bytes);

    if (store_ret == 0 && saved_filename[0] != '\0' && duration_ms > 0 && size_bytes > 0) {
        struct zsw_voice_memo_recording_event evt = {
            .state = ZSW_VOICE_MEMO_RECORDING_STOPPED,
            .duration_ms = duration_ms,
            .size_bytes = size_bytes,
            .timestamp = zsw_recording_manager_store_get_unix_timestamp(),
        };
        strncpy(evt.filename, saved_filename, sizeof(evt.filename) - 1);
        evt.filename[sizeof(evt.filename) - 1] = '\0';
        zbus_chan_pub(&voice_memo_recording_chan, &evt, K_MSEC(50));
    }

    return 0;
}

int zsw_recording_manager_abort(void)
{
    if (!is_recording && !codec_thread_running) {
        return -EINVAL;
    }

    shutdown_pipeline();
    zsw_recording_manager_store_abort_recording();

    LOG_INF("Voice memo recording aborted");

    struct zsw_voice_memo_recording_event evt = {
        .state = ZSW_VOICE_MEMO_RECORDING_ABORTED,
    };
    zbus_chan_pub(&voice_memo_recording_chan, &evt, K_MSEC(50));

    return 0;
}

bool zsw_recording_manager_is_recording(void)
{
    return is_recording;
}

uint8_t zsw_recording_manager_get_audio_level(void)
{
    return (uint8_t)peak_level;
}

uint32_t zsw_recording_manager_get_elapsed_ms(void)
{
    if (!is_recording) {
        return 0;
    }
    return k_uptime_get_32() - recording_start_time;
}

int zsw_recording_manager_list(zsw_recording_entry_t *entries, size_t max_entries)
{
    return zsw_recording_manager_store_list(entries, max_entries);
}

int zsw_recording_manager_delete(const char *filename)
{
    return zsw_recording_manager_store_delete(filename);
}

int zsw_recording_manager_get_free_space(uint32_t *free_bytes)
{
    return zsw_recording_manager_store_get_free_space(free_bytes);
}

int zsw_recording_manager_get_count(void)
{
    return zsw_recording_manager_store_get_count();
}
