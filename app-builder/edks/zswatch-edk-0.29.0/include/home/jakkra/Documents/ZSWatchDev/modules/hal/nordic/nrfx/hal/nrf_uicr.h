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

#ifndef NRF_UICR_H_
#define NRF_UICR_H_

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_uicr_hal UICR HAL
 * @{
 * @ingroup nrf_icr
 * @brief   Hardware access layer for managing the User Information Configuration Registers (UICR) peripheral.
 */

#if defined(UICR_MEM_CONFIG_READ_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether memory configuration through UICR is present. */
#define NRF_UICR_HAS_MEM_CONFIG 1
#else
#define NRF_UICR_HAS_MEM_CONFIG 0
#endif

#if defined(UICR_PERIPH_CONFIG_PROCESSOR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether peripheral configuration through UICR is present. */
#define NRF_UICR_HAS_PERIPH_CONFIG 1
#else
#define NRF_UICR_HAS_PERIPH_CONFIG 0
#endif

#if defined(UICR_GRTC_CC_OWN_CC0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether feature configuration through UICR is present. */
#define NRF_UICR_HAS_FEATURE_CONFIG 1
#else
#define NRF_UICR_HAS_FEATURE_CONFIG 0
#endif

#if defined(UICR_MAILBOX_CONFIG_SIZE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether mailbox configuration through UICR is present. */
#define NRF_UICR_HAS_MAILBOX 1
#else
#define NRF_UICR_HAS_MAILBOX 0
#endif

#if defined(UICR_INITSVTOR_INITSVTOR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether VTOR configuration through UICR is present. */
#define NRF_UICR_HAS_VTOR 1
#else
#define NRF_UICR_HAS_VTOR 0
#endif

#if defined(UICR_PTREXTUICR_PTREXTUICR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether extended UICR is present. */
#define NRF_UICR_HAS_PTREXT 1
#else
#define NRF_UICR_HAS_PTREXT 0
#endif

#if defined(UICREXTENDED_GPIO_OWN_PIN0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GPIO feature is present. */
#define NRF_UICR_HAS_FEATURE_GPIO 1
#else
#define NRF_UICR_HAS_FEATURE_GPIO 0
#endif

#if defined(UICR_BOOTCONF_READ_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether immutable boot region configuration is present. */
#define NRF_UICR_HAS_BOOTCONF 1
#else
#define NRF_UICR_HAS_BOOTCONF 0
#endif

#if (defined(UICR_DPPI_GLOBAL_CH_LINK_DIR_CH0_Msk) & \
    defined(UICR_DPPI_GLOBAL_CH_LINK_EN_CH0_Msk)) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Symbol indicating whether linking channels of DPPI as either source or sink is configured
 *        using the LINK.DIR and LINK.EN registers.
 */
#define NRF_UICR_HAS_CH_LINK_DIR_EN 1
#else
#define NRF_UICR_HAS_CH_LINK_DIR_EN 0
#endif

/** @brief Number of memory blocks. */
#define NRF_UICR_MEM_COUNT         UICR_MEM_MaxCount

/** @brief Number of peripherals. */
#define NRF_UICR_PERIPH_COUNT      UICR_PERIPH_MaxCount

#if NRF_UICR_HAS_FEATURE_GPIO
/** @brief Number of GPIOs. */
#define NRF_UICR_GPIO_COUNT        UICREXTENDED_GPIO_MaxCount
#endif

/** @brief Number of GPIOTE channels. */
#define NRF_UICR_GPIOTE_CH_COUNT   UICR_GPIOTE_MaxCount

/** @brief Number of global IPCTs. */
#define NRF_UICR_IPCT_GLOBAL_COUNT UICR_IPCT_GLOBAL_MaxCount

/** @brief Number of local IPCTs. */
#define NRF_UICR_DPPI_LOCAL_COUNT  UICR_DPPI_LOCAL_MaxCount

/** @brief Number of global DPPIs. */
#define NRF_UICR_DPPI_GLOBAL_COUNT UICR_DPPI_GLOBAL_MaxCount

/** @brief Number of IPCMAPs. */
#define NRF_UICR_IPCMAP_COUNT      UICR_IPCMAP_MaxCount

/** @brief Number of MAILBOXes. */
#define NRF_UICR_MAILBOX_COUNT     UICR_MAILBOX_MaxCount

/** @brief Immutable boot region permissions bitmask. */
#define NRF_UICR_BOOTCONF_PERM_MASK (UICR_BOOTCONF_READ_Msk | UICR_BOOTCONF_WRITE_Msk | \
                                     UICR_BOOTCONF_EXECUTE_Msk | UICR_BOOTCONF_SECURE_Msk)

