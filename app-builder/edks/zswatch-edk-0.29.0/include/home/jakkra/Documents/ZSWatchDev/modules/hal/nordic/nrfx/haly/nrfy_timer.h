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

#ifndef NRFY_TIMER_H__
#define NRFY_TIMER_H__

#include <nrfx.h>
#include <hal/nrf_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_timer_event_enabled_clear(NRF_TIMER_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_timer_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_timer_event_handle(NRF_TIMER_Type *  p_reg,
                                                           uint32_t          mask,
                                                           nrf_timer_event_t event,
                                                           uint32_t *        p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_timer_events_process(NRF_TIMER_Type * p_reg,
                                                                 uint32_t         mask);

/**
 * @defgroup nrfy_timer TIMER HALY
 * @{
 * @ingroup nrf_timer
 * @brief   Hardware access layer with cache and barrier support for managing the TIMER peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

 #if NRF_TIMER_HAS_ONE_SHOT || defined(__NRFX_DOXYGEN__)
 /** @refhal{NRF_TIMER_HAS_ONE_SHOT} */
 #define NRFY_TIMER_HAS_ONE_SHOT 1
 #else
 #define NRFY_TIMER_HAS_ONE_SHOT 0
 #endif

/** @brief TIMER configuration structure. */
typedef struct
{
    uint32_t              prescaler; ///< Prescaler value.
    nrf_timer_mode_t      mode;      ///< Mode of operation.
    nrf_timer_bit_width_t bit_width; ///< Bit width.
} nrfy_timer_config_t;

/**
 * @brief Function for configuring the TIMER.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_timer_periph_configure(NRF_TIMER_Type *            p_reg,
                                                    nrfy_timer_config_t const * p_config)
{
    nrf_timer_mode_set(p_reg, p_config->mode);
    nrf_timer_bit_width_set(p_reg, p_config->bit_width);
    nrf_timer_prescaler_set(p_reg, p_config->prescaler);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified TIMER interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_timer_int_init(NRF_TIMER_Type * p_reg,
                                            uint32_t         mask,
                                            uint8_t          irq_priority,
                                            bool             enable)
{
    for (uint8_t i = 0; i < NRF_TIMER_CC_COUNT_MAX; i++)
    {
        __nrfy_internal_timer_event_enabled_clear(p_reg, mask, nrf_timer_compare_event_get(i));
    }

    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_timer_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the TIMER interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_timer_int_uninit(NRF_TIMER_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified TIMER events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_timer_events_process(NRF_TIMER_Type * p_reg,
                                                      uint32_t         mask)
{
    uint32_t evt_mask = __nrfy_internal_timer_events_process(p_reg, mask);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for capturing the TIMER value.
 *
 * @note This function triggers the capture task for given @p channel and returns
 *       latched timer value.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel Capture channel number.
 *
 * @return Captured value.
 */
NRFY_STATIC_INLINE uint32_t nrfy_timer_capture_get(NRF_TIMER_Type *       p_reg,
                                                   nrf_timer_cc_channel_t channel)
{
    nrf_timer_task_trigger(p_reg, nrf_timer_capture_task_get(channel));
    nrf_barrier_rw();
    uint32_t cc = nrf_timer_cc_get(p_reg, channel);
    nrf_barrier_r();
    return cc;
}

