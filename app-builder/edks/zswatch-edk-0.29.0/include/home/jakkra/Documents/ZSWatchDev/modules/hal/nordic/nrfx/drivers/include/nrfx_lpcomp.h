/*
 * Copyright (c) 2014 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_LPCOMP_H__
#define NRFX_LPCOMP_H__

#include <nrfx.h>
#include <haly/nrfy_lpcomp.h>
#include <helpers/nrfx_analog_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_lpcomp LPCOMP driver
 * @{
 * @ingroup nrf_lpcomp
 * @brief   Low Power Comparator (LPCOMP) peripheral driver.
 */

/**
 * @brief LPCOMP event handler function type.
 * @param[in] event LPCOMP event.
 */
typedef void (* nrfx_lpcomp_event_handler_t)(nrf_lpcomp_event_t event);

/** @brief LPCOMP configuration. */
typedef struct
{
    nrf_lpcomp_ref_t    reference;          ///< Reference selection.
    nrfx_analog_input_t ext_ref;            ///< External analog reference selection.
    nrf_lpcomp_detect_t detection;          ///< Detection type.
#if NRF_LPCOMP_HAS_HYST
    nrf_lpcomp_hyst_t   hyst;               ///< Comparator hysteresis.
#endif
    nrfx_analog_input_t input;              ///< Input to be monitored.
    uint8_t             interrupt_priority; ///< LPCOMP interrupt priority.
} nrfx_lpcomp_config_t;

/**
 * @brief LPCOMP driver default configuration.
 *
 * This configuration sets up LPCOMP with the following options:
 * - reference voltage: 4/8 of supply voltage
 * - detection of both up and down crossings
 * - hysteresis disabled
 *
 * @param[in] _input Comparator input pin.
 */
#define NRFX_LPCOMP_DEFAULT_CONFIG(_input)                                               \
{                                                                                        \
    .reference = NRF_LPCOMP_REF_SUPPLY_4_8,                                              \
    .detection = NRF_LPCOMP_DETECT_CROSS,                                                \
    NRFX_COND_CODE_1(LPCOMP_FEATURE_HYST_PRESENT, (.hyst = NRF_LPCOMP_HYST_NOHYST,), ()) \
    .input = (nrfx_analog_input_t)_input,                                                \
    .interrupt_priority = NRFX_LPCOMP_DEFAULT_CONFIG_IRQ_PRIORITY                        \
}

/**
 * @brief Function for initializing the LPCOMP driver.
 *
 * This function initializes the LPCOMP driver, but does not enable the peripheral or any interrupts.
 * To start the driver, call the function nrfx_lpcomp_enable() after initialization.
 *
 * @param[in] p_config      Pointer to the structure with the initial configuration.
 * @param[in] event_handler Event handler provided by the user.
 *                          Must not be NULL.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver is already initialized.
 * @retval -EBUSY    The COMP peripheral is already in use.
 *                   This is possible only if @ref nrfx_prs module is enabled.
 * @retval -EINVAL   The analog input pin or external reference is invalid.
 */
int nrfx_lpcomp_init(nrfx_lpcomp_config_t const * p_config,
                     nrfx_lpcomp_event_handler_t  event_handler);

/**
 * @brief Function for reconfiguring the LPCOMP driver.
 *
 * @param[in] p_config Pointer to the structure with the configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EBUSY       The driver is running and cannot be reconfigured.
 * @retval -EINPROGRESS The driver is uninitialized.
 * @retval -EINVAL      The analog input pin or external reference is invalid.
 */
int nrfx_lpcomp_reconfigure(nrfx_lpcomp_config_t const * p_config);

/**
 * @brief Function for uninitializing the LPCOMP driver.
 *
 * This function uninitializes the LPCOMP driver. The LPCOMP peripheral and
 * its interrupts are disabled, and local variables are cleaned. After this call, you must
 * initialize the driver again by calling nrfx_lpcomp_init() if you want to use it.
 *
 * @sa nrfx_lpcomp_disable
 * @sa nrfx_lpcomp_init
 */
void  nrfx_lpcomp_uninit(void);

/**
 * @brief Function for checking if the LPCOMP driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_lpcomp_init_check(void);

/**
 * @brief Function for starting the LPCOMP peripheral and interrupts.
 *
 * Before calling this function, the driver must be initialized. This function
 * enables the LPCOMP peripheral and its interrupts.
 *
 * @param[in] lpcomp_evt_en_mask Mask of events to be enabled. This parameter is to be built as
 *                               an OR of elements from @ref nrf_lpcomp_int_mask_t.
 * @param[in] lpcomp_shorts_mask Mask of shortcuts to be enabled. This parameter is to be built as
 *                               an OR of elements from @ref nrf_lpcomp_short_mask_t.
 *
 * @sa nrfx_lpcomp_init
 */
void nrfx_lpcomp_start(uint32_t lpcomp_evt_en_mask, uint32_t lpcomp_shorts_mask);

/**
 * @brief Function for stopping the LPCOMP peripheral.
 *
 * Before calling this function, the driver must be enabled. This function disables the LPCOMP
 * peripheral and its interrupts.
 *
 * @sa nrfx_lpcomp_uninit
 */
void nrfx_lpcomp_stop(void);

/**
 * @brief Function for copying the current state of the low power comparator result to the RESULT register.
 *
 * @retval 0 The input voltage is below the threshold (VIN+ < VIN-).
 * @retval 1 The input voltage is above the threshold (VIN+ > VIN-).
 */
uint32_t nrfx_lpcomp_sample(void);

/** @} */


void nrfx_lpcomp_irq_handler(void);


#ifdef __cplusplus
}
#endif

#endif // NRFX_LPCOMP_H__
