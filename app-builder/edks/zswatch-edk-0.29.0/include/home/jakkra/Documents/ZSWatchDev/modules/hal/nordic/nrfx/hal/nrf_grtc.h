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

#ifndef NRF_GRTC_H
#define NRF_GRTC_H

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_grtc_hal GRTC HAL
 * @{
 * @ingroup nrf_grtc
 * @brief   Hardware access layer for managing the Global Real Time Counter (GRTC) peripheral.
 */

#if NRFX_CHECK(GRTC_PWMREGS) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GRTC has PWM registers. */
#define NRF_GRTC_HAS_PWM 1
#else
#define NRF_GRTC_HAS_PWM 0
#endif

#if NRFX_CHECK(GRTC_CLKOUTREG) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GRTC has clock output registers. */
#define NRF_GRTC_HAS_CLKOUT 1
#else
#define NRF_GRTC_HAS_CLKOUT 0
#endif

#if defined(GRTC_CLKCFG_CLKSEL_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GRTC has clock source selection. */
#define NRF_GRTC_HAS_CLKSEL 1
#else
#define NRF_GRTC_HAS_CLKSEL 0
#endif

#if defined(GRTC_CLKCFG_CLKSEL_LFLPRC) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RC oscillator clock source is available. */
#define NRF_GRTC_HAS_CLKSEL_LFLPRC 1
#else
#define NRF_GRTC_HAS_CLKSEL_LFLPRC 0
#endif

#if defined(GRTC_CLKCFG_CLKSEL_LFXO) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether crystal oscillator clock source is available. */
#define NRF_GRTC_HAS_CLKSEL_LFXO 1
#else
#define NRF_GRTC_HAS_CLKSEL_LFXO 0
#endif

#if defined(GRTC_SYSCOUNTER_SYSCOUNTERL_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GRTC has multiple SYSCOUNTER registers. */
#define NRF_GRTC_HAS_SYSCOUNTER_ARRAY 1
#else
#define NRF_GRTC_HAS_SYSCOUNTER_ARRAY 0
#endif

#if defined(GRTC_EVENTS_SYSCOUNTERVALID_EVENTS_SYSCOUNTERVALID_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SYSCOUNTERVALID event is present. */
#define NRF_GRTC_HAS_SYSCOUNTERVALID 1
#else
#define NRF_GRTC_HAS_SYSCOUNTERVALID 0
#endif

#if defined(GRTC_SYSCOUNTER_SYSCOUNTERH_LOADED_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SYSCOUNTERH LOADED bit is present. */
#define NRF_GRTC_HAS_SYSCOUNTER_LOADED 1
#else
#define NRF_GRTC_HAS_SYSCOUNTER_LOADED 0
#endif

#if defined(GRTC_KEEPRUNNING_DOMAIN0_Msk) || defined(GRTC_KEEPRUNNING_REQUEST0_Msk) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether KEEPRUNNING register is present. */
#define NRF_GRTC_HAS_KEEPRUNNING 1
#else
#define NRF_GRTC_HAS_KEEPRUNNING 0
#endif

#if defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GRTC has RTCOUNTER. */
#define NRF_GRTC_HAS_RTCOUNTER 1
#elif !defined(NRF_GRTC_HAS_RTCOUNTER)
#define NRF_GRTC_HAS_RTCOUNTER 0
#endif

#if !defined(NRF_GRTC_HAS_EXTENDED)
#if defined(GRTC_FORCE_EXTENDED) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GRTC has extended functionality. */
#define NRF_GRTC_HAS_EXTENDED 1
#else
#define NRF_GRTC_HAS_EXTENDED 0
#endif
#endif // !defined(NRF_GRTC_HAS_EXTENDED)

#if defined(GRTC_IRQ_GROUP) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GRTC interrupt groups are present. */
#define NRF_GRTC_HAS_INT_GROUPS 1
#else
#define NRF_GRTC_HAS_INT_GROUPS 0
#endif

/** @brief Symbol indicating actual domain index. */
#define NRF_GRTC_DOMAIN_INDEX GRTC_IRQ_GROUP

/** @brief Symbol indicating actual SYSCOUNTER index. */
#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    #define GRTC_SYSCOUNTER SYSCOUNTER[NRF_GRTC_DOMAIN_INDEX]
#endif

/** @brief Number of capture/compare channels for SYSCOUNTER. */
#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    #define NRF_GRTC_SYSCOUNTER_COUNT GRTC_SYSCOUNTER_MaxCount
#endif

/** @brief Interrupts INTEN register definition. */
#define GRTC_INTEN        NRFX_CONCAT_2(INTEN, GRTC_IRQ_GROUP)
/** @brief Interrupts INTENSET register definition. */
#define GRTC_INTENSET     NRFX_CONCAT_2(INTENSET, GRTC_IRQ_GROUP)
/** @brief Interrupts INTENCLR register definition. */
#define GRTC_INTENCLR     NRFX_CONCAT_2(INTENCLR, GRTC_IRQ_GROUP)
/** @brief Interrupts INTPEND register definition. */
#define GRTC_INTPEND      NRFX_CONCAT_2(INTPEND, GRTC_IRQ_GROUP)

/** @brief Main SYSCOUNTER frequency in Hz. */
#define NRF_GRTC_SYSCOUNTER_MAIN_FREQUENCY_HZ 1000000UL

/** @brief Number of capture/compare channels for SYSCOUNTER. */
#define NRF_GRTC_SYSCOUNTER_CC_COUNT GRTC_CC_MaxCount

/** @brief Bitmask of the higher 32-bits of capture/compare register for the SYSCOUNTER. */
#define NRF_GRTC_SYSCOUNTER_CCH_MASK GRTC_CC_CCH_CCH_Msk

/** @brief Bitmask of CCADD register for the SYSCOUNTER. */
#define NRF_GRTC_SYSCOUNTER_CCADD_MASK GRTC_CC_CCADD_VALUE_Msk

#if defined(GRTC_SYSCOUNTERL_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Bitmask of the lower part of the SYSCOUNTER value. */
#define NRF_GRTC_SYSCOUNTERL_VALUE_MASK GRTC_SYSCOUNTERL_VALUE_Msk
#else
#define NRF_GRTC_SYSCOUNTERL_VALUE_MASK GRTC_SYSCOUNTER_SYSCOUNTERL_VALUE_Msk
#endif

#if defined(GRTC_SYSCOUNTERH_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Bitmask of the higher part of the SYSCOUNTER value. */
#define NRF_GRTC_SYSCOUNTERH_VALUE_MASK GRTC_SYSCOUNTERH_VALUE_Msk
#else
#define NRF_GRTC_SYSCOUNTERH_VALUE_MASK GRTC_SYSCOUNTER_SYSCOUNTERH_VALUE_Msk
#endif

/** @brief Bitmask of the higher 32-bits of capture/compare register for the RTCOUNTER. */
#define NRF_GRTC_RTCOUNTER_CCH_MASK GRTC_RTCOMPAREH_VALUE_Msk

#if defined(GRTC_SYSCOUNTERH_OVERFLOW_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Bitmask of the OVERFLOW bit. */
#define NRF_GRTC_SYSCOUNTERH_OVERFLOW_MASK GRTC_SYSCOUNTERH_OVERFLOW_Msk
#else
#define NRF_GRTC_SYSCOUNTERH_OVERFLOW_MASK GRTC_SYSCOUNTER_SYSCOUNTERH_OVERFLOW_Msk
#endif

#if defined(GRTC_SYSCOUNTERH_BUSY_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Bitmask of the BUSY bit. */
#define NRF_GRTC_SYSCOUNTERH_BUSY_MASK GRTC_SYSCOUNTERH_BUSY_Msk
#else
#define NRF_GRTC_SYSCOUNTERH_BUSY_MASK GRTC_SYSCOUNTER_SYSCOUNTERH_BUSY_Msk
#endif

/** @brief Maximum value of TIMEOUT register content. */
#define NRF_GRTC_TIMEOUT_MAX_VALUE (GRTC_TIMEOUT_VALUE_Msk >> GRTC_TIMEOUT_VALUE_Pos)

/** @brief Maximum value of WAKETIME register content. */
#define NRF_GRTC_WAKETIME_MAX_VALUE (GRTC_WAKETIME_VALUE_Msk >> GRTC_WAKETIME_VALUE_Pos)

/** @brief Maximum value of CLKFASTDIV register content. */
#define NRF_GRTC_CLKCFG_CLKFASTDIV_MAX_VALUE GRTC_CLKCFG_CLKFASTDIV_Max

/** @brief Macro for creating the interrupt bitmask for the specified compare channel. */
#define NRF_GRTC_CHANNEL_INT_MASK(ch) ((uint32_t)(NRF_GRTC_INT_COMPARE0_MASK) << (ch))

/** @brief Main channel that can be used only by the owner of GRTC. */
#define NRF_GRTC_MAIN_CC_CHANNEL GRTC_MAIN_CC_CHANNEL

/** @brief Bitmask of interrupt enable. */
#define NRF_GRTC_INTEN_MASK NRFX_BIT_MASK(GRTC_CC_MaxCount)

/** @brief Mask for all channels represented by CC channels. */
#define NRF_GRTC_SYSCOUNTER_ALL_CHANNELS_INT_MASK \
    ((uint32_t)(((1 << NRF_GRTC_SYSCOUNTER_CC_COUNT) - 1) << GRTC_INTEN0_COMPARE0_Pos))

/** @brief GRTC tasks. */
typedef enum
{
#if NRF_GRTC_HAS_EXTENDED
    NRF_GRTC_TASK_START      = offsetof(NRF_GRTC_Type, TASKS_START),       /**< Start. */
    NRF_GRTC_TASK_STOP       = offsetof(NRF_GRTC_Type, TASKS_STOP),        /**< Stop. */
    NRF_GRTC_TASK_CLEAR      = offsetof(NRF_GRTC_Type, TASKS_CLEAR),       /**< Clear. */
#endif
#if NRF_GRTC_HAS_PWM
    NRF_GRTC_TASK_PWM_START  = offsetof(NRF_GRTC_Type, TASKS_PWMSTART),    /**< Start the PWM. */
    NRF_GRTC_TASK_PWM_STOP   = offsetof(NRF_GRTC_Type, TASKS_PWMSTOP),     /**< Stop the PWM. */
#endif // NRF_GRTC_HAS_PWM
    NRF_GRTC_TASK_CAPTURE_0  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[0]),  /**< Capture the counter value on channel 0. */
    NRF_GRTC_TASK_CAPTURE_1  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[1]),  /**< Capture the counter value on channel 1. */
    NRF_GRTC_TASK_CAPTURE_2  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[2]),  /**< Capture the counter value on channel 2. */
    NRF_GRTC_TASK_CAPTURE_3  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[3]),  /**< Capture the counter value on channel 3. */
    NRF_GRTC_TASK_CAPTURE_4  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[4]),  /**< Capture the counter value on channel 4. */
    NRF_GRTC_TASK_CAPTURE_5  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[5]),  /**< Capture the counter value on channel 5. */
    NRF_GRTC_TASK_CAPTURE_6  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[6]),  /**< Capture the counter value on channel 6. */
    NRF_GRTC_TASK_CAPTURE_7  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[7]),  /**< Capture the counter value on channel 7. */
    NRF_GRTC_TASK_CAPTURE_8  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[8]),  /**< Capture the counter value on channel 8. */
    NRF_GRTC_TASK_CAPTURE_9  = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[9]),  /**< Capture the counter value on channel 9. */
    NRF_GRTC_TASK_CAPTURE_10 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[10]), /**< Capture the counter value on channel 10. */
    NRF_GRTC_TASK_CAPTURE_11 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[11]), /**< Capture the counter value on channel 11. */
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 12
    NRF_GRTC_TASK_CAPTURE_12 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[12]), /**< Capture the counter value on channel 12. */
    NRF_GRTC_TASK_CAPTURE_13 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[13]), /**< Capture the counter value on channel 13. */
    NRF_GRTC_TASK_CAPTURE_14 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[14]), /**< Capture the counter value on channel 14. */
    NRF_GRTC_TASK_CAPTURE_15 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[15]), /**< Capture the counter value on channel 15. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 16
    NRF_GRTC_TASK_CAPTURE_16 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[16]), /**< Capture the counter value on channel 16. */
    NRF_GRTC_TASK_CAPTURE_17 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[17]), /**< Capture the counter value on channel 17. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 18
    NRF_GRTC_TASK_CAPTURE_18 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[18]), /**< Capture the counter value on channel 18. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 19
    NRF_GRTC_TASK_CAPTURE_19 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[19]), /**< Capture the counter value on channel 19. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 20
    NRF_GRTC_TASK_CAPTURE_20 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[20]), /**< Capture the counter value on channel 20. */
    NRF_GRTC_TASK_CAPTURE_21 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[21]), /**< Capture the counter value on channel 21. */
    NRF_GRTC_TASK_CAPTURE_22 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[22]), /**< Capture the counter value on channel 22. */
    NRF_GRTC_TASK_CAPTURE_23 = offsetof(NRF_GRTC_Type, TASKS_CAPTURE[23]), /**< Capture the counter value on channel 23. */
