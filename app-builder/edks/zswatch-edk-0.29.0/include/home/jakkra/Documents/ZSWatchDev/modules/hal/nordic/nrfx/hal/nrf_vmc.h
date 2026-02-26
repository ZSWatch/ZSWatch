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

#ifndef NRF_VMC_H__
#define NRF_VMC_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_vmc_hal VMC HAL
 * @{
 * @ingroup nrf_vmc
 * @brief   Hardware access layer for managing the Volatile Memory Controller (VMC) peripheral.
 */

/** @brief Number of RAM sections. */
#define NRF_VMC_RAM_SECTION_COUNT VMC_RAM_SECTION_COUNT

/** @brief Power configuration bits for each section in particular RAM block. */
typedef enum
{
    NRF_VMC_POWER_S0  = VMC_RAM_POWER_S0POWER_Msk,  ///< Keep retention on RAM section S0 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S1  = VMC_RAM_POWER_S1POWER_Msk,  ///< Keep retention on RAM section S1 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S2  = VMC_RAM_POWER_S2POWER_Msk,  ///< Keep retention on RAM section S2 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S3  = VMC_RAM_POWER_S3POWER_Msk,  ///< Keep retention on RAM section S3 of the particular RAM block when RAM section is switched off.
#if (NRF_VMC_RAM_SECTION_COUNT > 4) || defined(__NRFX_DOXYGEN__)
    NRF_VMC_POWER_S4  = VMC_RAM_POWER_S4POWER_Msk,  ///< Keep retention on RAM section S4 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S5  = VMC_RAM_POWER_S5POWER_Msk,  ///< Keep retention on RAM section S5 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S6  = VMC_RAM_POWER_S6POWER_Msk,  ///< Keep retention on RAM section S6 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S7  = VMC_RAM_POWER_S7POWER_Msk,  ///< Keep retention on RAM section S7 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S8  = VMC_RAM_POWER_S8POWER_Msk,  ///< Keep retention on RAM section S8 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S9  = VMC_RAM_POWER_S9POWER_Msk,  ///< Keep retention on RAM section S9 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S10 = VMC_RAM_POWER_S10POWER_Msk, ///< Keep retention on RAM section S10 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S11 = VMC_RAM_POWER_S11POWER_Msk, ///< Keep retention on RAM section S11 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S12 = VMC_RAM_POWER_S12POWER_Msk, ///< Keep retention on RAM section S12 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S13 = VMC_RAM_POWER_S13POWER_Msk, ///< Keep retention on RAM section S13 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S14 = VMC_RAM_POWER_S14POWER_Msk, ///< Keep retention on RAM section S14 of the particular RAM block when RAM section is switched off.
    NRF_VMC_POWER_S15 = VMC_RAM_POWER_S15POWER_Msk, ///< Keep retention on RAM section S15 of the particular RAM block when RAM section is switched off.
#endif
} nrf_vmc_power_t;

/** @brief Position of power configuration bits for RAM section 0. */
#define NRF_VMC_POWER_S0_POS VMC_RAM_POWER_S0POWER_Pos

/** @brief Position of retention configuration bits for RAM section 0. */
#define NRF_VMC_RETENTION_S0_POS VMC_RAM_POWER_S0RETENTION_Pos

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

/** @brief Retention configuration bits for each section in particular RAM block. */
typedef enum
{
    NRF_VMC_RETENTION_S0  = VMC_RAM_POWER_S0RETENTION_Msk,  ///< Keep RAM section S0 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S1  = VMC_RAM_POWER_S1RETENTION_Msk,  ///< Keep RAM section S1 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S2  = VMC_RAM_POWER_S2RETENTION_Msk,  ///< Keep RAM section S2 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S3  = VMC_RAM_POWER_S3RETENTION_Msk,  ///< Keep RAM section S3 of the particular RAM block on or off in System ON mode.
#if (NRF_VMC_RAM_SECTION_COUNT > 4) || defined(__NRFX_DOXYGEN__)
    NRF_VMC_RETENTION_S4  = VMC_RAM_POWER_S4RETENTION_Msk,  ///< Keep RAM section S4 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S5  = VMC_RAM_POWER_S5RETENTION_Msk,  ///< Keep RAM section S5 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S6  = VMC_RAM_POWER_S6RETENTION_Msk,  ///< Keep RAM section S6 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S7  = VMC_RAM_POWER_S7RETENTION_Msk,  ///< Keep RAM section S7 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S8  = VMC_RAM_POWER_S8RETENTION_Msk,  ///< Keep RAM section S8 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S9  = VMC_RAM_POWER_S9RETENTION_Msk,  ///< Keep RAM section S9 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S10 = VMC_RAM_POWER_S10RETENTION_Msk, ///< Keep RAM section S10 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S11 = VMC_RAM_POWER_S11RETENTION_Msk, ///< Keep RAM section S11 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S12 = VMC_RAM_POWER_S12RETENTION_Msk, ///< Keep RAM section S12 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S13 = VMC_RAM_POWER_S13RETENTION_Msk, ///< Keep RAM section S13 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S14 = VMC_RAM_POWER_S14RETENTION_Msk, ///< Keep RAM section S14 of the particular RAM block on or off in System ON mode.
    NRF_VMC_RETENTION_S15 = VMC_RAM_POWER_S15RETENTION_Msk, ///< Keep RAM section S15 of the particular RAM block on or off in System ON mode.
#endif
} nrf_vmc_retention_t;

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

/**
 * @brief Function for setting power configuration for the particular RAM block.
 *
 * @note Overrides current configuration.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] ram_block_num  RAM block number.
 * @param[in] power_mask     Bitmask with sections configuration of particular RAM block.
 *                           @ref nrf_vmc_power_t should be use to prepare this bitmask.
 * @param[in] retention_mask Bitmask with sections configuration of particular RAM block.
 *                           @ref nrf_vmc_retention_t should be use to prepare this bitmask.
 */
NRF_STATIC_INLINE void nrf_vmc_ram_block_config(NRF_VMC_Type * p_reg,
                                                uint8_t        ram_block_num,
                                                uint32_t       power_mask,
                                                uint32_t       retention_mask);

/**
 * @brief Function for clearing power configuration for the particular RAM block.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] ram_block_num RAM block number.
 */
NRF_STATIC_INLINE void nrf_vmc_ram_block_clear(NRF_VMC_Type * p_reg, uint8_t ram_block_num);

/**
 * @brief Function for setting power configuration for the particular RAM block.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] ram_block_num RAM block number.
 * @param[in] sect_power    Paricular section of the RAM block.
 */
NRF_STATIC_INLINE void nrf_vmc_ram_block_power_set(NRF_VMC_Type *  p_reg,
                                                   uint8_t         ram_block_num,
                                                   nrf_vmc_power_t sect_power);

/**
 * @brief Function for setting power configuration for all available RAM blocks.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_vmc_ram_block_power_all_set(NRF_VMC_Type * p_reg);

/**
 * @brief Function for clearing power configuration for the particular RAM block.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] ram_block_num RAM block number.
 * @param[in] sect_power    Paricular section of the RAM block.
 */
NRF_STATIC_INLINE void nrf_vmc_ram_block_power_clear(NRF_VMC_Type *  p_reg,
                                                     uint8_t         ram_block_num,
                                                     nrf_vmc_power_t sect_power);

/**
 * @brief Function for getting power configuration of the particular RAM block.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] ram_block_num RAM block number.
 *
 * @return Bitmask with power configuration of sections of particular RAM block.
 */
NRF_STATIC_INLINE uint32_t nrf_vmc_ram_block_power_mask_get(NRF_VMC_Type const * p_reg,
                                                            uint8_t              ram_block_num);

/**
 * @brief Function for setting retention configuration for the particular RAM block.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] ram_block_num  RAM block number.
 * @param[in] sect_retention Paricular section of the RAM block.
 */
NRF_STATIC_INLINE void nrf_vmc_ram_block_retention_set(NRF_VMC_Type *      p_reg,
                                                       uint8_t             ram_block_num,
                                                       nrf_vmc_retention_t sect_retention);

/**
 * @brief Function for setting retention configuration for all available RAM blocks.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_vmc_ram_block_retention_all_set(NRF_VMC_Type * p_reg);

/**
 * @brief Function for clearing retention configuration for the particular RAM block.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] ram_block_num  RAM block number.
 * @param[in] sect_retention Paricular section of the RAM block.
 */
NRF_STATIC_INLINE void nrf_vmc_ram_block_retention_clear(NRF_VMC_Type *      p_reg,
                                                         uint8_t             ram_block_num,
                                                         nrf_vmc_retention_t sect_retention);

/**
 * @brief Function for getting retention configuration of the particular RAM block.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] ram_block_num RAM block number.
 *
 * @return Bitmask with retention configuration of sections of particular RAM block
 */
NRF_STATIC_INLINE uint32_t nrf_vmc_ram_block_retention_mask_get(NRF_VMC_Type const * p_reg,
                                                                uint8_t              ram_block_num);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_vmc_ram_block_config(NRF_VMC_Type * p_reg,
                                                uint8_t        ram_block_num,
                                                uint32_t       power_mask,
                                                uint32_t       retention_mask)
{
    p_reg->RAM[ram_block_num].POWER =
            (power_mask & (NRFX_BIT_MASK(VMC_RAM_SECTION_COUNT) << VMC_RAM_POWER_S0POWER_Pos)) |
            (retention_mask & (NRFX_BIT_MASK(VMC_RAM_SECTION_COUNT) <<
                               VMC_RAM_POWER_S0RETENTION_Pos));
    // Ensure that memory write operation is completed.
    __DSB();
}

NRF_STATIC_INLINE void nrf_vmc_ram_block_clear(NRF_VMC_Type * p_reg, uint8_t ram_block_num)
{
    p_reg->RAM[ram_block_num].POWER = 0;
}

NRF_STATIC_INLINE void nrf_vmc_ram_block_power_set(NRF_VMC_Type *  p_reg,
                                                   uint8_t         ram_block_num,
                                                   nrf_vmc_power_t sect_power)
{
    p_reg->RAM[ram_block_num].POWERSET = (uint32_t)sect_power;
    // Ensure that memory write operation is completed.
    __DSB();
}

NRF_STATIC_INLINE void nrf_vmc_ram_block_power_all_set(NRF_VMC_Type * p_reg)
{
    for (size_t i = 0; i < VMC_FEATURE_RAM_REGISTERS_COUNT; i++)
    {
        p_reg->RAM[i].POWERSET = NRF_VMC_POWER_S0 | NRF_VMC_POWER_S1 |
                                 NRF_VMC_POWER_S2 | NRF_VMC_POWER_S3 |
#if (NRF_VMC_RAM_SECTION_COUNT > 4)
                                 NRF_VMC_POWER_S4  | NRF_VMC_POWER_S5  |
                                 NRF_VMC_POWER_S6  | NRF_VMC_POWER_S7  |
                                 NRF_VMC_POWER_S8  | NRF_VMC_POWER_S9  |
                                 NRF_VMC_POWER_S10 | NRF_VMC_POWER_S11 |
                                 NRF_VMC_POWER_S12 | NRF_VMC_POWER_S13 |
                                 NRF_VMC_POWER_S14 | NRF_VMC_POWER_S15 |
#endif
                                 0;
    }
    // Ensure that memory write operation is completed.
    __DSB();
}

NRF_STATIC_INLINE void nrf_vmc_ram_block_power_clear(NRF_VMC_Type *  p_reg,
                                                     uint8_t         ram_block_num,
                                                     nrf_vmc_power_t sect_power)
{
    p_reg->RAM[ram_block_num].POWERCLR = (uint32_t)sect_power;
}

NRF_STATIC_INLINE uint32_t nrf_vmc_ram_block_power_mask_get(NRF_VMC_Type const * p_reg,
                                                            uint8_t              ram_block_num)
{
    return p_reg->RAM[ram_block_num].POWER & (NRFX_BIT_MASK(VMC_RAM_SECTION_COUNT) <<
                                              VMC_RAM_POWER_S0POWER_Pos);
}

NRF_STATIC_INLINE void nrf_vmc_ram_block_retention_set(NRF_VMC_Type *      p_reg,
                                                       uint8_t             ram_block_num,
                                                       nrf_vmc_retention_t sect_retention)
{
    p_reg->RAM[ram_block_num].POWERSET = (uint32_t)sect_retention;
    // Ensure that memory write operation is completed.
    __DSB();
}

NRF_STATIC_INLINE void nrf_vmc_ram_block_retention_all_set(NRF_VMC_Type * p_reg)
{
    for (size_t i = 0; i < VMC_FEATURE_RAM_REGISTERS_COUNT; i++)
    {
        p_reg->RAM[i].POWERSET = NRF_VMC_RETENTION_S0 | NRF_VMC_RETENTION_S1 |
                                 NRF_VMC_RETENTION_S2 | NRF_VMC_RETENTION_S3 |
#if (NRF_VMC_RAM_SECTION_COUNT > 4)
                                 NRF_VMC_RETENTION_S4  | NRF_VMC_RETENTION_S5 |
                                 NRF_VMC_RETENTION_S6  | NRF_VMC_RETENTION_S7 |
                                 NRF_VMC_RETENTION_S8  | NRF_VMC_RETENTION_S9 |
                                 NRF_VMC_RETENTION_S10 | NRF_VMC_RETENTION_S11 |
                                 NRF_VMC_RETENTION_S12 | NRF_VMC_RETENTION_S13 |
                                 NRF_VMC_RETENTION_S14 | NRF_VMC_RETENTION_S15 |
#endif
                                 0;
    }
    // Ensure that memory write operation is completed.
    __DSB();
}

NRF_STATIC_INLINE void nrf_vmc_ram_block_retention_clear(NRF_VMC_Type *      p_reg,
                                                         uint8_t             ram_block_num,
                                                         nrf_vmc_retention_t sect_retention)
{
    p_reg->RAM[ram_block_num].POWERCLR = (uint32_t)sect_retention;
}

NRF_STATIC_INLINE uint32_t nrf_vmc_ram_block_retention_mask_get(NRF_VMC_Type const * p_reg,
                                                                uint8_t              ram_block_num)
{
    return p_reg->RAM[ram_block_num].POWER & (NRFX_BIT_MASK(VMC_RAM_SECTION_COUNT) <<
                                              VMC_RAM_POWER_S0RETENTION_Pos);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_VMC_H__
