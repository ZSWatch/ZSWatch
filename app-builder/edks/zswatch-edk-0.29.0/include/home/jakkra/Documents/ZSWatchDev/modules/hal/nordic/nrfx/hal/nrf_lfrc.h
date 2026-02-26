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

#ifndef NRF_LFRC_H__
#define NRF_LFRC_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_lfrc_hal LFRC HAL
 * @{
 * @ingroup nrf_clock
 * @brief   Hardware access layer for managing the Low Frequency 32 KHz RC Oscillator (LFRC).
 */

#if defined (LFRC_CONFIG_CFG_DOUBLETAILCURRENT_Pos) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether double tail current is present. */
#define NRF_LFRC_HAS_DOUBLE_TAIL_CURRENT 1
#else
#define NRF_LFRC_HAS_DOUBLE_TAIL_CURRENT 0
#endif

#if defined (LFRC_CONFIG_CFG_CONTINUOUSTAILBIAS_Pos) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether continuous tail bias is present. */
#define NRF_LFRC_HAS_CONTINUOUS_TAIL_BIAS 1
#else
#define NRF_LFRC_HAS_CONTINUOUS_TAIL_BIAS 0
#endif

#if defined (LFRC_CONFIG_CFG_ENABLERETENTION_Pos) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CAL retention is present. */
#define NRF_LFRC_HAS_RETENTION 1
#else
#define NRF_LFRC_HAS_RETENTION 0
#endif

#if defined (LFRC_CONFIG_CFG_SPARE_Pos) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether spare general purpose configuration is present. */
#define NRF_LFRC_HAS_CFG_SPARE 1
#else
#define NRF_LFRC_HAS_CFG_SPARE 0
#endif

#if defined (LFRC_INTPEND_ResetValue) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether INTPEND register is present. */
#define NRF_LFRC_HAS_INTPEND 1
#else
#define NRF_LFRC_HAS_INTPEND 0
#endif

#if defined (LFRC_INTENSET_CALDONE_Msk) || defined (__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the Low Frequency Clock calibration is present. */
#define NRF_LFRC_HAS_CALIBRATION 1
#else
#define NRF_LFRC_HAS_CALIBRATION 0
#endif

#if defined(LFRC_PWRUPCTRL_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the PWRUPCTRL register is present. */
#define NRF_LFRC_HAS_PWRUPCTRL 1
#else
#define NRF_LFRC_HAS_PWRUPCTRL 0
#endif

#if defined(LFRC_STATUSTRIM_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the STATUSTRIM register is present. */
#define NRF_LFRC_HAS_STATUSTRIM 1
#else
#define NRF_LFRC_HAS_STATUSTRIM 0
#endif

#if defined(LFRC_STATUSANA_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the STATUSANA register is present. */
#define NRF_LFRC_HAS_STATUSANA 1
#else
#define NRF_LFRC_HAS_STATUSANA 0
#endif

#if defined(LFRC_CONFIG_CFG_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CONFIG.CFG register is present. */
#define NRF_LFRC_HAS_CONFIG_CFG 1
#else
#define NRF_LFRC_HAS_CONFIG_CFG 0
#endif

#if defined(LFRC_CAL_TRIMLIMITLO_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CAL.TRIMLIMITLO register is present. */
#define NRF_LFRC_HAS_CAL_TRIMLIMITLO 1
#else
#define NRF_LFRC_HAS_CAL_TRIMLIMITLO 0
#endif

#if defined(LFRC_CAL_TRIMLIMITHI_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CAL.TRIMLIMITHI register is present. */
#define NRF_LFRC_HAS_CAL_TRIMLIMITHI 1
#else
#define NRF_LFRC_HAS_CAL_TRIMLIMITHI 0
#endif

#if defined(LFRC_CAL_RESULT_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CAL.RESULT register is present. */
#define NRF_LFRC_HAS_CAL_RESULT 1
#else
#define NRF_LFRC_HAS_CAL_RESULT 0
#endif

#if defined(LFRC_CAL_LENGTH_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CAL.LENGTH register is present. */
#define NRF_LFRC_HAS_CAL_LENGTH 1
#else
#define NRF_LFRC_HAS_CAL_LENGTH 0
#endif

#if defined(LFRC_CAL_NHI_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the CAL.NHI register is present. */
#define NRF_LFRC_HAS_CAL_NHI 1
#else
#define NRF_LFRC_HAS_CAL_NHI 0
#endif

/**
 * @brief LFRC tasks.
 *
 * @details The NRF_LFRC_TASK_CANCELCAL task cannot be set when the calibration is not running.
 */
typedef enum
{
    NRF_LFRC_TASK_CAL       = offsetof(NRF_LFRC_Type, TASKS_CAL),       /**< Start LFRC calibration. */
    NRF_LFRC_TASK_CANCELCAL = offsetof(NRF_LFRC_Type, TASKS_CANCELCAL), /**< Cancel LFRC calibration. */
} nrf_lfrc_task_t;

/** @brief LFRC events. */
typedef enum
{
    NRF_LFRC_EVENT_CALDONE   = offsetof(NRF_LFRC_Type, EVENTS_CALDONE),   /**< LFRC calibration done. */
    NRF_LFRC_EVENT_TRIMDONE  = offsetof(NRF_LFRC_Type, EVENTS_TRIMDONE),  /**< LFRC trim done. */
    NRF_LFRC_EVENT_TRIMERROR = offsetof(NRF_LFRC_Type, EVENTS_TRIMERROR), /**< LFRC trim error. */
} nrf_lfrc_event_t;

/** @brief LFRC interrupts. */
typedef enum
{
    NRF_LFRC_INT_CALDONE_MASK   = LFRC_INTENSET_CALDONE_Msk,   /**< Interrupt on CALDONE event. */
    NRF_LFRC_INT_TRIMDONE_MASK  = LFRC_INTENSET_TRIMDONE_Msk,  /**< Interrupt on TRIMDONE event. */
    NRF_LFRC_INT_TRIMERROR_MASK = LFRC_INTENSET_TRIMERROR_Msk, /**< Interrupt on TRIMERROR event. */
} nrf_lfrc_int_mask_t;

#if NRF_LFRC_HAS_CONFIG_CFG
/** @brief LFRC override configuration. */
typedef struct
{
#if NRF_LFRC_HAS_DOUBLE_TAIL_CURRENT
    bool doubletailcurrent_en;  /**< Enable double tail current. */
#endif
#if NRF_LFRC_HAS_CONTINUOUS_TAIL_BIAS
    bool continuoustailbias_en; /**< Enable continuous tail bias. */
#endif
#if NRF_LFRC_HAS_RETENTION
    bool retention_en;          /**< Enable retention for CAL. */
#endif
#if NRF_LFRC_HAS_CFG_SPARE
    bool spare_en;              /**< Enable spare general purpose configuration bits. */
#endif
} nrf_lfrc_config_t;
#endif

/** @brief Number of measurement cycles used during calibration. */
typedef enum
{
    NRF_LFRC_CAL_CYCLE_LENGTH_64  = LFRC_CAL_LENGTH_LENGTH_N64,  /**< 64 cycles  */
    NRF_LFRC_CAL_CYCLE_LENGTH_128 = LFRC_CAL_LENGTH_LENGTH_N128, /**< 128 cycles */
    NRF_LFRC_CAL_CYCLE_LENGTH_256 = LFRC_CAL_LENGTH_LENGTH_N256, /**< 256 cycles */
    NRF_LFRC_CAL_CYCLE_LENGTH_512 = LFRC_CAL_LENGTH_LENGTH_N512  /**< 512 cycles */
} nrf_lfrc_cal_cycle_length_t;

#if NRF_LFRC_HAS_PWRUPCTRL
/** @brief Power up control. */
typedef enum
{
    NRF_LFRC_POWER_CONTROL_AUTO       = LFRC_PWRUPCTRL_CTRL_Auto,     /**< Automatically handled by the peripheral */
    NRF_LFRC_POWER_CONTROL_POWER_UP   = LFRC_PWRUPCTRL_CTRL_PowerUp,  /**< Power up */
    NRF_LFRC_POWER_CONTROL_POWER_DOWN = LFRC_PWRUPCTRL_CTRL_PowerDown /**< Power down */
} nrf_lfrc_power_control_t;
#endif

/**
 * @brief Function for retrieving the address of the specified task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  LFRC Task.
 *
 * @return Address of the requested task register.
 */
NRF_STATIC_INLINE uint32_t nrf_lfrc_task_address_get(NRF_LFRC_Type const * p_reg,
                                                     nrf_lfrc_task_t       task);

/**
 * @brief Function for setting the specified task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_lfrc_task_trigger(NRF_LFRC_Type * p_reg, nrf_lfrc_task_t task);

/**
 * @brief Function for retrieving the address of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event LFRC Event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_lfrc_event_address_get(NRF_LFRC_Type const * p_reg,
                                                      nrf_lfrc_event_t      event);

/**
 * @brief Function for clearing the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_lfrc_event_clear(NRF_LFRC_Type * p_reg, nrf_lfrc_event_t event);

/**
 * @brief Function for retrieving the state of the specified event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_lfrc_event_check(NRF_LFRC_Type const * p_reg, nrf_lfrc_event_t event);

/**
 * @brief Function for enabling the specified interrupt.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_lfrc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_lfrc_int_enable(NRF_LFRC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupt.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_lfrc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_lfrc_int_disable(NRF_LFRC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_lfrc_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_lfrc_int_enable_check(NRF_LFRC_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * States of pending interrupt are saved as a bitmask.
 * One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_lfrc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_lfrc_int_pending_get(NRF_LFRC_Type const * p_reg);

#if NRF_LFRC_HAS_STATUSTRIM
/**
 * @brief Function for getting current trimming value of the LFRCOSC.
 *
 * @note Returned value is in 2's complement format, 1.4% steps.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Trim value register contents.
 */
NRF_STATIC_INLINE int32_t nrf_lfrc_trim_status_get(NRF_LFRC_Type const * p_reg);
#endif

#if NRF_LFRC_HAS_STATUSANA
/**
 * @brief Function for checking status of analog module READY signal.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The signal is logic 1.
 * @retval false The signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_lfrc_statusana_ready_check(NRF_LFRC_Type const * p_reg);

/**
 * @brief Function for checking status of analog module SETTLED signal.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The signal is logic 1.
 * @retval false The signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_lfrc_statusana_settled_check(NRF_LFRC_Type const * p_reg);
#endif

#if NRF_LFRC_HAS_CONFIG_CFG
/**
 * @brief Function for override the configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Configuration parameters.
 */
NRF_STATIC_INLINE void nrf_lfrc_config_set(NRF_LFRC_Type *           p_reg,
                                           nrf_lfrc_config_t const * p_config);
#endif

#if NRF_LFRC_HAS_CAL_LENGTH
/**
 * @brief Function for setting measurement cycle count length used while calibration.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] length Number of cycles.
 */
NRF_STATIC_INLINE void nrf_lfrc_cal_cycle_length_set(NRF_LFRC_Type *             p_reg,
                                                     nrf_lfrc_cal_cycle_length_t length);
#endif

#if NRF_LFRC_HAS_CAL_TRIMLIMITLO
/**
 * @brief Function for setting lower trim limit.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] trimlimit TRIM limit value.
 */
NRF_STATIC_INLINE void nrf_lfrc_cal_trim_limit_low_set(NRF_LFRC_Type * p_reg, uint32_t trimlimit);
#endif

#if NRF_LFRC_HAS_CAL_TRIMLIMITHI
/**
 * @brief Function for setting higher trim limit.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] trimlimit TRIM limit value.
 */
NRF_STATIC_INLINE void nrf_lfrc_cal_trim_limit_high_set(NRF_LFRC_Type * p_reg, uint32_t trimlimit);
#endif

#if NRF_LFRC_HAS_CAL_RESULT
/**
 * @brief Function for reading calibration results.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] number Result number.
 *
 * @return Result in 16 MHz clock cycles.
 */
NRF_STATIC_INLINE uint32_t nrf_lfrc_cal_result_get(NRF_LFRC_Type const * p_reg, uint8_t number);
#endif

#if NRF_LFRC_HAS_CAL_NHI
/**
 * @brief Function for reading number of cycles when the CAL signal is high.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of cycles.
 */
NRF_STATIC_INLINE uint16_t nrf_lfrc_cal_num_of_cycles_get(NRF_LFRC_Type const * p_reg);
#endif

#if NRF_LFRC_HAS_PWRUPCTRL
/**
 * @brief Function for power control set.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] pwrctrl Power up control.
 */
NRF_STATIC_INLINE void nrf_lfrc_power_control_set(NRF_LFRC_Type *          p_reg,
                                                  nrf_lfrc_power_control_t pwrctrl);
#endif

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE uint32_t nrf_lfrc_task_address_get(NRF_LFRC_Type const * p_reg,
                                                     nrf_lfrc_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_lfrc_task_trigger(NRF_LFRC_Type * p_reg, nrf_lfrc_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_lfrc_event_address_get(NRF_LFRC_Type const * p_reg,
                                                      nrf_lfrc_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_lfrc_event_clear(NRF_LFRC_Type * p_reg, nrf_lfrc_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_lfrc_event_check(NRF_LFRC_Type const * p_reg, nrf_lfrc_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_lfrc_int_enable(NRF_LFRC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_lfrc_int_disable(NRF_LFRC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_lfrc_int_enable_check(NRF_LFRC_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_lfrc_int_pending_get(NRF_LFRC_Type const * p_reg)
{
    return p_reg->INTPEND;
}

#if NRF_LFRC_HAS_STATUSTRIM
NRF_STATIC_INLINE int32_t nrf_lfrc_trim_status_get(NRF_LFRC_Type const * p_reg)
{
    uint32_t raw_measurement = (p_reg->STATUSTRIM & LFRC_STATUSTRIM_VAL_Msk) >>
                                    LFRC_STATUSTRIM_VAL_Pos;

    /* Register is only 6 bits length so perform sign bit extension */
    if (raw_measurement > (LFRC_STATUSTRIM_VAL_Msk >> 1))
    {
        raw_measurement |= ~LFRC_STATUSTRIM_VAL_Msk;
    }

    return (int32_t)raw_measurement;
}
#endif

#if NRF_LFRC_HAS_STATUSANA
NRF_STATIC_INLINE bool nrf_lfrc_statusana_ready_check(NRF_LFRC_Type const * p_reg)
{
    return (bool)(p_reg->STATUSANA & LFRC_STATUSANA_READY_Msk);
}

NRF_STATIC_INLINE bool nrf_lfrc_statusana_settled_check(NRF_LFRC_Type const * p_reg)
{
    return (bool)(p_reg->STATUSANA & LFRC_STATUSANA_SETTLED_Msk);
}
#endif

#if NRF_LFRC_HAS_CONFIG_CFG
NRF_STATIC_INLINE void nrf_lfrc_config_set(NRF_LFRC_Type *           p_reg,
                                           nrf_lfrc_config_t const * p_config)
{
#if NRF_LFRC_HAS_DOUBLE_TAIL_CURRENT
    p_reg->CONFIG.CFG =
        ((p_config->doubletailcurrent_en  << LFRC_CONFIG_CFG_DOUBLETAILCURRENT_Pos)  &
              LFRC_CONFIG_CFG_DOUBLETAILCURRENT_Msk)
#endif
#if NRF_LFRC_HAS_CONTINUOUS_TAIL_BIAS
      | ((p_config->continuoustailbias_en << LFRC_CONFIG_CFG_CONTINUOUSTAILBIAS_Pos) &
              LFRC_CONFIG_CFG_CONTINUOUSTAILBIAS_Msk)
#endif
#if NRF_LFRC_HAS_RETENTION
      | ((p_config->retention_en          << LFRC_CONFIG_CFG_ENABLERETENTION_Pos)    &
              LFRC_CONFIG_CFG_ENABLERETENTION_Msk)
#endif
#if NRF_LFRC_HAS_CFG_SPARE
      | ((p_config->spare_en              << LFRC_CONFIG_CFG_SPARE_Pos)              &
              LFRC_CONFIG_CFG_SPARE_Msk);
#endif
}
#endif //NRF_LFRC_HAS_CONFIG_CFG

#if NRF_LFRC_HAS_CAL_LENGTH
NRF_STATIC_INLINE void nrf_lfrc_cal_cycle_length_set(NRF_LFRC_Type *             p_reg,
                                                     nrf_lfrc_cal_cycle_length_t length)
{
    p_reg->CAL.LENGTH = (uint32_t)length << LFRC_CAL_LENGTH_LENGTH_Pos;
}
#endif

#if NRF_LFRC_HAS_CAL_TRIMLIMITLO
NRF_STATIC_INLINE void nrf_lfrc_cal_trim_limit_low_set(NRF_LFRC_Type * p_reg, uint32_t trimlimit)
{
    p_reg->CAL.TRIMLIMITLO = (trimlimit << LFRC_CAL_TRIMLIMITLO_VALUE_Pos) &
                                   LFRC_CAL_TRIMLIMITLO_VALUE_Msk;
}
#endif

#if NRF_LFRC_HAS_CAL_TRIMLIMITHI
NRF_STATIC_INLINE void nrf_lfrc_cal_trim_limit_high_set(NRF_LFRC_Type * p_reg, uint32_t trimlimit)
{
    p_reg->CAL.TRIMLIMITHI = (trimlimit << LFRC_CAL_TRIMLIMITHI_VALUE_Pos) &
                                   LFRC_CAL_TRIMLIMITHI_VALUE_Msk;
}
#endif

#if NRF_LFRC_HAS_CAL_RESULT
NRF_STATIC_INLINE uint32_t nrf_lfrc_cal_result_get(NRF_LFRC_Type const * p_reg, uint8_t number)
{
    NRFX_ASSERT(number < LFRC_CAL_RESULT_MaxCount);
    return p_reg->CAL.RESULT[number];
}
#endif

#if NRF_LFRC_HAS_CAL_NHI
NRF_STATIC_INLINE uint16_t nrf_lfrc_cal_num_of_cycles_get(NRF_LFRC_Type const * p_reg)
{
    return (uint16_t)p_reg->CAL.NHI;
}
#endif

#if NRF_LFRC_HAS_PWRUPCTRL
NRF_STATIC_INLINE void nrf_lfrc_power_control_set(NRF_LFRC_Type *          p_reg,
                                                  nrf_lfrc_power_control_t pwrctrl)
{
    p_reg->PWRUPCTRL = (uint32_t)pwrctrl << LFRC_PWRUPCTRL_CTRL_Pos;
}
#endif
#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_LFRC_H__
