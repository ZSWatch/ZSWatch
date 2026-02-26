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

#ifndef NRFY_TBM_H__
#define NRFY_TBM_H__

#include <nrfx.h>
#include <hal/nrf_tbm.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE uint32_t  __nrfy_internal_tbm_event_handle(NRF_TBM_Type *  p_reg,
                                                              uint32_t        mask,
                                                              nrf_tbm_event_t event);

NRFY_STATIC_INLINE void __nrfy_internal_tbm_event_enabled_clear(NRF_TBM_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_tbm_event_t event);
/**
* @defgroup nrfy_tbm TBM HALY
* @{
* @ingroup nrf_tbm
* @brief   Hardware access layer with cache and barrier support
*          for managing the Trace Buffer Monitor (TBM).
* @note    Extended Hardware Access Layer (HALY) is deprecated.
*/

/**
 * @brief Function for configuring the TBM.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] size  Buffer size (in 32 bit words).
 */
NRFY_STATIC_INLINE void nrfy_tbm_configure(NRF_TBM_Type * p_reg, uint32_t size)
{
    nrf_tbm_buffersize_set(p_reg, size);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified TBM interrutps.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_tbm_int_init(NRF_TBM_Type * p_reg,
                                          uint32_t       mask,
                                          uint8_t        irq_priority,
                                          bool           enable)
{
    __nrfy_internal_tbm_event_enabled_clear(p_reg, mask, NRF_TBM_EVENT_HALFFULL);
    __nrfy_internal_tbm_event_enabled_clear(p_reg, mask, NRF_TBM_EVENT_FULL);
    __nrfy_internal_tbm_event_enabled_clear(p_reg, mask, NRF_TBM_EVENT_FLUSH);
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_tbm_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the TBM interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_tbm_int_uninit(NRF_TBM_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified TBM events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed,
 *                  created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_tbm_events_process(NRF_TBM_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask;

    nrf_barrier_r();

    evt_mask = __nrfy_internal_tbm_event_handle(p_reg, mask, NRF_TBM_EVENT_HALFFULL);
    evt_mask |= __nrfy_internal_tbm_event_handle(p_reg, mask, NRF_TBM_EVENT_FULL);
    evt_mask |= __nrfy_internal_tbm_event_handle(p_reg, mask, NRF_TBM_EVENT_FLUSH);

    nrf_barrier_w();
    return evt_mask;
}

/** @refhal{nrf_tbm_count_get} */
NRFY_STATIC_INLINE uint32_t nrfy_tbm_count_get(NRF_TBM_Type * p_reg)
{
    nrf_barrier_r();
    return nrf_tbm_count_get(p_reg);
}

/** @refhal{nrf_tbm_task_trigger} */
NRFY_STATIC_INLINE void nrfy_tbm_task_trigger(NRF_TBM_Type * p_reg, nrf_tbm_task_t task)
{
    nrf_tbm_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_tbm_event_clear} */
NRFY_STATIC_INLINE void nrfy_tbm_event_clear(NRF_TBM_Type *  p_reg,
                                             nrf_tbm_event_t event)
{
    nrf_tbm_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_tbm_event_check} */
NRFY_STATIC_INLINE bool nrfy_tbm_event_check(NRF_TBM_Type const * p_reg,
                                             nrf_tbm_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_tbm_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_tbm_int_enable} */
NRFY_STATIC_INLINE void nrfy_tbm_int_enable(NRF_TBM_Type * p_reg, uint32_t mask)
{
    nrf_tbm_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_tbm_int_disable} */
NRFY_STATIC_INLINE void nrfy_tbm_int_disable(NRF_TBM_Type * p_reg, uint32_t mask)
{
    nrf_tbm_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_tbm_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_tbm_int_enable_check(NRF_TBM_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_tbm_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @} */

NRFY_STATIC_INLINE uint32_t  __nrfy_internal_tbm_event_handle(NRF_TBM_Type *  p_reg,
                                                              uint32_t        mask,
                                                              nrf_tbm_event_t event)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_tbm_event_check(p_reg, event))
    {
        nrf_tbm_event_clear(p_reg, event);
        return NRFY_EVENT_TO_INT_BITMASK(event);
    }
    return 0;
}

NRFY_STATIC_INLINE void __nrfy_internal_tbm_event_enabled_clear(NRF_TBM_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_tbm_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_tbm_event_clear(p_reg, event);
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_TBM_H__
