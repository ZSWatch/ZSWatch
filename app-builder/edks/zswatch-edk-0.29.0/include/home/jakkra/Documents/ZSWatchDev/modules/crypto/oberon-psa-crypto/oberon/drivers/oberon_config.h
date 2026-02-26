/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef OBERON_CONFIG_H
#define OBERON_CONFIG_H

#include "psa/crypto_driver_config.h"
#include "oberon_check_unsupported.h"

/* Oberon AEAD Driver */

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CCM)
    #if defined(PSA_WANT_AES_KEY_SIZE_128) && !defined(PSA_ACCEL_CCM_AES_128)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CCM_AES_128"
        #endif
        #define PSA_NEED_OBERON_AEAD_DRIVER 1
        #define PSA_NEED_OBERON_CCM_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_192) && !defined(PSA_ACCEL_CCM_AES_192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CCM_AES_192"
        #endif
        #define PSA_NEED_OBERON_AEAD_DRIVER 1
        #define PSA_NEED_OBERON_CCM_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_256) && !defined(PSA_ACCEL_CCM_AES_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CCM_AES_256"
        #endif
        #define PSA_NEED_OBERON_AEAD_DRIVER 1
        #define PSA_NEED_OBERON_CCM_AES 1
    #endif
#endif

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_GCM)
    #if defined(PSA_WANT_AES_KEY_SIZE_128) && !defined(PSA_ACCEL_GCM_AES_128)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for GCM_AES_128"
        #endif
        #define PSA_NEED_OBERON_AEAD_DRIVER 1
        #define PSA_NEED_OBERON_GCM_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_192) && !defined(PSA_ACCEL_GCM_AES_192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for GCM_AES_192"
        #endif
        #define PSA_NEED_OBERON_AEAD_DRIVER 1
        #define PSA_NEED_OBERON_GCM_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_256) && !defined(PSA_ACCEL_GCM_AES_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for GCM_AES_256"
        #endif
        #define PSA_NEED_OBERON_AEAD_DRIVER 1
        #define PSA_NEED_OBERON_GCM_AES 1
    #endif
#endif

#if defined(PSA_WANT_ALG_CHACHA20_POLY1305) && !defined(PSA_ACCEL_CHACHA20_POLY1305)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CHACHA20_POLY1305"
    #endif
    #define PSA_NEED_OBERON_AEAD_DRIVER 1
    #define PSA_NEED_OBERON_CHACHA20_POLY1305 1
#endif

#if defined(PSA_WANT_ALG_XCHACHA20_POLY1305) && !defined(PSA_ACCEL_XCHACHA20_POLY1305)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CHACHA20_POLY1305"
    #endif
    #define PSA_NEED_OBERON_AEAD_DRIVER 1
    #define PSA_NEED_OBERON_XCHACHA20_POLY1305 1
#endif

#if defined(PSA_WANT_ALG_ASCON_AEAD128) && !defined(PSA_ACCEL_ASCON_AEAD128)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ASCON_AEAD128"
    #endif
    #define PSA_NEED_OBERON_AEAD_DRIVER 1
    #define PSA_NEED_OBERON_ASCON_AEAD128 1
#endif

/* Oberon Cipher Driver */

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CTR)
    #if defined(PSA_WANT_AES_KEY_SIZE_128) && !defined(PSA_ACCEL_CTR_AES_128)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CTR_AES_128"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CTR_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_192) && !defined(PSA_ACCEL_CTR_AES_192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CTR_AES_192"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CTR_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_256) && !defined(PSA_ACCEL_CTR_AES_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CTR_AES_256"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CTR_AES 1
    #endif
#endif

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CBC_PKCS7)
    #if defined(PSA_WANT_AES_KEY_SIZE_128) && !defined(PSA_ACCEL_CBC_PKCS7_AES_128)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CBC_PKCS7_AES_128"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CBC_PKCS7_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_192) && !defined(PSA_ACCEL_CBC_PKCS7_AES_192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CBC_PKCS7_AES_192"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CBC_PKCS7_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_256) && !defined(PSA_ACCEL_CBC_PKCS7_AES_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CBC_PKCS7_AES_256"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CBC_PKCS7_AES 1
    #endif
#endif

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CBC_NO_PADDING)
    #if defined(PSA_WANT_AES_KEY_SIZE_128) && !defined(PSA_ACCEL_CBC_NO_PADDING_AES_128)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CBC_NO_PADDING_AES_128"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CBC_NO_PADDING_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_192) && !defined(PSA_ACCEL_CBC_NO_PADDING_AES_192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CBC_NO_PADDING_AES_192"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CBC_NO_PADDING_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_256) && !defined(PSA_ACCEL_CBC_NO_PADDING_AES_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CBC_NO_PADDING_AES_256"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CBC_NO_PADDING_AES 1
    #endif
#endif

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_ECB_NO_PADDING)
    #if defined(PSA_WANT_AES_KEY_SIZE_128) && !defined(PSA_ACCEL_ECB_NO_PADDING_AES_128)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECB_NO_PADDING_AES_128"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_ECB_NO_PADDING_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_192) && !defined(PSA_ACCEL_ECB_NO_PADDING_AES_192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECB_NO_PADDING_AES_192"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_ECB_NO_PADDING_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_256) && !defined(PSA_ACCEL_ECB_NO_PADDING_AES_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECB_NO_PADDING_AES_256"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_ECB_NO_PADDING_AES 1
    #endif
#endif

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_CCM_STAR_NO_TAG)
    #if defined(PSA_WANT_AES_KEY_SIZE_128) && !defined(PSA_ACCEL_CCM_STAR_NO_TAG_AES_128)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CCM_STAR_NO_TAG_AES_128"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CCM_STAR_NO_TAG_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_192) && !defined(PSA_ACCEL_CCM_STAR_NO_TAG_AES_192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CCM_STAR_NO_TAG_AES_192"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CCM_STAR_NO_TAG_AES 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_256) && !defined(PSA_ACCEL_CCM_STAR_NO_TAG_AES_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CCM_STAR_NO_TAG_AES_256"
        #endif
        #define PSA_NEED_OBERON_CIPHER_DRIVER 1
        #define PSA_NEED_OBERON_CCM_STAR_NO_TAG_AES 1
    #endif
#endif

#if defined(PSA_WANT_KEY_TYPE_CHACHA20) && defined(PSA_WANT_ALG_STREAM_CIPHER) &&                                      \
    !defined(PSA_ACCEL_STREAM_CIPHER_CHACHA20)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for STREAM_CIPHER_CHACHA20"
    #endif
    #define PSA_NEED_OBERON_CIPHER_DRIVER 1
    #define PSA_NEED_OBERON_STREAM_CIPHER_CHACHA20 1
#endif

#if defined(PSA_WANT_KEY_TYPE_XCHACHA20) && defined(PSA_WANT_ALG_STREAM_CIPHER) &&                                      \
    !defined(PSA_ACCEL_STREAM_CIPHER_XCHACHA20)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for STREAM_CIPHER_CHACHA20"
    #endif
    #define PSA_NEED_OBERON_CIPHER_DRIVER 1
    #define PSA_NEED_OBERON_STREAM_CIPHER_XCHACHA20 1
