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

#ifndef NRFX_INTERCONNECT_IPCT_HALTIUM_GLOBAL_H__
#define NRFX_INTERCONNECT_IPCT_HALTIUM_GLOBAL_H__

#include "drivers/nrfx_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Instance of the IPCT peripheral which belongs to the main APB bus. */
#define NRFX_INTERCONNECT_MAIN_IPCT_INSTANCE 130

#ifndef NRFX_INTERCONNECT_IPCT_GLOBAL_DEFINE
/* Default IPCT static variables generation in case of bare-metal application. */
#ifndef NRFX_IPCTx_CHANNELS_SINGLE_VAR_NAME_BY_INST_NUM
#define NRFX_IPCTx_CHANNELS_SINGLE_VAR_NAME_BY_INST_NUM(inst_num) \
        NRFX_CONCAT(m_ipct, inst_num, _channels)
#else
#error "Invalid set of configuration for IPCT."
#endif

#ifndef NRFX_IPCTx_CONFIG_OWNED_CHANNELS_MASK_BY_INST_NUM
#define NRFX_IPCTx_CONFIG_OWNED_CHANNELS_MASK_BY_INST_NUM(inst_num) \
        NRFX_CONCAT(NRFX_IPCT, inst_num, _CONFIG_OWNED_CHANNELS_MASK)
#else
#error Invalid set of configuration for IPCT.
#endif

#ifndef NRFX_IPCTx_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM
#define NRFX_IPCTx_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num) \
        NRFX_CONCAT(NRFX_IPCT, inst_num, _PUB_CONFIG_ALLOWED_CHANNELS_MASK)
#else
#error "Invalid set of configuration for IPCT."
#endif

#ifndef NRFX_IPCTx_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM
#define NRFX_IPCTx_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num) \
        NRFX_CONCAT(NRFX_IPCT, inst_num, _SUB_CONFIG_ALLOWED_CHANNELS_MASK)
#else
#error "Invalid set of configuration for IPCT."
#endif

/* Macro checks if owned inst_num mask is not zero and then returns 1, else 0 */
#ifdef NRF_SECURE
#define NRFX_IPCT_OWNED_MASK(inst_num) 1
#else
#define NRFX_IPCT_OWNED_MASK(inst_num) \
    NRFX_COND_CODE_0(NRFX_IPCTx_CONFIG_OWNED_CHANNELS_MASK_BY_INST_NUM(inst_num), (0), (1))
#endif

#define NRFX_IPCT_CHANNELS_ENTRY(inst_num)                                               \
    NRFX_COND_CODE_1(NRFX_IPCT_OWNED_MASK(inst_num), \
            ( \
              NRFX_CONCAT(static nrfx_atomic_t m_ipct, inst_num, _channels __attribute__((used)) = \
                NRFX_IPCTx_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num) |      \
                NRFX_IPCTx_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num)); \
            ), ())

/* Global entry discard case when instance has no inst_num (NRF_IPCT) which is a local
 * instance.
 */
#define _NRFX_IPCT_CHANNELS_ENTRY(periph_name, prefix, inst_num, _) \
    NRFX_COND_CODE_1(NRFX_IS_EMPTY(inst_num), (), (NRFX_IPCT_CHANNELS_ENTRY(inst_num)))

#define NRFX_INTERCONNECT_IPCT_GLOBAL_DEFINE \
        NRFX_FOREACH_PRESENT(IPCT, _NRFX_IPCT_CHANNELS_ENTRY, (), (), _)
#else
#ifndef NRFX_IPCT_OWNED_MASK
#define NRFX_IPCT_OWNED_MASK NRFX_IPCT_PUB_OR_SUB_MASK
#endif
#endif // NRFX_INTERCONNECT_IPCT_GLOBAL_DEFINE

#define _NRFX_IPCT_INSTANCE(inst_num) NRFX_CONCAT(NRF_, IPCT, inst_num)

#define NRFX_INTERCONNECT_IPCT_PROP_ENTRY(inst_num)                                               \
    NRFX_COND_CODE_1(NRFX_IPCT_OWNED_MASK(inst_num), \
    ({                                                                                             \
            .p_ipct                 = _NRFX_IPCT_INSTANCE(inst_num),                               \
            .p_ipct_channels        = &NRFX_IPCTx_CHANNELS_SINGLE_VAR_NAME_BY_INST_NUM(inst_num),  \
            .ipct_pub_channels_mask = \
                NRFX_IPCTx_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num), \
            .ipct_sub_channels_mask = \
                NRFX_IPCTx_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num), \
    },), ())

/* Global entry discard case when instance has no inst_num (NRF_IPCT) which is a local
 * instance.
 */
#define _NRFX_INTERCONNECT_IPCT_GLOBAL_IPCT_PROP_ENTRY(periph_name, prefix, inst_num, _) \
    NRFX_COND_CODE_1(NRFX_IS_EMPTY(inst_num), (), (NRFX_INTERCONNECT_IPCT_PROP_ENTRY(inst_num)))

#define NRFX_INTERCONNECT_IPCT_GLOBAL_IPCT_PROP                                            \
{                                                                                          \
        NRFX_FOREACH_PRESENT(IPCT, _NRFX_INTERCONNECT_IPCT_GLOBAL_IPCT_PROP_ENTRY, (), ()) \
}

#ifdef __cplusplus
}
#endif

#endif // NRFX_INTERCONNECT_IPCT_HALTIUM_GLOBAL_H__
