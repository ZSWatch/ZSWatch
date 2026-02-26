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

#ifndef NRFX_IRQS_NRF54H20_FLPR_H__
#define NRFX_IRQS_NRF54H20_FLPR_H__

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

// VPRTIM_IRQHandler

// GPIOTE130_0_IRQHandler
// GPIOTE130_1_IRQHandler
#define nrfx_gpiote_130_irq_handler     GPIOTE130_IRQHandler

// GRTC_0_IRQHandler
// GRTC_1_IRQHandler
#define nrfx_grtc_irq_handler           GRTC_IRQHandler

// GRTC_2_IRQHandler

// TBM_IRQHandler
#define nrfx_tbm_irq_handler            TBM_IRQHandler

// USBHS_IRQHandler

// EXMIF_IRQHandler

// IPCT120_0_IRQHandler

// I3C120_IRQHandler

// VPR121_IRQHandler

// CAN120_IRQHandler

// MVDMA120_IRQHandler

// I3C121_IRQHandler

// TIMER120_IRQHandler
#define nrfx_timer_120_irq_handler      TIMER120_IRQHandler

// TIMER121_IRQHandler
#define nrfx_timer_121_irq_handler      TIMER121_IRQHandler

// PWM120_IRQHandler
#define nrfx_pwm_120_irq_handler        PWM120_IRQHandler

// SPIS120
#define nrfx_spis_120_irq_handler       SPIS120_IRQHandler

// SPIM120_UARTE120_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_8_ENABLED)
#define nrfx_prs_box_8_irq_handler      SPIM120_UARTE120_IRQHandler
#else
#define nrfx_spim_120_irq_handler       SPIM120_UARTE120_IRQHandler
#define nrfx_uarte_120_irq_handler      SPIM120_UARTE120_IRQHandler
#endif

// SPIM121_IRQHandler
#define nrfx_spim_121_irq_handler       SPIM121_IRQHandler

// VPR130_IRQHandler

// IPCT130_0_IRQHandler

// RTC130_IRQHandler
#define nrfx_rtc_130_irq_handler        RTC130_IRQHandler

// RTC131_IRQHandler
#define nrfx_rtc_131_irq_handler        RTC131_IRQHandler

// WDT131_IRQHandler
#define nrfx_wdt_131_irq_handler        WDT131_IRQHandler

// WDT132_IRQHandler
#define nrfx_wdt_132_irq_handler        WDT132_IRQHandler

// EGU130_IRQHandler
#define nrfx_egu_130_irq_handler        EGU130_IRQHandler

// SAADC_IRQHandler
#define nrfx_saadc_irq_handler          SAADC_IRQHandler

// COMP_LPCOMP_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_9_ENABLED)
#define nrfx_prs_box_9_irq_handler      COMP_LPCOMP_IRQHandler
#else
#define nrfx_comp_irq_handler           COMP_LPCOMP_IRQHandler
#define nrfx_lpcomp_irq_handler         COMP_LPCOMP_IRQHandler
#endif

// TEMP_IRQHandler
#define nrfx_temp_irq_handler           TEMP_IRQHandler

// NFCT_IRQHandler
#define nrfx_nfct_irq_handler           NFCT_IRQHandler

// TDM130_IRQHandler

// PDM_IRQHandler
#define nrfx_pdm_0_irq_handler          PDM_IRQHandler

// QDEC130_IRQHandler
#define nrfx_qdec_130_irq_handler       QDEC130_IRQHandler

// QDEC131_IRQHandler
#define nrfx_qdec_131_irq_handler       QDEC131_IRQHandler

// TDM131_IRQHandler

// TIMER130_IRQHandler
#define nrfx_timer_130_irq_handler      TIMER130_IRQHandler

// TIMER131_IRQHandler
#define nrfx_timer_131_irq_handler      TIMER131_IRQHandler

// PWM130_IRQHandler
#define nrfx_pwm_130_irq_handler        PWM130_IRQHandler

// SERIAL0_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_0_ENABLED)
#define nrfx_prs_box_0_irq_handler      SERIAL0_IRQHandler
#else
#define nrfx_spim_130_irq_handler       SERIAL0_IRQHandler
#define nrfx_spis_130_irq_handler       SERIAL0_IRQHandler
#define nrfx_twim_130_irq_handler       SERIAL0_IRQHandler
#define nrfx_twis_130_irq_handler       SERIAL0_IRQHandler
#define nrfx_uarte_130_irq_handler      SERIAL0_IRQHandler
#endif

// SERIAL1_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_1_ENABLED)
#define nrfx_prs_box_1_irq_handler      SERIAL1_IRQHandler
#else
#define nrfx_spim_131_irq_handler       SERIAL1_IRQHandler
#define nrfx_spis_131_irq_handler       SERIAL1_IRQHandler
#define nrfx_twim_131_irq_handler       SERIAL1_IRQHandler
#define nrfx_twis_131_irq_handler       SERIAL1_IRQHandler
#define nrfx_uarte_131_irq_handler      SERIAL1_IRQHandler
#endif

