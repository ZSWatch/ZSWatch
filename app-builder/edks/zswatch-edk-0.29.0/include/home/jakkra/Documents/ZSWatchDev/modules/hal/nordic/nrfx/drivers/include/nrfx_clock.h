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

#ifndef NRFX_CLOCK_H__
#define NRFX_CLOCK_H__

#include <nrfx.h>
#include <hal/nrf_clock.h>
#include <nrfx_power_clock.h>
#if defined(LFRC_PRESENT)
#include <hal/nrf_lfrc.h>
#endif
#if NRF_CLOCK_HAS_HFCLK
#include <nrfx_clock_hfclk.h>
#endif
#if NRF_CLOCK_HAS_XO
#include <nrfx_clock_xo.h>
#endif
#include <nrfx_clock_lfclk.h>
#if NRF_CLOCK_HAS_HFCLK192M
#include <nrfx_clock_hfclk192m.h>
#endif
#if NRF_CLOCK_HAS_HFCLK24M
#include <nrfx_clock_xo24m.h>
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
#include <nrfx_clock_hfclkaudio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_clock CLOCK driver
 * @{
 * @ingroup nrf_clock
 * @brief   CLOCK peripheral driver.
 */

/** @brief Clock events. */
typedef enum
{
#if NRF_CLOCK_HAS_HFCLK
    NRFX_CLOCK_EVT_HFCLK_STARTED      = NRFX_CLOCK_HFCLK_EVT_HFCLK_STARTED,           ///< HFCLK has been started.
#else
    NRFX_CLOCK_EVT_HFCLK_STARTED      = NRFX_CLOCK_XO_EVT_HFCLK_STARTED,              ///< XO has been started.
#endif
#if NRF_CLOCK_HAS_PLL
    NRFX_CLOCK_EVT_PLL_STARTED        = NRFX_CLOCK_XO_EVT_PLL_STARTED,                ///< PLL has been started.
#endif
    NRFX_CLOCK_EVT_LFCLK_STARTED      = NRFX_CLOCK_LFCLK_EVT_LFCLK_STARTED,           ///< LFCLK has been started.
#if NRF_CLOCK_HAS_CALIBRATION_TIMER
    NRFX_CLOCK_EVT_CTTO               = NRFX_CLOCK_LFCLK_EVT_CTTO,                    ///< Calibration timeout.
#endif
#if NRF_CLOCK_HAS_CALIBRATION || NRFX_CHECK(NRF_LFRC_HAS_CALIBRATION)
    NRFX_CLOCK_EVT_CAL_DONE           = NRFX_CLOCK_LFCLK_EVT_CAL_DONE,                ///< Calibration has been done.
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
    NRFX_CLOCK_EVT_HFCLKAUDIO_STARTED = NRFX_CLOCK_HFCLKAUDIO_EVT_HFCLKAUDIO_STARTED, ///< HFCLKAUDIO has been started.
#endif
#if NRF_CLOCK_HAS_HFCLK24M
    NRFX_CLOCK_EVT_HFCLK24M_STARTED   = NRFX_CLOCK_XO24M_EVT_HFCLK24M_STARTED,        ///< HFCLK24M has been started.
#endif
#if NRF_CLOCK_HAS_HFCLK192M
    NRFX_CLOCK_EVT_HFCLK192M_STARTED  = NRFX_CLOCK_HFCLK192M_EVT_HFCLK192M_STARTED,   ///< HFCLK192M has been started.
#endif
#if NRF_CLOCK_HAS_XO_TUNE
    NRFX_CLOCK_EVT_XO_TUNED           = NRFX_CLOCK_XO_EVT_XO_TUNED,                   ///< XO tune has been done.
    NRFX_CLOCK_EVT_XO_TUNE_ERROR      = NRFX_CLOCK_XO_EVT_XO_TUNE_ERROR,              ///< XO is not tuned.
    NRFX_CLOCK_EVT_XO_TUNE_FAILED     = NRFX_CLOCK_XO_EVT_XO_TUNE_FAILED,             ///< XO tune operation failed.
#endif
} nrfx_clock_evt_type_t;

/**
 * @brief Clock event handler.
 *
 * @param[in] event Event.
 */
typedef void (*nrfx_clock_event_handler_t)(nrfx_clock_evt_type_t event);

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
int nrfx_clock_init(nrfx_clock_event_handler_t  event_handler);

/** @brief Function for enabling interrupts in the clock module. */
void nrfx_clock_enable(void);

/** @brief Function for disabling interrupts in the clock module. */
void nrfx_clock_disable(void);

/** @brief Function for uninitializing the clock module. */
void nrfx_clock_uninit(void);

/**
 * @brief Function for checking if the clock driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_clock_init_check(void);

/**
 * @brief Function for starting the specified clock domain.
 *
 * @param[in] domain Clock domain.
 */
void nrfx_clock_start(nrf_clock_domain_t domain);

/**
 * @brief Function for stopping the specified clock domain.
 *
 * @param[in] domain Clock domain.
 */
void nrfx_clock_stop(nrf_clock_domain_t domain);

/**
 * @brief Function for checking the specified clock domain state.
 *
 * XTAL source is assumed for domains with multiple sources.
 *
 * @param[in]  domain    Clock domain.
 * @param[out] p_clk_src Pointer to a clock source that is running. Set to NULL if not needed.
 *                       Ignored for HFCLKAUDIO domain. Variable pointed by @p p_clk_src
 *                       must be of either @ref nrf_clock_lfclk_t type for LFCLK
 *                       or @ref nrf_clock_hfclk_t type for HFCLK and HFCLK192M.
 *
 * @retval true  The clock domain is running.
 * @retval false The clock domain is not running.
 */
NRFX_STATIC_INLINE bool nrfx_clock_is_running(nrf_clock_domain_t domain, void * p_clk_src);

#if NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT || NRF_CLOCK_HAS_HFCLK192M || \
    defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the specified clock domain divider.
 *
 * @param[in] domain Clock domain.
 * @param[in] div    New divider for the clock domain.
 *
 * @retval 0        Divider successfully set.
 * @retval -ENOTSUP Domain does not support setting the divider.
 * @retval -EINVAL  Divider not supported by the specified domain.
 */
int nrfx_clock_divider_set(nrf_clock_domain_t    domain,
                           nrf_clock_hfclk_div_t div);

/**
 * @brief Function for getting the specified clock domain divider.
 *
 * @param[in] domain Clock domain.
 *
 * @return Current divider for the specified clock domain.
 */

NRFX_STATIC_INLINE nrf_clock_hfclk_div_t nrfx_clock_divider_get(nrf_clock_domain_t domain);
#endif

#if ((NRF_CLOCK_HAS_CALIBRATION || NRFX_CHECK(NRF_LFRC_HAS_CALIBRATION)) && \
     NRFX_CHECK(NRFX_CLOCK_CONFIG_LF_CAL_ENABLED)) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for starting the calibration of internal LFCLK.
 *
 * This function starts the calibration process. The process cannot be aborted. LFCLK and HFCLK
 * must be running before this function is called.
 *
 * @retval 0            The procedure is successful.
 * @retval -EINPROGRESS The low-frequency or high-frequency clock is off.
 * @retval -EBUSY       Clock is in the calibration phase.
 */
