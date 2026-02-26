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

#ifndef NRFX_CLOCK_LFCLK_H__
#define NRFX_CLOCK_LFCLK_H__

#include <nrfx.h>
#include <hal/nrf_clock.h>

#if defined(LFRC_PRESENT)
#include <hal/nrf_lfrc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_clock_lfclk LFCLK driver
 * @{
 * @ingroup nrf_clock_lfclk
 * @brief   LFCLK clock driver.
 */

 /** @brief Symbol specifying driver event offset for LFRC hardware events. */
#define NRFX_CLOCK_LFCLK_LFRC_EVT_OFFSET 32

/** @brief Clock events. */
typedef enum
{
    NRFX_CLOCK_LFCLK_EVT_LFCLK_STARTED = NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_LF_STARTED_MASK), ///< LFCLK has been started.
#if NRF_CLOCK_HAS_CALIBRATION_TIMER
    NRFX_CLOCK_LFCLK_EVT_CTTO          = NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_CTTO_MASK),       ///< Calibration timeout.
#endif
#if NRF_CLOCK_HAS_CALIBRATION
    NRFX_CLOCK_LFCLK_EVT_CAL_DONE      = NRFX_BITMASK_TO_BITPOS(NRF_CLOCK_INT_DONE_MASK),       ///< Calibration has been done.
#elif NRFX_CHECK(NRF_LFRC_HAS_CALIBRATION)
    NRFX_CLOCK_LFCLK_EVT_CAL_DONE      = (NRFX_BITMASK_TO_BITPOS(NRF_LFRC_INT_CALDONE_MASK) + \
                                         NRFX_CLOCK_LFCLK_LFRC_EVT_OFFSET),                     ///< Calibration has been done.
#endif
} nrfx_clock_lfclk_evt_type_t;

/**
 * @brief Lfclk event handler.
 *
 * @param[in] event Event.
 */
typedef void (*nrfx_clock_lfclk_event_handler_t)(nrfx_clock_lfclk_evt_type_t event);

/**
 * @brief Function for initializing internal structures in the nrfx_clock_lfclk module.
 *
 * After initialization, the module is in power off state (clocks are not started).
 *
 * @param[in] event_handler Event handler provided by the user.
 *                          If not provided, driver works in blocking mode.
 *
 * @retval 0         The procedure is successful.
 * @retval -EALREADY The driver is already initialized.
 */
int nrfx_clock_lfclk_init(nrfx_clock_lfclk_event_handler_t  event_handler);

/** @brief Function for uninitializing the lfclk module. */
void nrfx_clock_lfclk_uninit(void);

/**
 * @brief Function for checking if the lfclk driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_clock_lfclk_init_check(void);

/**
 * @brief Function for starting the LFCLK.
 */
void nrfx_clock_lfclk_start(void);

/**
 * @brief Function for stopping the LFCLK.
 */
void nrfx_clock_lfclk_stop(void);

/**
 * @brief Function for checking the LFCLK state.
 *
 * XTAL source is assumed for domains with multiple sources.
 *
 * @param[out] p_clk_src Pointer to a clock source that is running. Set to NULL if not needed.
 *
 * @retval true  The clock domain is running.
 * @retval false The clock domain is not running.
 */
NRFX_STATIC_INLINE bool nrfx_clock_lfclk_running_check(nrf_clock_lfclk_t * p_clk_src);

#if ((NRFX_CHECK(NRF_CLOCK_HAS_CALIBRATION) || NRFX_CHECK(NRF_LFRC_HAS_CALIBRATION)) && \
     NRFX_CHECK(NRFX_CLOCK_CONFIG_LF_CAL_ENABLED)) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for starting the calibration of internal LFCLK.
 *
 * This function starts the calibration process. The process cannot be aborted. LFCLK and (HFCLK or XO)
 * must be running before this function is called.
 *
 * @retval 0            The procedure is successful.
 * @retval -EINPROGRESS The low-frequency or high-frequency clock is off.
 * @retval -EBUSY       Clock is in the calibration phase.
 */
int nrfx_clock_lfclk_calibration_start(void);

/**
 * @brief Function for checking if calibration is in progress.
 *
 * This function indicates that the system is in calibration phase.
 *
 * @retval 0      The procedure is successful.
 * @retval -EBUSY Clock is in the calibration phase.
 */
int nrfx_clock_lfclk_calibrating_check(void);

#if (NRF_CLOCK_HAS_CALIBRATION_TIMER && NRFX_CHECK(NRFX_CLOCK_CONFIG_CT_ENABLED)) || \
    defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for starting calibration timer.
 *
 * @param[in] interval Time after which the CTTO event and interrupt will be generated (in 0.25 s units).
 */
void nrfx_clock_lfclk_calibration_timer_start(uint8_t interval);

/** @brief Function for stopping the calibration timer. */
void nrfx_clock_lfclk_calibration_timer_stop(void);
#endif
#endif /* ((NRFX_CHECK(NRF_CLOCK_HAS_CALIBRATION) || NRFX_CHECK(NRF_LFRC_HAS_CALIBRATION)) && \
     NRFX_CHECK(NRFX_CLOCK_CONFIG_LF_CAL_ENABLED)) || defined(__NRFX_DOXYGEN__) */

#ifndef NRFX_DECLARE_ONLY

NRFX_STATIC_INLINE bool nrfx_clock_lfclk_running_check(nrf_clock_lfclk_t * p_clk_src)
{
    return nrf_clock_is_running(NRF_CLOCK, NRF_CLOCK_DOMAIN_LFCLK, (void*)p_clk_src);
}

#endif // NRFX_DECLARE_ONLY

/** @} */

void nrfx_clock_lfclk_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif // NRFX_CLOCK_LFCLK_H__
