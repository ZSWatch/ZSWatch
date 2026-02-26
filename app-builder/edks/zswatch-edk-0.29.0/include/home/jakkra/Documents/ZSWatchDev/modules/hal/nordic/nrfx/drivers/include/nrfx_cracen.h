/*
 * Copyright (c) 2025, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NRFX_CRACEN_H
#define NRFX_CRACEN_H

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_cracen CRACEN driver
 * @{
 * @ingroup nrf_cracen
 * @brief   Cryptographic accelerator engine (CRACEN) peripheral driver
 */

/**
 * @brief Function for initializing the CRACEN CTR_DRBG random generator.
 *
 * @note This initialization is relatively slow and power consuming.
 *
 * @note This function assumes exclusive access to the CRACEN TRNG and CryptoMaster, and may
 *       not be used while any other component is using those peripherals.
 *
 * @retval 0          Initialization was successful.
 * @retval -ECANCELED Unexpected error.
 * @retval -EALREADY  If it was was already initialized.
 */
int nrfx_cracen_ctr_drbg_init(void);

/** @brief Function for uninitializing the CRACEN CTR_DRBG random generator. */
void nrfx_cracen_ctr_drbg_uninit(void);

/**
 * @brief Function for filling the specified /p p_buf buffer with /p size bytes of random data.
 *
 * @note This function assumes exclusive access to the CRACEN TRNG and CryptoMaster, and may
 *       not be used while any other component is using those peripherals.
 *
 * @param[out] p_buf Buffer into which to copy \p size bytes of entropy.
 * @param[in]  size  Number of bytes to copy.
 *
 * @retval 0          Success.
 * @retval -EINVAL    Invalid inputs.
 * @retval -ECANCELED Unexpected error.
 */
int nrfx_cracen_ctr_drbg_random_get(uint8_t * p_buf, size_t size);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRFX_CRACEN_H */
