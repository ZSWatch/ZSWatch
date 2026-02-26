/*
 * Copyright (c) 2023 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_HSFLL_H__
#define NRF_HSFLL_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_hsfll_hal HSFLL HAL
 * @{
 * @ingroup nrf_clock
 * @brief   Hardware access layer for managing the High Speed Frequency Locked Loop (HSFLL).
 */

#if defined(HSFLL_CLOCKCTRL_DITHERING_INITVALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether initial value for clock dithering configuration is present. */
#define NRF_HSFLL_HAS_DITHERING_INITVALUE 1
#else
#define NRF_HSFLL_HAS_DITHERING_INITVALUE 0
#endif

#if defined(HSFLL_CLOCKCTRL_DITHERINIT_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether initial value for clock dithering seed configuration is present. */
#define NRF_HSFLL_HAS_DITHERINIT 1
#else
#define NRF_HSFLL_HAS_DITHERINIT 0
#endif

#if defined(HSFLL_TRIM_TCOEF_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has temperature coefficient trimming. */
#define NRF_HSFLL_HAS_TCOEF_TRIM 1
#else
#define NRF_HSFLL_HAS_TCOEF_TRIM 0
#endif

#if defined(HSFLL_TASKS_FREQMEAS_TASKS_FREQMEAS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has TASKS_FREQMEAS. */
#define NRF_HSFLL_HAS_TASKS_FREQMEAS 1
#else
#define NRF_HSFLL_HAS_TASKS_FREQMEAS 0
#endif

#if defined(HSFLL_TASKS_START_TASKS_START_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has START and STOP tasks. */
#define NRF_HSFLL_HAS_TASK_START_STOP 1
#else
#define NRF_HSFLL_HAS_TASK_START_STOP 0
#endif

#if defined(HSFLL_CLOCKCTRL_MODE_OVERRIDE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has CLOCKCTRL_MODE_OVERRIDE. */
#define NRF_HSFLL_HAS_CLOCKCTRL_MODE_OVERRIDE 1
#else
#define NRF_HSFLL_HAS_CLOCKCTRL_MODE_OVERRIDE 0
#endif

#if defined(HSFLL_CLOCKSTATUS_OVERRIDE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has CLOCKSTATUS_OVERRIDE. */
#define NRF_HSFLL_HAS_CLOCKSTATUS_OVERRIDE 1
#else
#define NRF_HSFLL_HAS_CLOCKSTATUS_OVERRIDE 0
#endif

#if defined(HSFLL_CLOCKCTRL_SLEEP_MODE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has CLOCKCTRL_SLEEP_MODE. */
#define NRF_HSFLL_HAS_CLOCKCTRL_SLEEP_MODE 1
#else
#define NRF_HSFLL_HAS_CLOCKCTRL_SLEEP_MODE 0
#endif

#if defined(HSFLL_CLOCKCTRL_RETAINFINETRIM_RETAIN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has CLOCKCTRL_RETAINFINETRIM. */
#define NRF_HSFLL_HAS_CLOCKCTRL_RETAINFINETRIM 1
#else
#define NRF_HSFLL_HAS_CLOCKCTRL_RETAINFINETRIM 0
#endif

#if defined(HSFLL_CLOCKCTRL_OVERRIDELOCKED_OVERRIDE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has CLOCKCTRL_OVERRIDELOCKED. */
#define NRF_HSFLL_HAS_CLOCKCTRL_OVERRIDELOCKED 1
#else
#define NRF_HSFLL_HAS_CLOCKCTRL_OVERRIDELOCKED 0
#endif

#if defined(HSFLL_CLOCKSTATUS_MODE_Bypass) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HSFLL has CLOCKSTATUS_MODE_BYPASS. */
#define NRF_HSFLL_HAS_CLOCKSTATUS_MODE_BYPASS 1
#else
#define NRF_HSFLL_HAS_CLOCKSTATUS_MODE_BYPASS 0
#endif

