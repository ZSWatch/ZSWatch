/*
 * Copyright (c) 2018 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_KMU_H__
#define NRF_KMU_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_kmu_hal KMU HAL
 * @{
 * @ingroup nrf_kmu
 * @brief   Hardware access layer for managing the Key Management Unit (KMU) peripheral.
 */

#if defined(KMU_INTEN_KEYSLOT_ERROR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether INTEN register is available. */
#define NRF_KMU_HAS_INTEN 1
#else
#define NRF_KMU_HAS_INTEN 0
#endif

#if defined(KMU_TASKS_PROVISION_TASKS_PROVISION_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether KMU has PROVISION registers. */
#define NRF_KMU_HAS_PROVISION 1
#else
#define NRF_KMU_HAS_PROVISION 0
#endif

#if defined(KMU_TASKS_REVOKE_TASKS_REVOKE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether KMU has REVOKE registers. */
#define NRF_KMU_HAS_REVOKE 1
#else
#define NRF_KMU_HAS_REVOKE 0
#endif

#if defined(KMU_TASKS_READMETADATA_TASKS_READMETADATA_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether KMU has READMETADATA registers. */
#define NRF_KMU_HAS_READ_METADATA 1
#else
#define NRF_KMU_HAS_READ_METADATA 0
#endif

#if defined(KMU_TASKS_PUSHBLOCK_TASKS_PUSHBLOCK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether KMU has PUSHBLOCK registers. */
#define NRF_KMU_HAS_PUSH_BLOCK 1
#else
#define NRF_KMU_HAS_PUSH_BLOCK 0
#endif

#if defined(KMU_SRC_SRC_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether KMU has SRC registers. */
#define NRF_KMU_HAS_SRC 1
#else
#define NRF_KMU_HAS_SRC 0
#endif

#if defined(KMU_METADATA_METADATA_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether KMU has METADATA registers. */
#define NRF_KMU_HAS_METADATA 1
#else
#define NRF_KMU_HAS_METADATA 0
#endif

/** @brief KMU tasks. */
typedef enum
{
#if NRF_KMU_HAS_PROVISION
    NRF_KMU_TASK_PROVISION_KEYSLOT = offsetof(NRF_KMU_Type, TASKS_PROVISION),    ///< Provision key slot.
#endif
#if defined(KMU_TASKS_PUSH_KEYSLOT_TASKS_PUSH_KEYSLOT_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_KMU_TASK_PUSH_KEYSLOT      = offsetof(NRF_KMU_Type, TASKS_PUSH_KEYSLOT), ///< Push a key slot over secure APB.
#else
    NRF_KMU_TASK_PUSH_KEYSLOT      = offsetof(NRF_KMU_Type, TASKS_PUSH),         ///< Push key slot.
#endif
#if NRF_KMU_HAS_REVOKE
    NRF_KMU_TASK_REVOKE_KEYSLOT    = offsetof(NRF_KMU_Type, TASKS_REVOKE),       ///< Revoke key slot.
#endif
#if NRF_KMU_HAS_READ_METADATA
    NRF_KMU_TASK_READ_METADATA     = offsetof(NRF_KMU_Type, TASKS_READMETADATA), ///< Read key slot metedata into METADATA register.
#endif
#if NRF_KMU_HAS_PUSH_BLOCK
    NRF_KMU_TASK_PUSH_BLOCK        = offsetof(NRF_KMU_Type, TASKS_PUSHBLOCK),    ///< Block the PUSH operation of key slot, preventing the key slot being PUSH until next reset.
#endif
} nrf_kmu_task_t;

