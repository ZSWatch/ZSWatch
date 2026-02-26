/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
/**@file
 * @defgroup nrf_cc3xx_platform nrf_cc3xx_platform APIs
 * @{
 * @brief nrf_cc3xx_platform library containing cc3xx
 * hardware initialization and entropy gathering APIs. The library also contains
 * APIs and companion source-files to setup RTOS dependent mutex and abort
 * functionality for the nrf_cc3xx_mbedcrypto library in Zephyr RTOS and FreeRTOS.
 * @}
 *
 * @defgroup nrf_cc3xx_platform_init nrf_cc3xx_platform initialization APIs
 * @ingroup nrf_cc3xx_platform
 * @{
 * @brief The nrf_cc3xx_platform APIs provides functions related to
 *        initialization of the Arm CryptoCell cc3xx hardware accelerator for
 *        usage in nrf_cc3xx_platform and dependent libraries.
 */
#ifndef NRF_CC3XX_PLATFORM_H__
#define NRF_CC3XX_PLATFORM_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "nrf_cc3xx_platform_defines.h"
#include "nrf_cc3xx_platform_abort.h"
#include "nrf_cc3xx_platform_mutex.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**@brief Function to initialize the nrf_cc3xx_platform with rng support.
 *        The function is using CTR_DRBG to generate a random seed.
 *
 * @return Zero on success, otherwise a non-zero error code.
 */
int nrf_cc3xx_platform_init(void);


/**@brief Function to initialize the nrf_cc3xx_platform with rng.
 *        The function is using HMAC_DRBG to generate a random seed.
 *
 * @note If this is called after nrf_cc3xx_platform_init it will
 *       create a new random seed using HMAC_DRBG.
 *
 * @return Zero on success, otherwise a non-zero error code.
 */
int nrf_cc3xx_platform_init_hmac_drbg(void);

/**@brief Function to initialize the nrf_cc3xx_platform without rng support
 *
 * @return Zero on success, otherwise a non-zero error code.
 */
int nrf_cc3xx_platform_init_no_rng(void);


/** @brief Function to deinitialize the nrf_cc3xx_platform
 *
 * @return Zero on success, otherwise a non-zero error code.
 */
int nrf_cc3xx_platform_deinit(void);


/** @brief Function to check if the nrf_cc3xx_platform is initialized
 *
 * @retval True if initialized, otherwise false.
 */
bool nrf_cc3xx_platform_is_initialized(void);


/** @brief Function to check if the nrf_cc3xx_platform is initialized
 *         with RNG support
 *
 * @retval True if RNG is initialized, otherwise false.
 */
bool nrf_cc3xx_platform_rng_is_initialized(void);


/** @brief ISR Function for processing of cc3xx Interrupts.
 *         This cc3xx interrupt service routine function should be called for
 *         interrupt processing.
 *         Either by placing this function directly in the vector table or by
 *         calling it from the ISR in the OS.
 */
void CRYPTOCELL_IRQHandler(void);


/** @brief Function to get the nonce seed used for encrypted ITS usage
 *
 * @param[in]	buffer 	Buffer to fill the nonce seed generated during boot.
 *
 * @return Zero on success, otherwise a non-zero error code.
 */
int nrf_cc3xx_platform_get_nonce_seed(uint8_t buffer[NRF_CC3XX_PLATFORM_EITS_NONCE_SIZE]);


/** @brief Function to get the boot seed used by TF-M attestation
 *
 * @param[in]	buffer 	Buffer to fill the boot seed generated during boot.
 *
 * @return Zero on success, otherwise a non-zero error code.
 */
int nrf_cc3xx_platform_get_boot_seed(uint8_t buffer[NRF_CC3XX_PLATFORM_TFM_BOOT_SEED_SIZE]);

#ifdef __cplusplus
}
#endif

#endif /* NRF_CC3XX_PLATFORM_H__ */
/** @} */
