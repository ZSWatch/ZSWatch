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

#ifndef NRF_AUXPLL_H__
#define NRF_AUXPLL_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_auxpll_hal Auxiliary PLL HAL
 * @{
 * @ingroup nrf_auxpll
 * @ingroup nrf_clock
 * @brief   Hardware access layer for managing the Auxiliary Phase Locked Loop (AUXPLL) peripheral.
 */

/** @brief AUXPLL tasks. */
typedef enum
{
    NRF_AUXPLL_TASK_START          = offsetof(NRF_AUXPLL_Type, TASKS_START),        /**< Start the AUXPLL. */
    NRF_AUXPLL_TASK_STOP           = offsetof(NRF_AUXPLL_Type, TASKS_STOP),         /**< Stop the AUXPLL. */
    NRF_AUXPLL_TASK_FREQ_NEW_FINE  = offsetof(NRF_AUXPLL_Type, TASKS_NEWFINEFREQ),  /**< Change fine frequency. */
    NRF_AUXPLL_TASK_FREQ_NEW_BASE  = offsetof(NRF_AUXPLL_Type, TASKS_NEWBASEFREQ),  /**< Change base frequency. */
    NRF_AUXPLL_TASK_FREQ_INC_START = offsetof(NRF_AUXPLL_Type, TASKS_FREQINCSTART), /**< Start automated frequency increment. */
    NRF_AUXPLL_TASK_FREQ_INC_STOP  = offsetof(NRF_AUXPLL_Type, TASKS_FREQINCSTOP),  /**< Stop automated frequency increment. */
} nrf_auxpll_task_t;

/** @brief AUXPLL events. */
typedef enum
{
    NRF_AUXPLL_EVENT_STARTED = offsetof(NRF_AUXPLL_Type, EVENTS_STARTED), /**< Event indicating that AUXPLL started. */
    NRF_AUXPLL_EVENT_STOPPED = offsetof(NRF_AUXPLL_Type, EVENTS_STOPPED), /**< Event indicating that AUXPLL stopped. */
    NRF_AUXPLL_EVENT_LOCKED  = offsetof(NRF_AUXPLL_Type, EVENTS_LOCKED),  /**< Event indicating that AUXPLL locked. */
} nrf_auxpll_event_t;

/** @brief AUXPLL interrupts. */
typedef enum
{
    NRF_AUXPLL_INT_STARTED_MASK = AUXPLL_INTEN_STARTED_Msk, /**< AUXPLL interrupt for STARTED event. */
    NRF_AUXPLL_INT_STOPPED_MASK = AUXPLL_INTEN_STOPPED_Msk, /**< AUXPLL interrupt for STOPPED event. */
    NRF_AUXPLL_INT_LOCKED_MASK  = AUXPLL_INTEN_LOCKED_Msk   /**< AUXPLL interrupt for LOCKED event. */
} nrf_auxpll_int_mask_t;

/** @brief AUXPLL STATUS register bit masks. */
typedef enum
{
    NRF_AUXPLL_STATUS_MODE_MASK             = AUXPLL_STATUS_MODE_Msk,           /**< AUXPLL mode indication. 1 - Locked mode, 0 - Freerunning mode. */
    NRF_AUXPLL_STATUS_PLL_RUNNING_MASK      = AUXPLL_STATUS_PLLRUNNING_Msk,     /**< AUXPLL running indication. 1 - PLL running, 0 - PLL not running. */
    MRF_AUXPLL_STATUS_FREQUENCY_ACTUAL_MASK = AUXPLL_STATUS_FREQUENCYACTUAL_Msk /**< Actual fractional PLL divider ratio. */
} nrf_auxpll_status_mask_t;

/** @brief AUXPLL output prescaler ratio. */
typedef enum
{
    NRF_AUXPLL_CTRL_OUTSEL_DIV_DISABLED = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_DivDisabled, /**< Divider disabled. Bypassed external clock still supported. */
    NRF_AUXPLL_CTRL_OUTSEL_DIV_1        = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Div1,        /**< Divide by 1 */
    NRF_AUXPLL_CTRL_OUTSEL_DIV_2        = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Div2,        /**< Divide by 2 */
    NRF_AUXPLL_CTRL_OUTSEL_DIV_3        = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Div3,        /**< Divide by 3 */
    NRF_AUXPLL_CTRL_OUTSEL_DIV_4        = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Div4,        /**< Divide by 4 */
    NRF_AUXPLL_CTRL_OUTSEL_DIV_6        = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Div6,        /**< Divide by 6 */
    NRF_AUXPLL_CTRL_OUTSEL_DIV_8        = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Div8,        /**< Divide by 8 */
    NRF_AUXPLL_CTRL_OUTSEL_DIV_12       = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Div12,       /**< Divide by 12 */
    NRF_AUXPLL_CTRL_OUTSEL_DIV_16       = AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Div16,       /**< Divide by 16 */
} nrf_auxpll_ctrl_outsel_t;

/** @brief AUXPLL freerunning mode control. */
typedef enum
{
    NRF_AUXPLL_CTRL_MODE_AUTO    = AUXPLL_AUXPLLCTRL_MODE_MODECTRL_Auto,    /**< Automatically handled by the AUXPLL peripheral. */
    NRF_AUXPLL_CTRL_MODE_FREERUN = AUXPLL_AUXPLLCTRL_MODE_MODECTRL_Freerun, /**< Keep AUXPLL in freerunning mode. */
    NRF_AUXPLL_CTRL_MODE_LOCKED  = AUXPLL_AUXPLLCTRL_MODE_MODECTRL_Locked   /**< Keep AUXPLL in locked mode. */
} nrf_auxpll_ctrl_mode_t;

/** @brief AUXPLL Loop divider base settings. */
typedef enum
{
    NRF_AUXPLL_DIVIDER_RANGE_LOW  = AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_Low,        /**< Low range divider setting. Fractional divider in the range 3..4. */
    NRF_AUXPLL_DIVIDER_RANGE_MID  = AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_Mid,        /**< Mid range divider setting. Fractional divider in the range 4..5. */
    NRF_AUXPLL_DIVIDER_RANGE_HIGH = AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_High,       /**< High range divider setting. Fractional divider in the range 5..6. */
    NRF_AUXPLL_DIVIDER_RANGE_MAX  = AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_StaticHigh, /**< Maximum static divider setting. Fractional division not supported. */
} nrf_auxpll_divider_range_t;

/** @brief AUXPLL fractional PLL Divider ratio. */
typedef enum
{
    NRF_AUXPLL_FREQUENCY_DIV_MIN     = AUXPLL_AUXPLLCTRL_FREQUENCY_FREQUENCY_MinimumDiv, /**< Minimum division ratio of 4. */
    NRF_AUXPLL_FREQUENCY_AUDIO_44K1  = AUXPLL_AUXPLLCTRL_FREQUENCY_FREQUENCY_Audio44k1,  /**< Division ratio for audio 44.1kHz frequency family. */
    NRF_AUXPLL_FREQUENCY_USB_24M     = AUXPLL_AUXPLLCTRL_FREQUENCY_FREQUENCY_USB24M,     /**< Division ratio for USB PHY 24MHz clock. */
    NRF_AUXPLL_FREQUENCY_AUDIO_48K   = AUXPLL_AUXPLLCTRL_FREQUENCY_FREQUENCY_Audio48k,   /**< Division ratio for 48kHz frequency family. */
    NRF_AUXPLL_FREQUENCY_DIV_MAX     = AUXPLL_AUXPLLCTRL_FREQUENCY_FREQUENCY_MaximumDiv, /**< Maximum division ratio of 5. */
} nrf_auxpll_freq_div_ratio_t;

/** @brief AUXPLL configuration. */
typedef struct
{
    uint8_t outdrive;                 /**< Output buffer drive strength selection. Range 0..3. */
    uint8_t current_tune;             /**< Constant current tune for ring oscillator. Range 0..15. */
    bool sdm_off;                     /**< Turn off sigma delta modulation. */
    bool dither_off;                  /**< Turn off dither in sigma delta modulator. */
    nrf_auxpll_divider_range_t range; /**< Loop divider base settings. */
} nrf_auxpll_config_t;

/**
 * @brief Function for activating the specified AUXPLL task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_auxpll_task_trigger(NRF_AUXPLL_Type * p_reg,
                                               nrf_auxpll_task_t task);

/**
 * @brief Function for getting the address of the specified AUXPLL task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  The specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_auxpll_task_address_get(NRF_AUXPLL_Type const * p_reg,
                                                       nrf_auxpll_task_t       task);

/**
 * @brief Function for clearing the specified AUXPLL event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_auxpll_event_clear(NRF_AUXPLL_Type *  p_reg,
                                              nrf_auxpll_event_t event);

/**
 * @brief Function for retrieving the state of the AUXPLL event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_auxpll_event_check(NRF_AUXPLL_Type const * p_reg,
                                              nrf_auxpll_event_t      event);

/**
 * @brief Function for getting the address of the specified AUXPLL event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event The specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_auxpll_event_address_get(NRF_AUXPLL_Type const * p_reg,
                                                        nrf_auxpll_event_t      event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_auxpll_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_auxpll_int_enable(NRF_AUXPLL_Type * p_reg,
                                             uint32_t          mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_auxpll_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_auxpll_int_disable(NRF_AUXPLL_Type * p_reg,
                                              uint32_t          mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_auxpll_int_mask_t values for bit masking.
 *
 * @return true  requested interrupts are enabled.
 * @return false requested interrupts are disabled.
 */
NRF_STATIC_INLINE bool nrf_auxpll_int_enable_check(NRF_AUXPLL_Type const * p_reg,
                                                   uint32_t                mask);

/**
 * @brief Function for checking if the specified interrupts are pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_auxpll_int_mask_t values for bit masking.
 *
 * @return true  requested interrupts are pending.
 * @return false requested interrupts are not pending.
 */
NRF_STATIC_INLINE bool nrf_auxpll_int_pending_check(NRF_AUXPLL_Type const * p_reg,
                                                    uint32_t                mask);

/**
 * @brief Function for getting AUXPLL status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The AUXPLL STATUS register value. Use @ref nrf_auxpll_status_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_auxpll_status_get(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Function for getting the AUXPLL configuration.
 *
 * @param[in]  p_reg Pointer to the structure of registers of the peripheral.
 * @param[out] p_cfg Pointer to the structure to be filled with current AUXPLL configuration.
 */
NRF_STATIC_INLINE void nrf_auxpll_config_get(NRF_AUXPLL_Type const * p_reg,
                                             nrf_auxpll_config_t   * p_cfg);

/**
 * @brief Function for setting the AUXPLL configuration.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_cfg Pointer to the structure with AUXPLL configuration.
 */
NRF_STATIC_INLINE void nrf_auxpll_config_set(NRF_AUXPLL_Type *           p_reg,
                                             nrf_auxpll_config_t const * p_cfg);

/**
 * @brief Function for setting the AUXPLL ring oscillator core process corner tuning.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value tuning frequency value.
 */
NRF_STATIC_INLINE void nrf_auxpll_trim_ctune_set(NRF_AUXPLL_Type * p_reg,
                                                 uint8_t           value);

/**
 * @brief Function for getting the AUXPLL ring oscillator core process corner tuning.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The AUXPLL ring oscillator core process corner tuning value.
 */
NRF_STATIC_INLINE uint8_t nrf_auxpll_trim_ctune_get(NRF_AUXPLL_Type const * p_reg);


/**
 * @brief Function for setting the AUXPLL fractional PLL divider ratio tuning.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value Fractional divider ratio.
 */
NRF_STATIC_INLINE void nrf_auxpll_ctrl_frequency_set(NRF_AUXPLL_Type *           p_reg,
                                                     nrf_auxpll_freq_div_ratio_t value);

/**
 * @brief Function for getting the AUXPLL fractional PLL divider ratio.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Fractional divider ratio.
 */
NRF_STATIC_INLINE uint16_t nrf_auxpll_ctrl_frequency_get(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Function for setting the AUXPLL frequency increment value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value Signed 8-bit frequency increment, applied to current value of FREQUENCY register.
 */
NRF_STATIC_INLINE void nrf_auxpll_ctrl_freqinc_set(NRF_AUXPLL_Type * p_reg,
                                                   int8_t            value);

/**
 * @brief Function for getting the AUXPLL frequency increment value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Signed 8-bit frequency increment.
 */
NRF_STATIC_INLINE int8_t nrf_auxpll_ctrl_freqinc_get(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Function for setting the AUXPLL frequency increment period.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value Frequency increment period in 1 us steps.
 */
NRF_STATIC_INLINE void nrf_auxpll_ctrl_freqinc_period_set(NRF_AUXPLL_Type * p_reg,
                                                          uint16_t          value);

/**
 * @brief Function for getting the AUXPLL frequency increment period value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Frequency increment period.
 */
NRF_STATIC_INLINE uint16_t nrf_auxpll_ctrl_freqinc_period_get(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Function for setting the AUXPLL output prescaler.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value Prescaler ratio.
 */
NRF_STATIC_INLINE void nrf_auxpll_ctrl_outsel_set(NRF_AUXPLL_Type *        p_reg,
                                                  nrf_auxpll_ctrl_outsel_t value);

/**
 * @brief Function for getting the AUXPLL output prescaler value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Prescaler ratio.
 */
NRF_STATIC_INLINE nrf_auxpll_ctrl_outsel_t nrf_auxpll_ctrl_outsel_get(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Function for setting the AUXPLL mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] value AUXPLL running mode.
 */
NRF_STATIC_INLINE void nrf_auxpll_ctrl_mode_set(NRF_AUXPLL_Type *      p_reg,
                                                nrf_auxpll_ctrl_mode_t value);

/**
 * @brief Function for getting the AUXPLL mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return AUXPLL running mode.
 */
NRF_STATIC_INLINE nrf_auxpll_ctrl_mode_t nrf_auxpll_ctrl_mode_get(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Enable LOCK for mirrored AUXPLL registers.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_auxpll_lock(NRF_AUXPLL_Type * p_reg);

/**
 * @brief Disable the lock after configuring all AUXPLL mirrored registers.
 *
 * @details The individual mirrored registers can be updated any time when the lock is disabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_auxpll_unlock(NRF_AUXPLL_Type * p_reg);

/**
 * @brief Check if mirrored AUXPLL registers are locked.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The AUXPLL mirrored register lock enabled.
 * @retval false The AUXPLL mirrored register lock disabled.
 */
NRF_STATIC_INLINE bool nrf_auxpll_lock_check(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Obtain static ratio when DSM is disabled.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Static ratio value.
 */
NRF_STATIC_INLINE uint8_t nrf_auxpll_static_ratio_get(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Check if AUXPLL is locked.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The AUXPLL is locked.
 * @retval false The AUXPLL is not locked.
 */
NRF_STATIC_INLINE bool nrf_auxpll_mode_locked_check(NRF_AUXPLL_Type const * p_reg);

/**
 * @brief Check if AUXPLL is running.
 *
 * @param p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The AUXPLL is running.
 * @retval false The AUXPLL is not running.
 */
NRF_STATIC_INLINE bool nrf_auxpll_running_check(NRF_AUXPLL_Type const * p_reg);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_auxpll_task_trigger(NRF_AUXPLL_Type * p_reg,
                                               nrf_auxpll_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_auxpll_task_address_get(NRF_AUXPLL_Type const * p_reg,
                                                       nrf_auxpll_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_auxpll_event_clear(NRF_AUXPLL_Type *  p_reg,
                                              nrf_auxpll_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_auxpll_event_check(NRF_AUXPLL_Type const * p_reg,
                                              nrf_auxpll_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_auxpll_event_address_get(NRF_AUXPLL_Type const * p_reg,
                                                        nrf_auxpll_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_auxpll_int_enable(NRF_AUXPLL_Type * p_reg,
                                             uint32_t          mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_auxpll_int_disable(NRF_AUXPLL_Type * p_reg,
                                              uint32_t          mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE bool nrf_auxpll_int_enable_check(NRF_AUXPLL_Type const * p_reg,
                                                   uint32_t                mask)
{
    return p_reg->INTEN & mask;
}

NRF_STATIC_INLINE bool nrf_auxpll_int_pending_check(NRF_AUXPLL_Type const * p_reg,
                                                   uint32_t                 mask)
{
    return p_reg->INTPEND & mask;
}

NRF_STATIC_INLINE uint32_t nrf_auxpll_status_get(NRF_AUXPLL_Type const * p_reg)
{
    return p_reg->STATUS;
}

NRF_STATIC_INLINE void nrf_auxpll_config_get(NRF_AUXPLL_Type const * p_reg,
                                             nrf_auxpll_config_t   * p_cfg)
{
    NRFX_ASSERT(p_cfg);
    uint32_t reg = p_reg->CONFIG.CFGSTATIC;

    p_cfg->outdrive =
        (reg & AUXPLL_CONFIG_CFGSTATIC_OUTDRIVE_Msk) >> AUXPLL_CONFIG_CFGSTATIC_OUTDRIVE_Pos;

    p_cfg->current_tune =
        (reg & AUXPLL_CONFIG_CFGSTATIC_SELCONSTANTI_Msk) >> AUXPLL_CONFIG_CFGSTATIC_SELCONSTANTI_Pos;

    p_cfg->sdm_off =
        (reg & AUXPLL_CONFIG_CFGSTATIC_SDMOFF_Msk) >> AUXPLL_CONFIG_CFGSTATIC_SDMOFF_Pos;

    p_cfg->dither_off =
        (reg & AUXPLL_CONFIG_CFGSTATIC_SDMDITHEROFF_Msk) >> AUXPLL_CONFIG_CFGSTATIC_SDMDITHEROFF_Pos;

    p_cfg->range = (nrf_auxpll_divider_range_t)((reg & AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_Msk)
                                      >> AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_Pos);
}

NRF_STATIC_INLINE void nrf_auxpll_config_set(NRF_AUXPLL_Type           * p_reg,
                                             nrf_auxpll_config_t const * p_cfg)
{
    NRFX_ASSERT(p_cfg);

    p_reg->CONFIG.CFGSTATIC =
                    ((p_cfg->outdrive << AUXPLL_CONFIG_CFGSTATIC_OUTDRIVE_Pos)
                      & AUXPLL_CONFIG_CFGSTATIC_OUTDRIVE_Msk)
                  | ((p_cfg->current_tune << AUXPLL_CONFIG_CFGSTATIC_SELCONSTANTI_Pos)
                      & AUXPLL_CONFIG_CFGSTATIC_SELCONSTANTI_Msk)
                  | ((p_cfg->sdm_off << AUXPLL_CONFIG_CFGSTATIC_SDMOFF_Pos)
                      & AUXPLL_CONFIG_CFGSTATIC_SDMOFF_Msk)
                  | ((p_cfg->dither_off << AUXPLL_CONFIG_CFGSTATIC_SDMDITHEROFF_Pos)
                      & AUXPLL_CONFIG_CFGSTATIC_SDMDITHEROFF_Msk)
                  | ((p_cfg->range << AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_Pos)
                      & AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_Msk);
}

NRF_STATIC_INLINE void nrf_auxpll_trim_ctune_set(NRF_AUXPLL_Type * p_reg,
                                                 uint8_t           value)
{
    p_reg->TRIM.CTUNE = value;
}

NRF_STATIC_INLINE uint8_t nrf_auxpll_trim_ctune_get(NRF_AUXPLL_Type const * p_reg)
{
    return (uint8_t)p_reg->TRIM.CTUNE;
}

NRF_STATIC_INLINE void nrf_auxpll_ctrl_frequency_set(NRF_AUXPLL_Type *           p_reg,
                                                     nrf_auxpll_freq_div_ratio_t value)
{
    p_reg->AUXPLLCTRL.FREQUENCY = value;
}

NRF_STATIC_INLINE uint16_t nrf_auxpll_ctrl_frequency_get(NRF_AUXPLL_Type const * p_reg)
{
    return (uint16_t)p_reg->AUXPLLCTRL.FREQUENCY;
}

NRF_STATIC_INLINE void nrf_auxpll_ctrl_freqinc_set(NRF_AUXPLL_Type * p_reg,
                                                   int8_t            value)
{
    p_reg->AUXPLLCTRL.FREQINC = (uint8_t)value;
}

NRF_STATIC_INLINE int8_t nrf_auxpll_ctrl_freqinc_get(NRF_AUXPLL_Type const * p_reg)
{
    return (int8_t)p_reg->AUXPLLCTRL.FREQINC;
}

NRF_STATIC_INLINE void nrf_auxpll_ctrl_freqinc_period_set(NRF_AUXPLL_Type * p_reg,
                                                         uint16_t          value)
{
    p_reg->AUXPLLCTRL.FREQINCPERIOD = value;
}

NRF_STATIC_INLINE uint16_t nrf_auxpll_ctrl_freqinc_period_get(NRF_AUXPLL_Type const * p_reg)
{
    return (uint16_t)p_reg->AUXPLLCTRL.FREQINCPERIOD;
}

NRF_STATIC_INLINE void nrf_auxpll_ctrl_outsel_set(NRF_AUXPLL_Type *        p_reg,
                                                  nrf_auxpll_ctrl_outsel_t value)
{
    p_reg->AUXPLLCTRL.OUTSEL = (value << AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Pos);
}

NRF_STATIC_INLINE nrf_auxpll_ctrl_outsel_t nrf_auxpll_ctrl_outsel_get(NRF_AUXPLL_Type const * p_reg)
{
    return (nrf_auxpll_ctrl_outsel_t)((p_reg->AUXPLLCTRL.OUTSEL & AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Msk) >>
                                       AUXPLL_AUXPLLCTRL_OUTSEL_OUTSEL_Pos);
}

NRF_STATIC_INLINE void nrf_auxpll_ctrl_mode_set(NRF_AUXPLL_Type *      p_reg,
                                                nrf_auxpll_ctrl_mode_t value)
{
    p_reg->AUXPLLCTRL.MODE = value << AUXPLL_AUXPLLCTRL_MODE_MODECTRL_Pos;
}

NRF_STATIC_INLINE nrf_auxpll_ctrl_mode_t nrf_auxpll_ctrl_mode_get(NRF_AUXPLL_Type const * p_reg)
{
    uint8_t val = (p_reg->AUXPLLCTRL.MODE & AUXPLL_AUXPLLCTRL_MODE_MODECTRL_Msk) >>
                   AUXPLL_AUXPLLCTRL_MODE_MODECTRL_Pos;

    return (nrf_auxpll_ctrl_mode_t)val;
}

NRF_STATIC_INLINE void nrf_auxpll_lock(NRF_AUXPLL_Type * p_reg)
{
    p_reg->MIRROR = AUXPLL_MIRROR_LOCK_Enabled;
}

NRF_STATIC_INLINE void nrf_auxpll_unlock(NRF_AUXPLL_Type * p_reg)
{
    p_reg->MIRROR = AUXPLL_MIRROR_LOCK_Disabled;
}

NRF_STATIC_INLINE bool nrf_auxpll_lock_check(NRF_AUXPLL_Type const * p_reg)
{
    return ((p_reg->MIRROR) & AUXPLL_MIRROR_LOCK_Enabled);
}

NRF_STATIC_INLINE uint8_t nrf_auxpll_static_ratio_get(NRF_AUXPLL_Type const * p_reg)
{
    return ((p_reg->CONFIG.CFGSTATIC & AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_Msk) >>
            AUXPLL_CONFIG_CFGSTATIC_AUXPLLRANGE_Pos) + 3U;
}

NRF_STATIC_INLINE bool nrf_auxpll_mode_locked_check(NRF_AUXPLL_Type const * p_reg)
{
    return (p_reg->STATUS & AUXPLL_STATUS_MODE_Msk) ==
           (AUXPLL_STATUS_MODE_Locked << AUXPLL_STATUS_MODE_Pos);
}

NRF_STATIC_INLINE bool nrf_auxpll_running_check(NRF_AUXPLL_Type const * p_reg)
{
    return (p_reg->STATUS & AUXPLL_STATUS_PLLRUNNING_Msk) ==
           (AUXPLL_STATUS_PLLRUNNING_Running << AUXPLL_STATUS_PLLRUNNING_Pos);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // NRF_AUXPLL_H__
