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

#ifndef NRF_MEMCONF_H__
#define NRF_MEMCONF_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_memconf_hal MEMCONF HAL
 * @{
 * @ingroup nrf_memconf
 * @brief   Hardware access layer for managing the Memory Configuration (MEMCONF) peripheral.
 */

#if defined(MEMCONF_POWER_RET2_MEM0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the second retention configuration register is present. */
#define NRF_MEMCONF_HAS_RET2 1
#else
#define NRF_MEMCONF_HAS_RET2 0
#endif

#if defined(MEMCONF_REPAIR_BITLINE_ADDR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the repair configuration for RAM blocks is present. */
#define NRF_MEMCONF_HAS_REPAIR 1
#else
#define NRF_MEMCONF_HAS_REPAIR 0
#endif

#if defined(MEMCONF_BLOCKTYPE_TRIM_MEMTRIM0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the trim configuration is present. */
#define NRF_MEMCONF_HAS_TRIM 1
#else
#define NRF_MEMCONF_HAS_TRIM 0
#endif

/** @brief Symbol specifying maximum number of available power blocks. */
#define NRF_MEMCONF_POWERBLOCK_COUNT MEMCONF_POWER_MaxCount

/** @brief Symbol specifying maximum number of control RAM blocks. */
#define NRF_MEMCONF_POWERBLOCK_RAMBLOCK_CONTROL_COUNT MEMCONF_POWER_CONTROL_MEM31_Pos

/** @brief Symbol specifying maximum number of retention RAM blocks. */
#define NRF_MEMCONF_POWERBLOCK_RAMBLOCK_RET_COUNT MEMCONF_POWER_RET_MEM31_Pos

/** @brief Symbol specifying maximum number of the second bank retention RAM blocks. */
#define NRF_MEMCONF_POWERBLOCK_RAMBLOCK_RET2_COUNT MEMCONF_POWER_RET2_MEM31_Pos

/**
 * @brief Macro to generate a mask bit for the memory read/write margin MEMCONF trim.
 *
 * @param[in] idx Zero-based memory margin trim index.
 * @param[in] _ Internal placeholder, to be ignored.
 */
#define NRF_MEMCONF_MEMTRIM_MASK_BIT(idx, _) \
    (NRFX_CONCAT(MEMCONF_BLOCKTYPE_TRIM_MEMTRIM, idx, _Msk))

/**
 * @brief Macro to generate a mask bit for the memory retention read/write margin MEMCONF trim.
 *
 * @param[in] idx Zero based memory margin retention trim index.
 * @param[in] _ Internal placeholder, to be ignored.
 */
#define NRF_MEMCONF_MEMRETTRIM_MASK_BIT(idx, _) \
    (NRFX_CONCAT(MEMCONF_BLOCKTYPE_TRIM_MEMRETTRIM, idx, _Msk))

#if (defined(MEMCONF_WIFI_NMEMTRIM_INTERNAL_SIZE) && \
     defined(MEMCONF_NMEMTRIM_INTERNAL_SIZE)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol determining the maximum memory read/write margin trim size across all MEMCONF variants. */
#if MEMCONF_WIFI_NMEMTRIM_INTERNAL_SIZE > MEMCONF_NMEMTRIM_INTERNAL_SIZE
#define NRF_MEMCONF_MEMTRIM_SIZE_MAX MEMCONF_WIFI_NMEMTRIM_INTERNAL_SIZE
#else
#define NRF_MEMCONF_MEMTRIM_SIZE_MAX MEMCONF_NMEMTRIM_INTERNAL_SIZE
#endif
#elif defined(MEMCONF_WIFI_NMEMTRIM_INTERNAL_SIZE)
#define NRF_MEMCONF_MEMTRIM_SIZE_MAX MEMCONF_WIFI_NMEMTRIM_INTERNAL_SIZE
#elif defined(MEMCONF_NMEMTRIM_INTERNAL_SIZE)
#define NRF_MEMCONF_MEMTRIM_SIZE_MAX MEMCONF_NMEMTRIM_INTERNAL_SIZE
#else
#define NRF_MEMCONF_MEMTRIM_SIZE_MAX 16
#endif

#if (defined(MEMCONF_WIFI_NMEMRETTRIM_INTERNAL_SIZE) && \
     defined(MEMCONF_NMEMRETTRIM_INTERNAL_SIZE)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol determining the maximum memory retention read/write margin trim size across all MEMCONF variants. */
#if MEMCONF_WIFI_NMEMRETTRIM_INTERNAL_SIZE > MEMCONF_NMEMRETTRIM_INTERNAL_SIZE
#define NRF_MEMCONF_MEMRETTRIM_SIZE_MAX MEMCONF_WIFI_NMEMRETTRIM_INTERNAL_SIZE
#else
#define NRF_MEMCONF_MEMRETTRIM_SIZE_MAX MEMCONF_NMEMRETTRIM_INTERNAL_SIZE
#endif
#elif defined(MEMCONF_WIFI_NMEMRETTRIM_INTERNAL_SIZE)
#define NRF_MEMCONF_MEMRETTRIM_SIZE_MAX MEMCONF_WIFI_NMEMRETTRIM_INTERNAL_SIZE
#elif defined(MEMCONF_NMEMRETTRIM_INTERNAL_SIZE)
#define NRF_MEMCONF_MEMRETTRIM_SIZE_MAX MEMCONF_NMEMRETTRIM_INTERNAL_SIZE
#else
#define NRF_MEMCONF_MEMRETTRIM_SIZE_MAX 16
#endif

/** @brief Symbol specifying bitmask collecting all memory read and write margin trims. */
#define NRF_MEMCONF_BLOCKTYPE_TRIM_MEMTRIM_MASK \
    (NRFX_LISTIFY(NRF_MEMCONF_MEMTRIM_SIZE_MAX, NRF_MEMCONF_MEMTRIM_MASK_BIT, (|), _))

/** @brief Symbol specifying bitmask collecting all memory retention trims. */
#define NRF_MEMCONF_BLOCKTYPE_TRIM_MEMRETTRIM_MASK \
    (NRFX_LISTIFY(NRF_MEMCONF_MEMRETTRIM_SIZE_MAX, NRF_MEMCONF_MEMRETTRIM_MASK_BIT, (|), _))

/**
 * @brief Macro for getting index of specified RAM block.
 *
 * @param[in] _ret RAM bank index. Can be empty if specified bank has no index.
 * @param[in] _mem RAM block number.
 *
 * @return RAM block index.
 */
#define NRF_MEMCONF_RAMBLOCK_INDEX(_ret, _mem) \
    NRFX_CONCAT(MEMCONF_POWER_RET, _ret, _MEM, _mem, _Pos)

/**
 * @brief Macro for getting mask of specified RAM block.
 *
 * @param[in] _ret RAM bank index. Can be empty if specified bank has no index.
 * @param[in] _mem RAM block number.
 *
 * @return RAM block mask.
 */
#define NRF_MEMCONF_RAMBLOCK_MASK(_ret, _mem) \
    NRFX_CONCAT(MEMCONF_POWER_RET, _ret, _MEM, _mem, _Msk)

/**
 * @brief Macro for getting mask of retention enabled for specified RAM block.
 *
 * @param[in] _ret RAM bank index. Can be empty if specified bank has no index.
 * @param[in] _mem RAM block number.
 *
 * @return Retention of specified RAM block mask.
 */
#define NRF_MEMCONF_RAMBLOCK_RETENTION_ON_MASK(_ret, _mem) \
    (NRFX_CONCAT(MEMCONF_POWER_RET, _ret, _MEM, _mem, _On) << \
     NRFX_CONCAT(MEMCONF_POWER_RET, _ret, _MEM, _mem, _Pos))

/**
 * @brief Function for enabling or disabling given RAM block.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] power_id Power block index.
 * @param[in] ramblock RAM block index. Use @ref NRF_MEMCONF_RAMBLOCK_INDEX for indexing RAM blocks.
 * @param[in] enable   True if RAM block is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_memconf_ramblock_control_enable_set(NRF_MEMCONF_Type * p_reg,
                                                               uint8_t            power_id,
                                                               uint8_t            ramblock,
                                                               bool               enable);

/**
 * @brief Function for enabling or disabling specified RAM blocks.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] power_id      Power block index.
 * @param[in] ramblock_mask Mask of RAM blocks. Use @ref NRF_MEMCONF_RAMBLOCK_MASK for creating RAM block masks.
 * @param[in] enable        True if RAM blocks are to be enabled, false otherwise.
 */
NRF_STATIC_INLINE
void nrf_memconf_ramblock_control_mask_enable_set(NRF_MEMCONF_Type * p_reg,
                                                  uint8_t            power_id,
                                                  uint32_t           ramblock_mask,
                                                  bool               enable);

/**
 * @brief Function for setting mask of RAM blocks powered-on in System ON mode.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] power_id      Power block index. Use @ref NRF_MEMCONF_RAMBLOCK_INDEX for indexing RAM blocks.
 * @param[in] ramblock_mask Mask of RAM blocks.
 */
NRF_STATIC_INLINE void nrf_memconf_ramblock_control_mask_set(NRF_MEMCONF_Type * p_reg,
                                                             uint8_t            power_id,
                                                             uint32_t           ramblock_mask);

/**
 * @brief Function for checking whether given RAM block is enabled.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] power_id Power block index.
 * @param[in] ramblock RAM block index. Use @ref NRF_MEMCONF_RAMBLOCK_INDEX for indexing RAM blocks.
 *
 * @retval true  RAM block is enabled.
 * @retval false RAM block is disabled.
 */
NRF_STATIC_INLINE bool nrf_memconf_ramblock_control_enable_check(NRF_MEMCONF_Type const * p_reg,
                                                                 uint8_t                  power_id,
                                                                 uint8_t                  ramblock);

/**
 * @brief Function for enabling or disabling the retention for given RAM block.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] power_id Power block index.
 * @param[in] ramblock RAM block index. Use @ref NRF_MEMCONF_RAMBLOCK_INDEX for indexing RAM blocks.
 * @param[in] enable   True if RAM block retention is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_memconf_ramblock_ret_enable_set(NRF_MEMCONF_Type * p_reg,
                                                           uint8_t            power_id,
                                                           uint8_t            ramblock,
                                                           bool               enable);

/**
 * @brief Function for enabling or disabling retention for the specified RAM blocks.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] power_id      Power block index.
 * @param[in] ramblock_mask Mask of RAM blocks. Use @ref NRF_MEMCONF_RAMBLOCK_MASK for creating RAM block masks.
 * @param[in] enable        True if retention for RAM blocks is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_memconf_ramblock_ret_mask_enable_set(NRF_MEMCONF_Type * p_reg,
                                                                uint8_t            power_id,
                                                                uint32_t           ramblock_mask,
                                                                bool               enable);

/**
 * @brief Function for setting mask of RAM blocks retained in System OFF mode.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] power_id      Power block index.
 * @param[in] ramblock_mask Mask of RAM blocks. Use @ref NRF_MEMCONF_RAMBLOCK_MASK for creating RAM block masks.
 */
NRF_STATIC_INLINE void nrf_memconf_ramblock_ret_mask_set(NRF_MEMCONF_Type * p_reg,
                                                         uint8_t            power_id,
                                                         uint32_t           ramblock_mask);

/**
 * @brief Function for checking whether the retention of specified RAM block is enabled.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] power_id Power block index.
 * @param[in] ramblock RAM block index. Use @ref NRF_MEMCONF_RAMBLOCK_INDEX for indexing RAM blocks.
 *
 * @retval true  RAM block is enabled.
 * @retval false RAM block is disabled.
 */
NRF_STATIC_INLINE bool nrf_memconf_ramblock_ret_enable_check(NRF_MEMCONF_Type const * p_reg,
                                                             uint8_t                  power_id,
                                                             uint8_t                  ramblock);

#if NRF_MEMCONF_HAS_RET2
/**
 * @brief Function for enabling or disabling the retention within the second bank for given RAM block.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] power_id Power block index.
 * @param[in] ramblock RAM block index. Use @ref NRF_MEMCONF_RAMBLOCK_INDEX for indexing RAM blocks.
 * @param[in] enable   True if RAM block retention is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_memconf_ramblock_ret2_enable_set(NRF_MEMCONF_Type * p_reg,
                                                            uint8_t            power_id,
                                                            uint8_t            ramblock,
                                                            bool               enable);

/**
 * @brief Function for checking whether the retention of the second bank in specified RAM block is enabled.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] power_id Power block index.
 * @param[in] ramblock RAM block index. Use @ref NRF_MEMCONF_RAMBLOCK_INDEX for indexing RAM blocks.
 *
 * @retval true  RAM block is enabled.
 * @retval false RAM block is disabled.
 */
NRF_STATIC_INLINE bool nrf_memconf_ramblock_ret2_enable_check(NRF_MEMCONF_Type const * p_reg,
                                                              uint8_t                  power_id,
                                                              uint8_t                  ramblock);

/**
 * @brief Function for enabling or disabling retention of the second bank for the specified RAM blocks.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] power_id      Power block index.
 * @param[in] ramblock_mask Mask of RAM blocks. Use @ref NRF_MEMCONF_RAMBLOCK_MASK for creating RAM block masks.
 * @param[in] enable        True if retention for RAM blocks is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_memconf_ramblock_ret2_mask_enable_set(NRF_MEMCONF_Type * p_reg,
                                                                 uint8_t            power_id,
                                                                 uint32_t           ramblock_mask,
                                                                 bool               enable);

/**
 * @brief Function for setting mask of RAM blocks whose secondary banks are to be retained in System OFF mode.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] power_id      Power block index.
 * @param[in] ramblock_mask Mask of RAM blocks. Use @ref NRF_MEMCONF_RAMBLOCK_MASK for creating RAM block masks.
 */
NRF_STATIC_INLINE void nrf_memconf_ramblock_ret2_mask_set(NRF_MEMCONF_Type * p_reg,
                                                          uint8_t            power_id,
                                                          uint32_t           ramblock_mask);
#endif

#if NRF_MEMCONF_HAS_REPAIR
/**
 * @brief Function for enabling or disabling given bitline.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] bitline Bitline to be enabled/disabled.
 * @param[in] enable  True if bitline is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_memconf_bitline_enable_set(NRF_MEMCONF_Type * p_reg,
                                                      uint8_t            bitline,
                                                      bool               enable);

/**
 * @brief Function for getting enable status for given bitline.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] bitline Bitline index.
 *
 * @return Status of requested bitline.
 */
NRF_STATIC_INLINE bool nrf_memconf_bitline_enable_check(NRF_MEMCONF_Type const * p_reg,
                                                        uint8_t                  bitline);

/**
 * @brief Function for setting bitline address.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] bitline Bitline index.
 * @param[in] address Addres for given bitline.
 *
 */
NRF_STATIC_INLINE void nrf_memconf_bitline_address_set(NRF_MEMCONF_Type * p_reg,
                                                       uint8_t            bitline,
                                                       uint8_t            address);

/**
 * @brief Function for getting bitline address.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] bitline Bitline index.
 *
 * @return Address of requested bitline.
 */
NRF_STATIC_INLINE uint32_t nrf_memconf_bitline_address_get(NRF_MEMCONF_Type const * p_reg,
                                                           uint8_t                  bitline);
#endif

#if NRF_MEMCONF_HAS_TRIM
/**
 * @brief Function for setting memory trim value.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] trim_id  Trim index.
 * @param[in] trim_val Trim value.
 */
NRF_STATIC_INLINE void nrf_memconf_memtrim_set(NRF_MEMCONF_Type * p_reg,
                                               uint8_t            trim_id,
                                               uint16_t           trim_val);

/**
* @brief Function for getting memory trim value.
*
* @param[in] p_reg   Pointer to the structure of registers of the peripheral.
* @param[in] trim_id Trim index.
*
* @return Requested trim value.
*/
NRF_STATIC_INLINE uint16_t nrf_memconf_memtrim_get(NRF_MEMCONF_Type const * p_reg,
                                                   uint8_t                  trim_id);

/**
 * @brief Function for setting retention trim value.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] trim_id  Trim index.
 * @param[in] trim_val Trim value.
 */
NRF_STATIC_INLINE void nrf_memconf_rettrim_set(NRF_MEMCONF_Type * p_reg,
                                               uint8_t            trim_id,
                                               uint16_t           trim_val);

/**
 * @brief Function for getting retention trim value.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] trim_id Trim index.
 *
 * @return Requested trim value.
 */
NRF_STATIC_INLINE uint16_t nrf_memconf_rettrim_get(NRF_MEMCONF_Type const * p_reg,
                                                   uint8_t                  trim_id);
#endif

#ifndef NRF_DECLARE_ONLY
NRF_STATIC_INLINE void nrf_memconf_ramblock_control_enable_set(NRF_MEMCONF_Type * p_reg,
                                                               uint8_t            power_id,
                                                               uint8_t            ramblock,
                                                               bool               enable)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);
    NRFX_ASSERT(ramblock <= NRF_MEMCONF_POWERBLOCK_RAMBLOCK_CONTROL_COUNT);

    p_reg->POWER[power_id].CONTROL = ((p_reg->POWER[power_id].CONTROL &
                                       ~(MEMCONF_POWER_CONTROL_MEM0_On << ramblock)) |
                                      ((enable ?
                                        MEMCONF_POWER_CONTROL_MEM0_On :
                                        MEMCONF_POWER_CONTROL_MEM0_Off) << ramblock));
}

