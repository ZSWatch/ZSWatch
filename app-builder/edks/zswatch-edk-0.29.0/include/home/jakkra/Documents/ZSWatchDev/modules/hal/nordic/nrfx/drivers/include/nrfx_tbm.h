/*
 * Copyright (c) 2022 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_TBM_H__
#define NRFX_TBM_H__

#include <nrfx.h>
#include <hal/nrf_tbm.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_tbm TBM driver
 * @{
 * @ingroup nrf_tbm
 * @brief   Trace Buffer Monitor (TBM) driver.
 */

/** @brief Structure for TBM configuration. */
typedef struct
{
    uint32_t size;               /**< Buffer size (32 bit words). */
    uint8_t  interrupt_priority; /**< Interrupt priority. */
} nrfx_tbm_config_t;

/** @brief tbm default configuration. */
#define NRFX_TBM_DEFAULT_CONFIG                                        \
    {                                                                  \
        .size = 128,                                                   \
        .interrupt_priority = NRFX_TBM_DEFAULT_CONFIG_IRQ_PRIORITY,    \
    }

/**
 * @brief tbm driver data ready handler type.
 *
 * @param event Event.
 */
typedef void (* nrfx_tbm_event_handler_t)(nrf_tbm_event_t event);

/**
 * @brief Function for initializing the TBM driver.
 *
 * @param[in] p_config  pointer to the structure with initial configuration.
 * @param[in] handler   data handler provided by the user. if not provided,
 *                      the driver is initialized in blocking mode.
 *
 * @retval 0         Driver was successfully initialized.
 * @retval -EALREADY Driver was already initialized.
 */
int nrfx_tbm_init(nrfx_tbm_config_t const * p_config, nrfx_tbm_event_handler_t handler);

/** @brief Function for starting the TBM. */
void nrfx_tbm_start(void);

/** @brief Function for stopping the TBM. */
void nrfx_tbm_stop(void);

/** @brief Function for uninitializing the TBM driver. */
void nrfx_tbm_uninit(void);

/**
 * @brief Function for checking if the TBM driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_tbm_init_check(void);

/**
 * @brief Function for getting current counter value.
 *
 * @return Current counter value.
 */
uint32_t nrfx_tbm_count_get(void);

/** @} */

void nrfx_tbm_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif // NRFX_TBM_H__
