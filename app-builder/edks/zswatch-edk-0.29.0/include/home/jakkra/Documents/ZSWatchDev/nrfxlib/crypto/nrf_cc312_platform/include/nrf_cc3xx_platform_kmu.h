/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
/**@file
 * @defgroup nrf_cc3xx_platform_kmu nrf_cc3xx_platform kmu APIs
 * @ingroup nrf_cc3xx_platform
 * @{
 * @brief The nrf_cc3xx_platform_kmu APIs provides RTOS integration for storing
 *        keys in KMU hardware peripherals
 */
#ifndef NRF_CC3XX_PLATFORM_KMU__
#define NRF_CC3XX_PLATFORM_KMU__

#include <stdint.h>
#include <stddef.h>
#include "nrfx.h"

#if defined(NRF91_SERIES) || defined(NRF5340_XXAA_APPLICATION)

/** @brief Enumeration type listing the key types which support the CryptoCell push operation */
typedef enum {
    NRF_CC3XX_PLATFORM_KEY_TYPE_KDR_AES_128_BIT      = 0, /**< AES 128 bit key to be used in KDR. */
    NRF_CC3XX_PLATFORM_KEY_TYPE_CHACHAPOLY_256_BIT   = 1, /**< ChaChaPoly 256 bit key to be used in a KMU slot. */
    NRF_CC3XX_PLATFORM_KEY_TYPE_AES_128_BIT          = 2, /**< AES 128 bit key to be used in a KMU slot. */
#if defined(NRF5340_XXAA_APPLICATION)
    NRF_CC3XX_PLATFORM_KEY_TYPE_AES_256_BIT          = 3, /**< AES 256 bit key to be used in a KMU slot. */

#endif
} nrf_cc3xx_platform_key_type_t;

/** @brief Union type holding the key material to be loaded in the CryptoCell */
typedef union {
    uint8_t buff_128[16];
    uint8_t buff_256[32];
} nrf_cc3xx_platform_key_buff_t;


/** @brief Constant value representing the default permission to use
 *         when writing a key to KMU.
 *
 * @details This sets up the written key to be non-writable, non-readable
 *          and pushable.
 *
 * @warning Deviating from this mask when setting up permissions may allow
 *          reading the key from CPU, which has security implications.
 */
#define NRF_CC3XX_PLATFORM_KMU_DEFAULT_PERMISSIONS          (0xFFFFFFFCUL)


/** @brief Constant value representing the permission to use when writing an
 *         indentity_key to KMU
 *
 * @details This sets up the written key to be non-writable, readable and
 *          non-pushable
 */
#define NRF_CC3XX_PLATFORM_KMU_IDENTITY_KEY_PERMISSIONS     (0xFFFFFFFAUL)

#if defined(NRF91_SERIES)

    /** @brief Address of the AES key register in CryptoCell */
    #define NRF_CC3XX_PLATFORM_KMU_AES_ADDR                 (0x50841400UL)

    /** @brief Address of the ChaChaPoly key register in CryptoCell for 128 bit keys */
    #define NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR          (0x50841388UL)

    /** Address of the first 128 bits of ChaChaPoly key in CryptoCell */
    #define NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR1         (0x50841388UL)

    /** Address of the last 128 bits bits of ChaChaPoly key register in CryptoCell HW
     *
     * @note Used only when ChaChaPoly key is larger than 128 bits, in which case
     *       the ChaChaPoly key is split between two slots in KMU
     */
    #define NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR2         (0x50841398UL)

    #define NRF_CC3XX_PLATFORM_NRF9160_KDR_ADDR             (0x50841A50UL)

#elif defined(NRF5340_XXAA_APPLICATION)

    /** Address of the AES key register in CryptoCell for 128 bit keys */
    #define NRF_CC3XX_PLATFORM_KMU_AES_ADDR                 (0x50845400UL)

    /** Address of the first 128 bits of AES key in CryptoCell */
    #define NRF_CC3XX_PLATFORM_KMU_AES_ADDR_1               (0x50845400UL)

    /** Address of the subsequent bits of AES key register in CryptoCell HW
     *
     * @note Used only when AES key is larger than 128 bits, in which case
     *       the AES key is split between two slots in KMU
     */
    #define NRF_CC3XX_PLATFORM_KMU_AES_ADDR_2               (0x50845410UL)

    /** @brief Address of the ChaChaPoly key register in CryptoCell for 128 bit keys */
    #define NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR          (0x50845388UL)

    /** Address of the first 128 bits of ChaChaPoly key in CryptoCell */
    #define NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR1         (0x50845388UL)

    /** Address of the last 128 bits bits of ChaChaPoly key register in CryptoCell HW
     *
     * @note Used only when ChaChaPoly key is larger than 128 bits, in which case
     *       the ChaChaPoly key is split between two slots in KMU
     */
    #define NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR2         (0x50845398UL)
#endif

