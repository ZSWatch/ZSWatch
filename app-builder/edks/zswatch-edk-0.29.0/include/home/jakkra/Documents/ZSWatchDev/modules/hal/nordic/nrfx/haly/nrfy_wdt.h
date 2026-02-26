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

#ifndef NRFY_WDT_H__
#define NRFY_WDT_H__

#include <nrfx.h>
#include <hal/nrf_wdt.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE bool __nrfy_internal_wdt_event_handle(NRF_WDT_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_wdt_event_t event,
                                                         uint32_t *      p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_wdt_events_process(NRF_WDT_Type * p_reg, uint32_t mask);

NRFY_STATIC_INLINE void __nrfy_internal_wdt_event_enabled_clear(NRF_WDT_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_wdt_event_t event);

/**
 * @defgroup nrfy_wdt WDT HALY
 * @{
 * @ingroup nrf_wdt
 * @brief   Hardware access layer with cache and barrier support for managing the WDT peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

#if NRF_WDT_HAS_STOP || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_WDT_HAS_STOP} */
#define NRFY_WDT_HAS_STOP 1
#else
#define NRFY_WDT_HAS_STOP 0
#endif

/** @brief WDT configuration structure. */
typedef struct
{
    uint32_t behaviour;    ///< Watchdog behaviour flags bitmask, constructed from @ref nrf_wdt_behaviour_mask_t.
    uint32_t reload_value; ///< Watchdog counter initial value.
} nrfy_wdt_config_t;

