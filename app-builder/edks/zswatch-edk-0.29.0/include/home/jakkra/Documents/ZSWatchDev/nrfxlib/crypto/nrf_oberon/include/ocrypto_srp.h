/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @defgroup ocrypto_srp SRP
 * @ingroup ocrypto_pake
 * @{
 * @brief SRP (Secure Remote Password) algorithm based on SRP-6 3072 bit SHA512.
 *
 * SRP is an augmented password-authenticated key exchange protocol,
 * specifically designed to work around existing patents. SRP allows the use of
 * user names and passwords over unencrypted channels and supplies a shared
 * secret at the end of the authentication sequence that can be used to generate
 * encryption keys.
 *
 * An eavesdropper or man in the middle cannot obtain enough information to be
 * able to brute force guess a password without further interactions with the
 * parties for each guess.
 *
 * The server does not store password-equivalent data. This means that an
 * attacker who steals the server data cannot masquerade as the client unless
 * they first perform a brute force search for the password.
 *
 * The specific variant implemented here is SRP-6 3072 bit SHA512.
 *
 * @see [RFC - The SRP Authentication and Key Exchange System](https://tools.ietf.org/html/rfc2945)
 * @see [RFC - Using the Secure Remote Password (SRP) Protocol for TLS Authentication](https://tools.ietf.org/html/rfc5054)
 * @see [The Stanford SRP Homepage](http://srp.stanford.edu)
 *
 * **Basic protocol overview**
 *
 * *Setup*
 * 1. Server generates a username / password combination together with a salt.
 * 2. Server derives a password verifier (see #ocrypto_srp_verifier).
 * 3. The username, salt and verifier are stored and required to open sessions.
 *    The original password is no longer needed.
 *
 * *Session opening*
 * 1. Client sends a username and the public key of an ephemeral key pair to the
 *    server.
 * 2. Server sends the salt and the public key of another ephemeral key pair to
 *    the client (see #ocrypto_srp_public_key).
 * 3. Client and Server both compute the session key from this information (see
 *    #ocrypto_srp_scrambling_parameter, #ocrypto_srp_premaster_secret,
 *    #ocrypto_srp_session_key).
 * 4. Client sends proof of the session key to the server.
 * 5. Server validates proof (see #ocrypto_srp_proof_m1), then sends proof of the
 *    session key to the client (see #ocrypto_srp_proof_m2).
 * 6. Client validates proof. Both parties know that they share the same private
 *    session key.
 */

/**
 * @file
 * @brief SRP (Secure Remote Password) algorithm based on SRP-6 3072 bit SHA512.
 */

#ifndef OCRYPTO_SRP_H
#define OCRYPTO_SRP_H

#include "ocrypto_types_srp.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Salt length.
 */
#define ocrypto_srp_SALT_BYTES (16)

/**
 * Password verifier length.
 */
#define ocrypto_srp_VERIFIER_BYTES (384)

/**
 * Secret key length.
 */
#define ocrypto_srp_SECRET_KEY_BYTES (32)

/**
 * Public key length.
 */
#define ocrypto_srp_PUBLIC_KEY_BYTES (384)

/**
 * Scrambling parameter length.
 */
#define ocrypto_srp_SCRAMBLING_PARAMETER_BYTES (64)

/**
 * Premaster secret length.
 */
#define ocrypto_srp_PREMASTER_SECRET_BYTES (384)

/**
 * Session key length.
 */
#define ocrypto_srp_SESSION_KEY_BYTES (64)

/**
 * Proof length.
 */
#define ocrypto_srp_PROOF_BYTES (64)


/**
 * @name SRP-6 password verifier generation
 *
 * A password verifier is generated from a user name and a password. The
 * password @p pass may be discarded, as only the verifier is used during later
 * computations.
 */
/**@{*/
/**
 * SRP-6 password verifier.
 *
 * The verifier is generated for a given user name @p user, a password @p pass
 * and salt @p salt.
 *
 * @param[out] v        Generated password verifier.
 * @param      salt     Salt.
 * @param      user     User name.
 * @param      user_len Length of @p user.
 * @param      pass     Password.
 * @param      pass_len Length of @p pass.
 */
void ocrypto_srp_verifier(
    uint8_t v[ocrypto_srp_VERIFIER_BYTES],
    const uint8_t salt[ocrypto_srp_SALT_BYTES],
    const uint8_t *user, size_t user_len,
    const uint8_t *pass, size_t pass_len);
/**@}*/

/**
 * @name SRP-6 public key generation
 *
 * An ephemeral keypair can be generated based on the password verifier to be
 * used when opening a new session.
 */
/**@{*/
/**
 * SRP-6 public Key.
 *
 * The public key for a given private key @p priv_b is generated using the
 * password verifier @p v and put into @p pub_b.
 *
 * @param[out] pub_b  Generated public key.
 * @param      priv_b Private key.
 * @param      v      Password verifier.
 */
void ocrypto_srp_public_key(
    uint8_t pub_b[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t priv_b[ocrypto_srp_SECRET_KEY_BYTES],
    const uint8_t v[ocrypto_srp_VERIFIER_BYTES]);
/**@}*/

/**
 * SRP-6 Server Public Key.
 *
 * @param[out] pub_b  Generated public key.
 * @param      priv_b Private key.
 * @param      k      Multiplier.
 * @param      v      Password verifier.
 */
void ocrypto_srp_server_public_key(
    uint8_t pub_b[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t priv_b[ocrypto_srp_SECRET_KEY_BYTES],
    const uint8_t k[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t v[ocrypto_srp_VERIFIER_BYTES]);
    
/**
 * SRP-6 Client Public Key.
 *
 * @param[out] pub_a  Generated public key.
 * @param      priv_a Private key.
 * @param      a_len  Length of @p priv_a.
 */
void ocrypto_srp_client_public_key(
    unsigned char pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const unsigned char *priv_a, size_t a_len);

/**
 * @name SRP-6 session key generation
 *
 * A premaster secret can be derived from both the client's and server's public
 * keys, the server's private key and the password verifier. A shared session
 * key can be generated from this premaster secret.
 */
/**@{*/
/**
 * SRP-6 scrambling parameter.
 *
 * The scrambling parameter is computed from both the client's public key
 * @p pub_a and the server's public key @p pub_b. The scrambling parameter
 * is required to compute the premaster secret.
 *
 * @param[out] u     Generated scrambling parameter.
 * @param      pub_a Client public key.
 * @param      pub_b Server public key.
 */
void ocrypto_srp_scrambling_parameter(
    uint8_t u[ocrypto_srp_SCRAMBLING_PARAMETER_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t pub_b[ocrypto_srp_PUBLIC_KEY_BYTES]);

/**
 * SRP-6 premaster secret.
 *
 * The premaster secret between the client and the server is computed using the
 * client public key @p pub_a, the server private key @p priv_b, the scrambling
 * parameter @p u and the password verifier @p v. If the client public key
 * @p pub_a is valid, the premaster secret is then put into @p s. The premaster
 * secret can be used to generate encryption keys.
 *
 * @param[out] s      Generated premaster secret.
 * @param      pub_a  Client public key.
 * @param      priv_b Server private key.
 * @param      u      Scrambling parameter; generated with @c ocrypto_srp_scrambling_parameter.
 * @param      v      Password verifier.
 *
 * @retval 0 If @p pub_a is a valid public key.
 * @retval 1 Otherwise.
 */
int ocrypto_srp_premaster_secret(
    uint8_t s[ocrypto_srp_PREMASTER_SECRET_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t priv_b[ocrypto_srp_SECRET_KEY_BYTES],
    const uint8_t u[ocrypto_srp_SCRAMBLING_PARAMETER_BYTES],
    const uint8_t v[ocrypto_srp_VERIFIER_BYTES]);

/**
 * SRP-6 server premaster secret.
 *
 * The premaster secret between the client and the server is computed using the
 * client public key @p pub_a, the server private key @p priv_b, the scrambling
 * parameter @p u and the password verifier @p v. If the client public key
 * @p pub_a is valid, the premaster secret is then put into @p s. The premaster
 * secret can be used to generate encryption keys.
 *
 * @param[out] s      Generated premaster secret.
 * @param      pub_a  Client public key.
 * @param      priv_b Server private key.
 * @param      u      Scrambling parameter; generated with @c ocrypto_srp_scrambling_parameter.
 * @param      u_len  Length of @p u.
 * @param      v      Password verifier.
 *
 * @retval 0 If @p pub_a is a valid public key.
 * @retval 1 Otherwise.
 */
int ocrypto_srp_server_premaster_secret(
    uint8_t s[ocrypto_srp_PREMASTER_SECRET_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t priv_b[ocrypto_srp_SECRET_KEY_BYTES],
    const uint8_t *u, size_t u_len,
    const uint8_t v[ocrypto_srp_VERIFIER_BYTES]);

/**
 * SRP-6 client premaster secret.
 *
 * @param[out] s      Generated premaster secret.
 * @param      priv_a Client private key.
 * @param      pub_b  Server public key.
 * @param      k      Multiplier.
 * @param      u      Scrambling parameter; generated with @c srp_scrambling_parameter.
 * @param      h      Password hash.
 * @param      h_len  Length of @p h and @p u.
 *
 * @retval 0 If @p pub_a is a valid public key.
 * @retval 1 Otherwise.
 */
int ocrypto_srp_client_premaster_secret(
    uint8_t s[ocrypto_srp_PREMASTER_SECRET_BYTES],
    const uint8_t priv_a[ocrypto_srp_SECRET_KEY_BYTES],
    const uint8_t pub_b[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t k[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t *u,
    const uint8_t *h, size_t h_len);

 /**
 * SRP-6 SRP session key.
 *
 * Generates the shared SRP session key from the premaster secret @p s and puts
 * it into @p k.
 *
 * @param[out] k Generated SRP session key.
 * @param      s Premaster secret.
 */
void ocrypto_srp_session_key(
    uint8_t k[ocrypto_srp_SESSION_KEY_BYTES],
    const uint8_t s[ocrypto_srp_PREMASTER_SECRET_BYTES]);
/**@}*/

/**
 * @name SRP-6 proof exchange
 *
 * Proofs are exchanged from client to server and vice versa to ensure that both
 * parties computed the same shared session key. The proofs only match if the
 * correct password is used by the client.
 */
/**@{*/
/**
 * SRP-6 proof M1 (client to server).
 *
 * A proof is generated by the client and sent to the server to assert that the
 * client is in possession of the shared session key @p k. The server also
 * generates the proof. Only if the proofs match, the process can continue.
 * The proof is based on the salt @p salt, the client public key @p pub_a,
 * the server public key @p pub_b and the shared session key @p k.
 *
 * @param[out] m1       Generated proof.
 * @param      user     User name.
 * @param      user_len Length of @p user.
 * @param      salt     Salt.
 * @param      pub_a    Client public key.
 * @param      pub_b    Server public key.
 * @param      k        Session key.
 */
void ocrypto_srp_proof_m1(
    uint8_t m1[ocrypto_srp_PROOF_BYTES],
    const uint8_t *user, size_t user_len,
    const uint8_t salt[ocrypto_srp_SALT_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t pub_b[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t k[ocrypto_srp_SESSION_KEY_BYTES]);

/**
 * SRP-6 proof M2 (server to client).
 *
 * A second proof is generated by the server and sent back to the client to
 * assert that the server is in possession of the shared session key @p k. The
 * client also generates the proof. If the proofs match, both parties can assume
 * that they share the same session key @p k. The second proof is based on the
 * client public key @p pub_a, the first proof @p m1 and the session key @p k.
 *
 * @param[out] m2    Generated proof.
 * @param      pub_a Client public key.
 * @param      m1    First proof; generated with @c ocrypto_srp_proof_m1.
 * @param      k     Session key.
 */
void ocrypto_srp_proof_m2(
    uint8_t m2[ocrypto_srp_PROOF_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t m1[ocrypto_srp_PROOF_BYTES],
    const uint8_t k[ocrypto_srp_SESSION_KEY_BYTES]);
/**@}*/


/**
 * @name SRP-6 password verifier generation with context
 *
 * A password verifier is generated from a user name and a password. The
 * password @p pass may be discarded, as only the verifier is used in subsequent
 * computations.
 */
/**@{*/
/**
 * SRP-6 password verifier.
 *
 * The verifier is generated for a given user name @p user, a password @p pass
 * and salt @p salt.
 *
 * @param      ctx      Context.
 * @param[out] v        Generated password verifier.
 * @param      salt     Salt.
 * @param      user     User name.
 * @param      user_len Length of @p user.
 * @param      pass     Password.
 * @param      pass_len Length of @p pass.
 */
void ocrypto_srp_verifier_ctx(
    ocrypto_srp_ctx *ctx,
    uint8_t v[ocrypto_srp_VERIFIER_BYTES],
    const uint8_t salt[ocrypto_srp_SALT_BYTES],
    const uint8_t *user, size_t user_len,
    const uint8_t *pass, size_t pass_len);
/**@}*/

/**
 * @name SRP-6 public key generation with context
 *
 * An ephemeral keypair can be generated based on the password verifier to be
 * used when opening a new session.
 */
/**@{*/
/**
 * SRP-6 public Key.
 *
 * The public key for a given private key @p priv_b is generated using the
 * password verifier @p v and put into @p pub_b.
 *
 * @param      ctx    Context.
 * @param[out] pub_b  Generated public key.
 * @param      priv_b Private key.
 * @param      v      Password verifier.
 */
void ocrypto_srp_public_key_ctx(
    ocrypto_srp_ctx *ctx,
    uint8_t pub_b[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t priv_b[ocrypto_srp_SECRET_KEY_BYTES],
    const uint8_t v[ocrypto_srp_VERIFIER_BYTES]);
/**@}*/

/**
 * @name SRP-6 session key generation with context
 *
 * A premaster secret can be derived from both the client's and server's public
 * keys, the server's private key and the password verifier. A shared session
 * key can be generated from this premaster secret.
 */
/**@{*/
/**
 * SRP-6 scrambling parameter.
 *
 * The scrambling parameter is computed from both the client's public key
 * @p pub_a and the server's public key @p pub_b. The scrambling parameter
 * is required to compute the premaster secret.
 *
 * @param      ctx   Context.
 * @param[out] u     Generated scrambling parameter.
 * @param      pub_a Client public key.
 * @param      pub_b Server public key.
 */
void ocrypto_srp_scrambling_parameter_ctx(
    ocrypto_srp_ctx *ctx,
    uint8_t u[ocrypto_srp_SCRAMBLING_PARAMETER_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t pub_b[ocrypto_srp_PUBLIC_KEY_BYTES]);

/**
 * SRP-6 premaster secret with context.
 *
 * The premaster secret between the client and the server is computed using the
 * client public key @p pub_a, the server private key @p priv_b, the scrambling
 * parameter @p u and the password verifier @p v. If the client public key
 * @p pub_a is valid, the premaster secret is then put into @p s. The premaster
 * secret can be used to generate encryption keys.
 *
 * @param      ctx    Context.
 * @param[out] s      Generated premaster secret.
 * @param      pub_a  Client public key.
 * @param      priv_b Server private key.
 * @param      u      Scrambling parameter; generated with @c ocrypto_srp_scrambling_parameter.
 * @param      v      Password verifier.
 *
 * @retval 0 If @p pub_a is a valid public key.
 * @retval 1 Otherwise.
 */
int ocrypto_srp_premaster_secret_ctx(
    ocrypto_srp_ctx *ctx,
    uint8_t s[ocrypto_srp_PREMASTER_SECRET_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t priv_b[ocrypto_srp_SECRET_KEY_BYTES],
    const uint8_t u[ocrypto_srp_SCRAMBLING_PARAMETER_BYTES],
    const uint8_t v[ocrypto_srp_VERIFIER_BYTES]);

/**
 * SRP-6 SRP session Key with context.
 *
 * Generates the shared SRP session key from the premaster secret @p s and puts
 * it into @p k.
 *
 * @param      ctx Context.
 * @param[out] k   Generated SRP session key.
 * @param      s   Premaster secret.
 */
void ocrypto_srp_session_key_ctx(
    ocrypto_srp_ctx *ctx,
    uint8_t k[ocrypto_srp_SESSION_KEY_BYTES],
    const uint8_t s[ocrypto_srp_PREMASTER_SECRET_BYTES]);
/**@}*/

/**
 * @name SRP-6 proof exchange with context
 *
 * Proofs are exchanged from client to server and vice versa to ensure that both
 * parties computed the same shared session key. The proofs only match if the
 * correct password is used by the client.
 */
/**@{*/
/**
 * SRP-6 proof M1 (client to server) with context.
 *
 * A proof is generated by the client and sent to the server to assert that the
 * client is in possession of the shared session key @p k. The server also
 * generates the proof. Only if the proofs match, the process can continue.
 * The proof is based on the salt @p salt, the client public key @p pub_a,
 * the server public key @p pub_b and the shared session key @p k.
 *
 * @param      ctx      Context.
 * @param[out] m1       Generated proof.
 * @param      user     User name.
 * @param      user_len Length of @p user.
 * @param      salt     Salt.
 * @param      pub_a    Client public key.
 * @param      pub_b    Server public key.
 * @param      k        Session key.
 */
void ocrypto_srp_proof_m1_ctx(
    ocrypto_srp_ctx *ctx,
    uint8_t m1[ocrypto_srp_PROOF_BYTES],
    const uint8_t *user, size_t user_len,
    const uint8_t salt[ocrypto_srp_SALT_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t pub_b[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t k[ocrypto_srp_SESSION_KEY_BYTES]);

/**
 * SRP-6 proof M2 (server to client) with context.
 *
 * A second proof is generated by the server and sent back to the client to
 * assert that the server is in possession of the shared session key @p k. The
 * client also generates the proof. If the proofs match, both parties can assume
 * that they share the same session key @p k. The second proof is based on the
 * client public key @p pub_a, the first proof @p m1 and the session key @p k.
 *
 * @param      ctx   Context.
 * @param[out] m2    Generated proof.
 * @param      pub_a Client public key.
 * @param      m1    First proof; generated with @c ocrypto_srp_proof_m1.
 * @param      k     Session key.
 */
void ocrypto_srp_proof_m2_ctx(
    ocrypto_srp_ctx *ctx,
    uint8_t m2[ocrypto_srp_PROOF_BYTES],
    const uint8_t pub_a[ocrypto_srp_PUBLIC_KEY_BYTES],
    const uint8_t m1[ocrypto_srp_PROOF_BYTES],
    const uint8_t k[ocrypto_srp_SESSION_KEY_BYTES]);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif

/** @} */
