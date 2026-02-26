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

#ifndef NRFX_CLOCK_HFCLK_H__
#define NRFX_CLOCK_HFCLK_H__

#include <hal/nrf_clock.h>

#if NRF_CLOCK_HAS_HFCLK

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_clock_hfclk HFCLK CLOCK driver
 * @{
 * @ingroup nrf_clock
 * @brief   HFCLK CLOCK peripheral driver.
 */

/** @brief HFCLK clock event. */
#define NRFX_CLOCK_HFCLK_EVT_HFCLK_STARTED NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_HF_STARTED_MASK)

/** @brief HFCLK event handler. */
typedef void (*nrfx_clock_hfclk_event_handler_t)(void);

/**
 * @brief Function for initializing internal structures in the HFCLK clock.
 *
 * After initialization, the HFCLK is in power off state (clock is not started).
 *
 * @param[in] event_handler Event handler provided by the user.
 *                          If not provided, driver works in blocking mode.
 *
 * @retval 0         The procedure is successful.
 * @retval -EALREADY The driver is already initialized.
 */
int nrfx_clock_hfclk_init(nrfx_clock_hfclk_event_handler_t event_handler);

/**
 * @brief Function for checking if the HFCLK driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_clock_hfclk_init_check(void);

/** @brief Function for uninitializing the HFCLK clock. */
void nrfx_clock_hfclk_uninit(void);

/** @brief Function for starting the HFCLK clock. */
void nrfx_clock_hfclk_start(void);

/** @brief Function for stopping the HFCLK clock. */
void nrfx_clock_hfclk_stop(void);

#if NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
/**
 * @brief Function for setting the HFCLK divider.
 *
 * @param[in] div New divider for the HFCLK.
 */
void nrfx_clock_hfclk_divider_set(nrf_clock_hfclk_div_t div);

/**
 * @brief Function for getting the HFCLK clock divider.
 *
 * @return Current divider for the HFCLK clock.
 */
NRFX_STATIC_INLINE nrf_clock_hfclk_div_t nrfx_clock_hfclk_divider_get(void);
#endif

/**
 * @brief Function for checking the HFCLK state.
 *
 * @param[out] p_clk_src Pointer to a clock source that is running.
 *
 * @retval true  The HFCLK is running.
 * @retval false The HFCLK is not running.
 */
NRFX_STATIC_INLINE bool nrfx_clock_hfclk_running_check(nrf_clock_hfclk_t * p_clk_src);

/** @brief HFCLK interrupt handler. */
void nrfx_clock_hfclk_irq_handler(void);

#ifndef NRFX_DECLARE_ONLY
NRFX_STATIC_INLINE bool nrfx_clock_hfclk_running_check(nrf_clock_hfclk_t * p_clk_src)
{
    bool ret = nrf_clock_is_running(NRF_CLOCK, NRF_CLOCK_DOMAIN_HFCLK, p_clk_src);
    return (ret && (*p_clk_src == NRF_CLOCK_HFCLK_HIGH_ACCURACY));
}

#if NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
NRFX_STATIC_INLINE nrf_clock_hfclk_div_t nrfx_clock_hfclk_divider_get(void)
{
    return nrf_clock_hfclk_div_get(NRF_CLOCK);
}
#endif // NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
#endif // NRFX_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CLOCK_HAS_HFCLK

#endif // NRFX_CLOCK_HFCLK_H__
