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

#ifndef NRF_EXMIF_H__
#define NRF_EXMIF_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_exmif_hal EXMIF HAL
 * @{
 * @ingroup nrf_exmif
 * @brief   Hardware access layer for managing the External Memory Interface (EXMIF) peripheral.
 */

/** @brief Maximum EXMIF memory size. */
#define NRF_EXMIF_MAX_MEMORY_DEVICE_SIZE EXMIF_MAX_MEMORY_DEVICE_SIZE

/** @brief Maximum number of EXMIF memory devices. */
#define NRF_EXMIF_MAX_NUMBER_OF_DEVICES EXMIF_MAX_NUMBER_OF_DEVICES

/** @brief EXMIF tasks. */
typedef enum
{
    NRF_EXMIF_TASK_START = offsetof(NRF_EXMIF_Type, TASKS_START), ///< Start EXMIF peripheral power and clocks.
    NRF_EXMIF_TASK_STOP  = offsetof(NRF_EXMIF_Type, TASKS_STOP)   ///< Stop EXMIF peripheral power and clocks.
} nrf_exmif_task_t;

/**
 * @brief Structure for configuration of mapping of the memory device to the EXMIF peripheral.
 *
 * @note @p offset is subtracted from incoming address to produce an address used for an external
 *       memory device. It can be calculated using following equation:
 *       offset = incoming AXI address - expected base address of the memory device
 */
typedef struct
{
    uint32_t offset; ///< EXMIF address mapping offset.
    uint32_t size;   ///< EXMIF address mapping memory size.
} nrf_exmif_config_t;

/**
 * @brief Function for activating the specified EXMIF task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_exmif_task_trigger(NRF_EXMIF_Type * p_reg,
                                              nrf_exmif_task_t task);

/**
 * @brief Function for getting the address of the specified EXMIF task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  The specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_exmif_task_address_get(NRF_EXMIF_Type const * p_reg,
                                                      nrf_exmif_task_t       task);

/**
 * @brief Function for enabling or disabling EXMIF reset state.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if reset state is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_exmif_reset_set(NRF_EXMIF_Type * p_reg,
                                           bool             enable);

/**
 * @brief Function for enabling or disabling locked APB access to serial memory controller.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if locked APB access is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_exmif_locked_access_set(NRF_EXMIF_Type * p_reg,
                                                   bool             enable);

/**
 * @brief Function for configuring mapping of the memory device to EXMIF peripheral.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] device_idx Device number to configure.
 * @param[in] p_device   Pointer to the configuration structure of the memory device.
 */
NRF_STATIC_INLINE void nrf_exmif_device_config(NRF_EXMIF_Type *           p_reg,
                                               uint8_t                    device_idx,
                                               nrf_exmif_config_t const * p_device);

/**
 * @brief Function for enabling the EXMIF memory device.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] device_idx Device number to enable.
 */
NRF_STATIC_INLINE void nrf_exmif_device_enable(NRF_EXMIF_Type * p_reg,
                                               uint8_t          device_idx);

/**
 * @brief Function for disabling the EXMIF memory device.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] device_idx Device number to disable.
 */
NRF_STATIC_INLINE void nrf_exmif_device_disable(NRF_EXMIF_Type * p_reg,
                                                uint8_t          device_idx);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_exmif_task_trigger(NRF_EXMIF_Type * p_reg,
                                              nrf_exmif_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_exmif_task_address_get(NRF_EXMIF_Type const * p_reg,
                                                      nrf_exmif_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_exmif_reset_set(NRF_EXMIF_Type * p_reg,
                                           bool             enable)
{
    p_reg->RESET = (enable ? (EXMIF_RESET_RESET_Set << EXMIF_RESET_RESET_Pos) :
                             (EXMIF_RESET_RESET_Clear << EXMIF_RESET_RESET_Pos));
}

NRF_STATIC_INLINE void nrf_exmif_locked_access_set(NRF_EXMIF_Type * p_reg,
                                                   bool             enable)
{
    p_reg->LOCKEDACCESS =
        (enable ? (EXMIF_LOCKEDACCESS_ENABLE_Enabled << EXMIF_LOCKEDACCESS_ENABLE_Pos) :
                  (EXMIF_LOCKEDACCESS_ENABLE_Disabled << EXMIF_LOCKEDACCESS_ENABLE_Pos));
}

NRF_STATIC_INLINE void nrf_exmif_device_config(NRF_EXMIF_Type *           p_reg,
                                               uint8_t                    device_idx,
                                               nrf_exmif_config_t const * p_device)
{
    NRFX_ASSERT(p_device->size <= NRF_EXMIF_MAX_MEMORY_DEVICE_SIZE);
    NRFX_ASSERT(device_idx < NRF_EXMIF_MAX_NUMBER_OF_DEVICES);
    switch (device_idx)
    {
        case 0:
            p_reg->EXTCONF1.OFFSET = p_device->offset;
            p_reg->EXTCONF1.SIZE   = p_device->size;
            break;
        case 1:
            p_reg->EXTCONF2.OFFSET = p_device->offset;
            p_reg->EXTCONF2.SIZE   = p_device->size;
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_exmif_device_enable(NRF_EXMIF_Type * p_reg,
                                               uint8_t          device_idx)
{
    NRFX_ASSERT(device_idx < NRF_EXMIF_MAX_NUMBER_OF_DEVICES);
    switch (device_idx)
    {
        case 0:
            p_reg->EXTCONF1.ENABLE = (EXMIF_EXTCONF1_ENABLE_ENABLE_Enabled << EXMIF_EXTCONF1_ENABLE_ENABLE_Pos);
            break;
        case 1:
            p_reg->EXTCONF2.ENABLE = (EXMIF_EXTCONF2_ENABLE_ENABLE_Enabled << EXMIF_EXTCONF2_ENABLE_ENABLE_Pos);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_exmif_device_disable(NRF_EXMIF_Type * p_reg,
                                                uint8_t          device_idx)
{
    NRFX_ASSERT(device_idx < NRF_EXMIF_MAX_NUMBER_OF_DEVICES);
    switch (device_idx)
    {
        case 0:
            p_reg->EXTCONF1.ENABLE = (EXMIF_EXTCONF1_ENABLE_ENABLE_Disabled << EXMIF_EXTCONF1_ENABLE_ENABLE_Pos);
            break;
        case 1:
            p_reg->EXTCONF2.ENABLE = (EXMIF_EXTCONF2_ENABLE_ENABLE_Disabled << EXMIF_EXTCONF2_ENABLE_ENABLE_Pos);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_EXMIF_H__
