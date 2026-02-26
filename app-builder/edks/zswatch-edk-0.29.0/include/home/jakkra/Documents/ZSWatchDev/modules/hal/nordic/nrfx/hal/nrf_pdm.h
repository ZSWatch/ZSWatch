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
#ifndef NRF_PDM_H_
#define NRF_PDM_H_

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NRF_PDM0) && defined(NRF_PDM)
#define NRF_PDM0 NRF_PDM
#endif

/**
 * @defgroup nrf_pdm_hal PDM HAL
 * @{
 * @ingroup nrf_pdm
 * @brief   Hardware access layer for managing the Pulse Density Modulation (PDM) peripheral.
 */

/**
 * @brief Macro for getting a pointer to the structure of registers of the PDM peripheral.
 *
 * @param[in] idx PDM instance index.
 *
 * @return Pointer to the structure of registers of the PDM peripheral.
 */
#define NRF_PDM_INST_GET(idx) NRFX_CONCAT_2(NRF_PDM, idx)

#if defined(PDM_MCLKCONFIG_SRC_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether master clock source configuration is available. */
#define NRF_PDM_HAS_MCLKCONFIG 1
#else
#define NRF_PDM_HAS_MCLKCONFIG 0
#endif

#if defined(PDM_RATIO_RATIO_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether ratio configuration is available. */
#define NRF_PDM_HAS_RATIO_CONFIG 1
#else
#define NRF_PDM_HAS_RATIO_CONFIG 0
#endif

#if (defined(PDM_TASKS_DMA_START_START_Msk) && defined(PDM_EVENTS_DMA_END_END_Msk)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM DMA tasks and events are present. */
#define NRF_PDM_HAS_DMA_TASKS_EVENTS 1
#else
#define NRF_PDM_HAS_DMA_TASKS_EVENTS 0
#endif

#if defined(PDM_PDMCLKCTRL_FREQ_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM clock control register is available. */
#define NRF_PDM_HAS_PDMCLKCTRL 1
#else
#define NRF_PDM_HAS_PDMCLKCTRL 0
#endif

#if defined(PDM_PDMCLKCTRL_FREQ_1231K) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether 1231k PDM clock frequency is available. */
#define NRF_PDM_HAS_FREQ_1231K 1
#else
#define NRF_PDM_HAS_FREQ_1231K 0
#endif

#if defined(PDM_PDMCLKCTRL_FREQ_1280K) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether 1280k PDM clock frequency is available. */
#define NRF_PDM_HAS_FREQ_1280K 1
#else
#define NRF_PDM_HAS_FREQ_1280K 0
#endif

#if defined(PDM_PDMCLKCTRL_FREQ_1333K) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether 1333k PDM clock frequency is available. */
#define NRF_PDM_HAS_FREQ_1333K 1
#else
#define NRF_PDM_HAS_FREQ_1333K 0
#endif

#if defined(PDM_PRESCALER_DIVISOR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM prescaler register is available. */
#define NRF_PDM_HAS_PRESCALER 1
#else
#define NRF_PDM_HAS_PRESCALER 0
#endif

#if defined(PDM_CLKSELECT_SRC_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM clock select register is available. */
#define NRF_PDM_HAS_CLKSELECT 1
#else
#define NRF_PDM_HAS_CLKSELECT 0
#endif

#if defined(PDM_RATIO_RATIO_Ratio32)  || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM ratio of 32 configuration is available. */
#define NRF_PDM_HAS_RATIO32 1
#else
#define NRF_PDM_HAS_RATIO32 0
#endif

#if defined(PDM_RATIO_RATIO_Ratio48)  || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM ratio of 48 configuration is available. */
#define NRF_PDM_HAS_RATIO48 1
#else
#define NRF_PDM_HAS_RATIO48 0
#endif

#if defined(PDM_RATIO_RATIO_Ratio50)  || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM ratio of 50 configuration is available. */
#define NRF_PDM_HAS_RATIO50 1
#else
#define NRF_PDM_HAS_RATIO50 0
#endif

#if defined(PDM_RATIO_RATIO_Ratio96)  || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM ratio of 96 configuration is available. */
#define NRF_PDM_HAS_RATIO96 1
#else
#define NRF_PDM_HAS_RATIO96 0
#endif

#if defined(PDM_RATIO_RATIO_Ratio100) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM ratio of 100 configuration is available. */
#define NRF_PDM_HAS_RATIO100 1
#else
#define NRF_PDM_HAS_RATIO100 0
#endif

#if defined(PDM_RATIO_RATIO_Ratio128) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM ratio of 128 configuration is available. */
#define NRF_PDM_HAS_RATIO128 1
#else
#define NRF_PDM_HAS_RATIO128 0
#endif

#if defined(PDM_RATIO_RATIO_Ratio150) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM ratio of 150 configuration is available. */
#define NRF_PDM_HAS_RATIO150 1
#else
#define NRF_PDM_HAS_RATIO150 0
#endif

#if defined(PDM_RATIO_RATIO_Ratio192) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM ratio of 192 configuration is available. */
#define NRF_PDM_HAS_RATIO192 1
#else
#define NRF_PDM_HAS_RATIO192 0
#endif

#if defined(PDM_RATIO_RATIO_Custom) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM custom ratio configuration is available. */
#define NRF_PDM_HAS_CUSTOM_RATIO 1
#else
#define NRF_PDM_HAS_CUSTOM_RATIO 0
#endif

#if defined(PDM_FILTER_CTRL_DECRATIO_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PDM filter registers are available. */
#define NRF_PDM_HAS_FILTER 1
#else
#define NRF_PDM_HAS_FILTER 0
#endif

/** @brief Minimum value of PDM gain. */
#define NRF_PDM_GAIN_MINIMUM  0x00
/** @brief Default value of PDM gain. */
#define NRF_PDM_GAIN_DEFAULT  0x28
/** @brief Maximum value of PDM gain. */
#define NRF_PDM_GAIN_MAXIMUM  0x50

#if NRF_PDM_HAS_PRESCALER
/** @brief Minimum value of PDM prescaler. */
#define NRF_PDM_PRESCALER_MIN PDM_PRESCALER_DIVISOR_Min
/** @brief Maximum value of PDM prescaler. */
#define NRF_PDM_PRESCALER_MAX PDM_PRESCALER_DIVISOR_Max
#endif

