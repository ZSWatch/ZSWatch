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

#ifndef NRF_SPU_H__
#define NRF_SPU_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_spu_hal SPU HAL
 * @{
 * @ingroup nrf_spu
 * @brief   Hardware access layer for managing the System Protection Unit (SPU) peripheral.
 */

#if defined(SPU_PERIPHACCERR_ADDRESS_ADDRESS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of peripheral access feature. */
#define NRF_SPU_HAS_PERIPHERAL_ACCESS 1
#else
#define NRF_SPU_HAS_PERIPHERAL_ACCESS 0
#endif

#if defined(SPU_PERIPHACCERR_INFO_OWNERID_Msk) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Symbol indicating whether register containing information about the transaction
 *        that caused peripheral access error is present.
 */
#define NRF_SPU_HAS_PERIPHERAL_ACCESS_ERROR_INFO 1
#else
#define NRF_SPU_HAS_PERIPHERAL_ACCESS_ERROR_INFO 0
#endif

#if defined(SPU_PERIPH_PERM_OWNERPROG_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of ownership feature. */
#define NRF_SPU_HAS_OWNERSHIP 1
#else
#define NRF_SPU_HAS_OWNERSHIP 0
#endif

#if defined(SPU_FLASHREGION_PERM_EXECUTE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of memory feature. */
#define NRF_SPU_HAS_MEMORY 1
#else
#define NRF_SPU_HAS_MEMORY 0
#endif

#if defined(SPU_PERIPH_PERM_BLOCK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether block feature is present. */
#define NRF_SPU_HAS_BLOCK 1
#else
#define NRF_SPU_HAS_BLOCK 0
#endif

#if defined(SPU_FEATURE_BELLS_DOMAIN_MaxCount) || defined(SPU_FEATURE_BELLS_PROCESSOR_MaxCount) \
    || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SPU has registers related to BELLS. */
#define NRF_SPU_HAS_BELLS 1
#else
#define NRF_SPU_HAS_BELLS 0
#endif

#if defined(SPU_FEATURE_BELLS_DOMAIN_MaxCount) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SPU uses DOMAIN register name. */
#define NRF_SPU_HAS_DOMAIN 1
#else
#define NRF_SPU_HAS_DOMAIN 0
#endif

#if defined(SPU_FEATURE_IPCT_CH_MaxCount) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SPU has registers related to IPCT. */
#define NRF_SPU_HAS_IPCT 1
#else
#define NRF_SPU_HAS_IPCT 0
#endif

#if defined(SPU_FEATURE_TDD_MaxCount) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SPU has registers related to TDD. */
#define NRF_SPU_HAS_TDD 1
#else
#define NRF_SPU_HAS_TDD 0
#endif

#if defined(SPU_FEATURE_MRAMC_MaxCount) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SPU has registers related to MRAMC. */
#define NRF_SPU_HAS_MRAMC 1
#else
#define NRF_SPU_HAS_MRAMC 0
#endif

#if defined(SPU_FEATURE_GPIO_MaxCount) || defined(SPU_FEATURE_GRTC_CC_MaxCount) || \
    defined(SPU_FEATURE_GPIOTE_MaxCount) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SPU has FEATURE register. */
#define NRF_SPU_HAS_FEATURE 1
#else
#define NRF_SPU_HAS_FEATURE 0
#endif

#if NRF_SPU_HAS_PERIPHERAL_ACCESS

/** @brief Number of peripherals. */
#define NRF_SPU_PERIPH_COUNT                     SPU_PERIPH_MaxCount

#if NRF_SPU_HAS_FEATURE

#if NRF_SPU_HAS_IPCT
/** @brief Number of IPCT channels. */
#define NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT       SPU_FEATURE_IPCT_CH_MaxCount

/** @brief Number of IPCT interrupts. */
#define NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT     SPU_FEATURE_IPCT_INTERRUPT_MaxCount
#endif

/** @brief Number of DPPI channels. */
#define NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT       SPU_FEATURE_DPPIC_CH_MaxCount

/** @brief Number of DPPI channel groups. */
#define NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT SPU_FEATURE_DPPIC_CHG_MaxCount

/** @brief Number of GPIOTEs. */
#define NRF_SPU_FEATURE_GPIOTE_COUNT             SPU_FEATURE_GPIOTE_MaxCount

/** @brief Number of GPIOTE channels. */
#define NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT     SPU_FEATURE_GPIOTE_CH_MaxCount

/** @brief Number of GPIOTE interrupts. */
#define NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT   SPU_FEATURE_GPIOTE_INTERRUPT_MaxCount

/** @brief Number of GPIOs. */
#define NRF_SPU_FEATURE_GPIO_COUNT               SPU_FEATURE_GPIO_MaxCount

/** @brief Number of GPIO pins. */
#define NRF_SPU_FEATURE_GPIO_PIN_COUNT           SPU_FEATURE_GPIO_PIN_MaxCount

/** @brief Number of GRTC compare channels. */
#define NRF_SPU_FEATURE_GRTC_CC_COUNT            SPU_FEATURE_GRTC_CC_MaxCount

/** @brief Number of GRTC interrupts.. */
#define NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT     SPU_FEATURE_GRTC_INTERRUPT_MaxCount

/** @brief Number of BELL domains. */
#if NRF_SPU_HAS_DOMAIN
#define NRF_SPU_FEATURE_BELL_DOMAIN_COUNT        SPU_FEATURE_BELLS_DOMAIN_MaxCount
#else
#define NRF_SPU_FEATURE_BELLS_PROCESSOR_COUNT    SPU_FEATURE_BELLS_PROCESSOR_MaxCount
#endif

/** @brief Number of BELL Domain/Processor features. */
#if NRF_SPU_HAS_DOMAIN
#define NRF_SPU_FEATURE_BELL_BELL_COUNT          SPU_FEATURE_BELLS_DOMAIN_BELL_MaxCount
#else
#define NRF_SPU_FEATURE_BELLS_TASKS_COUNT        SPU_FEATURE_BELLS_PROCESSOR_TASKS_MaxCount
#define NRF_SPU_FEATURE_BELLS_EVENTS_COUNT       SPU_FEATURE_BELLS_PROCESSOR_EVENTS_MaxCount
#define NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT    SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_MaxCount
#endif

#if NRF_SPU_HAS_TDD
/** @brief Number of TDDs. */
#define NRF_SPU_FEATURE_TDD_COUNT                SPU_FEATURE_TDD_MaxCount
#endif

#if NRF_SPU_HAS_MRAMC
/** @brief Number of MRAMCs. */
#define NRF_SPU_FEATURE_MRAMC_COUNT              SPU_FEATURE_MRAMC_MaxCount
#endif

#endif // NRF_SPU_HAS_FEATURE

#endif // NRF_SPU_HAS_PERIPHERAL_ACCESS

/** @brief SPU events. */
typedef enum
{
#if NRF_SPU_HAS_MEMORY
    NRF_SPU_EVENT_RAMACCERR    = offsetof(NRF_SPU_Type, EVENTS_RAMACCERR),    ///< A security violation has been detected for the RAM memory space.
    NRF_SPU_EVENT_FLASHACCERR  = offsetof(NRF_SPU_Type, EVENTS_FLASHACCERR),  ///< A security violation has been detected for the Flash memory space.
#endif
    NRF_SPU_EVENT_PERIPHACCERR = offsetof(NRF_SPU_Type, EVENTS_PERIPHACCERR), ///< A security violation has been detected on one or several peripherals.
} nrf_spu_event_t;

/** @brief SPU interrupts. */
typedef enum
{
#if NRF_SPU_HAS_MEMORY
    NRF_SPU_INT_RAMACCERR_MASK     = SPU_INTENSET_RAMACCERR_Msk,   ///< Interrupt on RAMACCERR event.
    NRF_SPU_INT_FLASHACCERR_MASK   = SPU_INTENSET_FLASHACCERR_Msk, ///< Interrupt on FLASHACCERR event.
#endif
    NRF_SPU_INT_PERIPHACCERR_MASK  = SPU_INTENSET_PERIPHACCERR_Msk ///< Interrupt on PERIPHACCERR event.
} nrf_spu_int_mask_t;

#if NRF_SPU_HAS_MEMORY
/** @brief SPU Non-Secure Callable (NSC) region size. */
typedef enum
{
    NRF_SPU_NSC_SIZE_DISABLED = 0, ///< Not defined as a non-secure callable region.
    NRF_SPU_NSC_SIZE_32B      = 1, ///< Non-Secure Callable region with a 32-byte size
    NRF_SPU_NSC_SIZE_64B      = 2, ///< Non-Secure Callable region with a 64-byte size
    NRF_SPU_NSC_SIZE_128B     = 3, ///< Non-Secure Callable region with a 128-byte size
    NRF_SPU_NSC_SIZE_256B     = 4, ///< Non-Secure Callable region with a 256-byte size
    NRF_SPU_NSC_SIZE_512B     = 5, ///< Non-Secure Callable region with a 512-byte size
    NRF_SPU_NSC_SIZE_1024B    = 6, ///< Non-Secure Callable region with a 1024-byte size
    NRF_SPU_NSC_SIZE_2048B    = 7, ///< Non-Secure Callable region with a 2048-byte size
    NRF_SPU_NSC_SIZE_4096B    = 8  ///< Non-Secure Callable region with a 4096-byte size
} nrf_spu_nsc_size_t;

/** @brief SPU memory region permissions. */
typedef enum
{
    NRF_SPU_MEM_PERM_EXECUTE = SPU_FLASHREGION_PERM_EXECUTE_Msk, ///< Allow code execution from particular memory region.
    NRF_SPU_MEM_PERM_WRITE   = SPU_FLASHREGION_PERM_WRITE_Msk,   ///< Allow write operation on particular memory region.
    NRF_SPU_MEM_PERM_READ    = SPU_FLASHREGION_PERM_READ_Msk     ///< Allow read operation from particular memory region.
} nrf_spu_mem_perm_t;
#endif

#if NRF_SPU_HAS_PERIPHERAL_ACCESS
/** @brief SPU read capabilities for TrustZone Cortex-M secure attribute. */
typedef enum
{
    NRF_SPU_SECUREMAPPING_NONSECURE      = SPU_PERIPH_PERM_SECUREMAPPING_NonSecure,      /**< Peripheral is always accessible as non-secure. */
    NRF_SPU_SECUREMAPPING_SECURE         = SPU_PERIPH_PERM_SECUREMAPPING_Secure,         /**< Peripheral is always accessible as secure. */
    NRF_SPU_SECUREMAPPING_USERSELECTABLE = SPU_PERIPH_PERM_SECUREMAPPING_UserSelectable, /**< Non-secure or secure attribute for this peripheral is defined by the PERIPH[n].PERM register. */
    NRF_SPU_SECUREMAPPING_SPLIT          = SPU_PERIPH_PERM_SECUREMAPPING_Split,          /**< Peripheral implements the split security mechanism. */
} nrf_spu_securemapping_t;

/** @brief SPU DMA capabilities. */
typedef enum
{
    NRF_SPU_DMA_NODMA               = SPU_PERIPH_PERM_DMA_NoDMA,               /**< Peripheral has no DMA capability. */
    NRF_SPU_DMA_NOSEPARATEATTRIBUTE = SPU_PERIPH_PERM_DMA_NoSeparateAttribute, /**< DMA transfers always have the same security attribute as assigned to the peripheral. */
    NRF_SPU_DMA_SEPARATEATTRIBUTE   = SPU_PERIPH_PERM_DMA_SeparateAttribute,   /**< DMA transfers can have a different security attribute than the one assigned to the peripheral. */
} nrf_spu_dma_t;

#if NRF_SPU_HAS_FEATURE
/** @brief SPU features. */
typedef enum
{
#if NRF_SPU_HAS_IPCT
    NRF_SPU_FEATURE_IPCT_CHANNEL,         /**< IPCT channel. */
    NRF_SPU_FEATURE_IPCT_INTERRUPT,       /**< IPCT interrupt. */
#endif
    NRF_SPU_FEATURE_DPPI_CHANNEL,         /**< DPPI channel. */
    NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP,   /**< DPPI channel group. */
    NRF_SPU_FEATURE_GPIOTE_CHANNEL,       /**< GPIOTE channel. */
    NRF_SPU_FEATURE_GPIOTE_INTERRUPT,     /**< GPIOTE interrupt. */
    NRF_SPU_FEATURE_GPIO_PIN,             /**< GPIO pin. */
    NRF_SPU_FEATURE_GRTC_CC,              /**< GRTC compare channel. */
    NRF_SPU_FEATURE_GRTC_SYSCOUNTER,      /**< GRTC SYSCOUNTER. */
    NRF_SPU_FEATURE_GRTC_INTERRUPT,       /**< GRTC interrupt. */
#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
    NRF_SPU_FEATURE_BELLS_BELL,           /**< BELLS bell pair. */
#else
    NRF_SPU_FEATURE_BELLS_TASKS,          /**< BELLS tasks pair. */
    NRF_SPU_FEATURE_BELLS_EVENTS,         /**< BELLS events pair. */
    NRF_SPU_FEATURE_BELLS_INTERRUPT,      /**< BELLS interrupt pair. */
#endif
#endif
#if NRF_SPU_HAS_TDD
    NRF_SPU_FEATURE_TDD,                  /**< TDD. */
#endif
#if NRF_SPU_HAS_MRAMC
    NRF_SPU_FEATURE_MRAMC_WAITSTATES,     /**< MRAMC waitstates. */
    NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN, /**< MRAMC automatic power-down. */
    NRF_SPU_FEATURE_MRAMC_READY           /**< MRAMC ready. */
#endif
} nrf_spu_feature_t;
#endif // NRF_SPU_HAS_FEATURE

#endif

/**
 * @brief Function for clearing a specific SPU event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_spu_event_clear(NRF_SPU_Type *  p_reg,
                                           nrf_spu_event_t event);

/**
 * @brief Function for retrieving the state of the SPU event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_spu_event_check(NRF_SPU_Type const * p_reg,
                                           nrf_spu_event_t      event);

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_spu_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_spu_int_enable(NRF_SPU_Type * p_reg,
                                          uint32_t       mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_spu_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_spu_int_disable(NRF_SPU_Type * p_reg,
                                           uint32_t       mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_spu_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_spu_int_enable_check(NRF_SPU_Type const * p_reg, uint32_t mask);

#if NRF_SPU_HAS_MEMORY
/**
 * @brief Function for setting up publication configuration of a given SPU event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event to configure.
 * @param[in] channel Channel to connect with published event.
 */
NRF_STATIC_INLINE void nrf_spu_publish_set(NRF_SPU_Type *  p_reg,
                                           nrf_spu_event_t event,
                                           uint32_t        channel);

/**
 * @brief Function for clearing publication configuration of a given SPU event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_spu_publish_clear(NRF_SPU_Type *  p_reg,
                                             nrf_spu_event_t event);

/**
 * @brief Function for retrieving the capabilities of the current device.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  ARM TrustZone support is available.
 * @retval false ARM TrustZone support is not available.
 */
NRF_STATIC_INLINE bool nrf_spu_tz_is_available(NRF_SPU_Type const * p_reg);

/**
 * @brief Function for configuring the DPPI channels to be available in particular domains.
 *
 * Channels are configured as bitmask. Set one in bitmask to make channels available only in secure
 * domain. Set zero to make it available in secure and non-secure domains.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] dppi_id       DPPI peripheral id.
 * @param[in] channels_mask Bitmask with channels configuration.
 * @param[in] lock_conf     Lock configuration until next SoC reset.
 */
NRF_STATIC_INLINE void nrf_spu_dppi_config_set(NRF_SPU_Type * p_reg,
                                               uint8_t        dppi_id,
                                               uint32_t       channels_mask,
                                               bool           lock_conf);

/**
 * @brief Function for configuring the GPIO pins to be available in particular domains.
 *
 * GPIO pins are configured as bitmask. Set one in bitmask to make particular pin available only
 * in secure domain. Set zero to make it available in secure and non-secure domains.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] gpio_port Port number.
 * @param[in] gpio_mask Bitmask with gpio configuration.
 * @param[in] lock_conf Lock configuration until next SoC reset.
 */
NRF_STATIC_INLINE void nrf_spu_gpio_config_set(NRF_SPU_Type * p_reg,
                                               uint8_t        gpio_port,
                                               uint32_t       gpio_mask,
                                               bool           lock_conf);

/**
 * @brief Function for configuring non-secure callable flash region.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] flash_nsc_id   Non-secure callable flash region ID.
 * @param[in] flash_nsc_size Non-secure callable flash region size.
 * @param[in] region_number  Flash region number.
 * @param[in] lock_conf      Lock configuration until next SoC reset.
 */
NRF_STATIC_INLINE void nrf_spu_flashnsc_set(NRF_SPU_Type *     p_reg,
                                            uint8_t            flash_nsc_id,
                                            nrf_spu_nsc_size_t flash_nsc_size,
                                            uint8_t            region_number,
                                            bool               lock_conf);

/**
 * @brief Function for configuring non-secure callable RAM region.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] ram_nsc_id    Non-secure callable RAM region ID.
 * @param[in] ram_nsc_size  Non-secure callable RAM region size.
 * @param[in] region_number RAM region number.
 * @param[in] lock_conf     Lock configuration until next SoC reset.
 */
NRF_STATIC_INLINE void nrf_spu_ramnsc_set(NRF_SPU_Type *     p_reg,
                                          uint8_t            ram_nsc_id,
                                          nrf_spu_nsc_size_t ram_nsc_size,
                                          uint8_t            region_number,
                                          bool               lock_conf);

/**
 * @brief Function for configuring security for a particular flash region.
 *
 * Permissions parameter must be set by using the logical OR on the @ref nrf_spu_mem_perm_t values.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] region_id   Flash region index.
 * @param[in] secure_attr Set region attribute to secure.
 * @param[in] permissions Flash region permissions.
 * @param[in] lock_conf   Lock configuration until next SoC reset.
 */
NRF_STATIC_INLINE void nrf_spu_flashregion_set(NRF_SPU_Type * p_reg,
                                               uint8_t        region_id,
                                               bool           secure_attr,
                                               uint32_t       permissions,
                                               bool           lock_conf);

/**
 * @brief Function for configuring security for the RAM region.
 *
 * Permissions parameter must be set by using the logical OR on the @ref nrf_spu_mem_perm_t values.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] region_id   RAM region index.
 * @param[in] secure_attr Set region attribute to secure.
 * @param[in] permissions RAM region permissions.
 * @param[in] lock_conf   Lock configuration until next SoC reset.
 */
NRF_STATIC_INLINE void nrf_spu_ramregion_set(NRF_SPU_Type * p_reg,
                                             uint8_t        region_id,
                                             bool           secure_attr,
                                             uint32_t       permissions,
                                             bool           lock_conf);

/**
 * @brief Function for configuring access permissions of the peripheral.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] peripheral_id ID number of a particular peripheral.
 * @param[in] secure_attr   Peripheral registers accessible only from secure domain.
 * @param[in] secure_dma    DMA transfers possible only from RAM memory in secure domain.
 * @param[in] lock_conf     Lock configuration until next SoC reset.
 */
NRF_STATIC_INLINE void nrf_spu_peripheral_set(NRF_SPU_Type * p_reg,
                                              uint32_t       peripheral_id,
                                              bool           secure_attr,
                                              bool           secure_dma,
                                              bool           lock_conf);

/**
 * @brief Function for configuring bus access permissions of the specified external domain.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] domain_id   ID number of a particular external domain.
 * @param[in] secure_attr Specifies if the bus accesses from this domain have the secure attribute set.
 * @param[in] lock_conf   Specifies if the configuration should be locked until next SoC reset.
 */
NRF_STATIC_INLINE void nrf_spu_extdomain_set(NRF_SPU_Type * p_reg,
                                             uint32_t       domain_id,
                                             bool           secure_attr,
                                             bool           lock_conf);
#endif

#if NRF_SPU_HAS_PERIPHERAL_ACCESS

/**
 * @brief Function for getting the address of the security violation.
 *
 * @note The event PERIPHACCERR must be cleared to clear this register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Address of the transaction that caused first error.
 */
NRF_STATIC_INLINE uint32_t nrf_spu_periphaccerr_address_get(NRF_SPU_Type const * p_reg);

#if NRF_SPU_HAS_OWNERSHIP && NRF_SPU_HAS_PERIPHERAL_ACCESS_ERROR_INFO
/**
 * @brief Function for getting the owner ID of the security violation.
 *
 * @note The event PERIPHACCERR must be cleared to clear this register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Owner ID of the transaction that caused first error.
 */
NRF_STATIC_INLINE nrf_owner_t nrf_spu_periphaccerr_ownerid_get(NRF_SPU_Type const * p_reg);
#endif

/**
 * @brief Function for getting the capabilities for TrustZone Cortex-M secure attribute
 *        of the specified slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @return TrustZone capabilities.
 */
NRF_STATIC_INLINE
nrf_spu_securemapping_t nrf_spu_periph_perm_securemapping_get(NRF_SPU_Type const * p_reg,
                                                              uint8_t              index);

/**
 * @brief Function for getting the DMA capabilities of the specified slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @return DMA capabilities.
 */
NRF_STATIC_INLINE nrf_spu_dma_t nrf_spu_periph_perm_dma_get(NRF_SPU_Type const * p_reg,
                                                            uint8_t              index);

/**
 * @brief Function for getting the security mapping of the specified slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @retval true  Peripheral is mapped in secure peripheral address space.
 * @retval false If TrustZone capabilities are @ref NRF_SPU_SECUREMAPPING_USERSELECTABLE,
 *               then peripheral is mapped in non-secure peripheral address space.
 *               If TrustZone capabilities are @ref NRF_SPU_SECUREMAPPING_SPLIT,
 *               then peripheral is mapped in non-secure and secure peripheral address space.
 */
NRF_STATIC_INLINE bool nrf_spu_periph_perm_secattr_get(NRF_SPU_Type const * p_reg,
                                                       uint8_t              index);

/**
 * @brief Function for setting the security mapping of the specified slave.
 *
 * @note This bit has effect only if TrustZone capabilities are either
 *       @ref NRF_SPU_SECUREMAPPING_USERSELECTABLE or @ref NRF_SPU_SECUREMAPPING_SPLIT.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Peripheral slave index.
 * @param[in] enable True if security mapping is to be set, false otherwise.
 */
NRF_STATIC_INLINE void nrf_spu_periph_perm_secattr_set(NRF_SPU_Type * p_reg,
                                                       uint8_t        index,
                                                       bool           enable);

/**
 * @brief Function for getting the security attribution for the DMA transfer
 *        of the specified slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @return True if DMA transfers initiated by this peripheral have the secure attribute set,
 *         false otherwise.
 */
NRF_STATIC_INLINE bool nrf_spu_periph_perm_dmasec_get(NRF_SPU_Type const * p_reg,
                                                      uint8_t              index);

/**
 * @brief Function for setting the security attribution for the DMA transfer
 *        of the specified slave.
 *
 * @note This bit has effect only if peripheral security mapping is enabled.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Peripheral slave index.
 * @param[in] enable True if secure attribute for the DMA transfer is to be set, false otherwise.
 */
NRF_STATIC_INLINE void nrf_spu_periph_perm_dmasec_set(NRF_SPU_Type * p_reg,
                                                      uint8_t        index,
                                                      bool           enable);

#if NRF_SPU_HAS_BLOCK
/**
 * @brief Function for getting the status of the peripheral access lock of the specified slave.
 *
 * @note When peripheral access lock is enabled, reading or modifying the registers of the peripheral
 *       is blocked.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @return True if the peripheral access is locked, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_spu_periph_perm_block_get(NRF_SPU_Type const * p_reg,
                                                     uint8_t              index);

/**
 * @brief Function for enabling the peripheral access lock of the specified slave.
 *
 * @note When peripheral access lock is enabled, reading or modifying the registers of the peripheral
 *       is blocked.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Peripheral slave index.
 */
NRF_STATIC_INLINE void nrf_spu_periph_perm_block_enable(NRF_SPU_Type * p_reg,
                                                        uint8_t        index);
#endif

/**
 * @brief Function for getting the status of the peripheral management lock of the specified slave.
 *
 * @note When peripheral management lock is enabled, modifying the SPU configuration associated
 *       with specified peripheral is not possible.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @return True if the peripheral management is locked, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_spu_periph_perm_lock_get(NRF_SPU_Type const * p_reg,
                                                    uint8_t              index);

/**
 * @brief Function for enabling the peripheral management lock of the specified slave.
 *
 * @note When peripheral management lock is enabled, modifying the SPU configuration associated
 *       with specified peripheral is not possible.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Peripheral slave index.
 */
NRF_STATIC_INLINE void nrf_spu_periph_perm_lock_enable(NRF_SPU_Type * p_reg,
                                                       uint8_t        index);

#if NRF_SPU_HAS_OWNERSHIP
/**
 * @brief Function for getting the peripheral owner ID of the specified slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @return Owner ID.
 */
NRF_STATIC_INLINE nrf_owner_t nrf_spu_periph_perm_ownerid_get(NRF_SPU_Type const * p_reg,
                                                              uint8_t              index);

/**
 * @brief Function for setting the peripheral owner ID of the specified slave.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] index    Peripheral slave index.
 * @param[in] owner_id Owner ID to be set.
 */
NRF_STATIC_INLINE void nrf_spu_periph_perm_ownerid_set(NRF_SPU_Type * p_reg,
                                                       uint8_t        index,
                                                       nrf_owner_t    owner_id);

/**
 * @brief Function for getting the indication if owner ID of the specified slave
 *        is programmable or not.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @return True if owner ID is programmable, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_spu_periph_perm_ownerprog_get(NRF_SPU_Type const * p_reg,
                                                         uint8_t              index);
#endif // #if NRF_SPU_HAS_OWNERSHIP

/**
 * @brief Function for getting the indication if peripheral with
 *        the specified slave index is present.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Peripheral slave index.
 *
 * @return True if peripheral is present, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_spu_periph_perm_present_get(NRF_SPU_Type const * p_reg,
                                                       uint8_t              index);

#if NRF_SPU_HAS_FEATURE
/**
 * @brief Function for getting the security mapping of the specified feature.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] feature  Feature to be accessed.
 * @param[in] index    Feature index.
 * @param[in] subindex Feature subindex. Only used for applicable features, otherwise skipped.
 *
 * @retval true  Feature is available for secure usage.
 * @retval false Feature is available for non-secure usage.
 */
NRF_STATIC_INLINE bool nrf_spu_feature_secattr_get(NRF_SPU_Type const * p_reg,
                                                   nrf_spu_feature_t    feature,
                                                   uint8_t              index,
                                                   uint8_t              subindex);

/**
 * @brief Function for setting the security mapping of the specified feature.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] feature  Feature to be accessed.
 * @param[in] index    Feature index.
 * @param[in] subindex Feature subindex. Only used for applicable features, otherwise skipped.
 * @param[in] enable   True if security mapping is to be set, false otherwise.
 */
NRF_STATIC_INLINE void nrf_spu_feature_secattr_set(NRF_SPU_Type *    p_reg,
                                                   nrf_spu_feature_t feature,
                                                   uint8_t           index,
                                                   uint8_t           subindex,
                                                   bool              enable);

/**
 * @brief Function for getting the status of the management lock of the specified feature.
 *
 * @note When feature management lock is enabled, modifying the SPU configuration associated
 *       with specified feature is not possible.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] feature  Feature to be accessed.
 * @param[in] index    Feature index.
 * @param[in] subindex Feature subindex. Only used for applicable features, otherwise skipped.
 *
 * @return True if feature management is locked, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_spu_feature_lock_get(NRF_SPU_Type const * p_reg,
                                                nrf_spu_feature_t    feature,
                                                uint8_t              index,
                                                uint8_t              subindex);

/**
 * @brief Function for enabling the management lock of the specified feature.
 *
 * @note When feature management lock is enabled, modifying the SPU configuration associated
 *       with specified feature is not possible.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] feature  Feature to be accessed.
 * @param[in] index    Feature index.
 * @param[in] subindex Feature subindex. Only used for applicable features, otherwise skipped.
 */
NRF_STATIC_INLINE void nrf_spu_feature_lock_enable(NRF_SPU_Type *    p_reg,
                                                   nrf_spu_feature_t feature,
                                                   uint8_t           index,
                                                   uint8_t           subindex);

#if NRF_SPU_HAS_BLOCK
/**
 * @brief Function for getting status of the access lock of the specified feature.
 *
 * @note When feature access lock is enabled, reading or modifying the registers of the feature
 *       is blocked.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] feature  Feature to be accessed.
 * @param[in] index    Feature index.
 * @param[in] subindex Feature subindex. Only used for applicable features, otherwise skipped.
 *
 * @return True if the feature access is locked, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_spu_feature_block_get(NRF_SPU_Type const * p_reg,
                                                 nrf_spu_feature_t    feature,
                                                 uint8_t              index,
                                                 uint8_t              subindex);

/**
 * @brief Function for enabling the feature block of the specified feature.
 *
 * @note When feature access lock is enabled, reading or modifying the registers of the feature
 *       is blocked.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] feature  Feature to be accessed.
 * @param[in] index    Feature index.
 * @param[in] subindex Feature subindex. Only used for applicable features, otherwise skipped.
 */
NRF_STATIC_INLINE void nrf_spu_feature_block_enable(NRF_SPU_Type *    p_reg,
                                                    nrf_spu_feature_t feature,
                                                    uint8_t           index,
                                                    uint8_t           subindex);
#endif

#if NRF_SPU_HAS_OWNERSHIP
/**
 * @brief Function for getting the feature owner ID of the specified feature.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] feature  Feature to be accessed.
 * @param[in] index    Feature index.
 * @param[in] subindex Feature subindex. Only used for applicable features, otherwise skipped.
 *
 * @return Owner ID.
 */
NRF_STATIC_INLINE nrf_owner_t nrf_spu_feature_ownerid_get(NRF_SPU_Type const * p_reg,
                                                          nrf_spu_feature_t    feature,
                                                          uint8_t              index,
                                                          uint8_t              subindex);

/**
 * @brief Function for setting the feature owner ID of the specified feature.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] feature  Feature to be accessed.
 * @param[in] index    Feature index.
 * @param[in] subindex Feature subindex. Only used for applicable features, otherwise skipped.
 * @param[in] owner_id Owner ID to be set.
 */
NRF_STATIC_INLINE void nrf_spu_feature_ownerid_set(NRF_SPU_Type *    p_reg,
                                                   nrf_spu_feature_t feature,
                                                   uint8_t           index,
                                                   uint8_t           subindex,
                                                   nrf_owner_t       owner_id);
#endif // NRF_SPU_HAS_OWNERSHIP
#endif // NRF_SPU_HAS_FEATURE
#endif // NRF_SPU_HAS_PERIPHERAL_ACCESS

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_spu_event_clear(NRF_SPU_Type *  p_reg,
                                           nrf_spu_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_spu_event_check(NRF_SPU_Type const * p_reg,
                                           nrf_spu_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_spu_int_enable(NRF_SPU_Type * p_reg,
                                          uint32_t       mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_spu_int_disable(NRF_SPU_Type * p_reg,
                                           uint32_t       mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_spu_int_enable_check(NRF_SPU_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

#if NRF_SPU_HAS_MEMORY
NRF_STATIC_INLINE void nrf_spu_publish_set(NRF_SPU_Type *  p_reg,
                                           nrf_spu_event_t event,
                                           uint32_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
        (channel | (NRF_SUBSCRIBE_PUBLISH_ENABLE));
}

NRF_STATIC_INLINE void nrf_spu_publish_clear(NRF_SPU_Type *  p_reg,
                                             nrf_spu_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE bool nrf_spu_tz_is_available(NRF_SPU_Type const * p_reg)
{
    return (p_reg->CAP & SPU_CAP_TZM_Msk ? true : false);
}

NRF_STATIC_INLINE void nrf_spu_dppi_config_set(NRF_SPU_Type * p_reg,
                                               uint8_t        dppi_id,
                                               uint32_t       channels_mask,
                                               bool           lock_conf)
{
    NRFX_ASSERT(!(p_reg->DPPI[dppi_id].LOCK & SPU_DPPI_LOCK_LOCK_Msk));

    p_reg->DPPI[dppi_id].PERM = channels_mask;

    if (lock_conf)
    {
        p_reg->DPPI[dppi_id].LOCK = (SPU_DPPI_LOCK_LOCK_Msk);
    }
}

NRF_STATIC_INLINE void nrf_spu_gpio_config_set(NRF_SPU_Type * p_reg,
                                               uint8_t        gpio_port,
                                               uint32_t       gpio_mask,
                                               bool           lock_conf)
{
    NRFX_ASSERT(!(p_reg->GPIOPORT[gpio_port].LOCK & SPU_GPIOPORT_LOCK_LOCK_Msk));

    p_reg->GPIOPORT[gpio_port].PERM = gpio_mask;

    if (lock_conf)
    {
        p_reg->GPIOPORT[gpio_port].LOCK = (SPU_GPIOPORT_LOCK_LOCK_Msk);
    }
}

NRF_STATIC_INLINE void nrf_spu_flashnsc_set(NRF_SPU_Type *     p_reg,
                                            uint8_t            flash_nsc_id,
                                            nrf_spu_nsc_size_t flash_nsc_size,
                                            uint8_t            region_number,
                                            bool               lock_conf)
{
    NRFX_ASSERT(!(p_reg->FLASHNSC[flash_nsc_id].REGION & SPU_FLASHNSC_REGION_LOCK_Msk));
    NRFX_ASSERT(!(p_reg->FLASHNSC[flash_nsc_id].SIZE & SPU_FLASHNSC_SIZE_LOCK_Msk));

    p_reg->FLASHNSC[flash_nsc_id].REGION = (uint32_t)region_number |
        (lock_conf ? SPU_FLASHNSC_REGION_LOCK_Msk : 0);
    p_reg->FLASHNSC[flash_nsc_id].SIZE = (uint32_t)flash_nsc_size |
        (lock_conf ? SPU_FLASHNSC_SIZE_LOCK_Msk : 0);
}

NRF_STATIC_INLINE void nrf_spu_ramnsc_set(NRF_SPU_Type *     p_reg,
                                          uint8_t            ram_nsc_id,
                                          nrf_spu_nsc_size_t ram_nsc_size,
                                          uint8_t            region_number,
                                          bool               lock_conf)
{
    NRFX_ASSERT(!(p_reg->RAMNSC[ram_nsc_id].REGION & SPU_RAMNSC_REGION_LOCK_Msk));
    NRFX_ASSERT(!(p_reg->RAMNSC[ram_nsc_id].SIZE & SPU_RAMNSC_SIZE_LOCK_Msk));

    p_reg->RAMNSC[ram_nsc_id].REGION = (uint32_t)region_number |
        (lock_conf ? SPU_RAMNSC_REGION_LOCK_Msk : 0);
    p_reg->RAMNSC[ram_nsc_id].SIZE = (uint32_t)ram_nsc_size |
        (lock_conf ? SPU_RAMNSC_SIZE_LOCK_Msk : 0);
}

NRF_STATIC_INLINE void nrf_spu_flashregion_set(NRF_SPU_Type * p_reg,
                                               uint8_t        region_id,
                                               bool           secure_attr,
                                               uint32_t       permissions,
                                               bool           lock_conf)
{
    NRFX_ASSERT(!(p_reg->FLASHREGION[region_id].PERM & SPU_FLASHREGION_PERM_LOCK_Msk));

    p_reg->FLASHREGION[region_id].PERM = permissions         |
        (secure_attr ? SPU_FLASHREGION_PERM_SECATTR_Msk : 0) |
        (lock_conf   ? SPU_FLASHREGION_PERM_LOCK_Msk    : 0);
}

NRF_STATIC_INLINE void nrf_spu_ramregion_set(NRF_SPU_Type * p_reg,
                                             uint8_t        region_id,
                                             bool           secure_attr,
                                             uint32_t       permissions,
                                             bool           lock_conf)
{
    NRFX_ASSERT(!(p_reg->RAMREGION[region_id].PERM & SPU_RAMREGION_PERM_LOCK_Msk));

    p_reg->RAMREGION[region_id].PERM = permissions         |
        (secure_attr ? SPU_RAMREGION_PERM_SECATTR_Msk : 0) |
        (lock_conf   ? SPU_RAMREGION_PERM_LOCK_Msk    : 0);
}

NRF_STATIC_INLINE void nrf_spu_peripheral_set(NRF_SPU_Type * p_reg,
                                              uint32_t       peripheral_id,
                                              bool           secure_attr,
                                              bool           secure_dma,
                                              bool           lock_conf)
{
    NRFX_ASSERT(p_reg->PERIPHID[peripheral_id].PERM & SPU_PERIPHID_PERM_PRESENT_Msk);
    NRFX_ASSERT(!(p_reg->PERIPHID[peripheral_id].PERM & SPU_PERIPHID_PERM_LOCK_Msk));

    p_reg->PERIPHID[peripheral_id].PERM =
         (secure_attr ? SPU_PERIPHID_PERM_SECATTR_Msk : 0) |
         (secure_dma  ? SPU_PERIPHID_PERM_DMASEC_Msk  : 0) |
         (lock_conf   ? SPU_PERIPHID_PERM_LOCK_Msk    : 0);
}

NRF_STATIC_INLINE void nrf_spu_extdomain_set(NRF_SPU_Type * p_reg,
                                             uint32_t       domain_id,
                                             bool           secure_attr,
                                             bool           lock_conf)
{
    NRFX_ASSERT(!(p_reg->EXTDOMAIN[domain_id].PERM & SPU_EXTDOMAIN_PERM_LOCK_Msk));

    p_reg->EXTDOMAIN[domain_id].PERM =
        (secure_attr ? SPU_EXTDOMAIN_PERM_SECATTR_Msk : 0) |
        (lock_conf   ? SPU_EXTDOMAIN_PERM_LOCK_Msk    : 0);
}
#endif

#if NRF_SPU_HAS_PERIPHERAL_ACCESS
NRF_STATIC_INLINE uint32_t nrf_spu_periphaccerr_address_get(NRF_SPU_Type const * p_reg)
{
    return p_reg->PERIPHACCERR.ADDRESS;
}

#if NRF_SPU_HAS_OWNERSHIP && NRF_SPU_HAS_PERIPHERAL_ACCESS_ERROR_INFO
NRF_STATIC_INLINE nrf_owner_t nrf_spu_periphaccerr_ownerid_get(NRF_SPU_Type const * p_reg)
{
    return (nrf_owner_t)p_reg->PERIPHACCERR.INFO;
}
#endif

NRF_STATIC_INLINE bool nrf_spu_periph_perm_present_get(NRF_SPU_Type const * p_reg,
                                                       uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    return (p_reg->PERIPH[index].PERM & SPU_PERIPH_PERM_PRESENT_Msk) >>
           SPU_PERIPH_PERM_PRESENT_Pos;
}

#if NRF_SPU_HAS_OWNERSHIP
NRF_STATIC_INLINE bool nrf_spu_periph_perm_ownerprog_get(NRF_SPU_Type const * p_reg,
                                                         uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    return (p_reg->PERIPH[index].PERM & SPU_PERIPH_PERM_OWNERPROG_Msk) >>
           SPU_PERIPH_PERM_OWNERPROG_Pos;
}
#endif

NRF_STATIC_INLINE bool nrf_spu_periph_perm_lock_get(NRF_SPU_Type const * p_reg,
                                                    uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    return (p_reg->PERIPH[index].PERM & SPU_PERIPH_PERM_LOCK_Msk) >>
           SPU_PERIPH_PERM_LOCK_Pos;
}

#if NRF_SPU_HAS_BLOCK
NRF_STATIC_INLINE bool nrf_spu_periph_perm_block_get(NRF_SPU_Type const * p_reg,
                                                     uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    return (p_reg->PERIPH[index].PERM & SPU_PERIPH_PERM_BLOCK_Msk) >>
           SPU_PERIPH_PERM_BLOCK_Pos;
}
#endif

NRF_STATIC_INLINE bool nrf_spu_periph_perm_dmasec_get(NRF_SPU_Type const * p_reg,
                                                      uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    return (p_reg->PERIPH[index].PERM & SPU_PERIPH_PERM_DMASEC_Msk) >>
           SPU_PERIPH_PERM_DMASEC_Pos;
}

NRF_STATIC_INLINE bool nrf_spu_periph_perm_secattr_get(NRF_SPU_Type const * p_reg,
                                                       uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    return (p_reg->PERIPH[index].PERM & SPU_PERIPH_PERM_SECATTR_Msk) >>
           SPU_PERIPH_PERM_SECATTR_Pos;
}

NRF_STATIC_INLINE void nrf_spu_periph_perm_lock_enable(NRF_SPU_Type * p_reg,
                                                       uint8_t        index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    p_reg->PERIPH[index].PERM = ((p_reg->PERIPH[index].PERM & ~SPU_PERIPH_PERM_LOCK_Msk)
                                 | (SPU_PERIPH_PERM_LOCK_Locked <<
                                    SPU_PERIPH_PERM_LOCK_Pos));
}

#if NRF_SPU_HAS_BLOCK
NRF_STATIC_INLINE void nrf_spu_periph_perm_block_enable(NRF_SPU_Type * p_reg,
                                                        uint8_t        index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    p_reg->PERIPH[index].PERM = ((p_reg->PERIPH[index].PERM & ~SPU_PERIPH_PERM_BLOCK_Msk)
                                 | (SPU_PERIPH_PERM_BLOCK_Blocked <<
                                    SPU_PERIPH_PERM_BLOCK_Pos));
}
#endif

NRF_STATIC_INLINE void nrf_spu_periph_perm_dmasec_set(NRF_SPU_Type * p_reg,
                                                      uint8_t        index,
                                                      bool           enable)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    p_reg->PERIPH[index].PERM = ((p_reg->PERIPH[index].PERM & ~SPU_PERIPH_PERM_DMASEC_Msk)
                                 | ((enable ? SPU_PERIPH_PERM_DMASEC_Secure :
                                     SPU_PERIPH_PERM_DMASEC_NonSecure) <<
                                    SPU_PERIPH_PERM_DMASEC_Pos));
}

NRF_STATIC_INLINE void nrf_spu_periph_perm_secattr_set(NRF_SPU_Type * p_reg,
                                                       uint8_t        index,
                                                       bool           enable)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);
    p_reg->PERIPH[index].PERM = ((p_reg->PERIPH[index].PERM & ~SPU_PERIPH_PERM_SECATTR_Msk)
                                 | ((enable ? SPU_PERIPH_PERM_SECATTR_Secure :
                                     SPU_PERIPH_PERM_SECATTR_NonSecure) <<
                                    SPU_PERIPH_PERM_SECATTR_Pos));
}

NRF_STATIC_INLINE
nrf_spu_securemapping_t nrf_spu_periph_perm_securemapping_get(NRF_SPU_Type const * p_reg,
                                                              uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);

    return (nrf_spu_securemapping_t)((p_reg->PERIPH[index].PERM
                                      & SPU_PERIPH_PERM_SECUREMAPPING_Msk) >>
                                     SPU_PERIPH_PERM_SECUREMAPPING_Pos);
}

NRF_STATIC_INLINE nrf_spu_dma_t nrf_spu_periph_perm_dma_get(NRF_SPU_Type const * p_reg,
                                                            uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);

    return (nrf_spu_dma_t)((p_reg->PERIPH[index].PERM & SPU_PERIPH_PERM_DMA_Msk) >>
                           SPU_PERIPH_PERM_DMA_Pos);
}

#if NRF_SPU_HAS_OWNERSHIP
NRF_STATIC_INLINE nrf_owner_t nrf_spu_periph_perm_ownerid_get(NRF_SPU_Type const * p_reg,
                                                              uint8_t              index)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);

    return (nrf_owner_t)((p_reg->PERIPH[index].PERM & SPU_PERIPH_PERM_OWNERID_Msk) >>
                         SPU_PERIPH_PERM_OWNERID_Pos);
}

