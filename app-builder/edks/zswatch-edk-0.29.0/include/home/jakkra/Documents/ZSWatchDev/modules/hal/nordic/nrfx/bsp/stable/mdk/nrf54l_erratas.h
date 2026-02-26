#ifndef NRF54L_ERRATAS_H
#define NRF54L_ERRATAS_H

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

#include <stdint.h>
#include <stdbool.h>
#include "compiler_abstraction.h"

static bool nrf54l_errata_1(void) __UNUSED;
static bool nrf54l_errata_2(void) __UNUSED;
static bool nrf54l_errata_3(void) __UNUSED;
static bool nrf54l_errata_4(void) __UNUSED;
static bool nrf54l_errata_6(void) __UNUSED;
static bool nrf54l_errata_7(void) __UNUSED;
static bool nrf54l_errata_8(void) __UNUSED;
static bool nrf54l_errata_9(void) __UNUSED;
static bool nrf54l_errata_10(void) __UNUSED;
static bool nrf54l_errata_13(void) __UNUSED;
static bool nrf54l_errata_16(void) __UNUSED;
static bool nrf54l_errata_17(void) __UNUSED;
static bool nrf54l_errata_18(void) __UNUSED;
static bool nrf54l_errata_19(void) __UNUSED;
static bool nrf54l_errata_20(void) __UNUSED;
static bool nrf54l_errata_21(void) __UNUSED;
static bool nrf54l_errata_22(void) __UNUSED;
static bool nrf54l_errata_23(void) __UNUSED;
static bool nrf54l_errata_24(void) __UNUSED;
static bool nrf54l_errata_25(void) __UNUSED;
static bool nrf54l_errata_26(void) __UNUSED;
static bool nrf54l_errata_27(void) __UNUSED;
static bool nrf54l_errata_30(void) __UNUSED;
static bool nrf54l_errata_31(void) __UNUSED;
static bool nrf54l_errata_32(void) __UNUSED;
static bool nrf54l_errata_33(void) __UNUSED;
static bool nrf54l_errata_35(void) __UNUSED;
static bool nrf54l_errata_37(void) __UNUSED;
static bool nrf54l_errata_38(void) __UNUSED;
static bool nrf54l_errata_39(void) __UNUSED;
static bool nrf54l_errata_40(void) __UNUSED;
static bool nrf54l_errata_41(void) __UNUSED;
static bool nrf54l_errata_42(void) __UNUSED;
static bool nrf54l_errata_43(void) __UNUSED;
static bool nrf54l_errata_44(void) __UNUSED;
static bool nrf54l_errata_45(void) __UNUSED;
static bool nrf54l_errata_46(void) __UNUSED;
static bool nrf54l_errata_47(void) __UNUSED;
static bool nrf54l_errata_48(void) __UNUSED;
static bool nrf54l_errata_49(void) __UNUSED;
static bool nrf54l_errata_50(void) __UNUSED;
static bool nrf54l_errata_55(void) __UNUSED;
static bool nrf54l_configuration_56(void) __UNUSED;
static bool nrf54l_errata_57(void) __UNUSED;
static bool nrf54l_errata_60(void) __UNUSED;
static bool nrf54l_errata_61(void) __UNUSED;
static bool nrf54l_errata_63(void) __UNUSED;
static bool nrf54l_errata_65(void) __UNUSED;
static bool nrf54l_errata_66(void) __UNUSED;
static bool nrf54l_errata_68(void) __UNUSED;
static bool nrf54l_errata_69(void) __UNUSED;
static bool nrf54l_errata_70(void) __UNUSED;
static bool nrf54l_errata_71(void) __UNUSED;
static bool nrf54l_errata_72(void) __UNUSED;
static bool nrf54l_errata_73(void) __UNUSED;
static bool nrf54l_errata_74(void) __UNUSED;
static bool nrf54l_errata_78(void) __UNUSED;
static bool nrf54l_errata_79(void) __UNUSED;
static bool nrf54l_errata_81(void) __UNUSED;
static bool nrf54l_errata_83(void) __UNUSED;
static bool nrf54l_errata_85(void) __UNUSED;
static bool nrf54l_errata_86(void) __UNUSED;
static bool nrf54l_errata_87(void) __UNUSED;
static bool nrf54l_errata_88(void) __UNUSED;
static bool nrf54l_errata_89(void) __UNUSED;
static bool nrf54l_errata_90(void) __UNUSED;
static bool nrf54l_errata_91(void) __UNUSED;
static bool nrf54l_errata_92(void) __UNUSED;
static bool nrf54l_errata_93(void) __UNUSED;
static bool nrf54l_errata_94(void) __UNUSED;
static bool nrf54l_errata_95(void) __UNUSED;

