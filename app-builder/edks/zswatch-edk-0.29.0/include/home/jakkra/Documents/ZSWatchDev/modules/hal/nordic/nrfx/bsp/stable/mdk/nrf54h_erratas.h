#ifndef NRF54H_ERRATAS_H
#define NRF54H_ERRATAS_H

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

static bool nrf54h_errata_8(void) __UNUSED;
static bool nrf54h_errata_12(void) __UNUSED;
static bool nrf54h_errata_13(void) __UNUSED;
static bool nrf54h_errata_14(void) __UNUSED;
static bool nrf54h_errata_19(void) __UNUSED;
static bool nrf54h_errata_20(void) __UNUSED;
static bool nrf54h_errata_21(void) __UNUSED;
static bool nrf54h_errata_23(void) __UNUSED;
static bool nrf54h_errata_26(void) __UNUSED;
static bool nrf54h_errata_31(void) __UNUSED;
static bool nrf54h_errata_32(void) __UNUSED;
static bool nrf54h_errata_34(void) __UNUSED;
static bool nrf54h_errata_35(void) __UNUSED;
static bool nrf54h_errata_36(void) __UNUSED;
static bool nrf54h_errata_37(void) __UNUSED;
static bool nrf54h_errata_39(void) __UNUSED;
static bool nrf54h_errata_40(void) __UNUSED;
static bool nrf54h_errata_41(void) __UNUSED;
static bool nrf54h_errata_42(void) __UNUSED;
static bool nrf54h_errata_43(void) __UNUSED;
static bool nrf54h_errata_44(void) __UNUSED;
static bool nrf54h_errata_45(void) __UNUSED;
static bool nrf54h_errata_46(void) __UNUSED;
static bool nrf54h_errata_47(void) __UNUSED;
static bool nrf54h_errata_50(void) __UNUSED;
static bool nrf54h_errata_51(void) __UNUSED;
static bool nrf54h_errata_53(void) __UNUSED;
static bool nrf54h_errata_55(void) __UNUSED;
static bool nrf54h_errata_57(void) __UNUSED;
static bool nrf54h_errata_58(void) __UNUSED;
static bool nrf54h_errata_62(void) __UNUSED;
static bool nrf54h_errata_63(void) __UNUSED;
static bool nrf54h_errata_64(void) __UNUSED;
static bool nrf54h_errata_65(void) __UNUSED;
static bool nrf54h_errata_66(void) __UNUSED;
static bool nrf54h_errata_67(void) __UNUSED;
static bool nrf54h_errata_69(void) __UNUSED;
static bool nrf54h_errata_72(void) __UNUSED;
static bool nrf54h_errata_73(void) __UNUSED;
static bool nrf54h_errata_74(void) __UNUSED;
static bool nrf54h_errata_75(void) __UNUSED;
static bool nrf54h_errata_76(void) __UNUSED;
static bool nrf54h_errata_77(void) __UNUSED;
static bool nrf54h_errata_78(void) __UNUSED;
static bool nrf54h_errata_84(void) __UNUSED;
static bool nrf54h_errata_92(void) __UNUSED;
static bool nrf54h_errata_93(void) __UNUSED;
static bool nrf54h_errata_103(void) __UNUSED;
static bool nrf54h_errata_104(void) __UNUSED;
static bool nrf54h_errata_105(void) __UNUSED;
static bool nrf54h_errata_106(void) __UNUSED;
static bool nrf54h_errata_107(void) __UNUSED;
static bool nrf54h_errata_109(void) __UNUSED;
static bool nrf54h_errata_110(void) __UNUSED;
static bool nrf54h_errata_111(void) __UNUSED;
static bool nrf54h_errata_112(void) __UNUSED;
static bool nrf54h_errata_115(void) __UNUSED;
static bool nrf54h_errata_116(void) __UNUSED;
static bool nrf54h_errata_117(void) __UNUSED;
static bool nrf54h_errata_118(void) __UNUSED;
static bool nrf54h_errata_119(void) __UNUSED;
static bool nrf54h_errata_120(void) __UNUSED;
static bool nrf54h_errata_121(void) __UNUSED;
static bool nrf54h_errata_122(void) __UNUSED;
static bool nrf54h_errata_123(void) __UNUSED;
static bool nrf54h_errata_124(void) __UNUSED;
static bool nrf54h_errata_125(void) __UNUSED;
static bool nrf54h_errata_126(void) __UNUSED;
static bool nrf54h_errata_127(void) __UNUSED;
static bool nrf54h_errata_128(void) __UNUSED;
static bool nrf54h_errata_129(void) __UNUSED;
static bool nrf54h_errata_130(void) __UNUSED;
static bool nrf54h_errata_131(void) __UNUSED;
static bool nrf54h_errata_132(void) __UNUSED;
static bool nrf54h_errata_133(void) __UNUSED;
static bool nrf54h_errata_134(void) __UNUSED;
static bool nrf54h_errata_135(void) __UNUSED;
static bool nrf54h_errata_136(void) __UNUSED;
static bool nrf54h_errata_137(void) __UNUSED;
static bool nrf54h_errata_138(void) __UNUSED;
static bool nrf54h_errata_139(void) __UNUSED;
static bool nrf54h_errata_140(void) __UNUSED;
static bool nrf54h_errata_141(void) __UNUSED;
static bool nrf54h_errata_142(void) __UNUSED;
static bool nrf54h_errata_143(void) __UNUSED;
static bool nrf54h_errata_146(void) __UNUSED;
static bool nrf54h_errata_148(void) __UNUSED;
static bool nrf54h_errata_149(void) __UNUSED;
static bool nrf54h_errata_151(void) __UNUSED;
static bool nrf54h_errata_152(void) __UNUSED;
static bool nrf54h_errata_156(void) __UNUSED;
static bool nrf54h_errata_157(void) __UNUSED;
static bool nrf54h_errata_158(void) __UNUSED;
static bool nrf54h_errata_159(void) __UNUSED;
static bool nrf54h_errata_160(void) __UNUSED;
static bool nrf54h_errata_161(void) __UNUSED;
static bool nrf54h_errata_162(void) __UNUSED;
static bool nrf54h_errata_163(void) __UNUSED;
static bool nrf54h_errata_165(void) __UNUSED;
static bool nrf54h_errata_166(void) __UNUSED;
static bool nrf54h_errata_169(void) __UNUSED;
static bool nrf54h_errata_171(void) __UNUSED;
static bool nrf54h_errata_173(void) __UNUSED;
static bool nrf54h_errata_175(void) __UNUSED;
static bool nrf54h_errata_177(void) __UNUSED;
static bool nrf54h_errata_178(void) __UNUSED;
static bool nrf54h_errata_179(void) __UNUSED;
static bool nrf54h_errata_182(void) __UNUSED;
static bool nrf54h_errata_184(void) __UNUSED;
static bool nrf54h_errata_185(void) __UNUSED;
static bool nrf54h_errata_186(void) __UNUSED;
static bool nrf54h_errata_187(void) __UNUSED;
static bool nrf54h_errata_190(void) __UNUSED;
static bool nrf54h_errata_192(void) __UNUSED;
static bool nrf54h_errata_193(void) __UNUSED;
static bool nrf54h_errata_194(void) __UNUSED;
static bool nrf54h_errata_195(void) __UNUSED;
static bool nrf54h_errata_196(void) __UNUSED;
static bool nrf54h_errata_198(void) __UNUSED;
static bool nrf54h_errata_200(void) __UNUSED;
static bool nrf54h_errata_201(void) __UNUSED;
static bool nrf54h_errata_202(void) __UNUSED;
static bool nrf54h_errata_205(void) __UNUSED;
static bool nrf54h_errata_206(void) __UNUSED;
static bool nrf54h_errata_207(void) __UNUSED;
static bool nrf54h_errata_208(void) __UNUSED;
static bool nrf54h_errata_209(void) __UNUSED;
static bool nrf54h_errata_210(void) __UNUSED;
static bool nrf54h_errata_212(void) __UNUSED;
static bool nrf54h_errata_213(void) __UNUSED;
static bool nrf54h_errata_216(void) __UNUSED;
static bool nrf54h_errata_217(void) __UNUSED;
static bool nrf54h_errata_218(void) __UNUSED;
static bool nrf54h_errata_229(void) __UNUSED;
static bool nrf54h_errata_230(void) __UNUSED;
static bool nrf54h_errata_231(void) __UNUSED;