/** @brief HSFLL tasks. */
typedef enum
{
#if NRF_HSFLL_HAS_TASK_START_STOP
    NRF_HSFLL_TASK_START       = offsetof(NRF_HSFLL_Type, TASKS_START),      ///< Start the HSFLL.
    NRF_HSFLL_TASK_STOP        = offsetof(NRF_HSFLL_Type, TASKS_STOP),       ///< Stop the HSFLL.
#endif
#if NRF_HSFLL_HAS_TASKS_FREQMEAS
    NRF_HSFLL_TASK_FREQ_MEAS   = offsetof(NRF_HSFLL_Type, TASKS_FREQMEAS),   ///< Start frequency measurement in software-controlled mode.
#endif
    NRF_HSFLL_TASK_FREQ_CHANGE = offsetof(NRF_HSFLL_Type, TASKS_FREQCHANGE), ///< Trigger frequency change.
} nrf_hsfll_task_t;

/** @brief HSFLL events. */
typedef enum
{
#if NRF_HSFLL_HAS_TASK_START_STOP
    NRF_HSFLL_EVENT_STARTED      = offsetof(NRF_HSFLL_Type, EVENTS_STARTED),     ///< HSFLL started.
    NRF_HSFLL_EVENT_STOPPED      = offsetof(NRF_HSFLL_Type, EVENTS_STOPPED),     ///< HSFLL stopped.
#endif
#if NRF_HSFLL_HAS_TASKS_FREQMEAS
    NRF_HSFLL_EVENT_FREQM_DONE   = offsetof(NRF_HSFLL_Type, EVENTS_FREQMDONE),   ///< HSFLL frequency measurement done.
#endif
    NRF_HSFLL_EVENT_FREQ_CHANGED = offsetof(NRF_HSFLL_Type, EVENTS_FREQCHANGED), ///< HSFLL frequency change done.
} nrf_hsfll_event_t;

/** @brief HSFLL clock status operating modes. */
typedef enum
{
    NRF_HSFLL_MODE_STATUS_OPEN_LOOP   = HSFLL_CLOCKSTATUS_MODE_OpenLoop,   ///< Open loop mode.
    NRF_HSFLL_MODE_STATUS_CLOSED_LOOP = HSFLL_CLOCKSTATUS_MODE_ClosedLoop, ///< Closed loop mode.
#if NRF_HSFLL_HAS_CLOCKSTATUS_MODE_BYPASS
    NRF_HSFLL_MODE_STATUS_BYPASS      = HSFLL_CLOCKSTATUS_MODE_Bypass,     ///< Bypass mode.
#endif
} nrf_hsfll_mode_status_t;

/** @brief HSFLL clock status. */
typedef struct
{
    nrf_hsfll_mode_status_t mode;     ///< HSFLL operating mode.
    bool                    override; ///< HSFLL override mode is enabled.
    bool                    accuracy; ///< Clock accurracy is within 2%.
    bool                    locked;   ///< HSFLL locked to reference clock.
} nrf_hsfll_status_clk_t;

/** @brief HSFLL frequency measurements errors. */
typedef struct
{
    bool error;          ///< Trim error status. True if outside limit, false if within.
    bool trim_underflow; ///< Underflow error status. True if outside limit, false if within.
    bool trim_overflow;  ///< Overflow error status. True if outside limit, false if within.
} nrf_hsfll_freqm_error_t;

/** @brief HSFLL clock control operating mode settings. */
typedef enum
{
    NRF_HSFLL_MODE_CTRL_AUTO        = HSFLL_CLOCKCTRL_MODE_MODE_Auto,       ///< The PCGC controls the mode automatically.
    NRF_HSFLL_MODE_CTRL_OPEN_LOOP   = HSFLL_CLOCKCTRL_MODE_MODE_OpenLoop,   ///< Open loop mode.
    NRF_HSFLL_MODE_CTRL_CLOSED_LOOP = HSFLL_CLOCKCTRL_MODE_MODE_ClosedLoop, ///< Closed loop mode.
#if NRF_HSFLL_HAS_CLOCKSTATUS_MODE_BYPASS
    NRF_HSFLL_MODE_CTRL_BYPASS      = HSFLL_CLOCKCTRL_MODE_MODE_Bypass,     ///< Bypass mode.
#endif
} nrf_hsfll_mode_ctrl_t;

