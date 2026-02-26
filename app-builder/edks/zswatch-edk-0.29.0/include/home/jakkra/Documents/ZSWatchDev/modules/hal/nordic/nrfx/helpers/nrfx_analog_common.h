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

#ifndef NRFX_ANALOG_COMMON_H__
#define NRFX_ANALOG_COMMON_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_analog_common Generic analog input layer.
 * @{
 * @ingroup nrfx
 * @ingroup nrf_saadc
 * @ingroup nrf_comp
 * @ingroup nrf_lpcomp
 *
 * @brief Helper layer that provides the common functionality of SAADC, COMP and LPCOMP drivers.
 */

/** @brief Symbol specifying internal reference voltage. */
#define NRFX_ANALOG_REF_INTERNAL_VAL ANALOG_REF_INTERNAL_VAL

/** @brief Symbol specifying internal inputs offset. */
#define NRFX_ANALOG_AIN_INTERNAL_OFFSET 128

/** @brief Generic analog input types. */
typedef enum {
    NRFX_ANALOG_EXTERNAL_AIN0 = 0,                              ///< External analog input 0
    NRFX_ANALOG_EXTERNAL_AIN1,                                  ///< External analog input 1
    NRFX_ANALOG_EXTERNAL_AIN2,                                  ///< External analog input 2
    NRFX_ANALOG_EXTERNAL_AIN3,                                  ///< External analog input 3
    NRFX_ANALOG_EXTERNAL_AIN4,                                  ///< External analog input 4
    NRFX_ANALOG_EXTERNAL_AIN5,                                  ///< External analog input 5
    NRFX_ANALOG_EXTERNAL_AIN6,                                  ///< External analog input 6
    NRFX_ANALOG_EXTERNAL_AIN7,                                  ///< External analog input 7
    NRFX_ANALOG_EXTERNAL_AIN8,                                  ///< External analog input 8
    NRFX_ANALOG_EXTERNAL_AIN9,                                  ///< External analog input 9
    NRFX_ANALOG_EXTERNAL_AIN10,                                 ///< External analog input 10
    NRFX_ANALOG_EXTERNAL_AIN11,                                 ///< External analog input 11
    NRFX_ANALOG_EXTERNAL_AIN12,                                 ///< External analog input 12
    NRFX_ANALOG_EXTERNAL_AIN13,                                 ///< External analog input 13
    NRFX_ANALOG_INTERNAL_VDD = NRFX_ANALOG_AIN_INTERNAL_OFFSET, ///< Internal VDD
    NRFX_ANALOG_INTERNAL_VDDDIV2,                               ///< Internal VDD/2
    NRFX_ANALOG_INTERNAL_AVDD,                                  ///< Internal AVDD
    NRFX_ANALOG_INTERNAL_DVDD,                                  ///< Internal DVDD
    NRFX_ANALOG_INTERNAL_VDDHDIV5,                              ///< Internal VDDH/5
    NRFX_ANALOG_INTERNAL_VDDL,                                  ///< Internal VDDL
    NRFX_ANALOG_INTERNAL_DECB,                                  ///< Internal DECB
    NRFX_ANALOG_INTERNAL_VSS,                                   ///< Internal VSS
    NRFX_ANALOG_INTERNAL_VDDAO3V0,                              ///< Internal VDDA - 3.0V
    NRFX_ANALOG_INTERNAL0,                                      ///< Internal input 0
    NRFX_ANALOG_INTERNAL1,                                      ///< Internal input 1
    NRFX_ANALOG_INTERNAL2,                                      ///< Internal input 2
    NRFX_ANALOG_INTERNAL3,                                      ///< Internal input 3
    NRFX_ANALOG_INPUT_DISABLED = UINT8_MAX,                     ///< Analog input disabled
} nrfx_analog_input_t;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRFX_ANALOG_COMMON_H__ */
