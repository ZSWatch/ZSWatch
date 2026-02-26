/*
 * Copyright (c) 2014 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_TIMER_H__
#define NRF_TIMER_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_timer_hal TIMER HAL
 * @{
 * @ingroup nrf_timer
 * @brief   Hardware access layer for managing the TIMER peripheral.
 */

/**
 * @brief Macro getting pointer to the structure of registers of the TIMER peripheral.
 *
 * @param[in] idx TIMER instance index.
 *
 * @return Pointer to the structure of registers of the TIMER peripheral.
 */
#define NRF_TIMER_INST_GET(idx) NRFX_CONCAT(NRF_, TIMER, idx)

#if defined(TIMER_INTENSET_COMPARE4_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether timer has capture/compare channel 4. */
#define NRF_TIMER_HAS_CC4 1
#else
#define NRF_TIMER_HAS_CC4 0
#endif

#if defined(TIMER_INTENSET_COMPARE5_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether timer has capture/compare channel 5. */
#define NRF_TIMER_HAS_CC5 1
#else
#define NRF_TIMER_HAS_CC5 0
#endif

#if defined(TIMER_INTENSET_COMPARE6_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether timer has capture/compare channel 6. */
#define NRF_TIMER_HAS_CC6 1
#else
#define NRF_TIMER_HAS_CC6 0
#endif

#if defined(TIMER_INTENSET_COMPARE7_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether timer has capture/compare channel 7. */
#define NRF_TIMER_HAS_CC7 1
#else
#define NRF_TIMER_HAS_CC7 0
#endif

#if defined(TIMER_MODE_MODE_LowPowerCounter) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether timer supports low power mode. */
#define NRF_TIMER_HAS_LOW_POWER_MODE 1
#else
#define NRF_TIMER_HAS_LOW_POWER_MODE 0
#endif

#if defined(TIMER_ONESHOTEN_ONESHOTEN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether timer supports one-shot operation. */
#define NRF_TIMER_HAS_ONE_SHOT 1
#else
#define NRF_TIMER_HAS_ONE_SHOT 0
#endif

#if defined(TIMER_TASKS_SHUTDOWN_TASKS_SHUTDOWN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether timer supports shutdown task. */
#define NRF_TIMER_HAS_SHUTDOWN 1
#else
#define NRF_TIMER_HAS_SHUTDOWN 0
#endif

/** @brief Base frequency value 320 MHz for TIMER. */
#define NRF_TIMER_BASE_FREQUENCY_320MHZ (NRFX_MHZ_TO_HZ(320UL))
/** @brief Base frequency value 256 MHz for TIMER. */
#define NRF_TIMER_BASE_FREQUENCY_256MHZ (NRFX_MHZ_TO_HZ(256UL))
/** @brief Base frequency value 128 MHz for TIMER. */
#define NRF_TIMER_BASE_FREQUENCY_128MHZ (NRFX_MHZ_TO_HZ(128UL))
/** @brief Base frequency value 64 MHz for TIMER. */
#define NRF_TIMER_BASE_FREQUENCY_64MHZ  (NRFX_MHZ_TO_HZ(64UL))
/** @brief Base frequency value 32 MHz for TIMER. */
#define NRF_TIMER_BASE_FREQUENCY_32MHZ  (NRFX_MHZ_TO_HZ(32UL))
/** @brief Base frequency value 16 MHz for TIMER. */
#define NRF_TIMER_BASE_FREQUENCY_16MHZ  (NRFX_MHZ_TO_HZ(16UL))

#if defined(TIMER_PRESCALER_PRESCALER_Max) || defined(__NRFX_DOXYGEN__)
/** @brief Maximum value of PRESCALER register. */
#define NRF_TIMER_PRESCALER_MAX TIMER_PRESCALER_PRESCALER_Max
#else
#define NRF_TIMER_PRESCALER_MAX 9
#endif

/**
 * @brief Macro for getting the maximum bit resolution of the specified timer instance.
 *
 * @param[in] id Index of the specified timer instance.
 *
 * @retval Maximum bit resolution of the specified timer instance.
 */
#define TIMER_MAX_SIZE(id)  NRFX_CONCAT_3(TIMER, id, _MAX_SIZE)

/**
 * @brief Macro for validating the correctness of the bit width resolution setting.
 *
 * @param[in] id        Index of the specified timer instance.
 * @param[in] bit_width Bit width resolution value to be checked.
 *
 * @retval true  Timer instance supports the specified bit width resolution value.
 * @retval false Timer instance does not support the specified bit width resolution value.
 */
