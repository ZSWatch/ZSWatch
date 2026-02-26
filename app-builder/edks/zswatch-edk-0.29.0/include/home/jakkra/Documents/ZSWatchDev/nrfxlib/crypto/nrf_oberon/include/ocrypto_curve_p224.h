/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**@file
 * @defgroup ocrypto_p224 ECC secp224r1 low-level APIs
 * @ingroup ocrypto
 * @{
 * @brief Type declarations and APIs for low-level elliptic curve point operations
 * based on the NIST secp224r1 curve.
 */

#ifndef OCRYPTO_CURVE_P224_H
#define OCRYPTO_CURVE_P224_H

#include "ocrypto_sc_p224.h"

#ifdef __cplusplus
extern "C" {
#endif


// (x,y) only jacobian coordinates
/**@cond */
typedef struct {
    ocrypto_mod_p224 x;
    ocrypto_mod_p224 y;
} ocrypto_cp_p224;
/**@endcond */


/** Load r.x from bytes, keep r.y.
 *
 * @param[out]      r       Point with r.x loaded, r.y kept.
 * @param           p       x as as array of bytes.
 *
 * @retval 0  If @p r is a valid curve point.
 * @retval -1 Otherwise.
 */
int ocrypto_curve_p224_from28bytes(ocrypto_cp_p224 *r, const uint8_t p[28]);

/** Load point from bytes.
 *
 * @param[out]      r       Loaded point.
 * @param           p       Point as array of bytes.
 *
 * @retval 0  If @p r is a valid curve point.
 * @retval -1 Otherwise.
 */
int ocrypto_curve_p224_from56bytes(ocrypto_cp_p224 *r, const uint8_t p[56]);

/** Store p.x to bytes.
 *
 * @param[out]           r       x stored as array.
 * @param                p       Point with x to be stored.
 */
void ocrypto_curve_p224_to28bytes(uint8_t r[28], ocrypto_cp_p224 *p);

/** Store p.x to bytes.
 *
 * @param[out]           r       Point stored as array.
 * @param                p       Point to be stored.
 */
void ocrypto_curve_p224_to56bytes(uint8_t r[56], ocrypto_cp_p224 *p);

/** P224 scalar multiplication.
 *
 * r = p * s
 * r = [0,0] if p = [0,0] or s mod q = 0
 *
 * @param[out]      r       Output point.
 * @param           p       Input point.
 * @param           s       Scalar.
 *
 * @retval -1 If r = [0,0].
 * @retval 0  If 0 < s < q.
 * @retval 1  If s > q.
 */
int ocrypto_curve_p224_scalarmult(ocrypto_cp_p224 *r, const ocrypto_cp_p224 *p, const ocrypto_sc_p224 *s);

/** P224 scalar base multiplication.
 *
 * r = basePoint * s
 * r = [0,0] if s mod q = 0
 *
 * @param[out]      r       Output point.
 * @param           s       Scalar.
 *
 * @retval -1 If r = [0,0].
 * @retval 0  If 0 < s < q.
 * @retval 1  If s > q.
 */
int ocrypto_curve_p224_scalarmult_base(ocrypto_cp_p224 *r, const ocrypto_sc_p224 *s);

/** P224 add and double
 *
 * r = p + q
 * p == [0,0] -> r = q
 * q == [0,0] -> r = p
 * p == -q    -> r = [0,0]
 *
 * @param[out]  r       Resulting point
 * @param       p       Input point.
 * @param       q       input point.
 *
 * @retval -1 if r = [0,0].
 * @retval 0 if successfull.
 */
int ocrypto_curve_p224_add(ocrypto_cp_p224 *r, const ocrypto_cp_p224 *p, const ocrypto_cp_p224 *q);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef OCRYPTO_CURVE_P224_H */

/** @} */
