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

#ifndef NRFX_IRQS_NRF54L15_FLPR_H__
#define NRFX_IRQS_NRF54L15_FLPR_H__

#include "drivers/nrfx_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

// VPRCLIC_0_IRQHandler
#define nrfx_vevif_0_irq_handler        VPRCLIC_0_IRQHandler

// VPRCLIC_1_IRQHandler
#define nrfx_vevif_1_irq_handler        VPRCLIC_1_IRQHandler

// VPRCLIC_2_IRQHandler
#define nrfx_vevif_2_irq_handler        VPRCLIC_2_IRQHandler

// VPRCLIC_3_IRQHandler
#define nrfx_vevif_3_irq_handler        VPRCLIC_3_IRQHandler

// VPRCLIC_4_IRQHandler
#define nrfx_vevif_4_irq_handler        VPRCLIC_4_IRQHandler

// VPRCLIC_5_IRQHandler
#define nrfx_vevif_5_irq_handler        VPRCLIC_5_IRQHandler

// VPRCLIC_6_IRQHandler
#define nrfx_vevif_6_irq_handler        VPRCLIC_6_IRQHandler

// VPRCLIC_7_IRQHandler
#define nrfx_vevif_7_irq_handler        VPRCLIC_7_IRQHandler

// VPRCLIC_8_IRQHandler
#define nrfx_vevif_8_irq_handler        VPRCLIC_8_IRQHandler

// VPRCLIC_9_IRQHandler
#define nrfx_vevif_9_irq_handler        VPRCLIC_9_IRQHandler

// VPRCLIC_10_IRQHandler
#define nrfx_vevif_10_irq_handler       VPRCLIC_10_IRQHandler

// VPRCLIC_11_IRQHandler
#define nrfx_vevif_11_irq_handler       VPRCLIC_11_IRQHandler

// VPRCLIC_12_IRQHandler
#define nrfx_vevif_12_irq_handler       VPRCLIC_12_IRQHandler

// VPRCLIC_13_IRQHandler
#define nrfx_vevif_13_irq_handler       VPRCLIC_13_IRQHandler

// VPRCLIC_14_IRQHandler
#define nrfx_vevif_14_irq_handler       VPRCLIC_14_IRQHandler

// VPRCLIC_15_IRQHandler
#define nrfx_vevif_15_irq_handler       VPRCLIC_15_IRQHandler

// VPRCLIC_16_IRQHandler
#define nrfx_vevif_16_irq_handler        VPRCLIC_16_IRQHandler

// VPRCLIC_17_IRQHandler
#define nrfx_vevif_17_irq_handler        VPRCLIC_17_IRQHandler

// VPRCLIC_18_IRQHandler
#define nrfx_vevif_18_irq_handler        VPRCLIC_18_IRQHandler

// VPRCLIC_19_IRQHandler
#define nrfx_vevif_19_irq_handler        VPRCLIC_19_IRQHandler

// VPRCLIC_20_IRQHandler
#define nrfx_vevif_20_irq_handler        VPRCLIC_20_IRQHandler

// VPRCLIC_21_IRQHandler
#define nrfx_vevif_21_irq_handler        VPRCLIC_21_IRQHandler

// VPRCLIC_22_IRQHandler
#define nrfx_vevif_22_irq_handler        VPRCLIC_22_IRQHandler

// VPRCLIC_23_IRQHandler
#define nrfx_vevif_23_irq_handler        VPRCLIC_23_IRQHandler

// VPRCLIC_24_IRQHandler
#define nrfx_vevif_24_irq_handler        VPRCLIC_24_IRQHandler

// VPRCLIC_25_IRQHandler
#define nrfx_vevif_25_irq_handler        VPRCLIC_25_IRQHandler

// VPRCLIC_26_IRQHandler
#define nrfx_vevif_26_irq_handler        VPRCLIC_26_IRQHandler

