/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ed448ph EdDSA Ed448ph
 * @ingroup ocrypto_asym_sig
 * @{
 * @brief Ed448ph algorithm, used for EdDSA with pre-hashing.
 *
 * Ed448 is a specific implementation of EdDSA, a digital signature scheme.
 * EdDSA is based on Twisted Edwards curves and is designed to be faster than
 * existing digital signature schemes without sacrificing security.
 * 
 * @see [RFC - Edwards-Curve Digital Signature Algorithm (EdDSA)](https://datatracker.ietf.org/doc/html/rfc8032)
 */

/**
 * @file
 * @brief Ed448ph algorithm, used for EdDSA with pre-hashing.
 */

#ifndef OCRYPTO_ED448PH_H
#define OCRYPTO_ED448PH_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of a public key.
 */
#define ocrypto_ed448ph_PUBLIC_KEY_BYTES (57)

/**
 * Length of a secret key.
 */
#define ocrypto_ed448ph_SECRET_KEY_BYTES (57)

/**
 * Length of a message hash.
 */
#define ocrypto_ed448ph_HASH_BYTES (64)

/**
 * Length of a signature.
 */
#define ocrypto_ed448ph_BYTES (114)


/**
 * Ed448ph signature key pair generation.
 *
 * Given a secret key @p sk, the corresponding public key is computed and put
 * into @p pk. The key pair can then be used to sign and verify message signatures.
 *
 * @param[out] pk Generated public key.
 * @param      sk Secret key. Must be pre-filled with random data.
 */
void ocrypto_ed448ph_public_key(
    uint8_t pk[ocrypto_ed448ph_PUBLIC_KEY_BYTES],
    const uint8_t sk[ocrypto_ed448ph_SECRET_KEY_BYTES]);

/**
 * Ed448ph signature generation.
 *
 * The message hash @p hash is signed using the secret key @p sk and the
 * corresponding public key @p pk. The signature is put into @p sig.
 *
 * @param[out] sig   Generated signature.
 * @param      hash  Message hash, SHAKE256(message).
 * @param      sk    Secret key.
 * @param      pk    Public key.
 */
void ocrypto_ed448ph_sign(
    uint8_t sig[ocrypto_ed448ph_BYTES],
    const uint8_t hash[ocrypto_ed448ph_HASH_BYTES],
    const uint8_t sk[ocrypto_ed448ph_SECRET_KEY_BYTES],
    const uint8_t pk[ocrypto_ed448ph_PUBLIC_KEY_BYTES]);

/**
 * Ed448ph signature verification.
 *
 * The signature @p sig of the input message hash @p hash is verified
 * using the signer's public key @p pk.
 *
 * @param sig   Input signature.
 * @param hash  Message hash, SHAKE256(message).
 * @param pk    Signer's public key.
 *
 * @retval 0  If the signature is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ed448ph_verify(
    const uint8_t sig[ocrypto_ed448ph_BYTES],
    const uint8_t hash[ocrypto_ed448ph_HASH_BYTES],
    const uint8_t pk[ocrypto_ed448ph_PUBLIC_KEY_BYTES]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
