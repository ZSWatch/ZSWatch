/*
 * Copyright (c) 2019 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_OSCILLATORS_H__
#define NRF_OSCILLATORS_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_oscillators_hal OSCILLATORS HAL
 * @{
 * @ingroup nrf_clock
 * @brief   Hardware access layer for managing the OSCILLATORS peripheral.
 */

#if defined(OSCILLATORS_PLL_FREQ_FREQ_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PLL is present. */
#define NRF_OSCILLATORS_HAS_PLL 1
#else
#define NRF_OSCILLATORS_HAS_PLL 0
#endif

#if defined(OSCILLATORS_XOSC32M_CLOCKQUALITY_INDICATOR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether clock quality indicator is present. */
#define NRF_OSCILLATORS_HAS_CLOCK_QUALITY_IND 1
#else
#define NRF_OSCILLATORS_HAS_CLOCK_QUALITY_IND 0
#endif

#if defined(OSCILLATORS_XOSC32KI_INTCAP_VAL_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether configuration of internal capacitor using integer value is present. */
#define NRF_OSCILLATORS_HAS_LFXO_CAP_AS_INT_VALUE 1
#else
#define NRF_OSCILLATORS_HAS_LFXO_CAP_AS_INT_VALUE 0
#endif

#if defined(OSCILLATORS_XOSC32KI_BYPASS_BYPASS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether LFXO bypass is present. */
#define NRF_OSCILLATORS_HAS_LFXO_BYPASS 1
#else
#define NRF_OSCILLATORS_HAS_LFXO_BYPASS 0
#endif

#if NRF_OSCILLATORS_HAS_LFXO_BYPASS || NRF_OSCILLATORS_HAS_LFXO_CAP_AS_INT_VALUE
/** @brief Symbol indicating whether LFXO is present. */
#define NRF_OSCILLATORS_HAS_LFXO 1
#else
#define NRF_OSCILLATORS_HAS_LFXO 0
#endif

/**
 * @brief Macro for calculating HFXO internal capacitor value.
 *
 * Depending on the SoC used, a range of capacitance of internal capacitors is as follows:
 * - From 7 pF to 20 pF in 0.5 pF steps for nRF5340.
 * - From 4 pF to 17 pF in 0.25 pF steps for other SoCs.
 * This macro should be used to calculate argument's value for @ref nrf_oscillators_hfxo_cap_set function.
*/
#define NRF_OSCILLATORS_HFXO_CAP_CALCULATE(p_ficr_reg, cap_val) \
    OSCILLATORS_HFXO_CAP_CALCULATE(p_ficr_reg, cap_val)

#if NRF_OSCILLATORS_HAS_LFXO_CAP_AS_INT_VALUE
/**
 * @brief Macro for calculating LFXO internal capacitor value.
 *
 * The capacitance of internal capacitors ranges from 4 pF to 18 pF in 0.5 pF steps.
 * This macro should be used to calculate argument's value for @ref nrf_oscillators_lfxo_cap_set function.
*/
#define NRF_OSCILLATORS_LFXO_CAP_CALCULATE(p_ficr_reg, cap_val)                      \
      (((((p_ficr_reg->XOSC32KTRIM & FICR_XOSC32KTRIM_SLOPE_Msk)                     \
        >> FICR_XOSC32KTRIM_SLOPE_Pos) + 392) >> 9) * (uint32_t)(cap_val * 2 - 12) + \
       (((p_ficr_reg->XOSC32KTRIM & FICR_XOSC32KTRIM_OFFSET_Msk)                     \
        >> FICR_XOSC32KTRIM_OFFSET_Pos) >> 6))
#endif

#if NRF_OSCILLATORS_HAS_CLOCK_QUALITY_IND
/** @brief HFXO clock quality indicator. */
typedef enum
{
    NRF_OSCILLATORS_HFXO_CLOCK_QUALITY_NONE     = OSCILLATORS_XOSC32M_CLOCKQUALITY_INDICATOR_NoStatus, ///< Clock XOSC32M status is not defined.
    NRF_OSCILLATORS_HFXO_CLOCK_QUALITY_STARTING = OSCILLATORS_XOSC32M_CLOCKQUALITY_INDICATOR_Starting, ///< Clock XOSC32M has started but has not yet reached the specified frequency tolerance requirement fTOL_HFXO.
    NRF_OSCILLATORS_HFXO_CLOCK_QUALITY_STARTED  = OSCILLATORS_XOSC32M_CLOCKQUALITY_INDICATOR_Started   ///< Clock XOSC32M has started and is operating with the specified frequency tolerance requirement fTOL_HFXO.
} nrf_oscillators_hfxo_clock_quality_t;
#endif

