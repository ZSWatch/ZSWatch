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

#ifndef NRF_GPIOTE_H__
#define NRF_GPIOTE_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NRF_GPIOTE0) && defined(NRF_GPIOTE)
#define NRF_GPIOTE0 NRF_GPIOTE
#endif

#if !defined(GPIOTE0_CH_NUM) && defined(GPIOTE_CH_NUM) && \
    (defined(NRF_GPIOTE) || defined(NRF_GPIOTE0))
#define GPIOTE0_CH_NUM GPIOTE_CH_NUM
#endif

#if !defined(GPIOTE1_CH_NUM) && defined(GPIOTE_CH_NUM) && defined(NRF_GPIOTE1)
#define GPIOTE1_CH_NUM GPIOTE_CH_NUM
#endif

#if !defined(GPIOTE0_AVAILABLE_GPIO_PORTS) && defined(GPIO_COUNT) && \
    (defined(NRF_GPIOTE) || defined(NRF_GPIOTE0))
#define GPIOTE0_AVAILABLE_GPIO_PORTS NRFX_BIT_MASK(GPIO_COUNT)
#endif

#if !defined(GPIOTE1_AVAILABLE_GPIO_PORTS) && defined(GPIO_COUNT) && defined(NRF_GPIOTE1)
#define GPIOTE1_AVAILABLE_GPIO_PORTS NRFX_BIT_MASK(GPIO_COUNT)
#endif

/* Internal macro used for NRF_GPIOTE_INT_IN_MASK. */
#define NRF_GPIOTE_INT_IN(idx, _) NRFX_CONCAT(NRF_GPIOTE_INT_IN, idx, _MASK)

/**
* @defgroup nrf_gpiote_hal GPIOTE HAL
* @{
* @ingroup nrf_gpiote
* @brief   Hardware access layer (HAL) for managing the GPIOTE peripheral.
*/

/**
 * @brief Macro for getting a pointer to the structure of registers of the GPIOTE peripheral.
 *
 * @param[in] idx GPIOTE instance index.
 *
 * @return Pointer to the structure of registers of the GPIOTE peripheral.
 */
#define NRF_GPIOTE_INST_GET(idx) NRFX_CONCAT(NRF_, GPIOTE, idx)

#if defined(GPIOTE_CONFIG_PORT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Mask for covering port and pin bits in registers. */
#define GPIOTE_CONFIG_PORT_PIN_Msk (GPIOTE_CONFIG_PORT_Msk | GPIOTE_CONFIG_PSEL_Msk)
#else
#define GPIOTE_CONFIG_PORT_PIN_Msk GPIOTE_CONFIG_PSEL_Msk
#endif

#if defined(GPIOTE_LATENCY_LATENCY_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the latency setting. */
#define NRF_GPIOTE_HAS_LATENCY 1
#else
#define NRF_GPIOTE_HAS_LATENCY 0
#endif

#if defined(GPIOTE_IRQ_GROUP) || defined(GPIOTE130_IRQ_GROUP) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GPIOTE interrupt groups are present. */
#define NRF_GPIOTE_HAS_INT_GROUPS 1
#else
#define NRF_GPIOTE_HAS_INT_GROUPS 0
#endif

#if defined(GPIOTE_IRQ_GROUP) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating which interrupt group to use. Empty if there are no groups. */
#define NRF_GPIOTE_IRQ_GROUP GPIOTE_IRQ_GROUP
#elif defined(GPIOTE130_IRQ_GROUP)
#define NRF_GPIOTE_IRQ_GROUP GPIOTE130_IRQ_GROUP
#else
#define NRF_GPIOTE_IRQ_GROUP
#endif

#if defined(GPIOTE_INTENSET0_PORT0SECURE_Msk) || defined(GPIOTE_INTENSET0_PORT0NONSECURE_Msk)
#if defined(GPIOTE_SECURE_SUFFIX_OVERRIDE)
#define NRF_GPIOTE_SECURE_SUFFIX GPIOTE_SECURE_SUFFIX_OVERRIDE
#else
#if defined(NRF_TRUSTZONE_NONSECURE)
#define NRF_GPIOTE_SECURE_SUFFIX NONSECURE
#else
#define NRF_GPIOTE_SECURE_SUFFIX SECURE
#endif // defined(NRF_TRUSTZONE_NONSECURE)
#endif // !defined(GPIOTE_SECURE_SUFFIX_OVERRIDE)
#else
/** @brief Symbol indicating a TrustZone suffix added to the register name. */
#define NRF_GPIOTE_SECURE_SUFFIX
#endif // defined(GPIOTE_INTENSET0_PORT0SECURE_Msk) || defined(GPIOTE_INTENSET0_PORT0NONSECURE_Msk)

#if defined(GPIOTE_INTENSET0_IN0_Msk)
#define NRF_GPIOTE_PORT_ID GPIOTE_PORT_ID
#endif

