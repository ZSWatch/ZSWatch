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

#ifndef NRFX_UTILS_H__
#define NRFX_UTILS_H__

#include "nrfx_utils_internal.h"
/**
 * @defgroup nrfx_utils Preprocessor utility macros
 * @{
 * @ingroup nrfx
 * @brief Preprocessor utility macros.
 */

/**
 * @brief Macro for checking if the specified identifier is defined and it has
 *        a non-zero value.
 *
 * Normally, preprocessors treat all undefined identifiers as having the value
 * zero. However, some tools, like static code analyzers, can issue a warning
 * when such identifier is evaluated. This macro gives the possibility to suppress
 * such warnings only in places where this macro is used for evaluation, not in
 * the whole analyzed code.
 */
#define NRFX_CHECK(module_enabled) NRFX_IS_ENABLED(module_enabled)

/**
 * @brief Macro for checking if the nrfx version is greater than or equal
 *        to the specified version.
 *
 * @note Current nrfx version is specified with the following symbols:
 *       - @ref NRFX_RELEASE_VER_MAJOR
 *       - @ref NRFX_RELEASE_VER_MINOR
 *       - @ref NRFX_RELEASE_VER_MICRO
 *
 * @param[in] major Major version.
 * @param[in] minor Minor version.
 * @param[in] micro Micro version.
 *
 * @retval true  Current nrfx version is greater than or equal to the specified version.
 * @retval false Current nrfx version is smaller than the specified version.
 */
#define NRFX_RELEASE_VER_AT_LEAST(major, minor, micro) \
    (((NRFX_RELEASE_VER_MAJOR > (major))) ||                                      \
     ((NRFX_RELEASE_VER_MAJOR == major) && (NRFX_RELEASE_VER_MINOR > (minor))) || \
     ((NRFX_RELEASE_VER_MAJOR == major) && (NRFX_RELEASE_VER_MINOR == minor) &&   \
      (NRFX_RELEASE_VER_MICRO >= (micro))))

/**
 * @brief Macro for checking if the configured API version is greater than or equal
 *        to the specified API version.
 *
 * @note API version to be used is configured using following symbols:
 *       - @ref NRFX_CONFIG_API_VER_MAJOR
 *       - @ref NRFX_CONFIG_API_VER_MINOR
 *       - @ref NRFX_CONFIG_API_VER_MICRO
 *
 * @param[in] major Major API version.
 * @param[in] minor Minor API version.
 * @param[in] micro Micro API version.
 *
 * @retval true  Configured API version is greater than or equal to the specified API version.
 * @retval false Configured API version is smaller than the specified API version.
 */
#define NRFX_API_VER_AT_LEAST(major, minor, micro) \
    (((NRFX_CONFIG_API_VER_MAJOR > (major))) ||                                         \
     ((NRFX_CONFIG_API_VER_MAJOR == major) && (NRFX_CONFIG_API_VER_MINOR > (minor))) || \
     ((NRFX_CONFIG_API_VER_MAJOR == major) && (NRFX_CONFIG_API_VER_MINOR == minor) &&   \
      (NRFX_CONFIG_API_VER_MICRO >= (micro))))

/**
 * @brief Macro for creating unsigned integer with bit position @p x set.
 *
 * @param[in] x Bit position to be set.
 *
 * @return Unsigned integer with requested bit position set.
 */
#define NRFX_BIT(x) (1UL << (x))

/**
 * @brief Macro for returning bit mask or 0 if @p x is 0.
 *
 * @param[in] x Bit mask size. Bit mask has bits 0 through x-1 (inclusive) set.
 *
 * @return Bit mask.
 */
#define NRFX_BIT_MASK(x) (((x) == 32) ? UINT32_MAX : ((1UL << (x)) - 1))

/**
 * @brief Macro for returning size in bits for given size in bytes.
 *
 * @param[in] x Size in bytes.
 *
 * @return Size in bits.
 */
#define NRFX_BIT_SIZE(x) ((x) << 3)

/**
 * @brief Macro for concatenating two tokens in macro expansion.
 *
 * @note This macro is expanded in two steps so that tokens given as macros
 *       themselves are fully expanded before they are merged.
 *
 * @param[in] p1 First token.
 * @param[in] p2 Second token.
 *
 * @return The two tokens merged into one, unless they cannot together form
 *         a valid token (in such case, the preprocessor issues a warning and
 *         does not perform the concatenation).
 *
 * @sa NRFX_CONCAT_3
 */
#define NRFX_CONCAT_2(p1, p2) NRFX_CONCAT_2_(p1, p2)

/** @brief Internal macro used by @ref NRFX_CONCAT_2 to perform the expansion in two steps. */
#define NRFX_CONCAT_2_(p1, p2) p1 ## p2

/**
 * @brief Macro for concatenating three tokens in macro expansion.
 *
 * @note This macro is expanded in two steps so that tokens given as macros
 *       themselves are fully expanded before they are merged.
 *
 * @param[in] p1 First token.
 * @param[in] p2 Second token.
 * @param[in] p3 Third token.
 *
 * @return The three tokens merged into one, unless they cannot together form
 *         a valid token (in such case, the preprocessor issues a warning and
 *         does not perform the concatenation).
 *
 * @sa NRFX_CONCAT_2
 */
