/*
 * Copyright (c) 2025, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NRFX_INTERCONNECT_IPCT_H__
#define NRFX_INTERCONNECT_IPCT_H__

#include "nrfx_interconnect_apb_haltium_global.h"
#include "nrfx_interconnect_ipct_haltium_global.h"

#if defined(NRF_RADIOCORE)
    #include "nrfx_interconnect_apb_haltium_radiocore.h"
    #include "nrfx_interconnect_ipct_haltium_radiocore.h"
#elif defined(NRF_APPLICATION)
    #include "nrfx_interconnect_apb_haltium_application.h"
    #include "nrfx_interconnect_ipct_haltium_application.h"
#elif defined(NRF_PPR)
    #include "nrfx_interconnect_apb_haltium_ppr.h"
    #include "nrfx_interconnect_ipct_haltium_ppr.h"
#elif defined(NRF_FLPR)
    #include "nrfx_interconnect_apb_haltium_flpr.h"
    #include "nrfx_interconnect_ipct_haltium_flpr.h"
#else
    #include "nrfx_interconnect_apb_haltium_ext.h"
    #include "nrfx_interconnect_ipct_haltium_ext.h"
#endif

#endif // NRFX_INTERCONNECT_IPCT_H__
