/*
 * Copyright (c) 2021 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_VEVIF_H__
#define NRFX_VEVIF_H__

#include <nrfx.h>

#include <hal/nrf_vpr_clic.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NRFX_VEVIF_IRQ_HANDLER_DECLARE(idx, _) \
void nrfx_vevif_##idx##_irq_handler(void);

/**
 * @defgroup nrfx_vevif VEVIF driver
 * @{
 * @ingroup nrf_vpr
 * @brief   VPR Event Interface (VEVIF) mechanism driver.
 */

/**
 * @brief VEVIF event handler callback.
 *
 * @param[in] event_idx VEVIF event index.
 * @param[in] p_context Context passed to the event handler. Set on initialization.
 */
typedef void (*nrfx_vevif_event_handler_t)(uint8_t event_idx, void * p_context);

/**
 * @brief Function for initializing the VEVIF driver.
 *
 * @param[in] interrupt_priority Interrupt priority.
 * @param[in] event_handler      Function to be called on interrupt.
 *                               Must not be NULL.
 * @param[in] p_context          Context passed to the event handler.
 *
 * @retval 0         Driver successfully initialized.
 * @retval -EALREADY The driver is already initialized.
 */
int nrfx_vevif_init(uint8_t                    interrupt_priority,
                    nrfx_vevif_event_handler_t event_handler,
                    void *                     p_context);

/** @brief Function for uninitializing the VEVIF driver. */
void nrfx_vevif_uninit(void);

/**
 * @brief Function for checking if the VEVIF driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_vevif_init_check(void);

/**
 * @brief Function for enabling interrupts on specified VEVIF events.
 *
 * @param[in] mask Mask of interrupts to be enabled.
 */
void nrfx_vevif_int_enable(uint32_t mask);

/**
 * @brief Function for disabling interrupts on specified VEVIF events.
 *
 * @param[in] mask Mask of interrupts to be disabled.
 */
void nrfx_vevif_int_disable(uint32_t mask);

/** @} */

/* Declare interrupt handlers for 0..31 NRF_VEVIF driver instances. */
NRFX_LISTIFY(32, NRFX_VEVIF_IRQ_HANDLER_DECLARE, (;), _)

#ifdef __cplusplus
}
#endif

#endif // NRFX_VEVIF_H__
