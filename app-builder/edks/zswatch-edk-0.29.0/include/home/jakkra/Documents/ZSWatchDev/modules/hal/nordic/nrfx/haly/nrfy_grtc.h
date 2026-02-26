/*
 * Copyright (c) 2021 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFY_GRTC_H__
#define NRFY_GRTC_H__

#include <nrfx.h>
#include <hal/nrf_grtc.h>
#include <lib/nrfx_coredep.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_grtc_event_enabled_clear(NRF_GRTC_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_grtc_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_grtc_event_handle(NRF_GRTC_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_grtc_event_t event,
                                                          uint32_t *       p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_grtc_events_process(NRF_GRTC_Type * p_reg,
                                                                uint32_t        mask);

#if NRF_GRTC_HAS_RTCOUNTER
NRFY_STATIC_INLINE uint64_t __nrfy_internal_grtc_rt_counter_read(NRF_GRTC_Type const * p_reg);
#endif

NRFY_STATIC_INLINE bool __nrfy_internal_grtc_sys_counter_ready_check(NRF_GRTC_Type const * p_reg);

/**
 * @defgroup nrfy_grtc GRTC HALY
 * @{
 * @ingroup nrf_grtc
 * @brief   Hardware access layer with cache and barrier support for managing the GRTC peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

#if NRF_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_EXTENDED} */
#define NRFY_GRTC_HAS_EXTENDED 1
#else
#define NRFY_GRTC_HAS_EXTENDED 0
#endif

#if NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_RTCOUNTER} */
#define NRFY_GRTC_HAS_RTCOUNTER 1
#else
#define NRFY_GRTC_HAS_RTCOUNTER 0
#endif

#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_SYSCOUNTER_ARRAY} */
#define NRFY_GRTC_HAS_SYSCOUNTER_ARRAY 1
#else
#define NRFY_GRTC_HAS_SYSCOUNTER_ARRAY 0
#endif

#if NRF_GRTC_HAS_SYSCOUNTERVALID || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_SYSCOUNTERVALID} */
#define NRFY_GRTC_HAS_SYSCOUNTERVALID 1
#else
#define NRFY_GRTC_HAS_SYSCOUNTERVALID 0
#endif

#if NRF_GRTC_HAS_SYSCOUNTER_LOADED || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_SYSCOUNTER_LOADED} */
#define NRFY_GRTC_HAS_SYSCOUNTER_LOADED 1
#else
#define NRFY_GRTC_HAS_SYSCOUNTER_LOADED 0
#endif

#if NRF_GRTC_HAS_KEEPRUNNING || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_KEEPRUNNING} */
#define NRFY_GRTC_HAS_KEEPRUNNING 1
#else
#define NRFY_GRTC_HAS_KEEPRUNNING 0
#endif

#if NRF_GRTC_HAS_PWM || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_CLKOUT} */
#define NRFY_GRTC_HAS_PWM 1
#else
#define NRFY_GRTC_HAS_PWM 0
#endif

#if NRF_GRTC_HAS_CLKOUT || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_CLKOUT} */
#define NRFY_GRTC_HAS_CLKOUT 1
#else
#define NRFY_GRTC_HAS_CLKOUT 0
#endif

#if NRF_GRTC_HAS_CLKSEL || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_GRTC_HAS_CLKSEL} */
#define NRFY_GRTC_HAS_CLKSEL 1
#else
#define NRFY_GRTC_HAS_CLKSEL 0
#endif

#if NRF_GRTC_HAS_SYSCOUNTER_ARRAY || defined(__NRFX_DOXYGEN__)
#if (NRFX_CHECK(ISA_ARM) && (__CORTEX_M == 33U)) || defined(__NRFX_DOXYGEN__)
/** @brief Mask to determine whether the SYSCOUNTER value is reliable. */
#define NRFY_GRTC_SYSCOUNTER_RETRY_MASK \
    ((uint64_t)(NRF_GRTC_SYSCOUNTERH_OVERFLOW_MASK + NRF_GRTC_SYSCOUNTERH_BUSY_MASK) << 32)
#else
#define NRFY_GRTC_SYSCOUNTER_RETRY_MASK \
    (NRF_GRTC_SYSCOUNTERH_OVERFLOW_MASK + NRF_GRTC_SYSCOUNTERH_BUSY_MASK)