/**
 * @brief Function for configuring the WDT.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_wdt_periph_configure(NRF_WDT_Type *            p_reg,
                                                  nrfy_wdt_config_t const * p_config)
{
    nrf_wdt_behaviour_set(p_reg, p_config->behaviour);
    nrf_wdt_reload_value_set(p_reg, p_config->reload_value);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified WDT interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_wdt_int_init(NRF_WDT_Type * p_reg,
                                          uint32_t       mask,
                                          uint8_t        irq_priority,
                                          bool           enable)
{
    __nrfy_internal_wdt_event_enabled_clear(p_reg, mask, NRF_WDT_EVENT_TIMEOUT);
#if NRFY_WDT_HAS_STOP
    __nrfy_internal_wdt_event_enabled_clear(p_reg, mask, NRF_WDT_EVENT_STOPPED);
#endif
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));

    if (enable)
    {
        nrf_wdt_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the WDT interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_wdt_int_uninit(NRF_WDT_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified WDT events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_wdt_events_process(NRF_WDT_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = __nrfy_internal_wdt_events_process(p_reg, mask);
    nrf_barrier_w();
    return evt_mask;
}

/** @refhal{nrf_wdt_task_trigger} */
NRFY_STATIC_INLINE void nrfy_wdt_task_trigger(NRF_WDT_Type * p_reg, nrf_wdt_task_t task)
{
    nrf_wdt_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_wdt_task_address_get(NRF_WDT_Type const * p_reg,
                                                      nrf_wdt_task_t       task)
{
    return nrf_wdt_task_address_get(p_reg, task);
}

/** @refhal{nrf_wdt_event_clear} */
NRFY_STATIC_INLINE void nrfy_wdt_event_clear(NRF_WDT_Type * p_reg, nrf_wdt_event_t event)
{
    nrf_wdt_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_event_check} */
NRFY_STATIC_INLINE bool nrfy_wdt_event_check(NRF_WDT_Type const * p_reg, nrf_wdt_event_t event)
{
    nrf_barrier_r();
    bool check = nrf_wdt_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_wdt_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_wdt_event_address_get(NRF_WDT_Type const * p_reg,
                                                       nrf_wdt_event_t      event)
{
    return nrf_wdt_event_address_get(p_reg, event);
}

/** @refhal{nrf_wdt_int_enable} */
NRFY_STATIC_INLINE void nrfy_wdt_int_enable(NRF_WDT_Type * p_reg, uint32_t mask)
{
    nrf_wdt_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_wdt_int_enable_check(NRF_WDT_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_wdt_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_wdt_int_disable} */
NRFY_STATIC_INLINE void nrfy_wdt_int_disable(NRF_WDT_Type * p_reg, uint32_t mask)
{
    nrf_wdt_int_disable(p_reg, mask);
    nrf_barrier_w();
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_wdt_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_wdt_subscribe_set(NRF_WDT_Type * p_reg,
                                               nrf_wdt_task_t task,
                                               uint8_t        channel)
{
    nrf_wdt_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_wdt_subscribe_clear(NRF_WDT_Type * p_reg, nrf_wdt_task_t task)
{
    nrf_wdt_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_publish_set} */
NRFY_STATIC_INLINE void nrfy_wdt_publish_set(NRF_WDT_Type *  p_reg,
                                             nrf_wdt_event_t event,
                                             uint8_t         channel)
{
    nrf_wdt_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_publish_clear} */
NRFY_STATIC_INLINE void nrfy_wdt_publish_clear(NRF_WDT_Type * p_reg, nrf_wdt_event_t event)
{
    nrf_wdt_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/** @refhal{nrf_wdt_behaviour_set} */
NRFY_STATIC_INLINE void nrfy_wdt_behaviour_set(NRF_WDT_Type * p_reg, uint32_t mask)
{
    nrf_wdt_behaviour_set(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_started_check} */
NRFY_STATIC_INLINE bool nrfy_wdt_started_check(NRF_WDT_Type const * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_wdt_started_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_wdt_request_status_check} */
NRFY_STATIC_INLINE bool nrfy_wdt_request_status_check(NRF_WDT_Type const *  p_reg,
                                                      nrf_wdt_rr_register_t rr_register)
{
    nrf_barrier_r();
    bool check = nrf_wdt_request_status_check(p_reg, rr_register);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_wdt_request_status_get} */
NRFY_STATIC_INLINE uint32_t nrfy_wdt_request_status_get(NRF_WDT_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t ret = nrf_wdt_request_status_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_wdt_reload_value_set} */
NRFY_STATIC_INLINE void nrfy_wdt_reload_value_set(NRF_WDT_Type * p_reg, uint32_t reload_value)
{
    nrf_wdt_reload_value_set(p_reg, reload_value);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_reload_value_get} */
NRFY_STATIC_INLINE uint32_t nrfy_wdt_reload_value_get(NRF_WDT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_wdt_reload_value_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_wdt_reload_request_enable} */
NRFY_STATIC_INLINE void nrfy_wdt_reload_request_enable(NRF_WDT_Type *       p_reg,
                                                       nrf_wdt_rr_register_t rr_register)
{
    nrf_wdt_reload_request_enable(p_reg, rr_register);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_reload_request_disable} */
NRFY_STATIC_INLINE void nrfy_wdt_reload_request_disable(NRF_WDT_Type *       p_reg,
                                                        nrf_wdt_rr_register_t rr_register)
{
    nrf_wdt_reload_request_disable(p_reg, rr_register);
    nrf_barrier_w();
}

/** @refhal{nrf_wdt_reload_request_enable_check} */
NRFY_STATIC_INLINE bool nrfy_wdt_reload_request_enable_check(NRF_WDT_Type const *  p_reg,
                                                             nrf_wdt_rr_register_t rr_register)
{
    nrf_barrier_rw();
    bool check = nrf_wdt_reload_request_enable_check(p_reg, rr_register);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_wdt_reload_request_set} */
NRFY_STATIC_INLINE void nrfy_wdt_reload_request_set(NRF_WDT_Type *        p_reg,
                                                    nrf_wdt_rr_register_t rr_register)
{
    nrf_wdt_reload_request_set(p_reg, rr_register);
    nrf_barrier_w();
}

#if NRFY_WDT_HAS_STOP
/** @refhal{nrf_wdt_task_stop_enable_set} */
NRFY_STATIC_INLINE void nrfy_wdt_task_stop_enable_set(NRF_WDT_Type * p_reg, bool enable)
{
    nrf_wdt_task_stop_enable_set(p_reg, enable);
    nrf_barrier_w();
}
#endif

/** @} */

NRFY_STATIC_INLINE bool __nrfy_internal_wdt_event_handle(NRF_WDT_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_wdt_event_t event,
                                                         uint32_t *      p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_wdt_event_check(p_reg, event))
    {
        nrf_wdt_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_wdt_events_process(NRF_WDT_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    (void)__nrfy_internal_wdt_event_handle(p_reg, mask, NRF_WDT_EVENT_TIMEOUT, &evt_mask);
#if NRFY_WDT_HAS_STOP
    (void)__nrfy_internal_wdt_event_handle(p_reg, mask, NRF_WDT_EVENT_STOPPED, &evt_mask);
#endif

    return evt_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_wdt_event_enabled_clear(NRF_WDT_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_wdt_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_wdt_event_clear(p_reg, event);
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_WDT_H__
