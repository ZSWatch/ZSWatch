/*
 * Copyright (c) 2012 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFY_TEMP_H__
#define NRFY_TEMP_H__

#include <nrfx.h>
#include <hal/nrf_temp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @defgroup nrfy_temp TEMP HALY
* @{
* @ingroup nrf_temp
* @brief   Hardware access layer with cache and barrier support
           for managing the Temperature sensor (TEMP).
* @note    Extended Hardware Access Layer (HALY) is deprecated.
*/

#if NRF_TEMP_HAS_CALIBRATION || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_TEMP_HAS_CALIBRATION} */
#define NRFY_TEMP_HAS_CALIBRATION 1
#else
#define NRFY_TEMP_HAS_CALIBRATION 0
#endif

/**
 * @brief Function for initializing the specified TEMP interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_temp_int_init(NRF_TEMP_Type * p_reg,
                                           uint32_t        mask,
                                           uint8_t         irq_priority,
                                           bool            enable)
{
    (void)mask;
    nrf_temp_event_clear(p_reg, NRF_TEMP_EVENT_DATARDY);
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_temp_int_enable(p_reg, NRF_TEMP_INT_DATARDY_MASK);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the TEMP interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_temp_int_uninit(NRF_TEMP_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/** @refhal{nrf_temp_int_enable} */
NRFY_STATIC_INLINE void nrfy_temp_int_enable(NRF_TEMP_Type * p_reg, uint32_t mask)
{
    nrf_temp_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_temp_int_disable} */
NRFY_STATIC_INLINE void nrfy_temp_int_disable(NRF_TEMP_Type * p_reg, uint32_t mask)
{
    nrf_temp_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_temp_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_temp_int_enable_check(NRF_TEMP_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_temp_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_temp_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_temp_task_address_get(NRF_TEMP_Type const * p_reg,
                                                       nrf_temp_task_t       task)
{
    return nrf_temp_task_address_get(p_reg, task);
}

/** @refhal{nrf_temp_task_trigger} */
NRFY_STATIC_INLINE void nrfy_temp_task_trigger(NRF_TEMP_Type * p_reg, nrf_temp_task_t task)
{
    nrf_temp_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_temp_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_temp_event_address_get(NRF_TEMP_Type const * p_reg,
                                                        nrf_temp_event_t      event)
{
    return nrf_temp_event_address_get(p_reg, event);
}

/** @refhal{nrf_temp_event_clear} */
NRFY_STATIC_INLINE void nrfy_temp_event_clear(NRF_TEMP_Type * p_reg, nrf_temp_event_t event)
{
    nrf_temp_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_temp_event_check} */
NRFY_STATIC_INLINE bool nrfy_temp_event_check(NRF_TEMP_Type const * p_reg, nrf_temp_event_t event)
{
    nrf_barrier_rw();
    uint32_t check = nrf_temp_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_temp_result_get} */
NRFY_STATIC_INLINE int32_t nrfy_temp_result_get(NRF_TEMP_Type const * p_reg)
{
    nrf_barrier_r();
    int32_t temperature = nrf_temp_result_get(p_reg);
    nrf_barrier_r();
    return temperature;
}

#if NRFY_TEMP_HAS_CALIBRATION
/** @refhal{nrf_temp_calibration_coeff_set} */
NRFY_STATIC_INLINE void nrfy_temp_calibration_coeff_set(NRF_TEMP_Type * p_reg, uint32_t coeff)
{
    nrf_temp_calibration_coeff_set(p_reg, coeff);
    nrf_barrier_w();
}

/** @refhal{nrf_temp_calibration_coeff_get} */
NRFY_STATIC_INLINE uint32_t nrfy_temp_calibration_coeff_get(NRF_TEMP_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t coeff = nrf_temp_calibration_coeff_get(p_reg);
    nrf_barrier_r();
    return coeff;
}
#endif

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_temp_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_temp_subscribe_set(NRF_TEMP_Type * p_reg,
                                                nrf_temp_task_t task,
                                                uint8_t         channel)
{
    nrf_temp_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_temp_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_temp_subscribe_clear(NRF_TEMP_Type * p_reg, nrf_temp_task_t task)
{
    nrf_temp_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_temp_publish_set} */
NRFY_STATIC_INLINE void nrfy_temp_publish_set(NRF_TEMP_Type *  p_reg,
                                              nrf_temp_event_t event,
                                              uint8_t          channel)
{
    nrf_temp_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_temp_publish_clear} */
NRFY_STATIC_INLINE void nrfy_temp_publish_clear(NRF_TEMP_Type *  p_reg, nrf_temp_event_t event)
{
    nrf_temp_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFY_TEMP_H__