int nrfx_clock_calibration_start(void);

/**
 * @brief Function for checking if calibration is in progress.
 *
 * This function indicates that the system is in calibration phase.
 *
 * @retval 0      The procedure is successful.
 * @retval -EBUSY Clock is in the calibration phase.
 */
int nrfx_clock_is_calibrating(void);

#if (NRF_CLOCK_HAS_CALIBRATION_TIMER && NRFX_CHECK(NRFX_CLOCK_CONFIG_CT_ENABLED)) || \
    defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for starting calibration timer.
 *
 * @param[in] interval Time after which the CTTO event and interrupt will be generated (in 0.25 s units).
 */
void nrfx_clock_calibration_timer_start(uint8_t interval);

/** @brief Function for stopping the calibration timer. */
void nrfx_clock_calibration_timer_stop(void);
#endif
#endif /* ((NRF_CLOCK_HAS_CALIBRATION || NRFX_CHECK(NRF_LFRC_HAS_CALIBRATION)) && \
           NRFX_CHECK(NRFX_CLOCK_CONFIG_LF_CAL_ENABLED)) || defined(__NRFX_DOXYGEN__) */

/**
 * @brief Function for returning a requested task address for the clock driver module.
 *
 * @param[in] task One of the peripheral tasks.
 *
 * @return Task address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_clock_task_address_get(nrf_clock_task_t task);

/**
 * @brief Function for returning a requested event address for the clock driver module.
 *
 * @param[in] event One of the peripheral events.
 *
 * @return Event address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_clock_event_address_get(nrf_clock_event_t event);

#ifndef NRFX_DECLARE_ONLY

#if NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT || NRF_CLOCK_HAS_HFCLK192M
NRFX_STATIC_INLINE nrf_clock_hfclk_div_t nrfx_clock_divider_get(nrf_clock_domain_t domain)
{
    switch (domain)
    {
#if NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
        case NRF_CLOCK_DOMAIN_HFCLK:
            return nrfx_clock_hfclk_divider_get();
#endif
#if NRF_CLOCK_HAS_HFCLK192M
        case NRF_CLOCK_DOMAIN_HFCLK192M:
            return nrfx_clock_hfclk192m_divider_get();
#endif
        default:
            NRFX_ASSERT(0);
            return (nrf_clock_hfclk_div_t)0;
    }
}
#endif // NRF_CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT || NRF_CLOCK_HAS_HFCLK192M

NRFX_STATIC_INLINE uint32_t nrfx_clock_task_address_get(nrf_clock_task_t task)
{
    return nrf_clock_task_address_get(NRF_CLOCK, task);
}

NRFX_STATIC_INLINE uint32_t nrfx_clock_event_address_get(nrf_clock_event_t event)
{
    return nrf_clock_event_address_get(NRF_CLOCK, event);
}

NRFX_STATIC_INLINE bool nrfx_clock_is_running(nrf_clock_domain_t domain, void * p_clk_src)
{
    switch (domain)
    {
        case NRF_CLOCK_DOMAIN_HFCLK:
#if NRF_CLOCK_HAS_HFCLK
            return nrfx_clock_hfclk_running_check((nrf_clock_hfclk_t *)p_clk_src);
#elif NRF_CLOCK_HAS_XO
            return nrfx_clock_xo_running_check((nrf_clock_hfclk_t *)p_clk_src);
#endif
        case NRF_CLOCK_DOMAIN_LFCLK:
            return nrfx_clock_lfclk_running_check((nrf_clock_lfclk_t *)p_clk_src);
#if NRF_CLOCK_HAS_HFCLK192M
        case NRF_CLOCK_DOMAIN_HFCLK192M:
            return nrfx_clock_hfclk192m_running_check((nrf_clock_hfclk_t *)p_clk_src);
#endif
#if NRF_CLOCK_HAS_HFCLK24M
        case NRF_CLOCK_DOMAIN_HFCLK24M:
            return nrfx_clock_xo24m_running_check();
#endif
#if NRF_CLOCK_HAS_HFCLKAUDIO
        case NRF_CLOCK_DOMAIN_HFCLKAUDIO:
            return nrfx_clock_hfclkaudio_running_check();
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

#endif // NRFX_DECLARE_ONLY

/** @} */


void nrfx_clock_irq_handler(void);


#ifdef __cplusplus
}
#endif

#endif // NRFX_CLOCK_H__