NRF_STATIC_INLINE
void nrf_memconf_ramblock_control_mask_enable_set(NRF_MEMCONF_Type * p_reg,
                                                  uint8_t            power_id,
                                                  uint32_t           ramblock_mask,
                                                  bool               enable)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);

    if (enable)
    {
        p_reg->POWER[power_id].CONTROL |= ramblock_mask;
    }
    else
    {
        p_reg->POWER[power_id].CONTROL &= ~ramblock_mask;
    }
}

NRF_STATIC_INLINE void nrf_memconf_ramblock_control_mask_set(NRF_MEMCONF_Type * p_reg,
                                                             uint8_t            power_id,
                                                             uint32_t           ramblock_mask)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);

    p_reg->POWER[power_id].CONTROL = ramblock_mask;
}

NRF_STATIC_INLINE bool nrf_memconf_ramblock_control_enable_check(NRF_MEMCONF_Type const * p_reg,
                                                                 uint8_t                  power_id,
                                                                 uint8_t                  ramblock)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);
    NRFX_ASSERT(ramblock <= NRF_MEMCONF_POWERBLOCK_RAMBLOCK_CONTROL_COUNT);

    return (bool)(p_reg->POWER[power_id].CONTROL & (MEMCONF_POWER_CONTROL_MEM0_Msk << ramblock));
}