#define NRFX_CONCAT_3(p1, p2, p3) NRFX_CONCAT_3_(p1, p2, p3)

/** @brief Internal macro used by @ref NRFX_CONCAT_3 to perform the expansion in two steps. */
#define NRFX_CONCAT_3_(p1, p2, p3) p1 ## p2 ## p3

/**
 * @brief Macro for computing the absolute value of an integer number.
 *
 * @param[in] a Input value.
 *
 * @return Absolute value.
 */
#define NRFX_ABS(a) ((a) < (0) ? -(a) : (a))

/**
 * @brief Macro for computing the difference between two unsigned values.
 *
 * @param[in] a First input value.
 * @param[in] b Second input value.
 *
 * @return Difference.
 */
#define NRFX_DIFF(a, b) ((a) < (b) ? ((b) - (a)) : ((a) - (b)))

/**
 * @brief Macro for checking whether any of the instance of the specified peripheral supports a given feature.
 *
 * Macro checks flags set in \<device\>_peripherals.h file.
 *
 * Macro supports check on instances with following names:
 * - \<periph_name\>0 - \<periph_name\>255 - e.g. SPIM0, SPIM255
 * - \<periph_name\>00 - \<periph_name\>099 - e.g. SPIM00, SPIM099
 * - \<periph_name\>000 - \<periph_name\>009 - e.g. SPIM000, SPIM009
 *
 * @param[in] periph_name  Peripheral name, e.g. SPIM.
 * @param[in] feature_name Feature flag name suffix following an instance name, e.g.
 *                         _FEATURE_HARDWARE_CSN_PRESENT.
 *
 * @retval 1 At least one instance on current device supports a given feature.
 * @retval 0 None of peripheral instances supports a given feature.
 */
#define NRFX_FEATURE_PRESENT(periph_name, feature_name)                                            \
        NRFX_COND_CODE_0(NRFX_CONCAT(0,                                                            \
                            _NRFX_FEATURE_PRESENT(periph_name, feature_name, 256),                 \
                            _NRFX_FEATURE_PRESENT(NRFX_CONCAT(periph_name, 0), feature_name, 100), \
                            _NRFX_FEATURE_PRESENT(NRFX_CONCAT(periph_name, 00), feature_name, 10)  \
                         ),                                                                        \
                        (0), (1))

/**
 * @brief Macro for resolving provided user macro for enabled instances of a driver.
 *
 * Macro checks if driver instances are enabled for all potential instaces of a
 * peripheral. It takes peripheral name and checks whether NRFX_\<peripheral\>\<id\>_ENABLED
 * is set to 1 and if yes then provided macro is evaluated for given instance.
 *
 * Macro supports check on instances with following names:
 * - \<periph_name\>0 - \<periph_name\>255 - e.g. SPIM0, SPIM255
 * - \<periph_name\>00 - \<periph_name\>099 - e.g. SPIM00, SPIM099
 * - \<periph_name\>000 - \<periph_name\>009 - e.g. SPIM000, SPIM009
 *
 * @param[in] periph_name Peripheral name, e.g. SPIM.
 * @param[in] macro       Macro which is resolved if driver instance is enabled. Macro has following
 *                        arguments: macro(periph_name, prefix, i, ...).
 * @param[in] sep         Separator added between all evaluations, in parentheses.
 * @param[in] off_code    Code injected for disabled instances, in parentheses.
 */
#define NRFX_FOREACH_ENABLED(periph_name, macro, sep, off_code, ...)                  \
        NRFX_LISTIFY(256, _NRFX_EVAL_IF_ENABLED, sep,                                 \
                     off_code, periph_name, , macro, __VA_ARGS__) NRFX_DEBRACKET sep  \
        NRFX_LISTIFY(100, _NRFX_EVAL_IF_ENABLED, sep,                                 \
                     off_code, periph_name, 0, macro, __VA_ARGS__) NRFX_DEBRACKET sep \
        NRFX_LISTIFY(10, _NRFX_EVAL_IF_ENABLED, sep,                                  \
                     off_code, periph_name, 00, macro, __VA_ARGS__)

/**
 * @brief Macro for resolving provided user macro for present indexed instances of a peripheral.
 *
 * Macro checks if peripheral instances are present by checking if there is a token
 * NRF_\<periph_name\>\<id\> defined with wrapped in parenthesis value.
 *
 * Macro supports check on instances with following names:
 * - \<periph_name\>0 - \<periph_name\>255 - e.g. SPIM0, SPIM255
 * - \<periph_name\>00 - \<periph_name\>099 - e.g. SPIM00, SPIM099
 * - \<periph_name\>000 - \<periph_name\>009 - e.g. SPIM000, SPIM009
 *
 * @param[in] periph_name Peripheral name, e.g. SPIM.
 * @param[in] macro       Macro which is resolved if peripheral instance is present.
 *                        Macro has following arguments: macro(periph_name, prefix, i, ...).
 * @param[in] sep         Separator added between all evaluations, in parentheses.
 * @param[in] off_code    Code injected for disabled instances, in parentheses.
 */