#endif
} nrf_grtc_task_t;

/** @brief GRTC events. */
typedef enum
{
    NRF_GRTC_EVENT_COMPARE_0       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[0]),      /**< Compare 0 event. */
    NRF_GRTC_EVENT_COMPARE_1       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[1]),      /**< Compare 1 event. */
    NRF_GRTC_EVENT_COMPARE_2       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[2]),      /**< Compare 2 event. */
    NRF_GRTC_EVENT_COMPARE_3       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[3]),      /**< Compare 3 event. */
    NRF_GRTC_EVENT_COMPARE_4       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[4]),      /**< Compare 4 event. */
    NRF_GRTC_EVENT_COMPARE_5       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[5]),      /**< Compare 5 event. */
    NRF_GRTC_EVENT_COMPARE_6       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[6]),      /**< Compare 6 event. */
    NRF_GRTC_EVENT_COMPARE_7       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[7]),      /**< Compare 7 event. */
    NRF_GRTC_EVENT_COMPARE_8       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[8]),      /**< Compare 8 event. */
    NRF_GRTC_EVENT_COMPARE_9       = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[9]),      /**< Compare 9 event. */
    NRF_GRTC_EVENT_COMPARE_10      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[10]),     /**< Compare 10 event. */
    NRF_GRTC_EVENT_COMPARE_11      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[11]),     /**< Compare 11 event. */
    NRF_GRTC_EVENT_COMPARE_12      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[12]),     /**< Compare 12 event. */
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 12
    NRF_GRTC_EVENT_COMPARE_13      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[13]),     /**< Compare 13 event. */
    NRF_GRTC_EVENT_COMPARE_14      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[14]),     /**< Compare 14 event. */
    NRF_GRTC_EVENT_COMPARE_15      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[15]),     /**< Compare 15 event. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 16
    NRF_GRTC_EVENT_COMPARE_16      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[16]),     /**< Compare 16 event. */
    NRF_GRTC_EVENT_COMPARE_17      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[17]),     /**< Compare 17 event. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 18
    NRF_GRTC_EVENT_COMPARE_18      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[18]),     /**< Compare 18 event. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 19
    NRF_GRTC_EVENT_COMPARE_19      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[19]),     /**< Compare 19 event. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 20
    NRF_GRTC_EVENT_COMPARE_20      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[20]),     /**< Compare 20 event. */
    NRF_GRTC_EVENT_COMPARE_21      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[21]),     /**< Compare 21 event. */
    NRF_GRTC_EVENT_COMPARE_22      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[22]),     /**< Compare 22 event. */
    NRF_GRTC_EVENT_COMPARE_23      = offsetof(NRF_GRTC_Type, EVENTS_COMPARE[23]),     /**< Compare 23 event. */
#endif
#if NRF_GRTC_HAS_RTCOUNTER
    NRF_GRTC_EVENT_RTCOMPARE       = offsetof(NRF_GRTC_Type, EVENTS_RTCOMPARE),       /**< RTCOUNTER compare event. */
    NRF_GRTC_EVENT_RTCOMPARESYNC   = offsetof(NRF_GRTC_Type, EVENTS_RTCOMPARESYNC),   /**< RTCOUNTER synchronized compare event. */
#endif
#if NRF_GRTC_HAS_SYSCOUNTERVALID
    NRF_GRTC_EVENT_SYSCOUNTERVALID = offsetof(NRF_GRTC_Type, EVENTS_SYSCOUNTERVALID), /**< SYSCOUNTER value valid event. */
#endif
#if NRF_GRTC_HAS_PWM
    NRF_GRTC_EVENT_PWM_PERIOD_END  = offsetof(NRF_GRTC_Type, EVENTS_PWMPERIODEND),    /**< End of PWM period event. */
#endif // NRF_GRTC_HAS_PWM
} nrf_grtc_event_t;

#if NRF_GRTC_HAS_RTCOUNTER
/** @brief Types of GRTC shortcuts. */
typedef enum
{
    NRF_GRTC_SHORT_RTCOMPARE_CLEAR_MASK = GRTC_SHORTS_RTCOMPARE_CLEAR_Msk, /**< Shortcut between RTCOMPARE event and CLEAR task. */
} nrf_grtc_short_mask_t;
#endif

/** @brief Types of GRTC CC references. */
typedef enum
{
    NRF_GRTC_CC_ADD_REFERENCE_SYSCOUNTER = GRTC_CC_CCADD_REFERENCE_SYSCOUNTER, /**< The SYSCOUNTER register's content will be used as the reference. */
    NRF_GRTC_CC_ADD_REFERENCE_CC         = GRTC_CC_CCADD_REFERENCE_CC          /**< The CC[n] register's content will be used as the reference. */
} nrf_grtc_cc_add_reference_t;