NRF_STATIC_INLINE void nrf_memconf_ramblock_ret_enable_set(NRF_MEMCONF_Type * p_reg,
                                                           uint8_t            power_id,
                                                           uint8_t            ramblock,
                                                           bool               enable)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);
    NRFX_ASSERT(ramblock <= NRF_MEMCONF_POWERBLOCK_RAMBLOCK_RET_COUNT);

    p_reg->POWER[power_id].RET = ((p_reg->POWER[power_id].RET &
                                   ~(MEMCONF_POWER_RET_MEM0_On << ramblock)) |
                                  ((enable ?
                                    MEMCONF_POWER_RET_MEM0_On :
                                    MEMCONF_POWER_RET_MEM0_Off) << ramblock));
}

NRF_STATIC_INLINE void nrf_memconf_ramblock_ret_mask_enable_set(NRF_MEMCONF_Type * p_reg,
                                                                uint8_t            power_id,
                                                                uint32_t           ramblock_mask,
                                                                bool               enable)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);

    if (enable)
    {
        p_reg->POWER[power_id].RET |= ramblock_mask;
    }
    else
    {
        p_reg->POWER[power_id].RET &= ~ramblock_mask;
    }
}

NRF_STATIC_INLINE void nrf_memconf_ramblock_ret_mask_set(NRF_MEMCONF_Type * p_reg,
                                                         uint8_t            power_id,
                                                         uint32_t           ramblock_mask)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);

    p_reg->POWER[power_id].RET = ramblock_mask;
}