#if NRF_UICR_HAS_MEM_CONFIG
/**
 * @brief Memory permissions mask.
 *
 * @note When bit is set, the selected action is not allowed.
 */
typedef enum
{
    NRF_UICR_MEM_CONFIG_PERM_READ_MASK      = UICR_MEM_CONFIG_READ_Msk,    /**< Read access. */
    NRF_UICR_MEM_CONFIG_PERM_WRITE_MASK     = UICR_MEM_CONFIG_WRITE_Msk,   /**< Write access. */
    NRF_UICR_MEM_CONFIG_PERM_EXECUTE_MASK   = UICR_MEM_CONFIG_EXECUTE_Msk, /**< Software execute. */
    NRF_UICR_MEM_CONFIG_PERM_NONSECURE_MASK = UICR_MEM_CONFIG_SECURE_Msk,  /**< Non-secure access. */
} nrf_uicr_mem_config_perm_mask_t;

/** @brief Memory configuration. */
typedef struct
{
    uint32_t    permissions; /**< Permissions. */
    nrf_owner_t owner;       /**< Owner identifier. */
    uint32_t    address;     /**< Block start address. */
} nrf_uicr_mem_config_t;
#endif

#if NRF_UICR_HAS_FEATURE_CONFIG
/**
 * @brief Feature index mask.
 *
 * @note Ownership of the pin is indicated by bit not set.
 */
typedef enum
{
    NRF_UICR_FEATURE_INDEX_0_MASK  = UICR_GPIOTE_CH_OWN_CH0_Msk,  /**< Feature index 0.  */
    NRF_UICR_FEATURE_INDEX_1_MASK  = UICR_GPIOTE_CH_OWN_CH1_Msk,  /**< Feature index 1.  */
    NRF_UICR_FEATURE_INDEX_2_MASK  = UICR_GPIOTE_CH_OWN_CH2_Msk,  /**< Feature index 2.  */
    NRF_UICR_FEATURE_INDEX_3_MASK  = UICR_GPIOTE_CH_OWN_CH3_Msk,  /**< Feature index 3.  */
    NRF_UICR_FEATURE_INDEX_4_MASK  = UICR_GPIOTE_CH_OWN_CH4_Msk,  /**< Feature index 4.  */
    NRF_UICR_FEATURE_INDEX_5_MASK  = UICR_GPIOTE_CH_OWN_CH5_Msk,  /**< Feature index 5.  */
    NRF_UICR_FEATURE_INDEX_6_MASK  = UICR_GPIOTE_CH_OWN_CH6_Msk,  /**< Feature index 6.  */
    NRF_UICR_FEATURE_INDEX_7_MASK  = UICR_GPIOTE_CH_OWN_CH7_Msk,  /**< Feature index 7.  */
    NRF_UICR_FEATURE_INDEX_8_MASK  = UICR_GPIOTE_CH_OWN_CH8_Msk,  /**< Feature index 8.  */
    NRF_UICR_FEATURE_INDEX_9_MASK  = UICR_GPIOTE_CH_OWN_CH9_Msk,  /**< Feature index 9.  */
    NRF_UICR_FEATURE_INDEX_10_MASK = UICR_GPIOTE_CH_OWN_CH10_Msk, /**< Feature index 10. */
    NRF_UICR_FEATURE_INDEX_11_MASK = UICR_GPIOTE_CH_OWN_CH11_Msk, /**< Feature index 11. */
    NRF_UICR_FEATURE_INDEX_12_MASK = UICR_GPIOTE_CH_OWN_CH12_Msk, /**< Feature index 12. */
    NRF_UICR_FEATURE_INDEX_13_MASK = UICR_GPIOTE_CH_OWN_CH13_Msk, /**< Feature index 13. */
    NRF_UICR_FEATURE_INDEX_14_MASK = UICR_GPIOTE_CH_OWN_CH14_Msk, /**< Feature index 14. */
    NRF_UICR_FEATURE_INDEX_15_MASK = UICR_GPIOTE_CH_OWN_CH15_Msk, /**< Feature index 15. */
    NRF_UICR_FEATURE_INDEX_16_MASK = UICR_GPIOTE_CH_OWN_CH16_Msk, /**< Feature index 16. */
    NRF_UICR_FEATURE_INDEX_17_MASK = UICR_GPIOTE_CH_OWN_CH17_Msk, /**< Feature index 17. */
    NRF_UICR_FEATURE_INDEX_18_MASK = UICR_GPIOTE_CH_OWN_CH18_Msk, /**< Feature index 18. */
    NRF_UICR_FEATURE_INDEX_19_MASK = UICR_GPIOTE_CH_OWN_CH19_Msk, /**< Feature index 19. */
    NRF_UICR_FEATURE_INDEX_20_MASK = UICR_GPIOTE_CH_OWN_CH20_Msk, /**< Feature index 20. */
    NRF_UICR_FEATURE_INDEX_21_MASK = UICR_GPIOTE_CH_OWN_CH21_Msk, /**< Feature index 21. */
    NRF_UICR_FEATURE_INDEX_22_MASK = UICR_GPIOTE_CH_OWN_CH22_Msk, /**< Feature index 22. */
    NRF_UICR_FEATURE_INDEX_23_MASK = UICR_GPIOTE_CH_OWN_CH23_Msk, /**< Feature index 23. */
    NRF_UICR_FEATURE_INDEX_24_MASK = UICR_GPIOTE_CH_OWN_CH24_Msk, /**< Feature index 24. */
    NRF_UICR_FEATURE_INDEX_25_MASK = UICR_GPIOTE_CH_OWN_CH25_Msk, /**< Feature index 25. */
    NRF_UICR_FEATURE_INDEX_26_MASK = UICR_GPIOTE_CH_OWN_CH26_Msk, /**< Feature index 26. */
    NRF_UICR_FEATURE_INDEX_27_MASK = UICR_GPIOTE_CH_OWN_CH27_Msk, /**< Feature index 27. */
    NRF_UICR_FEATURE_INDEX_28_MASK = UICR_GPIOTE_CH_OWN_CH28_Msk, /**< Feature index 28. */
    NRF_UICR_FEATURE_INDEX_29_MASK = UICR_GPIOTE_CH_OWN_CH29_Msk, /**< Feature index 29. */
    NRF_UICR_FEATURE_INDEX_30_MASK = UICR_GPIOTE_CH_OWN_CH30_Msk, /**< Feature index 30. */
    NRF_UICR_FEATURE_INDEX_31_MASK = UICR_GPIOTE_CH_OWN_CH31_Msk, /**< Feature index 31. */
} nrf_uicr_feature_index_mask_t;