/* ========= Errata 8 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_8_PRESENT 1
#else
    #define NRF54H_ERRATA_8_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_8_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_8_ENABLE_WORKAROUND NRF54H_ERRATA_8_PRESENT
#endif

static bool nrf54h_errata_8(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 12 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_12_PRESENT 1
#else
    #define NRF54H_ERRATA_12_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_12_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_12_ENABLE_WORKAROUND NRF54H_ERRATA_12_PRESENT
#endif

static bool nrf54h_errata_12(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_13_PRESENT 1
#else
    #define NRF54H_ERRATA_13_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_13_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_13_ENABLE_WORKAROUND NRF54H_ERRATA_13_PRESENT
#endif

static bool nrf54h_errata_13(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 14 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_14_PRESENT 1
#else
    #define NRF54H_ERRATA_14_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_14_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_14_ENABLE_WORKAROUND NRF54H_ERRATA_14_PRESENT
#endif

static bool nrf54h_errata_14(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_19_PRESENT 1
#else
    #define NRF54H_ERRATA_19_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_19_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_19_ENABLE_WORKAROUND NRF54H_ERRATA_19_PRESENT
#endif

static bool nrf54h_errata_19(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_20_PRESENT 1
#else
    #define NRF54H_ERRATA_20_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_20_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_20_ENABLE_WORKAROUND NRF54H_ERRATA_20_PRESENT
#endif

static bool nrf54h_errata_20(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#define NRF54H_ERRATA_21_PRESENT 0

#ifndef NRF54H_ERRATA_21_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_21_ENABLE_WORKAROUND NRF54H_ERRATA_21_PRESENT
#endif

static bool nrf54h_errata_21(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 23 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_23_PRESENT 1
#else
    #define NRF54H_ERRATA_23_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_23_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_23_ENABLE_WORKAROUND NRF54H_ERRATA_23_PRESENT
#endif

static bool nrf54h_errata_23(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_26_PRESENT 1
#else
    #define NRF54H_ERRATA_26_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_26_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_26_ENABLE_WORKAROUND NRF54H_ERRATA_26_PRESENT
#endif

static bool nrf54h_errata_26(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_31_PRESENT 1
#else
    #define NRF54H_ERRATA_31_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_31_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_31_ENABLE_WORKAROUND NRF54H_ERRATA_31_PRESENT
#endif

static bool nrf54h_errata_31(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 32 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_32_PRESENT 1
#else
    #define NRF54H_ERRATA_32_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_32_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_32_ENABLE_WORKAROUND NRF54H_ERRATA_32_PRESENT
#endif

static bool nrf54h_errata_32(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 34 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_34_PRESENT 1
#else
    #define NRF54H_ERRATA_34_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_34_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_34_ENABLE_WORKAROUND NRF54H_ERRATA_34_PRESENT
#endif

static bool nrf54h_errata_34(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 35 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_35_PRESENT 1
#else
    #define NRF54H_ERRATA_35_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_35_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_35_ENABLE_WORKAROUND NRF54H_ERRATA_35_PRESENT
#endif

static bool nrf54h_errata_35(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 36 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_36_PRESENT 1
#else
    #define NRF54H_ERRATA_36_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_36_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_36_ENABLE_WORKAROUND NRF54H_ERRATA_36_PRESENT
#endif

static bool nrf54h_errata_36(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 37 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_37_PRESENT 1
#else
    #define NRF54H_ERRATA_37_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_37_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_37_ENABLE_WORKAROUND NRF54H_ERRATA_37_PRESENT
#endif

static bool nrf54h_errata_37(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 39 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_39_PRESENT 1
#else
    #define NRF54H_ERRATA_39_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_39_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_39_ENABLE_WORKAROUND NRF54H_ERRATA_39_PRESENT
#endif

static bool nrf54h_errata_39(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_40_PRESENT 1
#else
    #define NRF54H_ERRATA_40_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_40_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_40_ENABLE_WORKAROUND NRF54H_ERRATA_40_PRESENT
#endif

static bool nrf54h_errata_40(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 41 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_41_PRESENT 1
#else
    #define NRF54H_ERRATA_41_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_41_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_41_ENABLE_WORKAROUND NRF54H_ERRATA_41_PRESENT
#endif

static bool nrf54h_errata_41(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_42_PRESENT 1
#else
    #define NRF54H_ERRATA_42_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_42_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_42_ENABLE_WORKAROUND NRF54H_ERRATA_42_PRESENT
#endif

static bool nrf54h_errata_42(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_43_PRESENT 1
#else
    #define NRF54H_ERRATA_43_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_43_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_43_ENABLE_WORKAROUND NRF54H_ERRATA_43_PRESENT
#endif

static bool nrf54h_errata_43(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_44_PRESENT 1
#else
    #define NRF54H_ERRATA_44_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_44_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_44_ENABLE_WORKAROUND NRF54H_ERRATA_44_PRESENT
#endif

static bool nrf54h_errata_44(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_45_PRESENT 1
#else
    #define NRF54H_ERRATA_45_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_45_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_45_ENABLE_WORKAROUND NRF54H_ERRATA_45_PRESENT
#endif

static bool nrf54h_errata_45(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_46_PRESENT 1
#else
    #define NRF54H_ERRATA_46_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_46_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_46_ENABLE_WORKAROUND NRF54H_ERRATA_46_PRESENT
#endif

static bool nrf54h_errata_46(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_47_PRESENT 1
#else
    #define NRF54H_ERRATA_47_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_47_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_47_ENABLE_WORKAROUND NRF54H_ERRATA_47_PRESENT
#endif

static bool nrf54h_errata_47(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_50_PRESENT 1
#else
    #define NRF54H_ERRATA_50_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_50_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_50_ENABLE_WORKAROUND NRF54H_ERRATA_50_PRESENT
#endif

static bool nrf54h_errata_50(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 51 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_51_PRESENT 1
#else
    #define NRF54H_ERRATA_51_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_51_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_51_ENABLE_WORKAROUND NRF54H_ERRATA_51_PRESENT
#endif

static bool nrf54h_errata_51(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 53 ========= */
#define NRF54H_ERRATA_53_PRESENT 0

