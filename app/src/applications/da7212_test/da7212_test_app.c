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
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <zephyr/sys/util.h>
#include <errno.h>
#include <math.h>

#include "managers/zsw_app_manager.h"
#include "managers/zsw_microphone_manager.h"
#include "managers/zsw_speaker_manager.h"
#include "drivers/zsw_microphone.h"
#include "da7212_test_ui.h"

LOG_MODULE_REGISTER(da7212_test, LOG_LEVEL_INF);

#define SPEAKER_SAMPLE_FREQUENCY     48000
#define MIC_SAMPLE_FREQUENCY         16000
#define MIC_PLAYBACK_RATIO           (SPEAKER_SAMPLE_FREQUENCY / MIC_SAMPLE_FREQUENCY)
#define VOICE_DEMO_RECORD_DURATION_MS 3000
#define VOICE_DEMO_FILE_PATH         "/user/da7212_demo.pcm"
#define PLAYBACK_READ_SAMPLES        160
#define VOICE_DEMO_MIC_GAIN          0x50
#define VOICE_PLAYBACK_GAIN          4

#define NOTE_C4   262
#define NOTE_CS4  277
#define NOTE_D4   294
#define NOTE_DS4  311
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_FS4  370
#define NOTE_G4   392
#define NOTE_GS4  415
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_D5   587
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_FS5  740
#define NOTE_G5   784
#define NOTE_A5   880
#define NOTE_B5   988
#define NOTE_C6   1047
#define NOTE_REST 0

#define SINE_AMPLITUDE 32700

BUILD_ASSERT((SPEAKER_SAMPLE_FREQUENCY % MIC_SAMPLE_FREQUENCY) == 0,
             "Mic playback ratio must be an integer");

struct melody_note {
    uint16_t freq_hz;
    uint16_t duration_ms;
};

typedef enum {
    DA7212_TEST_STATE_IDLE,
    DA7212_TEST_STATE_PLAYING_MELODY,
    DA7212_TEST_STATE_RECORDING_VOICE,
    DA7212_TEST_STATE_PLAYING_VOICE,
} da7212_test_state_t;

static const struct melody_note melody[] = {
    /* Super Mario Bros theme */
    {NOTE_REST, 500},
    {NOTE_E5, 150}, {NOTE_E5, 150}, {NOTE_REST, 150}, {NOTE_E5, 150},
    {NOTE_REST, 150}, {NOTE_C5, 150}, {NOTE_E5, 300},
    {NOTE_G5, 300}, {NOTE_REST, 300},
    {NOTE_G4, 300}, {NOTE_REST, 300},
    {NOTE_C5, 300}, {NOTE_REST, 150}, {NOTE_G4, 300},
    {NOTE_REST, 150}, {NOTE_E4, 300},
    {NOTE_REST, 150}, {NOTE_A4, 300}, {NOTE_B4, 300},
    {NOTE_AS4, 150}, {NOTE_A4, 300},
    {NOTE_G4, 200}, {NOTE_E5, 200}, {NOTE_G5, 200},
    {NOTE_A5, 300}, {NOTE_F5, 150}, {NOTE_G5, 150},
    {NOTE_REST, 150}, {NOTE_E5, 300},
    {NOTE_C5, 150}, {NOTE_D5, 150}, {NOTE_B4, 300},
    {NOTE_REST, 1000},
};

#define MELODY_LEN ARRAY_SIZE(melody)

static float phase_accum;
static uint32_t note_index;
static uint32_t note_samples;
static da7212_test_state_t demo_state = DA7212_TEST_STATE_IDLE;
static struct fs_file_t playback_file;
static bool playback_file_open;
static bool playback_failed;
static int16_t playback_read_buffer[PLAYBACK_READ_SAMPLES];
static uint8_t voice_demo_previous_mic_gain;
static bool voice_demo_restore_mic_gain;

static void set_demo_state(da7212_test_state_t state);
static void close_playback_file(void);
static void set_voice_demo_mic_gain(void);
static void restore_voice_demo_mic_gain(void);
static void speaker_event_cb(zsw_speaker_event_t event, void *user_data);
static void voice_demo_mic_event_cb(zsw_mic_event_t event, zsw_mic_event_data_t *data,
                                    void *user_data);
static void start_melody_playback(void);
static int start_recording_playback(void);
static void start_voice_demo_recording(void);
static void stop_active_demo(void);

static void on_melody_button(void);
static void on_voice_demo_button(void);
static void on_stop_button(void);

