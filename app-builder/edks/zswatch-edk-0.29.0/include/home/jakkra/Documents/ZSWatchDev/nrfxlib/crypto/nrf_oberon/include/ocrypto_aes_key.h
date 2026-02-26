/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_aes_key AES Key Sizes
 * @ingroup ocrypto_unauth_enc
 * @{
 * @brief Declaration of AES key sizes.
 *
 * AES key sizes in bytes.
 * 
 * The Advanced Encryption Standard (AES) is a block cipher with a block size of
 * 128 bits. It supports key lengths of 128, 192 and 256 bits.
 * 
 * @see [FIPS - Advanced Encryption Standard (AES)](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf)
 */

/**
 * @file
 * @brief Declaration of AES key sizes.
 */

#ifndef OCRYPTO_AES_KEY_H
#define OCRYPTO_AES_KEY_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of AES-128 key.
 */
#define ocrypto_aes128_KEY_BYTES ((size_t) 16)

/**
 * Length of AES-192 key.
 */
#define ocrypto_aes192_KEY_BYTES ((size_t) 24)

/**
 * Length of AES-256 key.
 */
#define ocrypto_aes256_KEY_BYTES ((size_t) 32)

#ifdef __cplusplus
}
#endif

#endif

/** @} */
