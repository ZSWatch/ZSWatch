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

#ifndef NRF_MPC_H_
#define NRF_MPC_H_

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_mpc_hal MPC HAL
 * @{
 * @ingroup nrf_mpc
 * @brief   Hardware access layer for managing the Memory Privilege Controller (MPC)
 *          peripheral.
 */

#if defined(MPC_RTCHOKE_WRITEACCESS_ENABLE0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RTCHOKE functionality is present. */
#define NRF_MPC_HAS_RTCHOKE 1
#else
#define NRF_MPC_HAS_RTCHOKE 0
#endif

#if defined(MPC_OVERRIDE_CONFIG_SECDOMENABLE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SECDOM functionality is present. */
#define NRF_MPC_HAS_SECDOM 1
#else
#define NRF_MPC_HAS_SECDOM 0
#endif

#if defined(MPC_GLOBALSLAVE_MASTERPORT_CONNECTION0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether configuration of global slave master port connection is present. */
#define NRF_MPC_HAS_GLOBALSLAVE 1
#else
#define NRF_MPC_HAS_GLOBALSLAVE 0
#endif

#if defined(MPC_OVERRIDE_OFFSET_OFFSET_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether configuration of offset for override region is present. */
#define NRF_MPC_HAS_OVERRIDE_OFFSET 1
#else
#define NRF_MPC_HAS_OVERRIDE_OFFSET 0
#endif

#if defined(MPC_OVERRIDE_OWNER_OWNERID_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether configuration of owner for override region is present. */
#define NRF_MPC_HAS_OVERRIDE_OWNERID 1
#else
#define NRF_MPC_HAS_OVERRIDE_OWNERID 0
#endif

#if defined(MPC_OVERRIDE_MASTERPORT_ENABLE0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether configuration of master port for override region is present. */
#define NRF_MPC_HAS_OVERRIDE_MASTERPORT 1
#else
#define NRF_MPC_HAS_OVERRIDE_MASTERPORT 0
#endif

#if defined(MPC_OVERRIDE_CONFIG_SLAVENUMBER_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether configuration of slave for override region is present. */
#define NRF_MPC_HAS_OVERRIDE_SLAVENUMBER 1
#else
#define NRF_MPC_HAS_OVERRIDE_SLAVENUMBER 0
#endif

#if defined(MPC_OVERRIDE_CONFIG_SECUREMASK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether secure bit check for override region indication is present. */
#define NRF_MPC_HAS_OVERRIDE_SECUREMASK 1
#else
#define NRF_MPC_HAS_OVERRIDE_SECUREMASK 0
#endif

#if defined(MPC_REGION_MaxCount) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether region configuration is present. */
#define NRF_MPC_HAS_REGION 1
#else
#define NRF_MPC_HAS_REGION 0
#endif

#if NRF_MPC_HAS_REGION
/** @brief Number of regions. */
#define NRF_MPC_REGION_COUNT   MPC_REGION_MaxCount
#endif

/** @brief Number of override regions. */
#define NRF_MPC_OVERRIDE_COUNT MPC_OVERRIDE_MaxCount

#if NRF_MPC_HAS_REGION
/** @brief Number of master ports. */
#define NRF_MPC_MASTER_PORTS_COUNT MPC_MASTER_PORTS_MaxCount
#endif

#if NRF_MPC_HAS_RTCHOKE
/** @brief Number of Real Time Choke slaves. */
#define NRF_MPC_RTCHOKE_COUNT  MPC_RTCHOKE_DELAY_MaxCount
#endif

/** @brief MPC events. */
typedef enum
{
    NRF_MPC_EVENT_MEMACCERR = offsetof(NRF_MPC_Type, EVENTS_MEMACCERR), /**< Memory access error. */
} nrf_mpc_event_t;

/** @brief MPC interrupts. */
typedef enum
{
    NRF_MPC_INT_MEMACCERR_MASK = MPC_INTENSET_MEMACCERR_Msk, /**< Interrupt on MEMACCERR event. */
} nrf_mpc_int_mask_t;

/** @brief Error sources. */
typedef enum
{
    NRF_MPC_ERRORSOURCE_SLAVE = MPC_MEMACCERR_INFO_ERRORSOURCE_Slave, /**< Error was triggered by an AXI slave. */
    NRF_MPC_ERRORSOURCE_MPC   = MPC_MEMACCERR_INFO_ERRORSOURCE_MPC,   /**< Error was triggered by MCP module. */
} nrf_mpc_errorsource_t;

/**
 * @brief Permissions mask.
 *
 * @note This enum may be used for both permission settings and permission settings mask.
 */
typedef enum
{
    NRF_MPC_PERM_READ_MASK    = MPC_OVERRIDE_PERM_READ_Msk,    /**< Read access. */
    NRF_MPC_PERM_WRITE_MASK   = MPC_OVERRIDE_PERM_WRITE_Msk,   /**< Write access. */
    NRF_MPC_PERM_EXECUTE_MASK = MPC_OVERRIDE_PERM_EXECUTE_Msk, /**< Software execute. */
    NRF_MPC_PERM_SECURE_MASK  = MPC_OVERRIDE_PERM_SECATTR_Msk, /**< Security mapping. */
} nrf_mpc_permission_mask_t;

#if NRF_MPC_HAS_REGION
/** @brief Masterport mask. */
typedef enum
{
    NRF_MPC_MASTERPORT_0_MASK  = MPC_REGION_MASTERPORT_ENABLE0_Msk,  /**< Enable master port 0. */
    NRF_MPC_MASTERPORT_1_MASK  = MPC_REGION_MASTERPORT_ENABLE1_Msk,  /**< Enable master port 1. */
    NRF_MPC_MASTERPORT_2_MASK  = MPC_REGION_MASTERPORT_ENABLE2_Msk,  /**< Enable master port 2. */
    NRF_MPC_MASTERPORT_3_MASK  = MPC_REGION_MASTERPORT_ENABLE3_Msk,  /**< Enable master port 3. */
    NRF_MPC_MASTERPORT_4_MASK  = MPC_REGION_MASTERPORT_ENABLE4_Msk,  /**< Enable master port 4. */
    NRF_MPC_MASTERPORT_5_MASK  = MPC_REGION_MASTERPORT_ENABLE5_Msk,  /**< Enable master port 5. */
    NRF_MPC_MASTERPORT_6_MASK  = MPC_REGION_MASTERPORT_ENABLE6_Msk,  /**< Enable master port 6. */
    NRF_MPC_MASTERPORT_7_MASK  = MPC_REGION_MASTERPORT_ENABLE7_Msk,  /**< Enable master port 7. */
#if (NRF_MPC_MASTER_PORTS_COUNT > 8)
    NRF_MPC_MASTERPORT_8_MASK  = MPC_REGION_MASTERPORT_ENABLE8_Msk,  /**< Enable master port 8. */
    NRF_MPC_MASTERPORT_9_MASK  = MPC_REGION_MASTERPORT_ENABLE9_Msk,  /**< Enable master port 9. */
    NRF_MPC_MASTERPORT_10_MASK = MPC_REGION_MASTERPORT_ENABLE10_Msk, /**< Enable master port 10. */
    NRF_MPC_MASTERPORT_11_MASK = MPC_REGION_MASTERPORT_ENABLE11_Msk, /**< Enable master port 11. */
#endif
#if (NRF_MPC_MASTER_PORTS_COUNT > 12)
    NRF_MPC_MASTERPORT_12_MASK = MPC_REGION_MASTERPORT_ENABLE12_Msk, /**< Enable master port 12. */
    NRF_MPC_MASTERPORT_13_MASK = MPC_REGION_MASTERPORT_ENABLE13_Msk, /**< Enable master port 13. */
    NRF_MPC_MASTERPORT_14_MASK = MPC_REGION_MASTERPORT_ENABLE14_Msk, /**< Enable master port 14. */
#endif
#if (NRF_MPC_MASTER_PORTS_COUNT > 15)
    NRF_MPC_MASTERPORT_15_MASK = MPC_REGION_MASTERPORT_ENABLE15_Msk, /**< Enable master port 15. */
    NRF_MPC_MASTERPORT_16_MASK = MPC_REGION_MASTERPORT_ENABLE16_Msk, /**< Enable master port 16. */
    NRF_MPC_MASTERPORT_17_MASK = MPC_REGION_MASTERPORT_ENABLE17_Msk, /**< Enable master port 17. */
    NRF_MPC_MASTERPORT_18_MASK = MPC_REGION_MASTERPORT_ENABLE18_Msk, /**< Enable master port 18. */
    NRF_MPC_MASTERPORT_19_MASK = MPC_REGION_MASTERPORT_ENABLE19_Msk, /**< Enable master port 19. */
    NRF_MPC_MASTERPORT_20_MASK = MPC_REGION_MASTERPORT_ENABLE20_Msk, /**< Enable master port 20. */
    NRF_MPC_MASTERPORT_21_MASK = MPC_REGION_MASTERPORT_ENABLE21_Msk, /**< Enable master port 21. */
    NRF_MPC_MASTERPORT_22_MASK = MPC_REGION_MASTERPORT_ENABLE22_Msk, /**< Enable master port 22. */
    NRF_MPC_MASTERPORT_23_MASK = MPC_REGION_MASTERPORT_ENABLE23_Msk, /**< Enable master port 23. */
    NRF_MPC_MASTERPORT_24_MASK = MPC_REGION_MASTERPORT_ENABLE24_Msk, /**< Enable master port 24. */
    NRF_MPC_MASTERPORT_25_MASK = MPC_REGION_MASTERPORT_ENABLE25_Msk, /**< Enable master port 25. */
    NRF_MPC_MASTERPORT_26_MASK = MPC_REGION_MASTERPORT_ENABLE26_Msk, /**< Enable master port 26. */
    NRF_MPC_MASTERPORT_27_MASK = MPC_REGION_MASTERPORT_ENABLE27_Msk, /**< Enable master port 27. */
    NRF_MPC_MASTERPORT_28_MASK = MPC_REGION_MASTERPORT_ENABLE28_Msk, /**< Enable master port 28. */
    NRF_MPC_MASTERPORT_29_MASK = MPC_REGION_MASTERPORT_ENABLE29_Msk, /**< Enable master port 29. */
    NRF_MPC_MASTERPORT_30_MASK = MPC_REGION_MASTERPORT_ENABLE30_Msk, /**< Enable master port 30. */
    NRF_MPC_MASTERPORT_31_MASK = MPC_REGION_MASTERPORT_ENABLE31_Msk, /**< Enable master port 31. */
#endif
} nrf_mpc_masterport_mask_t;

/** @brief Region configuration. */
typedef struct
{
    uint8_t     slave_number; /**< Target slave number. */
    bool        lock;         /**< Lock region until next reset. */
    bool        enable;       /**< Enable region. */
    nrf_owner_t owner;        /**< Owner identifier. */
    uint32_t    permissions;  /**< Permissions. */
} nrf_mpc_region_config_t;
#endif // NRF_MPC_HAS_REGION

/** @brief Override region configuration. */
typedef struct
{
    uint8_t slave_number;  /**< Target slave number. */
    bool    lock;          /**< Lock region until next reset. */
    bool    enable;        /**< Enable region. */
    bool    secdom_enable; /**< Enable overriding of secure domain permissions. */
    bool    secure_mask;   /**< Secure mask. Read only.
                            *   If set, the bit 28 of the transaction is ignored while address matching. */
} nrf_mpc_override_config_t;

/**
 * @brief Function for retrieving the state of the specified MPC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_mpc_event_check(NRF_MPC_Type const * p_reg, nrf_mpc_event_t event);

/**
 * @brief Function for clearing the specified MPC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_mpc_event_clear(NRF_MPC_Type * p_reg, nrf_mpc_event_t event);

/**
 * @brief Function for getting the address of the specified MPC event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to get the address of.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_event_address_get(NRF_MPC_Type const * p_reg,
                                                     nrf_mpc_event_t      event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_mpc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_mpc_int_enable(NRF_MPC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_mpc_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_int_enable_check(NRF_MPC_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_mpc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_mpc_int_disable(NRF_MPC_Type * p_reg, uint32_t mask);

#if NRF_MPC_HAS_REGION
/**
 * @brief Function for setting configuration of the region.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] index    Region index.
 * @param[in] p_config Pointer to the structure of the region configuration parameters.
 */
NRF_STATIC_INLINE void nrf_mpc_region_config_set(NRF_MPC_Type *                  p_reg,
                                                 uint8_t                         index,
                                                 nrf_mpc_region_config_t const * p_config);

/**
 * @brief Function for getting configuration of the region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Region index.
 *
 * @return Structure with configuration of the region.
 */
NRF_STATIC_INLINE nrf_mpc_region_config_t nrf_mpc_region_config_get(NRF_MPC_Type const * p_reg,
                                                                    uint8_t              index);

/**
 * @brief Function for setting start address of the region.
 *
 * @note Address must be on a 4kB memory boundary.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] index   Region index.
 * @param[in] address Address to be set.
 */
NRF_STATIC_INLINE void nrf_mpc_region_startaddr_set(NRF_MPC_Type * p_reg,
                                                    uint8_t        index,
                                                    uint32_t       address);

/**
 * @brief Function for getting start address of the region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Region index.
 *
 * @return Start address of the region.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_region_startaddr_get(NRF_MPC_Type const * p_reg, uint8_t index);

/**
 * @brief Function for setting address mask of the region.
 *
 * @note Mask must be on a 4kB memory boundary.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] index   Region index.
 * @param[in] address Address to be set.
 */
NRF_STATIC_INLINE void nrf_mpc_region_addrmask_set(NRF_MPC_Type * p_reg,
                                                   uint8_t        index,
                                                   uint32_t       address);

/**
 * @brief Function for getting address mask of the region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Region index.
 *
 * @return Address mask of the region.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_region_addrmask_get(NRF_MPC_Type const * p_reg, uint8_t index);

/**
 * @brief Function for enabling the specified master ports of the region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Region index.
 * @param[in] mask  Mask of master ports to be enabled,
 *                  constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_mpc_region_masterport_set(NRF_MPC_Type * p_reg,
                                                     uint8_t        index,
                                                     uint32_t       mask);

/**
 * @brief Function for getting enabled master ports of the region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Region index.
 *
 * @return Mask of enabled master ports,
 *         constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_region_masterport_get(NRF_MPC_Type const * p_reg, uint8_t index);
#endif // NRF_MPC_HAS_REGION

/**
 * @brief Function for setting configuration of the override region.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] index    Override region index.
 * @param[in] p_config Pointer to the structure of the override region configuration parameters.
 */
NRF_STATIC_INLINE void nrf_mpc_override_config_set(NRF_MPC_Type *                    p_reg,
                                                   uint8_t                           index,
                                                   nrf_mpc_override_config_t const * p_config);

/**
 * @brief Function for getting configuration of the override region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 *
 * @return Structure with configuration of the override region.
 */
NRF_STATIC_INLINE nrf_mpc_override_config_t nrf_mpc_override_config_get(NRF_MPC_Type const * p_reg,
                                                                        uint8_t              index);

/**
 * @brief Function for setting start address of the override region.
 *
 * @note Address must be on a 4kB memory boundary.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] index   Override region index.
 * @param[in] address Address to be set.
 */
NRF_STATIC_INLINE void nrf_mpc_override_startaddr_set(NRF_MPC_Type * p_reg,
                                                      uint8_t        index,
                                                      uint32_t       address);

/**
 * @brief Function for getting start address of the override region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 *
 * @return Start address of the override region.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_override_startaddr_get(NRF_MPC_Type const * p_reg,
                                                          uint8_t              index);

/**
 * @brief Function for setting end address of the override region.
 *
 * @note Address must be on a 4kB memory boundary.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] index   Override region index.
 * @param[in] address Address to be set.
 */
NRF_STATIC_INLINE void nrf_mpc_override_endaddr_set(NRF_MPC_Type * p_reg,
                                                    uint8_t        index,
                                                    uint32_t       address);

/**
 * @brief Function for getting end address of the override region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 *
 * @return End address of the override region.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_override_endaddr_get(NRF_MPC_Type const * p_reg,
                                                        uint8_t              index);

#if NRF_MPC_HAS_OVERRIDE_OFFSET
/**
 * @brief Function for setting offset of the override region.
 *
 * @note Offset will be left shifted before applying, creating a 33-bit signed integer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Override region index.
 * @param[in] offset Address offset value divided by 2.
 */
NRF_STATIC_INLINE void nrf_mpc_override_offset_set(NRF_MPC_Type * p_reg,
                                                   uint8_t        index,
                                                   uint32_t       offset);

/**
 * @brief Function for getting offset of the override region.
 *
 * @note Offset is left shifted before applying, creating a 33-bit signed integer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 *
 * @return Address offset value divided by 2.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_override_offset_get(NRF_MPC_Type const * p_reg,
                                                       uint8_t              index);
#endif

/**
 * @brief Function for setting permission settings for the override region.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] index       Override region index.
 * @param[in] permissions Mask of permissions to be set,
 *                        constructed from @ref nrf_mpc_permission_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_mpc_override_perm_set(NRF_MPC_Type * p_reg,
                                                 uint8_t        index,
                                                 uint32_t       permissions);

/**
 * @brief Function for getting permission settings of the override region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 *
 * @return Mask of permissions, constructed from @ref nrf_mpc_permission_mask_t enumerator values.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_override_perm_get(NRF_MPC_Type const * p_reg,
                                                     uint8_t              index);

/**
 * @brief Function for setting permission settings mask for the override region.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] index       Override region index.
 * @param[in] permissions Mask of permissions settings mask to be set,
 *                        constructed from @ref nrf_mpc_permission_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_mpc_override_permmask_set(NRF_MPC_Type * p_reg,
                                                     uint8_t        index,
                                                     uint32_t       permissions);

/**
 * @brief Function for getting permission settings mask of the override region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 *
 * @return Mask of permissions settings mask,
 *         constructed from @ref nrf_mpc_permission_mask_t enumerator values.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_override_permmask_get(NRF_MPC_Type const * p_reg,
                                                         uint8_t              index);

#if NRF_MPC_HAS_OVERRIDE_OWNERID
/**
 * @brief Function for setting owner ID for the override region.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] index    Override region index.
 * @param[in] owner_id Owner ID to be set.
 */
NRF_STATIC_INLINE void nrf_mpc_override_ownerid_set(NRF_MPC_Type * p_reg,
                                                    uint8_t        index,
                                                    nrf_owner_t    owner_id);

/**
 * @brief Function for getting owner ID of the override region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 *
 * @return Owner ID of the overridde region.
 */
NRF_STATIC_INLINE nrf_owner_t nrf_mpc_override_ownerid_get(NRF_MPC_Type const * p_reg,
                                                           uint8_t              index);
#endif

#if NRF_MPC_HAS_OVERRIDE_MASTERPORT
/**
 * @brief Function for enabling the specified master ports of the override region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 * @param[in] mask  Mask of master ports to be enabled,
 *                  constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_mpc_override_masterport_set(NRF_MPC_Type * p_reg,
                                                       uint8_t        index,
                                                       uint32_t       mask);

/**
 * @brief Function for getting enabled master ports of the override region.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Override region index.
 *
 * @return Mask of enabled master ports,
 *         constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_override_masterport_get(NRF_MPC_Type const * p_reg,
                                                           uint8_t              index);
#endif

/**
 * @brief Function for getting the memory address of memory access error.
 *
 * @note Register content will not be changed as long as MEMACCERR event is active.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Target address for the errroneous access.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_memaccerr_address_get(NRF_MPC_Type const * p_reg);

#if NRF_MPC_HAS_OVERRIDE_OWNERID
/**
 * @brief Function for getting the owner identifier of the transaction that triggered memory access error.
 *
 * @note Register content will not be changed as long as MEMACCERR event is active.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Owner identifier of the errorneous access.
 */
NRF_STATIC_INLINE nrf_owner_t nrf_mpc_memaccerr_info_ownerid_get(NRF_MPC_Type const * p_reg);
#endif

#if NRF_MPC_HAS_OVERRIDE_MASTERPORT
/**
 * @brief Function for getting the master port of the transaction that triggered memory access error.
 *
 * @note Register content will not be changed as long as MEMACCERR event is active.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Master port where errorneous access is detected.
 */
NRF_STATIC_INLINE uint8_t nrf_mpc_memaccerr_info_masterport_get(NRF_MPC_Type const * p_reg);
#endif

/**
 * @brief Function for getting the permissions of the transaction that triggered memory access error.
 *
 * @note Register content will not be changed as long as MEMACCERR event is active.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Permission settings of the errorneous access.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_memaccerr_info_perm_get(NRF_MPC_Type const * p_reg);

/**
 * @brief Function for getting the source of the transaction that triggered memory access error.
 *
 * @note Register content will not be changed as long as MEMACCERR event is active.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Source of the errorneous access.
 */
NRF_STATIC_INLINE nrf_mpc_errorsource_t
nrf_mpc_memaccerr_info_errorsource_get(NRF_MPC_Type const * p_reg);

#if NRF_MPC_HAS_GLOBALSLAVE
/**
 * @brief Function for enabling the specified master ports connection to global slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of master ports to be connected,
 *                  constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_mpc_globalslave_masterport_set(NRF_MPC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for getting enabled master ports connection to global slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask of master ports connected,
 *         constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_globalslave_masterport_get(NRF_MPC_Type const * p_reg);

/**
 * @brief Function for enabling the global slave registers lock.
 *
 * @note When global slave registers is enabled, modifying the global slave configuration
 *       is not possible.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_mpc_globalslave_lock_enable(NRF_MPC_Type * p_reg);

/**
 * @brief Function for getting the status of the global slave registers lock.
 *
 * @note When global slave registers is enabled, modifying the global slave configuration
 *       is not possible.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return True if global slave registers are locked, false otherwise.
 */
NRF_STATIC_INLINE bool nrf_mpc_globalslave_lock_check(NRF_MPC_Type const * p_reg);
#endif

#if NRF_MPC_HAS_RTCHOKE
/**
 * @brief Function for enabling the AXI Write Address Channel Real Time Choke for specified master ports.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of master ports to have the Write Real Time Choke enabled,
 *                  constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_mpc_rtchoke_writeaccess_set(NRF_MPC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for getting master ports with enabled AXI Write Address Channel Real Time Choke.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask of master ports with the Write Real Time Choke enabled,
 *         constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_rtchoke_writeaccess_get(NRF_MPC_Type const * p_reg);

/**
 * @brief Function for enabling the AXI Read Address Channel Real Time Choke for specified master ports.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of master ports to have the Read Real Time Choke enabled,
 *                  constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_mpc_rtchoke_readaccess_set(NRF_MPC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for getting master ports with enabled AXI Read Address Channel Real Time Choke.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask of master ports with the Read Real Time Choke enabled,
 *         constructed from @ref nrf_mpc_masterport_mask_t enumerator values.
 */
NRF_STATIC_INLINE uint32_t nrf_mpc_rtchoke_readaccess_get(NRF_MPC_Type const * p_reg);

/**
 * @brief Function for setting the Real Time Choke delay for the specified slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] slave Slave number.
 * @param[in] delay Delay value to be set.
 */
NRF_STATIC_INLINE void nrf_mpc_rtchoke_delay_set(NRF_MPC_Type * p_reg,
                                                 uint8_t        slave,
                                                 uint8_t        delay);

/**
 * @brief Function for getting the Real Time Choke delay for the specified slave.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] slave Slave number.
 *
 * @return Delay value for slave.
 */
NRF_STATIC_INLINE uint8_t nrf_mpc_rtchoke_delay_get(NRF_MPC_Type const * p_reg, uint8_t slave);

#endif // NRF_MPC_HAS_RTCHOKE

#ifndef NRF_DECLARE_ONLY
NRF_STATIC_INLINE bool nrf_mpc_event_check(NRF_MPC_Type const * p_reg, nrf_mpc_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_mpc_event_clear(NRF_MPC_Type * p_reg, nrf_mpc_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE uint32_t nrf_mpc_event_address_get(NRF_MPC_Type const * p_reg,
                                                     nrf_mpc_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_mpc_int_enable(NRF_MPC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_int_enable_check(NRF_MPC_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE void nrf_mpc_int_disable(NRF_MPC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

#if NRF_MPC_HAS_REGION
NRF_STATIC_INLINE void nrf_mpc_region_config_set(NRF_MPC_Type *                  p_reg,
                                                 uint8_t                         index,
                                                 nrf_mpc_region_config_t const * p_config)
{
    NRFX_ASSERT(index < NRF_MPC_REGION_COUNT);
    NRFX_ASSERT(p_config != NULL);

    p_reg->REGION[index].CONFIG = (((p_config->slave_number <<
                                     MPC_REGION_CONFIG_SLAVENUMBER_Pos) &
                                    MPC_REGION_CONFIG_SLAVENUMBER_Msk) |
                                   ((p_config->lock ? MPC_REGION_CONFIG_LOCK_Locked :
                                     MPC_REGION_CONFIG_LOCK_Unlocked) <<
                                    MPC_REGION_CONFIG_LOCK_Pos) |
                                   ((p_config->enable ? MPC_REGION_CONFIG_ENABLE_Enabled :
                                     MPC_REGION_CONFIG_ENABLE_Disabled) <<
                                    MPC_REGION_CONFIG_ENABLE_Pos) |
                                   ((p_config->permissions << MPC_REGION_CONFIG_READ_Pos) &
                                    (MPC_REGION_CONFIG_READ_Msk | MPC_REGION_CONFIG_WRITE_Msk |
                                     MPC_REGION_CONFIG_EXECUTE_Msk |
                                     MPC_REGION_CONFIG_SECATTR_Msk)) |
                                   ((p_config->owner <<
                                     MPC_REGION_CONFIG_OWNERID_Pos) &
                                    MPC_REGION_CONFIG_OWNERID_Msk));
}

NRF_STATIC_INLINE nrf_mpc_region_config_t nrf_mpc_region_config_get(NRF_MPC_Type const * p_reg,
                                                                    uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_REGION_COUNT);

    nrf_mpc_region_config_t ret;

    ret.slave_number = (p_reg->REGION[index].CONFIG & MPC_REGION_CONFIG_SLAVENUMBER_Msk)
                        >> MPC_REGION_CONFIG_SLAVENUMBER_Pos;

    ret.lock = ((p_reg->REGION[index].CONFIG & MPC_REGION_CONFIG_LOCK_Msk)
                >> MPC_REGION_CONFIG_LOCK_Pos) ==  MPC_REGION_CONFIG_LOCK_Locked;

    ret.enable = ((p_reg->REGION[index].CONFIG & MPC_REGION_CONFIG_ENABLE_Msk)
                  >> MPC_REGION_CONFIG_ENABLE_Pos) == MPC_REGION_CONFIG_ENABLE_Enabled;

    ret.permissions = (p_reg->REGION[index].CONFIG &
                       (MPC_REGION_CONFIG_READ_Msk | MPC_REGION_CONFIG_WRITE_Msk |
                        MPC_REGION_CONFIG_EXECUTE_Msk | MPC_REGION_CONFIG_SECATTR_Msk))
                      >> MPC_REGION_CONFIG_READ_Pos;

    ret.owner = (nrf_owner_t)((p_reg->REGION[index].CONFIG & MPC_REGION_CONFIG_OWNERID_Msk)
                              >> MPC_REGION_CONFIG_OWNERID_Pos);

    return ret;
}

NRF_STATIC_INLINE void nrf_mpc_region_startaddr_set(NRF_MPC_Type * p_reg,
                                                    uint8_t        index,
                                                    uint32_t       address)
{
    NRFX_ASSERT(index < NRF_MPC_REGION_COUNT);
    NRFX_ASSERT((address & 0xFFFUL) == 0);

    p_reg->REGION[index].STARTADDR = address;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_region_startaddr_get(NRF_MPC_Type const * p_reg, uint8_t index)
{
    NRFX_ASSERT(index < NRF_MPC_REGION_COUNT);

    return p_reg->REGION[index].STARTADDR;
}

NRF_STATIC_INLINE void nrf_mpc_region_addrmask_set(NRF_MPC_Type * p_reg,
                                                   uint8_t        index,
                                                   uint32_t       address)
{
    NRFX_ASSERT(index < NRF_MPC_REGION_COUNT);
    NRFX_ASSERT((address & 0xFFFUL) == 0);

    p_reg->REGION[index].ADDRMASK = address;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_region_addrmask_get(NRF_MPC_Type const * p_reg, uint8_t index)
{
    NRFX_ASSERT(index < NRF_MPC_REGION_COUNT);

    return p_reg->REGION[index].ADDRMASK;
}

NRF_STATIC_INLINE void nrf_mpc_region_masterport_set(NRF_MPC_Type * p_reg,
                                                     uint8_t        index,
                                                     uint32_t       mask)
{
    NRFX_ASSERT(index < NRF_MPC_REGION_COUNT);

    p_reg->REGION[index].MASTERPORT = mask;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_region_masterport_get(NRF_MPC_Type const * p_reg, uint8_t index)
{
    NRFX_ASSERT(index < NRF_MPC_REGION_COUNT);

    return p_reg->REGION[index].MASTERPORT;
}
#endif // NRF_MPC_HAS_REGION

NRF_STATIC_INLINE void nrf_mpc_override_config_set(NRF_MPC_Type *                    p_reg,
                                                   uint8_t                           index,
                                                   nrf_mpc_override_config_t const * p_config)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);
    NRFX_ASSERT(p_config != NULL);

    p_reg->OVERRIDE[index].CONFIG = (((p_config->lock ? MPC_OVERRIDE_CONFIG_LOCK_Locked :
                                       MPC_OVERRIDE_CONFIG_LOCK_Unlocked) <<
                                      MPC_OVERRIDE_CONFIG_LOCK_Pos) |
#if NRF_MPC_HAS_OVERRIDE_SLAVENUMBER
                                     ((p_config->slave_number <<
                                       MPC_OVERRIDE_CONFIG_SLAVENUMBER_Pos) &
                                      MPC_OVERRIDE_CONFIG_SLAVENUMBER_Msk) |
#endif
#if NRF_MPC_HAS_SECDOM
                                     ((p_config->secdom_enable ?
                                       MPC_OVERRIDE_CONFIG_SECDOMENABLE_Enabled :
                                       MPC_OVERRIDE_CONFIG_SECDOMENABLE_Disabled) <<
                                      MPC_OVERRIDE_CONFIG_SECDOMENABLE_Pos) |
#endif
                                     ((p_config->enable ? MPC_OVERRIDE_CONFIG_ENABLE_Enabled :
                                       MPC_OVERRIDE_CONFIG_ENABLE_Disabled) <<
                                      MPC_OVERRIDE_CONFIG_ENABLE_Pos));
}

NRF_STATIC_INLINE nrf_mpc_override_config_t nrf_mpc_override_config_get(NRF_MPC_Type const * p_reg,
                                                                        uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    nrf_mpc_override_config_t ret;

#if NRF_MPC_HAS_OVERRIDE_SLAVENUMBER
    ret.slave_number = (p_reg->OVERRIDE[index].CONFIG & MPC_OVERRIDE_CONFIG_SLAVENUMBER_Msk)
                        >> MPC_OVERRIDE_CONFIG_SLAVENUMBER_Pos;
#endif

    ret.lock = ((p_reg->OVERRIDE[index].CONFIG & MPC_OVERRIDE_CONFIG_LOCK_Msk)
                >> MPC_OVERRIDE_CONFIG_LOCK_Pos) == MPC_OVERRIDE_CONFIG_LOCK_Locked;

    ret.enable = ((p_reg->OVERRIDE[index].CONFIG & MPC_OVERRIDE_CONFIG_ENABLE_Msk)
                  >> MPC_OVERRIDE_CONFIG_ENABLE_Pos) == MPC_OVERRIDE_CONFIG_ENABLE_Enabled;
#if NRF_MPC_HAS_SECDOM
    ret.secdom_enable = ((p_reg->OVERRIDE[index].CONFIG & MPC_OVERRIDE_CONFIG_SECDOMENABLE_Msk)
                         >> MPC_OVERRIDE_CONFIG_SECDOMENABLE_Pos)
                        == MPC_OVERRIDE_CONFIG_SECDOMENABLE_Enabled;
#endif
#if NRF_MPC_HAS_OVERRIDE_SECUREMASK
    ret.secure_mask = ((p_reg->OVERRIDE[index].CONFIG & MPC_OVERRIDE_CONFIG_SECUREMASK_Msk)
                       >> MPC_OVERRIDE_CONFIG_SECUREMASK_Pos) ==
                      MPC_OVERRIDE_CONFIG_SECUREMASK_Enabled;
#endif

    return ret;
}

NRF_STATIC_INLINE void nrf_mpc_override_startaddr_set(NRF_MPC_Type * p_reg,
                                                      uint8_t        index,
                                                      uint32_t       address)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);
    NRFX_ASSERT((address & 0xFFFUL) == 0);

    p_reg->OVERRIDE[index].STARTADDR = address;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_override_startaddr_get(NRF_MPC_Type const * p_reg,
                                                          uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    return p_reg->OVERRIDE[index].STARTADDR;
}

NRF_STATIC_INLINE void nrf_mpc_override_endaddr_set(NRF_MPC_Type * p_reg,
                                                    uint8_t        index,
                                                    uint32_t       address)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);
    NRFX_ASSERT((address & 0xFFFUL) == 0);

    p_reg->OVERRIDE[index].ENDADDR = address;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_override_endaddr_get(NRF_MPC_Type const * p_reg,
                                                        uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    return p_reg->OVERRIDE[index].ENDADDR;
}

#if NRF_MPC_HAS_OVERRIDE_OFFSET
NRF_STATIC_INLINE void nrf_mpc_override_offset_set(NRF_MPC_Type * p_reg,
                                                   uint8_t        index,
                                                   uint32_t       offset)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);
    NRFX_ASSERT((offset & 0x3FFUL) == 0);

    p_reg->OVERRIDE[index].OFFSET = (int32_t)offset;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_override_offset_get(NRF_MPC_Type const * p_reg,
                                                       uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    return (uint32_t)p_reg->OVERRIDE[index].OFFSET;
}
#endif

NRF_STATIC_INLINE void nrf_mpc_override_perm_set(NRF_MPC_Type * p_reg,
                                                 uint8_t        index,
                                                 uint32_t       permissions)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    p_reg->OVERRIDE[index].PERM = permissions;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_override_perm_get(NRF_MPC_Type const * p_reg,
                                                     uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    return p_reg->OVERRIDE[index].PERM;
}

NRF_STATIC_INLINE void nrf_mpc_override_permmask_set(NRF_MPC_Type * p_reg,
                                                     uint8_t        index,
                                                     uint32_t       permissions)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    p_reg->OVERRIDE[index].PERMMASK = permissions;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_override_permmask_get(NRF_MPC_Type const * p_reg,
                                                         uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    return p_reg->OVERRIDE[index].PERMMASK;
}

#if NRF_MPC_HAS_OVERRIDE_OWNERID
NRF_STATIC_INLINE void nrf_mpc_override_ownerid_set(NRF_MPC_Type * p_reg,
                                                    uint8_t        index,
                                                    nrf_owner_t    owner_id)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    p_reg->OVERRIDE[index].OWNER = (owner_id << MPC_OVERRIDE_OWNER_OWNERID_Pos) &
                                   MPC_OVERRIDE_OWNER_OWNERID_Msk;
}

NRF_STATIC_INLINE nrf_owner_t nrf_mpc_override_ownerid_get(NRF_MPC_Type const * p_reg,
                                                           uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    return (nrf_owner_t)p_reg->OVERRIDE[index].OWNER;
}
#endif

#if NRF_MPC_HAS_OVERRIDE_MASTERPORT
NRF_STATIC_INLINE void nrf_mpc_override_masterport_set(NRF_MPC_Type * p_reg,
                                                       uint8_t        index,
                                                       uint32_t       mask)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    p_reg->OVERRIDE[index].MASTERPORT = mask;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_override_masterport_get(NRF_MPC_Type const * p_reg,
                                                           uint8_t              index)
{
    NRFX_ASSERT(index < NRF_MPC_OVERRIDE_COUNT);

    return p_reg->OVERRIDE[index].MASTERPORT;
}
#endif

NRF_STATIC_INLINE uint32_t nrf_mpc_memaccerr_address_get(NRF_MPC_Type const * p_reg)
{
    return p_reg->MEMACCERR.ADDRESS;
}

#if NRF_MPC_HAS_OVERRIDE_OWNERID
NRF_STATIC_INLINE nrf_owner_t nrf_mpc_memaccerr_info_ownerid_get(NRF_MPC_Type const * p_reg)
{
    return (nrf_owner_t)((p_reg->MEMACCERR.INFO & MPC_MEMACCERR_INFO_OWNERID_Msk)
                         >> MPC_MEMACCERR_INFO_OWNERID_Pos);
}
#endif

#if NRF_MPC_HAS_OVERRIDE_MASTERPORT
NRF_STATIC_INLINE uint8_t nrf_mpc_memaccerr_info_masterport_get(NRF_MPC_Type const * p_reg)
{
    return ((p_reg->MEMACCERR.INFO & MPC_MEMACCERR_INFO_MASTERPORT_Msk)
            >> MPC_MEMACCERR_INFO_MASTERPORT_Pos);
}
#endif

NRF_STATIC_INLINE uint32_t nrf_mpc_memaccerr_info_perm_get(NRF_MPC_Type const * p_reg)
{
    return ((p_reg->MEMACCERR.INFO &
             (MPC_MEMACCERR_INFO_READ_Msk | MPC_MEMACCERR_INFO_WRITE_Msk |
              MPC_MEMACCERR_INFO_EXECUTE_Msk | MPC_MEMACCERR_INFO_SECURE_Msk))
            >> MPC_MEMACCERR_INFO_READ_Pos);
}

NRF_STATIC_INLINE nrf_mpc_errorsource_t
nrf_mpc_memaccerr_info_errorsource_get(NRF_MPC_Type const * p_reg)
{
    return (nrf_mpc_errorsource_t)((p_reg->MEMACCERR.INFO & MPC_MEMACCERR_INFO_ERRORSOURCE_Msk)
                                   >> MPC_MEMACCERR_INFO_ERRORSOURCE_Pos);
}

#if NRF_MPC_HAS_GLOBALSLAVE
NRF_STATIC_INLINE void nrf_mpc_globalslave_masterport_set(NRF_MPC_Type * p_reg, uint32_t mask)
{
    p_reg->GLOBALSLAVE.MASTERPORT = mask;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_globalslave_masterport_get(NRF_MPC_Type const * p_reg)
{
    return p_reg->GLOBALSLAVE.MASTERPORT;
}

NRF_STATIC_INLINE void nrf_mpc_globalslave_lock_enable(NRF_MPC_Type * p_reg)
{
    p_reg->GLOBALSLAVE.LOCK = (MPC_GLOBALSLAVE_LOCK_LOCK_Enabled << MPC_GLOBALSLAVE_LOCK_LOCK_Pos);
}

NRF_STATIC_INLINE bool nrf_mpc_globalslave_lock_check(NRF_MPC_Type const * p_reg)
{
    return ((p_reg->GLOBALSLAVE.LOCK & MPC_GLOBALSLAVE_LOCK_LOCK_Msk)
            >> MPC_GLOBALSLAVE_LOCK_LOCK_Pos) == MPC_GLOBALSLAVE_LOCK_LOCK_Enabled;
}
#endif

#if NRF_MPC_HAS_RTCHOKE
NRF_STATIC_INLINE void nrf_mpc_rtchoke_writeaccess_set(NRF_MPC_Type * p_reg, uint32_t mask)
{
    p_reg->RTCHOKE.WRITEACCESS = mask;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_rtchoke_writeaccess_get(NRF_MPC_Type const * p_reg)
{
    return p_reg->RTCHOKE.WRITEACCESS;
}

NRF_STATIC_INLINE void nrf_mpc_rtchoke_readaccess_set(NRF_MPC_Type * p_reg, uint32_t mask)
{
    p_reg->RTCHOKE.READACCESS = mask;
}

NRF_STATIC_INLINE uint32_t nrf_mpc_rtchoke_readaccess_get(NRF_MPC_Type const * p_reg)
{
    return p_reg->RTCHOKE.READACCESS;
}

NRF_STATIC_INLINE void nrf_mpc_rtchoke_delay_set(NRF_MPC_Type * p_reg,
                                                 uint8_t        slave,
                                                 uint8_t        delay)
{
    NRFX_ASSERT(slave < NRF_MPC_RTCHOKE_COUNT);

    p_reg->RTCHOKE.DELAY[slave] = delay;
}

NRF_STATIC_INLINE uint8_t nrf_mpc_rtchoke_delay_get(NRF_MPC_Type const * p_reg, uint8_t slave)
{
    NRFX_ASSERT(slave < NRF_MPC_RTCHOKE_COUNT);

    return (uint8_t)p_reg->RTCHOKE.DELAY[slave];
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_MPC_H_ */