#endif

/* Oberon Key Agreement Driver */

#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_ECC_SECP_R1_224) && !defined(PSA_ACCEL_ECDH_SECP_R1_224)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDH_SECP_R1_224"
    #endif
    #define PSA_NEED_OBERON_KEY_AGREEMENT_DRIVER 1
    #define PSA_NEED_OBERON_ECDH 1
    #define PSA_NEED_OBERON_ECDH_SECP_R1_224 1
#endif

#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_ECC_SECP_R1_256) && !defined(PSA_ACCEL_ECDH_SECP_R1_256)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDH_SECP_R1_256"
    #endif
    #define PSA_NEED_OBERON_KEY_AGREEMENT_DRIVER 1
    #define PSA_NEED_OBERON_ECDH 1
    #define PSA_NEED_OBERON_ECDH_SECP_R1_256 1
#endif

#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_ECC_SECP_R1_384) && !defined(PSA_ACCEL_ECDH_SECP_R1_384)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDH_SECP_R1_384"
    #endif
    #define PSA_NEED_OBERON_KEY_AGREEMENT_DRIVER 1
    #define PSA_NEED_OBERON_ECDH 1
    #define PSA_NEED_OBERON_ECDH_SECP_R1_384 1
#endif

#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_ECC_SECP_R1_521) && !defined(PSA_ACCEL_ECDH_SECP_R1_521)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDH_SECP_R1_521"
    #endif
    #define PSA_NEED_OBERON_KEY_AGREEMENT_DRIVER 1
    #define PSA_NEED_OBERON_ECDH 1
    #define PSA_NEED_OBERON_ECDH_SECP_R1_521 1
#endif

#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_ECC_SECP_K1_256) && !defined(PSA_ACCEL_ECDH_SECP_K1_256)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDH_SECP_K1_256"
    #endif
    #define PSA_NEED_OBERON_KEY_AGREEMENT_DRIVER 1
    #define PSA_NEED_OBERON_ECDH 1
    #define PSA_NEED_OBERON_ECDH_SECP_K1_256 1
#endif

#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_ECC_MONTGOMERY_255) && !defined(PSA_ACCEL_ECDH_MONTGOMERY_255)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDH_MONTGOMERY_255"
    #endif
    #define PSA_NEED_OBERON_KEY_AGREEMENT_DRIVER 1
    #define PSA_NEED_OBERON_ECDH 1
    #define PSA_NEED_OBERON_ECDH_MONTGOMERY_255 1
#endif

#if defined(PSA_WANT_ALG_ECDH) && defined(PSA_WANT_ECC_MONTGOMERY_448) && !defined(PSA_ACCEL_ECDH_MONTGOMERY_448)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDH_MONTGOMERY_448"
    #endif
    #define PSA_NEED_OBERON_KEY_AGREEMENT_DRIVER 1
    #define PSA_NEED_OBERON_ECDH 1
    #define PSA_NEED_OBERON_ECDH_MONTGOMERY_448 1
#endif

/* Oberon Asymmetric Signature Driver */

#if defined(PSA_WANT_ALG_ECDSA) || defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
    #define PSA_WANT_ALG_ANY_ECDSA
#endif

#if defined(PSA_WANT_ALG_ANY_ECDSA) && defined(PSA_WANT_ECC_SECP_R1_224)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA_512)) || \
        (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA3_224)) || \
        (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA3_256)) || \
        (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA3_384)) || \
        (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_ECDSA_SECP_R1_224_SHA3_512))
        #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
        #define PSA_NEED_OBERON_ECDSA_VERIFY 1
        #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
            #define PSA_NEED_OBERON_ECDSA_SIGN 1
        #endif
        #define PSA_NEED_OBERON_ECDSA_SECP_R1_224 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_ECDSA_VERIFY)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDSA_SECP_R1_224"
    #endif
#endif

#if defined(PSA_WANT_ALG_ANY_ECDSA) && defined(PSA_WANT_ECC_SECP_R1_256)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA_512)) || \
        (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA3_224)) || \
        (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA3_256)) || \
        (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA3_384)) || \
        (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_ECDSA_SECP_R1_256_SHA3_512))
        #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
        #define PSA_NEED_OBERON_ECDSA_VERIFY 1
        #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
            #define PSA_NEED_OBERON_ECDSA_SIGN 1
        #endif
        #define PSA_NEED_OBERON_ECDSA_SECP_R1_256 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_ECDSA_VERIFY)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDSA_SECP_R1_256"
    #endif
#endif

#if defined(PSA_WANT_ALG_ANY_ECDSA) && defined(PSA_WANT_ECC_SECP_R1_384)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA_512)) || \
        (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA3_224)) || \
        (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA3_256)) || \
        (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA3_384)) || \
        (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_ECDSA_SECP_R1_384_SHA3_512))
        #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
        #define PSA_NEED_OBERON_ECDSA_VERIFY 1
        #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
            #define PSA_NEED_OBERON_ECDSA_SIGN 1
        #endif
        #define PSA_NEED_OBERON_ECDSA_SECP_R1_384 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_ECDSA_VERIFY)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDSA_SECP_R1_384"
    #endif
#endif

#if defined(PSA_WANT_ALG_ANY_ECDSA) && defined(PSA_WANT_ECC_SECP_R1_521)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA_512)) || \
        (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA3_224)) || \
        (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA3_256)) || \
        (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA3_384)) || \
        (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_ECDSA_SECP_R1_521_SHA3_512))
        #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
        #define PSA_NEED_OBERON_ECDSA_VERIFY 1
        #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
            #define PSA_NEED_OBERON_ECDSA_SIGN 1
        #endif
        #define PSA_NEED_OBERON_ECDSA_SECP_R1_521 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_ECDSA_VERIFY)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDSA_SECP_R1_521"
    #endif
#endif

#if defined(PSA_WANT_ALG_ANY_ECDSA) && defined(PSA_WANT_ECC_SECP_K1_256)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA_512)) || \
        (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA3_224)) || \
        (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA3_256)) || \
        (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA3_384)) || \
        (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_ECDSA_SECP_K1_256_SHA3_512))
        #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
        #define PSA_NEED_OBERON_ECDSA_VERIFY 1
        #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
            #define PSA_NEED_OBERON_ECDSA_SIGN 1
        #endif
        #define PSA_NEED_OBERON_ECDSA_SECP_K1_256 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_ECDSA_VERIFY)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECDSA_SECP_K1_256"
    #endif
#endif

#if defined(PSA_NEED_OBERON_ECDSA_VERIFY) && defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
#define PSA_NEED_OBERON_ECDSA_DETERMINISTIC 1
#endif

#if defined(PSA_NEED_OBERON_ECDSA_VERIFY) && defined(PSA_WANT_ALG_ECDSA)
#define PSA_NEED_OBERON_ECDSA_RANDOMIZED 1
#endif

