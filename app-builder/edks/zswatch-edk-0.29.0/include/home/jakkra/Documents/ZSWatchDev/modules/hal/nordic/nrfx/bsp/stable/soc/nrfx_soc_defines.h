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

#ifndef NRFX_SOC_DEFINES_H__
#define NRFX_SOC_DEFINES_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Analog pins definitions. */

#if defined(NRF54LV10A_ENGA_XXAA)
#define ANALOG_REF_INTERNAL_VAL 1300
#elif defined(HALTIUM_XXAA) || defined(NRF7120_ENGA_XXAA) || defined(NRF54LS05B_ENGA_XXAA)
#define ANALOG_REF_INTERNAL_VAL 1024
#elif defined(LUMOS_XXAA)
#define ANALOG_REF_INTERNAL_VAL 900
#else
#define ANALOG_REF_INTERNAL_VAL 600
#endif

#if defined(HALTIUM_XXAA)
    #define COMP_EXTERNAL_AIN_PSELS        \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1)
#elif defined(NRF54L05_XXAA) || defined(NRF54L10_XXAA) || defined(NRF54L15_XXAA)
    #define COMP_EXTERNAL_AIN_PSELS         \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(13U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(14U, 1)
#elif defined(NRF54LM20A_ENGA_XXAA)
    #define COMP_EXTERNAL_AIN_PSELS         \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(31U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(30U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(29U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1)
#elif defined(NRF54LV10A_ENGA_XXAA)
    #define COMP_EXTERNAL_AIN_PSELS         \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(10U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1)
#elif defined(NRF54LS05B_ENGA_XXAA)
    #define COMP_EXTERNAL_AIN_PSELS         \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1)
#elif defined(NRF7120_ENGA_XXAA)
    #define COMP_EXTERNAL_AIN_PSELS                     \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 0),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 0),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 0),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 0),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 0),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 0),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 0),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(8U, 0),              \
        (nrf_comp_input_t)NRFX_COMP_INPUT_NOT_PRESENT,  \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 4),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 4),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 4),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 4),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 4),              \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 4)
#else /* legacy platforms */
    #define COMP_EXTERNAL_AIN_PSELS                                             \
        NRF_COMP_INPUT_0,                                                       \
        NRF_COMP_INPUT_1,                                                       \
        NRF_COMP_INPUT_2,                                                       \
        NRF_COMP_INPUT_3,                                                       \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(COMP_PSEL_PSEL_AnalogInput4), \
            (NRF_COMP_INPUT_4,), ())                                            \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(COMP_PSEL_PSEL_AnalogInput5), \
            (NRF_COMP_INPUT_5,), ())                                            \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(COMP_PSEL_PSEL_AnalogInput6), \
            (NRF_COMP_INPUT_6,), ())                                            \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(COMP_PSEL_PSEL_AnalogInput7), \
            (NRF_COMP_INPUT_7,), ())
#endif

#if defined(HALTIUM_XXAA)
    #define COMP_EXTERNAL_REF_PSELS        \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),
#elif defined(NRF54L05_XXAA) || defined(NRF54L10_XXAA) || defined(NRF54L15_XXAA)
    #define COMP_EXTERNAL_REF_PSELS         \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(13U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(14U, 1),
#elif defined(NRF54LM20A_ENGA_XXAA)
    #define COMP_EXTERNAL_REF_PSELS         \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(31U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(30U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(29U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),
#elif defined(NRF54LV10A_ENGA_XXAA)
    #define COMP_EXTERNAL_REF_PSELS         \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(10U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1),
#elif defined(NRF54LS05B_ENGA_XXAA)
    #define COMP_EXTERNAL_REF_PSELS        \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),
#elif defined(NRF7120_ENGA_XXAA)
    #define COMP_EXTERNAL_REF_PSELS                      \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(8U, 0),               \
        (nrf_comp_ext_ref_t)NRFX_COMP_INPUT_NOT_PRESENT, \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 4),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 4),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 4),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 4),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 4),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 4),
#else /* legacy platforms */
    #define COMP_EXTERNAL_REF_PSELS                                                           \
        NRF_COMP_EXT_REF_0,                                                                   \
        NRF_COMP_EXT_REF_1,                                                                   \
        NRF_COMP_EXT_REF_2,                                                                   \
        NRF_COMP_EXT_REF_3,                                                                   \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(COMP_EXTREFSEL_EXTREFSEL_AnalogReference4), \
            (NRF_COMP_EXT_REF_4,), ())                                                        \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(COMP_EXTREFSEL_EXTREFSEL_AnalogReference5), \
            (NRF_COMP_EXT_REF_5,), ())                                                        \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(COMP_EXTREFSEL_EXTREFSEL_AnalogReference6), \
            (NRF_COMP_EXT_REF_6,), ())                                                        \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(COMP_EXTREFSEL_EXTREFSEL_AnalogReference7), \
            (NRF_COMP_EXT_REF_7,), ())
