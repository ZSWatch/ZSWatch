/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

 
/**
 * @defgroup ocrypto_sha_3_sha_3 SHA3
 * @ingroup ocrypto_sha
 * @{
 * @brief SHA3 algorithms, with 224, 256, 384 and 512 bit outputs.
 *
 * The SHA3 family is a set of cryptographic hash functions initiated
 * by the NSA. It is the successor of the SHA1 and SHA2 algorithms.
 *
 * A fixed-sized message digest is computed from variable length input data.
 * The function is practically impossible to revert, and small changes in the
 * input message lead to major changes in the message digest.
 * 
 * @see [FIPS - SHA-3 Standard: Permutation-Based Hash and Extendable-Output Functions](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf)
 */

/**
 * @file
 * @brief SHA3 algorithms, with 224, 256, 384 and 512 bit outputs.
 */

#ifndef OCRYPTO_SHA3_H
#define OCRYPTO_SHA3_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of SHA3-224 hash.
 */
#define ocrypto_sha3_224_BYTES (28)

/**
 * Length of SHA3-256 hash.
 */
#define ocrypto_sha3_256_BYTES (32)

/**
 * Length of SHA3-384 hash.
 */
#define ocrypto_sha3_384_BYTES (48)

/**
 * Length of SHA3-512 hash.
 */
#define ocrypto_sha3_512_BYTES (64)


/**@cond */
typedef struct {
    uint64_t s[25];
    uint8_t  data[8];
    uint8_t  length;
    uint8_t  index;
} ocrypto_sha3_ctx;
/**@endcond */


/**
 * @name Incremental SHA3 generator
 * 
 * This group of functions can be used to incrementally compute the SHA3
 * hash for a given message.
 */
/**@{*/

/**
 * SHA3 initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx Generator state.
 */
void ocrypto_sha3_init(
    ocrypto_sha3_ctx *ctx);

/**
 * SHA3-224 incremental data input.
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
 *         @c ocrypto_sha3_init is required before this function can be called.
 */
void ocrypto_sha3_224_update(
    ocrypto_sha3_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * SHA3-256 incremental data input.
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
 *         @c ocrypto_sha3_init is required before this function can be called.
 */
void ocrypto_sha3_256_update(
    ocrypto_sha3_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * SHA3-384 incremental data input.
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
 *         @c ocrypto_sha3_init is required before this function can be called.
 */
void ocrypto_sha3_384_update(
    ocrypto_sha3_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * SHA3-512 incremental data input.
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
 *         @c ocrypto_sha3_init is required before this function can be called.
 */
void ocrypto_sha3_512_update(
    ocrypto_sha3_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * SHA3-224 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The hash is put into @p r.
 *
 * @param      ctx Generator state.
 * @param[out] r   Generated hash value.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_sha3_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_sha3_224_update and @c ocrypto_sha3_224_final unless it is
 *         reinitialized using @c ocrypto_sha3_init.
 */
void ocrypto_sha3_224_final(
    ocrypto_sha3_ctx *ctx,
    uint8_t r[ocrypto_sha3_224_BYTES]);

/**
 * SHA3-256 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The hash is put into @p r.
 *
 * @param      ctx Generator state.
 * @param[out] r   Generated hash value.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_sha3_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_sha3_256_update and @c ocrypto_sha3_256_final unless it is
 *         reinitialized using @c ocrypto_sha3_init.
 */
void ocrypto_sha3_256_final(
    ocrypto_sha3_ctx *ctx,
    uint8_t r[ocrypto_sha3_256_BYTES]);

/**
 * SHA3-384 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The hash is put into @p r.
 *
 * @param      ctx Generator state.
 * @param[out] r   Generated hash value.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_sha3_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_sha3_384_update and @c ocrypto_sha3_384_final unless it is
 *         reinitialized using @c ocrypto_sha3_init.
 */
void ocrypto_sha3_384_final(
    ocrypto_sha3_ctx *ctx,
    uint8_t r[ocrypto_sha3_384_BYTES]);

/**
 * SHA3-512 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The hash is put into @p r.
 *
 * @param      ctx Generator state.
 * @param[out] r   Generated hash value.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_sha3_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_sha3_512_update and @c ocrypto_sha3_512_final unless it is
 *         reinitialized using @c ocrypto_sha3_init.
 */
void ocrypto_sha3_512_final(
    ocrypto_sha3_ctx *ctx,
    uint8_t r[ocrypto_sha3_512_BYTES]);
/**@}*/

/**
 * SHA3-224 hash.
 *
 * The SHA3-224 hash of a given input message @p in is computed and put into @p r.
 *
 * @param[out] r      Generated hash.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_sha3_224(
    uint8_t r[ocrypto_sha3_224_BYTES],
    const uint8_t *in, size_t in_len);

/**
 * SHA3-256 hash.
 *
 * The SHA3-256 hash of a given input message @p in is computed and put into @p r.
 *
 * @param[out] r      Generated hash.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_sha3_256(
    uint8_t r[ocrypto_sha3_256_BYTES],
    const uint8_t *in, size_t in_len);

/**
 * SHA3-384 hash.
 *
 * The SHA3-384 hash of a given input message @p in is computed and put into @p r.
 *
 * @param[out] r      Generated hash.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_sha3_384(
    uint8_t r[ocrypto_sha3_384_BYTES],
    const uint8_t *in, size_t in_len);

/**
 * SHA3-512 hash.
 *
 * The SHA3-512 hash of a given input message @p in is computed and put into @p r.
 *
 * @param[out] r      Generated hash.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_sha3_512(
    uint8_t r[ocrypto_sha3_512_BYTES],
    const uint8_t *in, size_t in_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
