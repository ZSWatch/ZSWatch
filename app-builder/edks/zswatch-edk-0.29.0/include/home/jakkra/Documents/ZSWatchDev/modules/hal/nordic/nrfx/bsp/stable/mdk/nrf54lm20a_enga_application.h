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

#ifndef NRF54LM20A_ENGA_APPLICATION_H
#define NRF54LM20A_ENGA_APPLICATION_H

#ifdef __cplusplus
    extern "C" {
#endif


#ifdef NRF_APPLICATION                               /*!< Processor information is domain local.                               */


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
  SWI00_IRQn                             = 28,       /*!< 28 SWI00                                                             */
  SWI01_IRQn                             = 29,       /*!< 29 SWI01                                                             */
  SWI02_IRQn                             = 30,       /*!< 30 SWI02                                                             */
  SWI03_IRQn                             = 31,       /*!< 31 SWI03                                                             */
  SPU00_IRQn                             = 64,       /*!< 64 SPU00                                                             */
  MPC00_IRQn                             = 65,       /*!< 65 MPC00                                                             */
  AAR00_CCM00_IRQn                       = 74,       /*!< 74 AAR00_CCM00                                                       */
  ECB00_IRQn                             = 75,       /*!< 75 ECB00                                                             */
  VPR00_IRQn                             = 76,       /*!< 76 VPR00                                                             */
  SERIAL00_IRQn                          = 77,       /*!< 77 SERIAL00                                                          */
  RRAMC_IRQn                             = 78,       /*!< 78 RRAMC                                                             */
  CTRLAP_IRQn                            = 82,       /*!< 82 CTRLAP                                                            */
  CM33SS_IRQn                            = 84,       /*!< 84 CM33SS                                                            */
  TIMER00_IRQn                           = 85,       /*!< 85 TIMER00                                                           */
  EGU00_IRQn                             = 88,       /*!< 88 EGU00                                                             */
  CRACEN_IRQn                            = 89,       /*!< 89 CRACEN                                                            */
  USBHS_IRQn                             = 90,       /*!< 90 USBHS                                                             */
  SPU10_IRQn                             = 128,      /*!< 128 SPU10                                                            */
  TIMER10_IRQn                           = 133,      /*!< 133 TIMER10                                                          */
  EGU10_IRQn                             = 135,      /*!< 135 EGU10                                                            */
  RADIO_0_IRQn                           = 138,      /*!< 138 RADIO_0                                                          */
  RADIO_1_IRQn                           = 139,      /*!< 139 RADIO_1                                                          */
  SPU20_IRQn                             = 192,      /*!< 192 SPU20                                                            */
  SERIAL20_IRQn                          = 198,      /*!< 198 SERIAL20                                                         */
  SERIAL21_IRQn                          = 199,      /*!< 199 SERIAL21                                                         */
  SERIAL22_IRQn                          = 200,      /*!< 200 SERIAL22                                                         */
  EGU20_IRQn                             = 201,      /*!< 201 EGU20                                                            */
  TIMER20_IRQn                           = 202,      /*!< 202 TIMER20                                                          */
  TIMER21_IRQn                           = 203,      /*!< 203 TIMER21                                                          */
  TIMER22_IRQn                           = 204,      /*!< 204 TIMER22                                                          */
  TIMER23_IRQn                           = 205,      /*!< 205 TIMER23                                                          */
  TIMER24_IRQn                           = 206,      /*!< 206 TIMER24                                                          */
  PDM20_IRQn                             = 208,      /*!< 208 PDM20                                                            */
  PDM21_IRQn                             = 209,      /*!< 209 PDM21                                                            */
  PWM20_IRQn                             = 210,      /*!< 210 PWM20                                                            */
  PWM21_IRQn                             = 211,      /*!< 211 PWM21                                                            */
  PWM22_IRQn                             = 212,      /*!< 212 PWM22                                                            */
  SAADC_IRQn                             = 213,      /*!< 213 SAADC                                                            */
  NFCT_IRQn                              = 214,      /*!< 214 NFCT                                                             */
  TEMP_IRQn                              = 215,      /*!< 215 TEMP                                                             */
  GPIOTE20_0_IRQn                        = 218,      /*!< 218 GPIOTE20_0                                                       */
  GPIOTE20_1_IRQn                        = 219,      /*!< 219 GPIOTE20_1                                                       */
  QDEC20_IRQn                            = 224,      /*!< 224 QDEC20                                                           */
  QDEC21_IRQn                            = 225,      /*!< 225 QDEC21                                                           */
  GRTC_0_IRQn                            = 226,      /*!< 226 GRTC_0                                                           */
  GRTC_1_IRQn                            = 227,      /*!< 227 GRTC_1                                                           */
  GRTC_2_IRQn                            = 228,      /*!< 228 GRTC_2                                                           */
  GRTC_3_IRQn                            = 229,      /*!< 229 GRTC_3                                                           */
  TDM_IRQn                               = 232,      /*!< 232 TDM                                                              */
  SERIAL23_IRQn                          = 237,      /*!< 237 SERIAL23                                                         */
  SERIAL24_IRQn                          = 238,      /*!< 238 SERIAL24                                                         */
  TAMPC_IRQn                             = 239,      /*!< 239 TAMPC                                                            */
  SPU30_IRQn                             = 256,      /*!< 256 SPU30                                                            */
  SERIAL30_IRQn                          = 260,      /*!< 260 SERIAL30                                                         */
  COMP_LPCOMP_IRQn                       = 262,      /*!< 262 COMP_LPCOMP                                                      */
  WDT30_IRQn                             = 264,      /*!< 264 WDT30                                                            */
  WDT31_IRQn                             = 265,      /*!< 265 WDT31                                                            */
  GPIOTE30_0_IRQn                        = 268,      /*!< 268 GPIOTE30_0                                                       */
  GPIOTE30_1_IRQn                        = 269,      /*!< 269 GPIOTE30_1                                                       */
  CLOCK_POWER_IRQn                       = 270,      /*!< 270 CLOCK_POWER                                                      */
  VREGUSB_IRQn                           = 289,      /*!< 289 VREGUSB                                                          */
} IRQn_Type;