/** @brief GRTC interrupts. */
typedef enum
{
    NRF_GRTC_INT_COMPARE0_MASK        = GRTC_INTENSET0_COMPARE0_Msk,        /**< GRTC interrupt from compare event on channel 0. */
    NRF_GRTC_INT_COMPARE1_MASK        = GRTC_INTENSET0_COMPARE1_Msk,        /**< GRTC interrupt from compare event on channel 1. */
    NRF_GRTC_INT_COMPARE2_MASK        = GRTC_INTENSET0_COMPARE2_Msk,        /**< GRTC interrupt from compare event on channel 2. */
    NRF_GRTC_INT_COMPARE3_MASK        = GRTC_INTENSET0_COMPARE3_Msk,        /**< GRTC interrupt from compare event on channel 3. */
    NRF_GRTC_INT_COMPARE4_MASK        = GRTC_INTENSET0_COMPARE4_Msk,        /**< GRTC interrupt from compare event on channel 4. */
    NRF_GRTC_INT_COMPARE5_MASK        = GRTC_INTENSET0_COMPARE5_Msk,        /**< GRTC interrupt from compare event on channel 5. */
    NRF_GRTC_INT_COMPARE6_MASK        = GRTC_INTENSET0_COMPARE6_Msk,        /**< GRTC interrupt from compare event on channel 6. */
    NRF_GRTC_INT_COMPARE7_MASK        = GRTC_INTENSET0_COMPARE7_Msk,        /**< GRTC interrupt from compare event on channel 7. */
    NRF_GRTC_INT_COMPARE8_MASK        = GRTC_INTENSET0_COMPARE8_Msk,        /**< GRTC interrupt from compare event on channel 8. */
    NRF_GRTC_INT_COMPARE9_MASK        = GRTC_INTENSET0_COMPARE9_Msk,        /**< GRTC interrupt from compare event on channel 9. */
    NRF_GRTC_INT_COMPARE10_MASK       = GRTC_INTENSET0_COMPARE10_Msk,       /**< GRTC interrupt from compare event on channel 10. */
    NRF_GRTC_INT_COMPARE11_MASK       = GRTC_INTENSET0_COMPARE11_Msk,       /**< GRTC interrupt from compare event on channel 11. */
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 12
    NRF_GRTC_INT_COMPARE12_MASK       = GRTC_INTENSET0_COMPARE12_Msk,       /**< GRTC interrupt from compare event on channel 12. */
    NRF_GRTC_INT_COMPARE13_MASK       = GRTC_INTENSET0_COMPARE13_Msk,       /**< GRTC interrupt from compare event on channel 13. */
    NRF_GRTC_INT_COMPARE14_MASK       = GRTC_INTENSET0_COMPARE14_Msk,       /**< GRTC interrupt from compare event on channel 14. */
    NRF_GRTC_INT_COMPARE15_MASK       = GRTC_INTENSET0_COMPARE15_Msk,       /**< GRTC interrupt from compare event on channel 15. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 16
    NRF_GRTC_INT_COMPARE16_MASK       = GRTC_INTENSET0_COMPARE16_Msk,       /**< GRTC interrupt from compare event on channel 16. */
    NRF_GRTC_INT_COMPARE17_MASK       = GRTC_INTENSET0_COMPARE17_Msk,       /**< GRTC interrupt from compare event on channel 17. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 18
    NRF_GRTC_INT_COMPARE18_MASK       = GRTC_INTENSET0_COMPARE18_Msk,       /**< GRTC interrupt from compare event on channel 18. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 19
    NRF_GRTC_INT_COMPARE19_MASK       = GRTC_INTENSET0_COMPARE19_Msk,       /**< GRTC interrupt from compare event on channel 19. */
#endif
#if NRF_GRTC_SYSCOUNTER_CC_COUNT > 20
    NRF_GRTC_INT_COMPARE20_MASK       = GRTC_INTENSET0_COMPARE20_Msk,       /**< GRTC interrupt from compare event on channel 20. */
    NRF_GRTC_INT_COMPARE21_MASK       = GRTC_INTENSET0_COMPARE21_Msk,       /**< GRTC interrupt from compare event on channel 21. */
    NRF_GRTC_INT_COMPARE22_MASK       = GRTC_INTENSET0_COMPARE22_Msk,       /**< GRTC interrupt from compare event on channel 22. */
    NRF_GRTC_INT_COMPARE23_MASK       = GRTC_INTENSET0_COMPARE23_Msk,       /**< GRTC interrupt from compare event on channel 23. */
#endif
#if NRF_GRTC_HAS_RTCOUNTER
    NRF_GRTC_INT_RTCOMPARE_MASK       = GRTC_INTENSET0_RTCOMPARE_Msk,       /**< GRTC interrupt from RTCOUNTER compare event. */
    NRF_GRTC_INT_RTCOMPARESYNC_MASK   = GRTC_INTENSET0_RTCOMPARESYNC_Msk,   /**< GRTC interrupt from RTCOUNTER synchronized compare event. */
#endif
#if NRF_GRTC_HAS_EXTENDED && NRF_GRTC_HAS_SYSCOUNTERVALID
    NRF_GRTC_INT_SYSCOUNTERVALID_MASK = GRTC_INTENSET0_SYSCOUNTERVALID_Msk, /**< GRTC interrupt from SYSCOUNTER valid event. */
#endif
} nrf_grtc_int_mask_t;

#if NRF_GRTC_HAS_CLKOUT
/** @brief Configuration of clock output. */
typedef enum
{
    NRF_GRTC_CLKOUT_32K  = GRTC_CLKOUT_CLKOUT32K_Msk,  /**< Enable 32K clock output on pin. */
    NRF_GRTC_CLKOUT_FAST = GRTC_CLKOUT_CLKOUTFAST_Msk, /**< Enable fast clock output on pin. */
} nrf_grtc_clkout_t;
#endif

#if NRF_GRTC_HAS_CLKSEL
/** @brief Configuration of the GRTC clock source selection. */
typedef enum
{
#if NRF_GRTC_HAS_CLKSEL_LFXO
    NRF_GRTC_CLKSEL_LFXO  = GRTC_CLKCFG_CLKSEL_LFXO,        /**< LFXO oscillator as the clock source. */
#endif
    NRF_GRTC_CLKSEL_LFCLK = GRTC_CLKCFG_CLKSEL_SystemLFCLK, /**< System LFCLK as the clock source. */
#if NRF_GRTC_HAS_CLKSEL_LFLPRC
    NRF_GRTC_CLKSEL_LFLPRC = GRTC_CLKCFG_CLKSEL_LFLPRC,     /**< System LFLPRC as the clock source. */
#endif
} nrf_grtc_clksel_t;
#endif

/**
 * @brief Function for setting the compare value of channel for the SYSCOUNTER.
 *
 * @note The corresponding event is automatically disabled by hardware during the operation.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel The specified capture/compare channel.
 * @param[in] cc_value   Compare value to be set in 1 MHz units.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_cc_set(NRF_GRTC_Type * p_reg,
                                                   uint8_t         cc_channel,
                                                   uint64_t        cc_value);

/**
 * @brief Function for getting the capture/compare value of channel for the SYSCOUNTER.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel The specified capture/compare channel.
 *
 * @return Value from the specified capture/compare register in 1MHz units.
 */
NRF_STATIC_INLINE uint64_t nrf_grtc_sys_counter_cc_get(NRF_GRTC_Type const * p_reg,
                                                       uint8_t               cc_channel);

/**
 * @brief Function for setting the value to be added to capture/compare register for
 *        the SYSCOUNTER.
 *
 * @note There are two available configurations of adding operation:
 *       When @p reference value equals @ref NRF_GRTC_CC_ADD_REFERENCE_SYSCOUNTER then
 *       the final value of capture/compare register is a sum of SYSCOUNTER current value
 *       and @p value.
 *       When @p reference value equals @ref NRF_GRTC_CC_ADD_REFERENCE_CC then
 *       the final value of capture/compare register is a sum of current capture/compare
 *       value and @p value.
 *       If the capture/compare register overflows after this write, then the corresponding event
 *       is generated immediately.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel The specified capture/compare channel.
 * @param[in] value      Value to be added in 1 MHz units.
 * @param[in] reference  Configuration of adding mode.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_cc_add_set(NRF_GRTC_Type *             p_reg,
                                                       uint8_t                     cc_channel,
                                                       uint32_t                    value,
                                                       nrf_grtc_cc_add_reference_t reference);

#if NRF_GRTC_HAS_RTCOUNTER
/**
 * @brief Function for setting a compare value for the RTCOUNTER.
 *
 * @note The internal synchronization mechanism ensures that the desired value will be properly
 *       latched by the GRTC. However when @p sync parameter is true then the process of capturing
 *       the value lasts up to two 32 kHz cycles.
 *       If the @p sync parameter is false then the capturing the value will occur on the
 *       following rising edge of 32 kHz clock. In this case it is user's responsibility
 *       to execute the function between the 32 kHz rising edges.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] cc_value Compare value to be set in 32 kHz units.
 * @param[in] sync     True if the internal synchronization mechanism shall be used,
 *                     false otherwise.
 */
NRF_STATIC_INLINE void nrf_grtc_rt_counter_cc_set(NRF_GRTC_Type * p_reg,
                                                  uint64_t        cc_value,
                                                  bool            sync);

/**
 * @brief Function for returning the compare value for the RTCOUNTER.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Value from the capture/compare register in 32 kHz units.
 */
