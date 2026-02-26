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

#ifndef NRFY_RTC_H__
#define NRFY_RTC_H__

#include <nrfx.h>
#include <hal/nrf_rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE bool __nrfy_internal_rtc_event_handle(NRF_RTC_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_rtc_event_t event,
                                                         uint32_t *      p_event_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_rtc_events_process(NRF_RTC_Type * p_reg,
                                                               uint32_t       mask);

NRFY_STATIC_INLINE void __nrfy_internal_rtc_event_enabled_clear(NRF_RTC_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_rtc_event_t event);

/**
 * @defgroup nrfy_rtc RTC HALY
 * @{
 * @ingroup nrf_rtc
 * @brief   Hardware access layer with cache and barrier support for managing the RTC peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @brief Structure for RTC configuration. */
typedef struct
{
    uint32_t prescaler; ///< Prescaler.
} nrfy_rtc_config_t;

/**
 * @brief Function for configuring the RTC.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_rtc_periph_configure(NRF_RTC_Type *            p_reg,
                                                  nrfy_rtc_config_t const * p_config)
{
    nrf_rtc_prescaler_set(p_reg, p_config->prescaler);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified RTC interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_rtc_int_init(NRF_RTC_Type * p_reg,
                                          uint32_t       mask,
                                          uint8_t        irq_priority,
                                          bool           enable)
{
    __nrfy_internal_rtc_event_enabled_clear(p_reg, mask, NRF_RTC_EVENT_TICK);
    __nrfy_internal_rtc_event_enabled_clear(p_reg, mask, NRF_RTC_EVENT_OVERFLOW);

    for (uint8_t i = 0; i < NRF_RTC_CC_COUNT_MAX; i++)
    {
        __nrfy_internal_rtc_event_enabled_clear(p_reg, mask, nrf_rtc_compare_event_get(i));
    }

    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_rtc_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the RTC interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_rtc_int_uninit(NRF_RTC_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified RTC events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_rtc_events_process(NRF_RTC_Type * p_reg,
                                                    uint32_t       mask)
{
    uint32_t evt_mask = __nrfy_internal_rtc_events_process(p_reg, mask);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for stopping the RTC.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be disabled.
 */
