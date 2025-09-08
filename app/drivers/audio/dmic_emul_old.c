/**
 * @file
 *
 * @brief Emulated DMIC driver
 */

/*
 * Copyright (c) 2025 Jakob Krantz
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
 * @brief Generate sine wave samples
 *
 * @param data Internal data of DMIC emulator
 * @param buffer Output buffer for samples
 * @param samples Number of samples to generate
 */
static void dmic_emul_generate_sine_wave(struct dmic_emul_data *data,
                                         int16_t *buffer, size_t samples)
{
    const uint32_t phase_increment = (data->sine_freq * UINT32_MAX) / data->pcm_rate;

    for (size_t i = 0; i < samples; i++) {
        /* Generate sine wave sample */
        float phase_rad = (float)data->phase_accumulator * 2.0f * M_PI / UINT32_MAX;
        int16_t sample = (int16_t)(sinf(phase_rad) * data->amplitude);

        /* Store sample for each channel */
        if (data->num_channels == 1) {
            buffer[i] = sample;
        } else {
            /* Stereo: same signal on both channels for simplicity */
            buffer[i * 2] = sample;     /* Left channel */
            buffer[i * 2 + 1] = sample; /* Right channel */
        }

        /* Update phase accumulator */
        data->phase_accumulator += phase_increment;
    }
}

/**
 * @brief Main function of thread which generates audio data
 *
 * @param p1 Internal data of DMIC emulator
 * @param p2 Unused
 * @param p3 Unused
 */
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

            /* Allocate buffer from memory slab */
            ret = k_mem_slab_alloc(data->mem_slab, &buffer, K_NO_WAIT);
            if (ret < 0) {
                LOG_WRN("Failed to allocate buffer: %d", ret);
                k_sleep(K_MSEC(1));
                continue;
            }

            /* Calculate number of samples to generate based on block size */
            size_t bytes_per_sample = data->pcm_width / 8;
            samples_per_buffer = data->block_size / bytes_per_sample;
            if (data->num_channels == 2) {
                samples_per_buffer /= 2; /* Account for stereo */
            }

            LOG_INF("Block calculation: block_size=%u, pcm_width=%u, bytes_per_sample=%zu, samples_per_buffer=%zu, channels=%u",
                    data->block_size, data->pcm_width, bytes_per_sample, samples_per_buffer, data->num_channels);

            /* Generate audio data */
            k_mutex_lock(&data->cfg_mtx, K_FOREVER);
            dmic_emul_generate_sine_wave(data, (int16_t *)buffer, samples_per_buffer);
            k_mutex_unlock(&data->cfg_mtx);

            /* Queue the buffer */
            ret = k_msgq_put(&data->rx_queue, &buffer, K_NO_WAIT);
            if (ret < 0) {
                static uint32_t queue_failures = 0;
                queue_failures++;
                if ((queue_failures % 50) == 1) {
                    LOG_WRN("Failed to queue buffer: %d (failure #%u)", ret, queue_failures);
                }
                k_mem_slab_free(data->mem_slab, buffer);
                k_sleep(K_MSEC(1));
                continue;
            }

            LOG_INF("Generated buffer with %zu samples (%u bytes) at %u Hz",
                    samples_per_buffer, data->block_size, data->pcm_rate);

            /* Sleep for duration that this buffer represents in real time */
            sleep_ms = (samples_per_buffer * 1000) / data->pcm_rate;
            if (sleep_ms == 0) {
                sleep_ms = 1; /* Minimum sleep for very small buffers */
            }

            LOG_INF("Sleep calculation: samples=%zu, rate=%u, sleep_ms=%u",
                    samples_per_buffer, data->pcm_rate, sleep_ms);

            /* For 16kHz with 16 samples per buffer, this should be 1ms */
            /* native_sim timing seems to be off, so just yield CPU briefly */
            k_yield();
        }

        /* Clean up any remaining buffers when stopping */
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
    /* const struct dmic_emul_config *cfg = dev->config; */
    struct pdm_chan_cfg *channel = &config->channel;
    struct pcm_stream_cfg *stream = &config->streams[0];

    if (data->active) {
        LOG_ERR("Cannot configure device while it is active");
        return -EBUSY;
    }

    /* Validate stream configuration */
    if (channel->req_num_streams != 1) {
        LOG_ERR("Only 1 stream supported, requested %d", channel->req_num_streams);
        return -EINVAL;
    }

    if (channel->req_num_chan > DMIC_EMUL_MAX_CHANNELS || channel->req_num_chan < 1) {
        LOG_ERR("Unsupported channel count: %d", channel->req_num_chan);
        return -EINVAL;
    }

    /* If either rate or width is 0, disable the stream */
    if (stream->pcm_rate == 0 || stream->pcm_width == 0) {
        data->configured = false;
        return 0;
    }

    if (stream->pcm_width != 16) {
        LOG_ERR("Only 16-bit samples are supported, requested %d", stream->pcm_width);
        return -EINVAL;
    }

    k_mutex_lock(&data->cfg_mtx, K_FOREVER);

    /* Store configuration */
    data->current_cfg = *config;
    data->mem_slab = stream->mem_slab;
    data->block_size = stream->block_size;
    data->pcm_rate = stream->pcm_rate;
    data->pcm_width = stream->pcm_width;
    data->num_channels = channel->req_num_chan;

    /* Set actual channel configuration */
    channel->act_num_streams = 1;
    channel->act_num_chan = channel->req_num_chan;
    channel->act_chan_map_lo = channel->req_chan_map_lo;
    channel->act_chan_map_hi = 0;

    data->configured = true;

    k_mutex_unlock(&data->cfg_mtx);

    LOG_INF("DMIC configured: %u Hz, %u channels, %u bytes per block, %u samples per block, %u ms per block",
            data->pcm_rate, data->num_channels, data->block_size,
            data->block_size / (data->pcm_width / 8) / data->num_channels,
            (data->block_size / (data->pcm_width / 8) / data->num_channels * 1000) / data->pcm_rate);

    return 0;
}