#endif
#else
#if NRFX_CHECK(ISA_ARM) && (__CORTEX_M == 33U)
#define NRFY_GRTC_SYSCOUNTER_RETRY_MASK ((uint64_t)(NRF_GRTC_SYSCOUNTERH_OVERFLOW_MASK) << 32)
#else
#define NRFY_GRTC_SYSCOUNTER_RETRY_MASK (NRF_GRTC_SYSCOUNTERH_OVERFLOW_MASK)
#endif
#endif

/** @brief Mask of the SYSCOUNTER value. */
#define NRFY_GRTC_SYSCOUNTER_MASK ((uint64_t)(NRF_GRTC_SYSCOUNTERL_VALUE_MASK) + \
                                   ((uint64_t)(NRF_GRTC_SYSCOUNTERH_VALUE_MASK) << 32))

/**
 * @brief Function for initializing the specified GRTC interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_grtc_int_init(NRF_GRTC_Type * p_reg,
                                           uint32_t        mask,
                                           uint8_t         irq_priority,
                                           bool            enable)
{
    for (uint8_t cc_channel = 0; cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT; cc_channel++)
    {
        nrf_grtc_event_t event = nrf_grtc_sys_counter_compare_event_get(cc_channel);
        __nrfy_internal_grtc_event_enabled_clear(p_reg, mask, event);
    }
#if NRFY_GRTC_HAS_RTCOUNTER
    __nrfy_internal_grtc_event_enabled_clear(p_reg, mask, NRF_GRTC_EVENT_RTCOMPARE);
    __nrfy_internal_grtc_event_enabled_clear(p_reg, mask, NRF_GRTC_EVENT_RTCOMPARESYNC);
#endif
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(GRTC_IRQn, irq_priority);
    NRFX_IRQ_ENABLE(GRTC_IRQn);
    if (enable)
    {
        nrf_grtc_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the GRTC interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_grtc_int_uninit(NRF_GRTC_Type * p_reg)
{
    (void)p_reg;
    NRFX_IRQ_DISABLE(GRTC_IRQn);
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified GRTC events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_events_process(NRF_GRTC_Type * p_reg,
                                                     uint32_t        mask)
{
    uint32_t evt_mask = __nrfy_internal_grtc_events_process(p_reg, mask);

    nrf_barrier_w();
    return evt_mask;
}

#if NRFY_GRTC_HAS_EXTENDED
/**
 * @brief Function for preparing the GRTC peripheral.
 *
 * @note This function clears all shorts, interrupts, resets and starts the GRTC.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] busy_wait True if wait for synchronization operation is to be performed,
 *                      false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_grtc_prepare(NRF_GRTC_Type * p_reg, bool busy_wait)
{
#if NRFY_GRTC_HAS_CLKSEL && NRFX_IS_ENABLED(NRFX_GRTC_CONFIG_LFCLK_SELECT_AT_INIT)
    nrf_grtc_clksel_set(p_reg, NRF_GRTC_CLKSEL_LFCLK);
#endif
    nrf_grtc_sys_counter_set(p_reg, false);
    nrf_barrier_w();
#if NRFY_GRTC_HAS_RTCOUNTER
    nrf_grtc_shorts_disable(p_reg, NRF_GRTC_SHORT_RTCOMPARE_CLEAR_MASK);
#endif
    nrf_grtc_int_disable(p_reg, NRF_GRTC_INTEN_MASK);
    for (uint8_t cc_channel = 0; cc_channel < NRF_GRTC_SYSCOUNTER_CC_COUNT; cc_channel++)
    {
        nrf_grtc_publish_clear(p_reg, nrf_grtc_sys_counter_compare_event_get(cc_channel));
        nrf_grtc_subscribe_clear(p_reg, nrf_grtc_sys_counter_capture_task_get(cc_channel));
    }
#if NRFY_GRTC_HAS_RTCOUNTER
    nrf_grtc_publish_clear(p_reg, NRF_GRTC_EVENT_RTCOMPARE);
#endif
#if NRFX_IS_ENABLED(NRFX_GRTC_CONFIG_CLEAR_AT_INIT)
    nrf_grtc_task_trigger(p_reg, NRF_GRTC_TASK_CLEAR);
    nrf_barrier_w();
#endif
    nrf_grtc_task_trigger(p_reg, NRF_GRTC_TASK_START);
    nrf_barrier_w();
    if (busy_wait)
    {
#if NRFY_GRTC_HAS_RTCOUNTER
#if NRFX_IS_ENABLED(NRFX_GRTC_CONFIG_CLEAR_AT_INIT)
        // Make sure that RTCOUNTER is cleared and does not contain the old value.
        while (__nrfy_internal_grtc_rt_counter_read(p_reg) > 1ULL)
        {}
#endif
        // Wait one 32k cycle to make sure that RTCOUNTER has started.
        uint64_t t = __nrfy_internal_grtc_rt_counter_read(p_reg);
        while (__nrfy_internal_grtc_rt_counter_read(p_reg) == t)
        {}
#else
        // Wait 3 32k cycles.
        // `NRFX_DELAY_US()` macro cannot be used here because in Zephyr environment
        // it calls `k_busy_wait()`. This function relies on system timer which is
        // not started yet.
        nrfx_coredep_delay_us(93);
#endif // NRFY_GRTC_HAS_RTCOUNTER
    }
}

/**
 * @brief Function for starting the SYSCOUNTER.
 *
 * @note This function enables the 1 MHz counter and set it as always active.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] busy_wait True if wait for synchronization operation is to be performed,
 *                      false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_start(NRF_GRTC_Type * p_reg, bool busy_wait)
{
    nrf_grtc_sys_counter_set(p_reg, true);
    nrf_barrier_w();
    if (busy_wait)
    {
        bool active;
#if NRFY_GRTC_HAS_SYSCOUNTER_ARRAY
        active = nrf_grtc_sys_counter_active_check(p_reg);
        nrf_barrier_r();
        if (!active)
        {
            nrf_grtc_sys_counter_active_set(p_reg, true);
            nrf_barrier_w();
        }
#else
        active = nrf_grtc_sys_counter_active_state_request_check(p_reg);
        nrf_barrier_r();
        if (!active)
        {
            nrf_grtc_sys_counter_active_state_request_set(p_reg, true);
            nrf_barrier_w();
        }
#endif // NRFY_GRTC_HAS_SYSCOUNTER_ARRAY
        while (!__nrfy_internal_grtc_sys_counter_ready_check(p_reg))
        {}

        if (!active)
        {
#if NRFY_GRTC_HAS_SYSCOUNTER_ARRAY
            nrf_grtc_sys_counter_active_set(p_reg, false);
#else
            nrf_grtc_sys_counter_active_state_request_set(p_reg, true);
#endif
            nrf_barrier_w();
        }

    }
}
#endif // NRFY_GRTC_HAS_EXTENDED

/**
 * @brief Function for returning the SYSCOUNTER 1 MHz value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SYSCOUNTER value.
 */