static void melody_reset(void)
{
    phase_accum = 0.0f;
    note_index = 0;
    note_samples = (uint32_t)melody[0].duration_ms * SPEAKER_SAMPLE_FREQUENCY / 1000;
}

static uint32_t melody_fill_cb(int16_t *buf, uint32_t num_frames)
{
    for (uint32_t i = 0; i < num_frames; i++) {
        float freq = (float)melody[note_index].freq_hz;
        int16_t sample = 0;

        if (freq > 0.0f) {
            sample = (int16_t)(SINE_AMPLITUDE * sinf(2.0f * 3.14159265f * phase_accum));
            phase_accum += freq / (float)SPEAKER_SAMPLE_FREQUENCY;
            if (phase_accum >= 1.0f) {
                phase_accum -= 1.0f;
            }
        } else {
            phase_accum = 0.0f;
        }

        if (--note_samples == 0U) {
            note_index = (note_index + 1U) % MELODY_LEN;
            note_samples = (uint32_t)melody[note_index].duration_ms * SPEAKER_SAMPLE_FREQUENCY / 1000;
            phase_accum = 0.0f;
        }

        buf[i * 2U] = sample;
        buf[(i * 2U) + 1U] = sample;
    }

    return num_frames;
}

static void da7212_test_app_start(lv_obj_t *root, lv_group_t *group);
static void da7212_test_app_stop(void);