#if NRF_OSCILLATORS_HAS_PLL
/** @brief PLL frequencies. */
typedef enum
{
    NRF_OSCILLATORS_PLL_FREQ_64M  = OSCILLATORS_PLL_FREQ_FREQ_CK64M,  ///< PLL 64 MHz frequency.
    NRF_OSCILLATORS_PLL_FREQ_128M = OSCILLATORS_PLL_FREQ_FREQ_CK128M, ///< PLL 128 MHz frequency.
} nrf_oscillators_pll_freq_t;
#endif

#if NRF_OSCILLATORS_HAS_LFXO
#if NRF_OSCILLATORS_HAS_LFXO_CAP_AS_INT_VALUE
/** @brief LFXO capacitance type. */
typedef uint32_t nrf_oscillators_lfxo_cap_t;

/** @brief Symbol specifying usage of external capacitors. */
#define NRF_OSCILLATORS_LFXO_CAP_EXTERNAL ((nrf_oscillators_lfxo_cap_t)0)
#else
/** @brief Capacitors configuration for LFXO. */
typedef enum
{
    NRF_OSCILLATORS_LFXO_CAP_EXTERNAL = OSCILLATORS_XOSC32KI_INTCAP_INTCAP_External, ///< Use external capacitors.
    NRF_OSCILLATORS_LFXO_CAP_6PF      = OSCILLATORS_XOSC32KI_INTCAP_INTCAP_C6PF,     ///< Use 6 pF internal capacitors.
    NRF_OSCILLATORS_LFXO_CAP_7PF      = OSCILLATORS_XOSC32KI_INTCAP_INTCAP_C7PF,     ///< Use 7 pF internal capacitors.
    NRF_OSCILLATORS_LFXO_CAP_9PF      = OSCILLATORS_XOSC32KI_INTCAP_INTCAP_C9PF,     ///< Use 9 pF internal capacitors.
#if defined(OSCILLATORS_XOSC32KI_INTCAP_INTCAP_C11PF) || defined(__NRFX_DOXYGEN__)
    NRF_OSCILLATORS_LFXO_CAP_11PF     = OSCILLATORS_XOSC32KI_INTCAP_INTCAP_C11PF,    ///< Use 11 pF internal capacitors.
#endif
} nrf_oscillators_lfxo_cap_t;
#endif // NRF_OSCILLATORS_HAS_LFXO_CAP_AS_INT_VALUE
#endif // NRF_OSCILLATORS_HAS_LFXO

#if NRF_OSCILLATORS_HAS_CLOCK_QUALITY_IND
/**
 * @brief Function for reading HFXO clock quality indicator.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Clock quality indicator value.
 */
NRF_STATIC_INLINE nrf_oscillators_hfxo_clock_quality_t
nrf_oscillators_hfxo_clock_quality_get(NRF_OSCILLATORS_Type * p_reg);
#endif

#if NRF_OSCILLATORS_HAS_PLL
/**
 * @brief Function for setting PLL frequency.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] freq  New PLL frequency.
 */
NRF_STATIC_INLINE void nrf_oscillators_pll_freq_set(NRF_OSCILLATORS_Type *     p_reg,
                                                    nrf_oscillators_pll_freq_t freq);

/**
 * @brief Function for getting PLL frequency.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Current PLL frequency value.
 */
NRF_STATIC_INLINE
nrf_oscillators_pll_freq_t nrf_oscillators_pll_freq_get(NRF_OSCILLATORS_Type * p_reg);
#endif

#if NRF_OSCILLATORS_HAS_LFXO
#if NRF_OSCILLATORS_HAS_LFXO_BYPASS
/**
 * @brief Function for enabling or disabling the bypass of LFXO with external clock source.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if bypass is to be enabled (use with rail-to-rail external source).
 *                   False if bypass is to be disabled (use with xtal or low-swing external source).
 */