/** @brief HSFLL clock control. */
typedef struct
{
    nrf_hsfll_mode_ctrl_t mode;     ///< HSFLL operating mode.
    bool                  override; ///< HSFLL override mode. True if enabled, false otherwise.
} nrf_hsfll_clkctrl_t;

/** @brief HSFLL clock dithering configuration. */
typedef struct
{
    uint8_t  cyclecount; ///< Cycle count configuration for clock dithering.
    uint8_t  maxoffset;  ///< Maximum offset configuration for clock dithering.
#if NRF_HSFLL_HAS_DITHERING_INITVALUE
    uint16_t initvalue;  ///< Initial value for the clock dithering.
#endif
    bool     enable;     ///< Enable the clock dithering.
} nrf_hsfll_dithering_t;

/** @brief HSFLL clock sleep configuration. */
typedef struct
{
    bool mode;   ///< Power down the HSFLL core. True if powered down, false if in normal mode.
    bool retain; ///< Retain all inputs while powered down. True if retention is enabled, false otherwise.
} nrf_hsfll_sleep_t;

/** @brief HSFLL trims configuration. */
typedef struct
{
    uint16_t coarse; ///< Coarse frequance trimming.
    uint16_t fine;   ///< Fine frequency trimming.
    uint8_t  vsup;   ///< Internal regulator voltage supply level trimming.
#if NRF_HSFLL_HAS_TCOEF_TRIM
    uint8_t  tcoef;  ///< Temperature coefficient trimming.
#endif
} nrf_hsfll_trim_t;

/**
 * @brief Function for getting the address of the specified task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  HSFLL task.
 *
 * @return Address of the requested task register.
 */
NRF_STATIC_INLINE uint32_t nrf_hsfll_task_address_get(NRF_HSFLL_Type const * p_reg,
                                                      nrf_hsfll_task_t       task);

/**
 * @brief Function for triggering the specified task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_hsfll_task_trigger(NRF_HSFLL_Type * p_reg, nrf_hsfll_task_t task);

/**
 * @brief Function for retrieving the address of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event HSFLL event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_hsfll_event_address_get(NRF_HSFLL_Type const * p_reg,
                                                       nrf_hsfll_event_t      event);

/**
 * @brief Function for clearing the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_hsfll_event_clear(NRF_HSFLL_Type * p_reg, nrf_hsfll_event_t event);

/**
 * @brief Function for retrieving the state of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_hsfll_event_check(NRF_HSFLL_Type const * p_reg,
                                             nrf_hsfll_event_t      event);

/**
 * @brief Function for getting the HSFLL status.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_status Pointer to the structure to be filled with the HSFLL status.
 */
NRF_STATIC_INLINE void nrf_hsfll_status_clk_get(NRF_HSFLL_Type const *   p_reg,
                                                nrf_hsfll_status_clk_t * p_status);

#if NRF_HSFLL_HAS_TASKS_FREQMEAS
/**
 * @brief Function for checking whether the HSFLL frequency measurement is completed.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The frequency measurement is completed.
 * @retval false The frequency measurement is in progress.
 */
NRF_STATIC_INLINE bool nrf_hsfll_freqm_done_check(NRF_HSFLL_Type const * p_reg);

/**
 * @brief Function for getting HSFLL frequency measurement errors.
 *
 * @param[in]  p_reg   Pointer to the structure of registers of the peripheral.
 * @param[out] p_error Pointer to the structure to be filled with HSFLL frequency measurement
 *                     errors.
 */
NRF_STATIC_INLINE void nrf_hsfll_freqm_error_get(NRF_HSFLL_Type const *    p_reg,
                                                 nrf_hsfll_freqm_error_t * p_error);

