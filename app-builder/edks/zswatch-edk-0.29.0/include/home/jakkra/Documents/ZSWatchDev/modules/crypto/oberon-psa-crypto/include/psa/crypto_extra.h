/**
 * \file psa/crypto_extra.h
 *
 * \brief PSA cryptography module: Mbed TLS vendor extensions
 *
 * \note This file may not be included directly. Applications must
 * include psa/crypto.h.
 *
 * This file is reserved for vendor-specific definitions.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/*
 * NOTICE: This file has been modified by Oberon microsystems AG.
 */

#ifndef PSA_CRYPTO_EXTRA_H
#define PSA_CRYPTO_EXTRA_H
#include "mbedtls/private_access.h"

#include "crypto_types.h"
#include "crypto_compat.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Identification of Oberon PSA Crypto Core */
#ifndef PSA_CRYPTO_CORE_OBERON
#define PSA_CRYPTO_CORE_OBERON
#endif

/* UID for secure storage seed */
#define PSA_CRYPTO_ITS_RANDOM_SEED_UID 0xFFFFFF52

/* See mbedtls_config.h for definition */
#if !defined(MBEDTLS_PSA_KEY_SLOT_COUNT)
#define MBEDTLS_PSA_KEY_SLOT_COUNT 32
#endif

/* If the size of static key slots is not explicitly defined by the user, then
 * set it to the maximum between PSA_EXPORT_KEY_PAIR_OR_PUBLIC_MAX_SIZE and
 * PSA_CIPHER_MAX_KEY_LENGTH.
 * See mbedtls_config.h for the definition. */
#if !defined(MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE)
#define MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE  \
    ((PSA_EXPORT_KEY_PAIR_OR_PUBLIC_MAX_SIZE > PSA_CIPHER_MAX_KEY_LENGTH) ? \
     PSA_EXPORT_KEY_PAIR_OR_PUBLIC_MAX_SIZE : PSA_CIPHER_MAX_KEY_LENGTH)
#endif /* !MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE*/

/** \addtogroup attributes
 * @{
 */

/** \brief Declare the enrollment algorithm for a key.
 *
 * An operation on a key may indifferently use the algorithm set with
 * psa_set_key_algorithm() or with this function.
 *
 * \param[out] attributes       The attribute structure to write to.
 * \param alg2                  A second algorithm that the key may be used
 *                              for, in addition to the algorithm set with
 *                              psa_set_key_algorithm().
 *
 * \warning Setting an enrollment algorithm is not recommended, because
 *          using the same key with different algorithms can allow some
 *          attacks based on arithmetic relations between different
 *          computations made with the same key, or can escalate harmless
 *          side channels into exploitable ones. Use this function only
 *          if it is necessary to support a protocol for which it has been
 *          verified that the usage of the key with multiple algorithms
 *          is safe.
 */
static inline void psa_set_key_enrollment_algorithm(
    psa_key_attributes_t *attributes,
    psa_algorithm_t alg2)
{
    attributes->MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(alg2) = alg2;
}

/** Retrieve the enrollment algorithm policy from key attributes.
 *
 * \param[in] attributes        The key attribute structure to query.
 *
 * \return The enrollment algorithm stored in the attribute structure.
 */
static inline psa_algorithm_t psa_get_key_enrollment_algorithm(
    const psa_key_attributes_t *attributes)
{
    return attributes->MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(alg2);
}

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)

/** Retrieve the slot number where a key is stored.
 *
 * A slot number is only defined for keys that are stored in a secure
 * element.
 *
 * This information is only useful if the secure element is not entirely
 * managed through the PSA Cryptography API. It is up to the secure
 * element driver to decide how PSA slot numbers map to any other interface
 * that the secure element may have.
 *
 * \param[in] attributes        The key attribute structure to query.
 * \param[out] slot_number      On success, the slot number containing the key.
 *
 * \retval #PSA_SUCCESS
 *         The key is located in a secure element, and \p *slot_number
 *         indicates the slot number that contains it.
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         The caller is not permitted to query the slot number.
 *         Mbed TLS currently does not return this error.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The key is not located in a secure element.
 */
psa_status_t psa_get_key_slot_number(
    const psa_key_attributes_t *attributes,
    psa_key_slot_number_t *slot_number);

/** Choose the slot number where a key is stored.
 *
 * This function declares a slot number in the specified attribute
 * structure.
 *
 * A slot number is only meaningful for keys that are stored in a secure
 * element. It is up to the secure element driver to decide how PSA slot
 * numbers map to any other interface that the secure element may have.
 *
 * \note Setting a slot number in key attributes for a key creation can
 *       cause the following errors when creating the key:
 *       - #PSA_ERROR_NOT_SUPPORTED if the selected secure element does
 *         not support choosing a specific slot number.
 *       - #PSA_ERROR_NOT_PERMITTED if the caller is not permitted to
 *         choose slot numbers in general or to choose this specific slot.
 *       - #PSA_ERROR_INVALID_ARGUMENT if the chosen slot number is not
 *         valid in general or not valid for this specific key.
 *       - #PSA_ERROR_ALREADY_EXISTS if there is already a key in the
 *         selected slot.
 *
 * \param[out] attributes       The attribute structure to write to.
 * \param slot_number           The slot number to set.
 */
static inline void psa_set_key_slot_number(
    psa_key_attributes_t *attributes,
    psa_key_slot_number_t slot_number)
{
    attributes->MBEDTLS_PRIVATE(has_slot_number) = 1;
    attributes->MBEDTLS_PRIVATE(slot_number) = slot_number;
}

/** Remove the slot number attribute from a key attribute structure.
 *
 * This function undoes the action of psa_set_key_slot_number().
 *
 * \param[out] attributes       The attribute structure to write to.
 */
static inline void psa_clear_key_slot_number(
    psa_key_attributes_t *attributes)
{
    attributes->MBEDTLS_PRIVATE(has_slot_number) = 0;
}

/** Register a key that is already present in a secure element.
 *
 * The key must be located in a secure element designated by the
 * lifetime field in \p attributes, in the slot set with
 * psa_set_key_slot_number() in the attribute structure.
 * This function makes the key available through the key identifier
 * specified in \p attributes.
 *
 * \param[in] attributes        The attributes of the existing key.
 *                              - The lifetime must be a persistent lifetime
 *                                in a secure element. Volatile lifetimes are
 *                                not currently supported.
 *                              - The key identifier must be in the valid
 *                                range for persistent keys.
 *                              - The key type and size must be specified and
 *                                must be consistent with the key material
 *                                in the secure element.
 *
 * \retval #PSA_SUCCESS
 *         The key was successfully registered.
 *         Note that depending on the design of the driver, this may or may
 *         not guarantee that a key actually exists in the designated slot
 *         and is compatible with the specified attributes.
 * \retval #PSA_ERROR_ALREADY_EXISTS
 *         There is already a key with the identifier specified in
 *         \p attributes.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The secure element driver for the specified lifetime does not
 *         support registering a key.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The identifier in \p attributes is invalid, namely the identifier is
 *         not in the user range, or
 *         \p attributes specifies a lifetime which is not located
 *         in a secure element, or no slot number is specified in \p attributes,
 *         or the specified slot number is not valid.
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         The caller is not authorized to register the specified key slot.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_INSUFFICIENT_STORAGE \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_DATA_INVALID \emptydescription
 * \retval #PSA_ERROR_DATA_CORRUPT \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t mbedtls_psa_register_se_key(
    const psa_key_attributes_t *attributes);

#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

/**@}*/

/**
 * \brief Library deinitialization.
 *
 * This function clears all data associated with the PSA layer,
 * including the whole key store.
 * This function is not thread safe, it wipes every key slot regardless of
 * state and reader count. It should only be called when no slot is in use.
 *
 * This is an Mbed TLS extension.
 */
void mbedtls_psa_crypto_free(void);

/** \brief Statistics about
 * resource consumption related to the PSA keystore.
 *
 * \note The content of this structure is not part of the stable API and ABI
 *       of Mbed TLS and may change arbitrarily from version to version.
 */
typedef struct mbedtls_psa_stats_s {
    /** Number of slots containing key material for a volatile key. */
    size_t MBEDTLS_PRIVATE(volatile_slots);
    /** Number of slots containing key material for a key which is in
     * internal persistent storage. */
    size_t MBEDTLS_PRIVATE(persistent_slots);
    /** Number of slots containing a reference to a key in a
     * secure element. */
    size_t MBEDTLS_PRIVATE(external_slots);
    /** Number of slots which are occupied, but do not contain
     * key material yet. */
    size_t MBEDTLS_PRIVATE(half_filled_slots);
    /** Number of slots that contain cache data. */
    size_t MBEDTLS_PRIVATE(cache_slots);
    /** Number of slots that are not used for anything. */
    size_t MBEDTLS_PRIVATE(empty_slots);
    /** Number of slots that are locked. */
    size_t MBEDTLS_PRIVATE(locked_slots);
    /** Largest key id value among open keys in internal persistent storage. */
    psa_key_id_t MBEDTLS_PRIVATE(max_open_internal_key_id);
    /** Largest key id value among open keys in secure elements. */
    psa_key_id_t MBEDTLS_PRIVATE(max_open_external_key_id);
} mbedtls_psa_stats_t;

/** \brief Get statistics about
 * resource consumption related to the PSA keystore.
 *
 * \note When Mbed TLS is built as part of a service, with isolation
 *       between the application and the keystore, the service may or
 *       may not expose this function.
 */
void mbedtls_psa_get_stats(mbedtls_psa_stats_t *stats);

/**
 * \brief Inject an initial entropy seed for the random generator into
 *        secure storage.
 *
 * This function injects data to be used as a seed for the random generator
 * used by the PSA Crypto implementation. On devices that lack a trusted
 * entropy source (preferably a hardware random number generator),
 * the Mbed PSA Crypto implementation uses this value to seed its
 * random generator.
 *
 * On devices without a trusted entropy source, this function must be
 * called exactly once in the lifetime of the device. On devices with
 * a trusted entropy source, calling this function is optional.
 * In all cases, this function may only be called before calling any
 * other function in the PSA Crypto API, including psa_crypto_init().
 *
 * When this function returns successfully, it populates a file in
 * persistent storage. Once the file has been created, this function
 * can no longer succeed.
 *
 * If any error occurs, this function does not change the system state.
 * You can call this function again after correcting the reason for the
 * error if possible.
 *
 * \warning This function **can** fail! Callers MUST check the return status.
 *
 * \warning If you use this function, you should use it as part of a
 *          factory provisioning process. The value of the injected seed
 *          is critical to the security of the device. It must be
 *          *secret*, *unpredictable* and (statistically) *unique per device*.
 *          You should be generate it randomly using a cryptographically
 *          secure random generator seeded from trusted entropy sources.
 *          You should transmit it securely to the device and ensure
 *          that its value is not leaked or stored anywhere beyond the
 *          needs of transmitting it from the point of generation to
 *          the call of this function, and erase all copies of the value
 *          once this function returns.
 *
 * This is an Mbed TLS extension.
 *
 * \note This function is only available on the following platforms:
 * * If the compile-time option MBEDTLS_PSA_INJECT_ENTROPY is enabled.
 *   Note that you must provide compatible implementations of
 *   mbedtls_nv_seed_read and mbedtls_nv_seed_write.
 * * In a client-server integration of PSA Cryptography, on the client side,
 *   if the server supports this feature.
 * \param[in] seed          Buffer containing the seed value to inject.
 * \param[in] seed_size     Size of the \p seed buffer.
 *                          The size of the seed in bytes must be greater
 *                          or equal to both #MBEDTLS_ENTROPY_BLOCK_SIZE
 *                          and the value of \c MBEDTLS_ENTROPY_MIN_PLATFORM
 *                          in `library/entropy_poll.h` in the Mbed TLS source
 *                          code.
 *                          It must be less or equal to
 *                          #MBEDTLS_ENTROPY_MAX_SEED_SIZE.
 *
 * \retval #PSA_SUCCESS
 *         The seed value was injected successfully. The random generator
 *         of the PSA Crypto implementation is now ready for use.
 *         You may now call psa_crypto_init() and use the PSA Crypto
 *         implementation.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p seed_size is out of range.
 * \retval #PSA_ERROR_STORAGE_FAILURE
 *         There was a failure reading or writing from storage.
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         The library has already been initialized. It is no longer
 *         possible to call this function.
 */
psa_status_t mbedtls_psa_inject_entropy(const uint8_t *seed,
                                        size_t seed_size);

/** \addtogroup crypto_types
 * @{
 */

/** DSA public key.
 *
 * The import and export format is the
 * representation of the public key `y = g^x mod p` as a big-endian byte
 * string. The length of the byte string is the length of the base prime `p`
 * in bytes.
 */
#define PSA_KEY_TYPE_DSA_PUBLIC_KEY                 ((psa_key_type_t) 0x400E) /* !!OM */

/** DSA key pair (private and public key).
 *
 * The import and export format is the
 * representation of the private key `x` as a big-endian byte string. The
 * length of the byte string is the private key size in bytes (leading zeroes
 * are not stripped).
 *
 * Deterministic DSA key derivation with psa_generate_derived_key follows
 * FIPS 186-4 &sect;B.1.2: interpret the byte string as integer
 * in big-endian order. Discard it if it is not in the range
 * [0, *N* - 2] where *N* is the boundary of the private key domain
 * (the prime *p* for Diffie-Hellman, the subprime *q* for DSA,
 * or the order of the curve's base point for ECC).
 * Add 1 to the resulting integer and use this as the private key *x*.
 *
 */
#define PSA_KEY_TYPE_DSA_KEY_PAIR                    ((psa_key_type_t) 0x700E) /* !!OM */

/** Whether a key type is a DSA key (pair or public-only). */
#define PSA_KEY_TYPE_IS_DSA(type)                                       \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == PSA_KEY_TYPE_DSA_PUBLIC_KEY)

#define PSA_ALG_DSA_BASE                        ((psa_algorithm_t) 0x06000400)
/** DSA signature with hashing.
 *
 * This is the signature scheme defined by FIPS 186-4,
 * with a random per-message secret number (*k*).
 *
 * \param hash_alg      A hash algorithm (\c PSA_ALG_XXX value such that
 *                      #PSA_ALG_IS_HASH(\p hash_alg) is true).
 *                      This includes #PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a usage policy.
 *
 * \return              The corresponding DSA signature algorithm.
 * \return              Unspecified if \p hash_alg is not a supported
 *                      hash algorithm.
 */