NRF_STATIC_INLINE uint64_t nrf_grtc_rt_counter_cc_get(NRF_GRTC_Type const * p_reg);
#endif // NRF_GRTC_HAS_RTCOUNTER

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_grtc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_grtc_int_enable(NRF_GRTC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_grtc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_grtc_int_disable(NRF_GRTC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_grtc_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_int_enable_check(NRF_GRTC_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * @note States of pending interrupt are saved as a bitmask.
 *       One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_grtc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_int_pending_get(NRF_GRTC_Type const * p_reg);

#if NRF_GRTC_HAS_INT_GROUPS
/**
 * @brief Function for enabling interrupts in the specified group.
 *
 * @note Not all @p group_idx might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be enabled.
 * @param[in] mask      Mask of interrupts to be enabled.
 *                      Use @ref nrf_grtc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_grtc_int_group_enable(NRF_GRTC_Type * p_reg,
                                                 uint8_t         group_idx,
                                                 uint32_t        mask);

/**
 * @brief Function for disabling interrupts in the specified group.
 *
 * @note Not all @p group_idx might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be disabled.
 * @param[in] mask      Mask of interrupts to be disabled.
 *                      Use @ref nrf_grtc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_grtc_int_group_disable(NRF_GRTC_Type * p_reg,
                                                  uint8_t         group_idx,
                                                  uint32_t        mask);

/**
 * @brief Function for checking if the specified interrupts from a given group are enabled.
 *
 * @note Not all @p group_idx might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be checked.
 * @param[in] mask      Mask of interrupts to be checked.
 *                      Use @ref nrf_grtc_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_int_group_enable_check(NRF_GRTC_Type const * p_reg,
                                                           uint8_t               group_idx,
                                                           uint32_t              mask);
#endif // NRF_GRTC_HAS_INT_GROUPS

#if NRF_GRTC_HAS_PWM
/**
 * @brief Function for enabling events.
 *
 * @note Only specific events can be individually enabled or disabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of event flags to be enabled.
 */
NRF_STATIC_INLINE void nrf_grtc_event_enable(NRF_GRTC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling events.
 *
 * @note Only specific events can be individually enabled or disabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of event flags to be disabled.
 */
NRF_STATIC_INLINE void nrf_grtc_event_disable(NRF_GRTC_Type * p_reg, uint32_t mask);
#endif // NRF_GRTC_HAS_PWM

#if NRF_GRTC_HAS_EXTENDED
/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Bitmask of shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_grtc_shorts_enable(NRF_GRTC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Bitmask of shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_grtc_shorts_disable(NRF_GRTC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for setting the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Bitmask of shortcuts to be set.
 */
NRF_STATIC_INLINE void nrf_grtc_shorts_set(NRF_GRTC_Type * p_reg, uint32_t mask);
#endif // NRF_GRTC_HAS_EXTENDED

/**
 * @brief Function for setting the subscribe configuration for a given
 *        GRTC task.
 *
 * @note Not every task has its corresponding subscribe register.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_grtc_subscribe_set(NRF_GRTC_Type * p_reg,
                                              nrf_grtc_task_t task,
                                              uint8_t         channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        GRTC task.
 *
 * @note Not every task has its corresponding subscribe register.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_grtc_subscribe_clear(NRF_GRTC_Type * p_reg,
                                                nrf_grtc_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        GRTC task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return GRTC subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_subscribe_get(NRF_GRTC_Type const * p_reg,
                                                  nrf_grtc_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        GRTC event.
 *
 * @note Not every event has its corresponding publish register.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_grtc_publish_set(NRF_GRTC_Type *  p_reg,
                                            nrf_grtc_event_t event,
                                            uint8_t          channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        GRTC event.
 *
 * @note Not every event has its corresponding publish register.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_grtc_publish_clear(NRF_GRTC_Type *  p_reg,
                                              nrf_grtc_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        GRTC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return GRTC publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_publish_get(NRF_GRTC_Type const * p_reg,
                                                nrf_grtc_event_t      event);

/**
 * @brief Function for retrieving the state of the GRTC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_grtc_event_check(NRF_GRTC_Type const * p_reg, nrf_grtc_event_t event);

/**
 * @brief Function for clearing an event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_grtc_event_clear(NRF_GRTC_Type * p_reg, nrf_grtc_event_t event);

#if NRF_GRTC_HAS_RTCOUNTER
/**
 * @brief Function for returning the lower 32-bits of RTCOUNTER value.
 *
 * @note The whole RTCOUNTER value is latched when @ref nrf_grtc_rt_counter_low_get function
 *       is executed. Thus @ref nrf_grtc_rt_counter_low_get must be executed before calling
 *       @ref nrf_grtc_rt_counter_high_get.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Lower part of RTCOUNTER value.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_rt_counter_low_get(NRF_GRTC_Type const * p_reg);

/**
 * @brief Function for returning the higher 32-bits of RTCOUNTER value.
 *
 * @note The whole RTCOUNTER value is latched when @ref nrf_grtc_rt_counter_low_get function
 *       is executed. Thus @ref nrf_grtc_rt_counter_low_get must be executed before calling
 *       @ref nrf_grtc_rt_counter_high_get.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Higher part of RTCOUNTER value.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_rt_counter_high_get(NRF_GRTC_Type const * p_reg);
#endif // NRF_GRTC_HAS_RTCOUNTER

/**
 * @brief Function for returning the lower 32-bits of SYSCOUNTER value.
 *
 * @note @ref nrf_grtc_sys_counter_low_get must be executed before calling
 *       @ref nrf_grtc_sys_counter_high_get. In addition, after this,
 *       @ref nrf_grtc_sys_counter_overflow_check should be called. If it retuns true,
 *       whole procedure should be repeated.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Lower part of SYSCOUNTER value.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_low_get(NRF_GRTC_Type const * p_reg);

/**
 * @brief Function for returning the higher 32-bits of SYSCOUNTER value.
 *
 * @note @ref nrf_grtc_sys_counter_low_get must be executed before calling
 *       @ref nrf_grtc_sys_counter_high_get. In addition, after this,
 *       @ref nrf_grtc_sys_counter_overflow_check should be called. If it retuns true,
 *       whole procedure should be repeated.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Higher part SYSCOUNTER value.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_high_get(NRF_GRTC_Type const * p_reg);

/**
 * @brief Function for returning the 64-bit SYSCOUNTER value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SYSCOUNTER value.
 */
NRF_STATIC_INLINE uint64_t nrf_grtc_sys_counter_get(NRF_GRTC_Type const * p_reg);

/**
 * @brief Function for checking whether the lower 32-bits of SYSCOUNTER overflowed after
 *        last execution of @ref nrf_grtc_sys_counter_low_get.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval True if the lower 32-bits of SYSCOUNTER overflowed, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_grtc_sys_counter_overflow_check(NRF_GRTC_Type const * p_reg);

#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
/**
 * @brief Function for returning the lower 32-bits of SYSCOUNTER value of the specified index.
 *
 * @note Not all @p index might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @note @ref nrf_grtc_sys_counter_low_indexed_get must be executed before calling
 *       @ref nrf_grtc_sys_counter_high_indexed_get. In addition, after this,
 *       @ref nrf_grtc_sys_counter_overflow_indexed_check should be called. If it retuns true,
 *       whole procedure should be repeated.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the domain for which the lower part of the SYSCOUNTER is to be read.
 *
 * @return Lower part of SYSCOUNTER value.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_low_indexed_get(NRF_GRTC_Type const * p_reg,
                                                                uint8_t               index);

/**
 * @brief Function for returning the higher 32-bits of SYSCOUNTER value of the specified index.
 *
 * @note Not all @p index might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @note @ref nrf_grtc_sys_counter_low_indexed_get must be executed before calling
 *       @ref nrf_grtc_sys_counter_high_indexed_get. In addition, after this,
 *       @ref nrf_grtc_sys_counter_overflow_indexed_check should be called. If it retuns true,
 *       whole procedure should be repeated.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the domain for which the higher part of the SYSCOUNTER is to be read.
 *
 * @return Higher part SYSCOUNTER value.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_high_indexed_get(NRF_GRTC_Type const * p_reg,
                                                                 uint8_t               index);

/**
 * @brief Function for returning the 64-bit SYSCOUNTER value of the specified index.
 *
 * @note Not all @p index might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the domain for which the SYSCOUNTER value is to be read.
 *
 * @return SYSCOUNTER value of specified index.
 */
NRF_STATIC_INLINE uint64_t nrf_grtc_sys_counter_indexed_get(NRF_GRTC_Type const * p_reg,
                                                            uint8_t               index);

/**
 * @brief Function for checking whether the lower 32-bits of SYSCOUNTER overflowed after
 *        last execution of @ref nrf_grtc_sys_counter_low_indexed_get.
 *
 * @note Not all @p index might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the domain for which the SYSCOUNTER overflow is to be checked.
 *
 * @retval True if the lower 32-bits of the specified SYSCOUNTER overflowed, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_grtc_sys_counter_overflow_indexed_check(NRF_GRTC_Type const * p_reg,
                                                                   uint8_t               index);

/**
 * @brief Function for setting the request to keep the SYSCOUNTER active.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the SYSCOUNTER channel is to be kept active, false otherwise.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_active_set(NRF_GRTC_Type * p_reg, bool enable);

/**
 * @brief Function for checking whether the SYSCOUNTER is requested to remain active.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval True if SYSCOUNTER channel is requested to remain active, false otherwise.
 */
NRF_STATIC_INLINE
bool nrf_grtc_sys_counter_active_check(NRF_GRTC_Type const * p_reg);

/**
 * @brief Function for setting the request for the specified domain to keep the SYSCOUNTER active.
 *
 * @note Not all @p index might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Index of the domain for which the SYSCOUNTER active request is to be set.
 * @param[in] enable True if the SYSCOUNTER for the specified domain is to be kept active, false
 *                   otherwise.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_active_indexed_set(NRF_GRTC_Type * p_reg,
                                                               uint8_t         index,
                                                               bool            enable);

/**
 * @brief Function for checking whether the specified domain requests the SYSCOUNTER to remain
 *         active.
 *
 * @note Not all @p index might be valid.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the domain for which the SYSCOUNTER active request is to be checked.
 *
 * @retval True if the specified domain requests the SYSCOUNTER to remain active, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_grtc_sys_counter_active_indexed_check(NRF_GRTC_Type const * p_reg,
                                                                 uint8_t               index);

#endif // NRF_GRTC_HAS_SYSCOUNTER_ARRAY

/**
 * @brief Function for returning the address of an event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 *
 * @return Address of the requested event register.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_event_address_get(NRF_GRTC_Type const * p_reg,
                                                      nrf_grtc_event_t      event);

/**
 * @brief Function for returning the address of a task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 *
 * @return Address of the requested task register.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_task_address_get(NRF_GRTC_Type const * p_reg,
                                                     nrf_grtc_task_t       task);

/**
 * @brief Function for starting a task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 */
NRF_STATIC_INLINE void nrf_grtc_task_trigger(NRF_GRTC_Type * p_reg, nrf_grtc_task_t task);

/**
 * @brief Function for getting the 1 MHz SYSCOUNTER timer capture task associated with the
 *        specified channel.
 *
 * @param[in] cc_channel Capture channel.
 *
 * @return Capture task.
 */
NRF_STATIC_INLINE nrf_grtc_task_t nrf_grtc_sys_counter_capture_task_get(uint8_t cc_channel);

/**
 * @brief Function for enabling SYSCOUNTER compare event.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Channel number of compare event to be enabled.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_compare_event_enable(NRF_GRTC_Type * p_reg,
                                                                 uint8_t         cc_channel);

/**
 * @brief Function for disabling SYSCOUNTER compare event.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Channel number of compare event to be disabled.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_compare_event_disable(NRF_GRTC_Type * p_reg,
                                                                  uint8_t         cc_channel);

/**
 * @brief Function for getting the SYSCOUNTER compare event associated with the specified
 *        compare cc_channel.
 *
 * @param[in] cc_channel Compare channel number.
 *
 * @return Requested compare event.
 */
NRF_STATIC_INLINE nrf_grtc_event_t nrf_grtc_sys_counter_compare_event_get(uint8_t cc_channel);

/**
 * @brief Function for checking whether the specified capture/compare channel is enabled.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Channel to be checked.
 *
 * @retval true  Specified channel is enabled.
 * @retval false Specified channel is disabled.
 */
NRF_STATIC_INLINE bool nrf_grtc_sys_counter_cc_enable_check(NRF_GRTC_Type const * p_reg,
                                                            uint8_t               cc_channel);

#if NRF_GRTC_HAS_EXTENDED
/**
 * @brief Function for setting the SYSCOUNTER.
 *
 * @note When the SYSCOUNTER is disabled the GRTC uses RTCOUNTER by default.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if SYSCOUNTER is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_set(NRF_GRTC_Type * p_reg, bool enable);

/**
 * @brief Function for setting automatic mode for the SYSCOUNTER.
 *
 * @note When @p enable is false then the SYSCOUNTER remains active when KEEPRUNNING is set,
 *       or any task register, INT register or SYSCOUNTER register is being accessed.
 *       When @p enable is true then in addition the SYSCOUNTER remains active when
 *       any local CPU that is not sleeping keeps the SYSCOUNTER active.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the automatic mode is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_auto_mode_set(NRF_GRTC_Type * p_reg, bool enable);

/**
 * @brief Function for checking whether the SYSCOUNTER has automatic mode enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return True  Automatic mode is enabled.
 * @return False Automatic mode is disabled.
 */
NRF_STATIC_INLINE bool nrf_grtc_sys_counter_auto_mode_check(NRF_GRTC_Type * p_reg);
#endif // NRF_GRTC_HAS_EXTENDED

/**
 * @brief Function for checking whether the SYSCOUNTER is in active state.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval True if the SYSCOUNTER is active, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_grtc_sys_counter_check(NRF_GRTC_Type const * p_reg);

#if NRF_GRTC_HAS_KEEPRUNNING
/**
 * @brief Function for setting the request to keep the SYSCOUNTER active.
 *
 * @note This function modifies the KEEPRUNNING register, which possesses information
 *       whether any local CPU needs keeping the SYSCOUNTER active.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the automatic mode is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_active_state_request_set(NRF_GRTC_Type * p_reg,
                                                                     bool            enable);

/**
 * @brief Function for checking whether the SYSCOUNTER is requested to remain active.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 *
 * @retval True if request for keeping the SYSCOUNTER is active, false otherwise.
 */
NRF_STATIC_INLINE
bool nrf_grtc_sys_counter_active_state_request_check(NRF_GRTC_Type const * p_reg);

/**
 * @brief Function for getting the domains that requested the SYSCOUNTER to remain active.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Domains mask.
 *
 * @retval Bitmask of domains that keep the SYSCOUNTER active.
 */
NRF_STATIC_INLINE
uint32_t nrf_grtc_sys_counter_active_state_request_get(NRF_GRTC_Type const * p_reg,
                                                       uint32_t              mask);
#endif // NRF_GRTC_HAS_KEEPRUNNING

#if NRF_GRTC_HAS_EXTENDED
/**
 * @brief Function for setting the periodic compare event for capture/compare channel 0.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value Period value in 1 MHz units.
 */
NRF_STATIC_INLINE void nrf_grtc_sys_counter_interval_set(NRF_GRTC_Type * p_reg, uint32_t value);

/**
 * @brief Function for getting the value of interval for periodic capture/compare event
 *        for channel 0.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Value of the interval in 1 MHz units.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_interval_get(NRF_GRTC_Type const * p_reg);

/**
 * @brief Function for setting the timeout value for GRTC.
 *
 * @note Timeout between all CPUs going to sleep and stopping the SYSCOUNTER.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value Timeout value in 32 kHz units.
 */
NRF_STATIC_INLINE void nrf_grtc_timeout_set(NRF_GRTC_Type * p_reg, uint32_t value);

/**
 * @brief Function for getting the value of the timeout value for GRTC.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Value of the timeout in 32 kHz units.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_timeout_get(NRF_GRTC_Type const * p_reg);

/**
 * @brief Function for setting the wake time value for GRTC.
 *
 * @note The wake time is maximum number of 32 kHz cycles takes to restore the APB registers
 *       when waking from sleep state.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value Wake time value in 32 kHz units.
 */
NRF_STATIC_INLINE void nrf_grtc_waketime_set(NRF_GRTC_Type * p_reg, uint32_t value);

/**
 * @brief Function for getting the wake time value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Value of wake time in 32 kHz units.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_waketime_get(NRF_GRTC_Type const * p_reg);
#endif // NRF_GRTC_HAS_EXTENDED

#if NRF_GRTC_HAS_PWM
/**
 * @brief Function for setting the PWM compare value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value PWM compare value.
 */
NRF_STATIC_INLINE void nrf_grtc_pwm_compare_set(NRF_GRTC_Type * p_reg, uint32_t value);

/**
 * @brief Function for getting the PWM compare value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Value of PWM compare.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_pwm_compare_get(NRF_GRTC_Type const * p_reg);
#endif // NRF_GRTC_HAS_PWM

#if NRF_GRTC_HAS_CLKOUT
/**
 * @brief Function for setting the specified clock source to be connected to output pin.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] clkout Selected clkout source.
 * @param[in] enable True if the clkout source is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_grtc_clkout_set(NRF_GRTC_Type *   p_reg,
                                           nrf_grtc_clkout_t clkout,
                                           bool              enable);

/**
 * @brief Function for checking whether clock source is connected to clkout pin.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] clkout Selected clkout source.
 *
 * @retval True if the clkout source is enabled, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_grtc_clkout_enable_check(NRF_GRTC_Type const * p_reg,
                                                    nrf_grtc_clkout_t     clkout);

/**
 * @brief Function for setting the fast clock divisor value of clock output.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value Fast clock divisor value.
 */
NRF_STATIC_INLINE void nrf_grtc_clkout_divider_set(NRF_GRTC_Type * p_reg, uint32_t value);

/**
 * @brief Function for getting the fast clock divisor value of clock output.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Fast clock divisor value.
 */
NRF_STATIC_INLINE uint32_t nrf_grtc_clkout_divider_get(NRF_GRTC_Type const * p_reg);
#endif // NRF_GRTC_HAS_CLKOUT

#if NRF_GRTC_HAS_CLKSEL
/**
 * @brief Function for setting the clock source for the GRTC low-frequency clock.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] clksel Selected clock source.
 */
NRF_STATIC_INLINE void nrf_grtc_clksel_set(NRF_GRTC_Type * p_reg, nrf_grtc_clksel_t clksel);

/**
 * @brief Function for getting the clock source of the GRTC low-frequency clock.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Clock source configuration.
 */
NRF_STATIC_INLINE nrf_grtc_clksel_t nrf_grtc_clksel_get(NRF_GRTC_Type const * p_reg);
#endif // NRF_GRTC_HAS_CLKSEL

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_grtc_sys_counter_cc_set(NRF_GRTC_Type * p_reg,
                                                   uint8_t         cc_channel,
                                                   uint64_t        cc_value)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    uint32_t cc_h = (uint32_t)(cc_value >> 32);
    NRFX_ASSERT(cc_h <= NRF_GRTC_SYSCOUNTER_CCH_MASK);

    p_reg->CC[cc_channel].CCL = (uint32_t)cc_value;
    p_reg->CC[cc_channel].CCH = cc_h & NRF_GRTC_SYSCOUNTER_CCH_MASK;
}

NRF_STATIC_INLINE uint64_t nrf_grtc_sys_counter_cc_get(NRF_GRTC_Type const * p_reg,
                                                       uint8_t               cc_channel)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    uint32_t cc_h = p_reg->CC[cc_channel].CCH;

    return (uint64_t)p_reg->CC[cc_channel].CCL | ((uint64_t)cc_h << 32);
}

NRF_STATIC_INLINE void nrf_grtc_sys_counter_cc_add_set(NRF_GRTC_Type *             p_reg,
                                                       uint8_t                     cc_channel,
                                                       uint32_t                    value,
                                                       nrf_grtc_cc_add_reference_t reference)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    NRFX_ASSERT(value <= NRF_GRTC_SYSCOUNTER_CCADD_MASK);

    p_reg->CC[cc_channel].CCADD = ((uint32_t)reference << GRTC_CC_CCADD_REFERENCE_Pos) |
                               (value & NRF_GRTC_SYSCOUNTER_CCADD_MASK);
}

#if NRF_GRTC_HAS_RTCOUNTER
NRF_STATIC_INLINE void nrf_grtc_rt_counter_cc_set(NRF_GRTC_Type * p_reg,
                                                  uint64_t        cc_value,
                                                  bool            sync)
{
    uint32_t cc_h = (uint32_t)(cc_value >> 32);
    NRFX_ASSERT(cc_h <= NRF_GRTC_RTCOUNTER_CCH_MASK);

    if (sync)
    {
        p_reg->RTCOMPARESYNCL = (uint32_t)cc_value;
        p_reg->RTCOMPARESYNCH = cc_h & NRF_GRTC_RTCOUNTER_CCH_MASK;
    }
    else
    {
        p_reg->RTCOMPAREL = (uint32_t)cc_value;
        p_reg->RTCOMPAREH = cc_h & NRF_GRTC_RTCOUNTER_CCH_MASK;
    }
}

NRF_STATIC_INLINE uint64_t nrf_grtc_rt_counter_cc_get(NRF_GRTC_Type const * p_reg)
{
    uint32_t cc_h = p_reg->RTCOMPAREH;

    return (uint64_t)p_reg->RTCOMPAREL | ((uint64_t)cc_h << 32);
}
#endif // NRF_GRTC_HAS_RTCOUNTER

NRF_STATIC_INLINE void nrf_grtc_int_enable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->GRTC_INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_grtc_int_disable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->GRTC_INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_int_enable_check(NRF_GRTC_Type const * p_reg, uint32_t mask)
{
    return p_reg->GRTC_INTEN & mask;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_int_pending_get(NRF_GRTC_Type const * p_reg)
{
    return p_reg->GRTC_INTPEND;
}

#if NRF_GRTC_HAS_INT_GROUPS
NRF_STATIC_INLINE void nrf_grtc_int_group_enable(NRF_GRTC_Type * p_reg,
                                                 uint8_t         group_idx,
                                                 uint32_t        mask)
{
    switch (group_idx)
    {
        case 0:
            p_reg->INTENSET0 = mask;
            break;
        case 1:
            p_reg->INTENSET1 = mask;
            break;
#if defined(GRTC_INTENSET2_COMPARE0_Msk)
        case 2:
            p_reg->INTENSET2 = mask;
            break;
#endif
#if defined(GRTC_INTENSET3_COMPARE0_Msk)
        case 3:
            p_reg->INTENSET3 = mask;
            break;
#endif
#if defined(GRTC_INTENSET4_COMPARE0_Msk)
        case 4:
            p_reg->INTENSET4 = mask;
            break;
#endif
#if defined(GRTC_INTENSET5_COMPARE0_Msk)
        case 5:
            p_reg->INTENSET5 = mask;
            break;
#endif
#if defined(GRTC_INTENSET6_COMPARE0_Msk)
        case 6:
            p_reg->INTENSET6 = mask;
            break;
#endif
#if defined(GRTC_INTENSET7_COMPARE0_Msk)
        case 7:
            p_reg->INTENSET7 = mask;
            break;
#endif
#if defined(GRTC_INTENSET8_COMPARE0_Msk)
        case 8:
            p_reg->INTENSET8 = mask;
            break;
#endif
#if defined(GRTC_INTENSET9_COMPARE0_Msk)
        case 9:
            p_reg->INTENSET9 = mask;
            break;
#endif
#if defined(GRTC_INTENSET10_COMPARE0_Msk)
        case 10:
            p_reg->INTENSET10 = mask;
            break;
#endif
#if defined(GRTC_INTENSET11_COMPARE0_Msk)
        case 11:
            p_reg->INTENSET11 = mask;
            break;
#endif
#if defined(GRTC_INTENSET12_COMPARE0_Msk)
        case 12:
            p_reg->INTENSET12 = mask;
            break;
#endif
#if defined(GRTC_INTENSET13_COMPARE0_Msk)
        case 13:
            p_reg->INTENSET13 = mask;
            break;
#endif
#if defined(GRTC_INTENSET14_COMPARE0_Msk)
        case 14:
            p_reg->INTENSET14 = mask;
            break;
#endif
#if defined(GRTC_INTENSET15_COMPARE0_Msk)
        case 15:
            p_reg->INTENSET15 = mask;
            break;
#endif
       default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_grtc_int_group_disable(NRF_GRTC_Type * p_reg,
                                                  uint8_t         group_idx,
                                                  uint32_t        mask)
{
    switch (group_idx)
    {
        case 0:
            p_reg->INTENCLR0 = mask;
            break;
        case 1:
            p_reg->INTENCLR1 = mask;
            break;
#if defined(GRTC_INTENCLR2_COMPARE0_Msk)
        case 2:
            p_reg->INTENCLR2 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR3_COMPARE0_Msk)
        case 3:
            p_reg->INTENCLR3 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR4_COMPARE0_Msk)
        case 4:
            p_reg->INTENCLR4 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR5_COMPARE0_Msk)
        case 5:
            p_reg->INTENCLR5 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR6_COMPARE0_Msk)
        case 6:
            p_reg->INTENCLR6 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR7_COMPARE0_Msk)
        case 7:
            p_reg->INTENCLR7 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR8_COMPARE0_Msk)
        case 8:
            p_reg->INTENCLR8 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR9_COMPARE0_Msk)
        case 9:
            p_reg->INTENCLR9 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR10_COMPARE0_Msk)
        case 10:
            p_reg->INTENCLR10 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR11_COMPARE0_Msk)
        case 11:
            p_reg->INTENCLR11 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR12_COMPARE0_Msk)
        case 12:
            p_reg->INTENCLR12 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR13_COMPARE0_Msk)
        case 13:
            p_reg->INTENCLR13 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR14_COMPARE0_Msk)
        case 14:
            p_reg->INTENCLR14 = mask;
            break;