NRFY_STATIC_INLINE uint64_t nrfy_grtc_sys_counter_get(NRF_GRTC_Type const * p_reg)
{
#if NRFX_CHECK(ISA_ARM) && (__CORTEX_M == 33U)
    uint64_t counter;

    do {
        counter = nrf_grtc_sys_counter_get(p_reg);
    } while (counter & NRFY_GRTC_SYSCOUNTER_RETRY_MASK);
#if NRFX_CHECK(NRFY_GRTC_HAS_SYSCOUNTER_LOADED)
    return counter & NRFY_GRTC_SYSCOUNTER_MASK;
#else
    return counter;
#endif
#else
    uint32_t counter_l, counter_h;

    do {
        counter_l = nrf_grtc_sys_counter_low_get(p_reg);
        nrf_barrier_r();
        counter_h = nrf_grtc_sys_counter_high_get(p_reg);
        nrf_barrier_r();
    } while (counter_h & NRFY_GRTC_SYSCOUNTER_RETRY_MASK);
#if NRFX_CHECK(NRFY_GRTC_HAS_SYSCOUNTER_LOADED)
    return (uint64_t)counter_l | ((uint64_t)(counter_h & NRF_GRTC_SYSCOUNTERH_VALUE_MASK) << 32);
#else
    return (uint64_t)counter_l | ((uint64_t)counter_h << 32);
#endif
#endif // NRFX_CHECK(ISA_ARM) && (__CORTEX_M == 33U)
}

#if NRFY_GRTC_HAS_SYSCOUNTER_ARRAY
/**
 * @brief Function for returning the value of 1 MHz SYSCOUNTER for the specified domain.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index index Index of the domain for which the SYSCOUNTER value is to be read.
 *
 * @return SYSCOUNTER value.
 */
