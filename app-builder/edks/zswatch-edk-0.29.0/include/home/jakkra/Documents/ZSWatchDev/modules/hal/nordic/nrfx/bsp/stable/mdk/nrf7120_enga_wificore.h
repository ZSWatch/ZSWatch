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

#ifndef NRF7120_ENGA_WIFICORE_H
#define NRF7120_ENGA_WIFICORE_H

#ifdef __cplusplus
    extern "C" {
#endif


#ifdef NRF_WIFICORE

  #define NRF_DOMAIN                    NRF_DOMAIN_WIFICORE
  #define NRF_OWNER                     NRF_OWNER_APPLICATION

#endif                                               /*!< NRF_WIFICORE                                                         */


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

#define NRF_WIFICORE_LMAC_VPR_BASE        0x48000000UL
#define NRF_WIFICORE_UMAC_VPR_BASE        0x48004000UL
#define NRF_WIFICORE_MVDMA_BASE           0x48008000UL
#define NRF_WIFICORE_LRCCONF_LRC0_BASE    0x4800B000UL
#define NRF_WIFICORE_PCGCM_LRC0_BASE      0x4800C000UL
#define NRF_WIFICORE_ANTSWC_BASE          0x4800D000UL
#define NRF_WIFICORE_BELLBOARD_BASE       0x40074000UL

/* =========================================================================================================================== */
/* ================                                  Peripheral Declaration                                  ================ */
/* =========================================================================================================================== */

#define NRF_WIFICORE_LMAC_VPR             ((NRF_VPR_Type*)                      NRF_WIFICORE_LMAC_VPR_BASE)
#define NRF_WIFICORE_UMAC_VPR             ((NRF_VPR_Type*)                      NRF_WIFICORE_UMAC_VPR_BASE)
#define NRF_WIFICORE_MVDMA                ((NRF_MVDMA_Type*)                    NRF_WIFICORE_MVDMA_BASE)
#define NRF_WIFICORE_LRCCONF_LRC0         ((NRF_LRCCONF_Type*)                  NRF_WIFICORE_LRCCONF_LRC0_BASE)
#define NRF_WIFICORE_PCGCM_LRC0           ((NRF_PCGCMASTER_Type*)               NRF_WIFICORE_PCGCM_LRC0_BASE)
#define NRF_WIFICORE_ANTSWC               ((NRF_ANTSWC_Type*)                   NRF_WIFICORE_ANTSWC_BASE)
#define NRF_WIFICORE_BELLBOARD            ((NRF_BELLBOARD_Type*)                NRF_WIFICORE_BELLBOARD_BASE)

/* =========================================================================================================================== */
/* ================                                  Local Domain Remapping                                  ================ */
/* =========================================================================================================================== */

#ifdef NRF_WIFICORE                                  /*!< Remap NRF_DOMAIN_X instances to NRF_X symbol for ease of use.        */
  #define NRF_LMAC_VPR                            NRF_WIFICORE_LMAC_VPR
  #define NRF_UMAC_VPR                            NRF_WIFICORE_UMAC_VPR
  #define NRF_MVDMA                               NRF_WIFICORE_MVDMA
  #define NRF_LRCCONF_LRC0                        NRF_WIFICORE_LRCCONF_LRC0
  #define NRF_PCGCM_LRC0                          NRF_WIFICORE_PCGCM_LRC0
  #define NRF_ANTSWC                              NRF_WIFICORE_ANTSWC
  #define NRF_BELLBOARD                           NRF_WIFICORE_BELLBOARD
#endif                                               /*!< NRF_WIFICORE                                                         */

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
#endif /* NRF7120_ENGA_WIFICORE_H */

