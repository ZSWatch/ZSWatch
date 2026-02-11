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
#include <zephyr/drivers/i2s.h>
#include <zephyr/audio/codec.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "managers/zsw_app_manager.h"
#include "da7212_test_ui.h"

LOG_MODULE_REGISTER(da7212_test, LOG_LEVEL_INF);

#define I2S_CODEC_TX_NODE DT_ALIAS(i2s_codec_tx)

#define SAMPLE_FREQUENCY    16000
#define SAMPLE_BIT_WIDTH    16
#define BYTES_PER_SAMPLE    2
#define NUMBER_OF_CHANNELS  2
#define SAMPLES_PER_BLOCK   (SAMPLE_FREQUENCY / 100 * NUMBER_OF_CHANNELS)
#define BLOCK_SIZE          (BYTES_PER_SAMPLE * SAMPLES_PER_BLOCK)
#define BLOCK_COUNT         4
#define INITIAL_BLOCKS      2

/*
 * 16 kHz 16-bit stereo 440 Hz sine wave PCM data.
 * Copied from zephyr/samples/drivers/i2s/i2s_codec/src/sine.h
 */
static const unsigned char sine_pcm[] = {
    0x00, 0x00, 0x00, 0x00, 0x08, 0x0b, 0x08, 0x0b, 0xbb, 0x15, 0xbb, 0x15,
    0xc9, 0x1f, 0xc9, 0x1f, 0xe4, 0x28, 0xe4, 0x28, 0xc8, 0x30, 0xc8, 0x30,
    0x38, 0x37, 0x38, 0x37, 0x03, 0x3c, 0x03, 0x3c, 0x04, 0x3f, 0x04, 0x3f,
    0x25, 0x40, 0x25, 0x40, 0x5d, 0x3f, 0x5d, 0x3f, 0xb1, 0x3c, 0xb1, 0x3c,
    0x38, 0x38, 0x38, 0x38, 0x11, 0x32, 0x11, 0x32, 0x6d, 0x2a, 0x6d, 0x2a,
    0x85, 0x21, 0x85, 0x21, 0x9e, 0x17, 0x9e, 0x17, 0x02, 0x0d, 0x02, 0x0d,
    0x04, 0x02, 0x04, 0x02, 0xf6, 0xf6, 0xf6, 0xf6, 0x2d, 0xec, 0x2d, 0xec,
    0xfb, 0xe1, 0xfb, 0xe1, 0xae, 0xd8, 0xae, 0xd8, 0x8d, 0xd0, 0x8d, 0xd0,
    0xd6, 0xc9, 0xd6, 0xc9, 0xbb, 0xc4, 0xbb, 0xc4, 0x65, 0xc1, 0x65, 0xc1,
    0xeb, 0xbf, 0xeb, 0xbf, 0x5b, 0xc0, 0x5b, 0xc0, 0xaf, 0xc2, 0xaf, 0xc2,
    0xd7, 0xc6, 0xd7, 0xc6, 0xb3, 0xcc, 0xb3, 0xcc, 0x16, 0xd4, 0x16, 0xd4,
    0xc7, 0xdc, 0xc7, 0xdc, 0x86, 0xe6, 0x86, 0xe6, 0x06, 0xf1, 0x06, 0xf1,
    0xf9, 0xfb, 0xf9, 0xfb, 0x0a, 0x07, 0x0a, 0x07, 0xe6, 0x11, 0xe6, 0x11,
    0x39, 0x1c, 0x39, 0x1c, 0xb5, 0x25, 0xb5, 0x25, 0x12, 0x2e, 0x12, 0x2e,
    0x0f, 0x35, 0x0f, 0x35, 0x78, 0x3a, 0x78, 0x3a, 0x23, 0x3e, 0x23, 0x3e,
    0xf4, 0x3f, 0xf4, 0x3f, 0xde, 0x3f, 0xde, 0x3f, 0xe1, 0x3d, 0xe1, 0x3d,
    0x0c, 0x3a, 0x0c, 0x3a, 0x7c, 0x34, 0x7c, 0x34, 0x5d, 0x2d, 0x5d, 0x2d,
    0xe3, 0x24, 0xe3, 0x24, 0x51, 0x1b, 0x51, 0x1b, 0xee, 0x10, 0xee, 0x10,
    0x0a, 0x06, 0x0a, 0x06, 0xf7, 0xfa, 0xf7, 0xfa, 0x0c, 0xf0, 0x0c, 0xf0,
    0x9a, 0xe5, 0x9a, 0xe5, 0xf1, 0xdb, 0xf1, 0xdb, 0x5b, 0xd3, 0x5b, 0xd3,
    0x1a, 0xcc, 0x1a, 0xcc, 0x64, 0xc6, 0x64, 0xc6, 0x65, 0xc2, 0x65, 0xc2,
    0x3c, 0xc0, 0x3c, 0xc0, 0xfa, 0xbf, 0xfa, 0xbf, 0x9f, 0xc1, 0x9f, 0xc1,
    0x20, 0xc5, 0x20, 0xc5, 0x62, 0xca, 0x62, 0xca, 0x3c, 0xd1, 0x3c, 0xd1,
    0x7b, 0xd9, 0x7b, 0xd9, 0xe0, 0xe2, 0xe0, 0xe2, 0x23, 0xed, 0x23, 0xed,
    0xf6, 0xf7, 0xf6, 0xf7,
};

