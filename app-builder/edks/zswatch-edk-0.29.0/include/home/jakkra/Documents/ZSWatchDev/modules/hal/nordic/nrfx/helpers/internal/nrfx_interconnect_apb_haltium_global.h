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

#ifndef NRFX_INTERCONNECT_APB_HALTIUM_GLOBAL_H__
#define NRFX_INTERCONNECT_APB_HALTIUM_GLOBAL_H__

#include "drivers/nrfx_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Instance of the DPPI peripheral which belongs to the main APB bus. */
#define NRFX_INTERCONNECT_MAIN_DPPI_INSTANCE 130

#ifndef NRFX_INTERCONNECT_APB_GLOBAL_DPPI_DEFINE
/* Default DPPI static variables generation in case of bare-metal application. */
#ifndef NRFX_DPPI_CHANNELS_SINGLE_VAR_NAME_BY_INST_NUM
#define NRFX_DPPI_CHANNELS_SINGLE_VAR_NAME_BY_INST_NUM(inst_num) \
        NRFX_CONCAT(m_dppi, inst_num, _channels)
#else
#error Invalid set of configuration for DPPIC.
#endif

#ifndef NRFX_DPPI_CONFIG_OWNED_CHANNELS_MASK_BY_INST_NUM
#define NRFX_DPPI_CONFIG_OWNED_CHANNELS_MASK_BY_INST_NUM(inst_num) 0
#else
#error Invalid set of configuration for DPPIC.
#endif

#ifndef NRFX_DPPI_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM
#define NRFX_DPPI_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num) 0
#else
#error Invalid set of configuration for DPPIC.
#endif

#ifndef NRFX_DPPI_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM
#define NRFX_DPPI_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num) 0
#else
#error "Invalid set of configuration for DPPIC."
#endif

/* Macro checks if owned inst_num mask is not zero and then returns 1, else 0 */
#ifdef NRF_SECURE
#define NRFX_DPPI_OWNED_MASK(inst_num) 1
#else
#define NRFX_DPPI_OWNED_MASK(inst_num) \
    NRFX_COND_CODE_0(NRFX_DPPI_CONFIG_OWNED_CHANNELS_MASK_BY_INST_NUM(inst_num), (0), (1))
#endif

#define NRFX_DPPI_CHANNELS_VAR(inst_num) \
    NRFX_CONCAT(m_dppi, inst_num, _channels)

#define NRFX_DPPI_CHANNELS_ENTRY(inst_num)                                                \
    NRFX_COND_CODE_1(NRFX_DPPI_OWNED_MASK(inst_num),                                      \
        (                                                                                 \
            static nrfx_atomic_t NRFX_DPPI_CHANNELS_VAR(inst_num) __attribute__((used)) = \
            NRFX_DPPI_CONFIG_OWNED_CHANNELS_MASK_BY_INST_NUM(inst_num);                   \
        ), ())

#define _NRFX_DPPI_CHANNELS_ENTRY(periph_name, prefix, inst_num, _)                               \
    /* Since this is used only for global instances macro shall skip instances starting with 0 */ \
    NRFX_COND_CODE_0(prefix, (), (NRFX_DPPI_CHANNELS_ENTRY(inst_num)))

#define NRFX_INTERCONNECT_APB_GLOBAL_DPPI_DEFINE \
        NRFX_FOREACH_PRESENT(DPPIC, _NRFX_DPPI_CHANNELS_ENTRY, (), ())
#else
#ifndef NRFX_DPPI_OWNED_MASK
#define NRFX_DPPI_OWNED_MASK NRFX_DPPI_PUB_OR_SUB_MASK
#endif
#endif // NRFX_INTERCONNECT_APB_GLOBAL_DPPI_DEFINE

#define NRFX_INTERCONNECT_APB_PROP_ENTRY(inst_num, ppib, apb_size)                                  \
    NRFX_COND_CODE_1(NRFX_DPPI_OWNED_MASK(inst_num),                                                \
    (                                                                                               \
    {                                                                                               \
        .p_dppi = NRFX_CONCAT(NRF_DPPIC, inst_num),                                                 \
        .p_dppi_channels = &NRFX_DPPI_CHANNELS_SINGLE_VAR_NAME_BY_INST_NUM(inst_num),               \
        .dppi_pub_channels_mask = NRFX_DPPI_PUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num), \
        .dppi_sub_channels_mask = NRFX_DPPI_SUB_CONFIG_ALLOWED_CHANNELS_MASK_BY_INST_NUM(inst_num), \
        .p_ppib = ppib,                                                                             \
        .size = apb_size,                                                                           \
    }, \
    ), ())

#if !defined(NRFX_INTERCONNECT_APB_GLOBAL_BUSES_PROP)
#define NRFX_INTERCONNECT_APB_GLOBAL_BUSES_PROP                      \
{                                                                    \
    NRFX_INTERCONNECT_APB_PROP_ENTRY(120, NULL, 0x10000) /* APB22 */ \
    NRFX_INTERCONNECT_APB_PROP_ENTRY(130, NULL, 0x20000) /* APB32 */ \
    NRFX_INTERCONNECT_APB_PROP_ENTRY(131, NULL, 0x10000) /* APB38 */ \
    NRFX_INTERCONNECT_APB_PROP_ENTRY(132, NULL, 0x10000) /* APB39 */ \
    NRFX_INTERCONNECT_APB_PROP_ENTRY(133, NULL, 0x10000) /* APB3A */ \
    NRFX_INTERCONNECT_APB_PROP_ENTRY(134, NULL, 0x10000) /* APB3B */ \
    NRFX_INTERCONNECT_APB_PROP_ENTRY(135, NULL, 0x10000) /* APB3C */ \
    NRFX_INTERCONNECT_APB_PROP_ENTRY(136, NULL, 0x10000) /* APB3D */ \
}
#endif

#ifdef __cplusplus
}
#endif

#endif // NRFX_INTERCONNECT_APB_HALTIUM_GLOBAL_H__
