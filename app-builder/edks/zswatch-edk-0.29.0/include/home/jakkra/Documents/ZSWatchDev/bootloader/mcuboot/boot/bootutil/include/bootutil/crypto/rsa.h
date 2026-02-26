/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2023 Arm Limited
 */

/*
 * This module provides a thin abstraction over some of the crypto
 * primitives to make it easier to swap out the used crypto library.
 *
 * At this point, the choices are: MCUBOOT_USE_MBED_TLS and
 * MCUBOOT_USE_PSA_CRYPTO. Note that support for  MCUBOOT_USE_PSA_CRYPTO is
 * still experimental and it might not support all the crypto abstractions
 * that MCUBOOT_USE_MBED_TLS supports. For this reason, it's allowed to have
 * both of them defined, and for crypto modules that support both abstractions,
 * the MCUBOOT_USE_PSA_CRYPTO will take precedence.
 */

/*
 * Note: The source file that includes this header should either define one of the
 * two options BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED or BOOTUTIL_CRYPTO_RSA_SIGN_ENABLED
 * This will make the signature functions or encryption functions visible without
 * generating a "defined but not used" compiler warning
 */

#ifndef __BOOTUTIL_CRYPTO_RSA_H_
#define __BOOTUTIL_CRYPTO_RSA_H_

#include "mcuboot_config/mcuboot_config.h"

#if defined(MCUBOOT_USE_PSA_CRYPTO) || defined(MCUBOOT_USE_MBED_TLS)
#define MCUBOOT_USE_PSA_OR_MBED_TLS
#endif /* MCUBOOT_USE_PSA_CRYPTO || MCUBOOT_USE_MBED_TLS */

#if (defined(MCUBOOT_USE_PSA_OR_MBED_TLS)) != 1
    #error "One crypto backend must be defined: either MBED_TLS/PSA_CRYPTO"
#endif

#if defined(MCUBOOT_USE_PSA_CRYPTO)

#include <psa/crypto.h>
#include "bootutil/enc_key_public.h"

#elif defined(MCUBOOT_USE_MBED_TLS)

#include "mbedtls/rsa.h"
#include "mbedtls/version.h"
#if defined(BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED)
#if MBEDTLS_VERSION_NUMBER >= 0x03000000
#include "rsa_alt_helpers.h"
#else
#include "mbedtls/rsa_internal.h"
#endif
#endif /* BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED */
#include "mbedtls/asn1.h"
#include "bootutil/crypto/common.h"

#endif /* MCUBOOT_USE_MBED_TLS */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MCUBOOT_USE_PSA_CRYPTO)

typedef struct {
    psa_key_id_t key_id;
} bootutil_rsa_context;
typedef bootutil_rsa_context bootutil_key_exchange_ctx;

static inline void bootutil_rsa_init(bootutil_rsa_context *ctx)
{
    ctx->key_id = PSA_KEY_ID_NULL;
}

static inline void bootutil_rsa_drop(bootutil_rsa_context *ctx)
{
    if (ctx->key_id != PSA_KEY_ID_NULL) {
        (void)psa_destroy_key(ctx->key_id);
    }
}

#if defined(BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED)
static int bootutil_rsa_oaep_decrypt(
    bootutil_rsa_context *ctx,
    size_t *olen,
    const uint8_t *input,
    uint8_t *output,
    size_t output_max_len)
{
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;

    /* Perform an additional defensive check to compare the modulus of the RSA
     * key to the expected input of the decryption function, i.e. TLV_ENC_RSA_SZ
     */
    psa_key_attributes_t key_attr =  psa_key_attributes_init();
    status = psa_get_key_attributes(ctx->key_id, &key_attr);
    if (status != PSA_SUCCESS) {
        return -1;
    }
    size_t input_size = PSA_BITS_TO_BYTES(psa_get_key_bits(&key_attr));
    if (input_size != BOOT_ENC_TLV_SIZE) {
        return -1;
    }

    status = psa_asymmetric_decrypt(ctx->key_id, PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256),
                                    input, BOOT_ENC_TLV_SIZE, NULL, 0,
                                    output, output_max_len, olen);
    return (int)status;
}

/*
 * Parse a RSA private key with format specified in RFC3447 A.1.2
 *
 * The key is meant to be used for OAEP decrypt hence algorithm and usage are hardcoded
 */
