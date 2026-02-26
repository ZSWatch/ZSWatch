/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef ECJPAKE_ALT_H
#define ECJPAKE_ALT_H

#include "common.h"
#include "mbedtls/ecp.h"
#include "mbedtls/md.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct mbedtls_ecjpake_context {
    mbedtls_ecjpake_role role; // Are we client or server?
    int point_format;          // Format for point export

    unsigned char secret[32];
    unsigned char s_key2[32];  // secret key 2
    unsigned char p_key1[64];  // public key 1
    unsigned char p_key2[64];  // public key 2
    unsigned char r_key1[64];  // remote key 1
    unsigned char r_key2[64];  // remote key 2
    unsigned char r_key3[64];  // remote key 3
} mbedtls_ecjpake_context;


#ifdef __cplusplus
}
#endif

#endif
