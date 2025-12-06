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

#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <zephyr/kernel.h>

#include <string.h>
#include <stdio.h>

#include "zsw_microphone_manager.h"
#include "drivers/zsw_microphone.h"

#if CONFIG_ZSW_MIC_SEND_READING_OVER_RTT
#include <SEGGER_RTT.h>

static uint8_t rtt_buffer[CONFIG_ZSW_MIC_LOG_RTT_TBUFFER_SIZE];
#endif

LOG_MODULE_REGISTER(zsw_mic_manager, CONFIG_ZSW_MIC_MANAGER_LOG_LEVEL);

typedef enum {
    ZSW_MIC_STATE_IDLE,          /**< Manager is idle, ready to start recording */
    ZSW_MIC_STATE_RECORDING      /**< Currently recording audio */
} mic_state_t;

static struct {
    mic_state_t state;
    zsw_mic_config_t config;
    zsw_mic_event_cb_t callback;
    void *user_data;
    uint32_t recorded_ms;
    struct fs_file_t file;
    bool file_open;
    char filename_buffer[64];
} mic_manager;

static void timeout_work_handler(struct k_work *work);
static void mic_audio_callback(void *audio_data, size_t size);
static int open_output_file(const char *filename);
static void close_output_file(void);
static int init_rtt_for_audio(void);

K_WORK_DELAYABLE_DEFINE(timeout_work, timeout_work_handler);

