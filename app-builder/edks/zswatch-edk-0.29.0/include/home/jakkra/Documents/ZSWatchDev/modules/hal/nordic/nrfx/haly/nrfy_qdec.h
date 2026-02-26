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

#ifndef NRFY_QDEC_H__
#define NRFY_QDEC_H__

#include <nrfx.h>
#include <hal/nrf_qdec.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_qdec_event_enabled_clear(NRF_QDEC_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_qdec_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_qdec_event_handle(NRF_QDEC_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_qdec_event_t event,
                                                          uint32_t *       p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_qdec_events_process(NRF_QDEC_Type * p_reg,
                                                                uint32_t        mask);

/**
 * @defgroup nrfy_qdec QDEC HALY
 * @{
 * @ingroup nrf_qdec
 * @brief   Hardware access layer with cache and barrier support for managing the QDEC peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @brief Configuration structure for QDEC pins. */
typedef struct
{
    uint32_t a_pin;   /**< Pin number for A input. */
    uint32_t b_pin;   /**< Pin number for B input. */
    uint32_t led_pin; /**< Pin number for LED output. */
} nrfy_qdec_pins_t;

/** @brief QDEC configuration structure. */
typedef struct
{
    nrf_qdec_reportper_t reportper;     /**< Report period in samples. */
    nrf_qdec_sampleper_t sampleper;     /**< Sampling period in microseconds. */
    nrfy_qdec_pins_t     pins;          /**< Pin configuration structure. */
    uint32_t             ledpre;        /**< Time (in microseconds) how long LED is switched on before sampling. */
    nrf_qdec_ledpol_t    ledpol;        /**< Active LED polarity. */
    bool                 dbfen;         /**< State of debouncing filter. */
    bool                 skip_psel_cfg; /**< Skip pin selection configuration.
                                             When set to true, the driver does not modify
                                             pin select registers in the peripheral.
                                             Those registers are supposed to be set up
                                             externally before the driver is initialized.
                                             @note When both GPIO configuration and pin
                                             selection are to be skipped, the structure
                                             fields that specify pins can be omitted,
                                             as they are ignored anyway. */
} nrfy_qdec_config_t;

/**
 * @brief Function for configuring the QDEC.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_qdec_periph_configure(NRF_QDEC_Type *            p_reg,
                                                   nrfy_qdec_config_t const * p_config)
{
    nrf_qdec_sampleper_set(p_reg, p_config->sampleper);
    nrf_qdec_reportper_set(p_reg, p_config->reportper);

    if (p_config->pins.led_pin != NRF_QDEC_PIN_NOT_CONNECTED)
    {
        nrf_qdec_ledpre_set(p_reg, p_config->ledpre);
        nrf_qdec_ledpol_set(p_reg, p_config->ledpol);
    }
    else
    {
        nrf_qdec_ledpre_set(p_reg, NRF_QDEC_LEDPRE_DEFAULT);
    }

    if (!p_config->skip_psel_cfg)
    {
        nrf_qdec_pins_set(p_reg,
                          p_config->pins.a_pin,
                          p_config->pins.b_pin,
                          p_config->pins.led_pin);
    }

    if (p_config->dbfen)
    {
        nrf_qdec_dbfen_enable(p_reg);
    }
    else
    {
        nrf_qdec_dbfen_disable(p_reg);
    }

    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified QDEC interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_qdec_int_init(NRF_QDEC_Type * p_reg,
                                           uint32_t        mask,
                                           uint8_t         irq_priority,
                                           bool            enable)
{
    __nrfy_internal_qdec_event_enabled_clear(p_reg, mask, NRF_QDEC_EVENT_SAMPLERDY);
    __nrfy_internal_qdec_event_enabled_clear(p_reg, mask, NRF_QDEC_EVENT_REPORTRDY);
    __nrfy_internal_qdec_event_enabled_clear(p_reg, mask, NRF_QDEC_EVENT_ACCOF);
#if NRF_QDEC_HAS_EVENT_DBLRDY
    __nrfy_internal_qdec_event_enabled_clear(p_reg, mask, NRF_QDEC_EVENT_DBLRDY);
#endif
#if NRF_QDEC_HAS_EVENT_STOPPED
    __nrfy_internal_qdec_event_enabled_clear(p_reg, mask, NRF_QDEC_EVENT_STOPPED);
#endif

    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));

    if (enable)
    {
        nrf_qdec_int_enable(p_reg, mask);
    }

    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the QDEC interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_qdec_int_uninit(NRF_QDEC_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified QDEC events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_events_process(NRF_QDEC_Type * p_reg,
                                                     uint32_t        mask)
{
    uint32_t evt_mask = __nrfy_internal_qdec_events_process(p_reg, mask);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for reading QDEC accumulators.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_acc    Pointer to store the accumulated transitions
 * @param[in] p_accdbl Pointer to store the accumulated double transitions.
 */
