/*
 * Copyright (c) 2023 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_INTERCONNECT_APB_HALTIUM_RADIOCORE_H__
#define NRFX_INTERCONNECT_APB_HALTIUM_RADIOCORE_H__

#include "drivers/nrfx_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((NRFX_DPPI_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(020) | \
     NRFX_DPPI_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(020)) > \
     NRFX_BIT_MASK(DPPI020_CH_NUM))
#error "Invalid mask for DPPIC020 subscibe/publish defined."
#endif

#if ((NRFX_DPPI_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(030) | \
     NRFX_DPPI_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(030)) > \
     NRFX_BIT_MASK(DPPI030_CH_NUM))
#error "Invalid mask for DPPIC030 subscibe/publish defined."
#endif

#ifndef NRFX_INTERCONNECT_APB_LOCAL_DPPI_DEFINE
#define NRFX_INTERCONNECT_APB_LOCAL_DPPI_DEFINE \
NRFX_DPPI_CHANNELS_ENTRY(020);                  \
NRFX_DPPI_CHANNELS_ENTRY(030);
#endif // NRFX_INTERCONNECT_APB_LOCAL_DPPI_DEFINE

#define NRFX_INTERCONNECT_APB_LOCAL_BUSES_PROP                                                     \
{                                                                                                  \
    NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(DPPIC020),                                              \
                     (NRFX_INTERCONNECT_APB_PROP_ENTRY(020, NRF_PPIB020, 0x10000)), ()) /* APB2 */ \
    NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(DPPIC030),                                              \
                     (NRFX_INTERCONNECT_APB_PROP_ENTRY(030, NRF_PPIB030, 0x10000)), ()) /* APB3 */ \
}

#ifdef __cplusplus
}
#endif

#endif // NRFX_INTERCONNECT_APB_HALTIUM_RADIOCORE_H__
