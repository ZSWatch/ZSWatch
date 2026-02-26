/*
 * Copyright (c) 2025, Nordic Semiconductor ASA
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

#ifndef NRF_CRACEN_CM_H__
#define NRF_CRACEN_CM_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_cracen_cm_hal CRACEN CryptoMaster HAL
 * @{
 * @ingroup nrf_cracen
 * @brief   Hardware access layer for managing the Crypto Accelerator Engine (CRACEN)
 *          CryptoMaster (CM) peripheral.
 */

/** @brief CRACEN CryptoMaster configuration indirect mask. */
typedef enum
{
    NRF_CRACEN_CM_CONFIG_INDIRECT_FETCH_MASK = CRACENCORE_CRYPTMSTRDMA_CONFIG_FETCHCTRLINDIRECT_Msk, ///< Set the fetch DMA in scatter/gather mode
    NRF_CRACEN_CM_CONFIG_INDIRECT_PUSH_MASK  = CRACENCORE_CRYPTMSTRDMA_CONFIG_PUSHCTRLINDIRECT_Msk,  ///< Set the push DMA in scatter/gather mode
} nrf_cracen_cm_config_indirect_mask_t;

/** @brief CRACEN CryptoMaster interrupts' masks. */
typedef enum
{
    NRF_CRACEN_CM_INT_FETCH_BLOCK_END_MASK = CRACENCORE_CRYPTMSTRDMA_INTEN_FETCHERBLOCKEND_Msk, ///< Interrupt on DMA fetch end of block (if enabled in the descriptor, for indirect mode only).
    NRF_CRACEN_CM_INT_FETCH_STOPPED_MASK   = CRACENCORE_CRYPTMSTRDMA_INTEN_FETCHERSTOPPED_Msk,  ///< Interrupt on DMA fetch stopped/ended.
    NRF_CRACEN_CM_INT_FETCH_ERROR_MASK     = CRACENCORE_CRYPTMSTRDMA_INTEN_FETCHERERROR_Msk,    ///< Interrupt on DMA fetch bus error.
    NRF_CRACEN_CM_INT_PUSH_BLOCK_END_MASK  = CRACENCORE_CRYPTMSTRDMA_INTEN_PUSHERBLOCKEND_Msk,  ///< Interrupt on DMA push end of block (if enabled in the descriptor, for indirect mode only).
    NRF_CRACEN_CM_INT_PUSH_STOPPED_MASK    = CRACENCORE_CRYPTMSTRDMA_INTEN_PUSHERSTOPPED_Msk,   ///< Interrupt on DMA push stopped/ended.
    NRF_CRACEN_CM_INT_PUSH_ERROR_MASK      = CRACENCORE_CRYPTMSTRDMA_INTEN_PUSHERERROR_Msk,     ///< Interrupt on DMA push bus error.
} nrf_cracen_cm_int_mask_t;

/** @brief CRACEN CryptoMaster status busy mask. */
typedef enum
{
    NRF_CRACEN_CM_STATUS_BUSY_FETCH_MASK      = CRACENCORE_CRYPTMSTRDMA_STATUS_FETCHBUSY_Msk,       ///< Fetch DMA is busy.
    NRF_CRACEN_CM_STATUS_BUSY_PUSH_MASK       = CRACENCORE_CRYPTMSTRDMA_STATUS_PUSHBUSY_Msk,        ///< Push DMA is busy.
    NRF_CRACEN_CM_STATUS_FETCH_NOT_EMPTY_MASK = CRACENCORE_CRYPTMSTRDMA_STATUS_FETCHNOTEMPTY_Msk,   ///< Fetch DMA FIFO is not empty.
    NRF_CRACEN_CM_STATUS_PUSH_WAITING_MASK    = CRACENCORE_CRYPTMSTRDMA_STATUS_PUSHWAITINGFIFO_Msk, ///< Push DMA is waiting for data from a crypto engine.
    NRF_CRACEN_CM_STATUS_SOFTRESET_BUSY_MASK  = CRACENCORE_CRYPTMSTRDMA_STATUS_SOFTRSTBUSY_Msk,     ///< Soft-resetting.
} nrf_cracen_cm_status_mask_t;

/**
 * @brief Function for setting the DMA fetch pointer to either the buffer from which the DMA will
 *        read data (in direct mode), or a pointer to a DMA descriptor (in scatter mode).
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the data to read, or a read descriptor.
 */
NRF_STATIC_INLINE void nrf_cracen_cm_fetch_addr_set(NRF_CRACENCORE_Type * p_reg,
                                                    void const *          p_buffer);

/**
 * @brief Function for setting the DMA push pointer to either the buffer into which the DMA will
 *        write data (in direct mode), or a pointer to a DMA descriptor (in scatter mode).
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the buffer where to write, or a write descriptor.
 */