static const unsigned int sine_pcm_len = sizeof(sine_pcm);

K_MEM_SLAB_DEFINE_STATIC(audio_mem_slab, BLOCK_SIZE, BLOCK_COUNT, 4);

static const struct device *i2s_dev;
static const struct device *codec_dev;
static bool streaming;
static struct k_work_delayable stream_work;

/**
 * Fill an I2S block buffer with repeated copies of the sine wave PCM data.
 */
static int fill_buf_with_sine(void **buf_out)
{
    void *buf;
    int ret = k_mem_slab_alloc(&audio_mem_slab, &buf, K_NO_WAIT);
    if (ret < 0) {
        return ret;
    }

    uint8_t *dst = (uint8_t *)buf;
    size_t remaining = BLOCK_SIZE;

    while (remaining > 0) {
        size_t chunk = MIN(remaining, sine_pcm_len);
        memcpy(dst, sine_pcm, chunk);
        dst += chunk;
        remaining -= chunk;
    }

    *buf_out = buf;
    return 0;
}

static void da7212_test_app_start(lv_obj_t *root, lv_group_t *group);
static void da7212_test_app_stop(void);

static application_t app = {
    .name = "DA7212 Test",
    .start_func = da7212_test_app_start,
    .stop_func = da7212_test_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static void stream_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (!streaming) {
        return;
    }

    int ret;
    void *buf;

    for (int i = 0; i < INITIAL_BLOCKS; i++) {
        ret = fill_buf_with_sine(&buf);
        if (ret < 0) {
            LOG_WRN("No slab buffers available");
            break;
        }

        ret = i2s_write(i2s_dev, buf, BLOCK_SIZE);
        if (ret < 0) {
            LOG_ERR("i2s_write failed: %d", ret);
            k_mem_slab_free(&audio_mem_slab, buf);
            streaming = false;
            da7212_test_ui_set_status("Write error!");
            da7212_test_ui_set_playing(false);
            return;
        }
    }

    /* Keep feeding data */
    if (streaming) {
        k_work_schedule(&stream_work, K_MSEC(50));
    }
}