#define PSA_ALG_DSA(hash_alg)                             \
    (PSA_ALG_DSA_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_DETERMINISTIC_DSA_BASE          ((psa_algorithm_t) 0x06000500)
#define PSA_ALG_DSA_DETERMINISTIC_FLAG PSA_ALG_ECDSA_DETERMINISTIC_FLAG
/** Deterministic DSA signature with hashing.
 *
 * This is the deterministic variant defined by RFC 6979 of
 * the signature scheme defined by FIPS 186-4.
 *
 * \param hash_alg      A hash algorithm (\c PSA_ALG_XXX value such that
 *                      #PSA_ALG_IS_HASH(\p hash_alg) is true).
 *                      This includes #PSA_ALG_ANY_HASH
 *                      when specifying the algorithm in a usage policy.
 *
 * \return              The corresponding DSA signature algorithm.
 * \return              Unspecified if \p hash_alg is not a supported
 *                      hash algorithm.
 */
#define PSA_ALG_DETERMINISTIC_DSA(hash_alg)                             \
    (PSA_ALG_DETERMINISTIC_DSA_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_IS_DSA(alg)                                             \
    (((alg) & ~PSA_ALG_HASH_MASK & ~PSA_ALG_DSA_DETERMINISTIC_FLAG) ==  \
     PSA_ALG_DSA_BASE)
#define PSA_ALG_DSA_IS_DETERMINISTIC(alg)               \
    (((alg) & PSA_ALG_DSA_DETERMINISTIC_FLAG) != 0)
#define PSA_ALG_IS_DETERMINISTIC_DSA(alg)                       \
    (PSA_ALG_IS_DSA(alg) && PSA_ALG_DSA_IS_DETERMINISTIC(alg))
#define PSA_ALG_IS_RANDOMIZED_DSA(alg)                          \
    (PSA_ALG_IS_DSA(alg) && !PSA_ALG_DSA_IS_DETERMINISTIC(alg))


/* We need to expand the sample definition of this macro from
 * the API definition. */
#undef PSA_ALG_IS_VENDOR_HASH_AND_SIGN
#define PSA_ALG_IS_VENDOR_HASH_AND_SIGN(alg)    \
    PSA_ALG_IS_DSA(alg)

/**@}*/

/** \defgroup psa_external_rng External random generator
 * @{
 */

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
/** External random generator function, implemented by the platform.
 *
 * When the compile-time option #MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG is enabled,
 * this function replaces Mbed TLS's entropy and DRBG modules for all
 * random generation triggered via PSA crypto interfaces.
 *
 * \note This random generator must deliver random numbers with cryptographic
 *       quality and high performance. It must supply unpredictable numbers
 *       with a uniform distribution. The implementation of this function
 *       is responsible for ensuring that the random generator is seeded
 *       with sufficient entropy. If you have a hardware TRNG which is slow
 *       or delivers non-uniform output, declare it as an entropy source
 *       with mbedtls_entropy_add_source() instead of enabling this option.
 *
 * \param[in,out] context       Pointer to the random generator context.
 *                              This is all-bits-zero on the first call
 *                              and preserved between successive calls.
 * \param[out] output           Output buffer. On success, this buffer
 *                              contains random data with a uniform
 *                              distribution.
 * \param output_size           The size of the \p output buffer in bytes.
 * \param[out] output_length    On success, set this value to \p output_size.
 *
 * \retval #PSA_SUCCESS
 *         Success. The output buffer contains \p output_size bytes of
 *         cryptographic-quality random data, and \c *output_length is
 *         set to \p output_size.
 * \retval #PSA_ERROR_INSUFFICIENT_ENTROPY
 *         The random generator requires extra entropy and there is no
 *         way to obtain entropy under current environment conditions.
 *         This error should not happen under normal circumstances since
 *         this function is responsible for obtaining as much entropy as
 *         it needs. However implementations of this function may return
 *         #PSA_ERROR_INSUFFICIENT_ENTROPY if there is no way to obtain
 *         entropy without blocking indefinitely.
 * \retval #PSA_ERROR_HARDWARE_FAILURE
 *         A failure of the random generator hardware that isn't covered
 *         by #PSA_ERROR_INSUFFICIENT_ENTROPY.
 */
psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output, size_t output_size, size_t *output_length);
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

/**@}*/

/** \defgroup psa_builtin_keys Built-in keys
 * @{
 */

/** The minimum value for a key identifier that is built into the
 * implementation.
 *
 * The range of key identifiers from #MBEDTLS_PSA_KEY_ID_BUILTIN_MIN
 * to #MBEDTLS_PSA_KEY_ID_BUILTIN_MAX within the range from
 * #PSA_KEY_ID_VENDOR_MIN and #PSA_KEY_ID_VENDOR_MAX and must not intersect
 * with any other set of implementation-chosen key identifiers.
 *
 * This value is part of the library's API since changing it would invalidate
 * the values of built-in key identifiers in applications.
 */
#define MBEDTLS_PSA_KEY_ID_BUILTIN_MIN          ((psa_key_id_t) 0x40001000)

/** The maximum value for a key identifier that is built into the
 * implementation.
 *
 * See #MBEDTLS_PSA_KEY_ID_BUILTIN_MIN for more information.
 */
#define MBEDTLS_PSA_KEY_ID_BUILTIN_MAX          ((psa_key_id_t) 0x7fffefff)

/** A slot number identifying a key in a driver.
 *
 * Values of this type are used to identify built-in keys.
 */
typedef uint64_t psa_drv_slot_number_t;

#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
/** Test whether a key identifier belongs to the builtin key range.
 *
 * \param key_id  Key identifier to test.
 *
 * \retval 1
 *         The key identifier is a builtin key identifier.
 * \retval 0
 *         The key identifier is not a builtin key identifier.
 */
static inline int psa_key_id_is_builtin(psa_key_id_t key_id)
{
    return (key_id >= MBEDTLS_PSA_KEY_ID_BUILTIN_MIN) &&
           (key_id <= MBEDTLS_PSA_KEY_ID_BUILTIN_MAX);
}

/** Platform function to obtain the location and slot number of a built-in key.
 *
 * An application-specific implementation of this function must be provided if
 * #MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS is enabled. This would typically be provided
 * as part of a platform's system image.
 *
 * #MBEDTLS_SVC_KEY_ID_GET_KEY_ID(\p key_id) needs to be in the range from
 * #MBEDTLS_PSA_KEY_ID_BUILTIN_MIN to #MBEDTLS_PSA_KEY_ID_BUILTIN_MAX.
 *
 * In a multi-application configuration
 * (\c MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER is defined),
 * this function should check that #MBEDTLS_SVC_KEY_ID_GET_OWNER_ID(\p key_id)
 * is allowed to use the given key.
 *
 * \param key_id                The key ID for which to retrieve the
 *                              location and slot attributes.
 * \param[out] lifetime         On success, the lifetime associated with the key
 *                              corresponding to \p key_id. Lifetime is a
 *                              combination of which driver contains the key,
 *                              and with what persistence level the key is
 *                              intended to be used. If the platform
 *                              implementation does not contain specific
 *                              information about the intended key persistence
 *                              level, the persistence level may be reported as
 *                              #PSA_KEY_PERSISTENCE_DEFAULT.
 * \param[out] slot_number      On success, the slot number known to the driver
 *                              registered at the lifetime location reported
 *                              through \p lifetime which corresponds to the
 *                              requested built-in key.
 *
 * \retval #PSA_SUCCESS
 *         The requested key identifier designates a built-in key.
 *         In a multi-application configuration, the requested owner
 *         is allowed to access it.
 * \retval #PSA_ERROR_DOES_NOT_EXIST
 *         The requested key identifier is not a built-in key which is known
 *         to this function. If a key exists in the key storage with this
 *         identifier, the data from the storage will be used.
 * \return (any other error)
 *         Any other error is propagated to the function that requested the key.
 *         Common errors include:
 *         - #PSA_ERROR_NOT_PERMITTED: the key exists but the requested owner
 *           is not allowed to access it.
 */
psa_status_t mbedtls_psa_platform_get_builtin_key(
    mbedtls_svc_key_id_t key_id,
    psa_key_lifetime_t *lifetime,
    psa_drv_slot_number_t *slot_number);
#endif /* MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS */

/** @} */

/** \defgroup psa_crypto_client Functions defined by a client provider
 *
 * The functions in this group are meant to be implemented by providers of
 * the PSA Crypto client interface. They are provided by the library when
 * #MBEDTLS_PSA_CRYPTO_C is enabled.
 *
 * \note All functions in this group are experimental, as using
 *       alternative client interface providers is experimental.
 *
 * @{
 */

/** Check if PSA is capable of handling the specified hash algorithm.
 *
 * This means that PSA core was built with the corresponding PSA_WANT_ALG_xxx
 * set and that psa_crypto_init has already been called.
 *
 * \note When using the built-in version of the PSA core (i.e.
 *       #MBEDTLS_PSA_CRYPTO_C is set), for now, this function only checks
 *       the state of the driver subsystem, not the algorithm.
 *       This might be improved in the future.
 *
 * \param hash_alg  The hash algorithm.
 *
 * \return 1 if the PSA can handle \p hash_alg, 0 otherwise.
 */
int psa_can_do_hash(psa_algorithm_t hash_alg);

/**
 * Tell if PSA is ready for this cipher.
 *
 * \note When using the built-in version of the PSA core (i.e.
 *       #MBEDTLS_PSA_CRYPTO_C is set), for now, this function only checks
 *       the state of the driver subsystem, not the key type and algorithm.
 *       This might be improved in the future.
 *
 * \param key_type    The key type.
 * \param cipher_alg  The cipher algorithm.
 *
 * \return 1 if the PSA can handle \p cipher_alg, 0 otherwise.
 */
int psa_can_do_cipher(psa_key_type_t key_type, psa_algorithm_t cipher_alg);

/**@}*/

/** \addtogroup crypto_types
 * @{
 */

#define PSA_KEY_TYPE_SPAKE2P_KEY_PAIR_BASE          ((psa_key_type_t) 0x7400)
#define PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY_BASE        ((psa_key_type_t) 0x4400)
#define PSA_KEY_TYPE_SPAKE2P_CURVE_MASK             ((psa_key_type_t) 0x00ff)

 /** SPAKE2+ key pair. Both the prover and verifier key.
 *
 * The size of a SPAKE2+ key is the size associated with the elliptic curve
 * group. See the documentation of each elliptic curve family for details.
 * To construct a SPAKE2+ key pair, it must be output from a key derivation
 * operation.
 * The corresponding public key can be exported using psa_export_public_key().
 * See also #PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY().
 *
 * \param curve A value of type psa_ecc_family_t that identifies the elliptic
 *              curve family to be used.
 */
#define PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(curve) \
    ((psa_key_type_t) (PSA_KEY_TYPE_SPAKE2P_KEY_PAIR_BASE | (curve)))

 /** SPAKE2+ public key. The verifier key.
 *
 * The size of an SPAKE2+ public key is the same as the corresponding private
 * key. See #PSA_KEY_TYPE_SPAKE2P_KEY_PAIR() and the documentation of each
 * elliptic curve family for details.
 * To construct a SPAKE2+ public key, it must be imported.
 *
 * \param curve A value of type psa_ecc_family_t that identifies the elliptic
 *              curve family to be used.
 */
#define PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY(curve) \
    ((psa_key_type_t) (PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY_BASE | (curve)))

 /** Whether a key type is a SPAKE2+ key (pair or public-only). */
#define PSA_KEY_TYPE_IS_SPAKE2P(type)                                 \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) &                     \
      ~PSA_KEY_TYPE_SPAKE2P_CURVE_MASK) ==                            \
      PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY_BASE)
 /** Whether a key type is a SPAKE2+ key pair. */
#define PSA_KEY_TYPE_IS_SPAKE2P_KEY_PAIR(type)                        \
    (((type) & ~PSA_KEY_TYPE_SPAKE2P_CURVE_MASK) ==                   \
     PSA_KEY_TYPE_SPAKE2P_KEY_PAIR_BASE)
 /** Whether a key type is a SPAKE2+ public key. */
#define PSA_KEY_TYPE_IS_SPAKE2P_PUBLIC_KEY(type)                      \
    (((type) & ~PSA_KEY_TYPE_SPAKE2P_CURVE_MASK) ==                   \
     PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY_BASE)
 /** Extract the curve from a SPAKE2+ key type. */
#define PSA_KEY_TYPE_SPAKE2P_GET_FAMILY(type)                         \
    ((psa_ecc_family_t) (PSA_KEY_TYPE_IS_SPAKE2P(type) ?              \
                         ((type) & PSA_KEY_TYPE_SPAKE2P_CURVE_MASK) : \
                         0))

#define PSA_KEY_TYPE_SRP_KEY_PAIR_BASE          ((psa_key_type_t) 0x7700)
#define PSA_KEY_TYPE_SRP_PUBLIC_KEY_BASE        ((psa_key_type_t) 0x4700)
#define PSA_KEY_TYPE_SRP_GROUP_MASK             ((psa_key_type_t) 0x00ff)

 /** SRP key pair. Both the client and server key.
 *
 * The size of a SRP key is the size associated with the Diffie-Hellman
 * group. See the documentation of each Diffie-Hellman group for details.
 * To construct a SRP key pair, the password hash must be imported.
 * The corresponding public key (password verifier) can be exported using
 * psa_export_public_key(). See also #PSA_KEY_TYPE_SRP_PUBLIC_KEY().
 *
 * \param group A value of type ::psa_dh_family_t that identifies the
 *              Diffie-Hellman group to be used.
 */
#define PSA_KEY_TYPE_SRP_KEY_PAIR(group) \
    ((psa_key_type_t) (PSA_KEY_TYPE_SRP_KEY_PAIR_BASE | (group)))

 /** SRP public key. The server key (password verifier).
 *
 * The size of an SRP public key is the same as the corresponding private
 * key. See #PSA_KEY_TYPE_SRP_KEY_PAIR() and the documentation of each
 * Diffie-Hellman group for details.
 * To construct a SRP public key, it must be imported. The key size
 * in attributes must not be zero.
 *
 * \param group A value of type ::psa_dh_family_t that identifies the
 *              Diffie-Hellman group to be used.
 */
#define PSA_KEY_TYPE_SRP_PUBLIC_KEY(group) \
    ((psa_key_type_t) (PSA_KEY_TYPE_SRP_PUBLIC_KEY_BASE | (group)))

 /** Whether a key type is a SRP key (pair or public-only). */
#define PSA_KEY_TYPE_IS_SRP(type)                                 \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) &                 \
      ~PSA_KEY_TYPE_SRP_GROUP_MASK) ==                            \
      PSA_KEY_TYPE_SRP_PUBLIC_KEY_BASE)
 /** Whether a key type is a SRP key pair. */
#define PSA_KEY_TYPE_IS_SRP_KEY_PAIR(type)                        \
    (((type) & ~PSA_KEY_TYPE_SRP_GROUP_MASK) ==                   \
     PSA_KEY_TYPE_SRP_KEY_PAIR_BASE)
 /** Whether a key type is a SRP public key. */
#define PSA_KEY_TYPE_IS_SRP_PUBLIC_KEY(type)                      \
    (((type) & ~PSA_KEY_TYPE_SRP_GROUP_MASK) ==                   \
     PSA_KEY_TYPE_SRP_PUBLIC_KEY_BASE)
 /** Extract the curve from a SRP key type. */