#if NRF_PDM_HAS_MCLKCONFIG || NRF_PDM_HAS_CLKSELECT
/** @brief Symbol indicating whether PDM has selectable clock source. */
#define NRF_PDM_HAS_SELECTABLE_CLOCK 1
#else
#define NRF_PDM_HAS_SELECTABLE_CLOCK 0
#endif

/** @brief PDM gain type. */
typedef uint8_t nrf_pdm_gain_t;

/** @brief PDM tasks. */
typedef enum
{
#if NRF_PDM_HAS_DMA_TASKS_EVENTS
    NRF_PDM_TASK_START = offsetof(NRF_PDM_Type, TASKS_DMA.START), ///< Starts continuous PDM transfer.
    NRF_PDM_TASK_STOP  = offsetof(NRF_PDM_Type, TASKS_DMA.STOP),  ///< Stops PDM transfer.
#else
    NRF_PDM_TASK_START = offsetof(NRF_PDM_Type, TASKS_START),     ///< Starts continuous PDM transfer.
    NRF_PDM_TASK_STOP  = offsetof(NRF_PDM_Type, TASKS_STOP),      ///< Stops PDM transfer.
#endif
} nrf_pdm_task_t;

/** @brief PDM events. */
typedef enum
{
    NRF_PDM_EVENT_STARTED = offsetof(NRF_PDM_Type, EVENTS_STARTED), ///< PDM transfer is started.
    NRF_PDM_EVENT_STOPPED = offsetof(NRF_PDM_Type, EVENTS_STOPPED), ///< PDM transfer is finished.
#if NRF_PDM_HAS_DMA_TASKS_EVENTS
    NRF_PDM_EVENT_END     = offsetof(NRF_PDM_Type, EVENTS_DMA.END), ///< The PDM has written the last sample specified by MAXCNT (or the last sample after a STOP task has been received) to Data RAM.
#else
    NRF_PDM_EVENT_END     = offsetof(NRF_PDM_Type, EVENTS_END),     ///< The PDM has written the last sample specified by MAXCNT (or the last sample after a STOP task has been received) to Data RAM.
#endif
} nrf_pdm_event_t;

/** @brief PDM interrupt masks. */
typedef enum
{
    NRF_PDM_INT_STARTED = PDM_INTENSET_STARTED_Msk, ///< Interrupt on EVENTS_STARTED event.
    NRF_PDM_INT_STOPPED = PDM_INTENSET_STOPPED_Msk, ///< Interrupt on EVENTS_STOPPED event.
#if NRF_PDM_HAS_DMA_TASKS_EVENTS
    NRF_PDM_INT_END     = PDM_INTENSET_DMAEND_Msk   ///< Interrupt on EVENTS_END event.
#else
    NRF_PDM_INT_END     = PDM_INTENSET_END_Msk      ///< Interrupt on EVENTS_END event.
#endif
} nrf_pdm_int_mask_t;

#if NRF_PDM_HAS_PDMCLKCTRL
/** @brief PDM clock frequency. */
typedef enum
{
    NRF_PDM_FREQ_1000K = PDM_PDMCLKCTRL_FREQ_1000K,   ///< PDM_CLK = 1.000 MHz.
    NRF_PDM_FREQ_1032K = PDM_PDMCLKCTRL_FREQ_Default, ///< PDM_CLK = 1.032 MHz.
    NRF_PDM_FREQ_1067K = PDM_PDMCLKCTRL_FREQ_1067K,   ///< PDM_CLK = 1.067 MHz.
#if NRF_PDM_HAS_FREQ_1231K || defined(__NRFX_DOXYGEN__)
    NRF_PDM_FREQ_1231K = PDM_PDMCLKCTRL_FREQ_1231K,   ///< PDM_CLK = 1.231 MHz.
#endif
#if NRF_PDM_HAS_FREQ_1280K || defined(__NRFX_DOXYGEN__)
    NRF_PDM_FREQ_1280K = PDM_PDMCLKCTRL_FREQ_1280K,   ///< PDM_CLK = 1.280 MHz.
#endif
#if NRF_PDM_HAS_FREQ_1333K || defined(__NRFX_DOXYGEN__)
    NRF_PDM_FREQ_1333K = PDM_PDMCLKCTRL_FREQ_1333K    ///< PDM_CLK = 1.333 MHz.
#endif
} nrf_pdm_freq_t;
#endif

#if NRF_PDM_HAS_RATIO_CONFIG
/** @brief PDM ratio between PDM_CLK and output sample rate. */
typedef enum
{
#if NRF_PDM_HAS_RATIO32  || defined(__NRFX_DOXYGEN__)
    NRF_PDM_RATIO_32X    = PDM_RATIO_RATIO_Ratio32,  ///< Ratio of 32.
#endif
#if NRF_PDM_HAS_RATIO48  || defined(__NRFX_DOXYGEN__)
    NRF_PDM_RATIO_48X    = PDM_RATIO_RATIO_Ratio48,  ///< Ratio of 48.
#endif
#if NRF_PDM_HAS_RATIO50  || defined(__NRFX_DOXYGEN__)
    NRF_PDM_RATIO_50X    = PDM_RATIO_RATIO_Ratio50,  ///< Ratio of 50.
#endif
    NRF_PDM_RATIO_64X    = PDM_RATIO_RATIO_Ratio64,  ///< Ratio of 64.
    NRF_PDM_RATIO_80X    = PDM_RATIO_RATIO_Ratio80,  ///< Ratio of 80.
#if NRF_PDM_HAS_RATIO96  || defined(__NRFX_DOXYGEN__)
    NRF_PDM_RATIO_96X    = PDM_RATIO_RATIO_Ratio96,  ///< Ratio of 96.
#endif
#if NRF_PDM_HAS_RATIO100 || defined(__NRFX_DOXYGEN__)
    NRF_PDM_RATIO_100X   = PDM_RATIO_RATIO_Ratio100, ///< Ratio of 100.
#endif
#if NRF_PDM_HAS_RATIO128 || defined(__NRFX_DOXYGEN__)
    NRF_PDM_RATIO_128X   = PDM_RATIO_RATIO_Ratio128, ///< Ratio of 128.
#endif
#if NRF_PDM_HAS_RATIO150 || defined(__NRFX_DOXYGEN__)
    NRF_PDM_RATIO_150X   = PDM_RATIO_RATIO_Ratio150, ///< Ratio of 150.
#endif
#if NRF_PDM_HAS_RATIO192 || defined(__NRFX_DOXYGEN__)
    NRF_PDM_RATIO_192X   = PDM_RATIO_RATIO_Ratio192, ///< Ratio of 192.
#endif
#if NRF_PDM_HAS_CUSTOM_RATIO
    NRF_PDM_RATIO_CUSTOM = PDM_RATIO_RATIO_Custom,   ///< Custom ratio.
#endif
} nrf_pdm_ratio_t;
#endif