static int dmic_emul_trigger(const struct device *dev, enum dmic_trigger cmd)
{
    struct dmic_emul_data *data = dev->data;

    switch (cmd) {
        case DMIC_TRIGGER_PAUSE:
        case DMIC_TRIGGER_STOP:
            if (data->active) {
                data->stopping = true;
                data->active = false;
                LOG_DBG("DMIC stopped");
            }
            break;

        case DMIC_TRIGGER_RELEASE:
        case DMIC_TRIGGER_START:
            if (!data->configured) {
                LOG_ERR("Device is not configured");
                return -EIO;
            }
            if (!data->active) {
                data->stopping = false;
                data->active = true;
                k_sem_give(&data->sem);
                LOG_DBG("DMIC started");
            }
            break;

        case DMIC_TRIGGER_RESET:
            data->active = false;
            data->stopping = true;
            data->phase_accumulator = 0;
            LOG_DBG("DMIC reset");
            break;

        default:
            LOG_ERR("Invalid trigger command: %d", cmd);
            return -EINVAL;
    }

    return 0;
}

static int dmic_emul_read(const struct device *dev, uint8_t stream,
                          void **buffer, size_t *size, int32_t timeout)
{
    struct dmic_emul_data *data = dev->data;
    int ret;

    ARG_UNUSED(stream);

    if (!data->configured) {
        LOG_ERR("Device is not configured");
        return -EIO;
    }

    ret = k_msgq_get(&data->rx_queue, buffer, SYS_TIMEOUT_MS(timeout));
    if (ret != 0) {
        if (ret == -EAGAIN) {
            LOG_DBG("No audio data available");
        } else {
            LOG_ERR("Failed to get buffer: %d", ret);
        }
        return ret;
    }

    *size = data->block_size;
    LOG_DBG("Provided buffer %p with %zu bytes", *buffer, *size);

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
    data->stopping = false;

    /* Initialize audio generation parameters */
    data->sine_freq = config->default_sine_freq;
    data->amplitude = config->default_amplitude;
    data->phase_accumulator = 0;

    /* Initialize synchronization objects */
    k_sem_init(&data->sem, 0, 1);
    k_mutex_init(&data->cfg_mtx);
    k_msgq_init(&data->rx_queue, (char *)data->rx_msgs, sizeof(void *),
                ARRAY_SIZE(data->rx_msgs));

    /* Create generation thread */
    k_thread_create(&data->thread, data->stack,
                    CONFIG_DMIC_EMUL_THREAD_STACK_SIZE,
                    dmic_emul_generation_thread,
                    data, NULL, NULL,
                    CONFIG_DMIC_EMUL_THREAD_PRIORITY,
                    0, K_NO_WAIT);
    k_thread_name_set(&data->thread, "dmic_emul");

    LOG_INF("DMIC emulator initialized: %u Hz sine wave, amplitude %d, device: %s",
            data->sine_freq, data->amplitude, dev->name);

    return 0;
}

#define DMIC_EMUL_INIT(_num)                            \
    static const struct dmic_emul_config dmic_emul_config_##_num = {    \
        .max_streams = DT_INST_PROP_OR(_num, max_streams, 1),       \
        .default_sine_freq = DT_INST_PROP_OR(_num, default_sine_freq,   \
                             DMIC_EMUL_DEFAULT_SINE_FREQ), \
        .default_amplitude = DT_INST_PROP_OR(_num, default_amplitude,   \
                             DMIC_EMUL_SINE_AMPLITUDE), \
    };                                  \
                                        \
    static struct dmic_emul_data dmic_emul_data_##_num;         \
                                        \
    DEVICE_DT_INST_DEFINE(_num, dmic_emul_init, NULL,           \
                  &dmic_emul_data_##_num,               \
                  &dmic_emul_config_##_num,             \
                  POST_KERNEL,                  \
                  CONFIG_AUDIO_DMIC_INIT_PRIORITY,          \
                  &dmic_emul_ops);

DT_INST_FOREACH_STATUS_OKAY(DMIC_EMUL_INIT)