#define PSA_KEY_TYPE_SRP_GET_FAMILY(type)                         \
    ((psa_ecc_family_t) (PSA_KEY_TYPE_IS_SRP(type) ?              \
                         ((type) & PSA_KEY_TYPE_SRP_GROUP_MASK) : \
                         0))

#define PSA_KEY_TYPE_WPA3_SAE_ECC_BASE         ((psa_key_type_t) 0x3280)
#define PSA_KEY_TYPE_WPA3_SAE_DH_BASE          ((psa_key_type_t) 0x3300)
#define PSA_KEY_TYPE_WPA3_SAE_GROUP_MASK       ((psa_key_type_t) 0x007f)

/** WPA3-SAE ECC key.
 *
 * The key is used to store the out of band calculated group element
 * used in the Hash-To-Element variant of WPA3-SAE. It can be used as
 * input to the WPA3-SAE PAKE instead of a password key.
 *
 * \param group A value of type ::psa_ec_family_t that identifies the
 *              group to be used.
 */
#define PSA_KEY_TYPE_WPA3_SAE_ECC(group) \
    ((psa_key_type_t) (PSA_KEY_TYPE_WPA3_SAE_ECC_BASE | (group)))

/** WPA3-SAE DH key.
 *
 * The key is used to store the out of band calculated group element
 * used in the Hash-To-Element variant of WPA3-SAE. It can be used as
 * input to the WPA3-SAE PAKE instead of a password key.
 *
 * \param group A value of type ::psa_dh_family_t that identifies the
 *              group to be used.
 */
#define PSA_KEY_TYPE_WPA3_SAE_DH(group) \
    ((psa_key_type_t) (PSA_KEY_TYPE_WPA3_SAE_DH_BASE | (group)))

 /** Whether a key type is a WPA3-SAE. */
#define PSA_KEY_TYPE_IS_WPA3_SAE(type)                    \
    ((((type) - (1 << 7)) & ~0x00ff) ==                      \
     (PSA_KEY_TYPE_WPA3_SAE_ECC_BASE - (1 << 7)))
 /** Whether a key type is a WPA3-SAE-ECC. */
#define PSA_KEY_TYPE_IS_WPA3_SAE_ECC(type)                   \
    (((type) & ~PSA_KEY_TYPE_WPA3_SAE_GROUP_MASK) ==         \
     PSA_KEY_TYPE_WPA3_SAE_ECC_BASE)
 /** Whether a key type is a WPA3-SAE-DH. */
#define PSA_KEY_TYPE_IS_WPA3_SAE_DH(type)                    \
    (((type) & ~PSA_KEY_TYPE_WPA3_SAE_GROUP_MASK) ==         \
     PSA_KEY_TYPE_WPA3_SAE_DH_BASE)
 /** Extract the group from a WPA3-SAE-DH key type. */
#define PSA_KEY_TYPE_WPA3_SAE_DH_GET_FAMILY(type)            \
    ((psa_ecc_family_t) (PSA_KEY_TYPE_IS_WPA3_SAE_DH(type) ? \
      ((type) & PSA_KEY_TYPE_WPA3_SAE_GROUP_MASK) :          \
      0))
 /** Extract the group from a WPA3-SAE-ECC key type. */
#define PSA_KEY_TYPE_WPA3_SAE_ECC_GET_FAMILY(type)            \
    ((psa_ecc_family_t) (PSA_KEY_TYPE_IS_WPA3_SAE_ECC(type) ? \
      ((type) & PSA_KEY_TYPE_WPA3_SAE_GROUP_MASK) :           \
      0))

#define PSA_ALG_WPA3_SAE_H2E_BASE         ((psa_algorithm_t) 0x08800400)
/** The WPA3-SAE password to PT KDF.
 * It takes the password p, a salt (uuid), and optionally a password id.
 * 
 * This key derivation algorithm uses the following inputs, which must be
 * provided in the following order:
 * - #PSA_KEY_DERIVATION_INPUT_SALT for the uuid.
 * - #PSA_KEY_DERIVATION_INPUT_PASSWORD for the password.
 * - optionally; #PSA_KEY_DERIVATION_INPUT_INFO for the password id.
 * The output has to be read as a key of type PSA_KEY_TYPE_WPA3_SAE.
 *
 * \param hash_alg      A hash algorithm (\c PSA_ALG_XXX value such that
 *                      #PSA_ALG_IS_HASH(\p hash_alg) is true).
 *
 * \return              The corresponding counter-mode KDF algorithm.
 * \return              Unspecified if \p hash_alg is not a supported
 *                      hash algorithm.
 */
