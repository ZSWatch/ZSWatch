/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_rsa_operations RSA Operations
 * @ingroup ocrypto_rsa
 * @{
 * @brief RSA encryption/decryption and sign/verify algorithms.
 *
 * These functions support RSA encryption and signatures with PKCS1 v1.5, OAEP and PSS padding.
 *
 * Supported key sizes (in bits) are: 1024, 2048, 3072, 4096, ...
 * 
 * @remark 1024 bit keys are no longer considered secure;
 *         use of at least 2048 bit keys is recommended.
 *
 * @see [RFC - PKCS #1: RSA Cryptography Specifications Version 2.2](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
 */

/**
 * @file
 * @brief RSA encryption/decryption and sign/verify algorithms.
 */

#ifndef OCRYPTO_RSA_OPERATIONS_H
#define OCRYPTO_RSA_OPERATIONS_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_rsa_primitives.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name RSA operations
 *
 * This group of functions is used for RSA with padding.
 */
/**@{*/
/**
 * RSA PKCS1 V1.5 encryption.
 *
 * The message @p m is encrypted to a ciphertext returned in @p c.
 *
 * @param[out] c         The generated ciphertext.
 * @param      c_len     Length of @p c.
 * @param      m         The message to be encrypted.
 * @param      m_len     Length of @p m. 0 <= m_len <= key-size - 11.
 * @param      seed      The random seed to be used for the padding.
 * @param      s_len     Length of @p seed. @p s_len >= key-size - 3 - @p m_len
 * @param      pk        A valid RSA public key.
 * @param      mem       The working memory. Length OCRYPTO_RSA_PUB_MEM_SIZE(bits).
 *
 * @retval -1 If the message is too long (m_len > key-size - 11).
 * @retval -2 If the seed is too short (s_len < key-size - 3 - m_len).
 * @retval 0  On success.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa_init_pub_key.
 * @remark The @p seed should consist of non-zero random bytes.
 * @remark @p c may be same as @p m or @p mem.
 */
int ocrypto_rsa_pkcs1_v15_encrypt(
    uint8_t *c, size_t c_len,
    const uint8_t *m, size_t m_len,
    const uint8_t *seed, size_t s_len,
    const ocrypto_rsa_pub_key *pk,
    uint32_t *mem);

/**
 * RSA PKCS1 V1.5 decryption.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      c       The ciphertext to decrypt.
 * @param      c_len   Length of @p c.
 * @param      key     A valid RSA secret key.
 * @param      mem     The working memory. Length OCRYPTO_RSA_MEM_SIZE(bits).
 *
 * @retval -1 If decryption failed.
 * @retval -2 If the output buffer is too short (m_len < length of message).
 * @retval  n If a message of length n was successfully decrypted.
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_key.
 * @remark @p c, @p m, and @p mem may be same.
 */
int ocrypto_rsa_pkcs1_v15_decrypt(
    uint8_t *m, size_t m_len,
    const uint8_t *c, size_t c_len,
    const ocrypto_rsa_key *key,
    uint32_t *mem);

/**
 * RSA PKCS1 V1.5 decryption with CRT acceleration.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      c       The ciphertext to decrypt.
 * @param      c_len   Length of @p c.
 * @param      key     A valid RSA secret key with CRT coefficients.
 * @param      mem     The working memory. Length OCRYPTO_RSA_CRT_MEM_SIZE(bits).
 *
 * @retval -1  If decryption failed.
 * @retval -2  If the output buffer is too short (m_len < length of message).
 * @retval  n  If a message of length n was successfully decrypted.
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_crt_key.
 * @remark @p c, @p m, and @p mem may be same.
 */
int ocrypto_rsa_pkcs1_v15_crt_decrypt(
    uint8_t *m, size_t m_len,
    const uint8_t *c, size_t c_len,
    const ocrypto_rsa_crt_key *key,
    uint32_t *mem);

/**
 * RSA OAEP SHA256 encryption.
 *
 * The message @p m is encrypted to a ciphertext returned in @p c.
 *
 * @param[out] c         The generated 128-byte ciphertext.
 * @param      c_len     Length of @p c.
 * @param      m         The message to be encrypted.
 * @param      m_len     Length of @p m. 0 <= m_len <= key-size - 66.
 * @param      label     The label associated with the message.
 * @param      l_len     Length of @p label. May be 0.
 * @param      seed      32-byte random seed.
 * @param      pk        A valid RSA public key.
 * @param      mem       The working memory. Length OCRYPTO_RSA_PUB_MEM_SIZE(bits).
 *
 * @retval -1 If the message is too long (m_len > key-size - 66).
 * @retval 0  On success.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa_init_pub_key.
 * @remark @p c, @p m, and @p mem may be same.
 */
int ocrypto_rsa_oaep_sha256_encrypt(
    uint8_t *c, size_t c_len,
    const uint8_t *m, size_t m_len,
    const uint8_t *label, size_t l_len,
    const uint8_t seed[32],
    const ocrypto_rsa_pub_key *pk,
    uint32_t *mem);

/**
 * RSA OAEP SHA256 decryption.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      c       The ciphertext to decrypt.
 * @param      c_len   Length of @p c.
 * @param      label   The label associated with the message.
 * @param      l_len   Length of @p label. May be 0.
 * @param      key     A valid RSA secret key.
 * @param      mem     The working memory. Length OCRYPTO_RSA_MEM_SIZE(bits).
 *
 * @retval -1 If decryption failed.
 * @retval -2 If the output buffer is too short (m_len < length of message).
 * @retval  n If a message of length n was successfully decrypted.
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_key.
 * @remark @p c, @p m, and @p mem may be same.
 */
int ocrypto_rsa_oaep_sha256_decrypt(
    uint8_t *m, size_t m_len,
    const uint8_t *c, size_t c_len,
    const uint8_t *label, size_t l_len,
    const ocrypto_rsa_key *key,
    uint32_t *mem);

/**
 * RSA OAEP SHA256 decryption with CRT acceleration.
 *
 * The ciphertext @p c is decrypted to the message returned in @p m.
 *
 * @param[out] m       The decrypted message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      c       The ciphertext to decrypt.
 * @param      c_len   Length of @p c.
 * @param      label   The label associated with the message.
 * @param      l_len   Length of @p label. May be 0.
 * @param      key     A valid RSA secret key with CRT coefficients.
 * @param      mem     The working memory. Length OCRYPTO_RSA_CRT_MEM_SIZE(bits).
 *
 * @retval -1  If decryption failed.
 * @retval -2  If the output buffer is too short (m_len < length of message).
 * @retval  n  If a message of length n was successfully decrypted.
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_crt_key.
 * @remark @p c, @p m, and @p mem may be same.
 */
int ocrypto_rsa_oaep_sha256_crt_decrypt(
    uint8_t *m, size_t m_len,
    const uint8_t *c, size_t c_len,
    const uint8_t *label, size_t l_len,
    const ocrypto_rsa_crt_key *key,
    uint32_t *mem);

/**
 * RSA PKCS1 V1.5 SHA256 sign.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated signature.
 * @param      s_len   Length of @p s.
 * @param      m       The message to be signed.
 * @param      m_len   Length of @p m.
 * @param      key     A valid RSA secret key.
 * @param      mem     The working memory. Length OCRYPTO_RSA_MEM_SIZE(bits).
 *
 * @retval 0
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_key.
 * @remark @p s, @p m, and @p mem may be same.
 */
int ocrypto_rsa_pkcs1_v15_sha256_sign(
    uint8_t *s, size_t s_len,
    const uint8_t *m, size_t m_len,
    const ocrypto_rsa_key *key,
    uint32_t *mem);

/**
 * RSA PKCS1 V1.5 SHA256 sign with CRT acceleration.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s       The generated signature.
 * @param      s_len   Length of @p s.
 * @param      m       The message to be signed.
 * @param      m_len   Length of @p m.
 * @param      key     A valid RSA secret key with CRT coefficients.
 * @param      mem     The working memory. Length OCRYPTO_RSA_CRT_MEM_SIZE(bits).
 *
 * @retval 0
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_crt_key.
 * @remark @p s, @p m, and @p mem may be same.
 */
int ocrypto_rsa_pkcs1_v15_sha256_crt_sign(
    uint8_t *s, size_t s_len,
    const uint8_t *m, size_t m_len,
    const ocrypto_rsa_crt_key *key,
    uint32_t *mem);

/**
 * RSA PKCS1 V1.5 SHA256 signature verify.
 *
 * The signature @p s of the input message @p m is verified.
 *
 * @param s      The signature.
 * @param s_len  Length of @p s.
 * @param m      The signed message.
 * @param m_len  Length of @p m.
 * @param pk     A valid RSA public key.
 * @param mem    The working memory. Length OCRYPTO_RSA_PUB_MEM_SIZE(bits).
 *
 * @retval 0  If the signature is valid.
 * @retval -1 If verification failed.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa_init_pub_key.
 * @remark @p mem may be same as @p s or @p m.
 */
int ocrypto_rsa_pkcs1_v15_sha256_verify(
    const uint8_t *s, size_t s_len,
    const uint8_t *m, size_t m_len,
    const ocrypto_rsa_pub_key *pk,
    uint32_t *mem);

/**
 * RSA PSS SHA256 sign.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s        The generated signature.
 * @param      s_len    Length of @p s.
 * @param      m        The message to be signed.
 * @param      m_len    Length of @p m.
 * @param      salt     The salt to be used.
 * @param      salt_len Length of @p salt.
 * @param      key      A valid RSA secret key.
 * @param      mem      The working memory. Length OCRYPTO_RSA_MEM_SIZE(bits).
 *
 * @retval -2  If the salt is too long.
 * @retval 0   On success.
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_key.
 * @remark @p s, @p m, and @p mem may be same.
 */
int ocrypto_rsa_pss_sha256_sign(
    uint8_t *s, size_t s_len,
    const uint8_t *m, size_t m_len,
    const uint8_t *salt, size_t salt_len,
    const ocrypto_rsa_key *key,
    uint32_t *mem);

/**
 * RSA PSS SHA256 sign with CRT acceleration.
 *
 * The message @p m is signed and the signature returned in @p s.
 *
 * @param[out] s        The generated signature.
 * @param      s_len    Length of @p m.
 * @param      m        The message to be signed.
 * @param      m_len    Length of @p m.
 * @param      salt     The salt to be used.
 * @param      salt_len Length of @p salt.
 * @param      key      A valid RSA secret key with CRT coefficients.
 * @param      mem      The working memory. Length OCRYPTO_RSA_CRT_MEM_SIZE(bits).
 *
 * @retval -2  If the salt is too long.
 * @retval 0   On success.
 *
 * @remark The key @p key should be initialized with @c ocrypto_rsa_init_crt_key.
 * @remark @p s, @p m, and @p mem may be same.
 */
int ocrypto_rsa_pss_sha256_crt_sign(
    uint8_t *s, size_t s_len,
    const uint8_t *m, size_t m_len,
    const uint8_t *salt, size_t salt_len,
    const ocrypto_rsa_crt_key *key,
    uint32_t *mem);

/**
 * RSA PSS SHA256 signature verify.
 *
 * The signature @p s of the input message @p m is verified.
 *
 * @param s        The signature.
 * @param s_len    Length of @p s.
 * @param m        The signed message.
 * @param m_len    Length of @p m.
 * @param salt_len The length of the salt.
 * @param pk       A valid RSA public key.
 * @param mem      The working memory. Length OCRYPTO_RSA_PUB_MEM_SIZE(bits).
 *
 * @retval 0   If the signature is valid.
 * @retval -1  If verification failed.
 * @retval -2  If the salt is too long.
 *
 * @remark The key @p pk should be initialized with @c ocrypto_rsa_init_pub_key.
 * @remark @p mem may be same as @p s or @p m.
 */
int ocrypto_rsa_pss_sha256_verify(
    const uint8_t *s, size_t s_len,
    const uint8_t *m, size_t m_len,
    size_t salt_len,
    const ocrypto_rsa_pub_key *pk,
    uint32_t *mem);
/**@}*/


#ifdef __cplusplus
}
#endif

#endif

/** @} */
