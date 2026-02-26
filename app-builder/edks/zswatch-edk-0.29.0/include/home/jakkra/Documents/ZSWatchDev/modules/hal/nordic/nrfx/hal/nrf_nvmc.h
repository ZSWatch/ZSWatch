/*
 * Copyright (c) 2012 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_NVMC_H__
#define NRF_NVMC_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_nvmc_hal NVMC HAL
 * @{
 * @ingroup nrf_nvmc
 * @brief   Hardware access layer (HAL) for managing the Non-Volatile Memory Controller (NVMC) peripheral.
 */

#if defined(NVMC_ERASEPAGEPARTIALCFG_DURATION_Msk) || defined(NVMC_CONFIG_WEN_PEen) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the option of page partial erase is present. */
#define NRF_NVMC_HAS_PARTIAL_ERASE 1
#else
#define NRF_NVMC_HAS_PARTIAL_ERASE 0
#endif

#if defined(NVMC_CONFIG_WEN_PEen) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether page partial erase mode is present. */
#define NRF_NVMC_HAS_PARTIAL_ERASE_MODE 1
#else
#define NRF_NVMC_HAS_PARTIAL_ERASE_MODE 0
#endif

#if defined(NVMC_CONFIGNS_WEN_Msk) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether NVMC has non-secure operations available. */
#define NRF_NVMC_HAS_NON_SECURE_OPERATIONS 1
#else
#define NRF_NVMC_HAS_NON_SECURE_OPERATIONS 0
#endif

#if defined(NVMC_ERASEUICR_ERASEUICR_Msk) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether NVMC has UICR erase available. */
#define NRF_NVMC_HAS_UICR_ERASE 1
#else
#define NRF_NVMC_HAS_UICR_ERASE 0
#endif

#if defined(NVMC_READYNEXT_READYNEXT_Msk) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether write ready check is available */
#define NRF_NVMC_HAS_WRITE_READY_CHECK 1
#else
#define NRF_NVMC_HAS_WRITE_READY_CHECK 0
#endif

/** @brief NVMC modes. */
typedef enum
{
    NRF_NVMC_MODE_READONLY      = NVMC_CONFIG_WEN_Ren, ///< NVMC in read-only mode.
    NRF_NVMC_MODE_WRITE         = NVMC_CONFIG_WEN_Wen, ///< NVMC in read and write mode.
    NRF_NVMC_MODE_ERASE         = NVMC_CONFIG_WEN_Een, ///< NVMC in read and erase mode.
#if NRF_NVMC_HAS_PARTIAL_ERASE_MODE
    NRF_NVMC_MODE_PARTIAL_ERASE = NVMC_CONFIG_WEN_PEen ///< NVMC in read and partial erase mode.
#endif
} nrf_nvmc_mode_t;

#if NRF_NVMC_HAS_NON_SECURE_OPERATIONS
/** @brief Non-secure NVMC modes. */
typedef enum
{
    NRF_NVMC_NS_MODE_READONLY = NVMC_CONFIGNS_WEN_Ren, ///< Non-secure NVMC in read-only mode.
    NRF_NVMC_NS_MODE_WRITE    = NVMC_CONFIGNS_WEN_Wen, ///< Non-secure NVMC in read and write mode.
    NRF_NVMC_NS_MODE_ERASE    = NVMC_CONFIGNS_WEN_Een, ///< Non-secure NVMC in read and erase mode.
} nrf_nvmc_ns_mode_t;
#endif

#if defined(NVMC_FEATURE_CACHE_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @brief NVMC ICache configuration. */
typedef enum
{
    NRF_NVMC_ICACHE_DISABLE               = NVMC_ICACHECNF_CACHEEN_Disabled, ///< Instruction Cache disabled.
    NRF_NVMC_ICACHE_ENABLE                = NVMC_ICACHECNF_CACHEEN_Enabled,  ///< Instruction Cache enabled.
    NRF_NVMC_ICACHE_ENABLE_WITH_PROFILING = NVMC_ICACHECNF_CACHEEN_Enabled | ///< Instruction Cache with cache profiling enabled.
                                            NVMC_ICACHECNF_CACHEPROFEN_Msk
} nrf_nvmc_icache_config_t;
#endif // defined(NVMC_FEATURE_CACHE_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for checking if NVMC is ready to perform write or erase operation.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  NVMC can perform write or erase.
 * @retval false NVMC is busy and cannot perform next operation yet.
 */
NRF_STATIC_INLINE bool nrf_nvmc_ready_check(NRF_NVMC_Type const * p_reg);

