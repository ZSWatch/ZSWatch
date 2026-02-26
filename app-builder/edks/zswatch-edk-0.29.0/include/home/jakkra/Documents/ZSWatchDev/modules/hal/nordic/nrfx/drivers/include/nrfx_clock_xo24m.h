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

#ifndef NRFX_CLOCK_XO24M_H__
#define NRFX_CLOCK_XO24M_H__

#include <nrfx.h>
#include <hal/nrf_clock.h>

#if NRF_CLOCK_HAS_HFCLK24M

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_clock_xo24m driver
 * @{
 * @ingroup nrf_clock_xo24m
 * @brief   XO24M peripheral driver.
 */

/** @brief XO24M event. */
#define NRFX_CLOCK_XO24M_EVT_HFCLK24M_STARTED NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_HFCLK24M_STARTED_MASK) ///< HFCLK24M has been started.

/** @brief XO24M event handler. */
typedef void (*nrfx_clock_xo24m_event_handler_t)(void);

/**
 * @brief Function for initializing internal structures in the nrfx_clock_xo24m module.
 *
 * After initialization, the module is in power off state (clocks are not started).
 *
 * @param[in] event_handler Event handler provided by the user.
 *                          If not provided, driver works in blocking mode.
 *
 * @retval 0         The procedure is successful.
 * @retval -EALREADY The driver is already initialized.
 */
int nrfx_clock_xo24m_init(nrfx_clock_xo24m_event_handler_t  event_handler);

/** @brief Function for uninitializing the xo24m module. */
void nrfx_clock_xo24m_uninit(void);

/**
 * @brief Function for checking if the xo24m driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_clock_xo24m_init_check(void);

/** @brief Function for starting the xo24m clock. */
void nrfx_clock_xo24m_start(void);

/** @brief Function for stopping the xo24m clock. */
void nrfx_clock_xo24m_stop(void);

/**
 * @brief Function for checking the specified clock domain state.
 *
 * XTAL source is assumed for domains with multiple sources.
 *
 * @retval true  The xo24m is running.
 * @retval false The xo24m is not running.
 */
NRFX_STATIC_INLINE bool nrfx_clock_xo24m_running_check(void);

/** @brief Function for handling xo24m interrupts. */
void nrfx_clock_xo24m_irq_handler(void);

#ifndef NRFX_DECLARE_ONLY

NRFX_STATIC_INLINE bool nrfx_clock_xo24m_running_check(void)
{
    return nrf_clock_is_running(NRF_CLOCK, NRF_CLOCK_DOMAIN_HFCLK24M, NULL);
}

#endif // NRFX_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CLOCK_HAS_HFCLK24M

#endif // NRFX_CLOCK_XO24M_H__
