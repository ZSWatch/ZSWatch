/*
 * Copyright (c) 2021 - 2025, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NRFY_SAADC_H__
#define NRFY_SAADC_H__

#include <nrfx.h>
#include <hal/nrf_saadc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nrfy_saadc_buffer_t nrfy_saadc_buffer_t;

NRFY_STATIC_INLINE bool __nrfy_internal_saadc_event_handle(NRF_SAADC_Type *  p_reg,
                                                           uint32_t          mask,
                                                           nrf_saadc_event_t event,
                                                           uint32_t *        p_event_mask);

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_saadc_events_process(NRF_SAADC_Type *            p_reg,
                                              uint32_t                    mask,
                                              nrfy_saadc_buffer_t const * p_desc);

NRFY_STATIC_INLINE void __nrfy_internal_saadc_event_enabled_clear(NRF_SAADC_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_saadc_event_t event);

NRFY_STATIC_INLINE void __nrfy_internal_saadc_buffer_latch(NRF_SAADC_Type * p_reg, bool wait);

NRFY_STATIC_INLINE void __nrfy_internal_saadc_stop(NRF_SAADC_Type * p_reg, bool wait);

/**
 * @defgroup nrfy_saadc SAADC HALY
 * @{
 * @ingroup nrf_saadc
 * @brief   Hardware access layer with cache and barrier support for managing the SAADC peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

 #if NRF_SAADC_HAS_CAL || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SAADC_HAS_CAL} */
#define NRFY_SAADC_HAS_CAL 1
#else
#define NRFY_SAADC_HAS_CAL 0
#endif

#if NRF_SAADC_HAS_CALREF || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SAADC_HAS_CALREF} */
#define NRFY_SAADC_HAS_CALREF 1
#else
#define NRFY_SAADC_HAS_CALREF 0
#endif

#if NRF_SAADC_HAS_LIN_CAL || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SAADC_HAS_LIN_CAL} */
#define NRFY_SAADC_HAS_LIN_CAL 1
#else
#define NRFY_SAADC_HAS_LIN_CAL 0
#endif

/** @brief Structure describing SAADC sampling buffer. */
struct nrfy_saadc_buffer_t
{
    nrf_saadc_value_t * p_buffer; ///< Pointer to the sampling buffer.
    size_t              length;   ///< Sampling buffer length.
};

/** @brief SAADC configuration structure. */
typedef struct
{
    nrf_saadc_resolution_t resolution;   ///< Sampling resolution.
    nrf_saadc_oversample_t oversampling; ///< Oversampling setting.
#if NRF_SAADC_HAS_BURST
    nrf_saadc_burst_t      burst;        ///< Burst mode configuration.
#endif
} nrfy_saadc_config_t;

/** @brief SAADC channel input configuration structure. */
typedef struct
{
    nrf_saadc_input_t input_p; ///< Positive input.
    nrf_saadc_input_t input_n; ///< Negative input.
} nrfy_saadc_channel_input_t;

