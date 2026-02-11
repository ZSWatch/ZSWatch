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
#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/audio/codec.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "zsw_speaker_manager.h"

LOG_MODULE_REGISTER(zsw_speaker_manager, LOG_LEVEL_DBG);

#define I2S_CODEC_TX_NODE  DT_ALIAS(i2s_codec_tx)
#define DA7212_NODE        DT_NODELABEL(audio_codec)

/* DA7212 register addresses not exposed by the Zephyr codec API */
#define DA7212_DAC_L_GAIN               0x45
#define DA7212_DAC_R_GAIN               0x46
#define DA7212_DAC_GAIN_MAX             0x7F

#define DA7212_LINE_GAIN                0x4A
#define DA7212_LINE_CTRL                0x6D
#define DA7212_LINE_GAIN_MAX            0x3F
#define DA7212_LINE_CTRL_EN_RAMP_OE     (BIT(7) | BIT(5) | BIT(3))

/* Fixed playback parameters */
#define SAMPLE_FREQUENCY    48000
#define SAMPLE_BIT_WIDTH    16
#define BYTES_PER_SAMPLE    2
#define NUMBER_OF_CHANNELS  2
/* 10 ms of stereo samples per block */
#define SAMPLES_PER_BLOCK   (SAMPLE_FREQUENCY / 100 * NUMBER_OF_CHANNELS)
#define BLOCK_SIZE          (BYTES_PER_SAMPLE * SAMPLES_PER_BLOCK)
#define BLOCK_COUNT         4
#define INITIAL_BLOCKS      2
#define FRAMES_PER_BLOCK    (BLOCK_SIZE / (BYTES_PER_SAMPLE * NUMBER_OF_CHANNELS))

K_MEM_SLAB_DEFINE_STATIC(spk_mem_slab, BLOCK_SIZE, BLOCK_COUNT, 4);

#define STREAM_STACK_SIZE  1024
#define STREAM_PRIORITY    5

K_THREAD_STACK_DEFINE(spk_stream_stack, STREAM_STACK_SIZE);

static const struct i2c_dt_spec codec_i2c = I2C_DT_SPEC_GET(DA7212_NODE);

static struct {
    const struct device *i2s_dev;
    const struct device *codec_dev;
    struct k_thread thread_data;
    k_tid_t thread_id;
    volatile bool streaming;
    zsw_speaker_config_t config;
    zsw_speaker_event_cb_t callback;
    void *user_data;
} spk;

static int fill_block(void **buf_out)
{
    void *buf;
    int ret = k_mem_slab_alloc(&spk_mem_slab, &buf, K_MSEC(500));
    if (ret < 0) {
        return ret;
    }

    int16_t *samples = (int16_t *)buf;
    uint32_t frames_written = spk.config.callback.fill_cb(samples, FRAMES_PER_BLOCK);

    if (frames_written == 0) {
        k_mem_slab_free(&spk_mem_slab, buf);
        return 1;
    }

    /* Zero-fill remainder if callback provided fewer frames than requested */
    if (frames_written < FRAMES_PER_BLOCK) {
        memset(&samples[frames_written * NUMBER_OF_CHANNELS], 0,
               (FRAMES_PER_BLOCK - frames_written) * BYTES_PER_SAMPLE * NUMBER_OF_CHANNELS);
    }

    *buf_out = buf;
    return 0;
}

static void speaker_teardown(void)
{
    audio_codec_stop_output(spk.codec_dev);

    int ret = i2s_trigger(spk.i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DROP);
    if (ret < 0) {
        LOG_WRN("I2S drop trigger failed during teardown: %d", ret);
    }
}

