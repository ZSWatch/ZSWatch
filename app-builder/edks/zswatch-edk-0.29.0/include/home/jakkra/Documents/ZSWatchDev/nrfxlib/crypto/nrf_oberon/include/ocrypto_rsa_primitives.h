/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_rsa_primitives RSA Primitives
 * @ingroup ocrypto_rsa
 * @{
 * @brief RSA primitives, including key setup.
 *
 * These functions support RSA key setup and primitives.
 *
 * Supported key sizes (in bits) are: size >= 1024, size = 2^n or 3 * 2^n.
 * This includes the popular sizes: 2048, 3072, 4096, ... bits.
 * 
 * @remark 1024 bit keys are no longer considered secure;
 *         use of at least 2048 bit keys is recommended.
 *
 * @see [RFC - PKCS #1: RSA Cryptography Specifications Version 2.2](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
 */

/**
 * @file
 * @brief RSA primitives, including key setup.
 */

#ifndef OCRYPTO_RSA_PRIMITIVES_H
#define OCRYPTO_RSA_PRIMITIVES_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Public key memory size in words.
 */
#define OCRYPTO_RSA_PUB_KEY_SIZE(bits) ((bits)/32)

/**
 * Secret key memory size in words.
 */
#define OCRYPTO_RSA_KEY_SIZE(bits) ((bits)/32*2)

/**
 * CRT key memory size in words.
 */
#define OCRYPTO_RSA_CRT_KEY_SIZE(bits) ((bits)/32*7/2)


/**
 * Public exponentiation scratch memory size in words.
 */
#define OCRYPTO_RSA_PUB_MEM_SIZE(bits) ((bits)/32*11)

/**
 * Secret exponentiation scratch memory size in words.
 */
#define OCRYPTO_RSA_MEM_SIZE(bits) ((bits)/32*26)

/**
 * CRT exponentiation scratch memory size in words.
 */
#define OCRYPTO_RSA_CRT_MEM_SIZE(bits) ((bits)/32*14)


/**@{*/
/**
 * RSA public key.
 */
typedef struct {
    /**@cond */
    uint32_t *n;
    uint32_t e;
    int blocks; // block size of n
    /**@endcond */
} ocrypto_rsa_pub_key;

/**
 * RSA secret key.
 */
typedef struct {
    /**@cond */
    uint32_t *n;
    uint32_t *d; // x^(e*d) mod n == x
    int blocks;  // block size of n
    /**@endcond */
} ocrypto_rsa_key;

/**
 * RSA secret key with CRT coefficients.
 */
typedef struct {
    /**@cond */
    uint32_t *n;
    uint32_t *p;    // primes, p*q = n
    uint32_t *q;
    uint32_t *dp;   // d mod (p-1)
    uint32_t *dq;   // d mod (q-1)
    uint32_t *qinv; // 1/q mod p
    int blocks;     // block size of n
    /**@endcond */
} ocrypto_rsa_crt_key;
/**@}*/


/**
 * @name variable-bit RSA key setup
 *
 * This group of functions is used for generic RSA key setup.
 */
/**@{*/
/**
 * RSA public key setup.
 *
 * @param[out] key     The initialized public key.
 * @param[out] key_mem Key memory. Length OCRYPTO_RSA_PUB_KEY_SIZE(bits).
 *                     Must have same lifetime as @p key.
 * @param      n       The RSA modulus, unsigned big-endian.
 * @param      n_len   Length of @p n.
 * @param      e       Public exponent.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 *
 * @remark The n value can be read directly from a DER encoded RSAPublicKey or RSAPrivateKey.
 */
int ocrypto_rsa_init_pub_key(
    ocrypto_rsa_pub_key *key,
    uint32_t *key_mem,
    const uint8_t *n, size_t n_len,
    uint32_t e);

/**
 * RSA secret key setup.
 *
 * @param[out] key     The initialized secret key.
 * @param[out] key_mem Key memory. Length OCRYPTO_RSA_KEY_SIZE(bits).
 *                     Must have same lifetime as @p key.
 * @param      n       The RSA modulus, unsigned big-endian.
 * @param      n_len   Length of @p n.
 * @param      d       The secret exponent, unsigned big-endian.
 * @param      d_len   Length of @p d. Must be <= @p n_len.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 *
 * @remark The n and d values can be read directly from the corresponding value
 *         parts of a DER encoded RSAPrivateKey.
 */
int ocrypto_rsa_init_key(
    ocrypto_rsa_key *key,
    uint32_t *key_mem,
    const uint8_t *n, size_t n_len,
    const uint8_t *d, size_t d_len);

/**
 * RSA secret key setup with CRT coefficients.
 *
 * @param[out] key     The initialized secret key.
 * @param[out] key_mem Key memory. Length OCRYPTO_RSA_CRT_KEY_SIZE(bits).
 *                     Must have same lifetime as @p key.
 * @param      p       The 1. RSA prime, unsigned big-endian.
 * @param      p_len   Length of @p p.
 * @param      q       The 2. RSA prime, unsigned big-endian.
 * @param      q_len   Length of @p q. Must be same as @p p_len.
 * @param      dp      The 1. CRT exponent, dp = d mod (p-1), unsigned big-endian.
 * @param      dp_len  Length of @p dp.
 * @param      dq      The 2. CRT exponent, dq = d mod (q-1), unsigned big-endian.
 * @param      dq_len  Length of @p dq.
 * @param      qinv    The CRT coefficient, qinv = 1/q mod p, unsigned big-endian.
 * @param      qi_len  Length of @p qinv.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 *
 * @remark The p, q, dp, dq, and qinv values can be read directly from the
 *         corresponding value parts of a DER encoded RSAPrivateKey.
 */
int ocrypto_rsa_init_crt_key(
    ocrypto_rsa_crt_key *key,
    uint32_t *key_mem,
    const uint8_t *p,    size_t p_len,
    const uint8_t *q,    size_t q_len,
    const uint8_t *dp,   size_t dp_len,
    const uint8_t *dq,   size_t dq_len,
    const uint8_t *qinv, size_t qi_len);
/**@}*/


/**
 * @name RSA primitives
 *
 * This group of functions is used for basic RSA arithmetic.
 */
/**@{*/
/**
 * RSA public key exponentiation.
 * c = m^e mod n
 *
 * @param[out] c         The generated ciphertext.
 * @param      c_len     Length of @p c.
 * @param      m         The message to be encrypted.
 * @param      m_len     Length of @p m.
 * @param      pk        A valid RSA public key.
 * @param      mem       The working memory. Length OCRYPTO_RSA_PUB_MEM_SIZE(bits).
 *
 * @retval -1 If the input is too large (m >= n).
 * @retval 0  On success.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa_init_pub_key.
 * @remark @p c, @p m, and @p mem may be same.
 */
int ocrypto_rsa_pub_exp(
    uint8_t *c, size_t c_len,
    const uint8_t *m, size_t m_len,
    const ocrypto_rsa_pub_key *pk,
    uint32_t *mem);

/**
 * RSA secret key exponentiation.
 * m = c^d mod n
 *
 * @param[out] m         The generated message.
 * @param      m_len     Length of @p m.
 * @param      c         The ciphertext to be decrypted.
 * @param      c_len     Length of @p c.
 * @param      key       A valid RSA secret key.
 * @param      mem       The working memory. Length OCRYPTO_RSA_MEM_SIZE(bits).
 *
 * @retval -1 If the input is too large (c >= n).
 * @retval 0  On success.
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_key.
 * @remark @p c, @p m, and @p mem may be same.
 */
int ocrypto_rsa_exp(
    uint8_t *m, size_t m_len,
    const uint8_t *c, size_t c_len,
    const ocrypto_rsa_key *key,
    uint32_t *mem);

/**
 * RSA CRT key exponentiation.
 * m = (c^dp mod p - c^dq mod q) * qinv mod p * q + c^dq mod q
 *
 * @param[out] m         The generated message.
 * @param      m_len     Length of @p m.
 * @param      c         The ciphertext to be decrypted.
 * @param      c_len     Length of @p c.
 * @param      key       A valid RSA secret key with CRT coefficients.
 * @param      mem       The working memory. Length OCRYPTO_RSA_CRT_MEM_SIZE(bits).
 *
 * @retval -1 If the input is too large (c >= n).
 * @retval 0  On success.
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_crt_key.
 * @remark @p c, @p m, and @p mem may be same.
 */
int ocrypto_rsa_crt_exp(
    uint8_t *m, size_t m_len,
    const uint8_t *c, size_t c_len,
    const ocrypto_rsa_crt_key *key,
    uint32_t *mem); 
/**@}*/


#ifdef __cplusplus
}
#endif

#endif

/** @} */