/**
 * @brief Function for configuring the SAADC.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_saadc_periph_configure(NRF_SAADC_Type *            p_reg,
                                                    nrfy_saadc_config_t const * p_config)
{
    nrf_saadc_resolution_set(p_reg, p_config->resolution);
    nrf_saadc_oversample_set(p_reg, p_config->oversampling);
#if NRF_SAADC_HAS_BURST
    nrf_saadc_burst_set(p_reg, p_config->burst);
#endif
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified SAADC interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_saadc_int_init(NRF_SAADC_Type * p_reg,
                                            uint32_t         mask,
                                            uint8_t          irq_priority,
                                            bool             enable)
{
    __nrfy_internal_saadc_event_enabled_clear(p_reg, mask, NRF_SAADC_EVENT_STARTED);
    __nrfy_internal_saadc_event_enabled_clear(p_reg, mask, NRF_SAADC_EVENT_END);
    __nrfy_internal_saadc_event_enabled_clear(p_reg, mask, NRF_SAADC_EVENT_DONE);
    __nrfy_internal_saadc_event_enabled_clear(p_reg, mask, NRF_SAADC_EVENT_RESULTDONE);
    __nrfy_internal_saadc_event_enabled_clear(p_reg, mask, NRF_SAADC_EVENT_CALIBRATEDONE);
    __nrfy_internal_saadc_event_enabled_clear(p_reg, mask, NRF_SAADC_EVENT_STOPPED);

    for (uint8_t i = 0; i < SAADC_CH_NUM; i++)
    {
         nrf_saadc_event_t event = nrf_saadc_limit_event_get(i, NRF_SAADC_LIMIT_LOW);
         __nrfy_internal_saadc_event_enabled_clear(p_reg, mask, event);

         event = nrf_saadc_limit_event_get(i, NRF_SAADC_LIMIT_HIGH);
         __nrfy_internal_saadc_event_enabled_clear(p_reg, mask, event);
    }

    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_saadc_int_enable(p_reg, mask);
    }

    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the SAADC interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_saadc_int_uninit(NRF_SAADC_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified SAADC events.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mask   Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 * @param[in] p_desc Pointer to the structure containing buffer associated with the last sampling.
 *                   Can be NULL.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_saadc_events_process(NRF_SAADC_Type *            p_reg,
                                                      uint32_t                    mask,
                                                      nrfy_saadc_buffer_t const * p_desc)
{
    uint32_t evt_mask = __nrfy_internal_saadc_events_process(p_reg, mask, p_desc);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for configuring the specified SAADC channel.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] channel  Channel number.
 * @param[in] p_config Pointer to the channel configuration structure.
 *                     NULL if configuration is to be omitted.
 * @param[in] p_input  Pointer to the channel input configuration structure.
 *                     NULL if configuration is to be omitted.
 */
