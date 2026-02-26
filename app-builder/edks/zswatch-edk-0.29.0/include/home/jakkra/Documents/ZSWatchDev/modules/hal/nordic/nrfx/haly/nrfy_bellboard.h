/*
 * Copyright (c) 2023 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFY_BELLBOARD_H__
#define NRFY_BELLBOARD_H__

#include <nrfx.h>
#include <hal/nrf_bellboard.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_bellboard_event_enabled_clear(NRF_BELLBOARD_Type *  p_reg,
                                                                      uint32_t              mask,
                                                                      nrf_bellboard_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_bellboard_event_handle(NRF_BELLBOARD_Type *  p_reg,
                                                               uint32_t              mask,
                                                               nrf_bellboard_event_t event,
                                                               uint32_t *            p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_bellboard_events_process(NRF_BELLBOARD_Type * p_reg,
                                                                     uint32_t             mask);

/**
 * @defgroup nrfy_bellboard BELLBOARD HALY
 * @{
 * @ingroup nrf_bellboard
 * @brief   Hardware access layer with cache and barrier support for managing the BELLBOARD peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/* BELLBOARD0_IRQn is not defined for RISC-V targets, so the interrupt (un)init functions need to be excluded from compilation.
 * RISC-V targets should use (and initialize) VEVIF for receiving inter-core signals.
 */
#if defined(ISA_ARM) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for initializing the specified BELLBOARD interupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 * @param[in] group_idx    Index of interrupts group to be enabled.
 */
NRFY_STATIC_INLINE void nrfy_bellboard_int_init(NRF_BELLBOARD_Type * p_reg,
                                                uint32_t             mask,
                                                uint8_t              irq_priority,
                                                bool                 enable,
                                                uint8_t              group_idx)
{
    IRQn_Type interrupt_index = (IRQn_Type)((uint32_t)BELLBOARD_0_IRQn + group_idx);

    for (uint8_t i = 0; i < NRF_BELLBOARD_EVENTS_TRIGGERED_COUNT; i++)
    {
         nrf_bellboard_event_t event = nrf_bellboard_triggered_event_get(i);
         __nrfy_internal_bellboard_event_enabled_clear(p_reg, mask, event);
    }

    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(interrupt_index, irq_priority);
    NRFX_IRQ_ENABLE(interrupt_index);

    if (enable)
    {
        nrf_bellboard_int_enable(p_reg, group_idx, mask);
    }

    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the specified BELLBOARD interrupts.
 *
 * @param[in] group_idx Index for interrupt group to be uninitialized.
 */
NRFY_STATIC_INLINE void nrfy_bellboard_int_uninit(uint8_t group_idx)
{
    IRQn_Type interrupt_index = (IRQn_Type)((uint32_t)BELLBOARD_0_IRQn + group_idx);

    NRFX_IRQ_DISABLE(interrupt_index);
    nrf_barrier_w();
}
#endif // // defined(ISA_ARM) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for processing the specified BELLBOARD events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */

NRFY_STATIC_INLINE uint32_t nrfy_bellboard_events_process(NRF_BELLBOARD_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = __nrfy_internal_bellboard_events_process(p_reg, mask);
    nrf_barrier_w();

    return evt_mask;
}

/** @refhal{nrf_bellboard_task_trigger} */
NRFY_STATIC_INLINE void nrfy_bellboard_task_trigger(NRF_BELLBOARD_Type * p_reg,
                                                    nrf_bellboard_task_t task)
{
    nrf_bellboard_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_bellboard_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_bellboard_task_address_get(NRF_BELLBOARD_Type const * p_reg,
                                                            nrf_bellboard_task_t       task)
{
    return nrf_bellboard_task_address_get(p_reg, task);
}

/** @refhal{nrf_bellboard_event_clear} */
NRFY_STATIC_INLINE void nrfy_bellboard_event_clear(NRF_BELLBOARD_Type *  p_reg,
                                                   nrf_bellboard_event_t event)
{
    nrf_bellboard_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_bellboard_event_check} */
NRFY_STATIC_INLINE bool nrfy_bellboard_event_check(NRF_BELLBOARD_Type const * p_reg,
                                                   nrf_bellboard_event_t      event)
{
    bool ret = nrf_bellboard_event_check(p_reg, event);
    nrf_barrier_r();

    return ret;
}

/** @refhal{nrf_bellboard_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_bellboard_event_address_get(NRF_BELLBOARD_Type const * p_reg,
                                                             nrf_bellboard_event_t      event)
{
    return nrf_bellboard_event_address_get(p_reg, event);
}

/** @refhal{nrf_bellboard_int_enable} */
NRFY_STATIC_INLINE void nrfy_bellboard_int_enable(NRF_BELLBOARD_Type * p_reg,
                                                  uint8_t              group_idx,
                                                  uint32_t             mask)
{
    nrf_bellboard_int_enable(p_reg, group_idx, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_bellboard_int_disable} */
NRFY_STATIC_INLINE void nrfy_bellboard_int_disable(NRF_BELLBOARD_Type * p_reg,
                                                   uint8_t              group_idx,
                                                   uint32_t             mask)
{
    nrf_bellboard_int_disable(p_reg, group_idx, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_bellboard_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_bellboard_int_enable_check(NRF_BELLBOARD_Type const * p_reg,
                                                            uint8_t                    group_idx,
                                                            uint32_t                   mask)
{
    uint32_t ret = nrf_bellboard_int_enable_check(p_reg, group_idx, mask);
    nrf_barrier_r();

    return ret;
}

/** @refhal{nrf_bellboard_int_pending_get} */
NRFY_STATIC_INLINE uint32_t nrfy_bellboard_int_pending_get(NRF_BELLBOARD_Type const * p_reg,
                                                           uint8_t                    group_idx)
{
    nrf_barrier_r();
    uint32_t int_ret = nrf_bellboard_int_pending_get(p_reg, group_idx);
    nrf_barrier_r();

    return int_ret;
}

/** @} */

NRFY_STATIC_INLINE uint32_t  __nrfy_internal_bellboard_events_process(NRF_BELLBOARD_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    for (uint8_t i = 0; i < NRF_BELLBOARD_EVENTS_TRIGGERED_COUNT; i++)
    {
        nrf_bellboard_event_t event = nrf_bellboard_triggered_event_get(i);
        (void)__nrfy_internal_bellboard_event_handle(p_reg, mask, event, &evt_mask);
    }

    return evt_mask;
}

NRFY_STATIC_INLINE bool __nrfy_internal_bellboard_event_handle(NRF_BELLBOARD_Type *  p_reg,
                                                               uint32_t              mask,
                                                               nrf_bellboard_event_t event,
                                                               uint32_t *            p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_bellboard_event_check(p_reg, event))
    {
        nrf_bellboard_event_clear(p_reg, event);

        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }

        return true;
    }

    return false;
}

NRFY_STATIC_INLINE void __nrfy_internal_bellboard_event_enabled_clear(NRF_BELLBOARD_Type *  p_reg,
                                                                      uint32_t              mask,
                                                                      nrf_bellboard_event_t event)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)))
    {
        nrf_bellboard_event_clear(p_reg, event);
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_BELLBOARD_H__