/** @brief UICR features. */
typedef enum
{
    NRF_UICR_FEATURE_GPIO,                  /**< GPIO port. */
    NRF_UICR_FEATURE_GPIOTE_CH,             /**< GPIOTE channel. */
    NRF_UICR_FEATURE_IPCT_LOCAL_CH,         /**< Local IPCT channel. */
    NRF_UICR_FEATURE_IPCT_LOCAL_INTERRUPT,  /**< Local IPCT interrupt. */
    NRF_UICR_FEATURE_IPCT_GLOBAL_CH,        /**< Global IPCT channel. */
    NRF_UICR_FEATURE_IPCT_GLOBAL_INTERRUPT, /**< Global IPCT interrupt. */
    NRF_UICR_FEATURE_DPPI_LOCAL_CH,         /**< Local DPPI channel. */
    NRF_UICR_FEATURE_DPPI_LOCAL_CHG,        /**< Local DPPI channel group. */
    NRF_UICR_FEATURE_DPPI_GLOBAL_CH,        /**< Global DPPI channel. */
    NRF_UICR_FEATURE_DPPI_GLOBAL_CHG,       /**< Global DPPI channel group. */
    NRF_UICR_FEATURE_GRTC_CC,               /**< GRTC compare channel. */
} nrf_uicr_feature_t;

/** @brief IPCMAP pair. */
typedef struct
{
    uint8_t      ipct_channel; /**< IPCT channel number. */
    nrf_domain_t domain;       /**< Domain ID. */
} nrf_uicr_ipcmap_pair_t;

/** @brief IPCMAP configuration. */
typedef struct
{
    nrf_uicr_ipcmap_pair_t source; /**< Source side. */
    nrf_uicr_ipcmap_pair_t sink;   /**< Sink side. */
} nrf_uicr_ipcmap_config_t;

/** @brief DPPI link. */
typedef struct
{
    uint32_t source; /**< Source side. */
    uint32_t sink;   /**< Sink side. */
} nrf_uicr_dppi_link_t;
#endif // NRF_UICR_HAS_FEATURE_CONFIG

#if NRF_UICR_HAS_PERIPH_CONFIG
/** @brief Peripheral configuration. */
typedef struct
{
    bool            secattr;   /**< Security mapping. */
    bool            dmasec;    /**< Security attribution for the DMA transfer. */
    nrf_processor_t processor; /**< Processor ID. */
    uint32_t        address;   /**< Peripheral address. */
} nrf_uicr_periph_config_t;
#endif

#if NRF_UICR_HAS_MAILBOX
/** @brief MAILBOX configuration. */
typedef struct
{
    uint16_t    size;   /**< Memory size. */
    nrf_owner_t owner;  /**< Remote owner identification. */
    bool        secure; /**< Secure permission. */
} nrf_uicr_mailbox_config_t;
#endif

