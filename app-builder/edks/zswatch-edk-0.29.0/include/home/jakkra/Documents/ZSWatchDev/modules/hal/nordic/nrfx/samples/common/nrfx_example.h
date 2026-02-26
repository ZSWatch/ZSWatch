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

#ifndef NRFX_EXAMPLE_H__
#define NRFX_EXAMPLE_H__

#include <hal/nrf_gpio.h>

#if defined(__ZEPHYR__)
#include <zephyr/logging/log_ctrl.h>
#define NRFX_EXAMPLE_LOG_INIT()    LOG_INIT()
#define NRFX_EXAMPLE_LOG_PROCESS() while (LOG_PROCESS())
#else
#define NRFX_EXAMPLE_LOG_INIT()
#define NRFX_EXAMPLE_LOG_PROCESS()
#endif

#if defined(NRF52832_XXAA) || defined(NRF52832_XXAB)
    #include "boards/nrf52dk_nrf52832_config.h"
#elif defined(NRF52830_XXAA)
    #include "boards/nrf52833dk_nrf52833_config.h"
#elif defined(NRF52833_XXAA)
    #include "boards/nrf52833dk_nrf52833_config.h"
#elif defined(NRF52840_XXAA)
    #include "boards/nrf52840dk_nrf52840_config.h"
#elif defined(NRF53_SERIES)
    #include "boards/nrf5340dk_nrf5340_config.h"
#elif defined(NRF91_SERIES)
    #include "boards/nrf9160dk_nrf9160_config.h"
#elif defined(NRF54L15_XXAA)
    #include "boards/nrf54l15dk_nrf54l15_config.h"
#else
#error "Unknown device."
#endif

#endif // NRFX_EXAMPLE_H__
