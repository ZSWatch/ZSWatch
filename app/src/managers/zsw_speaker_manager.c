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
#include <zephyr/fs/fs.h>
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

#define DA7212_HP_VOLUME_MAX            0x3F

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
#define BLOCK_DURATION_MS   (FRAMES_PER_BLOCK * 1000 / SAMPLE_FREQUENCY)

K_MEM_SLAB_DEFINE_STATIC(spk_mem_slab, BLOCK_SIZE, BLOCK_COUNT, 4);

#define STREAM_STACK_SIZE  1024
#define STREAM_PRIORITY    5

K_THREAD_STACK_DEFINE(spk_stream_stack, STREAM_STACK_SIZE);

static const struct i2c_dt_spec codec_i2c = I2C_DT_SPEC_GET(DA7212_NODE);

/* File-source playback state.
 * The reply WAV must already be 48 kHz / 16-bit / stereo (the phone converts
 * before uploading). We skip the standard 44-byte PCM WAV header and stream
 * the raw sample data into the I2S slab directly.
 */
#define WAV_HEADER_SIZE 44

static struct {
    struct fs_file_t file;
    bool open;
} file_pb;

typedef uint32_t (*spk_fill_cb_t)(int16_t *buf, uint32_t num_frames);

static spk_fill_cb_t active_fill_cb;

static void speaker_event_work_handler(struct k_work *work);
static K_WORK_DEFINE(speaker_event_work, speaker_event_work_handler);

static struct {
    const struct device *i2s_dev;
    const struct device *codec_dev;
    struct k_thread thread_data;
    k_tid_t thread_id;
    volatile bool streaming;
    zsw_speaker_config_t config;
    zsw_speaker_event_cb_t callback;
    void *user_data;
    zsw_speaker_event_t pending_event;
} spk;

static void speaker_event_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (spk.callback) {
        spk.callback(spk.pending_event, spk.user_data);
    }
}

static void close_file_pb(void)
{
    if (file_pb.open) {
        fs_close(&file_pb.file);
        file_pb.open = false;
    }
}

static uint32_t file_fill_cb(int16_t *buf, uint32_t num_frames)
{
    size_t bytes_to_read = num_frames * NUMBER_OF_CHANNELS * sizeof(int16_t);
    ssize_t got = fs_read(&file_pb.file, buf, bytes_to_read);
    if (got <= 0) {
        if (got < 0) {
            LOG_ERR("fs_read failed: %d", (int)got);
        }
        return 0;
    }
    return (uint32_t)got / (NUMBER_OF_CHANNELS * sizeof(int16_t));
}

static void finish_playback(zsw_speaker_event_t event)
{
    int ret;

    if (event == ZSW_SPEAKER_EVENT_PLAYBACK_FINISHED) {
        ret = i2s_trigger(spk.i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
        if (ret < 0) {
            LOG_WRN("I2S drain trigger failed: %d", ret);
        } else {
            k_msleep(BLOCK_DURATION_MS * BLOCK_COUNT + 5);
        }
    }

    audio_codec_stop_output(spk.codec_dev);

    ret = i2s_trigger(spk.i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DROP);
    if (ret < 0) {
        LOG_WRN("I2S drop trigger failed: %d", ret);
    }

    close_file_pb();

    spk.thread_id = NULL;

    if (spk.callback) {
        spk.pending_event = event;
        k_work_submit(&speaker_event_work);
    }
}

static int fill_block(void **buf_out)
{
    void *buf;
    int ret = k_mem_slab_alloc(&spk_mem_slab, &buf, K_MSEC(500));
    if (ret < 0) {
        return ret;
    }

    int16_t *samples = (int16_t *)buf;
    uint32_t frames_written = active_fill_cb(samples, FRAMES_PER_BLOCK);

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
            finish_playback(ZSW_SPEAKER_EVENT_PLAYBACK_FINISHED);
            return;
        }
        if (ret < 0) {
            LOG_ERR("fill_block failed: %d", ret);
            spk.streaming = false;
            finish_playback(ZSW_SPEAKER_EVENT_PLAYBACK_ERROR);
            return;
        }

        ret = i2s_write(spk.i2s_dev, buf, BLOCK_SIZE);
        if (ret < 0) {
            k_mem_slab_free(&spk_mem_slab, buf);
            if (spk.streaming) {
                LOG_ERR("i2s_write failed: %d", ret);
                spk.streaming = false;
                finish_playback(ZSW_SPEAKER_EVENT_PLAYBACK_ERROR);
            } else {
                spk.thread_id = NULL;
            }
            return;
        }
    }

    spk.thread_id = NULL;
}

