/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ecdsa_p521 ECDSA P-521
 * @ingroup ocrypto_asym_sig
 * @{
 * @brief Elliptic curve digital signature algorithm based on the
 *        NIST P-521 curve (aka secp521r1).
 *
 * ECDSA P-521 is a specific implementation of a digital signature scheme.
 * 
 * @see [FIPS - Digital Signature Standard (DSS)](https://csrc.nist.gov/pubs/fips/186-5/final)
 */

/**
 * @file
 * @brief Elliptic curve digital signature algorithm based on the
 *        NIST P-521 curve (aka secp521r1).
 */

#ifndef OCRYPTO_ECDSA_P521_H
#define OCRYPTO_ECDSA_P521_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * ECDSA P-521 public key generation.
 *
 * Given a secret key @p sk the corresponding public key is computed and put
 * into @p pk.
 *
 * @param[out] pk Generated public key.
 * @param      sk Secret key. Must be pre-filled with random data.
 *
 * @retval 0  If @p sk is a valid secret key.
 * @retval -1 Otherwise.
 */
int ocrypto_ecdsa_p521_public_key(
    uint8_t pk[132],
    const uint8_t sk[66]);

/**
 * ECDSA P-521 signature generation.
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
 *            do get_random(ek); while (ocrypto_ecdsa_p521_sign(sig, m, mlen, sk, ek));
 *            @endcode
 */
int ocrypto_ecdsa_p521_sign(
    uint8_t sig[132],
    const uint8_t *m, size_t mlen,
    const uint8_t sk[66],
    const uint8_t ek[66]);

/**
 * ECDSA P-521 signature generation from SHA512 hash.
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
 *            do get_random(ek); while (ocrypto_ecdsa_p521_sign_hash(sig, hash, sk, ek));
 *            @endcode
 */
int ocrypto_ecdsa_p521_sign_hash(
    uint8_t sig[132],
    const uint8_t hash[64],
    const uint8_t sk[66],
    const uint8_t ek[66]);

/**
 * ECDSA P-521 signature verification.
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
int ocrypto_ecdsa_p521_verify(
    const uint8_t sig[132],
    const uint8_t *m, size_t mlen,
    const uint8_t pk[132]);

/**
 * ECDSA P-521 signature verification from SHA512 hash.
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
int ocrypto_ecdsa_p521_verify_hash(
    const uint8_t sig[132],
    const uint8_t hash[64],
    const uint8_t pk[132]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