#define PSA_ALG_WPA3_SAE_H2E(hash_alg)                            \
    (PSA_ALG_WPA3_SAE_H2E_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

/** Whether the specified algorithm is a key derivation algorithm constructed
 * using #PSA_ALG_WPA3_SAE_H2E(\p hash_alg).
 *
 * \param alg An algorithm identifier (value of type #psa_algorithm_t).
 *
 * \return 1 if \p alg is a key derivation algorithm constructed using #PSA_ALG_WPA3_SAE_H2E(),
 *         0 otherwise. This macro may return either 0 or 1 if \c alg is not a supported
 *         key derivation algorithm identifier.
 */
#define PSA_ALG_IS_WPA3_SAE_H2E(alg)                         \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_WPA3_SAE_H2E_BASE)

#define PSA_ALG_CATEGORY_PAKE                   ((psa_algorithm_t) 0x0a000000)

/** Whether the specified algorithm is a password-authenticated key exchange.
 *
 * \param alg An algorithm identifier (value of type #psa_algorithm_t).
 *
 * \return 1 if \p alg is a password-authenticated key exchange (PAKE)
 *         algorithm, 0 otherwise.
 *         This macro may return either 0 or 1 if \p alg is not a supported
 *         algorithm identifier.
 */
#define PSA_ALG_IS_PAKE(alg)                                        \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_PAKE)

/** The Password-authenticated key exchange by juggling (J-PAKE) algorithm.
 *
 * This is J-PAKE as defined by RFC 8236, instantiated with the following
 * parameters:
 *
 * - The group can be either an elliptic curve or defined over a finite field.
 * - Schnorr NIZK proof as defined by RFC 8235 and using the same group as the
 *   J-PAKE algorithm.
 * - A cryptographic hash function.
 *
 * To select these parameters and set up the cipher suite, call these functions
 * in any order:
 *
 * \code
 * psa_pake_cs_set_algorithm(cipher_suite, PSA_ALG_JPAKE(hash));
 * psa_pake_cs_set_primitive(cipher_suite,
 *                           PSA_PAKE_PRIMITIVE(type, family, bits));
 * \endcode
 *
 * For more information on how to set a specific curve or field, refer to the
 * documentation of the individual \c PSA_PAKE_PRIMITIVE_TYPE_XXX constants.
 *
 * After initializing a J-PAKE operation, call
 *
 * \code
 * psa_pake_setup(operation, key, cipher_suite);
 * psa_pake_set_user(operation, ...);
 * psa_pake_set_peer(operation, ...);
 * \endcode
 *
 * The password is provided as a key. This can be the password text itself,
 * in an agreed character encoding, or some value derived from the password
 * as required by a higher level protocol.
 *
 * (The implementation converts the key material to a number as described in
 * Section 2.3.8 of _SEC 1: Elliptic Curve Cryptography_
 * (https://www.secg.org/sec1-v2.pdf), before reducing it modulo \c q. Here
 * \c q is order of the group defined by the primitive set in the cipher suite.
 * The \c psa_pake_setup() function returns an error if the result of the
 * reduction is 0.)
 *
 * The key exchange flow for J-PAKE is as follows:
 * -# To get the first round data that needs to be sent to the peer, call
 *    \code
 *    // Get g1
 *    psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 *    // Get the ZKP public key for x1
 *    psa_pake_output(operation, #PSA_PAKE_STEP_ZK_PUBLIC, ...);
 *    // Get the ZKP proof for x1
 *    psa_pake_output(operation, #PSA_PAKE_STEP_ZK_PROOF, ...);
 *    // Get g2
 *    psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 *    // Get the ZKP public key for x2
 *    psa_pake_output(operation, #PSA_PAKE_STEP_ZK_PUBLIC, ...);
 *    // Get the ZKP proof for x2
 *    psa_pake_output(operation, #PSA_PAKE_STEP_ZK_PROOF, ...);
 *    \endcode
 * -# To provide the first round data received from the peer to the operation,
 *    call
 *    \code
 *    // Set g3
 *    psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 *    // Set the ZKP public key for x3
 *    psa_pake_input(operation, #PSA_PAKE_STEP_ZK_PUBLIC, ...);
 *    // Set the ZKP proof for x3
 *    psa_pake_input(operation, #PSA_PAKE_STEP_ZK_PROOF, ...);
 *    // Set g4
 *    psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 *    // Set the ZKP public key for x4
 *    psa_pake_input(operation, #PSA_PAKE_STEP_ZK_PUBLIC, ...);
 *    // Set the ZKP proof for x4
 *    psa_pake_input(operation, #PSA_PAKE_STEP_ZK_PROOF, ...);
 *    \endcode
 * -# To get the second round data that needs to be sent to the peer, call
 *    \code
 *    // Get A
 *    psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 *    // Get ZKP public key for x2*s
 *    psa_pake_output(operation, #PSA_PAKE_STEP_ZK_PUBLIC, ...);
 *    // Get ZKP proof for x2*s
 *    psa_pake_output(operation, #PSA_PAKE_STEP_ZK_PROOF, ...);
 *    \endcode
 * -# To provide the second round data received from the peer to the operation,
 *    call
 *    \code
 *    // Set B
 *    psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 *    // Set ZKP public key for x4*s
 *    psa_pake_input(operation, #PSA_PAKE_STEP_ZK_PUBLIC, ...);
 *    // Set ZKP proof for x4*s
 *    psa_pake_input(operation, #PSA_PAKE_STEP_ZK_PROOF, ...);
 *    \endcode
 * -# To access the shared secret call
 *    \code
 *    // Get Ka=Kb=K
 *    psa_pake_get_shared_key()
 *    \endcode
 *
 * For more information consult the documentation of the individual
 * \c PSA_PAKE_STEP_XXX constants.
 *
 * At this point there is a cryptographic guarantee that only the authenticated
 * party who used the same password is able to compute the key. But there is no
 * guarantee that the peer is the party it claims to be and was able to do so.
 *
 * That is, the authentication is only implicit (the peer is not authenticated
 * at this point, and no action should be taken that assume that they are - like
 * for example accessing restricted files).
 *
 * To make the authentication explicit there are various methods, see Section 5
 * of RFC 8236 for two examples.
 *
 * \note The JPAKE implementation has the following limitations:
 *       - The only supported primitive is ECC on the curve secp256r1, i.e.
 *         `PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
 *          PSA_ECC_FAMILY_SECP_R1, 256)`.
 *       - The only supported hash algorithm is SHA-256, i.e.
 *         `PSA_ALG_SHA_256`.
 */
#define PSA_ALG_JPAKE_BASE                      ((psa_algorithm_t) 0x0a000100)
#define PSA_ALG_JPAKE(hash_alg) (PSA_ALG_JPAKE_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_IS_JPAKE(alg) (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_JPAKE_BASE)

 /** The SPAKE2+ algorithm.
 *
 * SPAKE2+ is the augmented password-authenticated key exchange protocol,
 * defined by RFC9383. SPAKE2+ includes confirmation of the shared secret
 * key that results from the key exchange.
 * SPAKE2+ is required by Matter Specification, Version 1.2, as MATTER_PAKE.
 * Matter uses an earlier draft of the SPAKE2+ protocol: "SPAKE2+, an
 * Augmented PAKE (Draft 02)".
 * Although the operation of the PAKE is similar for both of these variants,
 * they have different key schedules for the derivation of the shared secret.
 *
 * When setting up a PAKE cipher suite to use the SPAKE2+ protocol defined
 * in RFC9383:
 * - For cipher-suites that use HMAC for key confirmation, use the
 *   PSA_ALG_SPAKE2P_HMAC() algorithm, parameterized by the required hash
 *   algorithm.
 * - For cipher-suites that use CMAC-AES-128 for key confirmation, use the
 *   PSA_ALG_SPAKE2P_CMAC() algorithm, parameterized by the required hash
 *   algorithm.
 * - Use a PAKE primitive for the required elliptic curve.
 * 
 * For example, the following code creates a cipher suite to select SPAKE2+
 * using edwards25519 with the SHA-256 hash function:
 *
 * \code
 * psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
 * psa_pake_cs_set_algorithm(cipher_suite, PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_256));
 * psa_pake_cs_set_primitive(&cipher_suite,
 *                           PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
 *                               PSA_ECC_FAMILY_TWISTED_EDWARDS, 255));
 * \endcode
 *
 * When setting up a PAKE cipher suite to use the SPAKE2+ protocol used by
 * Matter:
 * - Use the PSA_ALG_SPAKE2P_MATTER algorithm.
 * - Use the PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
 *                              PSA_ECC_FAMILY_SECP_R1, 256)
 *   PAKE primitive.
 * 
 * The following code creates a cipher suite to select the Matter variant of
 * SPAKE2+:
 * 
 * \code
 * psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
 * psa_pake_cs_set_algorithm(&cipher_suite, PSA_ALG_SPAKE2P_MATTER);
 * psa_pake_cs_set_primitive(&cipher_suite,
 *                           PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
 *                               PSA_ECC_FAMILY_SECP_R1, 256));
 * \endcode
 *
 * After initializing a SPAKE2+ operation, call
 *
 * \code
 * psa_pake_setup(operation, password, cipher_suite);
 * psa_pake_set_role(operation, ...);
 * \endcode
 * 
 * The password provided to the client side must be of type
 * #PSA_KEY_TYPE_SPAKE2P_KEY_PAIR.
 * The password provided to the server side must be of type
 * #PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY.
 *
 * The role set by \c psa_pake_set_role() must be either
 * \c PSA_PAKE_ROLE_CLIENT or \c PSA_PAKE_ROLE_SERVER.
 * 
 * Then provide any additional, optional parameters:
 * 
 * \code
 * psa_pake_set_user(operation, ...);
 * psa_pake_set_peer(operation, ...);
 * psa_pake_set_context(operation, ...);
 * \endcode
 * 
 *
 * The key exchange flow for a SPAKE2+ client is as follows:
 * \code
 * // send shareP
 * psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * // receive shareV
 * psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * // receive confirmV
 * psa_pake_input(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // send confirmP
 * psa_pake_output(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // get K_shared
 * psa_pake_get_shared_key(operation, ...);
 * \endcode
 *
 * The key exchange flow for a SPAKE2+ server is as follows:
 * \code
 * // receive shareP
 * psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * // send shareV
 * psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * // send confirmV
 * psa_pake_output(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // receive confirmP
 * psa_pake_input(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // get K_shared
 * psa_pake_get_shared_key(operation, ...);
 * \endcode
 *
 * The shared secret that is produced by SPAKE2+ is pseudorandom. Although
 * it can be used directly as an encryption key, it is recommended to use
 * the shared secret as an input to a key derivation operation to produce
 * additional cryptographic keys.
 */
#define PSA_ALG_SPAKE2P_HMAC_BASE               ((psa_algorithm_t) 0x0a000400)
#define PSA_ALG_SPAKE2P_HMAC(hash_alg) (PSA_ALG_SPAKE2P_HMAC_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_SPAKE2P_CMAC_BASE               ((psa_algorithm_t) 0x0a000500)
#define PSA_ALG_SPAKE2P_CMAC(hash_alg) (PSA_ALG_SPAKE2P_CMAC_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_SPAKE2P_MATTER                  ((psa_algorithm_t) 0x0A000609)
#define PSA_ALG_IS_SPAKE2P(alg) (((alg) & ~0x000003ff) == PSA_ALG_SPAKE2P_HMAC_BASE)
#define PSA_ALG_IS_SPAKE2P_HMAC(alg) (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_SPAKE2P_HMAC_BASE)
#define PSA_ALG_IS_SPAKE2P_CMAC(alg) (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_SPAKE2P_CMAC_BASE)

 /** The Secure Remote Passwort key exchange (SRP) algorithm.
 *
 * This is SRP-6 as defined by RFC 2945 and RFC 5054, instantiated with the
 * following parameters:
 *
 * - The group is defined over a finite field using a secure prime.
 * - A cryptographic hash function.
 *
 * To select these parameters and set up the cipher suite, call these functions:
 *
 * \code
 * psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
 * psa_pake_cs_set_algorithm(cipher_suite, PSA_ALG_SRP_6(hash));
 * psa_pake_cs_set_primitive(&cipher_suite,
 *                           PSA_PAKE_PRIMITIVE(type, family, bits));
 * \endcode
 *
 * After initializing a SRP operation, call:
 *
 * \code
 * psa_pake_setup(operation, password, cipher_suite);
 * psa_pake_set_role(operation, ...);
 * psa_pake_set_user(operation, ...);
 * \endcode
 * 
 * The password provided to the client side must be of type
 * #PSA_KEY_TYPE_SRP_KEY_PAIR.
 * The password provided to the server side must be of type
 * #PSA_KEY_TYPE_SRP_PUBLIC_KEY.
 *
 * The role set by \c psa_pake_set_role() must be either
 * \c PSA_PAKE_ROLE_CLIENT or \c PSA_PAKE_ROLE_SERVER.
 * 
 * For the SRP client key exchange call the following functions in any order:
 * \code
 * // get salt
 * psa_pake_input(operation, #PSA_PAKE_STEP_SALT, ...);
 * // get server key
 * psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * // write client key
 * psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * \endcode
 *
 * For the SRP server key exchange call the following functions in any order:
 * \code
 * // get salt
 * psa_pake_input(operation, #PSA_PAKE_STEP_SALT, ...);
 * // get client key
 * psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * // write server key
 * psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...);
 * \endcode
 *
 * For the client proof phase call the following functions in this order:
 * \code
 * // send M1
 * psa_pake_input(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // receive M2
 * psa_pake_output(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // Get secret
 * psa_pake_get_shared_key()
 * \endcode
 *
 * For the server proof phase call the following functions in this order:
 * \code
 * // receive M1
 * psa_pake_output(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // send M2
 * psa_pake_input(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // Get secret
 * psa_pake_get_shared_key()
 * \endcode
 *
 * The shared secret that is produced by SRP is pseudorandom. Although
 * it can be used directly as an encryption key, it is recommended to use
 * the shared secret as an input to a key derivation operation to produce
 * additional cryptographic keys.
 */
#define PSA_ALG_SRP_6_BASE                      ((psa_algorithm_t) 0x0a000300)
#define PSA_ALG_SRP_6(hash_alg) (PSA_ALG_SRP_6_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_IS_SRP_6(alg) (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_SRP_6_BASE)

/** The WPA3-SAE key exchange algorithm.
 *
 * This is WPA3-SAE as defined by "IEEE Std 802.11-REVme/D7.0 2024, 
 * chapter 12.4", including the Hash-To-Element (H2E) variant.
 * It is instantiated with the following parameters:
 *
 * - The group is defined over a finite field or an elliptic curve.
 * - A cryptographic hash function.
 *
 * For WPA3-SAE selected by the AKM suites 8 and 9, use the
 * PSA_ALG_WPA3_SAE_FIXED() algorithm, parameterized by the required hash
 * algorithm.
 * For WPA3-SAE selected by the AKM suites 24 and 25 (SAE using group-dependent
 * hash), use the PSA_ALG_WPA3_SAE_GDH() algorithm, parameterized by the
 * required hash algorithm.
 * 
 * To select these parameters and set up the cipher suite, call these functions:
 *
 * \code
 * psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
 * psa_pake_cs_set_algorithm(cipher_suite, PSA_ALG_WPA3_SAE_FIXED(hash));
 * psa_pake_cs_set_primitive(&cipher_suite,
 *                           PSA_PAKE_PRIMITIVE(type, family, bits));
 * \endcode
 *
 * After initializing a WPA3-SAE operation, call:
 *
 * \code
 * psa_pake_setup(operation, password, cipher_suite);
 * psa_pake_set_user(operation, ...);
 * psa_pake_set_peer(operation, ...);
 * \endcode
 * 
 * For basic SAE the password must be of type #PSA_KEY_TYPE_PASSWORD,
 * for SAE-H2E the password must be of type #PSA_KEY_TYPE_WPA3_SAE.
 *
 * \c psa_pake_set_role() must not be called because WPA3-SAE is a symmetric PAKE.
 * 
 * For a key exchange first call the following functions in any order:
 * \code
 * // send commit message
 * psa_pake_output(operation, #PSA_PAKE_STEP_COMMIT, ...);
 * // receive commit message
 * psa_pake_input(operation, #PSA_PAKE_STEP_COMMIT, ...);
 * \endcode
 *
 * If the Hash-To-Element variant is used and a list of rejected groups
 * is available, it must be provided as a salt:
 *
 * \code
 * // input salt
 * psa_pake_input(operation, #PSA_PAKE_STEP_SALT, ...);
 * \endcode
 *
 * Then call the following functions in any order:
 * \code
 * // set send-confirm counter
 * psa_pake_input(operation, #PSA_PAKE_STEP_CONFIRM_COUNT, ...);
 * // send confirm message
 * psa_pake_output(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // receive confirm message
 * psa_pake_input(operation, #PSA_PAKE_STEP_CONFIRM, ...);
 * // get key id (optional)
 * psa_pake_output(operation, #PSA_PAKE_STEP_KEY_ID, ...);
 * \endcode
 * 
 * Remarks:
 * \c psa_pake_input(#PSA_PAKE_STEP_CONFIRM_COUNT) must be called before
 * \c psa_pake_output(#PSA_PAKE_STEP_CONFIRM) to set the send-confirm counter.
 * The #PSA_PAKE_STEP_CONFIRM_COUNT and #PSA_PAKE_STEP_CONFIRM steps may be used
 * multiple times to handle repeated confirm messages with varying counts.
 *
 * Finally get the shared secret: 
 *
 * \code
 * // get secret
 * psa_pake_get_shared_key();
 * \endcode
 *
 * The shared secret produced by WPA3-SAE is pseudorandom.
 * It can be used directly as an encryption key or as input to a key derivation
 * operation.
 */
#define PSA_ALG_WPA3_SAE_FIXED_BASE       ((psa_algorithm_t)0x0a000800)
#define PSA_ALG_WPA3_SAE_FIXED(hash_alg)  (PSA_ALG_WPA3_SAE_FIXED_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_WPA3_SAE_GDH_BASE         ((psa_algorithm_t)0x0a000900)
#define PSA_ALG_WPA3_SAE_GDH(hash_alg)    (PSA_ALG_WPA3_SAE_GDH_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_IS_WPA3_SAE(alg)          (((alg) & ~0x000001ff) == PSA_ALG_WPA3_SAE_FIXED_BASE)
#define PSA_ALG_IS_WPA3_SAE_FIXED(alg)    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_WPA3_SAE_FIXED_BASE)
#define PSA_ALG_IS_WPA3_SAE_GDH(alg)      (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_WPA3_SAE_GDH_BASE)

/** A wildcard algorithm for WPA3-SAE password keys and password token keys.
 *
 * If a password key (key type #PSA_KEY_TYPE_PASSWORD) specifies
 * #PSA_ALG_WPA3_SAE_ANY as its permitted algorithm, then the key can be used
 * for any WPA3-SAE cipher suite with the #PSA_ALG_WPA3_SAE_H2E key-derivation
 * algorithm, and with the #PSA_ALG_WPA3_SAE_FIXED PAKE algorithm.
 */
#define PSA_ALG_WPA3_SAE_ANY              ((psa_algorithm_t)0x0a0088ff)
/** @} */

/** \defgroup pake Password-authenticated key exchange (PAKE)
 *
 * This is a proposed PAKE interface for the PSA Crypto API. It is not part of
 * the official PSA Crypto API yet.
 *
 * \note The content of this section is not part of the stable API and ABI
 *       of Mbed TLS and may change arbitrarily from version to version.
 *       Same holds for the corresponding macros #PSA_ALG_CATEGORY_PAKE and
 *       #PSA_ALG_JPAKE.
 * @{
 */

/** A value to indicate no role in a PAKE algorithm.
 * This value can be used in a call to psa_pake_set_role() for symmetric PAKE
 * algorithms which do not assign roles.
 */
#define PSA_PAKE_ROLE_NONE                  ((psa_pake_role_t) 0x00)

/** The first peer in a balanced PAKE.
 *
 * Although balanced PAKE algorithms are symmetric, some of them need an
 * ordering of peers for the transcript calculations. If the algorithm does not
 * need this, both #PSA_PAKE_ROLE_FIRST and #PSA_PAKE_ROLE_SECOND are
 * accepted.
 */
#define PSA_PAKE_ROLE_FIRST                ((psa_pake_role_t) 0x01)

/** The second peer in a balanced PAKE.
 *
 * Although balanced PAKE algorithms are symmetric, some of them need an
 * ordering of peers for the transcript calculations. If the algorithm does not
 * need this, either #PSA_PAKE_ROLE_FIRST or #PSA_PAKE_ROLE_SECOND are
 * accepted.
 */
#define PSA_PAKE_ROLE_SECOND                ((psa_pake_role_t) 0x02)

/** The client in an augmented PAKE.
 *
 * Augmented PAKE algorithms need to differentiate between client and server.
 */
#define PSA_PAKE_ROLE_CLIENT                ((psa_pake_role_t) 0x11)

/** The server in an augmented PAKE.
 *
 * Augmented PAKE algorithms need to differentiate between client and server.
 */
#define PSA_PAKE_ROLE_SERVER                ((psa_pake_role_t) 0x12)

/** The PAKE primitive type indicating the use of elliptic curves.
 *
 * The values of the \c family and \c bits fields of the cipher suite identify a
 * specific elliptic curve, using the same mapping that is used for ECC
 * (::psa_ecc_family_t) keys.
 *
 * (Here \c family means the value returned by PSA_PAKE_PRIMITIVE_GET_FAMILY() and
 * \c bits means the value returned by PSA_PAKE_PRIMITIVE_GET_BITS().)
 *
 * Input and output during the operation can involve group elements and scalar
 * values:
 * -# The format for group elements is the same as for public keys on the
 *  specific curve would be. For more information, consult the documentation of
 *  psa_export_public_key().
 * -# The format for scalars is the same as for private keys on the specific
 *  curve would be. For more information, consult the documentation of
 *  psa_export_key().
 */
#define PSA_PAKE_PRIMITIVE_TYPE_ECC       ((psa_pake_primitive_type_t) 0x01)

/** The PAKE primitive type indicating the use of Diffie-Hellman groups.
 *
 * The values of the \c family and \c bits fields of the cipher suite identify
 * a specific Diffie-Hellman group, using the same mapping that is used for
 * Diffie-Hellman (::psa_dh_family_t) keys.
 *
 * (Here \c family means the value returned by PSA_PAKE_PRIMITIVE_GET_FAMILY() and
 * \c bits means the value returned by PSA_PAKE_PRIMITIVE_GET_BITS().)
 *
 * Input and output during the operation can involve group elements and scalar
 * values:
 * -# The format for group elements is the same as for public keys on the
 *  specific group would be. For more information, consult the documentation of
 *  psa_export_public_key().
 * -# The format for scalars is the same as for private keys on the specific
 *  group would be. For more information, consult the documentation of
 *  psa_export_key().
 */
#define PSA_PAKE_PRIMITIVE_TYPE_DH       ((psa_pake_primitive_type_t) 0x02)

/** Construct a PAKE primitive from type, family and bit-size.
 *
 * \param pake_type     The type of the primitive
 *                      (value of type ::psa_pake_primitive_type_t).
 * \param pake_family   The family of the primitive
 *                      (the type and interpretation of this parameter depends
 *                      on \p pake_type, for more information consult the
 *                      documentation of individual ::psa_pake_primitive_type_t
 *                      constants).
 * \param pake_bits     The bit-size of the primitive
 *                      (Value of type \c size_t. The interpretation
 *                      of this parameter depends on \p pake_family, for more
 *                      information consult the documentation of individual
 *                      ::psa_pake_primitive_type_t constants).
 *
 * \return The constructed primitive value of type ::psa_pake_primitive_t.
 *         Return 0 if the requested primitive can't be encoded as
 *         ::psa_pake_primitive_t.
 */
#define PSA_PAKE_PRIMITIVE(pake_type, pake_family, pake_bits) \
    (((pake_bits & 0xFFFF) != pake_bits) ? 0 :                 \
     ((psa_pake_primitive_t) (((pake_type) << 24 |             \
                              (pake_family) << 16) | (pake_bits))))

#define PSA_PAKE_PRIMITIVE_GET_BITS(pake_primitive) \
    ((size_t)(pake_primitive & 0xFFFF))

#define PSA_PAKE_PRIMITIVE_GET_FAMILY(pake_primitive) \
    ((psa_pake_family_t)((pake_primitive >> 16) & 0xFF))

#define PSA_PAKE_PRIMITIVE_GET_TYPE(pake_primitive) \
    ((psa_pake_primitive_type_t)((pake_primitive >> 24) & 0xFF))

/** A key confirmation value that indicates a confirmed key in a PAKE cipher
 * suite.
 *
 * This key confirmation value will result in the PAKE algorithm exchanging
 * data to verify that the shared key is identical for both parties. This is
 * the default key confirmation value in an initialized PAKE cipher suite
 * object.
 * Some algorithms do not include confirmation of the shared key.
 */
#define PSA_PAKE_CONFIRMED_KEY 0

/** A key confirmation value that indicates an unconfirmed key in a PAKE cipher
 * suite.
 *
 * This key confirmation value will result in the PAKE algorithm terminating
 * prior to confirming that the resulting shared key is identical for both
 * parties.
 * Some algorithms do not support returning an unconfirmed shared key.
 */
#define PSA_PAKE_UNCONFIRMED_KEY 1
 
 /** The key share being sent to or received from the peer.
 *
 * The format for both input and output at this step is the same as for public
 * keys on the group determined by the primitive (::psa_pake_primitive_t) would
 * be.
 *
 * For more information on the format, consult the documentation of
 * psa_export_public_key().
 *
 * For information regarding how the group is determined, consult the
 * documentation #PSA_PAKE_PRIMITIVE.
 */
#define PSA_PAKE_STEP_KEY_SHARE                 ((psa_pake_step_t) 0x01)

/** A Schnorr NIZKP public key.
 *
 * This is the ephemeral public key in the Schnorr Non-Interactive
 * Zero-Knowledge Proof (the value denoted by the letter 'V' in RFC 8235).
 *
 * The format for both input and output at this step is the same as for public
 * keys on the group determined by the primitive (::psa_pake_primitive_t) would
 * be.
 *
 * For more information on the format, consult the documentation of
 * psa_export_public_key().
 *
 * For information regarding how the group is determined, consult the
 * documentation #PSA_PAKE_PRIMITIVE.
 */
#define PSA_PAKE_STEP_ZK_PUBLIC                 ((psa_pake_step_t) 0x02)

/** A Schnorr NIZKP proof.
 *
 * This is the proof in the Schnorr Non-Interactive Zero-Knowledge Proof (the
 * value denoted by the letter 'r' in RFC 8235).
 *
 * Both for input and output, the value at this step is an integer less than
 * the order of the group selected in the cipher suite. The format depends on
 * the group as well:
 *
 * - For Montgomery curves, the encoding is little endian.
 * - For everything else the encoding is big endian (see Section 2.3.8 of
 *   _SEC 1: Elliptic Curve Cryptography_ at https://www.secg.org/sec1-v2.pdf).
 *
 * In both cases leading zeroes are allowed as long as the length in bytes does
 * not exceed the byte length of the group order.
 *
 * For information regarding how the group is determined, consult the
 * documentation #PSA_PAKE_PRIMITIVE.
 */
#define PSA_PAKE_STEP_ZK_PROOF                  ((psa_pake_step_t) 0x03)

/** The key confirmation value.
 * 
 * This value is used during the key confirmation phase of a PAKE protocol.
 * The format of the value depends on the algorithm and cipher suite:
 *
 * For SPAKE2+ algorithms, the format for both input and output at this step is
 * the same as the output of the MAC algorithm specified in the cipher suite.
 *
 * For PSA_ALG_SRP_6, the format for both input and output at this step is
 * the same as the output of the hash algorithm specified.
 *
 * For WPA3_SAE algorithms, the format for both input and output at this step
 * is a 2 byte little-endian "send-confirm" counter followed by the output of
 * the hash algorithm specified.
 */
#define PSA_PAKE_STEP_CONFIRM                   ((psa_pake_step_t)0x04)

/** The salt.
 *
 * The format for both input and output at this step is plain binary data.
 */
#define PSA_PAKE_STEP_SALT                      ((psa_pake_step_t)0x05)

/** The WPA3-SAE commit step.
 *
 * The format for both input and output at this step is the scalar followed
 * by the element of the used group.
 */
#define PSA_PAKE_STEP_COMMIT                    ((psa_pake_step_t)0x06)

/** The WPA3-SAE send-confirm input step.
 *
 * The format for the input at this step is a 2 byte little-endian number
 * specifying the send-confirm counter to be used in the following confirm
 * output step.
 */
#define PSA_PAKE_STEP_CONFIRM_COUNT             ((psa_pake_step_t)0x07)

/** The WPA3-SAE key id output step.
 *
 * The format of the output at this step is a 16 byte key id (PMKID).
 */
#define PSA_PAKE_STEP_KEY_ID                    ((psa_pake_step_t)0x08)

/** Retrieve the PAKE algorithm from a PAKE cipher suite.
 *
 * \param[in] cipher_suite     The cipher suite structure to query.
 *
 * \return The PAKE algorithm stored in the cipher suite structure.
 */
static psa_algorithm_t psa_pake_cs_get_algorithm(
    const psa_pake_cipher_suite_t *cipher_suite);

/** Declare the PAKE algorithm for the cipher suite.
 *
 * This function overwrites any PAKE algorithm
 * previously set in \p cipher_suite.
 *
 * \note For #PSA_ALG_JPAKE, the only supported hash algorithm is SHA-256.
 *
 * \param[out] cipher_suite    The cipher suite structure to write to.
 * \param algorithm            The PAKE algorithm to write.
 *                             (`PSA_ALG_XXX` values of type ::psa_algorithm_t
 *                             such that #PSA_ALG_IS_PAKE(\c alg) is true.)
 *                             If this is 0, the PAKE algorithm in
 *                             \p cipher_suite becomes unspecified.
 */
static void psa_pake_cs_set_algorithm(psa_pake_cipher_suite_t *cipher_suite,
                                      psa_algorithm_t algorithm);

/** Retrieve the primitive from a PAKE cipher suite.
 *
 * \param[in] cipher_suite     The cipher suite structure to query.
 *
 * \return The primitive stored in the cipher suite structure.
 */
static psa_pake_primitive_t psa_pake_cs_get_primitive(
    const psa_pake_cipher_suite_t *cipher_suite);

/** Declare the primitive for a PAKE cipher suite.
 *
 * This function overwrites any primitive previously set in \p cipher_suite.
 *
 * \note For #PSA_ALG_JPAKE, the only supported primitive is ECC on the curve
 *       secp256r1, i.e. `PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC,
 *       PSA_ECC_FAMILY_SECP_R1, 256)`.
 *
 * \param[out] cipher_suite    The cipher suite structure to write to.
 * \param primitive            The primitive to write. If this is 0, the
 *                             primitive type in \p cipher_suite becomes
 *                             unspecified.
 */
static void psa_pake_cs_set_primitive(psa_pake_cipher_suite_t *cipher_suite,
                                      psa_pake_primitive_t primitive);

/** The type of the state data structure for PAKE operations.
 *
 * Before calling any function on a PAKE operation object, the application
 * must initialize it by any of the following means:
 * - Set the structure to all-bits-zero, for example:
 *   \code
 *   psa_pake_operation_t operation;
 *   memset(&operation, 0, sizeof(operation));
 *   \endcode
 * - Initialize the structure to logical zero values, for example:
 *   \code
 *   psa_pake_operation_t operation = {0};
 *   \endcode
 * - Initialize the structure to the initializer #PSA_PAKE_OPERATION_INIT,
 *   for example:
 *   \code
 *   psa_pake_operation_t operation = PSA_PAKE_OPERATION_INIT;
 *   \endcode
 * - Assign the result of the function psa_pake_operation_init()
 *   to the structure, for example:
 *   \code
 *   psa_pake_operation_t operation;
 *   operation = psa_pake_operation_init();
 *   \endcode
 *
 * This is an implementation-defined \c struct. Applications should not
 * make any assumptions about the content of this structure.
 * Implementation details can change in future versions without notice. */
typedef struct psa_pake_operation_s psa_pake_operation_t;

/** Return an initial value for a PAKE operation object.
 */
static psa_pake_operation_t psa_pake_operation_init(void);

/** Set the session information for a password-authenticated key exchange.
 *
 * The sequence of operations to set up a password-authenticated key exchange
 * is as follows:
 * -# Allocate an operation object which will be passed to all the functions
 *    listed here.
 * -# Initialize the operation object with one of the methods described in the
 *    documentation for #psa_pake_operation_t, e.g.
 *    #PSA_PAKE_OPERATION_INIT.
 * -# Call psa_pake_setup() to specify the password key and the cipher suite.
 * -# Call \c psa_pake_set_xxx() functions on the operation to complete the
 *    setup. The exact sequence of \c psa_pake_set_xxx() functions that needs
 *    to be called depends on the algorithm in use.
 *
 * Refer to the documentation of individual PAKE algorithm types (`PSA_ALG_XXX`
 * values of type ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true)
 * for more information.
 *
 * A typical sequence of calls to perform a password-authenticated key
 * exchange:
 * -# Call psa_pake_output(operation, #PSA_PAKE_STEP_KEY_SHARE, ...) to get the
 *    key share that needs to be sent to the peer.
 * -# Call psa_pake_input(operation, #PSA_PAKE_STEP_KEY_SHARE, ...) to provide
 *    the key share that was received from the peer.
 * -# Depending on the algorithm additional calls to psa_pake_output() and
 *    psa_pake_input() might be necessary.
 * -# Call psa_pake_get_shared_key() for accessing the shared secret.
 *
 * Refer to the documentation of individual PAKE algorithm types (`PSA_ALG_XXX`
 * values of type ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true)
 * for more information.
 *
 * If an error occurs at any step after a call to psa_pake_setup(),
 * the operation will need to be reset by a call to psa_pake_abort(). The
 * application may call psa_pake_abort() at any time after the operation
 * has been initialized.
 *
 * After a successful call to psa_pake_setup(), the application must
 * eventually terminate the operation. The following events terminate an
 * operation:
 * - A call to psa_pake_abort().
 * - A successful call to psa_pake_get_shared_key().
 *
 * \param[in,out] operation     The operation object to set up. It must have
 *                              been initialized but not set up yet.
 * \param[in] password_key      Identifier of the key holding the password or
 *                              a value derived from the password. It must
 *                              remain valid until the operation terminates.
 *                              The valid key types depend on the PAKE algorithm,
 *                              and participant role.
 * \param[in] cipher_suite      The cipher suite to use. (A cipher suite fully
 *                              characterizes a PAKE algorithm and determines
 *                              the algorithm as well.)
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_HANDLE
 *         \p password_key is not a valid key identifier.
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         The key does not have the #PSA_KEY_USAGE_DERIVE flag, or it does not
 *         permit the \p operation's algorithm.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The algorithm in \p cipher_suite is not a PAKE algorithm or encodes
 *         an invalid hash algorithm, or the PAKE primitive in \p cipher_suite
 *         is not compatible with the PAKE algorithm, or the key confirmation
 *         value in \p cipher_suite is not compatible with the PAKE algorithm
 *         and primitive, or the \p password_key is not compatible with
 *         \p cipher_suite.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The algorithm in \p cipher_suite is not a supported PAKE algorithm,
 *         or the PAKE primitive in \p cipher_suite is not supported or not
 *         compatible with the PAKE algorithm, or the key confirmation value
 *         in \p cipher_suite is not supported or not compatible with the PAKE
 *         algorithm and primitive, or the key type or key size of
 *         \p password_key is not supported with \p cipher_suite.
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid, or
 *         the library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_setup(psa_pake_operation_t *operation,
                            mbedtls_svc_key_id_t password_key,
                            const psa_pake_cipher_suite_t *cipher_suite);

/** Set the application role for a password-authenticated key exchange.
 *
 * Not all PAKE algorithms need to differentiate the communicating entities.
 * It is optional to call this function for PAKEs that don't require a role
 * to be specified. For such PAKEs the application role parameter is ignored,
 * or #PSA_PAKE_ROLE_NONE can be passed as \c role.
 *
 * Refer to the documentation of individual PAKE algorithm types (`PSA_ALG_XXX`
 * values of type ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true)
 * for more information.
 *
 * \param[in,out] operation     The operation object to specify the
 *                              application's role for. It must have been set up
 *                              by psa_pake_setup() and not yet in use (neither
 *                              psa_pake_output() nor psa_pake_input() has been
 *                              called yet). It must be an operation for which
 *                              the application's role hasn't been specified
 *                              (psa_pake_set_role() hasn't been called yet).
 * \param role                  A value of type ::psa_pake_role_t indicating the
 *                              application's role in the PAKE algorithm
 *                              that is being set up. For more information see
 *                              the documentation of \c PSA_PAKE_ROLE_XXX
 *                              constants.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The \p role is not a valid PAKE role in the \p operation’s algorithm.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The \p role for this algorithm is not supported or is not valid.
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid, or
 *         the library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_set_role(psa_pake_operation_t *operation,
    psa_pake_role_t role);

/** Set the user ID for a password-authenticated key exchange.
 *
 * Call this function to set the user ID. For PAKE algorithms that associate a
 * user identifier with each side of the session you need to call
 * psa_pake_set_peer() as well. For PAKE algorithms that associate a single
 * user identifier with the session, call psa_pake_set_user() only.
 *
 * Refer to the documentation of individual PAKE algorithm types (`PSA_ALG_XXX`
 * values of type ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true)
 * for more information.
 *
 * \param[in,out] operation     The operation object to set the user ID for. It
 *                              must have been set up by psa_pake_setup() and
 *                              not yet in use (neither psa_pake_output() nor
 *                              psa_pake_input() has been called yet). It must
 *                              be on operation for which the user ID hasn't
 *                              been set (psa_pake_set_user() hasn't been
 *                              called yet).
 * \param[in] user_id           The user ID to authenticate with.
 * \param user_id_len           Size of the \p user_id buffer in bytes.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p user_id is not valid for the \p operation's algorithm and cipher
 *         suite.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The value of \p user_id is not supported by the implementation.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid, or
 *         the library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_set_user(psa_pake_operation_t *operation,
                               const uint8_t *user_id,
                               size_t user_id_len);

/** Set the peer ID for a password-authenticated key exchange.
 *
 * Call this function in addition to psa_pake_set_user() for PAKE algorithms
 * that associate a user identifier with each side of the session. For PAKE
 * algorithms that associate a single user identifier with the session, call
 * psa_pake_set_user() only.
 *
 * Refer to the documentation of individual PAKE algorithm types (`PSA_ALG_XXX`
 * values of type ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true)
 * for more information.
 *
 * \param[in,out] operation     The operation object to set the peer ID for. It
 *                              must have been set up by psa_pake_setup() and
 *                              not yet in use (neither psa_pake_output() nor
 *                              psa_pake_input() has been called yet). It must
 *                              be on operation for which the peer ID hasn't
 *                              been set (psa_pake_set_peer() hasn't been
 *                              called yet).
 * \param[in] peer_id           The peer's ID to authenticate.
 * \param peer_id_len           Size of the \p peer_id buffer in bytes.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p peer_id is not valid for the \p operation's algorithm and cipher
 *         suite.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The algorithm doesn't associate a second identity with the session.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         Calling psa_pake_set_peer() is invalid with the \p operation's
 *         algorithm, the operation state is not valid, or the library has not
 *         been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_set_peer(psa_pake_operation_t *operation,
                               const uint8_t *peer_id,
                               size_t peer_id_len);

/** Set the context data for a password-authenticated key exchange.
 *
 * Call this function for PAKE algorithms that accept additional context data
 * as part of the protocol setup.
 *
 * Refer to the documentation of individual PAKE algorithm types (`PSA_ALG_XXX`
 * values of type ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true)
 * for more information.
 *
 * \param[in,out] operation     The operation object to set the context for. It
 *                              must have been set up by psa_pake_setup() and
 *                              not yet in use (neither psa_pake_output() nor
 *                              psa_pake_input() has been called yet). It must
 *                              be on operation for which the context hasn't
 *                              been set (psa_pake_set_context() hasn't been
 *                              called yet).
 * \param[in] context           The context.
 * \param context_len           Size of the \p context buffer in bytes.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The \p context is not valid for the operation’s algorithm and cipher suite.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The \p context is not supported by the implementation.
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         Calling psa_pake_set_context() is invalid with the \p operation's
 *         algorithm, the operation state is not valid, or the library has not
 *         been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_set_context(psa_pake_operation_t *operation,
                                  const uint8_t *context,
                                  size_t context_len);

/** Get output for a step of a password-authenticated key exchange.
 *
 * Depending on the algorithm being executed, you might need to call this
 * function several times or you might not need to call this at all.
 *
 * The exact sequence of calls to perform a password-authenticated key
 * exchange depends on the algorithm in use.  Refer to the documentation of
 * individual PAKE algorithm types (`PSA_ALG_XXX` values of type
 * ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true) for more
 * information.
 *
 * If this function returns an error status, the operation enters an error
 * state and must be aborted by calling psa_pake_abort().
 *
 * \param[in,out] operation    Active PAKE operation.
 * \param step                 The step of the algorithm for which the output
 *                             is requested.
 * \param[out] output          Buffer where the output is to be written in the
 *                             format appropriate for this \p step. Refer to
 *                             the documentation of the individual
 *                             \c PSA_PAKE_STEP_XXX constants for more
 *                             information.
 * \param output_size          Size of the \p output buffer in bytes. This must
 *                             be at least #PSA_PAKE_OUTPUT_SIZE(\c alg, \c
 *                             primitive, \p output_step) where \c alg and
 *                             \p primitive are the PAKE algorithm and primitive
 *                             in the operation's cipher suite, and \p step is
 *                             the output step.
 *
 * \param[out] output_length   On success, the number of bytes of the returned
 *                             output.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_BUFFER_TOO_SMALL
 *         The size of the \p output buffer is too small.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p step is not compatible with the operation's algorithm.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p step is not supported with the operation's algorithm.
 * \retval #PSA_ERROR_INSUFFICIENT_ENTROPY \emptydescription
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_STORAGE_FAILURE \emptydescription
 * \retval #PSA_ERROR_DATA_CORRUPT \emptydescription
 * \retval #PSA_ERROR_DATA_INVALID \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid (it must be active, and fully set
 *         up, and this call must conform to the algorithm's requirements
 *         for ordering of input and output steps), or the library has not
 *         been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_output(psa_pake_operation_t *operation,
                             psa_pake_step_t step,
                             uint8_t *output,
                             size_t output_size,
                             size_t *output_length);

/** Provide input for a step of a password-authenticated key exchange.
 *
 * Depending on the algorithm being executed, you might need to call this
 * function several times or you might not need to call this at all.
 *
 * The exact sequence of calls to perform a password-authenticated key
 * exchange depends on the algorithm in use.  Refer to the documentation of
 * individual PAKE algorithm types (`PSA_ALG_XXX` values of type
 * ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true) for more
 * information.
 *
 * If this function returns an error status, the operation enters an error
 * state and must be aborted by calling psa_pake_abort().
 *
 * \param[in,out] operation    Active PAKE operation.
 * \param step                 The step for which the input is provided.
 * \param[in] input            Buffer containing the input in the format
 *                             appropriate for this \p step. Refer to the
 *                             documentation of the individual
 *                             \c PSA_PAKE_STEP_XXX constants for more
 *                             information.
 * \param input_length         Size of the \p input buffer in bytes.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_SIGNATURE
 *         The verification fails for a #PSA_PAKE_STEP_ZK_PROOF input step.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p step is not compatible with the operation's algorithm, or
 *         \p input_length is not compatible with the \p operation’s algorithm,
 *         or the \p input is not valid for the \p operation's algorithm,
 *         cipher suite or \p step.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p step is not supported with the operation's algorithm, or
 *         \p step p is not supported with the \p operation's algorithm, or the
 *         \p input is not supported for the \p operation's algorithm, cipher
 *         suite or \p step.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_STORAGE_FAILURE \emptydescription
 * \retval #PSA_ERROR_DATA_CORRUPT \emptydescription
 * \retval #PSA_ERROR_DATA_INVALID \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid (it must be active, and fully set
 *         up, and this call must conform to the algorithm's requirements
 *         for ordering of input and output steps), or the library has not
 *         been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_input(psa_pake_operation_t *operation,
                            psa_pake_step_t step,
                            const uint8_t *input,
                            size_t input_length);

/** Get shared secret from a PAKE.
 *
 * This is the final call in a PAKE operation, which retrieves the shared
 * secret as a key. It is recommended that this key is used as an input to a
 * key derivation operation to produce additional cryptographic keys. For
 * some PAKE algorithms, the shared secret is also suitable for use as a key
 * in cryptographic operations such as encryption. Refer to the documentation
 * of individual PAKE algorithm types (`PSA_ALG_XXX` values of type
 * ::psa_algorithm_t such that #PSA_ALG_IS_PAKE(\c alg) is true) for more
 * information.
 *
 * Depending on the key confirmation requested in the cipher suite,
 * psa_pake_get_shared_key() must be called either before or after the
 * key-confirmation output and input steps for the PAKE algorithm. The key
 * confirmation affects the guarantees that can be made about the shared key:
 * 
 * Unconfirmed key
 * If the cipher suite used to set up the operation requested an unconfirmed
 * key, the application must call psa_pake_get_shared_key() after the
 * key-exchange output and input steps are completed. The PAKE algorithm
 * provides a cryptographic guarantee that only a peer who used the same
 * password, and identity inputs, is able to compute the same key. However,
 * there is no guarantee that the peer is the participant it claims to be,
 * and was able to compute the same key.
 * Since the peer is not authenticated, no action should be taken that assumes
 * that the peer is who it claims to be. For example, do not access restricted
 * files on the peer’s behalf until an explicit authentication has succeeded.
 * Note:
 * Some PAKE algorithms do not enable the output of the shared secret until it
 * has been confirmed.
 * 
 * Confirmed key
 * If the cipher suite used to set up the operation requested a confirmed key,
 * the application must call psa_pake_get_shared_key() after the key-exchange
 * and key-confirmation output and input steps are completed.
 * Following key confirmation, the PAKE algorithm provides a cryptographic
 * guarantee that the peer used the same password and identity inputs, and has
 * computed the identical shared secret key.
 * Since the peer is not authenticated, no action should be taken that assumes
 * that the peer is who it claims to be. For example, do not access restricted
 * files on the peer’s behalf until an explicit authentication has succeeded.
 * Note:
 * Some PAKE algorithms do not include any key-confirmation steps.
 * 
 * The exact sequence of calls to perform a password-authenticated key
 * exchange depends on the algorithm in use.  
 *
 * When this function returns successfully, \p operation becomes inactive.
 * If this function returns an error status, both \p operation
 * and \c key_derivation operations enter an error state and must be aborted
 * by calling psa_pake_abort().
 *
 * \param[in,out] operation    Active PAKE operation.
 * \param[in] attributes       The attributes for the new key.
 * \param[out] key             On success, an identifier for the newly created
 *                             key. #PSA_KEY_ID_NULL on failure.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         The implementation does not permit creating a key with the
 *         specified attributes due to some implementation-specific policy.
 * \retval #PSA_ERROR_ALREADY_EXISTS
 *         This is an attempt to create a persistent key, and there is
 *         already a persistent key with the given identifier.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The key type is not valid for output from this operation’s
 *         algorithm, or the key size is nonzero, or the key lifetime is
 *         invalid, the key identifier is not valid for the key lifetime,
 *         or the key usage flags include invalid values, or the key’s
 *         permitted-usage algorithm is invalid, or the key attributes,
 *         as a whole, are invalid.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The key attributes, as a whole, are not supported for creation
 *         from a PAKE secret, either by the implementation in general or
 *         in the specified storage location.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_STORAGE_FAILURE \emptydescription
 * \retval #PSA_ERROR_DATA_CORRUPT \emptydescription
 * \retval #PSA_ERROR_DATA_INVALID \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The PAKE operation state is not valid (it must be ready to return
 *         the shared secret), or the library has not been previously
 *         initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_get_shared_key(psa_pake_operation_t *operation,
                                     const psa_key_attributes_t *attributes,
                                     mbedtls_svc_key_id_t *key);

/** Abort a PAKE operation.
 *
 * Aborting an operation frees all associated resources except for the \c
 * operation structure itself. Once aborted, the operation object can be reused
 * for another operation by calling psa_pake_setup() again.
 *
 * This function may be called at any time after the operation
 * object has been initialized as described in #psa_pake_operation_t.
 *
 * In particular, calling psa_pake_abort() after the operation has been
 * terminated by a call to psa_pake_abort() or psa_pake_get_shared_key()
 * is safe and has no effect.
 *
 * \param[in,out] operation    The operation to abort.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The library has not been previously initialized by psa_crypto_init().
 *         It is implementation-dependent whether a failure to initialize
 *         results in this error code.
 */
psa_status_t psa_pake_abort(psa_pake_operation_t *operation);

/**@}*/

/** A sufficient output buffer size for psa_pake_output().
 *
 * If the size of the output buffer is at least this large, it is guaranteed
 * that psa_pake_output() will not fail due to an insufficient output buffer
 * size. The actual size of the output might be smaller in any given call.
 *
 * See also #PSA_PAKE_OUTPUT_MAX_SIZE
 *
 * \param alg           A PAKE algorithm (\c PSA_ALG_XXX value such that
 *                      #PSA_ALG_IS_PAKE(\p alg) is true).
 * \param primitive     A primitive of type ::psa_pake_primitive_t that is
 *                      compatible with algorithm \p alg.
 * \param output_step   A value of type ::psa_pake_step_t that is valid for the
 *                      algorithm \p alg.
 * \return              A sufficient output buffer size for the specified
 *                      PAKE algorithm, primitive, and output step. If the
 *                      PAKE algorithm, primitive, or output step is not
 *                      recognized, or the parameters are incompatible,
 *                      return 0.
 */
#define PSA_PAKE_OUTPUT_SIZE(alg, primitive, output_step)               \
    (output_step == PSA_PAKE_STEP_KEY_SHARE ? \
        PSA_PAKE_PRIMITIVE_GET_TYPE(primitive) == PSA_PAKE_PRIMITIVE_TYPE_DH ? \
            PSA_BITS_TO_BYTES(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) : \
            PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) : \
     output_step == PSA_PAKE_STEP_ZK_PUBLIC ? \
        PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) : \
     output_step == PSA_PAKE_STEP_ZK_PROOF ? \
        PSA_BITS_TO_BYTES(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) : \
     output_step == PSA_PAKE_STEP_COMMIT ? \
        PSA_BITS_TO_BYTES(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) * 3 : \
     output_step == PSA_PAKE_STEP_CONFIRM ? \
        PSA_ALG_IS_SPAKE2P_CMAC(alg) ? \
            PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC) : \
            PSA_HASH_LENGTH(alg) + (PSA_ALG_IS_WPA3_SAE(alg) ? 2 : 0) : \
     output_step == PSA_PAKE_STEP_KEY_ID ? \
        16u : \
     0u)

