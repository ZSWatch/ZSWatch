/*
 * Copyright (c) 2015 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_SAADC_H_
#define NRF_SAADC_H_

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_saadc_hal SAADC HAL
 * @{
 * @ingroup nrf_saadc
 * @brief   Hardware access layer for managing the SAADC peripheral.
 */

/** @brief Symbol specifying the offset of interrupt bitmask for limits of all channels. */
#define NRF_SAADC_LIMITS_INT_OFFSET \
    NRFX_MIN(SAADC_INTENSET_CH0LIMITH_Pos, SAADC_INTENSET_CH0LIMITL_Pos)

/** @brief Symbol specifying number of LIMIT events per channel. */
#define NRF_SAADC_LIMITS_PER_CHANNEL (2)

/** @brief Symbol specifying the interrupt bitmask for limits of all channels. */
#define NRF_SAADC_ALL_CHANNELS_LIMITS_INT_MASK                             \
    ((uint32_t)(NRFX_BIT_MASK(SAADC_CH_NUM * NRF_SAADC_LIMITS_PER_CHANNEL) \
                << NRF_SAADC_LIMITS_INT_OFFSET))

#if defined(SAADC_CH_CONFIG_TACQ_3us) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the configuration of acquisition time using predefined values is present. */
#define NRF_SAADC_HAS_ACQTIME_ENUM 1
#else
#define NRF_SAADC_HAS_ACQTIME_ENUM 0
#endif

#if defined(SAADC_CH_CONFIG_TCONV_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the configuration of conversion time is present. */
#define NRF_SAADC_HAS_CONVTIME 1
#else
#define NRF_SAADC_HAS_CONVTIME 0
#endif

#if defined(SAADC_TRIM_LINCALCOEFF_VAL_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the configuration of linearity calibration coefficients is present. */
#define NRF_SAADC_HAS_LIN_CAL 1
#else
#define NRF_SAADC_HAS_LIN_CAL 0
#endif

#if defined(SAADC_CAL_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC CAL trim config is present. */
#define NRF_SAADC_HAS_CAL 1
#else
#define NRF_SAADC_HAS_CAL 0
#endif

#if defined(SAADC_CALREF_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC CALREF trim is present. */
#define NRF_SAADC_HAS_CALREF 1
#else
#define NRF_SAADC_HAS_CALREF 0
#endif

#if defined(SAADC_CH_PSELP_PIN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether configuration of analog input using pin number is present. */
#define NRF_SAADC_HAS_AIN_AS_PIN 1
#else
#define NRF_SAADC_HAS_AIN_AS_PIN 0
#endif

#if defined(SAADC_CH_CONFIG_RESP_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel resistor control is present. */
#define NRF_SAADC_HAS_CH_CONFIG_RES 1
#else
#define NRF_SAADC_HAS_CH_CONFIG_RES 0
#endif

#if defined(SAADC_CH_PSELP_CONNECT_Internal) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC positive internal inputs for pin number configurations are present. */
#define NRF_SAADC_HAS_CH_PSELP_INTERNAL 1
#else
#define NRF_SAADC_HAS_CH_PSELP_INTERNAL 0
#endif

#if defined(SAADC_CH_PSELN_CONNECT_Internal) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC negative internal inputs for pin number configurations are present. */
#define NRF_SAADC_HAS_CH_PSELN_INTERNAL 1
#else
#define NRF_SAADC_HAS_CH_PSELN_INTERNAL 0
#endif

#if defined(SAADC_CH_CONFIG_BURST_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific burst mode configuration is present. */
#define NRF_SAADC_HAS_CH_BURST 1
#else
#define NRF_SAADC_HAS_CH_BURST 0
#endif

#if defined(SAADC_BURST_BURST_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC peripheral burst mode configuration is present. */
#define NRF_SAADC_HAS_BURST 1
#else
#define NRF_SAADC_HAS_BURST 0
#endif

#if defined(SAADC_CH_CONFIG_CHOPPING_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific chopping mode configuration is present. */
#define NRF_SAADC_HAS_CH_CHOPPING 1
#else
#define NRF_SAADC_HAS_CH_CHOPPING 0
#endif

#if defined(SAADC_CH_PSELP_PSELP_VDDHDIV5) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether VDDH/5 can be selected as SAADC input. */
#define NRF_SAADC_HAS_INPUT_VDDHDIV5 1
#else
#define NRF_SAADC_HAS_INPUT_VDDHDIV5 0
#endif

#if defined(SAADC_CH_PSELP_PSELP_VDD) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether VDD can be selected as SAADC input. */
#define NRF_SAADC_HAS_INPUT_VDD 1
#else
#define NRF_SAADC_HAS_INPUT_VDD 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain configuration is present. */
#define NRF_SAADC_HAS_CH_GAIN 1
#else
#define NRF_SAADC_HAS_CH_GAIN 0
#endif

#if defined(SAADC_CH_CONFIG_HIGHSPEED_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific highspeed mode configuration is present. */
#define NRF_SAADC_HAS_CH_HIGHSPEED 1
#else
#define NRF_SAADC_HAS_CH_HIGHSPEED 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain1_6) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 1/6. */
#define NRF_SAADC_HAS_GAIN_1_6 1
#else
#define NRF_SAADC_HAS_GAIN_1_6 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain1_5) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 1/5. */
#define NRF_SAADC_HAS_GAIN_1_5 1
#else
#define NRF_SAADC_HAS_GAIN_1_5 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain1_4) || defined(SAADC_CH_CONFIG_GAIN_Gain2_8) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 1/4. */
#define NRF_SAADC_HAS_GAIN_1_4 1
#else
#define NRF_SAADC_HAS_GAIN_1_4 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain2_7) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 2/7. */
#define NRF_SAADC_HAS_GAIN_2_7 1
#else
#define NRF_SAADC_HAS_GAIN_2_7 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain1_3) || defined(SAADC_CH_CONFIG_GAIN_Gain2_6) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 1/3. */
#define NRF_SAADC_HAS_GAIN_1_3 1
#else
#define NRF_SAADC_HAS_GAIN_1_3 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain2_5) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 2/5. */
#define NRF_SAADC_HAS_GAIN_2_5 1
#else
#define NRF_SAADC_HAS_GAIN_2_5 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain1_2) || defined(SAADC_CH_CONFIG_GAIN_Gain2_4) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 1/2. */
#define NRF_SAADC_HAS_GAIN_1_2 1
#else
#define NRF_SAADC_HAS_GAIN_1_2 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain2_3) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 2/3. */
#define NRF_SAADC_HAS_GAIN_2_3 1
#else
#define NRF_SAADC_HAS_GAIN_2_3 0
#endif

#if defined(SAADC_CH_CONFIG_GAIN_Gain4) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channel specific gain can be configured to 4. */
#define NRF_SAADC_HAS_GAIN_4 1
#else
#define NRF_SAADC_HAS_GAIN_4 0
#endif

#if defined(SAADC_CH_CONFIG_REFSEL_Internal) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channels can be configured to use an internal reference. */
#define NRF_SAADC_HAS_REFERENCE_INTERNAL 1
#else
#define NRF_SAADC_HAS_REFERENCE_INTERNAL 0
#endif

#if defined(SAADC_CH_CONFIG_REFSEL_VDD1_4) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channels can be configured to use VDD/4 as a reference. */
#define NRF_SAADC_HAS_REFERENCE_VDD4 1
#else
#define NRF_SAADC_HAS_REFERENCE_VDD4 0
#endif

#if defined(SAADC_CH_CONFIG_REFSEL_External) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SAADC channels can be configured to use an external reference. */
#define NRF_SAADC_HAS_REFERENCE_EXTERNAL 1
#else
#define NRF_SAADC_HAS_REFERENCE_EXTERNAL 0
#endif

#if !NRF_SAADC_HAS_ACQTIME_ENUM || defined(__NRFX_DOXYGEN__)
/** @brief Maximum value of acquire time. */
#define NRF_SAADC_ACQTIME_MAX SAADC_CH_CONFIG_TACQ_Max
#endif

#if NRF_SAADC_HAS_CONVTIME
/** @brief Symbol specifying maximum value of conversion time. */
#define NRF_SAADC_CONVTIME_MAX SAADC_CH_CONFIG_TCONV_Max
#endif

#if NRF_SAADC_HAS_LIN_CAL
/** @brief Symbol specifying maximum count of linearity calibration coefficients. */
#define NRF_SAADC_LIN_CAL_MAX_COUNT SAADC_TRIM_LINCALCOEFF_MaxCount

/** @brief Symbol specifying maximum value of linearity calibration coefficient. */
#define NRF_SAADC_LIN_CAL_MAX SAADC_TRIM_LINCALCOEFF_VAL_Max
#endif

#if defined(SAADC_SAMPLERATE_CC_Min) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying minimum capture and compare value for sample rate. */
#define NRF_SAADC_SAMPLERATE_CC_MIN SAADC_SAMPLERATE_CC_Min
#endif

#if defined(SAADC_SAMPLERATE_CC_Max) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying maximum capture and compare value for sample rate. */
#define NRF_SAADC_SAMPLERATE_CC_MAX SAADC_SAMPLERATE_CC_Max
#endif

/** @brief Resolution of the analog-to-digital converter. */
typedef enum
{
    NRF_SAADC_RESOLUTION_8BIT  = SAADC_RESOLUTION_VAL_8bit,  ///< 8 bit resolution.
    NRF_SAADC_RESOLUTION_10BIT = SAADC_RESOLUTION_VAL_10bit, ///< 10 bit resolution.
    NRF_SAADC_RESOLUTION_12BIT = SAADC_RESOLUTION_VAL_12bit, ///< 12 bit resolution.
    NRF_SAADC_RESOLUTION_14BIT = SAADC_RESOLUTION_VAL_14bit  ///< 14 bit resolution.
} nrf_saadc_resolution_t;

#if NRF_SAADC_HAS_AIN_AS_PIN
/** @brief Analog input type. */
typedef uint32_t nrf_saadc_input_t;

#if defined(SAADC_CH_PSELP_INTERNAL_Avdd) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying internal 0.9 V analog supply rail as input. */
#define NRF_SAADC_INPUT_AVDD ((SAADC_CH_PSELP_INTERNAL_Avdd + 1) << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_Dvdd) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying internal 0.9 V digital supply rail as input. */
#define NRF_SAADC_INPUT_DVDD ((SAADC_CH_PSELP_INTERNAL_Dvdd + 1) << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_Vdd) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying VDD as input. */
#define NRF_SAADC_INPUT_VDD ((SAADC_CH_PSELP_INTERNAL_Vdd + 1) << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_VDDAO1V8) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying AVDD_AO_1V8 as input. */
#define NRF_SAADC_INPUT_VDDAO1V8 ((SAADC_CH_PSELP_INTERNAL_VDDAO1V8 + 1) \
                                    << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_VddL) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying AVDD_AO_1V5 as input. */
#define NRF_SAADC_INPUT_VDDL ((SAADC_CH_PSELP_INTERNAL_VddL + 1) << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_Decb) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying DECB as input. */
#define NRF_SAADC_INPUT_DECB ((SAADC_CH_PSELP_INTERNAL_Decb + 1) << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_Vss) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying VSS as input. */
#define NRF_SAADC_INPUT_VSS ((SAADC_CH_PSELP_INTERNAL_Vss + 1) << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_VDDAO0V8) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying VDD_AO_0V8 as input. */
#define NRF_SAADC_INPUT_VDDAO0V8 ((SAADC_CH_PSELP_INTERNAL_VDDAO0V8 + 1) \
                                    << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_VDDRF) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying VDDRF as input. */
#define NRF_SAADC_INPUT_VDDRF ((SAADC_CH_PSELP_INTERNAL_VDDRF + 1) << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_VBAT) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying VBat as input. */
#define NRF_SAADC_INPUT_VBAT ((SAADC_CH_PSELP_INTERNAL_VBAT + 1) << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_Internal0) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying Internal0 as input. */
#define NRF_SAADC_INPUT_INTERNAL0 ((SAADC_CH_PSELP_INTERNAL_Internal0 + 1) \
                                     << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_Internal1) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying Internal1 as input. */
#define NRF_SAADC_INPUT_INTERNAL1 ((SAADC_CH_PSELP_INTERNAL_Internal1 + 1) \
                                     << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_Internal2) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying Internal2 as input. */
#define NRF_SAADC_INPUT_INTERNAL2 ((SAADC_CH_PSELP_INTERNAL_Internal2 + 1) \
                                     << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

#if defined(SAADC_CH_PSELP_INTERNAL_Internal3) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying Internal3 as input. */
#define NRF_SAADC_INPUT_INTERNAL3 ((SAADC_CH_PSELP_INTERNAL_Internal3 + 1) \
                                     << SAADC_CH_PSELP_INTERNAL_Pos)
#endif

/** @brief Symbol specifying disconnected analog input. */
#define NRF_SAADC_INPUT_DISABLED ((nrf_saadc_input_t)UINT32_MAX)
#else
/** @brief Input selection for the analog-to-digital converter. */
typedef enum
{
    NRF_SAADC_INPUT_DISABLED = SAADC_CH_PSELP_PSELP_NC,            ///< Not connected.
    NRF_SAADC_INPUT_AIN0     = SAADC_CH_PSELP_PSELP_AnalogInput0,  ///< Analog input 0 (AIN0).
    NRF_SAADC_INPUT_AIN1     = SAADC_CH_PSELP_PSELP_AnalogInput1,  ///< Analog input 1 (AIN1).
    NRF_SAADC_INPUT_AIN2     = SAADC_CH_PSELP_PSELP_AnalogInput2,  ///< Analog input 2 (AIN2).
    NRF_SAADC_INPUT_AIN3     = SAADC_CH_PSELP_PSELP_AnalogInput3,  ///< Analog input 3 (AIN3).
    NRF_SAADC_INPUT_AIN4     = SAADC_CH_PSELP_PSELP_AnalogInput4,  ///< Analog input 4 (AIN4).
    NRF_SAADC_INPUT_AIN5     = SAADC_CH_PSELP_PSELP_AnalogInput5,  ///< Analog input 5 (AIN5).
    NRF_SAADC_INPUT_AIN6     = SAADC_CH_PSELP_PSELP_AnalogInput6,  ///< Analog input 6 (AIN6).
    NRF_SAADC_INPUT_AIN7     = SAADC_CH_PSELP_PSELP_AnalogInput7,  ///< Analog input 7 (AIN7).
#if NRF_SAADC_HAS_INPUT_VDD
    NRF_SAADC_INPUT_VDD      = SAADC_CH_PSELP_PSELP_VDD,           ///< VDD as input.
#endif
#if NRF_SAADC_HAS_INPUT_VDDHDIV5
    NRF_SAADC_INPUT_VDDHDIV5 = SAADC_CH_PSELP_PSELP_VDDHDIV5       ///< VDDH/5 as input.
#endif
#if defined(SAADC_CH_PSELP_PSELP_AnalogInput8) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_INPUT_AIN8     = SAADC_CH_PSELP_PSELP_AnalogInput8,  ///< Analog input 8 (AIN8).
#endif
#if defined(SAADC_CH_PSELP_PSELP_AnalogInput9) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_INPUT_AIN9     = SAADC_CH_PSELP_PSELP_AnalogInput9,  ///< Analog input 9 (AIN9).
#endif
#if defined(SAADC_CH_PSELP_PSELP_AnalogInput10) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_INPUT_AIN10    = SAADC_CH_PSELP_PSELP_AnalogInput10, ///< Analog input 10 (AIN10).
#endif
#if defined(SAADC_CH_PSELP_PSELP_AnalogInput11) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_INPUT_AIN11    = SAADC_CH_PSELP_PSELP_AnalogInput11, ///< Analog input 11 (AIN11).
#endif
#if defined(SAADC_CH_PSELP_PSELP_AnalogInput12) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_INPUT_AIN12    = SAADC_CH_PSELP_PSELP_AnalogInput12, ///< Analog input 12 (AIN12).
#endif
#if defined(SAADC_CH_PSELP_PSELP_AnalogInput13) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_INPUT_AIN13    = SAADC_CH_PSELP_PSELP_AnalogInput13, ///< Analog input 13 (AIN13).
#endif
} nrf_saadc_input_t;
#endif

/** @brief Analog-to-digital converter oversampling mode. */
typedef enum
{
    NRF_SAADC_OVERSAMPLE_DISABLED = SAADC_OVERSAMPLE_OVERSAMPLE_Bypass,   ///< No oversampling.
    NRF_SAADC_OVERSAMPLE_2X       = SAADC_OVERSAMPLE_OVERSAMPLE_Over2x,   ///< Oversample 2x.
    NRF_SAADC_OVERSAMPLE_4X       = SAADC_OVERSAMPLE_OVERSAMPLE_Over4x,   ///< Oversample 4x.
    NRF_SAADC_OVERSAMPLE_8X       = SAADC_OVERSAMPLE_OVERSAMPLE_Over8x,   ///< Oversample 8x.
    NRF_SAADC_OVERSAMPLE_16X      = SAADC_OVERSAMPLE_OVERSAMPLE_Over16x,  ///< Oversample 16x.
    NRF_SAADC_OVERSAMPLE_32X      = SAADC_OVERSAMPLE_OVERSAMPLE_Over32x,  ///< Oversample 32x.
    NRF_SAADC_OVERSAMPLE_64X      = SAADC_OVERSAMPLE_OVERSAMPLE_Over64x,  ///< Oversample 64x.
    NRF_SAADC_OVERSAMPLE_128X     = SAADC_OVERSAMPLE_OVERSAMPLE_Over128x, ///< Oversample 128x.
    NRF_SAADC_OVERSAMPLE_256X     = SAADC_OVERSAMPLE_OVERSAMPLE_Over256x  ///< Oversample 256x.
} nrf_saadc_oversample_t;

#if NRF_SAADC_HAS_CH_CONFIG_RES
/** @brief Analog-to-digital converter channel resistor control. */
typedef enum
{
    NRF_SAADC_RESISTOR_DISABLED = SAADC_CH_CONFIG_RESP_Bypass,       ///< Bypass resistor ladder.
    NRF_SAADC_RESISTOR_PULLDOWN = SAADC_CH_CONFIG_RESP_Pulldown,     ///< Pull-down to GND.
    NRF_SAADC_RESISTOR_PULLUP   = SAADC_CH_CONFIG_RESP_Pullup,       ///< Pull-up to VDD.
#if defined(SAADC_CH_CONFIG_RESP_VDD1_2) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_RESISTOR_VDD1_2   = SAADC_CH_CONFIG_RESP_VDD1_2,       ///< Set input at VDD/2.
#endif
#if defined(SAADC_CH_CONFIG_RESP_VDDAO1V8div2)
    NRF_SAADC_RESISTOR_VDD1_2   = SAADC_CH_CONFIG_RESP_VDDAO1V8div2, ///< Set input at VDD/2.
#endif
} nrf_saadc_resistor_t;
#endif

#if NRF_SAADC_HAS_CH_GAIN
/** @brief Gain factor of the analog-to-digital converter input. */
typedef enum
{
#if NRF_SAADC_HAS_GAIN_1_6
    NRF_SAADC_GAIN1_6 = SAADC_CH_CONFIG_GAIN_Gain1_6, ///< Gain factor 1/6.
#endif
#if NRF_SAADC_HAS_GAIN_1_5
    NRF_SAADC_GAIN1_5 = SAADC_CH_CONFIG_GAIN_Gain1_5, ///< Gain factor 1/5.
#endif
#if defined(SAADC_CH_CONFIG_GAIN_Gain1_4) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_GAIN1_4 = SAADC_CH_CONFIG_GAIN_Gain1_4, ///< Gain factor 1/4.
#elif defined(SAADC_CH_CONFIG_GAIN_Gain2_8) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_GAIN1_4 = SAADC_CH_CONFIG_GAIN_Gain2_8, ///< Gain factor 1/4.
#endif
#if NRF_SAADC_HAS_GAIN_2_7
    NRF_SAADC_GAIN2_7 = SAADC_CH_CONFIG_GAIN_Gain2_7, ///< Gain factor 2/7.
#endif
#if defined(SAADC_CH_CONFIG_GAIN_Gain1_3) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_GAIN1_3 = SAADC_CH_CONFIG_GAIN_Gain1_3, ///< Gain factor 1/3.
#elif defined(SAADC_CH_CONFIG_GAIN_Gain2_6) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_GAIN1_3 = SAADC_CH_CONFIG_GAIN_Gain2_6, ///< Gain factor 1/3.
#endif
#if NRF_SAADC_HAS_GAIN_2_5
    NRF_SAADC_GAIN2_5 = SAADC_CH_CONFIG_GAIN_Gain2_5, ///< Gain factor 2/5.
#endif
#if defined(SAADC_CH_CONFIG_GAIN_Gain1_2) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_GAIN1_2 = SAADC_CH_CONFIG_GAIN_Gain1_2, ///< Gain factor 1/2.
#elif defined(SAADC_CH_CONFIG_GAIN_Gain2_4) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_GAIN1_2 = SAADC_CH_CONFIG_GAIN_Gain2_4, ///< Gain factor 1/2.
#endif
#if NRF_SAADC_HAS_GAIN_2_3
    NRF_SAADC_GAIN2_3 = SAADC_CH_CONFIG_GAIN_Gain2_3, ///< Gain factor 2/3.
#endif
    NRF_SAADC_GAIN1   = SAADC_CH_CONFIG_GAIN_Gain1,   ///< Gain factor 1.
    NRF_SAADC_GAIN2   = SAADC_CH_CONFIG_GAIN_Gain2,   ///< Gain factor 2.
#if NRF_SAADC_HAS_GAIN_4
    NRF_SAADC_GAIN4   = SAADC_CH_CONFIG_GAIN_Gain4,   ///< Gain factor 4.
#endif
} nrf_saadc_gain_t;
#endif

/** @brief Reference selection for the analog-to-digital converter. */
typedef enum
{
#if NRF_SAADC_HAS_REFERENCE_INTERNAL
    NRF_SAADC_REFERENCE_INTERNAL = SAADC_CH_CONFIG_REFSEL_Internal, ///< Internal reference.
#endif
#if NRF_SAADC_HAS_REFERENCE_VDD4
    NRF_SAADC_REFERENCE_VDD4     = SAADC_CH_CONFIG_REFSEL_VDD1_4    ///< VDD/4 as reference.
#endif
#if NRF_SAADC_HAS_REFERENCE_EXTERNAL
    NRF_SAADC_REFERENCE_EXTERNAL = SAADC_CH_CONFIG_REFSEL_External, ///< External reference.
#endif
} nrf_saadc_reference_t;

#if NRF_SAADC_HAS_ACQTIME_ENUM
/** @brief Analog-to-digital converter acquisition time. */
typedef enum
{
    NRF_SAADC_ACQTIME_3US  = SAADC_CH_CONFIG_TACQ_3us,  ///< 3 us.
    NRF_SAADC_ACQTIME_5US  = SAADC_CH_CONFIG_TACQ_5us,  ///< 5 us.
    NRF_SAADC_ACQTIME_10US = SAADC_CH_CONFIG_TACQ_10us, ///< 10 us.
    NRF_SAADC_ACQTIME_15US = SAADC_CH_CONFIG_TACQ_15us, ///< 15 us.
    NRF_SAADC_ACQTIME_20US = SAADC_CH_CONFIG_TACQ_20us, ///< 20 us.
    NRF_SAADC_ACQTIME_40US = SAADC_CH_CONFIG_TACQ_40us  ///< 40 us.
} nrf_saadc_acqtime_t;
#else
typedef uint16_t nrf_saadc_acqtime_t;
#endif

/** @brief Analog-to-digital converter channel mode. */
typedef enum
{
    NRF_SAADC_MODE_SINGLE_ENDED = SAADC_CH_CONFIG_MODE_SE,  ///< Single-ended mode. PSELN will be ignored, negative input to ADC shorted to GND.
    NRF_SAADC_MODE_DIFFERENTIAL = SAADC_CH_CONFIG_MODE_Diff ///< Differential mode.
} nrf_saadc_mode_t;

/** @brief Analog-to-digital converter burst mode. */
typedef enum
{
#if NRF_SAADC_HAS_CH_BURST
    NRF_SAADC_BURST_DISABLED = SAADC_CH_CONFIG_BURST_Disabled, ///< Burst mode is disabled (normal operation).
    NRF_SAADC_BURST_ENABLED  = SAADC_CH_CONFIG_BURST_Enabled   ///< Burst mode is enabled. SAADC takes 2^OVERSAMPLE number of samples as fast as it can, and sends the average to Data RAM.
#elif NRF_SAADC_HAS_BURST
    NRF_SAADC_BURST_DISABLED = SAADC_BURST_BURST_Disabled,     ///< Burst mode is disabled (normal operation).
    NRF_SAADC_BURST_ENABLED  = SAADC_BURST_BURST_Enabled       ///< Burst mode is enabled. SAADC takes 2^OVERSAMPLE number of samples as fast as it can, and sends the average to Data RAM.
#endif
} nrf_saadc_burst_t;

#if NRF_SAADC_HAS_CH_CHOPPING
/** @brief Analog-to-digital converter chopping mode. */
typedef enum
{
    NRF_SAADC_CHOPPING_DISABLED = SAADC_CH_CONFIG_CHOPPING_Disabled, ///< Chopping mode is disabled.
    NRF_SAADC_CHOPPING_ENABLED  = SAADC_CH_CONFIG_CHOPPING_Enabled   ///< Chopping mode is enabled, inputs will be switched on every second sample.
} nrf_saadc_chopping_t;
#endif

#if NRF_SAADC_HAS_CH_HIGHSPEED
/** @brief Analog-to-digital converter highspeed mode. */
typedef enum
{
    NRF_SAADC_HIGHSPEED_DISABLED = SAADC_CH_CONFIG_HIGHSPEED_Disabled, ///< Highspeed mode is disabled.
    NRF_SAADC_HIGHSPEED_ENABLED  = SAADC_CH_CONFIG_HIGHSPEED_Enabled   ///< Highspeed mode is enabled.
} nrf_saadc_highspeed_t;
#endif

/** @brief Analog-to-digital converter tasks. */
typedef enum
{
    NRF_SAADC_TASK_START           = offsetof(NRF_SAADC_Type, TASKS_START),           ///< Start the ADC and prepare the result buffer in RAM.
    NRF_SAADC_TASK_STOP            = offsetof(NRF_SAADC_Type, TASKS_STOP),            ///< Stop the ADC and terminate any ongoing conversion.
    NRF_SAADC_TASK_SAMPLE          = offsetof(NRF_SAADC_Type, TASKS_SAMPLE),          ///< Take one ADC sample. If scan is enabled, all channels are sampled.
    NRF_SAADC_TASK_CALIBRATEOFFSET = offsetof(NRF_SAADC_Type, TASKS_CALIBRATEOFFSET), ///< Starts offset auto-calibration.
} nrf_saadc_task_t;

/** @brief Analog-to-digital converter events. */
typedef enum
{
    NRF_SAADC_EVENT_STARTED       = offsetof(NRF_SAADC_Type, EVENTS_STARTED),       ///< The ADC has started.
    NRF_SAADC_EVENT_END           = offsetof(NRF_SAADC_Type, EVENTS_END),           ///< The ADC has filled up the result buffer.
    NRF_SAADC_EVENT_DONE          = offsetof(NRF_SAADC_Type, EVENTS_DONE),          ///< A conversion task has been completed.
    NRF_SAADC_EVENT_RESULTDONE    = offsetof(NRF_SAADC_Type, EVENTS_RESULTDONE),    ///< A result is ready to get transferred to RAM.
    NRF_SAADC_EVENT_CALIBRATEDONE = offsetof(NRF_SAADC_Type, EVENTS_CALIBRATEDONE), ///< Calibration is complete.
    NRF_SAADC_EVENT_STOPPED       = offsetof(NRF_SAADC_Type, EVENTS_STOPPED),       ///< The ADC has stopped.
    NRF_SAADC_EVENT_CH0_LIMITH    = offsetof(NRF_SAADC_Type, EVENTS_CH[0].LIMITH),  ///< Last result is equal or above CH[0].LIMIT.HIGH.
    NRF_SAADC_EVENT_CH0_LIMITL    = offsetof(NRF_SAADC_Type, EVENTS_CH[0].LIMITL),  ///< Last result is equal or below CH[0].LIMIT.LOW.
    NRF_SAADC_EVENT_CH1_LIMITH    = offsetof(NRF_SAADC_Type, EVENTS_CH[1].LIMITH),  ///< Last result is equal or above CH[1].LIMIT.HIGH.
    NRF_SAADC_EVENT_CH1_LIMITL    = offsetof(NRF_SAADC_Type, EVENTS_CH[1].LIMITL),  ///< Last result is equal or below CH[1].LIMIT.LOW.
    NRF_SAADC_EVENT_CH2_LIMITH    = offsetof(NRF_SAADC_Type, EVENTS_CH[2].LIMITH),  ///< Last result is equal or above CH[2].LIMIT.HIGH.
    NRF_SAADC_EVENT_CH2_LIMITL    = offsetof(NRF_SAADC_Type, EVENTS_CH[2].LIMITL),  ///< Last result is equal or below CH[2].LIMIT.LOW.
    NRF_SAADC_EVENT_CH3_LIMITH    = offsetof(NRF_SAADC_Type, EVENTS_CH[3].LIMITH),  ///< Last result is equal or above CH[3].LIMIT.HIGH.
    NRF_SAADC_EVENT_CH3_LIMITL    = offsetof(NRF_SAADC_Type, EVENTS_CH[3].LIMITL),  ///< Last result is equal or below CH[3].LIMIT.LOW.
#if (SAADC_CH_NUM > 4) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_EVENT_CH4_LIMITH    = offsetof(NRF_SAADC_Type, EVENTS_CH[4].LIMITH),  ///< Last result is equal or above CH[4].LIMIT.HIGH.
    NRF_SAADC_EVENT_CH4_LIMITL    = offsetof(NRF_SAADC_Type, EVENTS_CH[4].LIMITL),  ///< Last result is equal or below CH[4].LIMIT.LOW.
    NRF_SAADC_EVENT_CH5_LIMITH    = offsetof(NRF_SAADC_Type, EVENTS_CH[5].LIMITH),  ///< Last result is equal or above CH[5].LIMIT.HIGH.
    NRF_SAADC_EVENT_CH5_LIMITL    = offsetof(NRF_SAADC_Type, EVENTS_CH[5].LIMITL),  ///< Last result is equal or below CH[5].LIMIT.LOW.
    NRF_SAADC_EVENT_CH6_LIMITH    = offsetof(NRF_SAADC_Type, EVENTS_CH[6].LIMITH),  ///< Last result is equal or above CH[6].LIMIT.HIGH.
    NRF_SAADC_EVENT_CH6_LIMITL    = offsetof(NRF_SAADC_Type, EVENTS_CH[6].LIMITL),  ///< Last result is equal or below CH[6].LIMIT.LOW.
    NRF_SAADC_EVENT_CH7_LIMITH    = offsetof(NRF_SAADC_Type, EVENTS_CH[7].LIMITH),  ///< Last result is equal or above CH[7].LIMIT.HIGH.
    NRF_SAADC_EVENT_CH7_LIMITL    = offsetof(NRF_SAADC_Type, EVENTS_CH[7].LIMITL)   ///< Last result is equal or below CH[7].LIMIT.LOW.
#endif
} nrf_saadc_event_t;

/** @brief Analog-to-digital converter interrupt masks. */
typedef enum
{
    NRF_SAADC_INT_STARTED       = SAADC_INTENSET_STARTED_Msk,       ///< Interrupt on EVENTS_STARTED event.
    NRF_SAADC_INT_END           = SAADC_INTENSET_END_Msk,           ///< Interrupt on EVENTS_END event.
    NRF_SAADC_INT_DONE          = SAADC_INTENSET_DONE_Msk,          ///< Interrupt on EVENTS_DONE event.
    NRF_SAADC_INT_RESULTDONE    = SAADC_INTENSET_RESULTDONE_Msk,    ///< Interrupt on EVENTS_RESULTDONE event.
    NRF_SAADC_INT_CALIBRATEDONE = SAADC_INTENSET_CALIBRATEDONE_Msk, ///< Interrupt on EVENTS_CALIBRATEDONE event.
    NRF_SAADC_INT_STOPPED       = SAADC_INTENSET_STOPPED_Msk,       ///< Interrupt on EVENTS_STOPPED event.
    NRF_SAADC_INT_CH0LIMITH     = SAADC_INTENSET_CH0LIMITH_Msk,     ///< Interrupt on EVENTS_CH[0].LIMITH event.
    NRF_SAADC_INT_CH0LIMITL     = SAADC_INTENSET_CH0LIMITL_Msk,     ///< Interrupt on EVENTS_CH[0].LIMITL event.
    NRF_SAADC_INT_CH1LIMITH     = SAADC_INTENSET_CH1LIMITH_Msk,     ///< Interrupt on EVENTS_CH[1].LIMITH event.
    NRF_SAADC_INT_CH1LIMITL     = SAADC_INTENSET_CH1LIMITL_Msk,     ///< Interrupt on EVENTS_CH[1].LIMITL event.
    NRF_SAADC_INT_CH2LIMITH     = SAADC_INTENSET_CH2LIMITH_Msk,     ///< Interrupt on EVENTS_CH[2].LIMITH event.
    NRF_SAADC_INT_CH2LIMITL     = SAADC_INTENSET_CH2LIMITL_Msk,     ///< Interrupt on EVENTS_CH[2].LIMITL event.
    NRF_SAADC_INT_CH3LIMITH     = SAADC_INTENSET_CH3LIMITH_Msk,     ///< Interrupt on EVENTS_CH[3].LIMITH event.
    NRF_SAADC_INT_CH3LIMITL     = SAADC_INTENSET_CH3LIMITL_Msk,     ///< Interrupt on EVENTS_CH[3].LIMITL event.
#if (SAADC_CH_NUM > 4) || defined(__NRFX_DOXYGEN__)
    NRF_SAADC_INT_CH4LIMITH     = SAADC_INTENSET_CH4LIMITH_Msk,     ///< Interrupt on EVENTS_CH[4].LIMITH event.
    NRF_SAADC_INT_CH4LIMITL     = SAADC_INTENSET_CH4LIMITL_Msk,     ///< Interrupt on EVENTS_CH[4].LIMITL event.
    NRF_SAADC_INT_CH5LIMITH     = SAADC_INTENSET_CH5LIMITH_Msk,     ///< Interrupt on EVENTS_CH[5].LIMITH event.
    NRF_SAADC_INT_CH5LIMITL     = SAADC_INTENSET_CH5LIMITL_Msk,     ///< Interrupt on EVENTS_CH[5].LIMITL event.
    NRF_SAADC_INT_CH6LIMITH     = SAADC_INTENSET_CH6LIMITH_Msk,     ///< Interrupt on EVENTS_CH[6].LIMITH event.
    NRF_SAADC_INT_CH6LIMITL     = SAADC_INTENSET_CH6LIMITL_Msk,     ///< Interrupt on EVENTS_CH[6].LIMITL event.
    NRF_SAADC_INT_CH7LIMITH     = SAADC_INTENSET_CH7LIMITH_Msk,     ///< Interrupt on EVENTS_CH[7].LIMITH event.
    NRF_SAADC_INT_CH7LIMITL     = SAADC_INTENSET_CH7LIMITL_Msk,     ///< Interrupt on EVENTS_CH[7].LIMITL event.
#endif
    NRF_SAADC_INT_ALL           = 0x7FFFFFFFUL                      ///< Mask of all interrupts.
} nrf_saadc_int_mask_t;

/** @brief Analog-to-digital converter value limit type. */
typedef enum
{
    NRF_SAADC_LIMIT_LOW  = 0, ///< Low limit type.
    NRF_SAADC_LIMIT_HIGH = 1  ///< High limit type.
} nrf_saadc_limit_t;

/** @brief Type of a single ADC conversion result. */
typedef int16_t nrf_saadc_value_t;

/** @brief Analog-to-digital converter configuration structure. */
typedef struct
{
    nrf_saadc_resolution_t resolution;  ///< Resolution of samples.
    nrf_saadc_oversample_t oversample;  ///< Oversampling configuration.
    nrf_saadc_value_t *    buffer;      ///< Pointer to sample buffer.
    uint32_t               buffer_size; ///< Size of the sample buffer.
} nrf_saadc_config_t;

/** @brief Analog-to-digital converter channel configuration structure. */
typedef struct
{
#if NRF_SAADC_HAS_CH_CONFIG_RES
    nrf_saadc_resistor_t  resistor_p; ///< Resistor value on positive input.
    nrf_saadc_resistor_t  resistor_n; ///< Resistor value on negative input.
#endif
#if NRF_SAADC_HAS_CH_GAIN
    nrf_saadc_gain_t      gain;       ///< Gain control value.
#endif
    nrf_saadc_reference_t reference;  ///< Reference control value.
    nrf_saadc_acqtime_t   acq_time;   ///< Acquisition time.
    nrf_saadc_mode_t      mode;       ///< SAADC mode. Single-ended or differential.
#if NRF_SAADC_HAS_CH_BURST
    nrf_saadc_burst_t     burst;      ///< Burst mode configuration.
#endif
#if NRF_SAADC_HAS_CH_CHOPPING
    nrf_saadc_chopping_t  chopping;   ///< Chopping mode configuration.
#endif
#if NRF_SAADC_HAS_CH_HIGHSPEED
    nrf_saadc_highspeed_t highspeed;  ///< Highspeed mode configuration.
#endif
#if NRF_SAADC_HAS_CONVTIME
    uint8_t               conv_time;  ///< Conversion time.
#endif
} nrf_saadc_channel_config_t;


/**
 * @brief Function for triggering the specified SAADC task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  SAADC task.
 */
NRF_STATIC_INLINE void nrf_saadc_task_trigger(NRF_SAADC_Type * p_reg,
                                              nrf_saadc_task_t task);

/**
 * @brief Function for getting the address of the specified SAADC task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  SAADC task.
 *
 * @return Address of the specified SAADC task.
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_task_address_get(NRF_SAADC_Type const * p_reg,
                                                      nrf_saadc_task_t       task);

/**
 * @brief Function for retrieving the state of the SAADC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_saadc_event_check(NRF_SAADC_Type const * p_reg,
                                             nrf_saadc_event_t      event);

/**
 * @brief Function for clearing the specific SAADC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event SAADC event.
 */
NRF_STATIC_INLINE void nrf_saadc_event_clear(NRF_SAADC_Type *  p_reg,
                                             nrf_saadc_event_t event);

/**
 * @brief Function for getting the address of the specified SAADC event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event SAADC event.
 *
 * @return Address of the specified SAADC event.
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_event_address_get(NRF_SAADC_Type const * p_reg,
                                                       nrf_saadc_event_t      event);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        SAADC task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_saadc_subscribe_set(NRF_SAADC_Type * p_reg,
                                               nrf_saadc_task_t task,
                                               uint8_t          channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        SAADC task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_saadc_subscribe_clear(NRF_SAADC_Type * p_reg,
                                                 nrf_saadc_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        SAADC task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return SAADC subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_subscribe_get(NRF_SAADC_Type const * p_reg,
                                                   nrf_saadc_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        SAADC event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_saadc_publish_set(NRF_SAADC_Type *  p_reg,
                                             nrf_saadc_event_t event,
                                             uint8_t           channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        SAADC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_saadc_publish_clear(NRF_SAADC_Type *  p_reg,
                                               nrf_saadc_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        SAADC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return SAADC publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_publish_get(NRF_SAADC_Type const * p_reg,
                                                 nrf_saadc_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for getting the SAADC channel monitoring limit events.
 *
 * @param[in] channel    Channel number.
 * @param[in] limit_type Low limit or high limit.
 *
 * @return The SAADC channel monitoring limit event.
 */
NRF_STATIC_INLINE nrf_saadc_event_t nrf_saadc_limit_event_get(uint8_t           channel,
                                                              nrf_saadc_limit_t limit_type);

/**
 * @brief Function for configuring the input pins for the specified SAADC channel.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel Channel number.
 * @param[in] pselp   Positive input.
 * @param[in] pseln   Negative input. Set to @ref NRF_SAADC_INPUT_DISABLED in single ended mode.
 */
NRF_STATIC_INLINE void nrf_saadc_channel_input_set(NRF_SAADC_Type *  p_reg,
                                                   uint8_t           channel,
                                                   nrf_saadc_input_t pselp,
                                                   nrf_saadc_input_t pseln);

/**
 * @brief Function for configuring the positive input pin for the specified SAADC channel.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel Channel number.
 * @param[in] pselp   Positive input.
 */
NRF_STATIC_INLINE void nrf_saadc_channel_pos_input_set(NRF_SAADC_Type *  p_reg,
                                                       uint8_t           channel,
                                                       nrf_saadc_input_t pselp);

/**
 * @brief Function for configuring the negative input pin for the specified SAADC channel.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel Channel number.
 * @param[in] pseln   Negative input.
 */
NRF_STATIC_INLINE void nrf_saadc_channel_neg_input_set(NRF_SAADC_Type *  p_reg,
                                                       uint8_t           channel,
                                                       nrf_saadc_input_t pseln);

/**
 * @brief Function for setting the SAADC channel monitoring limits.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel Channel number.
 * @param[in] low     Low limit.
 * @param[in] high    High limit.
 */
NRF_STATIC_INLINE void nrf_saadc_channel_limits_set(NRF_SAADC_Type * p_reg,
                                                    uint8_t          channel,
                                                    int16_t          low,
                                                    int16_t          high);

/**
 * @brief Function for setting the configuration of SAADC interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Interrupts configuration to be set.
 *                  Use @ref nrf_saadc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_saadc_int_set(NRF_SAADC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for enabling specified SAADC interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_saadc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_saadc_int_enable(NRF_SAADC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_saadc_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_int_enable_check(NRF_SAADC_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_saadc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_saadc_int_disable(NRF_SAADC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for generating masks for SAADC channel limit interrupts.
 *
 * @param[in] channel    SAADC channel number.
 * @param[in] limit_type Limit type.
 *
 * @return Interrupt mask.
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_limit_int_get(uint8_t           channel,
                                                   nrf_saadc_limit_t limit_type);

/**
 * @brief Function for checking whether the SAADC is busy.
 *
 * This function checks whether the analog-to-digital converter is busy with a conversion.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The SAADC is busy.
 * @retval false The SAADC is not busy.
 */
NRF_STATIC_INLINE bool nrf_saadc_busy_check(NRF_SAADC_Type const * p_reg);

/**
 * @brief Function for enabling the SAADC.
 *
 * The analog-to-digital converter must be enabled before use.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_saadc_enable(NRF_SAADC_Type * p_reg);

/**
 * @brief Function for disabling the SAADC.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_saadc_disable(NRF_SAADC_Type * p_reg);

/**
 * @brief Function for checking if the SAADC is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The SAADC is enabled.
 * @retval false The SAADC is not enabled.
 */
NRF_STATIC_INLINE bool nrf_saadc_enable_check(NRF_SAADC_Type const * p_reg);

/**
 * @brief Function for initializing the SAADC result buffer.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the result buffer.
 * @param[in] size     Size of the buffer (in samples).
 */
NRF_STATIC_INLINE void nrf_saadc_buffer_init(NRF_SAADC_Type *    p_reg,
                                             nrf_saadc_value_t * p_buffer,
                                             uint32_t            size);

/**
 * @brief Function for setting the SAADC result buffer pointer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the result buffer.
 */
NRF_STATIC_INLINE void nrf_saadc_buffer_pointer_set(NRF_SAADC_Type *    p_reg,
                                                    nrf_saadc_value_t * p_buffer);

/**
 * @brief Function for getting the SAADC result buffer pointer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the result buffer.
 */
NRF_STATIC_INLINE nrf_saadc_value_t * nrf_saadc_buffer_pointer_get(NRF_SAADC_Type const * p_reg);

/**
 * @brief Function for getting the number of samples written to the result
 *        buffer since the previous START task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of samples written to the buffer.
 */
NRF_STATIC_INLINE uint16_t nrf_saadc_amount_get(NRF_SAADC_Type const * p_reg);

/**
 * @brief Function for setting the SAADC sample resolution.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] resolution Bit resolution.
 */
NRF_STATIC_INLINE void nrf_saadc_resolution_set(NRF_SAADC_Type *       p_reg,
                                                nrf_saadc_resolution_t resolution);

/**
 * @brief Function for getting the SAADC sample resolution.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Sample resolution.
 */
NRF_STATIC_INLINE nrf_saadc_resolution_t nrf_saadc_resolution_get(NRF_SAADC_Type const * p_reg);

/**
 * @brief Function for configuring the oversampling feature.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] oversample Oversampling mode.
 */
NRF_STATIC_INLINE void nrf_saadc_oversample_set(NRF_SAADC_Type *       p_reg,
                                                nrf_saadc_oversample_t oversample);

/**
 * @brief Function for getting the oversampling feature configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Oversampling configuration.
 */
NRF_STATIC_INLINE nrf_saadc_oversample_t nrf_saadc_oversample_get(NRF_SAADC_Type const * p_reg);

/**
 * @brief Function for getting the sample count needed for one averaged result for a given
 *        oversampling configuration.
 *
 * @param[in] oversample Oversampling configuration.
 *
 * @return Sample count.
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_oversample_sample_count_get(nrf_saadc_oversample_t oversample);

/**
 * @brief Function for enabling the continuous sampling.
 *
 * This function configures the SAADC internal timer to automatically take new samples at a fixed
 * sample rate. Trigger the START task to begin continuous sampling. To stop the sampling, trigger
 * the STOP task.
 *
 * @note The internal timer can only be used when a single input channel is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] cc    Capture and compare value. Sample rate is 16 MHz/cc.
 *                  Valid @c CC range is from NRF_SAADC_SAMPLERATE_CC_MIN
 *                  to NRF_SAADC_SAMPLERATE_CC_MAX.
 */
NRF_STATIC_INLINE void nrf_saadc_continuous_mode_enable(NRF_SAADC_Type * p_reg,
                                                        uint16_t         cc);

/**
 * @brief Function for checking if the continuous sampling is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The continuous sampling is enabled.
 * @retval false The continuous sampling is disabled.
 */
NRF_STATIC_INLINE bool nrf_saadc_continuous_mode_enable_check(NRF_SAADC_Type const * p_reg);

/**
 * @brief Function for disabling the continuous sampling.
 *
 * New samples can still be acquired by manually triggering the SAMPLE task or by PPI.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_saadc_continuous_mode_disable(NRF_SAADC_Type * p_reg);

#if NRF_SAADC_HAS_LIN_CAL
/**
 * @brief Function for setting linearity calibration coefficient.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Coefficient number.
 * @param[in] coeff Value of the coefficient.
 */
NRF_STATIC_INLINE void nrf_saadc_linearity_calibration_coeff_set(NRF_SAADC_Type * p_reg,
                                                                 uint8_t          index,
                                                                 uint32_t         coeff);

/**
 * @brief Function for getting linearity calibration coefficient.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Coefficient number.
 *
 * @return Value of the coefficient.
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_linearity_calibration_coeff_get(NRF_SAADC_Type const * p_reg,
                                                                     uint8_t                index);
#endif

/**
 * @brief Function for initializing the SAADC channel.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel Channel number.
 * @param[in] config  Pointer to the channel configuration structure.
 */
NRF_STATIC_INLINE void nrf_saadc_channel_init(NRF_SAADC_Type *                   p_reg,
                                              uint8_t                            channel,
                                              nrf_saadc_channel_config_t const * config);
#if NRF_SAADC_HAS_CH_BURST
/**
 * @brief Function for configuring the burst mode for the specified channel.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel Channel number.
 * @param[in] burst   Burst mode setting.
 */
NRF_STATIC_INLINE void nrf_saadc_channel_burst_set(NRF_SAADC_Type *  p_reg,
                                                   uint8_t           channel,
                                                   nrf_saadc_burst_t burst);
#endif

#if NRF_SAADC_HAS_BURST
/**
 * @brief Function for configuring peripheral burst mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] burst Burst mode setting.
 */
NRF_STATIC_INLINE void nrf_saadc_burst_set(NRF_SAADC_Type *  p_reg,
                                           nrf_saadc_burst_t burst);
#endif

/**
 * @brief Function for getting the minimum value of the conversion result.
 *
 * The minimum value of the conversion result depends on the configured resolution.
 *
 * @param[in] resolution Bit resolution.
 *
 * @return Minimum value of the conversion result.
 */
NRF_STATIC_INLINE nrf_saadc_value_t nrf_saadc_value_min_get(nrf_saadc_resolution_t resolution);

/**
 * @brief Function for getting the maximum value of the conversion result.
 *
 * The maximum value of the conversion result depends on the configured resolution.
 *
 * @param[in] resolution Bit resolution.
 *
 * @return Maximum value of the conversion result.
 */
NRF_STATIC_INLINE nrf_saadc_value_t nrf_saadc_value_max_get(nrf_saadc_resolution_t resolution);

#if NRF_SAADC_HAS_CAL
/**
 * @brief Function for setting SAADC CAL trimming value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] trim  Trimming value
 */
NRF_STATIC_INLINE void nrf_saadc_cal_set(NRF_SAADC_Type * p_reg, uint32_t trim);

/**
 * @brief Function for getting SAADC CAL trimming value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Trimming value
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_cal_get(NRF_SAADC_Type const * p_reg);
#endif

#if NRF_SAADC_HAS_CALREF
/**
 * @brief Function for setting SAADC CALREF trimming value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] trim  Trimming value
 */
NRF_STATIC_INLINE void nrf_saadc_calref_set(NRF_SAADC_Type * p_reg, uint32_t trim);

/**
 * @brief Function for getting SAADC CALREF trimming value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Trimming value
 */
