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

#ifndef NRF54L05_GLOBAL_H
#define NRF54L05_GLOBAL_H

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

#define NRF_FICR_NS_BASE                  0x00FFC000UL
#define NRF_UICR_S_BASE                   0x00FFD000UL
#define NRF_SICR_S_BASE                   0x00FFE000UL
#define NRF_CRACENCORE_S_BASE             0x51800000UL
#define NRF_SPU00_S_BASE                  0x50040000UL
#define NRF_MPC00_S_BASE                  0x50041000UL
#define NRF_DPPIC00_NS_BASE               0x40042000UL
#define NRF_DPPIC00_S_BASE                0x50042000UL
#define NRF_PPIB00_NS_BASE                0x40043000UL
#define NRF_PPIB00_S_BASE                 0x50043000UL
#define NRF_PPIB01_NS_BASE                0x40044000UL
#define NRF_PPIB01_S_BASE                 0x50044000UL
#define NRF_KMU_S_BASE                    0x50045000UL
#define NRF_AAR00_NS_BASE                 0x40046000UL
#define NRF_CCM00_NS_BASE                 0x40046000UL
#define NRF_AAR00_S_BASE                  0x50046000UL
#define NRF_CCM00_S_BASE                  0x50046000UL
#define NRF_ECB00_NS_BASE                 0x40047000UL
#define NRF_ECB00_S_BASE                  0x50047000UL
#define NRF_CRACEN_S_BASE                 0x50048000UL
#define NRF_SPIM00_NS_BASE                0x4004A000UL
#define NRF_SPIS00_NS_BASE                0x4004A000UL
#define NRF_UARTE00_NS_BASE               0x4004A000UL
#define NRF_SPIM00_S_BASE                 0x5004A000UL
#define NRF_SPIS00_S_BASE                 0x5004A000UL
#define NRF_UARTE00_S_BASE                0x5004A000UL
#define NRF_GLITCHDET_S_BASE              0x5004B000UL
#define NRF_RRAMC_S_BASE                  0x5004B000UL
#define NRF_VPR00_NS_BASE                 0x4004C000UL
#define NRF_VPR00_S_BASE                  0x5004C000UL
#define NRF_P2_NS_BASE                    0x40050400UL
#define NRF_GPIOHSPADCTRL_S_BASE          0x50050400UL
#define NRF_P2_S_BASE                     0x50050400UL
#define NRF_CTRLAP_NS_BASE                0x40052000UL
#define NRF_CTRLAP_S_BASE                 0x50052000UL
#define NRF_TAD_NS_BASE                   0x40053000UL
#define NRF_TAD_S_BASE                    0x50053000UL
#define NRF_TIMER00_NS_BASE               0x40055000UL
#define NRF_TIMER00_S_BASE                0x50055000UL
#define NRF_SPU10_S_BASE                  0x50080000UL
#define NRF_DPPIC10_NS_BASE               0x40082000UL
#define NRF_DPPIC10_S_BASE                0x50082000UL
#define NRF_PPIB10_NS_BASE                0x40083000UL
#define NRF_PPIB10_S_BASE                 0x50083000UL
#define NRF_PPIB11_NS_BASE                0x40084000UL
#define NRF_PPIB11_S_BASE                 0x50084000UL
#define NRF_TIMER10_NS_BASE               0x40085000UL
#define NRF_TIMER10_S_BASE                0x50085000UL
#define NRF_EGU10_NS_BASE                 0x40087000UL
#define NRF_EGU10_S_BASE                  0x50087000UL
#define NRF_RADIO_NS_BASE                 0x4008A000UL
#define NRF_RADIO_S_BASE                  0x5008A000UL
#define NRF_SPU20_S_BASE                  0x500C0000UL
#define NRF_DPPIC20_NS_BASE               0x400C2000UL
#define NRF_DPPIC20_S_BASE                0x500C2000UL
#define NRF_PPIB20_NS_BASE                0x400C3000UL
#define NRF_PPIB20_S_BASE                 0x500C3000UL
#define NRF_PPIB21_NS_BASE                0x400C4000UL
#define NRF_PPIB21_S_BASE                 0x500C4000UL
#define NRF_PPIB22_NS_BASE                0x400C5000UL
#define NRF_PPIB22_S_BASE                 0x500C5000UL
#define NRF_SPIM20_NS_BASE                0x400C6000UL
#define NRF_SPIS20_NS_BASE                0x400C6000UL
#define NRF_TWIM20_NS_BASE                0x400C6000UL
#define NRF_TWIS20_NS_BASE                0x400C6000UL
#define NRF_UARTE20_NS_BASE               0x400C6000UL
#define NRF_SPIM20_S_BASE                 0x500C6000UL
#define NRF_SPIS20_S_BASE                 0x500C6000UL
#define NRF_TWIM20_S_BASE                 0x500C6000UL
#define NRF_TWIS20_S_BASE                 0x500C6000UL
#define NRF_UARTE20_S_BASE                0x500C6000UL
#define NRF_SPIM21_NS_BASE                0x400C7000UL
#define NRF_SPIS21_NS_BASE                0x400C7000UL
#define NRF_TWIM21_NS_BASE                0x400C7000UL
#define NRF_TWIS21_NS_BASE                0x400C7000UL
#define NRF_UARTE21_NS_BASE               0x400C7000UL
#define NRF_SPIM21_S_BASE                 0x500C7000UL
#define NRF_SPIS21_S_BASE                 0x500C7000UL
#define NRF_TWIM21_S_BASE                 0x500C7000UL
#define NRF_TWIS21_S_BASE                 0x500C7000UL
#define NRF_UARTE21_S_BASE                0x500C7000UL
#define NRF_SPIM22_NS_BASE                0x400C8000UL
#define NRF_SPIS22_NS_BASE                0x400C8000UL
#define NRF_TWIM22_NS_BASE                0x400C8000UL
#define NRF_TWIS22_NS_BASE                0x400C8000UL
#define NRF_UARTE22_NS_BASE               0x400C8000UL
#define NRF_SPIM22_S_BASE                 0x500C8000UL
#define NRF_SPIS22_S_BASE                 0x500C8000UL
#define NRF_TWIM22_S_BASE                 0x500C8000UL
#define NRF_TWIS22_S_BASE                 0x500C8000UL
#define NRF_UARTE22_S_BASE                0x500C8000UL
#define NRF_EGU20_NS_BASE                 0x400C9000UL
#define NRF_EGU20_S_BASE                  0x500C9000UL
#define NRF_TIMER20_NS_BASE               0x400CA000UL
#define NRF_TIMER20_S_BASE                0x500CA000UL
#define NRF_TIMER21_NS_BASE               0x400CB000UL
#define NRF_TIMER21_S_BASE                0x500CB000UL
#define NRF_TIMER22_NS_BASE               0x400CC000UL
#define NRF_TIMER22_S_BASE                0x500CC000UL
#define NRF_TIMER23_NS_BASE               0x400CD000UL
#define NRF_TIMER23_S_BASE                0x500CD000UL
#define NRF_TIMER24_NS_BASE               0x400CE000UL
#define NRF_TIMER24_S_BASE                0x500CE000UL
#define NRF_MEMCONF_NS_BASE               0x400CF000UL
#define NRF_MEMCONF_S_BASE                0x500CF000UL
#define NRF_PDM20_NS_BASE                 0x400D0000UL
#define NRF_PDM20_S_BASE                  0x500D0000UL
#define NRF_PDM21_NS_BASE                 0x400D1000UL
#define NRF_PDM21_S_BASE                  0x500D1000UL
#define NRF_PWM20_NS_BASE                 0x400D2000UL
#define NRF_PWM20_S_BASE                  0x500D2000UL
#define NRF_PWM21_NS_BASE                 0x400D3000UL
#define NRF_PWM21_S_BASE                  0x500D3000UL
#define NRF_PWM22_NS_BASE                 0x400D4000UL
#define NRF_PWM22_S_BASE                  0x500D4000UL
#define NRF_SAADC_NS_BASE                 0x400D5000UL
#define NRF_SAADC_S_BASE                  0x500D5000UL
#define NRF_NFCT_NS_BASE                  0x400D6000UL
#define NRF_NFCT_S_BASE                   0x500D6000UL
#define NRF_TEMP_NS_BASE                  0x400D7000UL
#define NRF_TEMP_S_BASE                   0x500D7000UL
#define NRF_P1_NS_BASE                    0x400D8200UL
#define NRF_P1_S_BASE                     0x500D8200UL
#define NRF_GPIOTE20_NS_BASE              0x400DA000UL
#define NRF_GPIOTE20_S_BASE               0x500DA000UL
#define NRF_TAMPC_S_BASE                  0x500DC000UL
#define NRF_I2S20_NS_BASE                 0x400DD000UL
#define NRF_I2S20_S_BASE                  0x500DD000UL
#define NRF_QDEC20_NS_BASE                0x400E0000UL
#define NRF_QDEC20_S_BASE                 0x500E0000UL
#define NRF_QDEC21_NS_BASE                0x400E1000UL
#define NRF_QDEC21_S_BASE                 0x500E1000UL
#define NRF_GRTC_NS_BASE                  0x400E2000UL
#define NRF_GRTC_S_BASE                   0x500E2000UL
#define NRF_SPU30_S_BASE                  0x50100000UL
#define NRF_DPPIC30_NS_BASE               0x40102000UL
#define NRF_DPPIC30_S_BASE                0x50102000UL
#define NRF_PPIB30_NS_BASE                0x40103000UL
#define NRF_PPIB30_S_BASE                 0x50103000UL
#define NRF_SPIM30_NS_BASE                0x40104000UL
#define NRF_SPIS30_NS_BASE                0x40104000UL
#define NRF_TWIM30_NS_BASE                0x40104000UL
#define NRF_TWIS30_NS_BASE                0x40104000UL
#define NRF_UARTE30_NS_BASE               0x40104000UL
#define NRF_SPIM30_S_BASE                 0x50104000UL
#define NRF_SPIS30_S_BASE                 0x50104000UL
#define NRF_TWIM30_S_BASE                 0x50104000UL
#define NRF_TWIS30_S_BASE                 0x50104000UL
#define NRF_UARTE30_S_BASE                0x50104000UL
#define NRF_COMP_NS_BASE                  0x40106000UL
#define NRF_LPCOMP_NS_BASE                0x40106000UL
#define NRF_COMP_S_BASE                   0x50106000UL
#define NRF_LPCOMP_S_BASE                 0x50106000UL
#define NRF_WDT30_S_BASE                  0x50108000UL
#define NRF_WDT31_NS_BASE                 0x40109000UL
#define NRF_WDT31_S_BASE                  0x50109000UL
#define NRF_P0_NS_BASE                    0x4010A000UL
#define NRF_P0_S_BASE                     0x5010A000UL
#define NRF_GPIOTE30_NS_BASE              0x4010C000UL
#define NRF_GPIOTE30_S_BASE               0x5010C000UL
#define NRF_CLOCK_NS_BASE                 0x4010E000UL
#define NRF_POWER_NS_BASE                 0x4010E000UL
#define NRF_RESET_NS_BASE                 0x4010E000UL
#define NRF_CLOCK_S_BASE                  0x5010E000UL
#define NRF_POWER_S_BASE                  0x5010E000UL
#define NRF_RESET_S_BASE                  0x5010E000UL
#define NRF_OSCILLATORS_NS_BASE           0x40120000UL
#define NRF_REGULATORS_NS_BASE            0x40120000UL
#define NRF_OSCILLATORS_S_BASE            0x50120000UL
#define NRF_REGULATORS_S_BASE             0x50120000UL

