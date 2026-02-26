/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto ocrypto
 *
 * 
 * @defgroup ocrypto_sha Message Digests (Hashes)
 * @ingroup ocrypto
 * @{
 * @brief Hash algorithms.
 * 
 * Hash functions are one-way functions that map long inputs into usually shorter and fixed-sized outputs.
 * @}
 * 
 * 
 * @defgroup ocrypto_mac Message Authentication Code (MAC)
 * @ingroup ocrypto
 * @{
 * @brief MAC algorithms.
 *
 * Message authentication codes are tags used for checking the authenticity and integrity of messages.
 * @}
 *
 * 
 * @defgroup ocrypto_unauth_enc Unauthenticated Ciphers
 * @ingroup ocrypto
 * @{
 * @brief Encryption algorithms without authentication.
 * 
 * Ciphers provide encryption/decryption, without authentication or integrity-checking.
 * @}
 *
 * 
 * @defgroup ocrypto_auth_enc Authenticated Encryption with Associated Data (AEAD)
 * @ingroup ocrypto
 * @{
 * @brief Encryption algorithms with authentication and support for additional data.
 * 
 * AEAD algorithms provide encryption with authentication, and support
 * integrity-checking of additional unencrypted data.
 * @}
 * 
 *
 * @defgroup ocrypto_key_wrap Key Wrap
 * @ingroup ocrypto
 * @{
 * @brief Key wrap algorithms, with and without padding.
 * 
 * Key wrap algorithms allow encryption of a key for secure transfer or storage.
 * @}
 * 
 * 
 * @defgroup ocrypto_asym_sig Asymmetric Signature
 * @ingroup ocrypto
 * @{
 * @brief Digital signature algorithms based on elliptic curves or hash-based algorithms.
 * 
 * These algorithms allow the receiver of a digitally signed message, having a
 * public key from the sender, to verify that the message originates from this
 * sender and has not been modified by a third party.
 * 
 * There are mainly four groups of algorithms: those based on the RSA standard,
 * those based on the NIST standard secpXXX elliptic curves (usually referred to
 * as ECDSA algorithms), those based on Twisted Edwards elliptic curves (usually
 * referred to as EdDSA algorithms), those based on hashing for post-quantum
 * resistance (LMS/HSS/XMSS/XMSS^MT), and those based on module-learning with
 * errors. The latter are often referred to as ML-DSA (Module-Lattice Digital
 * Signature Algorithm) and are quantum-resistant.
 * @}
 *
 * 
 * @defgroup ocrypto_rsa Asymmetric Signature and Encryption (RSA)
 * @ingroup ocrypto
 * @{
 * @brief RSA public-key encryption and signature algorithm, API that supports flexible key sizes.
 *
 * RSA (Rivest-Shamir-Adleman) is a number theoretic public-key encryption and signature algorithm.
 * 
 * @see [RFC - PKCS #1: RSA Cryptography Specifications Version 2.2](https://datatracker.ietf.org/doc/html/rfc8017)
 * @}
 *
 * 
 * @defgroup ocrypto_key_agreement Key Agreement / Key Encapsulation Mechanisms (KEM)
 * @ingroup ocrypto
 * @{
 * @brief Key agreement algorithms based on elliptic curves.
 * 
 * A key agreement algorithm generates a shared secret key, in a way that both
 * parties contribute to its creation, and that make it hard for an outside
 * observer to determine the key.
 * 
 * There are mainly three groups of algorithms: those based on the NIST standard
 * secpXXX curves (P-256 etc.), those based on Montgomery curves, and those based
 * on module-learning with errors. The latter are often referred to as KEM (key
 * encapsulation mechanisms) algorithms and are quantum-resistant.
 * @}
 * 
 * 
 * @defgroup ocrypto_kdf Key Derivation
 * @ingroup ocrypto
 * @{
 * @brief Key derivation algorithms.
 * 
 * Key derivation functions generate secret keys from secret values.
 * @}
 *
 * 
 * @defgroup ocrypto_pake Password-Authenticated Key Exchange (PAKE)
 * @ingroup ocrypto
 * @{
 * @brief Key exchange algorithms based on password authentication.
 * 
 * A PAKE agreement algorithm generates a shared secret key based on a password,
 * in a way that makes it hard for an outside observer to determine the key.
 * @}
 * 
 * 
 * @defgroup ocrypto_rsa_orig Asymmetric Signature and Encryption (RSA Original API)
 * @ingroup ocrypto
 * @{
 * @brief RSA public-key encryption and signature algorithm, original ocrypto API that supports 1024 and 2048 bit keys.
 *
 * RSA (Rivest-Shamir-Adleman) is a number theoretic public-key encryption and signature algorithm.
 * 
 * @see [PKCS #1: RSA Cryptography Specifications Version 2.2](https://datatracker.ietf.org/doc/html/rfc8017)
 * @}
 */

/**
 * 
 * @defgroup ocrypto_version Versioning
 * @ingroup ocrypto
 * @{
 * @brief Version of ocrypto to be used in dependency management.
 *
 * This versioning information is used by some clients to ensure compatibility
 * between different versions of the ocrypto library.
 */

/**
 * @file
 * @brief Version of ocrypto to be used in dependency management.
 */

#ifndef OCRYPTO_VERSION_H
#define OCRYPTO_VERSION_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * The version number Major.Minor.Patch is encoded as the following structure:
 *    MMNNPP00
 *    Major version | Minor version | Patch version
 */
#define OCRYPTO_VERSION_NUMBER   0x03090500


#ifdef __cplusplus
}
#endif

#endif

/** @} */
