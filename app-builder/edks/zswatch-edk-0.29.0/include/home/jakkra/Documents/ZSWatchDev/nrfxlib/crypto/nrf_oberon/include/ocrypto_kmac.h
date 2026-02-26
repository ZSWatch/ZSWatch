/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_kmax KMAC-SHA3
 * @ingroup ocrypto_mac
 * @{
 * @brief KMAC algorithm based on SHA3, with 128 and 256 bit security.
 * 
 * The KMAC family is a set of cryptographic MAC functions based on SHA3 and cSHAKE.
 *
 * A fixed-sized message digest is computed from variable length input data.
 * The function is practically impossible to revert, and small changes in the
 * input message lead to major changes in the message digest.
 * 
 * @see [FIPS - SHA-3 Standard: Permutation-Based Hash and Extendable-Output Functions](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf)
 * @see [NIST - SHA-3 Derived Functions: cSHAKE, KMAC, TupleHash and ParallelHash](https://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-185.pdf)
 */

/**
 * @file
 * @brief KMAC algorithm based on SHA3, with 128 and 256 bit security.
 */

#ifndef OCRYPTO_KMAC_H
#define OCRYPTO_KMAC_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_sha3.h"


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef ocrypto_sha3_ctx ocrypto_kmac_ctx;
/**@endcond */


/**
 * @name Incremental KMAC generator
 * 
 * This group of functions can be used to incrementally compute the KMAC
 * authenticator for a given message.
 */
/**@{*/

/**
 * KMAC128 initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx     Generator state.
 * @param      key     Key.
 * @param      key_len Length of @p key.
 * @param      s       Customization bit string.
 * @param      s_len   Length of @p s.
 */
void ocrypto_kmac128_init(
    ocrypto_kmac_ctx *ctx,
    const uint8_t *key, size_t key_len,
    const uint8_t *s, size_t s_len);

/**
 * KMAC256 initialization.
 *
 * The generator state @p ctx is initialized by this function.
 *
 * @param[out] ctx     Generator state.
 * @param      key     Key.
 * @param      key_len Length of @p key.
 * @param      s       Customization bit string.
 * @param      s_len   Length of @p s.
 */
void ocrypto_kmac256_init(
    ocrypto_kmac_ctx *ctx,
    const uint8_t *key, size_t key_len,
    const uint8_t *s, size_t s_len);

/**
 * KMAC128 incremental data input.
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
 *         @c ocrypto_kmac128_init is required before this function can be called.
 */
void ocrypto_kmac128_update(
    ocrypto_kmac_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * KMAC256 incremental data input.
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
 *         @c ocrypto_kmac256_init is required before this function can be called.
 */
void ocrypto_kmac256_update(
    ocrypto_kmac_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * KMAC256 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The variable length output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated mac value.
 * @param      r_len Length of @p r.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_kmac128_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_kmac128_update and @c ocrypto_kmac128_final unless it is
 *         reinitialized using @c ocrypto_kmac128_init.
 */
void ocrypto_kmac128_final(
    ocrypto_kmac_ctx *ctx,
    uint8_t *r, size_t r_len);

/**
 * KMAC256 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The variable length output is put into @p r.
 *
 * @param      ctx   Generator state.
 * @param[out] r     Generated mac value.
 * @param      r_len Length of @p r.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_kmac256_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_kmac256_update and @c ocrypto_kmac256_final unless it is
 *         reinitialized using @c ocrypto_kmac256_init.
 */
void ocrypto_kmac256_final(
    ocrypto_kmac_ctx *ctx,
    uint8_t *r, size_t r_len);
/**@}*/

/**
 * KMAC128 Message Authentication Code function.
 *
 * The KMAC128 output of a given input message @p in and key @p key is computed and put into @p r.
 *
 * @param[out] r       Generated mac.
 * @param      r_len   Length of @p r.
 * @param      key     Key.
 * @param      key_len Length of @p key.
 * @param      in      Input data.
 * @param      in_len  Length of @p in.
 * @param      s       Customization bit string.
 * @param      s_len   Length of @p s.
 */
void ocrypto_kmac128(
    uint8_t *r, size_t r_len,
    const uint8_t *key, size_t key_len,
    const uint8_t *in, size_t in_len,
    const uint8_t *s, size_t s_len);

/**
 * KMAC256 Message Authentication Code function.
 *
 * The KMAC256 output of a given input message @p in and key @p key is computed and put into @p r.
 *
 * @param[out] r       Generated mac.
 * @param      r_len   Length of @p r.
 * @param      key     Key.
 * @param      key_len Length of @p key.
 * @param      in      Input data.
 * @param      in_len  Length of @p in.
 * @param      s       Customization bit string.
 * @param      s_len   Length of @p s.
 */
void ocrypto_kmac256(
    uint8_t *r, size_t r_len,
    const uint8_t *key, size_t key_len,
    const uint8_t *in, size_t in_len,
    const uint8_t *s, size_t s_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
