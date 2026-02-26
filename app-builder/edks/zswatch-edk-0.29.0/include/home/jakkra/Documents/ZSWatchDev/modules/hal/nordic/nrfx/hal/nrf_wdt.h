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

#ifndef NRF_WDT_H__
#define NRF_WDT_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NRF_WDT0) && defined(NRF_WDT)
#define NRF_WDT0 NRF_WDT
#endif

/**
 * @defgroup nrf_wdt_hal WDT HAL
 * @{
 * @ingroup nrf_wdt
 * @brief   Hardware access layer for managing the Watchdog Timer (WDT) peripheral.
 */

/**
 * @brief Macro getting pointer to the structure of registers of the WDT peripheral.
 *
 * @param[in] idx WDT instance index.
 *
 * @return Pointer to the structure of registers of the WDT peripheral.
 */
#define NRF_WDT_INST_GET(idx) NRFX_CONCAT(NRF_, WDT, idx)

#if defined(WDT_TASKS_STOP_TASKS_STOP_Msk) || defined (__NRFX_DOXYGEN__)
/** @brief Presence of Task STOP functionality. */
#define NRF_WDT_HAS_STOP 1
#else
#define NRF_WDT_HAS_STOP 0
#endif

#if defined(WDT_NMIENSET_TIMEOUT_Msk) || defined (__NRFX_DOXYGEN__)
/** @brief Presence of non-maskable interrupt configuration. */
#define NRF_WDT_HAS_NMI 1
#else
#define NRF_WDT_HAS_NMI 0
#endif

/** @brief Number of WDT channels. */
#define NRF_WDT_CHANNEL_NUMBER WDT_RR_MaxCount

/** @brief WDT register reload value. */
#define NRF_WDT_RR_VALUE       WDT_RR_RR_Reload

/** @brief WDT maximum reload value in miliseconds. */
#define NRF_WDT_RR_VALUE_MS    0x07CFFFFFUL

/** @brief WDT tasks. */
typedef enum
{
    NRF_WDT_TASK_START = offsetof(NRF_WDT_Type, TASKS_START), /**< Task for starting WDT. */
#if NRF_WDT_HAS_STOP
    NRF_WDT_TASK_STOP  = offsetof(NRF_WDT_Type, TASKS_STOP),  /**< Task for stopping WDT. */
#endif
} nrf_wdt_task_t;

/** @brief WDT events. */
typedef enum
{
    NRF_WDT_EVENT_TIMEOUT = offsetof(NRF_WDT_Type, EVENTS_TIMEOUT), /**< Event from WDT time-out. */
#if NRF_WDT_HAS_STOP
    NRF_WDT_EVENT_STOPPED = offsetof(NRF_WDT_Type, EVENTS_STOPPED), /**< Event from WDT stop. */
#endif
} nrf_wdt_event_t;

/** @brief WDT behavior in the SLEEP or HALT CPU modes. */
typedef enum
{
    NRF_WDT_BEHAVIOUR_RUN_SLEEP_MASK     = WDT_CONFIG_SLEEP_Msk,  /**< WDT will run when CPU is in SLEEP mode. */
    NRF_WDT_BEHAVIOUR_RUN_HALT_MASK      = WDT_CONFIG_HALT_Msk,   /**< WDT will run when CPU is in HALT mode. */
#if NRF_WDT_HAS_STOP
    NRF_WDT_BEHAVIOUR_STOP_ENABLE_MASK   = WDT_CONFIG_STOPEN_Msk, /**< WDT allows stopping. */
#endif
} nrf_wdt_behaviour_mask_t;

/** @brief WDT reload request registers. */
typedef enum
{
    NRF_WDT_RR0 = 0, /**< Reload request register 0. */
    NRF_WDT_RR1,     /**< Reload request register 1. */
    NRF_WDT_RR2,     /**< Reload request register 2. */
    NRF_WDT_RR3,     /**< Reload request register 3. */
    NRF_WDT_RR4,     /**< Reload request register 4. */
    NRF_WDT_RR5,     /**< Reload request register 5. */
    NRF_WDT_RR6,     /**< Reload request register 6. */
    NRF_WDT_RR7      /**< Reload request register 7. */
} nrf_wdt_rr_register_t;