// TIMER132_IRQHandler
#define nrfx_timer_132_irq_handler      TIMER132_IRQHandler

// TIMER133_IRQHandler
#define nrfx_timer_133_irq_handler      TIMER133_IRQHandler

// PWM131_IRQHandler
#define nrfx_pwm_131_irq_handler        PWM131_IRQHandler

// SERIAL2_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_2_ENABLED)
#define nrfx_prs_box_2_irq_handler      SERIAL2_IRQHandler
#else
#define nrfx_spim_132_irq_handler       SERIAL2_IRQHandler
#define nrfx_spis_132_irq_handler       SERIAL2_IRQHandler
#define nrfx_twim_132_irq_handler       SERIAL2_IRQHandler
#define nrfx_twis_132_irq_handler       SERIAL2_IRQHandler
#define nrfx_uarte_132_irq_handler      SERIAL2_IRQHandler
#endif

// SERIAL3_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_3_ENABLED)
#define nrfx_prs_box_3_irq_handler      SERIAL3_IRQHandler
#else
#define nrfx_spim_133_irq_handler       SERIAL3_IRQHandler
#define nrfx_spis_133_irq_handler       SERIAL3_IRQHandler
#define nrfx_twim_133_irq_handler       SERIAL3_IRQHandler
#define nrfx_twis_133_irq_handler       SERIAL3_IRQHandler
#define nrfx_uarte_133_irq_handler      SERIAL3_IRQHandler
#endif

// TIMER134_IRQHandler
#define nrfx_timer_134_irq_handler      TIMER134_IRQHandler

// TIMER135_IRQHandler
#define nrfx_timer_135_irq_handler      TIMER135_IRQHandler

// PWM132_IRQHandler
#define nrfx_pwm_132_irq_handler        PWM132_IRQHandler

// SERIAL4_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_4_ENABLED)
#define nrfx_prs_box_4_irq_handler      SERIAL4_IRQHandler
#else
#define nrfx_spim_134_irq_handler       SERIAL4_IRQHandler
#define nrfx_spis_134_irq_handler       SERIAL4_IRQHandler
#define nrfx_twim_134_irq_handler       SERIAL4_IRQHandler
#define nrfx_twis_134_irq_handler       SERIAL4_IRQHandler
#define nrfx_uarte_134_irq_handler      SERIAL4_IRQHandler
#endif

// SERIAL5_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_5_ENABLED)
#define nrfx_prs_box_5_irq_handler      SERIAL5_IRQHandler
#else
#define nrfx_spim_135_irq_handler       SERIAL5_IRQHandler
#define nrfx_spis_135_irq_handler       SERIAL5_IRQHandler
#define nrfx_twim_135_irq_handler       SERIAL5_IRQHandler
#define nrfx_twis_135_irq_handler       SERIAL5_IRQHandler
#define nrfx_uarte_135_irq_handler      SERIAL5_IRQHandler
#endif

// TIMER136_IRQHandler
#define nrfx_timer_136_irq_handler      TIMER136_IRQHandler

// TIMER137_IRQHandler
#define nrfx_timer_137_irq_handler      TIMER137_IRQHandler

// PWM133_IRQHandler
#define nrfx_pwm_133_irq_handler        PWM133_IRQHandler

// SERIAL6_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_6_ENABLED)
#define nrfx_prs_box_6_irq_handler      SERIAL6_IRQHandler
#else
#define nrfx_spim_136_irq_handler       SERIAL6_IRQHandler
#define nrfx_spis_136_irq_handler       SERIAL6_IRQHandler
#define nrfx_twim_136_irq_handler       SERIAL6_IRQHandler
#define nrfx_twis_136_irq_handler       SERIAL6_IRQHandler
#define nrfx_uarte_136_irq_handler      SERIAL6_IRQHandler
#endif

// SERIAL7_IRQHandler
#if NRFX_CHECK(NRFX_PRS_ENABLED) && NRFX_CHECK(NRFX_PRS_BOX_7_ENABLED)
#define nrfx_prs_box_7_irq_handler      SERIAL7_IRQHandler
#else
#define nrfx_spim_137_irq_handler       SERIAL7_IRQHandler
#define nrfx_spis_137_irq_handler       SERIAL7_IRQHandler
#define nrfx_twim_137_irq_handler       SERIAL7_IRQHandler
#define nrfx_twis_137_irq_handler       SERIAL7_IRQHandler
#define nrfx_uarte_137_irq_handler      SERIAL7_IRQHandler
#endif

#ifdef __cplusplus
}
#endif

#endif // NRFX_IRQS_NRF54H20_FLPR_H__
