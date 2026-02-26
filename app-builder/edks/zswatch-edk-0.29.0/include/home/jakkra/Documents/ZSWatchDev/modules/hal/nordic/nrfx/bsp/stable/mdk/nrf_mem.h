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

#ifndef NRF_MEM_H_
#define NRF_MEM_H_

#if defined(NRF51422_XXAA)
    #include "nrf51422_xxaa_memory.h"
#elif defined(NRF51422_XXAB)
    #include "nrf51422_xxab_memory.h"
#elif defined(NRF51422_XXAC)
    #include "nrf51422_xxac_memory.h"
#elif defined(NRF51801_XXAB)
    #include "nrf51801_xxab_memory.h"
#elif defined(NRF51802_XXAA)
    #include "nrf51802_xxaa_memory.h"
#elif defined(NRF51822_XXAA)
    #include "nrf51822_xxaa_memory.h"
#elif defined(NRF51822_XXAB)
    #include "nrf51822_xxab_memory.h"
#elif defined(NRF51822_XXAC)
    #include "nrf51822_xxac_memory.h"
#elif defined(NRF51824_XXAA)
    #include "nrf51824_xxaa_memory.h"
#elif defined(NRF52805_XXAA)
    #include "nrf52805_xxaa_memory.h"
#elif defined(NRF52810_XXAA)
    #include "nrf52810_xxaa_memory.h"
#elif defined(NRF52811_XXAA)
    #include "nrf52811_xxaa_memory.h"
#elif defined(NRF52820_XXAA)
    #include "nrf52820_xxaa_memory.h"
#elif defined(NRF52832_XXAA)
    #include "nrf52832_xxaa_memory.h"
#elif defined(NRF52832_XXAB)
    #include "nrf52832_xxab_memory.h"
#elif defined(NRF52833_XXAA)
    #include "nrf52833_xxaa_memory.h"
#elif defined(NRF52840_XXAA)
    #include "nrf52840_xxaa_memory.h"
#elif defined(NRF5340_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf5340_xxaa_application_memory.h"
    #endif
    #if defined(NRF_NETWORK)
        #include "nrf5340_xxaa_network_memory.h"
    #endif
#elif defined(NRF54LV10A_ENGA_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf54lv10a_enga_xxaa_application_memory.h"
    #endif
    #if defined(NRF_FLPR)
        #include "nrf54lv10a_enga_xxaa_flpr_memory.h"
    #endif
#elif defined(NRF54L05_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf54l05_xxaa_application_memory.h"
    #endif
    #if defined(NRF_FLPR)
        #include "nrf54l05_xxaa_flpr_memory.h"
    #endif
#elif defined(NRF54L10_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf54l10_xxaa_application_memory.h"
    #endif
    #if defined(NRF_FLPR)
        #include "nrf54l10_xxaa_flpr_memory.h"
    #endif
#elif defined(NRF54L15_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf54l15_xxaa_application_memory.h"
    #endif
    #if defined(NRF_FLPR)
        #include "nrf54l15_xxaa_flpr_memory.h"
    #endif
#elif defined(NRF54LM20A_ENGA_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf54lm20a_enga_xxaa_application_memory.h"
    #endif
    #if defined(NRF_FLPR)
        #include "nrf54lm20a_enga_xxaa_flpr_memory.h"
    #endif
#elif defined(NRF54LS05B_ENGA_XXAA)
    #include "nrf54ls05b_enga_xxaa_application_memory.h"
#elif defined(NRF54H20_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf54h20_xxaa_application_memory.h"
    #endif
    #if defined(NRF_RADIOCORE)
        #include "nrf54h20_xxaa_radiocore_memory.h"
    #endif
    #if defined(NRF_PPR)
        #include "nrf54h20_xxaa_ppr_memory.h"
    #endif
    #if defined(NRF_FLPR)
        #include "nrf54h20_xxaa_flpr_memory.h"
    #endif
#elif defined(NRF7120_ENGA_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf7120_enga_xxaa_application_memory.h"
    #endif
    #if defined(NRF_FLPR)
        #include "nrf7120_enga_xxaa_flpr_memory.h"
    #endif
    #if defined(NRF_UMAC)
        #include "nrf7120_enga_xxaa_umac_memory.h"
    #endif
    #if defined(NRF_LMAC)
        #include "nrf7120_enga_xxaa_lmac_memory.h"
    #endif
#elif defined(NRF9120_XXAA)
    #include "nrf9120_xxaa_memory.h"
#elif defined(NRF9160_XXAA)
    #include "nrf9160_xxaa_memory.h"
#elif defined(NRF9230_ENGB_XXAA)
    #if defined(NRF_APPLICATION)
        #include "nrf9230_engb_xxaa_application_memory.h"
    #endif
    #if defined(NRF_RADIOCORE)
        #include "nrf9230_engb_xxaa_radiocore_memory.h"
    #endif
    #if defined(NRF_PPR)
        #include "nrf9230_engb_xxaa_ppr_memory.h"
    #endif
    #if defined(NRF_FLPR)
        #include "nrf9230_engb_xxaa_flpr_memory.h"
    #endif
#else
    #error "Device must be defined. See nrf_mem.h."
#endif


#ifdef  __STARTUP_CONFIG
    #include "startup_config.h"
    #ifndef __STARTUP_CONFIG_STACK_ALIGNEMENT
        #define __STARTUP_CONFIG_STACK_ALIGNEMENT 3
    #endif
#endif

#ifndef __STACK_SIZE
    #if defined(__STARTUP_CONFIG_STACK_SIZE)
        #define __STACK_SIZE __STARTUP_CONFIG_STACK_SIZE
    #else
        #define __STACK_SIZE __DEFAULT_STACK_SIZE
    #endif
#endif

#ifndef __STACK_ALIGNMENT
    #if defined(__STARTUP_CONFIG_STACK_ALIGNEMENT)
        #define __STACK_ALIGNMENT __STARTUP_CONFIG_STACK_ALIGNEMENT
    #else
        #define __STACK_ALIGNMENT 3
    #endif
#endif

#ifndef __HEAP_SIZE
    #if defined(__STARTUP_CONFIG_HEAP_SIZE)
        #define __HEAP_SIZE __STARTUP_CONFIG_HEAP_SIZE
    #else
        #define __HEAP_SIZE __DEFAULT_HEAP_SIZE
    #endif
#endif

#ifndef __HEAP_ALIGNMENT
    #define __HEAP_ALIGNMENT __STACK_ALIGNMENT
#endif

#endif