#if defined(PSA_WANT_ALG_PURE_EDDSA)
    #if defined(PSA_WANT_ECC_TWISTED_EDWARDS_255) && !defined(PSA_ACCEL_PURE_EDDSA_TWISTED_EDWARDS_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for PURE_EDDSA_TWISTED_EDWARDS_255"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
        #define PSA_NEED_OBERON_EDDSA_VERIFY 1
        #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
            #define PSA_NEED_OBERON_EDDSA_SIGN 1
        #endif
        #define PSA_NEED_OBERON_PURE_EDDSA_TWISTED_EDWARDS_255 1
    #endif
    #if defined(PSA_WANT_ECC_TWISTED_EDWARDS_448) && !defined(PSA_ACCEL_PURE_EDDSA_TWISTED_EDWARDS_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for PURE_EDDSA_TWISTED_EDWARDS_448"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
        #define PSA_NEED_OBERON_EDDSA_VERIFY 1
        #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
            #define PSA_NEED_OBERON_EDDSA_SIGN 1
        #endif
        #define PSA_NEED_OBERON_PURE_EDDSA_TWISTED_EDWARDS_448 1
    #endif
#endif

#if defined(PSA_WANT_ALG_ED25519PH) && !defined(PSA_ACCEL_ED25519PH)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ED25519PH"
    #endif
    #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
    #define PSA_NEED_OBERON_EDDSA_VERIFY 1
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
        #define PSA_NEED_OBERON_EDDSA_SIGN 1
    #endif
    #define PSA_NEED_OBERON_ED25519PH 1
#endif

#if defined(PSA_WANT_ALG_ED448PH) && !defined(PSA_ACCEL_ED448PH)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ED448PH"
    #endif
    #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
    #define PSA_NEED_OBERON_EDDSA_VERIFY 1
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
        #define PSA_NEED_OBERON_EDDSA_SIGN 1
    #endif
    #define PSA_NEED_OBERON_ED448PH 1
#endif

#if defined(PSA_WANT_ALG_LMS) && !defined(PSA_ACCEL_LMS)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for LMS"
    #endif
    #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
    #define PSA_NEED_OBERON_LMS_VERIFY 1
#endif

#if defined(PSA_WANT_ALG_HSS) && !defined(PSA_ACCEL_HSS)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for HSS"
    #endif
    #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
    #define PSA_NEED_OBERON_HSS_VERIFY 1
#endif

#if defined(PSA_WANT_ALG_XMSS) && !defined(PSA_ACCEL_XMSS)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for XMSS"
    #endif
    #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
    #define PSA_NEED_OBERON_XMSS_VERIFY 1
#endif

#if defined(PSA_WANT_ALG_XMSS_MT) && !defined(PSA_ACCEL_XMSS_MT)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for XMSS_MT"
    #endif
    #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
    #define PSA_NEED_OBERON_XMSS_MT_VERIFY 1
#endif

#if (defined(PSA_WANT_ALG_ML_DSA) || \
     defined(PSA_WANT_ALG_DETERMINISTIC_ML_DSA) || \
     defined(PSA_WANT_ALG_HASH_ML_DSA) || \
     defined(PSA_WANT_ALG_DETERMINISTIC_HASH_ML_DSA)) && !defined(PSA_ACCEL_ML_DSA)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ML_DSA"
    #endif
    #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
    #define PSA_NEED_OBERON_ML_DSA_VERIFY 1
    #if defined(PSA_WANT_KEY_TYPE_ML_DSA_KEY_PAIR_BASIC)
        #define PSA_NEED_OBERON_ML_DSA_SIGN 1
    #endif
    #define PSA_NEED_OBERON_ML_DSA 1
    #ifdef PSA_WANT_ML_DSA_KEY_SIZE_44
        #define PSA_NEED_OBERON_ML_DSA_44
    #endif
    #ifdef PSA_WANT_ML_DSA_KEY_SIZE_65
        #define PSA_NEED_OBERON_ML_DSA_65
    #endif
    #ifdef PSA_WANT_ML_DSA_KEY_SIZE_87
        #define PSA_NEED_OBERON_ML_DSA_87
    #endif
    #if defined(PSA_WANT_ALG_ML_DSA) || \
        defined(PSA_WANT_ALG_DETERMINISTIC_ML_DSA)
        #define PSA_NEED_OBERON_MESSAGE_ML_DSA 1
    #endif
    #if defined(PSA_WANT_ALG_HASH_ML_DSA) || \
        defined(PSA_WANT_ALG_DETERMINISTIC_HASH_ML_DSA)
        #define PSA_NEED_OBERON_HASH_ML_DSA 1
    #endif
#endif

#if defined(PSA_WANT_ALG_ML_KEM) && !defined(PSA_ACCEL_ML_KEM)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ML_KEM"
    #endif
    #define PSA_NEED_OBERON_KEY_ENCAPSULATION_DRIVER 1
    #define PSA_NEED_OBERON_ML_KEM 1
    #ifdef PSA_WANT_ML_KEM_KEY_SIZE_512
        #define PSA_NEED_OBERON_ML_KEM_512
    #endif
    #ifdef PSA_WANT_ML_KEM_KEY_SIZE_768
        #define PSA_NEED_OBERON_ML_KEM_768
    #endif
    #ifdef PSA_WANT_ML_KEM_KEY_SIZE_1024
        #define PSA_NEED_OBERON_ML_KEM_1024
    #endif
#endif

#if defined(PSA_WANT_ALG_RSA_PSS)
    #if defined(PSA_WANT_RSA_KEY_SIZE_1024)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PSS_1024_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PSS 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_1024 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PSS_1024"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_1536)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PSS_1536_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PSS 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_1536 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PSS_1536"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_2048)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PSS_2048_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PSS 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_2048 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PSS_2048"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_3072)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PSS_3072_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PSS 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_3072 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PSS_3072"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_4096)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PSS_4096_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PSS 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_4096 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PSS_4096"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_6144)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PSS_6144_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PSS 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_6144 1
            #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
                #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PSS_6144"
            #endif
        #endif
        #if defined(PSA_WANT_RSA_KEY_SIZE_8192)
            #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA_1)) || \
                (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA_224)) || \
                (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA_256)) || \
                (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA_384)) || \
                (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA_512)) || \
                (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA3_224)) || \
                (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA3_256)) || \
                (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA3_384)) || \
                (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PSS_8192_SHA3_512))
                #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
                #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
                #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                    #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
                #endif
                #define PSA_NEED_OBERON_RSA_PSS 1
                #define PSA_NEED_OBERON_RSA_KEY_SIZE_8192 1
            #endif
            #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
                #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PSS_8192"
            #endif
        #endif
    #endif
#endif // defined(PSA_WANT_ALG_RSA_PSS)

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
    #if defined(PSA_WANT_RSA_KEY_SIZE_1024)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1024_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PKCS1V15_SIGN 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_1024 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_SIGN_1024"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_1536)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_1536_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PKCS1V15_SIGN 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_1536 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_SIGN_1536"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_2048)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_2048_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PKCS1V15_SIGN 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_2048 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_SIGN_2048"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_3072)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_3072_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PKCS1V15_SIGN 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_3072 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_SIGN_3072"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_4096)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_4096_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PKCS1V15_SIGN 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_4096 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_SIGN_4096"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_6144)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_6144_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PKCS1V15_SIGN 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_6144 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_SIGN_4096"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_8192)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_PKCS1V15_SIGN_8192_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_SIGNATURE_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_VERIFY 1
            #if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
                #define PSA_NEED_OBERON_RSA_ANY_SIGN 1
            #endif
            #define PSA_NEED_OBERON_RSA_PKCS1V15_SIGN 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_8192 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_VERIFY)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_SIGN_8092"
        #endif
    #endif