#define TIMER_BIT_WIDTH_MAX(id, bit_width) \
    (TIMER_MAX_SIZE(id) == 8   ? (bit_width == NRF_TIMER_BIT_WIDTH_8)  :  \
    (TIMER_MAX_SIZE(id) == 16  ? (bit_width == NRF_TIMER_BIT_WIDTH_8)  || \
                                 (bit_width == NRF_TIMER_BIT_WIDTH_16)  : \
    (TIMER_MAX_SIZE(id) == 24  ? (bit_width == NRF_TIMER_BIT_WIDTH_8)  || \
                                 (bit_width == NRF_TIMER_BIT_WIDTH_16) || \
                                 (bit_width == NRF_TIMER_BIT_WIDTH_24) :  \
    (TIMER_MAX_SIZE(id) == 32  ? (bit_width == NRF_TIMER_BIT_WIDTH_8)  || \
                                 (bit_width == NRF_TIMER_BIT_WIDTH_16) || \
                                 (bit_width == NRF_TIMER_BIT_WIDTH_24) || \
                                 (bit_width == NRF_TIMER_BIT_WIDTH_32) :  \
    false))))

/** @brief Internal macro used by @ref NRF_TIMER_IS_BIT_WIDTH_VALID. */
#define _NRF_TIMER_IS_BIT_WIDTH_VALID_ELEM(periph_name, prefix, idx, reg, bit_width) \
    ((reg == NRFX_CONCAT(NRF_TIMER, prefix, idx)) && TIMER_BIT_WIDTH_MAX(prefix##idx, bit_width)) ||

/**
 * @brief Macro for checking correctness of bit width configuration for the specified timer.
 *
 * @param[in] p_reg     Timer instance register.
 * @param[in] bit_width Bit width resolution value to be checked.
 *
 * @retval true  Timer instance supports the specified bit width resolution value.
 * @retval false Timer instance does not support the specified bit width resolution value.
 */
#define NRF_TIMER_IS_BIT_WIDTH_VALID(p_reg, bit_width) \
    NRFX_FOREACH_PRESENT(TIMER, _NRF_TIMER_IS_BIT_WIDTH_VALID_ELEM, (), (), p_reg, bit_width) 0

#if !defined(NRF_TIMER_IS_320MHZ_TIMER)
/** @brief Macro for checking whether the base frequency for the specified timer is 320 MHz. */
#define NRF_TIMER_IS_320MHZ_TIMER(p_reg) \
    (NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER120), (p_reg == NRF_TIMER120), (false)) || \
     NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER121), (p_reg == NRF_TIMER121), (false)))
#endif

#if !defined(NRF_TIMER_IS_256MHZ_TIMER)
/** @brief Macro for checking whether the base frequency for the specified timer is 256 MHz. */
#define NRF_TIMER_IS_256MHZ_TIMER(p_reg)                                                     \
    (NRFX_COND_CODE_1(NRFX_IS_ENABLED(NRF_CPU_FREQ_IS_256MHZ),                               \
        (NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER00), (p_reg == NRF_TIMER00), (false))), \
        (false)))
#endif

#if !defined(NRF_TIMER_IS_128MHZ_TIMER)
/** @brief Macro for checking whether the base frequency for the specified timer is 128 MHz. */
#define NRF_TIMER_IS_128MHZ_TIMER(p_reg)                                                     \
    (NRFX_COND_CODE_1(NRFX_IS_ENABLED(NRF_CPU_FREQ_IS_128MHZ),                               \
        (NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER00), (p_reg == NRF_TIMER00), (false))), \
        (false)))
#endif

#if !defined(NRF_TIMER_IS_64MHZ_TIMER)
/** @brief Macro for checking whether the base frequency for the specified timer is 64 MHz. */
#define NRF_TIMER_IS_64MHZ_TIMER(p_reg)                                                      \
    (NRFX_COND_CODE_1(NRFX_IS_ENABLED(NRF_CPU_FREQ_IS_64MHZ),                                \
        (NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER00), (p_reg == NRF_TIMER00), (false))), \
        (false)))
#endif

#if !defined(NRF_TIMER_IS_32MHZ_TIMER)
/** @brief Macro for checking whether the base frequency for the specified timer is 32 MHz. */
#define NRF_TIMER_IS_32MHZ_TIMER(p_reg) \
    (NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER020), (p_reg == NRF_TIMER020), (false)) || \
     NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER021), (p_reg == NRF_TIMER021), (false)) || \
     NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER022), (p_reg == NRF_TIMER022), (false)) || \
     NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(TIMER10),  (p_reg == NRF_TIMER10),  (false)))
#endif

/**
 * @brief Macro for getting base frequency value in Hz for the specified timer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
#define NRF_TIMER_BASE_FREQUENCY_GET(p_reg)                                  \
    ((NRF_TIMER_IS_320MHZ_TIMER(p_reg)) ? (NRF_TIMER_BASE_FREQUENCY_320MHZ): \
    ((NRF_TIMER_IS_256MHZ_TIMER(p_reg)) ? (NRF_TIMER_BASE_FREQUENCY_256MHZ): \
    ((NRF_TIMER_IS_128MHZ_TIMER(p_reg)) ? (NRF_TIMER_BASE_FREQUENCY_128MHZ): \
    ((NRF_TIMER_IS_64MHZ_TIMER(p_reg))  ? (NRF_TIMER_BASE_FREQUENCY_64MHZ) : \
    ((NRF_TIMER_IS_32MHZ_TIMER(p_reg))  ? (NRF_TIMER_BASE_FREQUENCY_32MHZ) : \
    (NRF_TIMER_BASE_FREQUENCY_16MHZ))))))

/**
 * @brief Macro for computing prescaler value for given base frequency and desired frequency.
 *
 * @warning Not every combination of base frequency and desired frequency is supported.
 *
 * @param[in] base_freq Base clock frequency for timer in Hz.
 * @param[in] frequency Desired frequency value in Hz.
 */
