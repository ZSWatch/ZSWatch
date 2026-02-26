/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_chacha ChaCha20 and XChaCha20
 * @ingroup ocrypto_unauth_enc
 * @{
 * @brief ChaCha20 and XChaCha20 algorithms.
 *
 * ChaCha20 is a stream cipher developed by Daniel J. Bernstein based on the 20-round cipher
 * Salsa20/20.
 * 
 * A 256-bit key is expanded into 2^64 randomly accessible streams, each
 * containing 2^64 randomly accessible 64-byte (512-bit) blocks.
 *
 * The changes from Salsa20/20 to ChaCha20 are designed to improve diffusion per
 * round, conjecturally increasing resistance to cryptanalysis, while
 * preserving - and often improving - time per round.
 *
 * XChaCha20 uses a 192-bit nonce instead of ChaCha20's 96-bit nonce.
 * 
 * @see [XChaCha](https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-xchacha-03)
 * @see [RFC - ChaCha20 and Poly1305 for IETF Protocols](http://tools.ietf.org/html/rfc8439)
 * @see [The ChaCha family of stream ciphers](http://cr.yp.to/chacha.html)
 */

/**
 * @file
 * @brief ChaCha20 and XChaCha20 algorithms.
 */

#ifndef OCRYPTO_CHACHA20_H
#define OCRYPTO_CHACHA20_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of the encryption key.
 */
#define ocrypto_chacha20_KEY_BYTES (32)

/**
 * Maximum length of the nonce.
 */
#define ocrypto_chacha20_NONCE_BYTES_MAX (12)


/**@cond */
typedef struct {
    uint32_t x[16];    // generator state
    uint8_t  cipher[64];
    uint8_t  position; // Current position in cipher.
} ocrypto_chacha20_ctx;
/**@endcond */


/**
 * @name Incremental ChaCha20 Encoder
 *
 * This group of functions can be used to incrementally compute the ChaCha20 encryption
 * or decryption for a given message and key, by segmenting a message into smaller chunks.
 *
 * Use pattern:
 *
 * Encoding/Decoding:
 * @code
 * ocrypto_chacha20_init(ctx, n, n_len, key, count);
 * ocrypto_chacha20_update(ctx, c, m, m_len);
 * ...
 * ocrypto_chacha20_update(ctx, c, m, m_len);
 * @endcode
 */
/**@{*/

/**
 * ChaCha20 encoder/decoder initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx   Encoder/decoder state.
 * @param      n     Nonce (or IV). May be NULL.
 * @param      n_len Nonce length. @p n_len = 8, 12, or 24.
 * @param      key   Authentication key. May be NULL.
 * @param      count Initial block counter, usually 0 or 1.
 *
 * @remark If @p key is NULL only @p n and @p count are set. If @p n is NULL only @p key is set.
 *         Both @p key and @p n must be set before update is called.
 * @remark If @p n_len = 24, the XChaCha20 algorithm is used.
 * @remark When reusing an encryption key @p key for a different message, a
 *         different nonce @p n or initial block counter @p count must be used.
 */
void ocrypto_chacha20_init(
    ocrypto_chacha20_ctx *ctx,
    const uint8_t *n, size_t n_len,
    const uint8_t key[ocrypto_chacha20_KEY_BYTES],
    uint32_t count);

/**
 * ChaCha20 encoder/decoder.
 *
 * The message @p m is ChaCha20 encrypted and the resulting cipher stream
 * is written to @p c. 
 *
 * For decryption, the ciphertext is used as input message @p m and the 
 * resulting decrypted message is written to @p c.
 *
 * This function can be called repeatedly on arbitrarily small chunks of a larger
 * message until the whole message has been processed.
 *
 * @param      ctx   Encoder/decoder state.
 * @param[out] c     Generated output (ciphertext or plaintext). Same length as input message.
 * @param      m     Input message.
 * @param      m_len Length of @p c and @p m; @p m_len < 2^38 bytes.

 * @remark Initialization of the encoder state @p ctx through
 *         @c ocrypto_chacha20_init is required before this function can be called.
 */
void ocrypto_chacha20_update(
    ocrypto_chacha20_ctx *ctx,
    uint8_t *c,
    const uint8_t *m, size_t m_len);
/**@}*/

/**
 * ChaCha20 cipher stream encoder/decoder.
 *
 * The message @p m is encrypted by applying the XOR operation with a pseudo
 * random cipher stream derived from the encryption key @p key, the nonce @p n, and
 * the initial block counter @p count.
 *
 * For decryption, the ciphertext is used as input message @p m and the 
 * resulting decrypted message is written to @p c.
 *
 * @param[out] c     Generated ciphertext. Same length as input message.
 * @param      m     Input message.
 * @param      m_len Length of @p c and @p m.
 * @param      n     Nonce (or IV).
 * @param      n_len Nonce length. @p n_len = 8, 12, or 24.
 * @param      key   Encryption key.
 * @param      count Initial block counter.
 *
 * @remark @p c may be same as @p m.
 *
 * @remark When reusing an encryption key @p key for a different message @p m, a
 *         different nonce @p n or initial block counter @p count must be used.
 * @remark If @p n_len = 24, the XChaCha20 algorithm is used.
 */
void ocrypto_chacha20_encode(
    uint8_t *c,
    const uint8_t *m, size_t m_len,
    const uint8_t *n, size_t n_len,
    const uint8_t key[ocrypto_chacha20_KEY_BYTES],
    uint32_t count);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
