/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_JPAKE_H
#define OBERON_JPAKE_H

#include <psa/crypto_driver_common.h>
#include <psa/crypto_struct.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    psa_algorithm_t hash_alg;
    uint8_t         secret[32];
    uint8_t         user_id[16];
    uint8_t         peer_id[16];
    uint8_t         user_id_length;
    uint8_t         peer_id_length;
    uint8_t         rd_idx;
    uint8_t         wr_idx;
    uint8_t         x[3][32]; // secret keys
    uint8_t         X[3][64]; // public keys
    uint8_t         P[3][64]; // peer keys
    uint8_t         V[64]; // ZKP public key
    uint8_t         r[32]; // ZKP signature
} oberon_jpake_operation_t;


psa_status_t oberon_jpake_setup(
    oberon_jpake_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *password, size_t password_length,
    const psa_pake_cipher_suite_t *cipher_suite);

psa_status_t oberon_jpake_set_user(
    oberon_jpake_operation_t *operation,
    const uint8_t *user_id, size_t user_id_len);

psa_status_t oberon_jpake_set_peer(
    oberon_jpake_operation_t *operation,
    const uint8_t *peer_id, size_t peer_id_len);

psa_status_t oberon_jpake_output(
    oberon_jpake_operation_t *operation,
    psa_pake_step_t step,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_jpake_input(
    oberon_jpake_operation_t *operation,
    psa_pake_step_t step,
    const uint8_t *input, size_t input_length);

psa_status_t oberon_jpake_get_shared_key(
    oberon_jpake_operation_t *operation,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_jpake_abort(
    oberon_jpake_operation_t *operation);


#ifdef __cplusplus
}
#endif

#endif