#endif // defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)

#if defined(PSA_WANT_RSA_KEY_SIZE_8192)
    #define PSA_MAX_RSA_KEY_BITS 8192
#elif defined(PSA_WANT_RSA_KEY_SIZE_6144)
    #define PSA_MAX_RSA_KEY_BITS 6144
#elif defined(PSA_WANT_RSA_KEY_SIZE_4096)
    #define PSA_MAX_RSA_KEY_BITS 4096
#elif defined(PSA_WANT_RSA_KEY_SIZE_3072)
    #define PSA_MAX_RSA_KEY_BITS 3072
#elif defined(PSA_WANT_RSA_KEY_SIZE_2048)
    #define PSA_MAX_RSA_KEY_BITS 2048
#elif defined(PSA_WANT_RSA_KEY_SIZE_1536)
    #define PSA_MAX_RSA_KEY_BITS 1536
#elif defined(PSA_WANT_RSA_KEY_SIZE_1024)
    #define PSA_MAX_RSA_KEY_BITS 1024
#else
    #define PSA_MAX_RSA_KEY_BITS 0
#endif

#define PSA_MAX_RSA_KEY_SIZE PSA_BITS_TO_BYTES(PSA_MAX_RSA_KEY_BITS)

/* Oberon Asymmetric Encryption Driver */

#if defined(PSA_WANT_ALG_RSA_OAEP)
    #if defined(PSA_WANT_RSA_KEY_SIZE_1024)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_OAEP_1024_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
            #define PSA_NEED_OBERON_RSA_OAEP 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_1024 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_CRYPT)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_OAEP_1024"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_1536)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_OAEP_1536_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
            #define PSA_NEED_OBERON_RSA_OAEP 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_1536 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_CRYPT)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_OAEP_1536"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_2048)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_OAEP_2048_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
            #define PSA_NEED_OBERON_RSA_OAEP 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_2048 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_CRYPT)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_OAEP_2048"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_3072)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_OAEP_3072_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
            #define PSA_NEED_OBERON_RSA_OAEP 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_3072 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_CRYPT)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_OAEP_3072"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_4096)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_OAEP_4096_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
            #define PSA_NEED_OBERON_RSA_OAEP 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_4096 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_CRYPT)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_OAEP_4096"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_6144)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_OAEP_6144_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
            #define PSA_NEED_OBERON_RSA_OAEP 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_6144 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_CRYPT)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_OAEP_6144"
        #endif
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_8192)
        #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA_1)) || \
            (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA_224)) || \
            (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA_256)) || \
            (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA_384)) || \
            (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA_512)) || \
            (defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA3_224)) || \
            (defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA3_256)) || \
            (defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA3_384)) || \
            (defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_RSA_OAEP_8192_SHA3_512))
            #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
            #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
            #define PSA_NEED_OBERON_RSA_OAEP 1
            #define PSA_NEED_OBERON_RSA_KEY_SIZE_8192 1
        #endif
        #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_RSA_ANY_CRYPT)
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_OAEP_8192"
        #endif
    #endif
#endif // defined(PSA_WANT_ALG_RSA_OAEP)

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT)
    #if defined(PSA_WANT_RSA_KEY_SIZE_1024) && !defined(PSA_ACCEL_RSA_PKCS1V15_CRYPT_1024)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_CRYPT_1024"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
        #define PSA_NEED_OBERON_RSA_PKCS1V15_CRYPT 1
        #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
        #define PSA_NEED_OBERON_RSA_KEY_SIZE_1024 1
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_1536) && !defined(PSA_ACCEL_RSA_PKCS1V15_CRYPT_1536)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_CRYPT_1536"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
        #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
        #define PSA_NEED_OBERON_RSA_PKCS1V15_CRYPT 1
        #define PSA_NEED_OBERON_RSA_KEY_SIZE_1536 1
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_2048) && !defined(PSA_ACCEL_RSA_PKCS1V15_CRYPT_2048)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_CRYPT_2048"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
        #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
        #define PSA_NEED_OBERON_RSA_PKCS1V15_CRYPT 1
        #define PSA_NEED_OBERON_RSA_KEY_SIZE_2048 1
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_3072) && !defined(PSA_ACCEL_RSA_PKCS1V15_CRYPT_3072)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_CRYPT_3072"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
        #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
        #define PSA_NEED_OBERON_RSA_PKCS1V15_CRYPT 1
        #define PSA_NEED_OBERON_RSA_KEY_SIZE_3072 1
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_4096) && !defined(PSA_ACCEL_RSA_PKCS1V15_CRYPT_4096)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_CRYPT_4096"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
        #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
        #define PSA_NEED_OBERON_RSA_PKCS1V15_CRYPT 1
        #define PSA_NEED_OBERON_RSA_KEY_SIZE_4096 1
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_6144) && !defined(PSA_ACCEL_RSA_PKCS1V15_CRYPT_6144)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_CRYPT_6144"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
        #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
        #define PSA_NEED_OBERON_RSA_PKCS1V15_CRYPT 1
        #define PSA_NEED_OBERON_RSA_KEY_SIZE_6144 1
    #endif
    #if defined(PSA_WANT_RSA_KEY_SIZE_8192) && !defined(PSA_ACCEL_RSA_PKCS1V15_CRYPT_8192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for RSA_PKCS1V15_CRYPT_8192"
        #endif
        #define PSA_NEED_OBERON_ASYMMETRIC_ENCRYPTION_DRIVER 1
        #define PSA_NEED_OBERON_RSA_ANY_CRYPT 1
        #define PSA_NEED_OBERON_RSA_PKCS1V15_CRYPT 1
        #define PSA_NEED_OBERON_RSA_KEY_SIZE_8192 1
    #endif
#endif // defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT)

/* Oberon Hash Driver */

#if defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_SHA_1)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA_1"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA_1 1
#endif

#if defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_SHA_224)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA_224"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA_224 1
#endif

#if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_SHA_256)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA_256"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA_256 1
#endif

#if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_SHA_384)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA_384"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA_384 1
#endif

#if defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_SHA_512)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA_512"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA_512 1
#endif

#if defined(PSA_WANT_ALG_SHA_256_192) && !defined(PSA_ACCEL_SHA_256_192)
#ifdef PSA_HW_DRIVERS_ONLY
#error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA_256_192"
#endif
#define PSA_NEED_OBERON_HASH_DRIVER 1
#define PSA_NEED_OBERON_SHA_256_192 1
#endif

#if defined(PSA_WANT_ALG_SHA3_224) && !defined(PSA_ACCEL_SHA3_224)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA3_224"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA3 1
    #define PSA_NEED_OBERON_SHA3_224 1