/* =========================================================================================================================== */
/* ================                                  Peripheral Declaration                                  ================ */
/* =========================================================================================================================== */

#define NRF_FICR_NS                       ((NRF_FICR_Type*)                     NRF_FICR_NS_BASE)
#define NRF_UICR_S                        ((NRF_UICR_Type*)                     NRF_UICR_S_BASE)
#define NRF_SICR_S                        ((NRF_SICR_Type*)                     NRF_SICR_S_BASE)
#define NRF_CRACENCORE_S                  ((NRF_CRACENCORE_Type*)               NRF_CRACENCORE_S_BASE)
#define NRF_SPU00_S                       ((NRF_SPU_Type*)                      NRF_SPU00_S_BASE)
#define NRF_MPC00_S                       ((NRF_MPC_Type*)                      NRF_MPC00_S_BASE)
#define NRF_DPPIC00_NS                    ((NRF_DPPIC_Type*)                    NRF_DPPIC00_NS_BASE)
#define NRF_DPPIC00_S                     ((NRF_DPPIC_Type*)                    NRF_DPPIC00_S_BASE)
#define NRF_PPIB00_NS                     ((NRF_PPIB_Type*)                     NRF_PPIB00_NS_BASE)
#define NRF_PPIB00_S                      ((NRF_PPIB_Type*)                     NRF_PPIB00_S_BASE)
#define NRF_PPIB01_NS                     ((NRF_PPIB_Type*)                     NRF_PPIB01_NS_BASE)
#define NRF_PPIB01_S                      ((NRF_PPIB_Type*)                     NRF_PPIB01_S_BASE)
#define NRF_KMU_S                         ((NRF_KMU_Type*)                      NRF_KMU_S_BASE)
#define NRF_AAR00_NS                      ((NRF_AAR_Type*)                      NRF_AAR00_NS_BASE)
#define NRF_CCM00_NS                      ((NRF_CCM_Type*)                      NRF_CCM00_NS_BASE)
#define NRF_AAR00_S                       ((NRF_AAR_Type*)                      NRF_AAR00_S_BASE)
#define NRF_CCM00_S                       ((NRF_CCM_Type*)                      NRF_CCM00_S_BASE)
#define NRF_ECB00_NS                      ((NRF_ECB_Type*)                      NRF_ECB00_NS_BASE)
#define NRF_ECB00_S                       ((NRF_ECB_Type*)                      NRF_ECB00_S_BASE)
#define NRF_CRACEN_S                      ((NRF_CRACEN_Type*)                   NRF_CRACEN_S_BASE)
#define NRF_SPIM00_NS                     ((NRF_SPIM_Type*)                     NRF_SPIM00_NS_BASE)
#define NRF_SPIS00_NS                     ((NRF_SPIS_Type*)                     NRF_SPIS00_NS_BASE)
#define NRF_UARTE00_NS                    ((NRF_UARTE_Type*)                    NRF_UARTE00_NS_BASE)
#define NRF_SPIM00_S                      ((NRF_SPIM_Type*)                     NRF_SPIM00_S_BASE)
#define NRF_SPIS00_S                      ((NRF_SPIS_Type*)                     NRF_SPIS00_S_BASE)
#define NRF_UARTE00_S                     ((NRF_UARTE_Type*)                    NRF_UARTE00_S_BASE)
#define NRF_GLITCHDET_S                   ((NRF_GLITCHDET_Type*)                NRF_GLITCHDET_S_BASE)
#define NRF_RRAMC_S                       ((NRF_RRAMC_Type*)                    NRF_RRAMC_S_BASE)
#define NRF_VPR00_NS                      ((NRF_VPR_Type*)                      NRF_VPR00_NS_BASE)
#define NRF_VPR00_S                       ((NRF_VPR_Type*)                      NRF_VPR00_S_BASE)
#define NRF_P2_NS                         ((NRF_GPIO_Type*)                     NRF_P2_NS_BASE)
#define NRF_GPIOHSPADCTRL_S               ((NRF_GPIOHSPADCTRL_Type*)            NRF_GPIOHSPADCTRL_S_BASE)
#define NRF_P2_S                          ((NRF_GPIO_Type*)                     NRF_P2_S_BASE)
#define NRF_CTRLAP_NS                     ((NRF_CTRLAPPERI_Type*)               NRF_CTRLAP_NS_BASE)
#define NRF_CTRLAP_S                      ((NRF_CTRLAPPERI_Type*)               NRF_CTRLAP_S_BASE)
#define NRF_TAD_NS                        ((NRF_TAD_Type*)                      NRF_TAD_NS_BASE)
#define NRF_TAD_S                         ((NRF_TAD_Type*)                      NRF_TAD_S_BASE)
#define NRF_TIMER00_NS                    ((NRF_TIMER_Type*)                    NRF_TIMER00_NS_BASE)
#define NRF_TIMER00_S                     ((NRF_TIMER_Type*)                    NRF_TIMER00_S_BASE)
#define NRF_SPU10_S                       ((NRF_SPU_Type*)                      NRF_SPU10_S_BASE)
#define NRF_DPPIC10_NS                    ((NRF_DPPIC_Type*)                    NRF_DPPIC10_NS_BASE)
#define NRF_DPPIC10_S                     ((NRF_DPPIC_Type*)                    NRF_DPPIC10_S_BASE)
#define NRF_PPIB10_NS                     ((NRF_PPIB_Type*)                     NRF_PPIB10_NS_BASE)
#define NRF_PPIB10_S                      ((NRF_PPIB_Type*)                     NRF_PPIB10_S_BASE)
#define NRF_PPIB11_NS                     ((NRF_PPIB_Type*)                     NRF_PPIB11_NS_BASE)
#define NRF_PPIB11_S                      ((NRF_PPIB_Type*)                     NRF_PPIB11_S_BASE)
#define NRF_TIMER10_NS                    ((NRF_TIMER_Type*)                    NRF_TIMER10_NS_BASE)
#define NRF_TIMER10_S                     ((NRF_TIMER_Type*)                    NRF_TIMER10_S_BASE)
#define NRF_EGU10_NS                      ((NRF_EGU_Type*)                      NRF_EGU10_NS_BASE)
#define NRF_EGU10_S                       ((NRF_EGU_Type*)                      NRF_EGU10_S_BASE)
#define NRF_RADIO_NS                      ((NRF_RADIO_Type*)                    NRF_RADIO_NS_BASE)
#define NRF_RADIO_S                       ((NRF_RADIO_Type*)                    NRF_RADIO_S_BASE)
#define NRF_SPU20_S                       ((NRF_SPU_Type*)                      NRF_SPU20_S_BASE)
#define NRF_DPPIC20_NS                    ((NRF_DPPIC_Type*)                    NRF_DPPIC20_NS_BASE)
#define NRF_DPPIC20_S                     ((NRF_DPPIC_Type*)                    NRF_DPPIC20_S_BASE)
#define NRF_PPIB20_NS                     ((NRF_PPIB_Type*)                     NRF_PPIB20_NS_BASE)
#define NRF_PPIB20_S                      ((NRF_PPIB_Type*)                     NRF_PPIB20_S_BASE)
#define NRF_PPIB21_NS                     ((NRF_PPIB_Type*)                     NRF_PPIB21_NS_BASE)
#define NRF_PPIB21_S                      ((NRF_PPIB_Type*)                     NRF_PPIB21_S_BASE)
#define NRF_PPIB22_NS                     ((NRF_PPIB_Type*)                     NRF_PPIB22_NS_BASE)
#define NRF_PPIB22_S                      ((NRF_PPIB_Type*)                     NRF_PPIB22_S_BASE)
#define NRF_SPIM20_NS                     ((NRF_SPIM_Type*)                     NRF_SPIM20_NS_BASE)
#define NRF_SPIS20_NS                     ((NRF_SPIS_Type*)                     NRF_SPIS20_NS_BASE)
#define NRF_TWIM20_NS                     ((NRF_TWIM_Type*)                     NRF_TWIM20_NS_BASE)
#define NRF_TWIS20_NS                     ((NRF_TWIS_Type*)                     NRF_TWIS20_NS_BASE)
#define NRF_UARTE20_NS                    ((NRF_UARTE_Type*)                    NRF_UARTE20_NS_BASE)
#define NRF_SPIM20_S                      ((NRF_SPIM_Type*)                     NRF_SPIM20_S_BASE)
#define NRF_SPIS20_S                      ((NRF_SPIS_Type*)                     NRF_SPIS20_S_BASE)
#define NRF_TWIM20_S                      ((NRF_TWIM_Type*)                     NRF_TWIM20_S_BASE)
#define NRF_TWIS20_S                      ((NRF_TWIS_Type*)                     NRF_TWIS20_S_BASE)
#define NRF_UARTE20_S                     ((NRF_UARTE_Type*)                    NRF_UARTE20_S_BASE)
#define NRF_SPIM21_NS                     ((NRF_SPIM_Type*)                     NRF_SPIM21_NS_BASE)
#define NRF_SPIS21_NS                     ((NRF_SPIS_Type*)                     NRF_SPIS21_NS_BASE)
#define NRF_TWIM21_NS                     ((NRF_TWIM_Type*)                     NRF_TWIM21_NS_BASE)
#define NRF_TWIS21_NS                     ((NRF_TWIS_Type*)                     NRF_TWIS21_NS_BASE)
#define NRF_UARTE21_NS                    ((NRF_UARTE_Type*)                    NRF_UARTE21_NS_BASE)
#define NRF_SPIM21_S                      ((NRF_SPIM_Type*)                     NRF_SPIM21_S_BASE)
#define NRF_SPIS21_S                      ((NRF_SPIS_Type*)                     NRF_SPIS21_S_BASE)
#define NRF_TWIM21_S                      ((NRF_TWIM_Type*)                     NRF_TWIM21_S_BASE)
#define NRF_TWIS21_S                      ((NRF_TWIS_Type*)                     NRF_TWIS21_S_BASE)
#define NRF_UARTE21_S                     ((NRF_UARTE_Type*)                    NRF_UARTE21_S_BASE)
#define NRF_SPIM22_NS                     ((NRF_SPIM_Type*)                     NRF_SPIM22_NS_BASE)
#define NRF_SPIS22_NS                     ((NRF_SPIS_Type*)                     NRF_SPIS22_NS_BASE)
#define NRF_TWIM22_NS                     ((NRF_TWIM_Type*)                     NRF_TWIM22_NS_BASE)
#define NRF_TWIS22_NS                     ((NRF_TWIS_Type*)                     NRF_TWIS22_NS_BASE)
#define NRF_UARTE22_NS                    ((NRF_UARTE_Type*)                    NRF_UARTE22_NS_BASE)
#define NRF_SPIM22_S                      ((NRF_SPIM_Type*)                     NRF_SPIM22_S_BASE)
#define NRF_SPIS22_S                      ((NRF_SPIS_Type*)                     NRF_SPIS22_S_BASE)
#define NRF_TWIM22_S                      ((NRF_TWIM_Type*)                     NRF_TWIM22_S_BASE)
#define NRF_TWIS22_S                      ((NRF_TWIS_Type*)                     NRF_TWIS22_S_BASE)
#define NRF_UARTE22_S                     ((NRF_UARTE_Type*)                    NRF_UARTE22_S_BASE)
#define NRF_EGU20_NS                      ((NRF_EGU_Type*)                      NRF_EGU20_NS_BASE)
#define NRF_EGU20_S                       ((NRF_EGU_Type*)                      NRF_EGU20_S_BASE)
#define NRF_TIMER20_NS                    ((NRF_TIMER_Type*)                    NRF_TIMER20_NS_BASE)
#define NRF_TIMER20_S                     ((NRF_TIMER_Type*)                    NRF_TIMER20_S_BASE)
#define NRF_TIMER21_NS                    ((NRF_TIMER_Type*)                    NRF_TIMER21_NS_BASE)
#define NRF_TIMER21_S                     ((NRF_TIMER_Type*)                    NRF_TIMER21_S_BASE)
#define NRF_TIMER22_NS                    ((NRF_TIMER_Type*)                    NRF_TIMER22_NS_BASE)
#define NRF_TIMER22_S                     ((NRF_TIMER_Type*)                    NRF_TIMER22_S_BASE)
#define NRF_TIMER23_NS                    ((NRF_TIMER_Type*)                    NRF_TIMER23_NS_BASE)
#define NRF_TIMER23_S                     ((NRF_TIMER_Type*)                    NRF_TIMER23_S_BASE)
#define NRF_TIMER24_NS                    ((NRF_TIMER_Type*)                    NRF_TIMER24_NS_BASE)
#define NRF_TIMER24_S                     ((NRF_TIMER_Type*)                    NRF_TIMER24_S_BASE)
#define NRF_MEMCONF_NS                    ((NRF_MEMCONF_Type*)                  NRF_MEMCONF_NS_BASE)
#define NRF_MEMCONF_S                     ((NRF_MEMCONF_Type*)                  NRF_MEMCONF_S_BASE)
#define NRF_PDM20_NS                      ((NRF_PDM_Type*)                      NRF_PDM20_NS_BASE)
#define NRF_PDM20_S                       ((NRF_PDM_Type*)                      NRF_PDM20_S_BASE)
#define NRF_PDM21_NS                      ((NRF_PDM_Type*)                      NRF_PDM21_NS_BASE)
#define NRF_PDM21_S                       ((NRF_PDM_Type*)                      NRF_PDM21_S_BASE)
#define NRF_PWM20_NS                      ((NRF_PWM_Type*)                      NRF_PWM20_NS_BASE)
#define NRF_PWM20_S                       ((NRF_PWM_Type*)                      NRF_PWM20_S_BASE)
#define NRF_PWM21_NS                      ((NRF_PWM_Type*)                      NRF_PWM21_NS_BASE)
#define NRF_PWM21_S                       ((NRF_PWM_Type*)                      NRF_PWM21_S_BASE)
#define NRF_PWM22_NS                      ((NRF_PWM_Type*)                      NRF_PWM22_NS_BASE)
#define NRF_PWM22_S                       ((NRF_PWM_Type*)                      NRF_PWM22_S_BASE)
#define NRF_SAADC_NS                      ((NRF_SAADC_Type*)                    NRF_SAADC_NS_BASE)
#define NRF_SAADC_S                       ((NRF_SAADC_Type*)                    NRF_SAADC_S_BASE)
#define NRF_NFCT_NS                       ((NRF_NFCT_Type*)                     NRF_NFCT_NS_BASE)
#define NRF_NFCT_S                        ((NRF_NFCT_Type*)                     NRF_NFCT_S_BASE)
#define NRF_TEMP_NS                       ((NRF_TEMP_Type*)                     NRF_TEMP_NS_BASE)
#define NRF_TEMP_S                        ((NRF_TEMP_Type*)                     NRF_TEMP_S_BASE)
#define NRF_P1_NS                         ((NRF_GPIO_Type*)                     NRF_P1_NS_BASE)
#define NRF_P1_S                          ((NRF_GPIO_Type*)                     NRF_P1_S_BASE)
#define NRF_GPIOTE20_NS                   ((NRF_GPIOTE_Type*)                   NRF_GPIOTE20_NS_BASE)
#define NRF_GPIOTE20_S                    ((NRF_GPIOTE_Type*)                   NRF_GPIOTE20_S_BASE)
#define NRF_TAMPC_S                       ((NRF_TAMPC_Type*)                    NRF_TAMPC_S_BASE)
#define NRF_I2S20_NS                      ((NRF_I2S_Type*)                      NRF_I2S20_NS_BASE)
#define NRF_I2S20_S                       ((NRF_I2S_Type*)                      NRF_I2S20_S_BASE)
#define NRF_QDEC20_NS                     ((NRF_QDEC_Type*)                     NRF_QDEC20_NS_BASE)
#define NRF_QDEC20_S                      ((NRF_QDEC_Type*)                     NRF_QDEC20_S_BASE)
#define NRF_QDEC21_NS                     ((NRF_QDEC_Type*)                     NRF_QDEC21_NS_BASE)
#define NRF_QDEC21_S                      ((NRF_QDEC_Type*)                     NRF_QDEC21_S_BASE)
#define NRF_GRTC_NS                       ((NRF_GRTC_Type*)                     NRF_GRTC_NS_BASE)
#define NRF_GRTC_S                        ((NRF_GRTC_Type*)                     NRF_GRTC_S_BASE)
#define NRF_SPU30_S                       ((NRF_SPU_Type*)                      NRF_SPU30_S_BASE)
#define NRF_DPPIC30_NS                    ((NRF_DPPIC_Type*)                    NRF_DPPIC30_NS_BASE)
#define NRF_DPPIC30_S                     ((NRF_DPPIC_Type*)                    NRF_DPPIC30_S_BASE)
#define NRF_PPIB30_NS                     ((NRF_PPIB_Type*)                     NRF_PPIB30_NS_BASE)
#define NRF_PPIB30_S                      ((NRF_PPIB_Type*)                     NRF_PPIB30_S_BASE)
#define NRF_SPIM30_NS                     ((NRF_SPIM_Type*)                     NRF_SPIM30_NS_BASE)
#define NRF_SPIS30_NS                     ((NRF_SPIS_Type*)                     NRF_SPIS30_NS_BASE)
#define NRF_TWIM30_NS                     ((NRF_TWIM_Type*)                     NRF_TWIM30_NS_BASE)
#define NRF_TWIS30_NS                     ((NRF_TWIS_Type*)                     NRF_TWIS30_NS_BASE)
#define NRF_UARTE30_NS                    ((NRF_UARTE_Type*)                    NRF_UARTE30_NS_BASE)
#define NRF_SPIM30_S                      ((NRF_SPIM_Type*)                     NRF_SPIM30_S_BASE)
#define NRF_SPIS30_S                      ((NRF_SPIS_Type*)                     NRF_SPIS30_S_BASE)
#define NRF_TWIM30_S                      ((NRF_TWIM_Type*)                     NRF_TWIM30_S_BASE)
#define NRF_TWIS30_S                      ((NRF_TWIS_Type*)                     NRF_TWIS30_S_BASE)
#define NRF_UARTE30_S                     ((NRF_UARTE_Type*)                    NRF_UARTE30_S_BASE)
#define NRF_COMP_NS                       ((NRF_COMP_Type*)                     NRF_COMP_NS_BASE)
#define NRF_LPCOMP_NS                     ((NRF_LPCOMP_Type*)                   NRF_LPCOMP_NS_BASE)
#define NRF_COMP_S                        ((NRF_COMP_Type*)                     NRF_COMP_S_BASE)
#define NRF_LPCOMP_S                      ((NRF_LPCOMP_Type*)                   NRF_LPCOMP_S_BASE)
#define NRF_WDT30_S                       ((NRF_WDT_Type*)                      NRF_WDT30_S_BASE)
#define NRF_WDT31_NS                      ((NRF_WDT_Type*)                      NRF_WDT31_NS_BASE)
#define NRF_WDT31_S                       ((NRF_WDT_Type*)                      NRF_WDT31_S_BASE)
#define NRF_P0_NS                         ((NRF_GPIO_Type*)                     NRF_P0_NS_BASE)
#define NRF_P0_S                          ((NRF_GPIO_Type*)                     NRF_P0_S_BASE)
#define NRF_GPIOTE30_NS                   ((NRF_GPIOTE_Type*)                   NRF_GPIOTE30_NS_BASE)
#define NRF_GPIOTE30_S                    ((NRF_GPIOTE_Type*)                   NRF_GPIOTE30_S_BASE)
#define NRF_CLOCK_NS                      ((NRF_CLOCK_Type*)                    NRF_CLOCK_NS_BASE)
#define NRF_POWER_NS                      ((NRF_POWER_Type*)                    NRF_POWER_NS_BASE)
#define NRF_RESET_NS                      ((NRF_RESET_Type*)                    NRF_RESET_NS_BASE)
#define NRF_CLOCK_S                       ((NRF_CLOCK_Type*)                    NRF_CLOCK_S_BASE)
#define NRF_POWER_S                       ((NRF_POWER_Type*)                    NRF_POWER_S_BASE)
#define NRF_RESET_S                       ((NRF_RESET_Type*)                    NRF_RESET_S_BASE)
#define NRF_OSCILLATORS_NS                ((NRF_OSCILLATORS_Type*)              NRF_OSCILLATORS_NS_BASE)
#define NRF_REGULATORS_NS                 ((NRF_REGULATORS_Type*)               NRF_REGULATORS_NS_BASE)
#define NRF_OSCILLATORS_S                 ((NRF_OSCILLATORS_Type*)              NRF_OSCILLATORS_S_BASE)
#define NRF_REGULATORS_S                  ((NRF_REGULATORS_Type*)               NRF_REGULATORS_S_BASE)

