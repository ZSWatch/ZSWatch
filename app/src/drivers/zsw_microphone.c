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

#include "zsw_microphone.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/audio/dmic.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(zsw_mic, LOG_LEVEL_INF);

// 1ms of audio buffer per block for more efficient processing
// With 300 pool size we can record for 300ms
#define AUDIO_FREQ          16000
#define CHAN_SIZE           16
#define PCM_BLK_SIZE_MS     ((AUDIO_FREQ/1000) * sizeof(int16_t))
#define BUFFER_POOL_SIZE    300

static struct {
    bool initialized;
    bool recording;
    zsw_mic_audio_cb_t audio_callback;
    const struct device *mic_dev;
    struct gpio_dt_spec enable_gpio;

    struct k_thread audio_thread;
    k_tid_t audio_thread_id;

    uint32_t buffer_allocation_failures;
    uint32_t total_blocks_processed;
} mic_state;

K_MEM_SLAB_DEFINE(rx_mem_slab, PCM_BLK_SIZE_MS, BUFFER_POOL_SIZE, 1);

static struct pcm_stream_cfg mic_streams = {
    .pcm_rate = AUDIO_FREQ,
    .pcm_width = CHAN_SIZE,
    .block_size = PCM_BLK_SIZE_MS,
    .mem_slab = &rx_mem_slab,
};

static struct dmic_cfg dmic_config = {
    .io = {
        .min_pdm_clk_freq = 1000000,
        .max_pdm_clk_freq = 4800000,
        .min_pdm_clk_dc   = 40,
        .max_pdm_clk_dc   = 60,
    },
    .streams = &mic_streams,
    .channel = {
        .req_num_chan = 1,
        .req_num_streams = 1
    },
};

static K_THREAD_STACK_DEFINE(audio_thread_stack, 1024);

static void audio_thread_entry(void *p1, void *p2, void *p3);
static int power_on_microphone(void);
static int power_off_microphone(void);

int zsw_microphone_init(zsw_mic_audio_cb_t audio_callback)
{
    int ret;

    if (mic_state.initialized) {
        LOG_WRN("Microphone already initialized");
        return 0;
    }

    if (!audio_callback) {
        LOG_ERR("Audio callback is NULL");
        return -EINVAL;
    }

    LOG_INF("Initializing microphone");

    memset(&mic_state, 0, sizeof(mic_state));
    mic_state.audio_callback = audio_callback;

    mic_state.enable_gpio = (struct gpio_dt_spec)GPIO_DT_SPEC_GET_OR(DT_NODELABEL(mic_pwr), enable_gpios, {});

    if (mic_state.enable_gpio.port && !device_is_ready(mic_state.enable_gpio.port)) {
        LOG_ERR("Microphone power GPIO not ready");
    }

    if (mic_state.enable_gpio.port) {
        ret = gpio_pin_configure_dt(&mic_state.enable_gpio, GPIO_OUTPUT_LOW);
        if (ret != 0) {
            LOG_ERR("Failed to configure microphone power GPIO: %d", ret);
            return ret;
        }
    }

    mic_state.mic_dev = DEVICE_DT_GET(DT_NODELABEL(dmic_dev));
    if (!device_is_ready(mic_state.mic_dev)) {
        LOG_ERR("Microphone device %s is not ready", mic_state.mic_dev->name);
        return -ENODEV;
    }

    dmic_config.channel.req_chan_map_lo = dmic_build_channel_map(0, 0, PDM_CHAN_LEFT);

    mic_state.initialized = true;
    LOG_INF("Microphone initialized successfully");

    return 0;
}

int zsw_microphone_driver_start(void)
{
    int ret;

    if (!mic_state.initialized) {
        LOG_ERR("Microphone not initialized");
        return -EINVAL;
    }

    if (mic_state.recording) {
        LOG_WRN("Already recording");
        return -EALREADY;
    }

    LOG_INF("Starting microphone recording");

    ret = power_on_microphone();
    if (ret < 0) {
        LOG_ERR("Failed to power on microphone: %d", ret);
        return ret;
    }

    ret = dmic_configure(mic_state.mic_dev, &dmic_config);
    if (ret < 0) {
        LOG_ERR("Failed to configure DMIC: %d", ret);
        power_off_microphone();
        return ret;
    }

    ret = dmic_trigger(mic_state.mic_dev, DMIC_TRIGGER_START);
    if (ret < 0) {
        LOG_ERR("Failed to start DMIC: %d", ret);
        power_off_microphone();
        return ret;
    }

    mic_state.recording = true;

    mic_state.buffer_allocation_failures = 0;
    mic_state.total_blocks_processed = 0;

    mic_state.audio_thread_id = k_thread_create(&mic_state.audio_thread, audio_thread_stack,
                                                K_THREAD_STACK_SIZEOF(audio_thread_stack),
                                                audio_thread_entry, NULL, NULL, NULL,
                                                K_PRIO_COOP(8), 0, K_NO_WAIT);
    k_thread_name_set(&mic_state.audio_thread, "audio_mic");

    LOG_INF("Microphone recording started, block size: %d bytes", PCM_BLK_SIZE_MS);
    return 0;
}