NRF_STATIC_INLINE void nrf_spu_periph_perm_ownerid_set(NRF_SPU_Type * p_reg,
                                                       uint8_t        index,
                                                       nrf_owner_t    owner_id)
{
    NRFX_ASSERT(index < NRF_SPU_PERIPH_COUNT);

    p_reg->PERIPH[index].PERM = ((p_reg->PERIPH[index].PERM & ~SPU_PERIPH_PERM_OWNERID_Msk) |
                                 ((owner_id << SPU_PERIPH_PERM_OWNERID_Pos)
                                  & SPU_PERIPH_PERM_OWNERID_Msk));
}
#endif

#if NRF_SPU_HAS_FEATURE
NRF_STATIC_INLINE bool nrf_spu_feature_secattr_get(NRF_SPU_Type const * p_reg,
                                                   nrf_spu_feature_t    feature,
                                                   uint8_t              index,
                                                   uint8_t              subindex)
{
    switch (feature)
    {
#if NRF_SPU_HAS_IPCT
        case NRF_SPU_FEATURE_IPCT_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT);
            return (p_reg->FEATURE.IPCT.CH[index]
                    & SPU_FEATURE_IPCT_CH_SECATTR_Msk)
                   >> SPU_FEATURE_IPCT_CH_SECATTR_Pos;

        case NRF_SPU_FEATURE_IPCT_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT);
            return (p_reg->FEATURE.IPCT.INTERRUPT[index]
                    & SPU_FEATURE_IPCT_INTERRUPT_SECATTR_Msk)
                   >> SPU_FEATURE_IPCT_INTERRUPT_SECATTR_Pos;
