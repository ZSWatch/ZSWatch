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

#ifndef SYSTEM_CONFIG_SAU_H
#define SYSTEM_CONFIG_SAU_H

#include "nrf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function that configures default SAU settings in cores with 4 or more SAU regions. */
static inline void configure_default_sau(void)
{
    #if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    /* Configure SAU with default region settings. */

    /* Region 0: Mark [0x00000000:0x10000000> NS */
    SAU->RNR  = 0;
    SAU->RBAR = 0x00000000ul;
    SAU->RLAR = (0x0FFFFFFFul & (SAU_RLAR_LADDR_Msk)) | (1 << SAU_RLAR_ENABLE_Pos);

    /* Region 1: Not used, use this region to create a NSC region */

    /* Region 2: Mark [0x20000000:0x30000000> NS */
    SAU->RNR  = 2;
    SAU->RBAR = 0x20000000ul;
    SAU->RLAR = (0x2FFFFFFFul & (SAU_RLAR_LADDR_Msk)) | (1 << SAU_RLAR_ENABLE_Pos);

    /* Region 3: Mark [0x40000000:0xFFFFFFFF> NS, reuse to create data RAM NSC region by lowering BADDR */
    SAU->RNR  = 3;
    SAU->RBAR = 0x40000000ul;
    SAU->RLAR = (0xFFFFFFFFul & (SAU_RLAR_LADDR_Msk)) | (1 << SAU_RLAR_ENABLE_Pos);

    /* Enable SAU. */
    SAU->CTRL |= (1 << SAU_CTRL_ENABLE_Pos);
    #endif
}

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_CONFIG_SAU_H */