/* ==================================================== Interrupt Aliases ==================================================== */
#define AAR00_IRQn                    AAR00_CCM00_IRQn
#define AAR00_IRQHandler              AAR00_CCM00_IRQHandler
#define CCM00_IRQn                    AAR00_CCM00_IRQn
#define CCM00_IRQHandler              AAR00_CCM00_IRQHandler
#define SPIM00_IRQn                   SERIAL00_IRQn
#define SPIM00_IRQHandler             SERIAL00_IRQHandler
#define SPIS00_IRQn                   SERIAL00_IRQn
#define SPIS00_IRQHandler             SERIAL00_IRQHandler
#define UARTE00_IRQn                  SERIAL00_IRQn
#define UARTE00_IRQHandler            SERIAL00_IRQHandler
#define CPUC_IRQn                     CM33SS_IRQn
#define CPUC_IRQHandler               CM33SS_IRQHandler
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
#define COMP_IRQn                     COMP_LPCOMP_IRQn
#define COMP_IRQHandler               COMP_LPCOMP_IRQHandler
#define LPCOMP_IRQn                   COMP_LPCOMP_IRQn
#define LPCOMP_IRQHandler             COMP_LPCOMP_IRQHandler
#define CLOCK_IRQn                    CLOCK_POWER_IRQn
#define CLOCK_IRQHandler              CLOCK_POWER_IRQHandler
#define POWER_IRQn                    CLOCK_POWER_IRQn
#define POWER_IRQHandler              CLOCK_POWER_IRQHandler

/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* =========================== Configuration of the ARM Cortex-M33 Processor and Core Peripherals ============================ */
#define __CM33_REV                  r0p4             /*!< CM33 Core Revision                                                   */
#define __DSP_PRESENT                  1             /*!< DSP present or not                                                   */
#define __NVIC_PRIO_BITS               3             /*!< Number of Bits used for Priority Levels                              */
#define __VTOR_PRESENT                 1             /*!< CPU supports alternate Vector Table address                          */
#define __MPU_PRESENT                  1             /*!< MPU present                                                          */
#define __FPU_PRESENT                  1             /*!< FPU present                                                          */
#define __FPU_DP                       0             /*!< Double Precision FPU                                                 */
#define __INTERRUPTS_MAX             270             /*!< Size of interrupt vector table                                       */
#define __Vendor_SysTickConfig         0             /*!< Vendor SysTick Config implementation is used                         */
#define __SAUREGION_PRESENT            1             /*!< SAU present                                                          */
#define __NUM_SAUREGIONS               4             /*!< Number of regions                                                    */

#include "core_cm33.h"                               /*!< ARM Cortex-M33 processor and core peripherals                        */
#include "system_nrf.h"                              /*!< nrf54lm20a_enga_application System Library                           */

#endif                                               /*!< NRF_APPLICATION                                                      */


#ifdef NRF_APPLICATION

  #define NRF_DOMAIN                    NRF_DOMAIN_NONE
  #define NRF_PROCESSOR                 NRF_PROCESSOR_CM33

#endif                                               /*!< NRF_APPLICATION                                                      */


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

#define NRF_APPLICATION_ICACHEDATA_S_BASE 0x02F00000UL
#define NRF_APPLICATION_ICACHEINFO_S_BASE 0x02F10000UL
#define NRF_APPLICATION_TPIU_NS_BASE      0xE0040000UL
#define NRF_APPLICATION_ETM_NS_BASE       0xE0041000UL
#define NRF_APPLICATION_CPUC_S_BASE       0xE0080000UL
#define NRF_APPLICATION_ICACHE_S_BASE     0xE0082000UL
#define NRF_APPLICATION_SWI00_S_BASE      0x5001C000UL
#define NRF_APPLICATION_SWI01_S_BASE      0x5001D000UL
#define NRF_APPLICATION_SWI02_S_BASE      0x5001E000UL
#define NRF_APPLICATION_SWI03_S_BASE      0x5001F000UL