#if defined(NRF_GPIOTE_PORT_ID)
#define NRF_GPIOTE_EVENTS_PORT_REG EVENTS_PORT[NRF_GPIOTE_PORT_ID].NRF_GPIOTE_SECURE_SUFFIX

#define NRF_GPIOTE_INT_PORT_MASK_NAME     \
    NRFX_CONCAT(GPIOTE_INTENSET,          \
                NRF_GPIOTE_IRQ_GROUP,     \
                _PORT,                    \
                NRF_GPIOTE_PORT_ID,       \
                NRF_GPIOTE_SECURE_SUFFIX, \
                _Msk)
#else
/** @brief Symbol indicating a name of PORT event register to be used. */
#define NRF_GPIOTE_EVENTS_PORT_REG    EVENTS_PORT

/** @brief Symbol specifying interrupt bitmask associated with the PORT event. */
#define NRF_GPIOTE_INT_PORT_MASK_NAME GPIOTE_INTENSET_PORT_Msk
#endif

/** @brief Polarity for the GPIOTE channel. */
typedef enum
{
    NRF_GPIOTE_POLARITY_NONE   = GPIOTE_CONFIG_POLARITY_None,   ///< None.
    NRF_GPIOTE_POLARITY_LOTOHI = GPIOTE_CONFIG_POLARITY_LoToHi, ///< Low to high.
    NRF_GPIOTE_POLARITY_HITOLO = GPIOTE_CONFIG_POLARITY_HiToLo, ///< High to low.
    NRF_GPIOTE_POLARITY_TOGGLE = GPIOTE_CONFIG_POLARITY_Toggle, ///< Toggle.
} nrf_gpiote_polarity_t;

/** @brief Initial output value for the GPIOTE channel. */
typedef enum
{
    NRF_GPIOTE_INITIAL_VALUE_LOW  = GPIOTE_CONFIG_OUTINIT_Low,  ///< Low to high.
    NRF_GPIOTE_INITIAL_VALUE_HIGH = GPIOTE_CONFIG_OUTINIT_High, ///< High to low.
} nrf_gpiote_outinit_t;

#if NRF_GPIOTE_HAS_LATENCY
/** @brief Latency setting. */
typedef enum
{
    NRF_GPIOTE_LATENCY_LOWPOWER   = GPIOTE_LATENCY_LATENCY_LowPower,   ///< Low Power.
    NRF_GPIOTE_LATENCY_LOWLATENCY = GPIOTE_LATENCY_LATENCY_LowLatency, ///< Low Latency.
} nrf_gpiote_latency_t;
#endif

/** @brief GPIOTE tasks. */
typedef enum
{
    NRF_GPIOTE_TASK_OUT_0 = offsetof(NRF_GPIOTE_Type, TASKS_OUT[0]), ///< Out task 0.
    NRF_GPIOTE_TASK_OUT_1 = offsetof(NRF_GPIOTE_Type, TASKS_OUT[1]), ///< Out task 1.
    NRF_GPIOTE_TASK_OUT_2 = offsetof(NRF_GPIOTE_Type, TASKS_OUT[2]), ///< Out task 2.
    NRF_GPIOTE_TASK_OUT_3 = offsetof(NRF_GPIOTE_Type, TASKS_OUT[3]), ///< Out task 3.
#if (GPIOTE_CH_NUM > 4) || defined(__NRFX_DOXYGEN__)
    NRF_GPIOTE_TASK_OUT_4 = offsetof(NRF_GPIOTE_Type, TASKS_OUT[4]), ///< Out task 4.
    NRF_GPIOTE_TASK_OUT_5 = offsetof(NRF_GPIOTE_Type, TASKS_OUT[5]), ///< Out task 5.
    NRF_GPIOTE_TASK_OUT_6 = offsetof(NRF_GPIOTE_Type, TASKS_OUT[6]), ///< Out task 6.
    NRF_GPIOTE_TASK_OUT_7 = offsetof(NRF_GPIOTE_Type, TASKS_OUT[7]), ///< Out task 7.
#endif
#if defined(GPIOTE_FEATURE_SET_PRESENT) || defined(__NRFX_DOXYGEN__)
    NRF_GPIOTE_TASK_SET_0 = offsetof(NRF_GPIOTE_Type, TASKS_SET[0]), ///< Set task 0.
    NRF_GPIOTE_TASK_SET_1 = offsetof(NRF_GPIOTE_Type, TASKS_SET[1]), ///< Set task 1.
    NRF_GPIOTE_TASK_SET_2 = offsetof(NRF_GPIOTE_Type, TASKS_SET[2]), ///< Set task 2.
    NRF_GPIOTE_TASK_SET_3 = offsetof(NRF_GPIOTE_Type, TASKS_SET[3]), ///< Set task 3.
    NRF_GPIOTE_TASK_SET_4 = offsetof(NRF_GPIOTE_Type, TASKS_SET[4]), ///< Set task 4.
    NRF_GPIOTE_TASK_SET_5 = offsetof(NRF_GPIOTE_Type, TASKS_SET[5]), ///< Set task 5.
    NRF_GPIOTE_TASK_SET_6 = offsetof(NRF_GPIOTE_Type, TASKS_SET[6]), ///< Set task 6.
    NRF_GPIOTE_TASK_SET_7 = offsetof(NRF_GPIOTE_Type, TASKS_SET[7]), ///< Set task 7.
#endif
#if defined(GPIOTE_FEATURE_CLR_PRESENT) || defined(__NRFX_DOXYGEN__)
    NRF_GPIOTE_TASK_CLR_0 = offsetof(NRF_GPIOTE_Type, TASKS_CLR[0]), ///< Clear task 0.
    NRF_GPIOTE_TASK_CLR_1 = offsetof(NRF_GPIOTE_Type, TASKS_CLR[1]), ///< Clear task 1.
    NRF_GPIOTE_TASK_CLR_2 = offsetof(NRF_GPIOTE_Type, TASKS_CLR[2]), ///< Clear task 2.
    NRF_GPIOTE_TASK_CLR_3 = offsetof(NRF_GPIOTE_Type, TASKS_CLR[3]), ///< Clear task 3.
    NRF_GPIOTE_TASK_CLR_4 = offsetof(NRF_GPIOTE_Type, TASKS_CLR[4]), ///< Clear task 4.
    NRF_GPIOTE_TASK_CLR_5 = offsetof(NRF_GPIOTE_Type, TASKS_CLR[5]), ///< Clear task 5.
    NRF_GPIOTE_TASK_CLR_6 = offsetof(NRF_GPIOTE_Type, TASKS_CLR[6]), ///< Clear task 6.
    NRF_GPIOTE_TASK_CLR_7 = offsetof(NRF_GPIOTE_Type, TASKS_CLR[7]), ///< Clear task 7.
#endif
} nrf_gpiote_task_t;

