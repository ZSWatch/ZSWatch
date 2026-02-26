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

#ifndef NRFX_UTILS_INTERNAL_H__
#define NRFX_UTILS_INTERNAL_H__

/* NRFX_IS_ENABLED() helpers */

/* This is called from NRFX_IS_ENABLED(), and sticks on a "_XXXX" prefix,
 * it will now be "_XXXX1" if config_macro is "1", or just "_XXXX" if it's
 * undefined.
 *   ENABLED:   NRFX_IS_ENABLED2(_XXXX1)
 *   DISABLED   NRFX_IS_ENABLED2(_XXXX)
 */
#define _NRFX_IS_ENABLED1(config_macro) _NRFX_IS_ENABLED2(_XXXX##config_macro)

/* Here's the core trick, we map "_XXXX1" to "_YYYY," (i.e. a string
 * with a trailing comma), so it has the effect of making this a
 * two-argument tuple to the preprocessor only in the case where the
 * value is defined to "1"
 *   ENABLED:    _YYYY,    <--- note comma!
 *   DISABLED:   _XXXX
 */
#define _XXXX1 _YYYY,

/* Then we append an extra argument to fool the gcc preprocessor into
 * accepting it as a varargs macro.
 *                         arg1   arg2  arg3
 *   ENABLED:   NRFX_IS_ENABLED3(_YYYY,    1,    0)
 *   DISABLED   NRFX_IS_ENABLED3(_XXXX 1,  0)
 */
#define _NRFX_IS_ENABLED2(one_or_two_args) _NRFX_IS_ENABLED3(one_or_two_args 1, 0)

/* And our second argument is thus now cooked to be 1 in the case
 * where the value is defined to 1, and 0 if not:
 */
#define _NRFX_IS_ENABLED3(ignore_this, val, ...) val

/* Used internally by NRFX_COND_CODE_1 and NRFX_COND_CODE_0. */
#define _NRFX_COND_CODE_1(_flag, _if_1_code, _else_code) \
    _NRFX_COND_CODE1(_XXXX##_flag, _if_1_code, _else_code)

#define _NRFX_COND_CODE_0(_flag, _if_0_code, _else_code) \
    _NRFX_COND_CODE1(_ZZZZ##_flag, _if_0_code, _else_code)

#define _ZZZZ0 _YYYY,

#define _NRFX_COND_CODE1(one_or_two_args, _if_code, _else_code) \
    NRFX_GET_ARG2_DEBRACKET(one_or_two_args _if_code, _else_code)

#define NRFX_GET_ARG2_DEBRACKET(ignore_this, val, ...) NRFX_DEBRACKET val

#define NRFX_DEBRACKET(...) __VA_ARGS__

#define NRFX_EVAL(...) __VA_ARGS__

#define NRFX_EMPTY()

/* Helper macros used for @ref NRFX_MAX_N. */
#define _NRFX_MAX_P1(x) NRFX_MAX NRFX_EMPTY() ((x),
#define _NRFX_MAX_P2(x) )

/* Implementation details for NRFX_NUM_VA_ARGS_LESS_1 */
#define _NRFX_NUM_VA_ARGS_LESS_1_IMPL(\
            _ignored,\
            _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,\
            _11, _12, _13, _14, _15, _16, _17, _18, _19, _20,\
            _21, _22, _23, _24, _25, _26, _27, _28, _29, _30,\
            _31, _32, _33, _34, _35, _36, _37, _38, _39, _40,\
            _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,\
            _51, _52, _53, _54, _55, _56, _57, _58, _59, _60,\
            _61, _62, N, ...) N

/* Intermediate macros needed for @ref NRFX_FEATURE_PRESENT. */
#define NRFX_INSTANCE_FEATURE_PRESENT(i, _instance_name, _feature_name) \
        NRFX_COND_CODE_1(NRFX_CONCAT_3(_instance_name, i, _feature_name), (1), ())

#define _NRFX_FEATURE_PRESENT(_instance_name, _feature_name, _rpt) \
        NRFX_LISTIFY(_rpt, NRFX_INSTANCE_FEATURE_PRESENT, (), _instance_name, _feature_name)


/** Used by @ref NRFX_FOREACH_ENABLED. Execute provided macro if driver instance is enabled.
 *
 * @param[in] i           Instance index.
 * @param[in] off_code    Code which is pasted when given driver instance is disabled.
 *                        Must be given in parentheses.
 * @param[in] periph_name Peripheral name, e.g. SPIM.
 * @param[in] prefix      Prefix added before instance index, e.g. some device has
 *                        instances named like SPIM00. First 0 is passed here as prefix.
 * @param[in] macro       Macro which is executed.
 * @param[in] ...         Variable length arguments passed to the @p macro. Macro has following
 *                        arguments: macro(periph_name, prefix, i, ...).
 */
#define _NRFX_EVAL_IF_ENABLED(i, off_code, periph_name, prefix, macro, ...) \
        NRFX_COND_CODE_1(NRFX_CONCAT(NRFX_, periph_name, prefix, i, _ENABLED), \
                    (macro(periph_name, prefix, i, __VA_ARGS__)), \
                    off_code)

/** Used by @ref NRFX_FOREACH_PRESENT. Execute provided macro if instance is present.
 *
 * Presence is determined by existing of token NRF_\<instance\> defined with wrapped
 * in parenthesis value (see @ref NRFX_INSTANCE_PRESENT), where <instance\> is the concatenation
 * of @p periph_name, @p prefix and @p i.
 *
 * @param[in] i           Instance index.
 * @param[in] off_code    Code which is pasted when given driver instance is disabled.
 *                        Must be given in parentheses.
 * @param[in] periph_name Peripheral name, e.g. SPIM.
 * @param[in] prefix      Prefix added before instance index, e.g. some device has
 *                        instances named like SPIM00. First 0 is passed here as prefix.
 * @param[in] macro       Macro which is executed.
 * @param[in] ...         Variable length arguments passed to the @p macro. Macro has following
 *                        arguments: macro(periph_name, prefix, i, ...).
 */
#define _NRFX_EVAL_IF_PRESENT(i, off_code, periph_name, prefix, macro, ...) \
        NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(NRFX_CONCAT(periph_name, prefix, i)), \
                    (macro(periph_name, prefix, i, __VA_ARGS__)), \
                    off_code)

/* Macro used for enabled driver instances enum generation. */
#define _NRFX_INST_ENUM(periph_name, prefix, i, _) \
    NRFX_CONCAT(NRFX_, periph_name, prefix, i, _INST_IDX),

/* Macro used for generation of irq handlers.
 *
 * Macro is using enum created by _NRFX_INSG_ENUM macro.
 *
 * @param[in] periph_name       Peripheral name, e.g. SPIM.
 * @param[in] prefix            Prefix appended to the index.
 * @param[in] i                 Index.
 * @param[in] periph_name_small Peripheral name in small letters, e.g. spim.
 */
#define _NRFX_IRQ_HANDLER(periph_name, prefix, i, periph_name_small)                         \
void NRFX_CONCAT(nrfx_, periph_name_small, _, prefix, i, _irq_handler)(void *)               \
{                                                                                            \
    irq_handler(NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(NRFX_CONCAT(periph_name, prefix, i)), \
                                 (NRFX_CONCAT(NRF_, periph_name, prefix, i)), (NULL)),       \
                &m_cb[NRFX_CONCAT(NRFX_, periph_name, prefix, i, _INST_IDX)]);               \
}

