/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ml_dsa87 ML-DSA-87
 * @ingroup ocrypto_asym_sig
 * @{
 * @brief ML-DSA-87 signature algorithm based on NIST PQC standard.
 *
 * ML-DSA-87 is a specific implementation of CRYSTALS-Dilithium, a PQC digital
 * signature scheme. It is based on learning with errors (LWE) and is designed to
 * be resistant against quantum attacks.
 *
 * @see [FIPS - Module-Lattice-Based Digital Signature Standard](https://csrc.nist.gov/pubs/fips/204/final)
 */

/**
 * @file
 * @brief ML-DSA-87 (FIPS-204 variant of CRYSTALS-Dilithium) API.
 */

#ifndef OCRYPTO_ML_DSA87_H
#define OCRYPTO_ML_DSA87_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_sha3.h"


#ifdef __cplusplus
extern "C" {
#endif


#if defined(OCRYPTO_ML_DSA_SMALL) && defined(OCRYPTO_ML_DSA_FAST)
#error  "OCRYPTO_ML_DSA_SMALL & OCRYPTO_ML_DSA_FAST must not be defined simultaneously"
#endif


/**
 * Public key size in bytes.
 */
#define ocrypto_ml_dsa87_PK_SIZE  (2592)

/**
 * Secret key size in bytes.
 */
#define ocrypto_ml_dsa87_SK_SIZE  (4896)

/**
 * Signature size in bytes.
 */
#define ocrypto_ml_dsa87_SIG_SIZE (4627)


/**@cond */
typedef struct {
    ocrypto_sha3_ctx sha3;
    int32_t t[256];
    int32_t c[256];
    int32_t s[256];
} ocrypto_ml_dsa87_ctx_s;

typedef struct {
    ocrypto_sha3_ctx sha3;
    int32_t t[256];
    int32_t u[8][256];
} ocrypto_ml_dsa87_ctx_m;

typedef struct {
    ocrypto_sha3_ctx sha3;
    int32_t t[256];
    int32_t u[8][256];
    int32_t a[8][7][256];
    int32_t s1[7][256];
    int32_t s2[8][256];
    int32_t y[7][256];
    int32_t t0[8][256];
} ocrypto_ml_dsa87_ctx_f;

#if defined(OCRYPTO_ML_DSA_SMALL)
typedef ocrypto_ml_dsa87_ctx_s ocrypto_ml_dsa87_ctx;
#elif defined(OCRYPTO_ML_DSA_FAST)
typedef ocrypto_ml_dsa87_ctx_f ocrypto_ml_dsa87_ctx;
#else
typedef ocrypto_ml_dsa87_ctx_m ocrypto_ml_dsa87_ctx;
#endif


int ocrypto_ml_dsa87_key_pair_s(
    ocrypto_ml_dsa87_ctx_s *ctx,
    uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    uint8_t pk[ocrypto_ml_dsa87_PK_SIZE],
    const uint8_t zeta[32]);
int ocrypto_ml_dsa87_key_pair_m(
    ocrypto_ml_dsa87_ctx_m *ctx,
    uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    uint8_t pk[ocrypto_ml_dsa87_PK_SIZE],
    const uint8_t zeta[32]);

int ocrypto_ml_dsa87_sign_s(
    ocrypto_ml_dsa87_ctx_s *ctx,
    uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *m, size_t mlen,
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    const uint8_t rnd[32]);
int ocrypto_ml_dsa87_sign_m(
    ocrypto_ml_dsa87_ctx_m *ctx,
    uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *m, size_t mlen,
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    const uint8_t rnd[32]);
int ocrypto_ml_dsa87_sign_f(
    ocrypto_ml_dsa87_ctx_f *ctx,
    uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *m, size_t mlen,
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    const uint8_t rnd[32]);

int ocrypto_ml_dsa87_sign_hash_s(
    ocrypto_ml_dsa87_ctx_s *ctx,
    uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *hash, size_t hash_len,
    const uint8_t oid[11],
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    const uint8_t rnd[32]);
int ocrypto_ml_dsa87_sign_hash_m(
    ocrypto_ml_dsa87_ctx_m *ctx,
    uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *hash, size_t hash_len,
    const uint8_t oid[11],
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    const uint8_t rnd[32]);
int ocrypto_ml_dsa87_sign_hash_f(
    ocrypto_ml_dsa87_ctx_f *ctx,
    uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *hash, size_t hash_len,
    const uint8_t oid[11],
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    const uint8_t rnd[32]);

int ocrypto_ml_dsa87_verify_s(
    ocrypto_ml_dsa87_ctx_s *ctx,
    const uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *m, size_t mlen,
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t pk[ocrypto_ml_dsa87_PK_SIZE]);
int ocrypto_ml_dsa87_verify_m(
    ocrypto_ml_dsa87_ctx_m *ctx,
    const uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *m, size_t mlen,
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t pk[ocrypto_ml_dsa87_PK_SIZE]);

int ocrypto_ml_dsa87_verify_hash_s(
    ocrypto_ml_dsa87_ctx_s *ctx,
    const uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *hash, size_t hash_len,
    const uint8_t oid[11],
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t pk[ocrypto_ml_dsa87_PK_SIZE]);
int ocrypto_ml_dsa87_verify_hash_m(
    ocrypto_ml_dsa87_ctx_m *ctx,
    const uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *hash, size_t hash_len,
    const uint8_t oid[11],
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t pk[ocrypto_ml_dsa87_PK_SIZE]);
/**@endcond */


/**
 * Generates a key pair.
 *
 * @param[out] ctx  Context.
 * @param[out] sk   Secret key (can be NULL).
 * @param[out] pk   Public key (can be NULL).
 * @param[in]  zeta Random seed (32 bytes).
 * @return 0 on success, non-zero on failure.
 */
static inline int ocrypto_ml_dsa87_key_pair(
    ocrypto_ml_dsa87_ctx *ctx,
    uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    uint8_t pk[ocrypto_ml_dsa87_PK_SIZE],
    const uint8_t zeta[32])
{
#if defined(OCRYPTO_ML_DSA_SMALL)
    return ocrypto_ml_dsa87_key_pair_s(ctx, sk, pk, zeta);
#else
    return ocrypto_ml_dsa87_key_pair_m((ocrypto_ml_dsa87_ctx_m*)ctx, sk, pk, zeta);
#endif
}

/**
 * Signs a message.
 *
 * @param[in]  ctx     Context.
 * @param[out] sig     Generated signature.
 * @param[in]  m       Message to sign.
 * @param[in]  mlen    Length of the message.
 * @param[in]  ctx_str Context string (optional).
 * @param[in]  ctx_len Length of the context string (0 <= ctx_len < 256).
 * @param[in]  sk      Secret key.
 * @param[in]  rnd     Random seed (32 bytes, NULL for deterministic signing).
 * @return 0 on success, non-zero on failure.
 */
static inline int ocrypto_ml_dsa87_sign(
    ocrypto_ml_dsa87_ctx *ctx,
    uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *m, size_t mlen,
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    const uint8_t rnd[32])
{
#if defined(OCRYPTO_ML_DSA_SMALL)
    return ocrypto_ml_dsa87_sign_s(ctx, sig, m, mlen, ctx_str, ctx_len, sk, rnd);
#elif defined(OCRYPTO_ML_DSA_FAST)
    return ocrypto_ml_dsa87_sign_f(ctx, sig, m, mlen, ctx_str, ctx_len, sk, rnd);
#else
    return ocrypto_ml_dsa87_sign_m(ctx, sig, m, mlen, ctx_str, ctx_len, sk, rnd);
#endif
}

/**
 * Signs a hash.
 *
 * @param[in]  ctx      Context.
 * @param[out] sig      Generated signature.
 * @param[in]  hash     Hash to sign.
 * @param[in]  hash_len Length of the hash.
 * @param[in]  oid      Object identifier (11 bytes).
 * @param[in]  ctx_str  Context string (optional).
 * @param[in]  ctx_len  Length of the context string (0 <= ctx_len < 256).
 * @param[in]  sk       Secret key.
 * @param[in]  rnd      Random seed (32 bytes, NULL for deterministic signing).
 * @return 0 on success, non-zero on failure.
 */
static inline int ocrypto_ml_dsa87_sign_hash(
    ocrypto_ml_dsa87_ctx *ctx,
    uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *hash, size_t hash_len,
    const uint8_t oid[11],
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t sk[ocrypto_ml_dsa87_SK_SIZE],
    const uint8_t rnd[32])
{
#if defined(OCRYPTO_ML_DSA_SMALL)
    return ocrypto_ml_dsa87_sign_hash_s(ctx, sig, hash, hash_len, oid, ctx_str, ctx_len, sk, rnd);
#elif defined(OCRYPTO_ML_DSA_FAST)
    return ocrypto_ml_dsa87_sign_hash_f(ctx, sig, hash, hash_len, oid, ctx_str, ctx_len, sk, rnd);
#else
    return ocrypto_ml_dsa87_sign_hash_m(ctx, sig, hash, hash_len, oid, ctx_str, ctx_len, sk, rnd);
#endif
}

/**
 * Verifies a signature against a message.
 *
 * @param[in] ctx     Context.
 * @param[in] sig     Signature to verify.
 * @param[in] m       Message to verify.
 * @param[in] mlen    Length of the message.
 * @param[in] ctx_str Context string (optional).
 * @param[in] ctx_len Length of the context string (0 <= ctx_len < 256).
 * @param[in] pk      Public key.
 * @return 0 on success, non-zero on failure.
 */
static inline int ocrypto_ml_dsa87_verify(
    ocrypto_ml_dsa87_ctx *ctx,
    const uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *m, size_t mlen,
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t pk[ocrypto_ml_dsa87_PK_SIZE])
{
#if defined(OCRYPTO_ML_DSA_SMALL)
    return ocrypto_ml_dsa87_verify_s(ctx, sig, m, mlen, ctx_str, ctx_len, pk);
#else
    return ocrypto_ml_dsa87_verify_m((ocrypto_ml_dsa87_ctx_m*)ctx, sig, m, mlen, ctx_str, ctx_len, pk);
#endif
}

/**
 * Verifies a signature against a hash.
 *
 * @param[in] ctx      Context.
 * @param[in] sig      Signature to verify.
 * @param[in] hash     Hash to verify.
 * @param[in] hash_len Length of the hash.
 * @param[in] oid      Object identifier (11 bytes).
 * @param[in] ctx_str  Context string (optional).
 * @param[in] ctx_len  Length of the context string (0 <= ctx_len < 256).
 * @param[in] pk       Public key.
 * @return 0 on success, non-zero on failure.
 */
static inline int ocrypto_ml_dsa87_verify_hash(
    ocrypto_ml_dsa87_ctx *ctx,
    const uint8_t sig[ocrypto_ml_dsa87_SIG_SIZE],
    const uint8_t *hash, size_t hash_len,
    const uint8_t oid[11],
    const uint8_t *ctx_str, size_t ctx_len,
    const uint8_t pk[ocrypto_ml_dsa87_PK_SIZE])
{
#if defined(OCRYPTO_ML_DSA_SMALL)
    return ocrypto_ml_dsa87_verify_hash_s(ctx, sig, hash, hash_len, oid, ctx_str, ctx_len, pk);
#else
    return ocrypto_ml_dsa87_verify_hash_m((ocrypto_ml_dsa87_ctx_m*)ctx, sig, hash, hash_len, oid, ctx_str, ctx_len, pk);
#endif
}


#ifdef __cplusplus
}
#endif

#endif

/** @} */