#endif
#if defined(GRTC_INTENCLR15_COMPARE0_Msk)
        case 15:
            p_reg->INTENCLR15 = mask;
            break;
#endif
       default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE uint32_t nrf_grtc_int_group_enable_check(NRF_GRTC_Type const * p_reg,
                                                           uint8_t               group_idx,
                                                           uint32_t              mask)
{
    switch (group_idx)
    {
        case 0:
            return p_reg->INTENSET0 & mask;
        case 1:
            return p_reg->INTENSET1 & mask;
#if defined(GRTC_INTENSET2_COMPARE0_Msk)
        case 2:
            return p_reg->INTENSET2 & mask;
#endif
#if defined(GRTC_INTENSET3_COMPARE0_Msk)
        case 3:
            return p_reg->INTENSET3 & mask;
#endif
#if defined(GRTC_INTENSET4_COMPARE0_Msk)
        case 4:
            return p_reg->INTENSET4 & mask;
#endif
#if defined(GRTC_INTENSET5_COMPARE0_Msk)
        case 5:
            return p_reg->INTENSET5 & mask;
#endif
#if defined(GRTC_INTENSET6_COMPARE0_Msk)
        case 6:
            return p_reg->INTENSET6 & mask;
#endif
#if defined(GRTC_INTENSET7_COMPARE0_Msk)
        case 7:
            return p_reg->INTENSET7 & mask;
#endif
#if defined(GRTC_INTENSET8_COMPARE0_Msk)
        case 8:
            return p_reg->INTENSET8 & mask;
#endif
#if defined(GRTC_INTENSET9_COMPARE0_Msk)
        case 9:
            return p_reg->INTENSET9 & mask;
#endif
#if defined(GRTC_INTENSET10_COMPARE0_Msk)
        case 10:
            return p_reg->INTENSET10 & mask;
#endif
#if defined(GRTC_INTENSET11_COMPARE0_Msk)
        case 11:
            return p_reg->INTENSET11 & mask;
#endif
#if defined(GRTC_INTENSET12_COMPARE0_Msk)
        case 12:
            return p_reg->INTENSET12 & mask;
#endif
#if defined(GRTC_INTENSET13_COMPARE0_Msk)
        case 13:
            return p_reg->INTENSET13 & mask;
#endif
#if defined(GRTC_INTENSET14_COMPARE0_Msk)
        case 14:
            return p_reg->INTENSET14 & mask;
#endif
#if defined(GRTC_INTENSET15_COMPARE0_Msk)
        case 15:
            return p_reg->INTENSET15 & mask;
#endif
       default:
            NRFX_ASSERT(false);
            return 0;
    }
}
#endif // NRF_GRTC_HAS_INT_GROUPS

