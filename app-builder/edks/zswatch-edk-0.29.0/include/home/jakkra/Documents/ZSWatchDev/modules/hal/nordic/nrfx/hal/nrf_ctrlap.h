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

#ifndef NRF_CTRLAP_H__
#define NRF_CTRLAP_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_ctrlap_hal CTRL-AP HAL
 * @{
 * @ingroup nrf_ctrlap
 * @brief   Hardware access layer for managing the Control Access Port (CTRL-AP) peripheral.
 */

#if defined(CTRLAPPERI_INFO_PARTNO_PARTNO_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the INFO register is present. */
#define NRF_CTRLAP_HAS_INFO 1
#else
#define NRF_CTRLAP_HAS_INFO 0
#endif

#if defined(CTRLAPPERI_MAILBOX_BOOTMODE_MODE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the BOOTMODE register is present. */
#define NRF_CTRLAP_HAS_BOOTMODE 1
#else
#define NRF_CTRLAP_HAS_BOOTMODE 0
#endif

#if defined(CTRLAPPERI_INFO_READY_READY_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the READY register is present. */
#define NRF_CTRLAP_HAS_READY 1
#else
#define NRF_CTRLAP_HAS_READY 0
#endif

#if defined(CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the ERASEPROTECT register is present. */
#define NRF_CTRLAP_HAS_ERASEPROTECT 1
#else
#define NRF_CTRLAP_HAS_ERASEPROTECT 0
#endif

#if defined(CTRLAPPERI_RESET_RESET_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the RESET register is present. */
#define NRF_CTRLAP_HAS_RESET 1
#else
#define NRF_CTRLAP_HAS_RESET 0
#endif

/** @brief CTRLAP events. */
typedef enum
{
    NRF_CTRLAP_EVENT_RXREADY = offsetof(NRF_CTRLAPPERI_Type, EVENTS_RXREADY), ///< New data from the peer is available.
    NRF_CTRLAP_EVENT_TXDONE  = offsetof(NRF_CTRLAPPERI_Type, EVENTS_TXDONE),  ///< Data has been read by the peer.
} nrf_ctrlap_event_t;

/** @brief CTRLAP interrupts. */
typedef enum
{
    NRF_CTRLAP_INT_RXREADY_MASK = CTRLAPPERI_INTENSET_RXREADY_Msk, ///< Interrupt on RXREADY event.
    NRF_CTRLAP_INT_TXDONE_MASK  = CTRLAPPERI_INTENSET_TXDONE_Msk,  ///< Interrupt on TXDONE event.
} nrf_ctrlap_int_mask_t;

#if NRF_CTRLAP_HAS_INFO
/** @brief CTRLAP device information. */
typedef struct
{
    uint32_t partno;      ///< Part number of the device, this information is retained on system on idle.
    uint32_t hw_revision; ///< Hardware Revision of the device, this information is retained on system on idle.
#if NRF_CTRLAP_HAS_READY
    bool     ready;       ///< Set when INFO registers update is completed.
#endif
} nrf_ctrlap_info_t;
#endif

#if NRF_CTRLAP_HAS_BOOTMODE
/** @brief CTRLAP secure domain boot mode. */
typedef enum
{
    NRF_CTRLAP_MODE_NORMAL        = CTRLAPPERI_MAILBOX_BOOTMODE_MODE_Normal,       ///< Normal mode of operation.
    NRF_CTRLAP_MODE_ROM_OPERATION = CTRLAPPERI_MAILBOX_BOOTMODE_MODE_ROMOperation, ///< ROM operation mode.
} nrf_ctrlap_bootmode_t;
#endif

#if NRF_CTRLAP_HAS_RESET
/** @brief CTRLAP reset types. */
typedef enum
{
    NRF_CTRLAP_RESET_NONE = CTRLAPPERI_RESET_RESET_NoReset,   ///< No reset is performed.
    NRF_CTRLAP_RESET_SOFT = CTRLAPPERI_RESET_RESET_SoftReset, ///< Soft reset is performed.
    NRF_CTRLAP_RESET_HARD = CTRLAPPERI_RESET_RESET_HardReset, ///< Hard reset is performed.
    NRF_CTRLAP_RESET_PIN  = CTRLAPPERI_RESET_RESET_PinReset,  ///< Pin reset is performed.
} nrf_ctrlap_reset_t;
#endif

/**
 * @brief Function for clearing the specified CTRLAP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_ctrlap_event_clear(NRF_CTRLAPPERI_Type * p_reg,
                                              nrf_ctrlap_event_t    event);

/**
 * @brief Function for retrieving the state of the CTRLAP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_ctrlap_event_check(NRF_CTRLAPPERI_Type const * p_reg,
                                              nrf_ctrlap_event_t          event);

/**
 * @brief Function for getting the address of the specified CTRLAP event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 *
 * @retval Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_ctrlap_event_address_get(NRF_CTRLAPPERI_Type const * p_reg,
                                                        nrf_ctrlap_event_t          event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_ctrlap_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_ctrlap_int_enable(NRF_CTRLAPPERI_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_ctrlap_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_ctrlap_int_disable(NRF_CTRLAPPERI_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_ctrlap_int_mask_t values for bit masking.
 *
 * @retval Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_ctrlap_int_enable_check(NRF_CTRLAPPERI_Type const * p_reg,
                                                       uint32_t                    mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * @note States of pending interrupt are saved as a bitmask.
 *       One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Bitmask with information about pending interrupts.
 *         Use @ref nrf_ctrlap_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_ctrlap_int_pending_get(NRF_CTRLAPPERI_Type const * p_reg);

/**
 * @brief Function for reading data sent from the debugger to the CPU.
 *
 * @note Reading from this register will clear pending status of RX.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Data sent from the debugger to the CPU.
 */
NRF_STATIC_INLINE uint32_t nrf_ctrlap_mailbox_rxdata_get(NRF_CTRLAPPERI_Type const * p_reg);

/**
 * @brief Function for checking if data sent from the debugger to the CPU has been read.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Data pending in register RXDATA.
 * @retval false No data pending in register RXDATA.
 */
NRF_STATIC_INLINE bool nrf_ctrlap_mailbox_rxstatus_pending_check(NRF_CTRLAPPERI_Type const * p_reg);

/**
 * @brief Function for reading data sent from CPU to debugger.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Data sent from the CPU to the debugger.
 */
NRF_STATIC_INLINE uint32_t nrf_ctrlap_mailbox_txdata_get(NRF_CTRLAPPERI_Type const * p_reg);

/**
 * @brief Function for writing data sent from CPU to debugger.
 *
 * @details Writing to this register will automatically set field @p DataPending in register @p TXSTATUS.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] data  Data to send.
 */
NRF_STATIC_INLINE void nrf_ctrlap_mailbox_txdata_set(NRF_CTRLAPPERI_Type * p_reg,
                                                     uint32_t              data);

/**
 * @brief Function for checking if data sent from the CPU to the debugger has been read.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Data pending in register TXDATA.
 * @retval false No data pending in register TXDATA.
 */
NRF_STATIC_INLINE bool nrf_ctrlap_mailbox_txstatus_pending_check(NRF_CTRLAPPERI_Type const * p_reg);

#if NRF_CTRLAP_HAS_BOOTMODE
/**
 * @brief Function for checking boot mode.
 *
 * @note If ROM operation mode is set the MAILBOX is used to communicate the secure ROM operations.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Boot mode.
 */
NRF_STATIC_INLINE nrf_ctrlap_bootmode_t
nrf_ctrlap_mailbox_bootmode_get(NRF_CTRLAPPERI_Type const * p_reg);
#endif

#if NRF_CTRLAP_HAS_INFO
/**
 * @brief Function for setting the CTRLAP device information.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the device information structure.
 */
NRF_STATIC_INLINE void nrf_ctrlap_info_set(NRF_CTRLAPPERI_Type  *    p_reg,
                                           nrf_ctrlap_info_t const * p_data);

/**
 * @brief Function for getting the CTRLAP device information.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_data Pointer to the data structure to be filled with device information.
 */
NRF_STATIC_INLINE void nrf_ctrlap_info_get(NRF_CTRLAPPERI_Type const * p_reg,
                                           nrf_ctrlap_info_t *         p_data);
#endif

#if NRF_CTRLAP_HAS_ERASEPROTECT
/**
 * @brief Function for locking erase operation in CTRLAP until next reset.
 *
 * @note  Some devices do not support erase operation unlocking.
 *        Refer to the Product Specification for more information.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if erase is to be locked, false otherwise.
 */
NRF_STATIC_INLINE void nrf_ctrlap_erase_lock_set(NRF_CTRLAPPERI_Type * p_reg, bool enable);

/**
 * @brief Function for reading lock of the CTRLAP erase protection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return true  Erase is locked.
 * @return false Erase is unlocked.
 */
NRF_STATIC_INLINE bool nrf_ctrlap_erase_lock_get(NRF_CTRLAPPERI_Type const * p_reg);

/**
 * @brief Function for performing a secure erase of the device.
 *
 * @note To perform a secure erase, the value of key needs to be non-zero and match with the key
 *       on the debugger side.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] key   Key for performing a secure erase.
 */
NRF_STATIC_INLINE void nrf_ctrlap_erase_all(NRF_CTRLAPPERI_Type * p_reg, uint32_t key);
#endif

#if NRF_CTRLAP_HAS_RESET
/**
 * @brief Function for triggering a reset of requested type.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] reset Requested reset type.
 */
NRF_STATIC_INLINE void nrf_ctrlap_reset_trigger(NRF_CTRLAPPERI_Type * p_reg,
                                                nrf_ctrlap_reset_t    reset);
#endif

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_ctrlap_event_clear(NRF_CTRLAPPERI_Type * p_reg, nrf_ctrlap_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_ctrlap_event_check(NRF_CTRLAPPERI_Type const * p_reg,
                                              nrf_ctrlap_event_t          event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_ctrlap_event_address_get(NRF_CTRLAPPERI_Type const * p_reg,
                                                        nrf_ctrlap_event_t          event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_ctrlap_int_enable(NRF_CTRLAPPERI_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_ctrlap_int_disable(NRF_CTRLAPPERI_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_ctrlap_int_enable_check(NRF_CTRLAPPERI_Type const * p_reg,
                                                       uint32_t                    mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_ctrlap_int_pending_get(NRF_CTRLAPPERI_Type const * p_reg)
{
    return p_reg->INTPEND;
}

NRF_STATIC_INLINE uint32_t nrf_ctrlap_mailbox_rxdata_get(NRF_CTRLAPPERI_Type const * p_reg)
{
    return p_reg->MAILBOX.RXDATA;
}

NRF_STATIC_INLINE bool nrf_ctrlap_mailbox_rxstatus_pending_check(NRF_CTRLAPPERI_Type const * p_reg)
{
    return (bool)p_reg->MAILBOX.RXSTATUS;
}

NRF_STATIC_INLINE uint32_t nrf_ctrlap_mailbox_txdata_get(NRF_CTRLAPPERI_Type const * p_reg)
{
    return p_reg->MAILBOX.TXDATA;
}

NRF_STATIC_INLINE void nrf_ctrlap_mailbox_txdata_set(NRF_CTRLAPPERI_Type * p_reg,
                                                     uint32_t              data)
{
    p_reg->MAILBOX.TXDATA = data;
}

NRF_STATIC_INLINE bool nrf_ctrlap_mailbox_txstatus_pending_check(NRF_CTRLAPPERI_Type const * p_reg)
{
    return (bool)p_reg->MAILBOX.TXSTATUS;
}

#if NRF_CTRLAP_HAS_BOOTMODE
NRF_STATIC_INLINE nrf_ctrlap_bootmode_t
nrf_ctrlap_mailbox_bootmode_get(NRF_CTRLAPPERI_Type const * p_reg)
{
    return (nrf_ctrlap_bootmode_t)p_reg->MAILBOX.BOOTMODE;
}
#endif

#if NRF_CTRLAP_HAS_INFO
NRF_STATIC_INLINE void nrf_ctrlap_info_set(NRF_CTRLAPPERI_Type *     p_reg,
                                           nrf_ctrlap_info_t const * p_data)
{
    p_reg->INFO.PARTNO     = p_data->partno;
    p_reg->INFO.HWREVISION = p_data->hw_revision;
#if NRF_CTRLAP_HAS_READY
    p_reg->INFO.READY      = !p_data->ready;
#endif
}

NRF_STATIC_INLINE void nrf_ctrlap_info_get(NRF_CTRLAPPERI_Type const * p_reg,
                                           nrf_ctrlap_info_t *         p_data)
{
    p_data->partno      = p_reg->INFO.PARTNO;
    p_data->hw_revision = p_reg->INFO.HWREVISION;
#if NRF_CTRLAP_HAS_READY
    p_data->ready       = !p_reg->INFO.READY;
#endif
}
#endif

#if NRF_CTRLAP_HAS_ERASEPROTECT
NRF_STATIC_INLINE void nrf_ctrlap_erase_lock_set(NRF_CTRLAPPERI_Type * p_reg, bool enable)
{
    if (enable)
    {
        p_reg->ERASEPROTECT.LOCK = (CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Locked
                                    << CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Pos);
    }
    else
    {
#if defined(CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Unlocked)
        p_reg->ERASEPROTECT.LOCK = (CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Unlocked
                                    << CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Pos);
#else
        NRFX_ASSERT(0);
#endif
    }
}

NRF_STATIC_INLINE bool nrf_ctrlap_erase_lock_get(NRF_CTRLAPPERI_Type const * p_reg)
{
    return (p_reg->ERASEPROTECT.LOCK == (CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Locked
                                      << CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Pos));
}

NRF_STATIC_INLINE void nrf_ctrlap_erase_all(NRF_CTRLAPPERI_Type * p_reg, uint32_t key)
{
    p_reg->ERASEPROTECT.DISABLE = key;
}
#endif

#if NRF_CTRLAP_HAS_RESET
NRF_STATIC_INLINE void nrf_ctrlap_reset_trigger(NRF_CTRLAPPERI_Type * p_reg,
                                                nrf_ctrlap_reset_t    reset)
{
    p_reg->RESET = (uint32_t)reset;
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CTRLAP_H__
