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

#ifndef NRF_GLITCHDET_H__
#define NRF_GLITCHDET_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_glitchdet_hal GLITCHDET HAL
 * @{
 * @ingroup nrf_glitchdet
 * @brief   Hardware access layer for managing the Voltage Glitch Detectors (GLITCHDET) peripheral.
 */

/** @brief Glitch detector mode. */
typedef enum
{
    NRF_GLITCHDET_MODE_HIGH_PASS = GLITCHDET_CONFIG_MODE_HighPassFilter, ///< High pass filter mode.
    NRF_GLITCHDET_MODE_CAP_DIV   = GLITCHDET_CONFIG_MODE_CapDiv,         ///< Cap divider mode.
} nrf_glitchdet_mode_t;

/**
 * @brief Function for checking whether glitch detector is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Glitch detector is enabled.
 * @retval false Glitch detector is disabled.
 */
NRF_STATIC_INLINE bool nrf_glitchdet_enable_check(NRF_GLITCHDET_Type const * p_reg);

/**
 * @brief Function for enabling or disabling glitch detector.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if glitch detector is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_glitchdet_enable_set(NRF_GLITCHDET_Type * p_reg, bool enable);

/**
 * @brief Function for getting glitch detector mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Glitch detector mode.
 */
NRF_STATIC_INLINE nrf_glitchdet_mode_t nrf_glitchdet_mode_get(NRF_GLITCHDET_Type const * p_reg);

/**
 * @brief Function for setting glitch detector mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Glitch detector mode to be set.
 */
NRF_STATIC_INLINE void nrf_glitchdet_mode_set(NRF_GLITCHDET_Type * p_reg,
                                              nrf_glitchdet_mode_t mode);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE bool nrf_glitchdet_enable_check(NRF_GLITCHDET_Type const * p_reg)
{
    return ((p_reg->CONFIG & GLITCHDET_CONFIG_ENABLE_Msk)
            >> GLITCHDET_CONFIG_ENABLE_Pos) == GLITCHDET_CONFIG_ENABLE_Enable;
}

NRF_STATIC_INLINE void nrf_glitchdet_enable_set(NRF_GLITCHDET_Type * p_reg, bool enable)
{
    p_reg->CONFIG = (p_reg->CONFIG & ~GLITCHDET_CONFIG_ENABLE_Msk) |
                    ((enable ? GLITCHDET_CONFIG_ENABLE_Enable : GLITCHDET_CONFIG_ENABLE_Disable) <<
                     GLITCHDET_CONFIG_ENABLE_Pos);
}

NRF_STATIC_INLINE nrf_glitchdet_mode_t nrf_glitchdet_mode_get(NRF_GLITCHDET_Type const * p_reg)
{
    return (nrf_glitchdet_mode_t)((p_reg->CONFIG & GLITCHDET_CONFIG_MODE_Msk) >>
                                  GLITCHDET_CONFIG_MODE_Pos);
}

NRF_STATIC_INLINE void nrf_glitchdet_mode_set(NRF_GLITCHDET_Type * p_reg,
                                              nrf_glitchdet_mode_t mode)
{
    p_reg->CONFIG = (p_reg->CONFIG & ~GLITCHDET_CONFIG_MODE_Msk) |
                    (mode << GLITCHDET_CONFIG_MODE_Pos);
}
#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_GLITCHDET_H__
