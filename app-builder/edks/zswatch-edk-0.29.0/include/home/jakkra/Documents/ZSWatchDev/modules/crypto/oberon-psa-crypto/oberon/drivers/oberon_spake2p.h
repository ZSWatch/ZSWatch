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

#ifndef OBERON_SPAKE2P_H
#define OBERON_SPAKE2P_H

#include <psa/crypto_driver_common.h>
#include <psa/crypto_struct.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    psa_hash_operation_t hash_op; // TT
    psa_algorithm_t alg;
    uint8_t         w0[32];
    uint8_t         w1L[65];
    uint8_t         xy[32];
    uint8_t         XY[65];
    uint8_t         YX[65];
    uint8_t         shared[PSA_HASH_MAX_SIZE];
    uint8_t         KconfPV[PSA_HASH_MAX_SIZE];
    uint8_t         KconfVP[PSA_HASH_MAX_SIZE];
    uint8_t         prover[32];
    uint8_t         verifier[32];
    uint8_t         prover_len;
    uint8_t         verifier_len;
    uint8_t         shared_len;
    uint8_t         conf_len;
    uint8_t         mac_len;
    psa_pake_role_t role;
} oberon_spake2p_operation_t;


psa_status_t oberon_spake2p_setup(
    oberon_spake2p_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *password, size_t password_length,
    const psa_pake_cipher_suite_t *cipher_suite);

psa_status_t oberon_spake2p_set_role(
    oberon_spake2p_operation_t *operation,
    psa_pake_role_t role);

psa_status_t oberon_spake2p_set_user(
    oberon_spake2p_operation_t *operation,
    const uint8_t *user_id, size_t user_id_len);

psa_status_t oberon_spake2p_set_peer(
    oberon_spake2p_operation_t *operation,
    const uint8_t *peer_id, size_t peer_id_len);

psa_status_t oberon_spake2p_set_context(
    oberon_spake2p_operation_t *operation,
    const uint8_t *context, size_t context_len);

psa_status_t oberon_spake2p_output(
    oberon_spake2p_operation_t *operation,
    psa_pake_step_t step,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_spake2p_input(
    oberon_spake2p_operation_t *operation,
    psa_pake_step_t step,
    const uint8_t *input, size_t input_length);

psa_status_t oberon_spake2p_get_shared_key(
    oberon_spake2p_operation_t *operation,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_spake2p_abort(
    oberon_spake2p_operation_t *operation);


psa_status_t oberon_derive_spake2p_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *input, size_t input_length,
    uint8_t *key, size_t key_size, size_t *key_length);

psa_status_t oberon_import_spake2p_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length,
    uint8_t *key, size_t key_size, size_t *key_length,
    size_t *bits);

psa_status_t oberon_export_spake2p_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    uint8_t *data, size_t data_size, size_t *data_length);
    

#ifdef __cplusplus
}
#endif

#endif
