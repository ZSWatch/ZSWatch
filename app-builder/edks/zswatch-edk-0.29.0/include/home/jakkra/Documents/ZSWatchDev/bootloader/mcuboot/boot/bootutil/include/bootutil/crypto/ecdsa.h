/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2023-2025 Arm Limited
 */

/*
 * This module provides a thin abstraction over some of the crypto
 * primitives to make it easier to swap out the used crypto library.
 *
 * At this point, the choices are: MCUBOOT_USE_TINYCRYPT, MCUBOOT_USE_CC310,
 * MCUBOOT_USE_MBED_TLS, MCUBOOT_USE_PSA_CRYPTO. Note that support for
 * MCUBOOT_USE_PSA_CRYPTO is still experimental and it might not support all
 * the crypto abstractions that MCUBOOT_USE_MBED_TLS supports. For this
 * reason, it's allowed to have both of them defined, and for crypto modules
 * that support both abstractions, the MCUBOOT_USE_PSA_CRYPTO will take
 * precedence.
 */

#ifndef __BOOTUTIL_CRYPTO_ECDSA_H_
#define __BOOTUTIL_CRYPTO_ECDSA_H_

#include <stdint.h>
#include "mcuboot_config/mcuboot_config.h"

#if defined(MCUBOOT_USE_PSA_CRYPTO) || defined(MCUBOOT_USE_MBED_TLS)
#define MCUBOOT_USE_PSA_OR_MBED_TLS
#endif /* MCUBOOT_USE_PSA_CRYPTO || MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_SIGN_EC384) && \
    !defined(MCUBOOT_USE_PSA_CRYPTO)
    #error "P384 requires PSA_CRYPTO to be defined"
#endif

#if (defined(MCUBOOT_USE_TINYCRYPT) + \
     defined(MCUBOOT_USE_CC310) + \
     defined(MCUBOOT_USE_NRF_EXTERNAL_CRYPTO) + \
     defined(MCUBOOT_USE_PSA_OR_MBED_TLS)) != 1
    #error "One crypto backend must be defined: either CC310/TINYCRYPT/MBED_TLS/PSA_CRYPTO"
#endif

#if defined(MCUBOOT_USE_TINYCRYPT)
    #include <tinycrypt/ecc_dsa.h>
    #include <tinycrypt/constants.h>
#endif /* MCUBOOT_USE_TINYCRYPT */

#if defined(MCUBOOT_USE_CC310)
    #include <cc310_glue.h>
#endif /* MCUBOOT_USE_CC310 */

#if defined(MCUBOOT_USE_PSA_CRYPTO)
    #include <psa/crypto.h>
    #include <string.h>
#elif defined(MCUBOOT_USE_MBED_TLS)
    #include <mbedtls/ecdsa.h>
    /* Indicate to the caller that the verify function needs the raw ASN.1
     * signature, not a decoded one. */
    #define MCUBOOT_ECDSA_NEED_ASN1_SIG
#endif /* MCUBOOT_USE_MBED_TLS */

/*TODO: remove this after cypress port mbedtls to abstract crypto api */
#if defined(MCUBOOT_USE_CC310) || defined(MCUBOOT_USE_MBED_TLS)
#define NUM_ECC_BYTES (256 / 8)
#endif

/* Universal defines */
#define BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE (32)

#include "mbedtls/oid.h"
#include "mbedtls/asn1.h"
#include "bootutil/sign_key.h"
#if !defined(MCUBOOT_USE_PSA_CRYPTO)
#include "bootutil/crypto/common.h"
#endif

#if defined(MCUBOOT_USE_NRF_EXTERNAL_CRYPTO)
    #include <bl_crypto.h>
    #define NUM_ECC_BYTES (256 / 8)
#endif /* MCUBOOT_USE_NRF_EXTERNAL_CRYPTO */

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(MCUBOOT_USE_TINYCRYPT) || defined(MCUBOOT_USE_MBED_TLS) || \
     defined(MCUBOOT_USE_CC310) || defined(MCUBOOT_USE_NRF_EXTERNAL_CRYPTO)) \
     && !defined(MCUBOOT_USE_PSA_CRYPTO)