NRF_STATIC_INLINE void nrf_cracen_cm_push_addr_set(NRF_CRACENCORE_Type * p_reg,
                                                   void const *          p_buffer);

/**
 * @brief Function for setting the DMA's indirect configuration
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask selecting if the push and/or fetch DMA should be in indirect mode.
 *                  Use @ref nrf_cracen_cm_config_indirect_mask_t for bit masking.
 */
NRF_STATIC_INLINE void nrf_cracen_cm_config_indirect_set(NRF_CRACENCORE_Type *                p_reg,
                                                         nrf_cracen_cm_config_indirect_mask_t mask);

/**
 * @brief Function for soft resetting the CryptoMaster module
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_cracen_cm_softreset(NRF_CRACENCORE_Type * p_reg);

/**
 * @brief Function for starting the CryptoMaster
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @note Both the fetch and push DMA engines will be started simultaneously.
 */
NRF_STATIC_INLINE void nrf_cracen_cm_start(NRF_CRACENCORE_Type * p_reg);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_cracen_cm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_cracen_cm_int_enable(NRF_CRACENCORE_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_cracen_cm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_cracen_cm_int_disable(NRF_CRACENCORE_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_cracen_cm_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_cracen_cm_int_enable_check(NRF_CRACENCORE_Type const * p_reg,
                                                          uint32_t                    mask);

/**
 * @brief Function for clearing the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be cleared.
 *                  Use @ref nrf_cracen_cm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_cracen_cm_int_clear(NRF_CRACENCORE_Type * p_reg, uint32_t mask);

/**
 * @brief Function for getting the state of pending interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_cracen_cm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_cracen_cm_int_pending_get(NRF_CRACENCORE_Type const * p_reg);

/**
 * @brief Function for getting the status of the CryptoMaster.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of busy conditions to be checked.
 *                  Use @ref nrf_cracen_cm_status_mask_t values for bit masking.
 *
 * @return Masked busy conditions.
 */
NRF_STATIC_INLINE uint32_t nrf_cracen_cm_status_get(NRF_CRACENCORE_Type const * p_reg,
                                                    uint32_t                    mask);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_cracen_cm_fetch_addr_set(NRF_CRACENCORE_Type * p_reg,
                                                    void const *          p_buffer)
{
    p_reg->CRYPTMSTRDMA.FETCHADDRLSB = (uint32_t)p_buffer;
}

NRF_STATIC_INLINE void nrf_cracen_cm_push_addr_set(NRF_CRACENCORE_Type * p_reg,
                                                   void const *          p_buffer)
{
    p_reg->CRYPTMSTRDMA.PUSHADDRLSB = (uint32_t)p_buffer;
}

NRF_STATIC_INLINE void nrf_cracen_cm_config_indirect_set(NRF_CRACENCORE_Type *                p_reg,
                                                         nrf_cracen_cm_config_indirect_mask_t mask)
{
    p_reg->CRYPTMSTRDMA.CONFIG = (uint32_t)mask;
}

NRF_STATIC_INLINE void nrf_cracen_cm_softreset(NRF_CRACENCORE_Type * p_reg)
{
    p_reg->CRYPTMSTRDMA.CONFIG = CRACENCORE_CRYPTMSTRDMA_CONFIG_SOFTRST_Msk;
    p_reg->CRYPTMSTRDMA.CONFIG = 0;
}

NRF_STATIC_INLINE void nrf_cracen_cm_start(NRF_CRACENCORE_Type * p_reg)
{
    p_reg->CRYPTMSTRDMA.START = CRACENCORE_CRYPTMSTRDMA_START_STARTFETCH_Msk
                               | CRACENCORE_CRYPTMSTRDMA_START_STARTPUSH_Msk;
}

NRF_STATIC_INLINE void nrf_cracen_cm_int_enable(NRF_CRACENCORE_Type * p_reg, uint32_t mask)
{
    p_reg->CRYPTMSTRDMA.INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_cracen_cm_int_disable(NRF_CRACENCORE_Type * p_reg, uint32_t mask)
{
    p_reg->CRYPTMSTRDMA.INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_cracen_cm_int_enable_check(NRF_CRACENCORE_Type const * p_reg,
                                                          uint32_t                    mask)
{
    return p_reg->CRYPTMSTRDMA.INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_cracen_cm_int_pending_get(NRF_CRACENCORE_Type const * p_reg)
{
    return p_reg->CRYPTMSTRDMA.INTSTATRAW;
}

NRF_STATIC_INLINE void nrf_cracen_cm_int_clear(NRF_CRACENCORE_Type * p_reg, uint32_t mask)
{
    p_reg->CRYPTMSTRDMA.INTSTATCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_cracen_cm_status_get(NRF_CRACENCORE_Type const * p_reg,
                                                    uint32_t                    mask)
{
    return p_reg->CRYPTMSTRDMA.STATUS & mask;
}
#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CRACEN_CM_H__
