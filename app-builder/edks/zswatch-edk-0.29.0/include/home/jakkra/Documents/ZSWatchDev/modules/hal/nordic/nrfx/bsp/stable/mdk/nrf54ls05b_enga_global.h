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

#ifndef NRF54LS05B_ENGA_GLOBAL_H
#define NRF54LS05B_ENGA_GLOBAL_H

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

#define NRF_FICR_BASE                     0x00FFC000UL
#define NRF_UICR_BASE                     0x00FFD000UL
#define NRF_DPPIC00_BASE                  0x40042000UL
#define NRF_PPIB00_BASE                   0x40044000UL
#define NRF_PPIB01_BASE                   0x40045000UL
#define NRF_AAR00_BASE                    0x4004A000UL
#define NRF_CCM00_BASE                    0x4004A000UL
#define NRF_ECB00_BASE                    0x4004B000UL
#define NRF_RRAMC_BASE                    0x4004E000UL
#define NRF_CTRLAP_BASE                   0x40052000UL
#define NRF_TAD_BASE                      0x40053000UL
#define NRF_TIMER00_BASE                  0x40055000UL
#define NRF_EGU00_BASE                    0x40058000UL
#define NRF_CRACEN_BASE                   0x40059000UL
#define NRF_CRACENCORE_BASE               0x40059000UL
#define NRF_DPPIC10_BASE                  0x40082000UL
#define NRF_PPIB10_BASE                   0x40083000UL
#define NRF_PPIB11_BASE                   0x40084000UL
#define NRF_TIMER10_BASE                  0x40085000UL
#define NRF_EGU10_BASE                    0x40087000UL
#define NRF_RADIO_BASE                    0x4008A000UL
#define NRF_DPPIC20_BASE                  0x400C2000UL
#define NRF_PPIB20_BASE                   0x400C3000UL
#define NRF_PPIB21_BASE                   0x400C4000UL
#define NRF_PPIB22_BASE                   0x400C5000UL
#define NRF_SPIM20_BASE                   0x400C6000UL
#define NRF_SPIS20_BASE                   0x400C6000UL
#define NRF_TWIM20_BASE                   0x400C6000UL
#define NRF_TWIS20_BASE                   0x400C6000UL
#define NRF_UARTE20_BASE                  0x400C6000UL
#define NRF_SPIM21_BASE                   0x400C7000UL
#define NRF_SPIS21_BASE                   0x400C7000UL
#define NRF_TWIM21_BASE                   0x400C7000UL
#define NRF_TWIS21_BASE                   0x400C7000UL
#define NRF_UARTE21_BASE                  0x400C7000UL
#define NRF_SPIM22_BASE                   0x400C8000UL
#define NRF_SPIS22_BASE                   0x400C8000UL
#define NRF_TWIM22_BASE                   0x400C8000UL
#define NRF_TWIS22_BASE                   0x400C8000UL
#define NRF_UARTE22_BASE                  0x400C8000UL
#define NRF_EGU20_BASE                    0x400C9000UL
#define NRF_TIMER20_BASE                  0x400CA000UL
#define NRF_MEMCONF_BASE                  0x400CF000UL
#define NRF_PWM20_BASE                    0x400D2000UL
#define NRF_SAADC_BASE                    0x400D5000UL
#define NRF_TEMP_BASE                     0x400D7000UL
#define NRF_P1_BASE                       0x400D8200UL
#define NRF_GPIOTE20_BASE                 0x400DA000UL
#define NRF_QDEC20_BASE                   0x400E0000UL
#define NRF_GRTC_BASE                     0x400E2000UL
#define NRF_TAMPC_BASE                    0x400EF000UL
#define NRF_DPPIC30_BASE                  0x40102000UL
#define NRF_PPIB30_BASE                   0x40103000UL
#define NRF_WDT30_BASE                    0x40108000UL
#define NRF_P0_BASE                       0x4010A000UL
#define NRF_GPIOTE30_BASE                 0x4010C000UL
#define NRF_CLOCK_BASE                    0x4010E000UL
#define NRF_POWER_BASE                    0x4010E000UL
#define NRF_RESET_BASE                    0x4010E000UL
#define NRF_OSCILLATORS_BASE              0x40120000UL
#define NRF_REGULATORS_BASE               0x40120000UL

/* =========================================================================================================================== */
/* ================                                  Peripheral Declaration                                  ================ */
/* =========================================================================================================================== */