#endif

#if defined(HALTIUM_XXAA)
    #define LPCOMP_EXTERNAL_AIN_PSELS      \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),
#elif defined(NRF54L05_XXAA) || defined(NRF54L10_XXAA) || defined(NRF54L15_XXAA)
    #define LPCOMP_EXTERNAL_AIN_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(13U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(14U, 1),
#elif defined(NRF54LM20A_ENGA_XXAA)
    #define LPCOMP_EXTERNAL_AIN_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(31U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(30U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(29U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),
#elif defined(NRF54LV10A_ENGA_XXAA)
    #define LPCOMP_EXTERNAL_AIN_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(10U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1),
#elif defined(NRF54LS05B_ENGA_XXAA)
    #define LPCOMP_EXTERNAL_AIN_PSELS      \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),
#elif defined(NRF7120_ENGA_XXAA)
    #define LPCOMP_EXTERNAL_AIN_PSELS                      \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 0),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 0),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 0),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 0),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 0),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 0),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 0),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(8U, 0),                 \
        (nrf_lpcomp_input_t)NRFX_LPCOMP_INPUT_NOT_PRESENT, \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 4),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 4),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 4),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 4),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 4),                 \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 4),
#else /* legacy platforms */
    #define LPCOMP_EXTERNAL_AIN_PSELS \
        NRF_LPCOMP_INPUT_0,           \
        NRF_LPCOMP_INPUT_1,           \
        NRF_LPCOMP_INPUT_2,           \
        NRF_LPCOMP_INPUT_3,           \
        NRF_LPCOMP_INPUT_4,           \
        NRF_LPCOMP_INPUT_5,           \
        NRF_LPCOMP_INPUT_6,           \
        NRF_LPCOMP_INPUT_7,
#endif

#if defined(HALTIUM_XXAA)
    #define LPCOMP_EXTERNAL_REF_PSELS      \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),
#elif defined(NRF54L05_XXAA) || defined(NRF54L10_XXAA) || defined(NRF54L15_XXAA)
    #define LPCOMP_EXTERNAL_REF_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(13U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(14U, 1),
#elif defined(NRF54LM20A_ENGA_XXAA)
    #define LPCOMP_EXTERNAL_REF_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(31U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(30U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(29U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),
#elif defined(NRF54LV10A_ENGA_XXAA)
    #define LPCOMP_EXTERNAL_REF_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(10U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1),
#elif defined(NRF54LS05B_ENGA_XXAA)
    #define LPCOMP_EXTERNAL_REF_PSELS      \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),
#elif defined(NRF7120_ENGA_XXAA)
    #define LPCOMP_EXTERNAL_REF_PSELS                    \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 0),               \
        NRF_PIN_PORT_TO_PIN_NUMBER(8U, 0),               \
    (nrf_lpcomp_ext_ref_t)NRFX_LPCOMP_INPUT_NOT_PRESENT, \
    NRF_PIN_PORT_TO_PIN_NUMBER(0U, 4),                   \
    NRF_PIN_PORT_TO_PIN_NUMBER(1U, 4),                   \
    NRF_PIN_PORT_TO_PIN_NUMBER(2U, 4),                   \
    NRF_PIN_PORT_TO_PIN_NUMBER(3U, 4),                   \
    NRF_PIN_PORT_TO_PIN_NUMBER(4U, 4),                   \
    NRF_PIN_PORT_TO_PIN_NUMBER(5U, 4),
#else /* legacy platforms */
    #define LPCOMP_EXTERNAL_REF_PSELS \
        NRF_LPCOMP_EXT_REF_REF0,      \
        NRF_LPCOMP_EXT_REF_REF1,
#endif

#if defined(HALTIUM_XXAA)
    #define SAADC_EXTERNAL_AIN_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 9), \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 9), \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 9), \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 9), \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 9), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 9),
#elif defined(NRF54L05_XXAA) || defined(NRF54L10_XXAA) || defined(NRF54L15_XXAA)
    #define SAADC_EXTERNAL_AIN_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(13U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(14U, 1),
