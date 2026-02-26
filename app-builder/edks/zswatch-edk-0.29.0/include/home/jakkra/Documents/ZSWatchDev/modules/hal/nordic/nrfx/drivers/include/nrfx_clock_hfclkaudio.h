/*
 * Copyright (c) 2016 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_CLOCK_HFCLKAUDIO_H__
#define NRFX_CLOCK_HFCLKAUDIO_H__

#include <nrfx.h>
#include <hal/nrf_clock.h>

#if NRF_CLOCK_HAS_HFCLKAUDIO

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_clock_hfclkaudio driver
 * @{
 * @ingroup nrf_clock_hfclkaudio
 * @brief   HFCLKAUDIO peripheral driver.
 */

/** @brief HFCLKAUDIO events. */
#define NRFX_CLOCK_HFCLKAUDIO_EVT_HFCLKAUDIO_STARTED \
        NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_HFAUDIO_STARTED_MASK) ///< HFCLKAUDIO has been started.

/** @brief Clock event handler. */
typedef void (*nrfx_clock_hfclkaudio_event_handler_t)(void);

/**
 * @brief Function for initializing internal structures in the nrfx_clock module.
 *
 * After initialization, the module is in power off state (clocks are not started).
 *
 * @param[in] event_handler Event handler provided by the user.
 *                          If not provided, driver works in blocking mode.
 *
 * @retval 0         The procedure is successful.
 * @retval -EALREADY The driver is already initialized.
 */
int nrfx_clock_hfclkaudio_init(nrfx_clock_hfclkaudio_event_handler_t  event_handler);

/** @brief Function for uninitializing the hfclkaudio module. */
void nrfx_clock_hfclkaudio_uninit(void);

/**
 * @brief Function for checking if the hfclkaudio driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_clock_hfclkaudio_init_check(void);

/** @brief Function for starting the hfclkaudio clock. */
void nrfx_clock_hfclkaudio_start(void);

/** @brief Function for stopping the hfclkaudio clock. */
void nrfx_clock_hfclkaudio_stop(void);

/**
 * @brief Function for checking the specified hfclkaudio clock.
 *
 * @retval true  The clock domain is running.
 * @retval false The clock domain is not running.
 */
NRFX_STATIC_INLINE bool nrfx_clock_hfclkaudio_running_check(void);

/**
 * @brief Function for setting the HFCLKAUDIO configuration.
 *
 * The frequency of HFCLKAUDIO ranges from 10.666 MHz to 13.333 MHz in 40.7 Hz steps.
 * To calculate @p freq_value corresponding to the chosen frequency, use the following equation:
 * FREQ_VALUE = 2^16 * ((12 * f_out / 32M) - 4)
 *
 * @warning Chosen frequency must fit in 11.176 MHz - 11.402 MHz or 12.165 MHz - 12.411 MHz
 *          frequency bands.
 *
 * @param[in] freq_value New FREQ_VALUE for HFCLKAUDIO.
 */
NRFX_STATIC_INLINE void nrfx_clock_hfclkaudio_config_set(uint16_t freq_value);

/**
 * @brief Function for getting the HFCLKAUDIO configuration.
 *
 * The frequency of HFCLKAUDIO ranges from 10.666 MHz to 13.333 MHz in 40.7 Hz steps.
 * To calculate frequency corresponding to the returned FREQ_VALUE, use the following equation:
 * f_out = 32M * (4 + FREQ_VALUE * 2^(-16))/12
 *
 * @return Current value of FREQ_VALUE for HFCLKAUDIO.
 */
NRFX_STATIC_INLINE uint16_t nrfx_clock_hfclkaudio_config_get(void);

/** @brief HFCLKAUDIO interrupt handler. */
void nrfx_clock_hfclkaudio_irq_handler(void);

#ifndef NRFX_DECLARE_ONLY

NRFX_STATIC_INLINE bool nrfx_clock_hfclkaudio_running_check(void)
{
    return nrf_clock_is_running(NRF_CLOCK, NRF_CLOCK_DOMAIN_HFCLKAUDIO, NULL);
}

NRFX_STATIC_INLINE void nrfx_clock_hfclkaudio_config_set(uint16_t freq_value)
{
    nrf_clock_hfclkaudio_config_set(NRF_CLOCK, freq_value);
}

NRFX_STATIC_INLINE uint16_t nrfx_clock_hfclkaudio_config_get(void)
{
    return nrf_clock_hfclkaudio_config_get(NRF_CLOCK);
}

#endif // NRFX_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CLOCK_HAS_HFCLKAUDIO

#endif // NRFX_CLOCK_HFCLKAUDIO_H__
