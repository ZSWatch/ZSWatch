/*
 *  Functions to delegate cryptographic operations to an available
 *  and appropriate accelerator.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_DRIVER_WRAPPERS_H
#define PSA_CRYPTO_DRIVER_WRAPPERS_H

#include "psa/crypto.h"
#include "psa/crypto_driver_common.h"
#include "psa/crypto_types.h"

/*
 * Initialization and termination functions
 */
psa_status_t psa_driver_wrapper_init(void);
void psa_driver_wrapper_free(void);

/*
 * Signature functions
 */
psa_status_t psa_driver_wrapper_sign_message_with_context(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    const uint8_t *context, size_t context_length,
    uint8_t *signature, size_t signature_size, size_t *signature_length);

psa_status_t psa_driver_wrapper_verify_message_with_context(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    const uint8_t *context, size_t context_length,
    const uint8_t *signature, size_t signature_length);

psa_status_t psa_driver_wrapper_sign_hash_with_context(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash, size_t hash_length,
    const uint8_t *context, size_t context_length,
    uint8_t *signature, size_t signature_size, size_t *signature_length);

psa_status_t psa_driver_wrapper_verify_hash_with_context(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash, size_t hash_length,
    const uint8_t *context, size_t context_length,
    const uint8_t *signature, size_t signature_length);

/*
 * Interruptible Signature functions
 */

uint32_t psa_driver_wrapper_sign_hash_get_num_ops(
    psa_sign_hash_interruptible_operation_t *operation);

uint32_t psa_driver_wrapper_verify_hash_get_num_ops(
    psa_verify_hash_interruptible_operation_t *operation);

psa_status_t psa_driver_wrapper_sign_hash_start(
    psa_sign_hash_interruptible_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg,
    const uint8_t *hash, size_t hash_length);

psa_status_t psa_driver_wrapper_sign_hash_complete(
    psa_sign_hash_interruptible_operation_t *operation,
    uint8_t *signature, size_t signature_size,
    size_t *signature_length);

psa_status_t psa_driver_wrapper_sign_hash_abort(
    psa_sign_hash_interruptible_operation_t *operation);

psa_status_t psa_driver_wrapper_verify_hash_start(
    psa_verify_hash_interruptible_operation_t *operation,
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg,
    const uint8_t *hash, size_t hash_length,
    const uint8_t *signature, size_t signature_length);

psa_status_t psa_driver_wrapper_verify_hash_complete(
    psa_verify_hash_interruptible_operation_t *operation);

psa_status_t psa_driver_wrapper_verify_hash_abort(
    psa_verify_hash_interruptible_operation_t *operation);

/*
 * Key handling functions
 */

psa_status_t psa_driver_wrapper_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length,
    uint8_t *key_buffer, size_t key_buffer_size,
    size_t *key_buffer_length, size_t *bits);

psa_status_t psa_driver_wrapper_export_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length);

psa_status_t psa_driver_wrapper_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length);

psa_status_t psa_driver_wrapper_get_key_buffer_size(
    const psa_key_attributes_t *attributes,
    size_t *key_buffer_size);

psa_status_t psa_driver_wrapper_get_key_buffer_size_from_key_data(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    size_t *key_buffer_size);

psa_status_t psa_driver_wrapper_generate_key(
    const psa_key_attributes_t *attributes,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length);

psa_status_t psa_driver_wrapper_get_builtin_key(
    psa_drv_slot_number_t slot_number,
    psa_key_attributes_t *attributes,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length);

psa_status_t psa_driver_wrapper_copy_key(
    psa_key_attributes_t *attributes,
    const uint8_t *source_key, size_t source_key_length,
    uint8_t *target_key_buffer, size_t target_key_buffer_size,
    size_t *target_key_buffer_length);

psa_status_t psa_driver_wrapper_derive_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *input, size_t input_length,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length);

psa_status_t psa_driver_wrapper_destroy_builtin_key(
    const psa_key_attributes_t *attributes);

/*
 * Cipher functions
 */