/**
 * @brief Function for getting HSFLL frequency measurement.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The last frequency measurement value. Measures the number of reference clock cycles.
 */
NRF_STATIC_INLINE uint32_t nrf_hsfll_freqm_meas_get(NRF_HSFLL_Type const * p_reg);
#endif

#if NRF_HSFLL_HAS_CLOCKCTRL_MODE_OVERRIDE
/**
 * @brief Function for setting HSFLL clock control mode settings.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] p_clkctrl Pointer to the structure with new HSFLL clock control mode settings.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_mode_set(NRF_HSFLL_Type *            p_reg,
                                                  nrf_hsfll_clkctrl_t const * p_clkctrl);

/**
 * @brief Function for getting HSFLL clock control mode settings.
 *
 * @param[in]  p_reg     Pointer to the structure of registers of the peripheral.
 * @param[out] p_clkctrl Pointer to the structure to be filled with HSFLL clock control settings.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_mode_get(NRF_HSFLL_Type const * p_reg,
                                                  nrf_hsfll_clkctrl_t *  p_clkctrl);
#endif
/**
 * @brief Function for setting HSFLL clock dithering configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure with new HSFLL clock dithering configuration.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_dithering_set(NRF_HSFLL_Type *              p_reg,
                                                       nrf_hsfll_dithering_t const * p_config);

/**
 * @brief Function for getting HSFLL clock dithering configuration.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_config Pointer to the structure to be filled with HSFLL clock dithering
 *                      configuration.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_dithering_get(NRF_HSFLL_Type const *  p_reg,
                                                       nrf_hsfll_dithering_t * p_config);

/**
 * @brief Function for setting HSFLL frequency multiplier.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] multiplier Value of new multiplier. Valid @c multiplier range is from 4 to 25.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_mult_set(NRF_HSFLL_Type * p_reg, uint32_t multiplier);

/**
 * @brief Function for getting HSFLL frequency multiplier.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Current value of frequency multiplier used by HSFLL.
 */
NRF_STATIC_INLINE uint32_t nrf_hsfll_clkctrl_mult_get(NRF_HSFLL_Type const * p_reg);

#if NRF_HSFLL_HAS_CLOCKCTRL_SLEEP_MODE
/**
 * @brief Function for setting HSFLL clock sleep configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure with new HSFLL clock sleep configuration.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_sleep_set(NRF_HSFLL_Type *          p_reg,
                                                   nrf_hsfll_sleep_t const * p_config);

/**
 * @brief Function for getting HSFLL clock sleep configuration.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_config Pointer to the structure to be filled with HSFLL clock sleep configuration.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_sleep_get(NRF_HSFLL_Type const * p_reg,
                                                   nrf_hsfll_sleep_t *    p_config);
#endif

#if NRF_HSFLL_HAS_CLOCKCTRL_RETAINFINETRIM
/**
 * @brief Function for enabling or disabling the retention of HSFLL fine trim control.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] retain True if the fine trim control is to be retained when HSFLL goes to open-loop
 *                   mode, false otherwise.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_retainfinetrim_enable_set(NRF_HSFLL_Type * p_reg,
                                                                   bool             retain);
#endif

#if NRF_HSFLL_HAS_CLOCKCTRL_OVERRIDELOCKED
/**
 * @brief Function for enabling or disabling the override of the HSFLL LOCKED signal.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] override True if the override is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_overridelocked_enable_set(NRF_HSFLL_Type * p_reg,
                                                                   bool             override);
#endif

#if NRF_HSFLL_HAS_DITHERINIT
/**
 * @brief Function for setting the configurable seed of HSFLL clock dithering.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] seed  32-bit initial value for the PRBS.
 */
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_dither_init_set(NRF_HSFLL_Type * p_reg, uint32_t seed);
#endif // NRF_HSFLL_HAS_DITHERINIT

/**
 * @brief Function for enabling or disabling lock for mirrored registers.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the lock is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_hsfll_mirror_lock_set(NRF_HSFLL_Type * p_reg, bool enable);

/**
 * @brief Function to setup trims configuration.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_trim Pointer to the structure with new HSFLL trim configuration.
 */
