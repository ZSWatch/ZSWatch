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

#ifndef NRFY_DPPI_H__
#define NRFY_DPPI_H__

#include <nrfx.h>
#include <hal/nrf_dppi.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfy_dppi DPPI HALY
 * @{
 * @ingroup nrf_dppi
 * @brief   Hardware access layer with cache and barrier support for managing the DPPI peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/**
 * @brief Function for enabling or disabling multiple DPPI channels.
 *
 * The bits in @c mask value correspond to particular channels. It means that
 * writing 1 to bit 0 enables or disables channel 0,
 * writing 1 to bit 1 enables or disables channel 1 etc.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mask   Channel mask.
 * @param[in] enable True if specified channels are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_dppi_channels_set(NRF_DPPIC_Type * p_reg, uint32_t mask, bool enable)
{
    if (enable == true)
    {
        nrf_dppi_channels_enable(p_reg, mask);
    }
    else
    {
        nrf_dppi_channels_disable(p_reg, mask);
    }
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_channel_number_get} */
NRFY_STATIC_INLINE uint8_t nrfy_dppi_channel_number_get(NRF_DPPIC_Type const * p_reg)
{
    return nrf_dppi_channel_number_get(p_reg);
}

/** @refhal{nrf_dppi_group_number_get} */
NRFY_STATIC_INLINE uint8_t nrfy_dppi_group_number_get(NRF_DPPIC_Type const * p_reg)
{
    return nrf_dppi_group_number_get(p_reg);
}

/** @refhal{nrf_dppi_task_trigger} */
NRFY_STATIC_INLINE void nrfy_dppi_task_trigger(NRF_DPPIC_Type * p_reg, nrf_dppi_task_t dppi_task)
{
    nrf_dppi_task_trigger(p_reg, dppi_task);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_dppi_task_address_get(NRF_DPPIC_Type const * p_reg,
                                                       nrf_dppi_task_t        task)
{
    return nrf_dppi_task_address_get(p_reg, task);
}

/** @refhal{nrf_dppi_channel_check} */
NRFY_STATIC_INLINE bool nrfy_dppi_channel_check(NRF_DPPIC_Type const * p_reg, uint8_t channel)
{
    nrf_barrier_rw();
    bool check = nrf_dppi_channel_check(p_reg, channel);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_dppi_channels_enable} */
NRFY_STATIC_INLINE void nrfy_dppi_channels_enable(NRF_DPPIC_Type * p_reg, uint32_t mask)
{
    nrf_dppi_channels_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_channels_disable} */
NRFY_STATIC_INLINE void nrfy_dppi_channels_disable(NRF_DPPIC_Type * p_reg, uint32_t mask)
{
    nrf_dppi_channels_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_channels_disable_all} */
NRFY_STATIC_INLINE void nrfy_dppi_channels_disable_all(NRF_DPPIC_Type * p_reg)
{
    nrf_dppi_channels_disable_all(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_dppi_subscribe_set(NRF_DPPIC_Type * p_reg,
                                                nrf_dppi_task_t  task,
                                                uint8_t          channel)
{
    nrf_dppi_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_dppi_subscribe_clear(NRF_DPPIC_Type * p_reg, nrf_dppi_task_t task)
{
    nrf_dppi_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_channels_group_set} */
NRFY_STATIC_INLINE void nrfy_dppi_channels_group_set(NRF_DPPIC_Type *         p_reg,
                                                     uint32_t                 channel_mask,
                                                     nrf_dppi_channel_group_t channel_group)
{
    nrf_dppi_channels_group_set(p_reg, channel_mask, channel_group);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_channels_include_in_group} */
NRFY_STATIC_INLINE void nrfy_dppi_channels_include_in_group(NRF_DPPIC_Type *         p_reg,
                                                            uint32_t                 channel_mask,
                                                            nrf_dppi_channel_group_t channel_group)
{
    nrf_dppi_channels_include_in_group(p_reg, channel_mask, channel_group);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_channels_remove_from_group} */
NRFY_STATIC_INLINE
void nrfy_dppi_channels_remove_from_group(NRF_DPPIC_Type *         p_reg,
                                          uint32_t                 channel_mask,
                                          nrf_dppi_channel_group_t channel_group)
{
    nrf_dppi_channels_remove_from_group(p_reg, channel_mask, channel_group);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_group_clear} */
NRFY_STATIC_INLINE void nrfy_dppi_group_clear(NRF_DPPIC_Type *         p_reg,
                                              nrf_dppi_channel_group_t group)
{
    nrf_dppi_group_clear(p_reg, group);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_group_enable} */
NRFY_STATIC_INLINE void nrfy_dppi_group_enable(NRF_DPPIC_Type *         p_reg,
                                               nrf_dppi_channel_group_t group)
{
    nrf_dppi_group_enable(p_reg, group);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_group_disable} */
NRFY_STATIC_INLINE void nrfy_dppi_group_disable(NRF_DPPIC_Type *         p_reg,
                                                nrf_dppi_channel_group_t group)
{
    nrf_dppi_group_disable(p_reg, group);
    nrf_barrier_w();
}

/** @refhal{nrf_dppi_group_enable_task_get} */
NRFY_STATIC_INLINE nrf_dppi_task_t nrfy_dppi_group_enable_task_get(uint8_t index)
{
    return nrf_dppi_group_enable_task_get(index);
}

/** @refhal{nrf_dppi_group_disable_task_get} */
NRFY_STATIC_INLINE nrf_dppi_task_t nrfy_dppi_group_disable_task_get(uint8_t index)
{
    return nrf_dppi_group_disable_task_get(index);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFY_DPPI_H__