#endif // NRF_SPU_HAS_IPCT

        case NRF_SPU_FEATURE_DPPI_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT);
            return (p_reg->FEATURE.DPPIC.CH[index]
                    & SPU_FEATURE_DPPIC_CH_SECATTR_Msk)
                   >> SPU_FEATURE_DPPIC_CH_SECATTR_Pos;

        case NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT);
            return (p_reg->FEATURE.DPPIC.CHG[index]
                    & SPU_FEATURE_DPPIC_CHG_SECATTR_Msk)
                   >> SPU_FEATURE_DPPIC_CHG_SECATTR_Pos;

        case NRF_SPU_FEATURE_GPIOTE_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT);
            return (p_reg->FEATURE.GPIOTE[index].CH[subindex]
                    & SPU_FEATURE_GPIOTE_CH_SECATTR_Msk)
                   >> SPU_FEATURE_GPIOTE_CH_SECATTR_Pos;

        case NRF_SPU_FEATURE_GPIOTE_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT);
            return (p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex]
                    & SPU_FEATURE_GPIOTE_INTERRUPT_SECATTR_Msk)
                   >> SPU_FEATURE_GPIOTE_INTERRUPT_SECATTR_Pos;

        case NRF_SPU_FEATURE_GPIO_PIN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIO_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIO_PIN_COUNT);
            return (p_reg->FEATURE.GPIO[index].PIN[subindex]
                    & SPU_FEATURE_GPIO_PIN_SECATTR_Msk)
                   >> SPU_FEATURE_GPIO_PIN_SECATTR_Pos;

        case NRF_SPU_FEATURE_GRTC_CC:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_CC_COUNT);
            return (p_reg->FEATURE.GRTC.CC[index]
                    & SPU_FEATURE_GRTC_CC_SECATTR_Msk)
                   >> SPU_FEATURE_GRTC_CC_SECATTR_Pos;

        case NRF_SPU_FEATURE_GRTC_SYSCOUNTER:
            return (p_reg->FEATURE.GRTC.SYSCOUNTER
                    & SPU_FEATURE_GRTC_SYSCOUNTER_SECATTR_Msk)
                   >> SPU_FEATURE_GRTC_SYSCOUNTER_SECATTR_Pos;

        case NRF_SPU_FEATURE_GRTC_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT);
            return (p_reg->FEATURE.GRTC.INTERRUPT[index]
                    & SPU_FEATURE_GRTC_INTERRUPT_SECATTR_Msk)
                   >> SPU_FEATURE_GRTC_INTERRUPT_SECATTR_Pos;

