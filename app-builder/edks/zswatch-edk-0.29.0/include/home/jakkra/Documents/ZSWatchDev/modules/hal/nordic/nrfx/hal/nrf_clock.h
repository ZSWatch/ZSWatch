/*
 * Copyright (c) 2015 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_CLOCK_H__
#define NRF_CLOCK_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_clock_hal Clock HAL
 * @{
 * @ingroup nrf_clock
 * @brief   Hardware access layer for managing the CLOCK peripheral.
 *
 * This code can be used to manage low-frequency clock (LFCLK), high-frequency clock (HFCLK),
 * high-frequency 192 MHz clock (HFCLK192M), high-frequency audio clock (HFCLKAUDIO) and
 * high-frequency 24 MHz clock (HFCLK24M) settings.
 */

#if defined(CLOCK_LFCLKSRC_BYPASS_Msk) && defined(CLOCK_LFCLKSRC_EXTERNAL_Msk)
// Enable support for external LFCLK sources. Read more in the Product Specification.
#define NRF_CLOCK_USE_EXTERNAL_LFCLK_SOURCES
#endif

#if defined(CLOCK_INTENSET_DONE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the Low Frequency Clock calibration is present. */
#define NRF_CLOCK_HAS_CALIBRATION 1
#else
#define NRF_CLOCK_HAS_CALIBRATION 0
#endif

#if defined(CLOCK_CTIV_CTIV_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the Low Frequency Clock calibration timer is present. */
#define NRF_CLOCK_HAS_CALIBRATION_TIMER 1
#else
#define NRF_CLOCK_HAS_CALIBRATION_TIMER 0
#endif

#if defined(CLOCK_INTENSET_HFCLK192MSTARTED_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the 192 MHz clock is present. */
#define NRF_CLOCK_HAS_HFCLK192M 1
#else
#define NRF_CLOCK_HAS_HFCLK192M 0
#endif

#if defined(CLOCK_INTENSET_HFCLKAUDIOSTARTED_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the Audio clock is present. */
#define NRF_CLOCK_HAS_HFCLKAUDIO 1
#else
#define NRF_CLOCK_HAS_HFCLKAUDIO 0
#endif

#if defined(CLOCK_INTENSET_XO24MSTARTED_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the 24 MHz clock is present. */
#define NRF_CLOCK_HAS_HFCLK24M 1
#else
#define NRF_CLOCK_HAS_HFCLK24M 0
#endif

#if defined(CLOCK_HFCLKCTRL_HCLK_Div1) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the HFCLK frequency configuration is present. */
#define NRF_CLOCK_HAS_HFCLK_DIV 1
#else
#define NRF_CLOCK_HAS_HFCLK_DIV 0
#endif

#if defined(CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Msk) || defined(CLOCK_LFCLK_ALWAYSRUN_ALWAYSRUN_Msk) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the ALWAYSRUN register is present for LFCLK. */
#define NRF_CLOCK_HAS_LFCLK_ALWAYSRUN 1
#else
#define NRF_CLOCK_HAS_LFCLK_ALWAYSRUN 0
#endif

#if defined(CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the ALWAYSRUN register is present for HFCLK. */
#define NRF_CLOCK_HAS_HFCLK_ALWAYSRUN 1
#else
#define NRF_CLOCK_HAS_HFCLK_ALWAYSRUN 0
#endif

#if defined(CLOCK_HFCLKRUN_STATUS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the HFCLK clock is present. */
#define NRF_CLOCK_HAS_HFCLK 1
#else
#define NRF_CLOCK_HAS_HFCLK 0
#endif

#if defined(CLOCK_HFCLKSRC_SRC_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the HFCLKSRC register is present. */
#define NRF_CLOCK_HAS_HFCLKSRC 1
#else
#define NRF_CLOCK_HAS_HFCLKSRC 0
#endif

#if defined(CLOCK_TASKS_PLLSTART_TASKS_PLLSTART_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the PLL section is present. */
#define NRF_CLOCK_HAS_PLL 1
#else
#define NRF_CLOCK_HAS_PLL 0
#endif

#if defined(CLOCK_TASKS_XOSTART_TASKS_XOSTART_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether XO registers are present. */
#define NRF_CLOCK_HAS_XO 1
#else
#define NRF_CLOCK_HAS_XO 0
#endif

#if defined(CLOCK_LFCLKSRC_SRC_Xtal) || defined(CLOCK_LFCLK_SRC_SRC_LFXO) || \
    defined(CLOCK_LFCLKSTAT_SRC_LFXO_Msk) || defined(CLOCK_LFCLKSRC_SRC_LFXO) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether XTAL clock source is present. */
#define NRF_CLOCK_HAS_XTAL 1
#else
#define NRF_CLOCK_HAS_XTAL 0
#endif

#if defined(CLOCK_TASKS_XOTUNE_TASKS_XOTUNE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether XOTUNE registers are present. */
#define NRF_CLOCK_HAS_XO_TUNE 1
#else
#define NRF_CLOCK_HAS_XO_TUNE 0
#endif

#if defined(CLOCK_LFCLK_SRC_SRC_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CLOCK type contains LFCLK subtype. */
#define NRF_CLOCK_HAS_LFCLK_TYPE 1
#else
#define NRF_CLOCK_HAS_LFCLK_TYPE 0
#endif

#if defined(CLOCK_LFCLK_SRC_SRC_LFXO) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether LFCLK has LFXO source. */
#define NRF_CLOCK_HAS_LFCLK_LFXO 1
#else
#define NRF_CLOCK_HAS_LFCLK_LFXO 0
#endif

#if defined(CLOCK_INTPEND_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether INTPEND register is present. */
#define NRF_CLOCK_HAS_INTPEND 1
#else
#define NRF_CLOCK_HAS_INTPEND 0
#endif

#if defined(CLOCK_LFCLKCTRL_SRC_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CLOCK type contains LFCLKCTRL subtype. */
#define NRF_CLOCK_HAS_LFCLKCTRL 1
#else
#define NRF_CLOCK_HAS_LFCLKCTRL 0
#endif

#if defined(CLOCK_LFCLKSTAT_RUN_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CLOCK type contains LFCLKSTAT subtype. */
#define NRF_CLOCK_HAS_LFCLKSTAT 1
#else
#define NRF_CLOCK_HAS_LFCLKSTAT 0
#endif

#if defined(CLOCK_CONFIG_SETUP_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CLOCK type contains CONFIG subtype. */
#define NRF_CLOCK_HAS_CONFIG 1
#else
#define NRF_CLOCK_HAS_CONFIG 0
#endif

#if defined(CLOCK_INTENSET_LFCLKSRCCHANGED_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether LFCLK source change event is present. */
#define NRF_CLOCK_HAS_LFCLK_SRC_CHANGED 1
#else
#define NRF_CLOCK_HAS_LFCLK_SRC_CHANGED 0
#endif

#if defined(CLOCK_LFCLK_SRCCOPY_SRC_Msk) || defined(CLOCK_LFCLKSRCCOPY_SRC_Msk) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SRCCOPY register for LFCLK is present. */
#define NRF_CLOCK_HAS_SRC_COPY 1
#else
#define NRF_CLOCK_HAS_SRC_COPY 0
#endif

#if defined(CLOCK_LFCLK_STAT_ALWAYSRUNNING_Msk) || defined(CLOCK_LFCLKSTAT_ALWAYSRUNNING_Msk) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether clock domain always run setting is available. */
#define NRF_CLOCK_HAS_ALWAYSRUN_ACTIVE 1
#else
#define NRF_CLOCK_HAS_ALWAYSRUN_ACTIVE 0
#endif

#if !NRF_CLOCK_HAS_LFCLKCTRL || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether high frequency clock domain is present. */
#define NRF_CLOCK_HAS_HFDOMAIN 1
#else
#define NRF_CLOCK_HAS_HFDOMAIN 0
#endif

#if defined(CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether high frequency clock divide register is present. */
#define NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT 1
#else
#define NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT 0
#endif