/** @brief KMU events. */
typedef enum
{
#if NRF_KMU_HAS_PROVISION
    NRF_KMU_EVENT_EVENTS_PROVISIONED          = offsetof(NRF_KMU_Type, EVENTS_PROVISIONED),     ///< Key slot successfully provisioned.
#endif

#if defined(KMU_EVENTS_KEYSLOT_PUSHED_EVENTS_KEYSLOT_PUSHED_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_KMU_EVENT_KEYSLOT_PUSHED              = offsetof(NRF_KMU_Type, EVENTS_KEYSLOT_PUSHED),  ///< Key successfully pushed over secure APB.
#else
    NRF_KMU_EVENT_KEYSLOT_PUSHED              = offsetof(NRF_KMU_Type, EVENTS_PUSHED),          ///< Key successfully pushed over secure APB.
#endif

#if defined(KMU_EVENTS_KEYSLOT_REVOKED_EVENTS_KEYSLOT_REVOKED_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_KMU_EVENT_KEYSLOT_REVOKED             = offsetof(NRF_KMU_Type, EVENTS_KEYSLOT_REVOKED), ///< Key has been revoked and cannot be tasked for selection.
#else
    NRF_KMU_EVENT_KEYSLOT_REVOKED             = offsetof(NRF_KMU_Type, EVENTS_REVOKED),         ///< Key has been revoked and cannot be tasked for selection.
#endif

#if defined(KMU_EVENTS_KEYSLOT_ERROR_EVENTS_KEYSLOT_ERROR_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_KMU_EVENT_KEYSLOT_ERROR               = offsetof(NRF_KMU_Type, EVENTS_KEYSLOT_ERROR),   ///< No key slot selected or no destination address defined or error during push mechanism.
#else
    NRF_KMU_EVENT_KEYSLOT_ERROR               = offsetof(NRF_KMU_Type, EVENTS_ERROR),           ///< No key slot selected or no destination address defined or error during push mechanism.
#endif

#if NRF_KMU_HAS_READ_METADATA
    NRF_KMU_EVENT_EVENTS_EVENTS_METADATA_READ = offsetof(NRF_KMU_Type, EVENTS_METADATAREAD),    ///< Key slot metedata has been read into METADATA register.
#endif

#if NRF_KMU_HAS_PUSH_BLOCK
    NRF_KMU_EVENT_EVENTS_EVENTS_PUSHBLOCKED   = offsetof(NRF_KMU_Type, EVENTS_PUSHBLOCKED),     ///< The PUSHBLOCK operation was succesful.
#endif
} nrf_kmu_event_t;

#if NRF_KMU_HAS_INTEN
/** @brief KMU interrupts. */
typedef enum
{
    NRF_KMU_INT_PUSHED_MASK  = KMU_INTEN_KEYSLOT_PUSHED_Msk,  ///< Interrupt on KEYSLOT_PUSHED event.
    NRF_KMU_INT_REVOKED_MASK = KMU_INTEN_KEYSLOT_REVOKED_Msk, ///< Interrupt on KEYSLOT_REVOKED event.
    NRF_KMU_INT_ERROR_MASK   = KMU_INTEN_KEYSLOT_ERROR_Msk    ///< Interrupt on KEYSLOT_ERROR event.
} nrf_kmu_int_mask_t;
#endif

/** @brief KMU operation status. */
typedef enum
{
#if defined(KMU_STATUS_BLOCKED_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_KMU_STATUS_BLOCKED_MASK  = KMU_STATUS_BLOCKED_Msk,  ///< Access violation detected and blocked.
#endif
#if defined(KMU_STATUS_SELECTED_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_KMU_STATUS_SELECTED_MASK = KMU_STATUS_SELECTED_Msk, ///< Key slot ID successfully selected by KMU.
#endif
#if defined(KMU_STATUS_STATUS_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_KMU_STATUS_READY         = KMU_STATUS_STATUS_Ready, ///< KMU is ready for a new operation.
    NRF_KMU_STATUS_BUSY          = KMU_STATUS_STATUS_Busy,  ///< KMU is busy, an operation is in progress.
#endif
} nrf_kmu_status_t;

/**
 * @brief Function for activating a specific KMU task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_kmu_task_trigger(NRF_KMU_Type * p_reg, nrf_kmu_task_t task);

/**
 * @brief Function for getting the address of a specific KMU task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_kmu_task_address_get(NRF_KMU_Type const * p_reg,
                                                    nrf_kmu_task_t       task);

/**
 * @brief Function for clearing a specific KMU event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_kmu_event_clear(NRF_KMU_Type * p_reg, nrf_kmu_event_t event);

/**
 * @brief Function for retrieving the state of the KMU event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_kmu_event_check(NRF_KMU_Type const * p_reg, nrf_kmu_event_t event);

/**
 * @brief Function for getting the address of a specific KMU event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_kmu_event_address_get(NRF_KMU_Type const * p_reg,
                                                     nrf_kmu_event_t      event);

#if NRF_KMU_HAS_INTEN
/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_kmu_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_kmu_int_enable(NRF_KMU_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_kmu_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_kmu_int_disable(NRF_KMU_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_kmu_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_kmu_int_enable_check(NRF_KMU_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * @note States of pending interrupt are saved as a bitmask.
 *       One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_kmu_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_kmu_intpend_get(NRF_KMU_Type const * p_reg);
#endif // NRF_KMU_HAS_INTEN

/**
 * @brief Function for getting status bits of the KMU operation.
 *
 * Please use @ref nrf_kmu_status_t to check operations status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Operation status.
 */
