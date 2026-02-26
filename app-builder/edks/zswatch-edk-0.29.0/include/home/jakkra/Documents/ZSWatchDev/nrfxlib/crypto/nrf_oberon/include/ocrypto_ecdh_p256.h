/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ecdh_p256 ECDH P-256
 * @ingroup ocrypto_key_agreement
 * @{
 * @brief Elliptic curve Diffie-Hellman algorithm based on the NIST P-256 curve (aka secp256r1).
 *
 * ECDH P-256 is a specific implementation of a key agreement protocol.
 * 
 * @see [NIST - SP 800-56A Rev. 3](https://csrc.nist.gov/pubs/sp/800/56/a/r3/final)
 */

/**
 * @file
 * @brief Elliptic curve Diffie-Hellman algorithm based on the NIST P-256 curve (aka secp256r1).
 */

#ifndef OCRYPTO_ECDH_P256_H
#define OCRYPTO_ECDH_P256_H

#include "ocrypto_types_p256.h"


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef struct {
    ocrypto_p256_mult_ctx mul;
    int ret;
} ocrypto_ecdh_p256_ctx;
/**@endcond */


/**
 * ECDH P-256 secret key check.
 *
 * @param     sk Secret key to check.
 *
 * @retval 0  If @p sk is a valid secret key.
 * @retval -1 Otherwise.
 *
 * @remark To generate a valid secret key use the following code pattern:
 *            @code{.c}
 *            do get_random(sk); while (ocrypto_ecdh_p256_secret_key_check(sk));
 *            @endcode
 */
int ocrypto_ecdh_p256_secret_key_check(const uint8_t sk[32]);

/**
 * ECDH P-256 public key check.
 *
 * @param     pk Public key to check.
 *
 * @retval 0  If @p pk is a valid public key.
 * @retval -1 Otherwise.
 */
int ocrypto_ecdh_p256_public_key_check(const uint8_t pk[64]);

/**
 * ECDH P-256 public key generation.
 *
 * Given a secret key @p sk the corresponding public key is computed and put
 * into @p pk.
 *
 * @param[out] pk Generated public key.
 * @param      sk Secret key. Must be pre-filled with random data.
 *
 * @retval 0  If @p sk is a valid secret key.
 * @retval -1 Otherwise.
 *
 * @remark @p pk may be same as @p sk.
 * @remark To generate a valid key pair use the following code pattern:
 *            @code{.c}
 *            do get_random(sk); while (ocrypto_ecdh_p256_public_key(pk, sk));
 *            @endcode
 */
int ocrypto_ecdh_p256_public_key(uint8_t pk[64], const uint8_t sk[32]);

/**
 * ECDH P-256 common secret.
 *
 * The common secret is computed from both the client's public key @p pk
 * and the server's secret key @p sk and put into @p r.
 *
 * @param[out] r  Generated common secret.
 * @param      sk Server private key.
 * @param      pk Client public key.
 *
 * @retval 0  If @p sk is a valid secret key and @p pk is a valid public key.
 * @retval -1 Otherwise.
 *
 * @remark @p r may be same as @p sk or @p pk.
 */
int ocrypto_ecdh_p256_common_secret(uint8_t r[32], const uint8_t sk[32], const uint8_t pk[64]);


/**
 * @name Incremental ECDH P-256 calculation
 *
 * This group of functions can be used to incrementally calculate
 * the ECDH P-256 public key and common secret.
 * Each call completes in less than 25ms on a 16MHz Cortex-M0.
 *
 * Use pattern:
 *
 * Public Key:
 * @code{.c}
 *   ocrypto_ecdh_p256_public_key_init(ctx, sKey);
 *   while (ocrypto_ecdh_p256_public_key_iterate(ctx));
 *   res = ocrypto_ecdh_p256_public_key_final(ctx, pKey);
 * @endcode
 * Common Secret:
 * @code{.c}
 *   ocrypto_ecdh_p256_common_secret_init(ctx, sKey, pKey);
 *   while (ocrypto_ecdh_p256_common_secret_iterate(ctx));
 *   res = ocrypto_ecdh_p256_common_secret_final(ctx, secet);
 * @endcode
 */
/**@{*/

/**
 * Incremental ECDH P-256 public key generation start.
 *
 * Key generation is started and the context @p ctx is initialized by this function.
 *
 * @param[out] ctx Context.
 * @param      sk  Secret key. Must be pre-filled with random data.
 */
void ocrypto_ecdh_p256_public_key_init(ocrypto_ecdh_p256_ctx *ctx, const uint8_t sk[32]);

/**
 * Incremental ECDH P-256 public key generation step.
 *
 * The key calculation is advanced and the context @p ctx is updated by this function.
 *
 * @param     ctx Context.
 *
 * @retval 1  If another call to @c ocrypto_ecdh_p256_public_key_init is needed.
 * @retval 0  If key generation should be completed by a call to @c ocrypto_ecdh_p256_public_key_final.
 */
int ocrypto_ecdh_p256_public_key_iterate(ocrypto_ecdh_p256_ctx *ctx);

/**
 * Incremental ECDH P-256 public key generation final step.
 *
 * Key generation is finalized and the context @p ctx is used to generate the key.
 *
 * @param      ctx Context.
 * @param[out] pk  Generated public key.
 *
 * @retval 0  If @p sk is a valid secret key.
 * @retval -1 Otherwise.
 */
int ocrypto_ecdh_p256_public_key_final(ocrypto_ecdh_p256_ctx *ctx, uint8_t pk[64]);

/**
 * Incremental ECDH P-256 common secret generation start.
 *
 * Common secret calculation is started and the context @p ctx is initialized by this function.
 *
 * @param[out] ctx Context.
 * @param      sk  Server private key.
 * @param      pk  Client public key.
 */
void ocrypto_ecdh_p256_common_secret_init(ocrypto_ecdh_p256_ctx *ctx, const uint8_t sk[32], const uint8_t pk[64]);

/**
 * Incremental ECDH P-256 common secret generation step.
 *
 * Common secret calculation is advanced and the context @p ctx is updated by this function.
 *
 * @param     ctx Context.
 *
 * @retval 1  If another call to @c ocrypto_ecdh_p256_common_secret_iterate is needed.
 * @retval 0  If key generation should be completed by a call to @c ocrypto_ecdh_p256_common_secret_final.
 */
int ocrypto_ecdh_p256_common_secret_iterate(ocrypto_ecdh_p256_ctx *ctx);

/**
 * Incremental ECDH P-256 common secret generation final step.
 *
 * Common secret calculation is finalized and the context @p ctx is used to generate the secret.
 *
 * @param      ctx Context.
 * @param[out] r   Generated common secret.
 *
 * @retval 0  If @p sk is a valid secret key and @p pk is a valid public key.
 * @retval -1 Otherwise.
 */
int ocrypto_ecdh_p256_common_secret_final(ocrypto_ecdh_p256_ctx *ctx, uint8_t r[32]);
/**@}*/


#ifdef __cplusplus
}
#endif

#endif

/** @} */