#define NRF_TIMER_PRESCALER_CALCULATE(base_freq, frequency) \
        NRF_CTZ((uint32_t)(base_freq) / (uint32_t)(frequency))

/**
 * @brief Macro for checking whether specified frequency can be achived for given timer instance.
 *
 * @note Macro is using compile time assertion.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] frequency Desired frequency value in Hz.
 */
#define NRF_TIMER_FREQUENCY_STATIC_CHECK(p_reg, frequency)                                       \
    NRFX_STATIC_ASSERT(                                                                          \
        (NRF_TIMER_BASE_FREQUENCY_GET(p_reg) == frequency) ||                                    \
        ((NRF_TIMER_BASE_FREQUENCY_GET(p_reg) % frequency == 0) &&                               \
         NRFX_IS_POWER_OF_TWO(NRF_TIMER_BASE_FREQUENCY_GET(p_reg) / (uint32_t)frequency) &&      \
         ((NRF_TIMER_BASE_FREQUENCY_GET(p_reg) / frequency) <= (1 << NRF_TIMER_PRESCALER_MAX))))

/**
 * @brief Macro for getting the number of capture/compare channels available
 *        in a given timer instance.
 *
 * @param[in] id Index of the specified timer instance.
 */
#define NRF_TIMER_CC_CHANNEL_COUNT(id) NRFX_CONCAT_3(TIMER, id, _CC_NUM)

/** @brief Symbol specifying maximum number of available compare channels. */
#define NRF_TIMER_CC_COUNT_MAX NRFX_ARRAY_SIZE(((NRF_TIMER_Type*)0)->EVENTS_COMPARE)

/** @brief Symbol for creating the interrupt bitmask for all compare channels. */
#define NRF_TIMER_ALL_CHANNELS_INT_MASK \
        ((uint32_t)((1 << NRF_TIMER_CC_COUNT_MAX) - 1) << TIMER_INTENSET_COMPARE0_Pos)

/** @brief Timer tasks. */
typedef enum
{
    NRF_TIMER_TASK_START    = offsetof(NRF_TIMER_Type, TASKS_START),      ///< Task for starting the timer.
    NRF_TIMER_TASK_STOP     = offsetof(NRF_TIMER_Type, TASKS_STOP),       ///< Task for stopping the timer.
    NRF_TIMER_TASK_COUNT    = offsetof(NRF_TIMER_Type, TASKS_COUNT),      ///< Task for incrementing the timer (in counter mode).
    NRF_TIMER_TASK_CLEAR    = offsetof(NRF_TIMER_Type, TASKS_CLEAR),      ///< Task for resetting the timer value.
#if NRF_TIMER_HAS_SHUTDOWN
    NRF_TIMER_TASK_SHUTDOWN = offsetof(NRF_TIMER_Type, TASKS_SHUTDOWN),   ///< Task for powering off the timer.
#endif
    NRF_TIMER_TASK_CAPTURE0 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[0]), ///< Task for capturing the timer value on channel 0.
    NRF_TIMER_TASK_CAPTURE1 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[1]), ///< Task for capturing the timer value on channel 1.
    NRF_TIMER_TASK_CAPTURE2 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[2]), ///< Task for capturing the timer value on channel 2.
    NRF_TIMER_TASK_CAPTURE3 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[3]), ///< Task for capturing the timer value on channel 3.
#if NRF_TIMER_HAS_CC4
    NRF_TIMER_TASK_CAPTURE4 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[4]), ///< Task for capturing the timer value on channel 4.
#endif
#if NRF_TIMER_HAS_CC5
    NRF_TIMER_TASK_CAPTURE5 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[5]), ///< Task for capturing the timer value on channel 5.
#endif
#if NRF_TIMER_HAS_CC6
    NRF_TIMER_TASK_CAPTURE6 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[6]), ///< Task for capturing the timer value on channel 6.
#endif
#if NRF_TIMER_HAS_CC7
    NRF_TIMER_TASK_CAPTURE7 = offsetof(NRF_TIMER_Type, TASKS_CAPTURE[7]), ///< Task for capturing the timer value on channel 7.
#endif
} nrf_timer_task_t;