NRF_STATIC_INLINE void nrf_oscillators_lfxo_bypass_set(NRF_OSCILLATORS_Type * p_reg, bool enable);
#endif

/**
 * @brief Function for configuring the internal capacitors of LFXO.
 *
 * For SoCs other than nRF5340, to calculate the correct @p cap_value,
 * use @ref NRF_OSCILLATORS_LFXO_CAP_CALCULATE macro.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] cap   Capacitors configuration.
 */
NRF_STATIC_INLINE void nrf_oscillators_lfxo_cap_set(NRF_OSCILLATORS_Type *     p_reg,
                                                    nrf_oscillators_lfxo_cap_t cap);
#endif

/**
 * @brief Function for configuring the internal capacitors of HFXO.
 *
 * To calculate the correct @p cap_value, use @ref NRF_OSCILLATORS_HFXO_CAP_CALCULATE macro.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] enable    True if internal capacitors are to be enabled, false otherwise.
 * @param[in] cap_value Value representing capacitance, calculated using provided equation.
 *                      Ignored when internal capacitors are disabled.
 */
NRF_STATIC_INLINE void nrf_oscillators_hfxo_cap_set(NRF_OSCILLATORS_Type * p_reg,
                                                    bool                   enable,
                                                    uint32_t               cap_value);

#ifndef NRF_DECLARE_ONLY

#if NRF_OSCILLATORS_HAS_CLOCK_QUALITY_IND
NRF_STATIC_INLINE nrf_oscillators_hfxo_clock_quality_t
nrf_oscillators_hfxo_clock_quality_get(NRF_OSCILLATORS_Type * p_reg)
{
    return (nrf_oscillators_hfxo_clock_quality_t)(p_reg->XOSC32M.CLOCKQUALITY);
}
#endif

#if NRF_OSCILLATORS_HAS_PLL
NRF_STATIC_INLINE void nrf_oscillators_pll_freq_set(NRF_OSCILLATORS_Type *     p_reg,
                                                    nrf_oscillators_pll_freq_t freq)
{
    p_reg->PLL.FREQ = (uint32_t)freq;
}

NRF_STATIC_INLINE
nrf_oscillators_pll_freq_t nrf_oscillators_pll_freq_get(NRF_OSCILLATORS_Type * p_reg)
{
    return (nrf_oscillators_pll_freq_t)(p_reg->PLL.CURRENTFREQ);
}
#endif

#if NRF_OSCILLATORS_HAS_LFXO
#if NRF_OSCILLATORS_HAS_LFXO_BYPASS
NRF_STATIC_INLINE void nrf_oscillators_lfxo_bypass_set(NRF_OSCILLATORS_Type * p_reg, bool enable)
{
    p_reg->XOSC32KI.BYPASS = (enable ? OSCILLATORS_XOSC32KI_BYPASS_BYPASS_Enabled :
                                       OSCILLATORS_XOSC32KI_BYPASS_BYPASS_Disabled);
}
#endif

NRF_STATIC_INLINE void nrf_oscillators_lfxo_cap_set(NRF_OSCILLATORS_Type *     p_reg,
                                                    nrf_oscillators_lfxo_cap_t cap)
{
    p_reg->XOSC32KI.INTCAP = (uint32_t)cap;
}
#endif

NRF_STATIC_INLINE void nrf_oscillators_hfxo_cap_set(NRF_OSCILLATORS_Type * p_reg,
                                                    bool                   enable,
                                                    uint32_t               cap_value)
{
#if defined(OSCILLATORS_XOSC32MCAPS_CAPVALUE_Msk)
    p_reg->XOSC32MCAPS =
        (enable ? ((OSCILLATORS_XOSC32MCAPS_ENABLE_Enabled << OSCILLATORS_XOSC32MCAPS_ENABLE_Pos) |
                   (cap_value << OSCILLATORS_XOSC32MCAPS_CAPVALUE_Pos))
                : (OSCILLATORS_XOSC32MCAPS_ENABLE_Disabled << OSCILLATORS_XOSC32MCAPS_ENABLE_Pos));
#else
    p_reg->XOSC32M.CONFIG.INTCAP = enable ? cap_value : 0;
#endif
}
#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_OSCILLATORS_H__
