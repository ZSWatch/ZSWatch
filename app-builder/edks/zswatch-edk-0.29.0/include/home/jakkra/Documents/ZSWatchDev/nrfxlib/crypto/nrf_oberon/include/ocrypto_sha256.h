/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_sha_256 SHA256
 * @ingroup ocrypto_sha
 * @{
 * @brief SHA256 algorithm, a member of the SHA2 family, with 256 bit outputs.
 *
 * SHA256 is part of the SHA2 family that is a set of cryptographic hash
 * functions designed by the NSA. It is the successor of the SHA1 algorithm.
 *
 * A fixed-sized message digest is computed from variable length input data.
 * The function is practically impossible to revert, and small changes in the
 * input message lead to major changes in the message digest.
 *
 * @see [FIPS - Secure Hash Standard (SHS)](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
 */

/**
 * @file
 * @brief SHA256 algorithm, a member of the SHA2 family, with 256 bit outputs.
 */

#ifndef OCRYPTO_SHA256_H
#define OCRYPTO_SHA256_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of SHA256 hash.
 */
#define ocrypto_sha256_BYTES (32)


/**@cond */
typedef struct {
    uint32_t h[8], v[8], w[16];
    uint8_t  buffer[64];
    uint32_t length;
    size_t   in_length;
} ocrypto_sha256_ctx;
/**@endcond */


/**
 * @name Incremental SHA256 generator
 *
 * This group of functions can be used to incrementally compute the SHA256
 * hash for a given message.
 */
/**@{*/

/**
 * SHA256 initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx Generator state.
 */
void ocrypto_sha256_init(
    ocrypto_sha256_ctx *ctx);

/**
 * SHA256 incremental data input.
 *
 * The generator state @p ctx is updated to hash a message chunk @p in.
 *
 * This function can be called repeatedly until the whole message is processed.
 *
 * @param      ctx    Generator state.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_sha256_init is required before this function can be called.
 */
void ocrypto_sha256_update(
    ocrypto_sha256_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * SHA256 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The hash is put into @p r.
 *
 * @param      ctx Generator state.
 * @param[out] r   Generated hash value.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_sha256_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_sha256_update and @c ocrypto_sha256_final unless it is
 *         reinitialized using @c ocrypto_sha256_init.
 */
void ocrypto_sha256_final(
    ocrypto_sha256_ctx *ctx,
    uint8_t r[ocrypto_sha256_BYTES]);
/**@}*/

/**
 * SHA256 hash.
 *
 * The SHA256 hash of a given input message @p in is computed and put into @p r.
 *
 * @param[out] r      Generated hash.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_sha256(
    uint8_t r[ocrypto_sha256_BYTES],
    const uint8_t *in, size_t in_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