/*
 * Declaring these like this adds NULL termination.
 */
static const uint8_t ec_pubkey_oid[] = MBEDTLS_OID_EC_ALG_UNRESTRICTED;
static const uint8_t ec_secp256r1_oid[] = MBEDTLS_OID_EC_GRP_SECP256R1;

/*
 * Parse a public key. Helper function.
 */
static int bootutil_import_key(uint8_t **cp, uint8_t *end)
{
    size_t len;
    mbedtls_asn1_buf alg;
    mbedtls_asn1_buf param;

    if (mbedtls_asn1_get_tag(cp, end, &len,
        MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) {
        return -1;
    }
    end = *cp + len;

    /* ECParameters (RFC5480) */
    if (mbedtls_asn1_get_alg(cp, end, &alg, &param)) {
        return -2;
    }
    /* id-ecPublicKey (RFC5480) */
    if (alg.ASN1_CONTEXT_MEMBER(len) != sizeof(ec_pubkey_oid) - 1 ||
        memcmp(alg.ASN1_CONTEXT_MEMBER(p), ec_pubkey_oid, sizeof(ec_pubkey_oid) - 1)) {
        return -3;
    }
    /* namedCurve (RFC5480) */
    if (param.ASN1_CONTEXT_MEMBER(len) != sizeof(ec_secp256r1_oid) - 1 ||
        memcmp(param.ASN1_CONTEXT_MEMBER(p), ec_secp256r1_oid, sizeof(ec_secp256r1_oid) - 1)) {
        return -4;
    }
    /* ECPoint (RFC5480) */
    if (mbedtls_asn1_get_bitstring_null(cp, end, &len)) {
        return -6;
    }
    if (*cp + len != end) {
        return -7;
    }

    if (len != 2 * NUM_ECC_BYTES + 1) {
        return -8;
    }

    return 0;
}
#endif /* (MCUBOOT_USE_TINYCRYPT || MCUBOOT_USE_MBED_TLS || MCUBOOT_USE_CC310) && !MCUBOOT_USE_PSA_CRYPTO */

#ifndef MCUBOOT_USE_PSA_CRYPTO
/*
 * cp points to ASN1 string containing an integer.
 * Verify the tag, and that the length is 32 bytes. Helper function.
 */
static int bootutil_read_bigint(uint8_t i[NUM_ECC_BYTES], uint8_t **cp, uint8_t *end)
{
    size_t len;

    if (mbedtls_asn1_get_tag(cp, end, &len, MBEDTLS_ASN1_INTEGER)) {
        return -3;
    }

    if (len >= NUM_ECC_BYTES) {
        memcpy(i, *cp + len - NUM_ECC_BYTES, NUM_ECC_BYTES);
    } else {
        memset(i, 0, NUM_ECC_BYTES - len);
        memcpy(i + NUM_ECC_BYTES - len, *cp, len);
    }
    *cp += len;
    return 0;
}

/*
 * Read in signature. Signature has r and s encoded as integers. Helper function.
 */
static int bootutil_decode_sig(uint8_t signature[NUM_ECC_BYTES * 2], uint8_t *cp, uint8_t *end)
{
    int rc;
    size_t len;

    rc = mbedtls_asn1_get_tag(&cp, end, &len,
                              MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE);
    if (rc) {
        return -1;
    }
    if (cp + len > end) {
        return -2;
    }

    rc = bootutil_read_bigint(signature, &cp, end);
    if (rc) {
        return -3;
    }
    rc = bootutil_read_bigint(signature + NUM_ECC_BYTES, &cp, end);
    if (rc) {
        return -4;
    }
    return 0;
}
#endif /* !MCUBOOT_USE_PSA_CRYPTO */

#if defined(MCUBOOT_USE_TINYCRYPT)
typedef uintptr_t bootutil_ecdsa_context;
static inline void bootutil_ecdsa_init(bootutil_ecdsa_context *ctx)
{
    (void)ctx;
}

static inline void bootutil_ecdsa_drop(bootutil_ecdsa_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_ecdsa_verify(bootutil_ecdsa_context *ctx,
                                        uint8_t *pk, size_t pk_len,
                                        uint8_t *hash, size_t hash_len,
                                        uint8_t *sig, size_t sig_len)
{
    int rc;
    (void)ctx;
    (void)pk_len;
    (void)sig_len;
    (void)hash_len;

    uint8_t signature[2 * NUM_ECC_BYTES];
    rc = bootutil_decode_sig(signature, sig, sig + sig_len);
    if (rc) {
        return -1;
    }

    /* Only support uncompressed keys. */
    if (pk[0] != 0x04) {
        return -1;
    }
    pk++;

    rc = uECC_verify(pk, hash, BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE, signature, uECC_secp256r1());
    if (rc != TC_CRYPTO_SUCCESS) {
        return -1;
    }
    return 0;
}

static inline int bootutil_ecdsa_parse_public_key(bootutil_ecdsa_context *ctx,
                                                  uint8_t **cp,uint8_t *end)
{
    (void)ctx;
    return bootutil_import_key(cp, end);
}
#endif /* MCUBOOT_USE_TINYCRYPT */

#if defined(MCUBOOT_USE_CC310)
typedef uintptr_t bootutil_ecdsa_context;
static inline void bootutil_ecdsa_init(bootutil_ecdsa_context *ctx)
{
    (void)ctx;
}

static inline void bootutil_ecdsa_drop(bootutil_ecdsa_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_ecdsa_verify(bootutil_ecdsa_context *ctx,
                                        uint8_t *pk, size_t pk_len,
                                        uint8_t *hash, size_t hash_len,
                                        uint8_t *sig, size_t sig_len)
{
    (void)ctx;
    (void)pk_len;
    (void)hash_len;
    uint8_t dsig[2 * NUM_ECC_BYTES];

    if (bootutil_decode_sig(dsig, sig, sig + sig_len)) {
        return -1;
    }

    /* Only support uncompressed keys. */
    if (pk[0] != 0x04) {
        return -1;
    }
    pk++;

    return cc310_ecdsa_verify_secp256r1(hash, pk, dsig, BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE);
}

static inline int bootutil_ecdsa_parse_public_key(bootutil_ecdsa_context *ctx,
                                                  uint8_t **cp,uint8_t *end)
{
    (void)ctx;
    return bootutil_import_key(cp, end);
}
#endif /* MCUBOOT_USE_CC310 */

#if defined(MCUBOOT_USE_PSA_CRYPTO)
typedef struct {
    psa_key_id_t key_id;
    size_t curve_byte_count;
    psa_algorithm_t required_algorithm;
} bootutil_ecdsa_context;

/* ECDSA public key with format specified in RFC5280 et al. in ASN.1 syntax
 *
 * SEQUENCE {
 *    SEQUENCE {
 *        OBJECT idEcPublicKey
 *        OBJECT namedCurve
 *    }
 *    BIT STRING publicKey
 * }
 *
 * ECDSA signature format specified in RFC3279 et al. in ASN.1 syntax
 *
 * SEQUENCE  {
 *    INTEGER r
 *    INTEGER s
 * }
 *
 */

/* Offset in bytes from the start of the encoding to the length field
 * of the innermost SEQUENCE in the ECDSA public key
 */
#define PUB_KEY_LEN_OFF (3)

/* Offset in bytes from the start of the publicKey encoding of the BIT STRING */
#define PUB_KEY_VAL_OFF (3)

/* Computes the pointer to the idEcPublicKey OID from the base of the encoding */
#define PUB_KEY_OID_OFFSET(p) (*p + PUB_KEY_LEN_OFF+1)

/* Computes the pointer to the namedCurve OID from the base of the encoding */
#define CURVE_TYPE_OID_OFFSET(p) PUB_KEY_OID_OFFSET(p) + sizeof(IdEcPublicKey)

/* This helper function gets a pointer to the bitstring associated to the publicKey
 * as encoded per RFC 5280. This function assumes that the public key encoding is not
 * bigger than 127 bytes (i.e. usually up until 384 bit curves)
 *
 * \param[in,out] p    Double pointer to a buffer containing the RFC 5280 of the ECDSA public key.
 *                     On output, the pointer is updated to point to the start of the public key
 *                     in BIT STRING form.
 * \param[out]    size Pointer to a buffer containing the size of the public key extracted
 *
 */
static inline void get_public_key_from_rfc5280_encoding(uint8_t **p, size_t *size)
{
    uint8_t *key_start = (*p) + (PUB_KEY_LEN_OFF + 1 + (*p)[PUB_KEY_LEN_OFF] + PUB_KEY_VAL_OFF);
    *p = key_start;
    *size = key_start[-2]-1; /* -2 from PUB_KEY_VAL_OFF to get the length, -1 to remove the ASN.1 padding byte count */
}

/* This helper function parses a signature as specified in RFC3279 into a pair
 * (r,s) of contiguous bytes
 *
 * \param[in]  sig               Pointer to a buffer containing the encoded signature
 * \param[in] num_of_curve_bytes The required number of bytes for r and s
 * \param[out] r_s_pair          Buffer containing the (r,s) pair extracted. It's caller
 *                               responsibility to ensure the buffer is big enough to
 *                               hold the parsed (r,s) pair.
 */
static void parse_signature_from_rfc5480_encoding(const uint8_t *sig,
                                                  size_t num_of_curve_bytes,
                                                  uint8_t *r_s_pair)
{
    const uint8_t *sig_ptr = NULL;

    /* r or s can be greater than the expected size by one, due to the way
     * ASN.1 encodes signed integers. If either r or s starts with a bit 1,
     * a zero byte will be added in front of the encoding
     */

    /* sig[0] == 0x30, sig[1] == <length>, sig[2] == 0x02 */

    /* Move r in place */
    size_t r_len = sig[3];
    sig_ptr = &sig[4];
    if (r_len >= num_of_curve_bytes) {
        sig_ptr = sig_ptr + r_len - num_of_curve_bytes;
        memcpy(&r_s_pair[0], sig_ptr, num_of_curve_bytes);
        if(r_len % 2) {
            r_len--;
        }
    } else {
        /* For encodings that reduce the size of r or s in case of zeros */
        memcpy(&r_s_pair[num_of_curve_bytes - r_len], sig_ptr, r_len);
    }

    /* Move s in place */
    size_t s_len = sig_ptr[r_len+1]; /*  + 1 to skip SEQUENCE */
    sig_ptr  = &sig_ptr[r_len+2];
    if (s_len >= num_of_curve_bytes) {
        sig_ptr = sig_ptr + s_len - num_of_curve_bytes;
        memcpy(&r_s_pair[num_of_curve_bytes], sig_ptr, num_of_curve_bytes);
    } else {
        /* For encodings that reduce the size of r or s in case of zeros */
        memcpy(&r_s_pair[2*num_of_curve_bytes - s_len], sig_ptr, s_len);
    }
}

// OID id-ecPublicKey 1.2.840.10045.2.1.
static const uint8_t IdEcPublicKey[] = {0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01};
#if defined(MCUBOOT_SIGN_EC256)
// OID secp256r1 1.2.840.10045.3.1.7.
static const uint8_t Secp256r1[] = {0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07};
#endif /* MCUBOOT_SIGN_EC256 */
#if defined(MCUBOOT_SIGN_EC384)
// OID secp384r1 1.3.132.0.34
static const uint8_t Secp384r1[] = {0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22};
#endif /* MCUBOOT_SIGN_EC384 */

static inline void bootutil_ecdsa_init(bootutil_ecdsa_context *ctx)
{
#if !defined(MCUBOOT_BUILTIN_KEY)
    ctx->key_id = PSA_KEY_ID_NULL;
    ctx->curve_byte_count = 0;
    ctx->required_algorithm = 0;

#else /* !MCUBOOT_BUILTIN_KEY */
    /* The incoming key ID is equal to the image index. The key ID value must be
     * shifted (by one in this case) because zero is reserved (PSA_KEY_ID_NULL)
     * and considered invalid.
     */
    ctx->key_id++; /* Make sure it is not equal to 0. */
#if defined(MCUBOOT_SIGN_EC256)
    ctx->curve_byte_count = 32;
    ctx->required_algorithm = PSA_ALG_SHA_256;
#endif /* MCUBOOT_SIGN_EC256 */
#if defined(MCUBOOT_SIGN_EC384)
    ctx->curve_byte_count = 48;
    ctx->required_algorithm = PSA_ALG_SHA_384;
#endif /* MCUBOOT_SIGN_EC384 */
#endif /* !MCUBOOT_BUILTIN_KEY */
}

static inline void bootutil_ecdsa_drop(bootutil_ecdsa_context *ctx)
{
    if (ctx->key_id != PSA_KEY_ID_NULL) {
        (void)psa_destroy_key(ctx->key_id);
    }
}

#if !defined(MCUBOOT_BUILTIN_KEY)
/*
 * Parse a ECDSA public key with format specified in RFC5280 et al.
 *
 * OID for icEcPublicKey is 1.2.840.10045.2.1
 * OIDs for supported curves are as follows:
 *     secp256r1 (prime256v1): 1.2.840.10045.3.1.7
 *     secp384r1: 1.3.132.0.34
 */
static int bootutil_ecdsa_parse_public_key(bootutil_ecdsa_context *ctx,
                                           uint8_t **cp, uint8_t *end)
{
    psa_key_attributes_t key_attributes = psa_key_attributes_init();
    size_t key_size;
    (void)end;

    /* public key oid is valid */
    if (memcmp(PUB_KEY_OID_OFFSET(cp), IdEcPublicKey, sizeof(IdEcPublicKey))) {
        return (int)PSA_ERROR_INVALID_ARGUMENT;
    }

#if defined(MCUBOOT_SIGN_EC256)
    if (!memcmp(CURVE_TYPE_OID_OFFSET(cp), Secp256r1, sizeof(Secp256r1))) {
        ctx->curve_byte_count = 32;
        ctx->required_algorithm = PSA_ALG_SHA_256;
    } else
#endif /* MCUBOOT_SIGN_EC256 */
#if defined(MCUBOOT_SIGN_EC384)
    if (!memcmp(CURVE_TYPE_OID_OFFSET(cp), Secp384r1, sizeof(Secp384r1))) {
        ctx->curve_byte_count = 48;
        ctx->required_algorithm = PSA_ALG_SHA_384;
    } else
#endif /* MCUBOOT_SIGN_EC384 */
    {
        return (int)PSA_ERROR_INVALID_ARGUMENT;
    }

    get_public_key_from_rfc5280_encoding(cp, &key_size);
    /* Set attributes and import key */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(ctx->required_algorithm));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));

    return (int)psa_import_key(&key_attributes, *cp, key_size, &ctx->key_id);
}
#endif /* !MCUBOOT_BUILTIN_KEY */