NRF_STATIC_INLINE uint32_t nrf_kmu_status_get(NRF_KMU_Type const * p_reg);

/**
 * @brief Function for selecting the key slot ID.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] keyslot_id Key slot ID to be read over AHB or pushed over
 *                       secure APB when TASKS_PUSH_KEYSLOT is started.
 */
NRF_STATIC_INLINE void nrf_kmu_keyslot_set(NRF_KMU_Type * p_reg, uint8_t keyslot_id);

/**
 * @brief Function for getting the key slot ID.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Key slot ID.
 */
NRF_STATIC_INLINE uint8_t nrf_kmu_keyslot_get(NRF_KMU_Type const * p_reg);

#if NRF_KMU_HAS_SRC
/**
 * @brief Function for setting the source address for provisioning.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] src   Source address to be set.
 */
NRF_STATIC_INLINE void nrf_kmu_src_set(NRF_KMU_Type * p_reg, uint32_t src);

/**
 * @brief Function for getting the source address for provisioning.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Source address.
 */
NRF_STATIC_INLINE uint32_t nrf_kmu_src_get(NRF_KMU_Type const * p_reg);
#endif // NRF_KMU_HAS_SRC

#if NRF_KMU_HAS_METADATA
/**
 * @brief Function for setting the key slot metadata.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] metdata Key slot metadata.
 */
NRF_STATIC_INLINE void nrf_kmu_metadata_set(NRF_KMU_Type * p_reg, uint32_t metdata);

/**
 * @brief Function for getting the key slot metadata.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Key slot metadata.
 */
NRF_STATIC_INLINE uint32_t nrf_kmu_metadata_get(NRF_KMU_Type const * p_reg);
#endif // NRF_KMU_HAS_METADATA

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_kmu_task_trigger(NRF_KMU_Type * p_reg, nrf_kmu_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_kmu_task_address_get(NRF_KMU_Type const * p_reg,
                                                    nrf_kmu_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE void nrf_kmu_event_clear(NRF_KMU_Type * p_reg, nrf_kmu_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_kmu_event_check(NRF_KMU_Type const * p_reg, nrf_kmu_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_kmu_event_address_get(NRF_KMU_Type const * p_reg,
                                                     nrf_kmu_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

#if NRF_KMU_HAS_INTEN
NRF_STATIC_INLINE void nrf_kmu_int_enable(NRF_KMU_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_kmu_int_disable(NRF_KMU_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_kmu_int_enable_check(NRF_KMU_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_kmu_intpend_get(NRF_KMU_Type const * p_reg)
{
    return p_reg->INTPEND;
}
#endif // NRF_KMU_HAS_INTEN

NRF_STATIC_INLINE uint32_t nrf_kmu_status_get(NRF_KMU_Type const * p_reg)
{
    return p_reg->STATUS;
}

NRF_STATIC_INLINE void nrf_kmu_keyslot_set(NRF_KMU_Type * p_reg, uint8_t keyslot_id)
{
#if defined(KMU_SELECTKEYSLOT_ID_Msk)
    p_reg->SELECTKEYSLOT = (uint32_t)keyslot_id;
#elif defined(KMU_KEYSLOT_ID_Msk)
    p_reg->KEYSLOT = (uint32_t)keyslot_id;
#else
    #error "Unsupported"
#endif
}

NRF_STATIC_INLINE uint8_t nrf_kmu_keyslot_get(NRF_KMU_Type const * p_reg)
{
#if defined(KMU_SELECTKEYSLOT_ID_Msk)
    return (uint8_t)p_reg->SELECTKEYSLOT;
#elif defined(KMU_KEYSLOT_ID_Msk)
    return (uint8_t)p_reg->KEYSLOT;
#else
    #error "Unsupported"
#endif
}

#if NRF_KMU_HAS_SRC
NRF_STATIC_INLINE void nrf_kmu_src_set(NRF_KMU_Type * p_reg, uint32_t src)
{
    p_reg->SRC = src;
}

NRF_STATIC_INLINE uint32_t nrf_kmu_src_get(NRF_KMU_Type const * p_reg)
{
    return p_reg->SRC;
}
#endif // NRF_KMU_HAS_SRC

#if NRF_KMU_HAS_METADATA
NRF_STATIC_INLINE void nrf_kmu_metadata_set(NRF_KMU_Type * p_reg, uint32_t metdata)
{
    p_reg->METADATA = metdata;
}

NRF_STATIC_INLINE uint32_t nrf_kmu_metadata_get(NRF_KMU_Type const * p_reg)
{
    return p_reg->METADATA;
}
#endif // NRF_KMU_HAS_METADATA

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_KMU_H__
