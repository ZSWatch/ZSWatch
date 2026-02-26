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

#ifndef SYSTEM_NRF54L_APPROTECT_H
#define SYSTEM_NRF54L_APPROTECT_H
#ifndef NRF_TRUSTZONE_NONSECURE
#include "nrf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TAMPC_SIGNAL_IS_LOCKED                                                                                         \
    (TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Enabled << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Pos)
#define TAMPC_SIGNAL_IS_HIGH                                                                                            \
    (TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_High << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Pos)

#define TAMPC_SIGNAL_CLEAR_WRITEPROTECTION                                                                             \
    (TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_WRITEPROTECTION_Clear                                                         \
         << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_WRITEPROTECTION_Pos |                                                  \
     TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_KEY << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_Pos)

#define TAMPC_SIGNAL_LOCK                                                                                              \
    (TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Low << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Pos |                  \
     TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Enabled << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Pos |                \
     TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_KEY << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_Pos)
#define TAMPC_SIGNAL_OPEN                                                                                              \
    (TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_High << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Pos |                  \
     TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Disabled << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Pos |                \
     TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_KEY << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_Pos)


static inline void nrf54l_handle_approtect_signal(volatile uint32_t * signal_ctrl)
{
    if ((*signal_ctrl & TAMPC_SIGNAL_IS_LOCKED) != 0)
    {
        if ((*signal_ctrl & TAMPC_SIGNAL_IS_HIGH) != 0)
        {
            /* Signal is locked open. */
            /* Do hard reset, invalid state. */
            
            /* Temporary fix - use WDT to trigger hard reset. */
            /* Replace with proper hard reset task later if added. */
            /* Make sure WDT is always running independent of CPU state. */
            NRF_WDT30->CONFIG = 9;

            /* Set minimum tick length */
            NRF_WDT30->CRV = 0xF;

            /* Start WDT */
            NRF_WDT30->TASKS_START = 1;

            /* Wait for timer to elapse */
            while(1);
        }
        /* Do nothing, signal is locked by previous session / hardware. */
        return;
    }

#if defined(ENABLE_APPROTECT)
    /* Prevent processor from unlocking APPROTECT soft branch after this point. */
    *signal_ctrl = TAMPC_SIGNAL_CLEAR_WRITEPROTECTION;
    *signal_ctrl = TAMPC_SIGNAL_LOCK;
#elif defined(ENABLE_AUTHENTICATED_APPROTECT)
    /* Do nothing, TAMPC should already be in correct state. */
#else
    /* Disable APPROTECT */
    *signal_ctrl = TAMPC_SIGNAL_CLEAR_WRITEPROTECTION;
    *signal_ctrl = TAMPC_SIGNAL_OPEN;
#endif
}

static inline void nrf54l_handle_secureapprotect_signal(volatile uint32_t * signal_ctrl)
{
    if ((*signal_ctrl & TAMPC_SIGNAL_IS_LOCKED) != 0)
    {
        if ((*signal_ctrl & TAMPC_SIGNAL_IS_HIGH) != 0)
        {
            /* Signal is locked open. */
            /* Do hard reset, invalid state. */
            
            /* Temporary fix - use WDT to trigger hard reset. */
            /* Replace with proper hard reset task later if added. */
            /* Make sure WDT is always running independent of CPU state. */
            NRF_WDT30->CONFIG = 9;

            /* Set minimum tick length */
            NRF_WDT30->CRV = 0xF;

            /* Start WDT */
            NRF_WDT30->TASKS_START = 1;

            /* Wait for timer to elapse */
            while(1);
        }
        /* Do nothing, signal is locked by previous session / hardware. */
        return;
    }

#if defined(ENABLE_SECUREAPPROTECT)
    /* Prevent processor from unlocking APPROTECT soft branch after this point. */
    *signal_ctrl = TAMPC_SIGNAL_CLEAR_WRITEPROTECTION;
    *signal_ctrl = TAMPC_SIGNAL_LOCK;
#elif defined(ENABLE_AUTHENTICATED_SECUREAPPROTECT)
    /* Do nothing, TAMPC should already be in correct state. */
    // TODO: What about if approtect is disabled here?
#else
    /* Disable APPROTECT */
    *signal_ctrl = TAMPC_SIGNAL_CLEAR_WRITEPROTECTION;
    *signal_ctrl = TAMPC_SIGNAL_OPEN;
#endif
}


/* Function that handles firmware-driven enabling or disabling of APPROTECT on devices where it is supported.
        If ENABLE_APPROTECT is defined, the FW will lock the fw branch of the APPROTECT mechanism,
                            preventing it from being opened.
        Otherwise, the fw branch state is loaded from UICR, emulating the legacy APPROTECT behavior.

         The same mechanism is implemented for SECURE APPROTECT, with the macros
         ENABLE_SECURE_APPROTECT and ENABLE_SECURE_APPROTECT_USER_HANDLING. */
static inline void nrf54l_handle_approtect(void)
{
#if defined (NRF54LS05B_ENGA_XXAA)
    nrf54l_handle_approtect_signal(&NRF_TAMPC->PROTECT.DOMAIN[0].DBGEN.CTRL);
    nrf54l_handle_approtect_signal(&NRF_TAMPC->PROTECT.DOMAIN[0].NIDEN.CTRL);
#else
    nrf54l_handle_approtect_signal(&NRF_TAMPC->PROTECT.DOMAIN[0].DBGEN.CTRL);
    nrf54l_handle_approtect_signal(&NRF_TAMPC->PROTECT.DOMAIN[0].NIDEN.CTRL);
    nrf54l_handle_secureapprotect_signal(&NRF_TAMPC->PROTECT.DOMAIN[0].SPIDEN.CTRL);
    nrf54l_handle_secureapprotect_signal(&NRF_TAMPC->PROTECT.DOMAIN[0].SPNIDEN.CTRL);
#endif

    /* Handle AUX AP*/
    nrf54l_handle_approtect_signal(&NRF_TAMPC->PROTECT.AP[0].DBGEN.CTRL);
}

#ifdef __cplusplus
}
#endif
#endif
#endif /* SYSTEM_NRF54L_APPROTECT_H */