#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
        case NRF_SPU_FEATURE_BELLS_BELL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELL_BELL_COUNT);
            return (p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex]
                    & SPU_FEATURE_BELLS_DOMAIN_BELL_SECATTR_Msk)
                   >> SPU_FEATURE_BELLS_DOMAIN_BELL_SECATTR_Pos;
#else
        case NRF_SPU_FEATURE_BELLS_TASKS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_TASKS_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_TASKS_SECATTR_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_TASKS_SECATTR_Pos;

        case NRF_SPU_FEATURE_BELLS_EVENTS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_EVENTS_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_EVENTS_SECATTR_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_EVENTS_SECATTR_Pos;

        case NRF_SPU_FEATURE_BELLS_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_SECATTR_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_SECATTR_Pos;
#endif // NRF_SPU_HAS_DOMAIN
#endif // NRF_SPU_HAS_BELLS

#if NRF_SPU_HAS_TDD
        case NRF_SPU_FEATURE_TDD:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_TDD_COUNT);
            return (p_reg->FEATURE.TDD[index]
                    & SPU_FEATURE_TDD_SECATTR_Msk)
                   >> SPU_FEATURE_TDD_SECATTR_Pos;
#endif // NRF_SPU_HAS_TDD

#if NRF_SPU_HAS_MRAMC
        case NRF_SPU_FEATURE_MRAMC_WAITSTATES:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].WAITSTATES
                    & SPU_FEATURE_MRAMC_WAITSTATES_SECATTR_Msk)
                   >> SPU_FEATURE_MRAMC_WAITSTATES_SECATTR_Pos;

        case NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN
                    & SPU_FEATURE_MRAMC_AUTODPOWERDOWN_SECATTR_Msk)
                   >> SPU_FEATURE_MRAMC_AUTODPOWERDOWN_SECATTR_Pos;

        case NRF_SPU_FEATURE_MRAMC_READY:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].READY
                    & SPU_FEATURE_MRAMC_READY_SECATTR_Msk)
                   >> SPU_FEATURE_MRAMC_READY_SECATTR_Pos;
