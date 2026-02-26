/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_hmac_sha256 HMAC-SHA256
 * @ingroup ocrypto_mac
 * @{
 * @brief HMAC algorithm based on SHA256.
 *
 * HMAC-SHA256 is an algorithm for message authentication using the
 * cryptographic hash function SHA256 and a reusable secret key. Users in
 * possession of the key can verify the integrity and authenticity of the
 * message.
 *
 * @see [RFC - HMAC: Keyed-Hashing for Message Authentication](http://tools.ietf.org/html/rfc2104)
 */

/**
 * @file
 * @brief HMAC algorithm based on SHA256.
 */

#ifndef OCRYPTO_HMAC_SHA256_H
#define OCRYPTO_HMAC_SHA256_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_sha256.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of the authenticator.
 */
#define ocrypto_hmac_sha256_BYTES (32)


/**@cond */
typedef struct {
    ocrypto_sha256_ctx sha;
    uint8_t k[64];
} ocrypto_hmac_sha256_ctx;
/**@endcond */


/**
 * @name Incremental HMAC-SHA256 generator
 *
 * This group of functions can be used to incrementally compute the HMAC-SHA256
 * authenticator for a given message.
 */
/**@{*/

/**
 * HMAC-SHA256 initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx Generator state.
 * @param      key     HMAC key.
 * @param      key_len Length of @p key.
 */
void ocrypto_hmac_sha256_init(
    ocrypto_hmac_sha256_ctx *ctx,
    const uint8_t* key, size_t key_len);

/**
 * HMAC-SHA256 incremental data input.
 *
 * The generator state @p ctx is updated to authenticate a message chunk @p in.
 *
 * This function can be called repeatedly until the whole message is processed.
 *
 * @param      ctx    Generator state.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_hmac_sha256_init is required before this function can be called.
 */
void ocrypto_hmac_sha256_update(
    ocrypto_hmac_sha256_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * HMAC-SHA256 output.
 *
 * The generator state @p ctx is updated to finalize the HMAC for the previously
 * processed message chunks. The authenticator is put into @p r.
 *
 * @param      ctx Generator state.
 * @param[out] r   Generated HMAC value.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_hmac_sha256_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_hmac_sha256_update and @c ocrypto_hmac_sha256_final unless it is
 *         reinitialized using @c ocrypto_hmac_sha256_init.
 */
void ocrypto_hmac_sha256_final(
    ocrypto_hmac_sha256_ctx *ctx,
    uint8_t r[ocrypto_hmac_sha256_BYTES]);
/** @} */

/**
 * HMAC-SHA256 algorithm.
 *
 * The input message @p in is authenticated using the key @p key. The computed
 * authenticator is put into @p r. To verify the authenticator, the recipient
 * needs to recompute the HMAC authenticator and can then compare it with the
 * received authenticator.
 *
 * @param[out] r       HMAC output.
 * @param      key     HMAC key.
 * @param      key_len Length of @p key.
 * @param      in      Input data.
 * @param      in_len  Length of @p in.
 */
void ocrypto_hmac_sha256(
    uint8_t r[ocrypto_hmac_sha256_BYTES],
    const uint8_t* key, size_t key_len,
    const uint8_t* in, size_t in_len);

/**
 * HMAC-SHA256 algorithm with AAD.
 *
 * @param[out] r       HMAC output
 * @param      key     HMAC key.
 * @param      key_len Length of @p key.
 * @param      in      Input data.
 * @param      in_len  Length of @p in.
 * @param      aad     Additional authentication data. May be NULL.
 * @param      aad_len Length of @p aad.
 */
void ocrypto_hmac_sha256_aad(
    uint8_t r[ocrypto_hmac_sha256_BYTES],
    const uint8_t* key, size_t key_len,
    const uint8_t* in, size_t in_len,
    const uint8_t* aad, size_t aad_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
