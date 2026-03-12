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

#include "zsw_audio_codec.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <limits.h>
#include <opus.h>

#include "../managers/zsw_xip_manager.h"

LOG_MODULE_REGISTER(zsw_audio_codec, CONFIG_ZSW_AUDIO_CODEC_LOG_LEVEL);

#define OPUS_SAMPLE_RATE    16000
#define OPUS_CHANNELS       1
#define OPUS_MAX_FRAME_SIZE CONFIG_ZSW_OPUS_FRAME_SIZE_SAMPLES

/*
 * Encoder state is dynamically allocated from the system heap.
 * Allocated in zsw_audio_codec_init(), freed in zsw_audio_codec_deinit().
 */
static __aligned(4) uint8_t *encoder_mem;
static OpusEncoder *encoder;
static bool initialized;
static bool xip_acquired;

int zsw_audio_codec_init(void)
{
    int actual_size;
    int ret;

    if (initialized) {
        LOG_WRN("Audio codec already initialized");
        return 0;
    }

    if (!xip_acquired) {
        ret = zsw_xip_enable();
        if (ret < 0) {
            LOG_ERR("Failed to enable XIP for Opus codec: %d", ret);
            return ret;
        }
        xip_acquired = true;
    }

    actual_size = opus_encoder_get_size(OPUS_CHANNELS);

    if (!encoder_mem) {
        encoder_mem = k_malloc(actual_size);
        if (!encoder_mem) {
            LOG_ERR("Failed to allocate %d bytes for Opus encoder", actual_size);
            zsw_xip_disable();
            xip_acquired = false;
            return -ENOMEM;
        }
        encoder = (OpusEncoder *)encoder_mem;
    }

    ret = opus_encoder_init(encoder, OPUS_SAMPLE_RATE, OPUS_CHANNELS,
                            OPUS_APPLICATION_RESTRICTED_LOWDELAY);
    if (ret != OPUS_OK) {
        LOG_ERR("Opus encoder init failed: %d", ret);
        k_free(encoder_mem);
        encoder_mem = NULL;
        encoder = NULL;
        if (xip_acquired) {
            zsw_xip_disable();
            xip_acquired = false;
        }
        return -EIO;
    }

    /* Configure encoder per spec */
    opus_encoder_ctl(encoder, OPUS_SET_BITRATE(CONFIG_ZSW_OPUS_BITRATE));
    opus_encoder_ctl(encoder, OPUS_SET_VBR(1));
    opus_encoder_ctl(encoder, OPUS_SET_VBR_CONSTRAINT(0));
    opus_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(CONFIG_ZSW_OPUS_COMPLEXITY));
    opus_encoder_ctl(encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    opus_encoder_ctl(encoder, OPUS_SET_LSB_DEPTH(16));
    opus_encoder_ctl(encoder, OPUS_SET_DTX(0));
    opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(0));
    opus_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(0));

    initialized = true;

    LOG_INF("Opus encoder initialized: state_size=%d, frame_size=%d, bitrate=%d, complexity=%d",
            actual_size, OPUS_MAX_FRAME_SIZE, CONFIG_ZSW_OPUS_BITRATE, CONFIG_ZSW_OPUS_COMPLEXITY);

    return 0;
}

int zsw_audio_codec_encode(const int16_t *pcm_in, size_t samples,
                           uint8_t *opus_out, size_t max_out)
{
    opus_int32 encoded_bytes;

    if (!initialized) {
        return -EINVAL;
    }

    if (pcm_in == NULL || opus_out == NULL || max_out == 0 || max_out > INT32_MAX) {
        return -EINVAL;
    }

    if (samples != OPUS_MAX_FRAME_SIZE) {
        LOG_ERR("Invalid frame size: %zu (expected %d)", samples, OPUS_MAX_FRAME_SIZE);
        return -EINVAL;
    }

    encoded_bytes = opus_encode(encoder, pcm_in, (int)samples, opus_out, (opus_int32)max_out);
    if (encoded_bytes < 0) {
        LOG_WRN("Opus encoding failed: %d", encoded_bytes);
        return -EIO;
    }

    return (int)encoded_bytes;
}

void zsw_audio_codec_reset(void)
{
    if (!initialized) {
        return;
    }

    int ret = opus_encoder_init(encoder, OPUS_SAMPLE_RATE, OPUS_CHANNELS,
                                OPUS_APPLICATION_RESTRICTED_LOWDELAY);
    if (ret != OPUS_OK) {
        LOG_ERR("Opus encoder reset failed: %d", ret);
    } else {
        /* Reapply settings after reset */
        opus_encoder_ctl(encoder, OPUS_SET_BITRATE(CONFIG_ZSW_OPUS_BITRATE));
        opus_encoder_ctl(encoder, OPUS_SET_VBR(1));
        opus_encoder_ctl(encoder, OPUS_SET_VBR_CONSTRAINT(0));
        opus_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(CONFIG_ZSW_OPUS_COMPLEXITY));
        opus_encoder_ctl(encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
        opus_encoder_ctl(encoder, OPUS_SET_LSB_DEPTH(16));
        opus_encoder_ctl(encoder, OPUS_SET_DTX(0));
        opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(0));
        opus_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(0));

        LOG_DBG("Opus encoder state reset");
    }
}

size_t zsw_audio_codec_frame_samples(void)
{
    return OPUS_MAX_FRAME_SIZE;
}

void zsw_audio_codec_deinit(void)
{
    if (encoder_mem) {
        k_free(encoder_mem);
        encoder_mem = NULL;
        encoder = NULL;
    }
    initialized = false;

    if (xip_acquired) {
        zsw_xip_disable();
        xip_acquired = false;
    }
}
