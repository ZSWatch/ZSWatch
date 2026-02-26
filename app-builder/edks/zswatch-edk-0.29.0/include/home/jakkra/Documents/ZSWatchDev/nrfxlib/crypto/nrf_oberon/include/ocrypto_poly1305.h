/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_poly1305 Poly1305
 * @ingroup ocrypto_mac
 * @{
 * @brief Poly1305 authentication algorithm.
 *
 * Poly1305 is a message authentication code created by Daniel J.
 *        Bernstein. It can be used to verify the data integrity and the
 *        authenticity of a message.
 *
 * Poly1305 takes a one-time key to produce an authentication tag for a message.
 * Since a key can only be used to authenticate a single message, a new key
 * needs to be derived for each message.
 *
 * @see [RFC - ChaCha20 and Poly1305 for IETF Protocols](http://tools.ietf.org/html/rfc8439)
 * @see [Poly1305-AES: a state-of-the-art message-authentication code](http://cr.yp.to/mac.html)
 */

/**
 * @file
 * @brief Poly1305 authentication algorithm.
 */

#ifndef OCRYPTO_POLY1305_H
#define OCRYPTO_POLY1305_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Key length.
 */
#define ocrypto_poly1305_KEY_BYTES (32)

/**
 * Authenticator length.
 */
#define ocrypto_poly1305_BYTES (16)


/**@cond */
typedef struct {
    uint32_t h[5];
    uint32_t r[4];
    uint32_t c[4];
    uint8_t  buffer[16];
    uint8_t  length;   // Data length in buffer.
} ocrypto_poly1305_ctx;
/**@endcond */


/**
 * @name Incremental Poly1305 generator
 *
 * This group of functions can be used to incrementally compute 
 * the Poly1305 authenticator on a message.

 * Use pattern:
 *
 * @code
 * ocrypto_poly1305_init(ctx, key);
 * ocrypto_poly1305_update(ctx, m, m_len);
 * ...
 * ocrypto_chacha20_update(ctx, m, m_len);
 * ocrypto_poly1305_final(ctx, tag);
 * @endcode
 */
/**@{*/

/**
 * Poly1305 generator initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx Generator state.
 * @param      key Authentication key.
 */
void ocrypto_poly1305_init(
    ocrypto_poly1305_ctx *ctx,
    const uint8_t key[ocrypto_poly1305_KEY_BYTES]);

/**
 * Poly1305 generator.
 *
 * The generator state @p ctx is updated to authenticate a message chunk @p in.
 *
 * This function can be called repeatedly on arbitrarily small chunks of the 
 * message until the whole message has been processed.
 *
 * @param      ctx    Generator state.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_poly1305_init is required before this function can be called.
 */
void ocrypto_poly1305_update(
    ocrypto_poly1305_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * Poly1305 generator padding.
 *
 * The generator state @p ctx is updated to authenticate a zero padding of the last block.
 *
 * @param      ctx    Generator state.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_poly1305_init is required before this function can be called.
 */
void ocrypto_poly1305_pad(
    ocrypto_poly1305_ctx *ctx);

/**
 * Poly1305 generator output.
 *
 * The generator state @p ctx is updated to finalize the authenticator for the
 * previously processed message chunks. The authentication tag is put into @p r.
 *
 * @param      ctx Generator state.
 * @param[out] r   Generated authentication tag.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_poly1305_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_poly1305_update and @c ocrypto_poly1305_final unless it is
 *         reinitialized using @c ocrypto_poly1305_init.
 */
void ocrypto_poly1305_final(
    ocrypto_poly1305_ctx *ctx,
    uint8_t r[ocrypto_poly1305_BYTES]);
/**@}*/

/**
 * Poly1305 message authentication tag.
 *
 * The Poly1305 authentication of a given input message @p in is computed using
 * key @p k and put into @p r.
 *
 * @param[out] r      Generated authentication tag.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 * @param      k      Authentication key.
 */
void ocrypto_poly1305(
    uint8_t r[ocrypto_poly1305_BYTES],
    const uint8_t *in, size_t in_len,
    const uint8_t k[ocrypto_poly1305_KEY_BYTES]);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