#elif defined(NRF54LM20A_ENGA_XXAA)
    #define SAADC_EXTERNAL_AIN_PSELS        \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(31U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(30U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(29U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),
#elif defined(NRF54LV10A_ENGA_XXAA)
    #define SAADC_EXTERNAL_AIN_PSELS        \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),  \
        NRF_PIN_PORT_TO_PIN_NUMBER(10U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(11U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(12U, 1),
#elif defined(NRF54LS05B_ENGA_XXAA)
    #define SAADC_EXTERNAL_AIN_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 1), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 1),
#elif defined(NRF7120_ENGA_XXAA)
    #define SAADC_EXTERNAL_AIN_PSELS       \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 0), \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 0), \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 0), \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 0), \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 0), \
        NRF_PIN_PORT_TO_PIN_NUMBER(6U, 0), \
        NRF_PIN_PORT_TO_PIN_NUMBER(7U, 0), \
        NRF_PIN_PORT_TO_PIN_NUMBER(8U, 0), \
        NRFX_SAADC_INPUT_NOT_PRESENT,      \
        NRF_PIN_PORT_TO_PIN_NUMBER(0U, 4), \
        NRF_PIN_PORT_TO_PIN_NUMBER(1U, 4), \
        NRF_PIN_PORT_TO_PIN_NUMBER(2U, 4), \
        NRF_PIN_PORT_TO_PIN_NUMBER(3U, 4), \
        NRF_PIN_PORT_TO_PIN_NUMBER(4U, 4), \
        NRF_PIN_PORT_TO_PIN_NUMBER(5U, 4),
#else /* legacy platforms */
    #define SAADC_EXTERNAL_AIN_PSELS                                                   \
        NRF_SAADC_INPUT_AIN0,                                                          \
        NRF_SAADC_INPUT_AIN1,                                                          \
        NRF_SAADC_INPUT_AIN2,                                                          \
        NRF_SAADC_INPUT_AIN3,                                                          \
        NRF_SAADC_INPUT_AIN4,                                                          \
        NRF_SAADC_INPUT_AIN5,                                                          \
        NRF_SAADC_INPUT_AIN6,                                                          \
        NRF_SAADC_INPUT_AIN7,                                                          \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(SAADC_CH_PSELP_PSELP_AnalogInput8),  \
            (NRF_SAADC_INPUT_AIN8,), ())                                               \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(SAADC_CH_PSELP_PSELP_AnalogInput9),  \
            (NRF_SAADC_INPUT_AIN9,), ())                                               \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(SAADC_CH_PSELP_PSELP_AnalogInput10), \
            (NRF_SAADC_INPUT_AIN10,), ())                                              \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(SAADC_CH_PSELP_PSELP_AnalogInput11), \
            (NRF_SAADC_INPUT_AIN11,), ())                                              \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(SAADC_CH_PSELP_PSELP_AnalogInput12), \
            (NRF_SAADC_INPUT_AIN812,), ())                                             \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(SAADC_CH_PSELP_PSELP_AnalogInput13), \
            (NRF_SAADC_INPUT_AIN13,), ())
#endif

#if defined(NRF54L05_XXAA) || defined(NRF54L10_XXAA) || defined(NRF54L15_XXAA)
    #define SAADC_INTERNAL_AIN_PSELS                                     \
        NRF_SAADC_INPUT_VDD,                                             \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRF_SAADC_INPUT_AVDD,                                            \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(NRF_SAADC_INPUT_DVDD), \
            (NRF_SAADC_INPUT_DVDD,), (NRFX_SAADC_INPUT_NOT_PRESENT,))
#elif defined(NRF54LM20A_ENGA_XXAA)
    #define SAADC_INTERNAL_AIN_PSELS                                     \
        NRF_SAADC_INPUT_VDD,                                             \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRF_SAADC_INPUT_AVDD,                                            \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(NRF_SAADC_INPUT_DVDD), \
            (NRF_SAADC_INPUT_DVDD,), (NRFX_SAADC_INPUT_NOT_PRESENT,))
#elif defined(NRF54LV10A_ENGA_XXAA)
    #define SAADC_INTERNAL_AIN_PSELS                                     \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(NRF_SAADC_INPUT_DVDD), \
            (NRF_SAADC_INPUT_DVDD,), (NRFX_SAADC_INPUT_NOT_PRESENT,))    \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRF_SAADC_INPUT_VDDL,                                            \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRF_SAADC_INPUT_VSS,