/** @brief GPIOTE events. */
typedef enum
{
    NRF_GPIOTE_EVENT_IN_0 = offsetof(NRF_GPIOTE_Type, EVENTS_IN[0]),               ///< In event 0.
    NRF_GPIOTE_EVENT_IN_1 = offsetof(NRF_GPIOTE_Type, EVENTS_IN[1]),               ///< In event 1.
    NRF_GPIOTE_EVENT_IN_2 = offsetof(NRF_GPIOTE_Type, EVENTS_IN[2]),               ///< In event 2.
    NRF_GPIOTE_EVENT_IN_3 = offsetof(NRF_GPIOTE_Type, EVENTS_IN[3]),               ///< In event 3.
#if (GPIOTE_CH_NUM > 4) || defined(__NRFX_DOXYGEN__)
    NRF_GPIOTE_EVENT_IN_4 = offsetof(NRF_GPIOTE_Type, EVENTS_IN[4]),               ///< In event 4.
    NRF_GPIOTE_EVENT_IN_5 = offsetof(NRF_GPIOTE_Type, EVENTS_IN[5]),               ///< In event 5.
    NRF_GPIOTE_EVENT_IN_6 = offsetof(NRF_GPIOTE_Type, EVENTS_IN[6]),               ///< In event 6.
    NRF_GPIOTE_EVENT_IN_7 = offsetof(NRF_GPIOTE_Type, EVENTS_IN[7]),               ///< In event 7.
#endif
    NRF_GPIOTE_EVENT_PORT = offsetof(NRF_GPIOTE_Type, NRF_GPIOTE_EVENTS_PORT_REG), ///< Port event.
} nrf_gpiote_event_t;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
/** @brief GPIOTE interrupts. */
typedef enum
{
    NRF_GPIOTE_INT_IN0_MASK = NRFX_CONCAT(GPIOTE_INTENSET, NRF_GPIOTE_IRQ_GROUP, _IN0_Msk), ///< GPIOTE interrupt from IN0.
    NRF_GPIOTE_INT_IN1_MASK = NRFX_CONCAT(GPIOTE_INTENSET, NRF_GPIOTE_IRQ_GROUP, _IN1_Msk), ///< GPIOTE interrupt from IN1.
    NRF_GPIOTE_INT_IN2_MASK = NRFX_CONCAT(GPIOTE_INTENSET, NRF_GPIOTE_IRQ_GROUP, _IN2_Msk), ///< GPIOTE interrupt from IN2.
    NRF_GPIOTE_INT_IN3_MASK = NRFX_CONCAT(GPIOTE_INTENSET, NRF_GPIOTE_IRQ_GROUP, _IN3_Msk), ///< GPIOTE interrupt from IN3.
#if (GPIOTE_CH_NUM > 4) || defined(__NRFX_DOXYGEN__)
    NRF_GPIOTE_INT_IN4_MASK = NRFX_CONCAT(GPIOTE_INTENSET, NRF_GPIOTE_IRQ_GROUP, _IN4_Msk), ///< GPIOTE interrupt from IN4.
    NRF_GPIOTE_INT_IN5_MASK = NRFX_CONCAT(GPIOTE_INTENSET, NRF_GPIOTE_IRQ_GROUP, _IN5_Msk), ///< GPIOTE interrupt from IN5.
    NRF_GPIOTE_INT_IN6_MASK = NRFX_CONCAT(GPIOTE_INTENSET, NRF_GPIOTE_IRQ_GROUP, _IN6_Msk), ///< GPIOTE interrupt from IN6.
    NRF_GPIOTE_INT_IN7_MASK = NRFX_CONCAT(GPIOTE_INTENSET, NRF_GPIOTE_IRQ_GROUP, _IN7_Msk), ///< GPIOTE interrupt from IN7.
#endif
#if defined(__CC_ARM) || defined(__NRFX_DOXYGEN__)
    NRF_GPIOTE_INT_PORT_MASK = (int)NRF_GPIOTE_INT_PORT_MASK_NAME,                          ///< GPIOTE interrupt from PORT event.
#else
    NRF_GPIOTE_INT_PORT_MASK = NRF_GPIOTE_INT_PORT_MASK_NAME,
#endif
} nrf_gpiote_int_t;

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

