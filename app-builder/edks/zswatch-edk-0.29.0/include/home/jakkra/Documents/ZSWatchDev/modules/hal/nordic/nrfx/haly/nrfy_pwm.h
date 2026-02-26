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

#ifndef NRFY_PWM_H__
#define NRFY_PWM_H__

#include <nrfx.h>
#include <hal/nrf_pwm.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE bool __nrfy_internal_pwm_event_handle(NRF_PWM_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_pwm_event_t event,
                                                         uint32_t *      p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_pwm_events_process(NRF_PWM_Type * p_reg, uint32_t mask);

NRFY_STATIC_INLINE void __nrfy_internal_pwm_event_enabled_clear(NRF_PWM_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_pwm_event_t event);

/**
 * @defgroup nrfy_pwm PWM HALY
 * @{
 * @ingroup nrf_pwm
 * @brief   Hardware access layer with cache and barrier support for managing the PWM peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @brief PWM configuration structure. */
typedef struct
{
    uint32_t           output_pins[NRF_PWM_CHANNEL_COUNT]; ///< Pin numbers for individual output channels (optional).
                                                           /**< Use @ref NRF_PWM_PIN_NOT_CONNECTED
                                                            *   if a given output channel is not needed. */
    uint16_t           top_value;                          ///< Value up to which the pulse generator counter counts.
    nrf_pwm_clk_t      base_clock;                         ///< Base clock frequency.
    nrf_pwm_mode_t     count_mode;                         ///< Operating mode of the pulse generator counter.
    nrf_pwm_dec_load_t load_mode;                          ///< Mode of loading sequence data from RAM.
    nrf_pwm_dec_step_t step_mode;                          ///< Mode of advancing the active sequence.
    bool               skip_psel_cfg;                      ///< Skip pin selection configuration.
                                                           /**< When set to true, the driver does not modify
                                                            *   pin select registers in the peripheral.
                                                            *   Those registers are supposed to be set up
                                                            *   externally before the driver is initialized.
                                                            *   @note When both GPIO configuration and pin
                                                            *   selection are to be skipped, the structure
                                                            *   fields that specify pins can be omitted,
                                                            *   as they are ignored anyway. */
} nrfy_pwm_config_t;