NRF_STATIC_INLINE bool nrf_memconf_ramblock_ret_enable_check(NRF_MEMCONF_Type const * p_reg,
                                                             uint8_t                  power_id,
                                                             uint8_t                  ramblock)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);
    NRFX_ASSERT(ramblock <= NRF_MEMCONF_POWERBLOCK_RAMBLOCK_RET_COUNT);

    return (bool)(p_reg->POWER[power_id].RET & (MEMCONF_POWER_RET_MEM0_Msk << ramblock));
}

#if NRF_MEMCONF_HAS_RET2
NRF_STATIC_INLINE void nrf_memconf_ramblock_ret2_enable_set(NRF_MEMCONF_Type * p_reg,
                                                            uint8_t            power_id,
                                                            uint8_t            ramblock,
                                                            bool               enable)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);
    NRFX_ASSERT(ramblock <= NRF_MEMCONF_POWERBLOCK_RAMBLOCK_RET2_COUNT);

    p_reg->POWER[power_id].RET2 = ((p_reg->POWER[power_id].RET2 &
                                    ~(MEMCONF_POWER_RET2_MEM0_On << ramblock)) |
                                   ((enable ?
                                     MEMCONF_POWER_RET2_MEM0_On :
                                     MEMCONF_POWER_RET2_MEM0_Off) << ramblock));
}

