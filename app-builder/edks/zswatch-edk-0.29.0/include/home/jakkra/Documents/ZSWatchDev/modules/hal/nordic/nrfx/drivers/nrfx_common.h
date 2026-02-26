/*
 * Copyright (c) 2017 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_COMMON_H__
#define NRFX_COMMON_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

// To be included before bsp/mdk to override default errata values
#include "nrfx_errata.h"

#include <nrfx_bsp.h>
#include "drivers/nrfx_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CORTEX_M) || defined(__NRFX_DOXYGEN__)
#define ISA_ARM     1
#elif defined(__VPR_REV)
#define ISA_RISCV   1
#else
#define ISA_UNKNOWN 1
#endif

#if defined(ISA_RISCV)
#define __STATIC_INLINE __attribute__((always_inline)) static inline
#endif

#ifndef NRFX_STATIC_INLINE
#ifdef NRFX_DECLARE_ONLY
#define NRFX_STATIC_INLINE
#else
#define NRFX_STATIC_INLINE __STATIC_INLINE
#endif
#endif // NRFX_STATIC_INLINE

#define NRFY_STATIC_INLINE __STATIC_INLINE

#ifndef NRF_STATIC_INLINE
#ifdef NRF_DECLARE_ONLY
#define NRF_STATIC_INLINE
#else
#define NRF_STATIC_INLINE __STATIC_INLINE
#endif
#endif // NRF_STATIC_INLINE

/**
 * @defgroup nrfx_common Common module
 * @{
 * @ingroup nrfx
 * @brief Common module.
 */

/** @brief Symbol specifying major number of the current nrfx version. */
#define NRFX_RELEASE_VER_MAJOR 4

/** @brief Symbol specifying minor number of the current nrfx version. */
#define NRFX_RELEASE_VER_MINOR 0

/** @brief Symbol specifying micro number of the current nrfx version. */
#define NRFX_RELEASE_VER_MICRO 1

/** @brief IRQ handler type. */
typedef void (* nrfx_irq_handler_t)(void *);

/** @brief Driver state. */
typedef enum
{
    NRFX_DRV_STATE_UNINITIALIZED, ///< Uninitialized.
    NRFX_DRV_STATE_INITIALIZED,   ///< Initialized but powered off.
    NRFX_DRV_STATE_POWERED_ON,    ///< Initialized and powered on.
} nrfx_drv_state_t;

/**
 * @brief Function for checking if an object is placed in the Data RAM region.
 *
 * Several peripherals (the ones using EasyDMA) require the transfer buffers
 * to be placed in the Data RAM region. This function can be used to check if
 * this condition is met.
 *
 * @param[in] p_object Pointer to an object whose location is to be checked.
 *
 * @retval true  The pointed object is located in the Data RAM region.
 * @retval false The pointed object is not located in the Data RAM region.
 */
NRF_STATIC_INLINE bool nrfx_is_in_ram(void const * p_object);

/**
 * @brief Function for checking if an object is aligned to a 32-bit word
 *
 * Several peripherals (the ones using EasyDMA) require the transfer buffers
 * to be aligned to a 32-bit word. This function can be used to check if
 * this condition is met.
 *
 * @param[in] p_object  Pointer to an object whose location is to be checked.
 *
 * @retval true  The pointed object is aligned to a 32-bit word.
 * @retval false The pointed object is not aligned to a 32-bit word.
 */
NRF_STATIC_INLINE bool nrfx_is_word_aligned(void const * p_object);

/**
 * @brief Function for getting the interrupt number for the specified peripheral.
 *
 * @warning This function is valid only for peripherals with a single interrupt line.
 *
 * @param[in] p_reg Peripheral base pointer.
 *
 * @return Interrupt number associated with the pointed peripheral.
 */
NRF_STATIC_INLINE IRQn_Type nrfx_get_irq_number(void const * p_reg);

/**
 * @brief Function for converting an INTEN register bit position to the
 *        corresponding event identifier.
 *
 * The event identifier is the offset between the event register address and
 * the peripheral base address, and is equal (thus, can be directly cast) to
 * the corresponding value of the enumerated type from HAL (nrf_*_event_t).
 *
 * @param[in] bit INTEN register bit position.
 *
 * @return Event identifier.
 *
 * @sa nrfx_event_to_bitpos
 */
NRF_STATIC_INLINE uint32_t nrfx_bitpos_to_event(uint32_t bit);

/**
 * @brief Function for converting an event identifier to the corresponding
 *        INTEN register bit position.
 *
 * The event identifier is the offset between the event register address and
 * the peripheral base address, and is equal (thus, can be directly cast) to
 * the corresponding value of the enumerated type from HAL (nrf_*_event_t).
 *
 * @param[in] event Event identifier.
 *
 * @return INTEN register bit position.
 *
 * @sa nrfx_bitpos_to_event
 */
NRF_STATIC_INLINE uint32_t nrfx_event_to_bitpos(uint32_t event);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE bool nrfx_is_in_ram(void const * p_object)
{
    return ((((uint32_t)p_object) & 0xE0000000u) == 0x20000000u);
}

NRF_STATIC_INLINE bool nrfx_is_word_aligned(void const * p_object)
{
    return ((((uint32_t)p_object) & 0x3u) == 0u);
}

NRF_STATIC_INLINE IRQn_Type nrfx_get_irq_number(void const * p_reg)
{
    return (IRQn_Type)NRFX_IRQ_NUMBER_GET(p_reg);
}

NRF_STATIC_INLINE uint32_t nrfx_bitpos_to_event(uint32_t bit)
{
    static const uint32_t event_reg_offset = 0x100u;
    return event_reg_offset + (bit * sizeof(uint32_t));
}

NRF_STATIC_INLINE uint32_t nrfx_event_to_bitpos(uint32_t event)
{
    static const uint32_t event_reg_offset = 0x100u;
    return (event - event_reg_offset) / sizeof(uint32_t);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_COMMON_H__