#if NRF_CLOCK_HAS_LFCLK_TYPE
#define NRF_CLOCK_LFCLKRUN_STATUS_NotTriggered CLOCK_LFCLK_RUN_STATUS_NotTriggered /**< Task LFCLKSTART/HFCLKSTART has not been triggered definiton. */
#define NRF_CLOCK_LFCLKRUN_STATUS_Triggered    CLOCK_LFCLK_RUN_STATUS_Triggered    /**< Task LFCLKSTART/HFCLKSTART has been triggered. */
#define NRF_CLOCK_INTENSET_LFCLKSTARTED_Msk    CLOCK_INTENSET_LFCLKSTARTED_Msk     /**< Interrupt on LFCLKSTARTED event mask definition. */
#define NRF_LFCLKSRCCOPY                       LFCLK.SRCCOPY                       /**< LF clock SRCCOPY register definition. */
#define NRF_LFCLKRUN                           LFCLK.RUN                           /**< LF clock RUN register definition. */
#define NRF_LFCLKSTAT                          LFCLK.STAT                          /**< LF clock STAT register definition. */
#define NRF_LFCLKSRC                           LFCLK.SRC                           /**< LF clock SRC register definition. */
#define NRF_LFCLKALWAYSRUN                     LFCLK.ALWAYSRUN                     /**< LF clock ALWAYSRUN register definition. */
#define NRF_CLOCK_LFCLKSRCCOPY_SRC_Msk         CLOCK_LFCLK_SRCCOPY_SRC_Msk         /**< LF clock SRCCOPY status mask definition. */
#define NRF_CLOCK_LFCLKSRCCOPY_SRC_Pos         CLOCK_LFCLK_SRCCOPY_SRC_Pos         /**< LF clock SRCCOPY status position definition. */
#define NRF_CLOCK_LFCLKRUN_STATUS_Msk          CLOCK_LFCLK_RUN_STATUS_Msk          /**< LF clock RUN status mask definition. */
#define NRF_CLOCK_LFCLKRUN_STATUS_Pos          CLOCK_LFCLK_RUN_STATUS_Pos          /**< LF clock RUN status position definition. */
#define NRF_CLOCK_LFCLKSTAT_SRC_Msk            CLOCK_LFCLK_STAT_SRC_Msk            /**< LF clock STAT source mask definition. */
#define NRF_CLOCK_LFCLKSTAT_SRC_Pos            CLOCK_LFCLK_STAT_SRC_Pos            /**< LF clock STAT source position definition. */
#define NRF_CLOCK_LFCLKSTAT_STATE_Msk          CLOCK_LFCLK_STAT_STATE_Msk          /**< LF clock STAT state mask definition. */
#define NRF_CLOCK_LFCLKSTAT_STATE_Pos          CLOCK_LFCLK_STAT_STATE_Pos          /**< LF clock STAT state position definition. */
#define NRF_CLOCK_LFCLKSTAT_ALWAYSRUNNING_Msk  CLOCK_LFCLK_STAT_ALWAYSRUNNING_Msk  /**< LF clock STAT alwaysrunning position definition. */
#define NRF_CLOCK_LFCLKSTAT_ALWAYSRUNNING_Pos  CLOCK_LFCLK_STAT_ALWAYSRUNNING_Pos  /**< LF clock STAT alwaysrunning mask definition. */
#define NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Msk CLOCK_LFCLK_ALWAYSRUN_ALWAYSRUN_Msk /**< LF clock ALWAYSRUN position definition. */
#define NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Pos CLOCK_LFCLK_ALWAYSRUN_ALWAYSRUN_Pos /**< LF clock ALWAYSRUN mask definition. */
#elif NRF_CLOCK_HAS_LFCLKCTRL
#define NRF_LFCLKRUN                           LFCLKSTAT.RUN
#define NRF_CLOCK_LFCLKRUN_STATUS_NotTriggered CLOCK_LFCLKSTAT_RUN_STATUS_NotTriggered
#define NRF_CLOCK_LFCLKRUN_STATUS_Triggered    CLOCK_LFCLKSTAT_RUN_STATUS_Triggered
#define NRF_CLOCK_LFCLKRUN_STATUS_Msk          CLOCK_LFCLKSTAT_RUN_STATUS_Msk
#define NRF_CLOCK_LFCLKRUN_STATUS_Pos          CLOCK_LFCLKSTAT_RUN_STATUS_Pos
#define NRF_CLOCK_LFCLKSTAT_STATE_Msk          CLOCK_LFCLKSTAT_STATE_Msk
#define NRF_CLOCK_LFCLKSTAT_STATE_Pos          CLOCK_LFCLKSTAT_STATE_Pos
#define NRF_CLOCK_INTENSET_LFCLKSTARTED_Msk    CLOCK_INTENSET_LFCLKSTARTED_Msk
#define NRF_LFCLKALWAYSRUN                     LFCLKCTRL.ALWAYSRUN
#define NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Msk CLOCK_LFCLKCTRL_ALWAYSRUN_FORCE_Msk
#define NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Pos CLOCK_LFCLKCTRL_ALWAYSRUN_FORCE_Pos
#else
#define NRF_CLOCK_LFCLKRUN_STATUS_NotTriggered CLOCK_LFCLKRUN_STATUS_NotTriggered
#define NRF_CLOCK_LFCLKRUN_STATUS_Triggered    CLOCK_LFCLKRUN_STATUS_Triggered
#define NRF_CLOCK_INTENSET_LFCLKSTARTED_Msk    CLOCK_INTENSET_LFCLKSTARTED_Msk
#define NRF_LFCLKSRCCOPY                       LFCLKSRCCOPY
#define NRF_LFCLKRUN                           LFCLKRUN
#define NRF_LFCLKSTAT                          LFCLKSTAT
#define NRF_LFCLKSRC                           LFCLKSRC
#define NRF_LFCLKALWAYSRUN                     LFCLKALWAYSRUN
#define NRF_CLOCK_LFCLKSRCCOPY_SRC_Msk         CLOCK_LFCLKSRCCOPY_SRC_Msk
#define NRF_CLOCK_LFCLKSRCCOPY_SRC_Pos         CLOCK_LFCLKSRCCOPY_SRC_Pos
#define NRF_CLOCK_LFCLKRUN_STATUS_Msk          CLOCK_LFCLKRUN_STATUS_Msk
#define NRF_CLOCK_LFCLKRUN_STATUS_Pos          CLOCK_LFCLKRUN_STATUS_Pos
#define NRF_CLOCK_LFCLKSTAT_SRC_Msk            CLOCK_LFCLKSTAT_SRC_Msk
#define NRF_CLOCK_LFCLKSTAT_SRC_Pos            CLOCK_LFCLKSTAT_SRC_Pos
#define NRF_CLOCK_LFCLKSTAT_STATE_Msk          CLOCK_LFCLKSTAT_STATE_Msk
#define NRF_CLOCK_LFCLKSTAT_STATE_Pos          CLOCK_LFCLKSTAT_STATE_Pos
#define NRF_CLOCK_LFCLKSTAT_ALWAYSRUNNING_Msk  CLOCK_LFCLKSTAT_ALWAYSRUNNING_Msk
#define NRF_CLOCK_LFCLKSTAT_ALWAYSRUNNING_Pos  CLOCK_LFCLKSTAT_ALWAYSRUNNING_Pos
#define NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Msk CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Msk
#define NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Pos CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Pos
#endif

#if NRF_CLOCK_HAS_XO
#define NRF_CLOCK_INTENSET_HFCLKSTARTED_Msk CLOCK_INTEN_XOSTARTED_Msk /**< HF clock bit mask in interrupt register. */
#define NRF_TASKS_HFCLKSTART                TASKS_XOSTART             /**< Task HF clock start definition. */
#define NRF_TASKS_HFCLKSTOP                 TASKS_XOSTOP              /**< Task HF clock stop definition. */
#define NRF_EVENTS_HFCLKSTARTED             EVENTS_XOSTARTED          /**< Event HF clock startrd definition. */
#define NRF_HFCLKRUN                        XO.RUN                    /**< HF clock RUN register definition. */
#define NRF_HFCLKSTAT                       XO.STAT                   /**< HF clock STAT register definition. */
#define NRF_CLOCK_HFCLKRUN_STATUS_Msk       CLOCK_XO_RUN_STATUS_Msk   /**< HF clock RUN status mask definition. */
#define NRF_CLOCK_HFCLKRUN_STATUS_Pos       CLOCK_XO_RUN_STATUS_Pos   /**< HF clock RUN status position definition. */
#define NRF_CLOCK_HFCLKSTAT_STATE_Msk       CLOCK_XO_STAT_STATE_Msk   /**< HF clock STAT state mask definition. */
#define NRF_CLOCK_HFCLKSTAT_STATE_Pos       CLOCK_XO_STAT_STATE_Pos   /**< HF clock STAT state position definition. */
#elif NRF_CLOCK_HAS_HFDOMAIN
#define NRF_CLOCK_INTENSET_HFCLKSTARTED_Msk CLOCK_INTENSET_HFCLKSTARTED_Msk
#define NRF_TASKS_HFCLKSTART                TASKS_HFCLKSTART
#define NRF_TASKS_HFCLKSTOP                 TASKS_HFCLKSTOP
#define NRF_EVENTS_HFCLKSTARTED             EVENTS_HFCLKSTARTED
#define NRF_HFCLKRUN                        HFCLKRUN
#define NRF_HFCLKSTAT                       HFCLKSTAT
#define NRF_CLOCK_HFCLKRUN_STATUS_Msk       CLOCK_HFCLKRUN_STATUS_Msk
#define NRF_CLOCK_HFCLKRUN_STATUS_Pos       CLOCK_HFCLKRUN_STATUS_Pos
#define NRF_CLOCK_HFCLKSTAT_STATE_Msk       CLOCK_HFCLKSTAT_STATE_Msk
#define NRF_CLOCK_HFCLKSTAT_STATE_Pos       CLOCK_HFCLKSTAT_STATE_Pos
#endif

/**
 * @brief Low-frequency clock sources.
 * @details Used by LFCLKSRC, LFCLKSTAT, and LFCLKSRCCOPY registers.
 */
