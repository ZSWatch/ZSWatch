/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2020 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef OBERON_CHECK_UNSUPPORTED_H
#define OBERON_CHECK_UNSUPPORTED_H

/* Currently Unsupported Algorithms */

#if defined(CONFIG_PSA_WANT_ALG_SHA_512_224) && !defined(CONFIG_PSA_ACCEL_SHA_512_224)
    #error "No crypto implementation for SHA-512-224"
#endif
#if defined(CONFIG_PSA_WANT_ALG_SHA_512_256) && !defined(CONFIG_PSA_ACCEL_SHA_512_256)
    #error "No crypto implementation for SHA-512-256"
#endif
#if defined(CONFIG_PSA_WANT_ALG_MD5) && !defined(CONFIG_PSA_ACCEL_MD5)
    #error "No crypto implementation for MD5"
#endif
#if defined(CONFIG_PSA_WANT_ALG_RIPEMD160) && !defined(CONFIG_PSA_ACCEL_RIPEMD160)
    #error "No crypto implementation for RIPEMD160"
#endif

#if defined(CONFIG_PSA_WANT_KEY_TYPE_AES) && defined(CONFIG_PSA_WANT_ALG_CFB)
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_128) && !defined(CONFIG_PSA_ACCEL_CFB_AES_128)
        #error "No crypto implementation for 128 bit AES-CFB"
    #endif
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_192) && !defined(CONFIG_PSA_ACCEL_CFB_AES_192)
        #error "No crypto implementation for 192 bit AES-CFB"
    #endif
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_256) && !defined(CONFIG_PSA_ACCEL_CFB_AES_256)
        #error "No crypto implementation for 256 bit AES-CFB"
    #endif
#endif
#if defined(CONFIG_PSA_WANT_KEY_TYPE_AES) && defined(CONFIG_PSA_WANT_ALG_OFB)
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_128) && !defined(CONFIG_PSA_ACCEL_OFB_AES_128)
        #error "No crypto implementation for 128 bit AES-OFB"
    #endif
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_192) && !defined(CONFIG_PSA_ACCEL_OFB_AES_192)
        #error "No crypto implementation for 192 bit AES-OFB"
    #endif
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_256) && !defined(CONFIG_PSA_ACCEL_OFB_AES_256)
        #error "No crypto implementation for 256 bit AES-OFB"
    #endif
#endif
#if defined(CONFIG_PSA_WANT_KEY_TYPE_AES) && defined(CONFIG_PSA_WANT_ALG_XTS)
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_128) && !defined(CONFIG_PSA_ACCEL_XTS_AES_128)
        #error "No crypto implementation for 128 bit AES-XTS"
    #endif
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_192) && !defined(CONFIG_PSA_ACCEL_XTS_AES_192)
        #error "No crypto implementation for 192 bit AES-XTS"
    #endif
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_256) && !defined(CONFIG_PSA_ACCEL_XTS_AES_256)
        #error "No crypto implementation for 256 bit AES-XTS"
    #endif
