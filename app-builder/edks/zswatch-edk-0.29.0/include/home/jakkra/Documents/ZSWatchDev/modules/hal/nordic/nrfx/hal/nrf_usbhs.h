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

#ifndef NRF_USBHS_H__
#define NRF_USBHS_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_usbhs_hal USBHS HAL
 * @{
 * @ingroup nrf_usbhs
 * @brief   Hardware access layer for managing the Universal Serial Bus High Speed (USBHS) peripheral.
 */

/** @brief USBHS tasks. */
typedef enum
{
    NRF_USBHS_TASK_START = offsetof(NRF_USBHS_Type, TASKS_START), ///< Start the USB peripheral.
} nrf_usbhs_task_t;

/** @brief USBHS events. */
typedef enum
{
    NRF_USBHS_EVENT_CORE = offsetof(NRF_USBHS_Type, EVENTS_CORE), ///< Signal that the USB reset condition is detected on the USB lines.
} nrf_usbhs_event_t;

/** @brief USBHS interrupts. */
typedef enum
{
    NRF_USBHS_INT_USBCORE_MASK = USBHS_INTENSET_CORE_Msk, ///< Interrupt on the USBCORE event.
} nrf_usbhs_int_mask_t;

/**
 * @brief Function for activating the specified USBHS task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_usbhs_task_trigger(NRF_USBHS_Type * p_reg, nrf_usbhs_task_t task);

/**
 * @brief Function for returning the address of the specified USBHS task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  The specified task.
 *
 * @return Task address.
 */
NRF_STATIC_INLINE uint32_t nrf_usbhs_task_address_get(NRF_USBHS_Type const * p_reg,
                                                      nrf_usbhs_task_t       task);

/**
 * @brief Function for clearing the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_usbhs_event_clear(NRF_USBHS_Type * p_reg, nrf_usbhs_event_t event);

/**
 * @brief Function for retrieving the state of the USBHS event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_usbhs_event_check(NRF_USBHS_Type const * p_reg, nrf_usbhs_event_t event);

/**
 * @brief Function for getting and clearing the state of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 *
 * @retval true  The event was set.
 * @retval false The event was not set.
 */
NRF_STATIC_INLINE bool nrf_usbhs_event_get_and_clear(NRF_USBHS_Type * p_reg, nrf_usbhs_event_t event);

/**
 * @brief Function for returning the address of the specified USBHS event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event The specified event.
 *
 * @return Address of the event specified as a function parameter.
 */
NRF_STATIC_INLINE uint32_t nrf_usbhs_event_address_get(NRF_USBHS_Type const * p_reg,
                                                       nrf_usbhs_event_t      event);
/**
 * @brief Function for enabling the selected interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_usbhs_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_usbhs_int_enable(NRF_USBHS_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_usbhs_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_usbhs_int_enable_check(NRF_USBHS_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the information about the enabled interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The flags of the enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_usbhs_int_enable_get(NRF_USBHS_Type const * p_reg);

/**
 * @brief Function for disabling the selected interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_usbhs_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_usbhs_int_disable(NRF_USBHS_Type * p_reg, uint32_t mask);

/**
 * @brief Function for enabling the USBHS.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_usbhs_enable(NRF_USBHS_Type * p_reg);

/**
 * @brief Function for disabling the USBHS.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_usbhs_disable(NRF_USBHS_Type * p_reg);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_usbhs_task_trigger(NRF_USBHS_Type * p_reg, nrf_usbhs_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_usbhs_task_address_get(NRF_USBHS_Type const * p_reg,
                                                      nrf_usbhs_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE void nrf_usbhs_event_clear(NRF_USBHS_Type * p_reg, nrf_usbhs_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_usbhs_event_check(NRF_USBHS_Type const * p_reg, nrf_usbhs_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE bool nrf_usbhs_event_get_and_clear(NRF_USBHS_Type * p_reg, nrf_usbhs_event_t event)
{
    bool ret = nrf_usbhs_event_check(p_reg, event);
    if (ret)
    {
        nrf_usbhs_event_clear(p_reg, event);
    }
    return ret;
}

NRF_STATIC_INLINE uint32_t nrf_usbhs_event_address_get(NRF_USBHS_Type const * p_reg,
                                                       nrf_usbhs_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_usbhs_int_enable(NRF_USBHS_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE uint32_t nrf_usbhs_int_enable_check(NRF_USBHS_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_usbhs_int_enable_get(NRF_USBHS_Type const * p_reg)
{
    return p_reg->INTENSET;
}

NRF_STATIC_INLINE void nrf_usbhs_int_disable(NRF_USBHS_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE void nrf_usbhs_enable(NRF_USBHS_Type * p_reg)
{
    p_reg->ENABLE = (USBHS_ENABLE_PHY_Enabled << USBHS_ENABLE_PHY_Pos) |
                    (USBHS_ENABLE_CORE_Enabled << USBHS_ENABLE_CORE_Pos);
}

NRF_STATIC_INLINE void nrf_usbhs_disable(NRF_USBHS_Type * p_reg)
{
    p_reg->ENABLE = (USBHS_ENABLE_PHY_Disabled << USBHS_ENABLE_PHY_Pos) |
                    (USBHS_ENABLE_CORE_Disabled << USBHS_ENABLE_CORE_Pos);
}

#endif /* NRF_DECLARE_ONLY */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_USBHS_H__ */