/** A sufficient input buffer size for psa_pake_input().
 *
 * The value returned by this macro is guaranteed to be large enough for any
 * valid input to psa_pake_input() in an operation with the specified
 * parameters.
 *
 * See also #PSA_PAKE_INPUT_MAX_SIZE
 *
 * \param alg           A PAKE algorithm (\c PSA_ALG_XXX value such that
 *                      #PSA_ALG_IS_PAKE(\p alg) is true).
 * \param primitive     A primitive of type ::psa_pake_primitive_t that is
 *                      compatible with algorithm \p alg.
 * \param input_step    A value of type ::psa_pake_step_t that is valid for the
 *                      algorithm \p alg.
 * \return              A sufficient input buffer size for the specified
 *                      input, cipher suite and algorithm. If the cipher suite,
 *                      the input type or PAKE algorithm is not recognized, or
 *                      the parameters are incompatible, return 0.
 */
#define PSA_PAKE_INPUT_SIZE(alg, primitive, input_step)                 \
    (input_step == PSA_PAKE_STEP_KEY_SHARE ? \
        PSA_PAKE_PRIMITIVE_GET_TYPE(primitive) == PSA_PAKE_PRIMITIVE_TYPE_DH ? \
            PSA_BITS_TO_BYTES(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) : \
            PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) : \
     input_step == PSA_PAKE_STEP_ZK_PUBLIC ? \
        PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) : \
     input_step == PSA_PAKE_STEP_ZK_PROOF ? \
        PSA_BITS_TO_BYTES(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) : \
     input_step == PSA_PAKE_STEP_COMMIT ? \
        PSA_BITS_TO_BYTES(PSA_PAKE_PRIMITIVE_GET_BITS(primitive)) * 3 : \
     input_step == PSA_PAKE_STEP_CONFIRM ? \
        PSA_ALG_IS_SPAKE2P_CMAC(alg) ? \
            PSA_MAC_LENGTH(PSA_KEY_TYPE_AES, 128, PSA_ALG_CMAC) : \
            PSA_HASH_LENGTH(alg) + (PSA_ALG_IS_WPA3_SAE(alg) ? 2 : 0) : \
     input_step == PSA_PAKE_STEP_SALT ? \
        64u : \
     input_step == PSA_PAKE_STEP_CONFIRM_COUNT ? \
        2u : \
     0u)

