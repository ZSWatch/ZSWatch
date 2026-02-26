/*
 * Copyright (c) 2024 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_CRACEN_RNG_H__
#define NRF_CRACEN_RNG_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_cracen_rng_hal CRACEN RNG HAL
 * @{
 * @ingroup nrf_cracen
 * @brief   Hardware access layer for managing the Crypto Accelerator Engine (CRACEN)
 *          Random Generator (RNG) peripheral.
 */

#if defined(CRACENCORE_RNGCONTROL_SWOFFTMRVAL_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the TRNG FSM has an idle timer */
#define NRF_CRACEN_RNG_HAS_IDLE_TIMER 1
#else
#define NRF_CRACEN_RNG_HAS_IDLE_TIMER 0
#endif

#if defined(CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the TRNG has entropy blending */
#define NRF_CRACEN_RNG_HAS_BLENDING 1
#else
#define NRF_CRACEN_RNG_HAS_BLENDING 0
#endif

#if NRF_CRACEN_RNG_HAS_BLENDING
/** @brief CRACEN entropy blending methods */
typedef enum
{
  NRF_CRACEN_RNG_BLENDING_CONCATENATION = CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_CONCATENATION, /**< Collate all rings oscillators outputs */
  NRF_CRACEN_RNG_BLENDING_XOR_LEVEL_1   = CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_XORLEVEL1,     /**< XOR bits inside each ring oscillator set */
  NRF_CRACEN_RNG_BLENDING_XOR_LEVEL_2   = CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_XORLEVEL2,     /**< Also XOR bits in-between ring oscillator sets */
  NRF_CRACEN_RNG_BLENDING_VONNEUMANN    = CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_VONNEUMANN,    /**< On top of XOR_LEVEL_2 apply VON-NEUMANN debiasing */
} nrf_cracen_rng_blending_t;
#endif

/** @brief CRACEN random generator configuration */
typedef struct
{
    bool                      enable;            /**< Enable the RNG peripheral */
    bool                      fifo_full_int_en;  /**< Enable FIFO full interrupt */
    bool                      soft_reset;        /**< Soft reset the RNG peripheral */
    uint8_t                   number_128_blocks; /**< Number of 128bit blocks used for AES conditioning. Must be at least 1 */
#if NRF_CRACEN_RNG_HAS_BLENDING
    nrf_cracen_rng_blending_t blending_method;   /**< Which blending method to use */
#endif
} nrf_cracen_rng_control_t;

/** @brief CRACEN random generator FSM state */
typedef enum
{
    NRF_CRACEN_RNG_FSM_STATE_RESET        = CRACENCORE_RNGCONTROL_STATUS_STATE_RESET,    /**< RNG is not started */
    NRF_CRACEN_RNG_FSM_STATE_STARTUP      = CRACENCORE_RNGCONTROL_STATUS_STATE_STARTUP,  /**< RNG is starting */
    NRF_CRACEN_RNG_FSM_STATE_IDLE_READY   = CRACENCORE_RNGCONTROL_STATUS_STATE_IDLERON,  /**< RNG is idle, and ready to produce more data */
#if NRF_CRACEN_RNG_HAS_IDLE_TIMER
    NRF_CRACEN_RNG_FSM_STATE_IDLE_STANDBY = CRACENCORE_RNGCONTROL_STATUS_STATE_IDLEROFF, /**< RNG is idle, with the ring oscillators off */
#endif
    NRF_CRACEN_RNG_FSM_STATE_FILL_FIFO    = CRACENCORE_RNGCONTROL_STATUS_STATE_FILLFIFO, /**< RNG is filling the FIFO with entropy */
    NRF_CRACEN_RNG_FSM_STATE_ERROR        = CRACENCORE_RNGCONTROL_STATUS_STATE_ERROR,    /**< RNG has halted on an error. Reset is needed */
} nrf_cracen_rng_fsm_state_t;

/** Size of the RNG FIFO in bytes */
#define NRF_CRACEN_RNG_FIFO_SIZE ((CRACENCORE_RNGCONTROL_FIFOTHRESHOLD_ResetValue + 1) * 16)

/**
 * @brief Function for setting the control register
 *
 * @param[in] p_reg Pointer to the structure of registers of the RNG peripheral.
 * @param[in] p_config Configuration to be written in the register.
 */
NRF_STATIC_INLINE void nrf_cracen_rng_control_set(NRF_CRACENCORE_Type *            p_reg,
                                                  nrf_cracen_rng_control_t const * p_config);

/**
 * @brief Function for getting the FIFO level
 *
 * @param[in] p_reg Pointer to the structure of registers of the RNG peripheral.
 *
 * @return Number of random data words ready in the FIFO
 */
NRF_STATIC_INLINE uint32_t nrf_cracen_rng_fifo_level_get(NRF_CRACENCORE_Type const * p_reg);

/**
 * @brief Function for setting the AES conditioning KEY registers
 *
 * @param[in] p_reg Pointer to the structure of registers of the RNG peripheral.
 * @param[in] index Index of the key register to be written (0..3)
 * @param[in] value Value to be written in the register.
 */
NRF_STATIC_INLINE void nrf_cracen_rng_key_set(NRF_CRACENCORE_Type * p_reg,
                                              uint8_t               index,
                                              uint32_t              value);

/**
 * @brief Function for getting the RNG FSM state
 *
 * @param[in] p_reg Pointer to the structure of registers of the RNG peripheral.
 *
 * @return State of the FSM, one of @ref nrf_cracen_rng_fsm_state_t values.
 */