#elif defined(NRF54LS05B_ENGA_XXAA)
    #define SAADC_INTERNAL_AIN_PSELS                                     \
        NRF_SAADC_INPUT_VDD,                                             \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(NRF_SAADC_INPUT_DVDD), \
            (NRF_SAADC_INPUT_DVDD,), (NRFX_SAADC_INPUT_NOT_PRESENT,))    \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                    \
        NRF_SAADC_INPUT_VSS,
#elif defined(NRF7120_ENGA_XXAA)
    #define SAADC_INTERNAL_AIN_PSELS  \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRFX_SAADC_INPUT_NOT_PRESENT, \
        NRF_SAADC_INPUT_INTERNAL0,    \
        NRF_SAADC_INPUT_INTERNAL1,    \
        NRF_SAADC_INPUT_INTERNAL2,    \
        NRF_SAADC_INPUT_INTERNAL3,
#else  /* legacy platforms */
    #define SAADC_INTERNAL_AIN_PSELS                                              \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(SAADC_CH_PSELP_PSELP_VDD),      \
            (NRF_SAADC_INPUT_VDD,), (NRFX_SAADC_INPUT_NOT_PRESENT,))              \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                             \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                             \
        NRFX_SAADC_INPUT_NOT_PRESENT,                                             \
        NRFX_COND_CODE_1(NRFX_ARG_HAS_PARENTHESIS(SAADC_CH_PSELP_PSELP_VDDHDIV5), \
            (NRF_SAADC_INPUT_VDDHDIV5,), (NRFX_SAADC_INPUT_NOT_PRESENT,))
#endif

/* Ram sections definitions. */

#define RAM_NON_UNIFORM_SECTION_DECLARE(i, _block, _section) {.decoded = {_block, _section}}

#if defined(NRF51)
#define RAM_SECTION_UNIT_SIZE          8192
#define RAM_UNIFORM_BLOCKS             4
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 1
#define RAM_UNIFORM_SECTIONS_TOTAL     4
#elif defined(NRF52805_XXAA) || defined(NRF52810_XXAA) || defined(NRF52811_XXAA)
#define RAM_SECTION_UNIT_SIZE          4096
#define RAM_UNIFORM_BLOCKS             3
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 2
#define RAM_UNIFORM_SECTIONS_TOTAL     6
#elif defined(NRF52820_XXAA)
#define RAM_SECTION_UNIT_SIZE          4096
#define RAM_UNIFORM_BLOCKS             4
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 2
#define RAM_UNIFORM_SECTIONS_TOTAL     8
#elif defined(NRF52832_XXAA)
#define RAM_SECTION_UNIT_SIZE          4096
#define RAM_UNIFORM_BLOCKS             8
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 2
#define RAM_UNIFORM_SECTIONS_TOTAL     16
#elif defined(NRF52833_XXAA)
#define RAM_SECTION_UNIT_SIZE          4096
#define RAM_UNIFORM_BLOCKS             8
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 2
#define RAM_UNIFORM_SECTIONS_TOTAL     16
#define RAM_NON_UNIFORM_BLOCK_INDEX    8
#define RAM_NON_UNIFORM_BLOCK_UNITS    8
#define RAM_NON_UNIFORM_SECTIONS                                                               \
    NRFX_LISTIFY(RAM_NON_UNIFORM_BLOCK_UNITS,                                                  \
                 RAM_NON_UNIFORM_SECTION_DECLARE, (,),                                         \
                 RAM_NON_UNIFORM_BLOCK_INDEX, 0), /* Section 0 of block 8 - 8 * 4 kB units. */ \
    NRFX_LISTIFY(RAM_NON_UNIFORM_BLOCK_UNITS,                                                  \
                 RAM_NON_UNIFORM_SECTION_DECLARE, (,),                                         \
                 RAM_NON_UNIFORM_BLOCK_INDEX, 1)  /* Section 1 of block 8 - 8 * 4 kB units. */