#if NRF_UICR_HAS_BOOTCONF
/**
 * @brief Immutable boot region permissions mask.
 *
 * @note When bit is set, the selected action is allowed.
 */
typedef enum
{
    NRF_UICR_BOOT_REGION_PERM_READ_MASK    = UICR_BOOTCONF_READ_Msk,    ///< Read access.
    NRF_UICR_BOOT_REGION_PERM_WRITE_MASK   = UICR_BOOTCONF_WRITE_Msk,   ///< Write access.
    NRF_UICR_BOOT_REGION_PERM_EXECUTE_MASK = UICR_BOOTCONF_EXECUTE_Msk, ///< Software execute.
    NRF_UICR_BOOT_REGION_PERM_SECURE_MASK  = UICR_BOOTCONF_SECURE_Msk,  ///< Secure-only access.
} nrf_uicr_boot_region_perm_mask_t;

/** @brief Immutable boot region configuration. */
typedef struct
{
    uint32_t permissions; ///< Permissions created using @ref nrf_uicr_boot_region_perm_mask_t.
    bool     writeonce;   ///< True if writes to the boot region are to be applied only when the current data is 0xFFFFFFFF.
    bool     lock;        ///< True if RRAMC configuration registers for the boot region are to be read-only.
    uint16_t size_kb;     ///< Region size in kBs. */
} nrf_uicr_boot_region_config_t;
#endif

#if NRF_UICR_HAS_MEM_CONFIG
/**
 * @brief Function for getting the configuration of the memory block.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the memory block.
 *
 * @return Configuration of the specified memory block.
 */
NRF_STATIC_INLINE nrf_uicr_mem_config_t nrf_uicr_mem_config_get(NRF_UICR_Type const * p_reg,
                                                                uint8_t               index);

/**
 * @brief Function for getting the size of the memory block.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the memory block.
 *
 * @return Size of the specified memory block in bytes.
 */
NRF_STATIC_INLINE uint32_t nrf_uicr_mem_size_get(NRF_UICR_Type const * p_reg, uint8_t index);
#endif

#if NRF_UICR_HAS_PERIPH_CONFIG
/**
 * @brief Function for getting the configuration of the peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the peripheral.
 *
 * @return Configuration of the specified peripheral.
 */
NRF_STATIC_INLINE nrf_uicr_periph_config_t nrf_uicr_periph_config_get(NRF_UICR_Type const * p_reg,
                                                                      uint8_t               index);
#endif

#if NRF_UICR_HAS_FEATURE_CONFIG
/**
 * @brief Function for getting the ownership requests of the feature.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] feature Feature to be accessed.
 * @param[in] index   Index of the feature. Only used for applicable features, otherwise skipped.
 *
 * @return Ownership requests mask of the specified feature.
 */
NRF_STATIC_INLINE uint32_t nrf_uicr_feature_own_get(NRF_UICR_Type const * p_reg,
                                                    nrf_uicr_feature_t    feature,
                                                    uint8_t               index);

/**
 * @brief Function for getting the permission requests of the feature.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] feature Feature to be accessed.
 * @param[in] index   Index of the feature. Only used for applicable features, otherwise skipped.
 *
 * @return Permission requests mask of the specified feature.
 */
NRF_STATIC_INLINE uint32_t nrf_uicr_feature_secure_get(NRF_UICR_Type const * p_reg,
                                                       nrf_uicr_feature_t    feature,
                                                       uint8_t               index);

/**
 * @brief Function for getting the linking requests of the feature.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] feature Feature to be accessed.
 * @param[in] index   Index of the feature. Only used for applicable features, otherwise skipped.
 *
 * @return Linking requests masks for source and sink of the specified feature.
 */
NRF_STATIC_INLINE nrf_uicr_dppi_link_t nrf_uicr_feature_link_get(NRF_UICR_Type const * p_reg,
                                                                 nrf_uicr_feature_t    feature,
                                                                 uint8_t               index);

/**
 * @brief Function for getting the configuration of the IPCMAP channel.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the IPCMAP channel.
 *
 * @return Configuration of the specified IPCMAP channel.
 */
NRF_STATIC_INLINE nrf_uicr_ipcmap_config_t nrf_uicr_ipcmap_config_get(NRF_UICR_Type const * p_reg,
                                                                      uint8_t               index);
#endif

#if NRF_UICR_HAS_MAILBOX
/**
 * @brief Function for getting the address of the MAILBOX.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the memory block.
 *
 * @return Start address of the specified MAILBOX.
 */
NRF_STATIC_INLINE uint32_t nrf_uicr_mailbox_address_get(NRF_UICR_Type const * p_reg, uint8_t index);