/** @brief Timer events. */
typedef enum
{
    NRF_TIMER_EVENT_COMPARE0 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[0]), ///< Event from compare channel 0.
    NRF_TIMER_EVENT_COMPARE1 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[1]), ///< Event from compare channel 1.
    NRF_TIMER_EVENT_COMPARE2 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[2]), ///< Event from compare channel 2.
    NRF_TIMER_EVENT_COMPARE3 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[3]), ///< Event from compare channel 3.
#if NRF_TIMER_HAS_CC4
    NRF_TIMER_EVENT_COMPARE4 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[4]), ///< Event from compare channel 4.
#endif
#if NRF_TIMER_HAS_CC5
    NRF_TIMER_EVENT_COMPARE5 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[5]), ///< Event from compare channel 5.
#endif
#if NRF_TIMER_HAS_CC6
    NRF_TIMER_EVENT_COMPARE6 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[6]), ///< Event from compare channel 6.
#endif
#if NRF_TIMER_HAS_CC7
    NRF_TIMER_EVENT_COMPARE7 = offsetof(NRF_TIMER_Type, EVENTS_COMPARE[7]), ///< Event from compare channel 7.
#endif
} nrf_timer_event_t;

/** @brief Types of timer shortcuts. */
typedef enum
{
    NRF_TIMER_SHORT_COMPARE0_STOP_MASK = TIMER_SHORTS_COMPARE0_STOP_Msk,   ///< Shortcut for stopping the timer based on compare 0.
    NRF_TIMER_SHORT_COMPARE1_STOP_MASK = TIMER_SHORTS_COMPARE1_STOP_Msk,   ///< Shortcut for stopping the timer based on compare 1.
    NRF_TIMER_SHORT_COMPARE2_STOP_MASK = TIMER_SHORTS_COMPARE2_STOP_Msk,   ///< Shortcut for stopping the timer based on compare 2.
    NRF_TIMER_SHORT_COMPARE3_STOP_MASK = TIMER_SHORTS_COMPARE3_STOP_Msk,   ///< Shortcut for stopping the timer based on compare 3.
#if NRF_TIMER_HAS_CC4
    NRF_TIMER_SHORT_COMPARE4_STOP_MASK = TIMER_SHORTS_COMPARE4_STOP_Msk,   ///< Shortcut for stopping the timer based on compare 4.
#endif
#if NRF_TIMER_HAS_CC5
    NRF_TIMER_SHORT_COMPARE5_STOP_MASK = TIMER_SHORTS_COMPARE5_STOP_Msk,   ///< Shortcut for stopping the timer based on compare 5.
#endif
#if NRF_TIMER_HAS_CC6
    NRF_TIMER_SHORT_COMPARE6_STOP_MASK = TIMER_SHORTS_COMPARE6_STOP_Msk,   ///< Shortcut for stopping the timer based on compare 6.
#endif
#if NRF_TIMER_HAS_CC7
    NRF_TIMER_SHORT_COMPARE7_STOP_MASK = TIMER_SHORTS_COMPARE7_STOP_Msk,   ///< Shortcut for stopping the timer based on compare 7.
#endif
    NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK = TIMER_SHORTS_COMPARE0_CLEAR_Msk, ///< Shortcut for clearing the timer based on compare 0.
    NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK = TIMER_SHORTS_COMPARE1_CLEAR_Msk, ///< Shortcut for clearing the timer based on compare 1.
    NRF_TIMER_SHORT_COMPARE2_CLEAR_MASK = TIMER_SHORTS_COMPARE2_CLEAR_Msk, ///< Shortcut for clearing the timer based on compare 2.
    NRF_TIMER_SHORT_COMPARE3_CLEAR_MASK = TIMER_SHORTS_COMPARE3_CLEAR_Msk, ///< Shortcut for clearing the timer based on compare 3.
#if NRF_TIMER_HAS_CC4
    NRF_TIMER_SHORT_COMPARE4_CLEAR_MASK = TIMER_SHORTS_COMPARE4_CLEAR_Msk, ///< Shortcut for clearing the timer based on compare 4.
#endif
#if NRF_TIMER_HAS_CC5
    NRF_TIMER_SHORT_COMPARE5_CLEAR_MASK = TIMER_SHORTS_COMPARE5_CLEAR_Msk, ///< Shortcut for clearing the timer based on compare 5.
#endif
#if NRF_TIMER_HAS_CC6
    NRF_TIMER_SHORT_COMPARE6_CLEAR_MASK = TIMER_SHORTS_COMPARE6_CLEAR_Msk, ///< Shortcut for clearing the timer based on compare 6.
#endif
#if NRF_TIMER_HAS_CC7
    NRF_TIMER_SHORT_COMPARE7_CLEAR_MASK = TIMER_SHORTS_COMPARE7_CLEAR_Msk, ///< Shortcut for clearing the timer based on compare 7.
#endif
} nrf_timer_short_mask_t;