/** Output buffer size for psa_pake_output() for any of the supported PAKE
 * algorithm and primitive suites and output step.
 *
 * This macro must expand to a compile-time constant integer.
 *
 * The value of this macro must be at least as large as the largest value
 * returned by PSA_PAKE_OUTPUT_SIZE()
 *
 * See also #PSA_PAKE_OUTPUT_SIZE(\p alg, \p primitive, \p output_step).
 */
#ifdef PSA_WANT_ALG_SRP_6
#define PSA_PAKE_OUTPUT_MAX_SIZE PSA_BITS_TO_BYTES(PSA_VENDOR_FFDH_MAX_KEY_BITS)
#else
#if defined(PSA_WANT_ALG_WPA3_SAE_FIXED) || defined(PSA_WANT_ALG_WPA3_SAE_GDH)
#define PSA_PAKE_OUTPUT_MAX_SIZE (PSA_BITS_TO_BYTES(PSA_VENDOR_ECC_MAX_CURVE_BITS) * 3 + 2)
#else
#define PSA_PAKE_OUTPUT_MAX_SIZE PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_VENDOR_ECC_MAX_CURVE_BITS)
#endif
#endif

/** Input buffer size for psa_pake_input() for any of the supported PAKE
 * algorithm and primitive suites and input step.
 *
 * This macro must expand to a compile-time constant integer.
 *
 * The value of this macro must be at least as large as the largest value
 * returned by PSA_PAKE_INPUT_SIZE()
 *
 * See also #PSA_PAKE_INPUT_SIZE(\p alg, \p primitive, \p output_step).
 */