#if NRF_NVMC_HAS_WRITE_READY_CHECK
/**
 * @brief Function for checking if NVMC is ready to accept the next write operation.
 *
 * NVM writing time can be reduced by using this function.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  NVMC can accept the next write. It will be buffered and will be taken
 *               into account as soon as the ongoing write operation is completed.
 * @retval false NVMC is busy and cannot accept the next write yet.
 */
NRF_STATIC_INLINE bool nrf_nvmc_write_ready_check(NRF_NVMC_Type const * p_reg);
#endif // NRF_NVMC_HAS_WRITE_READY_CHECK

/**
 * @brief Function for setting the NVMC mode.
 *
 * Only activate erase and write modes when they are actively used.
 * If Instruction Cache (ICache) is present, enabling write or erase will
 * invalidate the cache and keep it invalidated.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Desired operating mode for NVMC.
 */
NRF_STATIC_INLINE void nrf_nvmc_mode_set(NRF_NVMC_Type * p_reg,
                                         nrf_nvmc_mode_t mode);

#if NRF_NVMC_HAS_NON_SECURE_OPERATIONS
/**
 * @brief Function for setting the NVMC mode for non-secure Flash page operations.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Desired operating mode for NVMC.
 */
NRF_STATIC_INLINE void nrf_nvmc_nonsecure_mode_set(NRF_NVMC_Type *    p_reg,
                                                   nrf_nvmc_ns_mode_t mode);
#endif

/**
 * @brief Function for writing a 32-bit word to flash.
 *
 * @note Before calling this function, the caller must ensure that:
 *     - the @p address is word-aligned,
 *     - write mode is enabled, using @ref nrf_nvmc_mode_set,
 *     - the NVMC is ready to accept another write, using
 *       @ref nrf_nvmc_ready_check or @ref nrf_nvmc_write_ready_check,
 *     - read-only mode is enabled as soon as writing is no longer needed,
 *       using @ref nrf_nvmc_mode_set.
 *
 * @warning It is recommended to use @ref nrfx_nvmc_word_write function instead.
 *
 * Using this function when accessing the flash gives the possibility
 * to run the code in an environment where the flash is simulated.
 *
 * @param[in] address  Address of the word to write.
 * @param[in] value    Value to write.
 */
NRF_STATIC_INLINE void nrf_nvmc_word_write(uint32_t address,
                                           uint32_t value);

/**
 * @brief Function for reading a byte from the flash.
 *
 * Using this function when accessing the flash gives the possibility
 * to run the code in an environment where the flash is simulated.
 *
 * @param[in] address Address of the byte to read.
 *
 * @return Value read from flash.
 */
NRF_STATIC_INLINE uint8_t nrf_nvmc_byte_read(uint32_t address);

/**
 * @brief Function for reading a 16-bit halfword from the flash.
 *
 * Using this function when accessing the flash gives the possibility
 * to run the code in an environment where the flash is simulated.
 *
 * @param[in] address Address of the halfword to read.
 *
 * @return Value read from flash.
 */
NRF_STATIC_INLINE uint16_t nrf_nvmc_halfword_read(uint32_t address);

/**
 * @brief Function for reading a 32-bit word from the flash.
 *
 * Using this function when accessing the flash gives the possibility
 * to run the code in an environment where the flash is simulated.
 *
 * @param[in] address Address of the word to read.
 *
 * @return Value read from flash.
 */
NRF_STATIC_INLINE uint32_t nrf_nvmc_word_read(uint32_t address);

/**
 * @brief Function for reading a given number of bytes from the flash into the specified buffer.
 *
 * Using this function when accessing the flash gives the possibility
 * to run the code in an environment where the flash is simulated.
 *
 * @param[in] dst       Pointer to the buffer to store the data.
 * @param[in] address   Address of the first byte to read.
 * @param[in] num_bytes Number of bytes to read.
 *
 */
NRF_STATIC_INLINE void nrf_nvmc_buffer_read(void *   dst,
                                            uint32_t address,
                                            uint32_t num_bytes);

/**
 * @brief Function for starting a single page erase in the Flash memory.
 *
 * The NVMC mode must be correctly configured with @ref nrf_nvmc_mode_set
 * before starting the erase operation.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] page_addr Address of the first word of the page to erase.
 */
NRF_STATIC_INLINE void nrf_nvmc_page_erase_start(NRF_NVMC_Type * p_reg,
                                                 uint32_t        page_addr);

#if NRF_NVMC_HAS_UICR_ERASE
/**
 * @brief Function for starting the user information configuration registers (UICR) erase.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_nvmc_uicr_erase_start(NRF_NVMC_Type * p_reg);
#endif

/**
 * @brief Function for starting the erase of the whole NVM, including UICR.
 *
 * This function purges all user code.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_nvmc_erase_all_start(NRF_NVMC_Type * p_reg);

#if NRF_NVMC_HAS_PARTIAL_ERASE
/**
 * @brief Function for configuring the page partial erase duration in milliseconds.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] duration_ms Page partial erase duration in milliseconds.
 */