/** @brief Timer modes. */
typedef enum
{
    NRF_TIMER_MODE_TIMER             = TIMER_MODE_MODE_Timer,           ///< Timer mode: timer.
    NRF_TIMER_MODE_COUNTER           = TIMER_MODE_MODE_Counter,         ///< Timer mode: counter.
#if NRF_TIMER_HAS_LOW_POWER_MODE
    NRF_TIMER_MODE_LOW_POWER_COUNTER = TIMER_MODE_MODE_LowPowerCounter, ///< Timer mode: low-power counter.
#endif
} nrf_timer_mode_t;

/** @brief Timer bit width. */
typedef enum
{
    NRF_TIMER_BIT_WIDTH_8  = TIMER_BITMODE_BITMODE_08Bit, ///< Timer bit width 8 bit.
    NRF_TIMER_BIT_WIDTH_16 = TIMER_BITMODE_BITMODE_16Bit, ///< Timer bit width 16 bit.
    NRF_TIMER_BIT_WIDTH_24 = TIMER_BITMODE_BITMODE_24Bit, ///< Timer bit width 24 bit.
    NRF_TIMER_BIT_WIDTH_32 = TIMER_BITMODE_BITMODE_32Bit  ///< Timer bit width 32 bit.
} nrf_timer_bit_width_t;

/** @brief Timer prescalers. */
typedef enum
{
    NRF_TIMER_FREQ_16MHz = 0, ///< Timer frequency 16 MHz.
    NRF_TIMER_FREQ_8MHz,      ///< Timer frequency 8 MHz.
    NRF_TIMER_FREQ_4MHz,      ///< Timer frequency 4 MHz.
    NRF_TIMER_FREQ_2MHz,      ///< Timer frequency 2 MHz.
    NRF_TIMER_FREQ_1MHz,      ///< Timer frequency 1 MHz.
    NRF_TIMER_FREQ_500kHz,    ///< Timer frequency 500 kHz.
    NRF_TIMER_FREQ_250kHz,    ///< Timer frequency 250 kHz.
    NRF_TIMER_FREQ_125kHz,    ///< Timer frequency 125 kHz.
    NRF_TIMER_FREQ_62500Hz,   ///< Timer frequency 62500 Hz.
    NRF_TIMER_FREQ_31250Hz    ///< Timer frequency 31250 Hz.
} nrf_timer_frequency_t;

/** @brief Timer capture/compare channels. */
typedef enum
{
    NRF_TIMER_CC_CHANNEL0 = 0, ///< Timer capture/compare channel 0.
    NRF_TIMER_CC_CHANNEL1,     ///< Timer capture/compare channel 1.
    NRF_TIMER_CC_CHANNEL2,     ///< Timer capture/compare channel 2.
    NRF_TIMER_CC_CHANNEL3,     ///< Timer capture/compare channel 3.
#if NRF_TIMER_HAS_CC4
    NRF_TIMER_CC_CHANNEL4,     ///< Timer capture/compare channel 4.
#endif
#if NRF_TIMER_HAS_CC5
    NRF_TIMER_CC_CHANNEL5,     ///< Timer capture/compare channel 5.
#endif
#if NRF_TIMER_HAS_CC6
    NRF_TIMER_CC_CHANNEL6,     ///< Timer capture/compare channel 6.
#endif
#if NRF_TIMER_HAS_CC7
    NRF_TIMER_CC_CHANNEL7,     ///< Timer capture/compare channel 7.
#endif
} nrf_timer_cc_channel_t;

/** @brief Timer interrupts. */
typedef enum
{
    NRF_TIMER_INT_COMPARE0_MASK = TIMER_INTENSET_COMPARE0_Msk, ///< Timer interrupt from compare event on channel 0.
    NRF_TIMER_INT_COMPARE1_MASK = TIMER_INTENSET_COMPARE1_Msk, ///< Timer interrupt from compare event on channel 1.
    NRF_TIMER_INT_COMPARE2_MASK = TIMER_INTENSET_COMPARE2_Msk, ///< Timer interrupt from compare event on channel 2.
    NRF_TIMER_INT_COMPARE3_MASK = TIMER_INTENSET_COMPARE3_Msk, ///< Timer interrupt from compare event on channel 3.
#if NRF_TIMER_HAS_CC4
    NRF_TIMER_INT_COMPARE4_MASK = TIMER_INTENSET_COMPARE4_Msk, ///< Timer interrupt from compare event on channel 4.
#endif
#if NRF_TIMER_HAS_CC5
    NRF_TIMER_INT_COMPARE5_MASK = TIMER_INTENSET_COMPARE5_Msk, ///< Timer interrupt from compare event on channel 5.
#endif
#if NRF_TIMER_HAS_CC6
    NRF_TIMER_INT_COMPARE6_MASK = TIMER_INTENSET_COMPARE6_Msk, ///< Timer interrupt from compare event on channel 6.
#endif
#if NRF_TIMER_HAS_CC7
    NRF_TIMER_INT_COMPARE7_MASK = TIMER_INTENSET_COMPARE7_Msk, ///< Timer interrupt from compare event on channel 7.
#endif
} nrf_timer_int_mask_t;


