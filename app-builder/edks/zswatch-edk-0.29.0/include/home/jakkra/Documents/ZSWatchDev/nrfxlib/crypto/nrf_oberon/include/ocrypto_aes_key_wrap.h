/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_aes_key_wrap AES Key Wrap
 * @ingroup ocrypto_key_wrap
 * @{
 * @brief AES key wrap algorithm.
 *
 * AES Key Wrap is an AES based key wrap algorithm.
 * There are variants with and without padding.
 * 
 * @see [NIST - SP 800-38F](https://csrc.nist.gov/pubs/sp/800/38/f/final)
 * @see [RFC - AES Key Wrap Algorithm](http://tools.ietf.org/html/rfc3394)
 * @see [RFC - AES Key Wrap with Padding Algorithm](http://tools.ietf.org/html/rfc5649)
 */

/**
 * @file
 * @brief AES key wrap API, with and without padding.
 */

#ifndef OCRYPTO_AES_KEY_WRAP_H
#define OCRYPTO_AES_KEY_WRAP_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * AES KW wrapping.
 *
 * @param[out] ct      Ciphertext. Length @p pt_len + 8.
 * @param      pt      Plaintext.
 * @param      pt_len  Plaintext length, @p pt_len = n * 8, n >= 2.
 * @param      key     AES key.
 * @param      size    Key size (16, 24, or 32).
 */
void ocrypto_aes_kw_wrap(
    uint8_t *ct,
    const uint8_t *pt, size_t pt_len,
    const uint8_t *key, size_t size);

/**
 * AES KW unwrapping.
 *
 * @param[out] pt      Plaintext. Length @p ct_len - 8.
 * @param      ct      Ciphertext.
 * @param      ct_len  Ciphertext length, @p ct_len = n * 8, n >= 3.
 * @param      key     AES key.
 * @param      size    Key size (16, 24, or 32).
 *
 * @retval 0  If @p ct is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_aes_kw_unwrap(
    uint8_t *pt,
    const uint8_t *ct, size_t ct_len,
    const uint8_t *key, size_t size);

/**
 * AES KWP wrapping.
 *
 * @param[out] ct      Ciphertext. Length (@p pt_len + 15) / 8 * 8.
 * @param      pt      Plaintext.
 * @param      pt_len  Plaintext length, @p pt_len > 0.
 * @param      key     AES key.
 * @param      size    Key size (16, 24, or 32).
 */
void ocrypto_aes_kwp_wrap(
    uint8_t *ct,
    const uint8_t *pt, size_t pt_len,
    const uint8_t *key, size_t size);

/**
 * AES KWP unwrapping.
 *
 * @param[out] pt      Plaintext. Buffer length >= @p ct_len - 8.
 * @param[out] pt_len  Plaintext length, @p ct_len - 16 < @p pt_len <= @p ct_len - 8.
 * @param      ct      Ciphertext.
 * @param      ct_len  Ciphertext length, @p ct_len = n * 8, n >= 2.
 * @param      key     AES key.
 * @param      size    Key size (16, 24, or 32).
 *
 * @retval 0  If @p ct is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_aes_kwp_unwrap(
    uint8_t *pt, size_t *pt_len,
    const uint8_t *ct, size_t ct_len,
    const uint8_t *key, size_t size);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
