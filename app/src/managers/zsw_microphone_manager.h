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

#pragma once

#include <zephyr/kernel.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Microphone manager events
 */
typedef enum {
    ZSW_MIC_EVENT_RECORDING_TIMEOUT,          /**< Recording timeout */
    ZSW_MIC_EVENT_RECORDING_DATA,      /**< Recording data */
} zsw_mic_event_t;

/**
 * @brief Audio output destinations
 */
typedef enum {
    ZSW_MIC_OUTPUT_RTT,     /**< Send audio data via RTT (for debugging) */
    ZSW_MIC_OUTPUT_FILE,    /**< Save audio data to filesystem */
    ZSW_MIC_OUTPUT_BLE,     /**< Stream audio data over BLE (future) */
    ZSW_MIC_OUTPUT_RAW      /**< Provide raw audio blocks to callback */
} zsw_mic_output_t;

/**
 * @brief Recording configuration
 */
typedef struct {
    uint32_t duration_ms;           /**< Recording duration in ms, 0 = unlimited (manual stop) */
    uint16_t sample_rate;           /**< Sample rate in Hz (typically 16000) */
    uint8_t bit_depth;              /**< Bit depth (typically 16) */
    zsw_mic_output_t output;        /**< Output destination */
    const char *filename;           /**< Filename for file output (optional, auto-generated if NULL) */
} zsw_mic_config_t;

/**
 * @brief Raw audio data block for ZSW_MIC_OUTPUT_RAW mode
 */
typedef struct {
    void *data;                     /**< Pointer to audio data */
    size_t size;                    /**< Size of audio data in bytes */
} zsw_mic_raw_block_t;

typedef struct {
    union {
        zsw_mic_raw_block_t raw_block;
    };
} zsw_mic_event_data_t;

/**
 * @brief Event callback function type
 *
 * @param event Current microphone event
 * @param data Event
 * @param user_data User data passed to start_recording
 */
typedef void (*zsw_mic_event_cb_t)(zsw_mic_event_t event, zsw_mic_event_data_t *data, void *user_data);

/**
 * @brief Initialize the microphone manager
 *
 * Must be called before any other microphone manager functions.
 *
 * @return 0 on success, negative error code on failure
 */
int zsw_microphone_manager_init(void);

/**
 * @brief Start recording audio (synchronous)
 *
 * This function blocks until the recording has actually started or an error occurs.
 *
 * @param config Recording configuration
 * @param callback Event callback function (can be NULL)
 * @param user_data User data passed to callback
 *
 * @return 0 on success, negative error code on failure
 */
int zsw_microphone_manager_start_recording(const zsw_mic_config_t *config,
                                           zsw_mic_event_cb_t callback,
                                           void *user_data);

/**
 * @brief Stop recording audio (synchronous)
 *
 * This function blocks until the recording has completely stopped.
 *
 * @return 0 on success, negative error code on failure
 */
int zsw_microphone_stop_recording(void);

/**
 * @brief Check if microphone manager is recording
 *
 * @return true if recording, false if idle or error
 */
bool zsw_microphone_manager_is_recording(void);

/**
 * @brief Get default recording configuration
 *
 * @param config Pointer to configuration struct to fill
 */
void zsw_microphone_manager_get_default_config(zsw_mic_config_t *config);

#ifdef __cplusplus
}
#endif