/**
 * @brief Function for setting the prescaler factor.
 *
 * @note Prescaler value is expressed as \f$ 2^{prescaler\_factor} \f$.
 *
 * @param[in] p_reg            Pointer to the structure of registers of the peripheral.
 * @param[in] prescaler_factor Prescaler factor.
 */
NRF_STATIC_INLINE void nrf_timer_prescaler_set(NRF_TIMER_Type * p_reg, uint32_t prescaler_factor);

/**
 * @brief Function for retrieving the prescaler factor.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Prescaler factor.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_prescaler_get(NRF_TIMER_Type const * p_reg);

/**
 * @brief Function for activating the specified timer task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_timer_task_trigger(NRF_TIMER_Type * p_reg,
                                              nrf_timer_task_t task);

/**
 * @brief Function for getting the address of the specified timer task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  The specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_task_address_get(NRF_TIMER_Type const * p_reg,
                                                      nrf_timer_task_t       task);

/**
 * @brief Function for clearing the specified timer event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_timer_event_clear(NRF_TIMER_Type *  p_reg,
                                             nrf_timer_event_t event);

/**
 * @brief Function for retrieving the state of the TIMER event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_timer_event_check(NRF_TIMER_Type const * p_reg,
                                             nrf_timer_event_t      event);

/**
 * @brief Function for getting the address of the specified timer event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event The specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_event_address_get(NRF_TIMER_Type const * p_reg,
                                                       nrf_timer_event_t      event);

/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_timer_shorts_enable(NRF_TIMER_Type * p_reg,
                                               uint32_t         mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_timer_shorts_disable(NRF_TIMER_Type * p_reg,
                                                uint32_t         mask);

/**
 * @brief Function for setting the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be set.
 */
NRF_STATIC_INLINE void nrf_timer_shorts_set(NRF_TIMER_Type * p_reg,
                                            uint32_t         mask);

/**
 * @brief Function for getting COMPARE_CLEAR short mask for the specified channel.
 *
 * @param[in] channel Channel.
 *
 * @return Short mask.
 */
NRF_STATIC_INLINE nrf_timer_short_mask_t nrf_timer_short_compare_clear_get(uint8_t channel);

/**
 * @brief Function for getting COMPARE_STOP short mask for the specified channel.
 *
 * @param[in] channel Channel.
 *
 * @return Short mask.
 */
