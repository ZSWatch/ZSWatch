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

#ifndef NRF_BELLBOARD_H__
#define NRF_BELLBOARD_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_bellboard_hal BELLBOARD HAL
 * @{
 * @ingroup nrf_bellboard
 * @brief   Hardware access layer for managing the BELLBOARD peripheral.
 */

/** @brief Symbol specifying maximum number of available events triggered. */
#define NRF_BELLBOARD_EVENTS_TRIGGERED_COUNT BELLBOARD_EVENTS_TRIGGERED_MaxCount

/** @brief BELLBOARD events. */
typedef enum
{
    NRF_BELLBOARD_EVENT_TRIGGERED_0  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[0]),  /**< Triggered 0 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_1  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[1]),  /**< Triggered 1 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_2  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[2]),  /**< Triggered 2 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_3  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[3]),  /**< Triggered 3 event.*/
#if NRF_BELLBOARD_EVENTS_TRIGGERED_COUNT > 4
    NRF_BELLBOARD_EVENT_TRIGGERED_4  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[4]),  /**< Triggered 4 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_5  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[5]),  /**< Triggered 5 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_6  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[6]),  /**< Triggered 6 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_7  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[7]),  /**< Triggered 7 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_8  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[8]),  /**< Triggered 8 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_9  = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[9]),  /**< Triggered 9 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_10 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[10]), /**< Triggered 10 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_11 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[11]), /**< Triggered 11 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_12 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[12]), /**< Triggered 12 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_13 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[13]), /**< Triggered 13 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_14 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[14]), /**< Triggered 14 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_15 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[15]), /**< Triggered 15 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_16 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[16]), /**< Triggered 16 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_17 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[17]), /**< Triggered 17 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_18 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[18]), /**< Triggered 18 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_19 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[19]), /**< Triggered 19 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_20 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[20]), /**< Triggered 20 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_21 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[21]), /**< Triggered 21 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_22 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[22]), /**< Triggered 22 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_23 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[23]), /**< Triggered 23 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_24 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[24]), /**< Triggered 24 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_25 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[25]), /**< Triggered 25 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_26 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[26]), /**< Triggered 26 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_27 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[27]), /**< Triggered 27 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_28 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[28]), /**< Triggered 28 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_29 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[29]), /**< Triggered 29 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_30 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[30]), /**< Triggered 30 event.*/
    NRF_BELLBOARD_EVENT_TRIGGERED_31 = offsetof(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[31]), /**< Triggered 31 event.*/
#endif // BELLBOARD_TASKS_TRIGGER_MaxCount > 4
} nrf_bellboard_event_t;

