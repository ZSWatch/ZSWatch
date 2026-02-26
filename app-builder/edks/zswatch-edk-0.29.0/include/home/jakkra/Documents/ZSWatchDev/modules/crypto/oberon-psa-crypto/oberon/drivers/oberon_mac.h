/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_MAC_H
#define OBERON_MAC_H

#include <psa/crypto_driver_common.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OBERON_HMAC_ALG = 1,
    OBERON_CMAC_ALG = 2,
    OBERON_CBC_MAC_ALG = 3,
} oberon_mac_alg;

typedef struct {
    psa_hash_operation_t hash_op;
    psa_algorithm_t hash_alg;
    uint8_t hash[PSA_HASH_MAX_SIZE];
    uint8_t k[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    uint8_t block_size;
} oberon_hmac_operation_t;

typedef struct {
    psa_cipher_operation_t aes_op;
    uint8_t block[PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE];
    uint8_t k[PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE];
    size_t length;
} oberon_cmac_operation_t;

typedef struct {
    union {
#ifdef PSA_NEED_OBERON_HMAC
        oberon_hmac_operation_t hmac;
#endif
#ifdef PSA_NEED_OBERON_CMAC
        oberon_cmac_operation_t cmac;
#endif
        int dummy;
    };
    oberon_mac_alg alg;
} oberon_mac_operation_t;


psa_status_t oberon_mac_sign_setup(
    oberon_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg );

psa_status_t oberon_mac_verify_setup(
    oberon_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg );

psa_status_t oberon_mac_update(
    oberon_mac_operation_t *operation,
    const uint8_t *input, size_t input_length );

psa_status_t oberon_mac_sign_finish(
    oberon_mac_operation_t *operation,
    uint8_t *mac, size_t mac_size, size_t *mac_length );

psa_status_t oberon_mac_verify_finish(
    oberon_mac_operation_t *operation,
    const uint8_t *mac, size_t mac_length );

psa_status_t oberon_mac_abort(
    oberon_mac_operation_t *operation );


psa_status_t oberon_mac_compute(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *mac, size_t mac_size, size_t *mac_length );


#ifdef __cplusplus
}
#endif

#endif
