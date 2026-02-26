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

#ifndef NRFX_BELLBOARD_H__
#define NRFX_BELLBOARD_H__

#include <nrfx.h>
#include <haly/nrfy_bellboard.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_bellboard BELLBOARD driver
 * @{
 * @ingroup nrf_bellboard
 * @brief   BELLBOARD peripheral driver.
 */

/** @brief Structure for the BELLBOARD driver instance. */
typedef struct
{
    uint8_t  drv_inst_idx; ///< Index of the driver instance. For internal use only.
    uint8_t  int_idx;      ///< Interrupt index. For internal use only.
} nrfx_bellboard_t;

#ifndef __NRFX_DOXYGEN__
enum {
    NRFX_INSTANCE_ENUM_LIST(BELLBOARD)
    NRFX_BELLBOARD_ENABLED_COUNT
};
#endif

/** @brief Macro for creating a BELLBOARD driver instance. */
#define NRFX_BELLBOARD_INSTANCE(id)                                 \
{                                                                   \
    .drv_inst_idx = NRFX_CONCAT(NRFX_BELLBOARD, id, _INST_IDX),     \
    .int_idx      = id,                                             \
}

/**
 * @brief Bellboard event handler callback.
 *
 * @param[in] event_idx Bellboard event index.
 * @param[in] p_context User context.
 */
typedef void (*nrfx_bellboard_event_handler_t)(uint8_t event_idx, void * p_context);

/**
 * @brief Initialize BELLBOARD driver instance.
 *
 * @param[in] p_instance    Pointer to BELLBOARD driver instance.
 * @param[in] irq_priority  Interrupt priority.
 * @param[in] event_handler Function to be called on bellboard interrupt.
 * @param[in] p_context     User context passed to event handler.
 *
 * @retval 0         Instance successfully initialized.
 * @retval -EALREADY Instance already initialized.
 */
int nrfx_bellboard_init(nrfx_bellboard_t const *       p_instance,
                        uint8_t                        irq_priority,
                        nrfx_bellboard_event_handler_t event_handler,
                        void *                         p_context);

/**
 * @brief Uninitialize BELLBOARD driver instance.
 *
 * @param[in] p_instance Pointer to BELLBOARD driver instance.
 */
void nrfx_bellboard_uninit(nrfx_bellboard_t const * p_instance);

/**
 * @brief Function for checking if the BELLBOARD driver instance is initialized.
 *
 * @param[in] p_instance Pointer to BELLBOARD driver instance.
 *
 * @retval true  Instance is already initialized.
 * @retval false Instance is not initialized.
 */
bool nrfx_bellboard_init_check(nrfx_bellboard_t const * p_instance);

/**
 * @brief Enable interrupt mask for given bellboard interrupt.
 *
 * @param[in] p_instance Pointer to BELLBOARD driver instance.
 * @param[in] mask       Interrupt mask.
 */
void nrfx_bellboard_int_enable(nrfx_bellboard_t const * p_instance, uint32_t mask);

/**
 * @brief Disable interrupt mask for given bellboard interrupt.
 *
 * @param[in] p_instance Pointer to BELLBOARD driver instance.
 * @param[in] mask       Interrupt mask.
 */
void nrfx_bellboard_int_disable(nrfx_bellboard_t const * p_instance, uint32_t mask);

/** @} */

NRFX_INSTANCE_IRQ_HANDLERS_DECLARE(BELLBOARD, bellboard)

#ifdef __cplusplus
}
#endif

#endif // NRFX_BELLBOARD_H__