/** @brief Write a key into the KMU
 *
 * @details This writes a key into the KMU slot defined by slot_id. If the key is larger than
 *          128 bit it will occupy two consecutive KMU slots.
 *
 * @note    This function uses the default KMU key permissions,
 *          see @ref NRF_CC3XX_PLATFORM_KMU_DEFAULT_PERMISSIONS.
 *
 * @param[in]   slot_id     KMU slot ID for the new key (2 - 127).
 * @param[in]   key_type    The type of the key to store in the KMU.
 * @param[in]   key_buff    Buffer containing the key material.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_kmu_write_key(
    uint32_t slot_id,
    nrf_cc3xx_platform_key_type_t key_type,
    nrf_cc3xx_platform_key_buff_t key_buff);


/** @brief Write a 128 bit key into a KMU slot
 *
 * @details This writes a key to KMU with the destination of the subsequent
 *          push operation set to the address of the AES or ChaChaPoly key
 *          registers in Arm CryptoCell.
 *
 * @note    The default mask for permissions is recommended to use.
 *          Please see @ref NRF_CC3XX_PLATFORM_KMU_DEFAULT_PERMISSIONS.
 *
 * @note AES keys of 128 bits can use @ref NRF_CC3XX_PLATFORM_KMU_AES_ADDR as the key_addr.
 *       ChaChaPoly keys of 128 bits can use NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR as the
 *       key addr.
 *       Keys larger than 128 bits must be split up to use two KMU slots.
 *       For AES (only applicable to nRF5340):
 *       Use NRF_CC3XX_PLATFORM_KMU_AES_ADDR_1 for the first 128 bits of
 *       the key.
 *       Use NRF_CC3XX_PLATFORM_KMU_AES_ADDR_2 for the subsequent bits of
 *       the key.
 *       For ChaChaPoly:
 *       Use NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR_1 for the first 128 bits of
 *       the key.
 *       Use NRF_CC3XX_PLATFORM_KMU_CHACHAPOLY_ADDR_2 for the subsequent bits of
 *       the key.
 *
 * @param[in]   slot_id     KMU slot ID for the new key (2 - 127).
 * @param[in]   key_addr    Destination address in CryptoCell used for key push.
 * @param[in]   key_perm    Permissions to set for the KMU slot.
 * @param[in]   key         Array with the 128 bit key to put in the KMU slot.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_kmu_write_key_slot(
    uint32_t slot_id,
    uint32_t key_addr,
    uint32_t key_perm,
    const uint8_t key[16]);

#endif /* defined(NRF91_SERIES) || defined(NRF5340_XXAA_APPLICATION) */

#if defined(NRF91_SERIES)

/** @brief Write a 128 bit AES key into a KMU slot for KDR use
 *
 * @details This writes a key to KMU with the destination of the subsequent
 *          push operation set to the address of the KDR registers in
 *          Arm CryptoCell.
 *
 * @note    The permission set by this function is "non-writable, non-readable
 *          and pushable". Please see
 *          @ref NRF_CC3XX_PLATFORM_KMU_DEFAULT_PERMISSIONS.
 *
 * @param[in]   slot_id     Slot id for the KDR key.
 * @param[in]   key         Array with the 128 bit key to put in the KMU slot.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_kmu_write_kdr_slot(uint32_t slot_id, const uint8_t key[16]);


/** @brief Push the 128 bit AES key from a KMU slot into CryptoCell KDR
 *         registers and set LCS state to secure
 *
 *  @note This function must be run once on every boot to load the KDR key
 *        and to set the LCS state to secure.
 *
 * @note The KDR key will be stored in the Always on Domain (AO) until the next
 *       reset. It is not possible to set the KDR value once the LCS state is
 *       set to secure.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_kmu_push_kdr_slot_and_lock(uint32_t slot_id);

#endif // defined(NRF91_SERIES)


#if defined(NRF52840_XXAA)

/** @brief Load a unique 128 bit root key into CryptoCell KDR registers and set
 *         CryptoCell LCS state to secure
 *
 * @note This function must be run once on every boot do load an AES key into
 *       KDR. It is recommended that this is done in an immutable bootloader
 *       stage and the page holding the key is ACL read+write protected after
 *       it has been loaded into KDR with this API.
 *
 * @note The KDR key should be a randomly generated unique key.
 *
 * @note The KDR key will be stored in the Always on Domain (AO) until the next
 *       reset. It is not possible to set the KDR value once the LCS state is
 *       set to secure.
 *
 * @param[in]   key     Array with the AES 128 bit key.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_kdr_load_key(uint8_t key[16]);

#endif // defined(NRF52840_XXAA)

/** @brief Function to use CMAC to derive a key stored in KMU/KDR
 *
 * @details The KDF is using a PRF function described in the Special publication
 *          800-108: Recommendation for Key Derivation Using Pseudorandom Functions
 *          https://csrc.nist.gov/publications/detail/sp/800-108/final.
 *
 *          This algorithm is described in chapter 5.1 - KDF in Counter Mode
 *
 *          The format of the PRF (the input) is as follows:
 *          PRF (KI, i || Label || 0x00 || Context || L)
 *
 *          KI: The Key derivation key
 *          i : The counter value for each iteration of the PRF represented
 *              as one byte.
 *          label: A string identifying the purpose of the derived key
 *                 that is up to 64 bytes long.
 *          0x00: A single byte delimiter.
 *          Context: Fixed information about the derived keying material
 *                   that is up to 64 bytes long.
 *          L : The length of derived key material in bits represented as two
 *              bytes.
 *
 * @note On nRF52840 only slot_id == 0 is valid, pointing to the
 *       Kdr key (also known as a HUK key) loaded into the CryptoCell.
 *
 * @param   slot_id         Identifier of the key slot.
 * @param   keybits         Key size in bits.
 * @param   label           Label to use for KDF.
 * @param   label_size      Size of the label in bytes to use for KDF.
 * @param   context         Context info to use for KDF.
 * @param   context_size    Context info size in bytes to use for KDF.
 * @param   output          Output buffer.
 * @param   output_size     Size of the output buffer in bytes.
 *
 * @returns 0 on success, otherwise a negative number.
 */
int nrf_cc3xx_platform_kmu_shadow_key_derive(
    uint32_t slot_id,
    unsigned int keybits,
    uint8_t const * label,
    size_t label_size,
    uint8_t const * context,
    size_t context_size,
    uint8_t * output,
    size_t output_size);



#endif /* NRF_CC3XX_PLATFORM_KMU__ */
/** @} */
