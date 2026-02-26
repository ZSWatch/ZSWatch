/*

Copyright (c) 2010 - 2025, Nordic Semiconductor ASA All rights reserved.

SPDX-License-Identifier: BSD-3-Clause

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of Nordic Semiconductor ASA nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef NRF9230_ENGB_GLOBAL_H
#define NRF9230_ENGB_GLOBAL_H

#ifdef __cplusplus
    extern "C" {
#endif


/* ========================================= Start of section using anonymous unions ========================================= */

#include "compiler_abstraction.h"

#if defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
  #pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
  #pragma clang diagnostic ignored "-Wnested-anon-types"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Unsupported compiler type
#endif

/* =========================================================================================================================== */
/* ================                                  Peripheral Address Map                                  ================ */
/* =========================================================================================================================== */

#define NRF_FICR_NS_BASE                  0x0FFFE000UL
#define NRF_USBHSCORE0_NS_BASE            0x2F700000UL
#define NRF_USBHSCORE0_S_BASE             0x2F700000UL
#define NRF_I3CCORE120_NS_BASE            0x2FBE0000UL
#define NRF_I3CCORE121_NS_BASE            0x2FBE1000UL
#define NRF_DMU120_NS_BASE                0x2FBEF800UL
#define NRF_MCAN120_NS_BASE               0x2FBEF800UL
#define NRF_DMU121_NS_BASE                0x2FBF7800UL
#define NRF_MCAN121_NS_BASE               0x2FBF7800UL
#define NRF_STMDATA_NS_BASE               0xA0000000UL
#define NRF_STMDATA_S_BASE                0xA0000000UL
#define NRF_TDDCONF_NS_BASE               0xBF001000UL
#define NRF_TDDCONF_S_BASE                0xBF001000UL
#define NRF_STM_NS_BASE                   0xBF042000UL
#define NRF_TPIU_NS_BASE                  0xBF043000UL
#define NRF_CTI210_NS_BASE                0xBF046000UL
#define NRF_CTI211_NS_BASE                0xBF047000UL
#define NRF_ATBREPLICATOR210_NS_BASE      0xBF048000UL
#define NRF_ATBREPLICATOR211_NS_BASE      0xBF049000UL
#define NRF_ATBREPLICATOR212_NS_BASE      0xBF04A000UL
#define NRF_ATBREPLICATOR213_NS_BASE      0xBF04B000UL
#define NRF_ATBFUNNEL210_NS_BASE          0xBF04C000UL
#define NRF_ATBFUNNEL211_NS_BASE          0xBF04D000UL
#define NRF_ATBFUNNEL212_NS_BASE          0xBF04E000UL
#define NRF_ATBFUNNEL213_NS_BASE          0xBF04F000UL
#define NRF_GPIOTE130_NS_BASE             0x4F934000UL
#define NRF_GPIOTE130_S_BASE              0x5F934000UL
#define NRF_GPIOTE131_NS_BASE             0x4F935000UL
#define NRF_GPIOTE131_S_BASE              0x5F935000UL
#define NRF_GRTC_NS_BASE                  0x4F99C000UL
#define NRF_GRTC_S_BASE                   0x5F99C000UL
#define NRF_TBM_NS_BASE                   0xBF003000UL
#define NRF_TBM_S_BASE                    0xBF003000UL
#define NRF_USBHS_NS_BASE                 0x4F086000UL
#define NRF_USBHS_S_BASE                  0x5F086000UL
#define NRF_EXMIF_NS_BASE                 0x4F095000UL
#define NRF_EXMIF_S_BASE                  0x5F095000UL
#define NRF_SECDOMBELLBOARD_NS_BASE       0x4F099000UL
#define NRF_SECDOMBELLBOARD_S_BASE        0x5F099000UL
#define NRF_CANPLL_NS_BASE                0x4F8C2000UL
#define NRF_CANPLL_S_BASE                 0x5F8C2000UL
#define NRF_VPR120_NS_BASE                0x4F8C8000UL
#define NRF_VPR120_S_BASE                 0x5F8C8000UL
#define NRF_IPCT120_NS_BASE               0x4F8D1000UL
#define NRF_IPCT120_S_BASE                0x5F8D1000UL
#define NRF_MUTEX120_NS_BASE              0x4F8D2000UL
#define NRF_I3C120_NS_BASE                0x4F8D3000UL
#define NRF_I3C120_S_BASE                 0x5F8D3000UL
#define NRF_VPR121_NS_BASE                0x4F8D4000UL
#define NRF_VPR121_S_BASE                 0x5F8D4000UL
#define NRF_CAN120_NS_BASE                0x4F8D8000UL
#define NRF_CAN120_S_BASE                 0x5F8D8000UL
#define NRF_MVDMA120_NS_BASE              0x4F8D9000UL
#define NRF_MVDMA120_S_BASE               0x5F8D9000UL
#define NRF_RAMC122_NS_BASE               0x4F8DA000UL
#define NRF_RAMC122_S_BASE                0x5F8DA000UL
#define NRF_CAN121_NS_BASE                0x4F8DB000UL
#define NRF_CAN121_S_BASE                 0x5F8DB000UL
#define NRF_MVDMA121_NS_BASE              0x4F8DC000UL
#define NRF_MVDMA121_S_BASE               0x5F8DC000UL
#define NRF_RAMC123_NS_BASE               0x4F8DD000UL
#define NRF_RAMC123_S_BASE                0x5F8DD000UL
#define NRF_I3C121_NS_BASE                0x4F8DE000UL
#define NRF_I3C121_S_BASE                 0x5F8DE000UL
#define NRF_DPPIC120_NS_BASE              0x4F8E1000UL
#define NRF_DPPIC120_S_BASE               0x5F8E1000UL
#define NRF_TIMER120_NS_BASE              0x4F8E2000UL
#define NRF_TIMER120_S_BASE               0x5F8E2000UL
#define NRF_TIMER121_NS_BASE              0x4F8E3000UL
#define NRF_TIMER121_S_BASE               0x5F8E3000UL
#define NRF_PWM120_NS_BASE                0x4F8E4000UL
#define NRF_PWM120_S_BASE                 0x5F8E4000UL
#define NRF_SPIS120_NS_BASE               0x4F8E5000UL
#define NRF_SPIS120_S_BASE                0x5F8E5000UL
#define NRF_SPIM120_NS_BASE               0x4F8E6000UL
#define NRF_UARTE120_NS_BASE              0x4F8E6000UL
#define NRF_SPIM120_S_BASE                0x5F8E6000UL
#define NRF_UARTE120_S_BASE               0x5F8E6000UL
#define NRF_SPIM121_NS_BASE               0x4F8E7000UL
#define NRF_SPIM121_S_BASE                0x5F8E7000UL
#define NRF_VPR130_NS_BASE                0x4F908000UL
#define NRF_VPR130_S_BASE                 0x5F908000UL
#define NRF_IPCT130_NS_BASE               0x4F921000UL
#define NRF_IPCT130_S_BASE                0x5F921000UL
#define NRF_DPPIC130_NS_BASE              0x4F922000UL
#define NRF_DPPIC130_S_BASE               0x5F922000UL
#define NRF_MUTEX130_NS_BASE              0x4F927000UL
#define NRF_RTC130_NS_BASE                0x4F928000UL
#define NRF_RTC130_S_BASE                 0x5F928000UL
#define NRF_RTC131_NS_BASE                0x4F929000UL
#define NRF_RTC131_S_BASE                 0x5F929000UL
#define NRF_WDT131_NS_BASE                0x4F92B000UL
#define NRF_WDT131_S_BASE                 0x5F92B000UL
#define NRF_WDT132_NS_BASE                0x4F92C000UL
#define NRF_WDT132_S_BASE                 0x5F92C000UL
#define NRF_EGU130_NS_BASE                0x4F92D000UL
#define NRF_EGU130_S_BASE                 0x5F92D000UL
#define NRF_P0_NS_BASE                    0x4F938000UL
#define NRF_P1_NS_BASE                    0x4F938200UL
#define NRF_P2_NS_BASE                    0x4F938400UL
#define NRF_P6_NS_BASE                    0x4F938C00UL
#define NRF_P0_S_BASE                     0x5F938000UL
#define NRF_P1_S_BASE                     0x5F938200UL
#define NRF_P2_S_BASE                     0x5F938400UL
#define NRF_P6_S_BASE                     0x5F938C00UL
#define NRF_P8_NS_BASE                    0x4F939000UL
#define NRF_P9_NS_BASE                    0x4F939200UL
#define NRF_P10_NS_BASE                   0x4F939400UL
#define NRF_P11_NS_BASE                   0x4F939600UL
#define NRF_P12_NS_BASE                   0x4F939800UL
#define NRF_P13_NS_BASE                   0x4F939A00UL
#define NRF_P8_S_BASE                     0x5F939000UL
#define NRF_P9_S_BASE                     0x5F939200UL
#define NRF_P10_S_BASE                    0x5F939400UL
#define NRF_P11_S_BASE                    0x5F939600UL
#define NRF_P12_S_BASE                    0x5F939800UL
#define NRF_P13_S_BASE                    0x5F939A00UL
#define NRF_DPPIC131_NS_BASE              0x4F981000UL
#define NRF_DPPIC131_S_BASE               0x5F981000UL
#define NRF_SAADC_NS_BASE                 0x4F982000UL
#define NRF_SAADC_S_BASE                  0x5F982000UL
#define NRF_COMP_NS_BASE                  0x4F983000UL
#define NRF_LPCOMP_NS_BASE                0x4F983000UL
#define NRF_COMP_S_BASE                   0x5F983000UL
#define NRF_LPCOMP_S_BASE                 0x5F983000UL
#define NRF_TEMP_NS_BASE                  0x4F984000UL
#define NRF_TEMP_S_BASE                   0x5F984000UL
#define NRF_DPPIC132_NS_BASE              0x4F991000UL
#define NRF_DPPIC132_S_BASE               0x5F991000UL
#define NRF_I2S130_NS_BASE                0x4F992000UL
#define NRF_I2S130_S_BASE                 0x5F992000UL
#define NRF_PDM_NS_BASE                   0x4F993000UL
#define NRF_PDM_S_BASE                    0x5F993000UL
#define NRF_QDEC130_NS_BASE               0x4F994000UL
#define NRF_QDEC130_S_BASE                0x5F994000UL
#define NRF_QDEC131_NS_BASE               0x4F995000UL
#define NRF_QDEC131_S_BASE                0x5F995000UL
#define NRF_I2S131_NS_BASE                0x4F997000UL
#define NRF_I2S131_S_BASE                 0x5F997000UL
#define NRF_DPPIC133_NS_BASE              0x4F9A1000UL
#define NRF_DPPIC133_S_BASE               0x5F9A1000UL
#define NRF_TIMER130_NS_BASE              0x4F9A2000UL
#define NRF_TIMER130_S_BASE               0x5F9A2000UL
#define NRF_TIMER131_NS_BASE              0x4F9A3000UL
#define NRF_TIMER131_S_BASE               0x5F9A3000UL
#define NRF_PWM130_NS_BASE                0x4F9A4000UL
#define NRF_PWM130_S_BASE                 0x5F9A4000UL
#define NRF_SPIM130_NS_BASE               0x4F9A5000UL
#define NRF_SPIS130_NS_BASE               0x4F9A5000UL
#define NRF_TWIM130_NS_BASE               0x4F9A5000UL
#define NRF_TWIS130_NS_BASE               0x4F9A5000UL
#define NRF_UARTE130_NS_BASE              0x4F9A5000UL
#define NRF_SPIM130_S_BASE                0x5F9A5000UL
#define NRF_SPIS130_S_BASE                0x5F9A5000UL
#define NRF_TWIM130_S_BASE                0x5F9A5000UL
#define NRF_TWIS130_S_BASE                0x5F9A5000UL
#define NRF_UARTE130_S_BASE               0x5F9A5000UL
#define NRF_SPIM131_NS_BASE               0x4F9A6000UL
#define NRF_SPIS131_NS_BASE               0x4F9A6000UL
#define NRF_TWIM131_NS_BASE               0x4F9A6000UL
#define NRF_TWIS131_NS_BASE               0x4F9A6000UL
#define NRF_UARTE131_NS_BASE              0x4F9A6000UL
#define NRF_SPIM131_S_BASE                0x5F9A6000UL
#define NRF_SPIS131_S_BASE                0x5F9A6000UL
#define NRF_TWIM131_S_BASE                0x5F9A6000UL
#define NRF_TWIS131_S_BASE                0x5F9A6000UL
#define NRF_UARTE131_S_BASE               0x5F9A6000UL
#define NRF_DPPIC134_NS_BASE              0x4F9B1000UL
#define NRF_DPPIC134_S_BASE               0x5F9B1000UL
#define NRF_TIMER132_NS_BASE              0x4F9B2000UL
#define NRF_TIMER132_S_BASE               0x5F9B2000UL
#define NRF_TIMER133_NS_BASE              0x4F9B3000UL
#define NRF_TIMER133_S_BASE               0x5F9B3000UL
#define NRF_PWM131_NS_BASE                0x4F9B4000UL
#define NRF_PWM131_S_BASE                 0x5F9B4000UL
#define NRF_SPIM132_NS_BASE               0x4F9B5000UL
#define NRF_SPIS132_NS_BASE               0x4F9B5000UL
#define NRF_TWIM132_NS_BASE               0x4F9B5000UL
#define NRF_TWIS132_NS_BASE               0x4F9B5000UL
#define NRF_UARTE132_NS_BASE              0x4F9B5000UL
#define NRF_SPIM132_S_BASE                0x5F9B5000UL
#define NRF_SPIS132_S_BASE                0x5F9B5000UL
#define NRF_TWIM132_S_BASE                0x5F9B5000UL
#define NRF_TWIS132_S_BASE                0x5F9B5000UL
#define NRF_UARTE132_S_BASE               0x5F9B5000UL
#define NRF_SPIM133_NS_BASE               0x4F9B6000UL
#define NRF_SPIS133_NS_BASE               0x4F9B6000UL
#define NRF_TWIM133_NS_BASE               0x4F9B6000UL
#define NRF_TWIS133_NS_BASE               0x4F9B6000UL
#define NRF_UARTE133_NS_BASE              0x4F9B6000UL
#define NRF_SPIM133_S_BASE                0x5F9B6000UL
#define NRF_SPIS133_S_BASE                0x5F9B6000UL
#define NRF_TWIM133_S_BASE                0x5F9B6000UL
#define NRF_TWIS133_S_BASE                0x5F9B6000UL
#define NRF_UARTE133_S_BASE               0x5F9B6000UL
#define NRF_DPPIC135_NS_BASE              0x4F9C1000UL
#define NRF_DPPIC135_S_BASE               0x5F9C1000UL
#define NRF_TIMER134_NS_BASE              0x4F9C2000UL
#define NRF_TIMER134_S_BASE               0x5F9C2000UL
#define NRF_TIMER135_NS_BASE              0x4F9C3000UL
#define NRF_TIMER135_S_BASE               0x5F9C3000UL
#define NRF_PWM132_NS_BASE                0x4F9C4000UL
#define NRF_PWM132_S_BASE                 0x5F9C4000UL
#define NRF_SPIM134_NS_BASE               0x4F9C5000UL
#define NRF_SPIS134_NS_BASE               0x4F9C5000UL
#define NRF_TWIM134_NS_BASE               0x4F9C5000UL
#define NRF_TWIS134_NS_BASE               0x4F9C5000UL
#define NRF_UARTE134_NS_BASE              0x4F9C5000UL
#define NRF_SPIM134_S_BASE                0x5F9C5000UL
#define NRF_SPIS134_S_BASE                0x5F9C5000UL
#define NRF_TWIM134_S_BASE                0x5F9C5000UL
#define NRF_TWIS134_S_BASE                0x5F9C5000UL
#define NRF_UARTE134_S_BASE               0x5F9C5000UL
#define NRF_SPIM135_NS_BASE               0x4F9C6000UL
#define NRF_SPIS135_NS_BASE               0x4F9C6000UL
#define NRF_TWIM135_NS_BASE               0x4F9C6000UL
#define NRF_TWIS135_NS_BASE               0x4F9C6000UL
#define NRF_UARTE135_NS_BASE              0x4F9C6000UL
#define NRF_SPIM135_S_BASE                0x5F9C6000UL
#define NRF_SPIS135_S_BASE                0x5F9C6000UL
#define NRF_TWIM135_S_BASE                0x5F9C6000UL
#define NRF_TWIS135_S_BASE                0x5F9C6000UL
#define NRF_UARTE135_S_BASE               0x5F9C6000UL
#define NRF_DPPIC136_NS_BASE              0x4F9D1000UL
#define NRF_DPPIC136_S_BASE               0x5F9D1000UL
#define NRF_TIMER136_NS_BASE              0x4F9D2000UL
#define NRF_TIMER136_S_BASE               0x5F9D2000UL
#define NRF_TIMER137_NS_BASE              0x4F9D3000UL
#define NRF_TIMER137_S_BASE               0x5F9D3000UL
#define NRF_PWM133_NS_BASE                0x4F9D4000UL
#define NRF_PWM133_S_BASE                 0x5F9D4000UL
#define NRF_SPIM136_NS_BASE               0x4F9D5000UL
#define NRF_SPIS136_NS_BASE               0x4F9D5000UL
#define NRF_TWIM136_NS_BASE               0x4F9D5000UL
#define NRF_TWIS136_NS_BASE               0x4F9D5000UL
#define NRF_UARTE136_NS_BASE              0x4F9D5000UL
#define NRF_SPIM136_S_BASE                0x5F9D5000UL
#define NRF_SPIS136_S_BASE                0x5F9D5000UL
#define NRF_TWIM136_S_BASE                0x5F9D5000UL
#define NRF_TWIS136_S_BASE                0x5F9D5000UL
#define NRF_UARTE136_S_BASE               0x5F9D5000UL
#define NRF_SPIM137_NS_BASE               0x4F9D6000UL
#define NRF_SPIS137_NS_BASE               0x4F9D6000UL
#define NRF_TWIM137_NS_BASE               0x4F9D6000UL
#define NRF_TWIS137_NS_BASE               0x4F9D6000UL
#define NRF_UARTE137_NS_BASE              0x4F9D6000UL
#define NRF_SPIM137_S_BASE                0x5F9D6000UL
#define NRF_SPIS137_S_BASE                0x5F9D6000UL
#define NRF_TWIM137_S_BASE                0x5F9D6000UL
#define NRF_TWIS137_S_BASE                0x5F9D6000UL
#define NRF_UARTE137_S_BASE               0x5F9D6000UL