// VPRCLIC_27_IRQHandler
#define nrfx_vevif_27_irq_handler        VPRCLIC_27_IRQHandler

// VPRCLIC_28_IRQHandler
#define nrfx_vevif_28_irq_handler        VPRCLIC_28_IRQHandler

// VPRCLIC_29_IRQHandler
#define nrfx_vevif_29_irq_handler        VPRCLIC_29_IRQHandler

// VPRCLIC_30_IRQHandler
#define nrfx_vevif_30_irq_handler        VPRCLIC_30_IRQHandler

// VPRCLIC_31_IRQHandler
#define nrfx_vevif_31_irq_handler        VPRCLIC_31_IRQHandler

// SWI00_IRQHandler

// SWI01_IRQHandler

// SWI02_IRQHandler

// SWI03_IRQHandler

// SPU00_IRQHandler

// MPC00_IRQHandler

// AAR00_CCM00_IRQHandler

// ECB00_IRQHandler

// CRACEN_IRQ

// SERIAL00_IRQ
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_0_ENABLED)
#define nrfx_prs_box_0_irq_handler      SERIAL00_IRQHandler
#else
#define nrfx_spim_00_irq_handler        SERIAL00_IRQHandler
#define nrfx_spis_00_irq_handler        SERIAL00_IRQHandler
#define nrfx_twim_00_irq_handler        SERIAL00_IRQHandler
#define nrfx_twis_00_irq_handler        SERIAL00_IRQHandler
#define nrfx_uarte_00_irq_handler       SERIAL00_IRQHandler
#endif

// RRAMC_IRQHandler
#define nrfx_rramc_irq_handler          RRAMC_IRQHandler

// VPR00_IRQHandler

// CTRLAP_IRQHandler

// CM33SS_IRQHandler

// TIMER00_IRQHandler
#define nrfx_timer_00_irq_handler       TIMER00_IRQHandler

// VREGRLDO_IRQHandler

// SPU10_IRQHandler

// TIMER10_IRQHandler
#define nrfx_timer_10_irq_handler       TIMER10_IRQHandler

// EGU10_IRQHandler
#define nrfx_egu_10_irq_handler         EGU10_IRQHandler

// RADIO_0_IRQHandler

// RADIO_1_IRQHandler

// SPU20_IRQHandler

// SERIAL20_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_1_ENABLED)
#define nrfx_prs_box_1_irq_handler      SERIAL20_IRQHandler
#else
#define nrfx_spim_20_irq_handler        SERIAL20_IRQHandler
#define nrfx_spis_20_irq_handler        SERIAL20_IRQHandler
#define nrfx_twim_20_irq_handler        SERIAL20_IRQHandler
#define nrfx_twis_20_irq_handler        SERIAL20_IRQHandler
#define nrfx_uarte_20_irq_handler       SERIAL20_IRQHandler
#endif

// SERIAL21_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_2_ENABLED)
#define nrfx_prs_box_2_irq_handler      SERIAL21_IRQHandler
#else
#define nrfx_spim_21_irq_handler        SERIAL21_IRQHandler
#define nrfx_spis_21_irq_handler        SERIAL21_IRQHandler
#define nrfx_twim_21_irq_handler        SERIAL21_IRQHandler
#define nrfx_twis_21_irq_handler        SERIAL21_IRQHandler
#define nrfx_uarte_21_irq_handler       SERIAL21_IRQHandler
#endif

// SERIAL22_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_3_ENABLED)
#define nrfx_prs_box_3_irq_handler      SERIAL22_IRQHandler
#else
#define nrfx_spim_22_irq_handler        SERIAL22_IRQHandler
#define nrfx_spis_22_irq_handler        SERIAL22_IRQHandler
#define nrfx_twim_22_irq_handler        SERIAL22_IRQHandler
#define nrfx_twis_22_irq_handler        SERIAL22_IRQHandler
#define nrfx_uarte_22_irq_handler       SERIAL22_IRQHandler
#endif

