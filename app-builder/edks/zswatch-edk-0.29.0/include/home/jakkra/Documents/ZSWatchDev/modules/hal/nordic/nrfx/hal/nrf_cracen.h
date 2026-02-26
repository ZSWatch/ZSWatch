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

#ifndef NRF_CRACEN_H__
#define NRF_CRACEN_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_cracen_hal CRACEN HAL
 * @{
 * @ingroup nrf_cracen
 * @brief   Hardware access layer for managing the Crypto Accelerator Engine (CRACEN) peripheral.
 */

#if defined(CRACEN_ENABLE_CRYPTOMASTER_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CRACEN has CRYPTOMASTER module. */
#define NRF_CRACEN_HAS_CRYPTOMASTER 1
#else
#define NRF_CRACEN_HAS_CRYPTOMASTER 0
#endif

#if defined(CRACEN_ENABLE_PKEIKG_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CRACEN has PKE and IKG modules. */
#define NRF_CRACEN_HAS_PKEIKG 1
#else
#define NRF_CRACEN_HAS_PKEIKG 0
#endif

#if defined(CRACEN_SEEDRAMLOCK_ENABLE_Enabled) || defined(CRACEN_SEEDLOCK_ENABLE_Enabled) || \
    defined(CRACEN_PROTECTEDRAMLOCK_ENABLE_Enabled) || defined(CRACEN_KEYLOCK_ENABLE_Enabled) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CRACEN has locking feature. */
#define NRF_CRACEN_HAS_LOCK 1
#else
#define NRF_CRACEN_HAS_LOCK 0
#endif

#if defined(CRACEN_SEED_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CRACEN has SEED register. */
#define NRF_CRACEN_HAS_SEED 1
#else
#define NRF_CRACEN_HAS_SEED 0
#endif

#if NRF_CRACEN_HAS_SEED
/** @brief Number of seed words for private key generation. */
#define NRF_CRACEN_SEED_COUNT CRACEN_SEED_MaxCount
#endif

/** @brief CRACEN events. */
typedef enum
{
#if NRF_CRACEN_HAS_CRYPTOMASTER
    NRF_CRACEN_EVENT_CRYPTOMASTER = offsetof(NRF_CRACEN_Type, EVENTS_CRYPTOMASTER), ///< Interrupt triggered at Cryptomaster.
#endif
    NRF_CRACEN_EVENT_RNG          = offsetof(NRF_CRACEN_Type, EVENTS_RNG),          ///< Interrupt triggered at RNG.
#if NRF_CRACEN_HAS_PKEIKG
    NRF_CRACEN_EVENT_PKE_IKG      = offsetof(NRF_CRACEN_Type, EVENTS_PKEIKG),       ///< Interrupt triggered at PKE or IKG.
#endif
} nrf_cracen_event_t;

/** @brief CRACEN interrupts. */
typedef enum
{
#if NRF_CRACEN_HAS_CRYPTOMASTER
    NRF_CRACEN_INT_CRYPTOMASTER_MASK = CRACEN_INTENSET_CRYPTOMASTER_Msk, ///< Interrupt on CRYPTOMASTER event.
#endif
    NRF_CRACEN_INT_RNG_MASK          = CRACEN_INTENSET_RNG_Msk,          ///< Interrupt on RNG event.
#if NRF_CRACEN_HAS_PKEIKG
    NRF_CRACEN_INT_PKE_IKG_MASK      = CRACEN_INTENSET_PKEIKG_Msk,       ///< Interrupt on PKEIKG event.
#endif
} nrf_cracen_int_mask_t;

/** @brief CRACEN modules mask. */
typedef enum
{
#if NRF_CRACEN_HAS_CRYPTOMASTER
    NRF_CRACEN_MODULE_CRYPTOMASTER_MASK = CRACEN_ENABLE_CRYPTOMASTER_Msk, ///< Cryptomaster module.
#endif
    NRF_CRACEN_MODULE_RNG_MASK          = CRACEN_ENABLE_RNG_Msk,          ///< RNG module.
#if NRF_CRACEN_HAS_PKEIKG
    NRF_CRACEN_MODULE_PKE_IKG_MASK      = CRACEN_ENABLE_PKEIKG_Msk,       ///< PKE and IKG module.
#endif
} nrf_cracen_module_mask_t;


/**
 * @brief Function for getting the address of the specified CRACEN event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event CRACEN event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_cracen_event_address_get(NRF_CRACEN_Type const * p_reg,
                                                        nrf_cracen_event_t      event);

/**
 * @brief Function for clearing the specified CRACEN event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event CRACEN event to be cleared.
 */
NRF_STATIC_INLINE void nrf_cracen_event_clear(NRF_CRACEN_Type *  p_reg,
                                              nrf_cracen_event_t event);

/**
 * @brief Function for checking the state of the specified CRACEN event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event CRACEN event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_cracen_event_check(NRF_CRACEN_Type const * p_reg,
                                              nrf_cracen_event_t      event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_cracen_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_cracen_int_enable(NRF_CRACEN_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_cracen_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_cracen_int_enable_check(NRF_CRACEN_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_cracen_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_cracen_int_disable(NRF_CRACEN_Type * p_reg, uint32_t mask);

/**
 * @brief Function for enabling CRACEN modules.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] modules Mask of modules to be enabled. See @ref nrf_cracen_module_mask_t.
 */
NRF_STATIC_INLINE void nrf_cracen_module_enable(NRF_CRACEN_Type * p_reg, uint32_t modules);

/**
 * @brief Function for disabling CRACEN modules.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] modules Mask of modules to be disabled. See @ref nrf_cracen_module_mask_t.
 */
NRF_STATIC_INLINE void nrf_cracen_module_disable(NRF_CRACEN_Type * p_reg, uint32_t modules);

/**
 * @brief Function for getting enabled CRACEN modules.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask of enabled modules. See @ref nrf_cracen_module_mask_t.
 */
NRF_STATIC_INLINE uint32_t nrf_cracen_module_get(NRF_CRACEN_Type const * p_reg);

#if NRF_CRACEN_HAS_LOCK
/**
 * @brief Function for enabling or disabling lock on access to the RAM used for the seed.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if lock is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_cracen_seedram_lock_enable_set(NRF_CRACEN_Type * p_reg, bool enable);

/**
 * @brief Function for checking if access to the RAM used for the seed is locked.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Access to the RAM used for the seed is locked.
 * @retval false Access to the RAM used for the seed is unlocked.
 */
NRF_STATIC_INLINE bool nrf_cracen_seedram_lock_check(NRF_CRACEN_Type const * p_reg);
#endif

#if NRF_CRACEN_HAS_SEED
/**
 * @brief Function for setting specified seed word for private key generation.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Index of the seed word.
 * @param[in] value Seed value to be set.
 */
NRF_STATIC_INLINE void nrf_cracen_seed_set(NRF_CRACEN_Type * p_reg, uint8_t idx, uint32_t value);
#endif

#ifndef NRF_DECLARE_ONLY
NRF_STATIC_INLINE uint32_t nrf_cracen_event_address_get(NRF_CRACEN_Type const * p_reg,
                                                        nrf_cracen_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_cracen_event_clear(NRF_CRACEN_Type *  p_reg,
                                              nrf_cracen_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_cracen_event_check(NRF_CRACEN_Type const * p_reg,
                                              nrf_cracen_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_cracen_int_enable(NRF_CRACEN_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE uint32_t nrf_cracen_int_enable_check(NRF_CRACEN_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE void nrf_cracen_int_disable(NRF_CRACEN_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE void nrf_cracen_module_enable(NRF_CRACEN_Type * p_reg, uint32_t modules)
{
    p_reg->ENABLE |= modules;
}

NRF_STATIC_INLINE void nrf_cracen_module_disable(NRF_CRACEN_Type * p_reg, uint32_t modules)
{
    p_reg->ENABLE &= ~modules;
}

NRF_STATIC_INLINE uint32_t nrf_cracen_module_get(NRF_CRACEN_Type const * p_reg)
{
    return p_reg->ENABLE;
}

#if NRF_CRACEN_HAS_LOCK
NRF_STATIC_INLINE void nrf_cracen_seedram_lock_enable_set(NRF_CRACEN_Type * p_reg, bool enable)
{
#if defined(CRACEN_SEEDRAMLOCK_ENABLE_Enabled)
    p_reg->SEEDRAMLOCK = (enable ? CRACEN_SEEDRAMLOCK_ENABLE_Enabled
                          : CRACEN_SEEDRAMLOCK_ENABLE_Disabled) << CRACEN_SEEDRAMLOCK_ENABLE_Pos;
#elif defined(CRACEN_SEEDLOCK_ENABLE_Enabled)
    p_reg->SEEDLOCK = (enable ? CRACEN_SEEDLOCK_ENABLE_Enabled
                       : CRACEN_SEEDLOCK_ENABLE_Disabled) << CRACEN_SEEDLOCK_ENABLE_Pos;
#elif defined(CRACEN_PROTECTEDRAMLOCK_ENABLE_Enabled)
    p_reg->PROTECTEDRAMLOCK = (enable ? CRACEN_PROTECTEDRAMLOCK_ENABLE_Enabled
                               : CRACEN_PROTECTEDRAMLOCK_ENABLE_Disabled) << CRACEN_PROTECTEDRAMLOCK_ENABLE_Pos;
#else
    p_reg->KEYLOCK = (enable ? CRACEN_KEYLOCK_ENABLE_Enabled : CRACEN_KEYLOCK_ENABLE_Disabled)
                     << CRACEN_KEYLOCK_ENABLE_Pos;
#endif
}

NRF_STATIC_INLINE bool nrf_cracen_seedram_lock_check(NRF_CRACEN_Type const * p_reg)
{
#if defined(CRACEN_SEEDRAMLOCK_ENABLE_Enabled)
    return p_reg->SEEDRAMLOCK == (CRACEN_SEEDRAMLOCK_ENABLE_Enabled
                                  << CRACEN_SEEDRAMLOCK_ENABLE_Pos);
#elif defined(CRACEN_SEEDLOCK_ENABLE_Enabled)
    return p_reg->SEEDLOCK == (CRACEN_SEEDLOCK_ENABLE_Enabled
                               << CRACEN_SEEDLOCK_ENABLE_Pos);
#elif defined(CRACEN_PROTECTEDRAMLOCK_ENABLE_Enabled)
    return p_reg->PROTECTEDRAMLOCK == (CRACEN_PROTECTEDRAMLOCK_ENABLE_Enabled
                                  << CRACEN_PROTECTEDRAMLOCK_ENABLE_Pos);
#else
    return p_reg->KEYLOCK == (CRACEN_KEYLOCK_ENABLE_Enabled << CRACEN_KEYLOCK_ENABLE_Pos);
#endif
}
#endif

#if NRF_CRACEN_HAS_SEED
NRF_STATIC_INLINE void nrf_cracen_seed_set(NRF_CRACEN_Type * p_reg, uint8_t idx, uint32_t value)
{
    NRFX_ASSERT(idx < NRF_CRACEN_SEED_COUNT);
    p_reg->SEED[idx] = value;
}
#endif
#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CRACEN_H__