#define NRFX_FOREACH_INDEXED_PRESENT(periph_name, macro, sep, off_code, ...)           \
        NRFX_LISTIFY(256, _NRFX_EVAL_IF_PRESENT, sep,                                  \
                     off_code, periph_name, , macro, __VA_ARGS__) NRFX_DEBRACKET sep   \
        NRFX_LISTIFY(100, _NRFX_EVAL_IF_PRESENT, sep,                                  \
                     off_code, periph_name, 0, macro, __VA_ARGS__) NRFX_DEBRACKET sep  \
        NRFX_LISTIFY(10, _NRFX_EVAL_IF_PRESENT, sep,                                   \
                     off_code, periph_name, 00, macro, __VA_ARGS__) NRFX_DEBRACKET sep \

/**
 * @brief Macro for resolving provided user macro for present instances of a peripheral.
 *
 * Macro checks if peripheral instances are present by checking if there is a token
 * NRF_\<periph_name\>\<id\> defined with wrapped in parenthesis value.
 *
 * Macro supports check on instances with following names:
 * - \<periph_name\>0 - \<periph_name\>255 - e.g. SPIM0, SPIM255
 * - \<periph_name\>00 - \<periph_name\>099 - e.g. SPIM00, SPIM099
 * - \<periph_name\>000 - \<periph_name\>009 - e.g. SPIM000, SPIM009
 * - \<periph_name\> - e.g. SPIM
 *
 * @param[in] periph_name Peripheral name, e.g. SPIM.
 * @param[in] macro       Macro which is resolved if peripheral instance is present.
 *                        Macro has following arguments: macro(periph_name, prefix, i, ...).
 * @param[in] sep         Separator added between all evaluations, in parentheses.
 * @param[in] off_code    Code injected for disabled instances, in parentheses.
 */
#define NRFX_FOREACH_PRESENT(periph_name, macro, sep, off_code, ...)                 \
        NRFX_FOREACH_INDEXED_PRESENT(periph_name, macro, sep, off_code, __VA_ARGS__) \
        _NRFX_EVAL_IF_PRESENT(, off_code, periph_name, , macro, __VA_ARGS__)

/**
 * @brief Macro for resolving provided user macro on concatenated peripheral name
 *        and instance index.
 *
 * Execute provided macro with single argument <instance\>
 * that is the concatenation of @p periph_name, @p prefix and @p i.
 *
 * @param[in] i           Instance index.
 * @param[in] periph_name Peripheral name, e.g. SPIM.
 * @param[in] prefix      Prefix added before instance index, e.g. some device has
 *                        instances named like SPIM00. First 0 is passed here as prefix.
 * @param[in] macro       Macro which is executed.
 * @param[in] ...         Variable length arguments passed to the @p macro. Macro has following
 *                        arguments: macro(instance, ...).
 */
#define NRFX_INSTANCE_CONCAT(periph_name, prefix, i, macro, ...) \
      macro(NRFX_CONCAT(periph_name, prefix, i), __VA_ARGS__)

/**
 * @brief Macro for creating a content for enum which is listing enabled driver instances.
 *
 * It creates comma separated list of entries like NRFX_\<instance_name\>_INST_IDX,
 * e.g. (NRFX_SPIM0_INST_IDX) for all enabled instances (NRFX_\<instance_name\>_ENABLED
 * is set to 1). It should be called within enum declaration. Created enum is used
 * by the driver to index all enabled instances of the driver.
 *
 * @param[in] periph_name Peripheral name (e.g. SPIM).
 */
#define NRFX_INSTANCE_ENUM_LIST(periph_name) \
        NRFX_FOREACH_ENABLED(periph_name, _NRFX_INST_ENUM, (), ())

/**
 * @brief Macro for creating a content for enum which is listing potential driver instances.
 *
 * It creates comma separated list of entries like NRFX_\<instance_name\>_INST_IDX,
 * e.g. (NRFX_SPIM0_INST_IDX) for all present instances (NRF_\<instance_name\> is present).
 * It should be called within enum declaration.
 *
 * @param[in] periph_name Peripheral name (e.g. SPIM).
 */
#define NRFX_INSTANCE_PRESENT_ENUM_LIST(periph_name) \
        NRFX_FOREACH_INDEXED_PRESENT(periph_name, _NRFX_INST_ENUM, (), ())

/**
 * @brief Macro for translating peripheral address to its instance number.
 *
 * @param[in] periph_name Peripheral name, e.g. SPIM.
 * @param[in] reg         Peripheral address.
 *
 * @return Peripheral instance number if address matches, 255 otherwise.
 */
#define NRFX_REG_TO_INSTANCE(periph_name, reg)                                      \
        NRFX_FOREACH_INDEXED_PRESENT(periph_name, _PERIPH_PTR_COMPARE, (), (), reg) \
        255

