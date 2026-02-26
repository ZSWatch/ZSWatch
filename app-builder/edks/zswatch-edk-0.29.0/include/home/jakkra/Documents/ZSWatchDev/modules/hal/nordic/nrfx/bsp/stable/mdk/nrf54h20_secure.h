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

#ifndef NRF54H20_SECURE_H
#define NRF54H20_SECURE_H

#ifdef __cplusplus
    extern "C" {
#endif


#ifdef NRF_SECURE                                    /*!< Processor information is domain local.                               */


/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */

typedef enum {
/* ===================================================== Core Interrupts ===================================================== */
  Reset_IRQn                             = -15,      /*!< -15 Reset Vector, invoked on Power up and warm reset                 */
  NonMaskableInt_IRQn                    = -14,      /*!< -14 Non maskable Interrupt, cannot be stopped or preempted           */
  HardFault_IRQn                         = -13,      /*!< -13 Hard Fault, all classes of Fault                                 */
  MemoryManagement_IRQn                  = -12,      /*!< -12 Memory Management, MPU mismatch, including Access Violation and No
                                                          Match*/
  BusFault_IRQn                          = -11,      /*!< -11 Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                          related Fault*/
  UsageFault_IRQn                        = -10,      /*!< -10 Usage Fault, i.e. Undef Instruction, Illegal State Transition    */
  SecureFault_IRQn                       = -9,       /*!<  -9 Secure Fault Handler                                             */
  SVCall_IRQn                            = -5,       /*!<  -5 System Service Call via SVC instruction                          */
  DebugMonitor_IRQn                      = -4,       /*!<  -4 Debug Monitor                                                    */
  PendSV_IRQn                            = -2,       /*!<  -2 Pendable request for system service                              */
  SysTick_IRQn                           = -1,       /*!<  -1 System Tick Timer                                                */
/* ============================================== Processor Specific Interrupts ============================================== */
  BELLBOARD_0_IRQn                       = 96,       /*!< 96 BELLBOARD_0                                                       */
  BELLBOARD_1_IRQn                       = 97,       /*!< 97 BELLBOARD_1                                                       */
  BELLBOARD_2_IRQn                       = 98,       /*!< 98 BELLBOARD_2                                                       */
  BELLBOARD_3_IRQn                       = 99,       /*!< 99 BELLBOARD_3                                                       */
  GPIOTE130_0_IRQn                       = 104,      /*!< 104 GPIOTE130_0                                                      */
  GPIOTE130_1_IRQn                       = 105,      /*!< 105 GPIOTE130_1                                                      */
  GRTC_0_IRQn                            = 108,      /*!< 108 GRTC_0                                                           */
  GRTC_1_IRQn                            = 109,      /*!< 109 GRTC_1                                                           */
  GRTC_2_IRQn                            = 110,      /*!< 110 GRTC_2                                                           */
  TBM_IRQn                               = 127,      /*!< 127 TBM                                                              */
  USBHS_IRQn                             = 134,      /*!< 134 USBHS                                                            */
  EXMIF_IRQn                             = 149,      /*!< 149 EXMIF                                                            */
  IPCT120_0_IRQn                         = 209,      /*!< 209 IPCT120_0                                                        */
  VPR121_IRQn                            = 212,      /*!< 212 VPR121                                                           */
  CAN120_IRQn                            = 216,      /*!< 216 CAN120                                                           */
  MVDMA120_IRQn                          = 217,      /*!< 217 MVDMA120                                                         */
  TIMER120_IRQn                          = 226,      /*!< 226 TIMER120                                                         */
  TIMER121_IRQn                          = 227,      /*!< 227 TIMER121                                                         */
  PWM120_IRQn                            = 228,      /*!< 228 PWM120                                                           */
  SPIS120_IRQn                           = 229,      /*!< 229 SPIS120                                                          */
  SPIM120_UARTE120_IRQn                  = 230,      /*!< 230 SPIM120_UARTE120                                                 */
  SPIM121_IRQn                           = 231,      /*!< 231 SPIM121                                                          */
  VPR130_IRQn                            = 264,      /*!< 264 VPR130                                                           */
  IPCT130_0_IRQn                         = 289,      /*!< 289 IPCT130_0                                                        */
  RTC130_IRQn                            = 296,      /*!< 296 RTC130                                                           */
  RTC131_IRQn                            = 297,      /*!< 297 RTC131                                                           */
  WDT131_IRQn                            = 299,      /*!< 299 WDT131                                                           */
  WDT132_IRQn                            = 300,      /*!< 300 WDT132                                                           */
  EGU130_IRQn                            = 301,      /*!< 301 EGU130                                                           */
  SAADC_IRQn                             = 386,      /*!< 386 SAADC                                                            */
  COMP_LPCOMP_IRQn                       = 387,      /*!< 387 COMP_LPCOMP                                                      */
  TEMP_IRQn                              = 388,      /*!< 388 TEMP                                                             */
  NFCT_IRQn                              = 389,      /*!< 389 NFCT                                                             */
  TDM130_IRQn                            = 402,      /*!< 402 TDM130                                                           */
  PDM_IRQn                               = 403,      /*!< 403 PDM                                                              */
  QDEC130_IRQn                           = 404,      /*!< 404 QDEC130                                                          */
  QDEC131_IRQn                           = 405,      /*!< 405 QDEC131                                                          */
  TDM131_IRQn                            = 407,      /*!< 407 TDM131                                                           */
  TIMER130_IRQn                          = 418,      /*!< 418 TIMER130                                                         */
  TIMER131_IRQn                          = 419,      /*!< 419 TIMER131                                                         */
  PWM130_IRQn                            = 420,      /*!< 420 PWM130                                                           */
  SERIAL0_IRQn                           = 421,      /*!< 421 SERIAL0                                                          */
  SERIAL1_IRQn                           = 422,      /*!< 422 SERIAL1                                                          */
  TIMER132_IRQn                          = 434,      /*!< 434 TIMER132                                                         */
  TIMER133_IRQn                          = 435,      /*!< 435 TIMER133                                                         */
  PWM131_IRQn                            = 436,      /*!< 436 PWM131                                                           */
  SERIAL2_IRQn                           = 437,      /*!< 437 SERIAL2                                                          */
  SERIAL3_IRQn                           = 438,      /*!< 438 SERIAL3                                                          */
  TIMER134_IRQn                          = 450,      /*!< 450 TIMER134                                                         */
  TIMER135_IRQn                          = 451,      /*!< 451 TIMER135                                                         */
  PWM132_IRQn                            = 452,      /*!< 452 PWM132                                                           */
  SERIAL4_IRQn                           = 453,      /*!< 453 SERIAL4                                                          */
  SERIAL5_IRQn                           = 454,      /*!< 454 SERIAL5                                                          */
  TIMER136_IRQn                          = 466,      /*!< 466 TIMER136                                                         */
  TIMER137_IRQn                          = 467,      /*!< 467 TIMER137                                                         */
  PWM133_IRQn                            = 468,      /*!< 468 PWM133                                                           */
  SERIAL6_IRQn                           = 469,      /*!< 469 SERIAL6                                                          */
  SERIAL7_IRQn                           = 470,      /*!< 470 SERIAL7                                                          */
} IRQn_Type;

/* ==================================================== Interrupt Aliases ==================================================== */
#define SPIM120_IRQn                  SPIM120_UARTE120_IRQn
#define SPIM120_IRQHandler            SPIM120_UARTE120_IRQHandler
#define UARTE120_IRQn                 SPIM120_UARTE120_IRQn
#define UARTE120_IRQHandler           SPIM120_UARTE120_IRQHandler
#define COMP_IRQn                     COMP_LPCOMP_IRQn
#define COMP_IRQHandler               COMP_LPCOMP_IRQHandler
#define LPCOMP_IRQn                   COMP_LPCOMP_IRQn
#define LPCOMP_IRQHandler             COMP_LPCOMP_IRQHandler
#define SPIM130_IRQn                  SERIAL0_IRQn
#define SPIM130_IRQHandler            SERIAL0_IRQHandler
#define SPIS130_IRQn                  SERIAL0_IRQn
#define SPIS130_IRQHandler            SERIAL0_IRQHandler
#define TWIM130_IRQn                  SERIAL0_IRQn
#define TWIM130_IRQHandler            SERIAL0_IRQHandler
#define TWIS130_IRQn                  SERIAL0_IRQn
#define TWIS130_IRQHandler            SERIAL0_IRQHandler
#define UARTE130_IRQn                 SERIAL0_IRQn
#define UARTE130_IRQHandler           SERIAL0_IRQHandler
#define SPIM131_IRQn                  SERIAL1_IRQn
#define SPIM131_IRQHandler            SERIAL1_IRQHandler
#define SPIS131_IRQn                  SERIAL1_IRQn
#define SPIS131_IRQHandler            SERIAL1_IRQHandler
#define TWIM131_IRQn                  SERIAL1_IRQn
#define TWIM131_IRQHandler            SERIAL1_IRQHandler
#define TWIS131_IRQn                  SERIAL1_IRQn
#define TWIS131_IRQHandler            SERIAL1_IRQHandler
#define UARTE131_IRQn                 SERIAL1_IRQn
#define UARTE131_IRQHandler           SERIAL1_IRQHandler
#define SPIM132_IRQn                  SERIAL2_IRQn
#define SPIM132_IRQHandler            SERIAL2_IRQHandler
#define SPIS132_IRQn                  SERIAL2_IRQn
#define SPIS132_IRQHandler            SERIAL2_IRQHandler
#define TWIM132_IRQn                  SERIAL2_IRQn
#define TWIM132_IRQHandler            SERIAL2_IRQHandler
#define TWIS132_IRQn                  SERIAL2_IRQn
#define TWIS132_IRQHandler            SERIAL2_IRQHandler
#define UARTE132_IRQn                 SERIAL2_IRQn
#define UARTE132_IRQHandler           SERIAL2_IRQHandler
#define SPIM133_IRQn                  SERIAL3_IRQn
#define SPIM133_IRQHandler            SERIAL3_IRQHandler
#define SPIS133_IRQn                  SERIAL3_IRQn
#define SPIS133_IRQHandler            SERIAL3_IRQHandler
#define TWIM133_IRQn                  SERIAL3_IRQn
#define TWIM133_IRQHandler            SERIAL3_IRQHandler
#define TWIS133_IRQn                  SERIAL3_IRQn
#define TWIS133_IRQHandler            SERIAL3_IRQHandler
#define UARTE133_IRQn                 SERIAL3_IRQn
#define UARTE133_IRQHandler           SERIAL3_IRQHandler
#define SPIM134_IRQn                  SERIAL4_IRQn
#define SPIM134_IRQHandler            SERIAL4_IRQHandler
#define SPIS134_IRQn                  SERIAL4_IRQn
#define SPIS134_IRQHandler            SERIAL4_IRQHandler
#define TWIM134_IRQn                  SERIAL4_IRQn
#define TWIM134_IRQHandler            SERIAL4_IRQHandler
#define TWIS134_IRQn                  SERIAL4_IRQn
#define TWIS134_IRQHandler            SERIAL4_IRQHandler
#define UARTE134_IRQn                 SERIAL4_IRQn
#define UARTE134_IRQHandler           SERIAL4_IRQHandler
#define SPIM135_IRQn                  SERIAL5_IRQn
#define SPIM135_IRQHandler            SERIAL5_IRQHandler
#define SPIS135_IRQn                  SERIAL5_IRQn
#define SPIS135_IRQHandler            SERIAL5_IRQHandler
#define TWIM135_IRQn                  SERIAL5_IRQn
#define TWIM135_IRQHandler            SERIAL5_IRQHandler
#define TWIS135_IRQn                  SERIAL5_IRQn
#define TWIS135_IRQHandler            SERIAL5_IRQHandler
#define UARTE135_IRQn                 SERIAL5_IRQn
#define UARTE135_IRQHandler           SERIAL5_IRQHandler
#define SPIM136_IRQn                  SERIAL6_IRQn
#define SPIM136_IRQHandler            SERIAL6_IRQHandler
#define SPIS136_IRQn                  SERIAL6_IRQn
#define SPIS136_IRQHandler            SERIAL6_IRQHandler
#define TWIM136_IRQn                  SERIAL6_IRQn
#define TWIM136_IRQHandler            SERIAL6_IRQHandler
#define TWIS136_IRQn                  SERIAL6_IRQn
#define TWIS136_IRQHandler            SERIAL6_IRQHandler
#define UARTE136_IRQn                 SERIAL6_IRQn
#define UARTE136_IRQHandler           SERIAL6_IRQHandler
#define SPIM137_IRQn                  SERIAL7_IRQn
#define SPIM137_IRQHandler            SERIAL7_IRQHandler
#define SPIS137_IRQn                  SERIAL7_IRQn
#define SPIS137_IRQHandler            SERIAL7_IRQHandler
#define TWIM137_IRQn                  SERIAL7_IRQn
#define TWIM137_IRQHandler            SERIAL7_IRQHandler
#define TWIS137_IRQn                  SERIAL7_IRQn
#define TWIS137_IRQHandler            SERIAL7_IRQHandler
#define UARTE137_IRQn                 SERIAL7_IRQn
#define UARTE137_IRQHandler           SERIAL7_IRQHandler

/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* =========================== Configuration of the ARM Cortex-M33 Processor and Core Peripherals ============================ */
#define __CM33_REV                  r0p4             /*!< CM33 Core Revision                                                   */
#define __DSP_PRESENT                  1             /*!< DSP present or not                                                   */
#define __NVIC_PRIO_BITS               3             /*!< Number of Bits used for Priority Levels                              */
#define __VTOR_PRESENT                 1             /*!< CPU supports alternate Vector Table address                          */
#define __MPU_PRESENT                  1             /*!< MPU present                                                          */
#define __FPU_PRESENT                  0             /*!< FPU present                                                          */
#define __FPU_DP                       0             /*!< Double Precision FPU                                                 */
#define __INTERRUPTS_MAX             480             /*!< Size of interrupt vector table                                       */
#define __Vendor_SysTickConfig         0             /*!< Vendor SysTick Config implementation is used                         */
#define __SAUREGION_PRESENT            1             /*!< SAU present                                                          */
#define __NUM_SAUREGIONS               8             /*!< Number of regions                                                    */

#include "core_cm33.h"                               /*!< ARM Cortex-M33 processor and core peripherals                        */
#include "system_nrf.h"                              /*!< nrf54h20_secure System Library                                       */

#endif                                               /*!< NRF_SECURE                                                           */


#ifdef NRF_SECURE

  #define NRF_DOMAIN                    NRF_DOMAIN_SECURE
  #define NRF_PROCESSOR                 NRF_PROCESSOR_SECURE
  #define NRF_OWNER                     NRF_OWNER_SECURE

#endif                                               /*!< NRF_SECURE                                                           */


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

#define NRF_SECURE_BELLBOARD_NS_BASE      0x4F099000UL
#define NRF_SECURE_BELLBOARD_S_BASE       0x5F099000UL
#define NRF_SECURE_IPCMAP_S_BASE          0x5F923000UL
#define NRF_SECURE_IRQMAP_S_BASE          0x5F924000UL

/* =========================================================================================================================== */
/* ================                                  Peripheral Declaration                                  ================ */
/* =========================================================================================================================== */

#define NRF_SECURE_BELLBOARD_NS           ((NRF_BELLBOARD_Type*)                NRF_SECURE_BELLBOARD_NS_BASE)
#define NRF_SECURE_BELLBOARD_S            ((NRF_BELLBOARD_Type*)                NRF_SECURE_BELLBOARD_S_BASE)
#define NRF_SECURE_IPCMAP_S               ((NRF_IPCMAP_Type*)                   NRF_SECURE_IPCMAP_S_BASE)
#define NRF_SECURE_IRQMAP_S               ((NRF_IRQMAP_Type*)                   NRF_SECURE_IRQMAP_S_BASE)

/* =========================================================================================================================== */
/* ================                                    TrustZone Remapping                                    ================ */
/* =========================================================================================================================== */

#ifdef NRF_TRUSTZONE_NONSECURE                       /*!< Remap NRF_X_NS instances to NRF_X symbol for ease of use.            */
  #define NRF_SECURE_BELLBOARD                    NRF_SECURE_BELLBOARD_NS
#else                                                /*!< Remap NRF_X_S instances to NRF_X symbol for ease of use.             */
  #define NRF_SECURE_BELLBOARD                    NRF_SECURE_BELLBOARD_S
  #define NRF_SECURE_IPCMAP                       NRF_SECURE_IPCMAP_S
  #define NRF_SECURE_IRQMAP                       NRF_SECURE_IRQMAP_S
#endif                                               /*!< NRF_TRUSTZONE_NONSECURE                                              */

/* =========================================================================================================================== */
/* ================                                  Local Domain Remapping                                  ================ */
/* =========================================================================================================================== */

#ifdef NRF_SECURE                                    /*!< Remap NRF_DOMAIN_X instances to NRF_X symbol for ease of use.        */
  #ifdef NRF_TRUSTZONE_NONSECURE                     /*!< Remap only nonsecure instances.                                      */
    #define NRF_BELLBOARD                         NRF_SECURE_BELLBOARD
  #else                                              /*!< Remap all instances.                                                 */
    #define NRF_BELLBOARD                         NRF_SECURE_BELLBOARD
    #define NRF_IPCMAP                            NRF_SECURE_IPCMAP
    #define NRF_IRQMAP                            NRF_SECURE_IRQMAP
  #endif                                             /*!< NRF_TRUSTZONE_NONSECURE                                              */
#endif                                               /*!< NRF_SECURE                                                           */

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
#endif /* NRF54H20_SECURE_H */

