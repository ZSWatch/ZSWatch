/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_pbkdf2_hmac_aes_cmac_prf_128 PBKDF2-AES-CMAC-PRF-128
 * @ingroup ocrypto_kdf
 * @{
 * @brief PBKDF2 algorithm based on AES-CMAC-PRF-128.
 *
 * PBKDF2 with HMAC-AES-CMAC-PRF-128 is password-based key derivation function.
 * 
 * @see [RFC - PKCS #5: Password-Based Cryptography Specification Version 2.1](https://datatracker.ietf.org/doc/html/rfc8018)
 * @see [RFC - The AES-CMAC-PRF-128 Algorithm for the Internet Key Exchange Protocol](https://datatracker.ietf.org/doc/html/rfc4615)
 */

/**
 * @file
 * @brief PBKDF2 algorithm based on AES-CMAC-PRF-128.
 */

#ifndef OCRYPTO_PBKDF2_CMAC_PRF128_H
#define OCRYPTO_PBKDF2_CMAC_PRF128_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Computes the PBKDF2-AES-CMAC-PRF-128 key from password, salt, and iteration count.
 *
 * @param[out] key                  PBKDF2 key to generate.
 * @param      key_len              Length of key.
 * @param      password             Password to use.
 * @param      password_len         Length of password.
 * @param      salt                 Salt to use.
 * @param      salt_len             Length of salt. 0 < salt_len <= 32.
 * @param      count                Iteration count.
 */
void ocrypto_pbkdf2_aes_cmac_prf128(
    uint8_t *key, size_t key_len,
    const uint8_t *password, size_t password_len,
    const uint8_t *salt, size_t salt_len,
    uint32_t count);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
