/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_chacha_poly ChaCha20-Poly1305
 * @ingroup ocrypto_auth_enc
 * @{
 * @brief ChaCha20-Poly1305 and XChaCha20 algorithms.
 *
 * ChaCha20-Poly1305 is an authenticated encryption algorithm with optional
 * additional authenticated data developed by Daniel J.Bernstein.
 *
 * The ChaCha20 stream cipher is combined with the Poly1305 authenticator.
 * The XChaCha20 variant uses a 192-bit nonce instead of ChaCha20's 96-bit nonce.
 * 
 * @see [RFC - ChaCha20 and Poly1305 for IETF Protocols](http://tools.ietf.org/html/rfc8439)
 * @see [XChaCha](https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-xchacha-03)
 */

/**
 * @file
 * @brief ChaCha20-Poly1305 and XChaCha20 algorithms.
 */

#ifndef OCRYPTO_CHACHA20_POLY1305_INC_H
#define OCRYPTO_CHACHA20_POLY1305_INC_H

#include <stdint.h>
#include <stddef.h>
#include "ocrypto_chacha20.h"
#include "ocrypto_poly1305.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of the encryption key.
 */
#define ocrypto_chacha20_poly1305_KEY_BYTES (32)

/**
 * Maximum length of the nonce.
 */
#define ocrypto_chacha20_poly1305_NONCE_BYTES_MAX (12)

/**
 * Length of the authentication tag.
 */
#define ocrypto_chacha20_poly1305_TAG_BYTES (16)


/**@cond */
typedef struct {
    ocrypto_chacha20_ctx enc_ctx;
    ocrypto_poly1305_ctx auth_ctx;
    size_t msg_len;
    size_t aad_len;
} ocrypto_chacha20_poly1305_ctx;
/**@endcond */


/**
 * @name Incremental ChaCha20-Poly1305 generator
 *
 * This group of functions can be used to incrementally encode and 
 * decode a message using the ChaCha20-Poly1305 stream cipher.
 *
 * Use pattern:
 *
 * Encoding:
 * @code
 *   ocrypto_chacha20_poly1305_init(ctx, nonce, nonce_len, key);
 *   ocrypto_chacha20_poly1305_update_aad(ctx, aad, aad_len);
 *   ...
 *   ocrypto_chacha20_poly1305_update_aad(ctx, aad, aad_len);
 *   ocrypto_chacha20_poly1305_update_enc(ctx, ct, pt, pt_len);
 *   ...
 *   ocrypto_chacha20_poly1305_update_enc(ctx, ct, pt, pt_len);
 *   ocrypto_chacha20_poly1305_final_enc(ctx, tag);
 * @endcode
 * Decoding:
 * @code
 *   ocrypto_chacha20_poly1305_init(ctx, nonce, nonce_len, key);
 *   ocrypto_chacha20_poly1305_update_aad(ctx, aad, aad_len);
 *   ...
 *   ocrypto_chacha20_poly1305_update_aad(ctx, aad, aad_len);
 *   ocrypto_chacha20_poly1305_update_dec(ctx, pt, ct, ct_len);
 *   ...
 *   ocrypto_chacha20_poly1305_update_dec(ctx, pt, ct, ct_len);
 *   res = ocrypto_chacha20_poly1305_final_dec(ctx, tag);
 * @endcode
 */
/**@{*/

/**
 * ChaCha20-Poly1305 initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx   Generator state.
 * @param      n     Nonce (or IV).
 * @param      n_len Length of @p n. @p n_len = 8, 12, or 24.
 * @param      k     Encryption key.
 */
void ocrypto_chacha20_poly1305_init(
    ocrypto_chacha20_poly1305_ctx *ctx,
    const uint8_t *n, size_t n_len,
    const uint8_t k[ocrypto_chacha20_poly1305_KEY_BYTES]);

/**
 * ChaCha20-Poly1305 incremental aad input.
 *
 * The generator state @p ctx is updated to include an additional authenticated data chunk @p a.
 *
 * This function can be called repeatedly until the whole data is processed.
 *
 * @param      ctx   Generator state.
 * @param      a     Additional authenticated data.
 * @param      a_len Length of @p a.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_chacha20_poly1305_init is required before this function can be called.
 *
 * @remark @c ocrypto_chacha20_poly1305_update_aad must be called before any call to
 *         @c ocrypto_chacha20_poly1305_update_enc or @c ocrypto_chacha20_poly1305_update_dec.
 * 
 * @remark If @p n_len = 24, the XChaCha20-Poly1305 algorithm is used.
 */
void ocrypto_chacha20_poly1305_update_aad(
    ocrypto_chacha20_poly1305_ctx *ctx,
    const uint8_t *a, size_t a_len);

/**
 * ChaCha20-Poly1305 incremental encoder input.
 *
 * The generator state @p ctx is updated to include a message chunk @p m.
 *
 * This function can be called repeatedly on arbitrarily small chunks of the 
 * message until the whole message has been processed.
 *
 * @param      ctx   Generator state.
 * @param[out] c     Generated ciphertext. Same length as input message.
 * @param      m     Message chunk.
 * @param      m_len Length of @p m.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_chacha20_poly1305_init is required before this function can be called.
 *
 * @remark @c ocrypto_chacha20_poly1305_update_enc must be called after any call to
 *         @c ocrypto_chacha20_poly1305_update_aad.
 *
 * @remark @p c may be same as @p m.
 */
void ocrypto_chacha20_poly1305_update_enc(
    ocrypto_chacha20_poly1305_ctx *ctx,
    uint8_t *c,
    const uint8_t *m, size_t m_len);

/**
 * ChaCha20-Poly1305 incremental decoder input.
 *
 * The generator state @p ctx is updated to include a ciphertext chunk @p c.
 *
 * This function can be called repeatedly on arbitrarily small chunks of the 
 * ciphertext until the whole ciphertext has been processed.
 *
 * @param      ctx   Generator state.
 * @param[out] m     Decoded message. Same length as received ciphertext.
 * @param      c     Ciphertext chunk.
 * @param      c_len Length of @p c.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_chacha20_poly1305_init is required before this function can be called.
 *
 * @remark @c ocrypto_chacha20_poly1305_update_dec must be called after any call to
 *         @c ocrypto_chacha20_poly1305_update_aad.
 *
 * @remark @p m may be same as @p c.
 */
void ocrypto_chacha20_poly1305_update_dec(
    ocrypto_chacha20_poly1305_ctx *ctx,
    uint8_t *m,
    const uint8_t *c, size_t c_len);

/**
 * ChaCha20-Poly1305 final encoder step.
 *
 * The generator state @p ctx is used to finalize the encryption and generate the tag.
 *
 * @param      ctx   Generator state.
 * @param[out] tag   Generated authentication tag.
 */
void ocrypto_chacha20_poly1305_final_enc(
    ocrypto_chacha20_poly1305_ctx *ctx,
    uint8_t tag[ocrypto_chacha20_poly1305_TAG_BYTES]);

/**
 * ChaCha20-Poly1305 final decoder step.
 *
 * The generator state @p ctx is used to finalize the decryption and check the tag.
 *
 * @param      ctx   Generator state.
 * @param      tag   Received authentication tag.
 *
 * @retval 0  If @p tag is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_chacha20_poly1305_final_dec(
    ocrypto_chacha20_poly1305_ctx *ctx,
    const uint8_t tag[ocrypto_chacha20_poly1305_TAG_BYTES]);
/**@}*/

/**
 * AEAD ChaCha20-Poly1305 encrypt.
 *
 * The message @p m is encrypted using a ChaCha20 cipher stream derived from the
 * encryption key @p k and the nonce @p n. The resulting ciphertext has the same
 * length @p m_len as the input message @p m and is put into @p c.
 *
 * Additionally, the ciphertext @p c, as well as the additional authenticated
 * data @p a, is authenticated with a tag that is generated with Poly1305 using a
 * unique subkey derived from @p k and @p n, and then put into @p tag.
 *
 * @param[out] tag   Generated authentication tag.
 * @param[out] c     Generated ciphertext. Same length as input message.
 * @param      m     Input message.
 * @param      m_len Length of @p m and @p c.
 * @param      a     Additional authenticated data.
 * @param      a_len Length of @p a. May be 0.
 * @param      n     Nonce (or IV).
 * @param      n_len Length of @p n. @p n_len = 8, 12, or 24.
 * @param      k     Encryption key.
 *
 * @remark @p c may be same as @p m.
 *
 * @remark When reusing an encryption key @p k for a different message @p m or
 *         different additional authenticated data @p a, a different nonce @p n
 *         must be used.
 * 
 * @remark If @p n_len = 24, the XChaCha20-Poly1305 algorithm is used.
 */
void ocrypto_chacha20_poly1305_encrypt(
    uint8_t tag[ocrypto_chacha20_poly1305_TAG_BYTES],
    uint8_t *c,
    const uint8_t *m, size_t m_len,
    const uint8_t *a, size_t a_len,
    const uint8_t *n, size_t n_len,
    const uint8_t k[ocrypto_chacha20_poly1305_KEY_BYTES]);

/**
 * AEAD ChaCha20-Poly1305 decrypt.
 *
 * If the authentication tag @p tag is valid for the ciphertext @p c, the
 * additional authenticated data @p a, the encryption key @p k and the nonce
 * @p n, the ciphertext is decrypted and put into @p m. The decrypted message
 * @p m has the same length @p c_len as the original ciphertext.
 *
 * @param      tag   Received authentication tag.
 * @param[out] m     Decoded message. Same length as received ciphertext.
 * @param      c     Received ciphertext.
 * @param      c_len Length of @p c and @p m.
 * @param      a     Received additional authenticated data.
 * @param      a_len Length of @p a. May be 0.
 * @param      n     Nonce (or IV).
 * @param      n_len Length of @p n. 0 <= @p n_len <= @c ocrypto_chacha20_poly1305_NONCE_BYTES_MAX.
 * @param      k     Encryption key.
 *
 * @retval 0  If @p tag is valid.
 * @retval -1 Otherwise.
 *
 * @remark @p m may be same as @p c.
 */
int ocrypto_chacha20_poly1305_decrypt(
    const uint8_t tag[ocrypto_chacha20_poly1305_TAG_BYTES],
    uint8_t *m,
    const uint8_t *c, size_t c_len,
    const uint8_t *a, size_t a_len,
    const uint8_t *n, size_t n_len,
    const uint8_t k[ocrypto_chacha20_poly1305_KEY_BYTES]);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
