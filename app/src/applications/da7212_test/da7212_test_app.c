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
#include <math.h>

#include "managers/zsw_app_manager.h"
#include "managers/zsw_speaker_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "da7212_test_ui.h"

LOG_MODULE_REGISTER(da7212_test, LOG_LEVEL_INF);

ZSW_LV_IMG_DECLARE(music);

#define SAMPLE_FREQUENCY    48000

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

#define SINE_AMPLITUDE  32700

struct melody_note {
    uint16_t freq_hz;
    uint16_t duration_ms;
};

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
    /* Silence before loop */
    {NOTE_REST, 1000},
};

#define MELODY_LEN  ARRAY_SIZE(melody)

static float phase_accum;
static uint32_t note_index;
static uint32_t note_samples;

static void melody_reset(void)
{
    phase_accum = 0.0f;
    note_index = 0;
    note_samples = (uint32_t)melody[0].duration_ms * SAMPLE_FREQUENCY / 1000;
}

static uint32_t melody_fill_cb(int16_t *buf, uint32_t num_frames)
{
    for (uint32_t i = 0; i < num_frames; i++) {
        float freq = (float)melody[note_index].freq_hz;
        int16_t s = 0;

        if (freq > 0.0f) {
            s = (int16_t)(SINE_AMPLITUDE * sinf(2.0f * 3.14159265f * phase_accum));
            phase_accum += freq / (float)SAMPLE_FREQUENCY;
            if (phase_accum >= 1.0f) {
                phase_accum -= 1.0f;
            }
        } else {
            phase_accum = 0.0f;
        }

        if (--note_samples == 0) {
            note_index = (note_index + 1) % MELODY_LEN;
            note_samples = (uint32_t)melody[note_index].duration_ms * SAMPLE_FREQUENCY / 1000;
            phase_accum = 0.0f;
        }

        buf[i * 2]     = s;
        buf[i * 2 + 1] = s;
    }

    return num_frames;
}

static void da7212_test_app_start(lv_obj_t *root, lv_group_t *group);
static void da7212_test_app_stop(void);

static application_t app = {
    .name = "DA7212 Test",
    .icon = ZSW_LV_IMG_USE(music),
    .start_func = da7212_test_app_start,
    .stop_func = da7212_test_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static void speaker_event_cb(zsw_speaker_event_t event, void *user_data)
{
    ARG_UNUSED(user_data);

    if (event == ZSW_SPEAKER_EVENT_PLAYBACK_FINISHED) {
        LOG_INF("Playback finished");
        da7212_test_ui_set_status("Finished");
        da7212_test_ui_set_playing(false);
    } else if (event == ZSW_SPEAKER_EVENT_PLAYBACK_ERROR) {
        LOG_ERR("Playback error");
        da7212_test_ui_set_status("Error!");
        da7212_test_ui_set_playing(false);
    }
}

static void start_playback(void)
{
    melody_reset();

    zsw_speaker_config_t cfg = {
        .source = ZSW_SPEAKER_SOURCE_CALLBACK,
        .callback.fill_cb = melody_fill_cb,
    };

    int ret = zsw_speaker_manager_start(&cfg, speaker_event_cb, NULL);
    if (ret < 0) {
        LOG_ERR("Failed to start speaker: %d", ret);
        da7212_test_ui_set_status("Start error!");
        da7212_test_ui_set_playing(false);
        return;
    }

    da7212_test_ui_set_status("Playing melody");
    da7212_test_ui_set_playing(true);
}

static void stop_playback(void)
{
    zsw_speaker_manager_stop();
    da7212_test_ui_set_status("Stopped");
    da7212_test_ui_set_playing(false);
}

static void on_play_stop(bool play)
{
    if (play) {
        start_playback();
    } else {
        stop_playback();
    }
}

static void da7212_test_app_start(lv_obj_t *root, lv_group_t *group)
{
    ARG_UNUSED(group);

    da7212_test_ui_show(root, on_play_stop);
    da7212_test_ui_set_status("Ready");
    da7212_test_ui_set_playing(false);
}

static void da7212_test_app_stop(void)
{
    if (zsw_speaker_manager_is_playing()) {
        zsw_speaker_manager_stop();
    }
    da7212_test_ui_remove();
}

static int da7212_test_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(da7212_test_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
