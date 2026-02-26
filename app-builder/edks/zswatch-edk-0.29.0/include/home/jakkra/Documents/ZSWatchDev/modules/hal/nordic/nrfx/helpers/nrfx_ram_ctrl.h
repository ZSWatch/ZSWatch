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

#ifndef NRFX_RAM_CTRL_H
#define NRFX_RAM_CTRL_H

#include <nrfx.h>

#if defined(MEMCONF_PRESENT)
#include <hal/nrf_memconf.h>
#elif defined(NRF_VMC)
#include <hal/nrf_vmc.h>
#elif defined(POWER_PRESENT)
#include <hal/nrf_power.h>
#else
#error "Unsupported."
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_ram_ctrl Generic RAM Control layer
 * @{
 * @ingroup nrfx
 *
 * @brief Helper layer that provides a uniform way of controlling the RAM power and retention settings.
 */

/**
 * @brief Function for setting if the RAM sections containing specified object
 *        are to be powered on or off.
 *
 * @param[in] p_object Pointer to the object.
 * @param[in] length   Object size in bytes.
 * @param[in] enable   True if RAM sections are to be powered on, false otherwise.
 */
void nrfx_ram_ctrl_power_enable_set(void const * p_object, size_t length, bool enable);

/**
 * @brief Function for setting if all RAM sections are to be powered on or off.
 *
 * @param[in] enable True if RAM sections are to be powered on, false otherwise.
 */
void nrfx_ram_ctrl_power_enable_all_set(bool enable);

/**
 * @brief Function for setting if the RAM sections containing specified object
 *        are to be retained or not.
 *
 * @param[in] p_object Pointer to the object.
 * @param[in] length   Object size in bytes.
 * @param[in] enable   True if RAM sections are to be retained, false otherwise.
 */
void nrfx_ram_ctrl_retention_enable_set(void const * p_object, size_t length, bool enable);

/**
 * @brief Function for setting if all RAM sections are to be retained or not.
 *
 * @param[in] enable True if RAM sections are to be retained, false otherwise.
 */
void nrfx_ram_ctrl_retention_enable_all_set(bool enable);

/**
 * @brief Function for setting if the specified mask of RAM sections contained within given RAM block
 *        is to be powered on or off.
 *
 * @param[in] block_idx    RAM block index.
 * @param[in] section_mask Mask of RAM sections.
 * @param[in] enable       True if RAM sections are to be powered on, false otherwise.
 */
__STATIC_INLINE void nrfx_ram_ctrl_section_power_mask_enable_set(uint8_t  block_idx,
                                                                 uint32_t section_mask,
                                                                 bool     enable)
{
#if defined(MEMCONF_PRESENT)
    nrf_memconf_ramblock_control_mask_enable_set(NRF_MEMCONF, block_idx, section_mask, enable);

#elif defined(NRF_VMC)
    section_mask <<= NRF_VMC_POWER_S0_POS;
    if (enable)
    {
        nrf_vmc_ram_block_power_set(NRF_VMC, block_idx, (nrf_vmc_power_t)section_mask);
    }
    else
    {
        nrf_vmc_ram_block_power_clear(NRF_VMC, block_idx, (nrf_vmc_power_t)section_mask);
    }

#elif defined(POWER_PRESENT)
    section_mask <<= NRF_POWER_RAMPOWER_S0POWER_POS;
    if (enable)
    {
        nrf_power_rampower_mask_on(NRF_POWER, block_idx, section_mask);
    }
    else
    {
        nrf_power_rampower_mask_off(NRF_POWER, block_idx, section_mask);
    }

#endif
}

/**
 * @brief Function for setting if the specified mask of RAM sections contained within given RAM block
 *        is to be retained or not.
 *
 * @param[in] block_idx    RAM block index.
 * @param[in] section_mask Mask of RAM sections.
 * @param[in] enable       True if RAM sections are to be retained, false otherwise.
 */
__STATIC_INLINE void nrfx_ram_ctrl_section_retention_mask_enable_set(uint8_t  block_idx,
                                                                     uint32_t section_mask,
                                                                     bool     enable)
{
#if defined(MEMCONF_PRESENT)
    NRFX_ASSERT(block_idx < NRF_MEMCONF_POWERBLOCK_COUNT);
    nrf_memconf_ramblock_ret_mask_enable_set(NRF_MEMCONF, block_idx, section_mask, enable);
#if NRF_MEMCONF_HAS_RET2
    nrf_memconf_ramblock_ret2_mask_enable_set(NRF_MEMCONF, block_idx, section_mask, enable);
#endif

#elif defined(NRF_VMC)
    section_mask <<= NRF_VMC_RETENTION_S0_POS;
    if (enable)
    {
        nrf_vmc_ram_block_retention_set(NRF_VMC, block_idx, (nrf_vmc_retention_t)section_mask);
    }
    else
    {
        nrf_vmc_ram_block_retention_clear(NRF_VMC, block_idx, (nrf_vmc_retention_t)section_mask);
    }

#elif defined(POWER_PRESENT)
    section_mask <<= NRF_POWER_RAMPOWER_S0RETENTION_POS;
    if (enable)
    {
        nrf_power_rampower_mask_on(NRF_POWER, block_idx, section_mask);
    }
    else
    {
        nrf_power_rampower_mask_off(NRF_POWER, block_idx, section_mask);
    }

#endif
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_RAM_CTRL_H
