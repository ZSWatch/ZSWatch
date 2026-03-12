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

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize the Opus encoder. Called on demand when recording starts. */
int zsw_audio_codec_init(void);

/**
 * @brief Encode a PCM frame.
 *
 * @param pcm_in   Pointer to interleaved 16-bit PCM samples.
 * @param samples  Number of samples (must match frame size from zsw_audio_codec_frame_samples()).
 * @param opus_out Output buffer for encoded Opus data.
 * @param max_out  Size of output buffer in bytes.
 * @return Encoded byte count on success, or negative error code.
 */
int zsw_audio_codec_encode(const int16_t *pcm_in, size_t samples,
                           uint8_t *opus_out, size_t max_out);

/** Reset encoder state (e.g., between recordings). */
void zsw_audio_codec_reset(void);

/** Release encoder resources (frees heap memory). */
void zsw_audio_codec_deinit(void);

/** Get the expected frame size in samples (e.g. 160 for 10 ms at 16 kHz). */
size_t zsw_audio_codec_frame_samples(void);

#ifdef __cplusplus
}
#endif
