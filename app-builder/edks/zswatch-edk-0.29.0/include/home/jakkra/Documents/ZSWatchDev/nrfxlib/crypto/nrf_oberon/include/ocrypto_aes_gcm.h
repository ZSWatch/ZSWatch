/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_aes_gcm AES-GCM
 * @ingroup ocrypto_auth_enc
 * @{
 * @brief AES Galois/Counter Mode.
 * 
 * AES-GCM (AES Galois/Counter Mode) is an AES mode which effectively turns the block cipher into
 * a stream cipher. The AES block cipher primitive is used in CTR mode for encryption and decryption.
 * In addition, 128-bit Galois Field multiplication is used for authentication.
 * 
 * @see [NIST - SP 800-38D](https://csrc.nist.gov/pubs/sp/800/38/d/final)
 */

/**
 * @file
 * @brief AES Galois/Counter Mode.
 */

#ifndef OCRYPTO_AES_GCM_H
#define OCRYPTO_AES_GCM_H

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
    uint8_t  x[16];
    uint8_t  m[16];
    uint8_t  position; // Current position in cipher.
    uint8_t  length;   // Data length added to x.
    uint8_t  key_size; // Key size (16, 24, or 32 bytes).
    size_t   ct_len;   // Total ciphertext length
    size_t   aa_len;   // Total aad length.
} ocrypto_aes_gcm_ctx;
/**@endcond */


/**
 * @name Incremental AES-GCM encryption/decryption
 *
 * This group of functions can be used to incrementally compute the
 * AES-GCM encryption/decryption for a given message.
 */
/**@{*/

/**
 * AES-GCM initialization.
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
void ocrypto_aes_gcm_init(ocrypto_aes_gcm_ctx *ctx, const uint8_t *key, size_t size, const uint8_t iv[12]);

/**
 * AES-GCM iv initialization.
 *
 * The context @p ctx is initialized using the given initial vector @p iv.
 *
 * @param[out] ctx   Context.
 * @param      iv    Initial vector.
 * @param      iv_len Length of @p iv.
 */
void ocrypto_aes_gcm_init_iv(ocrypto_aes_gcm_ctx* ctx, const uint8_t* iv, size_t iv_len);

/**
 * AES-GCM incremental aad input.
 *
 * The generator state @p ctx is updated to include a data chunk @p aa.
 *
 * This function can be called repeatedly until the whole data is processed.
 *
 * @param      ctx    Generator state.
 * @param      aa     Additional authenticated data.
 * @param      aa_len Length of @p aa.
 *
 * @remark Initialization of the context @p ctx through
 *         @c ocrypto_aes_gcm_init is required before this function can be called.
 * @remark @c ocrypto_aes_gcm_update_aad must be called before any call to
 *         @c ocrypto_aes_gcm_update_enc or @c ocrypto_aes_gcm_update_dec.
 */
void ocrypto_aes_gcm_update_aad(ocrypto_aes_gcm_ctx *ctx, const uint8_t *aa, size_t aa_len);

/**
 * AES-GCM incremental encryption.
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
 *         @c ocrypto_aes_gcm_init is required before this function can be called.
 */
void ocrypto_aes_gcm_update_enc(ocrypto_aes_gcm_ctx *ctx, uint8_t* ct, const uint8_t* pt, size_t pt_len);

/**
 * AES-GCM incremental decryption.
 *
 * The ciphertext @p ct is decrypted to the plaintext @p pt using the context @p ctx.
 *
 * This function can be called repeatedly until the whole message is processed.
 *
 * @param      ctx    Context.
 * @param[out] pt     Plaintext.
 * @param      ct     Ciphertext.
 * @param      ct_len Length of @p ct and @p pt.
 *
 * @remark @p ct may be same as @p pt.
 * @remark Initialization of the context @p ctx through
 *         @c ocrypto_aes_gcm_init is required before this function can be called.
 */
void ocrypto_aes_gcm_update_dec(ocrypto_aes_gcm_ctx *ctx, uint8_t* pt, const uint8_t* ct, size_t ct_len);

/**
 * AES-GCM final encoder step.
 *
 * The generator state @p ctx is used to finalize the encryption and generate the tag.
 *
 * @param      ctx     Generator state.
 * @param[out] tag     Generated authentication tag.
 * @param      tag_len Authentication tag length, 0 < @p tag_len <= 16.
 */
void ocrypto_aes_gcm_final_enc(ocrypto_aes_gcm_ctx *ctx, uint8_t *tag, size_t tag_len);

/**
 * AES-GCM final decoder step.
 *
 * The generator state @p ctx is used to finalize the decryption and check the tag.
 *
 * @param      ctx     Generator state.
 * @param      tag     Received authentication tag.
 * @param      tag_len Authentication tag length.
 *
 * @retval 0  If @p tag is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_aes_gcm_final_dec(ocrypto_aes_gcm_ctx *ctx, const uint8_t *tag, size_t tag_len);

/**@}*/


/**
 * AES-GCM encryption.
 *
 * @param[out] ct      Ciphertext.
 * @param[out] tag     Authentication tag.
 * @param      tag_len Authentication tag length, 0 < @p tag_len <= 16.
 * @param      pt      Plaintext.
 * @param      pt_len  Plaintext length, 0 < @p ct_len < 2^36-32 bytes.
 * @param      key     AES key.
 * @param      size    Key size (16, 24, or 32 bytes).
 * @param      iv      Initial vector.
 * @param      aa      Additional authentication data.
 * @param      aa_len  Additional authentication data length, 0 < @p aa_len < 2^61 bytes.
 *
 * @remark @p ct may be same as @p pt.
 */
void ocrypto_aes_gcm_encrypt (
    uint8_t* ct, uint8_t *tag, size_t tag_len, const uint8_t* pt, size_t pt_len,
    const uint8_t *key, size_t size, const uint8_t iv[12], const uint8_t *aa, size_t aa_len);

/**
 * AES-GCM decryption.
 *
 * @param[out] pt      Plaintext.
 * @param      tag     Authentication tag.
 * @param      tag_len Authentication tag length, 0 < @p tag_len <= 16.
 * @param      ct      Ciphertext.
 * @param      ct_len  Ciphertext length, 0 < @p ct_len < 2^36-32 bytes.
 * @param      key     AES key.
 * @param      size    Key size (16, 24, or 32 bytes).
 * @param      iv      Initial vector.
 * @param      aa      Additional authentication data.
 * @param      aa_len  Additional authentication data length, 0 < @p aa_len < 2^61 bytes.
 *
 * @remark @p ct may be same as @p pt.
 *
 * @retval 0  If @p tag is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_aes_gcm_decrypt (
    uint8_t* pt, const uint8_t *tag, size_t tag_len, const uint8_t* ct, size_t ct_len,
    const uint8_t *key, size_t size, const uint8_t iv[12], const uint8_t *aa, size_t aa_len);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
