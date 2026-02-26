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

#ifndef NRF_LFXO_H__
#define NRF_LFXO_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_lfxo_hal LFXO HAL
 * @{
 * @ingroup nrf_clock
 * @brief   Hardware access layer for managing the Low Frequency Crystal Oscillator (LFXO).
 */

#if defined(LFXO_STATUSANA_READY_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the STATUSANA register is present. */
#define NRF_LFXO_HAS_STATUSANA 1
#else
#define NRF_LFXO_HAS_STATUSANA 0
#endif

/** @brief LFXO events. */
typedef enum
{
    NRF_LFXO_EVENT_MODECHANGED   = offsetof(NRF_LFXO_Type, EVENTS_MODECHANGED),   /**< LFXO mode changed. */
    NRF_LFXO_EVENT_ERRORSTARTING = offsetof(NRF_LFXO_Type, EVENTS_ERRORSTARTING), /**< Error while starting the LFXO in PIERCE mode. */
    NRF_LFXO_EVENT_ERRORRUNNING  = offsetof(NRF_LFXO_Type, EVENTS_ERRORRUNNING),  /**< Error detected while LFXO was running. */
} nrf_lfxo_event_t;

/** @brief LFXO interrupts. */
typedef enum
{
    NRF_LFXO_INT_MODECHANGED_MASK   = LFXO_INTENSET_MODECHANGED_Msk,   /**< Interrupt on MODECHANGED event. */
    NRF_LFXO_INT_ERRORSTARTING_MASK = LFXO_INTENSET_ERRORSTARTING_Msk, /**< Interrupt on ERRORSTARTING event. */
    NRF_LFXO_INT_ERRORRUNNING_MASK  = LFXO_INTENSET_ERRORRUNNING_Msk,  /**< Interrupt on ERRORRUNNING event. */
} nrf_lfxo_int_mask_t;

/** @brief LFXO modes. */
typedef enum
{
    NRF_LFXO_OSCILLATOR_MODE_PIERCE  = LFXO_STATUS_MODE_Pierce,        /**< Pierce mode. */
    NRF_LFXO_OSCILLATOR_MODE_PIXO    = LFXO_STATUS_MODE_Pixo,          /**< PIXO mode. */
    NRF_LFXO_OSCILLATOR_MODE_EXTSINE = LFXO_STATUS_MODE_ExternalSine,  /**< External sine wave clock. */
    NRF_LFXO_OSCILLATOR_MODE_EXTSQ   = LFXO_STATUS_MODE_ExternalSquare /**< External square wave clock. */
} nrf_lfxo_oscillator_mode_t;

/** @brief LFXO status. */
typedef struct
{
    nrf_lfxo_oscillator_mode_t oscmode; /**< Oscillator mode. */
    bool                       hpmode;  /**< High performance mode. */
    bool                       running; /**< LFXO running status. */
} nrf_lfxo_status_t;

/** @brief Amplitude control. */
typedef struct
{
    uint8_t interval;  /**< Amplitude check interval. */
    uint8_t step;      /**< Step to adjust the IDAC when the peak detector triggers. */
    uint8_t idac_init; /**< Initial IDAC code for the pierce oscillator. */
    bool    pdctrl_en; /**< Enable peak detector. */
} nrf_lfxo_amplitude_ctrl_t;

/** @brief Power up control. */
typedef enum
{
    NRF_LFXO_POWER_CONTROL_AUTO       = LFXO_PWRUPCTRL_CTRL_Auto,     /**< Automatically handled by the peripheral. */
    NRF_LFXO_POWER_CONTROL_POWER_UP   = LFXO_PWRUPCTRL_CTRL_PowerUp,  /**< Power up. */
    NRF_LFXO_POWER_CONTROL_POWER_DOWN = LFXO_PWRUPCTRL_CTRL_PowerDown /**< Power down. */
} nrf_lfxo_power_control_t;

/**
 * @brief Function for retrieving the address of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event LFXO Event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_lfxo_event_address_get(NRF_LFXO_Type const * p_reg,
                                                      nrf_lfxo_event_t      event);

/**
 * @brief Function for clearing the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_lfxo_event_clear(NRF_LFXO_Type * p_reg, nrf_lfxo_event_t event);

/**
 * @brief Function for retrieving the state of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_lfxo_event_check(NRF_LFXO_Type const * p_reg, nrf_lfxo_event_t event);

/**
 * @brief Function for enabling the specified interrupt.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_lfxo_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_lfxo_int_enable(NRF_LFXO_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupt.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_lfxo_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_lfxo_int_disable(NRF_LFXO_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_lfxo_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_lfxo_int_enable_check(NRF_LFXO_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * States of pending interrupt are saved as a bitmask.
 * One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_lfxo_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_lfxo_int_pending_get(NRF_LFXO_Type const * p_reg);

/**
 * @brief Function for getting LFXO status.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_status Pointer to the structure to be filled with LFXO status.
 */
NRF_STATIC_INLINE void nrf_lfxo_status_get(NRF_LFXO_Type const * p_reg, 
                                           nrf_lfxo_status_t *   p_status);

#if NRF_LFXO_HAS_STATUSANA
/**
 * @brief Function for checking status of analog module READY signal.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The signal is logic 1.
 * @retval false The signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_lfxo_statusana_ready_check(NRF_LFXO_Type const * p_reg);

/**
 * @brief Function for checking status of analog module SETTLED signal.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The signal is logic 1.
 * @retval false The signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_lfxo_statusana_settled_check(NRF_LFXO_Type const * p_reg);
#endif

/**
 * @brief Function for getting internal capacitive load value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Capacitance value in pF, resolution: 1pF.
 */
NRF_STATIC_INLINE uint8_t nrf_lfxo_cload_get(NRF_LFXO_Type const * p_reg);

/**
 * @brief Function for setting internal capacitive load value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] cap   Capacitance value in pF, resolution: 1pF.
 */
NRF_STATIC_INLINE void nrf_lfxo_cload_set(NRF_LFXO_Type * p_reg, uint8_t cap);

/**
 * @brief Function for getting amplitude control configuration.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_ctrl Pointer to the structure to be filled with amplitude control configuration.
 */
NRF_STATIC_INLINE void nrf_lfxo_amplitude_control_get(NRF_LFXO_Type const *       p_reg,
                                                      nrf_lfxo_amplitude_ctrl_t * p_ctrl);

/**
 * @brief Function for setting amplitude control configuration.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_ctrl Pointer to the structure with amplitude control configuration.
 */
NRF_STATIC_INLINE void nrf_lfxo_amplitude_control_set(NRF_LFXO_Type *             p_reg,
                                                      nrf_lfxo_amplitude_ctrl_t * p_ctrl);

/**
 * @brief Function for power control set.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] pwrctrl Power up control.
 */
NRF_STATIC_INLINE void nrf_lfxo_power_control_set(NRF_LFXO_Type *          p_reg,
                                                  nrf_lfxo_power_control_t pwrctrl);

/**
 * @brief Function setting LFXO mode.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] oscmode Oscillator mode.
 * @param[in] hpmode  High performance mode enable.
 */
NRF_STATIC_INLINE void nrf_lfxo_mode_set(NRF_LFXO_Type *            p_reg,
                                         nrf_lfxo_oscillator_mode_t oscmode,
                                         bool                       hpmode);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE uint32_t nrf_lfxo_event_address_get(NRF_LFXO_Type const * p_reg,
                                                      nrf_lfxo_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_lfxo_event_clear(NRF_LFXO_Type * p_reg, nrf_lfxo_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_lfxo_event_check(NRF_LFXO_Type const * p_reg, nrf_lfxo_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_lfxo_int_enable(NRF_LFXO_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_lfxo_int_disable(NRF_LFXO_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_lfxo_int_enable_check(NRF_LFXO_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_lfxo_int_pending_get(NRF_LFXO_Type const * p_reg)
{
    return p_reg->INTPEND;
}

NRF_STATIC_INLINE void nrf_lfxo_status_get(NRF_LFXO_Type const * p_reg, 
                                           nrf_lfxo_status_t *   p_status)
{
    NRFX_ASSERT(p_status);
    p_status->oscmode = (nrf_lfxo_oscillator_mode_t)((p_reg->STATUS & LFXO_STATUS_MODE_Msk) 
                                                    >> LFXO_STATUS_MODE_Pos);
    p_status->hpmode  = (p_reg->STATUS & LFXO_STATUS_HPMODE_Msk)  >> LFXO_STATUS_HPMODE_Pos;
    p_status->running = (p_reg->STATUS & LFXO_STATUS_RUNNING_Msk) >> LFXO_STATUS_RUNNING_Pos;
}

#if NRF_LFXO_HAS_STATUSANA
NRF_STATIC_INLINE bool nrf_lfxo_statusana_ready_check(NRF_LFXO_Type const * p_reg)
{
    return p_reg->STATUSANA & LFXO_STATUSANA_READY_Msk;
}

NRF_STATIC_INLINE bool nrf_lfxo_statusana_settled_check(NRF_LFXO_Type const * p_reg)
{
    return p_reg->STATUSANA & LFXO_STATUSANA_SETTLED_Msk;
}
#endif

NRF_STATIC_INLINE uint8_t nrf_lfxo_cload_get(NRF_LFXO_Type const * p_reg)
{
    uint8_t cload_reg = (uint8_t)p_reg->CLOAD;
    uint8_t cap = (cload_reg & LFXO_CLOAD_VAL0_Msk) >> LFXO_CLOAD_VAL0_Pos;

    if (cload_reg & LFXO_CLOAD_VAL1_Msk)
    {
        cap += 10;
    }
    return cap;
}

NRF_STATIC_INLINE void nrf_lfxo_cload_set(NRF_LFXO_Type * p_reg, uint8_t cap)
{
    NRFX_ASSERT(cap <= 25);
    uint32_t cload_reg = 0;

    if (cap >= 10) /* in pF */
    {
        cload_reg = LFXO_CLOAD_VAL1_Msk;
        cap -= 10;
    }

    cload_reg |= (cap << LFXO_CLOAD_VAL0_Pos) & LFXO_CLOAD_VAL0_Msk;
    p_reg->CLOAD = cload_reg;
}

NRF_STATIC_INLINE void nrf_lfxo_amplitude_control_get(NRF_LFXO_Type const *       p_reg,
                                                      nrf_lfxo_amplitude_ctrl_t * p_ctrl)
{
    NRFX_ASSERT(p_ctrl);
    uint32_t reg = p_reg->AMPLITUDECTRL;

    p_ctrl->interval  = (reg & LFXO_AMPLITUDECTRL_INTERVAL_Msk) >> LFXO_AMPLITUDECTRL_INTERVAL_Pos;
    p_ctrl->step      = (reg & LFXO_AMPLITUDECTRL_STEP_Msk)     >> LFXO_AMPLITUDECTRL_STEP_Pos;
    p_ctrl->idac_init = (reg & LFXO_AMPLITUDECTRL_IDACINIT_Msk) >> LFXO_AMPLITUDECTRL_IDACINIT_Pos;
    p_ctrl->pdctrl_en = (reg & LFXO_AMPLITUDECTRL_PDCTRL_Msk)   >> LFXO_AMPLITUDECTRL_PDCTRL_Pos;
}

NRF_STATIC_INLINE void nrf_lfxo_amplitude_control_set(NRF_LFXO_Type *             p_reg,
                                                      nrf_lfxo_amplitude_ctrl_t * p_ctrl)
{
    NRFX_ASSERT(p_ctrl);
    p_reg->AMPLITUDECTRL =
             ((p_ctrl->interval  << LFXO_AMPLITUDECTRL_INTERVAL_Pos) & LFXO_AMPLITUDECTRL_INTERVAL_Msk)
           | ((p_ctrl->step      << LFXO_AMPLITUDECTRL_STEP_Pos)     & LFXO_AMPLITUDECTRL_STEP_Msk)
           | ((p_ctrl->idac_init << LFXO_AMPLITUDECTRL_IDACINIT_Pos) & LFXO_AMPLITUDECTRL_IDACINIT_Msk)
           | ((p_ctrl->pdctrl_en << LFXO_AMPLITUDECTRL_PDCTRL_Pos)   & LFXO_AMPLITUDECTRL_PDCTRL_Msk);
}

NRF_STATIC_INLINE void nrf_lfxo_power_control_set(NRF_LFXO_Type *          p_reg,
                                                  nrf_lfxo_power_control_t pwrctrl)
{
    p_reg->PWRUPCTRL = ((uint32_t)pwrctrl << LFXO_PWRUPCTRL_CTRL_Pos) & LFXO_PWRUPCTRL_CTRL_Msk;
}

NRF_STATIC_INLINE void nrf_lfxo_mode_set(NRF_LFXO_Type *            p_reg,
                                         nrf_lfxo_oscillator_mode_t oscmode,
                                         bool                       hpmode)
{
    p_reg->MODE = (((uint32_t)oscmode << LFXO_MODE_MODE_Pos)   & LFXO_MODE_MODE_Msk)
                | (((uint32_t)hpmode  << LFXO_MODE_HPMODE_Pos) & LFXO_MODE_HPMODE_Msk);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_LFXO_H__