typedef enum
{
#if NRF_CLOCK_HAS_LFCLKSTAT
    NRF_CLOCK_LFCLK_LPRC       = (CLOCK_LFCLKSTAT_SRC_LFLPRC_Active << CLOCK_LFCLKSTAT_SRC_LFLPRC_Pos), /**< Internal 32 kHz low power RC oscillator. */
#endif
#if defined(CLOCK_LFCLKSRC_SRC_LFULP) || defined(__NRFX_DOXYGEN__)
    NRF_CLOCK_LFCLK_LFULP      = CLOCK_LFCLKSRC_SRC_LFULP,                                              /**< Internal 32 kHz Ultra-low power oscillator. */
#endif

#if defined(CLOCK_LFCLKSRC_SRC_RC) || defined(__NRFX_DOXYGEN__)
    NRF_CLOCK_LFCLK_RC         = CLOCK_LFCLKSRC_SRC_RC,                                                 /**< Internal 32 kHz RC oscillator. */
#elif NRF_CLOCK_HAS_LFCLK_TYPE
    NRF_CLOCK_LFCLK_RC         = CLOCK_LFCLK_SRC_SRC_LFRC,                                              /**< Internal 32 kHz RC oscillator. */
#elif NRF_CLOCK_HAS_LFCLKSTAT
    NRF_CLOCK_LFCLK_RC         = (CLOCK_LFCLKSTAT_SRC_LFRC_Active << CLOCK_LFCLKSTAT_SRC_LFRC_Pos),     /**< Internal 32 kHz RC oscillator. */
#else
    NRF_CLOCK_LFCLK_RC         = CLOCK_LFCLKSRC_SRC_LFRC,                                               /**< Internal 32 kHz RC oscillator. */
#endif

#if NRF_CLOCK_HAS_XTAL
#if defined(CLOCK_LFCLKSRC_SRC_Xtal) || defined(__NRFX_DOXYGEN__)
    NRF_CLOCK_LFCLK_XTAL       = CLOCK_LFCLKSRC_SRC_Xtal,                                               /**< External 32 kHz crystal. */
#elif NRF_CLOCK_HAS_LFCLK_LFXO
    NRF_CLOCK_LFCLK_XTAL       = CLOCK_LFCLK_SRC_SRC_LFXO,                                              /**< External 32 kHz crystal. */
#elif NRF_CLOCK_HAS_LFCLKSTAT
    NRF_CLOCK_LFCLK_XTAL       = (CLOCK_LFCLKSTAT_SRC_LFXO_Active << CLOCK_LFCLKSTAT_SRC_LFXO_Pos),     /**< External 32 kHz crystal. */
#else
    NRF_CLOCK_LFCLK_XTAL       = CLOCK_LFCLKSRC_SRC_LFXO,                                               /**< External 32 kHz crystal. */
#endif
#endif // NRF_CLOCK_HAS_XTAL

#if defined(CLOCK_LFCLKSRC_SRC_Synth) || defined(__NRFX_DOXYGEN__)
    NRF_CLOCK_LFCLK_SYNTH      = CLOCK_LFCLKSRC_SRC_Synth,                                              /**< Internal 32 kHz synthesized from HFCLK system clock. */
#elif NRF_CLOCK_HAS_LFCLK_TYPE
    NRF_CLOCK_LFCLK_SYNTH      = CLOCK_LFCLK_SRC_SRC_LFSYNT,                                            /**< Internal 32 kHz synthesized from HFCLK system clock. */
#elif defined(CLOCK_LFCLKSRC_SRC_LFSYNT)
    NRF_CLOCK_LFCLK_SYNTH      = CLOCK_LFCLKSRC_SRC_LFSYNT,                                             /**< Internal 32 kHz synthesized from HFCLK system clock. */
#elif NRF_CLOCK_HAS_LFCLKSTAT
    NRF_CLOCK_LFCLK_SYNTH      = (CLOCK_LFCLKSTAT_SRC_SYNTH_Active << CLOCK_LFCLKSTAT_SRC_SYNTH_Pos),   /**< Internal 32 kHz synthesized. */
#endif

#if NRF_CLOCK_HAS_LFCLKSTAT
    NRF_CLOCK_LFCLK_LPRC_XTAL  = NRF_CLOCK_LFCLK_LPRC  | NRF_CLOCK_LFCLK_XTAL,                          /**< Internal 32 kHz low power RC oscillator and external 32 kHz crystal. */
    NRF_CLOCK_LFCLK_RC_XTAL    = NRF_CLOCK_LFCLK_RC    | NRF_CLOCK_LFCLK_XTAL,                          /**< Internal 32 kHz RC oscillator and external 32 kHz crystal. */
    NRF_CLOCK_LFCLK_SYNTH_XTAL = NRF_CLOCK_LFCLK_SYNTH | NRF_CLOCK_LFCLK_XTAL,                          /**< Internal 32 kHz synthesized and external 32 kHz crystal. */
#endif

#if defined(NRF_CLOCK_USE_EXTERNAL_LFCLK_SOURCES) || defined(__NRFX_DOXYGEN__)
    /**
     * External 32 kHz low swing signal. Used only with the LFCLKSRC register.
     * For the others @ref NRF_CLOCK_LFCLK_XTAL is returned for this setting.
     */
    NRF_CLOCK_LFCLK_XTAL_LOW_SWING = (CLOCK_LFCLKSRC_SRC_Xtal |
        (CLOCK_LFCLKSRC_EXTERNAL_Enabled << CLOCK_LFCLKSRC_EXTERNAL_Pos)),
    /**
     * External 32 kHz full swing signal. Used only with the LFCLKSRC register.
     * For the others @ref NRF_CLOCK_LFCLK_XTAL is returned for this setting.
     */
    NRF_CLOCK_LFCLK_XTAL_FULL_SWING = (CLOCK_LFCLKSRC_SRC_Xtal |
        (CLOCK_LFCLKSRC_BYPASS_Enabled   << CLOCK_LFCLKSRC_BYPASS_Pos) |
        (CLOCK_LFCLKSRC_EXTERNAL_Enabled << CLOCK_LFCLKSRC_EXTERNAL_Pos)),
#endif // defined(NRF_CLOCK_USE_EXTERNAL_LFCLK_SOURCES) || defined(__NRFX_DOXYGEN__)
} nrf_clock_lfclk_t;

#if NRF_CLOCK_HAS_HFDOMAIN
/**
 * @brief High-frequency clock sources.
 * @details Used by HFCLKSTAT and HFCLK192MSTAT registers.
 */
typedef enum
{
#if defined(CLOCK_HFCLKSTAT_SRC_Xtal) || defined(__NRFX_DOXYGEN__)
    NRF_CLOCK_HFCLK_HIGH_ACCURACY = CLOCK_HFCLKSTAT_SRC_Xtal,  /**< External 16 MHz/32 MHz crystal oscillator. */
#elif NRF_CLOCK_HAS_XO
    NRF_CLOCK_HFCLK_HIGH_ACCURACY,                             /**< External 32 MHz crystal oscillator. */
#else
    NRF_CLOCK_HFCLK_HIGH_ACCURACY = CLOCK_HFCLKSTAT_SRC_HFXO,  /**< External 32 MHz crystal oscillator. */
#endif

#if defined(CLOCK_HFCLKSTAT_SRC_RC) || defined(__NRFX_DOXYGEN__)
    NRF_CLOCK_HFCLK_LOW_ACCURACY  = CLOCK_HFCLKSTAT_SRC_RC,    /**< Internal 16 MHz RC oscillator. */
#elif defined(CLOCK_HFCLKSTAT_SRC_HFINT)
    NRF_CLOCK_HFCLK_LOW_ACCURACY  = CLOCK_HFCLKSTAT_SRC_HFINT, /**< Internal 16 MHz RC oscillator. */
#else
    NRF_CLOCK_HFCLK_LOW_ACCURACY,                              /**< Internal RC oscillator. */
#endif
} nrf_clock_hfclk_t;
#endif

/** @brief Clock domains. */
typedef enum
{
    NRF_CLOCK_DOMAIN_LFCLK,
#if NRF_CLOCK_HAS_HFDOMAIN
    NRF_CLOCK_DOMAIN_HFCLK,
#endif
#if NRF_CLOCK_HAS_HFCLK192M
    NRF_CLOCK_DOMAIN_HFCLK192M,
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
    NRF_CLOCK_DOMAIN_HFCLKAUDIO,
#endif
#if NRF_CLOCK_HAS_HFCLK24M
    NRF_CLOCK_DOMAIN_HFCLK24M,
#endif
} nrf_clock_domain_t;

#if NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT || NRF_CLOCK_HAS_HFCLK192M
/**
 * @brief High-frequency clock frequency configuration.
 * @details Used by HFCLKCTRL and HFCLK192MCTRL registers.
 */
typedef enum
{
    NRF_CLOCK_HFCLK_DIV_1 = CLOCK_HFCLKCTRL_HCLK_Div1,         /**< Divide HFCLK/HFCLK192M by 1 */
    NRF_CLOCK_HFCLK_DIV_2 = CLOCK_HFCLKCTRL_HCLK_Div2,         /**< Divide HFCLK/HFCLK192M by 2 */
#if NRF_CLOCK_HAS_HFCLK192M
    NRF_CLOCK_HFCLK_DIV_4 = CLOCK_HFCLK192MCTRL_HCLK192M_Div4, /**< Divide HFCLK192M by 4 */
#endif
} nrf_clock_hfclk_div_t;
#endif // NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT || NRF_CLOCK_HAS_HFCLK192M