/** @brief WDT reload request registers mask. */
typedef enum
{
    NRF_WDT_RR0_MASK = (1UL << NRF_WDT_RR0), /**< Mask for reload request register 0. */
    NRF_WDT_RR1_MASK = (1UL << NRF_WDT_RR1), /**< Mask for reload request register 1. */
    NRF_WDT_RR2_MASK = (1UL << NRF_WDT_RR2), /**< Mask for reload request register 2. */
    NRF_WDT_RR3_MASK = (1UL << NRF_WDT_RR3), /**< Mask for reload request register 3. */
    NRF_WDT_RR4_MASK = (1UL << NRF_WDT_RR4), /**< Mask for reload request register 4. */
    NRF_WDT_RR5_MASK = (1UL << NRF_WDT_RR5), /**< Mask for reload request register 5. */
    NRF_WDT_RR6_MASK = (1UL << NRF_WDT_RR6), /**< Mask for reload request register 6. */
    NRF_WDT_RR7_MASK = (1UL << NRF_WDT_RR7), /**< Mask for reload request register 7. */
} nrf_wdt_rr_register_mask_t;

/** @brief WDT interrupts. */
typedef enum
{
    NRF_WDT_INT_TIMEOUT_MASK = WDT_INTENSET_TIMEOUT_Msk, /**< WDT interrupt from time-out event. */
#if NRF_WDT_HAS_STOP
    NRF_WDT_INT_STOPPED_MASK = WDT_INTENSET_STOPPED_Msk, /**< WDT interrupt from stop event. */
#endif
} nrf_wdt_int_mask_t;

/**
 * @brief Function for configuring the watchdog behaviour when the CPU is sleeping or halted.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Watchdog behaviour mask, created using @ref nrf_wdt_behaviour_mask_t.
 */
NRF_STATIC_INLINE void nrf_wdt_behaviour_set(NRF_WDT_Type * p_reg, uint32_t mask);

/**
 * @brief Function for starting the WDT task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task.
 */
NRF_STATIC_INLINE void nrf_wdt_task_trigger(NRF_WDT_Type * p_reg, nrf_wdt_task_t task);

/**
 * @brief Function for clearing the WDT event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event.
 */
NRF_STATIC_INLINE void nrf_wdt_event_clear(NRF_WDT_Type * p_reg, nrf_wdt_event_t event);

/**
 * @brief Function for retrieving the state of the WDT event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_wdt_event_check(NRF_WDT_Type const * p_reg, nrf_wdt_event_t event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_wdt_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_wdt_int_enable(NRF_WDT_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_wdt_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_wdt_int_enable_check(NRF_WDT_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_wdt_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_wdt_int_disable(NRF_WDT_Type * p_reg, uint32_t mask);

#if NRF_WDT_HAS_NMI
/**
 * @brief Function for enabling the specified non-maskable interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_wdt_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_wdt_nmi_int_enable(NRF_WDT_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified non-maskable interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_wdt_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_wdt_nmi_int_enable_check(NRF_WDT_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for disabling a specified non-maskable interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_wdt_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_wdt_nmi_int_disable(NRF_WDT_Type * p_reg, uint32_t mask);
#endif

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        WDT task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_wdt_subscribe_set(NRF_WDT_Type * p_reg,
                                             nrf_wdt_task_t task,
                                             uint8_t        channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        WDT task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_wdt_subscribe_clear(NRF_WDT_Type * p_reg, nrf_wdt_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        WDT task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return WDT subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_wdt_subscribe_get(NRF_WDT_Type const * p_reg,
                                                 nrf_wdt_task_t       task);
/**
 * @brief Function for setting the publish configuration for a given
 *        WDT event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_wdt_publish_set(NRF_WDT_Type *  p_reg,
                                           nrf_wdt_event_t event,
                                           uint8_t         channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        WDT event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_wdt_publish_clear(NRF_WDT_Type * p_reg, nrf_wdt_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        WDT event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return WDT publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_wdt_publish_get(NRF_WDT_Type const * p_reg,
                                               nrf_wdt_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for returning the address of a specific WDT task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task.
 *
 * @return Address of requested task register.
 */