/** @brief Symbol specifying bitmask collecting all IN events interrupts. */
#define NRF_GPIOTE_INT_IN_MASK (NRFX_LISTIFY(GPIOTE_CH_NUM, NRF_GPIOTE_INT_IN, (|), _))

/**
 * @brief Function for activating the specified GPIOTE task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task.
 */
NRF_STATIC_INLINE void nrf_gpiote_task_trigger(NRF_GPIOTE_Type * p_reg, nrf_gpiote_task_t task);

/**
 * @brief Function for getting the address of the specified GPIOTE task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task.
 *
 * @return Address of the specified task.
 */
NRF_STATIC_INLINE uint32_t nrf_gpiote_task_address_get(NRF_GPIOTE_Type const * p_reg,
                                                       nrf_gpiote_task_t       task);

/**
 * @brief Function for getting the state of the specified GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event.
 *
 * @retval true  The event is set.
 * @retval false The event is not set.
 */
NRF_STATIC_INLINE bool nrf_gpiote_event_check(NRF_GPIOTE_Type const * p_reg,
                                              nrf_gpiote_event_t      event);

/**
 * @brief Function for clearing the specified GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event.
 */
NRF_STATIC_INLINE void nrf_gpiote_event_clear(NRF_GPIOTE_Type * p_reg, nrf_gpiote_event_t event);

/**
 * @brief Function for getting the address of the specified GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event.
 *
 * @return Address of the specified event.
 */
NRF_STATIC_INLINE uint32_t nrf_gpiote_event_address_get(NRF_GPIOTE_Type const * p_reg,
                                                        nrf_gpiote_event_t      event);

/**
 * @brief Function for enabling interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_gpiote_int_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_gpiote_int_enable(NRF_GPIOTE_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_gpiote_int_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_gpiote_int_disable(NRF_GPIOTE_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_gpiote_int_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_gpiote_int_enable_check(NRF_GPIOTE_Type const * p_reg,
                                                       uint32_t                mask);

#if NRF_GPIOTE_HAS_INT_GROUPS
/**
 * @brief Function for enabling interrupts in the specified group.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be enabled.
 * @param[in] mask      Mask of interrupts to be enabled.
 *                      Use @ref nrf_gpiote_int_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_gpiote_int_group_enable(NRF_GPIOTE_Type * p_reg,
                                                   uint8_t           group_idx,
                                                   uint32_t          mask);

/**
 * @brief Function for disabling interrupts in the specified group.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be disabled.
 * @param[in] mask      Mask of interrupts to be disabled.
 *                      Use @ref nrf_gpiote_int_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_gpiote_int_group_disable(NRF_GPIOTE_Type * p_reg,
                                                    uint8_t           group_idx,
                                                    uint32_t          mask);

/**
 * @brief Function for checking if the specified interrupts from a given group are enabled.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be checked.
 * @param[in] mask      Mask of interrupts to be checked.
 *                      Use @ref nrf_gpiote_int_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_gpiote_int_group_enable_check(NRF_GPIOTE_Type const * p_reg,
                                                             uint8_t                 group_idx,
                                                             uint32_t                mask);
#endif

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        GPIOTE task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_gpiote_subscribe_set(NRF_GPIOTE_Type * p_reg,
                                                nrf_gpiote_task_t task,
                                                uint8_t           channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        GPIOTE task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_gpiote_subscribe_clear(NRF_GPIOTE_Type * p_reg, nrf_gpiote_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        GPIOTE task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return GPIOTE subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_gpiote_subscribe_get(NRF_GPIOTE_Type const * p_reg,
                                                    nrf_gpiote_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        GPIOTE event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_gpiote_publish_set(NRF_GPIOTE_Type *  p_reg,
                                              nrf_gpiote_event_t event,
                                              uint8_t            channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_gpiote_publish_clear(NRF_GPIOTE_Type * p_reg, nrf_gpiote_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return GPIOTE publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_gpiote_publish_get(NRF_GPIOTE_Type const * p_reg,
                                                  nrf_gpiote_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for enabling a GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Task-Event index.
 */