#elif defined(NRF52840_XXAA)
#define RAM_SECTION_UNIT_SIZE          4096
#define RAM_UNIFORM_BLOCKS             8
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 2
#define RAM_UNIFORM_SECTIONS_TOTAL     16
#define RAM_NON_UNIFORM_BLOCK_INDEX    8
#define RAM_NON_UNIFORM_BLOCK_UNITS    8
#define RAM_NON_UNIFORM_SECTIONS                                                               \
    NRFX_LISTIFY(RAM_NON_UNIFORM_BLOCK_UNITS,                                                  \
                 RAM_NON_UNIFORM_SECTION_DECLARE, (,),                                         \
                 RAM_NON_UNIFORM_BLOCK_INDEX, 0), /* Section 0 of block 8 - 8 * 4 kB units. */ \
    NRFX_LISTIFY(RAM_NON_UNIFORM_BLOCK_UNITS,                                                  \
                 RAM_NON_UNIFORM_SECTION_DECLARE, (,),                                         \
                 RAM_NON_UNIFORM_BLOCK_INDEX, 1), /* Section 1 of block 8 - 8 * 4 kB units. */ \
    NRFX_LISTIFY(RAM_NON_UNIFORM_BLOCK_UNITS,                                                  \
                 RAM_NON_UNIFORM_SECTION_DECLARE, (,),                                         \
                 RAM_NON_UNIFORM_BLOCK_INDEX, 2), /* Section 2 of block 8 - 8 * 4 kB units. */ \
    NRFX_LISTIFY(RAM_NON_UNIFORM_BLOCK_UNITS,                                                  \
                 RAM_NON_UNIFORM_SECTION_DECLARE, (,),                                         \
                 RAM_NON_UNIFORM_BLOCK_INDEX, 3), /* Section 3 of block 8 - 8 * 4 kB units. */ \
    NRFX_LISTIFY(RAM_NON_UNIFORM_BLOCK_UNITS,                                                  \
                 RAM_NON_UNIFORM_SECTION_DECLARE, (,),                                         \
                 RAM_NON_UNIFORM_BLOCK_INDEX, 4), /* Section 4 of block 8 - 8 * 4 kB units. */ \
    NRFX_LISTIFY(RAM_NON_UNIFORM_BLOCK_UNITS,                                                  \
                 RAM_NON_UNIFORM_SECTION_DECLARE, (,),                                         \
                 RAM_NON_UNIFORM_BLOCK_INDEX, 5)  /* Section 5 of block 8 - 8 * 4 kB units. */
#elif defined(NRF5340_XXAA_APPLICATION)
#define RAM_SECTION_UNIT_SIZE          4096
#define RAM_UNIFORM_BLOCKS             8
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 16
#define RAM_UNIFORM_SECTIONS_TOTAL     128
#elif defined(NRF5340_XXAA_NETWORK)
#define RAM_SECTION_UNIT_SIZE          4096
#define RAM_UNIFORM_BLOCKS             4
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 4
#define RAM_UNIFORM_SECTIONS_TOTAL     16
#elif defined(NRF54H20_XXAA) && defined(NRF_APPLICATION)
#define RAM_SECTION_UNIT_SIZE          (32UL * 1024UL)
#define RAM_UNIFORM_BLOCKS             1
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 1
#define RAM_UNIFORM_SECTIONS_TOTAL     1
#elif defined(NRF54H20_XXAA) && defined(NRF_RADIOCORE)
#define RAM_SECTION_UNIT_SIZE          (2UL * 16UL * 1024UL) /* Consider both banks as single unit */
#define RAM_UNIFORM_BLOCKS             1
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 6
#define RAM_UNIFORM_SECTIONS_TOTAL     6
#elif defined(NRF54L05_XXAA)
#define RAM_SECTION_UNIT_SIZE          (32UL * 1024UL)
#define RAM_UNIFORM_BLOCKS             1
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 3
#define RAM_UNIFORM_SECTIONS_TOTAL     3
#elif defined(NRF54L10_XXAA)
#define RAM_SECTION_UNIT_SIZE          (32UL * 1024UL)
#define RAM_UNIFORM_BLOCKS             1
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 6
#define RAM_UNIFORM_SECTIONS_TOTAL     6
#elif defined(NRF54L15_XXAA)
#define RAM_SECTION_UNIT_SIZE          (32UL * 1024UL)
#define RAM_UNIFORM_BLOCKS             1
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 8
#define RAM_UNIFORM_SECTIONS_TOTAL     8
#elif defined(NRF54LM20A_ENGA_XXAA)
#define RAM_SECTION_UNIT_SIZE          (32UL * 1024UL)
#define RAM_UNIFORM_BLOCKS             1
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 16
#define RAM_UNIFORM_SECTIONS_TOTAL     16
#elif defined(NRF54LS05B_ENGA_XXAA)
#define RAM_SECTION_UNIT_SIZE          (32UL * 1024UL)
#define RAM_UNIFORM_BLOCKS             1
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 2
#define RAM_UNIFORM_SECTIONS_TOTAL     2
#elif defined(NRF54LV10A_ENGA_XXAA)
#define RAM_SECTION_UNIT_SIZE          (32UL * 1024UL)
#define RAM_UNIFORM_BLOCKS             1
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 6
#define RAM_UNIFORM_SECTIONS_TOTAL     6
#elif defined(NRF9120_XXAA) || defined(NRF9160_XXAA)
#define RAM_SECTION_UNIT_SIZE          8192
#define RAM_UNIFORM_BLOCKS             8
#define RAM_UNIFORM_SECTIONS_PER_BLOCK 4
#define RAM_UNIFORM_SECTIONS_TOTAL     32
#endif