/**
 * @brief Macro for creating an interrupt handler for all enabled driver instances.
 *
 * Macro creates a set of functions which calls generic @p irq_handler function with two parameters:
 * - peripheral instance register pointer
 * - pointer to a control block structure associated with the given instance
 *
 * Generic interrupt handler function with above mentioned parameters named @p irq_handler
 * must be implemented in the driver.
 *
 * @note Handlers are using enum which should be generated using @ref NRFX_INSTANCE_ENUM_LIST.
 *
 * @param[in] periph_name       Peripheral name, e.g. SPIM.
 * @param[in] periph_name_small Peripheral name written with small letters, e.g. spim.
 */
#define NRFX_INSTANCE_IRQ_HANDLERS(periph_name, periph_name_small) \
    NRFX_FOREACH_ENABLED(periph_name, _NRFX_IRQ_HANDLER, (), (), periph_name_small)

/**
 * @brief Macro for creating an interrupt handler for all enabled driver instances
 *        with the specified extra parameter.
 *
 * Macro creates set of function which calls generic @p irq_handler function with three parameters:
 * - peripheral instance register pointer
 * - pointer to a control block structure associated with the given instance
 * - provided @p ext_macro called with peripheral name suffix (e.g. 01 for TIMER01)
 *
 * Generic interrupt handler function with above mentioned parameters named @p irq_handler
 * must be implemented in the driver.
 *
 * @note Handlers are using enum which should be generated using @ref NRFX_INSTANCE_ENUM_LIST.
 *
 * @param[in] periph_name       Peripheral name, e.g. SPIM.
 * @param[in] periph_name_small Peripheral name written with small letters, e.g. rtc.
 * @param[in] ext_macro         External macro to be executed for each instance.
 */
#define NRFX_INSTANCE_IRQ_HANDLERS_EXT(periph_name, periph_name_small, ext_macro) \
    NRFX_FOREACH_ENABLED(periph_name, _NRFX_IRQ_HANDLER_EXT, (), (), periph_name_small, ext_macro)

/**
 * @brief Macro for declaring an interrupt handler for all enabled driver instances.
 *
 * Macro creates set of function declarations. It is intended to be used in the driver header.
 *
 * @param[in] periph_name       Peripheral name, e.g. SPIM.
 * @param[in] periph_name_small Peripheral name written with small letters, e.g. spim.
 */
#define NRFX_INSTANCE_IRQ_HANDLERS_DECLARE(periph_name, periph_name_small) \
    NRFX_FOREACH_ENABLED(periph_name, _NRFX_IRQ_HANDLER_DECLARE, (), (), periph_name_small)

/**
 * @brief Macro for generating comma-separated list of interrupt handlers for all
 *        enabled driver instances.
 *
 * Interrupt handlers are generated using @ref NRFX_INSTANCE_IRQ_HANDLERS.
 * It is intended to be used to create a list which is used for passing an interrupt
 * handler function to the PRS driver.
 *
 * @param[in] periph_name       Peripheral name, e.g. SPIM.
 * @param[in] periph_name_small Peripheral name written with small letters, e.g. spim.
 */
#define NRFX_INSTANCE_IRQ_HANDLERS_LIST(periph_name, periph_name_small) \
    NRFX_FOREACH_ENABLED(periph_name, _NRFX_IRQ_HANDLER_LIST, (), (), periph_name_small)

/**
 * @brief Macro for generating instance specific interrupt handler for a
 *        specific driver.
 *
 * This macro should be called in user code outside of function if user wants to
 * use interrupts with given peripheral instance.
 *
 * @param[in] periph_name_small Peripheral name written with small letters, e.g. spim.
 * @param[in] inst_idx          Instance index.
 * @param[in] p_instance        Pointer to the driver instance object.
 */
#define NRFX_INSTANCE_IRQ_HANDLER_DEFINE(periph_name_small, inst_idx, p_instance) \
    void NRFX_CONCAT(nrfx_, periph_name_small, _, inst_idx, _irq_handler)(void *) \
    {                                                                             \
        NRFX_CONCAT(nrfx_, periph_name_small, _irq_handler)(p_instance);          \
    }

/**
 * @brief Macro for checking if given peripheral instance is present on the target.
 *
 * Macro utilizes the fact that for each existing instance a define is created which points to
 * the memory mapped register set casted to a register set structure. It is wrapped in parenthesis
 * and existance of parethesis wrapping is used to determine if instance exists. It if does not
 * exist then token (e.g. NRF_SPIM10) is undefined so it does not have parenthesis wrapping.
 *
 * Since macro returns literal 1 it can be used by other macros.
 *
 * @param[in] _inst Instance, .e.g SPIM10.
 *
 * @retval 1 If instance is present.
 * @retval 0 If instance is not present.
 */
#define NRFX_INSTANCE_PRESENT(_inst) NRFX_ARG_HAS_PARENTHESIS(NRFX_CONCAT(NRF_, _inst))

/**
 * @brief Macro for getting the smaller value between two arguments.
 *
 * @param[in] a First argument.
 * @param[in] b Second argument.
 *
 * @return Smaller value between two arguments.
 */
#define NRFX_MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief Macro for getting the larger value between two arguments.
 *
 * @param[in] a First argument.
 * @param[in] b Second argument.
 *
 * @return Larger value between two arguments.
 */
