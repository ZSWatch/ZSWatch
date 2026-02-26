/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ed25519ph EdDSA Ed25519ph
 * @ingroup ocrypto_asym_sig
 * @{
 * @brief Ed25519ph algorithm, used for EdDSA with pre-hashing.
 *
 * Ed25519ph is a specific implementation of EdDSA, a digital signature scheme with
 * prehashing. EdDSA is based on Twisted Edwards curves and is designed to be faster
 * than existing digital signature schemes without sacrificing security. It was
 * developed by Daniel J. Bernstein, et al. Ed25519ph is intended to provide
 * attack resistance comparable to quality 128-bit symmetric ciphers.
 * 
 * @see [RFC - Edwards-Curve Digital Signature Algorithm (EdDSA)](https://datatracker.ietf.org/doc/html/rfc8032)
 */

/**
 * @file
 * @brief Ed25519ph algorithm, used for EdDSA with pre-hashing.
 */

#ifndef OCRYPTO_ED25519PH_H
#define OCRYPTO_ED25519PH_H

#include "ocrypto_types_25519.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@cond */
typedef ocrypto_ed25519_ctx ocrypto_ed25519ph_ctx;
/**@endcond */

/**
 * Length of a public key.
 */
#define ocrypto_ed25519ph_PUBLIC_KEY_BYTES (32)

/**
 * Length of a secret key.
 */
#define ocrypto_ed25519ph_SECRET_KEY_BYTES (32)

/**
 * Length of a message hash.
 */
#define ocrypto_ed25519ph_HASH_BYTES (64)

/**
 * Length of a signature.
 */
#define ocrypto_ed25519ph_BYTES (64)


/**
 * Ed25519ph signature key pair generation.
 *
 * Given a secret key @p sk, the corresponding public key is computed and put
 * into @p pk. The key pair can then be used to sign and verify message signatures.
 *
 * @param[out] pk          Generated public key.
 * @param      sk          Secret key. Must be pre-filled with random data.
 */
void ocrypto_ed25519ph_public_key(
    uint8_t pk[ocrypto_ed25519ph_PUBLIC_KEY_BYTES],
    const uint8_t sk[ocrypto_ed25519ph_SECRET_KEY_BYTES]);

/**
 * Ed25519ph signature generate.
 *
 * The message @p m is signed using the secret key @p sk and the corresponding
 * public key @p pk. The signature is put into @p sig.
 *
 * @param[out] sig         Generated signature.
 * @param      hash        Message hash, SHA512(message).
 * @param      sk          Secret key.
 * @param      pk          Public key.
 */
void ocrypto_ed25519ph_sign(
    uint8_t sig[ocrypto_ed25519ph_BYTES],
    const uint8_t hash[ocrypto_ed25519ph_HASH_BYTES],
    const uint8_t sk[ocrypto_ed25519ph_SECRET_KEY_BYTES],
    const uint8_t pk[ocrypto_ed25519ph_PUBLIC_KEY_BYTES]);

/**
 * Ed25519ph signature verification.
 *
 * The signature @p sig of the input message @p m is verified using the signer's
 * public key @p pk.
 *
 * @param sig         Input signature.
 * @param hash        Message hash, SHA512(message).
 * @param pk          Signer's public key.
 *
 * @retval 0  If the signature is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ed25519ph_verify(
    const uint8_t sig[ocrypto_ed25519ph_BYTES],
    const uint8_t hash[ocrypto_ed25519ph_HASH_BYTES],
    const uint8_t pk[ocrypto_ed25519ph_PUBLIC_KEY_BYTES]);


/**
 * Ed25519ph signature key pair generation with context.
 *
 * Given a secret key @p sk, the corresponding public key is computed and put
 * into @p pk. The key pair can then be used to sign and verify message signatures.
 *
 * @param      ctx         Context.
 * @param[out] pk          Generated public key.
 * @param      sk          Secret key. Must be pre-filled with random data.
 */
void ocrypto_ed25519ph_public_key_ctx(
    ocrypto_ed25519ph_ctx *ctx,
    uint8_t pk[ocrypto_ed25519ph_PUBLIC_KEY_BYTES],
    const uint8_t sk[ocrypto_ed25519ph_SECRET_KEY_BYTES]);

/**
 * Ed25519ph signature generate with context.
 *
 * The message @p m is signed using the secret key @p sk and the corresponding
 * public key @p pk. The signature is put into @p sig.
 *
 * @param      ctx         Context.
 * @param[out] sig         Generated signature.
 * @param      hash        Message hash, SHA512(message).
 * @param      sk          Secret key.
 * @param      pk          Public key.
 */
void ocrypto_ed25519ph_sign_ctx(
    ocrypto_ed25519ph_ctx *ctx,
    uint8_t sig[ocrypto_ed25519ph_BYTES],
    const uint8_t hash[ocrypto_ed25519ph_HASH_BYTES],
    const uint8_t sk[ocrypto_ed25519ph_SECRET_KEY_BYTES],
    const uint8_t pk[ocrypto_ed25519ph_PUBLIC_KEY_BYTES]);

/**
 * Ed25519ph signature verification with context.
 *
 * The signature @p sig of the input message @p m is verified using the signer's
 * public key @p pk.
 *
 * @param ctx         Context.
 * @param sig         Input signature.
 * @param hash        Message hash, SHA512(message).
 * @param pk          Signer's public key.
 *
 * @retval 0  If the signature is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ed25519ph_verify_ctx(
    ocrypto_ed25519ph_ctx *ctx,
    const uint8_t sig[ocrypto_ed25519ph_BYTES],
    const uint8_t hash[ocrypto_ed25519ph_HASH_BYTES],
    const uint8_t pk[ocrypto_ed25519ph_PUBLIC_KEY_BYTES]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
