/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_KEY_ENCAPSULATION_H
#define OBERON_KEY_ENCAPSULATION_H

#include <psa/crypto_driver_common.h>


#ifdef __cplusplus
extern "C" {
#endif


psa_status_t oberon_encapsulate(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const psa_key_attributes_t *output_attributes,
    uint8_t *output_key, size_t output_key_size, size_t *output_key_length,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length);

psa_status_t oberon_decapsulate(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *ciphertext, size_t ciphertext_length,
    const psa_key_attributes_t *output_attributes,
    uint8_t *output_key, size_t output_key_size, size_t *output_key_length);


#ifdef __cplusplus
}
#endif

#endif