#if NRF_GRTC_HAS_PWM
NRF_STATIC_INLINE void nrf_grtc_event_enable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->EVTENSET = mask;
}

NRF_STATIC_INLINE void nrf_grtc_event_disable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->EVTENCLR = mask;
}
#endif // NRF_GRTC_HAS_PWM

#if NRF_GRTC_HAS_EXTENDED
NRF_STATIC_INLINE void nrf_grtc_shorts_enable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_grtc_shorts_disable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE void nrf_grtc_shorts_set(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS = mask;
}
#endif // NRF_GRTC_HAS_EXTENDED

NRF_STATIC_INLINE void nrf_grtc_subscribe_set(NRF_GRTC_Type * p_reg,
                                              nrf_grtc_task_t task,
                                              uint8_t         channel)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT((task != NRF_GRTC_TASK_START) &&
                (task != NRF_GRTC_TASK_CLEAR) &&
                (task != NRF_GRTC_TASK_STOP));
#endif

    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_grtc_subscribe_clear(NRF_GRTC_Type * p_reg,
                                                nrf_grtc_task_t task)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT((task != NRF_GRTC_TASK_START) &&
                (task != NRF_GRTC_TASK_CLEAR) &&
                (task != NRF_GRTC_TASK_STOP));
#endif

    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_subscribe_get(NRF_GRTC_Type const * p_reg,
                                                  nrf_grtc_task_t       task)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT((task != NRF_GRTC_TASK_START) &&
                (task != NRF_GRTC_TASK_CLEAR) &&
                (task != NRF_GRTC_TASK_STOP));