/* =========================================================================================================================== */
/* ================                                  Peripheral Declaration                                  ================ */
/* =========================================================================================================================== */

#define NRF_FICR_NS                       ((NRF_FICR_Type*)                     NRF_FICR_NS_BASE)
#define NRF_USBHSCORE0_NS                 ((NRF_USBHSCORE_Type*)                NRF_USBHSCORE0_NS_BASE)
#define NRF_USBHSCORE0_S                  ((NRF_USBHSCORE_Type*)                NRF_USBHSCORE0_S_BASE)
#define NRF_I3CCORE120_NS                 ((NRF_I3CCORE_Type*)                  NRF_I3CCORE120_NS_BASE)
#define NRF_I3CCORE121_NS                 ((NRF_I3CCORE_Type*)                  NRF_I3CCORE121_NS_BASE)
#define NRF_DMU120_NS                     ((NRF_DMU_Type*)                      NRF_DMU120_NS_BASE)
#define NRF_MCAN120_NS                    ((NRF_MCAN_Type*)                     NRF_MCAN120_NS_BASE)
#define NRF_DMU121_NS                     ((NRF_DMU_Type*)                      NRF_DMU121_NS_BASE)
#define NRF_MCAN121_NS                    ((NRF_MCAN_Type*)                     NRF_MCAN121_NS_BASE)
#define NRF_STMDATA_NS                    ((NRF_STMDATA_Type*)                  NRF_STMDATA_NS_BASE)
#define NRF_STMDATA_S                     ((NRF_STMDATA_Type*)                  NRF_STMDATA_S_BASE)
#define NRF_TDDCONF_NS                    ((NRF_TDDCONF_Type*)                  NRF_TDDCONF_NS_BASE)
#define NRF_TDDCONF_S                     ((NRF_TDDCONF_Type*)                  NRF_TDDCONF_S_BASE)
#define NRF_STM_NS                        ((NRF_STM_Type*)                      NRF_STM_NS_BASE)
#define NRF_TPIU_NS                       ((NRF_TPIU_Type*)                     NRF_TPIU_NS_BASE)
#define NRF_CTI210_NS                     ((NRF_CTI_Type*)                      NRF_CTI210_NS_BASE)
#define NRF_CTI211_NS                     ((NRF_CTI_Type*)                      NRF_CTI211_NS_BASE)
#define NRF_ATBREPLICATOR210_NS           ((NRF_ATBREPLICATOR_Type*)            NRF_ATBREPLICATOR210_NS_BASE)
#define NRF_ATBREPLICATOR211_NS           ((NRF_ATBREPLICATOR_Type*)            NRF_ATBREPLICATOR211_NS_BASE)
#define NRF_ATBREPLICATOR212_NS           ((NRF_ATBREPLICATOR_Type*)            NRF_ATBREPLICATOR212_NS_BASE)
#define NRF_ATBREPLICATOR213_NS           ((NRF_ATBREPLICATOR_Type*)            NRF_ATBREPLICATOR213_NS_BASE)
#define NRF_ATBFUNNEL210_NS               ((NRF_ATBFUNNEL_Type*)                NRF_ATBFUNNEL210_NS_BASE)
#define NRF_ATBFUNNEL211_NS               ((NRF_ATBFUNNEL_Type*)                NRF_ATBFUNNEL211_NS_BASE)
#define NRF_ATBFUNNEL212_NS               ((NRF_ATBFUNNEL_Type*)                NRF_ATBFUNNEL212_NS_BASE)
#define NRF_ATBFUNNEL213_NS               ((NRF_ATBFUNNEL_Type*)                NRF_ATBFUNNEL213_NS_BASE)
#define NRF_GPIOTE130_NS                  ((NRF_GPIOTE_Type*)                   NRF_GPIOTE130_NS_BASE)
#define NRF_GPIOTE130_S                   ((NRF_GPIOTE_Type*)                   NRF_GPIOTE130_S_BASE)
#define NRF_GPIOTE131_NS                  ((NRF_GPIOTE_Type*)                   NRF_GPIOTE131_NS_BASE)
#define NRF_GPIOTE131_S                   ((NRF_GPIOTE_Type*)                   NRF_GPIOTE131_S_BASE)
#define NRF_GRTC_NS                       ((NRF_GRTC_Type*)                     NRF_GRTC_NS_BASE)
#define NRF_GRTC_S                        ((NRF_GRTC_Type*)                     NRF_GRTC_S_BASE)
#define NRF_TBM_NS                        ((NRF_TBM_Type*)                      NRF_TBM_NS_BASE)
#define NRF_TBM_S                         ((NRF_TBM_Type*)                      NRF_TBM_S_BASE)
#define NRF_USBHS_NS                      ((NRF_USBHS_Type*)                    NRF_USBHS_NS_BASE)
#define NRF_USBHS_S                       ((NRF_USBHS_Type*)                    NRF_USBHS_S_BASE)
#define NRF_EXMIF_NS                      ((NRF_EXMIF_Type*)                    NRF_EXMIF_NS_BASE)
#define NRF_EXMIF_S                       ((NRF_EXMIF_Type*)                    NRF_EXMIF_S_BASE)
#define NRF_SECDOMBELLBOARD_NS            ((NRF_BELLBOARDPUBLIC_Type*)          NRF_SECDOMBELLBOARD_NS_BASE)
#define NRF_SECDOMBELLBOARD_S             ((NRF_BELLBOARDPUBLIC_Type*)          NRF_SECDOMBELLBOARD_S_BASE)
#define NRF_CANPLL_NS                     ((NRF_AUXPLL_Type*)                   NRF_CANPLL_NS_BASE)
#define NRF_CANPLL_S                      ((NRF_AUXPLL_Type*)                   NRF_CANPLL_S_BASE)
#define NRF_VPR120_NS                     ((NRF_VPRPUBLIC_Type*)                NRF_VPR120_NS_BASE)
#define NRF_VPR120_S                      ((NRF_VPRPUBLIC_Type*)                NRF_VPR120_S_BASE)
#define NRF_IPCT120_NS                    ((NRF_IPCT_Type*)                     NRF_IPCT120_NS_BASE)
#define NRF_IPCT120_S                     ((NRF_IPCT_Type*)                     NRF_IPCT120_S_BASE)
#define NRF_MUTEX120_NS                   ((NRF_MUTEX_Type*)                    NRF_MUTEX120_NS_BASE)
#define NRF_I3C120_NS                     ((NRF_I3C_Type*)                      NRF_I3C120_NS_BASE)
#define NRF_I3C120_S                      ((NRF_I3C_Type*)                      NRF_I3C120_S_BASE)
#define NRF_VPR121_NS                     ((NRF_VPR_Type*)                      NRF_VPR121_NS_BASE)
#define NRF_VPR121_S                      ((NRF_VPR_Type*)                      NRF_VPR121_S_BASE)
#define NRF_CAN120_NS                     ((NRF_CAN_Type*)                      NRF_CAN120_NS_BASE)
#define NRF_CAN120_S                      ((NRF_CAN_Type*)                      NRF_CAN120_S_BASE)
#define NRF_MVDMA120_NS                   ((NRF_MVDMA_Type*)                    NRF_MVDMA120_NS_BASE)
#define NRF_MVDMA120_S                    ((NRF_MVDMA_Type*)                    NRF_MVDMA120_S_BASE)
#define NRF_RAMC122_NS                    ((NRF_RAMC_Type*)                     NRF_RAMC122_NS_BASE)
#define NRF_RAMC122_S                     ((NRF_RAMC_Type*)                     NRF_RAMC122_S_BASE)
#define NRF_CAN121_NS                     ((NRF_CAN_Type*)                      NRF_CAN121_NS_BASE)
#define NRF_CAN121_S                      ((NRF_CAN_Type*)                      NRF_CAN121_S_BASE)
#define NRF_MVDMA121_NS                   ((NRF_MVDMA_Type*)                    NRF_MVDMA121_NS_BASE)
#define NRF_MVDMA121_S                    ((NRF_MVDMA_Type*)                    NRF_MVDMA121_S_BASE)
#define NRF_RAMC123_NS                    ((NRF_RAMC_Type*)                     NRF_RAMC123_NS_BASE)
#define NRF_RAMC123_S                     ((NRF_RAMC_Type*)                     NRF_RAMC123_S_BASE)
#define NRF_I3C121_NS                     ((NRF_I3C_Type*)                      NRF_I3C121_NS_BASE)
#define NRF_I3C121_S                      ((NRF_I3C_Type*)                      NRF_I3C121_S_BASE)
#define NRF_DPPIC120_NS                   ((NRF_DPPIC_Type*)                    NRF_DPPIC120_NS_BASE)
#define NRF_DPPIC120_S                    ((NRF_DPPIC_Type*)                    NRF_DPPIC120_S_BASE)
#define NRF_TIMER120_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER120_NS_BASE)
#define NRF_TIMER120_S                    ((NRF_TIMER_Type*)                    NRF_TIMER120_S_BASE)
#define NRF_TIMER121_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER121_NS_BASE)
#define NRF_TIMER121_S                    ((NRF_TIMER_Type*)                    NRF_TIMER121_S_BASE)
#define NRF_PWM120_NS                     ((NRF_PWM_Type*)                      NRF_PWM120_NS_BASE)
#define NRF_PWM120_S                      ((NRF_PWM_Type*)                      NRF_PWM120_S_BASE)
#define NRF_SPIS120_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS120_NS_BASE)
#define NRF_SPIS120_S                     ((NRF_SPIS_Type*)                     NRF_SPIS120_S_BASE)
#define NRF_SPIM120_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM120_NS_BASE)
#define NRF_UARTE120_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE120_NS_BASE)
#define NRF_SPIM120_S                     ((NRF_SPIM_Type*)                     NRF_SPIM120_S_BASE)
#define NRF_UARTE120_S                    ((NRF_UARTE_Type*)                    NRF_UARTE120_S_BASE)
#define NRF_SPIM121_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM121_NS_BASE)
#define NRF_SPIM121_S                     ((NRF_SPIM_Type*)                     NRF_SPIM121_S_BASE)
#define NRF_VPR130_NS                     ((NRF_VPR_Type*)                      NRF_VPR130_NS_BASE)
#define NRF_VPR130_S                      ((NRF_VPR_Type*)                      NRF_VPR130_S_BASE)
#define NRF_IPCT130_NS                    ((NRF_IPCT_Type*)                     NRF_IPCT130_NS_BASE)
#define NRF_IPCT130_S                     ((NRF_IPCT_Type*)                     NRF_IPCT130_S_BASE)
#define NRF_DPPIC130_NS                   ((NRF_DPPIC_Type*)                    NRF_DPPIC130_NS_BASE)
#define NRF_DPPIC130_S                    ((NRF_DPPIC_Type*)                    NRF_DPPIC130_S_BASE)
#define NRF_MUTEX130_NS                   ((NRF_MUTEX_Type*)                    NRF_MUTEX130_NS_BASE)
#define NRF_RTC130_NS                     ((NRF_RTC_Type*)                      NRF_RTC130_NS_BASE)
#define NRF_RTC130_S                      ((NRF_RTC_Type*)                      NRF_RTC130_S_BASE)
#define NRF_RTC131_NS                     ((NRF_RTC_Type*)                      NRF_RTC131_NS_BASE)
#define NRF_RTC131_S                      ((NRF_RTC_Type*)                      NRF_RTC131_S_BASE)
#define NRF_WDT131_NS                     ((NRF_WDT_Type*)                      NRF_WDT131_NS_BASE)
#define NRF_WDT131_S                      ((NRF_WDT_Type*)                      NRF_WDT131_S_BASE)
#define NRF_WDT132_NS                     ((NRF_WDT_Type*)                      NRF_WDT132_NS_BASE)
#define NRF_WDT132_S                      ((NRF_WDT_Type*)                      NRF_WDT132_S_BASE)
#define NRF_EGU130_NS                     ((NRF_EGU_Type*)                      NRF_EGU130_NS_BASE)
#define NRF_EGU130_S                      ((NRF_EGU_Type*)                      NRF_EGU130_S_BASE)
#define NRF_P0_NS                         ((NRF_GPIO_Type*)                     NRF_P0_NS_BASE)
#define NRF_P1_NS                         ((NRF_GPIO_Type*)                     NRF_P1_NS_BASE)
#define NRF_P2_NS                         ((NRF_GPIO_Type*)                     NRF_P2_NS_BASE)
#define NRF_P6_NS                         ((NRF_GPIO_Type*)                     NRF_P6_NS_BASE)
#define NRF_P0_S                          ((NRF_GPIO_Type*)                     NRF_P0_S_BASE)
#define NRF_P1_S                          ((NRF_GPIO_Type*)                     NRF_P1_S_BASE)
#define NRF_P2_S                          ((NRF_GPIO_Type*)                     NRF_P2_S_BASE)
#define NRF_P6_S                          ((NRF_GPIO_Type*)                     NRF_P6_S_BASE)
#define NRF_P8_NS                         ((NRF_GPIO_Type*)                     NRF_P8_NS_BASE)
#define NRF_P9_NS                         ((NRF_GPIO_Type*)                     NRF_P9_NS_BASE)
#define NRF_P10_NS                        ((NRF_GPIO_Type*)                     NRF_P10_NS_BASE)
#define NRF_P11_NS                        ((NRF_GPIO_Type*)                     NRF_P11_NS_BASE)
#define NRF_P12_NS                        ((NRF_GPIO_Type*)                     NRF_P12_NS_BASE)
#define NRF_P13_NS                        ((NRF_GPIO_Type*)                     NRF_P13_NS_BASE)
#define NRF_P8_S                          ((NRF_GPIO_Type*)                     NRF_P8_S_BASE)
#define NRF_P9_S                          ((NRF_GPIO_Type*)                     NRF_P9_S_BASE)
#define NRF_P10_S                         ((NRF_GPIO_Type*)                     NRF_P10_S_BASE)
#define NRF_P11_S                         ((NRF_GPIO_Type*)                     NRF_P11_S_BASE)
#define NRF_P12_S                         ((NRF_GPIO_Type*)                     NRF_P12_S_BASE)
#define NRF_P13_S                         ((NRF_GPIO_Type*)                     NRF_P13_S_BASE)
#define NRF_DPPIC131_NS                   ((NRF_DPPIC_Type*)                    NRF_DPPIC131_NS_BASE)
#define NRF_DPPIC131_S                    ((NRF_DPPIC_Type*)                    NRF_DPPIC131_S_BASE)
#define NRF_SAADC_NS                      ((NRF_SAADC_Type*)                    NRF_SAADC_NS_BASE)
#define NRF_SAADC_S                       ((NRF_SAADC_Type*)                    NRF_SAADC_S_BASE)
#define NRF_COMP_NS                       ((NRF_COMP_Type*)                     NRF_COMP_NS_BASE)
#define NRF_LPCOMP_NS                     ((NRF_LPCOMP_Type*)                   NRF_LPCOMP_NS_BASE)
#define NRF_COMP_S                        ((NRF_COMP_Type*)                     NRF_COMP_S_BASE)
#define NRF_LPCOMP_S                      ((NRF_LPCOMP_Type*)                   NRF_LPCOMP_S_BASE)
#define NRF_TEMP_NS                       ((NRF_TEMP_Type*)                     NRF_TEMP_NS_BASE)
#define NRF_TEMP_S                        ((NRF_TEMP_Type*)                     NRF_TEMP_S_BASE)
#define NRF_DPPIC132_NS                   ((NRF_DPPIC_Type*)                    NRF_DPPIC132_NS_BASE)
#define NRF_DPPIC132_S                    ((NRF_DPPIC_Type*)                    NRF_DPPIC132_S_BASE)
#define NRF_I2S130_NS                     ((NRF_I2S_Type*)                      NRF_I2S130_NS_BASE)
#define NRF_I2S130_S                      ((NRF_I2S_Type*)                      NRF_I2S130_S_BASE)
#define NRF_PDM_NS                        ((NRF_PDM_Type*)                      NRF_PDM_NS_BASE)
#define NRF_PDM_S                         ((NRF_PDM_Type*)                      NRF_PDM_S_BASE)
#define NRF_QDEC130_NS                    ((NRF_QDEC_Type*)                     NRF_QDEC130_NS_BASE)
#define NRF_QDEC130_S                     ((NRF_QDEC_Type*)                     NRF_QDEC130_S_BASE)
#define NRF_QDEC131_NS                    ((NRF_QDEC_Type*)                     NRF_QDEC131_NS_BASE)
#define NRF_QDEC131_S                     ((NRF_QDEC_Type*)                     NRF_QDEC131_S_BASE)
#define NRF_I2S131_NS                     ((NRF_I2S_Type*)                      NRF_I2S131_NS_BASE)
#define NRF_I2S131_S                      ((NRF_I2S_Type*)                      NRF_I2S131_S_BASE)
#define NRF_DPPIC133_NS                   ((NRF_DPPIC_Type*)                    NRF_DPPIC133_NS_BASE)
#define NRF_DPPIC133_S                    ((NRF_DPPIC_Type*)                    NRF_DPPIC133_S_BASE)
#define NRF_TIMER130_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER130_NS_BASE)
#define NRF_TIMER130_S                    ((NRF_TIMER_Type*)                    NRF_TIMER130_S_BASE)
#define NRF_TIMER131_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER131_NS_BASE)
#define NRF_TIMER131_S                    ((NRF_TIMER_Type*)                    NRF_TIMER131_S_BASE)
#define NRF_PWM130_NS                     ((NRF_PWM_Type*)                      NRF_PWM130_NS_BASE)
#define NRF_PWM130_S                      ((NRF_PWM_Type*)                      NRF_PWM130_S_BASE)
#define NRF_SPIM130_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM130_NS_BASE)
#define NRF_SPIS130_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS130_NS_BASE)
#define NRF_TWIM130_NS                    ((NRF_TWIM_Type*)                     NRF_TWIM130_NS_BASE)
#define NRF_TWIS130_NS                    ((NRF_TWIS_Type*)                     NRF_TWIS130_NS_BASE)
#define NRF_UARTE130_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE130_NS_BASE)
#define NRF_SPIM130_S                     ((NRF_SPIM_Type*)                     NRF_SPIM130_S_BASE)
#define NRF_SPIS130_S                     ((NRF_SPIS_Type*)                     NRF_SPIS130_S_BASE)
#define NRF_TWIM130_S                     ((NRF_TWIM_Type*)                     NRF_TWIM130_S_BASE)
#define NRF_TWIS130_S                     ((NRF_TWIS_Type*)                     NRF_TWIS130_S_BASE)
#define NRF_UARTE130_S                    ((NRF_UARTE_Type*)                    NRF_UARTE130_S_BASE)
#define NRF_SPIM131_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM131_NS_BASE)
#define NRF_SPIS131_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS131_NS_BASE)
#define NRF_TWIM131_NS                    ((NRF_TWIM_Type*)                     NRF_TWIM131_NS_BASE)
#define NRF_TWIS131_NS                    ((NRF_TWIS_Type*)                     NRF_TWIS131_NS_BASE)
#define NRF_UARTE131_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE131_NS_BASE)
#define NRF_SPIM131_S                     ((NRF_SPIM_Type*)                     NRF_SPIM131_S_BASE)
#define NRF_SPIS131_S                     ((NRF_SPIS_Type*)                     NRF_SPIS131_S_BASE)
#define NRF_TWIM131_S                     ((NRF_TWIM_Type*)                     NRF_TWIM131_S_BASE)
#define NRF_TWIS131_S                     ((NRF_TWIS_Type*)                     NRF_TWIS131_S_BASE)
#define NRF_UARTE131_S                    ((NRF_UARTE_Type*)                    NRF_UARTE131_S_BASE)
#define NRF_DPPIC134_NS                   ((NRF_DPPIC_Type*)                    NRF_DPPIC134_NS_BASE)
#define NRF_DPPIC134_S                    ((NRF_DPPIC_Type*)                    NRF_DPPIC134_S_BASE)
#define NRF_TIMER132_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER132_NS_BASE)
#define NRF_TIMER132_S                    ((NRF_TIMER_Type*)                    NRF_TIMER132_S_BASE)
#define NRF_TIMER133_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER133_NS_BASE)
#define NRF_TIMER133_S                    ((NRF_TIMER_Type*)                    NRF_TIMER133_S_BASE)
#define NRF_PWM131_NS                     ((NRF_PWM_Type*)                      NRF_PWM131_NS_BASE)
#define NRF_PWM131_S                      ((NRF_PWM_Type*)                      NRF_PWM131_S_BASE)
#define NRF_SPIM132_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM132_NS_BASE)
#define NRF_SPIS132_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS132_NS_BASE)
#define NRF_TWIM132_NS                    ((NRF_TWIM_Type*)                     NRF_TWIM132_NS_BASE)
#define NRF_TWIS132_NS                    ((NRF_TWIS_Type*)                     NRF_TWIS132_NS_BASE)
#define NRF_UARTE132_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE132_NS_BASE)
#define NRF_SPIM132_S                     ((NRF_SPIM_Type*)                     NRF_SPIM132_S_BASE)
#define NRF_SPIS132_S                     ((NRF_SPIS_Type*)                     NRF_SPIS132_S_BASE)
#define NRF_TWIM132_S                     ((NRF_TWIM_Type*)                     NRF_TWIM132_S_BASE)
#define NRF_TWIS132_S                     ((NRF_TWIS_Type*)                     NRF_TWIS132_S_BASE)
#define NRF_UARTE132_S                    ((NRF_UARTE_Type*)                    NRF_UARTE132_S_BASE)
#define NRF_SPIM133_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM133_NS_BASE)
#define NRF_SPIS133_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS133_NS_BASE)
#define NRF_TWIM133_NS                    ((NRF_TWIM_Type*)                     NRF_TWIM133_NS_BASE)
#define NRF_TWIS133_NS                    ((NRF_TWIS_Type*)                     NRF_TWIS133_NS_BASE)
#define NRF_UARTE133_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE133_NS_BASE)
#define NRF_SPIM133_S                     ((NRF_SPIM_Type*)                     NRF_SPIM133_S_BASE)
#define NRF_SPIS133_S                     ((NRF_SPIS_Type*)                     NRF_SPIS133_S_BASE)
#define NRF_TWIM133_S                     ((NRF_TWIM_Type*)                     NRF_TWIM133_S_BASE)
#define NRF_TWIS133_S                     ((NRF_TWIS_Type*)                     NRF_TWIS133_S_BASE)
#define NRF_UARTE133_S                    ((NRF_UARTE_Type*)                    NRF_UARTE133_S_BASE)
#define NRF_DPPIC135_NS                   ((NRF_DPPIC_Type*)                    NRF_DPPIC135_NS_BASE)
#define NRF_DPPIC135_S                    ((NRF_DPPIC_Type*)                    NRF_DPPIC135_S_BASE)
#define NRF_TIMER134_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER134_NS_BASE)
#define NRF_TIMER134_S                    ((NRF_TIMER_Type*)                    NRF_TIMER134_S_BASE)
#define NRF_TIMER135_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER135_NS_BASE)
#define NRF_TIMER135_S                    ((NRF_TIMER_Type*)                    NRF_TIMER135_S_BASE)
#define NRF_PWM132_NS                     ((NRF_PWM_Type*)                      NRF_PWM132_NS_BASE)
#define NRF_PWM132_S                      ((NRF_PWM_Type*)                      NRF_PWM132_S_BASE)
#define NRF_SPIM134_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM134_NS_BASE)
#define NRF_SPIS134_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS134_NS_BASE)
#define NRF_TWIM134_NS                    ((NRF_TWIM_Type*)                     NRF_TWIM134_NS_BASE)
#define NRF_TWIS134_NS                    ((NRF_TWIS_Type*)                     NRF_TWIS134_NS_BASE)
#define NRF_UARTE134_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE134_NS_BASE)
#define NRF_SPIM134_S                     ((NRF_SPIM_Type*)                     NRF_SPIM134_S_BASE)
#define NRF_SPIS134_S                     ((NRF_SPIS_Type*)                     NRF_SPIS134_S_BASE)
#define NRF_TWIM134_S                     ((NRF_TWIM_Type*)                     NRF_TWIM134_S_BASE)
#define NRF_TWIS134_S                     ((NRF_TWIS_Type*)                     NRF_TWIS134_S_BASE)
#define NRF_UARTE134_S                    ((NRF_UARTE_Type*)                    NRF_UARTE134_S_BASE)
#define NRF_SPIM135_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM135_NS_BASE)
#define NRF_SPIS135_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS135_NS_BASE)
#define NRF_TWIM135_NS                    ((NRF_TWIM_Type*)                     NRF_TWIM135_NS_BASE)
#define NRF_TWIS135_NS                    ((NRF_TWIS_Type*)                     NRF_TWIS135_NS_BASE)
#define NRF_UARTE135_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE135_NS_BASE)
#define NRF_SPIM135_S                     ((NRF_SPIM_Type*)                     NRF_SPIM135_S_BASE)
#define NRF_SPIS135_S                     ((NRF_SPIS_Type*)                     NRF_SPIS135_S_BASE)
#define NRF_TWIM135_S                     ((NRF_TWIM_Type*)                     NRF_TWIM135_S_BASE)
#define NRF_TWIS135_S                     ((NRF_TWIS_Type*)                     NRF_TWIS135_S_BASE)
#define NRF_UARTE135_S                    ((NRF_UARTE_Type*)                    NRF_UARTE135_S_BASE)
#define NRF_DPPIC136_NS                   ((NRF_DPPIC_Type*)                    NRF_DPPIC136_NS_BASE)
#define NRF_DPPIC136_S                    ((NRF_DPPIC_Type*)                    NRF_DPPIC136_S_BASE)
#define NRF_TIMER136_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER136_NS_BASE)
#define NRF_TIMER136_S                    ((NRF_TIMER_Type*)                    NRF_TIMER136_S_BASE)
#define NRF_TIMER137_NS                   ((NRF_TIMER_Type*)                    NRF_TIMER137_NS_BASE)
#define NRF_TIMER137_S                    ((NRF_TIMER_Type*)                    NRF_TIMER137_S_BASE)
#define NRF_PWM133_NS                     ((NRF_PWM_Type*)                      NRF_PWM133_NS_BASE)
#define NRF_PWM133_S                      ((NRF_PWM_Type*)                      NRF_PWM133_S_BASE)
#define NRF_SPIM136_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM136_NS_BASE)
#define NRF_SPIS136_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS136_NS_BASE)
#define NRF_TWIM136_NS                    ((NRF_TWIM_Type*)                     NRF_TWIM136_NS_BASE)
#define NRF_TWIS136_NS                    ((NRF_TWIS_Type*)                     NRF_TWIS136_NS_BASE)
#define NRF_UARTE136_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE136_NS_BASE)
#define NRF_SPIM136_S                     ((NRF_SPIM_Type*)                     NRF_SPIM136_S_BASE)
#define NRF_SPIS136_S                     ((NRF_SPIS_Type*)                     NRF_SPIS136_S_BASE)
#define NRF_TWIM136_S                     ((NRF_TWIM_Type*)                     NRF_TWIM136_S_BASE)
#define NRF_TWIS136_S                     ((NRF_TWIS_Type*)                     NRF_TWIS136_S_BASE)
#define NRF_UARTE136_S                    ((NRF_UARTE_Type*)                    NRF_UARTE136_S_BASE)
#define NRF_SPIM137_NS                    ((NRF_SPIM_Type*)                     NRF_SPIM137_NS_BASE)
#define NRF_SPIS137_NS                    ((NRF_SPIS_Type*)                     NRF_SPIS137_NS_BASE)
#define NRF_TWIM137_NS                    ((NRF_TWIM_Type*)                     NRF_TWIM137_NS_BASE)
#define NRF_TWIS137_NS                    ((NRF_TWIS_Type*)                     NRF_TWIS137_NS_BASE)
#define NRF_UARTE137_NS                   ((NRF_UARTE_Type*)                    NRF_UARTE137_NS_BASE)
#define NRF_SPIM137_S                     ((NRF_SPIM_Type*)                     NRF_SPIM137_S_BASE)
#define NRF_SPIS137_S                     ((NRF_SPIS_Type*)                     NRF_SPIS137_S_BASE)
#define NRF_TWIM137_S                     ((NRF_TWIM_Type*)                     NRF_TWIM137_S_BASE)
#define NRF_TWIS137_S                     ((NRF_TWIS_Type*)                     NRF_TWIS137_S_BASE)
#define NRF_UARTE137_S                    ((NRF_UARTE_Type*)                    NRF_UARTE137_S_BASE)

