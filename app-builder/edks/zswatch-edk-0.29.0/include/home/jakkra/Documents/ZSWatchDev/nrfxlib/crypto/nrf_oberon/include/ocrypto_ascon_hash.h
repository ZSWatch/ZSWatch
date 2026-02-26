/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ascon_hash Ascon-Hash256
 * @ingroup ocrypto_sha
 * @{
 * @brief Ascon-Hash is a cryptographic hash function designed for small devices.
 *
 * A fixed- or variable-sized (XOF, CXOF) message digest is computed from
 * variable length input data. The function is practically impossible to revert,
 * and small changes in the input message lead to major changes in the message
 * digest.
 * 
 * @see [NIST SP 800-232](https://csrc.nist.gov/pubs/sp/800/232/final)
 */

/**
 * @file
 * @brief Ascon-Hash is a cryptographic hash function designed for small devices.
 */

#ifndef OCRYPTO_ASCON_HASH_H
#define OCRYPTO_ASCON_HASH_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of the generated hash value in bytes.
 */
#define ocrypto_ascon_hash256_BYTES (32)


/**@cond */
typedef struct {
    uint64_t s[5];
    uint8_t  data[8];
    uint8_t  length;
} ocrypto_ascon_hash_ctx;
/**@endcond */


/**
 * @name Incremental Ascon-Hash generator.
 * 
 * This group of functions can be used to incrementally compute the
 * Ascon-Hash or Ascon-XOF hash for a given message.
 */
/**@{*/

/**
 * Ascon-Hash256 initialization.
 *
 * The generator state @p ctx is initialized for fixed output size.
 *
 * @param[out] ctx Generator state.
 */
void ocrypto_ascon_hash256_init(
    ocrypto_ascon_hash_ctx *ctx);

/**
 * Ascon-XOF128 initialization.
 *
 * The generator state @p ctx is initialized for variable output size.
 *
 * @param[out] ctx Generator state.
 */
void ocrypto_ascon_xof128_init(
    ocrypto_ascon_hash_ctx *ctx);

/**
 * Ascon-CXOF128 initialization.
 *
 * The generator state @p ctx is initialized for variable output size with
 * customization string @p z.
 *
 * @param[out] ctx   Generator state.
 * @param      z     Customization string.
 * @param      z_len Length of @p z. @p z_len <= 256.
 */
void ocrypto_ascon_cxof128_init(
    ocrypto_ascon_hash_ctx *ctx,
    const uint8_t *z, size_t z_len);

/**
 * Ascon-Hash incremental data input.
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
 *         @c ocrypto_ascon_hash256_init is required before this function can be called.
 */
void ocrypto_ascon_hash256_update(
    ocrypto_ascon_hash_ctx *ctx,
    const uint8_t *in, size_t in_len);

/**
 * Ascon-Hash output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The hash is put into fixed size @p h.
 *
 * @param      ctx Generator state.
 * @param[out] h   Generated hash value.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_ascon_hash256_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_ascon_hash256_update and @c ocrypto_ascon_hash256_final unless it is
 *         reinitialized using @c ocrypto_ascon_hash256_init.
 */
void ocrypto_ascon_hash256_final(
    ocrypto_ascon_hash_ctx *ctx,
    uint8_t h[ocrypto_ascon_hash256_BYTES]);

/**
 * Ascon-XOF128 output.
 *
 * The generator state @p ctx is updated to finalize the hash for the previously
 * processed message chunks. The output is put into @p h.
 *
 * @param      ctx   Generator state.
 * @param[out] h     Generated output.
 * @param      h_len Length of @p h. May be 0.
 *
 * @remark Initialization of the generator state @p ctx through
 *         @c ocrypto_ascon_xof128_init is required before this function can be called.
 *
 * @remark After return, the generator state @p ctx must no longer be used with
 *         @c ocrypto_ascon_hash256_update and @c ocrypto_ascon_xof128_final but can still
 *         be used with @c ocrypto_ascon_xof128_ext to generate more output.
 */
void ocrypto_ascon_xof128_final(
    ocrypto_ascon_hash_ctx *ctx,
    uint8_t *h, size_t h_len);

/**
* Ascon-XOF128 extended output.
*
* Generates additional output data of length @p r_len and updates the generator state @p ctx
* accordingly. The output is put into @p h.
*
* @param      ctx   Generator state.
* @param[out] h     Generated output.
* @param      h_len Length of @p r.
*
* @remark Initialization of the generator state @p ctx through @c ocrypto_ascon_xof128_init,
*         @c ocrypto_ascon_hash256_update, and @c ocrypto_ascon_xof128_final is required
*         before this function can be called.
*/
void ocrypto_ascon_xof128_ext(
    ocrypto_ascon_hash_ctx *ctx,
    uint8_t *h, size_t h_len);
/**@}*/

/**
 * Ascon-Hash computation.
 *
 * The Ascon-Hash256 of a given input message @p in is computed and put into @p h.
 *
 * @param[out] h      Generated hash.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_ascon_hash256(
    uint8_t h[ocrypto_ascon_hash256_BYTES],
    const uint8_t *in, size_t in_len);

/**
 * Ascon-XOF computation.
 *
 * The Ascon-XOF128 of a given input message @p in is computed and put into @p h.
 *
 * @param[out] h      Generated hash.
 * @param      h_len  Length of @p h.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_ascon_xof128(
    uint8_t *h, size_t h_len,
    const uint8_t *in, size_t in_len);

/**
 * Ascon-CXOF computation.
 *
 * The Ascon-CXOF128 of a given input message @p in and customized with the
 * string @p z is computed and put into @p h.
 *
 * @param[out] h      Generated hash.
 * @param      h_len  Length of @p h.
 * @param      z      Customization string.
 * @param      z_len  Length of @p z.
 * @param      in     Input data.
 * @param      in_len Length of @p in.
 */
void ocrypto_ascon_cxof128(
    uint8_t *h, size_t h_len,
    const uint8_t *z, size_t z_len,
    const uint8_t *in, size_t in_len);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