static int
bootutil_rsa_parse_private_key(bootutil_rsa_context *ctx, uint8_t **p, uint8_t *end)
{
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_attributes_t key_attributes = psa_key_attributes_init();

    /* Set attributes and import key */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);

    status = psa_import_key(&key_attributes, *p, (end - *p), &ctx->key_id);
    return (int)status;
}

#endif /* BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED */

#if defined(BOOTUTIL_CRYPTO_RSA_SIGN_ENABLED)
/*
 * Parse a RSA public key with format specified in RFC3447 A.1.1
 *
 * The key is meant to be used for PSS signature verification hence algorithm and usage are hardcoded
 */
static int
bootutil_rsa_parse_public_key(bootutil_rsa_context *ctx, uint8_t **p, uint8_t *end)
{
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_attributes_t key_attributes = psa_key_attributes_init();

    /* Set attributes and import key */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_RSA_PSS(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_RSA_PUBLIC_KEY);

    status = psa_import_key(&key_attributes, *p, (end - *p), &ctx->key_id);
    return (int)status;
}

/* Get the modulus (N) length in bytes */
static size_t bootutil_rsa_get_len(const bootutil_rsa_context *ctx)
{
    psa_key_attributes_t key_attributes = psa_key_attributes_init();
    psa_status_t status = psa_get_key_attributes(ctx->key_id, &key_attributes);
    if (status != PSA_SUCCESS) {
        return 0;
    }
    return PSA_BITS_TO_BYTES(psa_get_key_bits(&key_attributes));
}

/* PSA Crypto has a dedicated API for RSASSA-PSS verification */
static inline int bootutil_rsassa_pss_verify(const bootutil_rsa_context *ctx,
    uint8_t *hash, size_t hlen, uint8_t *sig, size_t slen)
{
    return (int) psa_verify_hash(ctx->key_id, PSA_ALG_RSA_PSS(PSA_ALG_SHA_256),
                                 hash, hlen, sig, slen);
}
#endif /* BOOTUTIL_CRYPTO_RSA_SIGN_ENABLED */

#elif defined(MCUBOOT_USE_MBED_TLS)

typedef mbedtls_rsa_context bootutil_rsa_context;
typedef bootutil_rsa_context bootutil_key_exchange_ctx;

static inline void bootutil_rsa_init(bootutil_rsa_context *ctx)
{
#if MBEDTLS_VERSION_NUMBER >= 0x03000000
    mbedtls_rsa_init(ctx);
    mbedtls_rsa_set_padding(ctx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
#else
    mbedtls_rsa_init(ctx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
#endif
}

static inline void bootutil_rsa_drop(bootutil_rsa_context *ctx)
{
    mbedtls_rsa_free(ctx);
}

#if defined(BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED) && (MBEDTLS_VERSION_NUMBER >= 0x03000000)
static int fake_rng(void *p_rng, unsigned char *output, size_t len);
#endif /* BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED && MBEDTLS_VERSION_NUMBER >= 3.0 */

#if defined(BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED)
static inline int bootutil_rsa_oaep_decrypt(
    bootutil_rsa_context *ctx,
    size_t *olen,
    const uint8_t *input,
    uint8_t *output,
    size_t output_max_len)
{
    int rc = -1;
#if MBEDTLS_VERSION_NUMBER >= 0x03000000
    rc = mbedtls_rsa_rsaes_oaep_decrypt(ctx, fake_rng, NULL,
            NULL, 0, olen, input, output, output_max_len);
#else
    rc = mbedtls_rsa_rsaes_oaep_decrypt(ctx, NULL, NULL, MBEDTLS_RSA_PRIVATE,
            NULL, 0, olen, input, output, output_max_len);
#endif
    return rc;
}

/*
 * Parse a RSA private key with format specified in RFC3447 A.1.2
 */
static int
bootutil_rsa_parse_private_key(bootutil_rsa_context *ctx, uint8_t **p, uint8_t *end)
{
    size_t len;

    if (mbedtls_asn1_get_tag(p, end, &len,
                MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE) != 0) {
        return -1;
    }

    if (*p + len != end) {
        return -2;
    }

    /* Non-optional fields. */
    if ( /* version */
        mbedtls_asn1_get_int(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(ver)) != 0 ||
         /* public modulus */
        mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(N)) != 0 ||
         /* public exponent */
        mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(E)) != 0 ||
         /* private exponent */
        mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(D)) != 0 ||
         /* primes */
        mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(P)) != 0 ||
        mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(Q)) != 0) {

        return -3;
    }

