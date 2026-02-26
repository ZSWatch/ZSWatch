/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2017-2019 Linaro LTD
 * Copyright (c) 2017-2019 JUUL Labs
 * Copyright (c) 2021-2023 Arm Limited
 */

/*
 * This module provides a thin abstraction over some of the crypto
 * primitives to make it easier to swap out the used crypto library.
 *
 * At this point, the choices are: MCUBOOT_USE_MBED_TLS, MCUBOOT_USE_TINYCRYPT,
 * MCUBOOT_USE_PSA_CRYPTO, MCUBOOT_USE_CC310. Note that support for MCUBOOT_USE_PSA_CRYPTO
 * is still experimental and it might not support all the crypto abstractions
 * that MCUBOOT_USE_MBED_TLS supports. For this reason, it's allowed to have
 * both of them defined, and for crypto modules that support both abstractions,
 * the MCUBOOT_USE_PSA_CRYPTO will take precedence.
 */

#ifndef __BOOTUTIL_CRYPTO_SHA_H_
#define __BOOTUTIL_CRYPTO_SHA_H_

#include "mcuboot_config/mcuboot_config.h"
#include "mcuboot_config/mcuboot_logging.h"

#if defined(MCUBOOT_USE_PSA_CRYPTO) || defined(MCUBOOT_USE_MBED_TLS)
#define MCUBOOT_USE_PSA_OR_MBED_TLS
#endif /* MCUBOOT_USE_PSA_CRYPTO || MCUBOOT_USE_MBED_TLS */

#if (defined(MCUBOOT_USE_PSA_OR_MBED_TLS) + \
     defined(MCUBOOT_USE_TINYCRYPT) + \
     defined(MCUBOOT_USE_NRF_EXTERNAL_CRYPTO) + \
     defined(MCUBOOT_USE_CC310)) != 1
    #error "One crypto backend must be defined: either CC310/MBED_TLS/TINYCRYPT/PSA_CRYPTO"
#endif

#if defined(MCUBOOT_SHA512)
    #define IMAGE_HASH_SIZE (64)
    #define EXPECTED_HASH_TLV IMAGE_TLV_SHA512
#elif defined(MCUBOOT_SIGN_EC384)
    #define IMAGE_HASH_SIZE (48)
    #define EXPECTED_HASH_TLV IMAGE_TLV_SHA384
#else
    #define IMAGE_HASH_SIZE (32)
    #define EXPECTED_HASH_TLV IMAGE_TLV_SHA256
#endif /* MCUBOOT_SIGN */

/* Universal defines for SHA-256 */
#define BOOTUTIL_CRYPTO_SHA256_BLOCK_SIZE  (64)
#define BOOTUTIL_CRYPTO_SHA256_DIGEST_SIZE (32)

#if defined(MCUBOOT_USE_PSA_CRYPTO)

#include <psa/crypto.h>

#elif defined(MCUBOOT_USE_MBED_TLS)

#ifdef MCUBOOT_SHA512
#include <mbedtls/sha512.h>
#else
#include <mbedtls/sha256.h>
#endif

#include <mbedtls/version.h>
#if MBEDTLS_VERSION_NUMBER >= 0x03000000
#include <mbedtls/compat-2.x.h>
#endif

#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_TINYCRYPT)
#if defined(MCUBOOT_SHA512)
    #include <tinycrypt/sha512.h>
#else
    #include <tinycrypt/sha256.h>
#endif
    #include <tinycrypt/constants.h>
#endif /* MCUBOOT_USE_TINYCRYPT */

#if defined(MCUBOOT_USE_CC310)
    #include <cc310_glue.h>
#endif /* MCUBOOT_USE_CC310 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MCUBOOT_USE_PSA_CRYPTO)

typedef psa_hash_operation_t bootutil_sha_context;

static inline int bootutil_sha_init(bootutil_sha_context *ctx)
{
    *ctx = psa_hash_operation_init();
#if defined(MCUBOOT_SHA512)
    psa_status_t status = psa_hash_setup(ctx, PSA_ALG_SHA_512);
#elif defined(MCUBOOT_SIGN_EC384)
    psa_status_t status = psa_hash_setup(ctx, PSA_ALG_SHA_384);
#else
    psa_status_t status = psa_hash_setup(ctx, PSA_ALG_SHA_256);
#endif
    return (int)status;
}

static inline int bootutil_sha_drop(bootutil_sha_context *ctx)
{
    return (int)psa_hash_abort(ctx);
}

static inline int bootutil_sha_update(bootutil_sha_context *ctx,
                                      const void *data,
                                      uint32_t data_len)
{
    return (int)psa_hash_update(ctx, data, data_len);
}

