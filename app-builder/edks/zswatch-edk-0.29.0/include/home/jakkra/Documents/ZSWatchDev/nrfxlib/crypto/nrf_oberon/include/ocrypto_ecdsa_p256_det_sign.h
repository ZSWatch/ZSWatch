/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ecdsa_p256_det_sign ECDSA P-256 (Deterministic)
 * @ingroup ocrypto_asym_sig
 * @{
 * @brief Elliptic curve digital signature algorithm based on the
 *        NIST P-256 curve (aka secp256r1) (deterministic variant).
 *
 * ECDSA P-256 is a specific implementation of a digital signature scheme.
 * 
 * @see [FIPS - Digital Signature Standard (DSS)](https://csrc.nist.gov/pubs/fips/186-5/final)
 */

/**
 * @file
 * @brief Elliptic curve digital signature algorithm based on the
 *        NIST P-256 curve (aka secp256r1) (deterministic variant).
 */

#ifndef OCRYPTO_DET_ECDSA_P256_H
#define OCRYPTO_DET_ECDSA_P256_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * ECDSA P-256 deterministic signature generation.
 *
 * The message @p m is signed using the secret key @p sk and a session key calculated
 * from message hash and key. The signature is put into @p sig.
 *
 * @param[out] sig  Generated signature.
 * @param      m    Input message.
 * @param      mlen Length of @p m.
 * @param      sk   Secret key.
 */
void ocrypto_ecdsa_p256_det_sign(
    uint8_t sig[64],
    const uint8_t *m, size_t mlen,
    const uint8_t sk[32]);

/**
 * ECDSA P-256 deterministic signature generation from SHA256 hash.
 *
 * The message hash @p hash is signed using the secret key @p sk and a session key calculated
 * from hash and key. The signature is put into @p sig.
 *
 * @param[out] sig  Generated signature.
 * @param      hash Input hash.
 * @param      sk   Secret key.
 */
void ocrypto_ecdsa_p256_det_sign_hash(
    uint8_t sig[64],
    const uint8_t hash[32],
    const uint8_t sk[32]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
