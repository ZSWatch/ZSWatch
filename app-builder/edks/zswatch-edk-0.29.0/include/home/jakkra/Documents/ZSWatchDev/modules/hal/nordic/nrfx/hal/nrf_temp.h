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

#ifndef NRF_TEMP_H__
#define NRF_TEMP_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @defgroup nrf_temp_hal TEMP HAL
* @{
* @ingroup nrf_temp
* @brief   Hardware access layer for managing the Temperature sensor (TEMP).
*/

#if defined(TEMP_CALIB_CALIB_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the calibration of temperature measurement is present. */
#define NRF_TEMP_HAS_CALIBRATION 1
#else
#define NRF_TEMP_HAS_CALIBRATION 0
#endif

/** @brief TEMP tasks. */
typedef enum
{
    NRF_TEMP_TASK_START = offsetof(NRF_TEMP_Type, TASKS_START), /**< Start temperature measurement. */
    NRF_TEMP_TASK_STOP  = offsetof(NRF_TEMP_Type, TASKS_STOP)   /**< Stop temperature measurement. */
} nrf_temp_task_t;

/** @brief TEMP events. */
typedef enum
{
    NRF_TEMP_EVENT_DATARDY = offsetof(NRF_TEMP_Type, EVENTS_DATARDY) /**< Temperature measurement complete, data ready. */
} nrf_temp_event_t;

/** @brief TEMP interrupts. */
typedef enum
{
    NRF_TEMP_INT_DATARDY_MASK = TEMP_INTENSET_DATARDY_Msk /**< Interrupt on DATARDY event.  */
} nrf_temp_int_mask_t;

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_temp_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_temp_int_enable(NRF_TEMP_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_temp_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_temp_int_disable(NRF_TEMP_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_temp_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_temp_int_enable_check(NRF_TEMP_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for getting the address of the specified TEMP task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 *
 * @return Address of the requested task register.
 */
NRF_STATIC_INLINE uint32_t nrf_temp_task_address_get(NRF_TEMP_Type const * p_reg,
                                                     nrf_temp_task_t       task);

/**
 * @brief Function for activating the specified TEMP task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_temp_task_trigger(NRF_TEMP_Type * p_reg, nrf_temp_task_t task);

/**
 * @brief Function for getting the address of the specified TEMP event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 *
 * @return Address of the requested event register.
 */
NRF_STATIC_INLINE uint32_t nrf_temp_event_address_get(NRF_TEMP_Type const * p_reg,
                                                      nrf_temp_event_t      event);

/**
 * @brief Function for clearing the specified TEMP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_temp_event_clear(NRF_TEMP_Type *  p_reg, nrf_temp_event_t event);

/**
 * @brief Function for getting the state of a specific event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_temp_event_check(NRF_TEMP_Type const * p_reg, nrf_temp_event_t event);

/**
 * @brief Function for getting the result of temperature measurement.
 *
 * @note Returned value is in 2's complement format, 0.25 °C steps
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Temperature value register contents.
 */
NRF_STATIC_INLINE int32_t nrf_temp_result_get(NRF_TEMP_Type const * p_reg);

#if NRF_TEMP_HAS_CALIBRATION
/**
 * @brief Function for setting the calibration coefficient for the temperature measurement.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] coeff Calibration coefficient.
 */
NRF_STATIC_INLINE void nrf_temp_calibration_coeff_set(NRF_TEMP_Type * p_reg, uint32_t coeff);

/**
 * @brief Function for getting the calibration coefficient for the temperature measurement.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Calibration coefficient.
 */
NRF_STATIC_INLINE uint32_t nrf_temp_calibration_coeff_get(NRF_TEMP_Type const * p_reg);
#endif

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        TEMP task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_temp_subscribe_set(NRF_TEMP_Type * p_reg,
                                              nrf_temp_task_t task,
                                              uint8_t         channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        TEMP task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_temp_subscribe_clear(NRF_TEMP_Type * p_reg,
                                                nrf_temp_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        TEMP task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return TEMP subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_temp_subscribe_get(NRF_TEMP_Type const * p_reg,
                                                  nrf_temp_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        TEMP event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_temp_publish_set(NRF_TEMP_Type *  p_reg,
                                            nrf_temp_event_t event,
                                            uint8_t          channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        TEMP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_temp_publish_clear(NRF_TEMP_Type *  p_reg,
                                              nrf_temp_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        TEMP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return TEMP publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_temp_publish_get(NRF_TEMP_Type const * p_reg,
                                                nrf_temp_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_temp_int_enable(NRF_TEMP_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_temp_int_disable(NRF_TEMP_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_temp_int_enable_check(NRF_TEMP_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_temp_task_address_get(NRF_TEMP_Type const * p_reg,
                                                     nrf_temp_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_temp_task_trigger(NRF_TEMP_Type * p_reg, nrf_temp_task_t task)
{
    *(volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task) = 1;
}

NRF_STATIC_INLINE uint32_t nrf_temp_event_address_get(NRF_TEMP_Type const * p_reg,
                                                      nrf_temp_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_temp_event_clear(NRF_TEMP_Type * p_reg, nrf_temp_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_temp_event_check(NRF_TEMP_Type const * p_reg, nrf_temp_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE int32_t nrf_temp_result_get(NRF_TEMP_Type const * p_reg)
{
    return p_reg->TEMP;
}

#if NRF_TEMP_HAS_CALIBRATION
NRF_STATIC_INLINE void nrf_temp_calibration_coeff_set(NRF_TEMP_Type * p_reg, uint32_t coeff)
{
    p_reg->CALIB = coeff;
}

NRF_STATIC_INLINE uint32_t nrf_temp_calibration_coeff_get(NRF_TEMP_Type const * p_reg)
{
    return p_reg->CALIB;
}
#endif

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_temp_subscribe_set(NRF_TEMP_Type * p_reg,
                                              nrf_temp_task_t task,
                                              uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_temp_subscribe_clear(NRF_TEMP_Type * p_reg,
                                                nrf_temp_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_temp_subscribe_get(NRF_TEMP_Type const * p_reg,
                                                  nrf_temp_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_temp_publish_set(NRF_TEMP_Type *  p_reg,
                                            nrf_temp_event_t event,
                                            uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_temp_publish_clear(NRF_TEMP_Type *  p_reg,
                                              nrf_temp_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_temp_publish_get(NRF_TEMP_Type const * p_reg,
                                                nrf_temp_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_TEMP_H__
