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
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include "mic_app_ui.h"
#include "spectrum_analyzer.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_microphone_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(mic_app, LOG_LEVEL_DBG);

static void mic_app_start(lv_obj_t *root, lv_group_t *group);
static void mic_app_stop(void);

static struct k_work spectrum_update_work;
static void spectrum_update_work_handler(struct k_work *work);

static uint8_t spectrum_magnitudes[NUM_SPECTRUM_BARS];
static int16_t audio_samples[SPECTRUM_FFT_SIZE];
static float current_gain = 1.0f;
static bool rtt_output_enabled = false;

static void on_play_stop_toggle(void);
static void on_gain_changed(float new_gain);
static void on_rtt_output_toggled(bool rtt_output);
static void mic_event_callback(zsw_mic_event_t event, zsw_mic_event_data_t *data, void *user_data);

ZSW_LV_IMG_DECLARE(statistic_icon);

static application_t app = {
    .name = "Mic",
    .icon = ZSW_LV_IMG_USE(statistic_icon),
    .start_func = mic_app_start,
    .stop_func = mic_app_stop,
};

static size_t sample_buffer_index = 0;

static void mic_app_start(lv_obj_t *root, lv_group_t *group)
{
    k_work_init(&spectrum_update_work, spectrum_update_work_handler);

    int ret = spectrum_analyzer_init();
    if (ret < 0) {
        LOG_ERR("Failed to initialize spectrum analyzer: %d", ret);
    }

    mic_app_ui_create(root, on_play_stop_toggle, on_gain_changed, on_rtt_output_toggled, current_gain);
    LOG_INF("Circular spectrum watch UI created");

    sample_buffer_index = 0;
    LOG_INF("Microphone app started");
}

static void mic_app_stop(void)
{
    k_work_cancel(&spectrum_update_work);

    if (zsw_microphone_manager_is_recording()) {
        zsw_microphone_stop_recording();
    }

    mic_app_ui_remove();
    spectrum_analyzer_cleanup();

    LOG_INF("Microphone app stopped");
}

static void on_play_stop_toggle(void)
{
    if (zsw_microphone_manager_is_recording()) {
        LOG_INF("Stopping microphone recording");
        mic_app_ui_set_status("Stopping...");
        int ret = zsw_microphone_stop_recording();
        if (ret < 0) {
            LOG_ERR("Failed to stop recording: %d", ret);
            mic_app_ui_set_status("Stop Failed!");
        } else {
            mic_app_ui_set_status("Ready");
            mic_app_ui_set_recording(false);
            LOG_INF("Recording stopped successfully");
        }
    } else {
        LOG_INF("Starting microphone recording");

        if (zsw_microphone_manager_is_recording()) {
            LOG_WRN("Microphone is already busy");
            return;
        }

        sample_buffer_index = 0;
        memset(audio_samples, 0, sizeof(audio_samples));

        int ret;
        zsw_mic_config_t config;
        zsw_microphone_manager_get_default_config(&config);
        config.duration_ms = 0; // Continuous recording
        config.output = rtt_output_enabled ? ZSW_MIC_OUTPUT_RTT : ZSW_MIC_OUTPUT_RAW;

        mic_app_ui_set_status("Starting...");

        ret = zsw_microphone_manager_start_recording(&config, mic_event_callback, NULL);
        if (ret < 0) {
            LOG_ERR("Failed to start recording: %d", ret);
            mic_app_ui_set_status("Start Failed!");
        } else {
            mic_app_ui_set_status("Recording...");
            mic_app_ui_set_recording(true);
            LOG_INF("Recording started successfully");
        }
    }
}

static void on_gain_changed(float new_gain)
{
    current_gain = new_gain;
}

static void on_rtt_output_toggled(bool rtt_output)
{
    rtt_output_enabled = rtt_output;
    LOG_INF("RTT output %s", rtt_output ? "enabled" : "disabled");
}

static void mic_event_callback(zsw_mic_event_t event, zsw_mic_event_data_t *data, void *user_data)
{
    if (app.current_state == ZSW_APP_STATE_STOPPED) {
        return;
    }

    switch (event) {
        case ZSW_MIC_EVENT_RECORDING_DATA:
            if (data) {
                zsw_mic_raw_block_t *block = &data->raw_block;
                // Process audio data for spectrum analysis
                int16_t *samples = (int16_t *)block->data;
                size_t num_samples = block->size / sizeof(int16_t);

                // Accumulate samples until we have enough for FFT
                for (size_t i = 0; i < num_samples && sample_buffer_index < SPECTRUM_FFT_SIZE; i++) {
                    audio_samples[sample_buffer_index++] = samples[i];
                }

                // Process when buffer is full
                if (sample_buffer_index >= SPECTRUM_FFT_SIZE) {
                    // Process for circular spectrum UI
                    int ret = spectrum_analyzer_process(audio_samples, SPECTRUM_FFT_SIZE,
                                                        spectrum_magnitudes, NUM_SPECTRUM_BARS, current_gain);
                    if (ret == 0) {
                        // Submit work to update UI from main thread
                        k_work_submit(&spectrum_update_work);
                    }
                    sample_buffer_index = 0;
                }
            }
            break;
        case ZSW_MIC_EVENT_RECORDING_TIMEOUT:
            LOG_INF("Recording timeout");
            break;
        default:
            break;
    }
}

static void spectrum_update_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        mic_app_ui_update_spectrum(spectrum_magnitudes, NUM_SPECTRUM_BARS);
    }
}

static int mic_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(mic_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