#endif
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_grtc_publish_set(NRF_GRTC_Type *  p_reg,
                                            nrf_grtc_event_t event,
                                            uint8_t          channel)
{
#if NRF_GRTC_HAS_SYSCOUNTERVALID
    NRFX_ASSERT(event != NRF_GRTC_EVENT_SYSCOUNTERVALID);
#endif
#if NRF_GRTC_HAS_RTCOUNTER
    NRFX_ASSERT(event != NRF_GRTC_EVENT_RTCOMPARESYNC);
#endif

    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80UL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_grtc_publish_clear(NRF_GRTC_Type *  p_reg,
                                              nrf_grtc_event_t event)
{
#if NRF_GRTC_HAS_SYSCOUNTERVALID
    NRFX_ASSERT(event != NRF_GRTC_EVENT_SYSCOUNTERVALID);
#endif
#if NRF_GRTC_HAS_RTCOUNTER
    NRFX_ASSERT(event != NRF_GRTC_EVENT_RTCOMPARESYNC);
#endif

    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80UL)) = 0x0UL;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_publish_get(NRF_GRTC_Type const * p_reg,
                                                nrf_grtc_event_t      event)
{
#if NRF_GRTC_HAS_SYSCOUNTERVALID
    NRFX_ASSERT(event != NRF_GRTC_EVENT_SYSCOUNTERVALID);
#endif
#if NRF_GRTC_HAS_RTCOUNTER
    NRFX_ASSERT(event != NRF_GRTC_EVENT_RTCOMPARESYNC);
#endif

    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}

