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

#ifndef NRF_RNG_H__
#define NRF_RNG_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_rng_hal RNG HAL
 * @{
 * @ingroup nrf_rng
 * @brief   Hardware access layer (HAL) for managing the Random Number Generator (RNG) peripheral.
 */

/** @brief RNG tasks. */
typedef enum
{
    NRF_RNG_TASK_START = offsetof(NRF_RNG_Type, TASKS_START), /**< Start the random number generator. */
    NRF_RNG_TASK_STOP  = offsetof(NRF_RNG_Type, TASKS_STOP)   /**< Stop the random number generator. */
} nrf_rng_task_t;

/** @brief RNG events. */
typedef enum
{
    NRF_RNG_EVENT_VALRDY = offsetof(NRF_RNG_Type, EVENTS_VALRDY) /**< New random number generated event. */
} nrf_rng_event_t;

/** @brief RNG interrupts. */
typedef enum
{
    NRF_RNG_INT_VALRDY_MASK = RNG_INTENSET_VALRDY_Msk /**< Mask for enabling or disabling an interrupt on VALRDY event. */
} nrf_rng_int_mask_t;

/** @brief Types of RNG shortcuts. */
typedef enum
{
    NRF_RNG_SHORT_VALRDY_STOP_MASK = RNG_SHORTS_VALRDY_STOP_Msk /**< Mask for setting shortcut between EVENT_VALRDY and TASK_STOP. */
} nrf_rng_short_mask_t;


/**
 * @brief Function for enabling interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_rng_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_rng_int_enable(NRF_RNG_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_rng_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_rng_int_disable(NRF_RNG_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_rng_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_rng_int_enable_check(NRF_RNG_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for getting the address of the specified task.
 *
 * This function can be used by the PPI module.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] rng_task The specified task.
 *
 * @return Address of the specified task.
 */
NRF_STATIC_INLINE uint32_t nrf_rng_task_address_get(NRF_RNG_Type const * p_reg,
                                                    nrf_rng_task_t       rng_task);

/**
 * @brief Function for triggering the specified task.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] rng_task The specified Task.
 */
NRF_STATIC_INLINE void nrf_rng_task_trigger(NRF_RNG_Type * p_reg, nrf_rng_task_t rng_task);

/**
 * @brief Function for getting address of the specified event.
 *
 * This function can be used by the PPI module.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] rng_event The specified event.
 *
 * @return Address of the specified event.
 */
NRF_STATIC_INLINE uint32_t nrf_rng_event_address_get(NRF_RNG_Type const * p_reg,
                                                     nrf_rng_event_t      rng_event);

/**
 * @brief Function for clearing the specified event.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] rng_event The specified event.
 */
NRF_STATIC_INLINE void nrf_rng_event_clear(NRF_RNG_Type * p_reg, nrf_rng_event_t rng_event);

/**
 * @brief Function for retrieving the state of the specified event.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] rng_event The specified event.
 *
 * @retval true  The event is set.
 * @retval false The event is not set.
 */
NRF_STATIC_INLINE bool nrf_rng_event_check(NRF_RNG_Type const * p_reg, nrf_rng_event_t rng_event);

/**
 * @brief Function for setting shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of shortcuts.
 */
NRF_STATIC_INLINE void nrf_rng_shorts_enable(NRF_RNG_Type * p_reg, uint32_t mask);

/**
 * @brief Function for clearing shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of shortcuts.
 */
NRF_STATIC_INLINE void nrf_rng_shorts_disable(NRF_RNG_Type * p_reg, uint32_t mask);

/**
 * @brief Function for getting the previously generated random value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Previously generated random value.
 */
NRF_STATIC_INLINE uint8_t nrf_rng_random_value_get(NRF_RNG_Type const * p_reg);

/**
 * @brief Function for enabling digital error correction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_rng_error_correction_enable(NRF_RNG_Type * p_reg);

/**
 * @brief Function for disabling digital error correction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_rng_error_correction_disable(NRF_RNG_Type * p_reg);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        RNG task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_rng_subscribe_set(NRF_RNG_Type * p_reg,
                                             nrf_rng_task_t task,
                                             uint8_t        channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        RNG task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_rng_subscribe_clear(NRF_RNG_Type * p_reg,
                                               nrf_rng_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        RNG task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return ENG subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_rng_subscribe_get(NRF_RNG_Type const * p_reg,
                                                 nrf_rng_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        RNG event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_rng_publish_set(NRF_RNG_Type *  p_reg,
                                           nrf_rng_event_t event,
                                           uint8_t         channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        RNG event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_rng_publish_clear(NRF_RNG_Type *  p_reg,
                                             nrf_rng_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        RNG event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return RNG publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_rng_publish_get(NRF_RNG_Type const * p_reg,
                                               nrf_rng_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)


#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_rng_int_enable(NRF_RNG_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_rng_int_disable(NRF_RNG_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_rng_int_enable_check(NRF_RNG_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_rng_task_address_get(NRF_RNG_Type const * p_reg,
                                                    nrf_rng_task_t       rng_task)
{
    return nrf_task_event_address_get(p_reg, rng_task);
}

NRF_STATIC_INLINE void nrf_rng_task_trigger(NRF_RNG_Type * p_reg, nrf_rng_task_t rng_task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)rng_task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_rng_event_address_get(NRF_RNG_Type const * p_reg,
                                                     nrf_rng_event_t      rng_event)
{
    return nrf_task_event_address_get(p_reg, rng_event);
}

NRF_STATIC_INLINE void nrf_rng_event_clear(NRF_RNG_Type * p_reg, nrf_rng_event_t rng_event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)rng_event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)rng_event);
}

NRF_STATIC_INLINE bool nrf_rng_event_check(NRF_RNG_Type const * p_reg, nrf_rng_event_t rng_event)
{
    return nrf_event_check(p_reg, rng_event);
}

NRF_STATIC_INLINE void nrf_rng_shorts_enable(NRF_RNG_Type * p_reg, uint32_t mask)
{
     p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_rng_shorts_disable(NRF_RNG_Type * p_reg, uint32_t mask)
{
     p_reg->SHORTS &= ~mask;
}

NRF_STATIC_INLINE uint8_t nrf_rng_random_value_get(NRF_RNG_Type const * p_reg)
{
    return (uint8_t)(p_reg->VALUE & RNG_VALUE_VALUE_Msk);
}

NRF_STATIC_INLINE void nrf_rng_error_correction_enable(NRF_RNG_Type * p_reg)
{
    p_reg->CONFIG |= RNG_CONFIG_DERCEN_Msk;
}

NRF_STATIC_INLINE void nrf_rng_error_correction_disable(NRF_RNG_Type * p_reg)
{
    p_reg->CONFIG &= ~RNG_CONFIG_DERCEN_Msk;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_rng_subscribe_set(NRF_RNG_Type * p_reg,
                                             nrf_rng_task_t task,
                                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_rng_subscribe_clear(NRF_RNG_Type * p_reg,
                                               nrf_rng_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_rng_subscribe_get(NRF_RNG_Type const * p_reg,
                                                 nrf_rng_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_rng_publish_set(NRF_RNG_Type *  p_reg,
                                           nrf_rng_event_t event,
                                           uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_rng_publish_clear(NRF_RNG_Type *  p_reg,
                                             nrf_rng_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_rng_publish_get(NRF_RNG_Type const * p_reg,
                                               nrf_rng_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_RNG_H__ */
