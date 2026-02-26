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

#ifndef OCRYPTO_TYPES_P256_H
#define OCRYPTO_TYPES_P256_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef struct {
  uint32_t w[8];
} ocrypto_mod_p256;
/**@endcond */

/**@cond */
typedef struct {
    uint32_t w[8];
} ocrypto_sc_p256;
/**@endcond */

/**@cond */
typedef struct {
    ocrypto_mod_p256 x, x3, xn, t;
    int step;
} ocrypto_p256_inv_ctx;
/**@endcond */

/**@cond */
typedef struct {
    ocrypto_mod_p256 x;
    ocrypto_mod_p256 y;
} ocrypto_cp_p256;
/**@endcond */

/**@cond */
typedef struct {
    ocrypto_cp_p256 p, q0, q1;
    uint32_t e[8];
    ocrypto_p256_inv_ctx inv;
    int ret, prev, dec, step;
} ocrypto_p256_mult_ctx;
/**@endcond */


#ifdef __cplusplus
}
#endif

#endif

/** @} */