static application_t app = {
    .name = "DA7212 Test",
    .start_func = da7212_test_app_start,
    .stop_func = da7212_test_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static void set_demo_state(da7212_test_state_t state)
{
    demo_state = state;

    switch (state) {
    case DA7212_TEST_STATE_IDLE:
        da7212_test_ui_set_state(DA7212_TEST_UI_STATE_IDLE);
        break;
    case DA7212_TEST_STATE_PLAYING_MELODY:
        da7212_test_ui_set_state(DA7212_TEST_UI_STATE_PLAYING_MELODY);
        break;
    case DA7212_TEST_STATE_RECORDING_VOICE:
        da7212_test_ui_set_state(DA7212_TEST_UI_STATE_RECORDING_VOICE);
        break;
    case DA7212_TEST_STATE_PLAYING_VOICE:
        da7212_test_ui_set_state(DA7212_TEST_UI_STATE_PLAYING_VOICE);
        break;
    }
}

static void close_playback_file(void)
{
    if (playback_file_open) {
        fs_close(&playback_file);
        playback_file_open = false;
    }
}

static void set_voice_demo_mic_gain(void)
{
    voice_demo_previous_mic_gain = zsw_microphone_get_gain();
    voice_demo_restore_mic_gain = true;

    if (voice_demo_previous_mic_gain != VOICE_DEMO_MIC_GAIN) {
        zsw_microphone_set_gain(VOICE_DEMO_MIC_GAIN);
        LOG_INF("Voice demo mic gain set to 0x%02x", VOICE_DEMO_MIC_GAIN);
    }
}

static void restore_voice_demo_mic_gain(void)
{
    if (!voice_demo_restore_mic_gain) {
        return;
    }

    zsw_microphone_set_gain(voice_demo_previous_mic_gain);
    LOG_INF("Voice demo mic gain restored to 0x%02x", voice_demo_previous_mic_gain);
    voice_demo_restore_mic_gain = false;
}

static int delete_voice_demo_file(void)
{
    int ret = fs_unlink(VOICE_DEMO_FILE_PATH);

    if (ret < 0 && ret != -ENOENT) {
        LOG_ERR("Failed to delete existing voice demo file: %d", ret);
        return ret;
    }

    return 0;
}

static int open_playback_file(void)
{
    int ret;

    close_playback_file();
    fs_file_t_init(&playback_file);

    ret = fs_open(&playback_file, VOICE_DEMO_FILE_PATH, FS_O_READ);
    if (ret < 0) {
        LOG_ERR("Failed to open voice demo file for playback: %d", ret);
        return ret;
    }

    playback_file_open = true;
    return 0;
}

static uint32_t playback_fill_cb(int16_t *buf, uint32_t num_frames)
{
    uint32_t frames_written = 0;

    if (!playback_file_open) {
        return 0;
    }

    while (frames_written < num_frames) {
        uint32_t frames_remaining = num_frames - frames_written;
        size_t samples_to_read = DIV_ROUND_UP(frames_remaining, MIC_PLAYBACK_RATIO);

        if (samples_to_read > PLAYBACK_READ_SAMPLES) {
            samples_to_read = PLAYBACK_READ_SAMPLES;
        }

        ssize_t bytes_read = fs_read(&playback_file, playback_read_buffer,
                                     samples_to_read * sizeof(playback_read_buffer[0]));
        if (bytes_read < 0) {
            LOG_ERR("Failed to read voice demo file: %d", (int)bytes_read);
            playback_failed = true;
            return 0;
        }

        size_t samples_read = (size_t)bytes_read / sizeof(playback_read_buffer[0]);
        for (size_t index = 0; index < samples_read && frames_written < num_frames; index++) {
            for (uint32_t repeat = 0; repeat < MIC_PLAYBACK_RATIO && frames_written < num_frames; repeat++) {
                int32_t amplified = (int32_t)playback_read_buffer[index] * VOICE_PLAYBACK_GAIN;
                int16_t sample = (int16_t)CLAMP(amplified, -32768, 32767);

                buf[frames_written * 2U] = sample;
                buf[(frames_written * 2U) + 1U] = sample;
                frames_written++;
            }
        }

        if (samples_read < samples_to_read) {
            break;
        }
    }

    return frames_written;
}

static void speaker_event_cb(zsw_speaker_event_t event, void *user_data)
{
    ARG_UNUSED(user_data);

    close_playback_file();

    if (app.current_state == ZSW_APP_STATE_STOPPED) {
        demo_state = DA7212_TEST_STATE_IDLE;
        return;
    }

    if (event == ZSW_SPEAKER_EVENT_PLAYBACK_FINISHED) {
        if (demo_state == DA7212_TEST_STATE_PLAYING_VOICE) {
            if (playback_failed) {
                LOG_ERR("Voice demo playback ended after file read failure");
                da7212_test_ui_set_status("Voice playback error");
            } else {
                LOG_INF("Voice demo playback finished");
                da7212_test_ui_set_status("Voice demo finished");
            }
        } else {
            LOG_INF("Melody playback finished");
            da7212_test_ui_set_status("Melody finished");
        }

        set_demo_state(DA7212_TEST_STATE_IDLE);
        return;
    }

    LOG_ERR("Speaker playback error");
    if (demo_state == DA7212_TEST_STATE_PLAYING_VOICE) {
        da7212_test_ui_set_status("Voice playback error");
    } else {
        da7212_test_ui_set_status("Speaker playback error");
    }
    set_demo_state(DA7212_TEST_STATE_IDLE);
}

static void start_melody_playback(void)
{
    zsw_speaker_config_t cfg = {
        .source = ZSW_SPEAKER_SOURCE_CALLBACK,
        .callback.fill_cb = melody_fill_cb,
    };

    melody_reset();

    int ret = zsw_speaker_manager_start(&cfg, speaker_event_cb, NULL);
    if (ret < 0) {
        LOG_ERR("Failed to start melody playback: %d", ret);
        da7212_test_ui_set_status("Melody start error");
        set_demo_state(DA7212_TEST_STATE_IDLE);
        return;
    }

    da7212_test_ui_set_status("Playing melody");
    set_demo_state(DA7212_TEST_STATE_PLAYING_MELODY);
}

static int start_recording_playback(void)
{
    zsw_speaker_config_t cfg = {
        .source = ZSW_SPEAKER_SOURCE_CALLBACK,
        .callback.fill_cb = playback_fill_cb,
    };

    playback_failed = false;

    int ret = open_playback_file();
    if (ret < 0) {
        da7212_test_ui_set_status("Playback file error");
        set_demo_state(DA7212_TEST_STATE_IDLE);
        return ret;
    }

    ret = zsw_speaker_manager_start(&cfg, speaker_event_cb, NULL);
    if (ret < 0) {
        LOG_ERR("Failed to start voice playback: %d", ret);
        close_playback_file();
        da7212_test_ui_set_status("Voice playback error");
        set_demo_state(DA7212_TEST_STATE_IDLE);
        return ret;
    }

    da7212_test_ui_set_status("Playing recorded clip");
    set_demo_state(DA7212_TEST_STATE_PLAYING_VOICE);
    return 0;
}

static void voice_demo_mic_event_cb(zsw_mic_event_t event, zsw_mic_event_data_t *data,
                                    void *user_data)
{
    ARG_UNUSED(data);
    ARG_UNUSED(user_data);

    if (app.current_state == ZSW_APP_STATE_STOPPED ||
        demo_state != DA7212_TEST_STATE_RECORDING_VOICE) {
        return;
    }

    if (event == ZSW_MIC_EVENT_RECORDING_TIMEOUT) {
        LOG_INF("Voice demo recording completed, starting playback");
        restore_voice_demo_mic_gain();
        start_recording_playback();
    }
}

static void start_voice_demo_recording(void)
{
    zsw_mic_config_t cfg;

    int ret = delete_voice_demo_file();
    if (ret < 0) {
        da7212_test_ui_set_status("File delete error");
        set_demo_state(DA7212_TEST_STATE_IDLE);
        return;
    }

    zsw_microphone_manager_get_default_config(&cfg);
    cfg.duration_ms = VOICE_DEMO_RECORD_DURATION_MS;
    cfg.sample_rate = MIC_SAMPLE_FREQUENCY;
    cfg.bit_depth = 16;
    cfg.output = ZSW_MIC_OUTPUT_FILE;
    cfg.filename = VOICE_DEMO_FILE_PATH;

    set_voice_demo_mic_gain();

    ret = zsw_microphone_manager_start_recording(&cfg, voice_demo_mic_event_cb, NULL);
    if (ret < 0) {
        LOG_ERR("Failed to start voice demo recording: %d", ret);
        restore_voice_demo_mic_gain();
        da7212_test_ui_set_status("Record start error");
        set_demo_state(DA7212_TEST_STATE_IDLE);
        return;
    }

    da7212_test_ui_set_status("Recording 3 second clip");
    set_demo_state(DA7212_TEST_STATE_RECORDING_VOICE);
}

static void stop_active_demo(void)
{
    int ret;

    switch (demo_state) {
    case DA7212_TEST_STATE_RECORDING_VOICE:
        ret = zsw_microphone_stop_recording();
        if (ret < 0) {
            LOG_ERR("Failed to stop voice recording: %d", ret);
            da7212_test_ui_set_status("Stop recording error");
            set_demo_state(DA7212_TEST_STATE_IDLE);
            return;
        }

        restore_voice_demo_mic_gain();
        (void)start_recording_playback();
        break;
    case DA7212_TEST_STATE_PLAYING_VOICE:
        ret = zsw_speaker_manager_stop();
        if (ret < 0) {
            LOG_ERR("Failed to stop voice playback: %d", ret);
            da7212_test_ui_set_status("Stop playback error");
        } else {
            da7212_test_ui_set_status("Voice playback stopped");
        }

        close_playback_file();
        set_demo_state(DA7212_TEST_STATE_IDLE);
        break;
    case DA7212_TEST_STATE_PLAYING_MELODY:
        ret = zsw_speaker_manager_stop();
        if (ret < 0) {
            LOG_ERR("Failed to stop melody playback: %d", ret);
            da7212_test_ui_set_status("Stop playback error");
        } else {
            da7212_test_ui_set_status("Stopped");
        }

        set_demo_state(DA7212_TEST_STATE_IDLE);
        break;
    case DA7212_TEST_STATE_IDLE:
    default:
        break;
    }
}

static void on_melody_button(void)
{
    if (demo_state != DA7212_TEST_STATE_IDLE) {
        return;
    }

    start_melody_playback();
}

static void on_voice_demo_button(void)
{
    if (demo_state != DA7212_TEST_STATE_IDLE) {
        return;
    }

    start_voice_demo_recording();
}

static void on_stop_button(void)
{
    stop_active_demo();
}

static void da7212_test_app_start(lv_obj_t *root, lv_group_t *group)
{
    ARG_UNUSED(group);

    playback_file_open = false;
    playback_failed = false;
    voice_demo_restore_mic_gain = false;

    da7212_test_ui_show(root, on_melody_button, on_voice_demo_button, on_stop_button);
    da7212_test_ui_set_status("Ready for melody or voice demo");
    set_demo_state(DA7212_TEST_STATE_IDLE);
}

static void da7212_test_app_stop(void)
{
    if (zsw_speaker_manager_is_playing()) {
        zsw_speaker_manager_stop();
    }

    if (zsw_microphone_manager_is_recording()) {
        zsw_microphone_stop_recording();
    }

    restore_voice_demo_mic_gain();
    close_playback_file();
    playback_failed = false;
    demo_state = DA7212_TEST_STATE_IDLE;
    da7212_test_ui_remove();
}

static int da7212_test_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(da7212_test_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
