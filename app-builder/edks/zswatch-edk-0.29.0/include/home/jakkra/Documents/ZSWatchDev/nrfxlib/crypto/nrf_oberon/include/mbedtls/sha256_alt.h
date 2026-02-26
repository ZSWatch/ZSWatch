/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef SHA256_ALT_H
#define SHA256_ALT_H

#include "common.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct mbedtls_sha256_context {
    uint32_t data[51]; // size of ocrypto_sha256_ctx
    int is224;
} mbedtls_sha256_context;


#ifdef __cplusplus
}
#endif

#endif
