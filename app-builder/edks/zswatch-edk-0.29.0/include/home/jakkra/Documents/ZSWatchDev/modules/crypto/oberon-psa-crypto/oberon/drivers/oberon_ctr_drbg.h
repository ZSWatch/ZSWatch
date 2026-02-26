/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_CTR_DRBG_H
#define OBERON_CTR_DRBG_H

#include <psa/crypto_driver_common.h>
#include "oberon_helpers.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    psa_cipher_operation_t aes_op;
    union {
        uint8_t v[PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE];
        uint32_t counter_part;
    };
    uint32_t reseed_counter;
#ifdef OBERON_USE_MUTEX
    oberon_mutex_type mutex;
#endif
} oberon_ctr_drbg_context_t;


psa_status_t oberon_ctr_drbg_init(
    oberon_ctr_drbg_context_t *context);

psa_status_t oberon_ctr_drbg_get_random(
    oberon_ctr_drbg_context_t *context,
    uint8_t *output,
    size_t output_size);

psa_status_t oberon_ctr_drbg_free(
    oberon_ctr_drbg_context_t *context);


#ifdef __cplusplus
}
#endif

#endif