#if !defined(CONFIG_NCS_BOOT_SIGNATURE_USING_ITS)
/* Verify the signature against the provided hash. The signature gets parsed from
 * the encoding first, then PSA Crypto has a dedicated API for ECDSA verification
 */
static inline int bootutil_ecdsa_verify(bootutil_ecdsa_context *ctx,
                                        uint8_t *pk, size_t pk_len,
                                        uint8_t *hash, size_t hlen,
                                        uint8_t *sig, size_t slen)
{
    (void)pk;
    (void)pk_len;
    (void)slen;

    uint8_t reformatted_signature[96] = {0}; /* Enough for P-384 signature sizes */
    parse_signature_from_rfc5480_encoding(sig, ctx->curve_byte_count,reformatted_signature);

    return (int) psa_verify_hash(ctx->key_id, PSA_ALG_ECDSA(ctx->required_algorithm),
                                 hash, hlen, reformatted_signature, 2*ctx->curve_byte_count);
}
#else /* !CONFIG_NCS_BOOT_SIGNATURE_USING_ITS */

static const psa_key_id_t builtin_key_ids[] =  {
    0x40022100,
    0x40022101,
    0x40022102,
    0x40022103
};

#define BOOT_SIGNATURE_BUILTIN_KEY_SLOTS ARRAY_SIZE(builtin_key_ids)

