/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
/**@file
 * @defgroup nrf_cc3xx_platform_derived_key nrf_cc3xx_platform derived key APIs
 * @ingroup nrf_cc3xx_platform
 * @{
 * @brief The nrf_cc3xx_platform_derived_key APIs provide functions helping with
 *        encrypting/decrypting data using derived keys from the KMU/KDR
 */
#include <stdint.h>

#ifndef NRF_CC3XX_PLATFORM_DERIVED_KEY__
#define NRF_CC3XX_PLATFORM_DERIVED_KEY__

/** @brief Enum with the encryption/decryption algorithms supported with derived keys */
typedef enum {
    ALG_AES_128_BIT         = 1,
    ALG_AES_256_BIT         = 2,
    ALG_CHACHA20_256_BIT    = 3,
    ALG_CHACHAPOLY_256_BIT  = 3 /* Intentionally the same as CHACHA20 */
} nrf_cc3xx_platform_cipher_info_t;

/** @brief Structure containing information for doing the key derivation using AES CMAC */
typedef struct{
    uint32_t            slot_id;        /**< The slot id containing the AES key used in the derivation process. */
    size_t              key_size;       /**< The size of the AES key in bits. */
    const uint8_t     * label;          /**< Buffer containing the label used in the derivation process. */
    size_t              label_size;     /**< The size of the label in bytes. */
    const uint8_t     * context;        /**< Buffer containing the context used in the derivation process. */
    size_t              context_size;   /**< The size of the context in bytes. */
} nrf_cc3xx_platform_derivation_info_t;

/** @brief Authenticated encryption/decryption info */
typedef struct{
    const uint8_t     * nonce;      /**< Buffer containing the nonce used in the authenticated encryption/decryption. */
    size_t              nonce_size; /**< The nonce size in bytes. */
    const uint8_t     * aad;        /**< Buffer containing the additional data used in the authenticated encryption/decryption. */
    size_t              aad_size;   /**< The additional data size in bytes. */
    uint8_t           * tag;        /**< Buffer containing the authentication tag. */
    size_t              tag_size;   /**< The tag size in bytes. */

} nrf_cc3xx_platform_auth_info_t;

/** @brief Derived key context */
typedef struct{
    nrf_cc3xx_platform_derivation_info_t    deriv_info;     /**< Derivation info. */
    nrf_cc3xx_platform_cipher_info_t        cipher_info;    /**< Cipher info. */
    nrf_cc3xx_platform_auth_info_t          auth_info;      /**< Authentication info. */
    size_t                                  state;          /**< State info. */
} nrf_cc3xx_platform_derived_key_ctx_t;

/**@brief Function to initialize the derived key context
 *
 * @param[in,out]   ctx     Pointer to the derived key context.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_derived_key_init(nrf_cc3xx_platform_derived_key_ctx_t * ctx);


/**@brief Function to set the key derivation information
 *
 * @param[in,out]   ctx         Pointer to the derived key context.
 * @param[in]       slot_id     The slot id of the derivation key.
 * @param[in]       key_size    The key size of the derivation key.
 * @param[in]       label       Pointer to the derivation label.
 * @param[in]       label_size  Derivation label size.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_derived_key_set_info(nrf_cc3xx_platform_derived_key_ctx_t * ctx,
                                            uint32_t slot_id,
                                            unsigned int key_size,
                                            uint8_t const * label,
                                            size_t label_size);


/**@brief Function to set the derived cipher
 *
 * @param[in,out]   ctx             Pointer to the derived key context.
 * @param[in]       cipher_info     The cipher used by the derived key.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_derived_key_set_cipher(nrf_cc3xx_platform_derived_key_ctx_t * ctx,
                                              nrf_cc3xx_platform_cipher_info_t cipher_info);

/**@brief Function to set the authenticated encryption information
 *
 * @param[in,out]      ctx             Pointer to the derived key context.
 * @param[in]          nonce           Pointer to the nonce.
 * @param[in]          nonce_size      The size of the nonce in bytes.
 * @param[in]          aad             Pointer to the additional data used in the authenticated encryption.
 * @param[in]          aad_size        The size of the additional data in bytes.
 * @param[in]          tag             Pointer to the buffer used for the tag.
 * @param[in]          tag_size        The size of the tag buffer in bytes.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_derived_key_set_auth_info(nrf_cc3xx_platform_derived_key_ctx_t * ctx,
                                                 const uint8_t * nonce,
                                                 size_t nonce_size,
                                                 const uint8_t * aad,
                                                 size_t aad_size,
                                                 uint8_t * tag,
                                                 size_t tag_size);


/**@brief Function to encrypt data using a derived key
 *
 * @note The input and the output buffers should have the same size.
 *
 * @param[in,out]   ctx             Pointer to the derived key context.
 * @param[out]      output          Pointer to the output buffer.
 * @param[in]       input_size      The size of the input buffer in bytes.
 * @param[in]       input           Pointer to the input buffer.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_derived_key_encrypt(nrf_cc3xx_platform_derived_key_ctx_t * ctx,
                                           uint8_t * output,
                                           size_t input_size,
                                           uint8_t const * input);


/**@brief Function to decrypt data using a derived key
 *
 * @note The input and the output buffers should have the same size.
 *
 * @param[in,out]   ctx             Pointer to the derived key context.
 * @param[out]      output          Pointer to the output buffer.
 * @param[in]       input_size      The size of the input buffer in bytes.
 * @param[in]       input           Pointer to the input buffer.
 *
 * @return NRF_CC3XX_PLATFORM_SUCCESS on success, otherwise a negative value.
 */
int nrf_cc3xx_platform_derived_key_decrypt(nrf_cc3xx_platform_derived_key_ctx_t * ctx,
                                           uint8_t * output,
                                           size_t input_size,
                                           uint8_t const * input);

#endif /* NRF_CC3XX_PLATFORM_DERIVED_KEY__ */
/** @} */