/**
 * @brief Function for getting the configuration of the MAILBOX.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the MAILBOX.
 *
 * @return Configuration of the specified MAILBOX.
 */
NRF_STATIC_INLINE
nrf_uicr_mailbox_config_t nrf_uicr_mailbox_config_get(NRF_UICR_Type const * p_reg,
                                                      uint8_t               index);
#endif

#if NRF_UICR_HAS_VTOR
/**
 * @brief Function for getting the initial value of the secure VTOR (Vector Table Offset Register).
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Initial value of the secure VTOR.
 */
NRF_STATIC_INLINE uint32_t nrf_uicr_initsvtor_get(NRF_UICR_Type const * p_reg);

/**
 * @brief Function for getting the initial value of the non-secure VTOR.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Initial value of the non-secure VTOR.
 */
NRF_STATIC_INLINE uint32_t nrf_uicr_initnsvtor_get(NRF_UICR_Type const * p_reg);
#endif

#if NRF_UICR_HAS_PTREXT
/**
 * @brief Function for getting the pointer to the extended UICR.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the extended UICR.
 */
NRF_STATIC_INLINE uint32_t * nrf_uicr_ptrextuicr_get(NRF_UICR_Type const * p_reg);
#endif

#if NRF_UICR_HAS_BOOTCONF
/**
 * @brief Function for setting the configuration of the immutable boot region.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the configuration structure.
 */
NRF_STATIC_INLINE
void nrf_uicr_boot_region_config_set(NRF_UICR_Type *                       p_reg,
                                     nrf_uicr_boot_region_config_t const * p_config);

/**
 * @brief Function for getting the configuration of the immutable boot region.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure to be filled with immutable boot region settings.
 *
 * @retval true  Configuration is applied.
 * @retval false Register is equal to 0xFFFFFFFF, meaning that configuration is not applied.
 */
NRF_STATIC_INLINE bool nrf_uicr_boot_region_config_get(NRF_UICR_Type const *           p_reg,
                                                       nrf_uicr_boot_region_config_t * p_config);
#endif

#if NRF_UICR_HAS_FEATURE_GPIO
/**
 * @brief Function for getting the GPIO instance address associated with the specified GPIO entry.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of GPIO entry.
 *
 * @return GPIO instance address.
 */
NRF_STATIC_INLINE uint32_t nrf_uicr_gpio_instance_get(NRF_UICREXTENDED_Type const * p_reg,
                                                      uint8_t                       index);

/**
 * @brief Function for getting the CTRLSEL configuration associated with the specified GPIO pin.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] pin_number Absolute pin number.
 *
 * @return CTRLSEL configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_uicr_gpio_ctrlsel_get(NRF_UICREXTENDED_Type const * p_reg,
                                                     uint32_t                      pin_number);
#endif

#ifndef NRF_DECLARE_ONLY

#if NRF_UICR_HAS_MEM_CONFIG
NRF_STATIC_INLINE nrf_uicr_mem_config_t nrf_uicr_mem_config_get(NRF_UICR_Type const * p_reg,
                                                                uint8_t               index)
{
    NRFX_ASSERT(index < NRF_UICR_MEM_COUNT);
    nrf_uicr_mem_config_t config;

    config.permissions = (p_reg->MEM[index].CONFIG &
                          (UICR_MEM_CONFIG_READ_Msk | UICR_MEM_CONFIG_WRITE_Msk |
                           UICR_MEM_CONFIG_EXECUTE_Msk | UICR_MEM_CONFIG_SECURE_Msk)
                          >> UICR_MEM_CONFIG_READ_Pos);

    config.owner = (nrf_owner_t)((p_reg->MEM[index].CONFIG & UICR_MEM_CONFIG_OWNERID_Msk) >>
                                 UICR_MEM_CONFIG_OWNERID_Pos);

    /* Address should not be bit-shifted, as it contains bits [31:12]. The rest should be all zeroes. */
    config.address = (p_reg->MEM[index].CONFIG & UICR_MEM_CONFIG_ADDRESS_Msk);

    return config;
}

NRF_STATIC_INLINE uint32_t nrf_uicr_mem_size_get(NRF_UICR_Type const * p_reg, uint8_t index)
{
    NRFX_ASSERT(index < NRF_UICR_MEM_COUNT);
    return p_reg->MEM[index].SIZE;
}
#endif