NRFY_STATIC_INLINE uint64_t nrfy_grtc_sys_counter_indexed_get(NRF_GRTC_Type const * p_reg,
                                                              uint8_t               index)
{
#if NRFX_CHECK(ISA_ARM) && (__CORTEX_M == 33U)
    uint64_t counter;

    do {
        counter = nrf_grtc_sys_counter_indexed_get(p_reg, index);
    } while (counter & NRFY_GRTC_SYSCOUNTER_RETRY_MASK);
    return (counter & NRFY_GRTC_SYSCOUNTER_MASK);
#else
    uint32_t counter_l, counter_h;

    do {
        counter_l = nrf_grtc_sys_counter_low_indexed_get(p_reg, index);
        nrf_barrier_r();
        counter_h = nrf_grtc_sys_counter_high_indexed_get(p_reg, index);
        nrf_barrier_r();
    } while (counter_h & NRFY_GRTC_SYSCOUNTER_RETRY_MASK);
    return (uint64_t)counter_l | ((uint64_t)(counter_h & NRF_GRTC_SYSCOUNTERH_VALUE_MASK) << 32);
#endif // NRFX_CHECK(ISA_ARM) && (__CORTEX_M == 33U)
}
#endif // NRFY_GRTC_HAS_SYSCOUNTER_ARRAY

/**
 * @brief Function for checking whether SYSCOUNTER value is ready to be read.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return True if SYSCOUNTER is ready to be read, false otherwise.
 */
NRFY_STATIC_INLINE bool nrfy_grtc_sys_counter_ready_check(NRF_GRTC_Type const * p_reg)
{
    return __nrfy_internal_grtc_sys_counter_ready_check(p_reg);
}

#if NRFY_GRTC_HAS_RTCOUNTER
/**
 * @brief Function for returning the RTCOUNTER 32 kHz value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return RTCOUNTER value.
 */
NRFY_STATIC_INLINE uint64_t nrfy_grtc_rt_counter_get(NRF_GRTC_Type const * p_reg)
{
    return __nrfy_internal_grtc_rt_counter_read(p_reg);
}
#endif // NRFY_GRTC_HAS_RTCOUNTER

/**
 * @brief Function for enabling the GRTC compare event and optionally associated interrupt.
 *
 * @note Event is implicitly cleared before enabling the associated interrupt.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel Channel representing the GRTC compare event.
 * @param[in] enable  True if associated interrupt is to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE
void nrfy_grtc_sys_counter_compare_event_int_clear_enable(NRF_GRTC_Type * p_reg,
                                                          uint8_t         channel,
                                                          bool            enable)
{
    nrf_grtc_event_t event = nrf_grtc_sys_counter_compare_event_get(channel);

    if (enable)
    {
        nrf_grtc_event_clear(p_reg, event);
        nrf_barrier_w();
        nrf_grtc_int_enable(p_reg, NRFY_EVENT_TO_INT_BITMASK(event));
    }
    nrf_grtc_sys_counter_compare_event_enable(p_reg, channel);
    nrf_barrier_w();
}

/**
 * @brief Function for retrieving the state of the compare GRTC event.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Compare channel of the corresponding event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRFY_STATIC_INLINE bool nrfy_grtc_sys_counter_compare_event_check(NRF_GRTC_Type const * p_reg,
                                                                  uint8_t               cc_channel)
{
    nrf_barrier_r();
    bool check = nrf_grtc_event_check(p_reg, nrf_grtc_sys_counter_compare_event_get(cc_channel));
    nrf_barrier_r();
    return check;
}

/**
 * @brief Function for clearing a compare GRTC event.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Compare channel of the corresponding event to be cleared.
 */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_compare_event_clear(NRF_GRTC_Type * p_reg,
                                                                  uint8_t         cc_channel)
{
    nrf_grtc_event_clear(p_reg, nrf_grtc_sys_counter_compare_event_get(cc_channel));
    nrf_barrier_w();
}

/**
 * @brief Function for setting the subscribe configuration for a given
 *        GRTC capture channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Compare channel for which to set the configuration.
 * @param[in] channel    Channel through which to subscribe events.
 */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_capture_subscribe_set(NRF_GRTC_Type * p_reg,
                                                                    uint8_t         cc_channel,
                                                                    uint8_t         channel)
{
    nrf_grtc_subscribe_set(p_reg, nrf_grtc_sys_counter_capture_task_get(cc_channel), channel);
    nrf_barrier_w();
}

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        GRTC capture channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Compare channel for which to clear the configuration.
 */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_capture_subscribe_clear(NRF_GRTC_Type * p_reg,
                                                                      uint8_t         cc_channel)
{
    nrf_grtc_subscribe_clear(p_reg, nrf_grtc_sys_counter_capture_task_get(cc_channel));
    nrf_barrier_w();
}

