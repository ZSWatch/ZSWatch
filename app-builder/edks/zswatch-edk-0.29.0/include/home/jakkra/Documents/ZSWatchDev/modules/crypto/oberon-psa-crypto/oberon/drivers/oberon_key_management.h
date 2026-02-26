/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_KEY_MANAGEMENT_H
#define OBERON_KEY_MANAGEMENT_H

#include <psa/crypto_driver_common.h>


#ifdef __cplusplus
extern "C" {
#endif


psa_status_t oberon_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    uint8_t *data, size_t data_size, size_t *data_length);

psa_status_t oberon_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length,
    uint8_t *key, size_t key_size, size_t *key_length,
    size_t *bits);

psa_status_t oberon_generate_key(
    const psa_key_attributes_t *attributes,
    uint8_t *key, size_t key_size, size_t *key_length);

psa_status_t oberon_derive_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *input, size_t input_length,
    uint8_t *key, size_t key_size, size_t *key_length);


#ifdef __cplusplus
}
#endif

#endif
