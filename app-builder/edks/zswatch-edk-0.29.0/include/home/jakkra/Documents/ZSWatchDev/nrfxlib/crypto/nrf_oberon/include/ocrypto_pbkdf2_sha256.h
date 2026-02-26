/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_pbkdf2_hmac_sha256 PBKDF2-HMAC-SHA256
 * @ingroup ocrypto_kdf
 * @{
 * @brief PBKDF2 algorithm based on HMAC-SHA256.
 *
 * PBKDF2 with HMAC-SHA1 is password-based key derivation function.
 * 
 * @see [RFC - PKCS #5: Password-Based Cryptography Specification Version 2.1](https://datatracker.ietf.org/doc/html/rfc8018)
 */

/**
 * @file
 * @brief PBKDF2 algorithm based on HMAC-SHA256.
 */

#ifndef OCRYPTO_PBKDF2_SHA256_H
#define OCRYPTO_PBKDF2_SHA256_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Computes the PBKDF2-HMAC-SHA256 key from password, salt, and iteration count.
 *
 * @param[out] key                  PBKDF2 key to generate.
 * @param      key_len              Length of key.
 * @param      password             Password to use.
 * @param      password_len         Length of password.
 * @param      salt                 Salt to use.
 * @param      salt_len             Length of salt.
 * @param      count                Iteration count.
 */
void ocrypto_pbkdf2_hmac_sha256(
    uint8_t *key, size_t key_len,
    const uint8_t *password, size_t password_len,
    const uint8_t *salt, size_t salt_len,
    uint32_t count);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