/**
 * @brief Function for setting the publish configuration for a given
 *        GRTC compare channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Compare channel for which to set the configuration.
 * @param[in] channel    Channel through which to publish the event.
 */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_compare_publish_set(NRF_GRTC_Type * p_reg,
                                                                  uint8_t         cc_channel,
                                                                  uint8_t         channel)
{
    nrf_grtc_publish_set(p_reg, nrf_grtc_sys_counter_compare_event_get(cc_channel), channel);
    nrf_barrier_w();
}

/**
 * @brief Function for clearing the publish configuration for a given
 *        GRTC compare channel.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] cc_channel Compare channel for which to clear the configuration.
 */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_compare_publish_clear(NRF_GRTC_Type * p_reg,
                                                                    uint8_t         cc_channel)
{
    nrf_grtc_publish_clear(p_reg, nrf_grtc_sys_counter_compare_event_get(cc_channel));
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_cc_set} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_cc_set(NRF_GRTC_Type * p_reg,
                                                     uint8_t         cc_channel,
                                                     uint64_t        cc_value)
{
    nrf_grtc_sys_counter_cc_set(p_reg, cc_channel, cc_value);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_cc_get} */
NRFY_STATIC_INLINE uint64_t nrfy_grtc_sys_counter_cc_get(NRF_GRTC_Type const * p_reg,
                                                         uint8_t               cc_channel)
{
    nrf_barrier_rw();
    uint64_t cc = nrf_grtc_sys_counter_cc_get(p_reg, cc_channel);
    nrf_barrier_r();
    return cc;
}

/** @refhal{nrf_grtc_sys_counter_cc_add_set} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_cc_add_set(NRF_GRTC_Type *             p_reg,
                                                         uint8_t                     cc_channel,
                                                         uint32_t                    value,
                                                         nrf_grtc_cc_add_reference_t reference)
{
    nrf_grtc_sys_counter_cc_add_set(p_reg, cc_channel, value, reference);
    nrf_barrier_w();
}

#if NRFY_GRTC_HAS_RTCOUNTER
/** @refhal{nrf_grtc_rt_counter_cc_set} */
NRFY_STATIC_INLINE void nrfy_grtc_rt_counter_cc_set(NRF_GRTC_Type * p_reg,
                                                    uint64_t        cc_value,
                                                    bool            sync)
{
    nrf_grtc_rt_counter_cc_set(p_reg, cc_value, sync);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_rt_counter_cc_get} */
NRFY_STATIC_INLINE uint64_t nrfy_grtc_rt_counter_cc_get(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint64_t cc = nrf_grtc_rt_counter_cc_get(p_reg);
    nrf_barrier_r();
    return cc;
}
#endif // NRFY_GRTC_HAS_RTCOUNTER

/** @refhal{nrf_grtc_int_enable} */
NRFY_STATIC_INLINE void nrfy_grtc_int_enable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    nrf_grtc_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_int_disable} */
NRFY_STATIC_INLINE void nrfy_grtc_int_disable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    nrf_grtc_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_int_enable_check(NRF_GRTC_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_grtc_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_grtc_int_pending_get} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_int_pending_get(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t pending = nrf_grtc_int_pending_get(p_reg);
    nrf_barrier_r();
    return pending;
}

#if NRFY_GRTC_HAS_EXTENDED
/** @refhal{nrf_grtc_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_grtc_shorts_enable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    nrf_grtc_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_grtc_shorts_disable(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    nrf_grtc_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_shorts_set} */
NRFY_STATIC_INLINE void nrfy_grtc_shorts_set(NRF_GRTC_Type * p_reg, uint32_t mask)
{
    nrf_grtc_shorts_set(p_reg, mask);
    nrf_barrier_w();
}
#endif // NRFY_GRTC_HAS_EXTENDED

/** @refhal{nrf_grtc_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_grtc_subscribe_set(NRF_GRTC_Type * p_reg,
                                                nrf_grtc_task_t task,
                                                uint8_t         channel)
{
    nrf_grtc_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_grtc_subscribe_clear(NRF_GRTC_Type * p_reg, nrf_grtc_task_t task)
{
    nrf_grtc_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_publish_set} */
