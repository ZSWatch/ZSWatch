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

#ifndef NRF_VPR_CSR_VEVIF_H__
#define NRF_VPR_CSR_VEVIF_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_vpr_csr_vevif_hal VPR CSR VEVIF HAL
 * @{
 * @ingroup nrf_vpr
 * @brief   Hardware access layer for managing the VPR RISC-V CPU Control
 *          and Status Registers for VPR Event Interface (VPR CSR VEVIF).
 */

/** @brief Number of VEVIF events/tasks. */
#define NRF_VPR_CSR_VEVIF_EVENT_TASK_COUNT VPR_VEVIF_EVENT_MaxCount

/**
 * @brief Function for getting mask of pending VEVIF tasks.
 *
 * @return Mask of pending VEVIF tasks.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_tasks_get(void);

/**
 * @brief Function for clearing mask of pending VEVIF tasks.
 *
 * @param[in] mask Mask of VEVIF tasks to be cleared.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_tasks_clear(uint32_t mask);

/**
 * @brief Function for setting pending VEVIF tasks.
 *
 * @param[in] value VEVIF tasks value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_tasks_set(uint32_t value);

/**
 * @brief Function for getting mask of triggered VEVIF events.
 *
 * @return Mask of triggered VEVIF events.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_events_get(void);

/**
 * @brief Function for setting triggered VEVIF events.
 *
 * @param[in] value VEVIF events value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_events_set(uint32_t value);

/**
 * @brief Function for triggering VEVIF events.
 *
 * @param[in] mask Mask of VEVIF events to be triggered.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_events_trigger(uint32_t mask);

/**
 * @brief Function for setting buffered triggered VEVIF events.
 *
 * @param[in] value Buffered VEVIF events value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_events_buffered_set(uint32_t value);

/**
 * @brief Function for retrieving the dirty status of buffered VEVIF events.
 *
 * @retval true  Buffer is dirty.
 * @retval false Buffer is clean.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_vevif_events_buffered_dirty_check(void);

/**
 * @brief Function for getting the subscribe configuration for VEVIF.
 *
 * @return Mask of tasks with enabled subscription.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_subscribe_get(void);

/**
 * @brief Function for setting the subscribe configuration for VEVIF.
 *
 * @param[in] value VEVIF subscription configuration mask to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_subscribe_set(uint32_t value);

/**
 * @brief Function for getting the publish configuration for VEVIF.
 *
 * @return Mask of events with enabled publication.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_publish_get(void);

/**
 * @brief Function for setting the publish configuration for VEVIF.
 *
 * @param[in] value VEVIF publication configuration mask to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_publish_set(uint32_t value);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] mask Mask of interrupts to be enabled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_int_enable(uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] mask Mask of interrupts to be disabled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vevif_int_disable(uint32_t mask);

/**
 * @brief Function for checking it the specified interrupts are enabled.
 *
 * @param[in] mask Mask of interrupts to be checked.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_int_enable_check(uint32_t mask);

#ifndef NRF_DECLARE_ONLY
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_tasks_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_TASKS);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_tasks_clear(uint32_t mask)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_TASKS, mask);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_tasks_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_TASKS, value);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_events_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_EVENTS);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_events_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_EVENTS, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_events_trigger(uint32_t mask)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_EVENTS, mask);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_events_buffered_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_EVENTSB, value);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_vevif_events_buffered_dirty_check(void)
{
    return ((nrf_csr_read(VPRCSR_NORDIC_EVENTSBS) & VPRCSR_NORDIC_EVENTSBS_DIRTYBIT_Msk)
            >> VPRCSR_NORDIC_EVENTSBS_DIRTYBIT_Pos) == VPRCSR_NORDIC_EVENTSBS_DIRTYBIT_DIRTY;
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_subscribe_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_SUBSCRIBE);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_subscribe_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_SUBSCRIBE, value);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_publish_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_PUBLISH);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_publish_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_PUBLISH, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_int_enable(uint32_t mask)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_INTEN, mask);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vevif_int_disable(uint32_t mask)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_INTEN, mask);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vevif_int_enable_check(uint32_t mask)
{
    return nrf_csr_read(VPRCSR_NORDIC_INTEN) & mask;
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_VPR_CSR_VEVIF_H__