NRF_STATIC_INLINE bool nrf_grtc_event_check(NRF_GRTC_Type const * p_reg, nrf_grtc_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_grtc_event_clear(NRF_GRTC_Type * p_reg, nrf_grtc_event_t event)
{
#if NRF_GRTC_HAS_SYSCOUNTERVALID
    NRFX_ASSERT(event != NRF_GRTC_EVENT_SYSCOUNTERVALID);
#endif

    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
}

#if NRF_GRTC_HAS_RTCOUNTER
NRF_STATIC_INLINE uint32_t nrf_grtc_rt_counter_low_get(NRF_GRTC_Type const * p_reg)
{
    return p_reg->RTCOUNTERL;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_rt_counter_high_get(NRF_GRTC_Type const * p_reg)
{
    return p_reg->RTCOUNTERH;
}
#endif // NRF_GRTC_HAS_RTCOUNTER

NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_low_get(NRF_GRTC_Type const * p_reg)
{
#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    return p_reg->GRTC_SYSCOUNTER.SYSCOUNTERL;
#else
    return p_reg->SYSCOUNTERL;
#endif // NRF_GRTC_HAS_SYSCOUNTER_ARRAY
}

NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_high_get(NRF_GRTC_Type const * p_reg)
{
#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    return p_reg->GRTC_SYSCOUNTER.SYSCOUNTERH;
#else
    return p_reg->SYSCOUNTERH;
#endif // NRF_GRTC_HAS_SYSCOUNTER_ARRAY
}

NRF_STATIC_INLINE uint64_t nrf_grtc_sys_counter_get(NRF_GRTC_Type const * p_reg)
{
#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    uintptr_t ptr = (uintptr_t)&p_reg->GRTC_SYSCOUNTER.SYSCOUNTERL;
#else
    uintptr_t ptr = (uintptr_t)&p_reg->SYSCOUNTERL;
#endif // NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    return *(const uint64_t volatile *)ptr;
}

NRF_STATIC_INLINE bool nrf_grtc_sys_counter_overflow_check(NRF_GRTC_Type const * p_reg)
{
#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
    return (p_reg->GRTC_SYSCOUNTER.SYSCOUNTERH &
            GRTC_SYSCOUNTER_SYSCOUNTERH_OVERFLOW_Msk) ? true : false;
#else
    return (p_reg->SYSCOUNTERH & GRTC_SYSCOUNTERH_OVERFLOW_Msk) ? true : false;
#endif // NRF_GRTC_HAS_SYSCOUNTER_ARRAY
}

#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY
NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_low_indexed_get(NRF_GRTC_Type const * p_reg,
                                                                uint8_t               index)
{
    NRFX_ASSERT(index < NRF_GRTC_SYSCOUNTER_COUNT);
    return p_reg->SYSCOUNTER[index].SYSCOUNTERL;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_high_indexed_get(NRF_GRTC_Type const * p_reg,
                                                                 uint8_t               index)
{
    NRFX_ASSERT(index < NRF_GRTC_SYSCOUNTER_COUNT);
    return p_reg->SYSCOUNTER[index].SYSCOUNTERH;
}

NRF_STATIC_INLINE uint64_t nrf_grtc_sys_counter_indexed_get(NRF_GRTC_Type const * p_reg,
                                                            uint8_t               index)
{
    NRFX_ASSERT(index < NRF_GRTC_SYSCOUNTER_COUNT);
    uintptr_t ptr = (uintptr_t)&p_reg->SYSCOUNTER[index];
    return *(const uint64_t volatile *)ptr;
}

NRF_STATIC_INLINE bool nrf_grtc_sys_counter_overflow_indexed_check(NRF_GRTC_Type const * p_reg,
                                                                   uint8_t               index)
{
    NRFX_ASSERT(index < NRF_GRTC_SYSCOUNTER_COUNT);
    return (p_reg->SYSCOUNTER[index].SYSCOUNTERH &
            GRTC_SYSCOUNTER_SYSCOUNTERH_OVERFLOW_Msk) ? true : false;
}

NRF_STATIC_INLINE void nrf_grtc_sys_counter_active_set(NRF_GRTC_Type * p_reg, bool enable)
{
    p_reg->GRTC_SYSCOUNTER.ACTIVE = ((p_reg->GRTC_SYSCOUNTER.ACTIVE &
                                     ~(GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Msk)) |
                                     (enable ? GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Active :
                                               GRTC_SYSCOUNTER_ACTIVE_ACTIVE_NotActive));
}

NRF_STATIC_INLINE bool nrf_grtc_sys_counter_active_check(NRF_GRTC_Type const * p_reg)
{
    return (p_reg->GRTC_SYSCOUNTER.ACTIVE & GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Msk) ==
           GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Active;
}

NRF_STATIC_INLINE void nrf_grtc_sys_counter_active_indexed_set(NRF_GRTC_Type * p_reg,
                                                               uint8_t         index,
                                                               bool            enable)
{
    NRFX_ASSERT(index < NRF_GRTC_SYSCOUNTER_COUNT);

    p_reg->SYSCOUNTER[index].ACTIVE = ((p_reg->SYSCOUNTER[index].ACTIVE &
                                     ~(GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Msk)) |
                                     (enable ? GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Active :
                                               GRTC_SYSCOUNTER_ACTIVE_ACTIVE_NotActive));
}

NRF_STATIC_INLINE bool nrf_grtc_sys_counter_active_indexed_check(NRF_GRTC_Type const * p_reg,
                                                                 uint8_t               index)
{
    NRFX_ASSERT(index < NRF_GRTC_SYSCOUNTER_COUNT);

    return (p_reg->SYSCOUNTER[index].ACTIVE & GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Msk) ==
           GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Active;
}
#endif // NRF_GRTC_HAS_SYSCOUNTER_ARRAY

NRF_STATIC_INLINE uint32_t nrf_grtc_event_address_get(NRF_GRTC_Type const * p_reg,
                                                      nrf_grtc_event_t      event)
{
    return (uint32_t)p_reg + (uint32_t)event;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_task_address_get(NRF_GRTC_Type const * p_reg,
                                                     nrf_grtc_task_t       task)
{
    return (uint32_t)p_reg + (uint32_t)task;
}

NRF_STATIC_INLINE void nrf_grtc_task_trigger(NRF_GRTC_Type * p_reg, nrf_grtc_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE nrf_grtc_task_t nrf_grtc_sys_counter_capture_task_get(uint8_t cc_channel)
{
    return (nrf_grtc_task_t)NRFX_OFFSETOF(NRF_GRTC_Type, TASKS_CAPTURE[cc_channel]);
}

NRF_STATIC_INLINE void nrf_grtc_sys_counter_compare_event_enable(NRF_GRTC_Type * p_reg,
                                                                 uint8_t         cc_channel)
{

#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    p_reg->CC[cc_channel].CCEN = GRTC_CC_CCEN_ACTIVE_Enable;
}

NRF_STATIC_INLINE void nrf_grtc_sys_counter_compare_event_disable(NRF_GRTC_Type * p_reg,
                                                                  uint8_t         cc_channel)
{
#if NRF_GRTC_HAS_EXTENDED
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
#else
    NRFX_ASSERT(cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT &&
                cc_channel > NRF_GRTC_MAIN_CC_CHANNEL);
#endif
    p_reg->CC[cc_channel].CCEN = GRTC_CC_CCEN_ACTIVE_Disable;
}

NRF_STATIC_INLINE nrf_grtc_event_t nrf_grtc_sys_counter_compare_event_get(uint8_t cc_channel)
{
    return (nrf_grtc_event_t)NRFX_OFFSETOF(NRF_GRTC_Type, EVENTS_COMPARE[cc_channel]);
}

NRF_STATIC_INLINE bool nrf_grtc_sys_counter_cc_enable_check(NRF_GRTC_Type const * p_reg,
                                                            uint8_t               cc_channel)
{
    return ((p_reg->CC[cc_channel].CCEN & GRTC_CC_CCEN_ACTIVE_Msk) >> GRTC_CC_CCEN_ACTIVE_Pos) ==
           GRTC_CC_CCEN_ACTIVE_Enable;
}

#if NRF_GRTC_HAS_EXTENDED
NRF_STATIC_INLINE void nrf_grtc_sys_counter_set(NRF_GRTC_Type * p_reg, bool enable)
{
    p_reg->MODE = ((p_reg->MODE & ~GRTC_MODE_SYSCOUNTEREN_Msk) |
                  ((enable ? GRTC_MODE_SYSCOUNTEREN_Enabled :
                  GRTC_MODE_SYSCOUNTEREN_Disabled) << GRTC_MODE_SYSCOUNTEREN_Pos));
}

NRF_STATIC_INLINE void nrf_grtc_sys_counter_auto_mode_set(NRF_GRTC_Type * p_reg, bool enable)
{
    p_reg->MODE = ((p_reg->MODE & ~GRTC_MODE_AUTOEN_Msk) |
                  ((enable ? GRTC_MODE_AUTOEN_CpuActive :
                  GRTC_MODE_AUTOEN_Default) << GRTC_MODE_AUTOEN_Pos));
}

NRF_STATIC_INLINE bool nrf_grtc_sys_counter_auto_mode_check(NRF_GRTC_Type * p_reg)
{
    return (p_reg->MODE & GRTC_MODE_AUTOEN_Msk) == GRTC_MODE_AUTOEN_CpuActive;
}

#endif // NRF_GRTC_HAS_EXTENDED

NRF_STATIC_INLINE bool nrf_grtc_sys_counter_check(NRF_GRTC_Type const * p_reg)
{
    return (p_reg->MODE & GRTC_MODE_SYSCOUNTEREN_Msk) ? true : false;
}

#if NRF_GRTC_HAS_KEEPRUNNING
NRF_STATIC_INLINE void nrf_grtc_sys_counter_active_state_request_set(NRF_GRTC_Type * p_reg,
                                                                     bool            enable)
{
#if defined(GRTC_KEEPRUNNING_DOMAIN0_Msk)
    p_reg->KEEPRUNNING = ((p_reg->KEEPRUNNING &
                          ~(GRTC_KEEPRUNNING_DOMAIN0_Active  << NRF_GRTC_DOMAIN_INDEX)) |
                          ((enable ? GRTC_KEEPRUNNING_DOMAIN0_Active :
                           GRTC_KEEPRUNNING_DOMAIN0_NotActive) << NRF_GRTC_DOMAIN_INDEX));
#else
    p_reg->KEEPRUNNING = ((p_reg->KEEPRUNNING &
                          ~(GRTC_KEEPRUNNING_REQUEST0_Active  << NRF_GRTC_DOMAIN_INDEX)) |
                          ((enable ? GRTC_KEEPRUNNING_REQUEST0_Active :
                           GRTC_KEEPRUNNING_REQUEST0_NotActive) << NRF_GRTC_DOMAIN_INDEX));
#endif
}

NRF_STATIC_INLINE
bool nrf_grtc_sys_counter_active_state_request_check(NRF_GRTC_Type const * p_reg)
{
#if defined(GRTC_KEEPRUNNING_DOMAIN0_Msk)
    return (p_reg->KEEPRUNNING &
            (GRTC_KEEPRUNNING_DOMAIN0_Active << NRF_GRTC_DOMAIN_INDEX)) ? true : false;
#else
    return (p_reg->KEEPRUNNING &
            (GRTC_KEEPRUNNING_REQUEST0_Active << NRF_GRTC_DOMAIN_INDEX)) ? true : false;
#endif
}

NRF_STATIC_INLINE
uint32_t nrf_grtc_sys_counter_active_state_request_get(NRF_GRTC_Type const * p_reg,
                                                       uint32_t              mask)
{
    return p_reg->KEEPRUNNING & mask;
}
#endif // NRF_GRTC_HAS_KEEPRUNNING

#if NRF_GRTC_HAS_EXTENDED
NRF_STATIC_INLINE void nrf_grtc_sys_counter_interval_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    p_reg->INTERVAL = value;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_sys_counter_interval_get(NRF_GRTC_Type const * p_reg)
{
    return p_reg->INTERVAL;
}

NRF_STATIC_INLINE void nrf_grtc_timeout_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    NRFX_ASSERT(value <= NRF_GRTC_TIMEOUT_MAX_VALUE);
    p_reg->TIMEOUT = (value << GRTC_TIMEOUT_VALUE_Pos);
}

NRF_STATIC_INLINE uint32_t nrf_grtc_timeout_get(NRF_GRTC_Type const * p_reg)
{
    return (p_reg->TIMEOUT >> GRTC_TIMEOUT_VALUE_Pos);
}

NRF_STATIC_INLINE void nrf_grtc_waketime_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    NRFX_ASSERT(value <= NRF_GRTC_WAKETIME_MAX_VALUE);
    p_reg->WAKETIME = (value << GRTC_WAKETIME_VALUE_Pos);
}

NRF_STATIC_INLINE uint32_t nrf_grtc_waketime_get(NRF_GRTC_Type const * p_reg)
{
    return (p_reg->WAKETIME >> GRTC_WAKETIME_VALUE_Pos);
}
#endif // NRF_GRTC_HAS_EXTENDED

#if NRF_GRTC_HAS_PWM
NRF_STATIC_INLINE void nrf_grtc_pwm_compare_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    p_reg->PWMCONFIG = (value << GRTC_PWMCONFIG_COMPAREVALUE_Pos) & GRTC_PWMCONFIG_COMPAREVALUE_Msk;
}

NRF_STATIC_INLINE uint32_t nrf_grtc_pwm_compare_get(NRF_GRTC_Type const * p_reg)
{
    return (p_reg->PWMCONFIG >> GRTC_PWMCONFIG_COMPAREVALUE_Pos);
}
#endif // NRF_GRTC_HAS_PWM

#if NRF_GRTC_HAS_CLKOUT
NRF_STATIC_INLINE void nrf_grtc_clkout_set(NRF_GRTC_Type *   p_reg,
                                           nrf_grtc_clkout_t clkout,
                                           bool              enable)
{
    if (enable)
    {
        p_reg->CLKOUT |= (uint32_t)clkout;
    }
    else
    {
        p_reg->CLKOUT &= ~((uint32_t)clkout);
    }
}

NRF_STATIC_INLINE bool nrf_grtc_clkout_enable_check(NRF_GRTC_Type const * p_reg,
                                                    nrf_grtc_clkout_t     clkout)
{
    return p_reg->CLKOUT == (uint32_t)clkout;
}

NRF_STATIC_INLINE void nrf_grtc_clkout_divider_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    NRFX_ASSERT(value <= NRF_GRTC_CLKCFG_CLKFASTDIV_MAX_VALUE);
    p_reg->CLKCFG = (p_reg->CLKCFG & ~GRTC_CLKCFG_CLKFASTDIV_Msk) |
                    ((value & GRTC_CLKCFG_CLKFASTDIV_Msk) << GRTC_CLKCFG_CLKFASTDIV_Pos);
}

NRF_STATIC_INLINE uint32_t nrf_grtc_clkout_divider_get(NRF_GRTC_Type const * p_reg)
{
    return (p_reg->CLKCFG & GRTC_CLKCFG_CLKFASTDIV_Msk) >> GRTC_CLKCFG_CLKFASTDIV_Pos;
}
#endif // NRF_GRTC_HAS_CLKOUT

#if NRF_GRTC_HAS_CLKSEL
NRF_STATIC_INLINE void nrf_grtc_clksel_set(NRF_GRTC_Type * p_reg, nrf_grtc_clksel_t clksel)
{
    p_reg->CLKCFG = (p_reg->CLKCFG & ~GRTC_CLKCFG_CLKSEL_Msk) |
                    (clksel << GRTC_CLKCFG_CLKSEL_Pos);
}

NRF_STATIC_INLINE nrf_grtc_clksel_t nrf_grtc_clksel_get(NRF_GRTC_Type const * p_reg)
{
    return (nrf_grtc_clksel_t)((p_reg->CLKCFG & GRTC_CLKCFG_CLKSEL_Msk) >> GRTC_CLKCFG_CLKSEL_Pos);
}
#endif // NRF_GRTC_HAS_CLKSEL

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* NRF_GRTC_H */