static void stream_thread_fn(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    while (spk.streaming) {
        void *buf;
        int ret = fill_block(&buf);
        if (ret > 0) {
            /* End-of-stream signalled by fill callback */
            spk.streaming = false;
            speaker_teardown();
            if (spk.callback) {
                spk.callback(ZSW_SPEAKER_EVENT_PLAYBACK_FINISHED, spk.user_data);
            }
            return;
        }
        if (ret < 0) {
            LOG_ERR("fill_block failed: %d", ret);
            spk.streaming = false;
            speaker_teardown();
            if (spk.callback) {
                spk.callback(ZSW_SPEAKER_EVENT_PLAYBACK_ERROR, spk.user_data);
            }
            return;
        }

        ret = i2s_write(spk.i2s_dev, buf, BLOCK_SIZE);
        if (ret < 0) {
            k_mem_slab_free(&spk_mem_slab, buf);
            if (spk.streaming) {
                LOG_ERR("i2s_write failed: %d", ret);
                spk.streaming = false;
                speaker_teardown();
                if (spk.callback) {
                    spk.callback(ZSW_SPEAKER_EVENT_PLAYBACK_ERROR, spk.user_data);
                }
            }
            return;
        }
    }
}

static int configure_codec_gains(void)
{
    int ret;

    // TODO: move to driver

    /* HP volume via codec API — ~50% */
    audio_property_value_t vol = { .vol = 0x20 };
    ret = audio_codec_set_property(spk.codec_dev, AUDIO_PROPERTY_OUTPUT_VOLUME,
                                   AUDIO_CHANNEL_ALL, vol);
    if (ret < 0) {
        LOG_ERR("Failed to set HP volume: %d", ret);
        return ret;
    }

    /* DAC digital gain to max */
    ret = i2c_reg_write_byte_dt(&codec_i2c, DA7212_DAC_L_GAIN, DA7212_DAC_GAIN_MAX);
    if (ret < 0) {
        LOG_ERR("Failed to set left DAC gain: %d", ret);
        return ret;
    }

    ret = i2c_reg_write_byte_dt(&codec_i2c, DA7212_DAC_R_GAIN, DA7212_DAC_GAIN_MAX);
    if (ret < 0) {
        LOG_ERR("Failed to set right DAC gain: %d", ret);
        return ret;
    }

    /* LINE amp gain + enable */
    ret = i2c_reg_write_byte_dt(&codec_i2c, DA7212_LINE_GAIN, DA7212_LINE_GAIN_MAX);
    if (ret < 0) {
        LOG_ERR("Failed to set LINE gain: %d", ret);
        return ret;
    }

    ret = i2c_reg_write_byte_dt(&codec_i2c, DA7212_LINE_CTRL, DA7212_LINE_CTRL_EN_RAMP_OE);
    if (ret < 0) {
        LOG_ERR("Failed to enable LINE amp: %d", ret);
        return ret;
    }

    LOG_INF("Codec gains configured (HP=0x20, DAC=0x7F, LINE=0x3F)");
    return 0;
}