#endif // NRF_SPU_HAS_MRAMC

        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE bool nrf_spu_feature_lock_get(NRF_SPU_Type const * p_reg,
                                                nrf_spu_feature_t    feature,
                                                uint8_t              index,
                                                uint8_t              subindex)
{
    switch (feature)
    {
#if NRF_SPU_HAS_IPCT
        case NRF_SPU_FEATURE_IPCT_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT);
            return (p_reg->FEATURE.IPCT.CH[index]
                    & SPU_FEATURE_IPCT_CH_LOCK_Msk)
                   >> SPU_FEATURE_IPCT_CH_LOCK_Pos;

        case NRF_SPU_FEATURE_IPCT_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT);
            return (p_reg->FEATURE.IPCT.INTERRUPT[index]
                    & SPU_FEATURE_IPCT_INTERRUPT_LOCK_Msk)
                   >> SPU_FEATURE_IPCT_INTERRUPT_LOCK_Pos;
#endif // NRF_SPU_HAS_IPCT

        case NRF_SPU_FEATURE_DPPI_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT);
            return (p_reg->FEATURE.DPPIC.CH[index]
                    & SPU_FEATURE_DPPIC_CH_LOCK_Msk)
                   >> SPU_FEATURE_DPPIC_CH_LOCK_Pos;

        case NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT);
            return (p_reg->FEATURE.DPPIC.CHG[index]
                    & SPU_FEATURE_DPPIC_CHG_LOCK_Msk)
                   >> SPU_FEATURE_DPPIC_CHG_LOCK_Pos;

        case NRF_SPU_FEATURE_GPIOTE_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT);
            return (p_reg->FEATURE.GPIOTE[index].CH[subindex]
                    & SPU_FEATURE_GPIOTE_CH_LOCK_Msk)
                   >> SPU_FEATURE_GPIOTE_CH_LOCK_Pos;

        case NRF_SPU_FEATURE_GPIOTE_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT);
            return (p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex]
                    & SPU_FEATURE_GPIOTE_INTERRUPT_LOCK_Msk)
                   >> SPU_FEATURE_GPIOTE_INTERRUPT_LOCK_Pos;

        case NRF_SPU_FEATURE_GPIO_PIN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIO_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIO_PIN_COUNT);
            return (p_reg->FEATURE.GPIO[index].PIN[subindex]
                    & SPU_FEATURE_GPIO_PIN_LOCK_Msk)
                   >> SPU_FEATURE_GPIO_PIN_LOCK_Pos;

        case NRF_SPU_FEATURE_GRTC_CC:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_CC_COUNT);
            return (p_reg->FEATURE.GRTC.CC[index]
                    & SPU_FEATURE_GRTC_CC_LOCK_Msk)
                   >> SPU_FEATURE_GRTC_CC_LOCK_Pos;

        case NRF_SPU_FEATURE_GRTC_SYSCOUNTER:
            return (p_reg->FEATURE.GRTC.SYSCOUNTER
                    & SPU_FEATURE_GRTC_SYSCOUNTER_LOCK_Msk)
                   >> SPU_FEATURE_GRTC_SYSCOUNTER_LOCK_Pos;

        case NRF_SPU_FEATURE_GRTC_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT);
            return (p_reg->FEATURE.GRTC.INTERRUPT[index]
                    & SPU_FEATURE_GRTC_INTERRUPT_LOCK_Msk)
                   >> SPU_FEATURE_GRTC_INTERRUPT_LOCK_Pos;

#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
        case NRF_SPU_FEATURE_BELLS_BELL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELL_BELL_COUNT);
            return (p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex]
                    & SPU_FEATURE_BELLS_DOMAIN_BELL_LOCK_Msk)
                   >> SPU_FEATURE_BELLS_DOMAIN_BELL_LOCK_Pos;
#else
        case NRF_SPU_FEATURE_BELLS_TASKS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_TASKS_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_TASKS_LOCK_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_TASKS_LOCK_Pos;

        case NRF_SPU_FEATURE_BELLS_EVENTS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_EVENTS_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_EVENTS_LOCK_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_EVENTS_LOCK_Pos;

        case NRF_SPU_FEATURE_BELLS_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_LOCK_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_LOCK_Pos;
#endif // NRF_SPU_HAS_DOMAIN
#endif // NRF_SPU_HAS_BELLS

#if NRF_SPU_HAS_TDD
        case NRF_SPU_FEATURE_TDD:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_TDD_COUNT);
            return (p_reg->FEATURE.TDD[index]
                    & SPU_FEATURE_TDD_LOCK_Msk)
                   >> SPU_FEATURE_TDD_LOCK_Pos;
#endif // NRF_SPU_HAS_TDD

#if NRF_SPU_HAS_MRAMC
        case NRF_SPU_FEATURE_MRAMC_WAITSTATES:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].WAITSTATES
                    & SPU_FEATURE_MRAMC_WAITSTATES_LOCK_Msk)
                   >> SPU_FEATURE_MRAMC_WAITSTATES_LOCK_Pos;

        case NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN
                    & SPU_FEATURE_MRAMC_AUTODPOWERDOWN_LOCK_Msk)
                   >> SPU_FEATURE_MRAMC_AUTODPOWERDOWN_LOCK_Pos;

        case NRF_SPU_FEATURE_MRAMC_READY:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].READY
                    & SPU_FEATURE_MRAMC_READY_LOCK_Msk)
                   >> SPU_FEATURE_MRAMC_READY_LOCK_Pos;
#endif // NRF_SPU_HAS_MRAMC

        default:
            NRFX_ASSERT(0);
            return false;
    }
}

#if NRF_SPU_HAS_BLOCK
NRF_STATIC_INLINE bool nrf_spu_feature_block_get(NRF_SPU_Type const * p_reg,
                                                 nrf_spu_feature_t    feature,
                                                 uint8_t              index,
                                                 uint8_t              subindex)
{
    switch (feature)
    {
#if NRF_SPU_HAS_IPCT
        case NRF_SPU_FEATURE_IPCT_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT);
            return (p_reg->FEATURE.IPCT.CH[index]
                    & SPU_FEATURE_IPCT_CH_BLOCK_Msk)
                   >> SPU_FEATURE_IPCT_CH_BLOCK_Pos;

        case NRF_SPU_FEATURE_IPCT_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT);
            return (p_reg->FEATURE.IPCT.INTERRUPT[index]
                    & SPU_FEATURE_IPCT_INTERRUPT_BLOCK_Msk)
                   >> SPU_FEATURE_IPCT_INTERRUPT_BLOCK_Pos;
#endif // NRF_SPU_HAS_IPCT

        case NRF_SPU_FEATURE_DPPI_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT);
            return (p_reg->FEATURE.DPPIC.CH[index]
                    & SPU_FEATURE_DPPIC_CH_BLOCK_Msk)
                   >> SPU_FEATURE_DPPIC_CH_BLOCK_Pos;

        case NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT);
            return (p_reg->FEATURE.DPPIC.CHG[index]
                    & SPU_FEATURE_DPPIC_CHG_BLOCK_Msk)
                   >> SPU_FEATURE_DPPIC_CHG_BLOCK_Pos;

        case NRF_SPU_FEATURE_GPIOTE_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT);
            return (p_reg->FEATURE.GPIOTE[index].CH[subindex]
                    & SPU_FEATURE_GPIOTE_CH_BLOCK_Msk)
                   >> SPU_FEATURE_GPIOTE_CH_BLOCK_Pos;

        case NRF_SPU_FEATURE_GPIOTE_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT);
            return (p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex]
                    & SPU_FEATURE_GPIOTE_INTERRUPT_BLOCK_Msk)
                   >> SPU_FEATURE_GPIOTE_INTERRUPT_BLOCK_Pos;

        case NRF_SPU_FEATURE_GPIO_PIN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIO_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIO_PIN_COUNT);
            return (p_reg->FEATURE.GPIO[index].PIN[subindex]
                    & SPU_FEATURE_GPIO_PIN_BLOCK_Msk)
                   >> SPU_FEATURE_GPIO_PIN_BLOCK_Pos;

        case NRF_SPU_FEATURE_GRTC_CC:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_CC_COUNT);
            return (p_reg->FEATURE.GRTC.CC[index]
                    & SPU_FEATURE_GRTC_CC_BLOCK_Msk)
                   >> SPU_FEATURE_GRTC_CC_BLOCK_Pos;

        case NRF_SPU_FEATURE_GRTC_SYSCOUNTER:
            return (p_reg->FEATURE.GRTC.SYSCOUNTER
                    & SPU_FEATURE_GRTC_SYSCOUNTER_BLOCK_Msk)
                   >> SPU_FEATURE_GRTC_SYSCOUNTER_BLOCK_Pos;

        case NRF_SPU_FEATURE_GRTC_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT);
            return (p_reg->FEATURE.GRTC.INTERRUPT[index]
                    & SPU_FEATURE_GRTC_INTERRUPT_BLOCK_Msk)
                   >> SPU_FEATURE_GRTC_INTERRUPT_BLOCK_Pos;

#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
        case NRF_SPU_FEATURE_BELLS_BELL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELL_BELL_COUNT);
            return (p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex]
                    & SPU_FEATURE_BELLS_DOMAIN_BELL_BLOCK_Msk)
                   >> SPU_FEATURE_BELLS_DOMAIN_BELL_BLOCK_Pos;
#else
        case NRF_SPU_FEATURE_BELLS_TASKS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_TASKS_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_TASKS_BLOCK_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_TASKS_BLOCK_Pos;

        case NRF_SPU_FEATURE_BELLS_EVENTS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_EVENTS_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_EVENTS_BLOCK_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_EVENTS_BLOCK_Pos;

        case NRF_SPU_FEATURE_BELLS_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT);
            return (p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_BLOCK_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_BLOCK_Pos;
#endif // NRF_SPU_HAS_DOMAIN
#endif // NRF_SPU_HAS_BELLS

#if NRF_SPU_HAS_TDD
        case NRF_SPU_FEATURE_TDD:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_TDD_COUNT);
            return (p_reg->FEATURE.TDD[index]
                    & SPU_FEATURE_TDD_BLOCK_Msk)
                   >> SPU_FEATURE_TDD_BLOCK_Pos;
#endif // NRF_SPU_HAS_TDD

#if NRF_SPU_HAS_MRAMC
        case NRF_SPU_FEATURE_MRAMC_WAITSTATES:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].WAITSTATES
                    & SPU_FEATURE_MRAMC_WAITSTATES_BLOCK_Msk)
                   >> SPU_FEATURE_MRAMC_WAITSTATES_BLOCK_Pos;

        case NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN
                    & SPU_FEATURE_MRAMC_AUTODPOWERDOWN_BLOCK_Msk)
                   >> SPU_FEATURE_MRAMC_AUTODPOWERDOWN_BLOCK_Pos;

        case NRF_SPU_FEATURE_MRAMC_READY:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (p_reg->FEATURE.MRAMC[index].READY
                    & SPU_FEATURE_MRAMC_READY_BLOCK_Msk)
                   >> SPU_FEATURE_MRAMC_READY_BLOCK_Pos;
#endif // NRF_SPU_HAS_MRAMC

        default:
            NRFX_ASSERT(0);
            return false;
    }
}
#endif // NRF_SPU_HAS_BLOCK