#if NRF_UICR_HAS_PERIPH_CONFIG
NRF_STATIC_INLINE nrf_uicr_periph_config_t nrf_uicr_periph_config_get(NRF_UICR_Type const * p_reg,
                                                                      uint8_t               index)
{
    NRFX_ASSERT(index < NRF_UICR_PERIPH_COUNT);
    nrf_uicr_periph_config_t config;

    config.secattr = (p_reg->PERIPH[index].CONFIG & UICR_PERIPH_CONFIG_SECURE_Msk) >>
                     UICR_PERIPH_CONFIG_SECURE_Pos;

    config.dmasec = (p_reg->PERIPH[index].CONFIG & UICR_PERIPH_CONFIG_DMASEC_Msk) >>
                    UICR_PERIPH_CONFIG_DMASEC_Pos;

    config.processor = (nrf_processor_t)((p_reg->PERIPH[index].CONFIG & UICR_PERIPH_CONFIG_PROCESSOR_Msk)
                                         >> UICR_PERIPH_CONFIG_PROCESSOR_Pos);

    /* Address should not be bit-shifted, as it contains bits [31:12]. The rest should be all zeroes. */
    config.address = (p_reg->PERIPH[index].CONFIG & UICR_PERIPH_CONFIG_ADDRESS_Msk);

    return config;
}
#endif

