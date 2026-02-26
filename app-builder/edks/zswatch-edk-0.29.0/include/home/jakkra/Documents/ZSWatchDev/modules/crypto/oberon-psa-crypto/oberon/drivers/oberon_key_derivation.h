/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//
// This file is based on the Arm PSA Crypto Driver API.

#ifndef OBERON_KEY_DERIVATION_H
#define OBERON_KEY_DERIVATION_H

#include <psa/crypto_driver_common.h>
#include <psa/crypto_struct.h>


#ifdef __cplusplus
extern "C" {
#endif


#define OBERON_KDF_MAX_INFO_BYTES  256
#define OBERON_TLS12_PRF_MAX_KEY_BYTES  516


typedef enum {
    OBERON_HKDF_ALG = 1,
    OBERON_HKDF_EXTRACT_ALG = 2,
    OBERON_HKDF_EXPAND_ALG = 3,
    OBERON_PBKDF2_HMAC_ALG = 4,
    OBERON_PBKDF2_CMAC_ALG = 5,
    OBERON_TLS12_PRF_ALG = 6,
    OBERON_TLS12_PSK_TO_MS_ALG = 7,
    OBERON_ECJPAKE_TO_PMS_ALG = 8,
    OBERON_SP800_108_COUNTER_ALG = 9,
    OBERON_SRP_PASSWORD_HASH_ALG = 10,
    OBERON_WPA3_SAE_H2E_ALG = 11,
} oberon_kdf_alg;

typedef struct {
    union {
        psa_mac_operation_t  mac_op;
        psa_hash_operation_t hash_op;
    };
    uint8_t         data[PSA_HASH_MAX_SIZE];
    uint8_t         info[OBERON_KDF_MAX_INFO_BYTES];
#if defined(PSA_NEED_OBERON_TLS12_PRF) || defined(PSA_NEED_OBERON_TLS12_PSK_TO_MS)
    uint8_t         key[OBERON_TLS12_PRF_MAX_KEY_BYTES];
    uint8_t         a[PSA_HASH_MAX_SIZE];
#else
    uint8_t         key[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
#endif
    psa_algorithm_t mac_alg;
    psa_key_type_t  key_type;
    uint8_t         data_length;
    uint8_t         block_length;
    uint16_t        salt_length;
    uint16_t        info_length;
    uint16_t        key_length;
    uint32_t        index;
    uint32_t        count;
    oberon_kdf_alg  alg;
} oberon_key_derivation_operation_t;


psa_status_t oberon_key_derivation_setup(
    oberon_key_derivation_operation_t *operation,
    psa_algorithm_t alg);

psa_status_t oberon_key_derivation_set_capacity(
    oberon_key_derivation_operation_t *operation,
    size_t capacity);

psa_status_t oberon_key_derivation_input_bytes(
    oberon_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step,
    const uint8_t *data, size_t data_length);

psa_status_t oberon_key_derivation_input_integer(
    oberon_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step,
    uint64_t value);

psa_status_t oberon_key_derivation_output_bytes(
    oberon_key_derivation_operation_t *operation,
    uint8_t *output, size_t output_length);

psa_status_t oberon_key_derivation_abort(
    oberon_key_derivation_operation_t *operation );


#ifdef __cplusplus
}
#endif

#endif