#if NRF_SPU_HAS_OWNERSHIP
NRF_STATIC_INLINE nrf_owner_t nrf_spu_feature_ownerid_get(NRF_SPU_Type const * p_reg,
                                                          nrf_spu_feature_t    feature,
                                                          uint8_t              index,
                                                          uint8_t              subindex)
{
    switch (feature)
    {
#if NRF_SPU_HAS_IPCT
        case NRF_SPU_FEATURE_IPCT_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.IPCT.CH[index]
                                  & SPU_FEATURE_IPCT_CH_OWNERID_Msk)
                                 >> SPU_FEATURE_IPCT_CH_OWNERID_Pos);

        case NRF_SPU_FEATURE_IPCT_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.IPCT.INTERRUPT[index]
                                  & SPU_FEATURE_IPCT_INTERRUPT_OWNERID_Msk)
                                 >> SPU_FEATURE_IPCT_INTERRUPT_OWNERID_Pos);
#endif // NRF_SPU_HAS_IPCT

        case NRF_SPU_FEATURE_DPPI_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.DPPIC.CH[index]
                                  & SPU_FEATURE_DPPIC_CH_OWNERID_Msk)
                                 >> SPU_FEATURE_DPPIC_CH_OWNERID_Pos);

        case NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.DPPIC.CHG[index]
                                  & SPU_FEATURE_DPPIC_CHG_OWNERID_Msk)
                                 >> SPU_FEATURE_DPPIC_CHG_OWNERID_Pos);

        case NRF_SPU_FEATURE_GPIOTE_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.GPIOTE[index].CH[subindex]
                                  & SPU_FEATURE_GPIOTE_CH_OWNERID_Msk)
                                 >> SPU_FEATURE_GPIOTE_CH_OWNERID_Pos);

        case NRF_SPU_FEATURE_GPIOTE_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex]
                                  & SPU_FEATURE_GPIOTE_INTERRUPT_OWNERID_Msk)
                                 >> SPU_FEATURE_GPIOTE_INTERRUPT_OWNERID_Pos);

        case NRF_SPU_FEATURE_GPIO_PIN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIO_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIO_PIN_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.GPIO[index].PIN[subindex]
                                  & SPU_FEATURE_GPIO_PIN_OWNERID_Msk)
                                 >> SPU_FEATURE_GPIO_PIN_OWNERID_Pos);

        case NRF_SPU_FEATURE_GRTC_CC:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_CC_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.GRTC.CC[index]
                                  & SPU_FEATURE_GRTC_CC_OWNERID_Msk)
                                 >> SPU_FEATURE_GRTC_CC_OWNERID_Pos);

        case NRF_SPU_FEATURE_GRTC_SYSCOUNTER:
            return (nrf_owner_t)((p_reg->FEATURE.GRTC.SYSCOUNTER
                                  & SPU_FEATURE_GRTC_SYSCOUNTER_OWNERID_Msk)
                                 >> SPU_FEATURE_GRTC_SYSCOUNTER_OWNERID_Pos);

        case NRF_SPU_FEATURE_GRTC_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.GRTC.INTERRUPT[index]
                                  & SPU_FEATURE_GRTC_INTERRUPT_OWNERID_Msk)
                                 >> SPU_FEATURE_GRTC_INTERRUPT_OWNERID_Pos);

#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
        case NRF_SPU_FEATURE_BELLS_BELL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELL_BELL_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex]
                    & SPU_FEATURE_BELLS_DOMAIN_BELL_OWNERID_Msk)
                   >> SPU_FEATURE_BELLS_DOMAIN_BELL_OWNERID_Pos);
#else
        case NRF_SPU_FEATURE_BELLS_TASKS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_TASKS_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_TASKS_OWNERID_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_TASKS_OWNERID_Pos);

        case NRF_SPU_FEATURE_BELLS_EVENTS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_EVENTS_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_EVENTS_OWNERID_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_EVENTS_OWNERID_Pos);

        case NRF_SPU_FEATURE_BELLS_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex]
                    & SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_OWNERID_Msk)
                   >> SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_OWNERID_Pos);
#endif // NRF_SPU_HAS_DOMAIN
#endif // NRF_SPU_HAS_BELLS

#if NRF_SPU_HAS_TDD
        case NRF_SPU_FEATURE_TDD:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_TDD_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.TDD[index]
                    & SPU_FEATURE_TDD_OWNERID_Msk)
                   >> SPU_FEATURE_TDD_OWNERID_Pos);
#endif // NRF_SPU_HAS_TDD

#if NRF_SPU_HAS_MRAMC
        case NRF_SPU_FEATURE_MRAMC_WAITSTATES:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.MRAMC[index].WAITSTATES
                    & SPU_FEATURE_MRAMC_WAITSTATES_OWNERID_Msk)
                   >> SPU_FEATURE_MRAMC_WAITSTATES_OWNERID_Pos);

        case NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN
                    & SPU_FEATURE_MRAMC_AUTODPOWERDOWN_OWNERID_Msk)
                   >> SPU_FEATURE_MRAMC_AUTODPOWERDOWN_OWNERID_Pos);

        case NRF_SPU_FEATURE_MRAMC_READY:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            return (nrf_owner_t)((p_reg->FEATURE.MRAMC[index].READY
                    & SPU_FEATURE_MRAMC_READY_OWNERID_Msk)
                   >> SPU_FEATURE_MRAMC_READY_OWNERID_Pos);
#endif // NRF_SPU_HAS_MRAMC

        default:
            NRFX_ASSERT(0);
            return (nrf_owner_t)0;
    }
}
#endif // NRF_SPU_HAS_OWNERSHIP

NRF_STATIC_INLINE void nrf_spu_feature_secattr_set(NRF_SPU_Type *    p_reg,
                                                   nrf_spu_feature_t feature,
                                                   uint8_t           index,
                                                   uint8_t           subindex,
                                                   bool              enable)
{
    switch (feature)
    {
#if NRF_SPU_HAS_IPCT
        case NRF_SPU_FEATURE_IPCT_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT);
            p_reg->FEATURE.IPCT.CH[index] =
                ((p_reg->FEATURE.IPCT.CH[index] &
                  ~SPU_FEATURE_IPCT_CH_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_IPCT_CH_SECATTR_Secure :
                   SPU_FEATURE_IPCT_CH_SECATTR_NonSecure)
                  << SPU_FEATURE_IPCT_CH_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_IPCT_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT);
            p_reg->FEATURE.IPCT.INTERRUPT[index] =
                ((p_reg->FEATURE.IPCT.INTERRUPT[index] &
                  ~SPU_FEATURE_IPCT_INTERRUPT_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_IPCT_INTERRUPT_SECATTR_Secure :
                   SPU_FEATURE_IPCT_INTERRUPT_SECATTR_NonSecure)
                  <<  SPU_FEATURE_IPCT_INTERRUPT_SECATTR_Pos));
            break;
#endif // NRF_SPU_HAS_IPCT

        case NRF_SPU_FEATURE_DPPI_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT);
            p_reg->FEATURE.DPPIC.CH[index] =
                ((p_reg->FEATURE.DPPIC.CH[index] &
                  ~SPU_FEATURE_DPPIC_CH_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_DPPIC_CH_SECATTR_Secure :
                   SPU_FEATURE_DPPIC_CH_SECATTR_NonSecure)
                  <<  SPU_FEATURE_DPPIC_CH_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT);
            p_reg->FEATURE.DPPIC.CHG[index] =
                ((p_reg->FEATURE.DPPIC.CHG[index] &
                  ~SPU_FEATURE_DPPIC_CHG_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_DPPIC_CHG_SECATTR_Secure :
                   SPU_FEATURE_DPPIC_CHG_SECATTR_NonSecure)
                  <<  SPU_FEATURE_DPPIC_CHG_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_GPIOTE_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT);
            p_reg->FEATURE.GPIOTE[index].CH[subindex] =
                ((p_reg->FEATURE.GPIOTE[index].CH[subindex] &
                  ~SPU_FEATURE_GPIOTE_CH_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_GPIOTE_CH_SECATTR_Secure :
                   SPU_FEATURE_GPIOTE_CH_SECATTR_NonSecure)
                  <<  SPU_FEATURE_GPIOTE_CH_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_GPIOTE_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT);
            p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex] =
                ((p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex] &
                  ~SPU_FEATURE_GPIOTE_INTERRUPT_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_GPIOTE_INTERRUPT_SECATTR_Secure :
                   SPU_FEATURE_GPIOTE_INTERRUPT_SECATTR_NonSecure)
                  <<  SPU_FEATURE_GPIOTE_INTERRUPT_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_GPIO_PIN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIO_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIO_PIN_COUNT);
            p_reg->FEATURE.GPIO[index].PIN[subindex] =
                ((p_reg->FEATURE.GPIO[index].PIN[subindex] &
                  ~SPU_FEATURE_GPIO_PIN_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_GPIO_PIN_SECATTR_Secure :
                   SPU_FEATURE_GPIO_PIN_SECATTR_NonSecure)
                  <<  SPU_FEATURE_GPIO_PIN_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_CC:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_CC_COUNT);
            p_reg->FEATURE.GRTC.CC[index] =
                ((p_reg->FEATURE.GRTC.CC[index] &
                  ~SPU_FEATURE_GRTC_CC_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_GRTC_CC_SECATTR_Secure :
                   SPU_FEATURE_GRTC_CC_SECATTR_NonSecure)
                  <<  SPU_FEATURE_GRTC_CC_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_SYSCOUNTER:
            p_reg->FEATURE.GRTC.SYSCOUNTER =
                ((p_reg->FEATURE.GRTC.SYSCOUNTER &
                  ~SPU_FEATURE_GRTC_SYSCOUNTER_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_GRTC_SYSCOUNTER_SECATTR_Secure :
                   SPU_FEATURE_GRTC_SYSCOUNTER_SECATTR_NonSecure)
                  <<  SPU_FEATURE_GRTC_SYSCOUNTER_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT);
            p_reg->FEATURE.GRTC.INTERRUPT[index] =
                ((p_reg->FEATURE.GRTC.INTERRUPT[index] &
                  ~SPU_FEATURE_GRTC_INTERRUPT_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_GRTC_INTERRUPT_SECATTR_Secure :
                   SPU_FEATURE_GRTC_INTERRUPT_SECATTR_NonSecure)
                  <<  SPU_FEATURE_GRTC_INTERRUPT_SECATTR_Pos));
            break;

#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
        case NRF_SPU_FEATURE_BELLS_BELL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELL_BELL_COUNT);
            p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex] =
                ((p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex] &
                  ~SPU_FEATURE_BELLS_DOMAIN_BELL_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_BELLS_DOMAIN_BELL_SECATTR_Secure :
                   SPU_FEATURE_BELLS_DOMAIN_BELL_SECATTR_NonSecure)
                  <<  SPU_FEATURE_BELLS_DOMAIN_BELL_SECATTR_Pos));
            break;
#else
        case NRF_SPU_FEATURE_BELLS_TASKS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_TASKS_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_TASKS_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_BELLS_PROCESSOR_TASKS_SECATTR_Secure :
                   SPU_FEATURE_BELLS_PROCESSOR_TASKS_SECATTR_NonSecure)
                  <<  SPU_FEATURE_BELLS_PROCESSOR_TASKS_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_BELLS_EVENTS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_EVENTS_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_EVENTS_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_BELLS_PROCESSOR_EVENTS_SECATTR_Secure :
                   SPU_FEATURE_BELLS_PROCESSOR_EVENTS_SECATTR_NonSecure)
                  <<  SPU_FEATURE_BELLS_PROCESSOR_EVENTS_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_BELLS_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_SECATTR_Secure :
                   SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_SECATTR_NonSecure)
                  <<  SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_SECATTR_Pos));
            break;
#endif // NRF_SPU_HAS_DOMAIN
#endif // NRF_SPU_HAS_BELLS

#if NRF_SPU_HAS_TDD
        case NRF_SPU_FEATURE_TDD:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_TDD_COUNT);
            p_reg->FEATURE.TDD[index] =
                ((p_reg->FEATURE.TDD[index] &
                  ~SPU_FEATURE_TDD_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_TDD_SECATTR_Secure :
                   SPU_FEATURE_TDD_SECATTR_NonSecure)
                  << SPU_FEATURE_TDD_SECATTR_Pos));
            break;
#endif // NRF_SPU_HAS_TDD