int zsw_speaker_manager_start(const zsw_speaker_config_t *config,
                              zsw_speaker_event_cb_t callback,
                              void *user_data)
{
    int ret;
    bool i2s_configured = false;
    bool codec_output_started = false;

    if (!config) {
        return -EINVAL;
    }

    if (spk.streaming) {
        LOG_WRN("Already playing");
        return -EBUSY;
    }

    switch (config->source) {
        case ZSW_SPEAKER_SOURCE_CALLBACK:
            if (!config->callback.fill_cb) {
                LOG_ERR("fill_cb is NULL");
                return -EINVAL;
            }
            break;
        case ZSW_SPEAKER_SOURCE_FILE:
        case ZSW_SPEAKER_SOURCE_BUFFER:
            LOG_WRN("Source type %d not implemented yet", config->source);
            return -ENOTSUP;
        default:
            return -EINVAL;
    }

    spk.i2s_dev = DEVICE_DT_GET(I2S_CODEC_TX_NODE);
    spk.codec_dev = DEVICE_DT_GET(DA7212_NODE);

    if (!device_is_ready(spk.i2s_dev)) {
        LOG_ERR("I2S device not ready");
        return -ENODEV;
    }

    if (!device_is_ready(spk.codec_dev)) {
        LOG_ERR("Codec device not ready");
        return -ENODEV;
    }

    struct audio_codec_cfg audio_cfg = {
        .dai_route = AUDIO_ROUTE_PLAYBACK,
        .dai_type = AUDIO_DAI_TYPE_I2S,
        .dai_cfg.i2s = {
            .word_size = SAMPLE_BIT_WIDTH,
            .channels = NUMBER_OF_CHANNELS,
            .format = I2S_FMT_DATA_FORMAT_I2S,
            .options = I2S_OPT_FRAME_CLK_SLAVE | I2S_OPT_BIT_CLK_SLAVE,
            .frame_clk_freq = SAMPLE_FREQUENCY,
            .mem_slab = &spk_mem_slab,
            .block_size = BLOCK_SIZE,
        },
    };

    ret = audio_codec_configure(spk.codec_dev, &audio_cfg);
    if (ret < 0) {
        LOG_ERR("Codec configure failed: %d", ret);
        return ret;
    }

    struct i2s_config i2s_cfg = {
        .word_size = SAMPLE_BIT_WIDTH,
        .channels = NUMBER_OF_CHANNELS,
        .format = I2S_FMT_DATA_FORMAT_I2S,
        .options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER,
        .frame_clk_freq = SAMPLE_FREQUENCY,
        .mem_slab = &spk_mem_slab,
        .block_size = BLOCK_SIZE,
        .timeout = 2000,
    };

    ret = i2s_configure(spk.i2s_dev, I2S_DIR_TX, &i2s_cfg);
    if (ret < 0) {
        LOG_ERR("I2S TX configure failed: %d", ret);
        return ret;
    }

    i2s_configured = true;

    spk.config = *config;
    spk.callback = callback;
    spk.user_data = user_data;

    for (int i = 0; i < INITIAL_BLOCKS; i++) {
        void *buf;
        ret = fill_block(&buf);
        if (ret != 0) {
            LOG_ERR("Failed to fill initial block %d: %d", i, ret);
            ret = (ret < 0) ? ret : -EIO;
            goto start_fail;
        }

        ret = i2s_write(spk.i2s_dev, buf, BLOCK_SIZE);
        if (ret < 0) {
            LOG_ERR("i2s_write initial block failed: %d", ret);
            k_mem_slab_free(&spk_mem_slab, buf);
            goto start_fail;
        }
    }

    ret = i2s_trigger(spk.i2s_dev, I2S_DIR_TX, I2S_TRIGGER_START);
    if (ret < 0) {
        LOG_ERR("I2S start trigger failed: %d", ret);
        goto start_fail;
    }

    audio_codec_start_output(spk.codec_dev);
    codec_output_started = true;

    ret = configure_codec_gains();
    if (ret < 0) {
        LOG_ERR("Failed to configure codec gains: %d", ret);
        goto start_fail;
    }

    spk.streaming = true;
    spk.thread_id = k_thread_create(&spk.thread_data, spk_stream_stack,
                                    K_THREAD_STACK_SIZEOF(spk_stream_stack),
                                    stream_thread_fn, NULL, NULL, NULL,
                                    STREAM_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(spk.thread_id, "spk_stream");

    LOG_INF("Speaker playback started (48kHz/16bit/stereo)");
    return 0;

start_fail:
    if (codec_output_started) {
        audio_codec_stop_output(spk.codec_dev);
    }

    if (i2s_configured) {
        int drop_ret = i2s_trigger(spk.i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DROP);
        if (drop_ret < 0) {
            LOG_WRN("I2S drop trigger failed during rollback: %d", drop_ret);
        }
    }

    memset(&spk.config, 0, sizeof(spk.config));
    spk.callback = NULL;
    spk.user_data = NULL;
    spk.streaming = false;
    spk.thread_id = NULL;
    return ret;
}

int zsw_speaker_manager_stop(void)
{
    if (!spk.streaming) {
        return 0;
    }

    LOG_INF("Stopping speaker playback");

    spk.streaming = false;

    audio_codec_stop_output(spk.codec_dev);

    int ret = i2s_trigger(spk.i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DROP);
    if (ret < 0) {
        LOG_WRN("I2S drop trigger failed: %d", ret);
    }

    if (spk.thread_id) {
        ret = k_thread_join(&spk.thread_data, K_MSEC(500));
        if (ret == 0) {
            spk.thread_id = NULL;
        } else {
            LOG_WRN("Thread join failed/timed out: %d", ret);
        }
    }

    LOG_INF("Speaker playback stopped");
    return 0;
}

bool zsw_speaker_manager_is_playing(void)
{
    return spk.streaming;
}