/** @brief BELLBOARD interrupts. */
typedef enum
{
    NRF_BELLBOARD_INT_TRIGGERED_0_MASK  = BELLBOARD_INTENSET0_TRIGGERED0_Msk,  /**< Triggered 0 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_1_MASK  = BELLBOARD_INTENSET0_TRIGGERED1_Msk,  /**< Triggered 1 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_2_MASK  = BELLBOARD_INTENSET0_TRIGGERED2_Msk,  /**< Triggered 2 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_3_MASK  = BELLBOARD_INTENSET0_TRIGGERED3_Msk,  /**< Triggered 3 interrupt mask. */
#if NRF_BELLBOARD_EVENTS_TRIGGERED_COUNT > 4
    NRF_BELLBOARD_INT_TRIGGERED_4_MASK  = BELLBOARD_INTENSET0_TRIGGERED4_Msk,  /**< Triggered 4 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_5_MASK  = BELLBOARD_INTENSET0_TRIGGERED5_Msk,  /**< Triggered 5 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_6_MASK  = BELLBOARD_INTENSET0_TRIGGERED6_Msk,  /**< Triggered 6 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_7_MASK  = BELLBOARD_INTENSET0_TRIGGERED7_Msk,  /**< Triggered 7 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_8_MASK  = BELLBOARD_INTENSET0_TRIGGERED8_Msk,  /**< Triggered 8 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_9_MASK  = BELLBOARD_INTENSET0_TRIGGERED9_Msk,  /**< Triggered 9 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_10_MASK = BELLBOARD_INTENSET0_TRIGGERED10_Msk, /**< Triggered 10 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_11_MASK = BELLBOARD_INTENSET0_TRIGGERED11_Msk, /**< Triggered 11 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_12_MASK = BELLBOARD_INTENSET0_TRIGGERED12_Msk, /**< Triggered 12 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_13_MASK = BELLBOARD_INTENSET0_TRIGGERED13_Msk, /**< Triggered 13 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_14_MASK = BELLBOARD_INTENSET0_TRIGGERED14_Msk, /**< Triggered 14 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_15_MASK = BELLBOARD_INTENSET0_TRIGGERED15_Msk, /**< Triggered 15 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_16_MASK = BELLBOARD_INTENSET0_TRIGGERED16_Msk, /**< Triggered 16 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_17_MASK = BELLBOARD_INTENSET0_TRIGGERED17_Msk, /**< Triggered 17 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_18_MASK = BELLBOARD_INTENSET0_TRIGGERED18_Msk, /**< Triggered 18 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_19_MASK = BELLBOARD_INTENSET0_TRIGGERED19_Msk, /**< Triggered 19 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_20_MASK = BELLBOARD_INTENSET0_TRIGGERED20_Msk, /**< Triggered 20 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_21_MASK = BELLBOARD_INTENSET0_TRIGGERED21_Msk, /**< Triggered 21 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_22_MASK = BELLBOARD_INTENSET0_TRIGGERED22_Msk, /**< Triggered 22 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_23_MASK = BELLBOARD_INTENSET0_TRIGGERED23_Msk, /**< Triggered 23 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_24_MASK = BELLBOARD_INTENSET0_TRIGGERED24_Msk, /**< Triggered 24 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_25_MASK = BELLBOARD_INTENSET0_TRIGGERED25_Msk, /**< Triggered 25 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_26_MASK = BELLBOARD_INTENSET0_TRIGGERED26_Msk, /**< Triggered 26 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_27_MASK = BELLBOARD_INTENSET0_TRIGGERED27_Msk, /**< Triggered 27 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_28_MASK = BELLBOARD_INTENSET0_TRIGGERED28_Msk, /**< Triggered 28 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_29_MASK = BELLBOARD_INTENSET0_TRIGGERED29_Msk, /**< Triggered 29 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_30_MASK = BELLBOARD_INTENSET0_TRIGGERED30_Msk, /**< Triggered 30 interrupt mask. */
    NRF_BELLBOARD_INT_TRIGGERED_31_MASK = BELLBOARD_INTENSET0_TRIGGERED31_Msk, /**< Triggered 31 interrupt mask. */
#endif // BELLBOARD_TASKS_TRIGGER_MaxCount > 4
} nrf_bellboard_int_mask_t;

/** @brief BELLBOARD tasks. */
typedef enum
{
    NRF_BELLBOARD_TASK_TRIGGER_0  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[0]),  /**< Trigger 0 task. */
    NRF_BELLBOARD_TASK_TRIGGER_1  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[1]),  /**< Trigger 1 task. */
    NRF_BELLBOARD_TASK_TRIGGER_2  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[2]),  /**< Trigger 2 task. */
    NRF_BELLBOARD_TASK_TRIGGER_3  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[3]),  /**< Trigger 3 task. */
    NRF_BELLBOARD_TASK_TRIGGER_4  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[4]),  /**< Trigger 4 task. */
#if NRF_BELLBOARD_EVENTS_TRIGGERED_COUNT > 4
    NRF_BELLBOARD_TASK_TRIGGER_5  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[5]),  /**< Trigger 5 task. */
    NRF_BELLBOARD_TASK_TRIGGER_6  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[6]),  /**< Trigger 6 task. */
    NRF_BELLBOARD_TASK_TRIGGER_7  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[7]),  /**< Trigger 7 task. */
    NRF_BELLBOARD_TASK_TRIGGER_8  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[8]),  /**< Trigger 8 task. */
    NRF_BELLBOARD_TASK_TRIGGER_9  = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[9]),  /**< Trigger 9 task. */
    NRF_BELLBOARD_TASK_TRIGGER_10 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[10]), /**< Trigger 10 task. */
    NRF_BELLBOARD_TASK_TRIGGER_11 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[11]), /**< Trigger 11 task. */
    NRF_BELLBOARD_TASK_TRIGGER_12 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[12]), /**< Trigger 12 task. */
    NRF_BELLBOARD_TASK_TRIGGER_13 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[13]), /**< Trigger 13 task. */
    NRF_BELLBOARD_TASK_TRIGGER_14 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[14]), /**< Trigger 14 task. */
    NRF_BELLBOARD_TASK_TRIGGER_15 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[15]), /**< Trigger 15 task. */
    NRF_BELLBOARD_TASK_TRIGGER_16 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[16]), /**< Trigger 16 task. */
    NRF_BELLBOARD_TASK_TRIGGER_17 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[17]), /**< Trigger 17 task. */
    NRF_BELLBOARD_TASK_TRIGGER_18 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[18]), /**< Trigger 18 task. */
    NRF_BELLBOARD_TASK_TRIGGER_19 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[19]), /**< Trigger 19 task. */
    NRF_BELLBOARD_TASK_TRIGGER_20 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[20]), /**< Trigger 20 task. */
    NRF_BELLBOARD_TASK_TRIGGER_21 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[21]), /**< Trigger 21 task. */
    NRF_BELLBOARD_TASK_TRIGGER_22 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[22]), /**< Trigger 22 task. */
    NRF_BELLBOARD_TASK_TRIGGER_23 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[23]), /**< Trigger 23 task. */
    NRF_BELLBOARD_TASK_TRIGGER_24 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[24]), /**< Trigger 24 task. */
    NRF_BELLBOARD_TASK_TRIGGER_25 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[25]), /**< Trigger 25 task. */
    NRF_BELLBOARD_TASK_TRIGGER_26 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[26]), /**< Trigger 26 task. */
    NRF_BELLBOARD_TASK_TRIGGER_27 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[27]), /**< Trigger 27 task. */
    NRF_BELLBOARD_TASK_TRIGGER_28 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[28]), /**< Trigger 28 task. */
    NRF_BELLBOARD_TASK_TRIGGER_29 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[29]), /**< Trigger 29 task. */
    NRF_BELLBOARD_TASK_TRIGGER_30 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[30]), /**< Trigger 30 task. */
    NRF_BELLBOARD_TASK_TRIGGER_31 = offsetof(NRF_BELLBOARD_Type, TASKS_TRIGGER[31]), /**< Trigger 31 task. */
#endif // BELLBOARD_TASKS_TRIGGER_MaxCount > 4
} nrf_bellboard_task_t;

/**
 * @brief Function for triggering the specified BELLBOARD task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be triggered.
 */
NRF_STATIC_INLINE void nrf_bellboard_task_trigger(NRF_BELLBOARD_Type * p_reg,
                                                  nrf_bellboard_task_t task);

/**
 * @brief Function for getting the address of the specified BELLBOARD task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_bellboard_task_address_get(NRF_BELLBOARD_Type const * p_reg,
                                                          nrf_bellboard_task_t       task);

/**
 * @brief Function for getting the specified BELLBOARD TRIGGERED event.
 *
 * @param[in] index Event index.
 *
 * @return The specified BELLBOARD TRIGGERED event.
 */
NRF_STATIC_INLINE nrf_bellboard_event_t nrf_bellboard_triggered_event_get(uint8_t index);

/**
 * @brief Function for getting the specified BELLBOARD TRIGGER task.
 *
 * @param[in] index Task index.
 *
 * @return The specified BELLBOARD TRIGGER task.
 */
NRF_STATIC_INLINE nrf_bellboard_task_t nrf_bellboard_trigger_task_get(uint8_t index);

/**
 * @brief Function for clearing the specified BELLBOARD event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_bellboard_event_clear(NRF_BELLBOARD_Type *  p_reg,
                                                 nrf_bellboard_event_t event);

/**
 * @brief Function for retrieving the state of the BELLBOARD event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_bellboard_event_check(NRF_BELLBOARD_Type const * p_reg,
                                                 nrf_bellboard_event_t      event);

/**
 * @brief Function for getting the address of the specified BELLBOARD event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_bellboard_event_address_get(NRF_BELLBOARD_Type const * p_reg,
                                                           nrf_bellboard_event_t      event);

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be enabled.
 * @param[in] mask      Mask of interrupts to be enabled.
 *                      Use @ref nrf_bellboard_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_bellboard_int_enable(NRF_BELLBOARD_Type * p_reg,
                                                uint8_t              group_idx,
                                                uint32_t             mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be disabled.
 * @param[in] mask      Mask of interrupts to be disabled.
 *                      Use @ref nrf_bellboard_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_bellboard_int_disable(NRF_BELLBOARD_Type * p_reg,
                                                 uint8_t              group_idx,
                                                 uint32_t             mask);


/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be checked.
 * @param[in] mask      Mask of interrupts to be checked.
 *                      Use @ref nrf_bellboard_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_bellboard_int_enable_check(NRF_BELLBOARD_Type const * p_reg,
                                                          uint8_t                    group_idx,
                                                          uint32_t                   mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * States of pending interrupt are saved as a bitmask.
 * One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be retrieved.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_bellboard_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_bellboard_int_pending_get(NRF_BELLBOARD_Type const * p_reg,
                                                         uint8_t                    group_idx);

#ifndef NRF_DECLARE_ONLY
NRF_STATIC_INLINE void nrf_bellboard_task_trigger(NRF_BELLBOARD_Type * p_reg,
                                                  nrf_bellboard_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_bellboard_task_address_get(NRF_BELLBOARD_Type const * p_reg,
                                                          nrf_bellboard_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE nrf_bellboard_event_t nrf_bellboard_triggered_event_get(uint8_t index)
{
    return (nrf_bellboard_event_t)NRFX_OFFSETOF(NRF_BELLBOARD_Type, EVENTS_TRIGGERED[index]);
}

NRF_STATIC_INLINE nrf_bellboard_task_t nrf_bellboard_trigger_task_get(uint8_t index)
{
    return (nrf_bellboard_task_t)NRFX_OFFSETOF(NRF_BELLBOARD_Type, TASKS_TRIGGER[index]);
}

NRF_STATIC_INLINE void nrf_bellboard_event_clear(NRF_BELLBOARD_Type *  p_reg,
                                                 nrf_bellboard_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
}

NRF_STATIC_INLINE bool nrf_bellboard_event_check(NRF_BELLBOARD_Type const * p_reg,
                                                 nrf_bellboard_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_bellboard_event_address_get(NRF_BELLBOARD_Type const * p_reg,
                                                           nrf_bellboard_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_bellboard_int_enable(NRF_BELLBOARD_Type * p_reg,
                                                uint8_t              group_idx,
                                                uint32_t             mask)
{
    switch (group_idx)
    {
        case 0:
            p_reg->INTENSET0 = mask;
            break;
        case 1:
            p_reg->INTENSET1 = mask;
            break;
        case 2:
            p_reg->INTENSET2 = mask;
            break;
        case 3:
            p_reg->INTENSET3 = mask;
            break;
       default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_bellboard_int_disable(NRF_BELLBOARD_Type * p_reg,
                                                 uint8_t              group_idx,
                                                 uint32_t             mask)
{
    switch (group_idx)
    {
        case 0:
            p_reg->INTENCLR0 = mask;
            break;
        case 1:
            p_reg->INTENCLR1 = mask;
            break;
        case 2:
            p_reg->INTENCLR2 = mask;
            break;
        case 3:
            p_reg->INTENCLR3 = mask;
            break;
       default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE uint32_t nrf_bellboard_int_enable_check(NRF_BELLBOARD_Type const * p_reg,
                                                          uint8_t                    group_idx,
                                                          uint32_t                   mask)
{
    switch (group_idx)
    {
        case 0:
            return p_reg->INTENSET0 & mask;
        case 1:
            return p_reg->INTENSET1 & mask;
        case 2:
            return p_reg->INTENSET2 & mask;
        case 3:
            return p_reg->INTENSET3 & mask;
       default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE uint32_t nrf_bellboard_int_pending_get(NRF_BELLBOARD_Type const * p_reg,
                                                         uint8_t                    group_idx)
{
    switch (group_idx)
    {
        case 0:
            return p_reg->INTPEND0;
        case 1:
            return p_reg->INTPEND1;
        case 2:
            return p_reg->INTPEND2;
        case 3:
            return p_reg->INTPEND3;
       default:
            NRFX_ASSERT(false);
            return 0;
    }
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_BELLBOARD_H__
