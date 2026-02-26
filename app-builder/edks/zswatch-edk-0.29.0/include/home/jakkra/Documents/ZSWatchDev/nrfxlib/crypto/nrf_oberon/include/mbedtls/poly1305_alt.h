/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef POLY1305_ALT_H
#define POLY1305_ALT_H

#include "common.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct mbedtls_poly1305_context
{
    uint32_t state[18];
} mbedtls_poly1305_context;


#ifdef __cplusplus
}
#endif

#endif