/* =========================================================================================================================== */
/* ================                                    TrustZone Remapping                                    ================ */
/* =========================================================================================================================== */

#ifdef NRF_TRUSTZONE_NONSECURE                       /*!< Remap NRF_X_NS instances to NRF_X symbol for ease of use.            */
  #define NRF_FICR                                NRF_FICR_NS
  #define NRF_USBHSCORE0                          NRF_USBHSCORE0_NS
  #define NRF_I3CCORE120                          NRF_I3CCORE120_NS
  #define NRF_I3CCORE121                          NRF_I3CCORE121_NS
  #define NRF_DMU120                              NRF_DMU120_NS
  #define NRF_MCAN120                             NRF_MCAN120_NS
  #define NRF_DMU121                              NRF_DMU121_NS
  #define NRF_MCAN121                             NRF_MCAN121_NS
  #define NRF_STMDATA                             NRF_STMDATA_NS
  #define NRF_TDDCONF                             NRF_TDDCONF_NS
  #define NRF_STM                                 NRF_STM_NS
  #define NRF_TPIU                                NRF_TPIU_NS
  #define NRF_CTI210                              NRF_CTI210_NS
  #define NRF_CTI211                              NRF_CTI211_NS
  #define NRF_ATBREPLICATOR210                    NRF_ATBREPLICATOR210_NS
  #define NRF_ATBREPLICATOR211                    NRF_ATBREPLICATOR211_NS
  #define NRF_ATBREPLICATOR212                    NRF_ATBREPLICATOR212_NS
  #define NRF_ATBREPLICATOR213                    NRF_ATBREPLICATOR213_NS
  #define NRF_ATBFUNNEL210                        NRF_ATBFUNNEL210_NS
  #define NRF_ATBFUNNEL211                        NRF_ATBFUNNEL211_NS
  #define NRF_ATBFUNNEL212                        NRF_ATBFUNNEL212_NS
  #define NRF_ATBFUNNEL213                        NRF_ATBFUNNEL213_NS
  #define NRF_GPIOTE130                           NRF_GPIOTE130_NS
  #define NRF_GPIOTE131                           NRF_GPIOTE131_NS
  #define NRF_GRTC                                NRF_GRTC_NS
  #define NRF_TBM                                 NRF_TBM_NS
  #define NRF_USBHS                               NRF_USBHS_NS
  #define NRF_EXMIF                               NRF_EXMIF_NS
  #define NRF_SECDOMBELLBOARD                     NRF_SECDOMBELLBOARD_NS
  #define NRF_CANPLL                              NRF_CANPLL_NS
  #define NRF_VPR120                              NRF_VPR120_NS
  #define NRF_IPCT120                             NRF_IPCT120_NS
  #define NRF_MUTEX120                            NRF_MUTEX120_NS
  #define NRF_I3C120                              NRF_I3C120_NS
  #define NRF_VPR121                              NRF_VPR121_NS
  #define NRF_CAN120                              NRF_CAN120_NS
  #define NRF_MVDMA120                            NRF_MVDMA120_NS
  #define NRF_RAMC122                             NRF_RAMC122_NS
  #define NRF_CAN121                              NRF_CAN121_NS
  #define NRF_MVDMA121                            NRF_MVDMA121_NS
  #define NRF_RAMC123                             NRF_RAMC123_NS
  #define NRF_I3C121                              NRF_I3C121_NS
  #define NRF_DPPIC120                            NRF_DPPIC120_NS
  #define NRF_TIMER120                            NRF_TIMER120_NS
  #define NRF_TIMER121                            NRF_TIMER121_NS
  #define NRF_PWM120                              NRF_PWM120_NS
  #define NRF_SPIS120                             NRF_SPIS120_NS
  #define NRF_SPIM120                             NRF_SPIM120_NS
  #define NRF_UARTE120                            NRF_UARTE120_NS
  #define NRF_SPIM121                             NRF_SPIM121_NS
  #define NRF_VPR130                              NRF_VPR130_NS
  #define NRF_IPCT130                             NRF_IPCT130_NS
  #define NRF_DPPIC130                            NRF_DPPIC130_NS
  #define NRF_MUTEX130                            NRF_MUTEX130_NS
  #define NRF_RTC130                              NRF_RTC130_NS
  #define NRF_RTC131                              NRF_RTC131_NS
  #define NRF_WDT131                              NRF_WDT131_NS
  #define NRF_WDT132                              NRF_WDT132_NS
  #define NRF_EGU130                              NRF_EGU130_NS
  #define NRF_P0                                  NRF_P0_NS
  #define NRF_P1                                  NRF_P1_NS
  #define NRF_P2                                  NRF_P2_NS
  #define NRF_P6                                  NRF_P6_NS
  #define NRF_P8                                  NRF_P8_NS
  #define NRF_P9                                  NRF_P9_NS
  #define NRF_P10                                 NRF_P10_NS
  #define NRF_P11                                 NRF_P11_NS
  #define NRF_P12                                 NRF_P12_NS
  #define NRF_P13                                 NRF_P13_NS
  #define NRF_DPPIC131                            NRF_DPPIC131_NS
  #define NRF_SAADC                               NRF_SAADC_NS
  #define NRF_COMP                                NRF_COMP_NS
  #define NRF_LPCOMP                              NRF_LPCOMP_NS
  #define NRF_TEMP                                NRF_TEMP_NS
  #define NRF_DPPIC132                            NRF_DPPIC132_NS
  #define NRF_I2S130                              NRF_I2S130_NS
  #define NRF_PDM                                 NRF_PDM_NS
  #define NRF_QDEC130                             NRF_QDEC130_NS
  #define NRF_QDEC131                             NRF_QDEC131_NS
  #define NRF_I2S131                              NRF_I2S131_NS
  #define NRF_DPPIC133                            NRF_DPPIC133_NS
  #define NRF_TIMER130                            NRF_TIMER130_NS
  #define NRF_TIMER131                            NRF_TIMER131_NS
  #define NRF_PWM130                              NRF_PWM130_NS
  #define NRF_SPIM130                             NRF_SPIM130_NS
  #define NRF_SPIS130                             NRF_SPIS130_NS
  #define NRF_TWIM130                             NRF_TWIM130_NS
  #define NRF_TWIS130                             NRF_TWIS130_NS
  #define NRF_UARTE130                            NRF_UARTE130_NS
  #define NRF_SPIM131                             NRF_SPIM131_NS
  #define NRF_SPIS131                             NRF_SPIS131_NS
  #define NRF_TWIM131                             NRF_TWIM131_NS
  #define NRF_TWIS131                             NRF_TWIS131_NS
  #define NRF_UARTE131                            NRF_UARTE131_NS
  #define NRF_DPPIC134                            NRF_DPPIC134_NS
  #define NRF_TIMER132                            NRF_TIMER132_NS
  #define NRF_TIMER133                            NRF_TIMER133_NS
  #define NRF_PWM131                              NRF_PWM131_NS
  #define NRF_SPIM132                             NRF_SPIM132_NS
  #define NRF_SPIS132                             NRF_SPIS132_NS
  #define NRF_TWIM132                             NRF_TWIM132_NS
  #define NRF_TWIS132                             NRF_TWIS132_NS
  #define NRF_UARTE132                            NRF_UARTE132_NS
  #define NRF_SPIM133                             NRF_SPIM133_NS
  #define NRF_SPIS133                             NRF_SPIS133_NS
  #define NRF_TWIM133                             NRF_TWIM133_NS
  #define NRF_TWIS133                             NRF_TWIS133_NS
  #define NRF_UARTE133                            NRF_UARTE133_NS
  #define NRF_DPPIC135                            NRF_DPPIC135_NS
  #define NRF_TIMER134                            NRF_TIMER134_NS
  #define NRF_TIMER135                            NRF_TIMER135_NS
  #define NRF_PWM132                              NRF_PWM132_NS
  #define NRF_SPIM134                             NRF_SPIM134_NS
  #define NRF_SPIS134                             NRF_SPIS134_NS
  #define NRF_TWIM134                             NRF_TWIM134_NS
  #define NRF_TWIS134                             NRF_TWIS134_NS
  #define NRF_UARTE134                            NRF_UARTE134_NS
  #define NRF_SPIM135                             NRF_SPIM135_NS
  #define NRF_SPIS135                             NRF_SPIS135_NS
  #define NRF_TWIM135                             NRF_TWIM135_NS
  #define NRF_TWIS135                             NRF_TWIS135_NS
  #define NRF_UARTE135                            NRF_UARTE135_NS
  #define NRF_DPPIC136                            NRF_DPPIC136_NS
  #define NRF_TIMER136                            NRF_TIMER136_NS
  #define NRF_TIMER137                            NRF_TIMER137_NS
  #define NRF_PWM133                              NRF_PWM133_NS
  #define NRF_SPIM136                             NRF_SPIM136_NS
  #define NRF_SPIS136                             NRF_SPIS136_NS
  #define NRF_TWIM136                             NRF_TWIM136_NS
  #define NRF_TWIS136                             NRF_TWIS136_NS
  #define NRF_UARTE136                            NRF_UARTE136_NS
  #define NRF_SPIM137                             NRF_SPIM137_NS
  #define NRF_SPIS137                             NRF_SPIS137_NS
  #define NRF_TWIM137                             NRF_TWIM137_NS
  #define NRF_TWIS137                             NRF_TWIS137_NS
  #define NRF_UARTE137                            NRF_UARTE137_NS
