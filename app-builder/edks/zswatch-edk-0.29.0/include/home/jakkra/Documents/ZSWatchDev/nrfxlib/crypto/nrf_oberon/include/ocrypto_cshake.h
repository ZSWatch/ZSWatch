/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

 
/**
 * @defgroup ocrypto_sha_3_cshake cSHAKE
 * @ingroup ocrypto_sha
 * @brief cSHAKE variants of the SHA3 algorithms, with variable output lengths.
 * 
 * @{
 * The cSHAKE family is a variable output variant of the SHA3 hash functions
 * with additional inputs, with 128 and 256 bit security.
 *
 * A fixed-sized message digest is computed from variable length input data,
 * a function name, and a customization string. 
 * The function is practically impossible to revert, and small changes in the
 * input message lead to major changes in the message digest.
 * 
 * @see [FIPS - SHA-3 Standard: Permutation-Based Hash and Extendable-Output Functions](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf)
 * @see [NIST - SHA-3 Derived Functions: cSHAKE, KMAC, TupleHash and ParallelHash](https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-185.pdf)
 */

/**
 * @file
 * @brief cSHAKE variants of the SHA3 algorithms, with variable output lengths.
 */

#ifndef OCRYPTO_CSHAKE_H
#define OCRYPTO_CSHAKE_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_sha3.h"


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef ocrypto_sha3_ctx ocrypto_cshake_ctx;
/**@endcond */


/**
 * @name Incremental cSHAKE generator
 * 
 * This group of functions can be used to incrementally compute the cSHAKE
 * hash for a given input.
 * 
 * @{
 */

/**
 * cSHAKE128 initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx    Generator state.
 * @param      n      Function name bit string.
 * @param      n_len  Length of @p n.
 * @param      s      Customization bit string.
 * @param      s_len  Length of @p s.
 */
void ocrypto_cshake128_init(
    ocrypto_cshake_ctx *ctx,
    const uint8_t *n, size_t n_len,
    const uint8_t *s, size_t s_len);

/**
 * cSHAKE256 initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx    Generator state.
 * @param      n      Function name bit string.
 * @param      n_len  Length of @p n.
 * @param      s      Customization bit string.
 * @param      s_len  Length of @p s.
 */
void ocrypto_cshake256_init(
    ocrypto_cshake_ctx *ctx,
    const uint8_t *n, size_t n_len,
    const uint8_t *s, size_t s_len);

/**
 * cSHAKE128 incremental data input.
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
 *         @c ocrypto_cshake128_init is required before this function can be called.
 */
void ocrypto_cshake128_update(
    ocrypto_cshake_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * cSHAKE256 incremental data input.
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
 *         @c ocrypto_cshake256_init is required before this function can be called.
 */
void ocrypto_cshake256_update(
    ocrypto_cshake_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * cSHAKE128 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The variable length output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated hash value.
 * @param      r_len Length of @p r. May be 0.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_cshake128_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_cshake128_update and @c ocrypto_cshake128_final but can still
 *         be used with @c ocrypto_cshake128_ext to generate more output.
 */
void ocrypto_cshake128_final(
    ocrypto_cshake_ctx *ctx,
    uint8_t *r, size_t r_len);

/**
 * cSHAKE256 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The variable length output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated hash value.
 * @param      r_len Length of @p r. May be 0.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_cshake256_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_cshake256_update and @c ocrypto_cshake256_final but can still
 *         be used with @c ocrypto_cshake256_ext to generate more output.
 */
void ocrypto_cshake256_final(
    ocrypto_cshake_ctx *ctx,
    uint8_t *r, size_t r_len);

/**
 * cSHAKE128 extended output.
 *
 * Generates additional output data of length @p r_len and updates the generator state @p ctx
 * accordingly. The output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated output.
 * @param      r_len Length of @p r.
 *
 * @remark Initialization of the generator state @p ctx through @c ocrypto_cshake128_init,
 *         @c ocrypto_cshake128_update, and @c ocrypto_cshake128_final is required
 *         before this function can be called.
 */
void ocrypto_cshake128_ext(
    ocrypto_cshake_ctx *ctx,
    uint8_t *r, size_t r_len);

/**
 * cSHAKE256 extended output.
 *
 * Generates additional output data of length @p r_len and updates the generator state @p ctx
 * accordingly. The output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated output.
 * @param      r_len Length of @p r.
 *
 * @remark Initialization of the generator state @p ctx through @c ocrypto_cshake256_init,
 *         @c ocrypto_cshake256_update, and @c ocrypto_cshake256_final is required
 *         before this function can be called.
 */
void ocrypto_cshake256_ext(
    ocrypto_cshake_ctx *ctx,
    uint8_t *r, size_t r_len);
/**@}*/

/**
 * cSHAKE128 extendable-output function.
 *
 * The cSHAKE128 hash of a given input message @p in is computed and put into @p r.
 *
 * @remark @p n_len or @p s_len must be > 0. For empty @p n and @p s use @c shake128.
 *
 * @param[out] r      Generated hash.
 * @param      r_len  Length of @p r.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 * @param      n      Function name bit string.
 * @param      n_len  Length of @p n.
 * @param      s      Customization bit string.
 * @param      s_len  Length of @p s.
 */
void ocrypto_cshake128(
    uint8_t *r, size_t r_len,
    const uint8_t *in, size_t in_len,
    const uint8_t *n, size_t n_len,
    const uint8_t *s, size_t s_len);

/**
 * cSHAKE256 extendable-output function.
 *
 * The cSHAKE256 hash of a given input message @p in is computed and put into @p r.
 *
 * @remark @p n_len or @p s_len must be > 0. For empty @p n and @p s use @c shake256.
 *
 * @param[out] r      Generated hash.
 * @param      r_len  Length of @p r.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 * @param      n      Function name bit string.
 * @param      n_len  Length of @p n.
 * @param      s      Customization bit string.
 * @param      s_len  Length of @p s.
 */
void ocrypto_cshake256(
    uint8_t *r, size_t r_len,
    const uint8_t *in, size_t in_len,
    const uint8_t *n, size_t n_len,
    const uint8_t *s, size_t s_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