#endif
#if defined(CONFIG_PSA_WANT_KEY_TYPE_AES) && defined(CONFIG_PSA_WANT_ALG_CBC_MAC)
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_128) && !defined(CONFIG_PSA_ACCEL_CBC_MAC_AES_128)
        #error "No crypto implementation for 128 bit AES-CBC-MAC"
    #endif
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_192) && !defined(CONFIG_PSA_ACCEL_CBC_MAC_AES_192)
        #error "No crypto implementation for 192 bit AES-CBC-MAC"
    #endif
    #if defined(CONFIG_PSA_WANT_AES_KEY_SIZE_256) && !defined(CONFIG_PSA_ACCEL_CBC_MAC_AES_256)
        #error "No crypto implementation for 256 bit AES-CBC-MAC"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ALG_FFDH)
    #if defined(CONFIG_PSA_WANT_DH_KEY_SIZE_2048) && !defined(CONFIG_PSA_ACCEL_FFDH_2048)
        #error "No crypto implementation for 2048 bit FFDH"
    #endif
    #if defined(CONFIG_PSA_WANT_DH_KEY_SIZE_3072) && !defined(CONFIG_PSA_ACCEL_FFDH_3072)
        #error "No crypto implementation for 3072 bit FFDH"
    #endif
    #if defined(CONFIG_PSA_WANT_DH_KEY_SIZE_4096) && !defined(CONFIG_PSA_ACCEL_FFDH_4096)
        #error "No crypto implementation for 4096 bit FFDH"
    #endif
    #if defined(CONFIG_PSA_WANT_DH_KEY_SIZE_6144) && !defined(CONFIG_PSA_ACCEL_FFDH_6144)
        #error "No crypto implementation for 6144 bit FFDH"
    #endif
    #if defined(CONFIG_PSA_WANT_DH_KEY_SIZE_8192) && !defined(CONFIG_PSA_ACCEL_FFDH_8192)
        #error "No crypto implementation for 8192 bit FFDH"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECP_K1_192)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECP_K1_192)
        #error "No crypto implementation for secp-k1-192 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_K1_192)
        #error "No crypto implementation for secp-k1-192 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_K1_192)
        #error "No crypto implementation for secp-k1-192 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_K1_192)
        #error "No crypto implementation for secp-k1-192 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_K1_192)
        #error "No crypto implementation for secp-k1-192 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECP_K1_224)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECP_K1_224)
        #error "No crypto implementation for secp-k1-224 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_K1_224)
        #error "No crypto implementation for secp-k1-224 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_K1_224)
        #error "No crypto implementation for secp-k1-224 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_K1_224)
        #error "No crypto implementation for secp-k1-224 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_K1_224)
        #error "No crypto implementation for secp-k1-224 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECP_R1_192)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECP_R1_192)
        #error "No crypto implementation for secp-r1-192 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECP_R1_192)
        #error "No crypto implementation for secp-r1-192 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECP_R1_192)
        #error "No crypto implementation for secp-r1-192 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECP_R1_192)
        #error "No crypto implementation for secp-r1-192 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECP_R1_192)
        #error "No crypto implementation for secp-r1-192 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_K1_163)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_K1_163)
        #error "No crypto implementation for sect-k1-163 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_K1_163)
        #error "No crypto implementation for sect-k1-163 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_K1_163)
        #error "No crypto implementation for sect-k1-163 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_K1_163)
        #error "No crypto implementation for sect-k1-163 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_K1_163)
        #error "No crypto implementation for sect-k1-163 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_K1_233)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_K1_233)
        #error "No crypto implementation for sect-k1-233 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_K1_233)
        #error "No crypto implementation for sect-k1-233 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_K1_233)
        #error "No crypto implementation for sect-k1-233 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_K1_233)
        #error "No crypto implementation for sect-k1-233 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_K1_233)
        #error "No crypto implementation for sect-k1-233 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_K1_239)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_K1_239)
        #error "No crypto implementation for sect-k1-239 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_K1_239)
        #error "No crypto implementation for sect-k1-239 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_K1_239)
        #error "No crypto implementation for sect-k1-239 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_K1_239)
        #error "No crypto implementation for sect-k1-239 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_K1_239)
        #error "No crypto implementation for sect-k1-239 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_K1_283)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_K1_283)
        #error "No crypto implementation for sect-k1-283 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_K1_283)
        #error "No crypto implementation for sect-k1-283 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_K1_283)
        #error "No crypto implementation for sect-k1-283 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_K1_283)
        #error "No crypto implementation for sect-k1-283 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_K1_283)
        #error "No crypto implementation for sect-k1-283 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_K1_409)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_K1_409)
        #error "No crypto implementation for sect-k1-409 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_K1_409)
        #error "No crypto implementation for sect-k1-409 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_K1_409)
        #error "No crypto implementation for sect-k1-409 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_K1_409)
        #error "No crypto implementation for sect-k1-409 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_K1_409)
        #error "No crypto implementation for sect-k1-409 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_K1_571)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_K1_571)
        #error "No crypto implementation for sect-k1-571 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_K1_571)
        #error "No crypto implementation for sect-k1-571 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_K1_571)
        #error "No crypto implementation for sect-k1-571 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_K1_571)
        #error "No crypto implementation for sect-k1-571 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_K1_571)
        #error "No crypto implementation for sect-k1-571 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_R1_163)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_R1_163)
        #error "No crypto implementation for sect-r1-163 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_R1_163)
        #error "No crypto implementation for sect-r1-163 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_R1_163)
        #error "No crypto implementation for sect-r1-163 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_R1_163)
        #error "No crypto implementation for sect-r1-163 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_R1_163)
        #error "No crypto implementation for sect-r1-163 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_R1_233)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_R1_233)
        #error "No crypto implementation for sect-r1-233 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_R1_233)
        #error "No crypto implementation for sect-r1-233 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_R1_233)
        #error "No crypto implementation for sect-r1-233 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_R1_233)
        #error "No crypto implementation for sect-r1-233 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_R1_233)
        #error "No crypto implementation for sect-r1-233 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_R1_283)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_R1_283)
        #error "No crypto implementation for sect-r1-283 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_R1_283)
        #error "No crypto implementation for sect-r1-283 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_R1_283)
        #error "No crypto implementation for sect-r1-283 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_R1_283)
        #error "No crypto implementation for sect-r1-283 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_R1_283)
        #error "No crypto implementation for sect-r1-283 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_R1_409)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_R1_409)
        #error "No crypto implementation for sect-r1-409 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_R1_409)
        #error "No crypto implementation for sect-r1-409 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_R1_409)
        #error "No crypto implementation for sect-r1-409 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_R1_409)
        #error "No crypto implementation for sect-r1-409 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_R1_409)
        #error "No crypto implementation for sect-r1-409 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_SECT_R1_571)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_SECT_R1_571)
        #error "No crypto implementation for sect-r1-571 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_SECT_R1_571)
        #error "No crypto implementation for sect-r1-571 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_SECT_R1_571)
        #error "No crypto implementation for sect-r1-571 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_SECT_R1_571)
        #error "No crypto implementation for sect-r1-571 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_SECT_R1_571)
        #error "No crypto implementation for sect-r1-571 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_160)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_BRAINPOOL_P_R1_160)
        #error "No crypto implementation for brainpoolP160r1 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_BRAINPOOL_P_R1_160)
        #error "No crypto implementation for brainpoolP160r1 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_BRAINPOOL_P_R1_160)
        #error "No crypto implementation for brainpoolP160r1 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_BRAINPOOL_P_R1_160)
        #error "No crypto implementation for brainpoolP160r1 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_BRAINPOOL_P_R1_160)
        #error "No crypto implementation for brainpoolP160r1 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_192)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_BRAINPOOL_P_R1_192)
        #error "No crypto implementation for brainpoolP192r1 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_BRAINPOOL_P_R1_192)
        #error "No crypto implementation for brainpoolP192r1 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_BRAINPOOL_P_R1_192)
        #error "No crypto implementation for brainpoolP192r1 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_BRAINPOOL_P_R1_192)
        #error "No crypto implementation for brainpoolP192r1 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_BRAINPOOL_P_R1_192)
        #error "No crypto implementation for brainpoolP192r1 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_224)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_BRAINPOOL_P_R1_224)
        #error "No crypto implementation for brainpoolP224r1 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_BRAINPOOL_P_R1_224)
        #error "No crypto implementation for brainpoolP224r1 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_BRAINPOOL_P_R1_224)
        #error "No crypto implementation for brainpoolP224r1 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_BRAINPOOL_P_R1_224)
        #error "No crypto implementation for brainpoolP224r1 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_BRAINPOOL_P_R1_224)
        #error "No crypto implementation for brainpoolP224r1 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_256)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_BRAINPOOL_P_R1_256)
        #error "No crypto implementation for brainpoolP256r1 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_BRAINPOOL_P_R1_256)
        #error "No crypto implementation for brainpoolP256r1 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_BRAINPOOL_P_R1_256)
        #error "No crypto implementation for brainpoolP256r1 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_BRAINPOOL_P_R1_256)
        #error "No crypto implementation for brainpoolP256r1 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_BRAINPOOL_P_R1_256)
        #error "No crypto implementation for brainpoolP256r1 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_320)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_BRAINPOOL_P_R1_320)
        #error "No crypto implementation for brainpoolP320r1 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_BRAINPOOL_P_R1_320)
        #error "No crypto implementation for brainpoolP320r1 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_BRAINPOOL_P_R1_320)
        #error "No crypto implementation for brainpoolP320r1 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_BRAINPOOL_P_R1_320)
        #error "No crypto implementation for brainpoolP320r1 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_BRAINPOOL_P_R1_320)
        #error "No crypto implementation for brainpoolP320r1 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_384)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_BRAINPOOL_P_R1_384)
        #error "No crypto implementation for brainpoolP384r1 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_BRAINPOOL_P_R1_384)
        #error "No crypto implementation for brainpoolP384r1 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_BRAINPOOL_P_R1_384)
        #error "No crypto implementation for brainpoolP384r1 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_BRAINPOOL_P_R1_384)
        #error "No crypto implementation for brainpoolP384r1 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_BRAINPOOL_P_R1_384)
        #error "No crypto implementation for brainpoolP384r1 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_ECC_BRAINPOOL_P_R1_512)
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) && !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY_BRAINPOOL_P_R1_512)
        #error "No crypto implementation for brainpoolP512r1 public key"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_IMPORT_BRAINPOOL_P_R1_512)
        #error "No crypto implementation for brainpoolP512r1 key pair import"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_EXPORT_BRAINPOOL_P_R1_512)
        #error "No crypto implementation for brainpoolP512r1 key pair export"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_GENERATE_BRAINPOOL_P_R1_512)
        #error "No crypto implementation for brainpoolP512r1 key pair generate"
    #endif
    #if defined(CONFIG_PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE) && \
        !defined(CONFIG_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR_DERIVE_BRAINPOOL_P_R1_512)
        #error "No crypto implementation for brainpoolP512r1 key pair derive"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_KEY_TYPE_ARIA)
    #if defined(CONFIG_PSA_WANT_ALG_CCM) && !defined(CONFIG_PSA_ACCEL_CCM_ARIA)
        #error "No crypto implementation for ARIA-CCM"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_GCM) && !defined(CONFIG_PSA_ACCEL_GCM_ARIA)
        #error "No crypto implementation for ARIA-GCM"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CTR) && !defined(CONFIG_PSA_ACCEL_CTR_ARIA)
        #error "No crypto implementation for ARIA-CTR"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_PKCS7) && !defined(CONFIG_PSA_ACCEL_CBC_PKCS7_ARIA)
        #error "No crypto implementation for ARIA-CBC-PKCS7"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_NO_PADDING) && !defined(CONFIG_PSA_ACCEL_CBC_NO_PADDING_ARIA)
        #error "No crypto implementation for ARIA-CBC-no-padding"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_ECB_NO_PADDING) && !defined(CONFIG_PSA_ACCEL_ECB_NO_PADDING_ARIA)
        #error "No crypto implementation for ARIA-ECB-no-padding"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CFB) && !defined(CONFIG_PSA_ACCEL_CFB_ARIA)
        #error "No crypto implementation for ARIA-CFB"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_OFB) && !defined(CONFIG_PSA_ACCEL_OFB_ARIA)
        #error "No crypto implementation for ARIA-OFB"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_XTS) && !defined(CONFIG_PSA_ACCEL_XTS_ARIA)
        #error "No crypto implementation for ARIA-XTS"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_MAC) && !defined(CONFIG_PSA_ACCEL_CBC_MAC_ARIA)
        #error "No crypto implementation for ARIA-CBC-MAC"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CMAC) && !defined(CONFIG_PSA_ACCEL_CMAC_ARIA)
        #error "No crypto implementation for ARIA-CMAC"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_KEY_TYPE_CAMELLIA)
    #if defined(CONFIG_PSA_WANT_ALG_CCM) && !defined(CONFIG_PSA_ACCEL_CCM_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-CCM"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_GCM) && !defined(CONFIG_PSA_ACCEL_GCM_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-GCM"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CTR) && !defined(CONFIG_PSA_ACCEL_CTR_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-CTR"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_PKCS7) && !defined(CONFIG_PSA_ACCEL_CBC_PKCS7_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-CBC-PKCS7"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_NO_PADDING) && !defined(CONFIG_PSA_ACCEL_CBC_NO_PADDING_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-CBC-no-padding"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_ECB_NO_PADDING) && !defined(CONFIG_PSA_ACCEL_ECB_NO_PADDING_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-ECB-no-padding"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CFB) && !defined(CONFIG_PSA_ACCEL_CFB_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-CFB"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_OFB) && !defined(CONFIG_PSA_ACCEL_OFB_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-OFB"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_XTS) && !defined(CONFIG_PSA_ACCEL_XTS_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-XTS"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_MAC) && !defined(CONFIG_PSA_ACCEL_CBC_MAC_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-CBC-MAC"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CMAC) && !defined(CONFIG_PSA_ACCEL_CMAC_CAMELLIA)
        #error "No crypto implementation for CAMELLIA-CMAC"
    #endif
