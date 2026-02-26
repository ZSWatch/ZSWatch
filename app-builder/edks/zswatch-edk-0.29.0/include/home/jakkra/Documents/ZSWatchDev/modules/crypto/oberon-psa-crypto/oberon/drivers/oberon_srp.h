/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.
// Different from the draft spec, the setup function has parameters, in order to
// enable an implementation without memory allocation in the driver.

#ifndef OBERON_SRP_H
#define OBERON_SRP_H

#include <psa/crypto_driver_common.h>
#include <psa/crypto_struct.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    psa_algorithm_t hash_alg;
    size_t          hash_len;
    uint8_t         password[384];
    uint8_t         ab[32];
    uint8_t         A[384];
    uint8_t         B[384];
    uint8_t         m1[PSA_HASH_MAX_SIZE];
    uint8_t         m2[PSA_HASH_MAX_SIZE];
    uint8_t         k[PSA_HASH_MAX_SIZE];
    uint8_t         user[PSA_HASH_MAX_SIZE];
    uint8_t         salt[64];
    uint8_t         salt_len;
    uint16_t        pw_len;
    psa_pake_role_t role;
} oberon_srp_operation_t;


psa_status_t oberon_srp_setup(
    oberon_srp_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *password, size_t password_length,
    const psa_pake_cipher_suite_t *cipher_suite);

psa_status_t oberon_srp_set_role(
    oberon_srp_operation_t *operation,
    psa_pake_role_t role);

psa_status_t oberon_srp_set_user(
    oberon_srp_operation_t *operation,
    const uint8_t *user_id, size_t user_id_len);

psa_status_t oberon_srp_output(
    oberon_srp_operation_t *operation,
    psa_pake_step_t step,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_srp_input(
    oberon_srp_operation_t *operation,
    psa_pake_step_t step,
    const uint8_t *input, size_t input_length);

psa_status_t oberon_srp_get_shared_key(
    oberon_srp_operation_t *operation,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_srp_abort(
    oberon_srp_operation_t *operation);


psa_status_t oberon_import_srp_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length,
    uint8_t *key, size_t key_size, size_t *key_length,
    size_t *bits);

psa_status_t oberon_export_srp_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    uint8_t *data, size_t data_size, size_t *data_length);


#ifdef __cplusplus
}
#endif

#endif