#endif

#if defined(PSA_WANT_ALG_SHA3_256) && !defined(PSA_ACCEL_SHA3_256)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA3_256"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA3 1
    #define PSA_NEED_OBERON_SHA3_256 1
#endif

#if defined(PSA_WANT_ALG_SHA3_384) && !defined(PSA_ACCEL_SHA3_384)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA3_384"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA3 1
    #define PSA_NEED_OBERON_SHA3_384 1
#endif

#if defined(PSA_WANT_ALG_SHA3_512) && !defined(PSA_ACCEL_SHA3_512)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHA3_512"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHA3 1
    #define PSA_NEED_OBERON_SHA3_512 1
#endif

#if defined(PSA_WANT_ALG_SHAKE128_256) && !defined(PSA_ACCEL_SHAKE128_256)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHAKE128_256"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHAKE 1
    #define PSA_NEED_OBERON_SHAKE128_256 1
#endif

#if defined(PSA_WANT_ALG_SHAKE256_192) && !defined(PSA_ACCEL_SHAKE256_192)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHAKE256_192"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHAKE 1
    #define PSA_NEED_OBERON_SHAKE256_192 1
#endif

#if defined(PSA_WANT_ALG_SHAKE256_256) && !defined(PSA_ACCEL_SHAKE256_256)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHAKE256_256"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHAKE 1
    #define PSA_NEED_OBERON_SHAKE256_256 1
#endif

#if defined(PSA_WANT_ALG_SHAKE256_512) && !defined(PSA_ACCEL_SHAKE256_512)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHAKE256_512"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_SHAKE 1
    #define PSA_NEED_OBERON_SHAKE256_512 1
#endif

#if defined(PSA_WANT_ALG_ASCON_HASH256) && !defined(PSA_ACCEL_ASCON_HASH256)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ASCON_HASH256"
    #endif
    #define PSA_NEED_OBERON_HASH_DRIVER 1
    #define PSA_NEED_OBERON_ASCON_HASH256 1
#endif

/* Oberon XOF Driver */

#if defined(PSA_WANT_ALG_SHAKE128) && !defined(PSA_ACCEL_SHAKE128)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHAKE128"
    #endif
    #define PSA_NEED_OBERON_XOF_DRIVER 1
    #define PSA_NEED_OBERON_SHAKE128 1
#endif

#if defined(PSA_WANT_ALG_SHAKE256) && !defined(PSA_ACCEL_SHAKE256)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SHAKE256"
    #endif
    #define PSA_NEED_OBERON_XOF_DRIVER 1
    #define PSA_NEED_OBERON_SHAKE256 1
#endif

#if defined(PSA_WANT_ALG_ASCON_XOF128) && !defined(PSA_ACCEL_ASCON_XOF128)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ASCON_XOF128"
    #endif
    #define PSA_NEED_OBERON_XOF_DRIVER 1
    #define PSA_NEED_OBERON_ASCON_XOF128 1
#endif

#if defined(PSA_WANT_ALG_ASCON_CXOF128) && !defined(PSA_ACCEL_ASCON_CXOF128)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ASCON_CXOF128"
    #endif
    #define PSA_NEED_OBERON_XOF_DRIVER 1
    #define PSA_NEED_OBERON_ASCON_CXOF128 1
#endif

/* Oberon Key Management Driver */

#if defined(PSA_WANT_ECC_SECP_R1_224)
    #if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_224)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_224"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_224 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_224)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_224"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_224 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_224)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_224"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_224 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_224)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_224"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_224 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_224)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_224"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_224 1
    #endif
#endif // defined(PSA_WANT_ECC_SECP_R1_224)

#if defined(PSA_WANT_ECC_SECP_R1_256)
    #if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_256 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_256 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_256 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_256 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_256 1
    #endif
#endif // defined(PSA_WANT_ECC_SECP_R1_256)

#if defined(PSA_WANT_ECC_SECP_R1_384)
    #if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_384)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_384"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_384 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_384)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_384"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_384 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_384)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_384"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_384 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_384)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_384"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_384 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_384)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_384"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_384 1
    #endif
#endif // defined(PSA_WANT_ECC_SECP_R1_384)

#if defined(PSA_WANT_ECC_SECP_R1_521)
    #if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_521)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_521"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_521 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_521)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_521"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_521 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_521)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_521"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_521 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_521)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_521"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_521 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_521)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_521"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_521 1
    #endif
#endif // defined(PSA_WANT_ECC_SECP_R1_521)

#if defined(PSA_WANT_ECC_SECP_K1_256)
#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECP_K1_256)
#ifdef PSA_HW_DRIVERS_ONLY
#error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_SECP_K1_256"
#endif
#define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_SECP_K1_256 1
#endif
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_K1_256)
#ifdef PSA_HW_DRIVERS_ONLY
#error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_K1_256"
#endif
#define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_K1_256 1
#endif
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_K1_256)
#ifdef PSA_HW_DRIVERS_ONLY
#error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_K1_256"
#endif
#define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_K1_256 1
#endif
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_K1_256)
#ifdef PSA_HW_DRIVERS_ONLY
#error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_K1_256"
#endif
#define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_K1_256 1
#endif
#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_K1_256)
#ifdef PSA_HW_DRIVERS_ONLY
#error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_K1_256"
#endif
#define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP 1
#define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_K1_256 1
#endif
#endif // defined(PSA_WANT_ECC_SECP_K1_256)

#if defined(PSA_WANT_ECC_MONTGOMERY_255)
    #if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_MONTGOMERY_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_MONTGOMERY_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_MONTGOMERY_255 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_MONTGOMERY_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_MONTGOMERY_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_MONTGOMERY_255 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_MONTGOMERY_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_MONTGOMERY_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_MONTGOMERY_255 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_MONTGOMERY_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_MONTGOMERY_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_MONTGOMERY_255 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_MONTGOMERY_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_MONTGOMERY_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_MONTGOMERY_255 1
    #endif
#endif // defined(PSA_WANT_ECC_MONTGOMERY_255)

#if defined(PSA_WANT_ECC_MONTGOMERY_448)
    #if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_MONTGOMERY_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_MONTGOMERY_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_MONTGOMERY_448 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_MONTGOMERY_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_MONTGOMERY_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_MONTGOMERY_448 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_MONTGOMERY_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_MONTGOMERY_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_MONTGOMERY_448 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_MONTGOMERY_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_MONTGOMERY_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_MONTGOMERY_448 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_MONTGOMERY_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_MONTGOMERY_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_MONTGOMERY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_MONTGOMERY_448 1
    #endif
#endif // defined(PSA_WANT_ECC_MONTGOMERY_448)

#if defined(PSA_WANT_ECC_TWISTED_EDWARDS_255)
    #if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_TWISTED_EDWARDS_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_TWISTED_EDWARDS_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_TWISTED_EDWARDS_255 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_TWISTED_EDWARDS_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_TWISTED_EDWARDS_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_TWISTED_EDWARDS_255 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_TWISTED_EDWARDS_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_TWISTED_EDWARDS_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_TWISTED_EDWARDS_255 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_TWISTED_EDWARDS_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_TWISTED_EDWARDS_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_TWISTED_EDWARDS_255 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_TWISTED_EDWARDS_255)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_TWISTED_EDWARDS_255"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_TWISTED_EDWARDS_255 1
    #endif