#ifndef NRF54H_ERRATA_53_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_53_ENABLE_WORKAROUND NRF54H_ERRATA_53_PRESENT
#endif

static bool nrf54h_errata_53(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 55 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_55_PRESENT 1
#else
    #define NRF54H_ERRATA_55_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_55_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_55_ENABLE_WORKAROUND NRF54H_ERRATA_55_PRESENT
#endif

static bool nrf54h_errata_55(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_57_PRESENT 1
#else
    #define NRF54H_ERRATA_57_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_57_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_57_ENABLE_WORKAROUND NRF54H_ERRATA_57_PRESENT
#endif

static bool nrf54h_errata_57(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 58 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_58_PRESENT 1
#else
    #define NRF54H_ERRATA_58_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_58_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_58_ENABLE_WORKAROUND NRF54H_ERRATA_58_PRESENT
#endif

static bool nrf54h_errata_58(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 62 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_62_PRESENT 1
#else
    #define NRF54H_ERRATA_62_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_62_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_62_ENABLE_WORKAROUND NRF54H_ERRATA_62_PRESENT
#endif

static bool nrf54h_errata_62(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_63_PRESENT 1
#else
    #define NRF54H_ERRATA_63_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_63_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_63_ENABLE_WORKAROUND NRF54H_ERRATA_63_PRESENT
#endif

static bool nrf54h_errata_63(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 64 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_64_PRESENT 1
#else
    #define NRF54H_ERRATA_64_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_64_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_64_ENABLE_WORKAROUND NRF54H_ERRATA_64_PRESENT
#endif

static bool nrf54h_errata_64(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_65_PRESENT 1
#else
    #define NRF54H_ERRATA_65_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_65_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_65_ENABLE_WORKAROUND NRF54H_ERRATA_65_PRESENT
#endif

static bool nrf54h_errata_65(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 66 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_66_PRESENT 1
#else
    #define NRF54H_ERRATA_66_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_66_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_66_ENABLE_WORKAROUND NRF54H_ERRATA_66_PRESENT
#endif

static bool nrf54h_errata_66(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 67 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_67_PRESENT 1
#else
    #define NRF54H_ERRATA_67_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_67_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_67_ENABLE_WORKAROUND NRF54H_ERRATA_67_PRESENT
#endif

static bool nrf54h_errata_67(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_69_PRESENT 1
#else
    #define NRF54H_ERRATA_69_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_69_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_69_ENABLE_WORKAROUND NRF54H_ERRATA_69_PRESENT
#endif

static bool nrf54h_errata_69(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 72 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_72_PRESENT 1
#else
    #define NRF54H_ERRATA_72_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_72_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_72_ENABLE_WORKAROUND NRF54H_ERRATA_72_PRESENT
#endif

static bool nrf54h_errata_72(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_73_PRESENT 1
#else
    #define NRF54H_ERRATA_73_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_73_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_73_ENABLE_WORKAROUND NRF54H_ERRATA_73_PRESENT
#endif

static bool nrf54h_errata_73(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_74_PRESENT 1
#else
    #define NRF54H_ERRATA_74_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_74_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_74_ENABLE_WORKAROUND NRF54H_ERRATA_74_PRESENT
#endif

static bool nrf54h_errata_74(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 75 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_75_PRESENT 1
#else
    #define NRF54H_ERRATA_75_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_75_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_75_ENABLE_WORKAROUND NRF54H_ERRATA_75_PRESENT
#endif

static bool nrf54h_errata_75(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 76 ========= */
#define NRF54H_ERRATA_76_PRESENT 0

#ifndef NRF54H_ERRATA_76_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_76_ENABLE_WORKAROUND NRF54H_ERRATA_76_PRESENT
#endif

static bool nrf54h_errata_76(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 77 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_77_PRESENT 1
#else
    #define NRF54H_ERRATA_77_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_77_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_77_ENABLE_WORKAROUND NRF54H_ERRATA_77_PRESENT
#endif

static bool nrf54h_errata_77(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 78 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_78_PRESENT 1
#else
    #define NRF54H_ERRATA_78_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_78_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_78_ENABLE_WORKAROUND NRF54H_ERRATA_78_PRESENT
#endif

static bool nrf54h_errata_78(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 84 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_84_PRESENT 1
#else
    #define NRF54H_ERRATA_84_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_84_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_84_ENABLE_WORKAROUND NRF54H_ERRATA_84_PRESENT
#endif

static bool nrf54h_errata_84(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 92 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_92_PRESENT 1
#else
    #define NRF54H_ERRATA_92_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_92_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_92_ENABLE_WORKAROUND NRF54H_ERRATA_92_PRESENT
#endif

static bool nrf54h_errata_92(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 93 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_93_PRESENT 1
#else
    #define NRF54H_ERRATA_93_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_93_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_93_ENABLE_WORKAROUND NRF54H_ERRATA_93_PRESENT
#endif

static bool nrf54h_errata_93(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 103 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_103_PRESENT 1
#else
    #define NRF54H_ERRATA_103_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_103_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_103_ENABLE_WORKAROUND NRF54H_ERRATA_103_PRESENT
#endif

static bool nrf54h_errata_103(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 104 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_104_PRESENT 1
#else
    #define NRF54H_ERRATA_104_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_104_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_104_ENABLE_WORKAROUND 0
#endif

static bool nrf54h_errata_104(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 105 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_105_PRESENT 1
#else
    #define NRF54H_ERRATA_105_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_105_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_105_ENABLE_WORKAROUND NRF54H_ERRATA_105_PRESENT
#endif

static bool nrf54h_errata_105(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 106 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_106_PRESENT 1
#else
    #define NRF54H_ERRATA_106_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_106_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_106_ENABLE_WORKAROUND NRF54H_ERRATA_106_PRESENT
#endif

static bool nrf54h_errata_106(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 107 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_107_PRESENT 1
#else
    #define NRF54H_ERRATA_107_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_107_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_107_ENABLE_WORKAROUND NRF54H_ERRATA_107_PRESENT
#endif

static bool nrf54h_errata_107(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 109 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_109_PRESENT 1
#else
    #define NRF54H_ERRATA_109_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_109_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_109_ENABLE_WORKAROUND NRF54H_ERRATA_109_PRESENT
#endif

static bool nrf54h_errata_109(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 110 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_110_PRESENT 1
#else
    #define NRF54H_ERRATA_110_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_110_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_110_ENABLE_WORKAROUND NRF54H_ERRATA_110_PRESENT
#endif

static bool nrf54h_errata_110(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 111 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_111_PRESENT 1
#else
    #define NRF54H_ERRATA_111_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_111_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_111_ENABLE_WORKAROUND NRF54H_ERRATA_111_PRESENT
#endif

static bool nrf54h_errata_111(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 112 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_112_PRESENT 1
#else
    #define NRF54H_ERRATA_112_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_112_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_112_ENABLE_WORKAROUND NRF54H_ERRATA_112_PRESENT
#endif

static bool nrf54h_errata_112(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 115 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_115_PRESENT 1
#else
    #define NRF54H_ERRATA_115_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_115_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_115_ENABLE_WORKAROUND NRF54H_ERRATA_115_PRESENT
#endif

static bool nrf54h_errata_115(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 116 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_116_PRESENT 1
#else
    #define NRF54H_ERRATA_116_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_116_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_116_ENABLE_WORKAROUND NRF54H_ERRATA_116_PRESENT
#endif

static bool nrf54h_errata_116(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 117 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_117_PRESENT 1
#else
    #define NRF54H_ERRATA_117_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_117_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_117_ENABLE_WORKAROUND NRF54H_ERRATA_117_PRESENT
#endif

static bool nrf54h_errata_117(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 118 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_118_PRESENT 1
#else
    #define NRF54H_ERRATA_118_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_118_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_118_ENABLE_WORKAROUND NRF54H_ERRATA_118_PRESENT
#endif

static bool nrf54h_errata_118(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 119 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_119_PRESENT 1
#else
    #define NRF54H_ERRATA_119_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_119_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_119_ENABLE_WORKAROUND NRF54H_ERRATA_119_PRESENT
#endif

static bool nrf54h_errata_119(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 120 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_120_PRESENT 1
#else
    #define NRF54H_ERRATA_120_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_120_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_120_ENABLE_WORKAROUND NRF54H_ERRATA_120_PRESENT
#endif

static bool nrf54h_errata_120(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 121 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_121_PRESENT 1
#else
    #define NRF54H_ERRATA_121_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_121_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_121_ENABLE_WORKAROUND 0
#endif

static bool nrf54h_errata_121(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 122 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_122_PRESENT 1
#else
    #define NRF54H_ERRATA_122_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_122_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_122_ENABLE_WORKAROUND NRF54H_ERRATA_122_PRESENT
#endif

static bool nrf54h_errata_122(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 123 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_123_PRESENT 1
#else
    #define NRF54H_ERRATA_123_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_123_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_123_ENABLE_WORKAROUND NRF54H_ERRATA_123_PRESENT
#endif

static bool nrf54h_errata_123(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 124 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_124_PRESENT 1
#else
    #define NRF54H_ERRATA_124_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_124_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_124_ENABLE_WORKAROUND 0
#endif

static bool nrf54h_errata_124(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 125 ========= */
#define NRF54H_ERRATA_125_PRESENT 0

#ifndef NRF54H_ERRATA_125_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_125_ENABLE_WORKAROUND NRF54H_ERRATA_125_PRESENT
#endif

static bool nrf54h_errata_125(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 126 ========= */
#define NRF54H_ERRATA_126_PRESENT 0

#ifndef NRF54H_ERRATA_126_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_126_ENABLE_WORKAROUND NRF54H_ERRATA_126_PRESENT
#endif

static bool nrf54h_errata_126(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 127 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_127_PRESENT 1
#else
    #define NRF54H_ERRATA_127_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_127_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_127_ENABLE_WORKAROUND NRF54H_ERRATA_127_PRESENT
#endif

static bool nrf54h_errata_127(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 128 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_128_PRESENT 1
#else
    #define NRF54H_ERRATA_128_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_128_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_128_ENABLE_WORKAROUND NRF54H_ERRATA_128_PRESENT
#endif

static bool nrf54h_errata_128(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 129 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_129_PRESENT 1
#else
    #define NRF54H_ERRATA_129_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_129_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_129_ENABLE_WORKAROUND NRF54H_ERRATA_129_PRESENT
#endif

static bool nrf54h_errata_129(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 130 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_130_PRESENT 1
#else
    #define NRF54H_ERRATA_130_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_130_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_130_ENABLE_WORKAROUND NRF54H_ERRATA_130_PRESENT
#endif

static bool nrf54h_errata_130(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 131 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_131_PRESENT 1
#else
    #define NRF54H_ERRATA_131_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_131_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_131_ENABLE_WORKAROUND NRF54H_ERRATA_131_PRESENT
#endif

static bool nrf54h_errata_131(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 132 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_132_PRESENT 1
#else
    #define NRF54H_ERRATA_132_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_132_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_132_ENABLE_WORKAROUND NRF54H_ERRATA_132_PRESENT
#endif

static bool nrf54h_errata_132(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 133 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_133_PRESENT 1
#else
    #define NRF54H_ERRATA_133_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_133_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_133_ENABLE_WORKAROUND NRF54H_ERRATA_133_PRESENT
#endif

static bool nrf54h_errata_133(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 134 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_134_PRESENT 1
#else
    #define NRF54H_ERRATA_134_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_134_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_134_ENABLE_WORKAROUND NRF54H_ERRATA_134_PRESENT
#endif

static bool nrf54h_errata_134(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 135 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_135_PRESENT 1
#else
    #define NRF54H_ERRATA_135_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_135_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_135_ENABLE_WORKAROUND NRF54H_ERRATA_135_PRESENT
#endif

static bool nrf54h_errata_135(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 136 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_136_PRESENT 1
#else
    #define NRF54H_ERRATA_136_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_136_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_136_ENABLE_WORKAROUND NRF54H_ERRATA_136_PRESENT
#endif

static bool nrf54h_errata_136(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 137 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_137_PRESENT 1
#else
    #define NRF54H_ERRATA_137_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_137_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_137_ENABLE_WORKAROUND NRF54H_ERRATA_137_PRESENT
#endif

static bool nrf54h_errata_137(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 138 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_138_PRESENT 1
#else
    #define NRF54H_ERRATA_138_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_138_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_138_ENABLE_WORKAROUND NRF54H_ERRATA_138_PRESENT
#endif

static bool nrf54h_errata_138(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 139 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_139_PRESENT 1
#else
    #define NRF54H_ERRATA_139_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_139_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_139_ENABLE_WORKAROUND NRF54H_ERRATA_139_PRESENT
#endif

static bool nrf54h_errata_139(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 140 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_140_PRESENT 1
#else
    #define NRF54H_ERRATA_140_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_140_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_140_ENABLE_WORKAROUND NRF54H_ERRATA_140_PRESENT
#endif

static bool nrf54h_errata_140(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 141 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_141_PRESENT 1
#else
    #define NRF54H_ERRATA_141_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_141_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_141_ENABLE_WORKAROUND NRF54H_ERRATA_141_PRESENT
#endif

static bool nrf54h_errata_141(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 142 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_142_PRESENT 1
#else
    #define NRF54H_ERRATA_142_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_142_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_142_ENABLE_WORKAROUND NRF54H_ERRATA_142_PRESENT
#endif

static bool nrf54h_errata_142(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 143 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_143_PRESENT 1
#else
    #define NRF54H_ERRATA_143_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_143_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_143_ENABLE_WORKAROUND NRF54H_ERRATA_143_PRESENT
#endif

static bool nrf54h_errata_143(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 146 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_146_PRESENT 1
#else
    #define NRF54H_ERRATA_146_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_146_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_146_ENABLE_WORKAROUND NRF54H_ERRATA_146_PRESENT
#endif

static bool nrf54h_errata_146(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 148 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_148_PRESENT 1
#else
    #define NRF54H_ERRATA_148_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_148_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_148_ENABLE_WORKAROUND NRF54H_ERRATA_148_PRESENT
#endif

static bool nrf54h_errata_148(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 149 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_149_PRESENT 1
#else
    #define NRF54H_ERRATA_149_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_149_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_149_ENABLE_WORKAROUND NRF54H_ERRATA_149_PRESENT
#endif

static bool nrf54h_errata_149(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 151 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_151_PRESENT 1
#else
    #define NRF54H_ERRATA_151_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_151_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_151_ENABLE_WORKAROUND NRF54H_ERRATA_151_PRESENT
#endif

static bool nrf54h_errata_151(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 152 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_152_PRESENT 1
#else
    #define NRF54H_ERRATA_152_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_152_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_152_ENABLE_WORKAROUND NRF54H_ERRATA_152_PRESENT
#endif

static bool nrf54h_errata_152(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 156 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_156_PRESENT 1
#else
    #define NRF54H_ERRATA_156_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_156_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_156_ENABLE_WORKAROUND NRF54H_ERRATA_156_PRESENT
#endif

static bool nrf54h_errata_156(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 157 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_157_PRESENT 1
#else
    #define NRF54H_ERRATA_157_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_157_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_157_ENABLE_WORKAROUND NRF54H_ERRATA_157_PRESENT
#endif

static bool nrf54h_errata_157(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 158 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_158_PRESENT 1
#else
    #define NRF54H_ERRATA_158_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_158_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_158_ENABLE_WORKAROUND NRF54H_ERRATA_158_PRESENT
#endif

static bool nrf54h_errata_158(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 159 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_159_PRESENT 1
#else
    #define NRF54H_ERRATA_159_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_159_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_159_ENABLE_WORKAROUND NRF54H_ERRATA_159_PRESENT
#endif

static bool nrf54h_errata_159(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 160 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_160_PRESENT 1
#else
    #define NRF54H_ERRATA_160_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_160_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_160_ENABLE_WORKAROUND NRF54H_ERRATA_160_PRESENT
#endif

static bool nrf54h_errata_160(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 161 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_161_PRESENT 1
#else
    #define NRF54H_ERRATA_161_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_161_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_161_ENABLE_WORKAROUND NRF54H_ERRATA_161_PRESENT
#endif

static bool nrf54h_errata_161(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 162 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_162_PRESENT 1
#else
    #define NRF54H_ERRATA_162_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_162_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_162_ENABLE_WORKAROUND NRF54H_ERRATA_162_PRESENT
#endif

static bool nrf54h_errata_162(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 163 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_163_PRESENT 1
#else
    #define NRF54H_ERRATA_163_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_163_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_163_ENABLE_WORKAROUND NRF54H_ERRATA_163_PRESENT
#endif

static bool nrf54h_errata_163(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 165 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_165_PRESENT 1
#else
    #define NRF54H_ERRATA_165_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_165_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_165_ENABLE_WORKAROUND NRF54H_ERRATA_165_PRESENT
#endif

static bool nrf54h_errata_165(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 166 ========= */
#define NRF54H_ERRATA_166_PRESENT 0

#ifndef NRF54H_ERRATA_166_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_166_ENABLE_WORKAROUND NRF54H_ERRATA_166_PRESENT
#endif

static bool nrf54h_errata_166(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        return false;
    #endif
}

/* ========= Errata 169 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_169_PRESENT 1
#else
    #define NRF54H_ERRATA_169_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_169_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_169_ENABLE_WORKAROUND NRF54H_ERRATA_169_PRESENT
#endif

static bool nrf54h_errata_169(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 171 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_171_PRESENT 1
#else
    #define NRF54H_ERRATA_171_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_171_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_171_ENABLE_WORKAROUND NRF54H_ERRATA_171_PRESENT
#endif

static bool nrf54h_errata_171(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 173 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_173_PRESENT 1
#else
    #define NRF54H_ERRATA_173_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_173_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_173_ENABLE_WORKAROUND NRF54H_ERRATA_173_PRESENT
#endif

static bool nrf54h_errata_173(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 175 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_175_PRESENT 1
#else
    #define NRF54H_ERRATA_175_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_175_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_175_ENABLE_WORKAROUND NRF54H_ERRATA_175_PRESENT
#endif

static bool nrf54h_errata_175(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 177 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_177_PRESENT 1
#else
    #define NRF54H_ERRATA_177_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_177_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_177_ENABLE_WORKAROUND NRF54H_ERRATA_177_PRESENT
#endif

static bool nrf54h_errata_177(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 178 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_178_PRESENT 1
#else
    #define NRF54H_ERRATA_178_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_178_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_178_ENABLE_WORKAROUND NRF54H_ERRATA_178_PRESENT
#endif

static bool nrf54h_errata_178(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 179 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_179_PRESENT 1
#else
    #define NRF54H_ERRATA_179_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_179_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_179_ENABLE_WORKAROUND NRF54H_ERRATA_179_PRESENT
#endif

static bool nrf54h_errata_179(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 182 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_182_PRESENT 1
#else
    #define NRF54H_ERRATA_182_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_182_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_182_ENABLE_WORKAROUND NRF54H_ERRATA_182_PRESENT
#endif

static bool nrf54h_errata_182(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 184 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_184_PRESENT 1
#else
    #define NRF54H_ERRATA_184_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_184_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_184_ENABLE_WORKAROUND NRF54H_ERRATA_184_PRESENT
#endif

static bool nrf54h_errata_184(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 185 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_185_PRESENT 1
#else
    #define NRF54H_ERRATA_185_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_185_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_185_ENABLE_WORKAROUND NRF54H_ERRATA_185_PRESENT
#endif

static bool nrf54h_errata_185(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 186 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_186_PRESENT 1
#else
    #define NRF54H_ERRATA_186_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_186_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_186_ENABLE_WORKAROUND NRF54H_ERRATA_186_PRESENT
#endif

static bool nrf54h_errata_186(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 187 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_187_PRESENT 1
#else
    #define NRF54H_ERRATA_187_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_187_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_187_ENABLE_WORKAROUND NRF54H_ERRATA_187_PRESENT
#endif

static bool nrf54h_errata_187(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 190 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_190_PRESENT 1
#else
    #define NRF54H_ERRATA_190_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_190_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_190_ENABLE_WORKAROUND NRF54H_ERRATA_190_PRESENT
#endif

static bool nrf54h_errata_190(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 192 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_192_PRESENT 1
#else
    #define NRF54H_ERRATA_192_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_192_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_192_ENABLE_WORKAROUND NRF54H_ERRATA_192_PRESENT
#endif

static bool nrf54h_errata_192(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 193 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_193_PRESENT 1
#else
    #define NRF54H_ERRATA_193_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_193_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_193_ENABLE_WORKAROUND NRF54H_ERRATA_193_PRESENT
#endif

static bool nrf54h_errata_193(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 194 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_194_PRESENT 1
#else
    #define NRF54H_ERRATA_194_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_194_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_194_ENABLE_WORKAROUND NRF54H_ERRATA_194_PRESENT
#endif

static bool nrf54h_errata_194(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 195 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_195_PRESENT 1
#else
    #define NRF54H_ERRATA_195_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_195_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_195_ENABLE_WORKAROUND NRF54H_ERRATA_195_PRESENT
#endif

static bool nrf54h_errata_195(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 196 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_196_PRESENT 1
#else
    #define NRF54H_ERRATA_196_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_196_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_196_ENABLE_WORKAROUND NRF54H_ERRATA_196_PRESENT
#endif

static bool nrf54h_errata_196(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 198 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_198_PRESENT 1
#else
    #define NRF54H_ERRATA_198_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_198_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_198_ENABLE_WORKAROUND NRF54H_ERRATA_198_PRESENT
#endif

static bool nrf54h_errata_198(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 200 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_200_PRESENT 1
#else
    #define NRF54H_ERRATA_200_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_200_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_200_ENABLE_WORKAROUND NRF54H_ERRATA_200_PRESENT
#endif

static bool nrf54h_errata_200(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 201 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_201_PRESENT 1
#else
    #define NRF54H_ERRATA_201_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_201_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_201_ENABLE_WORKAROUND NRF54H_ERRATA_201_PRESENT
#endif

static bool nrf54h_errata_201(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 202 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_202_PRESENT 1
#else
    #define NRF54H_ERRATA_202_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_202_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_202_ENABLE_WORKAROUND NRF54H_ERRATA_202_PRESENT
#endif

static bool nrf54h_errata_202(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 205 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_205_PRESENT 1
#else
    #define NRF54H_ERRATA_205_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_205_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_205_ENABLE_WORKAROUND NRF54H_ERRATA_205_PRESENT
#endif

static bool nrf54h_errata_205(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 206 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_206_PRESENT 1
#else
    #define NRF54H_ERRATA_206_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_206_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_206_ENABLE_WORKAROUND NRF54H_ERRATA_206_PRESENT
#endif

static bool nrf54h_errata_206(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 207 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_207_PRESENT 1
#else
    #define NRF54H_ERRATA_207_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_207_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_207_ENABLE_WORKAROUND NRF54H_ERRATA_207_PRESENT
#endif

static bool nrf54h_errata_207(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 208 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_208_PRESENT 1
#else
    #define NRF54H_ERRATA_208_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_208_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_208_ENABLE_WORKAROUND NRF54H_ERRATA_208_PRESENT
#endif

static bool nrf54h_errata_208(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 209 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_209_PRESENT 1
#else
    #define NRF54H_ERRATA_209_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_209_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_209_ENABLE_WORKAROUND NRF54H_ERRATA_209_PRESENT
#endif

static bool nrf54h_errata_209(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 210 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_210_PRESENT 1
#else
    #define NRF54H_ERRATA_210_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_210_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_210_ENABLE_WORKAROUND NRF54H_ERRATA_210_PRESENT
#endif

static bool nrf54h_errata_210(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 212 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_212_PRESENT 1
#else
    #define NRF54H_ERRATA_212_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_212_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_212_ENABLE_WORKAROUND NRF54H_ERRATA_212_PRESENT
#endif

static bool nrf54h_errata_212(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 213 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_213_PRESENT 1
#else
    #define NRF54H_ERRATA_213_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_213_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_213_ENABLE_WORKAROUND NRF54H_ERRATA_213_PRESENT
#endif

static bool nrf54h_errata_213(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 216 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_216_PRESENT 1
#else
    #define NRF54H_ERRATA_216_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_216_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_216_ENABLE_WORKAROUND NRF54H_ERRATA_216_PRESENT
#endif

static bool nrf54h_errata_216(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 217 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_217_PRESENT 1
#else
    #define NRF54H_ERRATA_217_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_217_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_217_ENABLE_WORKAROUND NRF54H_ERRATA_217_PRESENT
#endif

static bool nrf54h_errata_217(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 218 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_218_PRESENT 1
#else
    #define NRF54H_ERRATA_218_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_218_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_218_ENABLE_WORKAROUND NRF54H_ERRATA_218_PRESENT
#endif

static bool nrf54h_errata_218(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 229 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_229_PRESENT 1
#else
    #define NRF54H_ERRATA_229_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_229_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_229_ENABLE_WORKAROUND NRF54H_ERRATA_229_PRESENT
#endif

static bool nrf54h_errata_229(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 230 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_230_PRESENT 1
#else
    #define NRF54H_ERRATA_230_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_230_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_230_ENABLE_WORKAROUND NRF54H_ERRATA_230_PRESENT
#endif

static bool nrf54h_errata_230(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

/* ========= Errata 231 ========= */
#if    defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
    #define NRF54H_ERRATA_231_PRESENT 1
#else
    #define NRF54H_ERRATA_231_PRESENT 0
#endif

#ifndef NRF54H_ERRATA_231_ENABLE_WORKAROUND
    #define NRF54H_ERRATA_231_ENABLE_WORKAROUND NRF54H_ERRATA_231_PRESENT
#endif

static bool nrf54h_errata_231(void)
{
    #ifndef NRF54H_SERIES
        return false;
    #else
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            uint32_t var1 = *(uint32_t *)0x0FFFE000ul;
            uint32_t var2 = *(uint32_t *)0x0FFFE004ul;
        #endif
        #if defined (NRF54H20_XXAA) || defined (DEVELOP_IN_NRF54H20)
            if (var1 == 0x16)
            {
                switch(var2)
                {
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

#endif /* NRF54H_ERRATAS_H */