NRF_STATIC_INLINE nrf_timer_short_mask_t nrf_timer_short_compare_stop_get(uint8_t channel);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_timer_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_timer_int_enable(NRF_TIMER_Type * p_reg,
                                            uint32_t         mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_timer_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_timer_int_disable(NRF_TIMER_Type * p_reg,
                                             uint32_t         mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_timer_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_int_enable_check(NRF_TIMER_Type const * p_reg, uint32_t mask);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        TIMER task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_timer_subscribe_set(NRF_TIMER_Type * p_reg,
                                               nrf_timer_task_t task,
                                               uint8_t          channel);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        TIMER task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return TIMER subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_subscribe_get(NRF_TIMER_Type const * p_reg,
                                                   nrf_timer_task_t       task);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        TIMER task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_timer_subscribe_clear(NRF_TIMER_Type * p_reg,
                                                 nrf_timer_task_t task);

/**
 * @brief Function for setting the publish configuration for a given
 *        TIMER event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_timer_publish_set(NRF_TIMER_Type *  p_reg,
                                             nrf_timer_event_t event,
                                             uint8_t           channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        TIMER event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_timer_publish_clear(NRF_TIMER_Type *  p_reg,
                                               nrf_timer_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        TIMER event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return TIMER publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_publish_get(NRF_TIMER_Type const * p_reg,
                                                 nrf_timer_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for setting the timer mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Timer mode.
 */
NRF_STATIC_INLINE void nrf_timer_mode_set(NRF_TIMER_Type * p_reg,
                                          nrf_timer_mode_t mode);

/**
 * @brief Function for retrieving the timer mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Timer mode.
 */
NRF_STATIC_INLINE nrf_timer_mode_t nrf_timer_mode_get(NRF_TIMER_Type const * p_reg);

/**
 * @brief Function for setting the timer bit width.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] bit_width Timer bit width.
 */
NRF_STATIC_INLINE void nrf_timer_bit_width_set(NRF_TIMER_Type *      p_reg,
                                               nrf_timer_bit_width_t bit_width);

/**
 * @brief Function for retrieving the timer bit width.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Timer bit width.
 */
NRF_STATIC_INLINE nrf_timer_bit_width_t nrf_timer_bit_width_get(NRF_TIMER_Type const * p_reg);

/**
 * @brief Function for setting the capture/compare register for the specified channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel The specified capture/compare channel.
 * @param[in] cc_value   Value to write to the capture/compare register.
 */
NRF_STATIC_INLINE void nrf_timer_cc_set(NRF_TIMER_Type *       p_reg,
                                        nrf_timer_cc_channel_t cc_channel,
                                        uint32_t               cc_value);

/**
 * @brief Function for retrieving the capture/compare value for a specified channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel The specified capture/compare channel.
 *
 * @return Value from the specified capture/compare register.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_cc_get(NRF_TIMER_Type const * p_reg,
                                            nrf_timer_cc_channel_t cc_channel);

/**
 * @brief Function for getting the specified timer capture task.
 *
 * @param[in] channel Capture channel.
 *
 * @return Capture task.
 */
NRF_STATIC_INLINE nrf_timer_task_t nrf_timer_capture_task_get(uint8_t channel);

/**
 * @brief Function for getting the specified timer compare event.
 *
 * @param[in] channel Compare channel.
 *
 * @return Compare event.
 */
NRF_STATIC_INLINE nrf_timer_event_t nrf_timer_compare_event_get(uint8_t channel);

/**
 * @brief Function for getting the specified timer compare interrupt.
 *
 * @param[in] channel Compare channel.
 *
 * @return Compare interrupt.
 */
NRF_STATIC_INLINE nrf_timer_int_mask_t nrf_timer_compare_int_get(uint8_t channel);

/**
 * @brief Function for calculating the number of timer ticks for a given time
 *        (in microseconds) and timer frequency.
 *
 * @param[in] time_us   Time in microseconds.
 * @param[in] frequency Timer frequency.
 *
 * @return Number of timer ticks.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_us_to_ticks(uint32_t              time_us,
                                                 nrf_timer_frequency_t frequency);

/**
 * @brief Function for calculating the number of timer ticks for a given time
 *        (in milliseconds) and timer frequency.
 *
 * @param[in] time_ms   Time in milliseconds.
 * @param[in] frequency Timer frequency.
 *
 * @return Number of timer ticks.
 */
NRF_STATIC_INLINE uint32_t nrf_timer_ms_to_ticks(uint32_t              time_ms,
                                                 nrf_timer_frequency_t frequency);

#if NRF_TIMER_HAS_ONE_SHOT
/**
 * @brief Function for enabling one-shot operation for the specified capture/compare channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Capture/compare channel.
 */
NRF_STATIC_INLINE void nrf_timer_one_shot_enable(NRF_TIMER_Type *       p_reg,
                                                 nrf_timer_cc_channel_t cc_channel);

/**
 * @brief Function for disabling one-shot operation for the specified capture/compare channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Capture/compare channel.
 */
NRF_STATIC_INLINE void nrf_timer_one_shot_disable(NRF_TIMER_Type *       p_reg,
                                                  nrf_timer_cc_channel_t cc_channel);

#endif // NRF_TIMER_HAS_ONE_SHOT

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_timer_task_trigger(NRF_TIMER_Type * p_reg,
                                              nrf_timer_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_timer_task_address_get(NRF_TIMER_Type const * p_reg,
                                                      nrf_timer_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_timer_event_clear(NRF_TIMER_Type *  p_reg,
                                             nrf_timer_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_timer_event_check(NRF_TIMER_Type const * p_reg,
                                             nrf_timer_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_timer_event_address_get(NRF_TIMER_Type const * p_reg,
                                                       nrf_timer_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_timer_shorts_enable(NRF_TIMER_Type * p_reg,
                                               uint32_t         mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_timer_shorts_disable(NRF_TIMER_Type * p_reg,
                                                uint32_t         mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE void nrf_timer_shorts_set(NRF_TIMER_Type * p_reg,
                                            uint32_t         mask)
{
    p_reg->SHORTS = mask;
}

NRF_STATIC_INLINE nrf_timer_short_mask_t nrf_timer_short_compare_clear_get(uint8_t channel)
{
    return (nrf_timer_short_mask_t)((uint32_t)NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK << channel);
}

NRF_STATIC_INLINE nrf_timer_short_mask_t nrf_timer_short_compare_stop_get(uint8_t channel)
{
    return (nrf_timer_short_mask_t)((uint32_t)NRF_TIMER_SHORT_COMPARE0_STOP_MASK << channel);
}

NRF_STATIC_INLINE void nrf_timer_int_enable(NRF_TIMER_Type * p_reg,
                                            uint32_t         mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_timer_int_disable(NRF_TIMER_Type * p_reg,
                                             uint32_t         mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_timer_int_enable_check(NRF_TIMER_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_timer_subscribe_set(NRF_TIMER_Type * p_reg,
                                               nrf_timer_task_t task,
                                               uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_timer_subscribe_clear(NRF_TIMER_Type * p_reg,
                                                 nrf_timer_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_timer_subscribe_get(NRF_TIMER_Type const * p_reg,
                                                   nrf_timer_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}


NRF_STATIC_INLINE void nrf_timer_publish_set(NRF_TIMER_Type *  p_reg,
                                             nrf_timer_event_t event,
                                             uint8_t           channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_timer_publish_clear(NRF_TIMER_Type *  p_reg,
                                               nrf_timer_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_timer_publish_get(NRF_TIMER_Type const * p_reg,
                                                 nrf_timer_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE void nrf_timer_mode_set(NRF_TIMER_Type * p_reg,
                                          nrf_timer_mode_t mode)
{
    p_reg->MODE = (p_reg->MODE & ~TIMER_MODE_MODE_Msk) |
                    ((mode << TIMER_MODE_MODE_Pos) & TIMER_MODE_MODE_Msk);
}

NRF_STATIC_INLINE nrf_timer_mode_t nrf_timer_mode_get(NRF_TIMER_Type const * p_reg)
{
    return (nrf_timer_mode_t)(p_reg->MODE);
}

NRF_STATIC_INLINE void nrf_timer_bit_width_set(NRF_TIMER_Type *      p_reg,
                                               nrf_timer_bit_width_t bit_width)
{
    p_reg->BITMODE = (p_reg->BITMODE & ~TIMER_BITMODE_BITMODE_Msk) |
                       ((bit_width << TIMER_BITMODE_BITMODE_Pos) &
                            TIMER_BITMODE_BITMODE_Msk);
}

NRF_STATIC_INLINE nrf_timer_bit_width_t nrf_timer_bit_width_get(NRF_TIMER_Type const * p_reg)
{
    return (nrf_timer_bit_width_t)(p_reg->BITMODE);
}

NRF_STATIC_INLINE void nrf_timer_prescaler_set(NRF_TIMER_Type * p_reg, uint32_t prescaler_factor)
{
    NRFX_ASSERT(prescaler_factor <= NRF_TIMER_PRESCALER_MAX);
    p_reg->PRESCALER = prescaler_factor;
}

NRF_STATIC_INLINE uint32_t nrf_timer_prescaler_get(NRF_TIMER_Type const * p_reg)
{
    return p_reg->PRESCALER;
}

NRF_STATIC_INLINE void nrf_timer_cc_set(NRF_TIMER_Type *       p_reg,
                                        nrf_timer_cc_channel_t cc_channel,
                                        uint32_t               cc_value)
{
    p_reg->CC[cc_channel] = cc_value;
}

NRF_STATIC_INLINE uint32_t nrf_timer_cc_get(NRF_TIMER_Type const * p_reg,
                                            nrf_timer_cc_channel_t cc_channel)
{
    return (uint32_t)p_reg->CC[cc_channel];
}

NRF_STATIC_INLINE nrf_timer_task_t nrf_timer_capture_task_get(uint8_t channel)
{
    return (nrf_timer_task_t)NRFX_OFFSETOF(NRF_TIMER_Type, TASKS_CAPTURE[channel]);
}

NRF_STATIC_INLINE nrf_timer_event_t nrf_timer_compare_event_get(uint8_t channel)
{
    return (nrf_timer_event_t)NRFX_OFFSETOF(NRF_TIMER_Type, EVENTS_COMPARE[channel]);
}

NRF_STATIC_INLINE nrf_timer_int_mask_t nrf_timer_compare_int_get(uint8_t channel)
{
    return (nrf_timer_int_mask_t)
        ((uint32_t)NRF_TIMER_INT_COMPARE0_MASK << channel);
}

NRF_STATIC_INLINE uint32_t nrf_timer_us_to_ticks(uint32_t              time_us,
                                                 nrf_timer_frequency_t frequency)
{
    // The "frequency" parameter here is actually the prescaler value, and the
    // timer runs at the following frequency: f = 16 MHz / 2^prescaler.
    uint32_t prescaler = (uint32_t)frequency;
    uint64_t ticks = ((time_us * 16ULL) >> prescaler);
    NRFX_ASSERT(ticks <= UINT32_MAX);
    return (uint32_t)ticks;
}

NRF_STATIC_INLINE uint32_t nrf_timer_ms_to_ticks(uint32_t              time_ms,
                                                 nrf_timer_frequency_t frequency)
{
    // The "frequency" parameter here is actually the prescaler value, and the
    // timer runs at the following frequency: f = 16000 kHz / 2^prescaler.
    uint32_t prescaler = (uint32_t)frequency;
    uint64_t ticks = ((time_ms * 16000ULL) >> prescaler);
    NRFX_ASSERT(ticks <= UINT32_MAX);
    return (uint32_t)ticks;
}

#if NRF_TIMER_HAS_ONE_SHOT
NRF_STATIC_INLINE void nrf_timer_one_shot_enable(NRF_TIMER_Type *       p_reg,
                                                 nrf_timer_cc_channel_t cc_channel)
{
    p_reg->ONESHOTEN[cc_channel] = TIMER_ONESHOTEN_ONESHOTEN_Msk;
}

NRF_STATIC_INLINE void nrf_timer_one_shot_disable(NRF_TIMER_Type *       p_reg,
                                                  nrf_timer_cc_channel_t cc_channel)
{
    p_reg->ONESHOTEN[cc_channel] = 0;
}
#endif // NRF_TIMER_HAS_ONE_SHOT

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_TIMER_H__