NRFY_STATIC_INLINE void nrfy_saadc_channel_configure(NRF_SAADC_Type *                   p_reg,
                                                     uint8_t                            channel,
                                                     nrf_saadc_channel_config_t const * p_config,
                                                     nrfy_saadc_channel_input_t const * p_input)
{
    if (p_config)
    {
        nrf_saadc_channel_init(p_reg, channel, p_config);
    }
    if (p_input)
    {
        nrf_saadc_channel_input_set(p_reg, channel, p_input->input_p, p_input->input_n);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for setting the SAADC sampling buffer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_desc Pointer to the structure containing sampling buffer description.
 * @param[in] latch  True if buffer is to be latched, false otherwise.
 * @param[in] wait   True if latching is to be blocking, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_saadc_buffer_set(NRF_SAADC_Type *            p_reg,
                                              nrfy_saadc_buffer_t const * p_desc,
                                              bool                        latch,
                                              bool                        wait)
{
    nrf_saadc_buffer_init(p_reg, p_desc->p_buffer, p_desc->length);
    nrf_barrier_w();
    if (latch)
    {
        __nrfy_internal_saadc_buffer_latch(p_reg, wait);
    }
}

/**
 * @brief Function for latching the SAADC sampling buffer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] wait  True if latching is to be blocking, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_saadc_buffer_latch(NRF_SAADC_Type * p_reg, bool wait)
{
    __nrfy_internal_saadc_buffer_latch(p_reg, wait);
}

/**
 * @brief Function for starting the SAADC sampling.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_desc Pointer to the structure containing sampling buffer
 *                   if the sampling is to be blocking. NULL for non-blocking operation.
 */
NRFY_STATIC_INLINE void nrfy_saadc_sample_start(NRF_SAADC_Type *            p_reg,
                                                nrfy_saadc_buffer_t const * p_desc)
{
    nrf_saadc_task_trigger(p_reg, NRF_SAADC_TASK_SAMPLE);
    if (p_desc)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_SAADC_EVENT_END);
        while (!__nrfy_internal_saadc_events_process(p_reg, evt_mask, p_desc))
        {}
        nrf_saadc_event_clear(p_reg, NRF_SAADC_EVENT_DONE);
        nrf_saadc_event_clear(p_reg, NRF_SAADC_EVENT_RESULTDONE);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for aborting the ongoing SAADC sampling.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_desc Pointer to the structure containing sampling buffer
 *                   if the abort is to be blocking. NULL for non-blocking operation.
 */
NRFY_STATIC_INLINE void nrfy_saadc_abort(NRF_SAADC_Type *            p_reg,
                                         nrfy_saadc_buffer_t const * p_desc)
{
    __nrfy_internal_saadc_stop(p_reg, p_desc ? true : false);
    if (p_desc)
    {
        (void)__nrfy_internal_saadc_events_process(p_reg,
                                                   NRFY_EVENT_TO_INT_BITMASK(NRF_SAADC_EVENT_END),
                                                   p_desc);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for stopping the SAADC.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] wait  True if stopping is to be blocking, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_saadc_stop(NRF_SAADC_Type * p_reg, bool wait)
{
    __nrfy_internal_saadc_stop(p_reg, wait);
    nrf_barrier_w();
}

/**
 * @brief Function for calibrating the SAADC.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] wait  True if calibration is to be blocking, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_saadc_calibrate(NRF_SAADC_Type * p_reg, bool wait)
{
    nrf_saadc_task_trigger(p_reg, NRF_SAADC_TASK_CALIBRATEOFFSET);
    if (wait)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_SAADC_EVENT_CALIBRATEDONE);
        while (!__nrfy_internal_saadc_events_process(p_reg, evt_mask, NULL))
        {}
        nrf_saadc_event_clear(p_reg, NRF_SAADC_EVENT_END);
    }
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_task_trigger} */
NRFY_STATIC_INLINE void nrfy_saadc_task_trigger(NRF_SAADC_Type * p_reg, nrf_saadc_task_t task)
{
    nrf_saadc_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_saadc_task_address_get(NRF_SAADC_Type const * p_reg,
                                                        nrf_saadc_task_t       task)
{
    return nrf_saadc_task_address_get(p_reg, task);
}

/** @refhal{nrf_saadc_event_check} */
NRFY_STATIC_INLINE bool nrfy_saadc_event_check(NRF_SAADC_Type const * p_reg,
                                               nrf_saadc_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_saadc_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_saadc_event_clear} */
NRFY_STATIC_INLINE void nrfy_saadc_event_clear(NRF_SAADC_Type * p_reg, nrf_saadc_event_t event)
{
    nrf_saadc_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_saadc_event_address_get(NRF_SAADC_Type const * p_reg,
                                                         nrf_saadc_event_t      event)
{
    return nrf_saadc_event_address_get(p_reg, event);
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_saadc_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_saadc_subscribe_set(NRF_SAADC_Type * p_reg,
                                                 nrf_saadc_task_t task,
                                                 uint8_t          channel)
{
    nrf_saadc_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_saadc_subscribe_clear(NRF_SAADC_Type * p_reg, nrf_saadc_task_t task)
{
    nrf_saadc_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_publish_set} */
NRFY_STATIC_INLINE void nrfy_saadc_publish_set(NRF_SAADC_Type *  p_reg,
                                               nrf_saadc_event_t event,
                                               uint8_t           channel)
{
    nrf_saadc_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_publish_clear} */
NRFY_STATIC_INLINE void nrfy_saadc_publish_clear(NRF_SAADC_Type * p_reg, nrf_saadc_event_t event)
{
    nrf_saadc_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/** @refhal{nrf_saadc_limit_event_get} */
NRFY_STATIC_INLINE nrf_saadc_event_t nrfy_saadc_limit_event_get(uint8_t           channel,
                                                                nrf_saadc_limit_t limit_type)
{
    return nrf_saadc_limit_event_get(channel, limit_type);
}

/** @refhal{nrf_saadc_channel_input_set} */
NRFY_STATIC_INLINE void nrfy_saadc_channel_input_set(NRF_SAADC_Type *  p_reg,
                                                     uint8_t           channel,
                                                     nrf_saadc_input_t pselp,
                                                     nrf_saadc_input_t pseln)
{
    nrf_saadc_channel_input_set(p_reg, channel, pselp, pseln);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_channel_pos_input_set} */
NRFY_STATIC_INLINE void nrfy_saadc_channel_pos_input_set(NRF_SAADC_Type *  p_reg,
                                                         uint8_t           channel,
                                                         nrf_saadc_input_t pselp)
{
    nrf_saadc_channel_pos_input_set(p_reg, channel, pselp);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_channel_neg_input_set} */
NRFY_STATIC_INLINE void nrfy_saadc_channel_neg_input_set(NRF_SAADC_Type *  p_reg,
                                                         uint8_t           channel,
                                                         nrf_saadc_input_t pseln)
{
    nrf_saadc_channel_neg_input_set(p_reg, channel, pseln);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_channel_limits_set} */
NRFY_STATIC_INLINE void nrfy_saadc_channel_limits_set(NRF_SAADC_Type * p_reg,
                                                      uint8_t          channel,
                                                      int16_t          low,
                                                      int16_t          high)
{
    nrf_saadc_channel_limits_set(p_reg, channel, low, high);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_int_set} */
NRFY_STATIC_INLINE void nrfy_saadc_int_set(NRF_SAADC_Type * p_reg, uint32_t mask)
{
    nrf_saadc_int_set(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_int_enable} */
NRFY_STATIC_INLINE void nrfy_saadc_int_enable(NRF_SAADC_Type * p_reg, uint32_t mask)
{
    nrf_saadc_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_saadc_int_enable_check(NRF_SAADC_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_saadc_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_saadc_int_disable} */
NRFY_STATIC_INLINE void nrfy_saadc_int_disable(NRF_SAADC_Type * p_reg, uint32_t mask)
{
    nrf_saadc_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_limit_int_get} */
NRFY_STATIC_INLINE uint32_t nrfy_saadc_limit_int_get(uint8_t           channel,
                                                     nrf_saadc_limit_t limit_type)
{
    return nrf_saadc_limit_int_get(channel, limit_type);
}

/** @refhal{nrf_saadc_busy_check} */
NRFY_STATIC_INLINE bool nrfy_saadc_busy_check(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_saadc_busy_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_saadc_enable} */
NRFY_STATIC_INLINE void nrfy_saadc_enable(NRF_SAADC_Type * p_reg)
{
    nrf_saadc_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_disable} */
NRFY_STATIC_INLINE void nrfy_saadc_disable(NRF_SAADC_Type * p_reg)
{
    nrf_saadc_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_enable_check} */
NRFY_STATIC_INLINE bool nrfy_saadc_enable_check(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_saadc_enable_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_saadc_buffer_init} */
NRFY_STATIC_INLINE void nrfy_saadc_buffer_init(NRF_SAADC_Type *    p_reg,
                                               nrf_saadc_value_t * p_buffer,
                                               uint32_t            size)
{
    nrf_saadc_buffer_init(p_reg, p_buffer, size);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_buffer_pointer_set} */
NRFY_STATIC_INLINE void nrfy_saadc_buffer_pointer_set(NRF_SAADC_Type *    p_reg,
                                                      nrf_saadc_value_t * p_buffer)
{
    nrf_saadc_buffer_pointer_set(p_reg, p_buffer);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_buffer_pointer_get} */
NRFY_STATIC_INLINE nrf_saadc_value_t * nrfy_saadc_buffer_pointer_get(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_saadc_value_t * p_buffer = nrf_saadc_buffer_pointer_get(p_reg);
    nrf_barrier_r();
    return p_buffer;
}

/** @refhal{nrf_saadc_amount_get} */
NRFY_STATIC_INLINE uint16_t nrfy_saadc_amount_get(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_r();
    uint16_t amount = nrf_saadc_amount_get(p_reg);
    nrf_barrier_r();
    return amount;
}

/** @refhal{nrf_saadc_resolution_set} */
NRFY_STATIC_INLINE void nrfy_saadc_resolution_set(NRF_SAADC_Type *       p_reg,
                                                  nrf_saadc_resolution_t resolution)
{
    nrf_saadc_resolution_set(p_reg, resolution);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_resolution_get} */
NRFY_STATIC_INLINE nrf_saadc_resolution_t nrfy_saadc_resolution_get(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_saadc_resolution_t resolution = nrf_saadc_resolution_get(p_reg);
    nrf_barrier_r();
    return resolution;
}

/** @refhal{nrf_saadc_oversample_set} */
NRFY_STATIC_INLINE void nrfy_saadc_oversample_set(NRF_SAADC_Type *       p_reg,
                                                  nrf_saadc_oversample_t oversample)
{
    nrf_saadc_oversample_set(p_reg, oversample);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_oversample_get} */
NRFY_STATIC_INLINE nrf_saadc_oversample_t nrfy_saadc_oversample_get(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_saadc_oversample_t oversample = nrf_saadc_oversample_get(p_reg);
    nrf_barrier_r();
    return oversample;
}

/** @refhal{nrf_saadc_oversample_sample_count_get} */
NRFY_STATIC_INLINE
uint32_t nrfy_saadc_oversample_sample_count_get(nrf_saadc_oversample_t oversample)
{
    return nrf_saadc_oversample_sample_count_get(oversample);
}

/** @refhal{nrf_saadc_continuous_mode_enable} */
NRFY_STATIC_INLINE void nrfy_saadc_continuous_mode_enable(NRF_SAADC_Type * p_reg, uint16_t cc)
{
    nrf_saadc_continuous_mode_enable(p_reg, cc);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_continuous_mode_enable_check} */
NRFY_STATIC_INLINE bool nrfy_saadc_continuous_mode_enable_check(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_saadc_continuous_mode_enable_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_saadc_continuous_mode_disable} */
NRFY_STATIC_INLINE void nrfy_saadc_continuous_mode_disable(NRF_SAADC_Type * p_reg)
{
    nrf_saadc_continuous_mode_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_channel_init} */
NRFY_STATIC_INLINE void nrfy_saadc_channel_init(NRF_SAADC_Type *                   p_reg,
                                                uint8_t                            channel,
                                                nrf_saadc_channel_config_t const * config)
{
    nrf_saadc_channel_init(p_reg, channel, config);
    nrf_barrier_w();
}

#if NRF_SAADC_HAS_CH_BURST
/** @refhal{nrf_saadc_channel_burst_set} */
NRFY_STATIC_INLINE void nrfy_saadc_channel_burst_set(NRF_SAADC_Type *  p_reg,
                                                     uint8_t           channel,
                                                     nrf_saadc_burst_t burst)
{
    nrf_saadc_channel_burst_set(p_reg, channel, burst);
    nrf_barrier_w();
}
#endif

#if NRF_SAADC_HAS_BURST
/** @refhal{nrf_saadc_burst_set} */
NRFY_STATIC_INLINE void nrfy_saadc_burst_set(NRF_SAADC_Type *  p_reg,
                                             nrf_saadc_burst_t burst)
{
    nrf_saadc_burst_set(p_reg, burst);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_saadc_value_min_get} */
NRFY_STATIC_INLINE nrf_saadc_value_t nrfy_saadc_value_min_get(nrf_saadc_resolution_t resolution)
{
    return nrf_saadc_value_min_get(resolution);
}

/** @refhal{nrf_saadc_value_max_get} */
NRFY_STATIC_INLINE nrf_saadc_value_t nrfy_saadc_value_max_get(nrf_saadc_resolution_t resolution)
{
    return nrf_saadc_value_max_get(resolution);
}

#if NRFY_SAADC_HAS_CAL
/** @refhal{nrf_saadc_cal_set} */
NRFY_STATIC_INLINE void nrfy_saadc_cal_set(NRF_SAADC_Type * p_reg, uint32_t trim)
{
    nrf_saadc_cal_set(p_reg, trim);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_cal_get} */
NRFY_STATIC_INLINE uint32_t nrfy_saadc_cal_get(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t trim = nrf_saadc_cal_get(p_reg);
    nrf_barrier_r();
    return trim;
}
#endif

#if NRFY_SAADC_HAS_CALREF
/** @refhal{nrf_saadc_calref_set} */
NRFY_STATIC_INLINE void nrfy_saadc_calref_set(NRF_SAADC_Type * p_reg, uint32_t trim)
{
    nrf_saadc_calref_set(p_reg, trim);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_calref_get} */
NRFY_STATIC_INLINE uint32_t nrfy_saadc_calref_get(NRF_SAADC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t trim = nrf_saadc_calref_get(p_reg);
    nrf_barrier_r();
    return trim;
}
#endif

#if NRFY_SAADC_HAS_LIN_CAL
/** @refhal{nrf_saadc_linearity_calibration_coeff_set} */
NRFY_STATIC_INLINE void nrfy_saadc_linearity_calibration_coeff_set(NRF_SAADC_Type * p_reg,
                                                                   uint8_t          index,
                                                                   uint32_t         coeff)
{
    nrf_saadc_linearity_calibration_coeff_set(p_reg, index, coeff);
    nrf_barrier_w();
}

/** @refhal{nrf_saadc_linearity_calibration_coeff_get} */
NRFY_STATIC_INLINE
uint32_t nrfy_saadc_linearity_calibration_coeff_get(NRF_SAADC_Type const * p_reg,
                                                    uint8_t                index)
{
    nrf_barrier_rw();
    uint32_t trim = nrf_saadc_linearity_calibration_coeff_get(p_reg, index);
    nrf_barrier_r();
    return trim;
}

#endif

/** @} */

NRFY_STATIC_INLINE bool __nrfy_internal_saadc_event_handle(NRF_SAADC_Type *  p_reg,
                                                           uint32_t          mask,
                                                           nrf_saadc_event_t event,
                                                           uint32_t *        p_event_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_saadc_event_check(p_reg, event))
    {
        nrf_saadc_event_clear(p_reg, event);
        if (p_event_mask)
        {
            *p_event_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_saadc_events_process(NRF_SAADC_Type *            p_reg,
                                              uint32_t                    mask,
                                              nrfy_saadc_buffer_t const * p_desc)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();

    (void)__nrfy_internal_saadc_event_handle(p_reg, mask, NRF_SAADC_EVENT_STARTED, &evt_mask);
    bool stop = __nrfy_internal_saadc_event_handle(p_reg, mask, NRF_SAADC_EVENT_STOPPED, &evt_mask);

    if (__nrfy_internal_saadc_event_handle(p_reg, mask, NRF_SAADC_EVENT_END, &evt_mask) && p_desc)
    {
        size_t size = stop ? nrf_saadc_amount_get(p_reg) : p_desc->length;
        NRFY_CACHE_INV(p_desc->p_buffer, size);
    }

    (void)__nrfy_internal_saadc_event_handle(p_reg, mask, NRF_SAADC_EVENT_DONE, &evt_mask);
    (void)__nrfy_internal_saadc_event_handle(p_reg, mask, NRF_SAADC_EVENT_RESULTDONE, &evt_mask);
    (void)__nrfy_internal_saadc_event_handle(p_reg, mask, NRF_SAADC_EVENT_CALIBRATEDONE, &evt_mask);

    if (mask & NRF_SAADC_ALL_CHANNELS_LIMITS_INT_MASK)
    {
        for (uint8_t i = 0; i < SAADC_CH_NUM; i++)
        {
             nrf_saadc_event_t event = nrf_saadc_limit_event_get(i, NRF_SAADC_LIMIT_LOW);
             __nrfy_internal_saadc_event_handle(p_reg, mask, event, &evt_mask);

             event = nrf_saadc_limit_event_get(i, NRF_SAADC_LIMIT_HIGH);
             __nrfy_internal_saadc_event_handle(p_reg, mask, event, &evt_mask);
        }
    }

    return evt_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_saadc_event_enabled_clear(NRF_SAADC_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_saadc_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_saadc_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE void __nrfy_internal_saadc_buffer_latch(NRF_SAADC_Type * p_reg, bool wait)
{
    nrf_saadc_task_trigger(p_reg, NRF_SAADC_TASK_START);
    if (wait)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_SAADC_EVENT_STARTED);
        while (!__nrfy_internal_saadc_events_process(p_reg, evt_mask, NULL))
        {}
    }
    nrf_barrier_w();
}

NRFY_STATIC_INLINE void __nrfy_internal_saadc_stop(NRF_SAADC_Type * p_reg, bool wait)
{
    nrf_saadc_task_trigger(p_reg, NRF_SAADC_TASK_STOP);
    if (wait)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_SAADC_EVENT_STOPPED);
        while (!__nrfy_internal_saadc_events_process(p_reg, evt_mask, NULL))
        {}
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_SAADC_H__