static inline int bootutil_sha_finish(bootutil_sha_context *ctx,
                                      uint8_t *output)
{
    size_t hash_length = 0;
    /* Assumes the output buffer is at least the expected size of the hash */
#if defined(MCUBOOT_SHA512)
    return (int)psa_hash_finish(ctx, output, PSA_HASH_LENGTH(PSA_ALG_SHA_512), &hash_length);
#elif defined(MCUBOOT_SIGN_EC384)
    return (int)psa_hash_finish(ctx, output, PSA_HASH_LENGTH(PSA_ALG_SHA_384), &hash_length);
#else
    return (int)psa_hash_finish(ctx, output, PSA_HASH_LENGTH(PSA_ALG_SHA_256), &hash_length);
#endif
}

#elif defined(MCUBOOT_USE_MBED_TLS)

#ifdef MCUBOOT_SHA512
typedef mbedtls_sha512_context bootutil_sha_context;
#else
typedef mbedtls_sha256_context bootutil_sha_context;
#endif

static inline int bootutil_sha_init(bootutil_sha_context *ctx)
{
    int ret;

#ifdef MCUBOOT_SHA512
    mbedtls_sha512_init(ctx);
    ret = mbedtls_sha512_starts_ret(ctx, 0);
#else
    mbedtls_sha256_init(ctx);
    ret = mbedtls_sha256_starts_ret(ctx, 0);
#endif

    return ret;
}

static inline int bootutil_sha_drop(bootutil_sha_context *ctx)
{
#ifdef MCUBOOT_SHA512
    mbedtls_sha512_free(ctx);
#else
    mbedtls_sha256_free(ctx);
#endif

    return 0;
}

static inline int bootutil_sha_update(bootutil_sha_context *ctx,
                                      const void *data,
                                      uint32_t data_len)
{
    int ret;

#ifdef MCUBOOT_SHA512
    ret = mbedtls_sha512_update_ret(ctx, data, data_len);
#else
    ret = mbedtls_sha256_update_ret(ctx, data, data_len);
#endif

    return ret;
}

static inline int bootutil_sha_finish(bootutil_sha_context *ctx,
                                      uint8_t *output)
{
    int ret;

#ifdef MCUBOOT_SHA512
    ret = mbedtls_sha512_finish_ret(ctx, output);
#else
    ret = mbedtls_sha256_finish_ret(ctx, output);
#endif

    return ret;
}

#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_TINYCRYPT)
#if defined(MCUBOOT_SHA512)
typedef struct tc_sha512_state_struct bootutil_sha_context;
#else
typedef struct tc_sha256_state_struct bootutil_sha_context;
#endif

static inline int bootutil_sha_init(bootutil_sha_context *ctx)
{
#if defined(MCUBOOT_SHA512)
    tc_sha512_init(ctx);
#else
    tc_sha256_init(ctx);
#endif
    return 0;
}

static inline int bootutil_sha_drop(bootutil_sha_context *ctx)
{
    (void)ctx;
    return 0;
}

static inline int bootutil_sha_update(bootutil_sha_context *ctx,
                                      const void *data,
                                      uint32_t data_len)
{
#if defined(MCUBOOT_SHA512)
    return tc_sha512_update(ctx, data, data_len);
#else
    return tc_sha256_update(ctx, data, data_len);
#endif
}

static inline int bootutil_sha_finish(bootutil_sha_context *ctx,
                                      uint8_t *output)
{
#if defined(MCUBOOT_SHA512)
    return tc_sha512_final(output, ctx);
#else
    return tc_sha256_final(output, ctx);
#endif
}
#endif /* MCUBOOT_USE_TINYCRYPT */

#if defined(MCUBOOT_USE_CC310)
static inline int bootutil_sha_init(bootutil_sha_context *ctx)
{
    cc310_sha256_init(ctx);
    return 0;
}

static inline int bootutil_sha_drop(bootutil_sha_context *ctx)
{
    (void)ctx;
    nrf_cc310_disable();
    return 0;
}

static inline int bootutil_sha_update(bootutil_sha_context *ctx,
                                      const void *data,
                                      uint32_t data_len)
{
    cc310_sha256_update(ctx, data, data_len);
    return 0;
}

static inline int bootutil_sha_finish(bootutil_sha_context *ctx,
                                      uint8_t *output)
{
    cc310_sha256_finalize(ctx, output);
    return 0;
}
#endif /* MCUBOOT_USE_CC310 */

#if defined(MCUBOOT_USE_NRF_EXTERNAL_CRYPTO)

#include <bl_crypto.h>

typedef bl_sha256_ctx_t bootutil_sha_context;

static inline void bootutil_sha_init(bootutil_sha_context *ctx)
{
    bl_sha256_init(ctx);
}

static inline void bootutil_sha_drop(bootutil_sha_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_sha_update(bootutil_sha_context *ctx,
                                      const void *data,
                                      uint32_t data_len)
{
    return bl_sha256_update(ctx, data, data_len);
}

static inline int bootutil_sha_finish(bootutil_sha_context *ctx,
                                      uint8_t *output)
{
    bl_sha256_finalize(ctx, output);
    return 0;
}
#endif /* MCUBOOT_USE_NRF_EXTERNAL_CRYPTO */

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_CRYPTO_SHA_H_ */