/* Macro used for generation of irq handlers with addtional parameter.
 *
 * Additional parameter passed to the interrupt handler is a value returned by
 * @p ext_macro. One of the use cases is a peripheral with variable number of
 * channels (e.g. RTC or TIMER).
 *
 * Macro is using enum created by _NRFX_INSG_ENUM macro.
 *
 * @param[in] periph_name       Peripheral name, e.g. SPIM.
 * @param[in] prefix            Prefix appended to the index.
 * @param[in] i                 Index.
 * @param[in] periph_name_small Peripheral name in small letters, e.g. spim.
 * @param[in] ext_macro         Macro called as third parameter of the handler.
 */
#define _NRFX_IRQ_HANDLER_EXT(periph_name, prefix, i, periph_name_small, ext_macro) \
void NRFX_CONCAT(nrfx_, periph_name_small, _, prefix, i, _irq_handler)(void *)      \
{                                                                                   \
    irq_handler(NRFX_CONCAT(NRF_, periph_name, prefix, i),                          \
                &m_cb[NRFX_CONCAT(NRFX_, periph_name, prefix, i, _INST_IDX)],       \
                ext_macro(NRFX_CONCAT(prefix, i)));                                 \
}

#define _NRFX_IRQ_HANDLER_LIST(periph_name, prefix, i, periph_name_small) \
    NRFX_CONCAT(nrfx_, periph_name_small, _, prefix, i, _irq_handler),

#define _NRFX_IRQ_HANDLER_DECLARE(periph_name, prefix, i, periph_name_small) \
    void NRFX_CONCAT(nrfx_, periph_name_small, _, prefix, i, _irq_handler)(void *);

#define _PERIPH_PTR_COMPARE(periph_name, prefix, idx, reg)                       \
    ((uintptr_t)reg == (uintptr_t)NRFX_CONCAT(NRF_, periph_name, prefix, idx)) ? \
    (NRFX_CONCAT(NRFX_, periph_name, prefix, idx, _INST_IDX)) :

/* Macro for getting the fourth argument from the set of input arguments. */
#define __NRFX_GET_ARG4(arg1, arg2, arg3, arg4, ...) arg4
#define _NRFX_GET_ARG4(...) __NRFX_GET_ARG4(__VA_ARGS__)

/* Macro for getting the third argument from the set of input arguments. */
#define __NRFX_GET_ARG3(arg1, arg2, arg3, ...) arg3
#define _NRFX_GET_ARG3(...) __NRFX_GET_ARG3(__VA_ARGS__)

/* Macro for getting the second argument from the set of input arguments. */
#define __NRFX_GET_ARG2(arg1, arg2, ...) arg2
#define _NRFX_GET_ARG2(...) __NRFX_GET_ARG2(__VA_ARGS__)

/* Macro for getting the first argument from the set of input arguments. */
#define __NRFX_GET_ARG1(arg1, ...) arg1
#define _NRFX_GET_ARG1(...) __NRFX_GET_ARG1(__VA_ARGS__)

/* Macro for triggering argument evaluation. */
#define _NRFX_EVAL(...) __VA_ARGS__

/* Macro used for a trick which detects if input argument is wrapped in parenthesis.
 *
 * Macro that has parenthesis will expand to additional comma (additional argument)
 * and that is used to return 0 or 1.
 */
#define _NRFX_ARG_HAS_PARENTHESIS(...) ,

#define _NRFX_ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15

/* Returns 1 if there is a comma in the arguments (if there is more than one argument) */
#define _NRFX_HAS_COMMA(...) \
    _NRFX_ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)

/* Internal macro used for @ref NRFX_IS_EMPTY */
#define _NRFX_IS_EMPTY2(_0, _1, _2, _3) \
    _NRFX_HAS_COMMA(NRFX_CONCAT(_NRFX_IS_EMPTY_CASE_, _0, _1, _2, _3))

#define _NRFX_IS_EMPTY_CASE_0001 ,

/* Internal macro used for @ref NRFX_IS_EMPTY */
#define _NRFX_IS_EMPTY(...)                                                                 \
    _NRFX_IS_EMPTY2(                                                                        \
    /* test if there is just one argument, eventually an empty one */                       \
    _NRFX_HAS_COMMA(__VA_ARGS__),                                                           \
    /* test if _TRIGGER_PARENTHESIS_ together with the argument adds a comma */             \
    _NRFX_HAS_COMMA(_NRFX_ARG_HAS_PARENTHESIS __VA_ARGS__),                                 \
    /* test if the argument together with a parenthesis adds a comma */                     \
    _NRFX_HAS_COMMA(__VA_ARGS__ (/*empty*/)),                                               \
    /* test if placing it between _TRIGGER_PARENTHESIS_ and the parenthesis adds a comma */ \
    _NRFX_HAS_COMMA(_NRFX_ARG_HAS_PARENTHESIS __VA_ARGS__ (/*empty*/))                      \
    )

/**
 * @brief Macro for generating else if statement code blocks that assignes token \<periph_name\>\<prefix\>\<i\>\<suffix\>
 *        to the variable \<var\> if \<p_reg\> points to the instance NRF_\<periph_name\>\<prefix\>\<i\>.
 *
 * @param[in] periph_name Peripheral name, e.g. SPIM.
 * @param[in] prefix      Prefix appended to the index.
 * @param[in] i           Index.
 * @param[in] var         Variable.
 * @param[in] suffix      Suffix following an instance name, e.g. _CH_NUM.
 * @param[in] p_reg       Specific peripheral instance register pointer.
 */
#define NRF_INTERNAL_ELSE_IF_EXTRACT_1(periph_name, prefix, i, var, suffix, p_reg) \
    else if (p_reg == NRFX_CONCAT(NRF_, periph_name, prefix, i))                   \
    {                                                                              \
        var = NRFX_CONCAT(periph_name, prefix, i, suffix);                         \
    }

/**
 * Macro used with @p NRFX_FOREACH_PRESENT for generating comma separated
 * \<periph_name\>\<prefix\>\<i\>_CH_NUM tokens.
 */
#define NRFX_INTERNAL_CHAN_NUM(periph_name, prefix, i, _) \
    NRFX_CONCAT(periph_name, prefix, i, _CH_NUM),

/**
 * Macro used with @p NRFX_FOREACH_PRESENT for generating comma separated
 * \<periph_name\>\<prefix\>\<i\>_GROUP_NUM tokens.
 */
#define NRFX_INTERNAL_GROUP_NUM(periph_name, prefix, i, suffix) \
    NRFX_CONCAT(periph_name, prefix, i, _GROUP_NUM),

/* Internal macros for @ref NRFX_FOR_EACH_IDX_FIXED_ARG */
#define _NRFX_FOR_EACH_IDX_FIXED_ARG_EXEC(idx, x, fixed_arg0, fixed_arg1) \
    fixed_arg0(idx, x, fixed_arg1)

#define _NRFX_FOR_EACH_IDX_FIXED_ARG(F, sep, fixed_arg, ...) \
    _NRFX_FOR_EACH_ENGINE(_NRFX_FOR_EACH_IDX_FIXED_ARG_EXEC, sep, \
                      F, fixed_arg, __VA_ARGS__)

/* Internal macros for @ref NRFX_FOR_EACH_FIXED_ARG */
#define _NRFX_FOR_EACH_FIXED_ARG_EXEC(idx, x, fixed_arg0, fixed_arg1) \
    fixed_arg0(x, fixed_arg1)

#define _NRFX_FOR_EACH_FIXED_ARG(F, sep, fixed_arg, ...) \
    _NRFX_FOR_EACH_ENGINE(_NRFX_FOR_EACH_FIXED_ARG_EXEC, sep, \
                      F, fixed_arg, __VA_ARGS__)

/* Internal macros for @ref NRFX_FOR_EACH_IDX */
#define _NRFX_FOR_EACH_IDX_EXEC(idx, x, fixed_arg0, fixed_arg1) \
    fixed_arg0(idx, x)

