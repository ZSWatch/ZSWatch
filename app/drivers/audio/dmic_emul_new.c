/**
 * @file
 *
 * @brief Emulated DMIC driver - On-demand generation approach
 */

/*
 * Copyright (c) 2025
 *
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
#define DMIC_EMUL_DEFAULT_SINE_FREQ 1000  /* 1kHz sine wave */
#define DMIC_EMUL_SINE_AMPLITUDE 16384    /* 50% of 16-bit range */

/**
 * @brief Emulated DMIC config
 *
 * This structure contains constant data for given instance of emulated DMIC.
 */
struct dmic_emul_config {
    /** Maximum number of supported streams */
    uint8_t max_streams;
    /** Default sine wave frequency in Hz */
    uint32_t default_sine_freq;
    /** Default sine wave amplitude */
    int16_t default_amplitude;
};

/**
 * @brief Emulated DMIC data
 *
 * This structure contains data structures used by a emulated DMIC.
 */
struct dmic_emul_data {
    /** Device instance */
    const struct device *dev;
    /** Configuration state */
    bool configured;
    /** Active state */
    bool active;
    /** Configuration mutex */
    struct k_mutex cfg_mtx;

    /** Audio generation parameters */
    uint32_t sine_freq;
    int16_t amplitude;
    double phase_accumulator;

    /** PCM configuration */
    uint32_t pcm_rate;
    uint16_t pcm_width;
    uint16_t num_channels;
    size_t block_size;
    struct k_mem_slab *mem_slab;

    /** Timing for on-demand generation */
    int64_t start_time_us;
    uint64_t total_samples_generated;
};

/**
 * @brief Generate sine wave data for specified number of samples
 *
 * @param data DMIC emulator data
 * @param buffer Output buffer for samples
 * @param samples Number of samples to generate
 */
static void dmic_emul_generate_sine_wave(struct dmic_emul_data *data, int16_t *buffer, size_t samples)
{
    double phase_step = 2.0 * M_PI * data->sine_freq / data->pcm_rate;

    for (size_t i = 0; i < samples; i++) {
        double sample = sin(data->phase_accumulator) * data->amplitude;
        buffer[i] = (int16_t)sample;
        data->phase_accumulator += phase_step;

        /* Keep phase accumulator in reasonable range */
        if (data->phase_accumulator >= 2.0 * M_PI) {
            data->phase_accumulator -= 2.0 * M_PI;
        }
    }
}

static int dmic_emul_configure(const struct device *dev, struct dmic_cfg *config)
{
    struct dmic_emul_data *data = dev->data;
    struct pdm_chan_cfg *channel = &config->channel;
    struct pcm_stream_cfg *stream = &config->streams[0];

    if (data->active) {
        LOG_ERR("Cannot configure device while it is active");
        return -EBUSY;
    }

    if (config->channel.req_num_streams > DMIC_EMUL_MAX_STREAMS) {
        LOG_ERR("Unsupported number of streams: %d", config->channel.req_num_streams);
        return -EINVAL;
    }

    if (channel->req_num_chan > DMIC_EMUL_MAX_CHANNELS) {
        LOG_ERR("Unsupported number of channels: %d", channel->req_num_chan);
        return -EINVAL;
    }

    k_mutex_lock(&data->cfg_mtx, K_FOREVER);

    /* Store PCM configuration */
    data->pcm_rate = stream->pcm_rate;
    data->pcm_width = stream->pcm_width;
    data->block_size = stream->block_size;
    data->mem_slab = stream->mem_slab;
    data->num_channels = channel->req_num_chan;

    data->configured = true;

    /* Calculate samples per block for logging */
    size_t bytes_per_sample = data->pcm_width / 8;
    size_t samples_per_block = data->block_size / bytes_per_sample;
    if (data->num_channels == 2) {
        samples_per_block /= 2;
    }
    uint32_t ms_per_block = (samples_per_block * 1000) / data->pcm_rate;

    LOG_INF("DMIC configured: %u Hz, %u channels, %zu bytes per block, %zu samples per block, %u ms per block",
            data->pcm_rate, data->num_channels, data->block_size, samples_per_block, ms_per_block);

    k_mutex_unlock(&data->cfg_mtx);

    return 0;
}