#define NRFX_MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Macro for checking if a given value is in a given range.
 *
 * @note @p val is evaluated twice.
 *
 * @param[in] val A value to be checked.
 * @param[in] min The lower bound (inclusive).
 * @param[in] max The upper bound (inclusive).
 *
 * @retval true  The value is in the given range.
 * @retval false The value is out of the given range.
 */
#define NRFX_IN_RANGE(val, min, max) ((val) >= (min) && (val) <= (max))

/**
 * @brief Macro for performing rounded integer division (as opposed to
 *        truncating the result).
 *
 * @param[in] a Numerator.
 * @param[in] b Denominator.
 *
 * @return Rounded (integer) result of dividing @c a by @c b.
 */
#define NRFX_ROUNDED_DIV(a, b) \
    ((((a) < 0) ^ ((b) < 0)) ? (((a) - (b) / 2) / (b)) : (((a) + (b) / 2) / (b)))

/**
 * @brief Macro for performing integer division, making sure the result is rounded up.
 *
 * @details A typical use case for this macro is to compute the number of objects
 *          with size @c b required to hold @c a number of bytes.
 *
 * @param[in] a Numerator.
 * @param[in] b Denominator.
 *
 * @return Integer result of dividing @c a by @c b, rounded up.
 */
#define NRFX_CEIL_DIV(a, b) ((((a) - 1) / (b)) + 1)

/**
 * @brief Macro for getting the number of elements in an array.
 *
 * @param[in] array Name of the array.
 *
 * @return Array element count.
 */
#define NRFX_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/**
 * @brief Macro for getting the offset (in bytes) from the beginning of a structure
 *        of the specified type to its specified member.
 *
 * @param[in] type   Structure type.
 * @param[in] member Structure member whose offset is searched for.
 *
 * @return Member offset in bytes.
 */
#define NRFX_OFFSETOF(type, member) ((size_t) & (((type *)0)->member))

/**
 * @brief Macro for checking whether given number is power of 2.
 *
 * @param[in] val Tested value.
 *
 * @retval true  The value is power of 2.
 * @retval false The value is not power of 2.
 */
#define NRFX_IS_POWER_OF_TWO(val) (((val) != 0) && ((val) & ((val) - 1)) == 0)

/**
 * @brief Macro for checking whether a given number is even.
 *
 * @param[in] val Tested value.
 *
 * @retval true  The value is even.
 * @retval false The value is odd.
 */
#define NRFX_IS_EVEN(val) (((val) % 2)  == 0)

/**
 * @brief Macro for checking if given lengths of EasyDMA transfers do not exceed
 *        the limit of the specified peripheral.
 *
 * @param[in] peripheral Peripheral to check the lengths against.
 * @param[in] length1    First length to be checked.
 * @param[in] length2    Second length to be checked (pass 0 if not needed).
 *
 * @retval true  The length of buffers does not exceed the limit of the specified peripheral.
 * @retval false The length of buffers exceeds the limit of the specified peripheral.
 */
#define NRFX_EASYDMA_LENGTH_VALIDATE(peripheral, length1, length2)            \
    (((length1) < (1U << NRFX_CONCAT_2(peripheral, _EASYDMA_MAXCNT_SIZE))) && \
     ((length2) < (1U << NRFX_CONCAT_2(peripheral, _EASYDMA_MAXCNT_SIZE))))

/**
 * @brief Macro for waiting until condition is met.
 *
 * @param[in]  condition Condition to meet.
 * @param[in]  attempts  Maximum number of condition checks. Must not be 0.
 * @param[in]  delay_us  Delay between consecutive checks, in microseconds.
 * @param[out] result    Boolean variable to store the result of the wait process.
 *                       Set to true if the condition is met or false otherwise.
 */
#define NRFX_WAIT_FOR(condition, attempts, delay_us, result) \
do {                                                         \
    result =  false;                                         \
    uint32_t remaining_attempts = (attempts);                \
    do {                                                     \
           if (condition)                                    \
           {                                                 \
               result =  true;                               \
               break;                                        \
           }                                                 \
           NRFX_DELAY_US(delay_us);                          \
    } while (--remaining_attempts);                          \
} while(0)

/**
 * @brief Macro for getting the ID number of the specified peripheral.
 *
 * For peripherals in Nordic SoCs, there is a direct relationship between their
 * ID numbers and their base addresses. See the chapter "Peripheral interface"
 * (section "Peripheral ID") in the Product Specification.
 *
 * @param[in] base_addr Peripheral base address or pointer.
 *
 * @return ID number associated with the specified peripheral.
 */
#define NRFX_PERIPHERAL_ID_GET(base_addr) (uint16_t)(((uint32_t)(base_addr) >> 12) & 0x000001FF)

/**
 * @brief Macro for getting the interrupt number assigned to a specific
 *        peripheral.
 *
 * For peripherals in Nordic SoCs, the IRQ number assigned to a peripheral is
 * equal to its ID number. See the chapter "Peripheral interface" (sections
 * "Peripheral ID" and "Interrupts") in the Product Specification.
 *
 * @warning This macro is valid only for peripherals with a single interrupt line.
 *
 * @param[in] base_addr Peripheral base address or pointer.
 *
 * @return Interrupt number associated with the specified peripheral.
 */