/* ========= Errata 1 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_1_PRESENT 1
#else
    #define NRF54L_ERRATA_1_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_1_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_1_ENABLE_WORKAROUND NRF54L_ERRATA_1_PRESENT
#endif

static bool nrf54l_errata_1(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 2 ========= */
#define NRF54L_ERRATA_2_PRESENT 0

#ifndef NRF54L_ERRATA_2_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_2_ENABLE_WORKAROUND NRF54L_ERRATA_2_PRESENT
#endif

static bool nrf54l_errata_2(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 3 ========= */
#define NRF54L_ERRATA_3_PRESENT 0

#ifndef NRF54L_ERRATA_3_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_3_ENABLE_WORKAROUND NRF54L_ERRATA_3_PRESENT
#endif

static bool nrf54l_errata_3(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 4 ========= */
#define NRF54L_ERRATA_4_PRESENT 0

#ifndef NRF54L_ERRATA_4_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_4_ENABLE_WORKAROUND NRF54L_ERRATA_4_PRESENT
#endif

static bool nrf54l_errata_4(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 6 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_6_PRESENT 1
#else
    #define NRF54L_ERRATA_6_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_6_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_6_ENABLE_WORKAROUND NRF54L_ERRATA_6_PRESENT
#endif

static bool nrf54l_errata_6(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 7 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_7_PRESENT 1
#else
    #define NRF54L_ERRATA_7_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_7_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_7_ENABLE_WORKAROUND NRF54L_ERRATA_7_PRESENT
#endif

static bool nrf54l_errata_7(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 8 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_8_PRESENT 1
#else
    #define NRF54L_ERRATA_8_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_8_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_8_ENABLE_WORKAROUND NRF54L_ERRATA_8_PRESENT
#endif

static bool nrf54l_errata_8(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 9 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_9_PRESENT 1
#else
    #define NRF54L_ERRATA_9_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_9_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_9_ENABLE_WORKAROUND NRF54L_ERRATA_9_PRESENT
#endif

static bool nrf54l_errata_9(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 10 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_10_PRESENT 1
#else
    #define NRF54L_ERRATA_10_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_10_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_10_ENABLE_WORKAROUND NRF54L_ERRATA_10_PRESENT
#endif

static bool nrf54l_errata_10(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 13 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_13_PRESENT 1
#else
    #define NRF54L_ERRATA_13_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_13_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_13_ENABLE_WORKAROUND NRF54L_ERRATA_13_PRESENT
#endif

static bool nrf54l_errata_13(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 16 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_16_PRESENT 1
#else
    #define NRF54L_ERRATA_16_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_16_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_16_ENABLE_WORKAROUND NRF54L_ERRATA_16_PRESENT
#endif

static bool nrf54l_errata_16(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 17 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_17_PRESENT 1
#else
    #define NRF54L_ERRATA_17_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_17_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_17_ENABLE_WORKAROUND NRF54L_ERRATA_17_PRESENT
#endif

static bool nrf54l_errata_17(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 18 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_18_PRESENT 1
#else
    #define NRF54L_ERRATA_18_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_18_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_18_ENABLE_WORKAROUND NRF54L_ERRATA_18_PRESENT
#endif

static bool nrf54l_errata_18(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 19 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_19_PRESENT 1
#else
    #define NRF54L_ERRATA_19_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_19_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_19_ENABLE_WORKAROUND NRF54L_ERRATA_19_PRESENT
#endif

static bool nrf54l_errata_19(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 20 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_20_PRESENT 1
#else
    #define NRF54L_ERRATA_20_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_20_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_20_ENABLE_WORKAROUND NRF54L_ERRATA_20_PRESENT
#endif

static bool nrf54l_errata_20(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 21 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_21_PRESENT 1
#else
    #define NRF54L_ERRATA_21_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_21_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_21_ENABLE_WORKAROUND NRF54L_ERRATA_21_PRESENT
#endif

static bool nrf54l_errata_21(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 22 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_22_PRESENT 1
#else
    #define NRF54L_ERRATA_22_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_22_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_22_ENABLE_WORKAROUND NRF54L_ERRATA_22_PRESENT
#endif

static bool nrf54l_errata_22(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 23 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_23_PRESENT 1
#else
    #define NRF54L_ERRATA_23_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_23_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_23_ENABLE_WORKAROUND NRF54L_ERRATA_23_PRESENT
#endif

static bool nrf54l_errata_23(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 24 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_24_PRESENT 1
#else
    #define NRF54L_ERRATA_24_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_24_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_24_ENABLE_WORKAROUND NRF54L_ERRATA_24_PRESENT
#endif

static bool nrf54l_errata_24(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 25 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_25_PRESENT 1
#else
    #define NRF54L_ERRATA_25_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_25_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_25_ENABLE_WORKAROUND NRF54L_ERRATA_25_PRESENT
#endif

static bool nrf54l_errata_25(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 26 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_26_PRESENT 1
#else
    #define NRF54L_ERRATA_26_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_26_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_26_ENABLE_WORKAROUND NRF54L_ERRATA_26_PRESENT
#endif

static bool nrf54l_errata_26(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 27 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_27_PRESENT 1
#else
    #define NRF54L_ERRATA_27_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_27_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_27_ENABLE_WORKAROUND NRF54L_ERRATA_27_PRESENT
#endif

static bool nrf54l_errata_27(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 30 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_30_PRESENT 1
#else
    #define NRF54L_ERRATA_30_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_30_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_30_ENABLE_WORKAROUND NRF54L_ERRATA_30_PRESENT
#endif

static bool nrf54l_errata_30(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 31 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_31_PRESENT 1
#else
    #define NRF54L_ERRATA_31_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_31_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_31_ENABLE_WORKAROUND NRF54L_ERRATA_31_PRESENT
#endif

static bool nrf54l_errata_31(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 32 ========= */
#if    defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_32_PRESENT 1
#else
    #define NRF54L_ERRATA_32_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_32_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_32_ENABLE_WORKAROUND NRF54L_ERRATA_32_PRESENT
#endif

static bool nrf54l_errata_32(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 33 ========= */
#if    defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_33_PRESENT 1
#else
    #define NRF54L_ERRATA_33_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_33_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_33_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_33(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 35 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_35_PRESENT 1
#else
    #define NRF54L_ERRATA_35_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_35_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_35_ENABLE_WORKAROUND NRF54L_ERRATA_35_PRESENT
#endif

static bool nrf54l_errata_35(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 37 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_37_PRESENT 1
#else
    #define NRF54L_ERRATA_37_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_37_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_37_ENABLE_WORKAROUND NRF54L_ERRATA_37_PRESENT
#endif

static bool nrf54l_errata_37(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 38 ========= */
#if    defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_38_PRESENT 1
#else
    #define NRF54L_ERRATA_38_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_38_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_38_ENABLE_WORKAROUND NRF54L_ERRATA_38_PRESENT
#endif

static bool nrf54l_errata_38(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 39 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_39_PRESENT 1
#else
    #define NRF54L_ERRATA_39_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_39_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_39_ENABLE_WORKAROUND NRF54L_ERRATA_39_PRESENT
#endif

static bool nrf54l_errata_39(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 40 ========= */
#if    defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_40_PRESENT 1
#else
    #define NRF54L_ERRATA_40_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_40_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_40_ENABLE_WORKAROUND NRF54L_ERRATA_40_PRESENT
#endif

static bool nrf54l_errata_40(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 41 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_41_PRESENT 1
#else
    #define NRF54L_ERRATA_41_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_41_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_41_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_41(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 42 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_42_PRESENT 1
#else
    #define NRF54L_ERRATA_42_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_42_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_42_ENABLE_WORKAROUND NRF54L_ERRATA_42_PRESENT
#endif

static bool nrf54l_errata_42(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 43 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_43_PRESENT 1
#else
    #define NRF54L_ERRATA_43_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_43_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_43_ENABLE_WORKAROUND NRF54L_ERRATA_43_PRESENT
#endif

static bool nrf54l_errata_43(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 44 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_44_PRESENT 1
#else
    #define NRF54L_ERRATA_44_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_44_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_44_ENABLE_WORKAROUND NRF54L_ERRATA_44_PRESENT
#endif

static bool nrf54l_errata_44(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 45 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_45_PRESENT 1
#else
    #define NRF54L_ERRATA_45_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_45_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_45_ENABLE_WORKAROUND NRF54L_ERRATA_45_PRESENT
#endif

static bool nrf54l_errata_45(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 46 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_46_PRESENT 1
#else
    #define NRF54L_ERRATA_46_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_46_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_46_ENABLE_WORKAROUND NRF54L_ERRATA_46_PRESENT
#endif

static bool nrf54l_errata_46(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 47 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_47_PRESENT 1
#else
    #define NRF54L_ERRATA_47_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_47_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_47_ENABLE_WORKAROUND NRF54L_ERRATA_47_PRESENT
#endif

static bool nrf54l_errata_47(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 48 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_ERRATA_48_PRESENT 1
#else
    #define NRF54L_ERRATA_48_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_48_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_48_ENABLE_WORKAROUND NRF54L_ERRATA_48_PRESENT
#endif

static bool nrf54l_errata_48(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 49 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_49_PRESENT 1
#else
    #define NRF54L_ERRATA_49_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_49_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_49_ENABLE_WORKAROUND NRF54L_ERRATA_49_PRESENT
#endif

static bool nrf54l_errata_49(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 50 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_50_PRESENT 1
#else
    #define NRF54L_ERRATA_50_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_50_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_50_ENABLE_WORKAROUND NRF54L_ERRATA_50_PRESENT
#endif

static bool nrf54l_errata_50(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 55 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_55_PRESENT 1
#else
    #define NRF54L_ERRATA_55_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_55_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_55_ENABLE_WORKAROUND NRF54L_ERRATA_55_PRESENT
#endif

static bool nrf54l_errata_55(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 56 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
    #define NRF54L_CONFIGURATION_56_PRESENT 1
#else
    #define NRF54L_CONFIGURATION_56_PRESENT 0
#endif

#ifndef NRF54L_CONFIGURATION_56_ENABLE
    #define NRF54L_CONFIGURATION_56_ENABLE NRF54L_CONFIGURATION_56_PRESENT
#endif

static bool nrf54l_configuration_56(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 57 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_57_PRESENT 1
#else
    #define NRF54L_ERRATA_57_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_57_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_57_ENABLE_WORKAROUND NRF54L_ERRATA_57_PRESENT
#endif

static bool nrf54l_errata_57(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 60 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_60_PRESENT 1
#else
    #define NRF54L_ERRATA_60_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_60_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_60_ENABLE_WORKAROUND NRF54L_ERRATA_60_PRESENT
#endif

static bool nrf54l_errata_60(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 61 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_61_PRESENT 1
#else
    #define NRF54L_ERRATA_61_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_61_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_61_ENABLE_WORKAROUND NRF54L_ERRATA_61_PRESENT
#endif

static bool nrf54l_errata_61(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 63 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_63_PRESENT 1
#else
    #define NRF54L_ERRATA_63_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_63_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_63_ENABLE_WORKAROUND NRF54L_ERRATA_63_PRESENT
#endif

static bool nrf54l_errata_63(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 65 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_65_PRESENT 1
#else
    #define NRF54L_ERRATA_65_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_65_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_65_ENABLE_WORKAROUND NRF54L_ERRATA_65_PRESENT
#endif

static bool nrf54l_errata_65(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 66 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_66_PRESENT 1
#else
    #define NRF54L_ERRATA_66_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_66_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_66_ENABLE_WORKAROUND NRF54L_ERRATA_66_PRESENT
#endif

static bool nrf54l_errata_66(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 68 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_68_PRESENT 1
#else
    #define NRF54L_ERRATA_68_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_68_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_68_ENABLE_WORKAROUND NRF54L_ERRATA_68_PRESENT
#endif

static bool nrf54l_errata_68(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 69 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_69_PRESENT 1
#else
    #define NRF54L_ERRATA_69_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_69_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_69_ENABLE_WORKAROUND NRF54L_ERRATA_69_PRESENT
#endif

static bool nrf54l_errata_69(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 70 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_70_PRESENT 1
#else
    #define NRF54L_ERRATA_70_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_70_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_70_ENABLE_WORKAROUND NRF54L_ERRATA_70_PRESENT
#endif

static bool nrf54l_errata_70(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 71 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_71_PRESENT 1
#else
    #define NRF54L_ERRATA_71_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_71_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_71_ENABLE_WORKAROUND NRF54L_ERRATA_71_PRESENT
#endif

static bool nrf54l_errata_71(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 72 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_72_PRESENT 1
#else
    #define NRF54L_ERRATA_72_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_72_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_72_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_72(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 73 ========= */
#if    defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05) \
    || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10) \
    || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15) \
    || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA) \
    || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
    #define NRF54L_ERRATA_73_PRESENT 1
#else
    #define NRF54L_ERRATA_73_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_73_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_73_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_73(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)\
         || defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)\
         || defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)\
         || defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)\
         || defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54L15_XXAA) || defined (DEVELOP_IN_NRF54L15)
            if (var1 == 0x1C)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LV10A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LV10A_ENGA)
            if (var1 == 0x27)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        #if defined (NRF54L10_XXAA) || defined (DEVELOP_IN_NRF54L10)
            if (var1 == 0x2E)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        #if defined (NRF54L05_XXAA) || defined (DEVELOP_IN_NRF54L05)
            if (var1 == 0x2F)
            {
                switch(var2)
                {
                    case 0x01ul:
                        return true;
                    case 0x02ul:
                        return true;
                    default:
                        return true;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 74 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_74_PRESENT 1
#else
    #define NRF54L_ERRATA_74_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_74_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_74_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_74(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 78 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_78_PRESENT 1
#else
    #define NRF54L_ERRATA_78_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_78_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_78_ENABLE_WORKAROUND NRF54L_ERRATA_78_PRESENT
#endif

static bool nrf54l_errata_78(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 79 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_79_PRESENT 1
#else
    #define NRF54L_ERRATA_79_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_79_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_79_ENABLE_WORKAROUND NRF54L_ERRATA_79_PRESENT
#endif

static bool nrf54l_errata_79(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 81 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_81_PRESENT 1
#else
    #define NRF54L_ERRATA_81_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_81_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_81_ENABLE_WORKAROUND NRF54L_ERRATA_81_PRESENT
#endif

static bool nrf54l_errata_81(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 83 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_83_PRESENT 1
#else
    #define NRF54L_ERRATA_83_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_83_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_83_ENABLE_WORKAROUND NRF54L_ERRATA_83_PRESENT
#endif

static bool nrf54l_errata_83(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 85 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_85_PRESENT 1
#else
    #define NRF54L_ERRATA_85_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_85_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_85_ENABLE_WORKAROUND NRF54L_ERRATA_85_PRESENT
#endif

static bool nrf54l_errata_85(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 86 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_86_PRESENT 1
#else
    #define NRF54L_ERRATA_86_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_86_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_86_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_86(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 87 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_87_PRESENT 1
#else
    #define NRF54L_ERRATA_87_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_87_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_87_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_87(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 88 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_88_PRESENT 1
#else
    #define NRF54L_ERRATA_88_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_88_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_88_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_88(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 89 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_89_PRESENT 1
#else
    #define NRF54L_ERRATA_89_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_89_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_89_ENABLE_WORKAROUND NRF54L_ERRATA_89_PRESENT
#endif

static bool nrf54l_errata_89(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 90 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_90_PRESENT 1
#else
    #define NRF54L_ERRATA_90_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_90_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_90_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_90(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 91 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_91_PRESENT 1
#else
    #define NRF54L_ERRATA_91_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_91_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_91_ENABLE_WORKAROUND NRF54L_ERRATA_91_PRESENT
#endif

static bool nrf54l_errata_91(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 92 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_92_PRESENT 1
#else
    #define NRF54L_ERRATA_92_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_92_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_92_ENABLE_WORKAROUND NRF54L_ERRATA_92_PRESENT
#endif

static bool nrf54l_errata_92(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 93 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_93_PRESENT 1
#else
    #define NRF54L_ERRATA_93_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_93_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_93_ENABLE_WORKAROUND NRF54L_ERRATA_93_PRESENT
#endif

static bool nrf54l_errata_93(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 94 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_94_PRESENT 1
#else
    #define NRF54L_ERRATA_94_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_94_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_94_ENABLE_WORKAROUND NRF54L_ERRATA_94_PRESENT
#endif

static bool nrf54l_errata_94(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

/* ========= Errata 95 ========= */
#if    defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
    #define NRF54L_ERRATA_95_PRESENT 1
#else
    #define NRF54L_ERRATA_95_PRESENT 0
#endif

#ifndef NRF54L_ERRATA_95_ENABLE_WORKAROUND
    #define NRF54L_ERRATA_95_ENABLE_WORKAROUND 0
#endif

static bool nrf54l_errata_95(void)
{
    #ifndef NRF54L_SERIES
        return false;
    #else
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            uint32_t var1 = *(uint32_t *)0x00FFC340ul;
            uint32_t var2 = *(uint32_t *)0x00FFC344ul;
        #endif
        #if defined (NRF54LM20A_ENGA_XXAA) || defined (DEVELOP_IN_NRF54LM20A_ENGA)
            if (var1 == 0x29)
            {
                switch(var2)
                {
                    case 0x00ul:
                        return true;
                    case 0x01ul:
                        return false;
                    default:
                        return false;
                }
            }
        #endif
        return false;
    #endif
}

#endif /* NRF54L_ERRATAS_H */