/** @refhal{nrf_timer_task_trigger} */
NRFY_STATIC_INLINE void nrfy_timer_task_trigger(NRF_TIMER_Type * p_reg,
                                                nrf_timer_task_t task)
{
    nrf_timer_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_timer_task_address_get(NRF_TIMER_Type const * p_reg,
                                                        nrf_timer_task_t       task)
{
    return nrf_timer_task_address_get(p_reg, task);
}

/** @refhal{nrf_timer_event_clear} */
NRFY_STATIC_INLINE void nrfy_timer_event_clear(NRF_TIMER_Type *  p_reg,
                                               nrf_timer_event_t event)
{
    nrf_timer_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_event_check} */
NRFY_STATIC_INLINE bool nrfy_timer_event_check(NRF_TIMER_Type const * p_reg,
                                               nrf_timer_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_timer_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_timer_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_timer_event_address_get(NRF_TIMER_Type const * p_reg,
                                                         nrf_timer_event_t      event)
{
    return nrf_timer_event_address_get(p_reg, event);
}

/** @refhal{nrf_timer_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_timer_shorts_enable(NRF_TIMER_Type * p_reg,
                                                 uint32_t         mask)
{
    nrf_timer_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_timer_shorts_disable(NRF_TIMER_Type * p_reg,
                                                  uint32_t         mask)
{
    nrf_timer_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_shorts_set} */
NRFY_STATIC_INLINE void nrfy_timer_shorts_set(NRF_TIMER_Type * p_reg,
                                              uint32_t         mask)
{
    nrf_timer_shorts_set(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_int_enable} */
NRFY_STATIC_INLINE void nrfy_timer_int_enable(NRF_TIMER_Type * p_reg,
                                              uint32_t         mask)
{
    nrf_timer_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_int_disable} */
NRFY_STATIC_INLINE void nrfy_timer_int_disable(NRF_TIMER_Type * p_reg,
                                               uint32_t         mask)
{
    nrf_timer_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_timer_int_enable_check(NRF_TIMER_Type const * p_reg,
                                                        uint32_t               mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_timer_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_timer_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_timer_subscribe_set(NRF_TIMER_Type * p_reg,
                                                 nrf_timer_task_t task,
                                                 uint8_t          channel)
{
    nrf_timer_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_timer_subscribe_clear(NRF_TIMER_Type * p_reg,
                                                   nrf_timer_task_t task)
{
    nrf_timer_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_publish_set} */
NRFY_STATIC_INLINE void nrfy_timer_publish_set(NRF_TIMER_Type *  p_reg,
                                               nrf_timer_event_t event,
                                               uint8_t           channel)
{
    nrf_timer_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_publish_clear} */
NRFY_STATIC_INLINE void nrfy_timer_publish_clear(NRF_TIMER_Type *  p_reg,
                                                 nrf_timer_event_t event)
{
    nrf_timer_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/** @refhal{nrf_timer_mode_set} */
NRFY_STATIC_INLINE void nrfy_timer_mode_set(NRF_TIMER_Type * p_reg,
                                            nrf_timer_mode_t mode)
{
    nrf_timer_mode_set(p_reg, mode);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_mode_get} */
NRFY_STATIC_INLINE nrf_timer_mode_t nrfy_timer_mode_get(NRF_TIMER_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_timer_mode_t mode = nrf_timer_mode_get(p_reg);
    nrf_barrier_r();
    return mode;
}

/** @refhal{nrf_timer_bit_width_set} */
NRFY_STATIC_INLINE void nrfy_timer_bit_width_set(NRF_TIMER_Type *      p_reg,
                                                 nrf_timer_bit_width_t bit_width)
{
    nrf_timer_bit_width_set(p_reg, bit_width);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_bit_width_get} */
NRFY_STATIC_INLINE nrf_timer_bit_width_t nrfy_timer_bit_width_get(NRF_TIMER_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_timer_bit_width_t bit_width = nrf_timer_bit_width_get(p_reg);
    nrf_barrier_r();
    return bit_width;
}

/** @refhal{nrf_timer_prescaler_set} */
NRFY_STATIC_INLINE void nrfy_timer_prescaler_set(NRF_TIMER_Type * p_reg, uint32_t prescaler_factor)
{
    nrf_timer_prescaler_set(p_reg, prescaler_factor);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_prescaler_get} */
NRFY_STATIC_INLINE uint32_t nrfy_timer_prescaler_get(NRF_TIMER_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t prescaler = nrf_timer_prescaler_get(p_reg);
    nrf_barrier_r();
    return prescaler;
}

/** @refhal{nrf_timer_cc_set} */
NRFY_STATIC_INLINE void nrfy_timer_cc_set(NRF_TIMER_Type *       p_reg,
                                          nrf_timer_cc_channel_t cc_channel,
                                          uint32_t               cc_value)
{
    nrf_timer_cc_set(p_reg, cc_channel, cc_value);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_cc_get} */
NRFY_STATIC_INLINE uint32_t nrfy_timer_cc_get(NRF_TIMER_Type const * p_reg,
                                              nrf_timer_cc_channel_t cc_channel)
{
    nrf_barrier_rw();
    uint32_t cc = nrf_timer_cc_get(p_reg, cc_channel);
    nrf_barrier_r();
    return cc;
}

/** @refhal{nrf_timer_capture_task_get} */
NRFY_STATIC_INLINE nrf_timer_task_t nrfy_timer_capture_task_get(uint8_t channel)
{
    return nrf_timer_capture_task_get(channel);
}

/** @refhal{nrf_timer_compare_event_get} */
NRFY_STATIC_INLINE nrf_timer_event_t nrfy_timer_compare_event_get(uint8_t channel)
{
    return nrf_timer_compare_event_get(channel);
}

/** @refhal{nrf_timer_compare_int_get} */
NRFY_STATIC_INLINE nrf_timer_int_mask_t nrfy_timer_compare_int_get(uint8_t channel)
{
    return nrf_timer_compare_int_get(channel);
}

#if NRFY_TIMER_HAS_ONE_SHOT
/** @refhal{nrf_timer_one_shot_enable} */
NRFY_STATIC_INLINE void nrfy_timer_one_shot_enable(NRF_TIMER_Type *       p_reg,
                                                   nrf_timer_cc_channel_t cc_channel)
{
    nrf_timer_one_shot_enable(p_reg, cc_channel);
    nrf_barrier_w();
}

/** @refhal{nrf_timer_one_shot_disable} */
NRFY_STATIC_INLINE void nrfy_timer_one_shot_disable(NRF_TIMER_Type *       p_reg,
                                                    nrf_timer_cc_channel_t cc_channel)
{
    nrf_timer_one_shot_disable(p_reg, cc_channel);
    nrf_barrier_w();
}
#endif // NRFY_TIMER_HAS_ONE_SHOT

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_timer_event_enabled_clear(NRF_TIMER_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_timer_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_timer_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE bool __nrfy_internal_timer_event_handle(NRF_TIMER_Type *  p_reg,
                                                           uint32_t          mask,
                                                           nrf_timer_event_t event,
                                                           uint32_t *        p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_timer_event_check(p_reg, event))
    {
        nrf_timer_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_timer_events_process(NRF_TIMER_Type * p_reg,
                                                                 uint32_t         mask)
{
    uint32_t event_mask = 0;

    nrf_barrier_r();
    for (uint8_t i = 0; i < NRF_TIMER_CC_COUNT_MAX; i++)
    {
        __nrfy_internal_timer_event_handle(p_reg,
                                           mask,
                                           nrf_timer_compare_event_get(i),
                                           &event_mask);
    }

    return event_mask;
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_TIMER_H__
