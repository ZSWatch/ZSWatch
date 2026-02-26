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

#ifndef NRFY_COMP_H__
#define NRFY_COMP_H__

#include <nrfx.h>
#include <hal/nrf_comp.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_comp_event_enabled_clear(NRF_COMP_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_comp_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_comp_event_handle(NRF_COMP_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_comp_event_t event,
                                                          uint32_t *       p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_comp_events_process(NRF_COMP_Type * p_reg,
                                                                uint32_t        mask);

/**
 * @defgroup nrfy_comp COMP HALY
 * @{
 * @ingroup nrf_comp
 * @brief   Hardware access layer with cache and barrier support for managing the COMP peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

#if NRF_COMP_HAS_ISOURCE || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_COMP_HAS_ISOURCE} */
#define NRFY_COMP_HAS_ISOURCE 1
#else
#define NRFY_COMP_HAS_ISOURCE 0
#endif

#if NRF_COMP_HAS_REFTRIM || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_COMP_HAS_REFTRIM} */
#define NRFY_COMP_HAS_REFTRIM 1
#else
#define NRFY_COMP_HAS_REFTRIM 0
#endif

/** @brief COMP configuration structure. */
typedef struct
{
    nrf_comp_ref_t       reference;  ///< Reference selection.
    nrf_comp_ext_ref_t   ext_ref;    ///< External analog reference selection.
    nrf_comp_main_mode_t main_mode;  ///< Main operation mode.
    nrf_comp_th_t        threshold;  ///< Structure holding THDOWN and THUP values needed by the COMP_TH register.
    nrf_comp_sp_mode_t   speed_mode; ///< Speed and power mode.
    nrf_comp_hyst_t      hyst;       ///< Comparator hysteresis.
#if NRFY_COMP_HAS_ISOURCE
    nrf_comp_isource_t   isource;    ///< Current source selected on analog input.
#endif
    nrf_comp_input_t     input;      ///< Input to be monitored.
} nrfy_comp_config_t;

/**
 * @brief Function for configuring the COMP.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_comp_periph_configure(NRF_COMP_Type *            p_reg,
                                                   nrfy_comp_config_t const * p_config)
{
    nrf_comp_ref_set(p_reg, p_config->reference);
    if (p_config->reference == NRF_COMP_REF_AREF)
    {
        nrf_comp_ext_ref_set(p_reg, p_config->ext_ref);
    }
    nrf_comp_th_set(p_reg, p_config->threshold);
    nrf_comp_main_mode_set(p_reg, p_config->main_mode);
    nrf_comp_speed_mode_set(p_reg, p_config->speed_mode);
    nrf_comp_hysteresis_set(p_reg, p_config->hyst);
#if NRF_COMP_HAS_ISOURCE
    nrf_comp_isource_set(p_reg, p_config->isource);
#endif
    nrf_comp_input_select(p_reg, p_config->input);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified COMP interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_comp_int_init(NRF_COMP_Type * p_reg,
                                           uint32_t        mask,
                                           uint8_t         irq_priority,
                                           bool            enable)
{

    __nrfy_internal_comp_event_enabled_clear(p_reg, mask, NRF_COMP_EVENT_READY);
    __nrfy_internal_comp_event_enabled_clear(p_reg, mask, NRF_COMP_EVENT_DOWN);
    __nrfy_internal_comp_event_enabled_clear(p_reg, mask, NRF_COMP_EVENT_UP);
    __nrfy_internal_comp_event_enabled_clear(p_reg, mask, NRF_COMP_EVENT_CROSS);
    nrf_barrier_w();

    NRFY_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFY_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_comp_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the COMP interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
 NRFY_STATIC_INLINE void nrfy_comp_int_uninit(NRF_COMP_Type * p_reg)
 {
    NRFY_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
 }

/**
 * @brief Function for processing the specified COMP events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_comp_events_process(NRF_COMP_Type * p_reg,
                                                     uint32_t        mask)
{
    uint32_t evt_mask = __nrfy_internal_comp_events_process(p_reg, mask);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for reading the current state of the COMP.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval 0 The input voltage is below the threshold.
 * @retval 1 The input voltage is above the threshold.
 */
NRFY_STATIC_INLINE uint32_t nrfy_comp_sample(NRF_COMP_Type * p_reg)
{
    nrf_comp_task_trigger(p_reg, NRF_COMP_TASK_SAMPLE);
    nrf_barrier_rw();
    uint32_t sample = nrf_comp_result_get(p_reg);
    nrf_barrier_r();
    return sample;
}

/** @refhal{nrf_comp_enable} */
NRFY_STATIC_INLINE void nrfy_comp_enable(NRF_COMP_Type * p_reg)
{
    nrf_comp_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_disable} */
NRFY_STATIC_INLINE void nrfy_comp_disable(NRF_COMP_Type * p_reg)
{
    nrf_comp_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_enable_check} */
NRFY_STATIC_INLINE bool nrfy_comp_enable_check(NRF_COMP_Type const * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_comp_enable_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_comp_ref_set} */
NRFY_STATIC_INLINE void nrfy_comp_ref_set(NRF_COMP_Type * p_reg, nrf_comp_ref_t reference)
{
    nrf_comp_ref_set(p_reg, reference);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_ext_ref_set} */
NRFY_STATIC_INLINE void nrfy_comp_ext_ref_set(NRF_COMP_Type * p_reg, nrf_comp_ext_ref_t ext_ref)
{
    nrf_comp_ext_ref_set(p_reg, ext_ref);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_th_set} */