NRF_STATIC_INLINE void nrf_gpiote_event_enable(NRF_GPIOTE_Type * p_reg, uint32_t idx);

/**
 * @brief Function for disabling a GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Task-Event index.
 */
NRF_STATIC_INLINE void nrf_gpiote_event_disable(NRF_GPIOTE_Type * p_reg, uint32_t idx);

/**
 * @brief Function for configuring a GPIOTE event.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] idx      Task-Event index.
 * @param[in] pin      Pin associated with event.
 * @param[in] polarity Transition that should generate an event.
 */
NRF_STATIC_INLINE void nrf_gpiote_event_configure(NRF_GPIOTE_Type *     p_reg,
                                                  uint32_t              idx,
                                                  uint32_t              pin,
                                                  nrf_gpiote_polarity_t polarity);

/**
 * @brief Function for getting the pin associated with a GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Task-Event index.
 *
 * @return Pin number.
 */
NRF_STATIC_INLINE uint32_t nrf_gpiote_event_pin_get(NRF_GPIOTE_Type const * p_reg, uint32_t idx);

/**
 * @brief Function for getting the polarity associated with a GPIOTE event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Task-Event index.
 *
 * @return Polarity.
 */
NRF_STATIC_INLINE nrf_gpiote_polarity_t nrf_gpiote_event_polarity_get(NRF_GPIOTE_Type const * p_reg,
                                                                      uint32_t                idx);

/**
 * @brief Function for enabling a GPIOTE task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Task-Event index.
 */
NRF_STATIC_INLINE void nrf_gpiote_task_enable(NRF_GPIOTE_Type * p_reg, uint32_t idx);

/**
 * @brief Function for disabling a GPIOTE task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Task-Event index.
 */
NRF_STATIC_INLINE void nrf_gpiote_task_disable(NRF_GPIOTE_Type * p_reg, uint32_t idx);

/**
 * @brief Function for configuring a GPIOTE task.
 *
 * @note  Function is not configuring mode field so task is disabled after this function is called.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] idx      Task-Event index.
 * @param[in] pin      Pin associated with event.
 * @param[in] polarity Transition that should generate an event.
 * @param[in] init_val Initial value of the pin.
 */
NRF_STATIC_INLINE void nrf_gpiote_task_configure(NRF_GPIOTE_Type *     p_reg,
                                                 uint32_t              idx,
                                                 uint32_t              pin,
                                                 nrf_gpiote_polarity_t polarity,
                                                 nrf_gpiote_outinit_t  init_val);

/**
 * @brief Function for forcing the specified state on the pin connected to GPIOTE.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] idx      Task-Event index.
 * @param[in] init_val Pin state.
 */
NRF_STATIC_INLINE void nrf_gpiote_task_force(NRF_GPIOTE_Type *    p_reg,
                                             uint32_t             idx,
                                             nrf_gpiote_outinit_t init_val);

/**
 * @brief Function for resetting a GPIOTE task event configuration to the default state.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Task-Event index.
 */
NRF_STATIC_INLINE void nrf_gpiote_te_default(NRF_GPIOTE_Type * p_reg, uint32_t idx);

/**
 * @brief Function for checking if particular Task-Event is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] idx   Task-Event index.
 *
 * @retval true  The Task-Event mode is set to Task or Event.
 * @retval false The Task-Event mode is set to Disabled.
 */
NRF_STATIC_INLINE bool nrf_gpiote_te_is_enabled(NRF_GPIOTE_Type const * p_reg, uint32_t idx);

/**
 * @brief Function for getting the OUT task associated with the specified GPIOTE channel.
 *
 * @param[in] index Channel index.
 *
 * @return Requested OUT task.
 */
NRF_STATIC_INLINE nrf_gpiote_task_t nrf_gpiote_out_task_get(uint8_t index);

#if defined(GPIOTE_FEATURE_SET_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for getting the SET task associated with the specified GPIOTE channel.
 *
 * @param[in] index Channel index.
 *
 * @return Requested SET task.
 */
NRF_STATIC_INLINE nrf_gpiote_task_t nrf_gpiote_set_task_get(uint8_t index);
#endif