/* =========================================================================================================================== */
/* ================                                  Peripheral Declaration                                  ================ */
/* =========================================================================================================================== */

#define NRF_APPLICATION_ICACHEDATA_S      ((NRF_CACHEDATA_Type*)                NRF_APPLICATION_ICACHEDATA_S_BASE)
#define NRF_APPLICATION_ICACHEINFO_S      ((NRF_CACHEINFO_Type*)                NRF_APPLICATION_ICACHEINFO_S_BASE)
#define NRF_APPLICATION_TPIU_NS           ((NRF_TPIU_Type*)                     NRF_APPLICATION_TPIU_NS_BASE)
#define NRF_APPLICATION_ETM_NS            ((NRF_ETM_Type*)                      NRF_APPLICATION_ETM_NS_BASE)
#define NRF_APPLICATION_CPUC_S            ((NRF_CPUC_Type*)                     NRF_APPLICATION_CPUC_S_BASE)
#define NRF_APPLICATION_ICACHE_S          ((NRF_CACHE_Type*)                    NRF_APPLICATION_ICACHE_S_BASE)
#define NRF_APPLICATION_SWI00_S           ((NRF_SWI_Type*)                      NRF_APPLICATION_SWI00_S_BASE)
#define NRF_APPLICATION_SWI01_S           ((NRF_SWI_Type*)                      NRF_APPLICATION_SWI01_S_BASE)
#define NRF_APPLICATION_SWI02_S           ((NRF_SWI_Type*)                      NRF_APPLICATION_SWI02_S_BASE)
#define NRF_APPLICATION_SWI03_S           ((NRF_SWI_Type*)                      NRF_APPLICATION_SWI03_S_BASE)

/* =========================================================================================================================== */
/* ================                                    TrustZone Remapping                                    ================ */
/* =========================================================================================================================== */

#ifdef NRF_TRUSTZONE_NONSECURE                       /*!< Remap NRF_X_NS instances to NRF_X symbol for ease of use.            */
  #define NRF_APPLICATION_TPIU                    NRF_APPLICATION_TPIU_NS
  #define NRF_APPLICATION_ETM                     NRF_APPLICATION_ETM_NS
#else                                                /*!< Remap NRF_X_S instances to NRF_X symbol for ease of use.             */
  #define NRF_APPLICATION_ICACHEDATA              NRF_APPLICATION_ICACHEDATA_S
  #define NRF_APPLICATION_ICACHEINFO              NRF_APPLICATION_ICACHEINFO_S
  #define NRF_APPLICATION_TPIU                    NRF_APPLICATION_TPIU_NS
  #define NRF_APPLICATION_ETM                     NRF_APPLICATION_ETM_NS
  #define NRF_APPLICATION_CPUC                    NRF_APPLICATION_CPUC_S
  #define NRF_APPLICATION_ICACHE                  NRF_APPLICATION_ICACHE_S
  #define NRF_APPLICATION_SWI00                   NRF_APPLICATION_SWI00_S
  #define NRF_APPLICATION_SWI01                   NRF_APPLICATION_SWI01_S
  #define NRF_APPLICATION_SWI02                   NRF_APPLICATION_SWI02_S
  #define NRF_APPLICATION_SWI03                   NRF_APPLICATION_SWI03_S
#endif                                               /*!< NRF_TRUSTZONE_NONSECURE                                              */

/* =========================================================================================================================== */
/* ================                                  Local Domain Remapping                                  ================ */
/* =========================================================================================================================== */

#ifdef NRF_APPLICATION                               /*!< Remap NRF_DOMAIN_X instances to NRF_X symbol for ease of use.        */
  #ifdef NRF_TRUSTZONE_NONSECURE                     /*!< Remap only nonsecure instances.                                      */
    #define NRF_TPIU                              NRF_APPLICATION_TPIU
    #define NRF_ETM                               NRF_APPLICATION_ETM
  #else                                              /*!< Remap all instances.                                                 */
    #define NRF_ICACHEDATA                        NRF_APPLICATION_ICACHEDATA
    #define NRF_ICACHEINFO                        NRF_APPLICATION_ICACHEINFO
    #define NRF_TPIU                              NRF_APPLICATION_TPIU
    #define NRF_ETM                               NRF_APPLICATION_ETM
    #define NRF_CPUC                              NRF_APPLICATION_CPUC
    #define NRF_ICACHE                            NRF_APPLICATION_ICACHE
    #define NRF_SWI00                             NRF_APPLICATION_SWI00
    #define NRF_SWI01                             NRF_APPLICATION_SWI01
    #define NRF_SWI02                             NRF_APPLICATION_SWI02
    #define NRF_SWI03                             NRF_APPLICATION_SWI03
  #endif                                             /*!< NRF_TRUSTZONE_NONSECURE                                              */
#endif                                               /*!< NRF_APPLICATION                                                      */

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
#endif /* NRF54LM20A_ENGA_APPLICATION_H */

