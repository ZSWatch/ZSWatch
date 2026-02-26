/*
 * This module provides a thin abstraction over some of the crypto
 * primitives to make it easier to swap out the used crypto library.
 *
 * At this point, there are two choices: MCUBOOT_USE_MBED_TLS, or
 * MCUBOOT_USE_TINYCRYPT.  It is a compile error there is not exactly
 * one of these defined.
 */

#ifndef __BOOTUTIL_CRYPTO_AES_CTR_H_
#define __BOOTUTIL_CRYPTO_AES_CTR_H_

#include <string.h>

#include "mcuboot_config/mcuboot_config.h"

#if (defined(MCUBOOT_USE_MBED_TLS) + \
     defined(MCUBOOT_USE_TINYCRYPT) + defined(MCUBOOT_USE_PSA_CRYPTO)) != 1
    #error "One crypto backend must be defined: either MBED_TLS or TINYCRYPT or PSA"
#endif

#include "bootutil/enc_key_public.h"

#if defined(MCUBOOT_USE_MBED_TLS)
    #include <mbedtls/aes.h>
    #define BOOT_ENC_BLOCK_SIZE (16)
#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_TINYCRYPT)
    #include <string.h>
    #include <tinycrypt/aes.h>
    #include <tinycrypt/ctr_mode.h>
    #include <tinycrypt/constants.h>
    #if defined(MCUBOOT_AES_256) || (BOOT_ENC_KEY_SIZE != TC_AES_KEY_SIZE)
        #error "Cannot use AES-256 for encryption with Tinycrypt library."
    #endif
    #define BOOT_ENC_BLOCK_SIZE TC_AES_BLOCK_SIZE
#endif /* MCUBOOT_USE_TINYCRYPT */

#if defined(MCUBOOT_USE_PSA_CRYPTO)
    #include <psa/crypto.h>
    #define BOOT_ENC_BLOCK_SIZE (16)
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MCUBOOT_USE_PSA_CRYPTO)
typedef struct {
	/* Fixme: This should not be, here, psa_key_id should be passed */
	uint8_t key[BOOT_ENC_KEY_SIZE];
} bootutil_aes_ctr_context;

void bootutil_aes_ctr_init(bootutil_aes_ctr_context *ctx);

static inline void bootutil_aes_ctr_drop(bootutil_aes_ctr_context *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
}

static inline int bootutil_aes_ctr_set_key(bootutil_aes_ctr_context *ctx, const uint8_t *k)
{
    memcpy(ctx->key, k, sizeof(ctx->key));

    return 0;
}

int bootutil_aes_ctr_encrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter,
                             const uint8_t *m, uint32_t mlen, size_t blk_off, uint8_t *c);
int bootutil_aes_ctr_decrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter,
                             const uint8_t *c, uint32_t clen, size_t blk_off, uint8_t *m);
#endif

#if defined(MCUBOOT_USE_MBED_TLS)
typedef mbedtls_aes_context bootutil_aes_ctr_context;
static inline void bootutil_aes_ctr_init(bootutil_aes_ctr_context *ctx)
{
    (void)mbedtls_aes_init(ctx);
}

static inline void bootutil_aes_ctr_drop(bootutil_aes_ctr_context *ctx)
{
    mbedtls_aes_free(ctx);
}

static inline int bootutil_aes_ctr_set_key(bootutil_aes_ctr_context *ctx, const uint8_t *k)
{
    return mbedtls_aes_setkey_enc(ctx, k, BOOT_ENC_KEY_SIZE * 8);
}

static inline int bootutil_aes_ctr_encrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *m, uint32_t mlen, size_t blk_off, uint8_t *c)
{
    uint8_t stream_block[BOOT_ENC_BLOCK_SIZE];
    return mbedtls_aes_crypt_ctr(ctx, mlen, &blk_off, counter, stream_block, m, c);
}

static inline int bootutil_aes_ctr_decrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *c, uint32_t clen, size_t blk_off, uint8_t *m)
{
    uint8_t stream_block[BOOT_ENC_BLOCK_SIZE];
    return mbedtls_aes_crypt_ctr(ctx, clen, &blk_off, counter, stream_block, c, m);
}
#endif /* MCUBOOT_USE_MBED_TLS */

#if defined(MCUBOOT_USE_TINYCRYPT)
typedef struct tc_aes_key_sched_struct bootutil_aes_ctr_context;
static inline void bootutil_aes_ctr_init(bootutil_aes_ctr_context *ctx)
{
    (void)ctx;
}

static inline void bootutil_aes_ctr_drop(bootutil_aes_ctr_context *ctx)
{
    (void)ctx;
}

static inline int bootutil_aes_ctr_set_key(bootutil_aes_ctr_context *ctx, const uint8_t *k)
{
    int rc;
    rc = tc_aes128_set_encrypt_key(ctx, k);
    if (rc != TC_CRYPTO_SUCCESS) {
        return -1;
    }
    return 0;
}

static int _bootutil_aes_ctr_crypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *in, uint32_t inlen, uint32_t blk_off, uint8_t *out)
{
    int rc;
    rc = tc_ctr_mode(out, inlen, in, inlen, counter, &blk_off, ctx);
    if (rc != TC_CRYPTO_SUCCESS) {
        return -1;
    }
    return 0;
}

static inline int bootutil_aes_ctr_encrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *m, uint32_t mlen, uint32_t blk_off, uint8_t *c)
{
    return _bootutil_aes_ctr_crypt(ctx, counter, m, mlen, blk_off, c);
}

static inline int bootutil_aes_ctr_decrypt(bootutil_aes_ctr_context *ctx, uint8_t *counter, const uint8_t *c, uint32_t clen, uint32_t blk_off, uint8_t *m)
{
    return _bootutil_aes_ctr_crypt(ctx, counter, c, clen, blk_off, m);
}
#endif /* MCUBOOT_USE_TINYCRYPT */

#ifdef __cplusplus
}
#endif

#endif /* __BOOTUTIL_CRYPTO_AES_CTR_H_ */