NRF_STATIC_INLINE uint32_t nrf_wdt_task_address_get(NRF_WDT_Type const * p_reg,
                                                    nrf_wdt_task_t       task);

/**
 * @brief Function for returning the address of a specific WDT event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event.
 *
 * @return Address of requested event register.
 */
NRF_STATIC_INLINE uint32_t nrf_wdt_event_address_get(NRF_WDT_Type const * p_reg,
                                                     nrf_wdt_event_t      event);

/**
 * @brief Function for retrieving the watchdog status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The watchdog is started.
 * @retval false The watchdog is not started.
 */
NRF_STATIC_INLINE bool nrf_wdt_started_check(NRF_WDT_Type const * p_reg);

/**
 * @brief Function for retrieving the watchdog reload request status for specified register.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] rr_register Reload request register to be checked.
 *
 * @retval true  Reload request is running.
 * @retval false No reload requests are running.
 */
NRF_STATIC_INLINE bool nrf_wdt_request_status_check(NRF_WDT_Type const *  p_reg,
                                                    nrf_wdt_rr_register_t rr_register);

/**
 * @brief Function for retrieving the watchdog reload requests status mask.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Running reload requests mask, constructed with @ref nrf_wdt_rr_register_mask_t.
 */
NRF_STATIC_INLINE uint32_t nrf_wdt_request_status_get(NRF_WDT_Type const * p_reg);

/**
 * @brief Function for setting the watchdog reload value.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] reload_value Watchdog counter initial value.
 */
NRF_STATIC_INLINE void nrf_wdt_reload_value_set(NRF_WDT_Type * p_reg, uint32_t reload_value);

/**
 * @brief Function for retrieving the watchdog reload value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Reload value.
 */
NRF_STATIC_INLINE uint32_t nrf_wdt_reload_value_get(NRF_WDT_Type const * p_reg);

/**
 * @brief Function for enabling a specific reload request register.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] rr_register Reload request register to be enabled.
 */
NRF_STATIC_INLINE void nrf_wdt_reload_request_enable(NRF_WDT_Type *        p_reg,
                                                     nrf_wdt_rr_register_t rr_register);

/**
 * @brief Function for disabling a specific reload request register.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] rr_register Reload request register to be disabled.
 */
NRF_STATIC_INLINE void nrf_wdt_reload_request_disable(NRF_WDT_Type *        p_reg,
                                                      nrf_wdt_rr_register_t rr_register);

/**
 * @brief Function for retrieving the status of a specific reload request register.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] rr_register Reload request register to be checked.
 *
 * @retval true  The reload request register is enabled.
 * @retval false The reload request register is not enabled.
 */
NRF_STATIC_INLINE bool nrf_wdt_reload_request_enable_check(NRF_WDT_Type const *  p_reg,
                                                           nrf_wdt_rr_register_t rr_register);

/**
 * @brief Function for setting a specific reload request register.
 *
 * @param[in] p_reg       Pointer to the structure of registers of the peripheral.
 * @param[in] rr_register Reload request register to set.
 */
NRF_STATIC_INLINE void nrf_wdt_reload_request_set(NRF_WDT_Type *        p_reg,
                                                  nrf_wdt_rr_register_t rr_register);

#if NRF_WDT_HAS_STOP
/**
 * @brief Function for enabling or disabling stopping the watchdog.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if stopping is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_wdt_task_stop_enable_set(NRF_WDT_Type * p_reg, bool enable);
#endif

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_wdt_behaviour_set(NRF_WDT_Type * p_reg, uint32_t mask)
{
    p_reg->CONFIG = mask;
}

NRF_STATIC_INLINE void nrf_wdt_task_trigger(NRF_WDT_Type * p_reg, nrf_wdt_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x01UL;
}

NRF_STATIC_INLINE void nrf_wdt_event_clear(NRF_WDT_Type * p_reg, nrf_wdt_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_wdt_event_check(NRF_WDT_Type const * p_reg, nrf_wdt_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_wdt_int_enable(NRF_WDT_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE uint32_t nrf_wdt_int_enable_check(NRF_WDT_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE void nrf_wdt_int_disable(NRF_WDT_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

#if NRF_WDT_HAS_NMI
NRF_STATIC_INLINE void nrf_wdt_nmi_int_enable(NRF_WDT_Type * p_reg, uint32_t mask)
{
    p_reg->NMIENSET = mask;
}

NRF_STATIC_INLINE uint32_t nrf_wdt_nmi_int_enable_check(NRF_WDT_Type const * p_reg, uint32_t mask)
{
    return p_reg->NMIENSET & mask;
}

NRF_STATIC_INLINE void nrf_wdt_nmi_int_disable(NRF_WDT_Type * p_reg, uint32_t mask)
{
    p_reg->NMIENCLR = mask;
}
#endif

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_wdt_subscribe_set(NRF_WDT_Type * p_reg,
                                             nrf_wdt_task_t task,
                                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_wdt_subscribe_clear(NRF_WDT_Type * p_reg, nrf_wdt_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_wdt_subscribe_get(NRF_WDT_Type const * p_reg,
                                                 nrf_wdt_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_wdt_publish_set(NRF_WDT_Type *  p_reg,
                                           nrf_wdt_event_t event,
                                           uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_wdt_publish_clear(NRF_WDT_Type * p_reg, nrf_wdt_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_wdt_publish_get(NRF_WDT_Type const * p_reg,
                                               nrf_wdt_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE uint32_t nrf_wdt_task_address_get(NRF_WDT_Type const * p_reg,
                                                    nrf_wdt_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE uint32_t nrf_wdt_event_address_get(NRF_WDT_Type const * p_reg,
                                                     nrf_wdt_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_wdt_started_check(NRF_WDT_Type const * p_reg)
{
#if defined(WDT_RUNSTATUS_RUNSTATUS_Msk)
    return (bool)(p_reg->RUNSTATUS & WDT_RUNSTATUS_RUNSTATUS_Msk);
#else
    return (bool)(p_reg->RUNSTATUS & WDT_RUNSTATUS_RUNSTATUSWDT_Msk);
#endif
}

NRF_STATIC_INLINE bool nrf_wdt_request_status_check(NRF_WDT_Type const *  p_reg,
                                                    nrf_wdt_rr_register_t rr_register)
{
    return (bool)(((p_reg->REQSTATUS) >> rr_register) & 0x1UL);
}

NRF_STATIC_INLINE uint32_t nrf_wdt_request_status_get(NRF_WDT_Type const * p_reg)
{
    return p_reg->REQSTATUS;
}

NRF_STATIC_INLINE void nrf_wdt_reload_value_set(NRF_WDT_Type * p_reg, uint32_t reload_value)
{
    p_reg->CRV = reload_value;
}

NRF_STATIC_INLINE uint32_t nrf_wdt_reload_value_get(NRF_WDT_Type const * p_reg)
{
    return (uint32_t)p_reg->CRV;
}

NRF_STATIC_INLINE void nrf_wdt_reload_request_enable(NRF_WDT_Type *        p_reg,
                                                     nrf_wdt_rr_register_t rr_register)
{
    p_reg->RREN |= 0x1UL << rr_register;
}

NRF_STATIC_INLINE void nrf_wdt_reload_request_disable(NRF_WDT_Type *        p_reg,
                                                      nrf_wdt_rr_register_t rr_register)
{
    p_reg->RREN &= ~(0x1UL << rr_register);
}

NRF_STATIC_INLINE bool nrf_wdt_reload_request_enable_check(NRF_WDT_Type const *  p_reg,
                                                           nrf_wdt_rr_register_t rr_register)
{
    return (bool)(p_reg->RREN & (0x1UL << rr_register));
}

NRF_STATIC_INLINE void nrf_wdt_reload_request_set(NRF_WDT_Type *        p_reg,
                                                  nrf_wdt_rr_register_t rr_register)
{
    p_reg->RR[rr_register] = NRF_WDT_RR_VALUE;
}

#if NRF_WDT_HAS_STOP
NRF_STATIC_INLINE void nrf_wdt_task_stop_enable_set(NRF_WDT_Type * p_reg, bool enable)
{
    p_reg->TSEN = enable ? NRF_WDT_RR_VALUE : 0;
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_WDT_H__