NRF_STATIC_INLINE void nrf_nvmc_partial_erase_duration_set(NRF_NVMC_Type * p_reg,
                                                           uint32_t        duration_ms);

/**
 * @brief Function for getting the current setting for the page partial erase duration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Interval duration setting in milliseconds.
 */
NRF_STATIC_INLINE uint32_t nrf_nvmc_partial_erase_duration_get(NRF_NVMC_Type const * p_reg);

/**
 * @brief Function for starting a partial erase operation.
 *
 * It must be called successively until the page erase time is reached.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] page_addr Address of the first word of the page to erase.
 */
NRF_STATIC_INLINE void nrf_nvmc_page_partial_erase_start(NRF_NVMC_Type * p_reg,
                                                         uint32_t        page_addr);
#endif // NRF_NVMC_HAS_PARTIAL_ERASE

#if defined(NVMC_FEATURE_CACHE_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for applying the Instruction Cache (ICache) configuration.
 *
 * Enabling the cache can increase CPU performance and reduce power
 * consumption by reducing the number of wait cycles and the number
 * of flash accesses.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] config ICache configuration.
 */
NRF_STATIC_INLINE void nrf_nvmc_icache_config_set(NRF_NVMC_Type *          p_reg,
                                                  nrf_nvmc_icache_config_t config);

/**
 * @brief Function for checking if ICache is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  ICache enabled.
 * @retval false ICache disabled.
 */
NRF_STATIC_INLINE bool nrf_nvmc_icache_enable_check(NRF_NVMC_Type const * p_reg);

/**
 * @brief Function for checking if the ICache profiling option is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  ICache profiling enabled.
 * @retval false ICache profiling disabled.
 */
NRF_STATIC_INLINE bool nrf_nvmc_icache_profiling_enable_check(NRF_NVMC_Type const * p_reg);

/**
 * @brief Function for getting the number of ICache hits.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Number of the ICache hits.
 */
NRF_STATIC_INLINE uint32_t nrf_nvmc_icache_hit_get(NRF_NVMC_Type const * p_reg);

/**
 * @brief Function for getting the number of ICache misses.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Number of the ICache misses.
 */
NRF_STATIC_INLINE uint32_t nrf_nvmc_icache_miss_get(NRF_NVMC_Type const * p_reg);

/**
 * @brief Function for resetting the ICache hit and miss counters.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
 NRF_STATIC_INLINE void nrf_nvmc_icache_hit_miss_reset(NRF_NVMC_Type * p_reg);
#endif // defined(NVMC_FEATURE_CACHE_PRESENT) || defined(__NRFX_DOXYGEN__)

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE bool nrf_nvmc_ready_check(NRF_NVMC_Type const * p_reg)
{
    return (bool)(p_reg->READY & NVMC_READY_READY_Msk);
}

#if NRF_NVMC_HAS_WRITE_READY_CHECK
NRF_STATIC_INLINE bool nrf_nvmc_write_ready_check(NRF_NVMC_Type const * p_reg)
{
    return (bool)(p_reg->READYNEXT & NVMC_READYNEXT_READYNEXT_Msk);
}
#endif

NRF_STATIC_INLINE void nrf_nvmc_mode_set(NRF_NVMC_Type * p_reg,
                                         nrf_nvmc_mode_t mode)
{
    p_reg->CONFIG = (uint32_t)mode;
}

#if NRF_NVMC_HAS_NON_SECURE_OPERATIONS
NRF_STATIC_INLINE void nrf_nvmc_nonsecure_mode_set(NRF_NVMC_Type *    p_reg,
                                                   nrf_nvmc_ns_mode_t mode)
{
    p_reg->CONFIGNS = (uint32_t)mode;
}
#endif

NRF_STATIC_INLINE void nrf_nvmc_word_write(uint32_t address,
                                           uint32_t value)
{
    *(volatile uint32_t *)address = value;
}

NRF_STATIC_INLINE uint8_t nrf_nvmc_byte_read(uint32_t address)
{
    return *(volatile uint8_t *)address;
}

NRF_STATIC_INLINE uint16_t nrf_nvmc_halfword_read(uint32_t address)
{
    return *(volatile uint16_t *)address;
}

NRF_STATIC_INLINE uint32_t nrf_nvmc_word_read(uint32_t address)
{
    return *(volatile uint32_t *)address;
}

NRF_STATIC_INLINE void nrf_nvmc_buffer_read(void *   dst,
                                            uint32_t address,
                                            uint32_t num_bytes)
{
    memcpy(dst, (void *)address, num_bytes);
}

NRF_STATIC_INLINE void nrf_nvmc_page_erase_start(NRF_NVMC_Type * p_reg,
                                                 uint32_t        page_addr)
{
#if defined(UICR_CLENR0_CLENR0_Msk)
    /* On certain targets, the code area can be divided into two regions: CR0 and CR1.
     * The length of CR0 is specified in the CLENR0 register of UICR.
     * If CLENR0 contains the 0xFFFFFFFF value,  CR0 is not set.
     * Moreover, the page from CR0 can be written or erased only from code
     * running in CR0.*/
    uint32_t cr0_len = NRF_UICR->CLENR0 == 0xFFFFFFFF ? 0 : NRF_UICR->CLENR0;
    if (page_addr < cr0_len)
    {
        p_reg->ERASEPCR0 = page_addr;
    }
    else
    {
        p_reg->ERASEPCR1 = page_addr;
    }
