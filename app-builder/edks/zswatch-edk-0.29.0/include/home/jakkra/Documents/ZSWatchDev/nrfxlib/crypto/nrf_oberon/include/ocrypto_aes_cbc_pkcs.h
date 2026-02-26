/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_aes_cbc_pkcs7 AES-CBC-PKCS7
 * @ingroup ocrypto_unauth_enc
 * @{
 * @brief AES Cipher Block Chaining with PKCS7 padding.
 *
 * AES-CBC (AES Cipher Block Chaining) is an AES block cipher mode which avoids the problems of the
 * ECB mode by xoring each plaintext block with the previous ciphertext block before being encrypted.
 * PKCS7 padding allows encoding/decoding of arbitrarily sized messages.
 *
 * @see [NIST - Special Publication 800-38A](https://csrc.nist.gov/pubs/sp/800/38/a/final)
 * @see [RFC - PKCS #7: Cryptographic Message Syntax Version 1.5](http://tools.ietf.org/html/rfc2315)
 */

/**
 * @file
 * @brief AES Cipher Block Chaining with PKCS7 padding.
 */

#ifndef OCRYPTO_AES_CBC_PKCS_H
#define OCRYPTO_AES_CBC_PKCS_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef struct {
    uint32_t xkey[60];
    uint8_t  iv[16];
    uint8_t  buffer[16];
    uint8_t  length;
    uint8_t  key_size;
    uint8_t  decrypt;
    uint8_t  iv_set;
} ocrypto_aes_cbc_pkcs_ctx;
/**@endcond */


/**
 * @name Incremental AES-CBC encryption/decryption
 *
 * This group of functions can be used to incrementally compute the
 * AES-CBC encryption/decryption for a given message.
 */
/**@{*/

/**
 * AES-CBC initialization.
 *
 * The context @p ctx is initialized using the given key @p key and initial vector @p iv.
 *
 * @param[out] ctx     Context.
 * @param      key     AES key. May be NULL.
 * @param      size    Key size (16, 24, or 32 bytes).
 * @param      iv      Initial vector. May be NULL.
 * @param      decrypt If 0, initialize for encryption;
 *                     If 1, initialize for decryption.
 *
 * @remark If @p key is NULL only @p iv is set. If @p iv is NULL only @p key is set.
 *         If not set by the same call, @p key must be set before @p iv.
 */
void ocrypto_aes_cbc_pkcs_init(
    ocrypto_aes_cbc_pkcs_ctx *ctx,
    const uint8_t *key, size_t size,
    const uint8_t iv[16],
    int decrypt);

/**
 * AES-CBC output size calculation.
 *
 * Calculates the length of the output written to @p out in a call to @c ocrypto_aes_cbc_pkcs_update. 
 *
 * @param   ctx    Context.
 * @param   in_len Length of input data for @c ocrypto_aes_cbc_pkcs_update.
 * 
 * @returns The exact length of the output data of @c ocrypto_aes_cbc_pkcs_update.
 *
 * @remark @c ocrypto_aes_cbc_pkcs_output_size must be called before @c ocrypto_aes_cbc_pkcs_update.
 * @remark Initialization of the context @p ctx through
 *         @c ocrypto_aes_ctr_init is required before this function can be called.
 */
size_t ocrypto_aes_cbc_pkcs_output_size(ocrypto_aes_cbc_pkcs_ctx *ctx, size_t in_len);

/**
 * AES-CBC incremental encryption/decryption.
 *
 * The input @p in is encrypted or decrypted to the output @p out using the context @p ctx.
 *
 * This function can be called repeatedly until the whole message is processed.
 *
 * @param      ctx     Context.
 * @param[out] out     Output, ciphertext for encryption, plaintext for decryption. 
 * @param      in      Input, plaintext for encryption, ciphertext for decryption.
 * @param      in_len  Length of @p in.
 *
 * @remark @p in may be same as @p out.
 * @remark The exact length of @p out is returned by @c ocrypto_aes_cbc_pkcs_output_size.
 * @remark The maximum length of @p out is @p in_len + 15.
 * @remark A single common buffer may be used for the whole plaintext and ciphertext
           if @p in and @p out of the first call are equal and incremented individually
           by the input and output size for each further call (meaning the plaintext
           and ciphertext are stored contiguously in the common buffer).
 * @remark Initialization of the context @p ctx through
 *         @c ocrypto_aes_ctr_init is required before this function can be called.
 */
void ocrypto_aes_cbc_pkcs_update(
    ocrypto_aes_cbc_pkcs_ctx *ctx,
    uint8_t *out,
    const uint8_t *in, size_t in_len);

/**
 * AES-CBC-PKCS7 final encryption output.
 *
 * @param      ctx    Context.
 * @param[out] ct     Last ciphertext block.
 */
void ocrypto_aes_cbc_pkcs_final_enc(ocrypto_aes_cbc_pkcs_ctx *ctx, uint8_t ct[16]);

/**
 * AES-CBC-PKCS7 final decryption output.
 *
 * @param      ctx    Context.
 * @param[out] pt     Last part of the plaintext.
 * @param[out] pt_len Length of @p pt.
 *
 * @retval 0  If the ciphertext input is properly padded.
 * @retval -1 Otherwise.
 *
 * @remark The total length of the ciphertext added before a call to this function
 *         must be non-zero and a multiple of 16.
 * @remark The maximum length of @p pt is 15.
 * @remark All clients, on all software layers, must ensure that the time for the
 *         operation is identical whether or not a padding error occurs.
 *         This is to prevent padding oracle attacks, a form of timing side-
 *         channel attacks.
 *         The provided implementation of this function does not leak timing
 *         information.
 */
int ocrypto_aes_cbc_pkcs_final_dec(ocrypto_aes_cbc_pkcs_ctx *ctx, uint8_t *pt, size_t *pt_len);
/** @} */

/**
 * AES-CBC-PKCS7 encryption.
 *
 * @param[out] ct     Ciphertext.
 * @param      pt     Plaintext.
 * @param      pt_len Plaintext length.
 * @param      key    AES key.
 * @param      size   Key size (16, 24, or 32).
 * @param      iv     Initial vector.
 *
 * @remark @p ct may be same as @p pt.
 * @remark The length of @p ct is @p pt_len + 1 rounded up to the next multiple of 16.
 */
void ocrypto_aes_cbc_pkcs_encrypt(
    uint8_t* ct,
    const uint8_t* pt, size_t pt_len,
    const uint8_t *key, size_t size,
    const uint8_t iv[16]);

/**
 * AES-CBC-PKCS7 decryption.
 *
 * @param[out] pt     Plaintext.
 * @param[out] pt_len Plaintext length.
 * @param      ct     Ciphertext.
 * @param      ct_len Ciphertext length. Must be a multiple of 16.
 * @param      key    AES key.
 * @param      size   Key size (16, 24, or 32).
 * @param      iv     Initial vector.
 *
 * @retval 0  If the input is properly padded.
 * @retval -1 Otherwise.
 *
 * @remark @p pt may be same as @p ct.
 * @remark The maximum length of @p pt is @p ct_len - 1.
*/
int ocrypto_aes_cbc_pkcs_decrypt(
    uint8_t *pt, size_t *pt_len,
    const uint8_t *ct, size_t ct_len,
    const uint8_t *key, size_t size, 
    const uint8_t iv[16]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