/** @brief Interrupts. */
typedef enum
{
#if NRF_CLOCK_HAS_HFDOMAIN
    NRF_CLOCK_INT_HF_STARTED_MASK       = NRF_CLOCK_INTENSET_HFCLKSTARTED_Msk,  /**< Interrupt on HFCLKSTARTED event. */
#endif
    NRF_CLOCK_INT_LF_STARTED_MASK       = NRF_CLOCK_INTENSET_LFCLKSTARTED_Msk,  /**< Interrupt on LFCLKSTARTED event. */
#if NRF_CLOCK_HAS_LFCLK_SRC_CHANGED
    NRF_CLOCK_INT_LF_SRC_CHANGED_MASK   = CLOCK_INTENSET_LFCLKSRCCHANGED_Msk,   /**< Interrupt on LFCLKCHANGED event. */
#endif
#if NRF_CLOCK_HAS_PLL
    NRF_CLOCK_INT_PLL_STARTED_MASK      = CLOCK_INTENSET_PLLSTARTED_Msk,        /**< Interrupt on PLLSTARTED event. */
#endif
#if NRF_CLOCK_HAS_CALIBRATION
    NRF_CLOCK_INT_DONE_MASK             = CLOCK_INTENSET_DONE_Msk,              /**< Interrupt on DONE event. */
#endif
#if NRF_CLOCK_HAS_CALIBRATION_TIMER
    NRF_CLOCK_INT_CTTO_MASK             = CLOCK_INTENSET_CTTO_Msk,              /**< Interrupt on CTTO event. */
#endif
#if defined(CLOCK_INTENSET_CTSTARTED_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_CLOCK_INT_CTSTARTED_MASK        = CLOCK_INTENSET_CTSTARTED_Msk,         /**< Interrupt on CTSTARTED event. */
    NRF_CLOCK_INT_CTSTOPPED_MASK        = CLOCK_INTENSET_CTSTOPPED_Msk          /**< Interrupt on CTSTOPPED event. */
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
    NRF_CLOCK_INT_HFAUDIO_STARTED_MASK  = CLOCK_INTENSET_HFCLKAUDIOSTARTED_Msk, /**< Interrupt on HFCLKAUDIOSTARTED event. */
#endif
#if NRF_CLOCK_HAS_HFCLK24M
    NRF_CLOCK_INT_HFCLK24M_STARTED_MASK = CLOCK_INTENSET_XO24MSTARTED_Msk,      /**< Interrupt on XO24MSTARTED event. */
#endif
#if NRF_CLOCK_HAS_HFCLK192M
    NRF_CLOCK_INT_HF192M_STARTED_MASK   = CLOCK_INTENSET_HFCLK192MSTARTED_Msk,  /**< Interrupt on HFCLK192MSTARTED event. */
#endif
#if NRF_CLOCK_HAS_XO_TUNE
     NRF_CLOCK_INT_XOTUNED_MASK         = CLOCK_INTENSET_XOTUNED_Msk,           /**< HFXO tuning is done. */
     NRF_CLOCK_INT_XOTUNEERROR_MASK     = CLOCK_INTENSET_XOTUNEERROR_Msk,       /**< HFXO quality issue detected, XOTUNE is needed. */
     NRF_CLOCK_INT_XOTUNEFAILED_MASK    = CLOCK_INTENSET_XOTUNEFAILED_Msk,      /**< HFXO tuning could not be completed. */
#endif
} nrf_clock_int_mask_t;

/**
 * @brief Tasks.
 *
 * @details The NRF_CLOCK_TASK_LFCLKSTOP task cannot be set when the low-frequency clock is not running.
 * The NRF_CLOCK_TASK_HFCLKSTOP task cannot be set when the high-frequency clock is not running.
 */
typedef enum
{
#if NRF_CLOCK_HAS_HFDOMAIN
#if NRF_CLOCK_HAS_XO
    NRF_CLOCK_TASK_HFCLKSTART      = offsetof(NRF_CLOCK_Type, TASKS_XOSTART),         /**< Start HFCLK clock source. */
    NRF_CLOCK_TASK_HFCLKSTOP       = offsetof(NRF_CLOCK_Type, TASKS_XOSTOP),          /**< Stop HFCLK clock source. */
#else
    NRF_CLOCK_TASK_HFCLKSTART      = offsetof(NRF_CLOCK_Type, TASKS_HFCLKSTART),      /**< Start HFCLK clock source. */
    NRF_CLOCK_TASK_HFCLKSTOP       = offsetof(NRF_CLOCK_Type, TASKS_HFCLKSTOP),       /**< Stop HFCLK clock source. */
#endif
#endif
#if NRF_CLOCK_HAS_PLL
    NRF_CLOCK_TASK_PLLSTART        = offsetof(NRF_CLOCK_Type, TASKS_PLLSTART),        /**< Start PLL and keep it running, regardless of the automatic clock requests. */
    NRF_CLOCK_TASK_PLLSTOP         = offsetof(NRF_CLOCK_Type, TASKS_PLLSTOP),         /**< Stop PLL. */
#endif
    NRF_CLOCK_TASK_LFCLKSTART      = offsetof(NRF_CLOCK_Type, TASKS_LFCLKSTART),      /**< Start LFCLK clock source. */
    NRF_CLOCK_TASK_LFCLKSTOP       = offsetof(NRF_CLOCK_Type, TASKS_LFCLKSTOP),       /**< Stop LFCLK clock source. */
#if NRF_CLOCK_HAS_CALIBRATION
    NRF_CLOCK_TASK_CAL             = offsetof(NRF_CLOCK_Type, TASKS_CAL),             /**< Start calibration of LFCLK RC oscillator. */
#endif
#if NRF_CLOCK_HAS_CALIBRATION_TIMER
    NRF_CLOCK_TASK_CTSTART         = offsetof(NRF_CLOCK_Type, TASKS_CTSTART),         /**< Start calibration timer. */
    NRF_CLOCK_TASK_CTSTOP          = offsetof(NRF_CLOCK_Type, TASKS_CTSTOP)           /**< Stop calibration timer. */
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
    NRF_CLOCK_TASK_HFCLKAUDIOSTART = offsetof(NRF_CLOCK_Type, TASKS_HFCLKAUDIOSTART), /**< Start HFCLKAUDIO clock source. */
    NRF_CLOCK_TASK_HFCLKAUDIOSTOP  = offsetof(NRF_CLOCK_Type, TASKS_HFCLKAUDIOSTOP),  /**< Stop HFCLKAUDIO clock source. */
#endif
#if NRF_CLOCK_HAS_HFCLK24M
    NRF_CLOCK_TASK_HFCLK24MSTART   = offsetof(NRF_CLOCK_Type, TASKS_XO24MSTART),      /**< Start HFCLK24M clock source. */
    NRF_CLOCK_TASK_HFCLK24MSTOP    = offsetof(NRF_CLOCK_Type, TASKS_XO24MSTOP),       /**< Stop HFCLK24M clock source. */
#endif
#if NRF_CLOCK_HAS_HFCLK192M
    NRF_CLOCK_TASK_HFCLK192MSTART  = offsetof(NRF_CLOCK_Type, TASKS_HFCLK192MSTART),  /**< Start HFCLK192M clock source. */
    NRF_CLOCK_TASK_HFCLK192MSTOP   = offsetof(NRF_CLOCK_Type, TASKS_HFCLK192MSTOP),   /**< Stop HFCLK192M clock source. */
#endif
#if NRF_CLOCK_HAS_XO_TUNE
    NRF_CLOCK_TASK_XOTUNE          = offsetof(NRF_CLOCK_Type, TASKS_XOTUNE),          /**< Start tuning for HFXO. */
    NRF_CLOCK_TASK_XOTUNEABORT     = offsetof(NRF_CLOCK_Type, TASKS_XOTUNEABORT),     /**< Stop tuning for HFXO. */
#endif
} nrf_clock_task_t;

/** @brief Events. */
typedef enum
{
#if NRF_CLOCK_HAS_HFDOMAIN
#if NRF_CLOCK_HAS_XO
    NRF_CLOCK_EVENT_HFCLKSTARTED      = offsetof(NRF_CLOCK_Type, EVENTS_XOSTARTED),         /**< HFCLK oscillator started. */
#else
    NRF_CLOCK_EVENT_HFCLKSTARTED      = offsetof(NRF_CLOCK_Type, EVENTS_HFCLKSTARTED),      /**< HFCLK oscillator started. */
#endif
#endif
#if NRF_CLOCK_HAS_PLL
    NRF_CLOCK_EVENT_PLLSTARTED        = offsetof(NRF_CLOCK_Type, EVENTS_PLLSTARTED),        /**< PLL started. */
#endif
    NRF_CLOCK_EVENT_LFCLKSTARTED      = offsetof(NRF_CLOCK_Type, EVENTS_LFCLKSTARTED),      /**< LFCLK oscillator started. */
#if NRF_CLOCK_HAS_LFCLK_SRC_CHANGED
    NRF_CLOCK_EVENT_LFCLKSRCCHANGED   = offsetof(NRF_CLOCK_Type, EVENTS_LFCLKSRCCHANGED),   /**< LFCLK source changed */
#endif
#if NRF_CLOCK_HAS_CALIBRATION
    NRF_CLOCK_EVENT_DONE              = offsetof(NRF_CLOCK_Type, EVENTS_DONE),              /**< Calibration of LFCLK RC oscillator completed. */
#endif
#if NRF_CLOCK_HAS_CALIBRATION_TIMER
    NRF_CLOCK_EVENT_CTTO              = offsetof(NRF_CLOCK_Type, EVENTS_CTTO),              /**< Calibration timer time-out. */
#endif
#if defined(CLOCK_INTENSET_CTSTARTED_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_CLOCK_EVENT_CTSTARTED         = offsetof(NRF_CLOCK_Type, EVENTS_CTSTARTED),         /**< Calibration timer started. */
    NRF_CLOCK_EVENT_CTSTOPPED         = offsetof(NRF_CLOCK_Type, EVENTS_CTSTOPPED)          /**< Calibration timer stopped. */
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
    NRF_CLOCK_EVENT_HFCLKAUDIOSTARTED = offsetof(NRF_CLOCK_Type, EVENTS_HFCLKAUDIOSTARTED), /**< HFCLKAUDIO oscillator started. */
#endif
#if NRF_CLOCK_HAS_HFCLK24M
    NRF_CLOCK_EVENT_HFCLK24MSTARTED   = offsetof(NRF_CLOCK_Type, EVENTS_XO24MSTARTED),      /**< HFCLK24M oscillator started. */
#endif
#if NRF_CLOCK_HAS_HFCLK192M
    NRF_CLOCK_EVENT_HFCLK192MSTARTED  = offsetof(NRF_CLOCK_Type, EVENTS_HFCLK192MSTARTED),  /**< HFCLK192M oscillator started. */
#endif
#if NRF_CLOCK_HAS_XO_TUNE
    NRF_CLOCK_EVENT_XOTUNED           = offsetof(NRF_CLOCK_Type, EVENTS_XOTUNED),           /**< HFXO tuning is done. */
    NRF_CLOCK_EVENT_XOTUNEERROR       = offsetof(NRF_CLOCK_Type, EVENTS_XOTUNEERROR),       /**< HFXO quality issue detected, XOTUNE is needed. */
    NRF_CLOCK_EVENT_XOTUNEFAILED      = offsetof(NRF_CLOCK_Type, EVENTS_XOTUNEFAILED),      /**< HFXO tuning could not be completed. */
#endif
} nrf_clock_event_t;