/* =========================================================================================================================== */
/* ================                                    TrustZone Remapping                                    ================ */
/* =========================================================================================================================== */

#ifdef NRF_TRUSTZONE_NONSECURE                       /*!< Remap NRF_X_NS instances to NRF_X symbol for ease of use.            */
  #define NRF_FICR                                NRF_FICR_NS
  #define NRF_DPPIC00                             NRF_DPPIC00_NS
  #define NRF_PPIB00                              NRF_PPIB00_NS
  #define NRF_PPIB01                              NRF_PPIB01_NS
  #define NRF_AAR00                               NRF_AAR00_NS
  #define NRF_CCM00                               NRF_CCM00_NS
  #define NRF_ECB00                               NRF_ECB00_NS
  #define NRF_SPIM00                              NRF_SPIM00_NS
  #define NRF_SPIS00                              NRF_SPIS00_NS
  #define NRF_UARTE00                             NRF_UARTE00_NS
  #define NRF_VPR00                               NRF_VPR00_NS
  #define NRF_P2                                  NRF_P2_NS
  #define NRF_CTRLAP                              NRF_CTRLAP_NS
  #define NRF_TAD                                 NRF_TAD_NS
  #define NRF_TIMER00                             NRF_TIMER00_NS
  #define NRF_DPPIC10                             NRF_DPPIC10_NS
  #define NRF_PPIB10                              NRF_PPIB10_NS
  #define NRF_PPIB11                              NRF_PPIB11_NS
  #define NRF_TIMER10                             NRF_TIMER10_NS
  #define NRF_EGU10                               NRF_EGU10_NS
  #define NRF_RADIO                               NRF_RADIO_NS
  #define NRF_DPPIC20                             NRF_DPPIC20_NS
  #define NRF_PPIB20                              NRF_PPIB20_NS
  #define NRF_PPIB21                              NRF_PPIB21_NS
  #define NRF_PPIB22                              NRF_PPIB22_NS
  #define NRF_SPIM20                              NRF_SPIM20_NS
  #define NRF_SPIS20                              NRF_SPIS20_NS
  #define NRF_TWIM20                              NRF_TWIM20_NS
  #define NRF_TWIS20                              NRF_TWIS20_NS
  #define NRF_UARTE20                             NRF_UARTE20_NS
  #define NRF_SPIM21                              NRF_SPIM21_NS
  #define NRF_SPIS21                              NRF_SPIS21_NS
  #define NRF_TWIM21                              NRF_TWIM21_NS
  #define NRF_TWIS21                              NRF_TWIS21_NS
  #define NRF_UARTE21                             NRF_UARTE21_NS
  #define NRF_SPIM22                              NRF_SPIM22_NS
  #define NRF_SPIS22                              NRF_SPIS22_NS
  #define NRF_TWIM22                              NRF_TWIM22_NS
  #define NRF_TWIS22                              NRF_TWIS22_NS
  #define NRF_UARTE22                             NRF_UARTE22_NS
  #define NRF_EGU20                               NRF_EGU20_NS
  #define NRF_TIMER20                             NRF_TIMER20_NS
  #define NRF_TIMER21                             NRF_TIMER21_NS
  #define NRF_TIMER22                             NRF_TIMER22_NS
  #define NRF_TIMER23                             NRF_TIMER23_NS
  #define NRF_TIMER24                             NRF_TIMER24_NS
  #define NRF_MEMCONF                             NRF_MEMCONF_NS
  #define NRF_PDM20                               NRF_PDM20_NS
  #define NRF_PDM21                               NRF_PDM21_NS
  #define NRF_PWM20                               NRF_PWM20_NS
  #define NRF_PWM21                               NRF_PWM21_NS
  #define NRF_PWM22                               NRF_PWM22_NS
  #define NRF_SAADC                               NRF_SAADC_NS
  #define NRF_NFCT                                NRF_NFCT_NS
  #define NRF_TEMP                                NRF_TEMP_NS
  #define NRF_P1                                  NRF_P1_NS
  #define NRF_GPIOTE20                            NRF_GPIOTE20_NS
  #define NRF_I2S20                               NRF_I2S20_NS
  #define NRF_QDEC20                              NRF_QDEC20_NS
  #define NRF_QDEC21                              NRF_QDEC21_NS
  #define NRF_GRTC                                NRF_GRTC_NS
  #define NRF_DPPIC30                             NRF_DPPIC30_NS
  #define NRF_PPIB30                              NRF_PPIB30_NS
  #define NRF_SPIM30                              NRF_SPIM30_NS
  #define NRF_SPIS30                              NRF_SPIS30_NS
  #define NRF_TWIM30                              NRF_TWIM30_NS
  #define NRF_TWIS30                              NRF_TWIS30_NS
  #define NRF_UARTE30                             NRF_UARTE30_NS
  #define NRF_COMP                                NRF_COMP_NS
  #define NRF_LPCOMP                              NRF_LPCOMP_NS
  #define NRF_WDT31                               NRF_WDT31_NS
  #define NRF_P0                                  NRF_P0_NS
  #define NRF_GPIOTE30                            NRF_GPIOTE30_NS
  #define NRF_CLOCK                               NRF_CLOCK_NS
  #define NRF_POWER                               NRF_POWER_NS
  #define NRF_RESET                               NRF_RESET_NS
  #define NRF_OSCILLATORS                         NRF_OSCILLATORS_NS
  #define NRF_REGULATORS                          NRF_REGULATORS_NS
