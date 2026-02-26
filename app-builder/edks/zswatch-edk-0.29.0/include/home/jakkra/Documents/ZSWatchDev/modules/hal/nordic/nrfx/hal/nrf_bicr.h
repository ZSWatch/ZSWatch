/*
 * Copyright (c) 2024 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_BICR_H__
#define NRF_BICR_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_bicr_hal BICR HAL
 * @{
 * @ingroup nrf_icr
 * @brief   Hardware access layer (HAL) for getting data from
 *          the Board Information Configuration Registers (BICR).
 */

#if defined(BICR_LFOSC_LFXOCONFIG_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether BICR LFOSC.LFXOCONFIG register is present. */
#define NRF_BICR_HAS_LFOSC_LFXOCONFIG 1
#else
#define NRF_BICR_HAS_LFOSC_LFXOCONFIG 0
#endif

#if defined(BICR_HFXO_STARTUPTIME_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether BICR HFXO.STARTUPTIME register is present. */
#define NRF_BICR_HAS_HFXO_STARTUPTIME 1
#else
#define NRF_BICR_HAS_HFXO_STARTUPTIME 0
#endif

#if NRF_BICR_HAS_LFOSC_LFXOCONFIG
/** @brief LFOSC unconfigured startup time value */
#define NRF_BICR_LFOSC_STARTUP_TIME_UNCONFIGURED BICR_LFOSC_LFXOCONFIG_TIME_Unconfigured

/** @brief LFOSC Mode. */
typedef enum
{
    NRF_BICR_LFOSC_MODE_UNCONFIGURED = BICR_LFOSC_LFXOCONFIG_MODE_Unconfigured, ///< The mode is unconfigured.
    NRF_BICR_LFOSC_MODE_CRYSTAL      = BICR_LFOSC_LFXOCONFIG_MODE_Crystal,      ///< LFXO in external crystal oscillator mode.
    NRF_BICR_LFOSC_MODE_EXTSINE      = BICR_LFOSC_LFXOCONFIG_MODE_ExtSine,      ///< LFXO in external sine wave mode.
    NRF_BICR_LFOSC_MODE_EXTSQUARE    = BICR_LFOSC_LFXOCONFIG_MODE_ExtSquare,    ///< LFXO in external square wave mode.
    NRF_BICR_LFOSC_MODE_DISABLED     = BICR_LFOSC_LFXOCONFIG_MODE_Disabled,     ///< LFXO is not to be used.
} nrf_bicr_lfosc_mode_t;

/** @brief LFOSC Accuracy. */
typedef enum
{
    NRF_BICR_LFOSC_ACCURACY_UNCONFIGURED = BICR_LFOSC_LFXOCONFIG_ACCURACY_Unconfigured, ///< The accuracy is unconfigured.
    NRF_BICR_LFOSC_ACCURACY_20PPM        = BICR_LFOSC_LFXOCONFIG_ACCURACY_20ppm,        ///< LFXO crystal or external signal has an accuracy of 20 ppm.
    NRF_BICR_LFOSC_ACCURACY_30PPM        = BICR_LFOSC_LFXOCONFIG_ACCURACY_30ppm,        ///< LFXO crystal or external signal has an accuracy of 30 ppm.
    NRF_BICR_LFOSC_ACCURACY_50PPM        = BICR_LFOSC_LFXOCONFIG_ACCURACY_50ppm,        ///< LFXO crystal or external signal has an accuracy of 50 ppm.
    NRF_BICR_LFOSC_ACCURACY_75PPM        = BICR_LFOSC_LFXOCONFIG_ACCURACY_75ppm,        ///< LFXO crystal or external signal has an accuracy of 75 ppm.
    NRF_BICR_LFOSC_ACCURACY_100PPM       = BICR_LFOSC_LFXOCONFIG_ACCURACY_100ppm,       ///< LFXO crystal or external signal has an accuracy of 100 ppm.
    NRF_BICR_LFOSC_ACCURACY_150PPM       = BICR_LFOSC_LFXOCONFIG_ACCURACY_150ppm,       ///< LFXO crystal or external signal has an accuracy of 150 ppm.
    NRF_BICR_LFOSC_ACCURACY_250PPM       = BICR_LFOSC_LFXOCONFIG_ACCURACY_250ppm,       ///< LFXO crystal or external signal has an accuracy of 250 ppm.
    NRF_BICR_LFOSC_ACCURACY_500PPM       = BICR_LFOSC_LFXOCONFIG_ACCURACY_500ppm,       ///< LFXO crystal or external signal has an accuracy of 500 ppm.
} nrf_bicr_lfosc_accuracy_t;
#endif