NRFY_STATIC_INLINE void nrfy_qdec_accumulators_read(NRF_QDEC_Type const * p_reg,
                                                    int32_t *             p_acc,
                                                    uint32_t *            p_accdbl)
{
    nrf_barrier_r();
    *p_acc    = nrf_qdec_accread_get(p_reg);
    *p_accdbl = nrf_qdec_accdblread_get(p_reg);
    nrf_barrier_r();
}

/**
 * @brief Function for reading QDEC pins.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_pins Pointer to the QDEC pin configurartion structure.
 */
NRFY_STATIC_INLINE void nrfy_qdec_pins_get(NRF_QDEC_Type const * p_reg,
                                           nrfy_qdec_pins_t *    p_pins)
{
    nrf_barrier_rw();
    p_pins->a_pin   = nrf_qdec_phase_a_pin_get(p_reg);
    p_pins->b_pin   = nrf_qdec_phase_b_pin_get(p_reg);
    p_pins->led_pin = nrf_qdec_led_pin_get(p_reg);
    nrf_barrier_r();
}

/**
 * @brief Function for setting QDEC pins.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_pins Pointer to the QDEC pin configurartion structure.
 */
NRFY_STATIC_INLINE void nrfy_qdec_pins_set(NRF_QDEC_Type *          p_reg,
                                           nrfy_qdec_pins_t const * p_pins)
{
    nrf_qdec_pins_set(p_reg, p_pins->a_pin, p_pins->b_pin, p_pins->led_pin);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_enable} */
NRFY_STATIC_INLINE void nrfy_qdec_enable(NRF_QDEC_Type * p_reg)
{
    nrf_qdec_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_disable} */
NRFY_STATIC_INLINE void nrfy_qdec_disable(NRF_QDEC_Type * p_reg)
{
    nrf_qdec_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_enable_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_enable_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_qdec_enable_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_int_enable} */
NRFY_STATIC_INLINE void nrfy_qdec_int_enable(NRF_QDEC_Type * p_reg, uint32_t mask)
{
    nrf_qdec_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_int_disable} */
NRFY_STATIC_INLINE void nrfy_qdec_int_disable(NRF_QDEC_Type * p_reg, uint32_t mask)
{
    nrf_qdec_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_int_enable_check(NRF_QDEC_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_qdec_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_dbfen_enable} */
NRFY_STATIC_INLINE void nrfy_qdec_dbfen_enable(NRF_QDEC_Type * p_reg)
{
    nrf_qdec_dbfen_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_dbfen_disable} */
NRFY_STATIC_INLINE void nrfy_qdec_dbfen_disable(NRF_QDEC_Type * p_reg)
{
    nrf_qdec_dbfen_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_dbfen_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_dbfen_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_qdec_dbfen_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_phase_a_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_a_pin_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_qdec_phase_a_pin_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_phase_b_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_b_pin_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_qdec_phase_b_pin_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_led_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_led_pin_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_qdec_led_pin_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_task_trigger} */
NRFY_STATIC_INLINE void nrfy_qdec_task_trigger(NRF_QDEC_Type * p_reg, nrf_qdec_task_t task)
{
    nrf_qdec_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_task_address_get(NRF_QDEC_Type const * p_reg,
                                                       nrf_qdec_task_t       task)
{
    return nrf_qdec_task_address_get(p_reg, task);
}

/** @refhal{nrf_qdec_event_clear} */
NRFY_STATIC_INLINE void nrfy_qdec_event_clear(NRF_QDEC_Type * p_reg, nrf_qdec_event_t event)
{
    nrf_qdec_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_event_check} */
NRFY_STATIC_INLINE bool nrfy_qdec_event_check(NRF_QDEC_Type const * p_reg, nrf_qdec_event_t event)
{
    nrf_barrier_rw();
    bool ret = nrf_qdec_event_check(p_reg, event);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_event_address_get(NRF_QDEC_Type const * p_reg,
                                                        nrf_qdec_event_t      event)
{
    return nrf_qdec_event_address_get(p_reg, event);
}

/** @refhal{nrf_qdec_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_qdec_shorts_enable(NRF_QDEC_Type * p_reg, uint32_t mask)
{
    nrf_qdec_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_qdec_shorts_disable(NRF_QDEC_Type * p_reg, uint32_t mask)
{
    nrf_qdec_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_sampleper_to_value} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_sampleper_to_value(nrf_qdec_sampleper_t sampleper)
{
    return nrf_qdec_sampleper_to_value(sampleper);
}

/** @refhal{nrf_qdec_sampleper_set} */
NRFY_STATIC_INLINE void nrfy_qdec_sampleper_set(NRF_QDEC_Type *      p_reg,
                                                nrf_qdec_sampleper_t sampleper)
{
    nrf_qdec_sampleper_set(p_reg, sampleper);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_sampleper_get} */
NRFY_STATIC_INLINE nrf_qdec_sampleper_t nrfy_qdec_sampleper_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_qdec_sampleper_t ret = nrf_qdec_sampleper_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_sample_get} */
NRFY_STATIC_INLINE int32_t nrfy_qdec_sample_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    int32_t ret = nrf_qdec_sample_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_acc_get} */
NRFY_STATIC_INLINE int32_t nrfy_qdec_acc_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_r();
    int32_t ret = nrf_qdec_acc_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_accread_get} */