/**
 * @brief Function for enabling the specified interrupt.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_clock_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_clock_int_enable(NRF_CLOCK_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupt.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_clock_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_clock_int_disable(NRF_CLOCK_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_clock_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_clock_int_enable_check(NRF_CLOCK_Type const * p_reg, uint32_t mask);

#if NRF_CLOCK_HAS_INTPEND
/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * @note States of pending interrupt are saved as a bitmask.
 *       One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_clock_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_clock_int_pending_get(NRF_CLOCK_Type const * p_reg);
#endif

/**
 * @brief Function for retrieving the address of the specified task.
 * @details This function can be used by the PPI module.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  CLOCK Task.
 *
 * @return Address of the requested task register.
 */
NRF_STATIC_INLINE uint32_t nrf_clock_task_address_get(NRF_CLOCK_Type const * p_reg,
                                                      nrf_clock_task_t       task);

/**
 * @brief Function for setting the specified task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_clock_task_trigger(NRF_CLOCK_Type * p_reg, nrf_clock_task_t task);

/**
 * @brief Function for retrieving the address of the specified event.
 * @details This function can be used by the PPI module.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event CLOCK Event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_clock_event_address_get(NRF_CLOCK_Type const * p_reg,
                                                       nrf_clock_event_t      event);

/**
 * @brief Function for clearing the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_clock_event_clear(NRF_CLOCK_Type * p_reg, nrf_clock_event_t event);

/**
 * @brief Function for retrieving the state of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_clock_event_check(NRF_CLOCK_Type const * p_reg, nrf_clock_event_t event);

/**
 * @brief Function for retrieving the trigger status of the task START for given domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] domain Clock domain.
 *
 * @retval false The task START for the given domain has not been triggered.
 * @retval true  The task START for the given domain has been triggered.
 */
NRF_STATIC_INLINE bool nrf_clock_start_task_check(NRF_CLOCK_Type const * p_reg,
                                                  nrf_clock_domain_t     domain);

/**
 * @brief Function for retrieving the state of the clock.
 *
 * @param[in]  p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in]  domain    Clock domain.
 * @param[out] p_clk_src Pointer to clock source that is running. Set to NULL if not needed.
 *                       Ignored for HFCLKAUDIO domain. Variable pointed by @p p_clk_src
 *                       must be of either @ref nrf_clock_lfclk_t type for LFCLK
 *                       or @ref nrf_clock_hfclk_t type for HFCLK and HFCLK192M.
 *
 * @retval false The clock is not running.
 * @retval true  The clock is running.
 */
NRF_STATIC_INLINE bool nrf_clock_is_running(NRF_CLOCK_Type const * p_reg,
                                            nrf_clock_domain_t     domain,
                                            void *                 p_clk_src);

/**
 * @brief Function for changing the low-frequency clock source.
 * @details Check in Product Specification if this function can be called when
 *          the low-frequency clock is running.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] source New low-frequency clock source.
 */
NRF_STATIC_INLINE void nrf_clock_lf_src_set(NRF_CLOCK_Type * p_reg, nrf_clock_lfclk_t source);

/**
 * @brief Function for retrieving the selected source for the low-frequency clock.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval NRF_CLOCK_LFCLK_RC    The internal 32 kHz RC oscillator
 *                               is the selected source for the low-frequency clock.
 * @retval NRF_CLOCK_LFCLK_Xtal  An external 32 kHz crystal oscillator
 *                               is the selected source for the low-frequency clock.
 * @retval NRF_CLOCK_LFCLK_Synth The internal 32 kHz synthesized from
 *                               the HFCLK is the selected source for the low-frequency clock.
 */
NRF_STATIC_INLINE nrf_clock_lfclk_t nrf_clock_lf_src_get(NRF_CLOCK_Type const * p_reg);

#if NRF_CLOCK_HAS_SRC_COPY
/**
 * @brief Function for retrieving the clock source for the LFCLK clock when
 *        the task LKCLKSTART is triggered.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval NRF_CLOCK_LFCLK_RC    The internal 32 kHz RC oscillator
 *                               is running and generating the LFCLK clock.
 * @retval NRF_CLOCK_LFCLK_Xtal  An external 32 kHz crystal oscillator
 *                               is running and generating the LFCLK clock.
 * @retval NRF_CLOCK_LFCLK_Synth The internal 32 kHz synthesized from
 *                               the HFCLK is running and generating the LFCLK clock.
 */
NRF_STATIC_INLINE nrf_clock_lfclk_t nrf_clock_lf_srccopy_get(NRF_CLOCK_Type const * p_reg);
#endif

#if NRF_CLOCK_HAS_HFDOMAIN
#if NRF_CLOCK_HAS_HFCLKSRC
/**
 * @brief Function for changing the high-frequency clock source.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] source New high-frequency clock source.
 */
NRF_STATIC_INLINE void nrf_clock_hf_src_set(NRF_CLOCK_Type * p_reg, nrf_clock_hfclk_t source);
#endif

/**
 * @brief Function for retrieving the selected source of the high-frequency clock.
 *
 * For SoCs not featuring the HFCLKSRC register, this is always also the active source
 * of the high-frequency clock.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval NRF_CLOCK_HFCLK_LOW_ACCURACY  The internal RC oscillator is the selected
 *                                       source of the high-frequency clock.
 * @retval NRF_CLOCK_HFCLK_HIGH_ACCURACY An external crystal oscillator is the selected
 *                                       source of the high-frequency clock.
 */
NRF_STATIC_INLINE nrf_clock_hfclk_t nrf_clock_hf_src_get(NRF_CLOCK_Type const * p_reg);

#if NRF_CLOCK_HAS_HFCLKAUDIO
/**
 * @brief Function for changing the Audio clock FREQ_VALUE.
 *
 * The frequency of HFCLKAUDIO ranges from 10.666 MHz to 13.333 MHz in 40.7 Hz steps.
 * To calculate @p freq_value corresponding to the chosen frequency, use the following equation:
 * FREQ_VALUE = 2^16 * ((12 * f_out / 32M) - 4)
 *
 * @warning Chosen frequency must fit in 11.176 MHz - 11.402 MHz or 12.165 MHz - 12.411 MHz frequency bands.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] freq_value New FREQ_VALUE
 */
NRF_STATIC_INLINE
void nrf_clock_hfclkaudio_config_set(NRF_CLOCK_Type * p_reg, uint16_t freq_value);

/**
 * @brief Function for retrieving the Audio clock FREQ_VALUE.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * The frequency of HFCLKAUDIO ranges from 10.666 MHz to 13.333 MHz in 40.7 Hz steps.
 * To calculate frequency corresponding to the returned FREQ_VALUE, use the following equation:
 * f_out = 32M * (4 + FREQ_VALUE * 2^(-16))/12

 * @return FREQ_VALUE of the Audio clock.
 */
NRF_STATIC_INLINE
uint16_t nrf_clock_hfclkaudio_config_get(NRF_CLOCK_Type const * p_reg);
#endif
#endif // NRF_CLOCK_HAS_HFDOMAIN

#if NRF_CLOCK_HAS_CALIBRATION_TIMER
/**
 * @brief Function for changing the calibration timer interval.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] interval New calibration timer interval in 0.25 s resolution
 *                     (range: 0.25 seconds to 31.75 seconds).
 */
NRF_STATIC_INLINE void nrf_clock_cal_timer_timeout_set(NRF_CLOCK_Type * p_reg, uint32_t interval);
#endif

#if NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
/**
 * @brief Function for changing the HFCLK frequency divider.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] divider New HFCLK divider.
 */
NRF_STATIC_INLINE void nrf_clock_hfclk_div_set(NRF_CLOCK_Type *      p_reg,
                                               nrf_clock_hfclk_div_t divider);

/**
 * @brief Function for retrieving the HFCLK frequency divider.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return HFCLK frequency divider.
 */
NRF_STATIC_INLINE nrf_clock_hfclk_div_t nrf_clock_hfclk_div_get(NRF_CLOCK_Type const * p_reg);
#endif

#if NRF_CLOCK_HAS_HFCLK192M
/**
 * @brief Function for changing the HFCLK192M frequency divider.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] divider New HFCLK192M divider.
 */
NRF_STATIC_INLINE void nrf_clock_hfclk192m_div_set(NRF_CLOCK_Type *      p_reg,
                                                   nrf_clock_hfclk_div_t divider);

