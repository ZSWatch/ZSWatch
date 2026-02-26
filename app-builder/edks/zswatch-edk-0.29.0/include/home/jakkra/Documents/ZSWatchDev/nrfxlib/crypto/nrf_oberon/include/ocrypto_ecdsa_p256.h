/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ecdsa_p256 ECDSA P-256
 * @ingroup ocrypto_asym_sig
 * @{
 * @brief Elliptic curve digital signature algorithm based on the
 *        NIST P-256 curve (aka secp256r1).
 *
 * ECDSA P-256 is a specific implementation of a digital signature scheme.
 * 
 * @see [FIPS - Digital Signature Standard (DSS)](https://csrc.nist.gov/pubs/fips/186-5/final)
 */

/**
 * @file
 * @brief Elliptic curve digital signature algorithm based on the
 *        NIST P-256 curve (aka secp256r1).
 */

#ifndef OCRYPTO_ECDSA_P256_H
#define OCRYPTO_ECDSA_P256_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * ECDSA P-256 public key generation.
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
 * @remark To generate a valid key pair use the following code pattern:
 *            @code{.c}
 *            do get_random(sk); while (ocrypto_ecdsa_p256_public_key(pk, sk));
 *            @endcode
 */
int ocrypto_ecdsa_p256_public_key(
    uint8_t pk[64],
    const uint8_t sk[32]);

/**
 * ECDSA P-256 signature generation.
 *
 * The message @p m is signed using the secret key @p sk and the ephemeral
 * session key @p ek. The signature is put into @p sig.
 *
 * @param[out] sig  Generated signature.
 * @param      m    Input message.
 * @param      mlen Length of @p m.
 * @param      sk   Secret key.
 * @param      ek   Session key. Must be pre-filled with random data.
 *
 * @retval 0  If @p ek is a valid session key.
 * @retval -1 Otherwise.
 *
 * @remark To generate a valid signature use the following code pattern:
 *            @code{.c}
 *            do get_random(ek); while (ocrypto_ecdsa_p256_sign(sig, m, mlen, sk, ek));
 *            @endcode
 */
int ocrypto_ecdsa_p256_sign(
    uint8_t sig[64],
    const uint8_t *m, size_t mlen,
    const uint8_t sk[32],
    const uint8_t ek[32]);

/**
 * ECDSA P-256 signature generation from SHA256 hash.
 *
 * The message hash @p hash is signed using the secret key @p sk and the ephemeral
 * session key @p ek. The signature is put into @p sig.
 *
 * @param[out] sig  Generated signature.
 * @param      hash Input hash.
 * @param      sk   Secret key.
 * @param      ek   Session key. Must be pre-filled with random data.
 *
 * @retval 0  If @p ek is a valid session key.
 * @retval -1 Otherwise.
 *
 * @remark To generate a valid signature use the following code pattern:
 *            @code{.c}
 *            do get_random(ek); while (ocrypto_ecdsa_p256_sign_hash(sig, hash, sk, ek));
 *            @endcode
 */
int ocrypto_ecdsa_p256_sign_hash(
    uint8_t sig[64],
    const uint8_t hash[32],
    const uint8_t sk[32],
    const uint8_t ek[32]);

/**
 * ECDSA P-256 signature verification.
 *
 * The signature @p sig of the input message @p m is verified using the signer's
 * public key @p pk.
 *
 * @param sig  Input signature.
 * @param m    Input message.
 * @param mlen Length of @p m.
 * @param pk   Signer's public key.
 *
 * @retval 0  If the signature is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecdsa_p256_verify(
    const uint8_t sig[64],
    const uint8_t *m, size_t mlen,
    const uint8_t pk[64]);

/**
 * ECDSA P-256 signature verification from SHA256 hash.
 *
 * The signature @p sig of the message hash @p hash is verified using the signer's
 * public key @p pk.
 *
 * @param sig  Input signature.
 * @param hash Input hash.
 * @param pk   Signer's public key.
 *
 * @retval 0  If the signature is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecdsa_p256_verify_hash(
    const uint8_t sig[64],
    const uint8_t hash[32],
    const uint8_t pk[64]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
