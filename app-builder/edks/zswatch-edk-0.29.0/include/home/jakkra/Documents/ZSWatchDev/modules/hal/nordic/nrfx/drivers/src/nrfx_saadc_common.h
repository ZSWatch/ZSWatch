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

#ifndef NRFX_SAADC_COMMON_H__
#define NRFX_SAADC_COMMON_H__

#include <helpers/nrfx_analog_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_saadc_common Generic SAADC input layer.
 * @{
 * @ingroup nrfx
 * @ingroup nrf_saadc
 *
 * @brief Helper layer that provides the common functionality of SAADC driver.
 */

/** @brief Symbol specifying not existing analog input. */
#define NRFX_SAADC_INPUT_NOT_PRESENT NRF_SAADC_INPUT_DISABLED

/** @brief SAADC external analog input pins mapping table. */
static const nrf_saadc_input_t nrfx_saadc_external_ain_psels[] =
{
    SAADC_EXTERNAL_AIN_PSELS
};

/** @brief SAADC internal inputs mapping table. */
static const nrf_saadc_input_t nrfx_saadc_internal_ain_psels[] =
{
    SAADC_INTERNAL_AIN_PSELS
};

/**
 * @brief Function for getting the SAADC input configuration for given generic analog input.
 *
 * @param[in] input The generic analog input to get the configuration for.
 *
 * @return The SAADC input configuration for the given analog input.
 *
 * @note The function takes into account the internal and external analog inputs.
 *       The function returns NRFX_SAADC_INPUT_NOT_PRESENT for analog inputs that are not
 *       supported by the given device.
 */
NRFX_STATIC_INLINE nrf_saadc_input_t nrfx_saadc_ain_get(nrfx_analog_input_t input)
{
    if ((input >= NRFX_ANALOG_AIN_INTERNAL_OFFSET) &&
        ((uint8_t)(input - NRFX_ANALOG_AIN_INTERNAL_OFFSET)
            < NRFX_ARRAY_SIZE(nrfx_saadc_internal_ain_psels)))
    {
        return nrfx_saadc_internal_ain_psels[(input - NRFX_ANALOG_AIN_INTERNAL_OFFSET)];
    }
    else if ((uint8_t)input < NRFX_ARRAY_SIZE(nrfx_saadc_external_ain_psels))
    {
        return nrfx_saadc_external_ain_psels[input];
    }

    return (nrf_saadc_input_t)NRFX_SAADC_INPUT_NOT_PRESENT;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRFX_SAADC_COMMON_H__ */