#define _NRFX_FOR_EACH_IDX(F, sep, ...) \
    _NRFX_FOR_EACH_ENGINE(_NRFX_FOR_EACH_IDX_EXEC, sep, F, _, __VA_ARGS__)

/* Internal macros for @ref NRFX_FOR_EACH */
#define _NRFX_FOR_EACH(F, sep, ...) \
    _NRFX_FOR_EACH_ENGINE(_NRFX_FOR_EACH_EXEC, sep, F, _, __VA_ARGS__)

#define _NRFX_FOR_EACH_EXEC(idx, x, fixed_arg0, fixed_arg1) \
    fixed_arg0(x)

#define _NRFX_FOR_EACH_ENGINE(x, sep, fixed_arg0, fixed_arg1, ...) \
    _NRFX_FOR_LOOP_GET_ARG(__VA_ARGS__, \
        _NRFX_FOR_LOOP_64, \
        _NRFX_FOR_LOOP_63, \
        _NRFX_FOR_LOOP_62, \
        _NRFX_FOR_LOOP_61, \
        _NRFX_FOR_LOOP_60, \
        _NRFX_FOR_LOOP_59, \
        _NRFX_FOR_LOOP_58, \
        _NRFX_FOR_LOOP_57, \
        _NRFX_FOR_LOOP_56, \
        _NRFX_FOR_LOOP_55, \
        _NRFX_FOR_LOOP_54, \
        _NRFX_FOR_LOOP_53, \
        _NRFX_FOR_LOOP_52, \
        _NRFX_FOR_LOOP_51, \
        _NRFX_FOR_LOOP_50, \
        _NRFX_FOR_LOOP_49, \
        _NRFX_FOR_LOOP_48, \
        _NRFX_FOR_LOOP_47, \
        _NRFX_FOR_LOOP_46, \
        _NRFX_FOR_LOOP_45, \
        _NRFX_FOR_LOOP_44, \
        _NRFX_FOR_LOOP_43, \
        _NRFX_FOR_LOOP_42, \
        _NRFX_FOR_LOOP_41, \
        _NRFX_FOR_LOOP_40, \
        _NRFX_FOR_LOOP_39, \
        _NRFX_FOR_LOOP_38, \
        _NRFX_FOR_LOOP_37, \
        _NRFX_FOR_LOOP_36, \
        _NRFX_FOR_LOOP_35, \
        _NRFX_FOR_LOOP_34, \
        _NRFX_FOR_LOOP_33, \
        _NRFX_FOR_LOOP_32, \
        _NRFX_FOR_LOOP_31, \
        _NRFX_FOR_LOOP_30, \
        _NRFX_FOR_LOOP_29, \
        _NRFX_FOR_LOOP_28, \
        _NRFX_FOR_LOOP_27, \
        _NRFX_FOR_LOOP_26, \
        _NRFX_FOR_LOOP_25, \
        _NRFX_FOR_LOOP_24, \
        _NRFX_FOR_LOOP_23, \
        _NRFX_FOR_LOOP_22, \
        _NRFX_FOR_LOOP_21, \
        _NRFX_FOR_LOOP_20, \
        _NRFX_FOR_LOOP_19, \
        _NRFX_FOR_LOOP_18, \
        _NRFX_FOR_LOOP_17, \
        _NRFX_FOR_LOOP_16, \
        _NRFX_FOR_LOOP_15, \
        _NRFX_FOR_LOOP_14, \
        _NRFX_FOR_LOOP_13, \
        _NRFX_FOR_LOOP_12, \
        _NRFX_FOR_LOOP_11, \
        _NRFX_FOR_LOOP_10, \
        _NRFX_FOR_LOOP_9, \
        _NRFX_FOR_LOOP_8, \
        _NRFX_FOR_LOOP_7, \
        _NRFX_FOR_LOOP_6, \
        _NRFX_FOR_LOOP_5, \
        _NRFX_FOR_LOOP_4, \
        _NRFX_FOR_LOOP_3, \
        _NRFX_FOR_LOOP_2, \
        _NRFX_FOR_LOOP_1, \
        _NRFX_FOR_LOOP_0)(x, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__)

/* Partial macros for @ref NRFX_CONCAT */
#define _NRFX_CONCAT_0(arg, ...) arg

#define _NRFX_CONCAT_1(arg, ...) NRFX_CONCAT_2(arg, _NRFX_CONCAT_0(__VA_ARGS__))

#define _NRFX_CONCAT_2(arg, ...) NRFX_CONCAT_2(arg, _NRFX_CONCAT_1(__VA_ARGS__))

#define _NRFX_CONCAT_3(arg, ...) NRFX_CONCAT_2(arg, _NRFX_CONCAT_2(__VA_ARGS__))

#define _NRFX_CONCAT_4(arg, ...) NRFX_CONCAT_2(arg, _NRFX_CONCAT_3(__VA_ARGS__))

#define _NRFX_CONCAT_5(arg, ...) NRFX_CONCAT_2(arg, _NRFX_CONCAT_4(__VA_ARGS__))

#define _NRFX_CONCAT_6(arg, ...) NRFX_CONCAT_2(arg, _NRFX_CONCAT_5(__VA_ARGS__))

#define _NRFX_CONCAT_7(arg, ...) NRFX_CONCAT_2(arg, _NRFX_CONCAT_6(__VA_ARGS__))

/* Set of UTIL_LISTIFY particles */
#define _NRFX_LISTIFY_0(F, sep, ...)

#define _NRFX_LISTIFY_1(F, sep, ...) \
    F(0, __VA_ARGS__)