#define NRF_FICR                          ((NRF_FICR_Type*)                     NRF_FICR_BASE)
#define NRF_UICR                          ((NRF_UICR_Type*)                     NRF_UICR_BASE)
#define NRF_DPPIC00                       ((NRF_DPPIC_Type*)                    NRF_DPPIC00_BASE)
#define NRF_PPIB00                        ((NRF_PPIB_Type*)                     NRF_PPIB00_BASE)
#define NRF_PPIB01                        ((NRF_PPIB_Type*)                     NRF_PPIB01_BASE)
#define NRF_AAR00                         ((NRF_AAR_Type*)                      NRF_AAR00_BASE)
#define NRF_CCM00                         ((NRF_CCM_Type*)                      NRF_CCM00_BASE)
#define NRF_ECB00                         ((NRF_ECB_Type*)                      NRF_ECB00_BASE)
#define NRF_RRAMC                         ((NRF_RRAMC_Type*)                    NRF_RRAMC_BASE)
#define NRF_CTRLAP                        ((NRF_CTRLAPPERI_Type*)               NRF_CTRLAP_BASE)
#define NRF_TAD                           ((NRF_TAD_Type*)                      NRF_TAD_BASE)
#define NRF_TIMER00                       ((NRF_TIMER_Type*)                    NRF_TIMER00_BASE)
#define NRF_EGU00                         ((NRF_EGU_Type*)                      NRF_EGU00_BASE)
#define NRF_CRACEN                        ((NRF_CRACEN_Type*)                   NRF_CRACEN_BASE)
#define NRF_CRACENCORE                    ((NRF_CRACENCORE_Type*)               NRF_CRACENCORE_BASE)
#define NRF_DPPIC10                       ((NRF_DPPIC_Type*)                    NRF_DPPIC10_BASE)
#define NRF_PPIB10                        ((NRF_PPIB_Type*)                     NRF_PPIB10_BASE)
#define NRF_PPIB11                        ((NRF_PPIB_Type*)                     NRF_PPIB11_BASE)
#define NRF_TIMER10                       ((NRF_TIMER_Type*)                    NRF_TIMER10_BASE)
#define NRF_EGU10                         ((NRF_EGU_Type*)                      NRF_EGU10_BASE)
#define NRF_RADIO                         ((NRF_RADIO_Type*)                    NRF_RADIO_BASE)
#define NRF_DPPIC20                       ((NRF_DPPIC_Type*)                    NRF_DPPIC20_BASE)
#define NRF_PPIB20                        ((NRF_PPIB_Type*)                     NRF_PPIB20_BASE)
#define NRF_PPIB21                        ((NRF_PPIB_Type*)                     NRF_PPIB21_BASE)
#define NRF_PPIB22                        ((NRF_PPIB_Type*)                     NRF_PPIB22_BASE)
#define NRF_SPIM20                        ((NRF_SPIM_Type*)                     NRF_SPIM20_BASE)
#define NRF_SPIS20                        ((NRF_SPIS_Type*)                     NRF_SPIS20_BASE)
#define NRF_TWIM20                        ((NRF_TWIM_Type*)                     NRF_TWIM20_BASE)
#define NRF_TWIS20                        ((NRF_TWIS_Type*)                     NRF_TWIS20_BASE)
#define NRF_UARTE20                       ((NRF_UARTE_Type*)                    NRF_UARTE20_BASE)
#define NRF_SPIM21                        ((NRF_SPIM_Type*)                     NRF_SPIM21_BASE)
#define NRF_SPIS21                        ((NRF_SPIS_Type*)                     NRF_SPIS21_BASE)
#define NRF_TWIM21                        ((NRF_TWIM_Type*)                     NRF_TWIM21_BASE)
#define NRF_TWIS21                        ((NRF_TWIS_Type*)                     NRF_TWIS21_BASE)
#define NRF_UARTE21                       ((NRF_UARTE_Type*)                    NRF_UARTE21_BASE)
#define NRF_SPIM22                        ((NRF_SPIM_Type*)                     NRF_SPIM22_BASE)
#define NRF_SPIS22                        ((NRF_SPIS_Type*)                     NRF_SPIS22_BASE)
#define NRF_TWIM22                        ((NRF_TWIM_Type*)                     NRF_TWIM22_BASE)
#define NRF_TWIS22                        ((NRF_TWIS_Type*)                     NRF_TWIS22_BASE)
#define NRF_UARTE22                       ((NRF_UARTE_Type*)                    NRF_UARTE22_BASE)
#define NRF_EGU20                         ((NRF_EGU_Type*)                      NRF_EGU20_BASE)
#define NRF_TIMER20                       ((NRF_TIMER_Type*)                    NRF_TIMER20_BASE)
#define NRF_MEMCONF                       ((NRF_MEMCONF_Type*)                  NRF_MEMCONF_BASE)
#define NRF_PWM20                         ((NRF_PWM_Type*)                      NRF_PWM20_BASE)
#define NRF_SAADC                         ((NRF_SAADC_Type*)                    NRF_SAADC_BASE)
#define NRF_TEMP                          ((NRF_TEMP_Type*)                     NRF_TEMP_BASE)
#define NRF_P1                            ((NRF_GPIO_Type*)                     NRF_P1_BASE)
#define NRF_GPIOTE20                      ((NRF_GPIOTE_Type*)                   NRF_GPIOTE20_BASE)
#define NRF_QDEC20                        ((NRF_QDEC_Type*)                     NRF_QDEC20_BASE)
#define NRF_GRTC                          ((NRF_GRTC_Type*)                     NRF_GRTC_BASE)
#define NRF_TAMPC                         ((NRF_TAMPC_Type*)                    NRF_TAMPC_BASE)
#define NRF_DPPIC30                       ((NRF_DPPIC_Type*)                    NRF_DPPIC30_BASE)
#define NRF_PPIB30                        ((NRF_PPIB_Type*)                     NRF_PPIB30_BASE)
#define NRF_WDT30                         ((NRF_WDT_Type*)                      NRF_WDT30_BASE)
#define NRF_P0                            ((NRF_GPIO_Type*)                     NRF_P0_BASE)
#define NRF_GPIOTE30                      ((NRF_GPIOTE_Type*)                   NRF_GPIOTE30_BASE)
#define NRF_CLOCK                         ((NRF_CLOCK_Type*)                    NRF_CLOCK_BASE)
#define NRF_POWER                         ((NRF_POWER_Type*)                    NRF_POWER_BASE)
#define NRF_RESET                         ((NRF_RESET_Type*)                    NRF_RESET_BASE)
#define NRF_OSCILLATORS                   ((NRF_OSCILLATORS_Type*)              NRF_OSCILLATORS_BASE)
#define NRF_REGULATORS                    ((NRF_REGULATORS_Type*)               NRF_REGULATORS_BASE)

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
#endif /* NRF54LS05B_ENGA_GLOBAL_H */

