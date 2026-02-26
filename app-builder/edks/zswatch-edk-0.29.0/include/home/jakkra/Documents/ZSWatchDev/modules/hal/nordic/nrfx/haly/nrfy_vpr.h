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

#ifndef NRFY_VPR_H__
#define NRFY_VPR_H__

#include <nrfx.h>
#include <hal/nrf_vpr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfy_vpr VPR HALY
 * @{
 * @ingroup nrf_vpr
 * @brief   Hardware access layer with cache and barrier support for managing the VPR peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @refhal{nrf_vpr_task_trigger} */
NRFY_STATIC_INLINE void nrfy_vpr_task_trigger(NRF_VPR_Type * p_reg, nrf_vpr_task_t task)
{
    nrf_vpr_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_vpr_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_vpr_task_address_get(NRF_VPR_Type const * p_reg,
                                                      nrf_vpr_task_t       task)
{
    return nrf_vpr_task_address_get(p_reg, task);
}

/** @refhal{nrf_vpr_trigger_task_get} */
NRFY_STATIC_INLINE nrf_vpr_task_t nrfy_vpr_trigger_task_get(uint8_t index)
{
    return nrf_vpr_trigger_task_get(index);
}

/** @refhal{nrf_vpr_event_clear} */
NRFY_STATIC_INLINE void nrfy_vpr_event_clear(NRF_VPR_Type * p_reg, nrf_vpr_event_t event)
{
    nrf_vpr_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_vpr_event_check} */
NRFY_STATIC_INLINE bool nrfy_vpr_event_check(NRF_VPR_Type const * p_reg, nrf_vpr_event_t event)
{
    nrf_barrier_r();
    bool ret = nrf_vpr_event_check(p_reg, event);
    nrf_barrier_r();

    return ret;
}

/** @refhal{nrf_vpr_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_vpr_event_address_get(NRF_VPR_Type const * p_reg,
                                                       nrf_vpr_event_t      event)
{
    return nrf_vpr_event_address_get(p_reg, event);
}

/** @refhal{nrf_vpr_triggered_event_get} */
NRFY_STATIC_INLINE nrf_vpr_event_t nrfy_vpr_triggered_event_get(uint8_t index)
{
    return nrf_vpr_triggered_event_get(index);
}

/** @refhal{nrf_vpr_int_enable} */
NRFY_STATIC_INLINE void nrfy_vpr_int_enable(NRF_VPR_Type * p_reg,
                                            uint32_t       mask)
{
    nrf_vpr_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_vpr_int_disable} */
NRFY_STATIC_INLINE void nrfy_vpr_int_disable(NRF_VPR_Type * p_reg,
                                             uint32_t       mask)
{
    nrf_vpr_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_vpr_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_vpr_int_enable_check(NRF_VPR_Type const * p_reg,
                                                      uint32_t             mask)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_vpr_int_enable_check(p_reg, mask);
    nrf_barrier_r();

    return ret;
}

/** @refhal{nrf_vpr_cpurun_set} */
NRFY_STATIC_INLINE void nrfy_vpr_cpurun_set(NRF_VPR_Type * p_reg,
                                            bool           enable)
{
    nrf_vpr_cpurun_set(p_reg, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_vpr_cpurun_get} */
NRFY_STATIC_INLINE bool nrfy_vpr_cpurun_get(NRF_VPR_Type const * p_reg)
{
    nrf_barrier_rw();
    bool ret = nrf_vpr_cpurun_get(p_reg);
    nrf_barrier_r();

    return ret;
}

/** @refhal{nrf_vpr_initpc_set} */
NRFY_STATIC_INLINE void nrfy_vpr_initpc_set(NRF_VPR_Type * p_reg,
                                            uint32_t       pc)
{
    nrf_vpr_initpc_set(p_reg, pc);
    nrf_barrier_w();
}

/** @refhal{nrf_vpr_initpc_get} */
NRFY_STATIC_INLINE uint32_t nrfy_vpr_initpc_get(NRF_VPR_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_vpr_initpc_get(p_reg);
    nrf_barrier_r();

    return ret;
}

/** @refhal{nrf_vpr_debugif_dmcontrol_set} */
NRFY_STATIC_INLINE void nrfy_vpr_debugif_dmcontrol_set(NRF_VPR_Type *      p_reg,
                                                       nrf_vpr_dmcontrol_t signal,
                                                       bool                enable)
{
    nrf_vpr_debugif_dmcontrol_set(p_reg, signal, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_vpr_debugif_dmcontrol_get} */
NRFY_STATIC_INLINE bool nrfy_vpr_debugif_dmcontrol_get(NRF_VPR_Type const * p_reg,
                                                       nrf_vpr_dmcontrol_t  signal)
{
    nrf_barrier_rw();
    bool ret = nrf_vpr_debugif_dmcontrol_get(p_reg, signal);
    nrf_barrier_r();

    return ret;
}

/** @} */

#ifdef __cplusplus
}
#endif
#endif // NRFY_VPR_H__