#define _NRFX_LISTIFY_2(F, sep, ...) \
    _NRFX_LISTIFY_1(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(1, __VA_ARGS__)

#define _NRFX_LISTIFY_3(F, sep, ...) \
    _NRFX_LISTIFY_2(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(2, __VA_ARGS__)

#define _NRFX_LISTIFY_4(F, sep, ...) \
    _NRFX_LISTIFY_3(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(3, __VA_ARGS__)

#define _NRFX_LISTIFY_5(F, sep, ...) \
    _NRFX_LISTIFY_4(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(4, __VA_ARGS__)

#define _NRFX_LISTIFY_6(F, sep, ...) \
    _NRFX_LISTIFY_5(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(5, __VA_ARGS__)

#define _NRFX_LISTIFY_7(F, sep, ...) \
    _NRFX_LISTIFY_6(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(6, __VA_ARGS__)

#define _NRFX_LISTIFY_8(F, sep, ...) \
    _NRFX_LISTIFY_7(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(7, __VA_ARGS__)

#define _NRFX_LISTIFY_9(F, sep, ...) \
    _NRFX_LISTIFY_8(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(8, __VA_ARGS__)

#define _NRFX_LISTIFY_10(F, sep, ...) \
    _NRFX_LISTIFY_9(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(9, __VA_ARGS__)

#define _NRFX_LISTIFY_11(F, sep, ...) \
    _NRFX_LISTIFY_10(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(10, __VA_ARGS__)

#define _NRFX_LISTIFY_12(F, sep, ...) \
    _NRFX_LISTIFY_11(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(11, __VA_ARGS__)

#define _NRFX_LISTIFY_13(F, sep, ...) \
    _NRFX_LISTIFY_12(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(12, __VA_ARGS__)

#define _NRFX_LISTIFY_14(F, sep, ...) \
    _NRFX_LISTIFY_13(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(13, __VA_ARGS__)

#define _NRFX_LISTIFY_15(F, sep, ...) \
    _NRFX_LISTIFY_14(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(14, __VA_ARGS__)

#define _NRFX_LISTIFY_16(F, sep, ...) \
    _NRFX_LISTIFY_15(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(15, __VA_ARGS__)

#define _NRFX_LISTIFY_17(F, sep, ...) \
    _NRFX_LISTIFY_16(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(16, __VA_ARGS__)

#define _NRFX_LISTIFY_18(F, sep, ...) \
    _NRFX_LISTIFY_17(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(17, __VA_ARGS__)

#define _NRFX_LISTIFY_19(F, sep, ...) \
    _NRFX_LISTIFY_18(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(18, __VA_ARGS__)

#define _NRFX_LISTIFY_20(F, sep, ...) \
    _NRFX_LISTIFY_19(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(19, __VA_ARGS__)

#define _NRFX_LISTIFY_21(F, sep, ...) \
    _NRFX_LISTIFY_20(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(20, __VA_ARGS__)

#define _NRFX_LISTIFY_22(F, sep, ...) \
    _NRFX_LISTIFY_21(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(21, __VA_ARGS__)

#define _NRFX_LISTIFY_23(F, sep, ...) \
    _NRFX_LISTIFY_22(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(22, __VA_ARGS__)

#define _NRFX_LISTIFY_24(F, sep, ...) \
    _NRFX_LISTIFY_23(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(23, __VA_ARGS__)

#define _NRFX_LISTIFY_25(F, sep, ...) \
    _NRFX_LISTIFY_24(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(24, __VA_ARGS__)

#define _NRFX_LISTIFY_26(F, sep, ...) \
    _NRFX_LISTIFY_25(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(25, __VA_ARGS__)

#define _NRFX_LISTIFY_27(F, sep, ...) \
    _NRFX_LISTIFY_26(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(26, __VA_ARGS__)

#define _NRFX_LISTIFY_28(F, sep, ...) \
    _NRFX_LISTIFY_27(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(27, __VA_ARGS__)

#define _NRFX_LISTIFY_29(F, sep, ...) \
    _NRFX_LISTIFY_28(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(28, __VA_ARGS__)

#define _NRFX_LISTIFY_30(F, sep, ...) \
    _NRFX_LISTIFY_29(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(29, __VA_ARGS__)

#define _NRFX_LISTIFY_31(F, sep, ...) \
    _NRFX_LISTIFY_30(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(30, __VA_ARGS__)

#define _NRFX_LISTIFY_32(F, sep, ...) \
    _NRFX_LISTIFY_31(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(31, __VA_ARGS__)

#define _NRFX_LISTIFY_33(F, sep, ...) \
    _NRFX_LISTIFY_32(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(32, __VA_ARGS__)

#define _NRFX_LISTIFY_34(F, sep, ...) \
    _NRFX_LISTIFY_33(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(33, __VA_ARGS__)

#define _NRFX_LISTIFY_35(F, sep, ...) \
    _NRFX_LISTIFY_34(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(34, __VA_ARGS__)

#define _NRFX_LISTIFY_36(F, sep, ...) \
    _NRFX_LISTIFY_35(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(35, __VA_ARGS__)

#define _NRFX_LISTIFY_37(F, sep, ...) \
    _NRFX_LISTIFY_36(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(36, __VA_ARGS__)

#define _NRFX_LISTIFY_38(F, sep, ...) \
    _NRFX_LISTIFY_37(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(37, __VA_ARGS__)

#define _NRFX_LISTIFY_39(F, sep, ...) \
    _NRFX_LISTIFY_38(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(38, __VA_ARGS__)

#define _NRFX_LISTIFY_40(F, sep, ...) \
    _NRFX_LISTIFY_39(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(39, __VA_ARGS__)

#define _NRFX_LISTIFY_41(F, sep, ...) \
    _NRFX_LISTIFY_40(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(40, __VA_ARGS__)

#define _NRFX_LISTIFY_42(F, sep, ...) \
    _NRFX_LISTIFY_41(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(41, __VA_ARGS__)

#define _NRFX_LISTIFY_43(F, sep, ...) \
    _NRFX_LISTIFY_42(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(42, __VA_ARGS__)

#define _NRFX_LISTIFY_44(F, sep, ...) \
    _NRFX_LISTIFY_43(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(43, __VA_ARGS__)

#define _NRFX_LISTIFY_45(F, sep, ...) \
    _NRFX_LISTIFY_44(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(44, __VA_ARGS__)

#define _NRFX_LISTIFY_46(F, sep, ...) \
    _NRFX_LISTIFY_45(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(45, __VA_ARGS__)

#define _NRFX_LISTIFY_47(F, sep, ...) \
    _NRFX_LISTIFY_46(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(46, __VA_ARGS__)

#define _NRFX_LISTIFY_48(F, sep, ...) \
    _NRFX_LISTIFY_47(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(47, __VA_ARGS__)

#define _NRFX_LISTIFY_49(F, sep, ...) \
    _NRFX_LISTIFY_48(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(48, __VA_ARGS__)

#define _NRFX_LISTIFY_50(F, sep, ...) \
    _NRFX_LISTIFY_49(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(49, __VA_ARGS__)

#define _NRFX_LISTIFY_51(F, sep, ...) \
    _NRFX_LISTIFY_50(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(50, __VA_ARGS__)

#define _NRFX_LISTIFY_52(F, sep, ...) \
    _NRFX_LISTIFY_51(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(51, __VA_ARGS__)

#define _NRFX_LISTIFY_53(F, sep, ...) \
    _NRFX_LISTIFY_52(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(52, __VA_ARGS__)

#define _NRFX_LISTIFY_54(F, sep, ...) \
    _NRFX_LISTIFY_53(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(53, __VA_ARGS__)

#define _NRFX_LISTIFY_55(F, sep, ...) \
    _NRFX_LISTIFY_54(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(54, __VA_ARGS__)

#define _NRFX_LISTIFY_56(F, sep, ...) \
    _NRFX_LISTIFY_55(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(55, __VA_ARGS__)

#define _NRFX_LISTIFY_57(F, sep, ...) \
    _NRFX_LISTIFY_56(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(56, __VA_ARGS__)

#define _NRFX_LISTIFY_58(F, sep, ...) \
    _NRFX_LISTIFY_57(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(57, __VA_ARGS__)

#define _NRFX_LISTIFY_59(F, sep, ...) \
    _NRFX_LISTIFY_58(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(58, __VA_ARGS__)

#define _NRFX_LISTIFY_60(F, sep, ...) \
    _NRFX_LISTIFY_59(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(59, __VA_ARGS__)

#define _NRFX_LISTIFY_61(F, sep, ...) \
    _NRFX_LISTIFY_60(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(60, __VA_ARGS__)

#define _NRFX_LISTIFY_62(F, sep, ...) \
    _NRFX_LISTIFY_61(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(61, __VA_ARGS__)

#define _NRFX_LISTIFY_63(F, sep, ...) \
    _NRFX_LISTIFY_62(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(62, __VA_ARGS__)

#define _NRFX_LISTIFY_64(F, sep, ...) \
    _NRFX_LISTIFY_63(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(63, __VA_ARGS__)

#define _NRFX_LISTIFY_65(F, sep, ...) \
    _NRFX_LISTIFY_64(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(64, __VA_ARGS__)

#define _NRFX_LISTIFY_66(F, sep, ...) \
    _NRFX_LISTIFY_65(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(65, __VA_ARGS__)

#define _NRFX_LISTIFY_67(F, sep, ...) \
    _NRFX_LISTIFY_66(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(66, __VA_ARGS__)

#define _NRFX_LISTIFY_68(F, sep, ...) \
    _NRFX_LISTIFY_67(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(67, __VA_ARGS__)

#define _NRFX_LISTIFY_69(F, sep, ...) \
    _NRFX_LISTIFY_68(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(68, __VA_ARGS__)

#define _NRFX_LISTIFY_70(F, sep, ...) \
    _NRFX_LISTIFY_69(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(69, __VA_ARGS__)

#define _NRFX_LISTIFY_71(F, sep, ...) \
    _NRFX_LISTIFY_70(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(70, __VA_ARGS__)

#define _NRFX_LISTIFY_72(F, sep, ...) \
    _NRFX_LISTIFY_71(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(71, __VA_ARGS__)

#define _NRFX_LISTIFY_73(F, sep, ...) \
    _NRFX_LISTIFY_72(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(72, __VA_ARGS__)

#define _NRFX_LISTIFY_74(F, sep, ...) \
    _NRFX_LISTIFY_73(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(73, __VA_ARGS__)

#define _NRFX_LISTIFY_75(F, sep, ...) \
    _NRFX_LISTIFY_74(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(74, __VA_ARGS__)

#define _NRFX_LISTIFY_76(F, sep, ...) \
    _NRFX_LISTIFY_75(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(75, __VA_ARGS__)

#define _NRFX_LISTIFY_77(F, sep, ...) \
    _NRFX_LISTIFY_76(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(76, __VA_ARGS__)

#define _NRFX_LISTIFY_78(F, sep, ...) \
    _NRFX_LISTIFY_77(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(77, __VA_ARGS__)

#define _NRFX_LISTIFY_79(F, sep, ...) \
    _NRFX_LISTIFY_78(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(78, __VA_ARGS__)

#define _NRFX_LISTIFY_80(F, sep, ...) \
    _NRFX_LISTIFY_79(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(79, __VA_ARGS__)

#define _NRFX_LISTIFY_81(F, sep, ...) \
    _NRFX_LISTIFY_80(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(80, __VA_ARGS__)

#define _NRFX_LISTIFY_82(F, sep, ...) \
    _NRFX_LISTIFY_81(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(81, __VA_ARGS__)

#define _NRFX_LISTIFY_83(F, sep, ...) \
    _NRFX_LISTIFY_82(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(82, __VA_ARGS__)

#define _NRFX_LISTIFY_84(F, sep, ...) \
    _NRFX_LISTIFY_83(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(83, __VA_ARGS__)

#define _NRFX_LISTIFY_85(F, sep, ...) \
    _NRFX_LISTIFY_84(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(84, __VA_ARGS__)

#define _NRFX_LISTIFY_86(F, sep, ...) \
    _NRFX_LISTIFY_85(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(85, __VA_ARGS__)

#define _NRFX_LISTIFY_87(F, sep, ...) \
    _NRFX_LISTIFY_86(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(86, __VA_ARGS__)

#define _NRFX_LISTIFY_88(F, sep, ...) \
    _NRFX_LISTIFY_87(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(87, __VA_ARGS__)

#define _NRFX_LISTIFY_89(F, sep, ...) \
    _NRFX_LISTIFY_88(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(88, __VA_ARGS__)

#define _NRFX_LISTIFY_90(F, sep, ...) \
    _NRFX_LISTIFY_89(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(89, __VA_ARGS__)

#define _NRFX_LISTIFY_91(F, sep, ...) \
    _NRFX_LISTIFY_90(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(90, __VA_ARGS__)

#define _NRFX_LISTIFY_92(F, sep, ...) \
    _NRFX_LISTIFY_91(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(91, __VA_ARGS__)

#define _NRFX_LISTIFY_93(F, sep, ...) \
    _NRFX_LISTIFY_92(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(92, __VA_ARGS__)

#define _NRFX_LISTIFY_94(F, sep, ...) \
    _NRFX_LISTIFY_93(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(93, __VA_ARGS__)

#define _NRFX_LISTIFY_95(F, sep, ...) \
    _NRFX_LISTIFY_94(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(94, __VA_ARGS__)

#define _NRFX_LISTIFY_96(F, sep, ...) \
    _NRFX_LISTIFY_95(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(95, __VA_ARGS__)

#define _NRFX_LISTIFY_97(F, sep, ...) \
    _NRFX_LISTIFY_96(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(96, __VA_ARGS__)

#define _NRFX_LISTIFY_98(F, sep, ...) \
    _NRFX_LISTIFY_97(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(97, __VA_ARGS__)

#define _NRFX_LISTIFY_99(F, sep, ...) \
    _NRFX_LISTIFY_98(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(98, __VA_ARGS__)

#define _NRFX_LISTIFY_100(F, sep, ...) \
    _NRFX_LISTIFY_99(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(99, __VA_ARGS__)

#define _NRFX_LISTIFY_101(F, sep, ...) \
    _NRFX_LISTIFY_100(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(100, __VA_ARGS__)

#define _NRFX_LISTIFY_102(F, sep, ...) \
    _NRFX_LISTIFY_101(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(101, __VA_ARGS__)

#define _NRFX_LISTIFY_103(F, sep, ...) \
    _NRFX_LISTIFY_102(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(102, __VA_ARGS__)

#define _NRFX_LISTIFY_104(F, sep, ...) \
    _NRFX_LISTIFY_103(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(103, __VA_ARGS__)

#define _NRFX_LISTIFY_105(F, sep, ...) \
    _NRFX_LISTIFY_104(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(104, __VA_ARGS__)

#define _NRFX_LISTIFY_106(F, sep, ...) \
    _NRFX_LISTIFY_105(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(105, __VA_ARGS__)

#define _NRFX_LISTIFY_107(F, sep, ...) \
    _NRFX_LISTIFY_106(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(106, __VA_ARGS__)

#define _NRFX_LISTIFY_108(F, sep, ...) \
    _NRFX_LISTIFY_107(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(107, __VA_ARGS__)

#define _NRFX_LISTIFY_109(F, sep, ...) \
    _NRFX_LISTIFY_108(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(108, __VA_ARGS__)

#define _NRFX_LISTIFY_110(F, sep, ...) \
    _NRFX_LISTIFY_109(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(109, __VA_ARGS__)

#define _NRFX_LISTIFY_111(F, sep, ...) \
    _NRFX_LISTIFY_110(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(110, __VA_ARGS__)

#define _NRFX_LISTIFY_112(F, sep, ...) \
    _NRFX_LISTIFY_111(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(111, __VA_ARGS__)

#define _NRFX_LISTIFY_113(F, sep, ...) \
    _NRFX_LISTIFY_112(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(112, __VA_ARGS__)

#define _NRFX_LISTIFY_114(F, sep, ...) \
    _NRFX_LISTIFY_113(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(113, __VA_ARGS__)

#define _NRFX_LISTIFY_115(F, sep, ...) \
    _NRFX_LISTIFY_114(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(114, __VA_ARGS__)

#define _NRFX_LISTIFY_116(F, sep, ...) \
    _NRFX_LISTIFY_115(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(115, __VA_ARGS__)

#define _NRFX_LISTIFY_117(F, sep, ...) \
    _NRFX_LISTIFY_116(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(116, __VA_ARGS__)

#define _NRFX_LISTIFY_118(F, sep, ...) \
    _NRFX_LISTIFY_117(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(117, __VA_ARGS__)

#define _NRFX_LISTIFY_119(F, sep, ...) \
    _NRFX_LISTIFY_118(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(118, __VA_ARGS__)

#define _NRFX_LISTIFY_120(F, sep, ...) \
    _NRFX_LISTIFY_119(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(119, __VA_ARGS__)

#define _NRFX_LISTIFY_121(F, sep, ...) \
    _NRFX_LISTIFY_120(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(120, __VA_ARGS__)

#define _NRFX_LISTIFY_122(F, sep, ...) \
    _NRFX_LISTIFY_121(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(121, __VA_ARGS__)

#define _NRFX_LISTIFY_123(F, sep, ...) \
    _NRFX_LISTIFY_122(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(122, __VA_ARGS__)

#define _NRFX_LISTIFY_124(F, sep, ...) \
    _NRFX_LISTIFY_123(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(123, __VA_ARGS__)

#define _NRFX_LISTIFY_125(F, sep, ...) \
    _NRFX_LISTIFY_124(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(124, __VA_ARGS__)

#define _NRFX_LISTIFY_126(F, sep, ...) \
    _NRFX_LISTIFY_125(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(125, __VA_ARGS__)

#define _NRFX_LISTIFY_127(F, sep, ...) \
    _NRFX_LISTIFY_126(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(126, __VA_ARGS__)

#define _NRFX_LISTIFY_128(F, sep, ...) \
    _NRFX_LISTIFY_127(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(127, __VA_ARGS__)

#define _NRFX_LISTIFY_129(F, sep, ...) \
    _NRFX_LISTIFY_128(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(128, __VA_ARGS__)

#define _NRFX_LISTIFY_130(F, sep, ...) \
    _NRFX_LISTIFY_129(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(129, __VA_ARGS__)

#define _NRFX_LISTIFY_131(F, sep, ...) \
    _NRFX_LISTIFY_130(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(130, __VA_ARGS__)

#define _NRFX_LISTIFY_132(F, sep, ...) \
    _NRFX_LISTIFY_131(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(131, __VA_ARGS__)

#define _NRFX_LISTIFY_133(F, sep, ...) \
    _NRFX_LISTIFY_132(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(132, __VA_ARGS__)

#define _NRFX_LISTIFY_134(F, sep, ...) \
    _NRFX_LISTIFY_133(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(133, __VA_ARGS__)

#define _NRFX_LISTIFY_135(F, sep, ...) \
    _NRFX_LISTIFY_134(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(134, __VA_ARGS__)

#define _NRFX_LISTIFY_136(F, sep, ...) \
    _NRFX_LISTIFY_135(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(135, __VA_ARGS__)

#define _NRFX_LISTIFY_137(F, sep, ...) \
    _NRFX_LISTIFY_136(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(136, __VA_ARGS__)

#define _NRFX_LISTIFY_138(F, sep, ...) \
    _NRFX_LISTIFY_137(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(137, __VA_ARGS__)

#define _NRFX_LISTIFY_139(F, sep, ...) \
    _NRFX_LISTIFY_138(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(138, __VA_ARGS__)

#define _NRFX_LISTIFY_140(F, sep, ...) \
    _NRFX_LISTIFY_139(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(139, __VA_ARGS__)

#define _NRFX_LISTIFY_141(F, sep, ...) \
    _NRFX_LISTIFY_140(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(140, __VA_ARGS__)

#define _NRFX_LISTIFY_142(F, sep, ...) \
    _NRFX_LISTIFY_141(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(141, __VA_ARGS__)

#define _NRFX_LISTIFY_143(F, sep, ...) \
    _NRFX_LISTIFY_142(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(142, __VA_ARGS__)

#define _NRFX_LISTIFY_144(F, sep, ...) \
    _NRFX_LISTIFY_143(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(143, __VA_ARGS__)

#define _NRFX_LISTIFY_145(F, sep, ...) \
    _NRFX_LISTIFY_144(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(144, __VA_ARGS__)

#define _NRFX_LISTIFY_146(F, sep, ...) \
    _NRFX_LISTIFY_145(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(145, __VA_ARGS__)

#define _NRFX_LISTIFY_147(F, sep, ...) \
    _NRFX_LISTIFY_146(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(146, __VA_ARGS__)

#define _NRFX_LISTIFY_148(F, sep, ...) \
    _NRFX_LISTIFY_147(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(147, __VA_ARGS__)

#define _NRFX_LISTIFY_149(F, sep, ...) \
    _NRFX_LISTIFY_148(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(148, __VA_ARGS__)

#define _NRFX_LISTIFY_150(F, sep, ...) \
    _NRFX_LISTIFY_149(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(149, __VA_ARGS__)

#define _NRFX_LISTIFY_151(F, sep, ...) \
    _NRFX_LISTIFY_150(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(150, __VA_ARGS__)

#define _NRFX_LISTIFY_152(F, sep, ...) \
    _NRFX_LISTIFY_151(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(151, __VA_ARGS__)

#define _NRFX_LISTIFY_153(F, sep, ...) \
    _NRFX_LISTIFY_152(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(152, __VA_ARGS__)

#define _NRFX_LISTIFY_154(F, sep, ...) \
    _NRFX_LISTIFY_153(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(153, __VA_ARGS__)

#define _NRFX_LISTIFY_155(F, sep, ...) \
    _NRFX_LISTIFY_154(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(154, __VA_ARGS__)

#define _NRFX_LISTIFY_156(F, sep, ...) \
    _NRFX_LISTIFY_155(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(155, __VA_ARGS__)

#define _NRFX_LISTIFY_157(F, sep, ...) \
    _NRFX_LISTIFY_156(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(156, __VA_ARGS__)

#define _NRFX_LISTIFY_158(F, sep, ...) \
    _NRFX_LISTIFY_157(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(157, __VA_ARGS__)

#define _NRFX_LISTIFY_159(F, sep, ...) \
    _NRFX_LISTIFY_158(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(158, __VA_ARGS__)

#define _NRFX_LISTIFY_160(F, sep, ...) \
    _NRFX_LISTIFY_159(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(159, __VA_ARGS__)

#define _NRFX_LISTIFY_161(F, sep, ...) \
    _NRFX_LISTIFY_160(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(160, __VA_ARGS__)

#define _NRFX_LISTIFY_162(F, sep, ...) \
    _NRFX_LISTIFY_161(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(161, __VA_ARGS__)

#define _NRFX_LISTIFY_163(F, sep, ...) \
    _NRFX_LISTIFY_162(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(162, __VA_ARGS__)

#define _NRFX_LISTIFY_164(F, sep, ...) \
    _NRFX_LISTIFY_163(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(163, __VA_ARGS__)

#define _NRFX_LISTIFY_165(F, sep, ...) \
    _NRFX_LISTIFY_164(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(164, __VA_ARGS__)

#define _NRFX_LISTIFY_166(F, sep, ...) \
    _NRFX_LISTIFY_165(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(165, __VA_ARGS__)

#define _NRFX_LISTIFY_167(F, sep, ...) \
    _NRFX_LISTIFY_166(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(166, __VA_ARGS__)

#define _NRFX_LISTIFY_168(F, sep, ...) \
    _NRFX_LISTIFY_167(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(167, __VA_ARGS__)

#define _NRFX_LISTIFY_169(F, sep, ...) \
    _NRFX_LISTIFY_168(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(168, __VA_ARGS__)

#define _NRFX_LISTIFY_170(F, sep, ...) \
    _NRFX_LISTIFY_169(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(169, __VA_ARGS__)

#define _NRFX_LISTIFY_171(F, sep, ...) \
    _NRFX_LISTIFY_170(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(170, __VA_ARGS__)

#define _NRFX_LISTIFY_172(F, sep, ...) \
    _NRFX_LISTIFY_171(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(171, __VA_ARGS__)

#define _NRFX_LISTIFY_173(F, sep, ...) \
    _NRFX_LISTIFY_172(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(172, __VA_ARGS__)

#define _NRFX_LISTIFY_174(F, sep, ...) \
    _NRFX_LISTIFY_173(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(173, __VA_ARGS__)

#define _NRFX_LISTIFY_175(F, sep, ...) \
    _NRFX_LISTIFY_174(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(174, __VA_ARGS__)

#define _NRFX_LISTIFY_176(F, sep, ...) \
    _NRFX_LISTIFY_175(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(175, __VA_ARGS__)

#define _NRFX_LISTIFY_177(F, sep, ...) \
    _NRFX_LISTIFY_176(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(176, __VA_ARGS__)

#define _NRFX_LISTIFY_178(F, sep, ...) \
    _NRFX_LISTIFY_177(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(177, __VA_ARGS__)

#define _NRFX_LISTIFY_179(F, sep, ...) \
    _NRFX_LISTIFY_178(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(178, __VA_ARGS__)

#define _NRFX_LISTIFY_180(F, sep, ...) \
    _NRFX_LISTIFY_179(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(179, __VA_ARGS__)

#define _NRFX_LISTIFY_181(F, sep, ...) \
    _NRFX_LISTIFY_180(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(180, __VA_ARGS__)

#define _NRFX_LISTIFY_182(F, sep, ...) \
    _NRFX_LISTIFY_181(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(181, __VA_ARGS__)

#define _NRFX_LISTIFY_183(F, sep, ...) \
    _NRFX_LISTIFY_182(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(182, __VA_ARGS__)

#define _NRFX_LISTIFY_184(F, sep, ...) \
    _NRFX_LISTIFY_183(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(183, __VA_ARGS__)

#define _NRFX_LISTIFY_185(F, sep, ...) \
    _NRFX_LISTIFY_184(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(184, __VA_ARGS__)

#define _NRFX_LISTIFY_186(F, sep, ...) \
    _NRFX_LISTIFY_185(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(185, __VA_ARGS__)

#define _NRFX_LISTIFY_187(F, sep, ...) \
    _NRFX_LISTIFY_186(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(186, __VA_ARGS__)

#define _NRFX_LISTIFY_188(F, sep, ...) \
    _NRFX_LISTIFY_187(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(187, __VA_ARGS__)

#define _NRFX_LISTIFY_189(F, sep, ...) \
    _NRFX_LISTIFY_188(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(188, __VA_ARGS__)

#define _NRFX_LISTIFY_190(F, sep, ...) \
    _NRFX_LISTIFY_189(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(189, __VA_ARGS__)

#define _NRFX_LISTIFY_191(F, sep, ...) \
    _NRFX_LISTIFY_190(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(190, __VA_ARGS__)

#define _NRFX_LISTIFY_192(F, sep, ...) \
    _NRFX_LISTIFY_191(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(191, __VA_ARGS__)

#define _NRFX_LISTIFY_193(F, sep, ...) \
    _NRFX_LISTIFY_192(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(192, __VA_ARGS__)

#define _NRFX_LISTIFY_194(F, sep, ...) \
    _NRFX_LISTIFY_193(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(193, __VA_ARGS__)

#define _NRFX_LISTIFY_195(F, sep, ...) \
    _NRFX_LISTIFY_194(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(194, __VA_ARGS__)

#define _NRFX_LISTIFY_196(F, sep, ...) \
    _NRFX_LISTIFY_195(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(195, __VA_ARGS__)

#define _NRFX_LISTIFY_197(F, sep, ...) \
    _NRFX_LISTIFY_196(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(196, __VA_ARGS__)

#define _NRFX_LISTIFY_198(F, sep, ...) \
    _NRFX_LISTIFY_197(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(197, __VA_ARGS__)

#define _NRFX_LISTIFY_199(F, sep, ...) \
    _NRFX_LISTIFY_198(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(198, __VA_ARGS__)

#define _NRFX_LISTIFY_200(F, sep, ...) \
    _NRFX_LISTIFY_199(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(199, __VA_ARGS__)

#define _NRFX_LISTIFY_201(F, sep, ...) \
    _NRFX_LISTIFY_200(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(200, __VA_ARGS__)

#define _NRFX_LISTIFY_202(F, sep, ...) \
    _NRFX_LISTIFY_201(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(201, __VA_ARGS__)

#define _NRFX_LISTIFY_203(F, sep, ...) \
    _NRFX_LISTIFY_202(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(202, __VA_ARGS__)

#define _NRFX_LISTIFY_204(F, sep, ...) \
    _NRFX_LISTIFY_203(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(203, __VA_ARGS__)

#define _NRFX_LISTIFY_205(F, sep, ...) \
    _NRFX_LISTIFY_204(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(204, __VA_ARGS__)

#define _NRFX_LISTIFY_206(F, sep, ...) \
    _NRFX_LISTIFY_205(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(205, __VA_ARGS__)

#define _NRFX_LISTIFY_207(F, sep, ...) \
    _NRFX_LISTIFY_206(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(206, __VA_ARGS__)

#define _NRFX_LISTIFY_208(F, sep, ...) \
    _NRFX_LISTIFY_207(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(207, __VA_ARGS__)

#define _NRFX_LISTIFY_209(F, sep, ...) \
    _NRFX_LISTIFY_208(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(208, __VA_ARGS__)

#define _NRFX_LISTIFY_210(F, sep, ...) \
    _NRFX_LISTIFY_209(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(209, __VA_ARGS__)

#define _NRFX_LISTIFY_211(F, sep, ...) \
    _NRFX_LISTIFY_210(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(210, __VA_ARGS__)

#define _NRFX_LISTIFY_212(F, sep, ...) \
    _NRFX_LISTIFY_211(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(211, __VA_ARGS__)

#define _NRFX_LISTIFY_213(F, sep, ...) \
    _NRFX_LISTIFY_212(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(212, __VA_ARGS__)

#define _NRFX_LISTIFY_214(F, sep, ...) \
    _NRFX_LISTIFY_213(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(213, __VA_ARGS__)

#define _NRFX_LISTIFY_215(F, sep, ...) \
    _NRFX_LISTIFY_214(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(214, __VA_ARGS__)

#define _NRFX_LISTIFY_216(F, sep, ...) \
    _NRFX_LISTIFY_215(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(215, __VA_ARGS__)

#define _NRFX_LISTIFY_217(F, sep, ...) \
    _NRFX_LISTIFY_216(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(216, __VA_ARGS__)

#define _NRFX_LISTIFY_218(F, sep, ...) \
    _NRFX_LISTIFY_217(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(217, __VA_ARGS__)

#define _NRFX_LISTIFY_219(F, sep, ...) \
    _NRFX_LISTIFY_218(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(218, __VA_ARGS__)

#define _NRFX_LISTIFY_220(F, sep, ...) \
    _NRFX_LISTIFY_219(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(219, __VA_ARGS__)

#define _NRFX_LISTIFY_221(F, sep, ...) \
    _NRFX_LISTIFY_220(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(220, __VA_ARGS__)

#define _NRFX_LISTIFY_222(F, sep, ...) \
    _NRFX_LISTIFY_221(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(221, __VA_ARGS__)

#define _NRFX_LISTIFY_223(F, sep, ...) \
    _NRFX_LISTIFY_222(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(222, __VA_ARGS__)

#define _NRFX_LISTIFY_224(F, sep, ...) \
    _NRFX_LISTIFY_223(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(223, __VA_ARGS__)

#define _NRFX_LISTIFY_225(F, sep, ...) \
    _NRFX_LISTIFY_224(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(224, __VA_ARGS__)

#define _NRFX_LISTIFY_226(F, sep, ...) \
    _NRFX_LISTIFY_225(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(225, __VA_ARGS__)

#define _NRFX_LISTIFY_227(F, sep, ...) \
    _NRFX_LISTIFY_226(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(226, __VA_ARGS__)

#define _NRFX_LISTIFY_228(F, sep, ...) \
    _NRFX_LISTIFY_227(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(227, __VA_ARGS__)

#define _NRFX_LISTIFY_229(F, sep, ...) \
    _NRFX_LISTIFY_228(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(228, __VA_ARGS__)

#define _NRFX_LISTIFY_230(F, sep, ...) \
    _NRFX_LISTIFY_229(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(229, __VA_ARGS__)

#define _NRFX_LISTIFY_231(F, sep, ...) \
    _NRFX_LISTIFY_230(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(230, __VA_ARGS__)

#define _NRFX_LISTIFY_232(F, sep, ...) \
    _NRFX_LISTIFY_231(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(231, __VA_ARGS__)

#define _NRFX_LISTIFY_233(F, sep, ...) \
    _NRFX_LISTIFY_232(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(232, __VA_ARGS__)

#define _NRFX_LISTIFY_234(F, sep, ...) \
    _NRFX_LISTIFY_233(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(233, __VA_ARGS__)

#define _NRFX_LISTIFY_235(F, sep, ...) \
    _NRFX_LISTIFY_234(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(234, __VA_ARGS__)

#define _NRFX_LISTIFY_236(F, sep, ...) \
    _NRFX_LISTIFY_235(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(235, __VA_ARGS__)

#define _NRFX_LISTIFY_237(F, sep, ...) \
    _NRFX_LISTIFY_236(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(236, __VA_ARGS__)

#define _NRFX_LISTIFY_238(F, sep, ...) \
    _NRFX_LISTIFY_237(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(237, __VA_ARGS__)

#define _NRFX_LISTIFY_239(F, sep, ...) \
    _NRFX_LISTIFY_238(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(238, __VA_ARGS__)

#define _NRFX_LISTIFY_240(F, sep, ...) \
    _NRFX_LISTIFY_239(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(239, __VA_ARGS__)

#define _NRFX_LISTIFY_241(F, sep, ...) \
    _NRFX_LISTIFY_240(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(240, __VA_ARGS__)

#define _NRFX_LISTIFY_242(F, sep, ...) \
    _NRFX_LISTIFY_241(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(241, __VA_ARGS__)

#define _NRFX_LISTIFY_243(F, sep, ...) \
    _NRFX_LISTIFY_242(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(242, __VA_ARGS__)

#define _NRFX_LISTIFY_244(F, sep, ...) \
    _NRFX_LISTIFY_243(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(243, __VA_ARGS__)

#define _NRFX_LISTIFY_245(F, sep, ...) \
    _NRFX_LISTIFY_244(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(244, __VA_ARGS__)

#define _NRFX_LISTIFY_246(F, sep, ...) \
    _NRFX_LISTIFY_245(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(245, __VA_ARGS__)

#define _NRFX_LISTIFY_247(F, sep, ...) \
    _NRFX_LISTIFY_246(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(246, __VA_ARGS__)

#define _NRFX_LISTIFY_248(F, sep, ...) \
    _NRFX_LISTIFY_247(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(247, __VA_ARGS__)

#define _NRFX_LISTIFY_249(F, sep, ...) \
    _NRFX_LISTIFY_248(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(248, __VA_ARGS__)

#define _NRFX_LISTIFY_250(F, sep, ...) \
    _NRFX_LISTIFY_249(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(249, __VA_ARGS__)

#define _NRFX_LISTIFY_251(F, sep, ...) \
    _NRFX_LISTIFY_250(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(250, __VA_ARGS__)

#define _NRFX_LISTIFY_252(F, sep, ...) \
    _NRFX_LISTIFY_251(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(251, __VA_ARGS__)

#define _NRFX_LISTIFY_253(F, sep, ...) \
    _NRFX_LISTIFY_252(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(252, __VA_ARGS__)

#define _NRFX_LISTIFY_254(F, sep, ...) \
    _NRFX_LISTIFY_253(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(253, __VA_ARGS__)

#define _NRFX_LISTIFY_255(F, sep, ...) \
    _NRFX_LISTIFY_254(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(254, __VA_ARGS__)

#define _NRFX_LISTIFY_256(F, sep, ...) \
    _NRFX_LISTIFY_255(F, sep, __VA_ARGS__) NRFX_DEBRACKET sep \
    F(255, __VA_ARGS__)

#define _NRFX_FOR_LOOP_GET_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, \
                _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                _21, _22, _23, _24, _25, _26, _27, _28, _29, \
                _30, _31, _32, _33, _34, _35, _36, _37, _38, \
                _39, _40, _41, _42, _43, _44, _45, _46, _47, \
                _48, _49, _50, _51, _52, _53, _54, _55, _56, \
                _57, _58, _59, _60, _61, _62, _63, _64, N, ...) N

#define _NRFX_FOR_LOOP_0(call, sep, fixed_arg0, fixed_arg1, ...)

#define _NRFX_FOR_LOOP_1(call, sep, fixed_arg0, fixed_arg1, x) \
    call(0, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_2(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_1(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(1, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_3(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_2(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(2, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_4(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_3(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(3, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_5(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_4(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(4, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_6(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_5(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(5, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_7(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_6(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(6, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_8(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_7(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(7, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_9(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_8(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(8, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_10(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_9(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(9, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_11(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_10(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(10, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_12(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_11(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(11, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_13(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_12(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(12, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_14(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_13(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(13, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_15(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_14(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(14, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_16(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_15(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(15, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_17(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_16(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(16, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_18(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_17(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(17, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_19(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_18(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(18, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_20(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_19(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(19, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_21(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_20(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(20, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_22(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_21(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(21, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_23(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_22(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(22, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_24(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_23(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(23, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_25(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_24(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(24, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_26(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_25(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(25, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_27(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_26(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(26, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_28(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_27(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(27, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_29(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_28(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(28, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_30(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_29(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(29, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_31(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_30(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(30, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_32(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_31(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(31, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_33(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_32(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(32, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_34(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_33(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(33, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_35(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_34(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(34, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_36(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_35(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(35, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_37(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_36(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(36, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_38(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_37(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(37, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_39(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_38(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(38, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_40(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_39(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(39, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_41(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_40(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(40, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_42(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_41(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(41, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_43(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_42(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(42, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_44(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_43(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(43, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_45(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_44(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(44, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_46(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_45(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(45, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_47(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_46(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(46, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_48(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_47(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(47, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_49(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_48(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(48, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_50(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_49(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(49, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_51(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_50(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(50, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_52(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_51(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(51, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_53(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_52(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(52, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_54(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_53(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(53, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_55(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_54(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(54, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_56(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_55(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(55, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_57(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_56(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(56, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_58(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_57(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(57, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_59(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_58(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(58, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_60(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_59(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(59, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_61(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_60(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(60, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_62(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_61(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(61, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_63(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_62(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(62, x, fixed_arg0, fixed_arg1)

#define _NRFX_FOR_LOOP_64(call, sep, fixed_arg0, fixed_arg1, x, ...) \
    _NRFX_FOR_LOOP_63(call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
    NRFX_DEBRACKET sep \
    call(63, x, fixed_arg0, fixed_arg1)

// for lowercasing the 54L and 54H series names
#define NRF_SERIES_LOWERCASE_52         52
#define NRF_SERIES_LOWERCASE_53         53
#define NRF_SERIES_LOWERCASE_54L        54l
#define NRF_SERIES_LOWERCASE_54H        54h
#define NRF_SERIES_LOWERCASE_71         71
#define NRF_SERIES_LOWERCASE_91         91
#define NRF_SERIES_LOWERCASE_92         92
#define NRF_SERIES_LOWERCASE(series)    NRFX_CONCAT_2(NRF_SERIES_LOWERCASE_, series)

#endif /* NRFX_UTILS_INTERNAL_H__ */