#if !defined(MBEDTLS_RSA_NO_CRT)
    /*
     * DP/DQ/QP are only used inside mbedTLS if it was built with the
     * Chinese Remainder Theorem enabled (default). In case it is disabled
     * we parse, or if not available, we calculate those values.
     */
    if (*p < end) {
        if ( /* d mod (p-1) and d mod (q-1) */
            mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(DP)) != 0 ||
            mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(DQ)) != 0 ||
             /* q ^ (-1) mod p */
            mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(QP)) != 0) {

            return -4;
        }
    } else {
        if (mbedtls_rsa_deduce_crt(&ctx->MBEDTLS_CONTEXT_MEMBER(P),
                                   &ctx->MBEDTLS_CONTEXT_MEMBER(Q),
                                   &ctx->MBEDTLS_CONTEXT_MEMBER(D),
                                   &ctx->MBEDTLS_CONTEXT_MEMBER(DP),
                                   &ctx->MBEDTLS_CONTEXT_MEMBER(DQ),
                                   &ctx->MBEDTLS_CONTEXT_MEMBER(QP)) != 0) {
            return -5;
        }
    }
#endif /* !MBEDTLS_RSA_NO_CRT */

    ctx->MBEDTLS_CONTEXT_MEMBER(len) = mbedtls_mpi_size(&ctx->MBEDTLS_CONTEXT_MEMBER(N));

    if (mbedtls_rsa_check_privkey(ctx) != 0) {
        return -6;
    }

    return 0;
}
#endif /* BOOTUTIL_CRYPTO_RSA_CRYPT_ENABLED */

#if defined(BOOTUTIL_CRYPTO_RSA_SIGN_ENABLED)
/*
 * Parse a RSA public key with format specified in RFC3447 A.1.1
 */
static int
bootutil_rsa_parse_public_key(bootutil_rsa_context *ctx, uint8_t **p, uint8_t *end)
{
    int rc;
    size_t len;

    if ((rc = mbedtls_asn1_get_tag(p, end, &len,
          MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) != 0) {
        return -1;
    }

    if (*p + len != end) {
        return -2;
    }

    if ((rc = mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(N))) != 0 ||
        (rc = mbedtls_asn1_get_mpi(p, end, &ctx->MBEDTLS_CONTEXT_MEMBER(E))) != 0) {
        return -3;
    }

    ctx->MBEDTLS_CONTEXT_MEMBER(len) = mbedtls_mpi_size(&ctx->MBEDTLS_CONTEXT_MEMBER(N));

    if (*p != end) {
        return -4;
    }

    /* The Mbed TLS version is more than 2.6.1 */
#if MBEDTLS_VERSION_NUMBER > 0x02060100
    rc = mbedtls_rsa_import(ctx, &ctx->MBEDTLS_CONTEXT_MEMBER(N), NULL,
                            NULL, NULL, &ctx->MBEDTLS_CONTEXT_MEMBER(E));
    if (rc != 0) {
        return -5;
    }
#endif

    rc = mbedtls_rsa_check_pubkey(ctx);
    if (rc != 0) {
        return -6;
    }

    ctx->MBEDTLS_CONTEXT_MEMBER(len) = mbedtls_mpi_size(&ctx->MBEDTLS_CONTEXT_MEMBER(N));

    return 0;
}

/* Get the modulus (N) length in bytes */
static inline size_t bootutil_rsa_get_len(const bootutil_rsa_context *ctx)
{
    return mbedtls_rsa_get_len(ctx);
}

/* Performs modular exponentiation using the public key output = input^E mod N */
static inline int bootutil_rsa_public(bootutil_rsa_context *ctx, const uint8_t *input, uint8_t *output)
{
    return mbedtls_rsa_public(ctx, input, output);
}
#endif /* BOOTUTIL_CRYPTO_RSA_SIGN_ENABLED */

#endif /* MCUBOOT_USE_MBED_TLS */

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_CRYPTO_RSA_H_ */
