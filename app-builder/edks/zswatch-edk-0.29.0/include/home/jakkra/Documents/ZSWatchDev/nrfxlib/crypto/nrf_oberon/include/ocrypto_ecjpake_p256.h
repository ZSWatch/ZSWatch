/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_ecjpake EC-JPAKE
 * @ingroup ocrypto_pake
 * @{
 * @brief EC-JPAKE algorithm based on P-256.
 *
 * @see [RFC - J-PAKE: Password-Authenticated Key Exchange by Juggling](https://tools.ietf.org/html/rfc8236)
 */

/**
 * @file
 * @brief EC-JPAKE algorithm based on P-256.
 */

#ifndef OCRYPTO_ECJPAKE_P256_H
#define OCRYPTO_ECJPAKE_P256_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * EC-JPAKE-P256 public key.
 *
 * @param[out] X       Public key.
 * @param      G       Generator. May be NULL to use the default generator.
 * @param      x       Secret key. 0 < x < group order
 *
 * @retval 0  If inputs are valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_get_public_key(
    uint8_t X[64],
    const uint8_t G[64],
    const uint8_t x[32]);

/**
 * EC-JPAKE-P256 zero knowledge proof hash.
 *
 * @param[out] hash    Generated hash.
 * @param      X       Public key.
 * @param      V       ZKP ephemeral public key.
 * @param      G       Generator. May be NULL to use the default generator.
 * @param      id      Identity of originator.
 * @param      id_len  Identity length.
 */
void ocrypto_ecjpake_get_zkp_hash(
    uint8_t hash[32],
    const uint8_t X[64],
    const uint8_t V[64],
    const uint8_t G[64],
    const char *id, size_t id_len);

/**
 * EC-JPAKE-P256 zero knowledge proof generation.
 *
 * @param[out] r        ZKP signature.
 * @param      x       Secret key. 0 < x < group order
 * @param      v       ZKP ephemeral secret key. 0 < v < group order
 * @param      hash     Identity of originator.
 * @param      hash_len Identity length.
 *
 * @retval 0  If inputs are valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_zkp_sign(
    uint8_t r[32],
    const uint8_t x[32],
    const uint8_t v[32],
    const uint8_t *hash,
    size_t hash_len);
    
/**
 * EC-JPAKE-P256 zero knowledge proof verification.
 *
 * @param      G        Generator. May be NULL to use the default generator.
 * @param      X        Public key.
 * @param      V        ZKP ephemeral public key.
 * @param      r        ZKP signature.
 * @param      hash     Identity of originator.
 * @param      hash_len Identity length.
 *
 * @retval 0  If the proof is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_zkp_verify(
    const uint8_t G[64],
    const uint8_t X[64],
    const uint8_t V[64],
    const uint8_t r[32],
    const uint8_t *hash,
    size_t hash_len);

/**
 * EC-JPAKE-P256 public key and zero knowledge proof generation.
 *
 * @param[out] X       Public key.
 * @param[out] V       ZKP ephemeral public key.
 * @param[out] r       ZKP signature.
 * @param      G       Generator. May be NULL to use the default generator.
 * @param      x       Secret key. 0 < x < group order
 * @param      v       ZKP ephemeral secret key. 0 < v < group order
 * @param      id      Identity of originator.
 * @param      id_len  Identity length.
 *
 * @retval 0  If inputs are valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_get_key(
    uint8_t X[64],
    uint8_t V[64],
    uint8_t r[32],
    const uint8_t G[64],
    const uint8_t x[32],
    const uint8_t v[32],
    const char *id, size_t id_len);

/**
 * EC-JPAKE-P256 zero knowledge proof verification.
 *
 * @param      G       Generator. May be NULL to use the default generator.
 * @param      X       Public key.
 * @param      V       ZKP ephemeral public key.
 * @param      r       ZKP signature.
 * @param      id      Identity of originator.
 * @param      id_len  Identity length.
 *
 * @retval 0  If proof is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_verify_key(
    const uint8_t G[64],
    const uint8_t X[64],
    const uint8_t V[64],
    const uint8_t r[32],
    const char *id, size_t id_len);


/**
 * EC-JPAKE-P256 generator derivation.
 *
 * @param[out] G       Generator.
 * @param      X1      Public key 1.
 * @param      X2      Public key 2.
 * @param      X3      Public key 3.
 *
 * @retval 0  If the generator is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_get_generator(
    uint8_t G[64],
    const uint8_t X1[64],
    const uint8_t X2[64],
    const uint8_t X3[64]);

/**
 * EC-JPAKE-P256 read shared secret.
 *
 * @param[out] rs          Reduced shared secret.
 * @param      secret      Shared secret.
 * @param      secret_len  Secret length.
 */
void ocrypto_ecjpake_read_shared_secret(
    uint8_t rs[32],
    const uint8_t *secret, size_t secret_len);

/**
 * EC-JPAKE-P256 shared secret handling.
 *
 * @param[out] xs          Client/server secret key.
 * @param      x2          Secret key 2.
 * @param      rs          Reduced shared secret.
 *
 * @retval 0  If the derived secret key is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_process_shared_secret(
    uint8_t xs[32],
    const uint8_t x2[32],
    const uint8_t rs[32]);

/**
 * EC-JPAKE-P256 premaster secret key generation.
 *
 * @param[out] secret  Resulting premaster secret key (curve point).
 * @param      Xr      Remote client/server public key.
 * @param      X2      Remote public key 2.
 * @param      xs      Client/server secret key.
 * @param      x2      Secret key 2.
 *
 * @retval 0  If the premaster secret key is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_get_premaster_secret_key(
    uint8_t secret[64],
    const uint8_t Xr[64],
    const uint8_t X2[64],
    const uint8_t xs[32],
    const uint8_t x2[32]);

/**
 * EC-JPAKE-P256 premaster secret generation.
 *
 * @param[out] secret  Resulting premaster secret.
 * @param      Xr      Remote client/server public key.
 * @param      X2      Remote public key 2.
 * @param      xs      Client/server secret key.
 * @param      x2      Secret key 2.
 *
 * @retval 0  If the secret is valid.
 * @retval -1 Otherwise.
 */
int ocrypto_ecjpake_get_premaster_secret(
    uint8_t secret[32],
    const uint8_t Xr[64],
    const uint8_t X2[64],
    const uint8_t xs[32],
    const uint8_t x2[32]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