NRF_STATIC_INLINE bool nrf_memconf_ramblock_ret2_enable_check(NRF_MEMCONF_Type const * p_reg,
                                                              uint8_t                  power_id,
                                                              uint8_t                  ramblock)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);
    NRFX_ASSERT(ramblock <= NRF_MEMCONF_POWERBLOCK_RAMBLOCK_RET2_COUNT);

    return (bool)(p_reg->POWER[power_id].RET2 & (MEMCONF_POWER_RET2_MEM0_Msk << ramblock));
}

NRF_STATIC_INLINE void nrf_memconf_ramblock_ret2_mask_enable_set(NRF_MEMCONF_Type * p_reg,
                                                                 uint8_t            power_id,
                                                                 uint32_t           ramblock_mask,
                                                                 bool               enable)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);

    if (enable)
    {
        p_reg->POWER[power_id].RET2 |= ramblock_mask;
    }
    else
    {
        p_reg->POWER[power_id].RET2 &= ~ramblock_mask;
    }
}

NRF_STATIC_INLINE void nrf_memconf_ramblock_ret2_mask_set(NRF_MEMCONF_Type * p_reg,
                                                          uint8_t            power_id,
                                                          uint32_t           ramblock_mask)
{
    NRFX_ASSERT(power_id < NRF_MEMCONF_POWERBLOCK_COUNT);

    p_reg->POWER[power_id].RET2 = ramblock_mask;
}
#endif