#elif defined(NVMC_ERASEPAGE_ERASEPAGE_Msk)
    p_reg->ERASEPAGE = page_addr;
#else
    *(volatile uint32_t *)page_addr = 0xFFFFFFFF;
    (void)p_reg;
#endif
}

#if NRF_NVMC_HAS_UICR_ERASE
NRF_STATIC_INLINE void nrf_nvmc_uicr_erase_start(NRF_NVMC_Type * p_reg)
{
    p_reg->ERASEUICR = 1;
}
#endif

NRF_STATIC_INLINE void nrf_nvmc_erase_all_start(NRF_NVMC_Type * p_reg)
{
    p_reg->ERASEALL = 1;
}

#if NRF_NVMC_HAS_PARTIAL_ERASE
NRF_STATIC_INLINE void nrf_nvmc_partial_erase_duration_set(NRF_NVMC_Type * p_reg,
                                                           uint32_t        duration_ms)
{
    p_reg->ERASEPAGEPARTIALCFG = duration_ms;
}

NRF_STATIC_INLINE uint32_t nrf_nvmc_partial_erase_duration_get(NRF_NVMC_Type const * p_reg)
{
    return p_reg->ERASEPAGEPARTIALCFG;
}

NRF_STATIC_INLINE void nrf_nvmc_page_partial_erase_start(NRF_NVMC_Type * p_reg,
                                                         uint32_t        page_addr)
{
#if defined(NVMC_ERASEPAGEPARTIAL_ERASEPAGEPARTIAL_Msk)
    p_reg->ERASEPAGEPARTIAL = page_addr;
#else
    nrf_nvmc_page_erase_start(p_reg, page_addr);
#endif
}
#endif // NRF_NVMC_HAS_PARTIAL_ERASE

#if defined(NVMC_FEATURE_CACHE_PRESENT)
NRF_STATIC_INLINE void nrf_nvmc_icache_config_set(NRF_NVMC_Type *          p_reg,
                                                  nrf_nvmc_icache_config_t config)
{
    if ((NRF_ERRATA_DYNAMIC_CHECK(53, 6) || NRF_ERRATA_DYNAMIC_CHECK(91, 21)) &&
        (config == NRF_NVMC_ICACHE_DISABLE))
    {
        NRFX_CRITICAL_SECTION_ENTER();
        __ISB();
        p_reg->ICACHECNF = (uint32_t)NRF_NVMC_ICACHE_DISABLE;
        __ISB();
        NRFX_CRITICAL_SECTION_EXIT();
    }
    else
    {
        p_reg->ICACHECNF = (uint32_t)config;
    }
}

NRF_STATIC_INLINE bool nrf_nvmc_icache_enable_check(NRF_NVMC_Type const * p_reg)
{
    return (bool)(p_reg->ICACHECNF & NVMC_ICACHECNF_CACHEEN_Msk);
}

NRF_STATIC_INLINE bool nrf_nvmc_icache_profiling_enable_check(NRF_NVMC_Type const * p_reg)
{
    return (bool)(p_reg->ICACHECNF & NVMC_ICACHECNF_CACHEPROFEN_Msk);
}

NRF_STATIC_INLINE uint32_t nrf_nvmc_icache_hit_get(NRF_NVMC_Type const * p_reg)
{
    return p_reg->IHIT;
}

NRF_STATIC_INLINE uint32_t nrf_nvmc_icache_miss_get(NRF_NVMC_Type const * p_reg)
{
    return p_reg->IMISS;
}

NRF_STATIC_INLINE void nrf_nvmc_icache_hit_miss_reset(NRF_NVMC_Type * p_reg)
{
    p_reg->IHIT = 0;
    p_reg->IMISS = 0;
}
#endif // defined(NVMC_FEATURE_CACHE_PRESENT)

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_NVMC_H__