static void start_playback(void)
{
    int ret;

    LOG_INF("Starting playback");
    da7212_test_ui_set_status("Starting...");

    /* Configure I2S TX */
    struct i2s_config config = {
        .word_size = SAMPLE_BIT_WIDTH,
        .channels = NUMBER_OF_CHANNELS,
        .format = I2S_FMT_DATA_FORMAT_I2S,
        .options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER,
        .frame_clk_freq = SAMPLE_FREQUENCY,
        .mem_slab = &audio_mem_slab,
        .block_size = BLOCK_SIZE,
        .timeout = 2000,
    };

    ret = i2s_configure(i2s_dev, I2S_DIR_TX, &config);
    if (ret < 0) {
        LOG_ERR("Failed to configure I2S TX: %d", ret);
        da7212_test_ui_set_status("I2S config err!");
        da7212_test_ui_set_playing(false);
        return;
    }

    /* Queue initial blocks */
    for (int i = 0; i < INITIAL_BLOCKS; i++) {
        void *buf;
        ret = fill_buf_with_sine(&buf);
        if (ret < 0) {
            LOG_ERR("Failed to allocate slab buffer: %d", ret);
            da7212_test_ui_set_status("Alloc error!");
            da7212_test_ui_set_playing(false);
            return;
        }

        ret = i2s_write(i2s_dev, buf, BLOCK_SIZE);
        if (ret < 0) {
            LOG_ERR("i2s_write initial failed: %d", ret);
            k_mem_slab_free(&audio_mem_slab, buf);
            da7212_test_ui_set_status("Write error!");
            da7212_test_ui_set_playing(false);
            return;
        }
    }

    /* Start I2S streaming */
    ret = i2s_trigger(i2s_dev, I2S_DIR_TX, I2S_TRIGGER_START);
    if (ret < 0) {
        LOG_ERR("I2S start trigger failed: %d", ret);
        da7212_test_ui_set_status("Start error!");
        da7212_test_ui_set_playing(false);
        return;
    }

    streaming = true;
    da7212_test_ui_set_status("Playing 440 Hz");
    da7212_test_ui_set_playing(true);

    /* Schedule continuous feeding */
    k_work_schedule(&stream_work, K_MSEC(50));
}

static void stop_playback(void)
{
    LOG_INF("Stopping playback");
    streaming = false;

    k_work_cancel_delayable(&stream_work);

    int ret = i2s_trigger(i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DROP);
    if (ret < 0) {
        LOG_WRN("I2S drop trigger failed: %d", ret);
    }

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

    i2s_dev = DEVICE_DT_GET(I2S_CODEC_TX_NODE);
    codec_dev = DEVICE_DT_GET(DT_NODELABEL(audio_codec));

    k_work_init_delayable(&stream_work, stream_work_handler);

    da7212_test_ui_show(root, on_play_stop);

    if (!device_is_ready(i2s_dev)) {
        LOG_ERR("I2S device not ready");
        da7212_test_ui_set_status("I2S not ready!");
        return;
    }

    if (!device_is_ready(codec_dev)) {
        LOG_ERR("Codec device not ready");
        da7212_test_ui_set_status("Codec not ready!");
        return;
    }

    /* Configure the codec for playback */
    struct audio_codec_cfg audio_cfg = {
        .dai_route = AUDIO_ROUTE_PLAYBACK,
        .dai_type = AUDIO_DAI_TYPE_I2S,
        .dai_cfg.i2s = {
            .word_size = SAMPLE_BIT_WIDTH,
            .channels = NUMBER_OF_CHANNELS,
            .format = I2S_FMT_DATA_FORMAT_I2S,
            .options = I2S_OPT_FRAME_CLK_SLAVE | I2S_OPT_BIT_CLK_SLAVE,
            .frame_clk_freq = SAMPLE_FREQUENCY,
            .mem_slab = &audio_mem_slab,
            .block_size = BLOCK_SIZE,
        },
    };

    int ret = audio_codec_configure(codec_dev, &audio_cfg);
    if (ret < 0) {
        LOG_ERR("Codec configure failed: %d", ret);
        da7212_test_ui_set_status("Codec cfg err!");
        return;
    }

    LOG_INF("DA7212 codec configured OK");
    da7212_test_ui_set_status("Ready - press Play");
}

static void da7212_test_app_stop(void)
{
    if (streaming) {
        stop_playback();
    }
    da7212_test_ui_remove();
}

static int da7212_test_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(da7212_test_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