#ifdef PSA_WANT_ALG_SRP_6
#define PSA_PAKE_INPUT_MAX_SIZE PSA_BITS_TO_BYTES(PSA_VENDOR_FFDH_MAX_KEY_BITS)
#else
#if defined(PSA_WANT_ALG_WPA3_SAE_FIXED) || defined(PSA_WANT_ALG_WPA3_SAE_GDH)
#define PSA_PAKE_INPUT_MAX_SIZE (PSA_BITS_TO_BYTES(PSA_VENDOR_ECC_MAX_CURVE_BITS) * 3 + 2)
#else
#define PSA_PAKE_INPUT_MAX_SIZE PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_VENDOR_ECC_MAX_CURVE_BITS)
#endif
#endif

static inline psa_algorithm_t psa_pake_cs_get_algorithm(
    const psa_pake_cipher_suite_t *cipher_suite)
{
    return cipher_suite->algorithm;
}

static inline void psa_pake_cs_set_algorithm(
    psa_pake_cipher_suite_t *cipher_suite,
    psa_algorithm_t algorithm)
{
    if (!PSA_ALG_IS_PAKE(algorithm)) {
        cipher_suite->algorithm = 0;
    } else {
        cipher_suite->algorithm = algorithm;
    }
}

static inline psa_pake_primitive_t psa_pake_cs_get_primitive(
    const psa_pake_cipher_suite_t *cipher_suite)
{
    return cipher_suite->primitive;
}

static inline void psa_pake_cs_set_primitive(
    psa_pake_cipher_suite_t *cipher_suite,
    psa_pake_primitive_t primitive)
{
    cipher_suite->primitive = primitive;
}

static inline uint32_t psa_pake_cs_get_key_confirmation(
    const psa_pake_cipher_suite_t* cipher_suite)
{
    return cipher_suite->key_confirmation;
}

static inline void psa_pake_cs_set_key_confirmation(
    psa_pake_cipher_suite_t* cipher_suite,
    uint32_t key_confirmation)
{
    cipher_suite->key_confirmation = key_confirmation;
}


/* Key Wrapping Interface */

#define PSA_ALG_CATEGORY_KEY_WRAP  ((psa_algorithm_t) 0x0B000000)

/** Whether the specified algorithm is a key wrap algorithm.
 *
 * \param alg An algorithm identifier (value of type #psa_algorithm_t).
 *
 * \return 1 if \p alg is a key wrap algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_KEY_WRAP(alg)   \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_KEY_WRAP)

/** The AES Key Wrap algorithm.
 *
 * This is AES-KW as defined by NIST-SP-800-38F and RFC3394.
 * For AES-KW, the size of the input key must be >= 16 and a multiple of 8.
 */
#define PSA_ALG_KW                    ((psa_algorithm_t) 0x0B400100)

/** The AES Key Wrap with padding algorithm.
 *
 * This is AES-KWP as defined by NIST-SP-800-38F and RFC5649.
 * The S bit is set to indicate acceptance of non-aligned key sizes.
 */
#define PSA_ALG_KWP                   ((psa_algorithm_t) 0x0BC00200)

/** Whether the key may be used to wrap another key.
 *
 * This flag allows the key to be used as a wrapping key for a key wrapping
 * operation, if otherwise permitted by the key's type and policy.
 */
#define PSA_KEY_USAGE_WRAP            ((psa_key_usage_t) 0x00010000)

/** Whether the key may be used to unwrap an encoded key.
 *
 * This flag allows the key to be used as a wrapping key for a key unwrapping
 * operation, if otherwise permitted by the key's type and policy.
 */
#define PSA_KEY_USAGE_UNWRAP          ((psa_key_usage_t) 0x00020000)

 /** A sufficient output buffer size for psa_wrap_key().
 *
 * If the size of the output buffer is at least this large, it is guaranteed
 * that psa_wrap_key() will not fail due to an insufficient output buffer
 * size. The actual size of the output might be smaller in any given call.
 *
 * See also #OBERON_PSA_WRAP_KEY_PAIR_MAX_SIZE
 *
 * \param wrap_key_type A wrap key type that is compatible with algorithm
 *                      \p alg.
 * \param alg           A key wrap algorithm (\c PSA_ALG_XXX value such that
 *                      #PSA_ALG_IS_KEY_WRAP(\p alg) is true).
 * \param key_type      An input key type that is compatible with algorithm
 *                      \p alg.
 * \param key_bits      The size of the input key in bits.
 * \return              A sufficient output buffer size for the specified
 *                      key wrap algorithm, wrap_key_type, key_type, and
 *                      key_bits. If the parameters are not recognized or
 *                      incompatible, return 0.
 */
#define OBERON_PSA_WRAP_KEY_OUTPUT_SIZE(wrap_key_type, alg, key_type, key_bits) \
    ((alg) == PSA_ALG_KW ? PSA_BITS_TO_BYTES(key_bits) + 8u : \
     (alg) == PSA_ALG_KWP ? ((PSA_BITS_TO_BYTES(key_bits) + 7u) & ~7u) + 8u : 0u)

/** Sufficient output buffer size for wrapping any asymmetric key pair.
 *
 * This macro expands to a compile-time constant integer. This value is
 * a sufficient buffer size when calling psa_wrap_key() to wrap any
 * asymmetric key pair, regardless of the exact key type and key size.
 *
 * See also #OBERON_PSA_WRAP_KEY_OUTPUT_SIZE(\p key_type, \p key_bits).
 */
#define OBERON_PSA_WRAP_KEY_PAIR_MAX_SIZE (((PSA_EXPORT_KEY_PAIR_MAX_SIZE + 7u) & ~7u) + 8u)

/** Export a key in a wrapped format.
 *
 * The output of this function can be passed to psa_unwrap_key() to
 * create an equivalent object.
 *
 * The key to be wrapped is encrypted using the given key wrapping algorithm.
 *
 * \param wrapping_key      Identifier of the key to wrap the input key. It
 *                          must allow the usage #PSA_KEY_USAGE_WRAP.
 * \param alg               The key wrapping algorithm to use
 *                          (\c PSA_ALG_XXX value such that
 *                          #PSA_ALG_IS_KEY_WRAP(\p alg) is true).
 * \param key               Identifier of the key to be wrapped. It must allow
 *                          the usage #PSA_KEY_USAGE_EXPORT.
 * \param[out] data         Buffer where the wrapped key data is to be written.
 * \param data_size         Size of the \p data buffer in bytes.
 * \param[out] data_length  On success, the number of bytes
 *                          that make up the wrapped data.
 *
 * \retval #PSA_SUCCESS \emptydescription
 * \retval #PSA_ERROR_INVALID_HANDLE \emptydescription
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         \p key does not have the #PSA_KEY_USAGE_EXPORT flag.
 *         \p wrapping_key does not have the #PSA_KEY_USAGE_WRAP flag.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported. 
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p key is not compatible with \p alg.
 * \retval #PSA_ERROR_BUFFER_TOO_SMALL
 *         The size of the \p data buffer is too small.
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_HARDWARE_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_STORAGE_FAILURE \emptydescription
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_BAD_STATE \emptydescription
 */
psa_status_t psa_wrap_key(
    mbedtls_svc_key_id_t wrapping_key,
    psa_algorithm_t alg,
    mbedtls_svc_key_id_t key,
    uint8_t *data,
    size_t data_size,
    size_t *data_length);

/** Import a key in a wrapped format.
 *
 * This function supports wrapped keys as output from psa_wrap_key().
 *
 * \param attributes        The attributes for the new key.
 * \param wrapping_key      Identifier of the key to unwrap the input key. It
 *                          must allow the usage #PSA_KEY_USAGE_UNWRAP.
 * \param alg               The key wrapping algorithm to use
 *                          (\c PSA_ALG_XXX value such that
 *                          #PSA_ALG_IS_KEY_WRAP(\p alg) is true).
 * \param data              Buffer containing the wrapped key data.
 * \param data_length       Size of the \p data buffer in bytes.
 * \param[out] key          On success, an identifier for the newly created
 *                          key. #PSA_KEY_ID_NULL on failure.
 *
 * \retval #PSA_SUCCESS \emptydescription
 * \retval #PSA_ERROR_INVALID_HANDLE \emptydescription
 * \retval #PSA_ERROR_NOT_PERMITTED
 *         \p wrapping_key does not have the #PSA_KEY_USAGE_UNWRAP flag.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported. 
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The wrapped data is not correctly formatted.
 * \retval #PSA_ERROR_INVALID_SIGNATURE
 *         Authentication of the wrapped key data failed.
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_HARDWARE_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_STORAGE_FAILURE \emptydescription
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_BAD_STATE \emptydescription
 */
psa_status_t psa_unwrap_key(
    const psa_key_attributes_t *attributes,
    mbedtls_svc_key_id_t wrapping_key,
    psa_algorithm_t alg,
    const uint8_t *data,
    size_t data_length,
    mbedtls_svc_key_id_t *key);


/** The SHA-256/192 message digest algorithm.
 *
 * SHA-256/192 is the first 192 bits (24 bytes) of the SHA-256 output.
 * SHA-256 is defined in [FIPS180-4].
 */
#define PSA_ALG_SHA_256_192 ((psa_algorithm_t)0x0200000E)

/** The SHAKE128/256 message digest algorithm.
 *
 * SHAKE128/256 is the first 256 bits (32 bytes) of the SHAKE128 output.
 * SHAKE128 is defined in [FIPS202].
 */
#define PSA_ALG_SHAKE128_256 ((psa_algorithm_t)0x02000016)

/** The SHAKE256/192 message digest algorithm.
 *
 * SHAKE256/192 is the first 192 bits (24 bytes) of the SHAKE256 output.
 * SHAKE256 is defined in [FIPS202].
 */
#define PSA_ALG_SHAKE256_192 ((psa_algorithm_t)0x02000017)

/** The SHAKE256/256 message digest algorithm.
 *
 * SHAKE256/256 is the first 256 bits (32 bytes) of the SHAKE256 output.
 * SHAKE256 is defined in [FIPS202].
 */
#define PSA_ALG_SHAKE256_256 ((psa_algorithm_t)0x02000018)


/** The type of the state object for multi-part XOF operations.
 *
 * Before calling any function on an XOF operation object, the application
 * must initialize it by any of the following means:
 * - Set the structure to all-bits-zero, for example:
 *   \code
 *   psa_xof_operation_t operation;
 *   memset(&operation, 0, sizeof(operation));
 *   \endcode
 * - Initialize the structure to logical zero values, for example:
 *   \code
 *   psa_xof_operation_t operation = {0};
 *   \endcode
 * - Initialize the structure to the initializer #PSA_XOF_OPERATION_INIT,
 *   for example:
 *   \code
 *   psa_xof_operation_t operation = PSA_XOF_OPERATION_INIT;
 *   \endcode
 * - Assign the result of the function psa_xof_operation_init()
 *   to the structure, for example:
 *   \code
 *   psa_xof_operation_t operation;
 *   operation = psa_xof_operation_init();
 *   \endcode
 *
 * This is an implementation-defined \c struct. Applications should not
 * make any assumptions about the content of this structure.
 * Implementation details can change in future versions without notice. */
typedef struct psa_xof_operation_s psa_xof_operation_t;

/** Return an initial value for an XOF operation object.
 */
static psa_xof_operation_t psa_xof_operation_init(void);

/** Set up an XOF operation.
 *
 * The sequence of operations to generate XOF output is as follows:
 * -# Allocate an XOF operation object which will be passed to all the
 *    functions listed here.
 * -# Initialize the operation object with one of the methods described in
 *    the documentation for #psa_xof_operation_t, e.g.
 *    #PSA_XOF_OPERATION_INIT.
 * -# Call psa_xof_setup() to specify the required XOF algorithm.
 * -# Call psa_xof_update() zero, one, or more times, passing a fragment of
 *    the input each time.
 * -# To extract XOF output data, call psa_xof_output() one or more times.
 * -# Finally, call psa_xof_abort() to end the operation.
 * 
 * After a successful call to psa_xof_setup(), the operation is active, and
 * the application must eventually terminate the operation with a call to
 * psa_xof_abort().
 *
 * If psa_xof_setup() returns an error, the operation object is unchanged.
 * If a subsequent function call with an active operation returns an error,
 * the operation enters an error state.
 *
 * \param[in,out] operation     The operation object to set up. It must have
 *                              been initialized and not yet in use.
 * \param[in] alg               The XOF algorithm to compute: a value of type
 *                              psa_algorithm_t such that PSA_ALG_IS_XOF(alg)
 *                              is true.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         \p alg is not supported or is not an XOF algorithm.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \p alg is not an XOF algorithm.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid: it must be inactive, or
 *         the library requires initializing by a call to psa_crypto_init().
 */
psa_status_t psa_xof_setup(psa_xof_operation_t *operation,
                           psa_algorithm_t alg);

/** Provide a context for a multi-part XOF operation.
 * 
 * The application must call psa_xof_setup() before calling this function.
 * For an XOF algorithm with a context parameter, this function must be called
 * immediately after psa_xof_setup(), before calling any other function on the
 * XOF operation.
 * 
 * This function must not be called if the XOF algorithm does not have a
 * context parameter. The macro PSA_ALG_XOF_HAS_CONTEXT() can be used to
 * determine if a context value is required for the XOF algorithm.
 * 
 * If this function returns an error status, the operation enters an error
 * state and must be aborted by calling psa_xof_abort().
 *
 * \param[in,out] operation   Active XOF operation.
 * \param[in] context         Buffer containing the context value.
 * \param[in] context_length  Size of the \p context in bytes.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The context value is not valid for the XOF algorithm.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The context value is not supported by this implementation.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid: it must be active, and no call to
 *         psa_xof_set_context(), psa_xof_output(), or psa_xof_output() has
 *         been made, or the library requires initializing by a call to
 *         psa_crypto_init().
 */
psa_status_t psa_xof_set_context(psa_xof_operation_t *operation,
                                 const uint8_t *context,
                                 size_t context_length);

/** Add input to a multi-part XOF operation.
 * 
 * The application must call psa_xof_setup() before calling this function.
 * 
 * If this function returns an error status, the operation enters an error
 * state and must be aborted by calling psa_xof_abort().
 *
 * \param[in,out] operation Active XOF operation.
 * \param[in] input         Buffer containing the input fragment.
 * \param[in] input_length  Size of the \p input buffer in bytes.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The total input for the operation is too large for the XOF algorithm.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The total input for the operation is too large for the implementation.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid: it must be active, or
 *         the library requires initializing by a call to psa_crypto_init().
 */
psa_status_t psa_xof_update(psa_xof_operation_t *operation,
                           const uint8_t *input,
                           size_t input_length);

