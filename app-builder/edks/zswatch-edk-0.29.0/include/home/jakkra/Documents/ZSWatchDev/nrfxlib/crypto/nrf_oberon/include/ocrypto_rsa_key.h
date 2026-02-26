/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_rsa_key RSA Keys
 * @ingroup ocrypto_rsa_orig
 * @{
 * @brief RSA key setup, for 1024 and 2048 bit keys.
 *
 * These functions support the setup of 1024 and 2048 RSA secret and public keys.
 * 
 * @remark 1024 bit keys are no longer considered secure;
 *         use of at least 2048 bit keys is recommended.
 * 
 * @remark This is the original ocrypto API for RSA. See the newer API that
 *         supports additional key sizes.
 *
 * @see [RFC - PKCS #1: RSA Cryptography Specifications Version 2.2](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
 */

/**
 * @file
 * @brief RSA key setup, for 1024 and 2048 bit keys.
 */

#ifndef OCRYPTO_RSA_KEY_H
#define OCRYPTO_RSA_KEY_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * The Public RSA Exponent.
 */
#define ocrypto_rsa_PUBLIC_EXPONENT 65537  // 2^16 + 1


/**
 * @name 1024-bit RSA Keys
 *
 * This group of keys is used for 1024-bit RSA.
 */
/**@{*/
/**
 * 1024-bit RSA public key.
 */
typedef struct {
    /**@cond */
    uint32_t n[32];
    // e = 65537
    /**@endcond */
} ocrypto_rsa1024_pub_key;

/**
 * 1024 bit RSA secret key.
 */
typedef struct {
    /**@cond */
    uint32_t n[32];
    uint32_t d[32];  // x^(e*d) mod n == x
    /**@endcond */
} ocrypto_rsa1024_key;

/**
 * 1024-bit RSA secret key with CRT coefficients.
 */
typedef struct {
    /**@cond */
    uint32_t n[32];
    uint32_t p[16], q[16];   // primes, p*q = n
    uint32_t dp[16], dq[16]; // d mod (p-1), d mod (q-1)
    uint32_t qinv[16];       // 1/q mod p
    /**@endcond */
} ocrypto_rsa1024_crt_key;
/**@}*/


/**
 * @name 2048-bit RSA Keys
 *
 * This group of keys is used for 2048-bit RSA.
 */
/**@{*/
/**
 * 2048-bit RSA public key.
 */
typedef struct {
    /**@cond */
    uint32_t n[64];
    // e = 65537
    /**@endcond */
} ocrypto_rsa2048_pub_key;

/**
 * 2048-bit RSA secret key.
 */
typedef struct {
    /**@cond */
    uint32_t n[64];
    uint32_t d[64];  // x^(e*d) mod n == x
    /**@endcond */
} ocrypto_rsa2048_key;

/**
 * 2048-bit RSA secret key with CRT coefficients.
 */
typedef struct {
    /**@cond */
    uint32_t n[64];
    uint32_t p[32], q[32];   // primes, p*q = n
    uint32_t dp[32], dq[32]; // d mod (p-1), d mod (q-1)
    uint32_t qinv[32];       // 1/q mod p
    /**@endcond */
} ocrypto_rsa2048_crt_key;
/**@}*/

/**
 * @name 1024-bit RSA key setup
 *
 * This group of functions is used for 1024-bit RSA key setup.
 */
/**@{*/
/**
 * 1024-bit RSA public key setup.
 *
 * @param[out] pk      The initialized public key.
 * @param      n       The RSA modulus. Must be exactly 1024 bits.
 * @param      n_len    Length of @p n.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 *
 * @remark The public exponent is fixed at 65537.
 */
int ocrypto_rsa1024_init_pub_key(
    ocrypto_rsa1024_pub_key *pk,
    const uint8_t *n, size_t n_len);

/**
 * 1024-bit RSA secret key setup.
 *
 * @param[out] pk      The initialized public key.
 * @param      n       The RSA modulus. Must be exactly 1024 bits.
 * @param      n_len   Length of @p n.
 * @param      d       The secret exponent. Must be <= 1024 bits.
 * @param      d_len   Length of @p d.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 */
int ocrypto_rsa1024_init_key(
    ocrypto_rsa1024_key *pk,
    const uint8_t *n, size_t n_len,
    const uint8_t *d, size_t d_len);

/**
 * 1024-bit RSA secret key setup with CRT coefficients.
 *
 * @param[out] sk         The initialized secret key.
 * @param      p          The 1. RSA prime. Must be exactly 512 bits.
 * @param      p_len      Length of @p p.
 * @param      q          The 2. RSA prime. Must be exactly 512 bits.
 * @param      q_len      Length of @p q.
 * @param      dp         The 1. CRT exponent. dp = d mod (p-1).
 * @param      dp_len     Length of @p dp.
 * @param      dq         The 2. CRT exponent. dq = d mod (q-1).
 * @param      dq_len     Length of @p dq.
 * @param      qinv       The CRT coefficient. qinv = 1/q mod p.
 * @param      qi_len     Length of @p qinv.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 */
int ocrypto_rsa1024_init_crt_key(
    ocrypto_rsa1024_crt_key *sk,
    const uint8_t *p,    size_t p_len,
    const uint8_t *q,    size_t q_len,
    const uint8_t *dp,   size_t dp_len,
    const uint8_t *dq,   size_t dq_len,
    const uint8_t *qinv, size_t qi_len);
/**@}*/

/**
 * @name 2048-bit RSA key setup
 *
 * This group of functions is used for 2048-bit RSA key setup.
 */
/**@{*/
/**
 * 2048-bit RSA public key setup.
 *
 * @param[out] pk      The initialized public key.
 * @param      n       The RSA modulus. Must be exactly 2048 bits.
 * @param      n_len   Length of @p n.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 *
 * @remark The public exponent is fixed at 65537.
 */
int ocrypto_rsa2048_init_pub_key(
    ocrypto_rsa2048_pub_key *pk,
    const uint8_t *n, size_t n_len);

/**
 * 2048-bit RSA secret key setup.
 *
 * @param[out] sk      The initialized public key.
 * @param      n       The RSA modulus. Must be exactly 2048 bits.
 * @param      n_len   Length of @p n.
 * @param      d       The secret exponent. Must be <= 2048 bits.
 * @param      d_len   Length of @p d.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 */
int ocrypto_rsa2048_init_key(ocrypto_rsa2048_key *sk,
    const uint8_t *n, size_t n_len,
    const uint8_t *d, size_t d_len);

/**
 * 2048-bit RSA secret key setup with CRT coefficients.
 *
 * @param[out] sk         The initialized secret key.
 * @param      p          The 1. RSA prime. Must be exactly 1024 bits.
 * @param      p_len      Length of @p p.
 * @param      q          The 2. RSA prime. Must be exactly 1024 bits.
 * @param      q_len      Length of @p q.
 * @param      dp         The 1. CRT exponent. dp = d mod (p-1).
 * @param      dp_len     Length of @p dp.
 * @param      dq         The 2. CRT exponent. dq = d mod (q-1).
 * @param      dq_len     Length of @p dq.
 * @param      qinv       The CRT coefficient. qinv = 1/q mod p.
 * @param      qi_len     Length of @p qinv.
 *
 * @retval -1 If the input length is invalid.
 * @retval 0  On success.
 */
int ocrypto_rsa2048_init_crt_key(
    ocrypto_rsa2048_crt_key *sk,
    const uint8_t *p,    size_t p_len,
    const uint8_t *q,    size_t q_len,
    const uint8_t *dp,   size_t dp_len,
    const uint8_t *dq,   size_t dq_len,
    const uint8_t *qinv, size_t qi_len);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif

/** @} */
