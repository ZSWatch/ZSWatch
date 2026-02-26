/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_ML_DSA_H
#define OBERON_ML_DSA_H

#include <psa/crypto_driver_common.h>


#ifdef __cplusplus
extern "C" {
#endif


psa_status_t oberon_export_ml_dsa_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    uint8_t *data, size_t data_size, size_t *data_length);

psa_status_t oberon_import_ml_dsa_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length,
    uint8_t *key, size_t key_size, size_t *key_length,
    size_t *bits);

psa_status_t oberon_ml_dsa_sign_message_with_context(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    const uint8_t *context, size_t context_length,
    uint8_t *signature, size_t signature_size, size_t *signature_length);

psa_status_t oberon_ml_dsa_sign_hash_with_context(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *hash, size_t hash_length,
    const uint8_t *context, size_t context_length,
    uint8_t *signature, size_t signature_size, size_t *signature_length);

psa_status_t oberon_ml_dsa_verify_message_with_context(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    const uint8_t *context, size_t context_length,
    const uint8_t *signature, size_t signature_length);

psa_status_t oberon_ml_dsa_verify_hash_with_context(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *hash, size_t hash_length,
    const uint8_t *context, size_t context_length,
    const uint8_t *signature, size_t signature_length);


#ifdef __cplusplus
}
#endif

#endif