/**
 * @brief Function for configuring the PWM.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_pwm_periph_configure(NRF_PWM_Type *            p_reg,
                                                  nrfy_pwm_config_t const * p_config)
{
    if (!p_config->skip_psel_cfg)
    {
        nrf_pwm_pins_set(p_reg, p_config->output_pins);
    }
    nrf_pwm_configure(p_reg, p_config->base_clock, p_config->count_mode, p_config->top_value);
    nrf_pwm_decoder_set(p_reg, p_config->load_mode, p_config->step_mode);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified PWM interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_pwm_int_init(NRF_PWM_Type * p_reg,
                                          uint32_t       mask,
                                          uint8_t        irq_priority,
                                          bool           enable)
{
    __nrfy_internal_pwm_event_enabled_clear(p_reg, mask, NRF_PWM_EVENT_LOOPSDONE);
    __nrfy_internal_pwm_event_enabled_clear(p_reg, mask, NRF_PWM_EVENT_SEQEND0);
    __nrfy_internal_pwm_event_enabled_clear(p_reg, mask, NRF_PWM_EVENT_SEQEND1);
    __nrfy_internal_pwm_event_enabled_clear(p_reg, mask, NRF_PWM_EVENT_STOPPED);
    __nrfy_internal_pwm_event_enabled_clear(p_reg, mask, NRF_PWM_EVENT_SEQSTARTED0);
    __nrfy_internal_pwm_event_enabled_clear(p_reg, mask, NRF_PWM_EVENT_SEQSTARTED1);
    __nrfy_internal_pwm_event_enabled_clear(p_reg, mask, NRF_PWM_EVENT_PWMPERIODEND);
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));

    if (enable)
    {
        nrf_pwm_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the PWM interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_pwm_int_uninit(NRF_PWM_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified PWM events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_pwm_events_process(NRF_PWM_Type * p_reg, uint32_t mask)
{
    nrf_barrier_r();
    uint32_t evt_mask = __nrfy_internal_pwm_events_process(p_reg, mask);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for starting the PWM sequence.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] seq_id Sequence index.
 * @param[in] wait   True if the sequence is to be blocking, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_pwm_start(NRF_PWM_Type * p_reg,
                                       uint8_t        seq_id,
                                       bool           wait)
{
    nrf_pwm_task_trigger(p_reg, nrf_pwm_seqstart_task_get(seq_id));

    if (wait)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(nrf_pwm_seqend_event_get(seq_id));
        while (!__nrfy_internal_pwm_events_process(p_reg, evt_mask))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for aborting the ongoing PWM sequence.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] wait  True if the abort is to be blocking, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_pwm_abort(NRF_PWM_Type * p_reg, bool wait)
{
    nrf_pwm_task_trigger(p_reg, NRF_PWM_TASK_STOP);

    if (wait)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_PWM_EVENT_STOPPED);
        while (!__nrfy_internal_pwm_events_process(p_reg, evt_mask))
        {}
        (void)__nrfy_internal_pwm_events_process(p_reg,
            NRFY_EVENT_TO_INT_BITMASK(NRF_PWM_EVENT_SEQEND0) |
            NRFY_EVENT_TO_INT_BITMASK(NRF_PWM_EVENT_SEQEND1));
    }
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_task_trigger} */
NRFY_STATIC_INLINE void nrfy_pwm_task_trigger(NRF_PWM_Type * p_reg,
                                              nrf_pwm_task_t task)
{
    nrf_pwm_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_pwm_task_address_get(NRF_PWM_Type const * p_reg,
                                                      nrf_pwm_task_t       task)
{
    return nrf_pwm_task_address_get(p_reg, task);
}

/** @refhal{nrf_pwm_event_clear} */
NRFY_STATIC_INLINE void nrfy_pwm_event_clear(NRF_PWM_Type *  p_reg,
                                             nrf_pwm_event_t event)
{
    nrf_pwm_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_event_check} */
NRFY_STATIC_INLINE bool nrfy_pwm_event_check(NRF_PWM_Type const * p_reg,
                                             nrf_pwm_event_t      event)
{
    nrf_barrier_r();
    bool ret = nrf_pwm_event_check(p_reg, event);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_pwm_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_pwm_event_address_get(NRF_PWM_Type const * p_reg,
                                                       nrf_pwm_event_t      event)
{
    return nrf_pwm_event_address_get(p_reg, event);
}

/** @refhal{nrf_pwm_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_pwm_shorts_enable(NRF_PWM_Type * p_reg,
                                               uint32_t       mask)
{
    nrf_pwm_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_pwm_shorts_disable(NRF_PWM_Type * p_reg,
                                                uint32_t       mask)
{
    nrf_pwm_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_shorts_set} */
NRFY_STATIC_INLINE void nrfy_pwm_shorts_set(NRF_PWM_Type * p_reg,
                                            uint32_t       mask)
{
    nrf_pwm_shorts_set(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_int_enable} */
NRFY_STATIC_INLINE void nrfy_pwm_int_enable(NRF_PWM_Type * p_reg,
                                            uint32_t       mask)
{
    nrf_pwm_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_int_disable} */
NRFY_STATIC_INLINE void nrfy_pwm_int_disable(NRF_PWM_Type * p_reg,
                                             uint32_t       mask)
{
    nrf_pwm_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_int_set} */
NRFY_STATIC_INLINE void nrfy_pwm_int_set(NRF_PWM_Type * p_reg,
                                         uint32_t       mask)
{
    nrf_pwm_int_set(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_pwm_int_enable_check(NRF_PWM_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_pwm_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return ret;
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_pwm_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_pwm_subscribe_set(NRF_PWM_Type * p_reg,
                                               nrf_pwm_task_t task,
                                               uint8_t        channel)
{
    nrf_pwm_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_pwm_subscribe_clear(NRF_PWM_Type * p_reg,
                                                 nrf_pwm_task_t task)
{
    nrf_pwm_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_publish_set} */
NRFY_STATIC_INLINE void nrfy_pwm_publish_set(NRF_PWM_Type *  p_reg,
                                             nrf_pwm_event_t event,
                                             uint8_t         channel)
{
    nrf_pwm_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_publish_clear} */
NRFY_STATIC_INLINE void nrfy_pwm_publish_clear(NRF_PWM_Type *  p_reg,
                                               nrf_pwm_event_t event)
{
    nrf_pwm_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_pwm_enable} */
NRFY_STATIC_INLINE void nrfy_pwm_enable(NRF_PWM_Type * p_reg)
{
    nrf_pwm_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_disable} */
NRFY_STATIC_INLINE void nrfy_pwm_disable(NRF_PWM_Type * p_reg)
{
    nrf_pwm_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_enable_check} */
NRFY_STATIC_INLINE bool nrfy_pwm_enable_check(NRF_PWM_Type * p_reg)
{
    nrf_barrier_rw();
    bool enabled = nrf_pwm_enable_check(p_reg);
    nrf_barrier_r();
    return enabled;
}

/** @refhal{nrf_pwm_pins_set} */
NRFY_STATIC_INLINE void nrfy_pwm_pins_set(NRF_PWM_Type * p_reg,
                                          uint32_t       out_pins[NRF_PWM_CHANNEL_COUNT])
{
    nrf_pwm_pins_set(p_reg, out_pins);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_pwm_pin_get(NRF_PWM_Type const * p_reg, uint8_t channel)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_pwm_pin_get(p_reg, channel);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_pwm_configure} */
NRFY_STATIC_INLINE void nrfy_pwm_configure(NRF_PWM_Type * p_reg,
                                           nrf_pwm_clk_t  base_clock,
                                           nrf_pwm_mode_t mode,
                                           uint16_t       top_value)
{
    nrf_pwm_configure(p_reg, base_clock, mode, top_value);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_sequence_set} */
NRFY_STATIC_INLINE void nrfy_pwm_sequence_set(NRF_PWM_Type *             p_reg,
                                              uint8_t                    seq_id,
                                              nrf_pwm_sequence_t const * p_seq)
{
    NRFY_CACHE_WB(p_seq->values.p_raw, p_seq->length);
    nrf_pwm_sequence_set(p_reg, seq_id, p_seq);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_decoder_set} */
NRFY_STATIC_INLINE void nrfy_pwm_decoder_set(NRF_PWM_Type *     p_reg,
                                             nrf_pwm_dec_load_t dec_load,
                                             nrf_pwm_dec_step_t dec_step)
{
    nrf_pwm_decoder_set(p_reg, dec_load, dec_step);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_loop_set} */
NRFY_STATIC_INLINE void nrfy_pwm_loop_set(NRF_PWM_Type * p_reg, uint16_t loop_count)
{
    nrf_pwm_loop_set(p_reg, loop_count);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_seqstart_task_get} */
NRFY_STATIC_INLINE nrf_pwm_task_t nrfy_pwm_seqstart_task_get(uint8_t seq_id)
{
    return nrf_pwm_seqstart_task_get(seq_id);
}

/** @refhal{nrf_pwm_seqend_event_get} */
NRFY_STATIC_INLINE nrf_pwm_event_t nrfy_pwm_seqend_event_get(uint8_t seq_id)
{
    return nrf_pwm_seqend_event_get(seq_id);
}

#if NRF_PWM_HAS_IDLEOUT
/** @refhal{nrf_pwm_channel_idle_set} */
NRFY_STATIC_INLINE void nrfy_pwm_channel_idle_set(NRF_PWM_Type * p_reg,
                                                  uint8_t        channel,
                                                  bool           value)
{
    nrf_pwm_channel_idle_set(p_reg, channel, value);
    nrf_barrier_w();
}

/** @refhal{nrf_pwm_channel_idle_get} */
NRFY_STATIC_INLINE bool nrfy_pwm_channel_idle_get(NRF_PWM_Type const * p_reg,
                                                  uint8_t              channel)
{
    return nrf_pwm_channel_idle_get(p_reg, channel);
}
#endif // NRF_PWM_HAS_IDLEOUT

/** @} */

NRFY_STATIC_INLINE bool __nrfy_internal_pwm_event_handle(NRF_PWM_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_pwm_event_t event,
                                                         uint32_t *      p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_pwm_event_check(p_reg, event))
    {
        nrf_barrier_r();
        nrf_pwm_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_pwm_events_process(NRF_PWM_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = 0;

    (void)__nrfy_internal_pwm_event_handle(p_reg, mask, NRF_PWM_EVENT_SEQEND0, &evt_mask);
    (void)__nrfy_internal_pwm_event_handle(p_reg, mask, NRF_PWM_EVENT_SEQEND1, &evt_mask);
    (void)__nrfy_internal_pwm_event_handle(p_reg, mask, NRF_PWM_EVENT_LOOPSDONE, &evt_mask);
    (void)__nrfy_internal_pwm_event_handle(p_reg, mask, NRF_PWM_EVENT_STOPPED, &evt_mask);
    (void)__nrfy_internal_pwm_event_handle(p_reg, mask, NRF_PWM_EVENT_SEQSTARTED0, &evt_mask);
    (void)__nrfy_internal_pwm_event_handle(p_reg, mask, NRF_PWM_EVENT_SEQSTARTED1, &evt_mask);
    (void)__nrfy_internal_pwm_event_handle(p_reg, mask, NRF_PWM_EVENT_PWMPERIODEND, &evt_mask);

    return evt_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_pwm_event_enabled_clear(NRF_PWM_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_pwm_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_pwm_event_clear(p_reg, event);
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_PWM_H__
