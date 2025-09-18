/*
 * Copyright (c) 2025 ZSWatch Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT zephyr_dmic_emul

#include <zephyr/audio/dmic.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

LOG_MODULE_REGISTER(dmic_emul, CONFIG_AUDIO_DMIC_LOG_LEVEL);

#define DMIC_EMUL_MAX_CHANNELS 2
#define DMIC_EMUL_MAX_STREAMS 1
#define DMIC_EMUL_DEFAULT_SINE_FREQ 440
#define DMIC_EMUL_DEFAULT_SINE_AMPLITUDE 20000
#define DMIC_EMUL_QUEUE_SIZE 4

struct dmic_emul_config {
    uint8_t max_streams;
    uint32_t default_sine_freq;
    int16_t default_amplitude;
};

struct dmic_emul_data {
    const struct device *dev;
    bool configured;
    bool active;
    bool stopping;
    struct k_mutex cfg_mtx;

    uint32_t sine_freq;
    int16_t amplitude;
    double phase_accumulator;

    uint32_t pcm_rate;
    uint16_t pcm_width;
    uint16_t num_channels;
    size_t block_size;
    struct k_mem_slab *mem_slab;

    struct k_thread thread;
    k_thread_stack_t stack[2048];
    struct k_sem sem;
    struct k_msgq rx_queue;
    void *rx_msgs[DMIC_EMUL_QUEUE_SIZE];

    uint64_t total_samples_generated;
    uint32_t queue_failures;
};

static void dmic_emul_generate_sine_wave(struct dmic_emul_data *data, int16_t *buffer, size_t samples)
{
    double phase_step = 2.0 * M_PI * data->sine_freq / data->pcm_rate;

    for (size_t i = 0; i < samples; i++) {
        double sample = sin(data->phase_accumulator) * data->amplitude;
        buffer[i] = (int16_t)sample;
        data->phase_accumulator += phase_step;

        while (data->phase_accumulator >= 2.0 * M_PI) {
            data->phase_accumulator -= 2.0 * M_PI;
        }
    }
}

static void dmic_emul_generation_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct dmic_emul_data *data = p1;

    while (true) {
        k_sem_take(&data->sem, K_FOREVER);

        while (data->active && !data->stopping) {
            void *buffer;
            int ret;
            size_t samples_per_buffer;
            uint32_t sleep_ms;

            ret = k_mem_slab_alloc(data->mem_slab, &buffer, K_NO_WAIT);
            if (ret < 0) {
                k_sleep(K_MSEC(10));
                continue;
            }

            k_mutex_lock(&data->cfg_mtx, K_FOREVER);

            size_t bytes_per_sample = data->pcm_width / 8;
            samples_per_buffer = data->block_size / bytes_per_sample;
            if (data->num_channels == 2) {
                samples_per_buffer /= 2;
            }

            dmic_emul_generate_sine_wave(data, (int16_t *)buffer, samples_per_buffer);
            data->total_samples_generated += samples_per_buffer;

            k_mutex_unlock(&data->cfg_mtx);

            ret = k_msgq_put(&data->rx_queue, &buffer, K_NO_WAIT);
            if (ret < 0) {
                data->queue_failures++;
                k_mem_slab_free(data->mem_slab, buffer);
                k_sleep(K_MSEC(1));
                continue;
            }

            sleep_ms = (samples_per_buffer * 1000) / data->pcm_rate;
            if (sleep_ms == 0) {
                sleep_ms = 1;
            }

            k_sleep(K_MSEC(sleep_ms));
        }

        if (data->stopping) {
            void *buffer;
            while (k_msgq_get(&data->rx_queue, &buffer, K_NO_WAIT) == 0) {
                k_mem_slab_free(data->mem_slab, buffer);
            }
            data->stopping = false;
        }
    }
}

static int dmic_emul_configure(const struct device *dev, struct dmic_cfg *config)
{
    struct dmic_emul_data *data = dev->data;
    struct pdm_chan_cfg *channel = &config->channel;
    struct pcm_stream_cfg *stream = &config->streams[0];

    if (data->active) {
        return -EBUSY;
    }

    if (config->channel.req_num_streams > DMIC_EMUL_MAX_STREAMS) {
        return -EINVAL;
    }

    if (channel->req_num_chan > DMIC_EMUL_MAX_CHANNELS) {
        return -EINVAL;
    }

    k_mutex_lock(&data->cfg_mtx, K_FOREVER);

    data->pcm_rate = stream->pcm_rate;
    data->pcm_width = stream->pcm_width;
    data->block_size = stream->block_size;
    data->mem_slab = stream->mem_slab;
    data->num_channels = channel->req_num_chan;

    channel->act_num_streams = 1;
    channel->act_num_chan = channel->req_num_chan;
    channel->act_chan_map_lo = channel->req_chan_map_lo;
    channel->act_chan_map_hi = 0;

    data->configured = true;

    size_t bytes_per_sample = data->pcm_width / 8;
    size_t samples_per_block = data->block_size / bytes_per_sample;
    if (data->num_channels == 2) {
        samples_per_block /= 2;
    }
    uint32_t ms_per_block = (samples_per_block * 1000) / data->pcm_rate;

    LOG_INF("DMIC configured: %u Hz, %u channels, %zu bytes per block, %u ms per block",
            data->pcm_rate, data->num_channels, data->block_size, ms_per_block);

    k_mutex_unlock(&data->cfg_mtx);
    return 0;
}

static int dmic_emul_trigger(const struct device *dev, enum dmic_trigger cmd)
{
    struct dmic_emul_data *data = dev->data;

    if (!data->configured) {
        return -EACCES;
    }

    k_mutex_lock(&data->cfg_mtx, K_FOREVER);

    switch (cmd) {
        case DMIC_TRIGGER_START:
            if (data->active) {
                k_mutex_unlock(&data->cfg_mtx);
                return -EALREADY;
            }

            data->active = true;
            data->stopping = false;
            data->total_samples_generated = 0;
            data->queue_failures = 0;

            k_sem_give(&data->sem);
            break;

        case DMIC_TRIGGER_STOP:
            if (!data->active) {
                k_mutex_unlock(&data->cfg_mtx);
                return -EALREADY;
            }

            data->stopping = true;
            data->active = false;
            break;

        default:
            k_mutex_unlock(&data->cfg_mtx);
            return -EINVAL;
    }

    k_mutex_unlock(&data->cfg_mtx);
    return 0;
}

static int dmic_emul_read(const struct device *dev, uint8_t stream, void **buffer,
                          size_t *size, int32_t timeout)
{
    struct dmic_emul_data *data = dev->data;
    int ret;

    if (!data->configured) {
        return -EACCES;
    }

    if (!data->active) {
        return -EAGAIN;
    }

    if (stream >= DMIC_EMUL_MAX_STREAMS) {
        return -EINVAL;
    }

    ret = k_msgq_get(&data->rx_queue, buffer, SYS_TIMEOUT_MS(timeout));
    if (ret != 0) {
        return ret;
    }

    *size = data->block_size;
    return 0;
}

static const struct _dmic_ops dmic_emul_ops = {
    .configure = dmic_emul_configure,
    .trigger = dmic_emul_trigger,
    .read = dmic_emul_read,
};

static int dmic_emul_init(const struct device *dev)
{
    const struct dmic_emul_config *config = dev->config;
    struct dmic_emul_data *data = dev->data;

    data->dev = dev;
    data->configured = false;
    data->active = false;
    data->stopping = false;

    data->sine_freq = config->default_sine_freq;
    data->amplitude = config->default_amplitude;
    data->phase_accumulator = 0;
    data->total_samples_generated = 0;
    data->queue_failures = 0;

    k_sem_init(&data->sem, 0, 1);
    k_mutex_init(&data->cfg_mtx);
    k_msgq_init(&data->rx_queue, (char *)data->rx_msgs, sizeof(void *),
                ARRAY_SIZE(data->rx_msgs));

    k_thread_create(&data->thread, data->stack,
                    sizeof(data->stack),
                    dmic_emul_generation_thread,
                    data, NULL, NULL,
                    K_PRIO_COOP(7),
                    0, K_NO_WAIT);
    k_thread_name_set(&data->thread, "dmic_emul");

    LOG_INF("DMIC emulator initialized: %u Hz sine wave, amplitude %d",
            data->sine_freq, data->amplitude);

    return 0;
}

#define DMIC_EMUL_INIT(inst)                            \
    static const struct dmic_emul_config dmic_emul_config_##inst = {    \
        .max_streams = DMIC_EMUL_MAX_STREAMS,               \
        .default_sine_freq = DT_INST_PROP_OR(inst, sine_freq,       \
                         DMIC_EMUL_DEFAULT_SINE_FREQ),  \
        .default_amplitude = DT_INST_PROP_OR(inst, amplitude,       \
                         DMIC_EMUL_DEFAULT_SINE_AMPLITUDE), \
    };                                  \
                                        \
    static struct dmic_emul_data dmic_emul_data_##inst;         \
                                        \
    DEVICE_DT_INST_DEFINE(inst, dmic_emul_init, NULL,           \
                  &dmic_emul_data_##inst,               \
                  &dmic_emul_config_##inst,             \
                  POST_KERNEL, CONFIG_AUDIO_DMIC_INIT_PRIORITY,     \
                  &dmic_emul_ops);

DT_INST_FOREACH_STATUS_OKAY(DMIC_EMUL_INIT)
