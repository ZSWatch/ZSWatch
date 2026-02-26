/*
 * Copyright (c) 2022 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_INTERCONNECT_IPCT_HALTIUM_RADIOCORE_H__
#define NRFX_INTERCONNECT_IPCT_HALTIUM_RADIOCORE_H__

#include "drivers/nrfx_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((NRFX_IPCTx_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM() | \
     NRFX_IPCTx_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM()) > \
     NRFX_BIT_MASK(LOCAL_IPCT_NUM))
#error "Invalid mask for IPCT subscibe/publish defined."
#endif

#ifndef NRFX_INTERCONNECT_IPCT_LOCAL_DEFINE
#define NRFX_INTERCONNECT_IPCT_LOCAL_DEFINE NRFX_IPCT_CHANNELS_ENTRY();
#endif // NRFX_INTERCONNECT_IPCT_LOCAL_DEFINE

#define NRFX_INTERCONNECT_IPCT_LOCAL_IPCT_PROP \
{                                              \
    NRFX_INTERCONNECT_IPCT_PROP_ENTRY()        \
}

#ifdef __cplusplus
}
#endif

#endif // NRFX_INTERCONNECT_IPCT_HALTIUM_RADIOCORE_H__