int zsw_microphone_driver_stop(void)
{
    int ret;

    if (!mic_state.recording) {
        LOG_WRN("Not currently recording");
        return -EINVAL;
    }

    LOG_INF("Stopping microphone recording");

    // Flag to the audio thread to exit loop
    mic_state.recording = false;

    ret = dmic_trigger(mic_state.mic_dev, DMIC_TRIGGER_STOP);
    if (ret < 0) {
        LOG_ERR("Failed to stop DMIC: %d", ret);
    }

    LOG_DBG("Waiting for audio thread to complete...");
    ret = k_thread_join(&mic_state.audio_thread, K_FOREVER);
    if (ret != 0) {
        LOG_ERR("Failed to join audio thread: %d", ret);
    } else {
        LOG_DBG("Audio thread completed successfully");
    }

    power_off_microphone();

    LOG_INF("Microphone recording stopped");
    return 0;
}

int zsw_microphone_get_config(uint16_t *sample_rate, uint8_t *bit_depth)
{
    if (!sample_rate || !bit_depth) {
        return -EINVAL;
    }

    *sample_rate = AUDIO_FREQ;
    *bit_depth = CHAN_SIZE;
    return 0;
}

static void audio_thread_entry(void *p1, void *p2, void *p3)
{
    void *rx_block_ptr;
    size_t rx_size = PCM_BLK_SIZE_MS;
    int ret;

    LOG_INF("Audio processing thread started");

    while (mic_state.recording) {
        ret = dmic_read(mic_state.mic_dev, 0, &rx_block_ptr, &rx_size, 100);

        // Check if we should stop immediately after read attempt
        if (!mic_state.recording) {
            LOG_DBG("Recording stopped during read, exiting");
            break;
        }

        if (ret < 0) {
            if (ret == -EAGAIN || ret == -EWOULDBLOCK) {
                k_msleep(5);
                continue;
            }

            if (ret == -ENOMEM) {
                mic_state.buffer_allocation_failures++;
                LOG_WRN("Buffer allocation failed (%d times), continuing... (error: %d)",
                        mic_state.buffer_allocation_failures, ret);
                k_msleep(10);
                continue;
            }

            LOG_ERR("DMIC read error: %d", ret);
            k_msleep(10);
            continue;
        }

        if (mic_state.audio_callback) {
            mic_state.audio_callback(rx_block_ptr, rx_size);
        }

        // Free the memory block immediately
        // Possibly later when processing the audio data more we will need to allow
        // reuse of the memory block and have a function to free it later from the user.
        k_mem_slab_free(&rx_mem_slab, rx_block_ptr);
        mic_state.total_blocks_processed++;

        // Yield to let other threads run. May or may not actually be needed.
        k_yield();
    }

    LOG_INF("Recording session complete. Processed: %d blocks. Buffer failures: %d",
            mic_state.total_blocks_processed, mic_state.buffer_allocation_failures);

    LOG_INF("Audio processing thread exiting");
}

static int power_on_microphone(void)
{
    if (mic_state.enable_gpio.port == NULL) {
        return 0;
    }
    int ret = gpio_pin_set_dt(&mic_state.enable_gpio, 1);
    if (ret == 0) {
        LOG_DBG("Microphone powered on");
        k_sleep(K_MSEC(1));
    }
    return ret;
}

static int power_off_microphone(void)
{
    if (mic_state.enable_gpio.port == NULL) {
        return 0;
    }
    int ret = gpio_pin_set_dt(&mic_state.enable_gpio, 0);
    if (ret == 0) {
        LOG_DBG("Microphone powered off");
    }
    return ret;
}