/* PRS boxes definitions. */

#if defined(NRF51)
    // SPI0, TWI0
    #define NRFX_PRS_BOX_0_ADDR     NRF_SPI0
    // SPI1, SPIS1, TWI1
    #define NRFX_PRS_BOX_1_ADDR     NRF_SPI1
#elif defined(NRF52805_XXAA) || defined(NRF52810_XXAA)
    // TWIM0, TWIS0, TWI0
    #define NRFX_PRS_BOX_0_ADDR     NRF_TWIM0
    // SPIM0, SPIS0, SPI0
    #define NRFX_PRS_BOX_1_ADDR     NRF_SPIM0
    // UARTE0, UART0
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE0
#elif defined(NRF52811_XXAA)
    // TWIM0, TWIS0, TWI0, SPIM1, SPIS1, SPI1
    #define NRFX_PRS_BOX_0_ADDR     NRF_TWIM0
    // SPIM0, SPIS0, SPI0
    #define NRFX_PRS_BOX_1_ADDR     NRF_SPIM0
    // UART0, UARTE0
    #define NRFX_PRS_BOX_2_ADDR     NRF_UART0
#elif defined(NRF52820_XXAA)
    // SPIM0, SPIS0, TWIM0, TWIS0, SPI0, TWI0
    #define NRFX_PRS_BOX_0_ADDR     NRF_SPIM0
    // SPIM1, SPIS1, TWIM1, TWIS1, SPI1, TWI1
    #define NRFX_PRS_BOX_1_ADDR     NRF_SPIM1
    // UARTE0, UART0
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE0
#elif defined(NRF52832_XXAA) || defined(NRF52832_XXAB) || \
      defined(NRF52833_XXAA) || defined(NRF52840_XXAA)
    // SPIM0, SPIS0, TWIM0, TWIS0, SPI0, TWI0
    #define NRFX_PRS_BOX_0_ADDR     NRF_SPIM0
    // SPIM1, SPIS1, TWIM1, TWIS1, SPI1, TWI1
    #define NRFX_PRS_BOX_1_ADDR     NRF_SPIM1
    // SPIM2, SPIS2, SPI2
    #define NRFX_PRS_BOX_2_ADDR     NRF_SPIM2
    // COMP, LPCOMP
    #define NRFX_PRS_BOX_3_ADDR     NRF_COMP
    // UARTE0, UART0
    #define NRFX_PRS_BOX_4_ADDR     NRF_UARTE0
#elif defined(NRF5340_XXAA_APPLICATION)
    // SPIM0, SPIS0, TWIM0, TWIS0, UARTE0
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE0
    // SPIM1, SPIS1, TWIM1, TWIS1, UARTE1
    #define NRFX_PRS_BOX_1_ADDR     NRF_UARTE1
    // SPIM2, SPIS2, TWIM2, TWIS2, UARTE2
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE2
    // SPIM3, SPIS3, TWIM3, TWIS3, UARTE3
    #define NRFX_PRS_BOX_3_ADDR     NRF_UARTE3
    // COMP, LPCOMP
    #define NRFX_PRS_BOX_4_ADDR     NRF_COMP