#endif // defined(PSA_WANT_ECC_TWISTED_EDWARDS_255)

#if defined(PSA_WANT_ECC_TWISTED_EDWARDS_448)
    #if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_TWISTED_EDWARDS_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_PUBLIC_KEY_TWISTED_EDWARDS_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_PUBLIC_KEY_TWISTED_EDWARDS_448 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_TWISTED_EDWARDS_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_IMPORT_TWISTED_EDWARDS_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_IMPORT_TWISTED_EDWARDS_448 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_TWISTED_EDWARDS_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_EXPORT_TWISTED_EDWARDS_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_EXPORT_TWISTED_EDWARDS_448 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) &&                                                            \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_TWISTED_EDWARDS_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_GENERATE_TWISTED_EDWARDS_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_GENERATE_TWISTED_EDWARDS_448 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) &&                                                              \
        !defined(PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_TWISTED_EDWARDS_448)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ECC_KEY_PAIR_DERIVE_TWISTED_EDWARDS_448"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_TWISTED_EDWARDS 1
        #define PSA_NEED_OBERON_KEY_TYPE_ECC_KEY_PAIR_DERIVE_TWISTED_EDWARDS_448 1
    #endif
#endif // defined(PSA_WANT_ECC_TWISTED_EDWARDS_448)

#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_RSA_PUBLIC_KEY)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_RSA_PUBLIC_KEY"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_RSA_PUBLIC_KEY 1
#endif
#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_RSA_KEY_PAIR_IMPORT)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_RSA_KEY_PAIR_IMPORT"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_RSA_KEY_PAIR_IMPORT 1
#endif
#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_RSA_KEY_PAIR_EXPORT)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_RSA_KEY_PAIR_EXPORT"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_RSA_KEY_PAIR_EXPORT 1
#endif

#if defined(PSA_WANT_KEY_TYPE_LMS_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_LMS_PUBLIC_KEY)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_LMS_PUBLIC_KEY"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_LMS_PUBLIC_KEY 1
#endif

#if defined(PSA_WANT_KEY_TYPE_HSS_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_HSS_PUBLIC_KEY)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_HSS_PUBLIC_KEY"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_HSS_PUBLIC_KEY 1
#endif

#if defined(PSA_WANT_KEY_TYPE_XMSS_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_XMSS_PUBLIC_KEY)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_XMSS_PUBLIC_KEY"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_XMSS_PUBLIC_KEY 1
#endif

#if defined(PSA_WANT_KEY_TYPE_XMSS_MT_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_XMSS_MT_PUBLIC_KEY)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_XMSS_MT_PUBLIC_KEY"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_XMSS_MT_PUBLIC_KEY 1
#endif

#if defined(PSA_WANT_KEY_TYPE_ML_DSA_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ML_DSA_PUBLIC_KEY)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ML_DSA_PUBLIC_KEY"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_ML_DSA_PUBLIC_KEY 1
#endif
#if defined(PSA_WANT_KEY_TYPE_ML_DSA_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_ML_DSA_KEY_PAIR_IMPORT)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ML_DSA_KEY_PAIR_IMPORT"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_ML_DSA_KEY_PAIR_IMPORT 1
#endif
#if defined(PSA_WANT_KEY_TYPE_ML_DSA_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_ML_DSA_KEY_PAIR_EXPORT)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ML_DSA_KEY_PAIR_EXPORT"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_ML_DSA_KEY_PAIR_EXPORT 1
#endif

#if defined(PSA_WANT_KEY_TYPE_ML_KEM_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_ML_KEM_PUBLIC_KEY)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ML_KEM_PUBLIC_KEY"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_ML_KEM_PUBLIC_KEY 1
#endif
#if defined(PSA_WANT_KEY_TYPE_ML_KEM_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_ML_KEM_KEY_PAIR_IMPORT)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ML_KEM_KEY_PAIR_IMPORT"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_ML_KEM_KEY_PAIR_IMPORT 1
#endif
#if defined(PSA_WANT_KEY_TYPE_ML_KEM_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_ML_KEM_KEY_PAIR_EXPORT)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_ML_KEM_KEY_PAIR_EXPORT"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_ML_KEM_KEY_PAIR_EXPORT 1
#endif


/* Oberon MAC Driver */

#if defined(PSA_WANT_ALG_HMAC)
    #if defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_HMAC_SHA_1)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for HMAC_SHA_1"
        #endif
        #define PSA_NEED_OBERON_MAC_DRIVER 1
        #define PSA_NEED_OBERON_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_HMAC_SHA_224)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for HMAC_SHA_224"
        #endif
        #define PSA_NEED_OBERON_MAC_DRIVER 1
        #define PSA_NEED_OBERON_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_HMAC_SHA_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for HMAC_SHA_256"
        #endif
        #define PSA_NEED_OBERON_MAC_DRIVER 1
        #define PSA_NEED_OBERON_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_HMAC_SHA_384)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for HMAC_SHA_384"
        #endif
        #define PSA_NEED_OBERON_MAC_DRIVER 1
        #define PSA_NEED_OBERON_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_HMAC_SHA_512)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for HMAC_SHA_512"
        #endif
        #define PSA_NEED_OBERON_MAC_DRIVER 1
        #define PSA_NEED_OBERON_HMAC 1
    #endif
#endif // defined(PSA_WANT_ALG_HMAC)

#if defined(PSA_WANT_ALG_CMAC)
    #if defined(PSA_WANT_AES_KEY_SIZE_128) && !defined(PSA_ACCEL_CMAC_AES_128)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CMAC_AES_128"
        #endif
        #define PSA_NEED_OBERON_MAC_DRIVER 1
        #define PSA_NEED_OBERON_CMAC 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_192) && !defined(PSA_ACCEL_CMAC_AES_192)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CMAC_AES_192"
        #endif
        #define PSA_NEED_OBERON_MAC_DRIVER 1
        #define PSA_NEED_OBERON_CMAC 1
    #endif
    #if defined(PSA_WANT_AES_KEY_SIZE_256) && !defined(PSA_ACCEL_CMAC_AES_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for CMAC_AES_256"
        #endif
        #define PSA_NEED_OBERON_MAC_DRIVER 1
        #define PSA_NEED_OBERON_CMAC 1
    #endif
#endif // defined(PSA_WANT_ALG_CMAC)

/* Oberon KDF Driver */

#if defined(PSA_WANT_ALG_HKDF)
    #if defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_HKDF_SHA_1)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_HKDF_SHA_224)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_HKDF_SHA_256)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_HKDF_SHA_384)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_HKDF_SHA_512)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF 1
    #endif
#endif // defined(PSA_WANT_ALG_HKDF)

