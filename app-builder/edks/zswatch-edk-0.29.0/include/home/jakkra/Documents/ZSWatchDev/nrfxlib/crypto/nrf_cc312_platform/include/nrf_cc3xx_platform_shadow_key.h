/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
/**@file
 * @defgroup nrf_cc3xx_platform_shadow_key nrf_cc3xx_platform derived key APIs
 * @ingroup nrf_cc3xx_platform
 * @{
 * @brief The nrf_cc3xx_platform_shadow_key APIs provide functions helping with
 *        encrypting/decrypting data using derived keys from the KMU/KDR
 */
#include <stdint.h>

#ifndef NRF_CC3XX_PLATFORM_SHADOW_KEY__
#define NRF_CC3XX_PLATFORM_SHADOW_KEY__

#define NRF_CC3XX_SHADOW_KEY_MAX_INPLACE_CHUNK_SIZE (512)
#define NRF_CC3XX_SHADOW_AEAD_CONTEXT_SIZE_WORDS (66)

typedef enum {
    MODE_ENCRYPT,
    MODE_DECRYPT
} nrf_cc3xx_platform_shadow_key_enc_mode_t;

/** @brief Enum with the encryption/decryption algorithms supported with derived keys */
typedef enum {
    ALG_AES_128_BIT         = 1,
    ALG_AES_256_BIT         = 2,
    ALG_CHACHA20_256_BIT    = 3,
    ALG_CHACHAPOLY_256_BIT  = 3 /* Intentionally the same as CHACHA20 */
} nrf_cc3xx_platform_shadow_key_enc_cipher_t;

/** @brief Structure containing information for doing the key derivation using AES CMAC */
typedef struct{
    uint32_t            slot_id;        /**< The slot id containing the AES key used in the derivation process. */
    size_t              key_size;       /**< The size of the AES key in bits. */
    uint8_t const       *label;         /**< Pointer to containing the label used in the derivation process. */
    size_t              label_size;     /**< Label size in bytes. */
} nrf_cc3xx_platform_shadow_key_derivation_info_t;

/** @brief Derived key context */
typedef struct{
    size_t                                          state;
    nrf_cc3xx_platform_shadow_key_enc_mode_t        mode;
    nrf_cc3xx_platform_shadow_key_derivation_info_t deriv_info;
    nrf_cc3xx_platform_shadow_key_enc_cipher_t      enc_cipher;
    union{
        /* The cc3xx_aead_operation size is opaque so only its size is written here */
        uint32_t cc3xx_aead[NRF_CC3XX_SHADOW_AEAD_CONTEXT_SIZE_WORDS];
    } driver_ctx;
} nrf_cc3xx_platform_shadow_key_ctx_t;


/**@brief Function to initialize the derived key context
 *
 * @param[in,out]   ctx             Pointer to the derived key context.
 * @param[in]       slot_id         The slot id of the derivation key.
 * @param[in]       deriv_key_size  The key size of the derivation key.
 * @param[in]       label           Pointer to the derivation label.
 * @param[in]       label_size      Derivation label size.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_shadow_key_init(nrf_cc3xx_platform_shadow_key_ctx_t * ctx,
                                       uint32_t slot_id,
                                       unsigned int deriv_key_size,
                                       uint8_t const * label,
                                       size_t label_size);

/**@brief Function to setup the encryption/decryption operation
 *
 * @param[in,out]      ctx             Pointer to the derived key context.
 * @param[in]          cipher          The cipher used for the encryption/decryption.
 * @param[in]          mode            The mode (encryption/decryption) of the operation.
 * @param[in]          nonce           Pointer to the additional data used in the authenticated encryption.
 * @param[in]          nonce_size      The size of the additional data in bytes.
 * @param[in]          aad             Pointer to the additional data used in the authenticated encryption.
 * @param[in]          aad_size        The size of the additional data in bytes.
 * @param[in]          plaintext_size  The size of the plaintext in bytes.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_shadow_key_crypt_setup(nrf_cc3xx_platform_shadow_key_ctx_t * ctx,
                                              nrf_cc3xx_platform_shadow_key_enc_cipher_t cipher,
                                              nrf_cc3xx_platform_shadow_key_enc_mode_t mode,
                                              const uint8_t * nonce,
                                              size_t nonce_size,
                                              const uint8_t * aad,
                                              size_t aad_size,
                                              size_t plaintext_size);


/**@brief Function to encrypt/decrypt data using a derived key
 *
 * @param[in,out]   ctx             Pointer to the derived key context.
 * @param[in]       input           Pointer to the input buffer.
 * @param[in]       input_size      The size of the input buffer in bytes.
 * @param[out]      output          Pointer to the output buffer.
 * @param[in]       output_size     The size of the output buffer in bytes.
 * @param[out]      output_length   The number of bytes written to the output buffer.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_shadow_key_crypt_update(nrf_cc3xx_platform_shadow_key_ctx_t * ctx,
                                               uint8_t const * input,
                                               size_t input_size,
                                               uint8_t * output,
                                               size_t output_size,
                                               size_t * output_length);

/**@brief Function to finalize the encryption/decryption operation
 *
 * @details The tag buffer is used as an ouput buffer when performing authenticated encryption and
 *          an input buffer when performing authenticated decryption. The tag_length parameter is
 *          only used when the tag is an output buffer (authenticated encryption).
 *
 * @param[in,out]   ctx             Pointer to the derived key context.
 * @param[out]      output          Pointer to the output buffer.
 * @param[in]       output_size     The size of the output buffer in bytes.
 * @param[out]      output_length   The number of bytes written to the output buffer.
 * @param[in,out]   tag             Pointer to the authentication tag.
 * @param[in]       tag_size        The size of the tag buffer in bytes.
 * @param[out]      tag_length      The number of bytes written to the tag buffer.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_shadow_key_crypt_finish(nrf_cc3xx_platform_shadow_key_ctx_t * ctx,
                                               uint8_t * output,
                                               size_t output_size,
                                               size_t *output_length,
                                               uint8_t * tag,
                                               size_t tag_size,
                                               size_t *tag_length);


#endif /* NRF_CC3XX_PLATFORM_SHADOW_KEY__ */
/** @} */