/** Extract data from an XOF operation.
 * 
 * The application must call psa_xof_setup() and supply all input data, using
 * calls to psa_xof_update(), before calling this function.
 * 
 * This function calculates output bytes from the XOF algorithm and returns
 * those bytes.
 * 
 * If this function returns an error status, the operation enters an error
 * state and must be aborted by calling psa_xof_abort().
 *
 * \param[in,out] operation  Active XOF operation.
 * \param[in] output         Buffer where the output will be written.
 * \param[in] output_length  Number of bytes to output.
 *
 * \retval #PSA_SUCCESS
 *         Success.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The operation state is not valid: it must be active, or
 *         the library requires initializing by a call to psa_crypto_init().
 */
psa_status_t psa_xof_output(psa_xof_operation_t *operation,
                            uint8_t *output,
                            size_t output_length);

/** Abort an XOF operation.
 * 
 * Aborting an operation frees all associated resources except for the
 * \p operation object itself. Once aborted, the operation object can be
 * reused for another operation by calling psa_xof_setup() again.
 * 
 * This function can be called any time after the operation object has been
 * initialized by one of the methods described in #psa_xof_operation_t.
 * In particular, calling psa_xof_abort() after the operation has been
 * terminated by a call to psa_xof_abort() is safe and has no effect.
 * 
 * \param[in,out] operation  Initialized XOF operation.
 *
 * \retval #PSA_SUCCESS
 *         Success. The operation object can now be discarded or reused.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY \emptydescription
 * \retval #PSA_ERROR_COMMUNICATION_FAILURE \emptydescription
 * \retval #PSA_ERROR_CORRUPTION_DETECTED \emptydescription
 * \retval #PSA_ERROR_BAD_STATE
 *         The library requires initializing by a call to psa_crypto_init().
 */
psa_status_t psa_xof_abort(psa_xof_operation_t *operation);

/** The SHAKE128 XOF algorithm.
 *
 * SHAKE128 is one of the KECCAK family of algorithms.
 * SHAKE128 is defined in [FIPS202].
 */
#define PSA_ALG_SHAKE128 ((psa_algorithm_t)0x0D000100)

/** The SHAKE256 XOF algorithm.
 *
 * SHAKE256 is one of the KECCAK family of algorithms.
 * SHAKE256 is defined in [FIPS202].
 */
#define PSA_ALG_SHAKE256 ((psa_algorithm_t)0x0D000200)

/** Whether the specified algorithm is an XOF algorithm.
 *
 * \param alg An algorithm identifier (value of type #psa_algorithm_t).
 *
 * \return 1 if \p alg is an XOF algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_XOF(alg) \
        (((alg) & 0x7f000000) == 0x0D000000)

#define PSA_ALG_XOF_HAS_CONTEXT(alg) \
        (((alg) & 0x00008000) != 0)


/** LMS signature algorithm
 *
 * This is the LMS stateful hash-based signature algorithm, defined by
 * Leighton-Micali Hash-Based Signatures [RFC8554]. LMS requires an
 * LMS key. The key and the signature must both encode the same LMS
 * parameter set, which is used for the verification procedure.
 * This message-signature algorithm can only be used with the
 * psa_verify_message() function.
 */
#define PSA_ALG_LMS ((psa_algorithm_t) 0x06004800)

/** HSS signature algorithm
 *
 * This is the HSS stateful hash-based signature algorithm, defined by
 * Leighton-Micali Hash-Based Signatures [RFC8554]. HSS requires an
 * HSS key. The key and the signature must both encode the same HSS
 * parameter set, which is used for the verification procedure.
 * This message-signature algorithm can only be used with the
 * psa_verify_message() function.
 */
#define PSA_ALG_HSS ((psa_algorithm_t) 0x06004900)

/** LMS public key.
 *
 * The parameterization of an LMS key is fully encoded in the key data.
 * The key attribute size of an LMS public key is output length, in bits,
 * of the hash function identified by the LMS parameter set.
 * To construct an LMS public key, it must be imported.
 * The data format for import or export of the public key is the encoded
 * lms_public_key structure, defined in [RFC8554] §3.
 */
#define PSA_KEY_TYPE_LMS_PUBLIC_KEY ((psa_key_type_t)0x4007)

/** HSS public key.
 *
 * The parameterization of an HSS key is fully encoded in the key data.
 * The key attribute size of an HSS public key is output length, in bits,
 * of the hash function identified by the HSS parameter set.
 * To construct an HSS public key, it must be imported.
 * The data format for import or export of the public key is the encoded
 * hss_public_key structure, defined in [RFC8554] §3.
 */
#define PSA_KEY_TYPE_HSS_PUBLIC_KEY ((psa_key_type_t)0x4008)


/** XMSS signature algorithm
 *
 * This is the XMSS stateful hash-based signature algorithm, defined by
 * XMSS: eXtended Merkle Signature Scheme [RFC8391]. XMSS requires an
 * XMSS key. The key and the signature must both encode the same XMSS
 * parameter set, which is used for the verification procedure.
 * This message-signature algorithm can only be used with the
 * psa_verify_message() function.
 */
#define PSA_ALG_XMSS ((psa_algorithm_t) 0x06004A00)

/** XMSS^MT signature algorithm
 *
 * This is the XMSS^MT stateful hash-based signature algorithm, defined by
 * XMSS: eXtended Merkle Signature Scheme [RFC8391]. XMSS^MT requires an
 * XMSS^MT key. The key and the signature must both encode the same XMSS^MT
 * parameter set, which is used for the verification procedure.
 * This message-signature algorithm can only be used with the
 * psa_verify_message() function.
 */
#define PSA_ALG_XMSS_MT ((psa_algorithm_t) 0x06004B00)

/** XMSS public key.
 *
 * The parameterization of an XMSS key is fully encoded in the key data.
 * The key attribute size of an XMSS public key is output length, in bits,
 * of the hash function identified by the XMSS parameter set.
 * To construct an XMSS public key, it must be imported.
 * The data format for import or export of the public key is the encoded
 * xmss_public_key structure, defined in [RFC8391] §3.
 */
#define PSA_KEY_TYPE_XMSS_PUBLIC_KEY ((psa_key_type_t)0x400B)

/** XMSS^MT public key.
 *
 * The parameterization of an XMSS^MT key is fully encoded in the key data.
 * The key attribute size of an XMSS^MT public key is output length, in bits,
 * of the hash function identified by the XMSS^MT parameter set.
 * To construct an XMSS^MT public key, it must be imported.
 * The data format for import or export of the public key is the encoded
 * xmssmt_public_key structure, defined in [RFC8391] Appendix C.3.
 */
#define PSA_KEY_TYPE_XMSS_MT_PUBLIC_KEY ((psa_key_type_t)0x400D)


/** Module lattice-based digital signature algorithm.
 *
 * This algorithm can only be used with the psa_sign_message() and
 * psa_verify_message() functions.
 * This is the pure ML-DSA digital signature algorithm, defined by FIPS
 * Publication 204: Module-Lattice-Based Digital Signature Standard [FIPS204],
 * using hedging. ML-DSA requires an ML-DSA key, which determines the ML-DSA
 * parameter set for the operation.
 * This algorithm is randomized: each invocation returns a different, equally
 * valid signature.
 */
#define PSA_ALG_ML_DSA                  ((psa_algorithm_t)0x06004400)

/** Deterministic module lattice-based digital signature algorithm.
 *
 * This algorithm can only be used with the psa_sign_message() and
 * psa_verify_message() functions.
 * This is the pure ML-DSA digital signature algorithm, defined by FIPS
 * Publication 204: Module-Lattice-Based Digital Signature Standard [FIPS204],
 * without hedging. ML-DSA requires an ML-DSA key, which determines the ML-DSA
 * parameter set for the operation.
 * This algorithm is deterministic: each invocation with the same inputs
 * returns an identical signature.
 */
#define PSA_ALG_DETERMINISTIC_ML_DSA    ((psa_algorithm_t)0x06004500)

#define PSA_ALG_HASH_ML_DSA_BASE        ((psa_algorithm_t)0x06004600)

/** Module lattice-based digital signature algorithm with pre-hashing.
 *
 * This is the pre-hashed ML-DSA digital signature algorithm, defined by FIPS
 * Publication 204: Module-Lattice-Based Digital Signature Standard [FIPS204],
 * using hedging. ML-DSA requires an ML-DSA key, which determines the ML-DSA
 * parameter set for the operation.
 * This algorithm is randomized: each invocation returns a different, equally
 * valid signature.
 * 
 * \param hash_alg A hash algorithm (\c PSA_ALG_XXX value such that
 *                 #PSA_ALG_IS_HASH(\p hash_alg) is true).
 *
 * \return         The corresponding Hash-ML-DSA signature algorithm.
 * \return         Unspecified if \p hash_alg is not a supported
 *                 hash algorithm.
 */
#define PSA_ALG_HASH_ML_DSA(hash_alg)   (PSA_ALG_HASH_ML_DSA_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

#define PSA_ALG_DETERMINISTIC_HASH_ML_DSA_BASE ((psa_algorithm_t)0x06004700)

/** Deterministic module lattice-based digital signature algorithm with pre-hashing.
 *
 * This is the pre-hashed ML-DSA digital signature algorithm, defined by FIPS
 * Publication 204: Module-Lattice-Based Digital Signature Standard [FIPS204],
 * without hedging. ML-DSA requires an ML-DSA key, which determines the ML-DSA
 * parameter set for the operation.
 * This algorithm is deterministic: each invocation with the same inputs
 * returns an identical signature.
 * 
 * \param hash_alg A hash algorithm (\c PSA_ALG_XXX value such that
 *                 #PSA_ALG_IS_HASH(\p hash_alg) is true).
 *
 * \return         The corresponding deterministic Hash-ML-DSA
 *                 signature algorithm.
 * \return         Unspecified if \p hash_alg is not a supported
 *                 hash algorithm.
 */
#define PSA_ALG_DETERMINISTIC_HASH_ML_DSA(hash_alg) \
    (PSA_ALG_DETERMINISTIC_HASH_ML_DSA_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))
#define PSA_ALG_ML_DSA_DETERMINISTIC_FLAG ((psa_algorithm_t) 0x00000100)

/** Whether the specified algorithm is a non-hash ML-DSA algorithm. */
#define PSA_ALG_IS_ML_DSA(alg) \
    (((alg) & ~PSA_ALG_ML_DSA_DETERMINISTIC_FLAG) == PSA_ALG_ML_DSA)

/** Whether the specified algorithm is a hash ML-DSA algorithm. */
#define PSA_ALG_IS_HASH_ML_DSA(alg) \
    (((alg) & ~0x000001ff) == PSA_ALG_HASH_ML_DSA_BASE)

/** Whether the specified algorithm is a hedged hash ML-DSA algorithm. */
#define PSA_ALG_IS_HEDGED_HASH_ML_DSA(alg) \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_HASH_ML_DSA_BASE)

/** Whether the specified algorithm is a deterministic hash ML-DSA algorithm. */
#define PSA_ALG_IS_DETERMINISTIC_HASH_ML_DSA(alg) \
    (((alg) & ~PSA_ALG_HASH_MASK) == PSA_ALG_DETERMINISTIC_HASH_ML_DSA_BASE)

/** ML-DSA key pair: both the private and public key.
 *
 * The key attribute size of an ML-DSA key is a measure of the security
 * strength of the ML-DSA parameter set in [FIPS204]:
 * ML-DSA-44: key_bits = 128
 * ML-DSA-65: key_bits = 192
 * ML-DSA-87: key_bits = 256
 * The data format for import and export of the key pair is the 32-byte seed.
 */
#define PSA_KEY_TYPE_ML_DSA_KEY_PAIR ((psa_key_type_t)0x7002)

/** ML-DSA public key.
 *
 * The key attribute size of an ML-DSA public key is the same as the
 * corresponding private key. See PSA_KEY_TYPE_ML_DSA_KEY_PAIR.
 * An ML-DSA public key is the pk output of ML-DSA.KeyGen(), defined
 * in [FIPS204] §5.1.
 */
#define PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY ((psa_key_type_t)0x4002)

/** Whether a key type is a ML_DSA key (pair or public-only). */
#define PSA_KEY_TYPE_IS_ML_DSA(type) \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY)


/** Module Lattice-based key-encapsulation mechanism.
 *
 * This is the ML-KEM key-encapsulation algorithm, defined by [FIPS203].
 * ML-KEM requires an ML-KEM key, which d etermines the ML-KEM parameter
 * set for the operation.
 */
#define PSA_ALG_ML_KEM ((psa_algorithm_t)0x0c000200)

/** ML-KEM key pair: both the decapsulation and encapsulation key.
 *
 * The Crypto API treats decapsulation keys as private keys and encapsulation
 * keys as public keys.
 * The key attribute size of an ML-KEM key is specified by the numeric part of
 * the parameter-set identifier defined in [FIPS203]. The parameter-set
 * identifier refers to the key strength, and not to the actual size of
 * the key. The following values for the key_bits key attribute are used
 * to select a specific ML-KEM parameter set:
 * ML-KEM-512 : key_bits = 512
 * ML-KEM-768 : key_bits = 768
 * ML-KEM-1024: key_bits = 1024
 * The data format for import and export of the key pair is the concatenation
 * of the two seed values: d || z.
 */
#define PSA_KEY_TYPE_ML_KEM_KEY_PAIR ((psa_key_type_t)0x7004)

/** ML-KEM public (encapsulation) key.
 *
 * The key attribute size of an ML-KEM public key is the same as the
 * corresponding private key. See PSA_KEY_TYPE_ML_KEM_KEY_PAIR.
 * An ML-KEM public key is the ek output of ML-KEM.KeyGen(), defined
 * in [FIPS203] §7.1.
 */
#define PSA_KEY_TYPE_ML_KEM_PUBLIC_KEY ((psa_key_type_t)0x4004)

/** Whether a key type is a ML_DSA key (pair or public-only). */
#define PSA_KEY_TYPE_IS_ML_KEM(type) \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(type) == PSA_KEY_TYPE_ML_KEM_PUBLIC_KEY)


/** Ascon (NIST SP 800-232) definitions.
 * @{
 */
        
/** Ascon-AEAD128 key type. */
#define PSA_KEY_TYPE_ASCON    ((psa_key_type_t)0x2008)

/** Ascon-AEAD128 AEAD algorithm. */
#define PSA_ALG_ASCON_AEAD128 ((psa_algorithm_t)0x05100700)

/** Ascon-Hash256 hash algorithm. */
#define PSA_ALG_ASCON_HASH256 ((psa_algorithm_t)0x02000020)

/** Ascon-XOF128 extended output function algorithm. */
#define PSA_ALG_ASCON_XOF128  ((psa_algorithm_t)0x0D000300)

/** Ascon-CXOF128 extended output function algorithm, with context. */
#define PSA_ALG_ASCON_CXOF128  ((psa_algorithm_t)0x0D008300)

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_EXTRA_H */