NRFY_STATIC_INLINE void nrfy_rtc_stop(NRF_RTC_Type * p_reg,
                                      uint32_t       mask)
{
    nrf_rtc_task_trigger(p_reg, NRF_RTC_TASK_STOP);
    nrf_barrier_w();
    nrf_rtc_event_disable(p_reg, mask);
    nrf_rtc_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/**
 * @brief Function for enabling the RTC event and optionally associated interrupt.
 *
 * @note Event is implicitly cleared before enabling the associated interrupt.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] event  Event to be enabled.
 * @param[in] enable True if associated interrupt is to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_rtc_event_int_clear_enable(NRF_RTC_Type *  p_reg,
                                                        nrf_rtc_event_t event,
                                                        bool            enable)
{
    if (enable)
    {
        nrf_rtc_event_clear(p_reg, event);
        nrf_barrier_w();
        nrf_rtc_int_enable(p_reg, NRFY_EVENT_TO_INT_BITMASK(event));
    }
    nrf_rtc_event_enable(p_reg,  NRFY_EVENT_TO_INT_BITMASK(event));
    nrf_barrier_w();
}

/**
 * @brief Function for disabling the RTC events and corresponding interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be disabled.
 */
NRFY_STATIC_INLINE void nrfy_rtc_event_int_disable(NRF_RTC_Type * p_reg,
                                                   uint32_t       mask)
{
    nrf_rtc_event_disable(p_reg, mask);
    nrf_rtc_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_cc_set} */
NRFY_STATIC_INLINE void nrfy_rtc_cc_set(NRF_RTC_Type * p_reg,
                                        uint32_t       ch,
                                        uint32_t       cc_val)
{
    nrf_rtc_cc_set(p_reg, ch, cc_val);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_cc_get} */
NRFY_STATIC_INLINE uint32_t nrfy_rtc_cc_get(NRF_RTC_Type const * p_reg,
                                            uint32_t             ch)
{
    nrf_barrier_rw();
    uint32_t cc = nrf_rtc_cc_get(p_reg, ch);
    nrf_barrier_r();
    return cc;
}

/** @refhal{nrf_rtc_int_enable} */
NRFY_STATIC_INLINE void nrfy_rtc_int_enable(NRF_RTC_Type * p_reg,
                                            uint32_t       mask)
{
    nrf_rtc_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_int_disable} */
NRFY_STATIC_INLINE void nrfy_rtc_int_disable(NRF_RTC_Type * p_reg,
                                             uint32_t       mask)
{
    nrf_rtc_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_rtc_int_enable_check(NRF_RTC_Type * p_reg,
                                                      uint32_t       mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_rtc_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_rtc_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_rtc_subscribe_set(NRF_RTC_Type * p_reg,
                                               nrf_rtc_task_t task,
                                               uint8_t        channel)
{
    nrf_rtc_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_rtc_subscribe_clear(NRF_RTC_Type * p_reg,
                                                 nrf_rtc_task_t task)
{
    nrf_rtc_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_publish_set} */
NRFY_STATIC_INLINE void nrfy_rtc_publish_set(NRF_RTC_Type *  p_reg,
                                             nrf_rtc_event_t event,
                                             uint8_t         channel)
{
    nrf_rtc_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_publish_clear} */
NRFY_STATIC_INLINE void nrfy_rtc_publish_clear(NRF_RTC_Type *  p_reg,
                                               nrf_rtc_event_t event)
{
    nrf_rtc_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_rtc_event_check} */
NRFY_STATIC_INLINE bool nrfy_rtc_event_check(NRF_RTC_Type *  p_reg,
                                             nrf_rtc_event_t event)
{
    nrf_barrier_r();
    bool check = nrf_rtc_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_rtc_event_clear} */
NRFY_STATIC_INLINE void nrfy_rtc_event_clear(NRF_RTC_Type *  p_reg,
                                             nrf_rtc_event_t event)
{
    nrf_rtc_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_counter_get} */
NRFY_STATIC_INLINE uint32_t nrfy_rtc_counter_get(NRF_RTC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t counter = nrf_rtc_counter_get(p_reg);
    nrf_barrier_r();
    return counter;
}

/** @refhal{nrf_rtc_prescaler_set} */
NRFY_STATIC_INLINE void nrfy_rtc_prescaler_set(NRF_RTC_Type * p_reg,
                                               uint32_t       val)
{
    nrf_rtc_prescaler_set(p_reg, val);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_prescaler_get} */
NRFY_STATIC_INLINE uint32_t nrfy_rtc_prescaler_get(NRF_RTC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t prescaler = nrf_rtc_prescaler_get(p_reg);
    nrf_barrier_r();
    return prescaler;
}

/** @refhal{nrf_rtc_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_rtc_event_address_get(NRF_RTC_Type const * p_reg,
                                                       nrf_rtc_event_t      event)
{
    return nrf_rtc_event_address_get(p_reg, event);
}

/** @refhal{nrf_rtc_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_rtc_task_address_get(NRF_RTC_Type const * p_reg,
                                                      nrf_rtc_task_t       task)
{
    return nrf_rtc_task_address_get(p_reg, task);
}

/** @refhal{nrf_rtc_task_trigger} */
NRFY_STATIC_INLINE void nrfy_rtc_task_trigger(NRF_RTC_Type * p_reg,
                                              nrf_rtc_task_t task)
{
    nrf_rtc_task_trigger(p_reg, task);
    nrf_barrier_w();
}

#if defined(RTC_TASKS_CAPTURE_TASKS_CAPTURE_Msk) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_rtc_capture_task_get} */
NRFY_STATIC_INLINE nrf_rtc_task_t nrfy_rtc_capture_task_get(uint8_t index)
{
    return nrf_rtc_capture_task_get(index);
}
#endif

/** @refhal{nrf_rtc_event_enable} */
NRFY_STATIC_INLINE void nrfy_rtc_event_enable(NRF_RTC_Type * p_reg,
                                              uint32_t       mask)
{
    nrf_rtc_event_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_event_disable} */
NRFY_STATIC_INLINE void nrfy_rtc_event_disable(NRF_RTC_Type * p_reg,
                                               uint32_t       mask)
{
    nrf_rtc_event_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_rtc_compare_event_get} */
NRFY_STATIC_INLINE nrf_rtc_event_t nrfy_rtc_compare_event_get(uint8_t index)
{
    return nrf_rtc_compare_event_get(index);
}

/** @} */

NRFY_STATIC_INLINE bool __nrfy_internal_rtc_event_handle(NRF_RTC_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_rtc_event_t event,
                                                         uint32_t *      p_event_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_rtc_event_check(p_reg, event))
    {
        nrf_rtc_event_clear(p_reg, event);
        if (p_event_mask)
        {
            *p_event_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_rtc_events_process(NRF_RTC_Type * p_reg,
                                                               uint32_t       mask)
{
    uint32_t event_mask = 0;

    nrf_barrier_r();
    for (uint8_t i = 0; i < NRF_RTC_CC_COUNT_MAX; i++)
    {
        (void)__nrfy_internal_rtc_event_handle(p_reg,
                                               mask,
                                               nrf_rtc_compare_event_get(i),
                                               &event_mask);
    }

    (void)__nrfy_internal_rtc_event_handle(p_reg, mask, NRF_RTC_EVENT_TICK, &event_mask);

    (void)__nrfy_internal_rtc_event_handle(p_reg, mask, NRF_RTC_EVENT_OVERFLOW, &event_mask);

    return event_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_rtc_event_enabled_clear(NRF_RTC_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_rtc_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_rtc_event_clear(p_reg, event);
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_RTC_H__
