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

#ifndef NRFY_COMMON_H__
#define NRFY_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfy_common Common nrfy module
 * @{
 * @ingroup nrfx
 * @brief Common nrfy module.
 * @note Extended Hardware Access Layer (HALY) is deprecated.
 */

/**
 * @brief Macro for calculating interrupt bit position associated with the specified event.
 *
 * @param[in] event Event.
 *
 * @return Interrupt bit position.
 */
#define NRFY_EVENT_TO_INT_BITPOS(event) ((((uint32_t)(event)) - 0x100) >> 2)

/**
 * @brief Macro for converting interrupt bit position to the specified event.
 *
 * @param[in] bitpos Interrupt bit position.
 *
 * @return Event.
 */
#define NRFY_INT_BITPOS_TO_EVENT(bitpos) (((bitpos) << 2) + 0x100)

/**
 * @brief Macro for calculating interrupt bitmask associated with the specified event.
 *
 * @param[in] event Event.
 *
 * @return Interrupt bitmask.
 */
#define NRFY_EVENT_TO_INT_BITMASK(event) (1U << NRFY_EVENT_TO_INT_BITPOS(event))

/** @sa NRFX_IRQ_PRIORITY_SET */
#define NRFY_IRQ_PRIORITY_SET(irq_number, priority) NRFX_IRQ_PRIORITY_SET(irq_number, priority)

/** @sa NRFX_IRQ_ENABLE */
#define NRFY_IRQ_ENABLE(irq_number) NRFX_IRQ_ENABLE(irq_number)

/** @sa NRFX_IRQ_IS_ENABLED */
#define NRFY_IRQ_IS_ENABLED(irq_number) NRFX_IRQ_IS_ENABLED(irq_number)

/** @sa NRFX_IRQ_DISABLE */
#define NRFY_IRQ_DISABLE(irq_number) NRFX_IRQ_DISABLE(irq_number)

/** @sa NRFX_IRQ_PENDING_SET */
#define NRFY_IRQ_PENDING_SET(irq_number) NRFX_IRQ_PENDING_SET(irq_number)

/** @sa NRFX_IRQ_PENDING_CLEAR */
#define NRFY_IRQ_PENDING_CLEAR(irq_number) NRFX_IRQ_PENDING_CLEAR(irq_number)

/** @sa NRFX_IRQ_IS_PENDING */
#define NRFY_IRQ_IS_PENDING(irq_number) NRFX_IRQ_IS_PENDING(irq_number)

/** @sa NRFX_CRITICAL_SECTION_ENTER */
#define NRFY_CRITICAL_SECTION_ENTER() NRFX_CRITICAL_SECTION_ENTER()

/** @sa NRFX_CRITICAL_SECTION_EXIT */
#define NRFY_CRITICAL_SECTION_EXIT() NRFX_CRITICAL_SECTION_EXIT()

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFY_COMMON_H__
