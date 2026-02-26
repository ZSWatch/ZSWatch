/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_KEY_AGREEMENT_H
#define OBERON_KEY_AGREEMENT_H

#include <psa/crypto_driver_common.h>


#ifdef __cplusplus
extern "C" {
#endif


psa_status_t oberon_key_agreement(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *peer_key, size_t peer_key_length,
    uint8_t *output, size_t output_size, size_t *output_length);


#ifdef __cplusplus
}
#endif

#endif