// EGU20_IRQHandler
#define nrfx_egu_20_irq_handler         EGU20_IRQHandler

// TIMER20_IRQHandler
#define nrfx_timer_20_irq_handler       TIMER20_IRQHandler

// TIMER21_IRQHandler
#define nrfx_timer_21_irq_handler       TIMER21_IRQHandler

// TIMER22_IRQHandler
#define nrfx_timer_22_irq_handler       TIMER22_IRQHandler

// TIMER23_IRQHandler
#define nrfx_timer_23_irq_handler       TIMER23_IRQHandler

// TIMER24_IRQHandler
#define nrfx_timer_24_irq_handler       TIMER24_IRQHandler

// PDM20_IRQHandler
#define nrfx_pdm_20_irq_handler         PDM20_IRQHandler

// PDM21_IRQHandler
#define nrfx_pdm_21_irq_handler         PDM21_IRQHandler

// PWM20_IRQHandler
#define nrfx_pwm_20_irq_handler         PWM20_IRQHandler

// PWM21_IRQHandler
#define nrfx_pwm_21_irq_handler         PWM21_IRQHandler

// PWM22_IRQHandler
#define nrfx_pwm_22_irq_handler         PWM22_IRQHandler

// SAADC_IRQHandler
#define nrfx_saadc_irq_handler          SAADC_IRQHandler

// NFCT_IRQHandler
#define nrfx_nfct_irq_handler           NFCT_IRQHandler

// TEMP_IRQHandler
#define nrfx_temp_irq_handler           TEMP_IRQHandler

// GPIOTE20_0_IRQHandler
// GPIOTE20_1_IRQHandler
#define nrfx_gpiote_20_irq_handler      GPIOTE20_IRQHandler

// TAMPC_IRQHandler

// I2S20_IRQHandler
#define nrfx_i2s_20_irq_handler         I2S20_IRQHandler

// QDEC20_IRQHandler
#define nrfx_qdec_20_irq_handler        QDEC20_IRQHandler

// QDEC21_IRQHandler
#define nrfx_qdec_21_irq_handler        QDEC21_IRQHandler

// GRTC_0_IRQHandler
// GRTC_1_IRQHandler
// GRTC_2_IRQHandler
// GRTC_3_IRQHandler
#define nrfx_grtc_irq_handler           GRTC_IRQHandler

// SPU30_IRQHandler

// SERIAL30_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_4_ENABLED)
#define nrfx_prs_box_4_irq_handler      SERIAL30_IRQHandler
#else
#define nrfx_spim_30_irq_handler        SERIAL30_IRQHandler
#define nrfx_spis_30_irq_handler        SERIAL30_IRQHandler
#define nrfx_twim_30_irq_handler        SERIAL30_IRQHandler
#define nrfx_twis_30_irq_handler        SERIAL30_IRQHandler
#define nrfx_uarte_30_irq_handler       SERIAL30_IRQHandler
#endif

// COMP_LPCOMP_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_5_ENABLED)
#define nrfx_prs_box_5_irq_handler      COMP_LPCOMP_IRQHandler
#else
#define nrfx_comp_irq_handler           COMP_LPCOMP_IRQHandler
#define nrfx_lpcomp_irq_handler         COMP_LPCOMP_IRQHandler
#endif

// WDT30_IRQHandler
#define nrfx_wdt_30_irq_handler         WDT30_IRQHandler

// WDT31_IRQHandler
#define nrfx_wdt_31_irq_handler         WDT31_IRQHandler

// GPIOTE30_0_IRQHandler
// GPIOTE30_1_IRQHandler
#define nrfx_gpiote_30_irq_handler      GPIOTE30_IRQHandler

// CLOCK_POWER_IRQHandler
#define nrfx_power_clock_irq_handler    CLOCK_POWER_IRQHandler

#ifdef __cplusplus
}
#endif

#endif // NRFX_IRQS_NRF54L15_FLPR_H___
