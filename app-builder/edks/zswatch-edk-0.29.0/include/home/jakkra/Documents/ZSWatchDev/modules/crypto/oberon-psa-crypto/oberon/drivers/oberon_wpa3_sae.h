/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_WPA3_SAE_H
#define OBERON_WPA3_SAE_H

#include <psa/crypto_driver_common.h>
#include <psa/crypto_struct.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    psa_mac_operation_t mac_op;
    psa_algorithm_t hash_alg;
    uint8_t password[256];
    uint8_t pwe[64];
    uint8_t max_id[6];
    uint8_t min_id[6];
    uint8_t rand[32];
    uint8_t kck[32];
    uint8_t pmk[32];
    uint8_t pmkid[16];
    uint8_t commit[96];
    uint8_t peer_commit[96];
    uint8_t hash_length;
    uint8_t pmk_length;
    uint8_t use_h2e:1;
    uint8_t keys_set:1;
    uint8_t salt_set:1;
    uint16_t pw_length;
    uint16_t send_confirm;
} oberon_wpa3_sae_operation_t;


psa_status_t oberon_wpa3_sae_setup(
    oberon_wpa3_sae_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *password, size_t password_length,
    const psa_pake_cipher_suite_t *cipher_suite);

psa_status_t oberon_wpa3_sae_set_user(
    oberon_wpa3_sae_operation_t *operation,
    const uint8_t *user_id, size_t user_id_len);

psa_status_t oberon_wpa3_sae_set_peer(
    oberon_wpa3_sae_operation_t *operation,
    const uint8_t *peer_id, size_t peer_id_len);

psa_status_t oberon_wpa3_sae_output(
    oberon_wpa3_sae_operation_t *operation,
    psa_pake_step_t step,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_wpa3_sae_input(
    oberon_wpa3_sae_operation_t *operation,
    psa_pake_step_t step,
    const uint8_t *input, size_t input_length);

psa_status_t oberon_wpa3_sae_get_shared_key(
    oberon_wpa3_sae_operation_t *operation,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_wpa3_sae_abort(
    oberon_wpa3_sae_operation_t *operation);


psa_status_t oberon_derive_wpa3_sae_pt_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *input, size_t input_length,
    uint8_t *key, size_t key_size, size_t *key_length);

psa_status_t oberon_import_wpa3_sae_pt_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length,
    uint8_t *key, size_t key_size, size_t *key_length,
    size_t *key_bits);


#ifdef __cplusplus
}
#endif

#endif