#endif

#if defined(CONFIG_PSA_WANT_KEY_TYPE_DES)
    #if defined(CONFIG_PSA_WANT_ALG_CTR) && !defined(CONFIG_PSA_ACCEL_CTR_DES)
        #error "No crypto implementation for DES-CTR"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_PKCS7) && !defined(CONFIG_PSA_ACCEL_CBC_PKCS7_DES)
        #error "No crypto implementation for DES-CBC-PKCS7"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_NO_PADDING) && !defined(CONFIG_PSA_ACCEL_CBC_NO_PADDING_DES)
        #error "No crypto implementation for DES-CBC-no-padding"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_ECB_NO_PADDING) && !defined(CONFIG_PSA_ACCEL_ECB_NO_PADDING_DES)
        #error "No crypto implementation for DES-ECB-no-padding"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CFB) && !defined(CONFIG_PSA_ACCEL_CFB_DES)
        #error "No crypto implementation for DES-CFB"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_OFB) && !defined(CONFIG_PSA_ACCEL_OFB_DES)
        #error "No crypto implementation for DES-OFB"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_XTS) && !defined(CONFIG_PSA_ACCEL_XTS_DES)
        #error "No crypto implementation for DES-XTS"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CBC_MAC) && !defined(CONFIG_PSA_ACCEL_CBC_MAC_DES)
        #error "No crypto implementation for DES-CBC-MAC"
    #endif
    #if defined(CONFIG_PSA_WANT_ALG_CMAC) && !defined(CONFIG_PSA_ACCEL_CMAC_DES)
        #error "No crypto implementation for DES-CMAC"
    #endif
#endif

#endif /* OBERON_CHECK_UNSUPPORTED_H */
