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

#pragma once

#include <zephyr/kernel.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ZSW_SPEAKER_SOURCE_CALLBACK,
    ZSW_SPEAKER_SOURCE_FILE,
    ZSW_SPEAKER_SOURCE_BUFFER,
} zsw_speaker_source_t;

typedef enum {
    ZSW_SPEAKER_EVENT_PLAYBACK_FINISHED,
    ZSW_SPEAKER_EVENT_PLAYBACK_ERROR,
} zsw_speaker_event_t;

/**
 * @brief Event callback function type
 *
 * @param event Current speaker event
 * @param user_data User data passed to zsw_speaker_manager_start
 */
typedef void (*zsw_speaker_event_cb_t)(zsw_speaker_event_t event, void *user_data);

/**
 * @brief Fill callback for ZSW_SPEAKER_SOURCE_CALLBACK mode.
 *
 * Called from the streaming thread to request the next block of audio.
 *
 * @param buf        Buffer to fill with interleaved stereo 16-bit PCM samples
 * @param num_frames Number of stereo frames to produce
 *                   (buf can hold num_frames * 2 samples)
 * @return           Number of frames actually written. Return 0 to signal end-of-stream.
 */
typedef uint32_t (*zsw_speaker_fill_cb_t)(int16_t *buf, uint32_t num_frames);

/** Playback configuration */
typedef struct {
    zsw_speaker_source_t source;
    union {
        struct {
            zsw_speaker_fill_cb_t fill_cb;
        } callback;
        struct {
            const char *path;
        } file;
        struct {
            const int16_t *data;
            size_t num_frames;
        } buffer;
    };
} zsw_speaker_config_t;

/**
 * @brief Start audio playback.
 *
 * Configures the codec and I2S, starts the streaming thread, and begins playback.
 * This function is synchronous — codec and I2S are ready when it returns.
 *
 * @param config   Playback configuration
 * @param callback Event callback (can be NULL)
 * @param user_data User data passed to callback
 * @return 0 on success, negative error code on failure
 */
int zsw_speaker_manager_start(const zsw_speaker_config_t *config,
                              zsw_speaker_event_cb_t callback,
                              void *user_data);

/**
 * @brief Stop audio playback.
 *
 * Blocks until the streaming thread exits and hardware is shut down.
 *
 * @return 0 on success, negative error code on failure
 */
int zsw_speaker_manager_stop(void);

/**
 * @brief Check if audio is currently playing.
 *
 * @return true if playing, false otherwise
 */
bool zsw_speaker_manager_is_playing(void);

#ifdef __cplusplus
}
#endif
