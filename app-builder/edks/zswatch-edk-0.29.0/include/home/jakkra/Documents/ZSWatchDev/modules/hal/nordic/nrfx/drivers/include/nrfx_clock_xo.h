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

#ifndef NRFX_CLOCK_XO_H__
#define NRFX_CLOCK_XO_H__

#include <nrfx.h>
#include <hal/nrf_clock.h>

#if NRF_CLOCK_HAS_XO
#include <nrfx_power_clock.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_clock_xo XO driver
 * @{
 * @ingroup nrf_clock_xo
 * @brief XO clock driver.
 */

/** @brief XO clock events. */
typedef enum
{
    NRFX_CLOCK_XO_EVT_HFCLK_STARTED  = NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_HF_STARTED_MASK),   ///< XO has been started.
#if NRF_CLOCK_HAS_PLL
    NRFX_CLOCK_XO_EVT_PLL_STARTED    = NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_PLL_STARTED_MASK),  ///< PLL has been started.
#endif
#if NRF_CLOCK_HAS_XO_TUNE
    NRFX_CLOCK_XO_EVT_XO_TUNED       = NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_XOTUNED_MASK),      ///< XO tune has been done.
    NRFX_CLOCK_XO_EVT_XO_TUNE_ERROR  = NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_XOTUNEERROR_MASK),  ///< XO is not tuned.
    NRFX_CLOCK_XO_EVT_XO_TUNE_FAILED = NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_XOTUNEFAILED_MASK), ///< XO tune operation failed.
#endif
} nrfx_clock_xo_event_type_t;

/**
 * @brief XO clock event handler.
 *
 * @param[in] event Event.
 */
typedef void (*nrfx_clock_xo_event_handler_t)(nrfx_clock_xo_event_type_t event);

/**
 * @brief Function for initializing internal structures in the nrfx_clock_xo module.
 *
 * After initialization, the module is in power off state (clocks are not started).
 *
 * @param[in] event_handler Event handler provided by the user.
 *                          If not provided, driver works in blocking mode.
 *
 * @retval 0         The procedure is successful.
 * @retval -EALREADY The driver is already initialized.
 */
int nrfx_clock_xo_init(nrfx_clock_xo_event_handler_t  event_handler);

/** @brief Function for uninitializing the clock module. */
void nrfx_clock_xo_uninit(void);

/**
 * @brief Function for checking if the clock driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_clock_xo_init_check(void);

/** @brief Function for starting the XO clock. */
void nrfx_clock_xo_start(void);

/** @brief Function for stopping the XO clock. */
void nrfx_clock_xo_stop(void);

/**
 * @brief Function for checking the XO clock state.
 *
 * @param[out] p_clk_src Pointer to a clock source that is running.
 *
 * @retval true  The XO clock is running.
 * @retval false The XO clock is not running.
 */
NRFX_STATIC_INLINE bool nrfx_clock_xo_running_check(nrf_clock_hfclk_t * p_clk_src);


#if NRF_CLOCK_HAS_XO_TUNE

/**
 * @brief Function for starting tune of crystal XO.
 *
 * This function starts tuning process of the XO.
 *
 * @retval 0            The procedure is successful.
 * @retval -EINPROGRESS The high-frequency XO clock is off or operation is in progress.
 * @retval -ECANCELED   XO tune operation failed.
 */
int nrfx_clock_xo_tune_start(void);

/**
 * @brief Function for aborting tune of crystal XO.
 *
 * This function aborts tuning process.
 *
 * @retval 0      The procedure is successful.
 * @retval -EPERM The high-frequency XO clock is off or operation is not in progress.
 */
int nrfx_clock_xo_tune_abort(void);

/**
 * @brief Function for checking if XO tune error occurred.
 *
 * @note Must be used only if @p event_handler was not provided during driver initialization.
 *
 * @retval true  XO tune procedure failed.
 * @retval false No error.
 */
bool nrfx_clock_xo_tune_error_check(void);

/**
 * @brief Function for checking if XO has been successfully tuned.
 *
 * @retval true  XO is successfully tuned.
 * @retval false XO is not tuned.
 */
bool nrfx_clock_xo_tune_status_check(void);

#endif

/** @brief XO interrupt handler. */
void nrfx_clock_xo_irq_handler(void);

#ifndef NRFX_DECLARE_ONLY

NRFX_STATIC_INLINE bool nrfx_clock_xo_running_check(nrf_clock_hfclk_t * p_clk_src)
{
    bool ret = nrf_clock_is_running(NRF_CLOCK, NRF_CLOCK_DOMAIN_HFCLK, p_clk_src);
    return (ret && (*p_clk_src == NRF_CLOCK_HFCLK_HIGH_ACCURACY));
}

#endif // NRFX_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CLOCK_HAS_XO

#endif // NRFX_CLOCK_XO_H__