psa_status_t psa_driver_wrapper_cipher_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *iv,
    size_t iv_length,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t psa_driver_wrapper_cipher_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t psa_driver_wrapper_cipher_encrypt_setup(
    psa_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_cipher_decrypt_setup(
    psa_cipher_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_cipher_set_iv(
    psa_cipher_operation_t *operation,
    const uint8_t *iv,
    size_t iv_length);

psa_status_t psa_driver_wrapper_cipher_update(
    psa_cipher_operation_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t psa_driver_wrapper_cipher_finish(
    psa_cipher_operation_t *operation,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t psa_driver_wrapper_cipher_abort(
    psa_cipher_operation_t *operation);

/*
 * Hashing functions
 */
psa_status_t psa_driver_wrapper_hash_compute(
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t psa_driver_wrapper_hash_setup(
    psa_hash_operation_t *operation,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_hash_clone(
    const psa_hash_operation_t *source_operation,
    psa_hash_operation_t *target_operation);

psa_status_t psa_driver_wrapper_hash_update(
    psa_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length);

psa_status_t psa_driver_wrapper_hash_finish(
    psa_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t psa_driver_wrapper_hash_abort(
    psa_hash_operation_t *operation);

/*
 * XOF functions
 */
psa_status_t psa_driver_wrapper_xof_setup(psa_xof_operation_t *operation,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_xof_set_context(psa_xof_operation_t *operation,
    const uint8_t *context,
    size_t context_length);

psa_status_t psa_driver_wrapper_xof_update(psa_xof_operation_t *operation,
    const uint8_t *input,
    size_t input_length);

psa_status_t psa_driver_wrapper_xof_output(psa_xof_operation_t *operation,
    uint8_t *output,
    size_t output_length);

psa_status_t psa_driver_wrapper_xof_abort(psa_xof_operation_t *operation);

/*
 * AEAD functions
 */
psa_status_t psa_driver_wrapper_aead_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *plaintext, size_t plaintext_length,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length);

psa_status_t psa_driver_wrapper_aead_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *ciphertext, size_t ciphertext_length,
    uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length);

psa_status_t psa_driver_wrapper_aead_encrypt_setup(
    psa_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_aead_decrypt_setup(
    psa_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_aead_set_nonce(
    psa_aead_operation_t *operation,
    const uint8_t *nonce,
    size_t nonce_length);

psa_status_t psa_driver_wrapper_aead_set_lengths(
    psa_aead_operation_t *operation,
    size_t ad_length,
    size_t plaintext_length);

psa_status_t psa_driver_wrapper_aead_update_ad(
    psa_aead_operation_t *operation,
    const uint8_t *input,
    size_t input_length);

psa_status_t psa_driver_wrapper_aead_update(
    psa_aead_operation_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t psa_driver_wrapper_aead_finish(
    psa_aead_operation_t *operation,
    uint8_t *ciphertext,
    size_t ciphertext_size,
    size_t *ciphertext_length,
    uint8_t *tag,
    size_t tag_size,
    size_t *tag_length);

psa_status_t psa_driver_wrapper_aead_verify(
    psa_aead_operation_t *operation,
    uint8_t *plaintext,
    size_t plaintext_size,
    size_t *plaintext_length,
    const uint8_t *tag,
    size_t tag_length);

psa_status_t psa_driver_wrapper_aead_abort(
    psa_aead_operation_t *operation);

/*
 * MAC functions
 */
psa_status_t psa_driver_wrapper_mac_compute(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length);

psa_status_t psa_driver_wrapper_mac_sign_setup(
    psa_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_mac_verify_setup(
    psa_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_mac_update(
    psa_mac_operation_t *operation,
    const uint8_t *input,
    size_t input_length);

psa_status_t psa_driver_wrapper_mac_sign_finish(
    psa_mac_operation_t *operation,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length);

psa_status_t psa_driver_wrapper_mac_verify_finish(
    psa_mac_operation_t *operation,
    const uint8_t *mac,
    size_t mac_length);

psa_status_t psa_driver_wrapper_mac_abort(
    psa_mac_operation_t *operation);

/*
 * Asymmetric cryptography
 */
psa_status_t psa_driver_wrapper_asymmetric_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    const uint8_t *salt,
    size_t salt_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t psa_driver_wrapper_asymmetric_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    const uint8_t *salt,
    size_t salt_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

/*
 * Raw Key Agreement
 */
psa_status_t psa_driver_wrapper_key_agreement(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *peer_key,
    size_t peer_key_length,
    uint8_t *shared_secret,
    size_t shared_secret_size,
    size_t *shared_secret_length);

/*
 * KDF functions
 */
psa_status_t psa_driver_wrapper_key_derivation_setup(
    psa_key_derivation_operation_t *operation,
    psa_algorithm_t alg);

psa_status_t psa_driver_wrapper_key_derivation_set_capacity(
    psa_key_derivation_operation_t *operation,
    size_t capacity);

psa_status_t psa_driver_wrapper_key_derivation_input_bytes(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step,
    const uint8_t *data, size_t data_length);

psa_status_t psa_driver_wrapper_key_derivation_input_key(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step,
    psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length);

psa_status_t psa_driver_wrapper_key_derivation_input_integer(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step,
    uint64_t value);

psa_status_t psa_driver_wrapper_key_derivation_output_bytes(
    psa_key_derivation_operation_t *operation,
    uint8_t *output, size_t output_length);

psa_status_t psa_driver_wrapper_key_derivation_abort(
    psa_key_derivation_operation_t *operation);

/*
 * Key encapsulation functions.
 */
 psa_status_t psa_driver_wrapper_encapsulate(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const psa_key_attributes_t *output_attributes,
    uint8_t *output_key, size_t output_key_size, size_t *output_key_length,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length);

psa_status_t psa_driver_wrapper_decapsulate(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    psa_algorithm_t alg,
    const uint8_t *ciphertext, size_t ciphertext_length,
    const psa_key_attributes_t *output_attributes,
    uint8_t *output_key, size_t output_key_size, size_t *output_key_length);

/*
 * PAKE functions.
 */
psa_status_t psa_driver_wrapper_pake_setup(
    psa_pake_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *password, size_t password_length,
    const psa_pake_cipher_suite_t *cipher_suite);

psa_status_t psa_driver_wrapper_pake_set_role(
    psa_pake_operation_t *operation,
    psa_pake_role_t role);

psa_status_t psa_driver_wrapper_pake_set_user(
    psa_pake_operation_t *operation,
    const uint8_t *user_id, size_t user_id_length);

psa_status_t psa_driver_wrapper_pake_set_peer(
    psa_pake_operation_t *operation,
    const uint8_t *peer_id, size_t peer_id_length);

psa_status_t psa_driver_wrapper_pake_set_context(
    psa_pake_operation_t *operation,
    const uint8_t *context, size_t context_length);

psa_status_t psa_driver_wrapper_pake_output(
    psa_pake_operation_t *operation,
    psa_pake_step_t step,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

psa_status_t psa_driver_wrapper_pake_input(
    psa_pake_operation_t *operation,
    psa_pake_step_t step,
    const uint8_t *input,
    size_t input_length);

psa_status_t psa_driver_wrapper_pake_get_shared_key(
    psa_pake_operation_t *operation,
    const psa_key_attributes_t *attributes,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length);

psa_status_t psa_driver_wrapper_pake_abort(
    psa_pake_operation_t *operation);

/*
 * Key wrapping functions.
 */
psa_status_t psa_driver_wrapper_wrap_key(
    const psa_key_attributes_t *wrapping_key_attributes,
    const uint8_t *wrapping_key_data, size_t wrapping_key_size,
    psa_algorithm_t alg,
    const psa_key_attributes_t *key_attributes,
    const uint8_t *key_data, size_t key_size,
    uint8_t *data, size_t data_size, size_t *data_length);

psa_status_t psa_driver_wrapper_unwrap_key(
    const psa_key_attributes_t *attributes,
    const psa_key_attributes_t *wrapping_key_attributes,
    const uint8_t *wrapping_key_data, size_t wrapping_key_size,
    psa_algorithm_t alg,
    const uint8_t *data, size_t data_length,
    uint8_t *key, size_t key_size, size_t *key_length);

/*
 * Random
 */
psa_status_t psa_driver_wrapper_get_entropy(
    uint32_t flags,
    size_t *estimate_bits,
    uint8_t *output,
    size_t output_size);

psa_status_t psa_driver_wrapper_init_random(
    psa_driver_random_context_t *context);

psa_status_t psa_driver_wrapper_get_random(
    psa_driver_random_context_t *context,
    uint8_t *output,
    size_t output_size);

psa_status_t psa_driver_wrapper_free_random(
    psa_driver_random_context_t *context);

#endif /* PSA_CRYPTO_DRIVER_WRAPPERS_H */
