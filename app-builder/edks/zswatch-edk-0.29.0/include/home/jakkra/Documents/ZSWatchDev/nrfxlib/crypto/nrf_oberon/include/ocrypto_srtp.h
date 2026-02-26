/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @file
 * @brief Secure Real-time Transport Protocol (SRTP) is an extension of the RTP
 *        protocol with an enhanced security mechanism.
 */

#ifndef OCRYPTO_SRTP_H
#define OCRYPTO_SRTP_H

#include <stddef.h>
#include <stdint.h>

#include "ocrypto_aes_key.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * SRTP Authentication Key Size.
 */
#define ocrypto_srtp_AUTH_KEY_SIZE  (20)

/**
 * SRTP Salt Size.
 */
#define ocrypto_srtp_SALT_SIZE      (14)

/**
 * SRTP Maximum Key Size.
 */
#define ocrypto_srtp_MAX_KEY_SIZE   (ocrypto_aes256_KEY_BYTES)

/**@cond
 * SRTP Context.
 */
typedef struct {
    /**
     * Key size [bytes].
     */
    uint32_t key_size;

    /**
     * Tag size [bytes].
     */
    uint32_t tag_size;

    /**
     * Session encryption key (max 256 bits).
     */
    uint8_t encr_key[ocrypto_srtp_MAX_KEY_SIZE];

    /**
     * Session authentication key
     * 160 bits.
     */
    uint8_t auth_key[ocrypto_srtp_AUTH_KEY_SIZE];

    /**
     * Session salt
     * 112 bits.
     */
    uint8_t salt_key[ocrypto_srtp_SALT_SIZE];
} ocrypto_srtp_ctx;
/**@endcond */

/**
 * Setup SRTP contexts.
 *
 * @param[out] srtp_ctx              SRTP context to be setup.
 * @param[out] srtcp_ctx             SRTCP context to be setup.
 * @param      key                   Master key.
 * @param      key_size              Size of the master key (16, 24, or 32 bytes).
 * @param      salt                  Master salt (@p ocrypto_srtp_SALT_SIZE bytes).
 * @param      tag_size              Size of the authentication tag.
 * @param      ssrc                  Synchronization source.
 */
void ocrypto_srtp_setup_ctx(
    ocrypto_srtp_ctx *srtp_ctx,
    ocrypto_srtp_ctx *srtcp_ctx,
    const uint8_t *key,
    uint32_t key_size,
    const uint8_t *salt,
    uint32_t tag_size,
    uint32_t ssrc);

/**
 * Encrypt SRTP packet.
 *
 * The final packet consists of @p num_header_bytes encrypted in place, followed
 * by @p num_data_bytes copied from @p data_bytes during encryption.
 *
 * @param         srtp_ctx             SRTP context.
 * @param[in,out] packet               Encrypted packet.
 * @param         data_bytes           Data bytes to be encrypted.
 * @param         num_header_bytes     Number of header bytes.
 * @param         num_data_bytes       Number of data bytes.
 * @param         index                Packet index.
 */
void ocrypto_srtp_encrypt(
    const ocrypto_srtp_ctx *srtp_ctx,
    uint8_t *packet,
    const uint8_t *data_bytes,
    size_t num_header_bytes,
    size_t num_data_bytes,
    uint32_t index);

/**
 * Decrypt SRTP packet.
 *
 * @param      srtp_ctx             SRTP context.
 * @param[out] data                 Decrypted data.
 * @param      packet_bytes         Packet bytes.
 * @param      num_packet_bytes     Number of packet bytes.
 * @param      index                Packet index.
 */
void ocrypto_srtp_decrypt(
    const ocrypto_srtp_ctx *srtp_ctx,
    uint8_t *data,
    const uint8_t *packet_bytes,
    size_t num_packet_bytes,
    uint32_t index);

/**
 * Generate SRTP authentication tag from bytes and index.
 *
 * @param      srtp_ctx             SRTP context.
 * @param[out] tag                  Authentication tag generated.
 * @param      bytes                Byte buffer.
 * @param      num_bytes            Number of bytes in buffer.
 * @param      index                Index.
 */
void ocrypto_srtp_authenticate(
    const ocrypto_srtp_ctx *srtp_ctx,
    uint8_t *tag,
    const uint8_t *bytes,
    size_t num_bytes,
    uint32_t index);

/**
 * Check SRTP authentication tag against bytes and index.
 *
 * @param      srtp_ctx             SRTP context.
 * @param      tag                  Tag.
 * @param      bytes                Byte buffer.
 * @param      num_bytes            Number of bytes in buffer.
 * @param      index                Index.
 *
 * @retval                          1 If the tag is valid.
 * @retval                          0 Otherwise.
 */
int ocrypto_srtp_verify_authentication(
    const ocrypto_srtp_ctx *srtp_ctx,
    const uint8_t *tag,
    const uint8_t *bytes,
    size_t num_bytes,
    uint32_t index);

#ifdef __cplusplus
}
#endif

#endif