#if defined(PSA_WANT_ALG_HKDF_EXTRACT)
    #if defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_HKDF_EXTRACT_SHA_1)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXTRACT 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_HKDF_EXTRACT_SHA_224)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXTRACT 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_HKDF_EXTRACT_SHA_256)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXTRACT 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_HKDF_EXTRACT_SHA_384)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXTRACT 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_HKDF_EXTRACT_SHA_512)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXTRACT 1
    #endif
#endif // defined(PSA_WANT_ALG_HKDF_EXTRACT)

#if defined(PSA_WANT_ALG_HKDF_EXPAND)
    #if defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_HKDF_EXPAND_SHA_1)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXPAND 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_HKDF_EXPAND_SHA_224)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXPAND 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_HKDF_EXPAND_SHA_256)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXPAND 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_HKDF_EXPAND_SHA_384)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXPAND 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_HKDF_EXPAND_SHA_512)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_HKDF_EXPAND 1
    #endif
#endif // defined(PSA_WANT_ALG_HKDF_EXPAND)

#if defined(PSA_WANT_ALG_TLS12_PRF)
    #if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_TLS12_PRF_SHA_256)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_TLS12_PRF 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_TLS12_PRF_SHA_384)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_TLS12_PRF 1
    #endif
#endif // defined(PSA_WANT_ALG_TLS12_PRF)

#if defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)
    #if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_TLS12_PSK_TO_MS_SHA_256)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_TLS12_PSK_TO_MS 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_TLS12_PSK_TO_MS_SHA_384)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_TLS12_PSK_TO_MS 1
    #endif
#endif // defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)

#if defined(PSA_WANT_ALG_PBKDF2_HMAC)
    #if defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_PBKDF2_HMAC_SHA_1)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_PBKDF2_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_PBKDF2_HMAC_SHA_224)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_PBKDF2_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_PBKDF2_HMAC_SHA_256)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_PBKDF2_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_PBKDF2_HMAC_SHA_384)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_PBKDF2_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_PBKDF2_HMAC_SHA_512)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_PBKDF2_HMAC 1
    #endif
#endif // defined(PSA_WANT_ALG_PBKDF2_HMAC)

#if defined(PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128) && !defined(PSA_ACCEL_PBKDF2_AES_CMAC_PRF_128)
    #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
    #define PSA_NEED_OBERON_PBKDF2_AES_CMAC_PRF_128 1
#endif

#if defined(PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS) && !defined(PSA_ACCEL_TLS12_ECJPAKE_TO_PMS)
    #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
    #define PSA_NEED_OBERON_TLS12_ECJPAKE_TO_PMS 1
#endif

#if defined(PSA_WANT_ALG_SRP_PASSWORD_HASH) && !defined(PSA_ACCEL_SRP_PASSWORD_HASH)
    #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
    #define PSA_NEED_OBERON_SRP_PASSWORD_HASH 1
#endif

#if defined(PSA_WANT_ALG_WPA3_SAE_H2E) && !defined(PSA_ACCEL_WPA3_SAE_H2E)
    #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
    #define PSA_NEED_OBERON_WPA3_SAE_H2E 1
#endif

#if defined(PSA_WANT_ALG_SP800_108_COUNTER_HMAC)
    #if defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_SP800_108_COUNTER_HMA_SHA_1)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_SP800_108_COUNTER_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_SP800_108_COUNTER_HMA_SHA_224)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_SP800_108_COUNTER_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_SP800_108_COUNTER_HMA_SHA_256)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_SP800_108_COUNTER_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_SP800_108_COUNTER_HMA_SHA_384)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_SP800_108_COUNTER_HMAC 1
    #endif
    #if defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_SP800_108_COUNTER_HMA_SHA_512)
        #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
        #define PSA_NEED_OBERON_SP800_108_COUNTER_HMAC 1
    #endif
#endif // defined(PSA_WANT_ALG_SP800_108_COUNTER_HMAC)

#if defined(PSA_WANT_ALG_SP800_108_COUNTER_CMAC) && !defined(PSA_ACCEL_SP800_108_COUNTER_CMAC)
    #define PSA_NEED_OBERON_KEY_DERIVATION_DRIVER 1
    #define PSA_NEED_OBERON_SP800_108_COUNTER_CMAC 1
#endif

/* Oberon PAKE Driver */

#if defined(PSA_WANT_ALG_JPAKE)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_ECJPAKE_SECP_R1_256_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_ECJPAKE_SECP_R1_256_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_ECJPAKE_SECP_R1_256_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_ECJPAKE_SECP_R1_256_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_ECJPAKE_SECP_R1_256_SHA_512))
        #define PSA_NEED_OBERON_PAKE_DRIVER 1
        #define PSA_NEED_OBERON_JPAKE 1
        #define PSA_NEED_OBERON_ECJPAKE_SECP_R1_256 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_JPAKE)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for ECJPAKE_SECP_R1_256"
    #endif
#endif

#if defined(PSA_WANT_ALG_SPAKE2P_HMAC)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_SPAKE2P_HMAC_SECP_R1_256_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_SPAKE2P_HMAC_SECP_R1_256_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_SPAKE2P_HMAC_SECP_R1_256_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_SPAKE2P_HMAC_SECP_R1_256_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_SPAKE2P_HMAC_SECP_R1_256_SHA_512))
        #define PSA_NEED_OBERON_PAKE_DRIVER 1
        #define PSA_NEED_OBERON_SPAKE2P 1
        #define PSA_NEED_OBERON_SPAKE2P_HMAC_SECP_R1_256 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_SPAKE2P)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SPAKE2P_HMAC_SECP_R1_256"
    #endif
#endif

#if defined(PSA_WANT_ALG_SPAKE2P_CMAC)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_SPAKE2P_CMAC_SECP_R1_256_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_SPAKE2P_CMAC_SECP_R1_256_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_SPAKE2P_CMAC_SECP_R1_256_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_SPAKE2P_CMAC_SECP_R1_256_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_SPAKE2P_CMAC_SECP_R1_256_SHA_512))
        #define PSA_NEED_OBERON_PAKE_DRIVER 1
        #define PSA_NEED_OBERON_SPAKE2P 1
        #define PSA_NEED_OBERON_SPAKE2P_CMAC_SECP_R1_256 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_SPAKE2P)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SPAKE2P_CMAC_SECP_R1_256"
    #endif
#endif

#if defined(PSA_WANT_ALG_SPAKE2P_MATTER) && !defined(PSA_ACCEL_SPAKE2P_MATTER)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SPAKE2P_MATTER"
    #endif
    #define PSA_NEED_OBERON_PAKE_DRIVER 1
    #define PSA_NEED_OBERON_SPAKE2P 1
    #define PSA_NEED_OBERON_SPAKE2P_MATTER 1
#endif

#if defined(PSA_WANT_ALG_SRP_6)
    #if (defined(PSA_WANT_ALG_SHA_1) && !defined(PSA_ACCEL_SRP_6_3072_SHA_1)) || \
        (defined(PSA_WANT_ALG_SHA_224) && !defined(PSA_ACCEL_SRP_6_3072_SHA_224)) || \
        (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_SRP_6_3072_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_SRP_6_3072_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_SRP_6_3072_SHA_512))
        #define PSA_NEED_OBERON_PAKE_DRIVER 1
        #define PSA_NEED_OBERON_SRP_6 1
        #define PSA_NEED_OBERON_SRP_6_3072 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_SRP_6)
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for SRP_6"
    #endif
