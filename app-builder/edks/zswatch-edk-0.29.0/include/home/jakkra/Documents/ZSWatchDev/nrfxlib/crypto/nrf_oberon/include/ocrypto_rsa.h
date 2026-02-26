/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_rsa_api RSA Operations
 * @ingroup ocrypto_rsa_orig
 * @{
 * @brief RSA encryption/decryption and sign/verify algorithms, for 1024 and 2048 bit RSA keys.
 *
 * These functions support RSA encryption and signatures with 1024 and 2048 bit
 * modulo, PKCS1 V1.5, OAEP and PSS padding.
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
 * @brief RSA encryption/decryption and sign/verify algorithms, for 1024 and 2048 bit RSA keys.
 */

#ifndef OCRYPTO_RSA_H
#define OCRYPTO_RSA_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_rsa_key.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name 1024-bit RSA Functions
 *
 * This group of functions is used for 1024-bit RSA.
 */
/**@{*/
/**
 * 1024 bit RSA PKCS1 V1.5 encryption.
 *
 * The message @p m is encrypted to a ciphertext returned in @p c.
 *
 * @param[out] c         The generated 128-byte ciphertext.
 * @param      m         The message to be encrypted.
 * @param      m_len     Length of @p m. 0 <= m_len <= 117.
 * @param      seed      The random seed to be used for the padding.
 * @param      s_len     Length of @p seed. @p s_len >= 125 - @p m_len
 * @param      pk        A valid 1024-bit RSA public key.
 *
 * @retval -1 If the message is too long (m_len > 117).
 * @retval -2 If the seed is too short (s_len < 125 - m_len).
 * @retval 0  On success.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa1024_init_pub_key.
 * @remark The @p seed should consist of non-zero random bytes.
 * @remark @p c may be same as @p m.
 */
int ocrypto_rsa1024_pkcs1_v15_encrypt(
    uint8_t c[128],
    const uint8_t *m, size_t m_len,
    const uint8_t *seed, size_t s_len,
    const ocrypto_rsa1024_pub_key *pk);

/**
 * 1024-bit RSA PKCS1 V1.5 decryption.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      c       The 128-byte ciphertext to decrypt.
 * @param      sk      A valid 1024-bit RSA secret key.
 *
 * @retval -1 If decryption failed.
 * @retval -2 If the output buffer is too short (m_len < length of message).
 * @retval  n If a message of length n was successfully decrypted.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa1024_init_key.
 * @remark @p m may be same as @p c.
 */
int ocrypto_rsa1024_pkcs1_v15_decrypt(
    uint8_t *m, size_t m_len,
    const uint8_t c[128],
    const ocrypto_rsa1024_key *sk);

/**
 * 1024-bit RSA PKCS1 V1.5 decryption with CRT acceleration.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      c       The 128-byte ciphertext to decrypt.
 * @param      sk      A valid 1024-bit RSA secret key with CRT coefficients.
 *
 * @retval -1  If decryption failed.
 * @retval -2  If the output buffer is too short (m_len < length of message).
 * @retval  n  If a message of length n was successfully decrypted.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa1024_init_crt_key.
 * @remark @p m may be same as @p c.
 */
int ocrypto_rsa1024_pkcs1_v15_crt_decrypt(
    uint8_t *m, size_t m_len,
    const uint8_t c[128],
    const ocrypto_rsa1024_crt_key *sk);

/**
 * 1024-bit RSA OAEP SHA256 encryption.
 *
 * The message @p m is encrypted to a ciphertext returned in @p c.
 *
 * @param[out] c         The generated 128-byte ciphertext.
 * @param      m         The message to be encrypted.
 * @param      m_len     Length of @p m. 0 <= m_len <= 62.
 * @param      label     The label associated with the message.
 * @param      l_len     Length of @p label. May be 0.
 * @param      seed      32-byte random seed.
 * @param      pk        A valid 1024-bit RSA public key.
 *
 * @retval -1 If the message is too long (m_len > 62).
 * @retval 0  On success.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa1024_init_pub_key.
 * @remark @p c may be same as @p m.
 */
int ocrypto_rsa1024_oaep_sha256_encrypt(
    uint8_t c[128],
    const uint8_t *m, size_t m_len,
    const uint8_t *label, size_t l_len,
    const uint8_t seed[32],
    const ocrypto_rsa1024_pub_key *pk);

/**
 * 1024 bit RSA OAEP SHA256 decryption.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      c       The 128 byte ciphertext to decrypt.
 * @param      label   The label associated with the message.
 * @param      l_len   Length of @p label. May be 0.
 * @param      sk      A valid 1024 bit RSA secret key.
 *
 * @retval -1 If decryption failed.
 * @retval -2 If the output buffer is too short (m_len < length of message).
 * @retval  n If a message of length n was successfully decrypted.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa1024_init_key.
 * @remark @p m may be same as @p c.
 */
int ocrypto_rsa1024_oaep_sha256_decrypt(
    uint8_t *m, size_t m_len,
    const uint8_t c[128],
    const uint8_t *label, size_t l_len,
    const ocrypto_rsa1024_key *sk);

/**
 * 1024-bit RSA OAEP SHA256 decryption with CRT acceleration.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      c       The 128-byte ciphertext to decrypt.
 * @param      label   The label associated with the message.
 * @param      l_len   Length of @p label. May be 0.
 * @param      sk      A valid 1024-bit RSA secret key with CRT coefficients.
 *
 * @retval -1  If decryption failed.
 * @retval -2  If the output buffer is too short (m_len < length of message).
 * @retval  n  If a message of length n was successfully decrypted.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa1024_init_crt_key.
 * @remark @p m may be same as @p c.
 */
int ocrypto_rsa1024_oaep_sha256_crt_decrypt(
    uint8_t *m, size_t m_len,
    const uint8_t c[128],
    const uint8_t *label, size_t l_len,
    const ocrypto_rsa1024_crt_key *sk);

/**
 * 1024-bit RSA PKCS1 V1.5 SHA256 sign.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated 128-byte signature.
 * @param      m       The message to be signed.
 * @param      m_len   Length of @p m.
 * @param      sk      A valid 1024-bit RSA secret key.
 *
 * @retval 0
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa1024_init_key.
 * @remark @p s may be same as @p m.
 */
int ocrypto_rsa1024_pkcs1_v15_sha256_sign(
    uint8_t s[128],
    const uint8_t *m, size_t m_len,
    const ocrypto_rsa1024_key *sk);

/**
 * 1024-bit RSA PKCS1 V1.5 SHA256 sign with CRT acceleration.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated 128-byte signature.
 * @param      m       The message to be signed.
 * @param      m_len   Length of @p m.
 * @param      sk      A valid 1024-bit RSA secret key with CRT coefficients.
 *
 * @retval 0
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa1024_init_crt_key.
 * @remark @p s may be same as @p m.
 */
int ocrypto_rsa1024_pkcs1_v15_sha256_crt_sign(
    uint8_t s[128],
    const uint8_t *m, size_t m_len,
    const ocrypto_rsa1024_crt_key *sk);

/**
 * 1024-bit RSA PKCS1 V1.5 SHA256 signature verify.
 *
 * The signature @p s of the input message @p m is verified.
 *
 * @param s      The 128-byte signature.
 * @param m      The signed message.
 * @param m_len  Length of @p m.
 * @param pk     A valid 1024-bit RSA public key.
 *
 * @retval 0  If the signature is valid.
 * @retval -1 If verification failed.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa1024_init_pub_key.
 */
int ocrypto_rsa1024_pkcs1_v15_sha256_verify(
    const uint8_t s[128],
    const uint8_t *m, size_t m_len,
    const ocrypto_rsa1024_pub_key *pk);

/**
 * 1024-bit RSA PSS SHA256 sign.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated 128-byte signature.
 * @param      m       The message to be signed.
 * @param      m_len   Length of @p m.
 * @param      salt    The salt to be used.
 * @param      s_len   Length of @p salt.
 * @param      sk      A valid 1024-bit RSA secret key.
 *
 * @retval -2  If the salt is too long.
 * @retval 0   On success.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa1024_init_key.
 * @remark @p s may be same as @p m.
 */
int ocrypto_rsa1024_pss_sha256_sign(
    uint8_t s[128],
    const uint8_t *m, size_t m_len,
    const uint8_t *salt, size_t s_len,
    const ocrypto_rsa1024_key *sk);

/**
 * 1024-bit RSA PSS SHA256 sign with CRT acceleration.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated 128-byte signature.
 * @param      m       The message to be signed.
 * @param      m_len   Length of @p m.
 * @param      salt    The salt to be used.
 * @param      s_len   Length of @p salt.
 * @param      sk      A valid 1024-bit RSA secret key with CRT coefficients.
 *
 * @retval -2  If the salt is too long.
 * @retval 0   On success.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa1024_init_crt_key.
 * @remark @p s may be same as @p m.
 */
int ocrypto_rsa1024_pss_sha256_crt_sign(
    uint8_t s[128],
    const uint8_t *m, size_t m_len,
    const uint8_t *salt, size_t s_len,
    const ocrypto_rsa1024_crt_key *sk);

/**
 * 1024-bit RSA PSS SHA256 signature verify.
 *
 * The signature @p s of the input message @p m is verified.
 *
 * @param s      The 128-byte signature.
 * @param m      The signed message.
 * @param m_len  Length of @p m.
 * @param s_len  The length of the salt.
 * @param pk     A valid 1024-bit RSA public key.
 *
 * @retval 0   If the signature is valid.
 * @retval -1  If verification failed.
 * @retval -2  If the salt is too long.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa1024_init_pub_key.
 */
int ocrypto_rsa1024_pss_sha256_verify(
    const uint8_t s[128],
    const uint8_t *m, size_t m_len,
    size_t s_len,
    const ocrypto_rsa1024_pub_key *pk);
/**@}*/


/**
 * @name 2048-bit RSA Functions
 *
 * This group of functions is used for 2048-bit RSA.
 */
/**@{*/
/**
 * 2048-bit RSA PKCS1 V1.5 encryption.
 *
 * The message @p m is encrypted to a ciphertext returned in @p c.
 *
 * @param[out] c      The generated 256-byte ciphertext.
 * @param      m      The message to be encrypted.
 * @param      mlen   Length of @p m. 0 <= @p mlen <= 245.
 * @param      seed   The random seed to be used for the padding.
 * @param      slen   Length of @p seed. @p slen >= 253 - @p mlen.
 * @param      pk     A valid 2048-bit RSA public key.
 *
 * @retval -1 If the message is too long (mlen > 245).
 * @retval -2 If the seed is too short (slen < 253 - mlen).
 * @retval 0  On success.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa2048_init_pub_key.
 * @remark The @p seed should consist of non-zero random bytes.
 * @remark @p c may be same as @p m.
 */
int ocrypto_rsa2048_pkcs1_v15_encrypt(
    uint8_t c[256],
    const uint8_t *m, size_t mlen,
    const uint8_t *seed, size_t slen,
    const ocrypto_rsa2048_pub_key *pk);

/**
 * 2048-bit RSA PKCS1 V1.5 decryption.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      mlen    Length of @p m.
 * @param      c       The 256-byte ciphertext to decrypt.
 * @param      sk      A valid 2048-bit RSA secret key.

 * @retval -1 If decryption failed.
 * @retval -2 If the output buffer is too short (mlen < length of message).
 * @retval n  If a message of length n was successfully decrypted.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa2048_init_key.
 * @remark @p m may be same as @p c.
 */
int ocrypto_rsa2048_pkcs1_v15_decrypt(
    uint8_t *m, size_t mlen,
    const uint8_t c[256],
    const ocrypto_rsa2048_key *sk);

/**
 * 2048-bit RSA PKCS1 V1.5 decryption with CRT acceleration.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      mlen    Length of @p m.
 * @param      c       The 256-byte ciphertext to decrypt.
 * @param      sk      A valid 2048-bit RSA secret key with CRT coefficients.

 * @retval -1  If decryption failed.
 * @retval -2  If the output buffer is too short (mlen < length of message).
 * @retval  n  If a message of length n was successfully decrypted.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa2048_init_crt_key.
 * @remark @p m may be same as @p c.
 */
int ocrypto_rsa2048_pkcs1_v15_crt_decrypt(
    uint8_t *m, size_t mlen,
    const uint8_t c[256],
    const ocrypto_rsa2048_crt_key *sk);

/**
 * 2048-bit RSA OAEP SHA256 encryption.
 *
 * The message @p m is encrypted to a ciphertext returned in @p c.
 *
 * @param[out] c         The generated 256-byte ciphertext.
 * @param      m         The message to be encrypted.
 * @param      mlen      Length of @p m. 0 <= mlen <= 190.
 * @param      label     The label associated with the message.
 * @param      llen      Length of @p label. May be 0.
 * @param      seed      32-byte random seed.
 * @param      pk        A valid 2048-bit RSA public key.
 *
 * @retval -1 If the message is too long (mlen > 190).
 * @retval 0  On success.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa2048_init_pub_key.
 * @remark @p c may be same as @p m.
 */
int ocrypto_rsa2048_oaep_sha256_encrypt(
    uint8_t c[256],
    const uint8_t *m, size_t mlen,
    const uint8_t *label, size_t llen,
    const uint8_t seed[32],
    const ocrypto_rsa2048_pub_key *pk);

/**
 * 2048-bit RSA OAEP SHA256 decryption.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      mlen    Length of @p m.
 * @param      c       The 256-byte ciphertext to decrypt.
 * @param      label   The label associated with the message.
 * @param      llen    Length of @p label. May be 0.
 * @param      sk      A valid 2048-bit RSA secret key.
 *
 * @retval -1 If decryption failed.
 * @retval -2 If the output buffer is too short (mlen < length of message).
 * @retval  n If a message of length n was successfully decrypted.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa2048_init_key.
 * @remark @p m may be same as @p c.
 */
int ocrypto_rsa2048_oaep_sha256_decrypt(
    uint8_t *m, size_t mlen,
    const uint8_t c[256],
    const uint8_t *label, size_t llen,
    const ocrypto_rsa2048_key *sk);

/**
 * 2048-bit RSA OAEP SHA256 decryption with CRT acceleration.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      mlen    Length of @p m.
 * @param      c       The 256-byte ciphertext to decrypt.
 * @param      label   The label associated with the message.
 * @param      llen    Length of @p label. May be 0.
 * @param      sk      A valid 2048-bit RSA secret key with CRT coefficients.
 *
 * @retval -1  If decryption failed.
 * @retval -2  If the output buffer is too short (mlen < length of message).
 * @retval  n  If a message of length n was successfully decrypted.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa2048_init_crt_key.
 * @remark @p m may be same as @p c.
 */
int ocrypto_rsa2048_oaep_sha256_crt_decrypt(
    uint8_t *m, size_t mlen,
    const uint8_t c[256],
    const uint8_t *label, size_t llen,
    const ocrypto_rsa2048_crt_key *sk);

/**
 * 2048-bit RSA PKCS1 V1.5 SHA256 sign.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated 256-byte signature.
 * @param      m       The message to be signed.
 * @param      mlen    Length of @p m.
 * @param      sk      A valid 2048-bit RSA secret key.

 * @retval 0
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa2048_init_key.
 * @remark @p s may be same as @p m.
 */
int ocrypto_rsa2048_pkcs1_v15_sha256_sign(
    uint8_t s[256],
    const uint8_t *m, size_t mlen,
    const ocrypto_rsa2048_key *sk);

/**
 * 2048-bit RSA PKCS1 V1.5 SHA256 sign with CRT acceleration.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated 256-byte signature.
 * @param      m       The message to be signed.
 * @param      mlen    Length of @p m.
 * @param      sk      A valid 2048-bit RSA secret key with CRT coefficients.

 * @retval 0
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa2048_init_crt_key.
 * @remark @p s may be same as @p m.
 */
int ocrypto_rsa2048_pkcs1_v15_sha256_crt_sign(
    uint8_t s[256],
    const uint8_t *m, size_t mlen,
    const ocrypto_rsa2048_crt_key *sk);

/**
 * 2048-bit RSA PKCS1 V1.5 SHA256 signature verify.
 *
 * The signature @p s of the input message @p m is verified.
 *
 * @param s      The 256-byte signature.
 * @param m      The signed message.
 * @param mlen   Length of @p m.
 * @param pk     A valid 2048-bit RSA public key.
 *
 * @retval 0  If the signature is valid.
 * @retval -1 If verification failed.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa2048_init_pub_key.
 */
int ocrypto_rsa2048_pkcs1_v15_sha256_verify(
    const uint8_t s[256],
    const uint8_t *m, size_t mlen,
    const ocrypto_rsa2048_pub_key *pk);

/**
 * 2048-bit RSA PSS SHA256 sign.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated 256-byte signature.
 * @param      m       The message to be signed.
 * @param      mlen    Length of @p m.
 * @param      salt    The salt to be used.
 * @param      slen    Length of @p salt.
 * @param      sk      A valid 2048-bit RSA secret key.
 *
 * @retval -2  If the salt is too long.
 * @retval 0   On success.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa2048_init_key.
 * @remark @p s may be same as @p m.
 */
int ocrypto_rsa2048_pss_sha256_sign(
    uint8_t s[256],
    const uint8_t *m, size_t mlen,
    const uint8_t *salt, size_t slen,
    const ocrypto_rsa2048_key *sk);

/**
 * 2048-bit RSA PSS SHA256 sign with CRT acceleration.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated 256-byte signature.
 * @param      m       The message to be signed.
 * @param      mlen    Length of @p m.
 * @param      salt    The salt to be used.
 * @param      slen    Length of @p salt.
 * @param      sk      A valid 2048-bit RSA secret key with CRT coefficients.
 *
 * @retval -2  If the salt is too long.
 * @retval 0   On success.
 *
 * @remark The key @p sk should be initialized with @c ocrypto_rsa2048_init_crt_key.
 * @remark @p s may be same as @p m.
 */
int ocrypto_rsa2048_pss_sha256_crt_sign(
    uint8_t s[256],
    const uint8_t *m, size_t mlen,
    const uint8_t *salt, size_t slen,
    const ocrypto_rsa2048_crt_key *sk);

/**
 * 2048-bit RSA PSS SHA256 signature verify.
 *
 * The signature @p s of the input message @p m is verified.
 *
 * @param s      The 256-byte signature.
 * @param m      The signed message.
 * @param mlen   Length of @p m.
 * @param slen   The length of the salt.
 * @param pk     A valid 2048-bit RSA public key.
 *
 * @retval 0   If the signature is valid.
 * @retval -1  If verification failed.
 * @retval -2  If the salt is too long.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa2048_init_pub_key.
 */
int ocrypto_rsa2048_pss_sha256_verify(
    const uint8_t s[256],
    const uint8_t *m, size_t mlen,
    size_t slen, // salt length
    const ocrypto_rsa2048_pub_key *pk);
/**@}*/


#ifdef __cplusplus
}
#endif

#endif

/** @} */
