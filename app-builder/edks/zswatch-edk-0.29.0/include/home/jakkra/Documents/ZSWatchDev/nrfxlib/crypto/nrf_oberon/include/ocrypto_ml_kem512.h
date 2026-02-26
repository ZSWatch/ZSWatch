/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ml_kem512 ML-KEM-512
 * @ingroup ocrypto_key_agreement
 * @{
 * @brief ML-KEM-512 key encapsulation algorithm based on NIST PQC standard.
 *
 * ML-KEM-512 is a specific implementation of CRYSTALS-Kyber, a post-quantum
 * key encapsulation mechanism. It is based on learning with errors (LWE) and
 * is designed to be resistant against quantum attacks.
 *
 * @see [FIPS - Module-Lattice-Based Key-Encapsulation Mechanism Standard](https://csrc.nist.gov/pubs/fips/203/final)
 */

/**
 * @file
 * @brief ML-KEM-512 (FIPS-203 variant of CRYSTALS-Kyber) API.
 */

#ifndef OCRYPTO_ML_KEM512_H
#define OCRYPTO_ML_KEM512_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_sha3.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Secret key size in bytes.
 */
#define ocrypto_ml_kem512_SK_SIZE ((2 * 384) + (2 * 384 + 32) + 64)

/**
 * Public key size in bytes.
 */
#define ocrypto_ml_kem512_PK_SIZE (2 * 384 + 32)

/**
 * Ciphertext size in bytes.
 */
#define ocrypto_ml_kem512_CT_SIZE ((2 * 10 + 4) * 32)


/**@cond */
typedef struct {
    ocrypto_sha3_ctx sha3;
    int16_t u[2][256];
    int16_t t[256];
    uint8_t kr[64];
} ocrypto_ml_kem512_ctx;
/**@endcond */


/**
 * Generates a key pair.
 *
 * @param[out] ctx Context.
 * @param[out] sk  Secret key (can be NULL).
 * @param[out] pk  Public key (can be NULL).
 * @param[in]  d   Random seed (32 bytes).
 * @param[in]  z   Random seed (32 bytes).
 */
void ocrypto_ml_kem512_key_pair(
    ocrypto_ml_kem512_ctx *ctx,
    uint8_t sk[ocrypto_ml_kem512_SK_SIZE],
    uint8_t pk[ocrypto_ml_kem512_PK_SIZE],
    const uint8_t d[32],
    const uint8_t z[32]);

/**
 * Checks the validity of a public key.
 *
 * @param[in] pk Public key to check.
 * @return 0 if valid, non-zero otherwise.
 */
int ocrypto_ml_kem512_check_key(
    const uint8_t pk[ocrypto_ml_kem512_PK_SIZE]);

/**
 * Encapsulates a key.
 *
 * @param[in]  ctx Context.
 * @param[out] key Encapsulated key (32 bytes).
 * @param[out] ct  Ciphertext.
 * @param[in]  pk  Public key.
 * @param[in]  m   Random message (32 bytes).
 */
void ocrypto_ml_kem512_encaps(
    ocrypto_ml_kem512_ctx *ctx,
    uint8_t key[32],
    uint8_t ct[ocrypto_ml_kem512_CT_SIZE],
    const uint8_t pk[ocrypto_ml_kem512_PK_SIZE],
    const uint8_t m[32]);

/**
 * Decapsulates a key.
 *
 * @param[in]  ctx Context.
 * @param[out] key Decapsulated key (32 bytes).
 * @param[in]  sk  Secret key.
 * @param[in]  ct  Ciphertext to decapsulate.
 */
void ocrypto_ml_kem512_decaps(
    ocrypto_ml_kem512_ctx *ctx,
    uint8_t key[32],
    const uint8_t sk[ocrypto_ml_kem512_SK_SIZE],
    const uint8_t ct[ocrypto_ml_kem512_CT_SIZE]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