NRFY_STATIC_INLINE void nrfy_grtc_publish_set(NRF_GRTC_Type *  p_reg,
                                              nrf_grtc_event_t event,
                                              uint8_t          channel)
{
    nrf_grtc_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_publish_clear} */
NRFY_STATIC_INLINE void nrfy_grtc_publish_clear(NRF_GRTC_Type * p_reg, nrf_grtc_event_t event)
{
    nrf_grtc_publish_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_event_check} */
NRFY_STATIC_INLINE bool nrfy_grtc_event_check(NRF_GRTC_Type const * p_reg, nrf_grtc_event_t event)
{
    nrf_barrier_r();
    bool check = nrf_grtc_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_grtc_event_clear} */
NRFY_STATIC_INLINE void nrfy_grtc_event_clear(NRF_GRTC_Type * p_reg, nrf_grtc_event_t event)
{
    nrf_grtc_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_overflow_check} */
NRFY_STATIC_INLINE bool nrfy_grtc_sys_counter_overflow_check(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_grtc_sys_counter_overflow_check(p_reg);
    nrf_barrier_r();
    return check;
}

#if NRFY_GRTC_HAS_SYSCOUNTER_ARRAY
/** @refhal{nrf_grtc_sys_counter_overflow_indexed_check} */
NRFY_STATIC_INLINE bool nrfy_grtc_sys_counter_overflow_indexed_check(NRF_GRTC_Type const * p_reg,
                                                                     uint8_t               index)
{
    nrf_barrier_r();
    bool check = nrf_grtc_sys_counter_overflow_indexed_check(p_reg, index);
    nrf_barrier_r();
    return check;
}
#endif // NRFY_GRTC_HAS_SYSCOUNTER_ARRAY

/** @refhal{nrf_grtc_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_event_address_get(NRF_GRTC_Type const * p_reg,
                                                        nrf_grtc_event_t      event)
{
    return nrf_grtc_event_address_get(p_reg, event);
}

/** @refhal{nrf_grtc_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_task_address_get(NRF_GRTC_Type const * p_reg,
                                                       nrf_grtc_task_t       task)
{
    return nrf_grtc_task_address_get(p_reg, task);
}

/** @refhal{nrf_grtc_task_trigger} */
NRFY_STATIC_INLINE void nrfy_grtc_task_trigger(NRF_GRTC_Type * p_reg, nrf_grtc_task_t task)
{
    nrf_grtc_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_capture_task_get} */
NRFY_STATIC_INLINE nrf_grtc_task_t nrfy_grtc_sys_counter_capture_task_get(uint8_t cc_channel)
{
    return nrf_grtc_sys_counter_capture_task_get(cc_channel);
}

/** @refhal{nrf_grtc_sys_counter_compare_event_enable} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_compare_event_enable(NRF_GRTC_Type * p_reg,
                                                                   uint8_t         cc_channel)
{
    nrf_grtc_sys_counter_compare_event_enable(p_reg, cc_channel);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_compare_event_disable} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_compare_event_disable(NRF_GRTC_Type * p_reg,
                                                                    uint8_t         cc_channel)
{
    nrf_grtc_sys_counter_compare_event_disable(p_reg, cc_channel);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_compare_event_get} */
NRFY_STATIC_INLINE nrf_grtc_event_t nrfy_grtc_sys_counter_compare_event_get(uint8_t cc_channel)
{
    return nrf_grtc_sys_counter_compare_event_get(cc_channel);
}

/** @refhal{nrf_grtc_sys_counter_cc_enable_check} */
NRFY_STATIC_INLINE bool nrfy_grtc_sys_counter_cc_enable_check(NRF_GRTC_Type const * p_reg,
                                                              uint8_t               cc_channel)
{
    nrf_barrier_rw();
    bool check = nrf_grtc_sys_counter_cc_enable_check(p_reg, cc_channel);
    nrf_barrier_r();
    return check;
}

#if NRFY_GRTC_HAS_EXTENDED
/** @refhal{nrf_grtc_sys_counter_set} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_set(NRF_GRTC_Type * p_reg, bool enable)
{
    nrf_grtc_sys_counter_set(p_reg, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_auto_mode_set} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_auto_mode_set(NRF_GRTC_Type * p_reg, bool enable)
{
    nrf_grtc_sys_counter_auto_mode_set(p_reg, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_auto_mode_check} */
NRFY_STATIC_INLINE bool nrfy_grtc_sys_counter_auto_mode_check(NRF_GRTC_Type * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_grtc_sys_counter_auto_mode_check(p_reg);
    nrf_barrier_r();
    return check;
}
#endif // NRFY_GRTC_HAS_EXTENDED