NRF_STATIC_INLINE
nrf_cracen_rng_fsm_state_t nrf_cracen_rng_fsm_state_get(NRF_CRACENCORE_Type const * p_reg);

/**
 * @brief Function for setting the initialization wait counter value
 *
 * @param[in] p_reg Pointer to the structure of registers of the RNG peripheral.
 * @param[in] value Value to be written in the register.
 */
NRF_STATIC_INLINE void nrf_cracen_rng_init_wait_val_set(NRF_CRACENCORE_Type * p_reg,
                                                        uint16_t              value);

#if NRF_CRACEN_RNG_HAS_IDLE_TIMER
/**
 * @brief Function for setting the switch off timer value
 *
 * @param[in] p_reg Pointer to the structure of registers of the RNG peripheral.
 * @param[in] value Value to be written in the register.
 */
NRF_STATIC_INLINE void nrf_cracen_rng_off_timer_set(NRF_CRACENCORE_Type * p_reg,
                                                    uint16_t              value);
#endif

/**
 * @brief Function for setting the entropy subsampling rate register
 *
 * @note The ring oscillators output is sampled at Fs=Fpclk/(ClkDiv+1)
 *
 * @param[in] p_reg Pointer to the structure of registers of the RNG peripheral.
 * @param[in] value Value to be written in the register.
 */
NRF_STATIC_INLINE void nrf_cracen_rng_clk_div_set(NRF_CRACENCORE_Type * p_reg,
                                                  uint16_t              value);

/**
 * @brief Function for getting a word from the entropy FIFO
 *
 * @note The caller must ensure there is enough data by calling
 *       @ref nrf_cracen_rng_fifo_level_get
 *
 * @param[in] p_reg Pointer to the structure of registers of the RNG peripheral
 *
 * @return Entropy word read from the FIFO
 */
NRF_STATIC_INLINE uint32_t nrf_cracen_rng_fifo_get(NRF_CRACENCORE_Type const * p_reg);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_cracen_rng_control_set(NRF_CRACENCORE_Type *            p_reg,
                                                  nrf_cracen_rng_control_t const * p_config)
{
    p_reg->RNGCONTROL.CONTROL =
          ((p_config->enable << CRACENCORE_RNGCONTROL_CONTROL_ENABLE_Pos)
           & CRACENCORE_RNGCONTROL_CONTROL_ENABLE_Msk)
        | ((p_config->fifo_full_int_en << CRACENCORE_RNGCONTROL_CONTROL_INTENFULL_Pos)
            & CRACENCORE_RNGCONTROL_CONTROL_INTENFULL_Msk)
        | ((p_config->soft_reset << CRACENCORE_RNGCONTROL_CONTROL_SOFTRST_Pos)
            & CRACENCORE_RNGCONTROL_CONTROL_SOFTRST_Msk)
#if NRF_CRACEN_RNG_HAS_BLENDING
        | ((p_config->blending_method << CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_Pos)
            & CRACENCORE_RNGCONTROL_CONTROL_BLENDINGMETHOD_Msk)
#endif
        | ((p_config->number_128_blocks << CRACENCORE_RNGCONTROL_CONTROL_NB128BITBLOCKS_Pos)
            & CRACENCORE_RNGCONTROL_CONTROL_NB128BITBLOCKS_Msk);
}

NRF_STATIC_INLINE uint32_t nrf_cracen_rng_fifo_level_get(NRF_CRACENCORE_Type const * p_reg)
{
    return p_reg->RNGCONTROL.FIFOLEVEL;
}

NRF_STATIC_INLINE void nrf_cracen_rng_key_set(NRF_CRACENCORE_Type * p_reg,
                                              uint8_t index, uint32_t value)
{
    p_reg->RNGCONTROL.KEY[index] = value;
}

NRF_STATIC_INLINE
nrf_cracen_rng_fsm_state_t nrf_cracen_rng_fsm_state_get(NRF_CRACENCORE_Type const * p_reg)
{
    return (nrf_cracen_rng_fsm_state_t)
           ((p_reg->RNGCONTROL.STATUS & CRACENCORE_RNGCONTROL_STATUS_STATE_Msk)
            >> CRACENCORE_RNGCONTROL_STATUS_STATE_Pos);
}

NRF_STATIC_INLINE void nrf_cracen_rng_init_wait_val_set(NRF_CRACENCORE_Type * p_reg,
                                                        uint16_t              value)
{
#if defined(CRACENCORE_RNGCONTROL_INITWAITVAL_ResetValue)
    p_reg->RNGCONTROL.INITWAITVAL = value;
#else
    p_reg->RNGCONTROL.WARMUPPERIOD = value;
#endif
}

#if NRF_CRACEN_RNG_HAS_IDLE_TIMER
NRF_STATIC_INLINE void nrf_cracen_rng_off_timer_set(NRF_CRACENCORE_Type * p_reg,
                                                    uint16_t value)
{
    p_reg->RNGCONTROL.SWOFFTMRVAL = value;
}
#endif

NRF_STATIC_INLINE void nrf_cracen_rng_clk_div_set(NRF_CRACENCORE_Type * p_reg,
                                                  uint16_t value)
{
#if defined(CRACENCORE_RNGCONTROL_CLKDIV_ResetValue)
    p_reg->RNGCONTROL.CLKDIV = value;
#else
    p_reg->RNGCONTROL.SAMPLINGPERIOD = value;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_cracen_rng_fifo_get(NRF_CRACENCORE_Type const * p_reg)
{
    return p_reg->RNGCONTROL.FIFO[0];
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_CRACEN_RNG_H__
