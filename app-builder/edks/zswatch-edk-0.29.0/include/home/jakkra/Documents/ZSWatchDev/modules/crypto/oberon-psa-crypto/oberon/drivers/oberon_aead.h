/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_AEAD_H
#define OBERON_AEAD_H

#include <psa/crypto_driver_common.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    struct {
        uint64_t l[5]; // 64 bit alignment for Ascon
#if defined(PSA_NEED_OBERON_GCM_AES)
        uint32_t a[67];
#elif defined(PSA_NEED_OBERON_CCM_AES)
        uint32_t a[63];
#elif defined(PSA_NEED_OBERON_CHACHA20_POLY1305) || defined(PSA_NEED_OBERON_XCHACHA20_POLY1305)
        uint32_t a[41];
#else /* PSA_NEED_OBERON_ASCON_AEAD128 */
        uint32_t a[7];
#endif
        size_t s[2];
    } ctx;
    size_t ad_length;
    size_t pt_length;
    psa_algorithm_t alg;
    uint8_t decrypt;
    uint8_t length_set;
    uint8_t tag_length;
} oberon_aead_operation_t;


psa_status_t oberon_aead_encrypt_setup(
    oberon_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg);

psa_status_t oberon_aead_decrypt_setup(
    oberon_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg);

psa_status_t oberon_aead_set_lengths(
    oberon_aead_operation_t *operation,
    size_t ad_length,
    size_t plaintext_length);

psa_status_t oberon_aead_set_nonce(
    oberon_aead_operation_t *operation,
    const uint8_t *nonce, size_t nonce_length);

psa_status_t oberon_aead_update_ad(
    oberon_aead_operation_t *operation,
    const uint8_t *input, size_t input_length);

psa_status_t oberon_aead_update(
    oberon_aead_operation_t *operation,
    const uint8_t *input, size_t input_length,
    uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t oberon_aead_finish(
    oberon_aead_operation_t *operation,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length,
    uint8_t *tag, size_t tag_size, size_t *tag_length);

psa_status_t oberon_aead_verify(
    oberon_aead_operation_t *operation,
    uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length,
    const uint8_t *tag, size_t tag_length);

psa_status_t oberon_aead_abort(
    oberon_aead_operation_t *operation);


psa_status_t oberon_aead_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *plaintext, size_t plaintext_length,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length);

psa_status_t oberon_aead_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *ciphertext, size_t ciphertext_length,
    uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length);


#ifdef __cplusplus
}
#endif

#endif
