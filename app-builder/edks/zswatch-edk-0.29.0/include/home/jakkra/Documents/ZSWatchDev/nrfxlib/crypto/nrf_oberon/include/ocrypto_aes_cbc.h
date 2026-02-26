/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_aes_cbc AES-CBC
 * @ingroup ocrypto_unauth_enc
 * @{
 * @brief AES Cipher Block Chaining.
 *
 * AES-CBC (AES Cipher Block Chaining) is an AES block cipher mode which avoids the problems of the
 * ECB mode by xoring each plaintext block with the previous ciphertext block before being encrypted.
 *
 * @see [NIST - Special Publication 800-38A](https://csrc.nist.gov/pubs/sp/800/38/a/final)
 */

/**
 * @file
 * @brief AES Cipher Block Chaining.
 */

#ifndef OCRYPTO_AES_CBC_H
#define OCRYPTO_AES_CBC_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef struct {
    uint32_t xkey[60];
    uint8_t  iv[16];
    uint8_t  key_size; // Key size (16, 24, or 32 bytes).
} ocrypto_aes_cbc_ctx;
/**@endcond */


/**
 * @name Incremental AES-CBC encryption/decryption
 *
 * This group of functions can be used to incrementally compute the
 * AES-CBC encryption/decryption for a given message.
 */
/**@{*/

/**
 * AES-CBC encrypt initialization.
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
void ocrypto_aes_cbc_init_enc(ocrypto_aes_cbc_ctx *ctx, const uint8_t *key, size_t size, const uint8_t iv[16]);

/**
 * AES-CBC decrypt initialization.
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
void ocrypto_aes_cbc_init_dec(ocrypto_aes_cbc_ctx *ctx, const uint8_t *key, size_t size, const uint8_t iv[16]);

/**
 * AES-CBC incremental encryption.
 *
 * The plaintext @p pt is encrypted to the ciphertext @p ct using the context @p ctx.
 *
 * This function can be called repeatedly until the whole message is processed.
 *
 * @param      ctx    Context.
 * @param[out] ct     Ciphertext.
 * @param      pt     Plaintext.
 * @param      pt_len Length of @p pt and @p ct. Must be a multiple of the block size.
 *
 * @remark @p ct may be same as @p pt.
 * @remark Initialization of the context @p ctx through
 *         @c ocrypto_aes_ctr_init is required before this function can be called.
 */
void ocrypto_aes_cbc_update_enc(ocrypto_aes_cbc_ctx *ctx, uint8_t* ct, const uint8_t* pt, size_t pt_len);

/**
 * AES-CBC incremental decryption.
 *
 * The ciphertext @p ct is decrypted to the plaintext @p pt using the context @p ctx.
 *
 * This function can be called repeatedly until the whole message is processed.
 *
 * @param      ctx    Context.
 * @param[out] pt     Plaintext.
 * @param      ct     Ciphertext.
 * @param      ct_len Length of @p ct and @p pt. Must be a multiple of the block size.
 *
 * @remark @p ct may be same as @p pt.
 * @remark Initialization of the context @p ctx through
 *         @c ocrypto_aes_ctr_init is required before this function can be called.
 */
void ocrypto_aes_cbc_update_dec(ocrypto_aes_cbc_ctx *ctx, uint8_t* pt, const uint8_t* ct, size_t ct_len);
/** @} */

/**
 * AES-CBC encryption.
 *
 * @param[out] ct     Ciphertext.
 * @param      pt     Plaintext.
 * @param      pt_len Plaintext length.
 * @param      key    AES key.
 * @param      size   Key size (16, 24, or 32).
 * @param      iv     Initial vector.
 *
 * @remark @p ct may be same as @p pt.
 */
void ocrypto_aes_cbc_encrypt(
    uint8_t* ct, const uint8_t* pt, size_t pt_len, const uint8_t *key, size_t size, const uint8_t iv[16]);

/**
 * AES-CBC decryption.
 *
 * @param[out] pt     Plaintext.
 * @param      ct     Ciphertext.
 * @param      ct_len Ciphertext length.
 * @param      key    AES key.
 * @param      size   Key size (16, 24, or 32).
 * @param      iv     Initial vector.
 *
 * @remark @p ct may be same as @p pt.
 */
void ocrypto_aes_cbc_decrypt(
    uint8_t* pt, const uint8_t* ct, size_t ct_len, const uint8_t *key, size_t size, const uint8_t iv[16]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