NRFY_STATIC_INLINE void nrfy_comp_th_set(NRF_COMP_Type * p_reg, nrf_comp_th_t threshold)
{
    nrf_comp_th_set(p_reg, threshold);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_main_mode_set} */
NRFY_STATIC_INLINE void nrfy_comp_main_mode_set(NRF_COMP_Type *      p_reg,
                                                nrf_comp_main_mode_t main_mode)
{
    nrf_comp_main_mode_set(p_reg, main_mode);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_speed_mode_set} */
NRFY_STATIC_INLINE void nrfy_comp_speed_mode_set(NRF_COMP_Type *    p_reg,
                                                 nrf_comp_sp_mode_t speed_mode)
{
    nrf_comp_speed_mode_set(p_reg, speed_mode);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_hysteresis_set} */
NRFY_STATIC_INLINE void nrfy_comp_hysteresis_set(NRF_COMP_Type * p_reg, nrf_comp_hyst_t hyst)
{
    nrf_comp_hysteresis_set(p_reg, hyst);
    nrf_barrier_w();
}

#if NRFY_COMP_HAS_ISOURCE
/** @refhal{nrf_comp_isource_set} */
NRFY_STATIC_INLINE void nrfy_comp_isource_set(NRF_COMP_Type * p_reg, nrf_comp_isource_t isource)
{
    nrf_comp_isource_set(p_reg, isource);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_comp_input_select} */
NRFY_STATIC_INLINE void nrfy_comp_input_select(NRF_COMP_Type * p_reg, nrf_comp_input_t input)
{
    nrf_comp_input_select(p_reg, input);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_result_get} */
NRFY_STATIC_INLINE uint32_t nrfy_comp_result_get(NRF_COMP_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t result = nrf_comp_result_get(p_reg);
    nrf_barrier_r();
    return result;
}

/** @refhal{nrf_comp_int_enable} */
NRFY_STATIC_INLINE void nrfy_comp_int_enable(NRF_COMP_Type * p_reg, uint32_t mask)
{
    nrf_comp_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_int_disable} */
NRFY_STATIC_INLINE void nrfy_comp_int_disable(NRF_COMP_Type * p_reg, uint32_t mask)
{
    nrf_comp_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_comp_int_enable_check(NRF_COMP_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_comp_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_comp_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_comp_task_address_get(NRF_COMP_Type const * p_reg,
                                                       nrf_comp_task_t       task)
{
    return nrf_comp_task_address_get(p_reg, task);
}

/** @refhal{nrf_comp_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_comp_event_address_get(NRF_COMP_Type const * p_reg,
                                                        nrf_comp_event_t      event)
{
    return nrf_comp_event_address_get(p_reg, event);
}

/** @refhal{nrf_comp_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_comp_shorts_enable(NRF_COMP_Type * p_reg, uint32_t mask)
{
    nrf_comp_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_comp_shorts_disable(NRF_COMP_Type * p_reg, uint32_t mask)
{
    nrf_comp_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_task_trigger} */
NRFY_STATIC_INLINE void nrfy_comp_task_trigger(NRF_COMP_Type * p_reg, nrf_comp_task_t task)
{
    nrf_comp_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_event_clear} */
NRFY_STATIC_INLINE void nrfy_comp_event_clear(NRF_COMP_Type * p_reg, nrf_comp_event_t event)
{
    nrf_comp_event_clear(p_reg, event);
    nrf_barrier_w();
}

#if NRFY_COMP_HAS_REFTRIM
/** @refhal{nrf_comp_reftrim_set} */
NRFY_STATIC_INLINE void nrfy_comp_reftrim_set(NRF_COMP_Type * p_reg, uint32_t trim)
{
    nrf_comp_reftrim_set(p_reg, trim);
    nrf_barrier_w();
}

/** @refhal{nrf_comp_reftrim_get} */
NRFY_STATIC_INLINE uint32_t nrfy_comp_reftrim_get(NRF_COMP_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t trim = nrf_comp_reftrim_get(p_reg);
    nrf_barrier_r();
    return trim;
}
#endif

/** @refhal{nrf_comp_event_check} */
NRFY_STATIC_INLINE bool nrfy_comp_event_check(NRF_COMP_Type const * p_reg, nrf_comp_event_t event)
{
    nrf_barrier_rw();
    bool check = nrf_comp_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_comp_event_enabled_clear(NRF_COMP_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_comp_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_comp_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE bool __nrfy_internal_comp_event_handle(NRF_COMP_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_comp_event_t event,
                                                          uint32_t *       p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_comp_event_check(p_reg, event))
    {
        nrf_comp_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_comp_events_process(NRF_COMP_Type * p_reg,
                                                                uint32_t        mask)
{
    uint32_t event_mask = 0;

    nrf_barrier_r();
    (void)__nrfy_internal_comp_event_handle(p_reg,
                                            mask,
                                            NRF_COMP_EVENT_READY,
                                            &event_mask);
    (void)__nrfy_internal_comp_event_handle(p_reg,
                                            mask,
                                            NRF_COMP_EVENT_DOWN,
                                            &event_mask);
    (void)__nrfy_internal_comp_event_handle(p_reg,
                                            mask,
                                            NRF_COMP_EVENT_UP,
                                            &event_mask);
    (void)__nrfy_internal_comp_event_handle(p_reg,
                                            mask,
                                            NRF_COMP_EVENT_CROSS,
                                            &event_mask);
    return event_mask;
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_COMP_H__
