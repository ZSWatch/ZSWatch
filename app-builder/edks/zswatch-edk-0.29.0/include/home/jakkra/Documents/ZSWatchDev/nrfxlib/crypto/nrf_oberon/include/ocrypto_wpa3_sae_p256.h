/*
 * Copyright (c) 2016 - 2025 Nordic Semiconductor ASA
 * Copyright (c) since 2013 Oberon microsystems AG
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */



#ifndef OCRYPTO_WPA3_SAE_P256_H
#define OCRYPTO_WPA3_SAE_P256_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup ocrypto_wpa3_sae_p256 WPA3 SAE P-256
 * @ingroup ocrypto_pake
 * @{
 * @brief SAE/SAE-H2E password based WPA3 authentication algorithm based on the
 *        NIST P-256 curve (aka secp256r1).
 *
 * @see [IEEE - Std 802.11 2024](https://ieeexplore.ieee.org/document/10979691)
 */

/**
 * Get ECC point using SSWU method (Simplified Shallue-Woestijne-Ulas).
 *
 * @param[out] pt      ECC point.
 * @param      u1      pwd-value 1.
 * @param      u2      pwd-value 2.
 *
 * @retval 0   If @p pt is a valid curve point.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_sswu_pt(
    uint8_t pt[64],
    const uint8_t u1[48],
    const uint8_t u2[48]);

/**
 * Get secret element PT using the "hash to curve" method (SAE-H2E).
 *
 * @param[out] pt        ECC point.
 * @param      pw        Password.
 * @param      pw_len    Password length.
 * @param      pwid      Password id. Optional, may be NULL.
 * @param      pwid_len  Password id length.
 * @param      ssid      SSID.
 * @param      ssid_len  SSID length.
 *
 * @retval 0   If @p pt is a valid curve point.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_h2e_pt(
    uint8_t pt[64],
    const uint8_t *pw, size_t pw_len,
    const uint8_t *pwid, size_t pwid_len,
    const uint8_t *ssid, size_t ssid_len);

/**
 * Check the consistency of a secret element PT.
 *
 * @param      pt     ECC point.
 *
 * @retval 0   If @p pt is a valid curve point.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_check_pt(
    const uint8_t pt[64]);

/**
 * Get password element from precomputed PT and scalar value.
 *
 * @param[out] pwe    Password element.
 * @param      pt     Precomputed ECC point.
 * @param      val    Scalar Value.
 */
void ocrypto_wpa3_sae_p256_h2e_pwe_from_scalar(
    uint8_t pwe[64],
    const uint8_t pt[64],
    const uint8_t val[32]);

/**
 * Get password element from precomputed PT and MAC addresses.
 *
 * @param[out] pwe     Password element.
 * @param      pt      Precomputed ECC point.
 * @param      mac_a   MAC address 1.
 * @param      mac_b   MAC address 2.
 */
void ocrypto_wpa3_sae_p256_h2e_pwe(
    uint8_t pwe[64],
    const uint8_t pt[64],
    const uint8_t mac_a[6],
    const uint8_t mac_b[6]);

/**
 * Get password element candidate from x value.
 *
 * @param[out] y      Y coordinate of ECC point.
 * @param      x      X coordinate of ECC point.
 * @param      sign   Proposed sign bit (lsb) of y. 0 or 1.
 *
 * @retval 0   If @p x is a valid x coordinate of a curve point.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_get_pwe_from_x(
    uint8_t y[32],
    const uint8_t x[32],
    int sign);

/**
 * Get password element by "SAE hunting and pecking".
 *
 * @param[out] pwe     Password element.
 * @param      pw      Password.
 * @param      pw_len  Password length.
 * @param      mac_a   MAC address 1.
 * @param      mac_b   MAC address 2.
 *
 * @retval 0   If a valid password element was found.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_pwe(
    uint8_t pwe[64],
    const uint8_t *pw, size_t pw_len,
    const uint8_t mac_a[6],
    const uint8_t mac_b[6]);

/**
 * Get SAE commit message.
 *
 * @param[out] commit  Commit message.
 * @param      rand    Secret random value rand.
 * @param      mask    Secret random value mask.
 * @param      pwe     Password element.
 *
 * @retval 0   If a valid commit message was assembled.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_get_commit(
    uint8_t commit[96],
    const uint8_t rand[32],
    const uint8_t mask[32],
    const uint8_t pwe[64]);

/**
 * Check SAE commit message for consistency.
 *
 * @param      commit  Commit message.
 *
 * @retval 0   If &p commit is a valid commit message.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_check_commit(
    const uint8_t commit[96]);

/**
 * Get SAE shared secret value and context.
 *
 * @param[out] k            Shared secret value.
 * @param[out] ctx          Context.
 * @param      pwe          Password element.
 * @param      scalar       Local scalar.
 * @param      peer_scalar  Peer scalar.
 * @param      peer_element Peer element.
 * @param      rand         Secret random value rand.
 *
 * @retval 0   If the arguments are valid.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_secret_value(
    uint8_t k[32],
    uint8_t ctx[32],
    const uint8_t pwe[64],
    const uint8_t scalar[32],
    const uint8_t peer_scalar[32],
    const uint8_t peer_element[64],
    const uint8_t rand[32]);

/**
 * Get confirmation key (KCK) and pairwise master key (PMK).
 *
 * @param[out] kck          Confirmation key.
 * @param[out] pmk          Pairwise master key.
 * @param[out] pmkid        Key id.
 * @param      pwe          Password element.
 * @param      rand         Secret random value rand.
 * @param      commit       Local commit message.
 * @param      peer_commit  Peer commit message.
 * @param      rej_list     Rejected group list. Optional, may be NULL.
 * @param      rej_len      Rejected group list length.
 *
 * @retval 0   If the arguments are valid.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_keys(
    uint8_t kck[32],
    uint8_t pmk[32],
    uint8_t pmkid[16],
    const uint8_t pwe[64],
    const uint8_t rand[32],
    const uint8_t commit[96],
    const uint8_t peer_commit[96],
    const uint8_t *rej_list, size_t rej_len);

/**
 * Get SAE confirm message.
 *
 * @param[out] confirm       Confirm message.
 * @param      kck           Key confirmation key.
 * @param      commit        Local commit message.
 * @param      peer_commit   Peer commit message.
 * @param      send_confirm  Send confirm counter.
 */
void ocrypto_wpa3_sae_p256_get_confirm(
    uint8_t confirm[34],
    const uint8_t kck[32],
    const uint8_t commit[96],
    const uint8_t peer_commit[96],
    uint16_t send_confirm);

/**
 * Check SAE confirm message for consistency.
 *
 * @param      confirm       Confirm message.
 * @param      kck           Confirmation key.
 * @param      commit        Local commit message.
 * @param      peer_commit   Peer commit message.
 *
 * @retval 0   If &p confirm is a valid confirm message.
 * @retval -1  Otherwise.
 */
int ocrypto_wpa3_sae_p256_check_confirm(
    const uint8_t confirm[34],
    const uint8_t kck[32],
    const uint8_t commit[96],
    const uint8_t peer_commit[96]);


#ifdef __cplusplus
}
#endif

#endif

/** @} */