/**
 * @brief Function for retrieving the HFCLK192M frequency divider.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return HFCLK192M frequency divider.
 */
NRF_STATIC_INLINE nrf_clock_hfclk_div_t nrf_clock_hfclk192m_div_get(NRF_CLOCK_Type const * p_reg);

/**
 * @brief Function for changing the HFCLK192M source.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] source New HFCLK192M source.
 */
NRF_STATIC_INLINE void nrf_clock_hfclk192m_src_set(NRF_CLOCK_Type *  p_reg,
                                                   nrf_clock_hfclk_t source);

/**
 * @brief Function for retrieving the selected source of the HFCLK192M.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval NRF_CLOCK_HFCLK_LOW_ACCURACY  The internal RC oscillator is the selected
 *                                       source of the HFCLK192M.
 * @retval NRF_CLOCK_HFCLK_HIGH_ACCURACY An external crystal oscillator is the selected
 *                                       source of the HFCLK192M.
 */
NRF_STATIC_INLINE nrf_clock_hfclk_t nrf_clock_hfclk192m_src_get(NRF_CLOCK_Type const * p_reg);
#endif // NRF_CLOCK_HAS_HFCLK192M

#if (NRF_CLOCK_HAS_LFCLK_ALWAYSRUN || NRF_CLOCK_HAS_HFCLK_ALWAYSRUN || NRF_CLOCK_HAS_HFCLK192M || \
     NRF_CLOCK_HAS_HFCLKAUDIO || NRF_CLOCK_HAS_LFCLKCTRL)
/**
 * @brief Function for setting the clock domain to always run.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] domain    Clock domain.
 * @param[in] alwaysrun Ensure the clock is always running.
 */
NRF_STATIC_INLINE void nrf_clock_alwaysrun_set(NRF_CLOCK_Type *   p_reg,
                                               nrf_clock_domain_t domain,
                                               bool               alwaysrun);
/**
 * @brief Function for checking if the clock domain is configured to always run.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] domain Clock domain.
 *
 * @retval true  The clock domain is configured to always run.
 * @retval false The clock domain is not configured to always run.
 */
NRF_STATIC_INLINE bool nrf_clock_alwaysrun_get(NRF_CLOCK_Type const * p_reg,
                                               nrf_clock_domain_t     domain);

#if NRF_CLOCK_HAS_ALWAYSRUN_ACTIVE
/**
 * @brief Function for checking if the clock domain always run setting is active.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] domain Clock domain.
 *
 * @retval true  The clock domain always run setting is active.
 * @retval false The clock domain always run setting is not active.
 */
NRF_STATIC_INLINE bool nrf_clock_alwaysrun_active_get(NRF_CLOCK_Type const * p_reg,
                                                      nrf_clock_domain_t     domain);
