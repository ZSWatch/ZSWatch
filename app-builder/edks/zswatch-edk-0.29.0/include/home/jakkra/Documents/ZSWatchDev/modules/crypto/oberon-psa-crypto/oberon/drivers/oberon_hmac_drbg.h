/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_HMAC_DRBG_H
#define OBERON_HMAC_DRBG_H

#include <psa/crypto_driver_common.h>
#include "oberon_helpers.h"


#ifdef __cplusplus
extern "C" {
#endif


#if !defined(OBERON_HMAC_DRBG_HASH_ALG)
#define OBERON_HMAC_DRBG_HASH_ALG  PSA_ALG_SHA_256
#endif


typedef struct {
    psa_mac_operation_t hmac_op;
    uint8_t k[PSA_HASH_LENGTH(OBERON_HMAC_DRBG_HASH_ALG)];
    uint8_t v[PSA_HASH_LENGTH(OBERON_HMAC_DRBG_HASH_ALG)];
    uint32_t reseed_counter;
#ifdef OBERON_USE_MUTEX
    oberon_mutex_type mutex;
#endif
} oberon_hmac_drbg_context_t;


psa_status_t oberon_hmac_drbg_init(
    oberon_hmac_drbg_context_t *context);

psa_status_t oberon_hmac_drbg_get_random(
    oberon_hmac_drbg_context_t *context,
    uint8_t *output,
    size_t output_size);

psa_status_t oberon_hmac_drbg_free(
    oberon_hmac_drbg_context_t *context);


#ifdef __cplusplus
}
#endif

#endif
