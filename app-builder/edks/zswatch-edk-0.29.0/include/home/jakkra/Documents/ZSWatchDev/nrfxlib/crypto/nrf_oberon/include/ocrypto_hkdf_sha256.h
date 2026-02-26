/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_hkdf_sha256 HKDF-SHA256
 * @ingroup ocrypto_kdf
 * @{
 * @brief HKDF algorithm based on HMAC-SHA256.
 *
 * HKDFs are key derivation functions based on HMAC Extract-and-Expand.
 * HKDF-SHA256 is a HKDF based on HMAC-SHA256.
 *
 * @see [RFC - HMAC-based Extract-and-Expand Key Derivation Function (HKDF)](http://tools.ietf.org/html/rfc5869)
 */

/**
 * @file
 * @brief Key derivation functions.
 */

#ifndef OCRYPTO_HKDF_SHA256_H
#define OCRYPTO_HKDF_SHA256_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * HKDF-SHA256 algorithm.
 *
 * A new pseudo-random key of length @p r_len is derived from an input key
 * @p key, a salt @p salt and additional information @p info. The new key is put
 * into @p r.
 *
 * @param[out] r        Output key.
 * @param      r_len    Length of @p r.
 * @param      key      Input key.
 * @param      key_len  Length of @p key.
 * @param      salt     Salt.
 * @param      salt_len Length of salt @p salt.
 * @param      info     Additional information.
 * @param      info_len Length of @p info.
 */
void ocrypto_hkdf_sha256(
    uint8_t* r, size_t r_len,
    const uint8_t* key, size_t key_len,
    const uint8_t* salt, size_t salt_len,
    const uint8_t* info, size_t info_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