#endif
#endif /* (NRF_CLOCK_HAS_LFCLK_ALWAYSRUN || NRF_CLOCK_HAS_HFCLK_ALWAYSRUN ||
           NRF_CLOCK_HAS_HFCLK192M || NRF_CLOCK_HAS_HFCLKAUDIO || NRF_CLOCK_HAS_LFCLKCTRL) */

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        CLOCK task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_clock_subscribe_set(NRF_CLOCK_Type * p_reg,
                                               nrf_clock_task_t task,
                                               uint8_t          channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        CLOCK task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_clock_subscribe_clear(NRF_CLOCK_Type * p_reg, nrf_clock_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        CLOCK task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return CLOCK subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_clock_subscribe_get(NRF_CLOCK_Type const * p_reg,
                                                   nrf_clock_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        CLOCK event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_clock_publish_set(NRF_CLOCK_Type *  p_reg,
                                             nrf_clock_event_t event,
                                             uint8_t           channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        CLOCK event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_clock_publish_clear(NRF_CLOCK_Type * p_reg, nrf_clock_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        CLOCK event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return CLOCK publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_clock_publish_get(NRF_CLOCK_Type const * p_reg,
                                                 nrf_clock_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

#if NRF_CLOCK_HAS_CONFIG
/**
 * @brief Function controls reset to the LF CLOCK peripheral.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] enable Enable reset.
 */
NRF_STATIC_INLINE void nrf_clock_config_reset_enable_set(NRF_CLOCK_Type * p_reg, bool enable);

/**
 * @brief Function for enabling bypass.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] source Bypass source.
 * @param[in] enable Enable bypass.
 */
NRF_STATIC_INLINE void nrf_clock_config_bypass_enable_set(NRF_CLOCK_Type *  p_reg,
                                                          nrf_clock_lfclk_t source,
                                                          bool              enable);

/**
 * @brief Function for setting the timeout value for the clock alignment procedure.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] timeout Number of 32Ki clocks.
 */
NRF_STATIC_INLINE void nrf_clock_config_timeout_set(NRF_CLOCK_Type * p_reg,
                                                    uint8_t          timeout);

/**
 * @brief Function for getting the timeout value for the clock alignment procedure.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Number of 32Ki clocks.
 */
NRF_STATIC_INLINE uint8_t nrf_clock_config_timeout_get(NRF_CLOCK_Type const * p_reg);

/**
 * @brief Function for setting threshold limit while clock switching.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] thr_low  Threshold low limit while clock switching.
 * @param[in] thr_high Threshold high limit while clock switching.
 * @param[in] thr_diff Threshold difference while clock switching.
 */
NRF_STATIC_INLINE void nrf_clock_config_threshold_set(NRF_CLOCK_Type * p_reg,
                                                      uint8_t          thr_low,
                                                      uint8_t          thr_high,
                                                      uint8_t          thr_diff);
#endif // NRF_CLOCK_HAS_CONFIG

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_clock_int_enable(NRF_CLOCK_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_clock_int_disable(NRF_CLOCK_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_clock_int_enable_check(NRF_CLOCK_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

#if NRF_CLOCK_HAS_INTPEND
NRF_STATIC_INLINE uint32_t nrf_clock_int_pending_get(NRF_CLOCK_Type const * p_reg)
{
    return p_reg->INTPEND;
}
#endif

NRF_STATIC_INLINE uint32_t nrf_clock_task_address_get(NRF_CLOCK_Type const * p_reg,
                                                      nrf_clock_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_clock_task_trigger(NRF_CLOCK_Type * p_reg, nrf_clock_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_clock_event_address_get(NRF_CLOCK_Type const * p_reg,
                                                       nrf_clock_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_clock_event_clear(NRF_CLOCK_Type * p_reg, nrf_clock_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_clock_event_check(NRF_CLOCK_Type const * p_reg, nrf_clock_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE bool nrf_clock_start_task_check(NRF_CLOCK_Type const * p_reg,
                                                  nrf_clock_domain_t     domain)
{
    switch (domain)
    {
        case NRF_CLOCK_DOMAIN_LFCLK:
            return ((p_reg->NRF_LFCLKRUN & NRF_CLOCK_LFCLKRUN_STATUS_Msk)
                    >> NRF_CLOCK_LFCLKRUN_STATUS_Pos);
#if NRF_CLOCK_HAS_HFDOMAIN
        case NRF_CLOCK_DOMAIN_HFCLK:
            return ((p_reg->NRF_HFCLKRUN & NRF_CLOCK_HFCLKRUN_STATUS_Msk)
                    >> NRF_CLOCK_HFCLKRUN_STATUS_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLK192M
        case NRF_CLOCK_DOMAIN_HFCLK192M:
            return ((p_reg->HFCLK192MRUN & CLOCK_HFCLK192MRUN_STATUS_Msk)
                    >> CLOCK_HFCLK192MRUN_STATUS_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
        case NRF_CLOCK_DOMAIN_HFCLKAUDIO:
            return ((p_reg->HFCLKAUDIORUN & CLOCK_HFCLKAUDIORUN_STATUS_Msk)
                    >> CLOCK_HFCLKAUDIORUN_STATUS_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLK24M
        case NRF_CLOCK_DOMAIN_HFCLK24M:
            return ((p_reg->PLL24M.RUN & CLOCK_PLL24M_RUN_STATUS_Msk)
                    >> CLOCK_PLL24M_RUN_STATUS_Pos);
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

/*
 * `-Warray-bounds` warning is disabled for the `nrf_clock_is_running`
 * function because GCC 12 and above may report a false positive due to the
 * size of the write access to the memory address pointed by the `p_clk_src`
 * argument being variable depending on the value of the `domain` argument.
 */
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

NRF_STATIC_INLINE bool nrf_clock_is_running(NRF_CLOCK_Type const * p_reg,
                                            nrf_clock_domain_t     domain,
                                            void *                 p_clk_src)
{
    bool clock_running = false;
    switch (domain)
    {
        case NRF_CLOCK_DOMAIN_LFCLK:
#if NRF_CLOCK_HAS_LFCLKSTAT
            clock_running = p_reg->LFCLKSTAT.SRC;
            if (p_clk_src != NULL)
            {
                (*(nrf_clock_lfclk_t *)p_clk_src) = (nrf_clock_lfclk_t)(p_reg->LFCLKSTAT.SRC);
            }
            break;
#else
            clock_running = p_reg->NRF_LFCLKSTAT & NRF_CLOCK_LFCLKSTAT_STATE_Msk;
            if (p_clk_src != NULL)
            {
                (*(nrf_clock_lfclk_t *)p_clk_src) =
                    (nrf_clock_lfclk_t)((p_reg->NRF_LFCLKSTAT & NRF_CLOCK_LFCLKSTAT_SRC_Msk)
                                        >> NRF_CLOCK_LFCLKSTAT_SRC_Pos);
            }
            break;
#endif
#if NRF_CLOCK_HAS_HFDOMAIN
        case NRF_CLOCK_DOMAIN_HFCLK:
            clock_running = p_reg->NRF_HFCLKSTAT & NRF_CLOCK_HFCLKSTAT_STATE_Msk;
            if (p_clk_src != NULL)
            {
#if NRF_CLOCK_HAS_XO
                /* XO registers do not contain information about low accuracy source being active.
                 * It has to be derived from HFCLK state. */
                (*(nrf_clock_hfclk_t *)p_clk_src) = clock_running ? NRF_CLOCK_HFCLK_HIGH_ACCURACY :
                                                                    NRF_CLOCK_HFCLK_LOW_ACCURACY;
#else
                (*(nrf_clock_hfclk_t *)p_clk_src) =
                    (nrf_clock_hfclk_t)((p_reg->NRF_HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Msk)
                                        >> CLOCK_HFCLKSTAT_SRC_Pos);
#endif
            }
            break;
#endif
#if NRF_CLOCK_HAS_HFCLK192M
        case NRF_CLOCK_DOMAIN_HFCLK192M:
            clock_running = p_reg->HFCLK192MSTAT & CLOCK_HFCLK192MSTAT_STATE_Msk;
            if (p_clk_src != NULL)
            {
                (*(nrf_clock_hfclk_t *)p_clk_src) =
                    (nrf_clock_hfclk_t)((p_reg->HFCLK192MSTAT & CLOCK_HFCLK192MSTAT_SRC_Msk)
                                        >> CLOCK_HFCLK192MSTAT_SRC_Pos);
            }
            break;
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
        case NRF_CLOCK_DOMAIN_HFCLKAUDIO:
            clock_running = p_reg->HFCLKAUDIOSTAT & CLOCK_HFCLKAUDIOSTAT_STATE_Msk;
            break;
#endif
#if NRF_CLOCK_HAS_HFCLK24M
        case NRF_CLOCK_DOMAIN_HFCLK24M:
            clock_running = p_reg->PLL24M.STAT & CLOCK_PLL24M_STAT_STATE_Msk;
            break;
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
    return clock_running;
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

NRF_STATIC_INLINE void nrf_clock_lf_src_set(NRF_CLOCK_Type * p_reg, nrf_clock_lfclk_t source)
{
#if NRF_CLOCK_HAS_LFCLKCTRL
    p_reg->LFCLKCTRL.SRC = (uint32_t)(source);
#else
    p_reg->NRF_LFCLKSRC = (uint32_t)(source);
#endif
}

NRF_STATIC_INLINE nrf_clock_lfclk_t nrf_clock_lf_src_get(NRF_CLOCK_Type const * p_reg)
{
#if NRF_CLOCK_HAS_LFCLKCTRL
    return (nrf_clock_lfclk_t)(p_reg->LFCLKCTRL.SRC);
#else
    return (nrf_clock_lfclk_t)(p_reg->NRF_LFCLKSRC);
#endif
}

#if NRF_CLOCK_HAS_SRC_COPY
NRF_STATIC_INLINE nrf_clock_lfclk_t nrf_clock_lf_srccopy_get(NRF_CLOCK_Type const * p_reg)
{
    return (nrf_clock_lfclk_t)((p_reg->NRF_LFCLKSRCCOPY & NRF_CLOCK_LFCLKSRCCOPY_SRC_Msk)
                                >> NRF_CLOCK_LFCLKSRCCOPY_SRC_Pos);
}
#endif

#if NRF_CLOCK_HAS_HFDOMAIN
#if NRF_CLOCK_HAS_HFCLKSRC
NRF_STATIC_INLINE void nrf_clock_hf_src_set(NRF_CLOCK_Type * p_reg, nrf_clock_hfclk_t source)
{
    p_reg->HFCLKSRC = (uint32_t)(source);
}
#endif

NRF_STATIC_INLINE nrf_clock_hfclk_t nrf_clock_hf_src_get(NRF_CLOCK_Type const * p_reg)
{
#if NRF_CLOCK_HAS_HFCLKSRC
    return (nrf_clock_hfclk_t)(p_reg->HFCLKSRC);
#elif NRF_CLOCK_HAS_XO
    /* XO registers do not contain information about low accuracy source being active.
     * It has to be derived from HFCLK state. */
    return (p_reg->NRF_HFCLKSTAT & NRF_CLOCK_HFCLKSTAT_STATE_Msk) ? NRF_CLOCK_HFCLK_HIGH_ACCURACY :
                                                                    NRF_CLOCK_HFCLK_LOW_ACCURACY;
#else
    return (nrf_clock_hfclk_t)((p_reg->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Msk)
                                >> CLOCK_HFCLKSTAT_SRC_Pos);
#endif
}
#endif // NRF_CLOCK_HAS_HFDOMAIN

#if NRF_CLOCK_HAS_HFCLKAUDIO
NRF_STATIC_INLINE
void nrf_clock_hfclkaudio_config_set(NRF_CLOCK_Type * p_reg, uint16_t freq_value)
{
    p_reg->HFCLKAUDIO.FREQUENCY = freq_value;
}

NRF_STATIC_INLINE
uint16_t nrf_clock_hfclkaudio_config_get(NRF_CLOCK_Type const * p_reg)
{
    return (uint16_t)(p_reg->HFCLKAUDIO.FREQUENCY);
}
#endif

#if NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
NRF_STATIC_INLINE
void nrf_clock_hfclk_div_set(NRF_CLOCK_Type * p_reg, nrf_clock_hfclk_div_t divider)
{
    p_reg->HFCLKCTRL = (((uint8_t)(divider) << CLOCK_HFCLKCTRL_HCLK_Pos) &
                        CLOCK_HFCLKCTRL_HCLK_Msk);
}

NRF_STATIC_INLINE nrf_clock_hfclk_div_t nrf_clock_hfclk_div_get(NRF_CLOCK_Type const * p_reg)
{
    return (nrf_clock_hfclk_div_t)((p_reg->HFCLKCTRL & CLOCK_HFCLKCTRL_HCLK_Msk)
                                   >> CLOCK_HFCLKCTRL_HCLK_Pos);
}
#endif

#if NRF_CLOCK_HAS_HFCLK192M
NRF_STATIC_INLINE
void nrf_clock_hfclk192m_div_set(NRF_CLOCK_Type * p_reg, nrf_clock_hfclk_div_t divider)
{
    p_reg->HFCLK192MCTRL = (((uint8_t)(divider) << CLOCK_HFCLK192MCTRL_HCLK192M_Pos) &
                            CLOCK_HFCLK192MCTRL_HCLK192M_Msk);
}

NRF_STATIC_INLINE nrf_clock_hfclk_div_t nrf_clock_hfclk192m_div_get(NRF_CLOCK_Type const * p_reg)
{
    return (nrf_clock_hfclk_div_t)((p_reg->HFCLK192MCTRL & CLOCK_HFCLK192MCTRL_HCLK192M_Msk)
                                   >> CLOCK_HFCLK192MCTRL_HCLK192M_Pos);
}

NRF_STATIC_INLINE void nrf_clock_hfclk192m_src_set(NRF_CLOCK_Type * p_reg, nrf_clock_hfclk_t source)
{
    p_reg->HFCLK192MSRC = (uint32_t)(source);
}

NRF_STATIC_INLINE nrf_clock_hfclk_t nrf_clock_hfclk192m_src_get(NRF_CLOCK_Type const * p_reg)
{
    return (nrf_clock_hfclk_t)(p_reg->HFCLK192MSRC);
}
#endif

#if NRF_CLOCK_HAS_CALIBRATION_TIMER
NRF_STATIC_INLINE void nrf_clock_cal_timer_timeout_set(NRF_CLOCK_Type * p_reg, uint32_t interval)
{
    p_reg->CTIV = ((interval << CLOCK_CTIV_CTIV_Pos) & CLOCK_CTIV_CTIV_Msk);
}
#endif

#if (NRF_CLOCK_HAS_LFCLK_ALWAYSRUN || NRF_CLOCK_HAS_HFCLK_ALWAYSRUN || NRF_CLOCK_HAS_HFCLK192M || \
     NRF_CLOCK_HAS_HFCLKAUDIO || NRF_CLOCK_HAS_LFCLKCTRL)
NRF_STATIC_INLINE void nrf_clock_alwaysrun_set(NRF_CLOCK_Type *   p_reg,
                                               nrf_clock_domain_t domain,
                                               bool               alwaysrun)
{
    switch (domain)
    {
#if NRF_CLOCK_HAS_LFCLK_ALWAYSRUN || NRF_CLOCK_HAS_LFCLKCTRL
        case NRF_CLOCK_DOMAIN_LFCLK:
            p_reg->NRF_LFCLKALWAYSRUN =
                ((alwaysrun << NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Pos)
                 & NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Msk);
            break;
#endif
#if NRF_CLOCK_HAS_HFCLK_ALWAYSRUN
        case NRF_CLOCK_DOMAIN_HFCLK:
            p_reg->HFCLKALWAYSRUN =
                ((alwaysrun << CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Pos)
                 & CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Msk);
            break;
#endif
#if NRF_CLOCK_HAS_HFCLK192M
        case NRF_CLOCK_DOMAIN_HFCLK192M:
            p_reg->HFCLK192MALWAYSRUN =
                ((alwaysrun << CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_Pos)
                 & CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_Msk);
            break;
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
        case NRF_CLOCK_DOMAIN_HFCLKAUDIO:
            p_reg->HFCLKAUDIOALWAYSRUN =
                ((alwaysrun << CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_Pos)
                 & CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_Msk);
            break;
#endif
        default:
            NRFX_ASSERT(0);
            break;
    }
}

NRF_STATIC_INLINE bool nrf_clock_alwaysrun_get(NRF_CLOCK_Type const * p_reg,
                                               nrf_clock_domain_t     domain)
{
    switch (domain)
    {
#if NRF_CLOCK_HAS_LFCLK_ALWAYSRUN || NRF_CLOCK_HAS_LFCLKCTRL
        case NRF_CLOCK_DOMAIN_LFCLK:
            return ((p_reg->NRF_LFCLKALWAYSRUN & NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Msk)
                    >> NRF_CLOCK_LFCLKALWAYSRUN_ALWAYSRUN_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLK_ALWAYSRUN
        case NRF_CLOCK_DOMAIN_HFCLK:
            return ((p_reg->HFCLKALWAYSRUN & CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Msk)
                    >> CLOCK_HFCLKALWAYSRUN_ALWAYSRUN_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLK192M
        case NRF_CLOCK_DOMAIN_HFCLK192M:
            return ((p_reg->HFCLK192MALWAYSRUN & CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_Msk)
                    >> CLOCK_HFCLK192MALWAYSRUN_ALWAYSRUN_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
        case NRF_CLOCK_DOMAIN_HFCLKAUDIO:
            return ((p_reg->HFCLKAUDIOALWAYSRUN & CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_Msk)
                    >> CLOCK_HFCLKAUDIOALWAYSRUN_ALWAYSRUN_Pos);
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

#if NRF_CLOCK_HAS_ALWAYSRUN_ACTIVE
NRF_STATIC_INLINE bool nrf_clock_alwaysrun_active_get(NRF_CLOCK_Type const * p_reg,
                                                      nrf_clock_domain_t     domain)
{
    switch (domain)
    {
#if NRF_CLOCK_HAS_LFCLK_ALWAYSRUN
        case NRF_CLOCK_DOMAIN_LFCLK:
            return ((p_reg->NRF_LFCLKSTAT & NRF_CLOCK_LFCLKSTAT_ALWAYSRUNNING_Msk)
                    >> NRF_CLOCK_LFCLKSTAT_ALWAYSRUNNING_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLK_ALWAYSRUN
        case NRF_CLOCK_DOMAIN_HFCLK:
            return ((p_reg->HFCLKSTAT & CLOCK_HFCLKSTAT_ALWAYSRUNNING_Msk)
                    >> CLOCK_HFCLKSTAT_ALWAYSRUNNING_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLK192M
        case NRF_CLOCK_DOMAIN_HFCLK192M:
            return ((p_reg->HFCLK192MSTAT & CLOCK_HFCLK192MSTAT_ALWAYSRUNNING_Msk)
                    >> CLOCK_HFCLK192MSTAT_ALWAYSRUNNING_Pos);
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
        case NRF_CLOCK_DOMAIN_HFCLKAUDIO:
            return ((p_reg->HFCLKAUDIOSTAT & CLOCK_HFCLKAUDIOSTAT_ALWAYSRUNNING_Msk)
                    >> CLOCK_HFCLKAUDIOSTAT_ALWAYSRUNNING_Pos);
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
}
#endif
#endif /* (NRF_CLOCK_HAS_LFCLK_ALWAYSRUN || NRF_CLOCK_HAS_HFCLK_ALWAYSRUN ||
           NRF_CLOCK_HAS_HFCLK192M || NRF_CLOCK_HAS_HFCLKAUDIO || NRF_CLOCK_HAS_LFCLKCTRL ||
           NRF_CLOCK_HAS_HFCLK24M) */

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_clock_subscribe_set(NRF_CLOCK_Type * p_reg,
                                               nrf_clock_task_t task,
                                               uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *)p_reg+ (uint32_t)task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_clock_subscribe_clear(NRF_CLOCK_Type * p_reg, nrf_clock_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *)p_reg + (uint32_t)task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_clock_subscribe_get(NRF_CLOCK_Type const * p_reg,
                                                   nrf_clock_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_clock_publish_set(NRF_CLOCK_Type *  p_reg,
                                             nrf_clock_event_t event,
                                             uint8_t           channel)
{
    *((volatile uint32_t *) ((uint8_t *)p_reg + (uint32_t)event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_clock_publish_clear(NRF_CLOCK_Type * p_reg, nrf_clock_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *)p_reg + (uint32_t)event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_clock_publish_get(NRF_CLOCK_Type const * p_reg,
                                                 nrf_clock_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

#if NRF_CLOCK_HAS_CONFIG
NRF_STATIC_INLINE void nrf_clock_config_reset_enable_set(NRF_CLOCK_Type * p_reg, bool enable)
{
    p_reg->CONFIG.SETUP = (p_reg->CONFIG.SETUP & ~CLOCK_CONFIG_SETUP_RESET_Msk) |
                          (enable ? CLOCK_CONFIG_SETUP_RESET_Enabled
                                  : CLOCK_CONFIG_SETUP_RESET_Disabled)
                                  << CLOCK_CONFIG_SETUP_RESET_Pos;
}

NRF_STATIC_INLINE void nrf_clock_config_bypass_enable_set(NRF_CLOCK_Type *  p_reg,
                                                          nrf_clock_lfclk_t source,
                                                          bool              enable)
{
    switch (source)
    {
        case NRF_CLOCK_LFCLK_LPRC:
            p_reg->CONFIG.SETUP = (p_reg->CONFIG.SETUP & ~CLOCK_CONFIG_SETUP_BYPASSLFLPRC_Msk) |
                                  (enable ? CLOCK_CONFIG_SETUP_BYPASSLFLPRC_Enabled
                                          : CLOCK_CONFIG_SETUP_BYPASSLFLPRC_Disabled)
                                          << CLOCK_CONFIG_SETUP_BYPASSLFLPRC_Pos;
            break;
        case NRF_CLOCK_LFCLK_RC:
            p_reg->CONFIG.SETUP = (p_reg->CONFIG.SETUP & ~CLOCK_CONFIG_SETUP_BYPASSLFRC_Msk) |
                                  (enable ? CLOCK_CONFIG_SETUP_BYPASSLFRC_Enabled
                                          : CLOCK_CONFIG_SETUP_BYPASSLFRC_Disabled)
                                          << CLOCK_CONFIG_SETUP_BYPASSLFRC_Pos;
            break;
        case NRF_CLOCK_LFCLK_XTAL:
            p_reg->CONFIG.SETUP = (p_reg->CONFIG.SETUP & ~CLOCK_CONFIG_SETUP_BYPASSLFXO_Msk) |
                                  (enable ? CLOCK_CONFIG_SETUP_BYPASSLFXO_Enabled
                                          : CLOCK_CONFIG_SETUP_BYPASSLFXO_Disabled)
                                          << CLOCK_CONFIG_SETUP_BYPASSLFXO_Pos;
            break;
        case NRF_CLOCK_LFCLK_SYNTH:
            p_reg->CONFIG.SETUP = (p_reg->CONFIG.SETUP & ~CLOCK_CONFIG_SETUP_BYPASSSYNTH_Msk) |
                                  (enable ? CLOCK_CONFIG_SETUP_BYPASSSYNTH_Enabled
                                          : CLOCK_CONFIG_SETUP_BYPASSSYNTH_Disabled)
                                          << CLOCK_CONFIG_SETUP_BYPASSSYNTH_Pos;
            break;
        default:
            NRFX_ASSERT(0);
            break;
    }
}

NRF_STATIC_INLINE void nrf_clock_config_timeout_set(NRF_CLOCK_Type * p_reg,
                                                    uint8_t          timeout)
{
    p_reg->CONFIG.SETUP = ((p_reg->CONFIG.SETUP & ~CLOCK_CONFIG_SETUP_TIMEOUT_Msk)
                           | ((timeout << CLOCK_CONFIG_SETUP_TIMEOUT_Pos)
                                        & CLOCK_CONFIG_SETUP_TIMEOUT_Msk));
}

NRF_STATIC_INLINE uint8_t nrf_clock_config_timeout_get(NRF_CLOCK_Type const * p_reg)
{
    return (p_reg->CONFIG.SETUP & CLOCK_CONFIG_SETUP_TIMEOUT_Msk) >> CLOCK_CONFIG_SETUP_TIMEOUT_Pos;
}

NRF_STATIC_INLINE void nrf_clock_config_threshold_set(NRF_CLOCK_Type * p_reg,
                                                      uint8_t          thr_low,
                                                      uint8_t          thr_high,
                                                      uint8_t          thr_diff)
{
    p_reg->CONFIG.THRESHOLD = ((((uint32_t)thr_low)  << CLOCK_CONFIG_THRESHOLD_LOW_Pos)
                                                      & CLOCK_CONFIG_THRESHOLD_LOW_Msk) |
                              ((((uint32_t)thr_high) << CLOCK_CONFIG_THRESHOLD_HIGH_Pos)
                                                      & CLOCK_CONFIG_THRESHOLD_HIGH_Msk) |
                              ((((uint32_t)thr_diff) << CLOCK_CONFIG_THRESHOLD_DIFF_Pos)
                                                      & CLOCK_CONFIG_THRESHOLD_DIFF_Msk);
}
#endif // NRF_CLOCK_HAS_CONFIG

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CLOCK_H__