static int init_rtt_for_audio(void)
{
#if CONFIG_ZSW_MIC_SEND_READING_OVER_RTT
    int ret = SEGGER_RTT_ConfigUpBuffer(CONFIG_ZSW_MIC_LOG_RTT_TRANSFER_CHANNEL, "ZSW_MIC",
                                        rtt_buffer, ARRAY_SIZE(rtt_buffer),
                                        SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    if (ret < 0) {
        LOG_ERR("Failed to configure RTT buffer: %d", ret);
        return -EIO;
    }

    LOG_DBG("RTT audio channel %d configured", CONFIG_ZSW_MIC_LOG_RTT_TRANSFER_CHANNEL);
#endif
    return 0;
}

int zsw_microphone_manager_init(void)
{
    LOG_INF("Initializing microphone manager");

    memset(&mic_manager, 0, sizeof(mic_manager));
    mic_manager.state = ZSW_MIC_STATE_IDLE;

    int ret = zsw_microphone_init(mic_audio_callback);
    if (ret < 0) {
        LOG_ERR("Failed to initialize microphone driver: %d", ret);
        return ret;
    }

    LOG_INF("Microphone manager initialized successfully");
    return 0;
}

int zsw_microphone_manager_start_recording(const zsw_mic_config_t *config,
                                           zsw_mic_event_cb_t callback,
                                           void *user_data)
{
    int ret;

    if (!config) {
        LOG_ERR("Configuration is NULL");
        return -EINVAL;
    }

    if (mic_manager.state != ZSW_MIC_STATE_IDLE) {
        LOG_WRN("Microphone is busy (state: %d)", mic_manager.state);
        return -EBUSY;
    }

    LOG_INF("Starting recording: duration=%dms, output=%d",
            config->duration_ms, config->output);

    mic_manager.config = *config;
    mic_manager.callback = callback;
    mic_manager.user_data = user_data;
    mic_manager.recorded_ms = 0;
    mic_manager.state = ZSW_MIC_STATE_RECORDING;

    switch (config->output) {
        case ZSW_MIC_OUTPUT_FILE:
            if (config->filename) {
                strncpy(mic_manager.filename_buffer, config->filename,
                        sizeof(mic_manager.filename_buffer) - 1);
            } else {
                LOG_ERR("No filename provided");
                return -EINVAL;
            }

            ret = open_output_file(mic_manager.filename_buffer);
            if (ret < 0) {
                LOG_ERR("Failed to open output file: %d", ret);
                return ret;
            }
            break;
        case ZSW_MIC_OUTPUT_RTT:
            ret = init_rtt_for_audio();
            if (ret < 0) {
                LOG_ERR("Failed to initialize RTT for recording: %d", ret);
                return ret;
            }
            break;
        case ZSW_MIC_OUTPUT_RAW:
            // Raw mode handled in callback
            break;
        case ZSW_MIC_OUTPUT_BLE:
            LOG_WRN("BLE output not implemented yet");
            return -ENOTSUP;
        default:
            LOG_ERR("Unsupported output mode: %d", config->output);
            return -EINVAL;
    }

    ret = zsw_microphone_driver_start();
    if (ret < 0) {
        LOG_ERR("Failed to start microphone recording: %d", ret);
        close_output_file();
        return ret;
    }

    // Set timeout if duration is specified (0 = infinite recording)
    if (config->duration_ms > 0) {
        LOG_INF("Recording will timeout after %d ms", config->duration_ms);
        k_work_schedule(&timeout_work, K_MSEC(config->duration_ms));
    } else {
        LOG_INF("Recording set to infinite duration - stop manually");
    }

    LOG_INF("Recording started successfully");
    return 0;
}

int zsw_microphone_stop_recording(void)
{
    if (mic_manager.state != ZSW_MIC_STATE_RECORDING) {
        LOG_WRN("Not currently recording (state: %d)", mic_manager.state);
        return -EINVAL;
    }

    LOG_INF("Stopping recording");

    k_work_cancel_delayable(&timeout_work);

    zsw_microphone_driver_stop();

    close_output_file();

    mic_manager.state = ZSW_MIC_STATE_IDLE;

    LOG_INF("Recording stopped, total duration: %dms", mic_manager.recorded_ms);
    return 0;
}

bool zsw_microphone_manager_is_recording(void)
{
    // Not thread safe, but ok for now
    return mic_manager.state == ZSW_MIC_STATE_RECORDING;
}

void zsw_microphone_manager_get_default_config(zsw_mic_config_t *config)
{
    if (!config) {
        return;
    }

    config->duration_ms = 0;
    config->sample_rate = 16000;
    config->bit_depth = 16;
    config->output = ZSW_MIC_OUTPUT_RTT;
    config->filename = NULL;
}

static void timeout_work_handler(struct k_work *work)
{
    LOG_INF("Recording timeout reached");

    if (mic_manager.state == ZSW_MIC_STATE_RECORDING) {
        zsw_microphone_stop_recording();

        mic_manager.callback(ZSW_MIC_EVENT_RECORDING_TIMEOUT, NULL, mic_manager.user_data);
    }
}

static void mic_audio_callback(void *audio_data, size_t size)
{
    if (mic_manager.state != ZSW_MIC_STATE_RECORDING) {
        LOG_WRN("Audio callback called but not recording (state: %d)", mic_manager.state);
        return;
    }

    if (!audio_data) {
        LOG_ERR("Audio data pointer is NULL!");
        return;
    }

    if (size == 0 || size > 1024) {
        LOG_ERR("Invalid audio data size: %d", size);
        return;
    }

    // Calculate duration dynamically based on sample rate, bit depth, and block size
    uint32_t bytes_per_sample = mic_manager.config.bit_depth / 8;
    uint32_t num_samples = (bytes_per_sample > 0) ? (size / bytes_per_sample) : 0;
    uint32_t duration_ms = (mic_manager.config.sample_rate > 0) ?
                           (num_samples * 1000) / mic_manager.config.sample_rate : 0;
    mic_manager.recorded_ms += duration_ms;

    switch (mic_manager.config.output) {
        case ZSW_MIC_OUTPUT_RTT:
#if CONFIG_ZSW_MIC_SEND_READING_OVER_RTT
            SEGGER_RTT_Write(CONFIG_ZSW_MIC_LOG_RTT_TRANSFER_CHANNEL, audio_data, size);
#endif
            break;

        case ZSW_MIC_OUTPUT_FILE:
            if (mic_manager.file_open) {
                ssize_t written = fs_write(&mic_manager.file, audio_data, size);
                if (written < 0) {
                    LOG_ERR("Failed to write to file: %d", (int)written);
                }
            }
            break;

        case ZSW_MIC_OUTPUT_RAW:
            if (mic_manager.callback) {
                zsw_mic_event_data_t data;
                data.raw_block.data = audio_data;
                data.raw_block.size = size;
                mic_manager.callback(ZSW_MIC_EVENT_RECORDING_DATA, &data,
                                     mic_manager.user_data);
            }
            break;

        case ZSW_MIC_OUTPUT_BLE:
            // TODO: Implement BLE streaming in future phase
            LOG_WRN("BLE output not implemented yet");
            break;
    }
}

static int open_output_file(const char *filename)
{
    int ret = fs_open(&mic_manager.file, filename, FS_O_CREATE | FS_O_WRITE);
    if (ret < 0) {
        LOG_ERR("Failed to open file %s: %d", filename, ret);
        return ret;
    }

    mic_manager.file_open = true;
    LOG_INF("Opened file: %s", filename);
    return 0;
}

static void close_output_file(void)
{
    if (mic_manager.file_open) {
        fs_close(&mic_manager.file);
        mic_manager.file_open = false;
        LOG_INF("Closed output file");
    }
}