#define NRFX_IRQ_NUMBER_GET(base_addr) NRFX_PERIPHERAL_ID_GET(base_addr)

/**
 * @brief Macro for converting frequency in kHz to Hz.
 *
 * @param[in] freq Frequency value in kHz.
 *
 * @return Number of Hz in @p freq kHz.
 */
#define NRFX_KHZ_TO_HZ(freq) ((freq) * 1000)

/**
 * @brief Macro for converting frequency in MHz to Hz.
 *
 * @param[in] freq Frequency value in MHz.
 *
 * @return Number of Hz in @p freq MHz.
 */
#define NRFX_MHZ_TO_HZ(freq) ((freq) * 1000 * 1000)

/**
 * @brief Macro for inserting code depending on whether @p _flag exists and expands to 1 or not.
 *
 * To prevent the preprocessor from treating commas as argument
 * separators, the @p _if_1_code and @p _else_code expressions must be
 * inside brackets/parentheses: <tt>()</tt>. These are stripped away
 * during macro expansion.
 *
 * Example:
 *
 *     NRFX_COND_CODE_1(CONFIG_FLAG, (uint32_t x;), (there_is_no_flag();))
 *
 * If @p CONFIG_FLAG is defined to 1, this expands to:
 *
 *     uint32_t x;
 *
 * It expands to <tt>there_is_no_flag();</tt> otherwise.
 *
 * This could be used as an alternative to:
 *
 *     #if defined(CONFIG_FLAG) && (CONFIG_FLAG == 1)
 *     #define MAYBE_DECLARE(x) uint32_t x
 *     #else
 *     #define MAYBE_DECLARE(x) there_is_no_flag()
 *     #endif
 *
 *     MAYBE_DECLARE(x);
 *
 * However, the advantage of COND_CODE_1() is that code is resolved in
 * place where it is used, while the @p \#if method defines @p
 * MAYBE_DECLARE on two lines and requires it to be invoked again on a
 * separate line. This makes COND_CODE_1() more concise and also
 * sometimes more useful when used within another macro's expansion.
 *
 * @note @p _flag can be the result of preprocessor expansion,
 *       however @p _if_1_code is only expanded if @p _flag expands
 *       to the integer literal 1. Integer expressions that evaluate
 *       to 1, e.g. after doing some arithmetic, will not work.
 *
 * @param[in] _flag      Evaluated flag
 * @param[in] _if_1_code Result if @p _flag expands to 1; must be in parentheses
 * @param[in] _else_code Result otherwise; must be in parentheses
 */
#define NRFX_COND_CODE_1(_flag, _if_1_code, _else_code) \
    _NRFX_COND_CODE_1(_flag, _if_1_code, _else_code)

/**
 * @brief Macro for inserting code depending on whether @p _flag exists and expands to 0 or not.
 *
 * This is like @ref NRFX_COND_CODE_1(), except that it tests whether @p _flag
 * expands to the integer literal 0. It expands to @p _if_0_code if
 * so, and @p _else_code otherwise; both of these must be enclosed in
 * parentheses.
 *
 * @param[in] _flag      Evaluated flag
 * @param[in] _if_0_code Result if @p _flag expands to 0; must be in parentheses
 * @param[in] _else_code Result otherwise; must be in parentheses
 */
#define NRFX_COND_CODE_0(_flag, _if_0_code, _else_code) \
    _NRFX_COND_CODE_0(_flag, _if_0_code, _else_code)

/**
 * @brief Macro for checking for macro definition in compiler-visible expressions
 *
 * It has the effect of taking a macro value that may be defined to "1"
 * or may not be defined at all and turning it into a literal
 * expression that can be handled by the C compiler instead of just
 * the preprocessor.
 *
 * That is, it works similarly to <tt>\#if defined(CONFIG_FOO)</tt>
 * except that its expansion is a C expression. Thus, much <tt>\#ifdef</tt>
 * usage can be replaced with equivalents like:
 *
 *     if (IS_ENABLED(CONFIG_FOO)) {
 *             do_something_with_foo
 *     }
 *
 * This is cleaner since the compiler can generate errors and warnings
 * for @p do_something_with_foo even when @p CONFIG_FOO is undefined.
 *
 * @param[in] config_macro Macro to check
 *
 * @return 1 if @p config_macro is defined to 1, 0 otherwise (including
 *         if @p config_macro is not defined)
 */
#define NRFX_IS_ENABLED(config_macro) _NRFX_IS_ENABLED1(config_macro)

/**
 * @brief Macro for generating a sequence of code with configurable separator.
 *
 * Example:
 *
 *     #define FOO(i, _) MY_PWM ## i
 *     { NRFX_LISTIFY(PWM_COUNT, FOO, (,)) }
 *
 * The above two lines expand to:
 *
 *    { MY_PWM0 , MY_PWM1 }
 *
 * @param[in] LEN The length of the sequence. Must be an integer literal less
 *                than 255.
 * @param[in] F   A macro function that accepts at least two arguments:
 *                <tt>F(i, ...)</tt>. @p F is called repeatedly in the expansion.
 *                Its first argument @p i is the index in the sequence, and
 *                the variable list of arguments passed to LISTIFY are passed
 *                through to @p F.
 * @param[in] sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            this is required to enable providing a comma as separator.
 *
 * @note Calling NRFX_LISTIFY with undefined arguments has undefined behavior.
 */
