/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef CHACHA20_ALT_H
#define CHACHA20_ALT_H

#include "common.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct mbedtls_chacha20_context
{
    uint32_t state[33];
    uint8_t key[32];
}
mbedtls_chacha20_context;


#ifdef __cplusplus
}
#endif

#endif