#if NRF_MEMCONF_HAS_REPAIR
NRF_STATIC_INLINE void nrf_memconf_bitline_enable_set(NRF_MEMCONF_Type * p_reg,
                                                      uint8_t            bitline,
                                                      bool               enable)
{
    p_reg->REPAIR[bitline].BITLINE = ((p_reg->REPAIR[bitline].BITLINE &
                                       ~MEMCONF_REPAIR_BITLINE_EN_Msk) |
                                      ((enable ?
                                        MEMCONF_REPAIR_BITLINE_EN_Enabled :
                                        MEMCONF_REPAIR_BITLINE_EN_Disabled)
                                        << MEMCONF_REPAIR_BITLINE_EN_Pos));
}

NRF_STATIC_INLINE bool nrf_memconf_bitline_enable_check(NRF_MEMCONF_Type const * p_reg,
                                                        uint8_t                  bitline)
{
    return (bool)(p_reg->REPAIR[bitline].BITLINE & MEMCONF_REPAIR_BITLINE_EN_Msk);
}

NRF_STATIC_INLINE void nrf_memconf_bitline_address_set(NRF_MEMCONF_Type * p_reg,
                                                       uint8_t            bitline,
                                                       uint8_t            address)
{
    p_reg->REPAIR[bitline].BITLINE = (p_reg->REPAIR[bitline].BITLINE &
                                      MEMCONF_REPAIR_BITLINE_ADDR_Msk) | (uint32_t)address;
}