#elif defined(NRF5340_XXAA_NETWORK)
    // SPIM0, SPIS0, TWIM0, TWIS0, UARTE0
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE0
#elif defined(NRF54H20_XXAA)
    // SPIM130, SPIS130, TWIM130, TWIS130, UARTE130
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE130
    // SPIM131, SPIS131, TWIM131, TWIS131, UARTE131
    #define NRFX_PRS_BOX_1_ADDR     NRF_UARTE131
    // SPIM132, SPIS132, TWIM132, TWIS132, UARTE132
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE132
    // SPIM133, SPIS133, TWIM133, TWIS133, UARTE133
    #define NRFX_PRS_BOX_3_ADDR     NRF_UARTE133
    // SPIM134, SPIS134, TWIM134, TWIS134, UARTE134
    #define NRFX_PRS_BOX_4_ADDR     NRF_UARTE134
    // SPIM135, SPIS135, TWIM135, TWIS135, UARTE135
    #define NRFX_PRS_BOX_5_ADDR     NRF_UARTE135
    // SPIM136, SPIS136, TWIM136, TWIS136, UARTE136
    #define NRFX_PRS_BOX_6_ADDR     NRF_UARTE136
    // SPIM137, SPIS137, TWIM137, TWIS137, UARTE137
    #define NRFX_PRS_BOX_7_ADDR     NRF_UARTE137
    // SPIM120, UARTE120
    #define NRFX_PRS_BOX_8_ADDR     NRF_UARTE120
    // COMP, LPCOMP
    #define NRFX_PRS_BOX_9_ADDR     NRF_COMP
#elif defined(NRF54L05_XXAA) || defined(NRF54L10_XXAA) || defined(NRF54L15_XXAA)
    // SPIM00, SPIS00, UARTE00
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE00
    // SPIM20, SPIS20, TWIM20, TWIS20, UARTE20
    #define NRFX_PRS_BOX_1_ADDR     NRF_UARTE20
    // SPIM21, SPIS21, TWIM21, TWIS21, UARTE21
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE21
    // SPIM22, SPIS22, TWIM22, TWIS22, UARTE22
    #define NRFX_PRS_BOX_3_ADDR     NRF_UARTE22
    // SPIM30, SPIS30, TWIM30, TWIS30, UARTE30
    #define NRFX_PRS_BOX_4_ADDR     NRF_UARTE30
    // COMP, LPCOMP
    #define NRFX_PRS_BOX_5_ADDR     NRF_COMP
#elif defined(NRF54LM20A_ENGA_XXAA)
    // SPIM00, SPIS00, UARTE00
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE00
    // SPIM20, SPIS20, TWIM20, TWIS20, UARTE20
    #define NRFX_PRS_BOX_1_ADDR     NRF_UARTE20
    // SPIM21, SPIS21, TWIM21, TWIS21, UARTE21
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE21
    // SPIM22, SPIS22, TWIM22, TWIS22, UARTE22
    #define NRFX_PRS_BOX_3_ADDR     NRF_UARTE22
    // SPIM23, SPIS23, TWIM23, TWIS23, UARTE23
    #define NRFX_PRS_BOX_4_ADDR     NRF_UARTE23
    // SPIM24, SPIS24, TWIM24, TWIS24, UARTE23
    #define NRFX_PRS_BOX_5_ADDR     NRF_UARTE24
    // SPIM30, SPIS30, TWIM30, TWIS30, UARTE30
    #define NRFX_PRS_BOX_6_ADDR     NRF_UARTE30
    // COMP, LPCOMP
    #define NRFX_PRS_BOX_7_ADDR     NRF_COMP
#elif defined(NRF54LS05B_ENGA_XXAA)
    // SPIM20, SPIS20, TWIM20, TWIS20, UARTE20
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE20
    // SPIM21, SPIS21, TWIM21, TWIS21, UARTE21
    #define NRFX_PRS_BOX_1_ADDR     NRF_UARTE21
    // SPIM32, SPIS32, TWIM32, TWIS32, UARTE32
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE22
#elif defined(NRF54LV10A_ENGA_XXAA)
    // SPIM20, SPIS20, TWIM20, TWIS20, UARTE20
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE20
    // SPIM21, SPIS21, TWIM21, TWIS21, UARTE21
    #define NRFX_PRS_BOX_1_ADDR     NRF_UARTE21
    // SPIM30, SPIS30, TWIM30, TWIS30, UARTE30
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE30
    // COMP, LPCOMP
    #define NRFX_PRS_BOX_3_ADDR     NRF_COMP
