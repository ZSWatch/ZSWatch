/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_rsa_padding RSA Padding
 * @ingroup ocrypto_rsa
 * @{
 * @brief RSA padding for PKCS1 v1.5, OAEP and PSS.
 *
 * These functions support RSA PKCS1, OAEP, and PSS padding.
 */

/**
 * @file
 * @brief RSA padding for PKCS1 v1.5, OAEP and PSS.
 */

#ifndef OCRYPTO_RSA_PADDING_H
#define OCRYPTO_RSA_PADDING_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * RSA PKCS1 V1.5 padding.
 *
 * @param[out] em      The padded message.
 * @param      em_len  Length of @p em.
 * @param      m       The message to be padded.
 * @param      m_len   Length of @p m. 0 <= @p m_len <= @p em_len - 11.
 * @param      seed    The random seed to be used for the padding.
 * @param      s_len   Length of @p seed. @p s_len >= @p em_len - 3 - @p m_len
 *
 * @retval -1 If the message is too long (@p m_len > @p em_len - 11).
 * @retval -2 If the seed is too short (@p s_len < @p em_len - 3 - @p m_len).
 * @retval 0  On success.
 *
 * @remark The @p seed should consist of non-zero random bytes.
 */
int ocrypto_rsa_pkcs1_v15_padding(
    uint8_t *em, size_t em_len,
    const uint8_t *m, size_t m_len,
    const uint8_t *seed, size_t s_len);

/**
 * RSA PKCS1 V1.5 unpadding.
 *
 * @param[out] m       The unpadded message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      em      The padded message.
 * @param      em_len  Length of @p em.
 *
 * @retval -1 If decryption failed.
 * @retval -2 If the output buffer is too short (@p m_len < length of message).
 * @retval  n If a message of length n was successfully decrypted.
 */
int ocrypto_rsa_pkcs1_v15_check_padding(
    uint8_t *m, size_t m_len,
    const uint8_t *em, size_t em_len);


/**
* RSA OAEP SHA256 padding.
*
* @param[out] em        The padded message.
* @param      em_len    Length of @p em.
* @param      m         The message to be encrypted.
* @param      m_len     Length of @p m. 0 <= @p m_len <= @p em_len - 66.
* @param      label     The label associated with the message.
* @param      l_len     Length of @p label. May be 0.
* @param      seed      32-byte random seed.
*
* @retval -1 If the message is too long (@p m_len > @p em_len - 66).
* @retval 0  On success.
*/
int ocrypto_rsa_oaep_sha256_padding(
    uint8_t *em, size_t em_len,
    const uint8_t *m, size_t m_len,
    const uint8_t *label, size_t l_len,
    const uint8_t seed[32]);

/**
 * RSA OAEP SHA256 unpadding.
 *
 * @param[out] m       The unpadded message. The buffer must be long enough to hold the message.
 * @param      m_len   Length of @p m.
 * @param      em      The padded message.
 * @param      em_len  Length of @p em.
 * @param      label   The label associated with the message.
 * @param      l_len   Length of @p label. May be 0.
 *
 * @retval -1 If decryption failed.
 * @retval -2 If the output buffer is too short (@p m_len < length of message).
 * @retval  n If a message of length n was successfully decrypted.
 */
int ocrypto_rsa_oaep_sha256_check_padding(
    uint8_t *m, size_t m_len,
    uint8_t *em, size_t em_len,
    const uint8_t *label, size_t l_len);


/**
 * RSA PKCS1 V1.5 SHA256 signature encoding.
 *
 * @param[out] em      The encoded message.
 * @param      em_len  Length of @p em.
 * @param      m       The message to be encoded.
 * @param      m_len   Length of @p m.
 *
 * @retval -2 If the length of the encoded message is too short (@p em_len < 62).
 * @retval 0  On success.
 */
int ocrypto_rsa_pkcs1_v15_sha256_encode(
    uint8_t *em, size_t em_len,
    const uint8_t *m, size_t m_len);


/**
* RSA PSS SHA256 signature encoding.
*
* @param[out] em      The encoded message.
* @param      em_len  Length of @p em.
* @param      m       The message to be encoded.
* @param      m_len   Length of @p m.
* @param      salt    The salt to be used.
* @param      s_len   Length of @p salt.
*
* @retval -2  If the salt is too long (@p s_len > @p em_len - 34).
* @retval 0   On success.
*/
int ocrypto_rsa_pss_sha256_encode(
    uint8_t *em, size_t em_len,
    const uint8_t *m, size_t m_len,
    const uint8_t *salt, size_t s_len);

/**
* RSA PSS SHA256 signature decoding.
*
* @param em       The encoded message.
* @param em_len   Length of @p em.
* @param m        The signed message.
* @param m_len    Length of @p m.
* @param salt_len The length of the salt.
*
* @retval 0   If the encoding is valid.
* @retval -1  If decoding failed.
* @retval -2  If the salt is too long (@p salt_len > @p em_len - 34).
*/
int ocrypto_rsa_pss_sha256_decode(
    uint8_t *em, size_t em_len,
    const uint8_t *m, size_t m_len,
    size_t salt_len);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