#if defined(GPIOTE_FEATURE_CLR_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for getting the CLR task associated with the specified GPIOTE channel.
 *
 * @param[in] index Channel index.
 *
 * @return Requested CLR task.
 */
NRF_STATIC_INLINE nrf_gpiote_task_t nrf_gpiote_clr_task_get(uint8_t index);
#endif

/**
 * @brief Function for getting the IN event associated with the specified GPIOTE channel.
 *
 * @param[in] index Channel index.
 *
 * @return Requested IN event.
 */
NRF_STATIC_INLINE nrf_gpiote_event_t nrf_gpiote_in_event_get(uint8_t index);

#if NRF_GPIOTE_HAS_LATENCY
/**
 * @brief Function for setting the latency setting.
 *
 * @note Available for event mode with rising or falling edge detection on the pin.
 *       Toggle task mode can only be used with low latency setting.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] latency Latency setting to be set.
 */
NRF_STATIC_INLINE void nrf_gpiote_latency_set(NRF_GPIOTE_Type *    p_reg,
                                              nrf_gpiote_latency_t latency);

/**
 * @brief Function for retrieving the latency setting.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Latency setting.
 */
NRF_STATIC_INLINE nrf_gpiote_latency_t nrf_gpiote_latency_get(NRF_GPIOTE_Type const * p_reg);
#endif

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_gpiote_task_trigger(NRF_GPIOTE_Type * p_reg, nrf_gpiote_task_t task)
{
    *(volatile uint32_t *)((uint32_t)p_reg + task) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_gpiote_task_address_get(NRF_GPIOTE_Type const * p_reg,
                                                       nrf_gpiote_task_t       task)
{
    return ((uint32_t)p_reg + task);
}

NRF_STATIC_INLINE bool nrf_gpiote_event_check(NRF_GPIOTE_Type const * p_reg,
                                              nrf_gpiote_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_gpiote_event_clear(NRF_GPIOTE_Type * p_reg, nrf_gpiote_event_t event)
{
    *((volatile uint32_t *)nrf_gpiote_event_address_get(p_reg, event)) = 0;
    nrf_event_readback((void *)nrf_gpiote_event_address_get(p_reg, event));
}

NRF_STATIC_INLINE uint32_t nrf_gpiote_event_address_get(NRF_GPIOTE_Type const * p_reg,
                                                        nrf_gpiote_event_t      event)
{
    return ((uint32_t)p_reg + event);
}

NRF_STATIC_INLINE void nrf_gpiote_int_enable(NRF_GPIOTE_Type * p_reg, uint32_t mask)
{
    p_reg->NRFX_CONCAT_2(INTENSET, NRF_GPIOTE_IRQ_GROUP) = mask;
}

NRF_STATIC_INLINE void nrf_gpiote_int_disable(NRF_GPIOTE_Type * p_reg, uint32_t mask)
{
    p_reg->NRFX_CONCAT_2(INTENCLR, NRF_GPIOTE_IRQ_GROUP) = mask;
}

NRF_STATIC_INLINE uint32_t nrf_gpiote_int_enable_check(NRF_GPIOTE_Type const * p_reg, uint32_t mask)
{
    return p_reg->NRFX_CONCAT_2(INTENSET, NRF_GPIOTE_IRQ_GROUP) & mask;
}

#if NRF_GPIOTE_HAS_INT_GROUPS
NRF_STATIC_INLINE void nrf_gpiote_int_group_enable(NRF_GPIOTE_Type * p_reg,
                                                   uint8_t           group_idx,
                                                   uint32_t          mask)
{
    switch (group_idx)
    {
        case 0:
            p_reg->INTENSET0 = mask;
            break;
        case 1:
            p_reg->INTENSET1 = mask;
            break;
#if defined(GPIOTE_INTENSET2_IN0_Msk)
        case 2:
            p_reg->INTENSET2 = mask;
            break;
#endif
#if defined(GPIOTE_INTENSET3_IN0_Msk)
        case 3:
            p_reg->INTENSET3 = mask;
            break;
#endif
#if defined(GPIOTE_INTENSET4_IN0_Msk)
        case 4:
            p_reg->INTENSET4 = mask;
            break;
#endif
#if defined(GPIOTE_INTENSET5_IN0_Msk)
        case 5:
            p_reg->INTENSET5 = mask;
            break;
#endif
#if defined(GPIOTE_INTENSET6_IN0_Msk)
        case 6:
            p_reg->INTENSET6 = mask;
            break;
#endif
       default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_gpiote_int_group_disable(NRF_GPIOTE_Type * p_reg,
                                                    uint8_t           group_idx,
                                                    uint32_t          mask)
{
    switch (group_idx)
    {
        case 0:
            p_reg->INTENCLR0 = mask;
            break;
        case 1:
            p_reg->INTENCLR1 = mask;
            break;
#if defined(GPIOTE_INTENCLR2_IN0_Msk)
        case 2:
            p_reg->INTENCLR2 = mask;
            break;
#endif
#if defined(GPIOTE_INTENCLR3_IN0_Msk)
        case 3:
            p_reg->INTENCLR3 = mask;
            break;
#endif
#if defined(GPIOTE_INTENCLR4_IN0_Msk)
        case 4:
            p_reg->INTENCLR4 = mask;
            break;
#endif
#if defined(GPIOTE_INTENCLR5_IN0_Msk)
        case 5:
            p_reg->INTENCLR5 = mask;
            break;
#endif
#if defined(GPIOTE_INTENCLR6_IN0_Msk)
        case 6:
            p_reg->INTENCLR6 = mask;
            break;
#endif
       default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE uint32_t nrf_gpiote_int_group_enable_check(NRF_GPIOTE_Type const * p_reg,
                                                             uint8_t                 group_idx,
                                                             uint32_t                mask)
{
    switch (group_idx)
    {
        case 0:
            return p_reg->INTENSET0 & mask;
        case 1:
            return p_reg->INTENSET1 & mask;
#if defined(GPIOTE_INTENSET2_IN0_Msk)
        case 2:
            return p_reg->INTENSET2 & mask;
#endif
#if defined(GPIOTE_INTENSET3_IN0_Msk)
        case 3:
            return p_reg->INTENSET3 & mask;
#endif
#if defined(GPIOTE_INTENSET4_IN0_Msk)
        case 4:
            return p_reg->INTENSET4 & mask;
#endif
#if defined(GPIOTE_INTENSET5_IN0_Msk)
        case 5:
            return p_reg->INTENSET5 & mask;
#endif
#if defined(GPIOTE_INTENSET6_IN0_Msk)
        case 6:
            return p_reg->INTENSET6 & mask;
#endif
       default:
            NRFX_ASSERT(false);
            return 0;
    }
}
#endif

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_gpiote_subscribe_set(NRF_GPIOTE_Type * p_reg,
                                                nrf_gpiote_task_t task,
                                                uint8_t           channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_gpiote_subscribe_clear(NRF_GPIOTE_Type * p_reg, nrf_gpiote_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_gpiote_subscribe_get(NRF_GPIOTE_Type const * p_reg,
                                                    nrf_gpiote_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}


NRF_STATIC_INLINE void nrf_gpiote_publish_set(NRF_GPIOTE_Type *  p_reg,
                                              nrf_gpiote_event_t event,
                                              uint8_t            channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_gpiote_publish_clear(NRF_GPIOTE_Type * p_reg, nrf_gpiote_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_gpiote_publish_get(NRF_GPIOTE_Type const * p_reg,
                                                  nrf_gpiote_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE void nrf_gpiote_event_enable(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
   p_reg->CONFIG[idx] |= GPIOTE_CONFIG_MODE_Event;
}

NRF_STATIC_INLINE void nrf_gpiote_event_disable(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
   p_reg->CONFIG[idx] &= ~GPIOTE_CONFIG_MODE_Msk;
}

NRF_STATIC_INLINE void nrf_gpiote_event_configure(NRF_GPIOTE_Type *     p_reg,
                                                  uint32_t              idx,
                                                  uint32_t              pin,
                                                  nrf_gpiote_polarity_t polarity)
{
  p_reg->CONFIG[idx] &= ~(GPIOTE_CONFIG_PORT_PIN_Msk | GPIOTE_CONFIG_POLARITY_Msk);
  p_reg->CONFIG[idx] |= ((pin << GPIOTE_CONFIG_PSEL_Pos) & GPIOTE_CONFIG_PORT_PIN_Msk) |
                        ((polarity << GPIOTE_CONFIG_POLARITY_Pos) & GPIOTE_CONFIG_POLARITY_Msk);
}

NRF_STATIC_INLINE uint32_t nrf_gpiote_event_pin_get(NRF_GPIOTE_Type const * p_reg, uint32_t idx)
{
    return ((p_reg->CONFIG[idx] & GPIOTE_CONFIG_PORT_PIN_Msk) >> GPIOTE_CONFIG_PSEL_Pos);
}

NRF_STATIC_INLINE nrf_gpiote_polarity_t nrf_gpiote_event_polarity_get(NRF_GPIOTE_Type const * p_reg,
                                                                      uint32_t                idx)
{
    return (nrf_gpiote_polarity_t)((p_reg->CONFIG[idx] & GPIOTE_CONFIG_POLARITY_Msk) >>
                                   GPIOTE_CONFIG_POLARITY_Pos);
}

NRF_STATIC_INLINE void nrf_gpiote_task_enable(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
    uint32_t final_config = p_reg->CONFIG[idx] | GPIOTE_CONFIG_MODE_Task;
    p_reg->CONFIG[idx] = final_config;
}

NRF_STATIC_INLINE void nrf_gpiote_task_disable(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
    p_reg->CONFIG[idx] &= ~GPIOTE_CONFIG_MODE_Msk;
}

NRF_STATIC_INLINE void nrf_gpiote_task_configure(NRF_GPIOTE_Type *     p_reg,
                                                 uint32_t              idx,
                                                 uint32_t              pin,
                                                 nrf_gpiote_polarity_t polarity,
                                                 nrf_gpiote_outinit_t  init_val)
{
  p_reg->CONFIG[idx] &= ~(GPIOTE_CONFIG_PORT_PIN_Msk |
                          GPIOTE_CONFIG_POLARITY_Msk |
                          GPIOTE_CONFIG_OUTINIT_Msk);

  p_reg->CONFIG[idx] |= ((pin << GPIOTE_CONFIG_PSEL_Pos) & GPIOTE_CONFIG_PORT_PIN_Msk) |
                        ((polarity << GPIOTE_CONFIG_POLARITY_Pos) & GPIOTE_CONFIG_POLARITY_Msk) |
                        ((init_val << GPIOTE_CONFIG_OUTINIT_Pos) & GPIOTE_CONFIG_OUTINIT_Msk);
}

NRF_STATIC_INLINE void nrf_gpiote_task_force(NRF_GPIOTE_Type *    p_reg,
                                             uint32_t             idx,
                                             nrf_gpiote_outinit_t init_val)
{
    p_reg->CONFIG[idx] = (p_reg->CONFIG[idx] & ~GPIOTE_CONFIG_OUTINIT_Msk) |
                         ((init_val << GPIOTE_CONFIG_OUTINIT_Pos) & GPIOTE_CONFIG_OUTINIT_Msk);
}

NRF_STATIC_INLINE void nrf_gpiote_te_default(NRF_GPIOTE_Type * p_reg, uint32_t idx)
{
    p_reg->CONFIG[idx] = 0;
    // CONFIG.MODE needs to be disabled first, only then writes to CONFIG.PSEL have an effect.
    p_reg->CONFIG[idx] = 0;
}

NRF_STATIC_INLINE bool nrf_gpiote_te_is_enabled(NRF_GPIOTE_Type const * p_reg, uint32_t idx)
{
    return (p_reg->CONFIG[idx] & GPIOTE_CONFIG_MODE_Msk) != GPIOTE_CONFIG_MODE_Disabled;
}

NRF_STATIC_INLINE nrf_gpiote_task_t nrf_gpiote_out_task_get(uint8_t index)
{
    NRFX_ASSERT(index < GPIOTE_CH_NUM);
    return (nrf_gpiote_task_t)NRFX_OFFSETOF(NRF_GPIOTE_Type, TASKS_OUT[index]);
}

#if defined(GPIOTE_FEATURE_SET_PRESENT)
NRF_STATIC_INLINE nrf_gpiote_task_t nrf_gpiote_set_task_get(uint8_t index)
{
    NRFX_ASSERT(index < GPIOTE_CH_NUM);
    return (nrf_gpiote_task_t)NRFX_OFFSETOF(NRF_GPIOTE_Type, TASKS_SET[index]);
}
#endif

#if defined(GPIOTE_FEATURE_CLR_PRESENT)
NRF_STATIC_INLINE nrf_gpiote_task_t nrf_gpiote_clr_task_get(uint8_t index)
{
    NRFX_ASSERT(index < GPIOTE_CH_NUM);
    return (nrf_gpiote_task_t)NRFX_OFFSETOF(NRF_GPIOTE_Type, TASKS_CLR[index]);
}
#endif

NRF_STATIC_INLINE nrf_gpiote_event_t nrf_gpiote_in_event_get(uint8_t index)
{
    NRFX_ASSERT(index < GPIOTE_CH_NUM);
    return (nrf_gpiote_event_t)NRFX_OFFSETOF(NRF_GPIOTE_Type, EVENTS_IN[index]);
}

#if NRF_GPIOTE_HAS_LATENCY
NRF_STATIC_INLINE void nrf_gpiote_latency_set(NRF_GPIOTE_Type *    p_reg,
                                              nrf_gpiote_latency_t latency)
{
    p_reg->LATENCY = (latency << GPIOTE_LATENCY_LATENCY_Pos) & GPIOTE_LATENCY_LATENCY_Msk;
}

NRF_STATIC_INLINE nrf_gpiote_latency_t nrf_gpiote_latency_get(NRF_GPIOTE_Type const * p_reg)
{
    return (nrf_gpiote_latency_t)((p_reg->LATENCY & GPIOTE_LATENCY_LATENCY_Msk) >>
                                  GPIOTE_LATENCY_LATENCY_Pos);
}
#endif // NRF_GPIOTE_HAS_LATENCY
#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif
