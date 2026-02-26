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

#ifndef NRFY_GPIO_H__
#define NRFY_GPIO_H__

#include <nrfx.h>
#include <hal/nrf_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfy_gpio GPIO HALY
 * @{
 * @ingroup nrf_gpio
 * @brief   Hardware access layer with cache and barrier support for managing the GPIO peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @refhal{nrf_gpio_range_cfg_output} */
NRFY_STATIC_INLINE void nrfy_gpio_range_cfg_output(uint32_t pin_range_start,
                                                   uint32_t pin_range_end)
{
    nrf_gpio_range_cfg_output(pin_range_start, pin_range_end);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_range_cfg_input} */
NRFY_STATIC_INLINE void nrfy_gpio_range_cfg_input(uint32_t            pin_range_start,
                                                  uint32_t            pin_range_end,
                                                  nrf_gpio_pin_pull_t pull_config)
{
    nrf_gpio_range_cfg_input(pin_range_start, pin_range_end, pull_config);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_cfg} */
NRFY_STATIC_INLINE void nrfy_gpio_cfg(uint32_t             pin_number,
                                      nrf_gpio_pin_dir_t   dir,
                                      nrf_gpio_pin_input_t input,
                                      nrf_gpio_pin_pull_t  pull,
                                      nrf_gpio_pin_drive_t drive,
                                      nrf_gpio_pin_sense_t sense)
{
    nrf_gpio_cfg(pin_number, dir, input, pull, drive, sense);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_reconfigure} */
NRFY_STATIC_INLINE void nrfy_gpio_reconfigure(uint32_t                     pin_number,
                                              const nrf_gpio_pin_dir_t *   p_dir,
                                              const nrf_gpio_pin_input_t * p_input,
                                              const nrf_gpio_pin_pull_t *  p_pull,
                                              const nrf_gpio_pin_drive_t * p_drive,
                                              const nrf_gpio_pin_sense_t * p_sense)
{
    nrf_gpio_reconfigure(pin_number, p_dir, p_input, p_pull, p_drive, p_sense);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_cfg_output} */
NRFY_STATIC_INLINE void nrfy_gpio_cfg_output(uint32_t pin_number)
{
    nrf_gpio_cfg_output(pin_number);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_cfg_input} */
NRFY_STATIC_INLINE void nrfy_gpio_cfg_input(uint32_t pin_number, nrf_gpio_pin_pull_t pull_config)
{
    nrf_gpio_cfg_input(pin_number, pull_config);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_cfg_default} */
NRFY_STATIC_INLINE void nrfy_gpio_cfg_default(uint32_t pin_number)
{
    nrf_gpio_cfg_default(pin_number);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_cfg_watcher} */
NRFY_STATIC_INLINE void nrfy_gpio_cfg_watcher(uint32_t pin_number)
{
    nrf_barrier_r();
    nrf_gpio_cfg_watcher(pin_number);
    nrf_barrier_rw();
}

/** @refhal{nrf_gpio_input_disconnect} */
NRFY_STATIC_INLINE void nrfy_gpio_input_disconnect(uint32_t pin_number)
{
    nrf_barrier_r();
    nrf_gpio_input_disconnect(pin_number);
    nrf_barrier_rw();
}

/** @refhal{nrf_gpio_cfg_sense_input} */
NRFY_STATIC_INLINE void nrfy_gpio_cfg_sense_input(uint32_t             pin_number,
                                                  nrf_gpio_pin_pull_t  pull_config,
                                                  nrf_gpio_pin_sense_t sense_config)
{
    nrf_gpio_cfg_sense_input(pin_number, pull_config, sense_config);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_cfg_sense_set} */
NRFY_STATIC_INLINE void nrfy_gpio_cfg_sense_set(uint32_t             pin_number,
                                                nrf_gpio_pin_sense_t sense_config)
{
    nrf_barrier_r();
    nrf_gpio_cfg_sense_set(pin_number, sense_config);
    nrf_barrier_rw();
}

/** @refhal{nrf_gpio_pin_dir_set} */
NRFY_STATIC_INLINE void nrfy_gpio_pin_dir_set(uint32_t pin_number, nrf_gpio_pin_dir_t direction)
{
    nrf_gpio_pin_dir_set(pin_number, direction);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_pin_set} */
NRFY_STATIC_INLINE void nrfy_gpio_pin_set(uint32_t pin_number)
{
    nrf_gpio_pin_set(pin_number);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_pin_clear} */
NRFY_STATIC_INLINE void nrfy_gpio_pin_clear(uint32_t pin_number)
{
    nrf_gpio_pin_clear(pin_number);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_pin_toggle} */
NRFY_STATIC_INLINE void nrfy_gpio_pin_toggle(uint32_t pin_number)
{
    nrf_gpio_pin_toggle(pin_number);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_pin_write} */
NRFY_STATIC_INLINE void nrfy_gpio_pin_write(uint32_t pin_number, uint32_t value)
{
    nrf_gpio_pin_write(pin_number, value);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_pin_read} */
NRFY_STATIC_INLINE uint32_t nrfy_gpio_pin_read(uint32_t pin_number)
{
    nrf_barrier_r();
    uint32_t pin = nrf_gpio_pin_read(pin_number);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_gpio_pin_out_read} */
NRFY_STATIC_INLINE uint32_t nrfy_gpio_pin_out_read(uint32_t pin_number)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_gpio_pin_out_read(pin_number);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_gpio_pin_sense_get} */
NRFY_STATIC_INLINE nrf_gpio_pin_sense_t nrfy_gpio_pin_sense_get(uint32_t pin_number)
{
    nrf_barrier_rw();
    nrf_gpio_pin_sense_t pin_sense = nrf_gpio_pin_sense_get(pin_number);
    nrf_barrier_r();
    return pin_sense;
}

/** @refhal{nrf_gpio_pin_dir_get} */
NRFY_STATIC_INLINE nrf_gpio_pin_dir_t nrfy_gpio_pin_dir_get(uint32_t pin_number)
{
    nrf_barrier_rw();
    nrf_gpio_pin_dir_t pin_dir = nrf_gpio_pin_dir_get(pin_number);
    nrf_barrier_r();
    return pin_dir;
}

/** @refhal{nrf_gpio_pin_input_get} */
NRFY_STATIC_INLINE nrf_gpio_pin_input_t nrfy_gpio_pin_input_get(uint32_t pin_number)
{
    nrf_barrier_rw();
    nrf_gpio_pin_input_t pin_input = nrf_gpio_pin_input_get(pin_number);
    nrf_barrier_r();
    return pin_input;
}

/** @refhal{nrf_gpio_pin_pull_get} */
NRFY_STATIC_INLINE nrf_gpio_pin_pull_t nrfy_gpio_pin_pull_get(uint32_t pin_number)
{
    nrf_barrier_rw();
    nrf_gpio_pin_pull_t pin_pull = nrf_gpio_pin_pull_get(pin_number);
    nrf_barrier_r();
    return pin_pull;
}

/** @refhal{nrf_gpio_port_dir_output_set} */
NRFY_STATIC_INLINE void nrfy_gpio_port_dir_output_set(NRF_GPIO_Type * p_reg, uint32_t out_mask)
{
    nrf_gpio_port_dir_output_set(p_reg, out_mask);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_port_dir_input_set} */
NRFY_STATIC_INLINE void nrfy_gpio_port_dir_input_set(NRF_GPIO_Type * p_reg, uint32_t in_mask)
{
    nrf_gpio_port_dir_input_set(p_reg, in_mask);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_port_dir_write} */
NRFY_STATIC_INLINE void nrfy_gpio_port_dir_write(NRF_GPIO_Type * p_reg, uint32_t dir_mask)
{
    nrf_gpio_port_dir_write(p_reg, dir_mask);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_port_dir_read} */
NRFY_STATIC_INLINE uint32_t nrfy_gpio_port_dir_read(NRF_GPIO_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t port_dir = nrf_gpio_port_dir_read(p_reg);
    nrf_barrier_r();
    return port_dir;
}

/** @refhal{nrf_gpio_port_in_read} */
NRFY_STATIC_INLINE uint32_t nrfy_gpio_port_in_read(NRF_GPIO_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t port_in = nrf_gpio_port_in_read(p_reg);
    nrf_barrier_r();
    return port_in;
}

/** @refhal{nrf_gpio_port_out_read} */
NRFY_STATIC_INLINE uint32_t nrfy_gpio_port_out_read(NRF_GPIO_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t port_out = nrf_gpio_port_out_read(p_reg);
    nrf_barrier_r();
    return port_out;
}

/** @refhal{nrf_gpio_port_out_write} */
NRFY_STATIC_INLINE void nrfy_gpio_port_out_write(NRF_GPIO_Type * p_reg, uint32_t value)
{
    nrf_gpio_port_out_write(p_reg, value);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_port_out_set} */
NRFY_STATIC_INLINE void nrfy_gpio_port_out_set(NRF_GPIO_Type * p_reg, uint32_t set_mask)
{
    nrf_gpio_port_out_set(p_reg, set_mask);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_port_out_clear} */
NRFY_STATIC_INLINE void nrfy_gpio_port_out_clear(NRF_GPIO_Type * p_reg, uint32_t clr_mask)
{
    nrf_gpio_port_out_clear(p_reg, clr_mask);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_ports_read} */
NRFY_STATIC_INLINE void nrfy_gpio_ports_read(uint32_t   start_port,
                                             uint32_t   length,
                                             uint32_t * p_masks)
{
    nrf_barrier_r();
    nrf_gpio_ports_read(start_port, length, p_masks);
    nrf_barrier_r();
}

#if defined(NRF_GPIO_LATCH_PRESENT)
/** @refhal{nrf_gpio_latches_read} */
NRFY_STATIC_INLINE void nrfy_gpio_latches_read(uint32_t   start_port,
                                               uint32_t   length,
                                               uint32_t * p_masks)
{
    nrf_barrier_r();
    nrf_gpio_latches_read(start_port, length, p_masks);
    nrf_barrier_r();
}

/** @refhal{nrf_gpio_latches_read_and_clear} */
NRFY_STATIC_INLINE void nrfy_gpio_latches_read_and_clear(uint32_t   start_port,
                                                         uint32_t   length,
                                                         uint32_t * p_masks)
{
    nrf_barrier_r();
    nrf_gpio_latches_read_and_clear(start_port, length, p_masks);
    nrf_barrier_rw();
}

/** @refhal{nrf_gpio_pin_latch_get} */
NRFY_STATIC_INLINE uint32_t nrfy_gpio_pin_latch_get(uint32_t pin_number)
{
    nrf_barrier_r();
    uint32_t pin_latch = nrf_gpio_pin_latch_get(pin_number);
    nrf_barrier_r();
    return pin_latch;
}

/** @refhal{nrf_gpio_pin_latch_clear} */
NRFY_STATIC_INLINE void nrfy_gpio_pin_latch_clear(uint32_t pin_number)
{
    nrf_gpio_pin_latch_clear(pin_number);
    nrf_barrier_w();
}
#endif // defined(NRF_GPIO_LATCH_PRESENT)

#if NRF_GPIO_HAS_SEL
/** @refhal{nrf_gpio_pin_control_select} */
NRFY_STATIC_INLINE void nrfy_gpio_pin_control_select(uint32_t pin_number, nrf_gpio_pin_sel_t ctrl)
{
    nrf_barrier_r();
    nrf_gpio_pin_control_select(pin_number, ctrl);
    nrf_barrier_rw();
}
#endif

#if NRF_GPIO_HAS_CLOCKPIN
/** @refhal{nrf_gpio_pin_clock_set} */
NRFY_STATIC_INLINE void nrfy_gpio_pin_clock_set(uint32_t pin_number, bool enable)
{
    nrf_gpio_pin_clock_set(pin_number, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_gpio_pin_clock_check} */
NRFY_STATIC_INLINE bool nrfy_gpio_pin_clock_check(uint32_t pin_number)
{
    nrf_barrier_rw();
    bool pin_clock = nrf_gpio_pin_clock_check(pin_number);
    nrf_barrier_r();
    return pin_clock;
}
#endif

/** @refhal{nrf_gpio_pin_present_check} */
NRFY_STATIC_INLINE bool nrfy_gpio_pin_present_check(uint32_t pin_number)
{
    return nrf_gpio_pin_present_check(pin_number);
}

/** @refhal{nrf_gpio_pin_port_number_extract} */
NRFY_STATIC_INLINE uint32_t nrfy_gpio_pin_port_number_extract(uint32_t * p_pin)
{
    return nrf_gpio_pin_port_number_extract(p_pin);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFY_GPIO_H__
