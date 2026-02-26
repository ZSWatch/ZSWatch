/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

 
/**
 * @defgroup ocrypto_sha_3_shake SHAKE
 * @ingroup ocrypto_sha
 * @{
 * @brief SHAKE algorithms, variants of SHA3 with variable output lengths, with 128 and 256 bit security.
 *
 * The SHAKE family is a variable output variant of the SHA3 hash function family.
 *
 * A variable length message digest is computed from variable length input data.
 * The function is practically impossible to revert, and small changes in the
 * input message lead to major changes in the message digest.
 * 
 * @see [FIPS - SHA-3 Standard: Permutation-Based Hash and Extendable-Output Functions](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf)
 */

/**
 * @file
 * @brief SHAKE algorithms, variants of SHA3 with variable output lengths, with 128 and 256 bit security.
 */

#ifndef OCRYPTO_SHAKE_H
#define OCRYPTO_SHAKE_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_sha3.h"


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef ocrypto_sha3_ctx ocrypto_shake_ctx;
/**@endcond */


/**
 * @name Incremental SHAKE generator
 * 
 * This group of functions can be used to incrementally compute the SHAKE
 * hash for a given message.
 * 
 * @{
 */

/**
 * SHAKE initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx Generator state.
 */
void ocrypto_shake_init(
    ocrypto_shake_ctx *ctx);

/**
 * SHAKE128 incremental data input.
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
 *         @c ocrypto_shake_init is required before this function can be called.
 */
void ocrypto_shake128_update(
    ocrypto_shake_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * SHAKE256 incremental data input.
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
 *         @c ocrypto_shake_init is required before this function can be called.
 */
void ocrypto_shake256_update(
    ocrypto_shake_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * SHAKE128 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The variable length output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated hash value.
 * @param      r_len Length of @p r. May be 0.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_shake_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_shake_128_update and @c ocrypto_shake128_final but can still
 *         be used with @c ocrypto_shake128_ext to generate more output.
 */
void ocrypto_shake128_final(
    ocrypto_shake_ctx *ctx,
    uint8_t *r, size_t r_len);

/**
 * SHAKE256 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The variable length output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated hash value.
 * @param      r_len Length of @p r. May be 0.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_shake_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_shake_256_update and @c ocrypto_shake256_final but can still
 *         be used with @c ocrypto_shake256_ext to generate more output.
 */
void ocrypto_shake256_final(
    ocrypto_shake_ctx *ctx,
    uint8_t *r, size_t r_len);

/**
 * SHAKE128 extended output.
 *
 * Generates additional output data of length @p r_len and updates the generator state @p ctx
 * accordingly. The variable length output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated output.
 * @param      r_len Length of @p r.
 *
 * @remark Initialization of the generator state @p ctx through @c ocrypto_shake_init,
 *         @c ocrypto_shake128_update, and @c ocrypto_shake128_final is required
 *         before this function can be called.
 */
void ocrypto_shake128_ext(
    ocrypto_shake_ctx *ctx,
    uint8_t *r, size_t r_len);

/**
 * SHAKE256 extended output.
 *
 * Generates additional output data of length @p r_len and updates the generator state @p ctx
 * accordingly. The variable length output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated output.
 * @param      r_len Length of @p r.
 *
 * @remark Initialization of the generator state @p ctx through @c ocrypto_shake_init,
 *         @c ocrypto_shake256_update, and @c ocrypto_shake256_final is required
 *         before this function can be called.
 */
void ocrypto_shake256_ext(
    ocrypto_shake_ctx *ctx,
    uint8_t *r, size_t r_len);
/**@}*/

/**
 * SHAKE128 extendable-output function.
 *
 * The SHAKE128 hash of a given input message @p in is computed and put into @p r.
 *
 * @param[out] r      Generated hash.
 * @param      r_len  Length of @p r.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_shake128(
    uint8_t *r, size_t r_len,
    const uint8_t *in, size_t in_len);

/**
 * SHAKE256 extendable-output function.
 *
 * The SHAKE256 hash of a given input message @p in is computed and put into @p r.
 *
 * @param[out] r      Generated hash.
 * @param      r_len  Length of @p r.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_shake256(
    uint8_t *r, size_t r_len,
    const uint8_t *in, size_t in_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