static inline int bootutil_ecdsa_verify(bootutil_ecdsa_context *ctx,
                                        uint8_t *pk, size_t pk_len,
                                        uint8_t *hash, size_t hlen,
                                        uint8_t *sig, size_t slen)
{
    (void)pk;
    (void)pk_len;
    (void)slen;
    psa_status_t status = PSA_ERROR_BAD_STATE;

    /* Initialize PSA Crypto */
    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        BOOT_LOG_ERR("PSA crypto init failed %d", status);
        return 1;
    }

    uint8_t reformatted_signature[96] = {0}; /* Enough for P-384 signature sizes */
    parse_signature_from_rfc5480_encoding(sig, ctx->curve_byte_count, reformatted_signature);

    status = PSA_ERROR_BAD_STATE;

    for (int i = 0; i < BOOT_SIGNATURE_BUILTIN_KEY_SLOTS; ++i) {
        psa_key_id_t kid = builtin_key_ids[i];

        status = psa_verify_hash(kid, PSA_ALG_ECDSA(ctx->required_algorithm),
                                 hash, hlen, reformatted_signature, 2*ctx->curve_byte_count);
        if (status == PSA_SUCCESS) {
            break;
        }
        BOOT_LOG_ERR("ECDSA signature verification failed %d", status);
    }

    return status == PSA_SUCCESS ? 0 : 2;
}