#if NRF_UICR_HAS_FEATURE_CONFIG
NRF_STATIC_INLINE uint32_t nrf_uicr_feature_own_get(NRF_UICR_Type const * p_reg,
                                                    nrf_uicr_feature_t    feature,
                                                    uint8_t               index)
{
    switch (feature)
    {
#if NRF_UICR_HAS_FEATURE_GPIO
        case NRF_UICR_FEATURE_GPIO:
            NRFX_ASSERT(index < NRF_UICR_GPIO_COUNT);
            return ((NRF_UICREXTENDED_Type *)nrf_uicr_ptrextuicr_get(p_reg))->GPIO[index].OWN;
#endif

        case NRF_UICR_FEATURE_GPIOTE_CH:
            NRFX_ASSERT(index < NRF_UICR_GPIOTE_CH_COUNT);
            return p_reg->GPIOTE[index].CH.OWN;

        case NRF_UICR_FEATURE_IPCT_GLOBAL_CH:
            NRFX_ASSERT(index < NRF_UICR_IPCT_GLOBAL_COUNT);
            return p_reg->IPCT.GLOBAL[index].CH.OWN;

        case NRF_UICR_FEATURE_IPCT_GLOBAL_INTERRUPT:
            NRFX_ASSERT(index < NRF_UICR_IPCT_GLOBAL_COUNT);
            return p_reg->IPCT.GLOBAL[index].INTERRUPT.OWN;

        case NRF_UICR_FEATURE_DPPI_GLOBAL_CH:
            NRFX_ASSERT(index < NRF_UICR_DPPI_GLOBAL_COUNT);
            return p_reg->DPPI.GLOBAL[index].CH.OWN;

        case NRF_UICR_FEATURE_DPPI_GLOBAL_CHG:
            NRFX_ASSERT(index < NRF_UICR_DPPI_GLOBAL_COUNT);
            return p_reg->DPPI.GLOBAL[index].CHG.OWN;

        case NRF_UICR_FEATURE_GRTC_CC:
            return p_reg->GRTC.CC.OWN;

        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE uint32_t nrf_uicr_feature_secure_get(NRF_UICR_Type const * p_reg,
                                                       nrf_uicr_feature_t    feature,
                                                       uint8_t               index)
{
    switch (feature)
    {
#if NRF_UICR_HAS_FEATURE_GPIO
        case NRF_UICR_FEATURE_GPIO:
            NRFX_ASSERT(index < NRF_UICR_GPIO_COUNT);
            return ((NRF_UICREXTENDED_Type *)nrf_uicr_ptrextuicr_get(p_reg))->GPIO[index].SECURE;
#endif

        case NRF_UICR_FEATURE_GPIOTE_CH:
            NRFX_ASSERT(index < NRF_UICR_GPIOTE_CH_COUNT);
            return p_reg->GPIOTE[index].CH.SECURE;

        case NRF_UICR_FEATURE_IPCT_LOCAL_CH:
            return p_reg->IPCT.LOCAL.CH.SECURE;

        case NRF_UICR_FEATURE_IPCT_LOCAL_INTERRUPT:
            return p_reg->IPCT.LOCAL.INTERRUPT.SECURE;

        case NRF_UICR_FEATURE_IPCT_GLOBAL_CH:
            NRFX_ASSERT(index < NRF_UICR_IPCT_GLOBAL_COUNT);
            return p_reg->IPCT.GLOBAL[index].CH.SECURE;

        case NRF_UICR_FEATURE_IPCT_GLOBAL_INTERRUPT:
            NRFX_ASSERT(index < NRF_UICR_IPCT_GLOBAL_COUNT);
            return p_reg->IPCT.GLOBAL[index].INTERRUPT.SECURE;

        case NRF_UICR_FEATURE_DPPI_LOCAL_CH:
            NRFX_ASSERT(index < NRF_UICR_DPPI_LOCAL_COUNT);
            return p_reg->DPPI.LOCAL[index].CH.SECURE;

        case NRF_UICR_FEATURE_DPPI_LOCAL_CHG:
            NRFX_ASSERT(index < NRF_UICR_DPPI_LOCAL_COUNT);
            return p_reg->DPPI.LOCAL[index].CHG.SECURE;

        case NRF_UICR_FEATURE_DPPI_GLOBAL_CH:
            NRFX_ASSERT(index < NRF_UICR_DPPI_GLOBAL_COUNT);
            return p_reg->DPPI.GLOBAL[index].CH.SECURE;

        case NRF_UICR_FEATURE_DPPI_GLOBAL_CHG:
            NRFX_ASSERT(index < NRF_UICR_DPPI_GLOBAL_COUNT);
            return p_reg->DPPI.GLOBAL[index].CHG.SECURE;

        case NRF_UICR_FEATURE_GRTC_CC:
            return p_reg->GRTC.CC.SECURE;

        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE nrf_uicr_dppi_link_t nrf_uicr_feature_link_get(NRF_UICR_Type const * p_reg,
                                                                 nrf_uicr_feature_t    feature,
                                                                 uint8_t               index)
{
    nrf_uicr_dppi_link_t link = {0};

    switch (feature)
    {
        case NRF_UICR_FEATURE_DPPI_LOCAL_CH:
            NRFX_ASSERT(index < NRF_UICR_DPPI_LOCAL_COUNT);
#if NRF_UICR_HAS_CH_LINK_DIR_EN
            link.source = p_reg->DPPI.LOCAL[index].CH.LINK.EN | ~p_reg->DPPI.LOCAL[index].CH.LINK.DIR;
            link.sink = p_reg->DPPI.LOCAL[index].CH.LINK.EN | p_reg->DPPI.LOCAL[index].CH.LINK.DIR;
#else
            link.source = p_reg->DPPI.LOCAL[index].CH.LINK.SOURCE;
            link.sink = p_reg->DPPI.LOCAL[index].CH.LINK.SINK;
#endif
            break;

        case NRF_UICR_FEATURE_DPPI_GLOBAL_CH:
            NRFX_ASSERT(index < NRF_UICR_DPPI_GLOBAL_COUNT);
#if NRF_UICR_HAS_CH_LINK_DIR_EN
            link.source = p_reg->DPPI.GLOBAL[index].CH.LINK.EN | ~p_reg->DPPI.GLOBAL[index].CH.LINK.DIR;
            link.sink = p_reg->DPPI.GLOBAL[index].CH.LINK.EN | p_reg->DPPI.GLOBAL[index].CH.LINK.DIR;
#else
            link.source = p_reg->DPPI.GLOBAL[index].CH.LINK.SOURCE;
            link.sink = p_reg->DPPI.GLOBAL[index].CH.LINK.SINK;
#endif
            break;

        default:
            NRFX_ASSERT(false);
            break;
    }

    return link;
}

NRF_STATIC_INLINE nrf_uicr_ipcmap_config_t nrf_uicr_ipcmap_config_get(NRF_UICR_Type const * p_reg,
                                                                      uint8_t               index)
{
    NRFX_ASSERT(index < NRF_UICR_IPCMAP_COUNT);
    nrf_uicr_ipcmap_config_t map;

    map.source.ipct_channel = (p_reg->IPCMAP[index] & UICR_IPCMAP_IPCTCHSOURCE_Msk)
                              >> UICR_IPCMAP_IPCTCHSOURCE_Pos;

    map.source.domain = (nrf_domain_t)((p_reg->IPCMAP[index] & UICR_IPCMAP_DOMAINIDSOURCE_Msk)
                                       >> UICR_IPCMAP_DOMAINIDSOURCE_Pos);

    map.sink.ipct_channel = (p_reg->IPCMAP[index] & UICR_IPCMAP_IPCTCHSINK_Msk)
                            >> UICR_IPCMAP_IPCTCHSINK_Pos;

    map.sink.domain = (nrf_domain_t)((p_reg->IPCMAP[index] & UICR_IPCMAP_DOMAINIDSINK_Msk)
                                     >> UICR_IPCMAP_DOMAINIDSINK_Pos);

    return map;
}
#endif

#if NRF_UICR_HAS_MAILBOX
NRF_STATIC_INLINE uint32_t nrf_uicr_mailbox_address_get(NRF_UICR_Type const * p_reg, uint8_t index)
{
    NRFX_ASSERT(index < NRF_UICR_MAILBOX_COUNT);

    return p_reg->MAILBOX[index].ADDRESS;
}

NRF_STATIC_INLINE
nrf_uicr_mailbox_config_t nrf_uicr_mailbox_config_get(NRF_UICR_Type const * p_reg,
                                                      uint8_t               index)
{
    NRFX_ASSERT(index < NRF_UICR_MAILBOX_COUNT);
    nrf_uicr_mailbox_config_t config;

    config.size = (p_reg->MAILBOX[index].CONFIG & UICR_MAILBOX_CONFIG_SIZE_Msk)
                  >> UICR_MAILBOX_CONFIG_SIZE_Pos;

    config.owner = (nrf_owner_t)((p_reg->MAILBOX[index].CONFIG & UICR_MAILBOX_CONFIG_OWNERID_Msk)
                                 >> UICR_MAILBOX_CONFIG_OWNERID_Pos);

    config.secure = (p_reg->MAILBOX[index].CONFIG & UICR_MAILBOX_CONFIG_SECURE_Msk)
                    >> UICR_MAILBOX_CONFIG_SECURE_Pos;

    return config;
}
#endif

#if NRF_UICR_HAS_VTOR
NRF_STATIC_INLINE uint32_t nrf_uicr_initsvtor_get(NRF_UICR_Type const * p_reg)
{
    return p_reg->INITSVTOR;
}

NRF_STATIC_INLINE uint32_t nrf_uicr_initnsvtor_get(NRF_UICR_Type const * p_reg)
{
    return p_reg->INITNSVTOR;
}
#endif

#if NRF_UICR_HAS_PTREXT
NRF_STATIC_INLINE uint32_t * nrf_uicr_ptrextuicr_get(NRF_UICR_Type const * p_reg)
{
    return (uint32_t *)p_reg->PTREXTUICR;
}
#endif

#if NRF_UICR_HAS_BOOTCONF

NRF_STATIC_INLINE
void nrf_uicr_boot_region_config_set(NRF_UICR_Type *                       p_reg,
                                     nrf_uicr_boot_region_config_t const * p_config)
{
    p_reg->BOOTCONF = (((p_config->permissions & NRF_UICR_BOOTCONF_PERM_MASK)) |
                       ((p_config->writeonce ? UICR_BOOTCONF_WRITEONCE_Enabled :
                                               UICR_BOOTCONF_WRITEONCE_Disabled)
                        << UICR_BOOTCONF_WRITE_Pos) |
                       ((p_config->lock ? UICR_BOOTCONF_LOCK_Enabled:
                                          UICR_BOOTCONF_LOCK_Disabled) << UICR_BOOTCONF_LOCK_Pos) |
                       ((p_config->size_kb << UICR_BOOTCONF_SIZE_Pos) & UICR_BOOTCONF_SIZE_Msk));
}

NRF_STATIC_INLINE bool nrf_uicr_boot_region_config_get(NRF_UICR_Type const *           p_reg,
                                                       nrf_uicr_boot_region_config_t * p_config)
{
    uint32_t reg = p_reg->BOOTCONF;
    p_config->permissions = reg & NRF_UICR_BOOTCONF_PERM_MASK;
    p_config->writeonce   = ((reg & UICR_BOOTCONF_WRITEONCE_Msk) >> UICR_BOOTCONF_WRITEONCE_Pos) ==
                            UICR_BOOTCONF_WRITEONCE_Enabled;
    p_config->lock        = ((reg & UICR_BOOTCONF_LOCK_Msk) >> UICR_BOOTCONF_LOCK_Pos) ==
                            UICR_BOOTCONF_LOCK_Enabled;
    p_config->size_kb     = (reg & UICR_BOOTCONF_SIZE_Msk) >> UICR_BOOTCONF_SIZE_Pos;
    return (reg != UICR_BOOTCONF_ResetValue);
}
#endif

#if NRF_UICR_HAS_FEATURE_GPIO
NRF_STATIC_INLINE uint32_t nrf_uicr_gpio_instance_get(NRF_UICREXTENDED_Type const * p_reg,
                                                      uint8_t                       index)
{
    NRFX_ASSERT(index < NRF_UICR_GPIO_COUNT);
    return p_reg->GPIO[index].INSTANCE;
}

NRF_STATIC_INLINE uint32_t nrf_uicr_gpio_ctrlsel_get(NRF_UICREXTENDED_Type const * p_reg,
                                                     uint32_t                      pin_number)
{
    uint32_t port = NRF_PIN_NUMBER_TO_PORT(pin_number);
    uint32_t pin  = NRF_PIN_NUMBER_TO_PIN(pin_number);
    NRFX_ASSERT(port < NRF_UICR_GPIO_COUNT);
    return p_reg->GPIO[port].PIN[pin].CTRLSEL;
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_UICR_H_ */
