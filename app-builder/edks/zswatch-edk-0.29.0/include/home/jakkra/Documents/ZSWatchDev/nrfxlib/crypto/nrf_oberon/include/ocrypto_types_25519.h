/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_types Context Types
 * @ingroup ocrypto
 * @{
 * @brief Declaration of internal types used in public interfaces.
 * 
 * The fields and their sizes are subject to change.
 */

/**
 * @file
 * @brief Declaration of internal types used in public interfaces.
 */

#ifndef OCRYPTO_TYPES_25519_H
#define OCRYPTO_TYPES_25519_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_sha512.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@cond */
typedef struct {
  uint32_t w[8];
} ocrypto_mod25519;
/**@endcond */

/**@cond */
typedef struct {
    uint32_t w[8];
} ocrypto_sc25519;
/**@endcond */

/**@cond */
typedef struct {
    ocrypto_mod25519 x;
    ocrypto_mod25519 y;
    ocrypto_mod25519 z;
} ocrypto_ge25519;
/**@endcond */

/**@cond */
typedef struct {
    ocrypto_mod25519 x;
    ocrypto_mod25519 y;
    ocrypto_mod25519 z;
    ocrypto_mod25519 t;
} ocrypto_ge25519_ext;
/**@endcond */

/**@cond */
typedef struct {
    ocrypto_mod25519 e;
    ocrypto_mod25519 f;
    ocrypto_mod25519 g;
    ocrypto_mod25519 h;
} ocrypto_ge25519_comp;
/**@endcond */


/**@cond */
typedef struct {
    ocrypto_ge25519_comp comp;
    ocrypto_ge25519_ext ext;
    ocrypto_ge25519_ext pre[3];
} ocrypto_ge25519_ctx;
/**@endcond */

/**
 * Context for stack-optimized Ed25519 
 */
typedef struct {
/**@cond */
    union {
        ocrypto_sha512_ctx sha;
        ocrypto_ge25519_ctx ge;
    } c;
    ocrypto_sc25519 r, k, s;
    ocrypto_ge25519 R;
    uint8_t ext[64];
/**@endcond */
} ocrypto_ed25519_ctx;

/**
 * Context for stack-optimized Curve25519 scalar multiplication 
 */
typedef struct {
/**@cond */
ocrypto_mod25519 x0, xn, zn, xm, zm, a, c;
/**@endcond */
} ocrypto_curve25519_ctx;

    
#ifdef __cplusplus
}
#endif

#endif

/** @} */