#if NRF_SPU_HAS_MRAMC
        case NRF_SPU_FEATURE_MRAMC_WAITSTATES:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].WAITSTATES =
                ((p_reg->FEATURE.MRAMC[index].WAITSTATES &
                  ~SPU_FEATURE_MRAMC_WAITSTATES_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_MRAMC_WAITSTATES_SECATTR_Secure :
                   SPU_FEATURE_MRAMC_WAITSTATES_SECATTR_NonSecure)
                  << SPU_FEATURE_MRAMC_WAITSTATES_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN =
                ((p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN &
                  ~SPU_FEATURE_MRAMC_AUTODPOWERDOWN_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_MRAMC_AUTODPOWERDOWN_SECATTR_Secure :
                   SPU_FEATURE_MRAMC_AUTODPOWERDOWN_SECATTR_NonSecure)
                  << SPU_FEATURE_MRAMC_AUTODPOWERDOWN_SECATTR_Pos));
            break;

        case NRF_SPU_FEATURE_MRAMC_READY:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].READY =
                ((p_reg->FEATURE.MRAMC[index].READY &
                  ~SPU_FEATURE_MRAMC_READY_SECATTR_Msk) |
                 ((enable ?
                   SPU_FEATURE_MRAMC_READY_SECATTR_Secure :
                   SPU_FEATURE_MRAMC_READY_SECATTR_NonSecure)
                  << SPU_FEATURE_MRAMC_READY_SECATTR_Pos));
            break;
#endif // NRF_SPU_HAS_MRAMC

        default:
            NRFX_ASSERT(0);
            break;
    }
}

NRF_STATIC_INLINE void nrf_spu_feature_lock_enable(NRF_SPU_Type *    p_reg,
                                                   nrf_spu_feature_t feature,
                                                   uint8_t           index,
                                                   uint8_t           subindex)
{
    switch (feature)
    {
#if NRF_SPU_HAS_IPCT
        case NRF_SPU_FEATURE_IPCT_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT);
            p_reg->FEATURE.IPCT.CH[index] =
                ((p_reg->FEATURE.IPCT.CH[index] &
                  ~SPU_FEATURE_IPCT_CH_LOCK_Msk) |
                 (SPU_FEATURE_IPCT_CH_LOCK_Locked
                  << SPU_FEATURE_IPCT_CH_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_IPCT_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT);
            p_reg->FEATURE.IPCT.INTERRUPT[index] =
                ((p_reg->FEATURE.IPCT.INTERRUPT[index] &
                  ~SPU_FEATURE_IPCT_INTERRUPT_LOCK_Msk) |
                 (SPU_FEATURE_IPCT_INTERRUPT_LOCK_Locked
                  << SPU_FEATURE_IPCT_INTERRUPT_LOCK_Pos));
            break;
#endif // NRF_SPU_HAS_IPCT

        case NRF_SPU_FEATURE_DPPI_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT);
            p_reg->FEATURE.DPPIC.CH[index] =
                ((p_reg->FEATURE.DPPIC.CH[index] &
                  ~SPU_FEATURE_DPPIC_CH_LOCK_Msk) |
                 (SPU_FEATURE_DPPIC_CH_LOCK_Locked
                  << SPU_FEATURE_DPPIC_CH_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT);
            p_reg->FEATURE.DPPIC.CHG[index] =
                ((p_reg->FEATURE.DPPIC.CHG[index] &
                  ~SPU_FEATURE_DPPIC_CHG_LOCK_Msk) |
                 (SPU_FEATURE_DPPIC_CHG_LOCK_Locked
                  << SPU_FEATURE_DPPIC_CHG_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GPIOTE_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT);
            p_reg->FEATURE.GPIOTE[index].CH[subindex] =
                ((p_reg->FEATURE.GPIOTE[index].CH[subindex] &
                  ~SPU_FEATURE_GPIOTE_CH_LOCK_Msk) |
                 (SPU_FEATURE_GPIOTE_CH_LOCK_Locked
                  << SPU_FEATURE_GPIOTE_CH_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GPIOTE_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT);
            p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex] =
                ((p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex] &
                  ~SPU_FEATURE_GPIOTE_INTERRUPT_LOCK_Msk) |
                 (SPU_FEATURE_GPIOTE_INTERRUPT_LOCK_Locked
                  << SPU_FEATURE_GPIOTE_INTERRUPT_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GPIO_PIN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIO_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIO_PIN_COUNT);
            p_reg->FEATURE.GPIO[index].PIN[subindex] =
                ((p_reg->FEATURE.GPIO[index].PIN[subindex] &
                  ~SPU_FEATURE_GPIO_PIN_LOCK_Msk) |
                 (SPU_FEATURE_GPIO_PIN_LOCK_Locked
                  << SPU_FEATURE_GPIO_PIN_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_CC:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_CC_COUNT);
            p_reg->FEATURE.GRTC.CC[index] =
                ((p_reg->FEATURE.GRTC.CC[index] &
                  ~SPU_FEATURE_GRTC_CC_LOCK_Msk) |
                 (SPU_FEATURE_GRTC_CC_LOCK_Locked
                  << SPU_FEATURE_GRTC_CC_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_SYSCOUNTER:
            p_reg->FEATURE.GRTC.SYSCOUNTER =
                ((p_reg->FEATURE.GRTC.SYSCOUNTER &
                  ~SPU_FEATURE_GRTC_SYSCOUNTER_LOCK_Msk) |
                 (SPU_FEATURE_GRTC_SYSCOUNTER_LOCK_Locked
                  << SPU_FEATURE_GRTC_SYSCOUNTER_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT);
            p_reg->FEATURE.GRTC.INTERRUPT[index] =
                ((p_reg->FEATURE.GRTC.INTERRUPT[index] &
                  ~SPU_FEATURE_GRTC_INTERRUPT_LOCK_Msk) |
                 (SPU_FEATURE_GRTC_INTERRUPT_LOCK_Locked
                  << SPU_FEATURE_GRTC_INTERRUPT_LOCK_Pos));
            break;

#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
        case NRF_SPU_FEATURE_BELLS_BELL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELL_BELL_COUNT);
            p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex] =
                ((p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex] &
                  ~SPU_FEATURE_BELLS_DOMAIN_BELL_LOCK_Msk) |
                 (SPU_FEATURE_BELLS_DOMAIN_BELL_LOCK_Locked
                  << SPU_FEATURE_BELLS_DOMAIN_BELL_LOCK_Pos));
            break;
#else
        case NRF_SPU_FEATURE_BELLS_TASKS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_TASKS_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_TASKS_LOCK_Msk) |
                 (SPU_FEATURE_BELLS_PROCESSOR_TASKS_LOCK_Locked
                  << SPU_FEATURE_BELLS_PROCESSOR_TASKS_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_BELLS_EVENTS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_EVENTS_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_EVENTS_LOCK_Msk) |
                 (SPU_FEATURE_BELLS_PROCESSOR_EVENTS_LOCK_Locked
                  << SPU_FEATURE_BELLS_PROCESSOR_EVENTS_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_BELLS_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_LOCK_Msk) |
                 (SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_LOCK_Locked
                  << SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_LOCK_Pos));
            break;
#endif // NRF_SPU_HAS_DOMAIN
#endif // NRF_SPU_HAS_BELLS

#if NRF_SPU_HAS_TDD
        case NRF_SPU_FEATURE_TDD:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_TDD_COUNT);
            p_reg->FEATURE.TDD[index] =
                ((p_reg->FEATURE.TDD[index] &
                  ~SPU_FEATURE_TDD_LOCK_Msk) |
                 (SPU_FEATURE_TDD_LOCK_Locked
                  << SPU_FEATURE_TDD_LOCK_Pos));
            break;
#endif // NRF_SPU_HAS_TDD

#if NRF_SPU_HAS_MRAMC
        case NRF_SPU_FEATURE_MRAMC_WAITSTATES:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].WAITSTATES =
                ((p_reg->FEATURE.MRAMC[index].WAITSTATES &
                  ~SPU_FEATURE_MRAMC_WAITSTATES_LOCK_Msk) |
                 (SPU_FEATURE_MRAMC_WAITSTATES_LOCK_Locked
                  << SPU_FEATURE_MRAMC_WAITSTATES_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN =
                ((p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN &
                  ~SPU_FEATURE_MRAMC_AUTODPOWERDOWN_LOCK_Msk) |
                 (SPU_FEATURE_MRAMC_AUTODPOWERDOWN_LOCK_Locked
                  << SPU_FEATURE_MRAMC_AUTODPOWERDOWN_LOCK_Pos));
            break;

        case NRF_SPU_FEATURE_MRAMC_READY:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].READY =
                ((p_reg->FEATURE.MRAMC[index].READY &
                  ~SPU_FEATURE_MRAMC_READY_LOCK_Msk) |
                 (SPU_FEATURE_MRAMC_READY_LOCK_Locked
                  << SPU_FEATURE_MRAMC_READY_LOCK_Pos));
            break;
#endif // NRF_SPU_HAS_MRAMC

        default:
            NRFX_ASSERT(0);
            break;
    }
}

