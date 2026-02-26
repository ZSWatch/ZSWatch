/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**@file
 * @defgroup nrf_cc3xx_platform_sha256 nrf_cc3xx_platform SHA256 APIs
 * @ingroup nrf_cc3xx_platform
 * @{
 * @brief The nrf_cc3xx_platform_sha256 APIs provide functions for
 *        hashing data using SHA256 algorithm.
 */

#ifndef NRF_CC3XX_PLATFORM_SHA256_H__
#define NRF_CC3XX_PLATFORM_SHA256_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Calculates SHA-256.
*
* @details The function will utilize the CryptoCell hardware to hash data from
*          RAM or flash. On CC310 a RAM buffer must be provided to support hashing
*          of data that resides in flash. If it is guaranteed
*          that the hashing input resides in RAM or the implementation supports
*          DMA from Flash, NULL and a size of 0 can be provided.
*
* @note    The address provided for @ref ram_buffer must be word-aligned.
*          The @ref ram_buffer_size should be a multiple of the word size
*          between 16 and 35520 bytes.
*
* @param[in,out]  ram_buffer        Pointer to a RAM buffer used to hash from flash.
* @param[in]      ram_buffer_size   The RAM buffer size in bytes.
* @param[in]      data              Input data to be hashed.
* @param[in]      len               Length of input data in bytes.
* @param[out]     out               Pointer to 32-byte buffer where hash digest will be written.
*
* @return Zero on success, otherwise a non-zero error code.
*/
int nrf_cc3xx_platform_sha256_hash(void *ram_buffer, size_t ram_buffer_size,
                                   void *data, size_t len, void *out);
#ifdef __cplusplus
}
#endif

#endif /* NRF_CC3XX_PLATFORM_SHA256_H__ */

/** @} */
