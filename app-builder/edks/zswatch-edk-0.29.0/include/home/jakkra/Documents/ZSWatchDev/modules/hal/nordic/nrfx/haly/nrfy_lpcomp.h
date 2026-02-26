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

#ifndef NRFY_LPCOMP_H__
#define NRFY_LPCOMP_H__

#include <nrfx.h>
#include <hal/nrf_lpcomp.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_lpcomp_event_enabled_clear(NRF_LPCOMP_Type *  p_reg,
                                                                   uint32_t           mask,
                                                                   nrf_lpcomp_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_lpcomp_event_handle(NRF_LPCOMP_Type *  p_reg,
                                                            uint32_t           mask,
                                                            nrf_lpcomp_event_t event,
                                                            uint32_t *         p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_lpcomp_events_process(NRF_LPCOMP_Type * p_reg,
                                                                  uint32_t          mask);

/**
 * @defgroup nrfy_lpcomp LPCOMP HALY
 * @{
 * @ingroup nrf_lpcomp
 * @brief   Hardware access layer with cache and barrier support for managing the LPCOMP peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @brief LPCOMP configuration structure. */
typedef struct
{
    nrf_lpcomp_ref_t     reference; ///< Reference selection.
    nrf_lpcomp_ext_ref_t ext_ref;   ///< External analog reference selection.
    nrf_lpcomp_detect_t  detection; ///< Detection type.
#if NRF_LPCOMP_HAS_HYST
    nrf_lpcomp_hyst_t    hyst;      ///< Comparator hysteresis.
#endif
    nrf_lpcomp_input_t   input;     ///< Input to be monitored.
} nrfy_lpcomp_config_t;

/**
 * @brief Function for configuring the LPCOMP.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_lpcomp_periph_configure(NRF_LPCOMP_Type *            p_reg,
                                                     nrfy_lpcomp_config_t const * p_config)
{
    nrf_lpcomp_ref_set(p_reg, p_config->reference);
    if (p_config->reference == NRF_LPCOMP_REF_EXT_REF)
    {
        nrf_lpcomp_ext_ref_set(p_reg, p_config->ext_ref);
    }
    nrf_lpcomp_detection_set(p_reg, p_config->detection);
#if defined(LPCOMP_FEATURE_HYST_PRESENT)
    nrf_lpcomp_hysteresis_set(p_reg, p_config->hyst);
#endif

    nrf_lpcomp_input_select(p_reg, p_config->input);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified LPCOMP interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_lpcomp_int_init(NRF_LPCOMP_Type * p_reg,
                                             uint32_t          mask,
                                             uint8_t           irq_priority,
                                             bool              enable)
{
    __nrfy_internal_lpcomp_event_enabled_clear(p_reg, mask, NRF_LPCOMP_EVENT_READY);
    __nrfy_internal_lpcomp_event_enabled_clear(p_reg, mask, NRF_LPCOMP_EVENT_DOWN);
    __nrfy_internal_lpcomp_event_enabled_clear(p_reg, mask, NRF_LPCOMP_EVENT_UP);
    __nrfy_internal_lpcomp_event_enabled_clear(p_reg, mask, NRF_LPCOMP_EVENT_CROSS);
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_lpcomp_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the LPCOMP interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_lpcomp_int_uninit(NRF_LPCOMP_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified LPCOMP events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_lpcomp_events_process(NRF_LPCOMP_Type * p_reg,
                                                       uint32_t          mask)
{
    uint32_t evt_mask = __nrfy_internal_lpcomp_events_process(p_reg, mask);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for reading the current state of the LPCOMP.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval 0 The input voltage is below the threshold.
 * @retval 1 The input voltage is above the threshold.
 */
NRFY_STATIC_INLINE uint32_t nrfy_lpcomp_sample(NRF_LPCOMP_Type * p_reg)
{
    nrf_lpcomp_task_trigger(p_reg, NRF_LPCOMP_TASK_SAMPLE);
    nrf_barrier_rw();
    uint32_t sample = nrf_lpcomp_result_get(p_reg);
    nrf_barrier_r();
    return sample;
}

/**
 * @brief Function for reading the current state of the LPCOMP input.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval false The input voltage is below the threshold.
 * @retval true  The input voltage is above the threshold.
 */
NRFY_STATIC_INLINE bool nrfy_lpcomp_sample_check(NRF_LPCOMP_Type * p_reg)
{
    return (bool)nrfy_lpcomp_sample(p_reg);
}

/** @refhal{nrf_lpcomp_ref_set} */
NRFY_STATIC_INLINE void nrfy_lpcomp_ref_set(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_ref_t reference)
{
    nrf_lpcomp_ref_set(p_reg, reference);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_ext_ref_set} */
NRFY_STATIC_INLINE void nrfy_lpcomp_ext_ref_set(NRF_LPCOMP_Type *    p_reg,
                                                nrf_lpcomp_ext_ref_t ext_ref)
{
    nrf_lpcomp_ext_ref_set(p_reg, ext_ref);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_detection_set} */
NRFY_STATIC_INLINE void nrfy_lpcomp_detection_set(NRF_LPCOMP_Type *   p_reg,
                                                  nrf_lpcomp_detect_t detection)
{
    nrf_lpcomp_detection_set(p_reg, detection);
    nrf_barrier_w();
}

#if defined(LPCOMP_FEATURE_HYST_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_lpcomp_hysteresis_set} */
NRFY_STATIC_INLINE void nrfy_lpcomp_hysteresis_set(NRF_LPCOMP_Type * p_reg,
                                                   nrf_lpcomp_hyst_t hyst)
{
    nrf_lpcomp_hysteresis_set(p_reg, hyst);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_lpcomp_input_select} */
NRFY_STATIC_INLINE void nrfy_lpcomp_input_select(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_input_t input)
{
    nrf_lpcomp_input_select(p_reg, input);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_enable} */
NRFY_STATIC_INLINE void nrfy_lpcomp_enable(NRF_LPCOMP_Type * p_reg)
{
    nrf_lpcomp_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_disable} */
NRFY_STATIC_INLINE void nrfy_lpcomp_disable(NRF_LPCOMP_Type * p_reg)
{
    nrf_lpcomp_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_result_get} */
NRFY_STATIC_INLINE uint32_t nrfy_lpcomp_result_get(NRF_LPCOMP_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t result = nrf_lpcomp_result_get(p_reg);
    nrf_barrier_r();
    return result;
}

/** @refhal{nrf_lpcomp_int_enable} */
NRFY_STATIC_INLINE void nrfy_lpcomp_int_enable(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    nrf_lpcomp_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_int_disable} */
NRFY_STATIC_INLINE void nrfy_lpcomp_int_disable(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    nrf_lpcomp_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_lpcomp_int_enable_check(NRF_LPCOMP_Type const * p_reg,
                                                         uint32_t                mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_lpcomp_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_lpcomp_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_lpcomp_task_address_get(NRF_LPCOMP_Type const * p_reg,
                                                         nrf_lpcomp_task_t       task)
{
    return nrf_lpcomp_task_address_get(p_reg, task);
}

/** @refhal{nrf_lpcomp_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_lpcomp_event_address_get(NRF_LPCOMP_Type const * p_reg,
                                                          nrf_lpcomp_event_t      event)
{
    return nrf_lpcomp_event_address_get(p_reg, event);
}

/** @refhal{nrf_lpcomp_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_lpcomp_shorts_enable(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    nrf_lpcomp_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_lpcomp_shorts_disable(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    nrf_lpcomp_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_task_trigger} */
NRFY_STATIC_INLINE void nrfy_lpcomp_task_trigger(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_task_t task)
{
    nrf_lpcomp_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_event_clear} */
NRFY_STATIC_INLINE void nrfy_lpcomp_event_clear(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_event_t event)
{
    nrf_lpcomp_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_lpcomp_event_check} */
NRFY_STATIC_INLINE bool nrfy_lpcomp_event_check(NRF_LPCOMP_Type const * p_reg,
                                                nrf_lpcomp_event_t      event)
{
    nrf_barrier_rw();
    bool check = nrf_lpcomp_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_lpcomp_event_enabled_clear(NRF_LPCOMP_Type *  p_reg,
                                                                   uint32_t           mask,
                                                                   nrf_lpcomp_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_lpcomp_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE bool __nrfy_internal_lpcomp_event_handle(NRF_LPCOMP_Type *  p_reg,
                                                            uint32_t           mask,
                                                            nrf_lpcomp_event_t event,
                                                            uint32_t *         p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_lpcomp_event_check(p_reg, event))
    {
        nrf_lpcomp_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_lpcomp_events_process(NRF_LPCOMP_Type * p_reg,
                                                                  uint32_t          mask)
{
    uint32_t event_mask = 0;

    (void)__nrfy_internal_lpcomp_event_handle(p_reg,
                                              mask,
                                              NRF_LPCOMP_EVENT_READY,
                                              &event_mask);
    (void)__nrfy_internal_lpcomp_event_handle(p_reg,
                                              mask,
                                              NRF_LPCOMP_EVENT_DOWN,
                                              &event_mask);
    (void)__nrfy_internal_lpcomp_event_handle(p_reg,
                                              mask,
                                              NRF_LPCOMP_EVENT_UP,
                                              &event_mask);
    (void)__nrfy_internal_lpcomp_event_handle(p_reg,
                                              mask,
                                              NRF_LPCOMP_EVENT_CROSS,
                                              &event_mask);
    return event_mask;
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_LPCOMP_H__
