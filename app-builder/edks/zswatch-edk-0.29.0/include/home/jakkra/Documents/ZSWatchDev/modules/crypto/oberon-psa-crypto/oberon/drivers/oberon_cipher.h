/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_CIPHER_H
#define OBERON_CIPHER_H

#include <psa/crypto_driver_common.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    uint32_t ctx[70];
    psa_algorithm_t alg;
    uint8_t decrypt;
    uint8_t iv_size;
} oberon_cipher_operation_t;


psa_status_t oberon_cipher_encrypt_setup(
    oberon_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg);

psa_status_t oberon_cipher_decrypt_setup(
    oberon_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg);

psa_status_t oberon_cipher_set_iv(
    oberon_cipher_operation_t *operation,
    const uint8_t *iv, size_t iv_length);

psa_status_t oberon_cipher_update(
    oberon_cipher_operation_t *operation,
    const uint8_t *input, size_t input_length,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_cipher_finish(
    oberon_cipher_operation_t *operation,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_cipher_abort(
    oberon_cipher_operation_t *operation);


psa_status_t oberon_cipher_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *iv, size_t iv_length,
    const uint8_t *input, size_t input_length,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_cipher_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *output, size_t output_size, size_t *output_length);


#ifdef __cplusplus
}
#endif

#endif