#elif defined(NRF7120_ENGA_XXAA)
    // SPIM00, UARTE00
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE00
    // SPIM01
    #define NRFX_PRS_BOX_1_ADDR     NRF_SPIM01
    // SPIM02
    #define NRFX_PRS_BOX_2_ADDR     NRF_SPIM02
    // SPIM20, SPIS20, TWIM20, TWIS20, UARTE20
    #define NRFX_PRS_BOX_3_ADDR     NRF_UARTE20
    // SPIM21, SPIS21, TWIM21, TWIS21, UARTE21
    #define NRFX_PRS_BOX_4_ADDR     NRF_UARTE21
    // SPIM22, SPIS22, TWIM22, TWIS22, UARTE22
    #define NRFX_PRS_BOX_5_ADDR     NRF_UARTE22
    // SPIM23, SPIS23, TWIM23, TWIS23, UARTE23
    #define NRFX_PRS_BOX_6_ADDR     NRF_UARTE23
    // SPIM24, SPIS24, TWIM24, TWIS24, UARTE24
    #define NRFX_PRS_BOX_7_ADDR     NRF_UARTE24
    // SPIM30, SPIS30, TWIM30, TWIS30, UARTE30
    #define NRFX_PRS_BOX_8_ADDR     NRF_UARTE30
    // COMP, LPCOMP
    #define NRFX_PRS_BOX_9_ADDR     NRF_COMP
#elif defined(NRF91_SERIES)
    // UARTE0, SPIM0, SPIS0, TWIM0, TWIS0
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE0
    // UARTE1, SPIM1, SPIS1, TWIM1, TWIS1
    #define NRFX_PRS_BOX_1_ADDR     NRF_UARTE1
    // UARTE2, SPIM2, SPIS2, TWIM2, TWIS2
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE2
    // UARTE3, SPIM3, SPIS3, TWIM3, TWIS3
    #define NRFX_PRS_BOX_3_ADDR     NRF_UARTE3
#elif defined(NRF9230_ENGB_XXAA)
    // SPIM130, SPIS130, TWIM130, TWIS130, UARTE130
    #define NRFX_PRS_BOX_0_ADDR     NRF_UARTE130
    // SPIM131, SPIS131, TWIM131, TWIS131, UARTE131
    #define NRFX_PRS_BOX_1_ADDR     NRF_UARTE131
    // SPIM132, SPIS132, TWIM132, TWIS132, UARTE132
    #define NRFX_PRS_BOX_2_ADDR     NRF_UARTE132
    // SPIM133, SPIS133, TWIM133, TWIS133, UARTE133
    #define NRFX_PRS_BOX_3_ADDR     NRF_UARTE133
    // SPIM134, SPIS134, TWIM134, TWIS134, UARTE134
    #define NRFX_PRS_BOX_4_ADDR     NRF_UARTE134
    // SPIM135, SPIS135, TWIM135, TWIS135, UARTE135
    #define NRFX_PRS_BOX_5_ADDR     NRF_UARTE135
    // SPIM136, SPIS136, TWIM136, TWIS136, UARTE136
    #define NRFX_PRS_BOX_6_ADDR     NRF_UARTE136
    // SPIM137, SPIS137, TWIM137, TWIS137, UARTE137
    #define NRFX_PRS_BOX_7_ADDR     NRF_UARTE137
    // SPIS120, UARTE120
    #define NRFX_PRS_BOX_8_ADDR     NRF_UARTE120
    // COMP, LPCOMP
    #define NRFX_PRS_BOX_9_ADDR     NRF_COMP
#endif

/* Definition of macro calculating HFXO internal capacitor value. */
#if defined(NRF5340_XXAA_APPLICATION)
#define OSCILLATORS_HFXO_CAP_CALCULATE(p_ficr_reg, cap_val)                   \
    (((((((p_ficr_reg->XOSC32MTRIM & FICR_XOSC32MTRIM_SLOPE_Msk)              \
       >> FICR_XOSC32MTRIM_SLOPE_Pos) + 56) * (uint32_t)(cap_val * 2 - 14)) + \
       ((((p_ficr_reg->XOSC32MTRIM & FICR_XOSC32MTRIM_OFFSET_Msk)             \
       >> FICR_XOSC32MTRIM_OFFSET_Pos) - 8) << 4)) + 32) >> 6)
#else
#define OSCILLATORS_HFXO_CAP_CALCULATE(p_ficr_reg, cap_val)                     \
      (((((p_ficr_reg->XOSC32MTRIM & FICR_XOSC32MTRIM_SLOPE_Msk)                \
         >> FICR_XOSC32MTRIM_SLOPE_Pos) + 791) * (uint32_t)(cap_val * 4 - 22) + \
        (((p_ficr_reg->XOSC32MTRIM & FICR_XOSC32MTRIM_OFFSET_Msk)               \
         >> FICR_XOSC32MTRIM_OFFSET_Pos) << 4)) >> 10)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NRFX_SOC_DEFINES_H__ */
