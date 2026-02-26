/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_aes_ctr AES-CTR
 * @ingroup ocrypto_unauth_enc
 * @{
 * @brief AES Counter Mode.
 *
 * AES-CTR (AES Counter Mode) is an AES mode which effectively turns the block cipher into a stream
 * cipher. The AES block encryption is used on a value which is incremented for each new block.
 * The resulting cipher stream is then xor combined with the plaintext to get the ciphertext.
 * In contrast to AES itself, encryption and decryption operations are identical for AES-CTR.
 *
 * @see [NIST - Special Publication 800-38A](https://csrc.nist.gov/pubs/sp/800/38/a/final)
 */

/**
 * @file
 * @brief AES Counter Mode.
 */

#ifndef OCRYPTO_AES_CTR_H
#define OCRYPTO_AES_CTR_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef struct {
    uint32_t xkey[60];
    uint8_t  counter[16];
    uint8_t  cipher[16];
    uint32_t position; // Current position in cipher.
    uint8_t  key_size; // Key size (16, 24, or 32 bytes).
} ocrypto_aes_ctr_ctx;
/**@endcond */


/**
 * @name Incremental AES-CTR encryption/decryption
 *
 * This group of functions can be used to incrementally compute the
 * AES-CTR encryption/decryption for a given message.
 */
/**@{*/

/**
 * AES-CTR initialization.
 *
 * The context @p ctx is initialized using the given key @p key and initial vector @p iv.
 *
 * @param[out] ctx   Context.
 * @param      key   AES key. May be NULL.
 * @param      size  Key size (16, 24, or 32 bytes).
 * @param      iv    Initial vector. May be NULL.
 *
 * @remark If @p key is NULL only @p iv is set. If @p iv is NULL only @p key is set.
           Both @p key and @p iv must be set before update is called.
 */
void ocrypto_aes_ctr_init(ocrypto_aes_ctr_ctx *ctx, const uint8_t *key, size_t size, const uint8_t iv[16]);

/**
 * AES-CTR incremental encryption/decryption.
 *
 * The plaintext @p pt is encrypted to the ciphertext @p ct using the context @p ctx.
 *
 * This function can be called repeatedly until the whole message is processed.
 *
 * @param      ctx    Context.
 * @param[out] ct     Ciphertext.
 * @param      pt     Plaintext.
 * @param      pt_len Length of @p pt and @p ct.
 *
 * @remark @p ct may be same as @p pt.
 * @remark Initialization of the context @p ctx through
 *         @c ocrypto_aes_ctr_init is required before this function can be called.
 */
void ocrypto_aes_ctr_update(ocrypto_aes_ctr_ctx *ctx, uint8_t* ct, const uint8_t* pt, size_t pt_len);
/**@}*/

/**
 * AES-CTR encryption.
 *
 * @param[out] ct     Ciphertext.
 * @param      pt     Plaintext.
 * @param      pt_len Length of @p pt and @p ct.
 * @param      key    AES key.
 * @param      size   Key size (16, 24, or 32).
 * @param      iv     Initial vector.
 *
 * @remark @p ct may be same as @p pt.
 */
void ocrypto_aes_ctr_encrypt(
    uint8_t* ct, const uint8_t* pt, size_t pt_len, const uint8_t *key, size_t size, const uint8_t iv[16]);

/**
 * AES-CTR decryption.
 *
 * @param[out] pt     Plaintext.
 * @param      ct     Ciphertext.
 * @param      ct_len Length of @p pt and @p ct.
 * @param      key    AES key.
 * @param      size   Key size (16, 24, or 32).
 * @param      iv     Initial vector.
 *
 * @remark @p ct may be same as @p pt.
 */
void ocrypto_aes_ctr_decrypt(
    uint8_t* pt, const uint8_t* ct, size_t ct_len, const uint8_t *key, size_t size, const uint8_t iv[16]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