#if NRFY_GRTC_HAS_SYSCOUNTER_ARRAY
/** @refhal{nrf_grtc_sys_counter_active_set} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_active_set(NRF_GRTC_Type * p_reg, bool enable)
{
    nrf_grtc_sys_counter_active_set(p_reg, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_active_check} */
NRFY_STATIC_INLINE bool nrfy_grtc_sys_counter_active_check(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_grtc_sys_counter_active_check(p_reg);
    nrf_barrier_r();
    return check;
}
#endif // NRFY_GRTC_HAS_SYSCOUNTER_ARRAY

/** @refhal{nrf_grtc_sys_counter_check} */
NRFY_STATIC_INLINE bool nrfy_grtc_sys_counter_check(NRF_GRTC_Type * p_reg)
{
    return nrf_grtc_sys_counter_check(p_reg);
}

#if NRFY_GRTC_HAS_KEEPRUNNING
/** @refhal{nrf_grtc_sys_counter_active_state_request_set} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_active_state_request_set(NRF_GRTC_Type * p_reg,
                                                                       bool            enable)
{
    nrf_grtc_sys_counter_active_state_request_set(p_reg, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_active_state_request_check} */
NRFY_STATIC_INLINE
bool nrfy_grtc_sys_counter_active_state_request_check(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_grtc_sys_counter_active_state_request_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_grtc_sys_counter_active_state_request_get} */
NRFY_STATIC_INLINE
uint32_t nrfy_grtc_sys_counter_active_state_request_get(NRF_GRTC_Type const * p_reg,
                                                        uint32_t              mask)
{
    nrf_barrier_rw();
    uint32_t request = nrf_grtc_sys_counter_active_state_request_get(p_reg, mask);
    nrf_barrier_r();
    return request;
}
#endif // NRFY_GRTC_HAS_KEEPRUNNING

#if NRFY_GRTC_HAS_EXTENDED
/** @refhal{nrf_grtc_sys_counter_interval_set} */
NRFY_STATIC_INLINE void nrfy_grtc_sys_counter_interval_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    nrf_grtc_sys_counter_interval_set(p_reg, value);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_sys_counter_interval_get} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_sys_counter_interval_get(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t interval = nrf_grtc_sys_counter_interval_get(p_reg);
    nrf_barrier_r();
    return interval;
}

/** @refhal{nrf_grtc_timeout_set} */
NRFY_STATIC_INLINE void nrfy_grtc_timeout_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    nrf_grtc_timeout_set(p_reg, value);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_timeout_get} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_timeout_get(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t timeout = nrf_grtc_timeout_get(p_reg);
    nrf_barrier_r();
    return timeout;
}

/** @refhal{nrf_grtc_waketime_set} */
NRFY_STATIC_INLINE void nrfy_grtc_waketime_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    nrf_grtc_waketime_set(p_reg, value);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_waketime_get} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_waketime_get(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t waketime = nrf_grtc_waketime_get(p_reg);
    nrf_barrier_r();
    return waketime;
}
#endif // NRFY_GRTC_HAS_EXTENDED

#if NRFY_GRTC_HAS_PWM
/** @refhal{nrf_grtc_pwm_compare_set} */
NRFY_STATIC_INLINE void nrfy_grtc_pwm_compare_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    nrf_grtc_pwm_compare_set(p_reg, value);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_pwm_compare_get} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_pwm_compare_get(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t compare = nrf_grtc_pwm_compare_get(p_reg);
    nrf_barrier_r();
    return compare;
}
#endif // NRFY_GRTC_HAS_PWM

