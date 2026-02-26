/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_curve25519 ECC X25519
 * @ingroup ocrypto_key_agreement
 * @{
 * @brief Elliptic curve Diffie-Hellman algorithm based on the Curve25519 curve.
 *
 * X25519 is the combination of the Curve25519 elliptic curve with the Diffie-Hellman
 * operation.
 * Curve25519 is an elliptic curve offering 128 bits of security.
 *
 * @see [RFC - Elliptic Curves for Security](https://tools.ietf.org/html/rfc7748)
 * @see [Curve25519: high-speed elliptic-curve cryptography](http://cr.yp.to/ecdh.html)
 */

/**
 * @file
 * @brief Elliptic curve Diffie-Hellman algorithm based on the Curve25519 curve.
 */

#ifndef OCRYPTO_CURVE25519_H
#define OCRYPTO_CURVE25519_H

#include "ocrypto_types_25519.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Length of a scalar.
 */
#define ocrypto_curve25519_SCALAR_BYTES (32)

/**
 * Length of a curve point.
 */
#define ocrypto_curve25519_BYTES (32)


/**
 * Curve25519 scalar multiplication `r = n * basePoint`.
 *
 * Given a secret key @p n, the corresponding Curve25519 public key is computed
 * and put into @p r.
 *
 * @param[out] r Resulting curve point.
 * @param[in]  n Scalar factor.
 *
 * @remark @p r may be same as @p n.
 */
void ocrypto_curve25519_scalarmult_base(
    uint8_t r[ocrypto_curve25519_BYTES],
    const uint8_t n[ocrypto_curve25519_SCALAR_BYTES]);

/**
 * Curve25519 scalar multiplication `r = n * p`.
 *
 * A shared secret is computed from the local secret key @p n and another
 * party's public key @p p and put into @p r. The same shared secret is
 * generated when the other party combines its private key with the local public
 * key.
 *
 * @param[out] r Resulting curve point.
 * @param[in]  n Scalar factor.
 * @param[in]  p Point factor.
 *
 * @remark @p r may be same as @p n.
 */
void ocrypto_curve25519_scalarmult(
    uint8_t r[ocrypto_curve25519_BYTES],
    const uint8_t n[ocrypto_curve25519_SCALAR_BYTES],
    const uint8_t p[ocrypto_curve25519_BYTES]);


/**
 * Curve25519 scalar multiplication `r = n * basePoint` with context.
 *
 * Given a secret key @p n, the corresponding Curve25519 public key is computed
 * and put into @p r.
 *
 * @param      ctx Context.
 * @param[out] r   Resulting curve point.
 * @param[in]  n   Scalar factor.
 *
 * @remark @p r may be same as @p n.
 */
void ocrypto_curve25519_scalarmult_base_ctx(
    ocrypto_curve25519_ctx *ctx,
    uint8_t r[ocrypto_curve25519_BYTES],
    const uint8_t n[ocrypto_curve25519_SCALAR_BYTES]);

/**
 * Curve25519 scalar multiplication `r = n * p` with context.
 *
 * A shared secret is computed from the local secret key @p n and another
 * party's public key @p p and put into @p r. The same shared secret is
 * generated when the other party combines its private key with the local public
 * key.
 *
 * @param      ctx Context.
 * @param[out] r   Resulting curve point.
 * @param[in]  n   Scalar factor.
 * @param[in]  p   Point factor.
 *
 * @remark @p r may be same as @p n.
 */
void ocrypto_curve25519_scalarmult_ctx(
    ocrypto_curve25519_ctx *ctx,
    uint8_t r[ocrypto_curve25519_BYTES],
    const uint8_t n[ocrypto_curve25519_SCALAR_BYTES],
    const uint8_t p[ocrypto_curve25519_BYTES]);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
