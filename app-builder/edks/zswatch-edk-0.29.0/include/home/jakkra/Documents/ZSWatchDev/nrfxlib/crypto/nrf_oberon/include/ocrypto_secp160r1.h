/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */



#ifndef OCRYPTO_SECP160R1_H
#define OCRYPTO_SECP160R1_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * ECDH secp160r1 scalar multiplication `r = (1 + s mod (n-1)) * G`.
 *
 * @param[out] r Resulting curve point in the form (x,y).
 * @param      s Scalar value. 256 bit big endian.
 *
 * @remark @p r may be same as @p s.
 */
void ocrypto_p160_scalar_mult(uint8_t r[40], const uint8_t s[32]);

/**
 * ECDH secp160r1 scalar multiplication `r = (s mod n) * G`.
 *
 * @param[out] r Resulting curve point in the form (x,y).
 * @param      s Scalar value. 256 bit big endian.
 *
 * @remark @p r may be same as @p s.
 */
void ocrypto_p160_scalar_mult_alt(uint8_t r[40], const uint8_t s[32]);


#ifdef __cplusplus
}
#endif

#endif