static int configure_codec_gains(void)
{
    int ret;

    // TODO: move to driver

    /* HP volume via codec API — 100% */
    audio_property_value_t vol = { .vol = DA7212_HP_VOLUME_MAX };
    ret = audio_codec_set_property(spk.codec_dev, AUDIO_PROPERTY_OUTPUT_VOLUME,
                                   AUDIO_CHANNEL_ALL, vol);
    if (ret < 0) {
        LOG_ERR("Failed to set HP volume: %d", ret);
        return ret;
    }

    /* DAC digital gain to max */
    ret = i2c_reg_write_byte_dt(&codec_i2c, DA7212_DAC_L_GAIN, DA7212_DAC_GAIN_MAX);
    ret |= i2c_reg_write_byte_dt(&codec_i2c, DA7212_DAC_R_GAIN, DA7212_DAC_GAIN_MAX);
    if (ret < 0) {
        LOG_ERR("Failed to set DAC gain: %d", ret);
        return ret;
    }

    /* LINE amp gain + enable */
    ret = i2c_reg_write_byte_dt(&codec_i2c, DA7212_LINE_GAIN, DA7212_LINE_GAIN_MAX);
    ret |= i2c_reg_write_byte_dt(&codec_i2c, DA7212_LINE_CTRL, DA7212_LINE_CTRL_EN_RAMP_OE);
    if (ret < 0) {
        LOG_ERR("Failed to set LINE amp: %d", ret);
        return ret;
    }

    LOG_INF("Codec gains configured (HP=0x3F, DAC=0x7F, LINE=0x3F)");
    return 0;
}

int zsw_speaker_manager_start(const zsw_speaker_config_t *config,
                              zsw_speaker_event_cb_t callback,
                              void *user_data)
{
    int ret;

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
        active_fill_cb = config->callback.fill_cb;
        break;
    case ZSW_SPEAKER_SOURCE_FILE:
        if (!config->file.path) {
            LOG_ERR("file.path is NULL");
            return -EINVAL;
        }
        close_file_pb();
        fs_file_t_init(&file_pb.file);
        ret = fs_open(&file_pb.file, config->file.path, FS_O_READ);
        if (ret < 0) {
            LOG_ERR("Failed to open '%s': %d", config->file.path, ret);
            return ret;
        }
        file_pb.open = true;
        ret = fs_seek(&file_pb.file, WAV_HEADER_SIZE, FS_SEEK_SET);
        if (ret < 0) {
            LOG_ERR("Failed to seek past WAV header: %d", ret);
            close_file_pb();
            return ret;
        }
        active_fill_cb = file_fill_cb;
        break;
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
        ret = -ENODEV;
        goto err;
    }

    if (!device_is_ready(spk.codec_dev)) {
        LOG_ERR("Codec device not ready");
        ret = -ENODEV;
        goto err;
    }

    spk.config = *config;
    spk.callback = callback;
    spk.user_data = user_data;

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
        goto err;
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
        goto err;
    }

    for (int i = 0; i < INITIAL_BLOCKS; i++) {
        void *buf;
        ret = fill_block(&buf);
        if (ret != 0) {
            LOG_ERR("Failed to fill initial block %d: %d", i, ret);
            if (ret > 0) {
                ret = -EIO;
            }
            goto err;
        }

        ret = i2s_write(spk.i2s_dev, buf, BLOCK_SIZE);
        if (ret < 0) {
            LOG_ERR("i2s_write initial block failed: %d", ret);
            k_mem_slab_free(&spk_mem_slab, buf);
            goto err;
        }
    }

    ret = i2s_trigger(spk.i2s_dev, I2S_DIR_TX, I2S_TRIGGER_START);
    if (ret < 0) {
        LOG_ERR("I2S start trigger failed: %d", ret);
        goto err;
    }

    spk.streaming = true;
    spk.thread_id = k_thread_create(&spk.thread_data, spk_stream_stack,
                                    K_THREAD_STACK_SIZEOF(spk_stream_stack),
                                    stream_thread_fn, NULL, NULL, NULL,
                                    STREAM_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(spk.thread_id, "spk_stream");

    audio_codec_start_output(spk.codec_dev);

    configure_codec_gains();

    LOG_INF("Speaker playback started (48kHz/16bit/stereo)");
    return 0;

err:
    close_file_pb();
    return ret;
}

int zsw_speaker_manager_stop(void)
{
    if (!spk.streaming) {
        LOG_WRN("Not currently playing");
        return -EINVAL;
    }

    LOG_INF("Stopping speaker playback");

    spk.streaming = false;

    audio_codec_stop_output(spk.codec_dev);

    int ret = i2s_trigger(spk.i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DROP);
    if (ret < 0) {
        LOG_WRN("I2S drop trigger failed: %d", ret);
    }

    if (spk.thread_id) {
        k_thread_join(&spk.thread_data, K_MSEC(500));
        spk.thread_id = NULL;
    }

    close_file_pb();

    LOG_INF("Speaker playback stopped");
    return 0;
}

bool zsw_speaker_manager_is_playing(void)
{
    return spk.streaming;
}