#if NRF_BICR_HAS_HFXO_STARTUPTIME
/** @brief HFXO unconfigured startup time value */
#define NRF_BICR_HFXO_STARTUP_TIME_UNCONFIGURED BICR_HFXO_STARTUPTIME_TIME_Unconfigured
#endif

#if NRF_BICR_HAS_LFOSC_LFXOCONFIG
/**
 * @brief Function for getting the LFOSC startup time in milliseconds.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return LFOSC startup time in milliseconds, or NRF_BICR_LFOSC_STARTUP_TIME_UNCONFIGURED if not configured.
 */
NRF_STATIC_INLINE uint16_t nrf_bicr_lfosc_startup_time_ms_get(NRF_BICR_Type const * p_reg);

/**
 * @brief Function for getting the LFOSC mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return LFOSC mode.
 */
NRF_STATIC_INLINE nrf_bicr_lfosc_mode_t nrf_bicr_lfosc_mode_get(NRF_BICR_Type const * p_reg);

/**
 * @brief Function for getting the LFOSC accuracy.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return LFOSC accuracy.
 */
NRF_STATIC_INLINE nrf_bicr_lfosc_accuracy_t nrf_bicr_lfosc_accuracy_get(NRF_BICR_Type const * p_reg);
#endif

#if NRF_BICR_HAS_HFXO_STARTUPTIME
/**
 * @brief Function for getting the HFXO startup time in microseconds.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return HFXO startup time in microseconds, or NRF_BICR_HFXO_STARTUP_TIME_UNCONFIGURED if not configured.
 */
NRF_STATIC_INLINE uint32_t nrf_bicr_hfxo_startup_time_us_get(NRF_BICR_Type const * p_reg);
#endif

#ifndef NRF_DECLARE_ONLY

#if NRF_BICR_HAS_LFOSC_LFXOCONFIG
NRF_STATIC_INLINE uint16_t nrf_bicr_lfosc_startup_time_ms_get(NRF_BICR_Type const * p_reg)
{
    return (uint16_t)((p_reg->LFOSC.LFXOCONFIG & BICR_LFOSC_LFXOCONFIG_TIME_Msk) >>
                      BICR_LFOSC_LFXOCONFIG_TIME_Pos);
}

NRF_STATIC_INLINE nrf_bicr_lfosc_mode_t nrf_bicr_lfosc_mode_get(NRF_BICR_Type const * p_reg)
{
    return (nrf_bicr_lfosc_mode_t)((p_reg->LFOSC.LFXOCONFIG & BICR_LFOSC_LFXOCONFIG_MODE_Msk) >>
                                   BICR_LFOSC_LFXOCONFIG_MODE_Pos);
}

NRF_STATIC_INLINE nrf_bicr_lfosc_accuracy_t nrf_bicr_lfosc_accuracy_get(NRF_BICR_Type const * p_reg)
{
    return (nrf_bicr_lfosc_accuracy_t)((p_reg->LFOSC.LFXOCONFIG & BICR_LFOSC_LFXOCONFIG_ACCURACY_Msk) >>
                                       BICR_LFOSC_LFXOCONFIG_ACCURACY_Pos);
}
#endif

#if NRF_BICR_HAS_HFXO_STARTUPTIME
NRF_STATIC_INLINE uint32_t nrf_bicr_hfxo_startup_time_us_get(NRF_BICR_Type const * p_reg)
{
    return (p_reg->HFXO.STARTUPTIME & BICR_HFXO_STARTUPTIME_TIME_Msk) >>
           BICR_HFXO_STARTUPTIME_TIME_Pos;
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_BICR_H__