NRF_STATIC_INLINE uint32_t nrf_memconf_bitline_address_get(NRF_MEMCONF_Type const * p_reg,
                                                           uint8_t                  bitline)
{
    return (uint32_t)(p_reg->REPAIR[bitline].BITLINE & MEMCONF_REPAIR_BITLINE_ADDR_Msk);
}
#endif

#if NRF_MEMCONF_HAS_TRIM
NRF_STATIC_INLINE void nrf_memconf_memtrim_set(NRF_MEMCONF_Type * p_reg,
                                               uint8_t            trim_id,
                                               uint16_t           trim_val)
{
    p_reg->BLOCKTYPE[trim_id].TRIM =
        (p_reg->BLOCKTYPE[trim_id].TRIM & ~NRF_MEMCONF_BLOCKTYPE_TRIM_MEMTRIM_MASK) |
        (trim_val << MEMCONF_BLOCKTYPE_TRIM_MEMTRIM0_Pos);
}

NRF_STATIC_INLINE uint16_t nrf_memconf_memtrim_get(NRF_MEMCONF_Type const * p_reg, uint8_t trim_id)
{
    return (uint16_t)((p_reg->BLOCKTYPE[trim_id].TRIM & NRF_MEMCONF_BLOCKTYPE_TRIM_MEMTRIM_MASK)
                      >> MEMCONF_BLOCKTYPE_TRIM_MEMTRIM0_Pos);
}

NRF_STATIC_INLINE void nrf_memconf_rettrim_set(NRF_MEMCONF_Type * p_reg,
                                               uint8_t            trim_id,
                                               uint16_t           trim_val)
{
    p_reg->BLOCKTYPE[trim_id].TRIM =
        (p_reg->BLOCKTYPE[trim_id].TRIM & ~NRF_MEMCONF_BLOCKTYPE_TRIM_MEMRETTRIM_MASK) |
        (trim_val << MEMCONF_BLOCKTYPE_TRIM_MEMRETTRIM0_Pos);
}

NRF_STATIC_INLINE uint16_t nrf_memconf_rettrim_get(NRF_MEMCONF_Type const * p_reg, uint8_t trim_id)
{
    return (uint16_t)((p_reg->BLOCKTYPE[trim_id].TRIM & NRF_MEMCONF_BLOCKTYPE_TRIM_MEMRETTRIM_MASK)
                      >> MEMCONF_BLOCKTYPE_TRIM_MEMRETTRIM0_Pos);
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_MEMCONF_H__