#endif

#if defined(PSA_WANT_ALG_WPA3_SAE_FIXED) || defined(PSA_WANT_ALG_WPA3_SAE_GDH)
    #if (defined(PSA_WANT_ALG_SHA_256) && !defined(PSA_ACCEL_WPA3_SAE_SECP_R1_256_SHA_256)) || \
        (defined(PSA_WANT_ALG_SHA_384) && !defined(PSA_ACCEL_WPA3_SAE_SECP_R1_256_SHA_384)) || \
        (defined(PSA_WANT_ALG_SHA_512) && !defined(PSA_ACCEL_WPA3_SAE_SECP_R1_256_SHA_512))
        #define PSA_NEED_OBERON_PAKE_DRIVER 1
        #define PSA_NEED_OBERON_WPA3_SAE 1
        #define PSA_NEED_OBERON_WPA3_SAE_SECP_R1_256 1
    #endif
    #if defined(PSA_HW_DRIVERS_ONLY) && defined(PSA_NEED_OBERON_WPA3_SAE)
    #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for WPA3_SAE_SECP_R1_256"
    #endif
#endif

#if defined(PSA_WANT_ECC_SECP_R1_256)
    #if defined(PSA_WANT_KEY_TYPE_SPAKE2P_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_SPAKE2P_PUBLIC_KEY_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_SPAKE2P_PUBLIC_KEY_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_PUBLIC_KEY 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_PUBLIC_KEY_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_PUBLIC_KEY_SECP_R1_256 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_SPAKE2P_KEY_PAIR_IMPORT) &&                                                          \
        !defined(PSA_ACCEL_KEY_TYPE_SPAKE2P_KEY_PAIR_IMPORT_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_SPAKE2P_KEY_PAIR_IMPORT_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_IMPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_IMPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_IMPORT_SECP_R1_256 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_SPAKE2P_KEY_PAIR_EXPORT) &&                                                          \
        !defined(PSA_ACCEL_KEY_TYPE_SPAKE2P_KEY_PAIR_EXPORT_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_SPAKE2P_KEY_PAIR_EXPORT_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_EXPORT 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_EXPORT_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_EXPORT_SECP_R1_256 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_SPAKE2P_KEY_PAIR_DERIVE) &&                                                          \
        !defined(PSA_ACCEL_KEY_TYPE_SPAKE2P_KEY_PAIR_DERIVE_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_SPAKE2P_KEY_PAIR_DERIVE_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_DERIVE 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_DERIVE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_SPAKE2P_KEY_PAIR_DERIVE_SECP_R1_256 1
    #endif
    #if defined(PSA_WANT_KEY_TYPE_WPA3_SAE) &&                                                          \
        !defined(PSA_ACCEL_KEY_TYPE_WPA3_SAE_SECP_R1_256)
        #ifdef PSA_HW_DRIVERS_ONLY
            #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_WPA3_SAE_SECP_R1_256"
        #endif
        #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
        #define PSA_NEED_OBERON_KEY_TYPE_WPA3_SAE 1
        #define PSA_NEED_OBERON_KEY_TYPE_WPA3_SAE_SECP 1
        #define PSA_NEED_OBERON_KEY_TYPE_WPA3_SAE_SECP_R1_256 1
    #endif
#endif // defined(PSA_WANT_ECC_SECP_R1_256)

#if defined(PSA_WANT_KEY_TYPE_SRP_PUBLIC_KEY) && !defined(PSA_ACCEL_KEY_TYPE_SRP_6_PUBLIC_KEY_3072)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_SRP_6_PUBLIC_KEY_3072"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_SRP_6_PUBLIC_KEY 1
    #define PSA_NEED_OBERON_KEY_TYPE_SRP_6_PUBLIC_KEY_3072 1
#endif
#if defined(PSA_WANT_KEY_TYPE_SRP_KEY_PAIR_IMPORT) && !defined(PSA_ACCEL_KEY_TYPE_SRP_6_KEY_PAIR_IMPORT_3072)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_SRP_6_KEY_PAIR_IMPORT_3072"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_SRP_6_KEY_PAIR_IMPORT 1
    #define PSA_NEED_OBERON_KEY_TYPE_SRP_6_KEY_PAIR_IMPORT_3072 1
#endif
#if defined(PSA_WANT_KEY_TYPE_SRP_KEY_PAIR_EXPORT) && !defined(PSA_ACCEL_KEY_TYPE_SRP_6_KEY_PAIR_EXPORT_3072)
    #ifdef PSA_HW_DRIVERS_ONLY
        #error "PSA_HW_DRIVERS_ONLY defined, but no hardware acceleration for KEY_TYPE_SRP_6_KEY_PAIR_EXPORT_3072"
    #endif
    #define PSA_NEED_OBERON_KEY_MANAGEMENT_DRIVER 1
    #define PSA_NEED_OBERON_KEY_TYPE_SRP_6_KEY_PAIR_EXPORT 1
    #define PSA_NEED_OBERON_KEY_TYPE_SRP_6_KEY_PAIR_EXPORT_3072 1
#endif

/* Oberon Key Wrap Driver */

#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_KW) && !defined(PSA_ACCEL_ALG_AES_KW)
#define PSA_NEED_OBERON_KEY_WRAP_DRIVER 1
#define PSA_NEED_OBERON_AES_KW 1
#endif
#if defined(PSA_WANT_KEY_TYPE_AES) && defined(PSA_WANT_ALG_KWP) && !defined(PSA_ACCEL_ALG_AES_KWP)
#define PSA_NEED_OBERON_KEY_WRAP_DRIVER 1
#define PSA_NEED_OBERON_AES_KWP 1
#endif

/* Oberon Random Driver */

#if defined(PSA_WANT_GENERATE_RANDOM)
    #if defined(PSA_USE_CTR_DRBG_DRIVER)
        #if defined(PSA_ACCEL_GENERATE_RANDOM)
            #error "No more than one DRBG_DRIVER usage must be defined."
        #endif
        #define PSA_NEED_OBERON_CTR_DRBG_DRIVER 1
        #define PSA_ACCEL_GENERATE_RANDOM
    #endif
    #if defined(PSA_USE_HMAC_DRBG_DRIVER)
        #if defined(PSA_ACCEL_GENERATE_RANDOM)
            #error "No more than one DRBG_DRIVER usage must be defined."
        #endif
        #define PSA_NEED_OBERON_HMAC_DRBG_DRIVER 1
        #define PSA_ACCEL_GENERATE_RANDOM
    #endif

    #if !defined(PSA_ACCEL_GENERATE_RANDOM)
        #error "PSA_WANT_GENERATE_RANDOM defined, but no random driver"
    #endif

    #if !defined(PSA_ACCEL_GET_ENTROPY)
        #error "PSA_WANT_GENERATE_RANDOM defined, but no entropy driver"
    #endif
#endif // defined(PSA_WANT_GENERATE_RANDOM)

#endif /* OBERON_CONFIG_H */