NRF_STATIC_INLINE void nrf_hsfll_trim_set(NRF_HSFLL_Type *         p_reg,
                                          nrf_hsfll_trim_t const * p_trim);

/**
 * @brief Function to getting trims configuration.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_trim Pointer to the structure to be filled with HSFLL trim configuration.
 */
NRF_STATIC_INLINE void nrf_hsfll_trim_get(NRF_HSFLL_Type const * p_reg,
                                          nrf_hsfll_trim_t *     p_trim);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE uint32_t nrf_hsfll_task_address_get(NRF_HSFLL_Type const * p_reg,
                                                      nrf_hsfll_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_hsfll_task_trigger(NRF_HSFLL_Type * p_reg, nrf_hsfll_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_hsfll_event_address_get(NRF_HSFLL_Type const * p_reg,
                                                       nrf_hsfll_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_hsfll_event_clear(NRF_HSFLL_Type * p_reg, nrf_hsfll_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_hsfll_event_check(NRF_HSFLL_Type const * p_reg,
                                             nrf_hsfll_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_hsfll_status_clk_get(NRF_HSFLL_Type const *   p_reg,
                                                nrf_hsfll_status_clk_t * p_status)
{
    NRFX_ASSERT(p_status);
    uint32_t reg = p_reg->CLOCKSTATUS;

    p_status->mode =
        (nrf_hsfll_mode_status_t)((reg & HSFLL_CLOCKSTATUS_MODE_Msk) >> HSFLL_CLOCKSTATUS_MODE_Pos);
#if NRF_HSFLL_HAS_CLOCKSTATUS_OVERRIDE
    p_status->override = ((reg & HSFLL_CLOCKSTATUS_OVERRIDE_Msk)
                          >> HSFLL_CLOCKSTATUS_OVERRIDE_Pos) ==
                         HSFLL_CLOCKSTATUS_OVERRIDE_Enabled;
#endif
    p_status->accuracy = ((reg & HSFLL_CLOCKSTATUS_ACCURACY_Msk)
                          >> HSFLL_CLOCKSTATUS_ACCURACY_Pos) ==
                         HSFLL_CLOCKSTATUS_ACCURACY_WithinLimit;
    p_status->locked = ((reg & HSFLL_CLOCKSTATUS_LOCKED_Msk)
                        >> HSFLL_CLOCKSTATUS_LOCKED_Pos) ==
                       HSFLL_CLOCKSTATUS_LOCKED_Locked;
}

#if NRF_HSFLL_HAS_TASKS_FREQMEAS
NRF_STATIC_INLINE bool nrf_hsfll_freqm_done_check(NRF_HSFLL_Type const * p_reg)
{
    return ((p_reg->FREQM.DONE & HSFLL_FREQM_DONE_DONE_Msk) >> HSFLL_FREQM_DONE_DONE_Pos) ==
           HSFLL_FREQM_DONE_DONE_Completed;
}

NRF_STATIC_INLINE void nrf_hsfll_freqm_error_get(NRF_HSFLL_Type const *    p_reg,
                                                 nrf_hsfll_freqm_error_t * p_error)
{
    NRFX_ASSERT(p_error);
    p_error->error =
        ((p_reg->FREQM.ERROR & HSFLL_FREQM_ERROR_ERROR_Msk) >> HSFLL_FREQM_ERROR_ERROR_Pos) ==
        HSFLL_FREQM_ERROR_ERROR_OutsideLimit;
    p_error->trim_underflow = ((p_reg->FREQM.ERROR & HSFLL_FREQM_ERROR_TRIMUNDERFLOW_Msk)
                               >> HSFLL_FREQM_ERROR_TRIMUNDERFLOW_Pos) ==
                              HSFLL_FREQM_ERROR_TRIMUNDERFLOW_OutsideLimit;
    p_error->trim_overflow = ((p_reg->FREQM.ERROR & HSFLL_FREQM_ERROR_TRIMOVERFLOW_Msk)
                              >> HSFLL_FREQM_ERROR_TRIMOVERFLOW_Pos) ==
                             HSFLL_FREQM_ERROR_TRIMOVERFLOW_OutsideLimit;
}

NRF_STATIC_INLINE uint32_t nrf_hsfll_freqm_meas_get(NRF_HSFLL_Type const * p_reg)
{
    return (p_reg->FREQM.MEAS & HSFLL_FREQM_MEAS_VALUE_Msk) >> HSFLL_FREQM_MEAS_VALUE_Pos;
}
#endif

#if NRF_HSFLL_HAS_CLOCKCTRL_MODE_OVERRIDE
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_mode_set(NRF_HSFLL_Type *            p_reg,
                                                  nrf_hsfll_clkctrl_t const * p_clkctrl)
{
    NRFX_ASSERT(p_clkctrl);
    p_reg->CLOCKCTRL.MODE = ((p_clkctrl->mode << HSFLL_CLOCKCTRL_MODE_MODE_Pos) &
                             HSFLL_CLOCKCTRL_MODE_MODE_Msk) |
                            (((p_clkctrl->override ?
                               HSFLL_CLOCKCTRL_MODE_OVERRIDE_Enabled :
                               HSFLL_CLOCKCTRL_MODE_OVERRIDE_Disabled)
                              << HSFLL_CLOCKCTRL_MODE_OVERRIDE_Pos) &
                             HSFLL_CLOCKCTRL_MODE_OVERRIDE_Msk);
}

NRF_STATIC_INLINE void nrf_hsfll_clkctrl_mode_get(NRF_HSFLL_Type const * p_reg,
                                                  nrf_hsfll_clkctrl_t *  p_clkctrl)
{
    NRFX_ASSERT(p_clkctrl);
    uint32_t reg = p_reg->CLOCKCTRL.MODE;

    p_clkctrl->mode =
        (nrf_hsfll_mode_ctrl_t)((reg & HSFLL_CLOCKCTRL_MODE_MODE_Msk)
                                >> HSFLL_CLOCKCTRL_MODE_MODE_Pos);
    p_clkctrl->override = ((reg & HSFLL_CLOCKCTRL_MODE_OVERRIDE_Msk)
                           >> HSFLL_CLOCKCTRL_MODE_OVERRIDE_Pos) ==
                          HSFLL_CLOCKCTRL_MODE_OVERRIDE_Enabled;
}
#endif

NRF_STATIC_INLINE void nrf_hsfll_clkctrl_dithering_set(NRF_HSFLL_Type *              p_reg,
                                                       nrf_hsfll_dithering_t const * p_config)
{
    NRFX_ASSERT(p_config);
    p_reg->CLOCKCTRL.DITHERING =
#if NRF_HSFLL_HAS_DITHERING_INITVALUE
        ((p_config->initvalue << HSFLL_CLOCKCTRL_DITHERING_INITVALUE_Pos) &
         HSFLL_CLOCKCTRL_DITHERING_INITVALUE_Msk) |
#endif
        ((p_config->cyclecount << HSFLL_CLOCKCTRL_DITHERING_CYCLECOUNT_Pos) &
         HSFLL_CLOCKCTRL_DITHERING_CYCLECOUNT_Msk) |
        ((p_config->maxoffset << HSFLL_CLOCKCTRL_DITHERING_MAXOFFSET_Pos) &
         HSFLL_CLOCKCTRL_DITHERING_MAXOFFSET_Msk) |
        (((p_config->enable ?
           HSFLL_CLOCKCTRL_DITHERING_EN_Enabled :
           HSFLL_CLOCKCTRL_DITHERING_EN_Disabled) << HSFLL_CLOCKCTRL_DITHERING_EN_Pos) &
         HSFLL_CLOCKCTRL_DITHERING_EN_Msk);
}

NRF_STATIC_INLINE void nrf_hsfll_clkctrl_dithering_get(NRF_HSFLL_Type const *  p_reg,
                                                       nrf_hsfll_dithering_t * p_config)
{
    NRFX_ASSERT(p_config);
    p_config->cyclecount = (p_reg->CLOCKCTRL.MODE & HSFLL_CLOCKCTRL_DITHERING_CYCLECOUNT_Msk)
                           >> HSFLL_CLOCKCTRL_DITHERING_CYCLECOUNT_Pos;
    p_config->maxoffset = (p_reg->CLOCKCTRL.MODE & HSFLL_CLOCKCTRL_DITHERING_MAXOFFSET_Msk)
                          >> HSFLL_CLOCKCTRL_DITHERING_MAXOFFSET_Pos;
    p_config->enable = ((p_reg->CLOCKCTRL.MODE & HSFLL_CLOCKCTRL_DITHERING_EN_Msk)
                        >> HSFLL_CLOCKCTRL_DITHERING_MAXOFFSET_Pos) ==
                       HSFLL_CLOCKCTRL_DITHERING_EN_Enabled;
#if NRF_HSFLL_HAS_DITHERING_INITVALUE
    p_config->initvalue = (p_reg->CLOCKCTRL.MODE & HSFLL_CLOCKCTRL_DITHERING_INITVALUE_Msk)
                          >> HSFLL_CLOCKCTRL_DITHERING_INITVALUE_Pos;
#endif
}

NRF_STATIC_INLINE void nrf_hsfll_clkctrl_mult_set(NRF_HSFLL_Type * p_reg,
                                                  uint32_t         multiplier)
{
    p_reg->CLOCKCTRL.MULT = (multiplier << HSFLL_CLOCKCTRL_MULT_VAL_Pos) &
                            HSFLL_CLOCKCTRL_MULT_VAL_Msk;
}

NRF_STATIC_INLINE uint32_t nrf_hsfll_clkctrl_mult_get(NRF_HSFLL_Type const * p_reg)
{
    return (p_reg->CLOCKCTRL.MULT & HSFLL_CLOCKCTRL_MULT_VAL_Msk) >> HSFLL_CLOCKCTRL_MULT_VAL_Pos;
}

#if NRF_HSFLL_HAS_CLOCKCTRL_SLEEP_MODE
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_sleep_set(NRF_HSFLL_Type *          p_reg,
                                                   nrf_hsfll_sleep_t const * p_config)
{
    NRFX_ASSERT(p_config);
    p_reg->CLOCKCTRL.SLEEP = (((p_config->mode ?
                                HSFLL_CLOCKCTRL_SLEEP_MODE_Sleep :
                                HSFLL_CLOCKCTRL_SLEEP_MODE_Normal)
                               << HSFLL_CLOCKCTRL_SLEEP_MODE_Pos) &
                              HSFLL_CLOCKCTRL_SLEEP_MODE_Msk) |
                             (((p_config->retain ?
                                HSFLL_CLOCKCTRL_SLEEP_RETAIN_Enabled :
                                HSFLL_CLOCKCTRL_SLEEP_RETAIN_Disabled)
                               << HSFLL_CLOCKCTRL_SLEEP_RETAIN_Pos) &
                              HSFLL_CLOCKCTRL_SLEEP_RETAIN_Msk);
}

NRF_STATIC_INLINE void nrf_hsfll_clkctrl_sleep_get(NRF_HSFLL_Type const * p_reg,
                                                   nrf_hsfll_sleep_t *    p_config)
{
    NRFX_ASSERT(p_config);
    p_config->mode = ((p_reg->CLOCKCTRL.SLEEP & HSFLL_CLOCKCTRL_SLEEP_MODE_Msk)
                      >> HSFLL_CLOCKCTRL_SLEEP_MODE_Pos) == HSFLL_CLOCKCTRL_SLEEP_MODE_Sleep;
    p_config->retain = ((p_reg->CLOCKCTRL.SLEEP & HSFLL_CLOCKCTRL_SLEEP_RETAIN_Msk)
                        >> HSFLL_CLOCKCTRL_SLEEP_RETAIN_Pos) ==
                       HSFLL_CLOCKCTRL_SLEEP_RETAIN_Enabled;
}
#endif

#if NRF_HSFLL_HAS_CLOCKCTRL_RETAINFINETRIM
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_retainfinetrim_enable_set(NRF_HSFLL_Type * p_reg,
                                                                   bool             retain)
{
    p_reg->CLOCKCTRL.RETAINFINETRIM = ((retain ?
                                        HSFLL_CLOCKCTRL_RETAINFINETRIM_RETAIN_Retain :
                                        HSFLL_CLOCKCTRL_RETAINFINETRIM_RETAIN_NoRetain)
                                       << HSFLL_CLOCKCTRL_RETAINFINETRIM_RETAIN_Pos) &
                                      HSFLL_CLOCKCTRL_RETAINFINETRIM_RETAIN_Msk;
}
#endif

#if NRF_HSFLL_HAS_CLOCKCTRL_OVERRIDELOCKED
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_overridelocked_enable_set(NRF_HSFLL_Type * p_reg,
                                                                   bool             override)
{
    p_reg->CLOCKCTRL.OVERRIDELOCKED = ((override ?
                                        HSFLL_CLOCKCTRL_OVERRIDELOCKED_OVERRIDE_Override :
                                        HSFLL_CLOCKCTRL_OVERRIDELOCKED_OVERRIDE_NoOperation)
                                       << HSFLL_CLOCKCTRL_OVERRIDELOCKED_OVERRIDE_Pos) &
                                      HSFLL_CLOCKCTRL_OVERRIDELOCKED_OVERRIDE_Msk;
}
#endif

#if NRF_HSFLL_HAS_DITHERINIT
NRF_STATIC_INLINE void nrf_hsfll_clkctrl_dither_init_set(NRF_HSFLL_Type * p_reg, uint32_t seed)
{
    p_reg->CLOCKCTRL.DITHERINIT = (seed << HSFLL_CLOCKCTRL_DITHERINIT_SEED_Pos) &
                                  HSFLL_CLOCKCTRL_DITHERINIT_SEED_Msk;
}

#endif // NRF_HSFLL_HAS_DITHERINIT

NRF_STATIC_INLINE void nrf_hsfll_mirror_lock_set(NRF_HSFLL_Type * p_reg, bool enable)
{
    p_reg->MIRROR = ((enable ? HSFLL_MIRROR_LOCK_Enabled : HSFLL_MIRROR_LOCK_Disabled)
                     << HSFLL_MIRROR_LOCK_Pos) & HSFLL_MIRROR_LOCK_Msk;
}

NRF_STATIC_INLINE void nrf_hsfll_trim_set(NRF_HSFLL_Type *         p_reg,
                                          nrf_hsfll_trim_t const * p_trim)
{
    NRFX_ASSERT(p_trim);
    nrf_hsfll_mirror_lock_set(p_reg, true);

    p_reg->TRIM.VSUP   = p_trim->vsup;
    p_reg->TRIM.COARSE = p_trim->coarse;
    p_reg->TRIM.FINE   = p_trim->fine;
#if NRF_HSFLL_HAS_TCOEF_TRIM
    p_reg->TRIM.TCOEF  = p_trim->tcoef;
#endif

    nrf_hsfll_mirror_lock_set(p_reg, false);
}

NRF_STATIC_INLINE void nrf_hsfll_trim_get(NRF_HSFLL_Type const * p_reg,
                                          nrf_hsfll_trim_t *     p_trim)
{
    NRFX_ASSERT(p_trim);

    p_trim->vsup   = (uint8_t)p_reg->TRIM.VSUP;
    p_trim->coarse = (uint8_t)p_reg->TRIM.COARSE;
    p_trim->fine   = (uint8_t)p_reg->TRIM.FINE;
#if NRF_HSFLL_HAS_TCOEF_TRIM
    p_trim->tcoef =  (uint8_t)p_reg->TRIM.TCOEF;
#endif
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_HSFLL_H__