#if NRF_SPU_HAS_BLOCK
NRF_STATIC_INLINE void nrf_spu_feature_block_enable(NRF_SPU_Type *    p_reg,
                                                    nrf_spu_feature_t feature,
                                                    uint8_t           index,
                                                    uint8_t           subindex)
{
    switch (feature)
    {
#if NRF_SPU_HAS_IPCT
        case NRF_SPU_FEATURE_IPCT_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT);
            p_reg->FEATURE.IPCT.CH[index] =
                ((p_reg->FEATURE.IPCT.CH[index] &
                  ~SPU_FEATURE_IPCT_CH_BLOCK_Msk) |
                 (SPU_FEATURE_IPCT_CH_BLOCK_Blocked
                  << SPU_FEATURE_IPCT_CH_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_IPCT_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT);
            p_reg->FEATURE.IPCT.INTERRUPT[index] =
                ((p_reg->FEATURE.IPCT.INTERRUPT[index] &
                  ~SPU_FEATURE_IPCT_INTERRUPT_BLOCK_Msk) |
                 (SPU_FEATURE_IPCT_INTERRUPT_BLOCK_Blocked
                  << SPU_FEATURE_IPCT_INTERRUPT_BLOCK_Pos));
            break;
#endif // NRF_SPU_HAS_IPCT

        case NRF_SPU_FEATURE_DPPI_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT);
            p_reg->FEATURE.DPPIC.CH[index] =
                ((p_reg->FEATURE.DPPIC.CH[index] &
                  ~SPU_FEATURE_DPPIC_CH_BLOCK_Msk) |
                 (SPU_FEATURE_DPPIC_CH_BLOCK_Blocked
                  << SPU_FEATURE_DPPIC_CH_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT);
            p_reg->FEATURE.DPPIC.CHG[index] =
                ((p_reg->FEATURE.DPPIC.CHG[index] &
                  ~SPU_FEATURE_DPPIC_CHG_BLOCK_Msk) |
                 (SPU_FEATURE_DPPIC_CHG_BLOCK_Blocked
                  << SPU_FEATURE_DPPIC_CHG_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GPIOTE_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT);
            p_reg->FEATURE.GPIOTE[index].CH[subindex] =
                ((p_reg->FEATURE.GPIOTE[index].CH[subindex] &
                  ~SPU_FEATURE_GPIOTE_CH_BLOCK_Msk) |
                 (SPU_FEATURE_GPIOTE_CH_BLOCK_Blocked
                  << SPU_FEATURE_GPIOTE_CH_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GPIOTE_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT);
            p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex] =
                ((p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex] &
                  ~SPU_FEATURE_GPIOTE_INTERRUPT_BLOCK_Msk) |
                 (SPU_FEATURE_GPIOTE_INTERRUPT_BLOCK_Blocked
                  << SPU_FEATURE_GPIOTE_INTERRUPT_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GPIO_PIN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIO_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIO_PIN_COUNT);
            p_reg->FEATURE.GPIO[index].PIN[subindex] =
                ((p_reg->FEATURE.GPIO[index].PIN[subindex] &
                  ~SPU_FEATURE_GPIO_PIN_BLOCK_Msk) |
                 (SPU_FEATURE_GPIO_PIN_BLOCK_Blocked
                  << SPU_FEATURE_GPIO_PIN_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_CC:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_CC_COUNT);
            p_reg->FEATURE.GRTC.CC[index] =
                ((p_reg->FEATURE.GRTC.CC[index] &
                  ~SPU_FEATURE_GRTC_CC_BLOCK_Msk) |
                 (SPU_FEATURE_GRTC_CC_BLOCK_Blocked
                  << SPU_FEATURE_GRTC_CC_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_SYSCOUNTER:
            p_reg->FEATURE.GRTC.SYSCOUNTER =
                ((p_reg->FEATURE.GRTC.SYSCOUNTER &
                  ~SPU_FEATURE_GRTC_SYSCOUNTER_BLOCK_Msk) |
                 (SPU_FEATURE_GRTC_SYSCOUNTER_BLOCK_Blocked
                  << SPU_FEATURE_GRTC_SYSCOUNTER_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_GRTC_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT);
            p_reg->FEATURE.GRTC.INTERRUPT[index] =
                ((p_reg->FEATURE.GRTC.INTERRUPT[index] &
                  ~SPU_FEATURE_GRTC_INTERRUPT_BLOCK_Msk) |
                 (SPU_FEATURE_GRTC_INTERRUPT_BLOCK_Blocked
                  << SPU_FEATURE_GRTC_INTERRUPT_BLOCK_Pos));
            break;

#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
        case NRF_SPU_FEATURE_BELLS_BELL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELL_BELL_COUNT);
            p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex] =
                ((p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex] &
                  ~SPU_FEATURE_BELLS_DOMAIN_BELL_BLOCK_Msk) |
                 (SPU_FEATURE_BELLS_DOMAIN_BELL_BLOCK_Blocked
                  << SPU_FEATURE_BELLS_DOMAIN_BELL_BLOCK_Pos));
            break;
#else
        case NRF_SPU_FEATURE_BELLS_TASKS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_TASKS_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_TASKS_BLOCK_Msk) |
                 (SPU_FEATURE_BELLS_PROCESSOR_TASKS_BLOCK_Blocked
                  << SPU_FEATURE_BELLS_PROCESSOR_TASKS_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_BELLS_EVENTS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_EVENTS_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_EVENTS_BLOCK_Msk) |
                 (SPU_FEATURE_BELLS_PROCESSOR_EVENTS_BLOCK_Blocked
                  << SPU_FEATURE_BELLS_PROCESSOR_EVENTS_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_BELLS_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_BLOCK_Msk) |
                 (SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_BLOCK_Blocked
                  << SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_BLOCK_Pos));
            break;
#endif // NRF_SPU_HAS_DOMAIN
#endif // NRF_SPU_HAS_BELLS

#if NRF_SPU_HAS_TDD
        case NRF_SPU_FEATURE_TDD:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_TDD_COUNT);
            p_reg->FEATURE.TDD[index] =
                ((p_reg->FEATURE.TDD[index] &
                  ~SPU_FEATURE_TDD_BLOCK_Msk) |
                 (SPU_FEATURE_TDD_BLOCK_Blocked
                  << SPU_FEATURE_TDD_BLOCK_Pos));
            break;
#endif // NRF_SPU_HAS_TDD

#if NRF_SPU_HAS_MRAMC
        case NRF_SPU_FEATURE_MRAMC_WAITSTATES:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].WAITSTATES =
                ((p_reg->FEATURE.MRAMC[index].WAITSTATES &
                  ~SPU_FEATURE_MRAMC_WAITSTATES_BLOCK_Msk) |
                 (SPU_FEATURE_MRAMC_WAITSTATES_BLOCK_Blocked
                  << SPU_FEATURE_MRAMC_WAITSTATES_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN =
                ((p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN &
                  ~SPU_FEATURE_MRAMC_AUTODPOWERDOWN_BLOCK_Msk) |
                 (SPU_FEATURE_MRAMC_AUTODPOWERDOWN_BLOCK_Blocked
                  << SPU_FEATURE_MRAMC_AUTODPOWERDOWN_BLOCK_Pos));
            break;

        case NRF_SPU_FEATURE_MRAMC_READY:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].READY =
                ((p_reg->FEATURE.MRAMC[index].READY &
                  ~SPU_FEATURE_MRAMC_READY_BLOCK_Msk) |
                 (SPU_FEATURE_MRAMC_READY_BLOCK_Blocked
                  << SPU_FEATURE_MRAMC_READY_LOCK_Pos));
            break;
#endif // NRF_SPU_HAS_MRAMC

        default:
            NRFX_ASSERT(0);
            break;
    }
}
#endif // NRF_SPU_HAS_BLOCK

#if NRF_SPU_HAS_OWNERSHIP
NRF_STATIC_INLINE void nrf_spu_feature_ownerid_set(NRF_SPU_Type *    p_reg,
                                                   nrf_spu_feature_t feature,
                                                   uint8_t           index,
                                                   uint8_t           subindex,
                                                   nrf_owner_t       owner_id)
{
    switch (feature)
    {
#if NRF_SPU_HAS_IPCT
        case NRF_SPU_FEATURE_IPCT_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_CHANNEL_COUNT);
            p_reg->FEATURE.IPCT.CH[index] =
                ((p_reg->FEATURE.IPCT.CH[index] &
                  ~SPU_FEATURE_IPCT_CH_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_IPCT_CH_OWNERID_Pos) &
                  SPU_FEATURE_IPCT_CH_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_IPCT_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_IPCT_INTERRUPT_COUNT);
            p_reg->FEATURE.IPCT.INTERRUPT[index] =
                ((p_reg->FEATURE.IPCT.INTERRUPT[index] &
                  ~SPU_FEATURE_IPCT_INTERRUPT_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_IPCT_INTERRUPT_OWNERID_Pos) &
                  SPU_FEATURE_IPCT_INTERRUPT_OWNERID_Msk));
            break;
#endif // NRF_SPU_HAS_IPCT

        case NRF_SPU_FEATURE_DPPI_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_COUNT);
            p_reg->FEATURE.DPPIC.CH[index] =
                ((p_reg->FEATURE.DPPIC.CH[index] &
                  ~SPU_FEATURE_DPPIC_CH_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_DPPIC_CH_OWNERID_Pos) &
                  SPU_FEATURE_DPPIC_CH_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_DPPI_CHANNEL_GROUP_COUNT);
            p_reg->FEATURE.DPPIC.CHG[index] =
                ((p_reg->FEATURE.DPPIC.CHG[index] &
                  ~SPU_FEATURE_DPPIC_CHG_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_DPPIC_CHG_OWNERID_Pos) &
                  SPU_FEATURE_DPPIC_CHG_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_GPIOTE_CHANNEL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_CHANNEL_COUNT);
            p_reg->FEATURE.GPIOTE[index].CH[subindex] =
                ((p_reg->FEATURE.GPIOTE[index].CH[subindex] &
                  ~SPU_FEATURE_GPIOTE_CH_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_GPIOTE_CH_OWNERID_Pos) &
                  SPU_FEATURE_GPIOTE_CH_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_GPIOTE_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIOTE_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIOTE_INTERRUPT_COUNT);
            p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex] =
                ((p_reg->FEATURE.GPIOTE[index].INTERRUPT[subindex] &
                  ~SPU_FEATURE_GPIOTE_INTERRUPT_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_GPIOTE_INTERRUPT_OWNERID_Pos) &
                  SPU_FEATURE_GPIOTE_INTERRUPT_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_GPIO_PIN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GPIO_COUNT);
            NRFX_ASSERT(subindex < NRF_SPU_FEATURE_GPIO_PIN_COUNT);
            p_reg->FEATURE.GPIO[index].PIN[subindex] =
                ((p_reg->FEATURE.GPIO[index].PIN[subindex] &
                  ~SPU_FEATURE_GPIO_PIN_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_GPIO_PIN_OWNERID_Pos) &
                  SPU_FEATURE_GPIO_PIN_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_GRTC_CC:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_CC_COUNT);
            p_reg->FEATURE.GRTC.CC[index] =
                ((p_reg->FEATURE.GRTC.CC[index] &
                  ~SPU_FEATURE_GRTC_CC_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_GRTC_CC_OWNERID_Pos) &
                  SPU_FEATURE_GRTC_CC_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_GRTC_SYSCOUNTER:
            p_reg->FEATURE.GRTC.SYSCOUNTER =
                ((p_reg->FEATURE.GRTC.SYSCOUNTER &
                  ~SPU_FEATURE_GRTC_SYSCOUNTER_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_GRTC_SYSCOUNTER_OWNERID_Pos) &
                  SPU_FEATURE_GRTC_SYSCOUNTER_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_GRTC_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_GRTC_INTERRUPT_COUNT);
            p_reg->FEATURE.GRTC.INTERRUPT[index] =
                ((p_reg->FEATURE.GRTC.INTERRUPT[index] &
                  ~SPU_FEATURE_GRTC_INTERRUPT_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_GRTC_INTERRUPT_OWNERID_Pos) &
                  SPU_FEATURE_GRTC_INTERRUPT_OWNERID_Msk));
            break;

#if NRF_SPU_HAS_BELLS
#if NRF_SPU_HAS_DOMAIN
        case NRF_SPU_FEATURE_BELLS_BELL:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELL_BELL_COUNT);
            p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex] =
                ((p_reg->FEATURE.BELLS.DOMAIN[index].BELL[subindex] &
                  ~SPU_FEATURE_BELLS_DOMAIN_BELL_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_BELLS_DOMAIN_BELL_OWNERID_Pos) &
                  SPU_FEATURE_BELLS_DOMAIN_BELL_OWNERID_Msk));
            break;
#else
        case NRF_SPU_FEATURE_BELLS_TASKS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_TASKS_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].TASKS[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_TASKS_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_BELLS_PROCESSOR_TASKS_OWNERID_Pos) &
                  SPU_FEATURE_BELLS_PROCESSOR_TASKS_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_BELLS_EVENTS:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_EVENTS_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].EVENTS[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_EVENTS_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_BELLS_PROCESSOR_EVENTS_OWNERID_Pos) &
                  SPU_FEATURE_BELLS_PROCESSOR_EVENTS_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_BELLS_INTERRUPT:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_BELLS_INTERRUPT_COUNT);
            p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex] =
                ((p_reg->FEATURE.BELLS.PROCESSOR[index].INTERRUPT[subindex] &
                  ~SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_OWNERID_Pos) &
                  SPU_FEATURE_BELLS_PROCESSOR_INTERRUPT_OWNERID_Msk));
            break;
#endif // NRF_SPU_HAS_DOMAIN
#endif // NRF_SPU_HAS_BELLS

#if NRF_SPU_HAS_TDD
        case NRF_SPU_FEATURE_TDD:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_TDD_COUNT);
            p_reg->FEATURE.TDD[index] =
                ((p_reg->FEATURE.TDD[index] &
                  ~SPU_FEATURE_TDD_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_TDD_OWNERID_Pos) &
                  SPU_FEATURE_TDD_OWNERID_Msk));
            break;
#endif // NRF_SPU_HAS_TDD

#if NRF_SPU_HAS_MRAMC
        case NRF_SPU_FEATURE_MRAMC_WAITSTATES:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].WAITSTATES =
                ((p_reg->FEATURE.MRAMC[index].WAITSTATES &
                  ~SPU_FEATURE_MRAMC_WAITSTATES_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_MRAMC_WAITSTATES_OWNERID_Pos) &
                  SPU_FEATURE_MRAMC_WAITSTATES_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_MRAMC_AUTODPOWERDOWN:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN =
                ((p_reg->FEATURE.MRAMC[index].AUTODPOWERDOWN &
                  ~SPU_FEATURE_MRAMC_AUTODPOWERDOWN_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_MRAMC_AUTODPOWERDOWN_OWNERID_Pos) &
                  SPU_FEATURE_MRAMC_AUTODPOWERDOWN_OWNERID_Msk));
            break;

        case NRF_SPU_FEATURE_MRAMC_READY:
            NRFX_ASSERT(index < NRF_SPU_FEATURE_MRAMC_COUNT);
            p_reg->FEATURE.MRAMC[index].READY =
                ((p_reg->FEATURE.MRAMC[index].READY &
                  ~SPU_FEATURE_MRAMC_READY_OWNERID_Msk) |
                 ((owner_id
                   << SPU_FEATURE_MRAMC_READY_OWNERID_Pos) &
                  SPU_FEATURE_MRAMC_READY_OWNERID_Msk));
            break;
#endif // NRF_SPU_HAS_MRAMC

        default:
            NRFX_ASSERT(0);
            break;
    }
}
#endif // NRF_SPU_HAS_OWNERSHIP

#endif // NRF_SPU_HAS_FEATURE

#endif // NRF_SPU_HAS_PERIPHERAL_ACCESS

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_SPU_H__