#else                                                /*!< Remap NRF_X_S instances to NRF_X symbol for ease of use.             */
  #define NRF_FICR                                NRF_FICR_NS
  #define NRF_UICR                                NRF_UICR_S
  #define NRF_SICR                                NRF_SICR_S
  #define NRF_CRACENCORE                          NRF_CRACENCORE_S
  #define NRF_SPU00                               NRF_SPU00_S
  #define NRF_MPC00                               NRF_MPC00_S
  #define NRF_DPPIC00                             NRF_DPPIC00_S
  #define NRF_PPIB00                              NRF_PPIB00_S
  #define NRF_PPIB01                              NRF_PPIB01_S
  #define NRF_KMU                                 NRF_KMU_S
  #define NRF_AAR00                               NRF_AAR00_S
  #define NRF_CCM00                               NRF_CCM00_S
  #define NRF_ECB00                               NRF_ECB00_S
  #define NRF_CRACEN                              NRF_CRACEN_S
  #define NRF_SPIM00                              NRF_SPIM00_S
  #define NRF_SPIS00                              NRF_SPIS00_S
  #define NRF_UARTE00                             NRF_UARTE00_S
  #define NRF_GLITCHDET                           NRF_GLITCHDET_S
  #define NRF_RRAMC                               NRF_RRAMC_S
  #define NRF_VPR00                               NRF_VPR00_S
  #define NRF_GPIOHSPADCTRL                       NRF_GPIOHSPADCTRL_S
  #define NRF_P2                                  NRF_P2_S
  #define NRF_CTRLAP                              NRF_CTRLAP_S
  #define NRF_TAD                                 NRF_TAD_S
  #define NRF_TIMER00                             NRF_TIMER00_S
  #define NRF_SPU10                               NRF_SPU10_S
  #define NRF_DPPIC10                             NRF_DPPIC10_S
  #define NRF_PPIB10                              NRF_PPIB10_S
  #define NRF_PPIB11                              NRF_PPIB11_S
  #define NRF_TIMER10                             NRF_TIMER10_S
  #define NRF_EGU10                               NRF_EGU10_S
  #define NRF_RADIO                               NRF_RADIO_S
  #define NRF_SPU20                               NRF_SPU20_S
  #define NRF_DPPIC20                             NRF_DPPIC20_S
  #define NRF_PPIB20                              NRF_PPIB20_S
  #define NRF_PPIB21                              NRF_PPIB21_S
  #define NRF_PPIB22                              NRF_PPIB22_S
  #define NRF_SPIM20                              NRF_SPIM20_S
  #define NRF_SPIS20                              NRF_SPIS20_S
  #define NRF_TWIM20                              NRF_TWIM20_S
  #define NRF_TWIS20                              NRF_TWIS20_S
  #define NRF_UARTE20                             NRF_UARTE20_S
  #define NRF_SPIM21                              NRF_SPIM21_S
  #define NRF_SPIS21                              NRF_SPIS21_S
  #define NRF_TWIM21                              NRF_TWIM21_S
  #define NRF_TWIS21                              NRF_TWIS21_S
  #define NRF_UARTE21                             NRF_UARTE21_S
  #define NRF_SPIM22                              NRF_SPIM22_S
  #define NRF_SPIS22                              NRF_SPIS22_S
  #define NRF_TWIM22                              NRF_TWIM22_S
  #define NRF_TWIS22                              NRF_TWIS22_S
  #define NRF_UARTE22                             NRF_UARTE22_S
  #define NRF_EGU20                               NRF_EGU20_S
  #define NRF_TIMER20                             NRF_TIMER20_S
  #define NRF_TIMER21                             NRF_TIMER21_S
  #define NRF_TIMER22                             NRF_TIMER22_S
  #define NRF_TIMER23                             NRF_TIMER23_S
  #define NRF_TIMER24                             NRF_TIMER24_S
  #define NRF_MEMCONF                             NRF_MEMCONF_S
  #define NRF_PDM20                               NRF_PDM20_S
  #define NRF_PDM21                               NRF_PDM21_S
  #define NRF_PWM20                               NRF_PWM20_S
  #define NRF_PWM21                               NRF_PWM21_S
  #define NRF_PWM22                               NRF_PWM22_S
  #define NRF_SAADC                               NRF_SAADC_S
  #define NRF_NFCT                                NRF_NFCT_S
  #define NRF_TEMP                                NRF_TEMP_S
  #define NRF_P1                                  NRF_P1_S
  #define NRF_GPIOTE20                            NRF_GPIOTE20_S
  #define NRF_TAMPC                               NRF_TAMPC_S
  #define NRF_I2S20                               NRF_I2S20_S
  #define NRF_QDEC20                              NRF_QDEC20_S
  #define NRF_QDEC21                              NRF_QDEC21_S
  #define NRF_GRTC                                NRF_GRTC_S
  #define NRF_SPU30                               NRF_SPU30_S
  #define NRF_DPPIC30                             NRF_DPPIC30_S
  #define NRF_PPIB30                              NRF_PPIB30_S
  #define NRF_SPIM30                              NRF_SPIM30_S
  #define NRF_SPIS30                              NRF_SPIS30_S
  #define NRF_TWIM30                              NRF_TWIM30_S
  #define NRF_TWIS30                              NRF_TWIS30_S
  #define NRF_UARTE30                             NRF_UARTE30_S
  #define NRF_COMP                                NRF_COMP_S
  #define NRF_LPCOMP                              NRF_LPCOMP_S
  #define NRF_WDT30                               NRF_WDT30_S
  #define NRF_WDT31                               NRF_WDT31_S
  #define NRF_P0                                  NRF_P0_S
  #define NRF_GPIOTE30                            NRF_GPIOTE30_S
  #define NRF_CLOCK                               NRF_CLOCK_S
  #define NRF_POWER                               NRF_POWER_S
  #define NRF_RESET                               NRF_RESET_S
  #define NRF_OSCILLATORS                         NRF_OSCILLATORS_S
  #define NRF_REGULATORS                          NRF_REGULATORS_S
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
#endif /* NRF54L05_GLOBAL_H */

