/*

Copyright (c) 2009-2025 ARM Limited. All rights reserved.

    SPDX-License-Identifier: Apache-2.0

Licensed under the Apache License, Version 2.0 (the License); you may
not use this file except in compliance with the License.
You may obtain a copy of the License at

    www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an AS IS BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

NOTICE: This file has been modified by Nordic Semiconductor ASA.

*/

#ifndef SYSTEM_NRF91_APPROTECT_H
#define SYSTEM_NRF91_APPROTECT_H

#include "nrf.h"
#include "nrf91_erratas.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function that handles firmware-driven enabling or disabling of APPROTECT on devices where it is supported.
        If ENABLE_APPROTECT is defined, the FW will lock the fw branch of the APPROTECT mechanism,
                            preventing it from being opened.
        If ENABLE_APPROTECT_USER_HANDLING is defined, the FW will not write to the fw branch of the APPROTECT mechanism.
                            This allows later stages of the fw to handle APPROTECT,
                            for example to implement authenticated debug.
        Otherwise, the fw branch state is loaded from UICR.

         The same mechanism is implemented for SECURE APPROTECT, with the macros
         ENABLE_SECURE_APPROTECT and ENABLE_SECURE_APPROTECT_USER_HANDLING. */

static inline void nrf91_handle_approtect(void)
{
    if (!nrf91_errata_36())
    {
        /* Target device does not support firmware-driven approtect. */
        return;
    }
    #if defined (NRF91_ERRATA_36_PRESENT) && NRF91_ERRATA_36_PRESENT

        /* Workaround for Errata 36 "Debug and Trace: Access port gets locked in WFI and WFE" found at the Errata document
            for your device located at https://infocenter.nordicsemi.com/index.jsp  */
        #if defined (NRF_CONSTANT_LATENCY_WORKAROUND)
           NRF_POWER_S->TASKS_CONSTLAT = 1;
        #endif

        #if defined (ENABLE_APPROTECT)
            /* Prevent processor from unlocking APPROTECT soft branch after this point. */
            NRF_APPROTECT_S->APPROTECT.FORCEPROTECT = (APPROTECT_APPROTECT_FORCEPROTECT_FORCEPROTECT_Force << APPROTECT_APPROTECT_FORCEPROTECT_FORCEPROTECT_Pos);

        #elif defined (ENABLE_APPROTECT_USER_HANDLING)
                /* Do nothing, allow user code to handle APPROTECT. Use this if you want to enable authenticated debug. */

        #else
            /* Load APPROTECT soft branch from UICR.
                If UICR->APPROTECT is disabled, APPROTECT->APPROTECT will be disabled. */
            NRF_APPROTECT_S->APPROTECT.DISABLE = NRF_UICR_S->APPROTECT == UICR_APPROTECT_PALL_HwUnprotected ? APPROTECT_APPROTECT_DISABLE_DISABLE_SwUnprotected : 0ul;
        #endif

        /* Secure APPROTECT is only available for Application core. */
        #if defined (ENABLE_SECURE_APPROTECT)
            /* Prevent processor from unlocking SECURE APPROTECT soft branch after this point. */
            NRF_APPROTECT_S->SECUREAPPROTECT.FORCEPROTECT = (APPROTECT_SECUREAPPROTECT_FORCEPROTECT_FORCEPROTECT_Force << APPROTECT_SECUREAPPROTECT_FORCEPROTECT_FORCEPROTECT_Pos);

        #elif defined (ENABLE_SECURE_APPROTECT_USER_HANDLING)
                /* Do nothing, allow user code to handle SECURE APPROTECT. Use this if you want to enable authenticated debug. */

        #else
            /* Load SECURE APPROTECT soft branch from UICR.
                If UICR->SECUREAPPROTECT is disabled, APPROTECT->SECUREAPPROTECT will be disabled. */
            NRF_APPROTECT_S->SECUREAPPROTECT.DISABLE = NRF_UICR_S->SECUREAPPROTECT == UICR_SECUREAPPROTECT_PALL_HwUnprotected ? APPROTECT_SECUREAPPROTECT_DISABLE_DISABLE_SwUnprotected : 0ul;
        #endif
    #endif
}

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_NRF91_APPROTECT_H */
