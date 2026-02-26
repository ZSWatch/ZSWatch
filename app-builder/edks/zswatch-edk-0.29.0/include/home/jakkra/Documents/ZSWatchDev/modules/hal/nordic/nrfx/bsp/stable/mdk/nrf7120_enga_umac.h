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

#ifndef NRF7120_ENGA_UMAC_H
#define NRF7120_ENGA_UMAC_H

#ifdef __cplusplus
    extern "C" {
#endif


#ifdef NRF_UMAC                                      /*!< Processor information is domain local.                               */


/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */

typedef enum {
/* ===================================================== Core Interrupts ===================================================== */
/* ============================================== Processor Specific Interrupts ============================================== */
  VPRTIM_IRQn                            = 7,        /*!< 7 VPRTIM                                                             */
  VPRCLIC_16_IRQn                        = 16,       /*!< 16 VPRCLIC_16                                                        */
  VPRCLIC_17_IRQn                        = 17,       /*!< 17 VPRCLIC_17                                                        */
  VPRCLIC_18_IRQn                        = 18,       /*!< 18 VPRCLIC_18                                                        */
  VPRCLIC_19_IRQn                        = 19,       /*!< 19 VPRCLIC_19                                                        */
  VPRCLIC_20_IRQn                        = 20,       /*!< 20 VPRCLIC_20                                                        */
  VPRCLIC_21_IRQn                        = 21,       /*!< 21 VPRCLIC_21                                                        */
  VPRCLIC_22_IRQn                        = 22,       /*!< 22 VPRCLIC_22                                                        */
  VPRCLIC_23_IRQn                        = 23,       /*!< 23 VPRCLIC_23                                                        */
  LMAC_VPR_IRQn                          = 40,       /*!< 40 LMAC_VPR                                                          */
  MVDMA_IRQn                             = 48,       /*!< 48 MVDMA                                                             */
  SERIAL00_IRQn                          = 77,       /*!< 77 SERIAL00                                                          */
  SERIAL01_IRQn                          = 93,       /*!< 93 SERIAL01                                                          */
  SERIAL20_IRQn                          = 198,      /*!< 198 SERIAL20                                                         */
  SERIAL21_IRQn                          = 199,      /*!< 199 SERIAL21                                                         */
  SERIAL22_IRQn                          = 200,      /*!< 200 SERIAL22                                                         */
  GRTC_5_IRQn                            = 231,      /*!< 231 GRTC_5                                                           */
  SERIAL23_IRQn                          = 237,      /*!< 237 SERIAL23                                                         */
  SERIAL24_IRQn                          = 238,      /*!< 238 SERIAL24                                                         */
  SERIAL30_IRQn                          = 260,      /*!< 260 SERIAL30                                                         */
} IRQn_Type;

/* ==================================================== Interrupt Aliases ==================================================== */
#define SPIM00_IRQn                   SERIAL00_IRQn
#define SPIM00_IRQHandler             SERIAL00_IRQHandler
#define SPIS00_IRQn                   SERIAL00_IRQn
#define SPIS00_IRQHandler             SERIAL00_IRQHandler
#define UARTE00_IRQn                  SERIAL00_IRQn
#define UARTE00_IRQHandler            SERIAL00_IRQHandler
#define SPIM01_IRQn                   SERIAL01_IRQn
#define SPIM01_IRQHandler             SERIAL01_IRQHandler
#define SPIS01_IRQn                   SERIAL01_IRQn
#define SPIS01_IRQHandler             SERIAL01_IRQHandler
#define UARTE01_IRQn                  SERIAL01_IRQn
#define UARTE01_IRQHandler            SERIAL01_IRQHandler
#define SPIM20_IRQn                   SERIAL20_IRQn
#define SPIM20_IRQHandler             SERIAL20_IRQHandler
#define SPIS20_IRQn                   SERIAL20_IRQn
#define SPIS20_IRQHandler             SERIAL20_IRQHandler
#define TWIM20_IRQn                   SERIAL20_IRQn
#define TWIM20_IRQHandler             SERIAL20_IRQHandler
#define TWIS20_IRQn                   SERIAL20_IRQn
#define TWIS20_IRQHandler             SERIAL20_IRQHandler
#define UARTE20_IRQn                  SERIAL20_IRQn
#define UARTE20_IRQHandler            SERIAL20_IRQHandler
#define SPIM21_IRQn                   SERIAL21_IRQn
#define SPIM21_IRQHandler             SERIAL21_IRQHandler
#define SPIS21_IRQn                   SERIAL21_IRQn
#define SPIS21_IRQHandler             SERIAL21_IRQHandler
#define TWIM21_IRQn                   SERIAL21_IRQn
#define TWIM21_IRQHandler             SERIAL21_IRQHandler
#define TWIS21_IRQn                   SERIAL21_IRQn
#define TWIS21_IRQHandler             SERIAL21_IRQHandler
#define UARTE21_IRQn                  SERIAL21_IRQn
#define UARTE21_IRQHandler            SERIAL21_IRQHandler
#define SPIM22_IRQn                   SERIAL22_IRQn
#define SPIM22_IRQHandler             SERIAL22_IRQHandler
#define SPIS22_IRQn                   SERIAL22_IRQn
#define SPIS22_IRQHandler             SERIAL22_IRQHandler
#define TWIM22_IRQn                   SERIAL22_IRQn
#define TWIM22_IRQHandler             SERIAL22_IRQHandler
#define TWIS22_IRQn                   SERIAL22_IRQn
#define TWIS22_IRQHandler             SERIAL22_IRQHandler
#define UARTE22_IRQn                  SERIAL22_IRQn
#define UARTE22_IRQHandler            SERIAL22_IRQHandler
#define SPIM23_IRQn                   SERIAL23_IRQn
#define SPIM23_IRQHandler             SERIAL23_IRQHandler
#define SPIS23_IRQn                   SERIAL23_IRQn
#define SPIS23_IRQHandler             SERIAL23_IRQHandler
#define TWIM23_IRQn                   SERIAL23_IRQn
#define TWIM23_IRQHandler             SERIAL23_IRQHandler
#define TWIS23_IRQn                   SERIAL23_IRQn
#define TWIS23_IRQHandler             SERIAL23_IRQHandler
#define UARTE23_IRQn                  SERIAL23_IRQn
#define UARTE23_IRQHandler            SERIAL23_IRQHandler
#define SPIM24_IRQn                   SERIAL24_IRQn
#define SPIM24_IRQHandler             SERIAL24_IRQHandler
#define SPIS24_IRQn                   SERIAL24_IRQn
#define SPIS24_IRQHandler             SERIAL24_IRQHandler
#define TWIM24_IRQn                   SERIAL24_IRQn
#define TWIM24_IRQHandler             SERIAL24_IRQHandler
#define TWIS24_IRQn                   SERIAL24_IRQn
#define TWIS24_IRQHandler             SERIAL24_IRQHandler
#define UARTE24_IRQn                  SERIAL24_IRQn
#define UARTE24_IRQHandler            SERIAL24_IRQHandler
#define SPIM30_IRQn                   SERIAL30_IRQn
#define SPIM30_IRQHandler             SERIAL30_IRQHandler
#define SPIS30_IRQn                   SERIAL30_IRQn
#define SPIS30_IRQHandler             SERIAL30_IRQHandler
#define TWIM30_IRQn                   SERIAL30_IRQn
#define TWIM30_IRQHandler             SERIAL30_IRQHandler
#define TWIS30_IRQn                   SERIAL30_IRQn
#define TWIS30_IRQHandler             SERIAL30_IRQHandler
#define UARTE30_IRQn                  SERIAL30_IRQn
#define UARTE30_IRQHandler            SERIAL30_IRQHandler

/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* ====================== Configuration of the Nordic Semiconductor VPR Processor and Core Peripherals ======================= */
#define __VPR_REV                    1.4             /*!< VPR Core Revision                                                    */
#define __VPR_REV_MAJOR                1             /*!< VPR Core Major Revision                                              */
#define __VPR_REV_MINOR                4             /*!< VPR Core Minor Revision                                              */
#define __VPR_REV_PATCH                0             /*!< VPR Core Patch Revision                                              */
#define __DSP_PRESENT                  0             /*!< DSP present or not                                                   */
#define __CLIC_PRIO_BITS               3             /*!< Number of Bits used for Priority Levels                              */
#define __MTVT_PRESENT                 1             /*!< CPU supports alternate Vector Table address                          */
#define __MPU_PRESENT                  1             /*!< MPU present                                                          */
#define __FPU_PRESENT                  0             /*!< FPU present                                                          */
#define __FPU_DP                       0             /*!< Double Precision FPU                                                 */
#define __INTERRUPTS_MAX             480             /*!< Size of interrupt vector table                                       */

#define NRF_VPR     NRF_WIFICORE_VPRUMAC             /*!< VPR instance name                                                    */
#include "core_vpr.h"                                /*!< Nordic Semiconductor VPR processor and core peripherals              */
#include "system_nrf.h"                              /*!< nrf7120_enga_umac System Library                                     */

#endif                                               /*!< NRF_UMAC                                                             */


#ifdef NRF_UMAC

  #define NRF_DOMAIN                    NRF_DOMAIN_WIFICORE
  #define NRF_PROCESSOR                 NRF_PROCESSOR_UMAC
  #define NRF_OWNER                     NRF_OWNER_APPLICATION

#endif                                               /*!< NRF_UMAC                                                             */


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

#define NRF_UMAC_VPRCLIC_BASE             0xF0000000UL

/* =========================================================================================================================== */
/* ================                                  Peripheral Declaration                                  ================ */
/* =========================================================================================================================== */

#define NRF_UMAC_VPRCLIC                  ((NRF_CLIC_Type*)                     NRF_UMAC_VPRCLIC_BASE)

/* =========================================================================================================================== */
/* ================                                  Local Domain Remapping                                  ================ */
/* =========================================================================================================================== */

#ifdef NRF_UMAC                                      /*!< Remap NRF_DOMAIN_X instances to NRF_X symbol for ease of use.        */
  #define NRF_VPRCLIC                             NRF_UMAC_VPRCLIC
#endif                                               /*!< NRF_UMAC                                                             */

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
#endif /* NRF7120_ENGA_UMAC_H */