#define NRFX_LISTIFY(LEN, F, sep, ...) \
    NRFX_CONCAT_2(_NRFX_LISTIFY_, LEN)(F, sep, __VA_ARGS__)

/**
 * @brief Macro for checking if input argument is empty.
 *
 * Empty means that nothing is provided or provided value is resolved to nothing
 * (e.g. empty define).
 *
 * Macro idea is taken from P99 which is under Apache 2.0 license and described by
 * Jens Gustedt https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
 *
 * @param arg Argument.
 *
 * @retval 1 if argument is empty.
 * @retval 0 if argument is not empty.
 */
#define NRFX_IS_EMPTY(arg) _NRFX_IS_EMPTY(arg)

/**
 * @brief Macro for calculating number of arguments in the variable arguments list minus one.
 *
 * @param[in] ... List of arguments
 *
 * @return Number of variadic arguments in the argument list, minus one
 */
#define NRFX_NUM_VA_ARGS_LESS_1(...) \
        _NRFX_NUM_VA_ARGS_LESS_1_IMPL(__VA_ARGS__, 63, 62, 61, \
                    60, 59, 58, 57, 56, 55, 54, 53, 52, 51, \
                    50, 49, 48, 47, 46, 45, 44, 43, 42, 41, \
                    40, 39, 38, 37, 36, 35, 34, 33, 32, 31, \
                    30, 29, 28, 27, 26, 25, 24, 23, 22, 21, \
                    20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
                    10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, ~)

/**
 * @brief Macro for concatenating multiple arguments.
 *
 * Support up to 8 arguments.
 *
 * @param[in] ... Arguments to concatenate.
 */
#define NRFX_CONCAT(...) \
    NRFX_CONCAT_2(_NRFX_CONCAT_, NRFX_NUM_VA_ARGS_LESS_1(__VA_ARGS__))(__VA_ARGS__)

/**
 * @brief Macro for checking if argument starts with opening round bracket and contains matching
 *        closing bracket (parenthesis).
 *
 * @param[in] x Input argument.
 *
 * @retval 1 If input argument starts with opening bracket and contains closing bracket.
 * @retval 0 If input argument does not match above mentioned condition.
 */
#define NRFX_ARG_HAS_PARENTHESIS(x) _NRFX_GET_ARG3(_NRFX_EVAL(_NRFX_ARG_HAS_PARENTHESIS x, 1, 0))

/**
 * @brief Macro for calling a macro @p F on each provided argument with a given
 *        separator between each call.
 *
 * Example:
 *
 *     #define F(x) int a##x
 *     NRFX_FOR_EACH(F, (;), 4, 5, 6);
 *
 * This expands to:
 *
 *     int a4;
 *     int a5;
 *     int a6;
 *
 * @param F Macro to invoke
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            this is required to enable providing a comma as a separator.
 * @param ... Variable argument list. The macro @p F is invoked as
 *            <tt>F(element)</tt> for each element in the list.
 */
#define NRFX_FOR_EACH(F, sep, ...) \
    _NRFX_FOR_EACH(F, sep, NRFX_REVERSE_ARGS(__VA_ARGS__))

/**
 * @brief Call macro @p F on each provided argument, with the argument's index
 *        as an additional parameter.
 *
 * This is like @ref NRFX_FOR_EACH(), except @p F should be a macro which takes two
 * arguments: <tt>F(index, variable_arg)</tt>.
 *
 * Example:
 *
 *     #define F(idx, x) int a##idx = x
 *     NRFX_FOR_EACH_IDX(F, (;), 4, 5, 6);
 *
 * This expands to:
 *
 *     int a0 = 4;
 *     int a1 = 5;
 *     int a2 = 6;
 *
 * @param F Macro to invoke
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            this is required to enable providing a comma as a separator.
 * @param ... Variable argument list. The macro @p F is invoked as
 *            <tt>F(index, element)</tt> for each element in the list.
 */
#define NRFX_FOR_EACH_IDX(F, sep, ...) \
    _NRFX_FOR_EACH_IDX(F, sep, NRFX_REVERSE_ARGS(__VA_ARGS__))

/**
 * @brief Macro for calling macro @p F on each provided argument, with an additional fixed
 *        argument as a parameter.
 *
 * This is like @ref NRFX_FOR_EACH(), except @p F should be a macro which takes two
 * arguments: <tt>F(variable_arg, fixed_arg)</tt>.
 *
 * Example:
 *
 *     static void func(int val, void *dev);
 *     NRFX_FOR_EACH_FIXED_ARG(func, (;), dev, 4, 5, 6);
 *
 * This expands to:
 *
 *     func(4, dev);
 *     func(5, dev);
 *     func(6, dev);
 *
 * @param F Macro to invoke
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            this is required to enable providing a comma as a separator.
 * @param fixed_arg Fixed argument passed to @p F as the second macro parameter.
 * @param ... Variable argument list. The macro @p F is invoked as
 *            <tt>F(element, fixed_arg)</tt> for each element in the list.
 */