#endif /* !CONFIG_NCS_BOOT_SIGNATURE_USING_ITS */

#elif defined(MCUBOOT_USE_MBED_TLS)

typedef mbedtls_ecdsa_context bootutil_ecdsa_context;
static inline void bootutil_ecdsa_init(bootutil_ecdsa_context *ctx)
{
    mbedtls_ecdsa_init(ctx);
}

static inline void bootutil_ecdsa_drop(bootutil_ecdsa_context *ctx)
{
    mbedtls_ecdsa_free(ctx);
}

#ifdef CY_MBEDTLS_HW_ACCELERATION
/*
 * Parse the public key used for signing.
 */
static int bootutil_parse_eckey(bootutil_ecdsa_context *ctx, uint8_t **p, uint8_t *end)
{
    size_t len;
    mbedtls_asn1_buf alg;
    mbedtls_asn1_buf param;

    if (mbedtls_asn1_get_tag(p, end, &len,
        MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) {
        return -1;
    }
    end = *p + len;

    if (mbedtls_asn1_get_alg(p, end, &alg, &param)) {
        return -2;
    }
    if (alg.ASN1_CONTEXT_MEMBER(len) != sizeof(ec_pubkey_oid) - 1 ||
      memcmp(alg.ASN1_CONTEXT_MEMBER(p), ec_pubkey_oid, sizeof(ec_pubkey_oid) - 1)) {
        return -3;
    }
    if (param.ASN1_CONTEXT_MEMBER(len) != sizeof(ec_secp256r1_oid) - 1||
      memcmp(param.ASN1_CONTEXT_MEMBER(p), ec_secp256r1_oid, sizeof(ec_secp256r1_oid) - 1)) {
        return -4;
    }

    if (mbedtls_ecp_group_load(&ctx->grp, MBEDTLS_ECP_DP_SECP256R1)) {
        return -5;
    }

    if (mbedtls_asn1_get_bitstring_null(p, end, &len)) {
        return -6;
    }
    if (*p + len != end) {
        return -7;
    }

    if (mbedtls_ecp_point_read_binary(&ctx->grp, &ctx->Q, *p, end - *p)) {
        return -8;
    }

    if (mbedtls_ecp_check_pubkey(&ctx->grp, &ctx->Q)) {
        return -9;
    }
    return 0;
}

static inline int bootutil_ecdsa_verify(bootutil_ecdsa_context *ctx,
                                        uint8_t *pk, size_t pk_len,
                                        uint8_t *hash, size_t hash_len,
                                        uint8_t *sig, size_t sig_len)
{
    (void)pk;
    (void)pk_len;

    /*
     * This is simplified, as the hash length is also 32 bytes.
     */
    while (sig[sig_len - 1] == '\0') {
        sig_len--;
    }
    return mbedtls_ecdsa_read_signature(&ctx, hash, hash_len, sig, sig_len);
}

#else /* CY_MBEDTLS_HW_ACCELERATION */

static inline int bootutil_ecdsa_verify(bootutil_ecdsa_context *ctx,
                                        uint8_t *pk, size_t pk_len,
                                        uint8_t *hash, size_t hash_len,
                                        uint8_t *sig, size_t sig_len)
{
    int rc;

    (void)sig;
    (void)hash;
    (void)hash_len;

    rc = mbedtls_ecp_group_load(&ctx->MBEDTLS_CONTEXT_MEMBER(grp), MBEDTLS_ECP_DP_SECP256R1);
    if (rc) {
        return -1;
    }

    rc = mbedtls_ecp_point_read_binary(&ctx->MBEDTLS_CONTEXT_MEMBER(grp), &ctx->MBEDTLS_CONTEXT_MEMBER(Q), pk, pk_len);
    if (rc) {
        return -1;
    }

    rc = mbedtls_ecp_check_pubkey(&ctx->MBEDTLS_CONTEXT_MEMBER(grp), &ctx->MBEDTLS_CONTEXT_MEMBER(Q));
    if (rc) {
        return -1;
    }

    rc = mbedtls_ecdsa_read_signature(ctx, hash, BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE,
                                      sig, sig_len);
    if (rc) {
        return -1;
    }

    return 0;
}

#endif /* CY_MBEDTLS_HW_ACCELERATION */

static inline int bootutil_ecdsa_parse_public_key(bootutil_ecdsa_context *ctx,
                                                  uint8_t **cp,uint8_t *end)
{
    int rc;
#ifdef CY_MBEDTLS_HW_ACCELERATION
    rc = bootutil_parse_eckey(&ctx, cp, end);
#else
    (void)ctx;
    rc = bootutil_import_key(cp, end);
#endif
    return rc;
}

#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_NRF_EXTERNAL_CRYPTO)
typedef uintptr_t bootutil_ecdsa_context;
static inline void bootutil_ecdsa_init(bootutil_ecdsa_context *ctx)
{
    (void)ctx;
}

static inline void bootutil_ecdsa_drop(bootutil_ecdsa_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_ecdsa_verify(bootutil_ecdsa_context *ctx,
                                        uint8_t *pk, size_t pk_len,
                                        uint8_t *hash, size_t hash_len,
                                        uint8_t *sig, size_t sig_len)
{
    (void)ctx;
    (void)pk_len;
    (void)hash_len;
    uint8_t dsig[2 * NUM_ECC_BYTES];

    if (bootutil_decode_sig(dsig, sig, sig + sig_len)) {
        return -1;
    }

    /* Only support uncompressed keys. */
    if (pk[0] != 0x04) {
        return -1;
    }
    pk++;

    return bl_secp256r1_validate(hash, BOOTUTIL_CRYPTO_ECDSA_P256_HASH_SIZE, pk, dsig);
}

static inline int bootutil_ecdsa_parse_public_key(bootutil_ecdsa_context *ctx,
                                                  uint8_t **cp,uint8_t *end)
{
    (void)ctx;
    return bootutil_import_key(cp, end);
}
#endif /* MCUBOOT_USE_NRF_EXTERNAL_CRYPTO */

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_CRYPTO_ECDSA_H_ */