#if NRFY_GRTC_HAS_CLKOUT
/** @refhal{nrf_grtc_clkout_set} */
NRFY_STATIC_INLINE void nrfy_grtc_clkout_set(NRF_GRTC_Type *   p_reg,
                                             nrf_grtc_clkout_t clkout,
                                             bool              enable)
{
    nrf_grtc_clkout_set(p_reg, clkout, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_clkout_enable_check} */
NRFY_STATIC_INLINE bool nrfy_grtc_clkout_enable_check(NRF_GRTC_Type const * p_reg,
                                                      nrf_grtc_clkout_t     clkout)
{
    nrf_barrier_rw();
    bool check = nrf_grtc_clkout_enable_check(p_reg, clkout);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_grtc_clkout_divider_set} */
NRFY_STATIC_INLINE void nrfy_grtc_clkout_divider_set(NRF_GRTC_Type * p_reg, uint32_t value)
{
    nrf_grtc_clkout_divider_set(p_reg, value);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_clkout_divider_get} */
NRFY_STATIC_INLINE uint32_t nrfy_grtc_clkout_divider_get(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t divider = nrf_grtc_clkout_divider_get(p_reg);
    nrf_barrier_r();
    return divider;
}
#endif // NRFY_GRTC_HAS_CLKOUT

#if NRFY_GRTC_HAS_CLKSEL
/** @refhal{nrf_grtc_clksel_set} */
NRFY_STATIC_INLINE void nrfy_grtc_clksel_set(NRF_GRTC_Type * p_reg, nrf_grtc_clksel_t clksel)
{
    nrf_grtc_clksel_set(p_reg, clksel);
    nrf_barrier_w();
}

/** @refhal{nrf_grtc_clksel_get} */
NRFY_STATIC_INLINE nrf_grtc_clksel_t nrfy_grtc_clksel_get(NRF_GRTC_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_grtc_clksel_t clksel = nrf_grtc_clksel_get(p_reg);
    nrf_barrier_r();
    return clksel;
}
#endif // NRFY_GRTC_HAS_CLKSEL

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_grtc_event_enabled_clear(NRF_GRTC_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_grtc_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_grtc_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE bool __nrfy_internal_grtc_event_handle(NRF_GRTC_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_grtc_event_t event,
                                                          uint32_t *       p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_grtc_event_check(p_reg, event))
    {
        nrf_grtc_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_grtc_events_process(NRF_GRTC_Type * p_reg,
                                                                uint32_t        mask)
{
    uint32_t event_mask   = 0;
    uint32_t channel_mask = NRF_GRTC_SYSCOUNTER_ALL_CHANNELS_INT_MASK & mask;

    nrf_barrier_r();
    while (channel_mask)
    {
        uint8_t          cc_channel = (uint8_t)NRF_CTZ(channel_mask);
        nrf_grtc_event_t event      = nrf_grtc_sys_counter_compare_event_get(cc_channel);
        (void)__nrfy_internal_grtc_event_handle(p_reg, mask, event, &event_mask);
        channel_mask &= ~NRF_GRTC_CHANNEL_INT_MASK(cc_channel);
    }
#if NRFY_GRTC_HAS_RTCOUNTER
    (void)__nrfy_internal_grtc_event_handle(p_reg,
                                            mask,
                                            NRF_GRTC_EVENT_RTCOMPARE,
                                            &event_mask);
    (void)__nrfy_internal_grtc_event_handle(p_reg,
                                            mask,
                                            NRF_GRTC_EVENT_RTCOMPARESYNC,
                                            &event_mask);
#endif // NRFY_GRTC_HAS_RTCOUNTER
    return event_mask;
}

#if NRFY_GRTC_HAS_RTCOUNTER
NRFY_STATIC_INLINE uint64_t __nrfy_internal_grtc_rt_counter_read(NRF_GRTC_Type const * p_reg)
{
    uint32_t counter_l = nrf_grtc_rt_counter_low_get(p_reg);
    uint32_t counter_h = nrf_grtc_rt_counter_high_get(p_reg);

    nrf_barrier_r();
    return (uint64_t)counter_l | ((uint64_t)counter_h << 32);
}
#endif // NRFY_GRTC_HAS_RTCOUNTER

NRFY_STATIC_INLINE bool __nrfy_internal_grtc_sys_counter_ready_check(NRF_GRTC_Type const * p_reg)
{
#if NRFY_GRTC_HAS_SYSCOUNTER_ARRAY
    nrf_grtc_sys_counter_low_get(p_reg); // Dummy read, required.
    nrf_barrier_r();
    bool check = ((nrf_grtc_sys_counter_high_get(p_reg) & GRTC_SYSCOUNTER_SYSCOUNTERH_BUSY_Msk)
                 >> GRTC_SYSCOUNTER_SYSCOUNTERH_BUSY_Pos) == GRTC_SYSCOUNTER_SYSCOUNTERH_BUSY_Ready;
    nrf_barrier_r();
    return check;
#else
    bool check = nrf_grtc_event_check(p_reg, NRF_GRTC_EVENT_SYSCOUNTERVALID);
    nrf_barrier_r();
    return check;
#endif
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_GRTC_H__
