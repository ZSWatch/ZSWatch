/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ascon_aead Ascon-AEAD128
 * @ingroup ocrypto_auth_enc
 * @{
 * @brief Ascon-AEAD is an authenticated encription algorithm designed for small devices.
 * 
 * @see [NIST SP 800-232](https://csrc.nist.gov/pubs/sp/800/232/final)
 */

/**
 * @file
 * @brief Ascon-AEAD is an authenticated encription algorithm designed for small devices.
 */

#ifndef OCRYPTO_ASCON_AEAD_H
#define OCRYPTO_ASCON_AEAD_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of the AEAD key in bytes.
 */
#define ocrypto_ascon_aead128_key_BYTES   (16)

/**
 * Length of the AEAD tag in bytes.
 */
#define ocrypto_ascon_aead128_tag_BYTES   (16)

/**
 * Length of the AEAD nonce in bytes.
 */
#define ocrypto_ascon_aead128_nonce_BYTES (16)


/**@cond */
typedef struct {
    uint64_t s[5];
    uint8_t  key[16];
    uint8_t  data[16];
    uint8_t  length;
    uint8_t  aad;
} ocrypto_ascon_aead_ctx;
/**@endcond */


/**
 * @name Incremental Ascon-AEAD128 encryption/decryption.
 *
 * This group of functions can be used to incrementally compute the
 * Ascon-AEAD128 encryption/decryption for a given message.
 */
/**@{*/

/**
 * Ascon-AEAD128 initialization.
 *
 * The context @p ctx is initialized using the given key @p key and nonce @p nonce.
 *
 * @param[out] ctx   Context.
 * @param      key   Ascon-AEAD128 key. May be NULL.
 * @param      nonce Nonce. May be NULL.
 *
 * @remark If @p key is NULL only @p nonce is set. If @p nonce is NULL only @p key is set.
           @p key must be set before @p nonce. 
           Both @p key and @p nonce must be set before update is called.
 */
void ocrypto_ascon_aead128_init(ocrypto_ascon_aead_ctx *ctx, const uint8_t key[16], const uint8_t nonce[16]);

/**
 * Ascon-AEAD128 incremental aad input.
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
 *         @c ocrypto_ascon_aead128_init is required before this function can be called.
 * @remark @c ocrypto_ascon_aead128_update_aad must be called before any call to
 *         @c ocrypto_ascon_aead128_update_enc or @c ocrypto_ascon_aead128_update_dec.
 */
void ocrypto_ascon_aead128_update_aad(ocrypto_ascon_aead_ctx *ctx, const uint8_t *aa, size_t aa_len);

/**
 * Ascon-AEAD128 incremental encryption.
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
 *         @c ocrypto_ascon_aead128_init is required before this function can be called.
 */
void ocrypto_ascon_aead128_update_enc(ocrypto_ascon_aead_ctx *ctx, uint8_t* ct, const uint8_t* pt, size_t pt_len);

/**
 * Ascon-AEAD128 incremental decryption.
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
 *         @c ocrypto_ascon_aead128_init is required before this function can be called.
 */
void ocrypto_ascon_aead128_update_dec(ocrypto_ascon_aead_ctx *ctx, uint8_t* pt, const uint8_t* ct, size_t ct_len);

/**
 * Ascon-AEAD128 final encoder step.
 *
 * The generator state @p ctx is used to finalize the encryption and generate the tag.
 *
 * @param      ctx     Generator state.
 * @param[out] tag     Generated authentication tag.
 */
void ocrypto_ascon_aead128_final_enc(ocrypto_ascon_aead_ctx *ctx, uint8_t tag[16]);

/**
 * Ascon-AEAD128 final decoder step.
 *
 * The generator state @p ctx is used to finalize the decryption and check the tag.
 *
 * @param      ctx     Generator state.
 * @param      tag     Received authentication tag.
 *
 * @retval 0  If @p tag is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ascon_aead128_final_dec(ocrypto_ascon_aead_ctx *ctx, const uint8_t tag[16]);

/**@}*/


/**
 * Ascon-AEAD128 encryption.
 *
 * @param[out] ct      Ciphertext.
 * @param[out] tag     Authentication tag.
 * @param      pt      Plaintext.
 * @param      pt_len  Plaintext length.
 * @param      key     Ascon-AEAD128 key.
 * @param      nonce   Nonce.
 * @param      aa      Additional authentication data.
 * @param      aa_len  Additional authentication data length.
 *
 * @remark @p ct may be same as @p pt.
 */
void ocrypto_ascon_aead128_encrypt (
    uint8_t* ct, uint8_t tag[16], const uint8_t* pt, size_t pt_len,
    const uint8_t key[16], const uint8_t nonce[16], const uint8_t *aa, size_t aa_len);

/**
 * Ascon-AEAD128 decryption.
 *
 * @param[out] pt      Plaintext.
 * @param      tag     Authentication tag.
 * @param      ct      Ciphertext.
 * @param      ct_len  Ciphertext length.
 * @param      key     Ascon-AEAD128 key.
 * @param      nonce   Nonce.
 * @param      aa      Additional authentication data.
 * @param      aa_len  Additional authentication data length.
 *
 * @remark @p ct may be same as @p pt.
 *
 * @retval 0  If @p tag is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ascon_aead128_decrypt (
    uint8_t* pt, const uint8_t tag[16], const uint8_t* ct, size_t ct_len,
    const uint8_t key[16], const uint8_t nonce[16], const uint8_t *aa, size_t aa_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
