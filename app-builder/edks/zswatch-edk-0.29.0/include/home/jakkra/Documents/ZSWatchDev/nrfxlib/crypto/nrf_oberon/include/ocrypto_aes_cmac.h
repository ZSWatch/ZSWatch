/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_aes_cmac AES-CMAC
 * @ingroup ocrypto_mac
 * @{
 * @brief AES Cipher-based Message Authentication Code algorithm.
 *
 * AES-CMAC (AES Cipher-based Message Authentication Code) is a block cipher-based message
 * authentication code algorithm. The AES block cipher primitive is used in variant of the
 * CBC mode to get the authentication tag.
 *
 * @see [RFC - The AES-CMAC Algorithm](http://tools.ietf.org/html/rfc4493)
 * @see [NIST - Special Publication 800-38B](https://csrc.nist.gov/pubs/sp/800/38/b/upd1/final)
 */

/**
 * @file
 * @brief AES Cipher-based Message Authentication Code.
 */

#ifndef OCRYPTO_AES_CMAC_H
#define OCRYPTO_AES_CMAC_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of the pseudo random function.
 */
#define ocrypto_aes_cmac_prf128_BYTES (16)

/**
 * AES-CMAC authentication algorithm.
 *
 * @param[out] tag     Resulting tag.
 * @param      tag_len Tag length, 0 < @p tag_len <= 16.
 * @param      msg     Message to authenticate.
 * @param      msg_len Message length.
 * @param      key     AES key.
 * @param      size    Key size (16, 24, or 32).
 */
void ocrypto_aes_cmac_authenticate(
    uint8_t *tag, size_t tag_len,
    const uint8_t *msg, size_t msg_len,
    const uint8_t *key, size_t size);

/**
 * AES-CMAC-PRF-128 pseudo random function algorithm.
 *
 * @param[out] prf     16 byte PRF output.
 * @param      msg     Message input.
 * @param      msg_len Message length.
 * @param      key     Key.
 * @param      key_len Key length.
 */
void ocrypto_aes_cmac_prf128(
    uint8_t prf[ocrypto_aes_cmac_prf128_BYTES],
    const uint8_t *msg, size_t msg_len,
    const uint8_t *key, size_t key_len);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