/** @brief PDM operation mode. */
typedef enum
{
    NRF_PDM_MODE_STEREO = PDM_MODE_OPERATION_Stereo,  ///< Sample and store one pair (Left + Right) of 16-bit samples per RAM word.
    NRF_PDM_MODE_MONO   = PDM_MODE_OPERATION_Mono     ///< Sample and store two successive Left samples (16 bit each) per RAM word.
} nrf_pdm_mode_t;

/** @brief PDM sampling mode. */
typedef enum
{
    NRF_PDM_EDGE_LEFTFALLING = PDM_MODE_EDGE_LeftFalling,  ///< Left (or mono) is sampled on falling edge of PDM_CLK.
    NRF_PDM_EDGE_LEFTRISING  = PDM_MODE_EDGE_LeftRising    ///< Left (or mono) is sampled on rising edge of PDM_CLK.
} nrf_pdm_edge_t;

#if NRF_PDM_HAS_SELECTABLE_CLOCK
/** @brief PDM clock source selection. */
typedef enum
{
#if NRF_PDM_HAS_MCLKCONFIG
    NRF_PDM_MCLKSRC_PCLK32M = PDM_MCLKCONFIG_SRC_PCLK32M, ///< 32MHz peripheral clock.
    NRF_PDM_MCLKSRC_ACLK    = PDM_MCLKCONFIG_SRC_ACLK     ///< Audio PLL clock.
#elif NRF_PDM_HAS_CLKSELECT
    NRF_PDM_MCLKSRC_PCLK32M = PDM_CLKSELECT_SRC_PCLK32M,  ///< 32MHz peripheral clock.
    NRF_PDM_MCLKSRC_ACLK    = PDM_CLKSELECT_SRC_ACLK      ///< Audio PLL clock.
#endif
} nrf_pdm_mclksrc_t;
#endif

#if NRF_PDM_HAS_FILTER
/** @brief PDM input data sample delay. */
typedef enum
{
    NRF_PDM_SAMPLE_DELAY_NONE  = PDM_FILTER_CTRL_DATASAMPLEDELAY_NoDelay,      ///< No delay added.
    NRF_PDM_SAMPLE_DELAY_LEFT  = PDM_FILTER_CTRL_DATASAMPLEDELAY_DelayOnLeft,  ///< One clock cycle delay on left channel.
    NRF_PDM_SAMPLE_DELAY_RIGHT = PDM_FILTER_CTRL_DATASAMPLEDELAY_DelayOnRight, ///< One clock cycle delay on right channel.
    NRF_PDM_SAMPLE_DELAY_BOTH  = PDM_FILTER_CTRL_DATASAMPLEDELAY_DelayOnBoth,  ///< One clock cycle delay on both channels.
} nrf_pdm_sample_delay_t;

/** @brief PDM CIC filter most significant bit. */
typedef enum
{
    NRF_PDM_FILTER_CIC_MSB_RANGE_0  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range0,  ///< OSR range low - 4 bits, OSR range high - 32 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_1  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range1,  ///< OSR range low - 34 bits, OSR range high - 36 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_2  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range2,  ///< OSR range low - 38 bits, OSR range high - 42 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_3  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range3,  ///< OSR range low - 44 bits, OSR range high - 48 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_4  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range4,  ///< OSR range low - 50 bits, OSR range high - 54 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_5  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range5,  ///< OSR range low - 56 bits, OSR range high - 64 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_6  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range6,  ///< OSR range low - 66 bits, OSR range high - 72 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_7  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range7,  ///< OSR range low - 74 bits, OSR range high - 84 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_8  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range8,  ///< OSR range low - 86 bits, OSR range high - 96 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_9  = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range9,  ///< OSR range low - 98 bits, OSR range high - 110 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_10 = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range10, ///< OSR range low - 112 bits, OSR range high - 128 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_11 = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range11, ///< OSR range low - 130 bits, OSR range high - 146 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_12 = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range12, ///< OSR range low - 148 bits, OSR range high - 168 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_13 = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range13, ///< OSR range low - 170 bits, OSR range high - 194 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_14 = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range14, ///< OSR range low - 196 bits, OSR range high - 222 bits.
    NRF_PDM_FILTER_CIC_MSB_RANGE_15 = PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Range15, ///< OSR range low - 224 bits, OSR range high - 256 bits.
} nrf_pdm_filter_cic_t;

/** @brief PDM high pass filter -3dB gain pole frequency. */
typedef enum
{
    NRF_PDM_FILTER_HP_POLE_0_16_HZ = PDM_FILTER_HPPOLE_HPPOLE_p0p16, ///< 0.16 Hz.
    NRF_PDM_FILTER_HP_POLE_0_32_HZ = PDM_FILTER_HPPOLE_HPPOLE_p0p32, ///< 0.32 Hz.
    NRF_PDM_FILTER_HP_POLE_0_64_HZ = PDM_FILTER_HPPOLE_HPPOLE_p0p64, ///< 0.64 Hz.
    NRF_PDM_FILTER_HP_POLE_1_25_HZ = PDM_FILTER_HPPOLE_HPPOLE_p1p25, ///< 1.25 Hz.
    NRF_PDM_FILTER_HP_POLE_2_5_HZ  = PDM_FILTER_HPPOLE_HPPOLE_p2p5,  ///< 2.5 Hz.
    NRF_PDM_FILTER_HP_POLE_5_HZ    = PDM_FILTER_HPPOLE_HPPOLE_p5,    ///< 5 Hz.
    NRF_PDM_FILTER_HP_POLE_10_HZ   = PDM_FILTER_HPPOLE_HPPOLE_p10,   ///< 10 Hz.
    NRF_PDM_FILTER_HP_POLE_20_HZ   = PDM_FILTER_HPPOLE_HPPOLE_p20,   ///< 20 Hz.
    NRF_PDM_FILTER_HP_POLE_40_HZ   = PDM_FILTER_HPPOLE_HPPOLE_p40,   ///< 40 Hz.
    NRF_PDM_FILTER_HP_POLE_79_HZ   = PDM_FILTER_HPPOLE_HPPOLE_p79,   ///< 79 Hz.
    NRF_PDM_FILTER_HP_POLE_157_HZ  = PDM_FILTER_HPPOLE_HPPOLE_p157,  ///< 157 Hz.
    NRF_PDM_FILTER_HP_POLE_310_HZ  = PDM_FILTER_HPPOLE_HPPOLE_p310,  ///< 310 Hz.
    NRF_PDM_FILTER_HP_POLE_603_HZ  = PDM_FILTER_HPPOLE_HPPOLE_p603,  ///< 603 Hz.
    NRF_PDM_FILTER_HP_POLE_1152_HZ = PDM_FILTER_HPPOLE_HPPOLE_p1152, ///< 1152 Hz.
    NRF_PDM_FILTER_HP_POLE_2110_HZ = PDM_FILTER_HPPOLE_HPPOLE_p2110, ///< 2110 Hz.
} nrf_pdm_filter_hp_pole_t;