#define NRFX_FOR_EACH_FIXED_ARG(F, sep, fixed_arg, ...) \
    _NRFX_FOR_EACH_FIXED_ARG(F, sep, fixed_arg, NRFX_REVERSE_ARGS(__VA_ARGS__))

/**
 * @brief Macro from calling macro @p F for each variable argument with an index and fixed
 *        argument
 *
 * This is like the combination of @ref NRFX_FOR_EACH_IDX() with @ref NRFX_FOR_EACH_FIXED_ARG().
 *
 * Example:
 *
 *     #define F(idx, x, fixed_arg) int fixed_arg##idx = x
 *     NRFX_FOR_EACH_IDX_FIXED_ARG(F, (;), a, 4, 5, 6);
 *
 * This expands to:
 *
 *     int a0 = 4;
 *     int a1 = 5;
 *     int a2 = 6;
 *
 * @param F Macro to invoke
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            This is required to enable providing a comma as a separator.
 * @param fixed_arg Fixed argument passed to @p F as the third macro parameter.
 * @param ... Variable list of arguments. The macro @p F is invoked as
 *            <tt>F(index, element, fixed_arg)</tt> for each element in
 *            the list.
 */
#define NRFX_FOR_EACH_IDX_FIXED_ARG(F, sep, fixed_arg, ...) \
    _NRFX_FOR_EACH_IDX_FIXED_ARG(F, sep, fixed_arg, NRFX_REVERSE_ARGS(__VA_ARGS__))

/**
 * @brief Macro for reversing arguments order.
 *
 * @param ... Variable argument list.
 *
 * @return Input arguments in reversed order.
 */
#define NRFX_REVERSE_ARGS(...) \
    _NRFX_FOR_EACH_ENGINE(_NRFX_FOR_EACH_EXEC, (,), NRFX_EVAL, _, __VA_ARGS__)


/**
 * @brief Macro for getting the highest value from input arguments.
 *
 * It is similar to @ref NRFX_MAX but accepts a variable number of arguments.
 *
 * @note Input arguments must be numeric variables.
 *
 * @param ... Variable argument list.
 *
 * @return Highest value from the input list.
 */
#define NRFX_MAX_N(...) \
    NRFX_EVAL(NRFX_FOR_EACH(_NRFX_MAX_P1, (), __VA_ARGS__) 0 \
              NRFX_FOR_EACH(_NRFX_MAX_P2, (), __VA_ARGS__))

/**
 * @brief Macro for getting a position of a bit in a bit mask with only one bit set.
 *
 * Macro shall be used only on fixed value so that it can be resolved at compile time.
 *
 * @param _bitmask 32 bit mask with one bit set.
 *
 * @return Index of a bit.
 */
#define NRFX_BITMASK_TO_BITPOS(_bitmask) \
    (_bitmask == NRFX_BIT(0) ? 0 : \
     _bitmask == NRFX_BIT(1) ? 1 : \
     _bitmask == NRFX_BIT(2) ? 2 : \
     _bitmask == NRFX_BIT(3) ? 3 : \
     _bitmask == NRFX_BIT(4) ? 4 : \
     _bitmask == NRFX_BIT(5) ? 5 : \
     _bitmask == NRFX_BIT(6) ? 6 : \
     _bitmask == NRFX_BIT(7) ? 7 : \
     _bitmask == NRFX_BIT(8) ? 8 : \
     _bitmask == NRFX_BIT(9) ? 9 : \
     _bitmask == NRFX_BIT(10) ? 10 : \
     _bitmask == NRFX_BIT(11) ? 11 : \
     _bitmask == NRFX_BIT(12) ? 12 : \
     _bitmask == NRFX_BIT(13) ? 13 : \
     _bitmask == NRFX_BIT(14) ? 14 : \
     _bitmask == NRFX_BIT(15) ? 15 : \
     _bitmask == NRFX_BIT(16) ? 16 : \
     _bitmask == NRFX_BIT(17) ? 17 : \
     _bitmask == NRFX_BIT(18) ? 18 : \
     _bitmask == NRFX_BIT(19) ? 19 : \
     _bitmask == NRFX_BIT(20) ? 20 : \
     _bitmask == NRFX_BIT(21) ? 21 : \
     _bitmask == NRFX_BIT(22) ? 22 : \
     _bitmask == NRFX_BIT(23) ? 23 : \
     _bitmask == NRFX_BIT(24) ? 24 : \
     _bitmask == NRFX_BIT(25) ? 25 : \
     _bitmask == NRFX_BIT(26) ? 26 : \
     _bitmask == NRFX_BIT(27) ? 27 : \
     _bitmask == NRFX_BIT(28) ? 28 : \
     _bitmask == NRFX_BIT(29) ? 29 : \
     _bitmask == NRFX_BIT(30) ? 30 : 31)

/** @} */

#endif /* NRFX_UTILS_H__ */