static int dmic_emul_trigger(const struct device *dev, enum dmic_trigger cmd)
{
    struct dmic_emul_data *data = dev->data;

    if (!data->configured) {
        LOG_ERR("Device not configured");
        return -EACCES;
    }

    k_mutex_lock(&data->cfg_mtx, K_FOREVER);

    switch (cmd) {
        case DMIC_TRIGGER_START:
            if (data->active) {
                LOG_WRN("Device already active");
                k_mutex_unlock(&data->cfg_mtx);
                return -EALREADY;
            }

            data->active = true;
            data->start_time_us = k_uptime_get() * 1000LL; /* Convert ms to us */
            data->total_samples_generated = 0;
            data->phase_accumulator = 0.0; /* Reset phase for consistent output */

            LOG_DBG("DMIC started at time %lld us", data->start_time_us);
            break;

        case DMIC_TRIGGER_STOP:
            if (!data->active) {
                LOG_WRN("Device not active");
                k_mutex_unlock(&data->cfg_mtx);
                return -EALREADY;
            }

            data->active = false;
            LOG_DBG("DMIC stopped after generating %llu samples", data->total_samples_generated);
            break;

        default:
            LOG_ERR("Unsupported trigger command: %d", cmd);
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
        LOG_ERR("Device not configured");
        return -EACCES;
    }

    if (!data->active) {
        LOG_DBG("Device not active");
        return -EAGAIN;
    }

    if (stream >= DMIC_EMUL_MAX_STREAMS) {
        LOG_ERR("Invalid stream: %d", stream);
        return -EINVAL;
    }

    /* Allocate buffer from memory slab */
    ret = k_mem_slab_alloc(data->mem_slab, buffer, SYS_TIMEOUT_MS(timeout));
    if (ret != 0) {
        if (ret == -EAGAIN) {
            LOG_DBG("No memory available");
        } else {
            LOG_ERR("Failed to allocate buffer: %d", ret);
        }
        return ret;
    }

    k_mutex_lock(&data->cfg_mtx, K_FOREVER);

    /* Calculate number of samples to generate based on block size */
    size_t bytes_per_sample = data->pcm_width / 8;
    size_t samples_per_buffer = data->block_size / bytes_per_sample;
    if (data->num_channels == 2) {
        samples_per_buffer /= 2; /* Account for stereo */
    }

    /* Generate audio data on-demand */
    dmic_emul_generate_sine_wave(data, (int16_t *)*buffer, samples_per_buffer);

    /* Update tracking */
    data->total_samples_generated += samples_per_buffer;

    k_mutex_unlock(&data->cfg_mtx);

    *size = data->block_size;
    LOG_DBG("Generated buffer %p with %zu samples (%zu bytes)", *buffer, samples_per_buffer, *size);

    return 0;
}

static const struct _dmic_ops dmic_emul_ops = {
    .configure = dmic_emul_configure,
    .trigger = dmic_emul_trigger,
    .read = dmic_emul_read,
};

/**
 * @brief Initialize DMIC emulator device
 *
 * @param dev DMIC emulator device
 *
 * @return 0 on success
 */
static int dmic_emul_init(const struct device *dev)
{
    const struct dmic_emul_config *config = dev->config;
    struct dmic_emul_data *data = dev->data;

    LOG_INF("DMIC emulator init starting...");

    data->dev = dev;
    data->configured = false;
    data->active = false;

    /* Initialize audio generation parameters */
    data->sine_freq = config->default_sine_freq;
    data->amplitude = config->default_amplitude;
    data->phase_accumulator = 0;
    data->total_samples_generated = 0;

    /* Initialize synchronization objects */
    k_mutex_init(&data->cfg_mtx);

    LOG_INF("DMIC emulator initialized: %u Hz sine wave, amplitude %d, device: %s",
            data->sine_freq, data->amplitude, dev->name);

    return 0;
}

#define DMIC_EMUL_INIT(inst)                            \
    static const struct dmic_emul_config dmic_emul_config_##inst = {    \
        .max_streams = DMIC_EMUL_MAX_STREAMS,               \
        .default_sine_freq = DT_INST_PROP_OR(inst, sine_freq,       \
                         DMIC_EMUL_DEFAULT_SINE_FREQ),  \
        .default_amplitude = DT_INST_PROP_OR(inst, amplitude,       \
                         DMIC_EMUL_SINE_AMPLITUDE), \
    };                                  \
                                        \
    static struct dmic_emul_data dmic_emul_data_##inst;         \
                                        \
    DEVICE_DT_INST_DEFINE(inst, dmic_emul_init, NULL,           \
                  &dmic_emul_data_##inst,               \
                  &dmic_emul_config_##inst,             \
                  POST_KERNEL, CONFIG_AUDIO_INIT_PRIORITY,      \
                  &dmic_emul_ops);

DT_INST_FOREACH_STATUS_OKAY(DMIC_EMUL_INIT)
