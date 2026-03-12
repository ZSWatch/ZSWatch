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
 * @brief Audio data callback function type
 *
 * Called by microphone driver when new audio data is available. This callback is called
 * from interrupt context or work queue context, so keep processing minimal.
 *
 * @param audio_data Pointer to raw audio data (16-bit PCM)
 * @param size Size of audio data in bytes
 */
typedef void (*zsw_mic_audio_cb_t)(void *audio_data, size_t size);

/**
 * @brief Initialize the microphone driver
 *
 * Sets up the PDM peripheral, GPIO power control, and memory management.
 *
 * @param audio_callback Callback function for audio data
 * @return 0 on success, negative error code on failure
 */
int zsw_microphone_init(zsw_mic_audio_cb_t audio_callback);

/**
 * @brief Start audio recording (driver level)
 *
 * Powers on the microphone, configures the PDM interface, and starts
 * audio capture. Audio data will be provided via the callback.
 *
 * @return 0 on success, negative error code on failure
 */
int zsw_microphone_driver_start(void);

/**
 * @brief Stop audio recording (driver level, synchronous)
 *
 * Stops audio capture, waits for processing thread to complete, and powers down
 * the microphone to save power. This function blocks until recording fully stops.
 *
 * @return 0 on success, negative error code on failure
 */
int zsw_microphone_driver_stop(void);

/**
 * @brief Set PDM microphone gain
 *
 * Sets the gain for both left and right PDM channels.
 * Can be called while recording — the nRF PDM GAIN register is
 * writable at any time without stopping capture.
 *
 * On native_sim this is a no-op (no real PDM hardware).
 *
 * @param gain  PDM gain register value:
 *              0x00 = mute (−20 dB), 0x28 = 0 dB (default), 0x50 = +20 dB.
 *              Valid range: 0–80 decimal (0x00–0x50).
 */
void zsw_microphone_set_gain(uint8_t gain);

/**
 * @brief Get current PDM microphone gain
 *
 * On native_sim returns CONFIG_ZSW_MIC_DEFAULT_GAIN.
 *
 * @return Current gain register value (0–80).
 */
uint8_t zsw_microphone_get_gain(void);

#ifdef __cplusplus
}
#endif
