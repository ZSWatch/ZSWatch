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

#ifndef OCRYPTO_TYPES_SRP_H
#define OCRYPTO_TYPES_SRP_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_sha512.h"


#ifdef __cplusplus
extern "C" {
#endif


/**@cond */
typedef struct {
    uint32_t w[96];
} ocrypto_srp_mg;
/**@endcond */

/**@cond */
typedef struct {
    uint32_t mem[96];
    uint32_t m[192], x[96];
    ocrypto_srp_mg a2, a3, ax;
} ocrypto_srp_math_ctx;
/**@endcond */

/**
 * Context for stack-optimized SRP 
 */
typedef struct {
/**@cond */
    union {
        ocrypto_sha512_ctx sha;
        ocrypto_srp_math_ctx mul;
    } c;
    ocrypto_srp_mg x, y;
/**@endcond */
} ocrypto_srp_ctx;


#ifdef __cplusplus
}
#endif

#endif

/** @} */