#else                                                /*!< Remap NRF_X_S instances to NRF_X symbol for ease of use.             */
  #define NRF_FICR                                NRF_FICR_NS
  #define NRF_USBHSCORE0                          NRF_USBHSCORE0_S
  #define NRF_I3CCORE120                          NRF_I3CCORE120_NS
  #define NRF_I3CCORE121                          NRF_I3CCORE121_NS
  #define NRF_DMU120                              NRF_DMU120_NS
  #define NRF_MCAN120                             NRF_MCAN120_NS
  #define NRF_DMU121                              NRF_DMU121_NS
  #define NRF_MCAN121                             NRF_MCAN121_NS
  #define NRF_STMDATA                             NRF_STMDATA_S
  #define NRF_TDDCONF                             NRF_TDDCONF_S
  #define NRF_STM                                 NRF_STM_NS
  #define NRF_TPIU                                NRF_TPIU_NS
  #define NRF_CTI210                              NRF_CTI210_NS
  #define NRF_CTI211                              NRF_CTI211_NS
  #define NRF_ATBREPLICATOR210                    NRF_ATBREPLICATOR210_NS
  #define NRF_ATBREPLICATOR211                    NRF_ATBREPLICATOR211_NS
  #define NRF_ATBREPLICATOR212                    NRF_ATBREPLICATOR212_NS
  #define NRF_ATBREPLICATOR213                    NRF_ATBREPLICATOR213_NS
  #define NRF_ATBFUNNEL210                        NRF_ATBFUNNEL210_NS
  #define NRF_ATBFUNNEL211                        NRF_ATBFUNNEL211_NS
  #define NRF_ATBFUNNEL212                        NRF_ATBFUNNEL212_NS
  #define NRF_ATBFUNNEL213                        NRF_ATBFUNNEL213_NS
  #define NRF_GPIOTE130                           NRF_GPIOTE130_S
  #define NRF_GPIOTE131                           NRF_GPIOTE131_S
  #define NRF_GRTC                                NRF_GRTC_S
  #define NRF_TBM                                 NRF_TBM_S
  #define NRF_USBHS                               NRF_USBHS_S
  #define NRF_EXMIF                               NRF_EXMIF_S
  #define NRF_SECDOMBELLBOARD                     NRF_SECDOMBELLBOARD_S
  #define NRF_CANPLL                              NRF_CANPLL_S
  #define NRF_VPR120                              NRF_VPR120_S
  #define NRF_IPCT120                             NRF_IPCT120_S
  #define NRF_MUTEX120                            NRF_MUTEX120_NS
  #define NRF_I3C120                              NRF_I3C120_S
  #define NRF_VPR121                              NRF_VPR121_S
  #define NRF_CAN120                              NRF_CAN120_S
  #define NRF_MVDMA120                            NRF_MVDMA120_S
  #define NRF_RAMC122                             NRF_RAMC122_S
  #define NRF_CAN121                              NRF_CAN121_S
  #define NRF_MVDMA121                            NRF_MVDMA121_S
  #define NRF_RAMC123                             NRF_RAMC123_S
  #define NRF_I3C121                              NRF_I3C121_S
  #define NRF_DPPIC120                            NRF_DPPIC120_S
  #define NRF_TIMER120                            NRF_TIMER120_S
  #define NRF_TIMER121                            NRF_TIMER121_S
  #define NRF_PWM120                              NRF_PWM120_S
  #define NRF_SPIS120                             NRF_SPIS120_S
  #define NRF_SPIM120                             NRF_SPIM120_S
  #define NRF_UARTE120                            NRF_UARTE120_S
  #define NRF_SPIM121                             NRF_SPIM121_S
  #define NRF_VPR130                              NRF_VPR130_S
  #define NRF_IPCT130                             NRF_IPCT130_S
  #define NRF_DPPIC130                            NRF_DPPIC130_S
  #define NRF_MUTEX130                            NRF_MUTEX130_NS
  #define NRF_RTC130                              NRF_RTC130_S
  #define NRF_RTC131                              NRF_RTC131_S
  #define NRF_WDT131                              NRF_WDT131_S
  #define NRF_WDT132                              NRF_WDT132_S
  #define NRF_EGU130                              NRF_EGU130_S
  #define NRF_P0                                  NRF_P0_S
  #define NRF_P1                                  NRF_P1_S
  #define NRF_P2                                  NRF_P2_S
  #define NRF_P6                                  NRF_P6_S
  #define NRF_P8                                  NRF_P8_S
  #define NRF_P9                                  NRF_P9_S
  #define NRF_P10                                 NRF_P10_S
  #define NRF_P11                                 NRF_P11_S
  #define NRF_P12                                 NRF_P12_S
  #define NRF_P13                                 NRF_P13_S
  #define NRF_DPPIC131                            NRF_DPPIC131_S
  #define NRF_SAADC                               NRF_SAADC_S
  #define NRF_COMP                                NRF_COMP_S
  #define NRF_LPCOMP                              NRF_LPCOMP_S
  #define NRF_TEMP                                NRF_TEMP_S
  #define NRF_DPPIC132                            NRF_DPPIC132_S
  #define NRF_I2S130                              NRF_I2S130_S
  #define NRF_PDM                                 NRF_PDM_S
  #define NRF_QDEC130                             NRF_QDEC130_S
  #define NRF_QDEC131                             NRF_QDEC131_S
  #define NRF_I2S131                              NRF_I2S131_S
  #define NRF_DPPIC133                            NRF_DPPIC133_S
  #define NRF_TIMER130                            NRF_TIMER130_S
  #define NRF_TIMER131                            NRF_TIMER131_S
  #define NRF_PWM130                              NRF_PWM130_S
  #define NRF_SPIM130                             NRF_SPIM130_S
  #define NRF_SPIS130                             NRF_SPIS130_S
  #define NRF_TWIM130                             NRF_TWIM130_S
  #define NRF_TWIS130                             NRF_TWIS130_S
  #define NRF_UARTE130                            NRF_UARTE130_S
  #define NRF_SPIM131                             NRF_SPIM131_S
  #define NRF_SPIS131                             NRF_SPIS131_S
  #define NRF_TWIM131                             NRF_TWIM131_S
  #define NRF_TWIS131                             NRF_TWIS131_S
  #define NRF_UARTE131                            NRF_UARTE131_S
  #define NRF_DPPIC134                            NRF_DPPIC134_S
  #define NRF_TIMER132                            NRF_TIMER132_S
  #define NRF_TIMER133                            NRF_TIMER133_S
  #define NRF_PWM131                              NRF_PWM131_S
  #define NRF_SPIM132                             NRF_SPIM132_S
  #define NRF_SPIS132                             NRF_SPIS132_S
  #define NRF_TWIM132                             NRF_TWIM132_S
  #define NRF_TWIS132                             NRF_TWIS132_S
  #define NRF_UARTE132                            NRF_UARTE132_S
  #define NRF_SPIM133                             NRF_SPIM133_S
  #define NRF_SPIS133                             NRF_SPIS133_S
  #define NRF_TWIM133                             NRF_TWIM133_S
  #define NRF_TWIS133                             NRF_TWIS133_S
  #define NRF_UARTE133                            NRF_UARTE133_S
  #define NRF_DPPIC135                            NRF_DPPIC135_S
  #define NRF_TIMER134                            NRF_TIMER134_S
  #define NRF_TIMER135                            NRF_TIMER135_S
  #define NRF_PWM132                              NRF_PWM132_S
  #define NRF_SPIM134                             NRF_SPIM134_S
  #define NRF_SPIS134                             NRF_SPIS134_S
  #define NRF_TWIM134                             NRF_TWIM134_S
  #define NRF_TWIS134                             NRF_TWIS134_S
  #define NRF_UARTE134                            NRF_UARTE134_S
  #define NRF_SPIM135                             NRF_SPIM135_S
  #define NRF_SPIS135                             NRF_SPIS135_S
  #define NRF_TWIM135                             NRF_TWIM135_S
  #define NRF_TWIS135                             NRF_TWIS135_S
  #define NRF_UARTE135                            NRF_UARTE135_S
  #define NRF_DPPIC136                            NRF_DPPIC136_S
  #define NRF_TIMER136                            NRF_TIMER136_S
  #define NRF_TIMER137                            NRF_TIMER137_S
  #define NRF_PWM133                              NRF_PWM133_S
  #define NRF_SPIM136                             NRF_SPIM136_S
  #define NRF_SPIS136                             NRF_SPIS136_S
  #define NRF_TWIM136                             NRF_TWIM136_S
  #define NRF_TWIS136                             NRF_TWIS136_S
  #define NRF_UARTE136                            NRF_UARTE136_S
  #define NRF_SPIM137                             NRF_SPIM137_S
  #define NRF_SPIS137                             NRF_SPIS137_S
  #define NRF_TWIM137                             NRF_TWIM137_S
  #define NRF_TWIS137                             NRF_TWIS137_S
  #define NRF_UARTE137                            NRF_UARTE137_S
#endif                                               /*!< NRF_TRUSTZONE_NONSECURE                                              */

/* ========================================== End of section using anonymous unions ========================================== */

#if defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#endif


#ifdef __cplusplus
}
#endif
#endif /* NRF9230_ENGB_GLOBAL_H */

