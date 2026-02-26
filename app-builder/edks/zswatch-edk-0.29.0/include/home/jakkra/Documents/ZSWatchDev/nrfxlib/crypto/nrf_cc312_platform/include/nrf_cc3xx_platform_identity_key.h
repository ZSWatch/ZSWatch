/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#ifndef NRF_CC3XX_PLATFORM_IDENTITY_KEY__
#define NRF_CC3XX_PLATFORM_IDENTITY_KEY__

#include <stdint.h>
#include <stdbool.h>

/** @brief Function to store an identity key
 *
 * @note    The primary usage of this API is to store asymmetric key material
 *          corresponding to a secret identity of the device, for example an ECC
 *          private key using curve type secp256r1.
 *
 *          This API encrypts 32 bytes of raw key material and stores it in two
 *          consecutive KMU slots. The key is encrypted using a key derived from
 *          the Master Key Encryption Key (MKEK).
 *
 *          The key is stored in the KMU in a way that makes it accessible
 *          to read it from the CPU, but only in encrypted form. To decrypt
 *          the key for usage, please use @ref nrf_cc3xx_platform_identity_key_retrieve.
 *
 * @note    This API assumes no format of the data stored.
 *
 * @note    The key material stored using this API may not be in flash
 *          and should be kept secret, for instance by generating it
 *          directly on the device or securely provisioning it.
 *
 *
 * @param[in]   slot_id     The first KMU slot ID for the new key (uses 2).
 * @param[in]   key         The key material to encrypt and store.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_identity_key_store(
    uint32_t slot_id,
    const uint8_t key[32]);

/** @brief Function to check if the identity key is already stored in KMU
 *
 * @param[in]       slot_id     The first KMU slot ID for the stored identity key (uses 2).
 *
 * @return TRUE if they identity key is stored, FALSE otherwise
 */
bool nrf_cc3xx_platform_identity_key_is_stored(uint32_t slot_id);

/** @brief Function to retrieve an identity key
 *
 * @details The primary usage of this API is to retrieve asymmetric key material
 *          corresponding to a secret identity of the device, for example an ECC
 *          private key using curve type secp256r1.
 *
 *          This API decrypts 32 bytes of raw key material that is stored in two
 *          consecutive KMU slots. The key is decrypted using a key derived from
 *          the Master Key Encryption Key (MKEK).
 *
 * @note    This API assumes no format of the data stored.
 *
 * @note    The identity key should be cleared from RAM using
 *          @ref nrf_cc3xx_platform_identity_key_free after usage, to keep it secret
 *          on the device.
 *
 * @param[in]       slot_id     The first KMU slot ID for the stored identity key (uses 2).
 * @param[in,out]   key         The retrieved identity key.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_identity_key_retrieve(
    uint32_t slot_id,
    uint8_t key[32]);

/** @brief Function to clear out an identity key after usage
 *
 * @details This function will clear out the memory of an identity key.
 *          This must be called after the identity key is used to ensure that
 *          it is not leaked in the system.
 *
 * @param[in,out]   key         Key to clear out
 */
void nrf_cc3xx_platform_identity_key_free(
    uint8_t key[32]);

#endif /* NRF_CC3XX_PLATFORM_IDENTITY_KEY__ */