/** @brief PDM number of cycles for soft mute transition. */
typedef enum
{
    NRF_PDM_FILTER_SOFTCYCLES_2      = PDM_FILTER_SOFTCYCLES_DISABLE_s2,     ///< 2 filter source clock cycles.
    NRF_PDM_FILTER_SOFTCYCLES_8      = PDM_FILTER_SOFTCYCLES_DISABLE_s8,     ///< 8 filter source clock cycles.
    NRF_PDM_FILTER_SOFTCYCLES_32     = PDM_FILTER_SOFTCYCLES_DISABLE_s32,    ///< 32 filter source clock cycles.
    NRF_PDM_FILTER_SOFTCYCLES_64     = PDM_FILTER_SOFTCYCLES_DISABLE_s64,    ///< 64 filter source clock cycles.
    NRF_PDM_FILTER_SOFTCYCLES_128    = PDM_FILTER_SOFTCYCLES_DISABLE_s128,   ///< 128 filter source clock cycles.
    NRF_PDM_FILTER_SOFTCYCLES_256    = PDM_FILTER_SOFTCYCLES_DISABLE_s256,   ///< 256 filter source clock cycles.
    NRF_PDM_FILTER_SOFTCYCLES_512    = PDM_FILTER_SOFTCYCLES_DISABLE_s512,   ///< 512 filter source clock cycles.
    NRF_PDM_FILTER_SOFTCYCLES_CUSTOM = PDM_FILTER_SOFTCYCLES_DISABLE_custom, ///< Custom number of filter source clock cycles.
} nrf_pdm_filter_softcycles_t;

/** @brief PDM fliter configuration. */
typedef struct
{
    bool                   r_mute;                 ///< Override soft mute for right channel.
    bool                   l_mute;                 ///< Override soft mute for left channel.
    bool                   gain_step_add;          ///< Add +0.25dB to the gain stage.
    bool                   minor_gain_step_custom; ///< Compensate gain with +0.25dB.
    uint8_t                gain_step_custom;       ///< +0.5dB steps to compensate gain.
    uint8_t                soft_cycles;            ///< Number of cycles for soft gain/mute function.
    nrf_pdm_sample_delay_t sample_delay;           ///< Input data sample delay.
    nrf_pdm_filter_cic_t   cic_filter;             ///< MSB for CIC filter.
} nrf_pdm_filter_ctrl_t;
#endif

/**
 * @brief Function for triggering a PDM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  PDM task.
 */
NRF_STATIC_INLINE void nrf_pdm_task_trigger(NRF_PDM_Type * p_reg, nrf_pdm_task_t task);

/**
 * @brief Function for getting the address of a PDM task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  PDM task.
 *
 * @return Address of the specified PDM task.
 */
NRF_STATIC_INLINE uint32_t nrf_pdm_task_address_get(NRF_PDM_Type const * p_reg,
                                                    nrf_pdm_task_t       task);

/**
 * @brief Function for retrieving the state of the PDM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_pdm_event_check(NRF_PDM_Type const * p_reg, nrf_pdm_event_t event);

/**
 * @brief Function for clearing a PDM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event PDM event.
 */
NRF_STATIC_INLINE void nrf_pdm_event_clear(NRF_PDM_Type * p_reg, nrf_pdm_event_t event);

/**
 * @brief Function for getting the address of a PDM event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event PDM event.
 *
 * @return Address of the specified PDM event.
 */
NRF_STATIC_INLINE uint32_t nrf_pdm_event_address_get(NRF_PDM_Type const * p_reg,
                                                     nrf_pdm_event_t      event);

/**
 * @brief Function for enabling PDM interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_pdm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_pdm_int_enable(NRF_PDM_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_pdm_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_pdm_int_enable_check(NRF_PDM_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for disabling interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_pdm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_pdm_int_disable(NRF_PDM_Type * p_reg, uint32_t mask);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        PDM task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_pdm_subscribe_set(NRF_PDM_Type * p_reg,
                                             nrf_pdm_task_t task,
                                             uint8_t        channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        PDM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_pdm_subscribe_clear(NRF_PDM_Type * p_reg, nrf_pdm_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        PDM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return PDM subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_pdm_subscribe_get(NRF_PDM_Type const * p_reg,
                                                 nrf_pdm_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        PDM event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_pdm_publish_set(NRF_PDM_Type *  p_reg,
                                           nrf_pdm_event_t event,
                                           uint8_t         channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        PDM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_pdm_publish_clear(NRF_PDM_Type * p_reg, nrf_pdm_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        PDM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return PDM publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_pdm_publish_get(NRF_PDM_Type const * p_reg,
                                               nrf_pdm_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for enabling the PDM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * The PDM peripheral must be enabled before use.
 */
NRF_STATIC_INLINE void nrf_pdm_enable(NRF_PDM_Type * p_reg);

/**
 * @brief Function for disabling the PDM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_pdm_disable(NRF_PDM_Type * p_reg);

/**
 * @brief Function for checking if the PDM peripheral is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The PDM peripheral is enabled.
 * @retval false The PDM peripheral is not enabled.
 */
NRF_STATIC_INLINE bool nrf_pdm_enable_check(NRF_PDM_Type const * p_reg);

/**
 * @brief Function for setting the PDM operation mode.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] pdm_mode PDM operation mode.
 * @param[in] pdm_edge PDM sampling mode.
 */
NRF_STATIC_INLINE void nrf_pdm_mode_set(NRF_PDM_Type * p_reg,
                                        nrf_pdm_mode_t pdm_mode,
                                        nrf_pdm_edge_t pdm_edge);

/**
 * @brief Function for getting the PDM operation mode.
 *
 * @param[in]  p_reg      Pointer to the structure of registers of the peripheral.
 * @param[out] p_pdm_mode PDM operation mode.
 * @param[out] p_pdm_edge PDM sampling mode.
 */
NRF_STATIC_INLINE void nrf_pdm_mode_get(NRF_PDM_Type const * p_reg,
                                        nrf_pdm_mode_t *     p_pdm_mode,
                                        nrf_pdm_edge_t *     p_pdm_edge);

#if NRF_PDM_HAS_PDMCLKCTRL
/**
 * @brief Function for setting the PDM clock frequency.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] pdm_freq PDM clock frequency.
 */
NRF_STATIC_INLINE void nrf_pdm_clock_set(NRF_PDM_Type * p_reg, nrf_pdm_freq_t pdm_freq);

/**
 * @brief Function for getting the PDM clock frequency.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return PDM clock frequency.
 */
NRF_STATIC_INLINE nrf_pdm_freq_t nrf_pdm_clock_get(NRF_PDM_Type const * p_reg);
#endif

#if NRF_PDM_HAS_PRESCALER
/**
 * @brief Function for setting the PDM prescaler divisor.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] prescaler PDM prescaler divisor.
 */
NRF_STATIC_INLINE void nrf_pdm_prescaler_set(NRF_PDM_Type * p_reg, uint32_t prescaler);

/**
 * @brief Function for getting the PDM prescaler divisor.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return PDM prescaler divisor.
 */
NRF_STATIC_INLINE uint32_t nrf_pdm_prescaler_get(NRF_PDM_Type const * p_reg);
#endif

/**
 * @brief Function for setting up the PDM pins.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] psel_clk CLK pin number.
 * @param[in] psel_din DIN pin number.
 */
NRF_STATIC_INLINE void nrf_pdm_psel_connect(NRF_PDM_Type * p_reg,
                                            uint32_t       psel_clk,
                                            uint32_t       psel_din);

/**
 * @brief Function for setting the CLK pin.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   CLK pin number.
 */
NRF_STATIC_INLINE void nrf_pdm_clk_pin_set(NRF_PDM_Type * p_reg, uint32_t pin);

/**
 * @brief Function for setting the DIN pin.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   DIN pin number.
 */
NRF_STATIC_INLINE void nrf_pdm_din_pin_set(NRF_PDM_Type * p_reg, uint32_t pin);

/**
 * @brief Function for getting the CLK pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return CLK pin selection;
 */
NRF_STATIC_INLINE uint32_t nrf_pdm_clk_pin_get(NRF_PDM_Type const * p_reg);

/**
 * @brief Function for getting the DIN pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return DIN pin selection;
 */
NRF_STATIC_INLINE uint32_t nrf_pdm_din_pin_get(NRF_PDM_Type const * p_reg);

/**
 * @brief Function for disconnecting the PDM pins.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_pdm_psel_disconnect(NRF_PDM_Type * p_reg);

/**
 * @brief Function for setting the PDM gain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] gain_l Left channel gain.
 * @param[in] gain_r Right channel gain.
 */
NRF_STATIC_INLINE void nrf_pdm_gain_set(NRF_PDM_Type * p_reg,
                                        nrf_pdm_gain_t gain_l,
                                        nrf_pdm_gain_t gain_r);

/**
 * @brief Function for getting the PDM gain.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_gain_l Left channel gain.
 * @param[out] p_gain_r Right channel gain.
 */
NRF_STATIC_INLINE void nrf_pdm_gain_get(NRF_PDM_Type const * p_reg,
                                        nrf_pdm_gain_t *     p_gain_l,
                                        nrf_pdm_gain_t *     p_gain_r);

/**
 * @brief Function for setting the PDM sample buffer.
 *
 * The amount of allocated RAM depends on the operation mode.
 * - For stereo mode: N 32-bit words.
 * - For mono mode: Ceil(N/2) 32-bit words.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the RAM address where samples are to be written with EasyDMA.
 * @param[in] num      Number of samples to allocate memory for in EasyDMA mode.
 */
NRF_STATIC_INLINE void nrf_pdm_buffer_set(NRF_PDM_Type * p_reg, uint32_t * p_buffer, uint32_t num);

/**
 * @brief Function for getting the current PDM sample buffer address.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the current sample buffer.
 */
NRF_STATIC_INLINE uint32_t * nrf_pdm_buffer_get(NRF_PDM_Type const * p_reg);

#if NRF_PDM_HAS_RATIO_CONFIG
/**
 * @brief Function for setting ratio between PDM_CLK and output sample rate.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] ratio Ratio between PDM_CLK and output sample rate.
 */
NRF_STATIC_INLINE void nrf_pdm_ratio_set(NRF_PDM_Type * p_reg, nrf_pdm_ratio_t ratio);

#if NRF_PDM_HAS_CUSTOM_RATIO
/**
 * @brief Function for setting custom ratio between PDM_CLK and output sample rate.
 *
 * @warning Before calling this function, the caller must ensure that custom ratio
 *          configuration has been set using @ref nrf_pdm_ratio_set.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] ratio Custom ratio between PDM_CLK and output sample rate.
 */
NRF_STATIC_INLINE void nrf_pdm_custom_ratio_set(NRF_PDM_Type * p_reg, uint8_t ratio);
#endif
#endif // NRF_PDM_HAS_RATIO_CONFIG

#if NRF_PDM_HAS_SELECTABLE_CLOCK
/**
 * @brief Function for configuring PDM clock source.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] mclksrc Clock source selection.
 */
NRF_STATIC_INLINE void nrf_pdm_mclksrc_configure(NRF_PDM_Type * p_reg, nrf_pdm_mclksrc_t mclksrc);
#endif

#if NRF_PDM_HAS_FILTER
/**
 * @brief Function for setting PDM filter configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] fctrl Pointer to the structure with filter configuration to be set.
 */
NRF_STATIC_INLINE
void nrf_pdm_filter_ctrl_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_ctrl_t const * fctrl);

/**
 * @brief Function for getting PDM filter configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] fctrl Pointer to the structure to be filled with filter configuration.
 */
NRF_STATIC_INLINE
void nrf_pdm_filter_ctrl_get(NRF_PDM_Type const * p_reg, nrf_pdm_filter_ctrl_t * fctrl);

/**
 * @brief Function for enabling PDM high-pass filter gain pole.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if high-pass filter is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE
void nrf_pdm_filter_hp_pole_enable(NRF_PDM_Type * p_reg, bool enable);

/**
 * @brief Function for setting PDM high-pass filter gain pole.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] hppole High-pass filter gain pole.
 */
NRF_STATIC_INLINE
void nrf_pdm_filter_hp_pole_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_hp_pole_t hppole);

/**
 * @brief Function for getting PDM high-pass filter gain pole.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return High-pass filter gain pole.
 */
NRF_STATIC_INLINE nrf_pdm_filter_hp_pole_t nrf_pdm_filter_hp_pole_get(NRF_PDM_Type const * p_reg);

/**
 * @brief Function for enabling PDM filter soft mute function.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if soft mute function is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_pdm_filter_softmute_enable(NRF_PDM_Type * p_reg, bool enable);

/**
 * @brief Function for setting PDM number of cycles used for soft mute transition.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] cycles Number of cycles.
 */
NRF_STATIC_INLINE
void nrf_pdm_filter_softcycles_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_softcycles_t cycles);

/**
 * @brief Function for getting PDM number of cycles used for soft mute transition.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 *
 * @return Number of cycles.
 */
NRF_STATIC_INLINE
nrf_pdm_filter_softcycles_t nrf_pdm_filter_softcycles_get(NRF_PDM_Type const * p_reg);

/**
 * @brief Function for enabling PDM input data sample delay.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if sample delay is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_pdm_filter_sampledelay_enable(NRF_PDM_Type * p_reg, bool enable);
#endif

#ifndef NRF_DECLARE_ONLY
NRF_STATIC_INLINE void nrf_pdm_task_trigger(NRF_PDM_Type * p_reg, nrf_pdm_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_pdm_task_address_get(NRF_PDM_Type const * p_reg, nrf_pdm_task_t task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE bool nrf_pdm_event_check(NRF_PDM_Type const * p_reg, nrf_pdm_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_pdm_event_clear(NRF_PDM_Type * p_reg, nrf_pdm_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE uint32_t nrf_pdm_event_address_get(NRF_PDM_Type const * p_reg,
                                                     nrf_pdm_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_pdm_int_enable(NRF_PDM_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE uint32_t nrf_pdm_int_enable_check(NRF_PDM_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE void nrf_pdm_int_disable(NRF_PDM_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_pdm_subscribe_set(NRF_PDM_Type * p_reg,
                                             nrf_pdm_task_t task,
                                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_pdm_subscribe_clear(NRF_PDM_Type * p_reg, nrf_pdm_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_pdm_subscribe_get(NRF_PDM_Type const * p_reg,
                                                 nrf_pdm_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_pdm_publish_set(NRF_PDM_Type *  p_reg,
                                           nrf_pdm_event_t event,
                                           uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_pdm_publish_clear(NRF_PDM_Type * p_reg, nrf_pdm_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_pdm_publish_get(NRF_PDM_Type const * p_reg,
                                               nrf_pdm_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE void nrf_pdm_enable(NRF_PDM_Type * p_reg)
{
    p_reg->ENABLE = (PDM_ENABLE_ENABLE_Enabled << PDM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_pdm_disable(NRF_PDM_Type * p_reg)
{
    p_reg->ENABLE = (PDM_ENABLE_ENABLE_Disabled << PDM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE bool nrf_pdm_enable_check(NRF_PDM_Type const * p_reg)
{
    return (p_reg->ENABLE == (PDM_ENABLE_ENABLE_Enabled << PDM_ENABLE_ENABLE_Pos));
}

NRF_STATIC_INLINE void nrf_pdm_mode_set(NRF_PDM_Type * p_reg,
                                        nrf_pdm_mode_t pdm_mode,
                                        nrf_pdm_edge_t pdm_edge)
{
    p_reg->MODE = ((pdm_mode << PDM_MODE_OPERATION_Pos) & PDM_MODE_OPERATION_Msk)
                    | ((pdm_edge << PDM_MODE_EDGE_Pos) & PDM_MODE_EDGE_Msk);
}

NRF_STATIC_INLINE void nrf_pdm_mode_get(NRF_PDM_Type const * p_reg,
                                        nrf_pdm_mode_t *     p_pdm_mode,
                                        nrf_pdm_edge_t *     p_pdm_edge)
{
    uint32_t mode = p_reg->MODE;
    *p_pdm_mode = (nrf_pdm_mode_t)((mode & PDM_MODE_OPERATION_Msk ) >> PDM_MODE_OPERATION_Pos);
    *p_pdm_edge = (nrf_pdm_edge_t)((mode & PDM_MODE_EDGE_Msk ) >> PDM_MODE_EDGE_Pos);
}

#if NRF_PDM_HAS_PDMCLKCTRL
NRF_STATIC_INLINE void nrf_pdm_clock_set(NRF_PDM_Type * p_reg, nrf_pdm_freq_t pdm_freq)
{
    p_reg->PDMCLKCTRL = ((pdm_freq << PDM_PDMCLKCTRL_FREQ_Pos) & PDM_PDMCLKCTRL_FREQ_Msk);
}

NRF_STATIC_INLINE nrf_pdm_freq_t nrf_pdm_clock_get(NRF_PDM_Type const * p_reg)
{
     return (nrf_pdm_freq_t) ((p_reg->PDMCLKCTRL << PDM_PDMCLKCTRL_FREQ_Pos) &
                              PDM_PDMCLKCTRL_FREQ_Msk);
}
#endif

#if NRF_PDM_HAS_PRESCALER
NRF_STATIC_INLINE void nrf_pdm_prescaler_set(NRF_PDM_Type * p_reg, uint32_t prescaler)
{
    NRFX_ASSERT(prescaler >= NRF_PDM_PRESCALER_MIN);
    NRFX_ASSERT(prescaler <= NRF_PDM_PRESCALER_MAX);
    p_reg->PRESCALER = prescaler;
}

NRF_STATIC_INLINE uint32_t nrf_pdm_prescaler_get(NRF_PDM_Type const * p_reg)
{
    return p_reg->PRESCALER;
}
#endif

NRF_STATIC_INLINE void nrf_pdm_psel_connect(NRF_PDM_Type * p_reg,
                                            uint32_t       psel_clk,
                                            uint32_t       psel_din)
{
    p_reg->PSEL.CLK = psel_clk;
    p_reg->PSEL.DIN = psel_din;
}

NRF_STATIC_INLINE void nrf_pdm_clk_pin_set(NRF_PDM_Type * p_reg, uint32_t pin)
{
    p_reg->PSEL.CLK = pin;
}

NRF_STATIC_INLINE void nrf_pdm_din_pin_set(NRF_PDM_Type * p_reg, uint32_t pin)
{
    p_reg->PSEL.DIN = pin;
}

NRF_STATIC_INLINE uint32_t nrf_pdm_clk_pin_get(NRF_PDM_Type const * p_reg)
{
    return p_reg->PSEL.CLK;
}

NRF_STATIC_INLINE uint32_t nrf_pdm_din_pin_get(NRF_PDM_Type const * p_reg)
{
    return p_reg->PSEL.DIN;
}

NRF_STATIC_INLINE void nrf_pdm_psel_disconnect(NRF_PDM_Type * p_reg)
{
    p_reg->PSEL.CLK = ((PDM_PSEL_CLK_CONNECT_Disconnected << PDM_PSEL_CLK_CONNECT_Pos)
                         & PDM_PSEL_CLK_CONNECT_Msk);
    p_reg->PSEL.DIN = ((PDM_PSEL_DIN_CONNECT_Disconnected << PDM_PSEL_DIN_CONNECT_Pos)
                         & PDM_PSEL_DIN_CONNECT_Msk);
}

NRF_STATIC_INLINE void nrf_pdm_gain_set(NRF_PDM_Type * p_reg,
                                        nrf_pdm_gain_t gain_l,
                                        nrf_pdm_gain_t gain_r)
{
    p_reg->GAINL = gain_l;
    p_reg->GAINR = gain_r;
}

NRF_STATIC_INLINE void nrf_pdm_gain_get(NRF_PDM_Type const * p_reg,
                                        nrf_pdm_gain_t *     p_gain_l,
                                        nrf_pdm_gain_t *     p_gain_r)
{
    *p_gain_l = (nrf_pdm_gain_t)p_reg->GAINL;
    *p_gain_r = (nrf_pdm_gain_t)p_reg->GAINR;
}

NRF_STATIC_INLINE void nrf_pdm_buffer_set(NRF_PDM_Type * p_reg, uint32_t * p_buffer, uint32_t num)
{
    p_reg->SAMPLE.PTR = (uint32_t)p_buffer;
#if defined(DMA_BUFFER_UNIFIED_BYTE_ACCESS)
    p_reg->SAMPLE.MAXCNT = num * sizeof(int16_t);
#else
    p_reg->SAMPLE.MAXCNT = num;
#endif
}

NRF_STATIC_INLINE uint32_t * nrf_pdm_buffer_get(NRF_PDM_Type const * p_reg)
{
    return (uint32_t *)p_reg->SAMPLE.PTR;
}

#if NRF_PDM_HAS_RATIO_CONFIG
NRF_STATIC_INLINE void nrf_pdm_ratio_set(NRF_PDM_Type * p_reg, nrf_pdm_ratio_t ratio)
{
    p_reg->RATIO = ratio;
}

#if NRF_PDM_HAS_CUSTOM_RATIO
NRF_STATIC_INLINE void nrf_pdm_custom_ratio_set(NRF_PDM_Type * p_reg, uint8_t ratio)
{
    NRFX_ASSERT(ratio >= PDM_FILTER_CTRL_DECRATIO_Min);
    NRFX_ASSERT(ratio <= PDM_FILTER_CTRL_DECRATIO_Max);
    p_reg->FILTER.CTRL = (p_reg->FILTER.CTRL & ~PDM_FILTER_CTRL_DECRATIO_Msk) |
                          (((uint32_t)ratio << PDM_FILTER_CTRL_DECRATIO_Pos)
                                             & PDM_FILTER_CTRL_DECRATIO_Msk);
}
#endif
#endif // NRF_PDM_HAS_RATIO_CONFIG

#if NRF_PDM_HAS_SELECTABLE_CLOCK
NRF_STATIC_INLINE void nrf_pdm_mclksrc_configure(NRF_PDM_Type * p_reg, nrf_pdm_mclksrc_t mclksrc)
{
#if NRF_PDM_HAS_MCLKCONFIG
    p_reg->MCLKCONFIG = mclksrc;
#elif NRF_PDM_HAS_CLKSELECT
    p_reg->CLKSELECT = mclksrc;
#endif
}
#endif

#if NRF_PDM_HAS_FILTER
NRF_STATIC_INLINE
void nrf_pdm_filter_ctrl_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_ctrl_t const * fctrl)
{
    NRFX_ASSERT(fctrl->gain_step_custom >= PDM_FILTER_CTRL_MINORSTEP050CUSTOM_Min);
    NRFX_ASSERT(fctrl->gain_step_custom <= PDM_FILTER_CTRL_MINORSTEP050CUSTOM_Max);
    NRFX_ASSERT(fctrl->soft_cycles >= PDM_FILTER_CTRL_SOFTCYCLES_Min);
    NRFX_ASSERT(fctrl->soft_cycles <= PDM_FILTER_CTRL_SOFTCYCLES_Max);
    p_reg->FILTER.CTRL = (((fctrl->r_mute ? PDM_FILTER_CTRL_OVERRIDERIGHTSOFTMUTE_Enable
                                          : PDM_FILTER_CTRL_OVERRIDERIGHTSOFTMUTE_Disable)
                                         << PDM_FILTER_CTRL_OVERRIDERIGHTSOFTMUTE_Pos) |
                ((fctrl->l_mute ? PDM_FILTER_CTRL_OVERRIDELEFTSOFTMUTE_Enable
                                : PDM_FILTER_CTRL_OVERRIDELEFTSOFTMUTE_Disable)
                                << PDM_FILTER_CTRL_OVERRIDELEFTSOFTMUTE_Pos) |
                ((fctrl->gain_step_add ? PDM_FILTER_CTRL_GAINADD0P25_Enable
                                       : PDM_FILTER_CTRL_GAINADD0P25_Disable)
                                       << PDM_FILTER_CTRL_GAINADD0P25_Pos) |
                ((fctrl->minor_gain_step_custom ? PDM_FILTER_CTRL_MINORSTEP025CUSTOM_Enable
                                                : PDM_FILTER_CTRL_MINORSTEP025CUSTOM_Disable)
                                                << PDM_FILTER_CTRL_MINORSTEP025CUSTOM_Pos) |
                ((fctrl->gain_step_custom << PDM_FILTER_CTRL_MINORSTEP050CUSTOM_Pos)
                                           & PDM_FILTER_CTRL_MINORSTEP050CUSTOM_Msk) |
                ((fctrl->soft_cycles << PDM_FILTER_CTRL_SOFTCYCLES_Pos)
                                      & PDM_FILTER_CTRL_SOFTCYCLES_Msk) |
                ((fctrl->sample_delay << PDM_FILTER_CTRL_DATASAMPLEDELAY_Pos)
                                       & PDM_FILTER_CTRL_DATASAMPLEDELAY_Msk) |
                ((fctrl->cic_filter << PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Pos)
                                     & PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Msk));
}

NRF_STATIC_INLINE
void nrf_pdm_filter_ctrl_get(NRF_PDM_Type const * p_reg, nrf_pdm_filter_ctrl_t * fctrl)
{
    fctrl->r_mute = (((p_reg->FILTER.CTRL & PDM_FILTER_CTRL_OVERRIDERIGHTSOFTMUTE_Msk)
                                         >> PDM_FILTER_CTRL_OVERRIDERIGHTSOFTMUTE_Pos)
                                         == PDM_FILTER_CTRL_OVERRIDERIGHTSOFTMUTE_Enable);
    fctrl->l_mute = (((p_reg->FILTER.CTRL & PDM_FILTER_CTRL_OVERRIDELEFTSOFTMUTE_Msk)
                                         >> PDM_FILTER_CTRL_OVERRIDELEFTSOFTMUTE_Pos)
                                         == PDM_FILTER_CTRL_OVERRIDELEFTSOFTMUTE_Enable);
    fctrl->gain_step_add = (((p_reg->FILTER.CTRL & PDM_FILTER_CTRL_GAINADD0P25_Msk)
                                                >> PDM_FILTER_CTRL_GAINADD0P25_Pos)
                                                == PDM_FILTER_CTRL_GAINADD0P25_Enable);
    fctrl->minor_gain_step_custom = (((p_reg->FILTER.CTRL & PDM_FILTER_CTRL_MINORSTEP025CUSTOM_Msk)
                                                         >> PDM_FILTER_CTRL_MINORSTEP025CUSTOM_Pos)
                                                         == PDM_FILTER_CTRL_MINORSTEP025CUSTOM_Enable);
    fctrl->gain_step_custom = ((p_reg->FILTER.CTRL & PDM_FILTER_CTRL_MINORSTEP050CUSTOM_Msk)
                                                  >> PDM_FILTER_CTRL_MINORSTEP050CUSTOM_Pos);
    fctrl->soft_cycles = ((p_reg->FILTER.CTRL & PDM_FILTER_CTRL_SOFTCYCLES_Msk)
                                             >> PDM_FILTER_CTRL_SOFTCYCLES_Pos);
    fctrl->sample_delay = (nrf_pdm_sample_delay_t)((p_reg->FILTER.CTRL
                                              & PDM_FILTER_CTRL_DATASAMPLEDELAY_Msk)
                                             >> PDM_FILTER_CTRL_DATASAMPLEDELAY_Pos);
    fctrl->cic_filter = (nrf_pdm_filter_cic_t)((p_reg->FILTER.CTRL
                                              & PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Msk)
                                             >> PDM_FILTER_CTRL_CICFILTERMSBCUSTOM_Pos);
}

NRF_STATIC_INLINE
void nrf_pdm_filter_hp_pole_enable(NRF_PDM_Type * p_reg, bool enable)
{
    p_reg->FILTER.HPDISABLE = enable ? PDM_FILTER_HPDISABLE_DISABLE_Enable
                                     : PDM_FILTER_HPDISABLE_DISABLE_Disable;
}

NRF_STATIC_INLINE
void nrf_pdm_filter_hp_pole_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_hp_pole_t hppole)
{
    p_reg->FILTER.HPPOLE = hppole;
}

NRF_STATIC_INLINE nrf_pdm_filter_hp_pole_t nrf_pdm_filter_hp_pole_get(NRF_PDM_Type const * p_reg)
{
    return (nrf_pdm_filter_hp_pole_t)p_reg->FILTER.HPPOLE;
}

NRF_STATIC_INLINE void nrf_pdm_filter_softmute_enable(NRF_PDM_Type * p_reg, bool enable)
{
    p_reg->FILTER.SOFTMUTE = enable ? PDM_FILTER_SOFTMUTE_ENABLE_Enabled
                                    : PDM_FILTER_SOFTMUTE_ENABLE_Disabled;
}

NRF_STATIC_INLINE
void nrf_pdm_filter_softcycles_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_softcycles_t cycles)
{
    p_reg->FILTER.SOFTCYCLES = cycles;
}

NRF_STATIC_INLINE
nrf_pdm_filter_softcycles_t nrf_pdm_filter_softcycles_get(NRF_PDM_Type const * p_reg)
{
    return (nrf_pdm_filter_softcycles_t)p_reg->FILTER.SOFTCYCLES;
}

NRF_STATIC_INLINE void nrf_pdm_filter_sampledelay_enable(NRF_PDM_Type * p_reg, bool enable)
{
    p_reg->FILTER.SAMPLEDELAY = enable ? PDM_FILTER_SAMPLEDELAY_DELAY_x1
                                       : PDM_FILTER_SAMPLEDELAY_DELAY_no_delay;
}
#endif

#endif // NRF_DECLARE_ONLY
/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_PDM_H_