NRFY_STATIC_INLINE int32_t nrfy_qdec_accread_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_r();
    int32_t ret = nrf_qdec_accread_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_accdbl_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_accdbl_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t ret = nrf_qdec_accdbl_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_accdblread_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_accdblread_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t ret = nrf_qdec_accdblread_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_ledpre_set} */
NRFY_STATIC_INLINE void nrfy_qdec_ledpre_set(NRF_QDEC_Type * p_reg, uint32_t time_us)
{
    nrf_qdec_ledpre_set(p_reg, time_us);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_ledpre_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_ledpre_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_qdec_ledpre_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_ledpol_set} */
NRFY_STATIC_INLINE void nrfy_qdec_ledpol_set(NRF_QDEC_Type * p_reg, nrf_qdec_ledpol_t pol)
{
    nrf_qdec_ledpol_set(p_reg, pol);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_ledpol_get} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_ledpol_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_qdec_ledpol_get(p_reg);
    nrf_barrier_r();

    return ret;
}

/** @refhal{nrf_qdec_reportper_set} */
NRFY_STATIC_INLINE void nrfy_qdec_reportper_set(NRF_QDEC_Type *      p_reg,
                                                nrf_qdec_reportper_t reportper)
{
    nrf_qdec_reportper_set(p_reg, reportper);
    nrf_barrier_w();
}

/** @refhal{nrf_qdec_reportper_get} */
NRFY_STATIC_INLINE nrf_qdec_reportper_t nrfy_qdec_reportper_get(NRF_QDEC_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_qdec_reportper_t ret = nrf_qdec_reportper_get(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_qdec_reportper_to_value} */
NRFY_STATIC_INLINE uint32_t nrfy_qdec_reportper_to_value(nrf_qdec_reportper_t reportper)
{
    return nrf_qdec_reportper_to_value(reportper);
}

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_qdec_event_enabled_clear(NRF_QDEC_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_qdec_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_qdec_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE bool __nrfy_internal_qdec_event_handle(NRF_QDEC_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_qdec_event_t event,
                                                          uint32_t *       p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_qdec_event_check(p_reg, event))
    {
        nrf_qdec_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_qdec_events_process(NRF_QDEC_Type * p_reg,
                                                                uint32_t        mask)
{
    uint32_t event_mask = 0;

    nrf_barrier_r();
    (void)__nrfy_internal_qdec_event_handle(p_reg,
                                            mask,
                                            NRF_QDEC_EVENT_SAMPLERDY,
                                            &event_mask);
    (void)__nrfy_internal_qdec_event_handle(p_reg,
                                            mask,
                                            NRF_QDEC_EVENT_REPORTRDY,
                                            &event_mask);
    (void)__nrfy_internal_qdec_event_handle(p_reg,
                                            mask,
                                            NRF_QDEC_EVENT_ACCOF,
                                            &event_mask);
#if NRF_QDEC_HAS_EVENT_DBLRDY
    (void)__nrfy_internal_qdec_event_handle(p_reg,
                                            mask,
                                            NRF_QDEC_EVENT_DBLRDY,
                                            &event_mask);
#endif
#if NRF_QDEC_HAS_EVENT_STOPPED
    (void)__nrfy_internal_qdec_event_handle(p_reg,
                                            mask,
                                            NRF_QDEC_EVENT_STOPPED,
                                            &event_mask);
#endif
    return event_mask;
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_QDEC_H__