NRF_STATIC_INLINE uint32_t nrf_saadc_calref_get(NRF_SAADC_Type const * p_reg);
#endif

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_saadc_task_trigger(NRF_SAADC_Type * p_reg, nrf_saadc_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_task_address_get(NRF_SAADC_Type const * p_reg,
                                                      nrf_saadc_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE bool nrf_saadc_event_check(NRF_SAADC_Type const * p_reg, nrf_saadc_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_saadc_event_clear(NRF_SAADC_Type * p_reg, nrf_saadc_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE uint32_t  nrf_saadc_event_address_get(NRF_SAADC_Type const * p_reg,
                                                        nrf_saadc_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_saadc_subscribe_set(NRF_SAADC_Type * p_reg,
                                               nrf_saadc_task_t task,
                                               uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_saadc_subscribe_clear(NRF_SAADC_Type * p_reg, nrf_saadc_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_subscribe_get(NRF_SAADC_Type const * p_reg,
                                                   nrf_saadc_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_saadc_publish_set(NRF_SAADC_Type *  p_reg,
                                             nrf_saadc_event_t event,
                                             uint8_t           channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_saadc_publish_clear(NRF_SAADC_Type * p_reg, nrf_saadc_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_publish_get(NRF_SAADC_Type const * p_reg,
                                                 nrf_saadc_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE nrf_saadc_event_t nrf_saadc_limit_event_get(uint8_t           channel,
                                                              nrf_saadc_limit_t limit_type)
{
    if (limit_type == NRF_SAADC_LIMIT_HIGH)
    {
        return (nrf_saadc_event_t)NRFX_OFFSETOF(NRF_SAADC_Type, EVENTS_CH[channel].LIMITH);
    }
    else
    {
        return (nrf_saadc_event_t)NRFX_OFFSETOF(NRF_SAADC_Type, EVENTS_CH[channel].LIMITL);
    }
}

NRF_STATIC_INLINE void nrf_saadc_channel_input_set(NRF_SAADC_Type *  p_reg,
                                                   uint8_t           channel,
                                                   nrf_saadc_input_t pselp,
                                                   nrf_saadc_input_t pseln)
{
    nrf_saadc_channel_pos_input_set(p_reg, channel, pselp);
    nrf_saadc_channel_neg_input_set(p_reg, channel, pseln);
}

NRF_STATIC_INLINE void nrf_saadc_channel_pos_input_set(NRF_SAADC_Type *  p_reg,
                                                       uint8_t           channel,
                                                       nrf_saadc_input_t pselp)
{
    uint32_t pselp_reg = pselp;
#if NRF_SAADC_HAS_AIN_AS_PIN
    if (pselp == NRF_SAADC_INPUT_DISABLED)
    {
        pselp_reg = 0;
    }
#if NRF_SAADC_HAS_CH_PSELP_INTERNAL
    else if (pselp >> SAADC_CH_PSELP_INTERNAL_Pos)
    {
        /* Substract shifted '1' before setting register value,
         * as it was artifically added before in the internal input definition */
        pselp_reg  = (pselp - (1 << SAADC_CH_PSELP_INTERNAL_Pos)) |
                     (SAADC_CH_PSELP_CONNECT_Internal << SAADC_CH_PSELP_CONNECT_Pos);
    }
#endif
    else
    {
        pselp_reg  = (NRF_PIN_NUMBER_TO_PIN(pselp)  << SAADC_CH_PSELP_PIN_Pos)  |
                     (NRF_PIN_NUMBER_TO_PORT(pselp) << SAADC_CH_PSELP_PORT_Pos) |
                     (SAADC_CH_PSELP_CONNECT_AnalogInput << SAADC_CH_PSELP_CONNECT_Pos);
    }
#endif
    p_reg->CH[channel].PSELP = pselp_reg;
}

NRF_STATIC_INLINE void nrf_saadc_channel_neg_input_set(NRF_SAADC_Type *  p_reg,
                                                       uint8_t           channel,
                                                       nrf_saadc_input_t pseln)
{
    uint32_t pseln_reg = pseln;
#if NRF_SAADC_HAS_AIN_AS_PIN
    if (pseln == NRF_SAADC_INPUT_DISABLED)
    {
        pseln_reg = 0;
    }
#if NRF_SAADC_HAS_CH_PSELN_INTERNAL
    else if (pseln >> SAADC_CH_PSELN_INTERNAL_Pos)
    {
        /* Substract shifted '1' before setting register value,
         * as it was artifically added before in the internal input definition */
        pseln_reg  = (pseln - (1 << SAADC_CH_PSELN_INTERNAL_Pos)) |
                     (SAADC_CH_PSELN_CONNECT_Internal << SAADC_CH_PSELN_CONNECT_Pos);
    }
#endif
    else
    {
        pseln_reg  = (NRF_PIN_NUMBER_TO_PIN(pseln)  << SAADC_CH_PSELN_PIN_Pos)  |
                     (NRF_PIN_NUMBER_TO_PORT(pseln) << SAADC_CH_PSELN_PORT_Pos) |
                     (SAADC_CH_PSELN_CONNECT_AnalogInput << SAADC_CH_PSELN_CONNECT_Pos);
    }
#endif
    p_reg->CH[channel].PSELN = pseln_reg;
}

NRF_STATIC_INLINE void nrf_saadc_channel_limits_set(NRF_SAADC_Type * p_reg,
                                                    uint8_t          channel,
                                                    int16_t          low,
                                                    int16_t          high)
{
    p_reg->CH[channel].LIMIT = (
            (((uint32_t) low << SAADC_CH_LIMIT_LOW_Pos) & SAADC_CH_LIMIT_LOW_Msk)
          | (((uint32_t) high << SAADC_CH_LIMIT_HIGH_Pos) & SAADC_CH_LIMIT_HIGH_Msk));
}

NRF_STATIC_INLINE void nrf_saadc_int_set(NRF_SAADC_Type * p_reg, uint32_t mask)
{
    p_reg->INTEN = mask;
}

NRF_STATIC_INLINE void nrf_saadc_int_enable(NRF_SAADC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_int_enable_check(NRF_SAADC_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE void nrf_saadc_int_disable(NRF_SAADC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_limit_int_get(uint8_t           channel,
                                                   nrf_saadc_limit_t limit_type)
{
    NRFX_ASSERT(channel < SAADC_CH_NUM);
    uint32_t mask = (limit_type == NRF_SAADC_LIMIT_LOW) ?
                     NRF_SAADC_INT_CH0LIMITL : NRF_SAADC_INT_CH0LIMITH;
    return mask << (channel * 2);
}

NRF_STATIC_INLINE bool nrf_saadc_busy_check(NRF_SAADC_Type const * p_reg)
{
    return (p_reg->STATUS == (SAADC_STATUS_STATUS_Busy << SAADC_STATUS_STATUS_Pos));
}

NRF_STATIC_INLINE void nrf_saadc_enable(NRF_SAADC_Type * p_reg)
{
    p_reg->ENABLE = (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_saadc_disable(NRF_SAADC_Type * p_reg)
{
    p_reg->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE bool nrf_saadc_enable_check(NRF_SAADC_Type const * p_reg)
{
    return (p_reg->ENABLE == (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos));
}

NRF_STATIC_INLINE void nrf_saadc_buffer_init(NRF_SAADC_Type *    p_reg,
                                             nrf_saadc_value_t * p_buffer,
                                             uint32_t            size)
{
    p_reg->RESULT.PTR = (uint32_t)p_buffer;
#if defined(DMA_BUFFER_UNIFIED_BYTE_ACCESS)
    p_reg->RESULT.MAXCNT = size * 2;
#else
    p_reg->RESULT.MAXCNT = size;
#endif
}

NRF_STATIC_INLINE void nrf_saadc_buffer_pointer_set(NRF_SAADC_Type *    p_reg,
                                                    nrf_saadc_value_t * p_buffer)
{
    p_reg->RESULT.PTR = (uint32_t)p_buffer;
}

NRF_STATIC_INLINE nrf_saadc_value_t * nrf_saadc_buffer_pointer_get(NRF_SAADC_Type const * p_reg)
{
    return (nrf_saadc_value_t *)p_reg->RESULT.PTR;
}

NRF_STATIC_INLINE uint16_t nrf_saadc_amount_get(NRF_SAADC_Type const * p_reg)
{
#if defined(DMA_BUFFER_UNIFIED_BYTE_ACCESS)
    return ((uint16_t)p_reg->RESULT.AMOUNT / 2);
#else
    return (uint16_t)p_reg->RESULT.AMOUNT;
#endif
}

NRF_STATIC_INLINE void nrf_saadc_resolution_set(NRF_SAADC_Type *       p_reg,
                                                nrf_saadc_resolution_t resolution)
{
    p_reg->RESOLUTION = resolution;
}

NRF_STATIC_INLINE nrf_saadc_resolution_t nrf_saadc_resolution_get(NRF_SAADC_Type const * p_reg)
{
    return (nrf_saadc_resolution_t)p_reg->RESOLUTION;
}

NRF_STATIC_INLINE void nrf_saadc_oversample_set(NRF_SAADC_Type *       p_reg,
                                                nrf_saadc_oversample_t oversample)
{
    p_reg->OVERSAMPLE = oversample;
}

NRF_STATIC_INLINE nrf_saadc_oversample_t nrf_saadc_oversample_get(NRF_SAADC_Type const * p_reg)
{
    return (nrf_saadc_oversample_t)p_reg->OVERSAMPLE;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_oversample_sample_count_get(nrf_saadc_oversample_t oversample)
{
    return (1 << (uint32_t)oversample);
}

NRF_STATIC_INLINE void nrf_saadc_continuous_mode_enable(NRF_SAADC_Type * p_reg, uint16_t cc)
{
    NRFX_ASSERT((cc >= NRF_SAADC_SAMPLERATE_CC_MIN) && (cc <= NRF_SAADC_SAMPLERATE_CC_MAX));
    p_reg->SAMPLERATE = (SAADC_SAMPLERATE_MODE_Timers << SAADC_SAMPLERATE_MODE_Pos)
                        | ((uint32_t)cc << SAADC_SAMPLERATE_CC_Pos);
}

NRF_STATIC_INLINE bool nrf_saadc_continuous_mode_enable_check(NRF_SAADC_Type const * p_reg)
{
    return (bool)((p_reg->SAMPLERATE & SAADC_SAMPLERATE_MODE_Msk)
                   == (SAADC_SAMPLERATE_MODE_Timers << SAADC_SAMPLERATE_MODE_Pos));
}

NRF_STATIC_INLINE void nrf_saadc_continuous_mode_disable(NRF_SAADC_Type * p_reg)
{
    p_reg->SAMPLERATE = SAADC_SAMPLERATE_MODE_Task << SAADC_SAMPLERATE_MODE_Pos;
}

#if NRF_SAADC_HAS_LIN_CAL
NRF_STATIC_INLINE void nrf_saadc_linearity_calibration_coeff_set(NRF_SAADC_Type * p_reg,
                                                                 uint8_t          index,
                                                                 uint32_t         coeff)
{
    NRFX_ASSERT(index < NRF_SAADC_LIN_CAL_MAX_COUNT);
    NRFX_ASSERT(coeff <= NRF_SAADC_LIN_CAL_MAX);
    p_reg->TRIM.LINCALCOEFF[index] = coeff;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_linearity_calibration_coeff_get(NRF_SAADC_Type const * p_reg,
                                                                     uint8_t                index)
{
    NRFX_ASSERT(index < NRF_SAADC_LIN_CAL_MAX_COUNT);
    return p_reg->TRIM.LINCALCOEFF[index];
}
#endif

NRF_STATIC_INLINE void nrf_saadc_channel_init(NRF_SAADC_Type *                   p_reg,
                                              uint8_t                            channel,
                                              nrf_saadc_channel_config_t const * config)
{
#if !NRF_SAADC_HAS_ACQTIME_ENUM
    NRFX_ASSERT(config->acq_time <= NRF_SAADC_ACQTIME_MAX);
#endif
#if NRF_SAADC_HAS_CONVTIME
    NRFX_ASSERT(config->conv_time <= NRF_SAADC_CONVTIME_MAX);
#endif
    p_reg->CH[channel].CONFIG =
            ((config->reference    << SAADC_CH_CONFIG_REFSEL_Pos) & SAADC_CH_CONFIG_REFSEL_Msk)
#if NRF_SAADC_HAS_CH_GAIN
            | ((config->gain       << SAADC_CH_CONFIG_GAIN_Pos)   & SAADC_CH_CONFIG_GAIN_Msk)
#endif
            | ((config->acq_time   << SAADC_CH_CONFIG_TACQ_Pos)   & SAADC_CH_CONFIG_TACQ_Msk)
#if NRF_SAADC_HAS_CH_CONFIG_RES
            | ((config->resistor_p << SAADC_CH_CONFIG_RESP_Pos)   & SAADC_CH_CONFIG_RESP_Msk)
            | ((config->resistor_n << SAADC_CH_CONFIG_RESN_Pos)   & SAADC_CH_CONFIG_RESN_Msk)
#endif
#if NRF_SAADC_HAS_CONVTIME
            | ((config->conv_time  << SAADC_CH_CONFIG_TCONV_Pos)  & SAADC_CH_CONFIG_TCONV_Msk)
#endif
#if NRF_SAADC_HAS_CH_BURST
            | ((config->burst      << SAADC_CH_CONFIG_BURST_Pos)  & SAADC_CH_CONFIG_BURST_Msk)
#endif
#if NRF_SAADC_HAS_CH_CHOPPING
            | ((config->chopping   << SAADC_CH_CONFIG_CHOPPING_Pos) & SAADC_CH_CONFIG_CHOPPING_Msk)
#endif
#if NRF_SAADC_HAS_CH_HIGHSPEED
            | ((config->highspeed  << SAADC_CH_CONFIG_HIGHSPEED_Pos) &
                                      SAADC_CH_CONFIG_HIGHSPEED_Msk)
#endif
            | ((config->mode       << SAADC_CH_CONFIG_MODE_Pos)   & SAADC_CH_CONFIG_MODE_Msk);
}

#if NRF_SAADC_HAS_CH_BURST
NRF_STATIC_INLINE void nrf_saadc_channel_burst_set(NRF_SAADC_Type *  p_reg,
                                                   uint8_t           channel,
                                                   nrf_saadc_burst_t burst)
{
    p_reg->CH[channel].CONFIG = (p_reg->CH[channel].CONFIG & ~SAADC_CH_CONFIG_BURST_Msk) |
                                (burst << SAADC_CH_CONFIG_BURST_Pos);
}
#endif

#if NRF_SAADC_HAS_BURST
NRF_STATIC_INLINE void nrf_saadc_burst_set(NRF_SAADC_Type *  p_reg,
                                           nrf_saadc_burst_t burst)
{
    p_reg->BURST = burst;
}
#endif

NRF_STATIC_INLINE nrf_saadc_value_t nrf_saadc_value_min_get(nrf_saadc_resolution_t resolution)
{
    uint8_t res_bits = 0;
    switch (resolution)
    {
        case NRF_SAADC_RESOLUTION_8BIT:
            res_bits = 8;
            break;
        case NRF_SAADC_RESOLUTION_10BIT:
            res_bits = 10;
            break;
        case NRF_SAADC_RESOLUTION_12BIT:
            res_bits = 12;
            break;
        case NRF_SAADC_RESOLUTION_14BIT:
            res_bits = 14;
            break;
        default:
            NRFX_ASSERT(false);
    }
    return (nrf_saadc_value_t)(-(1 << res_bits));
}

NRF_STATIC_INLINE nrf_saadc_value_t nrf_saadc_value_max_get(nrf_saadc_resolution_t resolution)
{
    uint8_t res_bits = 0;
    switch (resolution)
    {
        case NRF_SAADC_RESOLUTION_8BIT:
            res_bits = 8;
            break;
        case NRF_SAADC_RESOLUTION_10BIT:
            res_bits = 10;
            break;
        case NRF_SAADC_RESOLUTION_12BIT:
            res_bits = 12;
            break;
        case NRF_SAADC_RESOLUTION_14BIT:
            res_bits = 14;
            break;
        default:
            NRFX_ASSERT(false);
    }
    return (nrf_saadc_value_t)((1 << res_bits) - 1);
}

#if NRF_SAADC_HAS_CAL
NRF_STATIC_INLINE void nrf_saadc_cal_set(NRF_SAADC_Type * p_reg, uint32_t trim)
{
    p_reg->CAL = trim;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_cal_get(NRF_SAADC_Type const * p_reg)
{
    return p_reg->CAL;
}
#endif

#if NRF_SAADC_HAS_CALREF
NRF_STATIC_INLINE void nrf_saadc_calref_set(NRF_SAADC_Type * p_reg, uint32_t trim)
{
    p_reg->CALREF = trim;
}

NRF_STATIC_INLINE uint32_t nrf_saadc_calref_get(NRF_SAADC_Type const * p_reg)
{
    return p_reg->CALREF;
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_SAADC_H_
