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

#ifndef NRF_IPCT_H__
#define NRF_IPCT_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_ipct_hal IPCT HAL
 * @{
 * @ingroup nrf_ipct
 * @brief   Hardware access layer for managing the Inter-Processor Communication Transceiver
 *          (IPCT) peripheral.
 */

/** @brief Symbol indicating presence of ACK task and ACKED event. */
#if defined(IPCT_INTEN0_ACKED0_Msk) || defined(__NRFX_DOXYGEN__)
#define NRF_IPCT_HAS_ACK 1
#else
#define NRF_IPCT_HAS_ACK 0
#endif

/** @brief Number of send tasks. */
#define NRF_IPCT_TASKS_SEND_COUNT IPCT_TASKS_SEND_MaxCount

/** @brief Number of recieve tasks. */
#define NRF_IPCT_EVENTS_RECEIVE_COUNT IPCT_EVENTS_RECEIVE_MaxCount

/** @brief IPCT tasks. */
typedef enum
{
    NRF_IPCT_TASK_SEND_0  = offsetof(NRF_IPCT_Type, TASKS_SEND[0]),  /**< Send [0] task. */
    NRF_IPCT_TASK_SEND_1  = offsetof(NRF_IPCT_Type, TASKS_SEND[1]),  /**< Send [1] task. */
    NRF_IPCT_TASK_SEND_2  = offsetof(NRF_IPCT_Type, TASKS_SEND[2]),  /**< Send [2] task. */
    NRF_IPCT_TASK_SEND_3  = offsetof(NRF_IPCT_Type, TASKS_SEND[3]),  /**< Send [3] task. */
    NRF_IPCT_TASK_SEND_4  = offsetof(NRF_IPCT_Type, TASKS_SEND[4]),  /**< Send [4] task. */
#if NRF_IPCT_TASKS_SEND_COUNT > 5
    NRF_IPCT_TASK_SEND_5  = offsetof(NRF_IPCT_Type, TASKS_SEND[5]),  /**< Send [5] task. */
    NRF_IPCT_TASK_SEND_6  = offsetof(NRF_IPCT_Type, TASKS_SEND[6]),  /**< Send [6] task. */
    NRF_IPCT_TASK_SEND_7  = offsetof(NRF_IPCT_Type, TASKS_SEND[7]),  /**< Send [7] task. */
    NRF_IPCT_TASK_SEND_8  = offsetof(NRF_IPCT_Type, TASKS_SEND[8]),  /**< Send [8] task. */
    NRF_IPCT_TASK_SEND_9  = offsetof(NRF_IPCT_Type, TASKS_SEND[9]),  /**< Send [9] task. */
    NRF_IPCT_TASK_SEND_10 = offsetof(NRF_IPCT_Type, TASKS_SEND[10]), /**< Send [10] task. */
    NRF_IPCT_TASK_SEND_11 = offsetof(NRF_IPCT_Type, TASKS_SEND[11]), /**< Send [11] task. */
    NRF_IPCT_TASK_SEND_12 = offsetof(NRF_IPCT_Type, TASKS_SEND[12]), /**< Send [12] task. */
    NRF_IPCT_TASK_SEND_13 = offsetof(NRF_IPCT_Type, TASKS_SEND[13]), /**< Send [13] task. */
    NRF_IPCT_TASK_SEND_14 = offsetof(NRF_IPCT_Type, TASKS_SEND[14]), /**< Send [14] task. */
    NRF_IPCT_TASK_SEND_15 = offsetof(NRF_IPCT_Type, TASKS_SEND[15]), /**< Send [15] task. */
#endif
#if NRF_IPCT_HAS_ACK
    NRF_IPCT_TASK_ACK_0   = offsetof(NRF_IPCT_Type, TASKS_ACK[0]),   /**< Acknowledge the RECEIVE[0] task. */
    NRF_IPCT_TASK_ACK_1   = offsetof(NRF_IPCT_Type, TASKS_ACK[1]),   /**< Acknowledge the RECEIVE[1] task. */
    NRF_IPCT_TASK_ACK_2   = offsetof(NRF_IPCT_Type, TASKS_ACK[2]),   /**< Acknowledge the RECEIVE[2] task. */
    NRF_IPCT_TASK_ACK_3   = offsetof(NRF_IPCT_Type, TASKS_ACK[3]),   /**< Acknowledge the RECEIVE[3] task. */
    NRF_IPCT_TASK_ACK_4   = offsetof(NRF_IPCT_Type, TASKS_ACK[4]),   /**< Acknowledge the RECEIVE[4] task. */
    NRF_IPCT_TASK_ACK_5   = offsetof(NRF_IPCT_Type, TASKS_ACK[5]),   /**< Acknowledge the RECEIVE[5] task. */
    NRF_IPCT_TASK_ACK_6   = offsetof(NRF_IPCT_Type, TASKS_ACK[6]),   /**< Acknowledge the RECEIVE[6] task. */
    NRF_IPCT_TASK_ACK_7   = offsetof(NRF_IPCT_Type, TASKS_ACK[7]),   /**< Acknowledge the RECEIVE[7] task. */
    NRF_IPCT_TASK_ACK_8   = offsetof(NRF_IPCT_Type, TASKS_ACK[8]),   /**< Acknowledge the RECEIVE[8] task. */
    NRF_IPCT_TASK_ACK_9   = offsetof(NRF_IPCT_Type, TASKS_ACK[9]),   /**< Acknowledge the RECEIVE[9] task. */
    NRF_IPCT_TASK_ACK_10  = offsetof(NRF_IPCT_Type, TASKS_ACK[10]),  /**< Acknowledge the RECEIVE[10] task. */
    NRF_IPCT_TASK_ACK_11  = offsetof(NRF_IPCT_Type, TASKS_ACK[11]),  /**< Acknowledge the RECEIVE[11] task. */
    NRF_IPCT_TASK_ACK_12  = offsetof(NRF_IPCT_Type, TASKS_ACK[12]),  /**< Acknowledge the RECEIVE[12] task. */
    NRF_IPCT_TASK_ACK_13  = offsetof(NRF_IPCT_Type, TASKS_ACK[13]),  /**< Acknowledge the RECEIVE[13] task. */
    NRF_IPCT_TASK_ACK_14  = offsetof(NRF_IPCT_Type, TASKS_ACK[14]),  /**< Acknowledge the RECEIVE[14] task. */
    NRF_IPCT_TASK_ACK_15  = offsetof(NRF_IPCT_Type, TASKS_ACK[15]),  /**< Acknowledge the RECEIVE[15] task. */
#endif
} nrf_ipct_task_t;

/** @brief IPCT events. */
typedef enum
{
    NRF_IPCT_EVENT_RECEIVE_0  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[0]),  /**< Receive [0] event. */
    NRF_IPCT_EVENT_RECEIVE_1  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[1]),  /**< Receive [1] event. */
    NRF_IPCT_EVENT_RECEIVE_2  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[2]),  /**< Receive [2] event. */
    NRF_IPCT_EVENT_RECEIVE_3  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[3]),  /**< Receive [3] event. */
    NRF_IPCT_EVENT_RECEIVE_4  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[4]),  /**< Receive [4] event. */
#if NRF_IPCT_EVENTS_RECEIVE_COUNT > 5
    NRF_IPCT_EVENT_RECEIVE_5  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[5]),  /**< Receive [5] event. */
    NRF_IPCT_EVENT_RECEIVE_6  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[6]),  /**< Receive [6] event. */
    NRF_IPCT_EVENT_RECEIVE_7  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[7]),  /**< Receive [7] event. */
    NRF_IPCT_EVENT_RECEIVE_8  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[8]),  /**< Receive [8] event. */
    NRF_IPCT_EVENT_RECEIVE_9  = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[9]),  /**< Receive [9] event. */
    NRF_IPCT_EVENT_RECEIVE_10 = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[10]), /**< Receive [10] event. */
    NRF_IPCT_EVENT_RECEIVE_11 = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[11]), /**< Receive [11] event. */
    NRF_IPCT_EVENT_RECEIVE_12 = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[12]), /**< Receive [12] event. */
    NRF_IPCT_EVENT_RECEIVE_13 = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[13]), /**< Receive [13] event. */
    NRF_IPCT_EVENT_RECEIVE_14 = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[14]), /**< Receive [14] event. */
    NRF_IPCT_EVENT_RECEIVE_15 = offsetof(NRF_IPCT_Type, EVENTS_RECEIVE[15]), /**< Receive [15] event. */
#endif
#if NRF_IPCT_HAS_ACK
    NRF_IPCT_EVENT_ACKED_0    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[0]),    /**< Acknowledged event for the SEND[0] task. */
    NRF_IPCT_EVENT_ACKED_1    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[1]),    /**< Acknowledged event for the SEND[1] task. */
    NRF_IPCT_EVENT_ACKED_2    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[2]),    /**< Acknowledged event for the SEND[2] task. */
    NRF_IPCT_EVENT_ACKED_3    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[3]),    /**< Acknowledged event for the SEND[3] task. */
    NRF_IPCT_EVENT_ACKED_4    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[4]),    /**< Acknowledged event for the SEND[4] task. */
    NRF_IPCT_EVENT_ACKED_5    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[5]),    /**< Acknowledged event for the SEND[5] task. */
    NRF_IPCT_EVENT_ACKED_6    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[6]),    /**< Acknowledged event for the SEND[6] task. */
    NRF_IPCT_EVENT_ACKED_7    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[7]),    /**< Acknowledged event for the SEND[7] task. */
    NRF_IPCT_EVENT_ACKED_8    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[8]),    /**< Acknowledged event for the SEND[8] task. */
    NRF_IPCT_EVENT_ACKED_9    = offsetof(NRF_IPCT_Type, EVENTS_ACKED[9]),    /**< Acknowledged event for the SEND[9] task. */
    NRF_IPCT_EVENT_ACKED_10   = offsetof(NRF_IPCT_Type, EVENTS_ACKED[10]),   /**< Acknowledged event for the SEND[10] task. */
    NRF_IPCT_EVENT_ACKED_11   = offsetof(NRF_IPCT_Type, EVENTS_ACKED[11]),   /**< Acknowledged event for the SEND[11] task. */
    NRF_IPCT_EVENT_ACKED_12   = offsetof(NRF_IPCT_Type, EVENTS_ACKED[12]),   /**< Acknowledged event for the SEND[12] task. */
    NRF_IPCT_EVENT_ACKED_13   = offsetof(NRF_IPCT_Type, EVENTS_ACKED[13]),   /**< Acknowledged event for the SEND[13] task. */
    NRF_IPCT_EVENT_ACKED_14   = offsetof(NRF_IPCT_Type, EVENTS_ACKED[14]),   /**< Acknowledged event for the SEND[14] task. */
    NRF_IPCT_EVENT_ACKED_15   = offsetof(NRF_IPCT_Type, EVENTS_ACKED[15]),   /**< Acknowledged event for the SEND[15] task. */
#endif // NRF_IPCT_HAS_ACK
} nrf_ipct_event_t;

#if NRF_IPCT_HAS_ACK
/** @brief IPCT shortcuts. */
typedef enum
{
    NRF_IPCT_SHORT_RECEIVE0_ACK0_MASK   = IPCT_SHORTS_RECEIVE0_ACK0_Msk,   /**< Shortcut between event RECEIVE[0] and task ACK[0]. */
    NRF_IPCT_SHORT_RECEIVE1_ACK1_MASK   = IPCT_SHORTS_RECEIVE1_ACK1_Msk,   /**< Shortcut between event RECEIVE[1] and task ACK[1]. */
    NRF_IPCT_SHORT_RECEIVE2_ACK2_MASK   = IPCT_SHORTS_RECEIVE2_ACK2_Msk,   /**< Shortcut between event RECEIVE[2] and task ACK[2]. */
    NRF_IPCT_SHORT_RECEIVE3_ACK3_MASK   = IPCT_SHORTS_RECEIVE3_ACK3_Msk,   /**< Shortcut between event RECEIVE[3] and task ACK[3]. */
    NRF_IPCT_SHORT_RECEIVE4_ACK4_MASK   = IPCT_SHORTS_RECEIVE4_ACK4_Msk,   /**< Shortcut between event RECEIVE[4] and task ACK[4]. */
    NRF_IPCT_SHORT_RECEIVE5_ACK5_MASK   = IPCT_SHORTS_RECEIVE5_ACK5_Msk,   /**< Shortcut between event RECEIVE[5] and task ACK[5]. */
    NRF_IPCT_SHORT_RECEIVE6_ACK6_MASK   = IPCT_SHORTS_RECEIVE6_ACK6_Msk,   /**< Shortcut between event RECEIVE[6] and task ACK[6]. */
    NRF_IPCT_SHORT_RECEIVE7_ACK7_MASK   = IPCT_SHORTS_RECEIVE7_ACK7_Msk,   /**< Shortcut between event RECEIVE[7] and task ACK[7]. */
    NRF_IPCT_SHORT_RECEIVE8_ACK8_MASK   = IPCT_SHORTS_RECEIVE8_ACK8_Msk,   /**< Shortcut between event RECEIVE[8] and task ACK[8]. */
    NRF_IPCT_SHORT_RECEIVE9_ACK9_MASK   = IPCT_SHORTS_RECEIVE9_ACK9_Msk,   /**< Shortcut between event RECEIVE[9] and task ACK[9]. */
    NRF_IPCT_SHORT_RECEIVE10_ACK10_MASK = IPCT_SHORTS_RECEIVE10_ACK10_Msk, /**< Shortcut between event RECEIVE[10] and task ACK[10]. */
    NRF_IPCT_SHORT_RECEIVE11_ACK11_MASK = IPCT_SHORTS_RECEIVE11_ACK11_Msk, /**< Shortcut between event RECEIVE[11] and task ACK[11]. */
    NRF_IPCT_SHORT_RECEIVE12_ACK12_MASK = IPCT_SHORTS_RECEIVE12_ACK12_Msk, /**< Shortcut between event RECEIVE[12] and task ACK[12]. */
    NRF_IPCT_SHORT_RECEIVE13_ACK13_MASK = IPCT_SHORTS_RECEIVE13_ACK13_Msk, /**< Shortcut between event RECEIVE[13] and task ACK[13]. */
    NRF_IPCT_SHORT_RECEIVE14_ACK14_MASK = IPCT_SHORTS_RECEIVE14_ACK14_Msk, /**< Shortcut between event RECEIVE[14] and task ACK[14]. */
    NRF_IPCT_SHORT_RECEIVE15_ACK15_MASK = IPCT_SHORTS_RECEIVE15_ACK15_Msk, /**< Shortcut between event RECEIVE[15] and task ACK[15]. */
    NRF_IPCT_ALL_SHORTS_MASK            = NRF_IPCT_SHORT_RECEIVE0_ACK0_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE1_ACK1_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE2_ACK2_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE3_ACK3_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE4_ACK4_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE5_ACK5_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE6_ACK6_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE7_ACK7_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE8_ACK8_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE9_ACK9_MASK   |
                                          NRF_IPCT_SHORT_RECEIVE10_ACK10_MASK |
                                          NRF_IPCT_SHORT_RECEIVE11_ACK11_MASK |
                                          NRF_IPCT_SHORT_RECEIVE12_ACK12_MASK |
                                          NRF_IPCT_SHORT_RECEIVE13_ACK13_MASK |
                                          NRF_IPCT_SHORT_RECEIVE14_ACK14_MASK |
                                          NRF_IPCT_SHORT_RECEIVE15_ACK15_MASK, /**< All IPCT shortcuts. */
} nrf_ipct_short_mask_t;
#endif

/** @brief IPCT interrupts. */
typedef enum
{
    NRF_IPCT_INT_RECEIVE_0  = IPCT_INTEN0_RECEIVE0_Msk,  /**< Interrupt for event RECEIVE[0]. */
    NRF_IPCT_INT_RECEIVE_1  = IPCT_INTEN0_RECEIVE1_Msk,  /**< Interrupt for event RECEIVE[1]. */
    NRF_IPCT_INT_RECEIVE_2  = IPCT_INTEN0_RECEIVE2_Msk,  /**< Interrupt for event RECEIVE[2]. */
    NRF_IPCT_INT_RECEIVE_3  = IPCT_INTEN0_RECEIVE3_Msk,  /**< Interrupt for event RECEIVE[3]. */
#if NRF_IPCT_EVENTS_RECEIVE_COUNT > 4
    NRF_IPCT_INT_RECEIVE_4  = IPCT_INTEN0_RECEIVE4_Msk,  /**< Interrupt for event RECEIVE[4]. */
    NRF_IPCT_INT_RECEIVE_5  = IPCT_INTEN0_RECEIVE5_Msk,  /**< Interrupt for event RECEIVE[5]. */
    NRF_IPCT_INT_RECEIVE_6  = IPCT_INTEN0_RECEIVE6_Msk,  /**< Interrupt for event RECEIVE[6]. */
    NRF_IPCT_INT_RECEIVE_7  = IPCT_INTEN0_RECEIVE7_Msk,  /**< Interrupt for event RECEIVE[7]. */
    NRF_IPCT_INT_RECEIVE_8  = IPCT_INTEN0_RECEIVE8_Msk,  /**< Interrupt for event RECEIVE[8]. */
    NRF_IPCT_INT_RECEIVE_9  = IPCT_INTEN0_RECEIVE9_Msk,  /**< Interrupt for event RECEIVE[9]. */
    NRF_IPCT_INT_RECEIVE_10 = IPCT_INTEN0_RECEIVE10_Msk, /**< Interrupt for event RECEIVE[10]. */
    NRF_IPCT_INT_RECEIVE_11 = IPCT_INTEN0_RECEIVE11_Msk, /**< Interrupt for event RECEIVE[11]. */
    NRF_IPCT_INT_RECEIVE_12 = IPCT_INTEN0_RECEIVE12_Msk, /**< Interrupt for event RECEIVE[12]. */
    NRF_IPCT_INT_RECEIVE_13 = IPCT_INTEN0_RECEIVE13_Msk, /**< Interrupt for event RECEIVE[13]. */
    NRF_IPCT_INT_RECEIVE_14 = IPCT_INTEN0_RECEIVE14_Msk, /**< Interrupt for event RECEIVE[14]. */
    NRF_IPCT_INT_RECEIVE_15 = IPCT_INTEN0_RECEIVE15_Msk, /**< Interrupt for event RECEIVE[15]. */
#endif
#if NRF_IPCT_HAS_ACK
    NRF_IPCT_INT_ACKED_0    = IPCT_INTEN0_ACKED0_Msk,    /**< Interrupt for event ACKED[0]. */
    NRF_IPCT_INT_ACKED_1    = IPCT_INTEN0_ACKED1_Msk,    /**< Interrupt for event ACKED[1]. */
    NRF_IPCT_INT_ACKED_2    = IPCT_INTEN0_ACKED2_Msk,    /**< Interrupt for event ACKED[2]. */
    NRF_IPCT_INT_ACKED_3    = IPCT_INTEN0_ACKED3_Msk,    /**< Interrupt for event ACKED[3]. */
    NRF_IPCT_INT_ACKED_4    = IPCT_INTEN0_ACKED4_Msk,    /**< Interrupt for event ACKED[4]. */
    NRF_IPCT_INT_ACKED_5    = IPCT_INTEN0_ACKED5_Msk,    /**< Interrupt for event ACKED[5]. */
    NRF_IPCT_INT_ACKED_6    = IPCT_INTEN0_ACKED6_Msk,    /**< Interrupt for event ACKED[6]. */
    NRF_IPCT_INT_ACKED_7    = IPCT_INTEN0_ACKED7_Msk,    /**< Interrupt for event ACKED[7]. */
    NRF_IPCT_INT_ACKED_8    = IPCT_INTEN0_ACKED8_Msk,    /**< Interrupt for event ACKED[8]. */
    NRF_IPCT_INT_ACKED_9    = IPCT_INTEN0_ACKED9_Msk,    /**< Interrupt for event ACKED[9]. */
    NRF_IPCT_INT_ACKED_10   = IPCT_INTEN0_ACKED10_Msk,   /**< Interrupt for event ACKED[10]. */
    NRF_IPCT_INT_ACKED_11   = IPCT_INTEN0_ACKED11_Msk,   /**< Interrupt for event ACKED[11]. */
    NRF_IPCT_INT_ACKED_12   = IPCT_INTEN0_ACKED12_Msk,   /**< Interrupt for event ACKED[12]. */
    NRF_IPCT_INT_ACKED_13   = IPCT_INTEN0_ACKED13_Msk,   /**< Interrupt for event ACKED[13]. */
    NRF_IPCT_INT_ACKED_14   = IPCT_INTEN0_ACKED14_Msk,   /**< Interrupt for event ACKED[14]. */
    NRF_IPCT_INT_ACKED_15   = IPCT_INTEN0_ACKED15_Msk,   /**< Interrupt for event ACKED[15]. */
#endif
} nrf_ipct_int_mask_t;

/**
 * @brief Function for triggering the specified IPCT task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be triggered.
 */
NRF_STATIC_INLINE void nrf_ipct_task_trigger(NRF_IPCT_Type * p_reg, nrf_ipct_task_t task);

/**
 * @brief Function for getting the address of the specified IPCT task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_ipct_task_address_get(NRF_IPCT_Type const * p_reg,
                                                     nrf_ipct_task_t       task);

/**
 * @brief Function for clearing the specified IPCT event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_ipct_event_clear(NRF_IPCT_Type * p_reg, nrf_ipct_event_t event);

/**
 * @brief Function for retrieving the state of the IPCT event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_ipct_event_check(NRF_IPCT_Type const * p_reg, nrf_ipct_event_t event);

/**
 * @brief Function for getting the address of the specified IPCT event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_ipct_event_address_get(NRF_IPCT_Type const * p_reg,
                                                      nrf_ipct_event_t      event);

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be enabled.
 * @param[in] mask      Mask of interrupts to be enabled.
 *                      Use @ref nrf_ipct_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_ipct_int_enable(NRF_IPCT_Type * p_reg,
                                           uint8_t         group_idx,
                                           uint32_t        mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be disabled.
 * @param[in] mask      Mask of interrupts to be disabled.
 *                      Use @ref nrf_ipct_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_ipct_int_disable(NRF_IPCT_Type * p_reg,
                                            uint8_t         group_idx,
                                            uint32_t        mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be checked.
 * @param[in] mask      Mask of interrupts to be checked.
 *                      Use @ref nrf_ipct_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_ipct_int_enable_check(NRF_IPCT_Type const * p_reg,
                                                     uint8_t               group_idx,
                                                     uint32_t              mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * States of pending interrupt are saved as a bitmask.
 * One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] group_idx Index of interrupt group to be retrieved.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_ipct_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_ipct_int_pending_get(NRF_IPCT_Type const * p_reg,
                                                    uint8_t               group_idx);

/**
 *
 * @brief Function for setting the DPPI subscribe configuration for a given
 *        IPCT task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel DPPI channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_ipct_subscribe_set(NRF_IPCT_Type * p_reg,
                                              nrf_ipct_task_t task,
                                              uint8_t         channel);

/**
 * @brief Function for clearing the DPPI subscribe configuration for a given
 *        IPCT task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_ipct_subscribe_clear(NRF_IPCT_Type * p_reg, nrf_ipct_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        IPCT task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return IPCT subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_ipct_subscribe_get(NRF_IPCT_Type const * p_reg,
                                                  nrf_ipct_task_t       task);

/**
 * @brief Function for setting the DPPI publish configuration for a given
 *        IPCT event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel DPPI channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_ipct_publish_set(NRF_IPCT_Type *  p_reg,
                                           nrf_ipct_event_t event,
                                           uint8_t         channel);

/**
 * @brief Function for clearing the DPPI publish configuration for a given
 *        IPCT event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_ipct_publish_clear(NRF_IPCT_Type * p_reg, nrf_ipct_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        IPCT event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return IPCT publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_ipct_publish_get(NRF_IPCT_Type const * p_reg,
                                                nrf_ipct_event_t      event);

/**
 * @brief Function for getting value of the task overflow status for SEND task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index SEND task index to be checked.
 *
 * @retval true  Task overflow has happened.
 * @retval false Task overflow has not happened.
 */
NRF_STATIC_INLINE bool nrf_ipct_overflow_send_get(NRF_IPCT_Type const * p_reg, uint8_t index);

/**
 * @brief Function for getting SEND task by its index.
 *
 * @param[in] index Index of the SEND task.
 *
 * @return SEND task.
 */
NRF_STATIC_INLINE nrf_ipct_task_t nrf_ipct_send_task_get(uint8_t index);

/**
 * @brief Function for getting ACK task by its index.
 *
 * @param[in] index Index of the ACK task.
 *
 * @return ACK task.
 */
NRF_STATIC_INLINE nrf_ipct_task_t nrf_ipct_ack_task_get(uint8_t index);

/**
 * @brief Function for getting RECEIVE event by its index.
 *
 * @param[in] index Index of the RECEIVE event.
 *
 * @return RECEIVE event.
 */
NRF_STATIC_INLINE nrf_ipct_event_t nrf_ipct_receive_event_get(uint8_t index);

/**
 * @brief Function for getting ACKED event by its index.
 *
 * @param[in] index Index of the ACKED event.
 *
 * @return ACKED event.
 */
NRF_STATIC_INLINE nrf_ipct_event_t nrf_ipct_acked_event_get(uint8_t index);

/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_ipct_shorts_enable(NRF_IPCT_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_ipct_shorts_disable(NRF_IPCT_Type * p_reg, uint32_t mask);

/**
 * @brief Function for setting the configuration of IPCT shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be set.
 */
NRF_STATIC_INLINE void nrf_ipct_shorts_set(NRF_IPCT_Type * p_reg, uint32_t mask);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_ipct_task_trigger(NRF_IPCT_Type * p_reg, nrf_ipct_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_ipct_task_address_get(NRF_IPCT_Type const * p_reg,
                                                     nrf_ipct_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE void nrf_ipct_event_clear(NRF_IPCT_Type * p_reg, nrf_ipct_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
}

NRF_STATIC_INLINE bool nrf_ipct_event_check(NRF_IPCT_Type const * p_reg, nrf_ipct_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_ipct_event_address_get(NRF_IPCT_Type const * p_reg,
                                                      nrf_ipct_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_ipct_int_enable(NRF_IPCT_Type * p_reg,
                                           uint8_t         group_idx,
                                           uint32_t        mask)
{
    switch (group_idx)
    {
#if defined(IPCT_INTENSET0_ResetValue)
        case 0:
            p_reg->INTENSET0 = mask;
            break;
#endif // IPCT_INTENSET0_ResetValue
#if defined(IPCT_INTENSET1_ResetValue)
        case 1:
            p_reg->INTENSET1 = mask;
            break;
#endif // IPCT_INTENSET1_ResetValue
#if defined(IPCT_INTENSET2_ResetValue)
        case 2:
            p_reg->INTENSET2 = mask;
            break;
#endif // IPCT_INTENSET2_ResetValue
#if defined(IPCT_INTENSET3_ResetValue)
        case 3:
            p_reg->INTENSET3 = mask;
            break;
#endif // IPCT_INTENSET3_ResetValue
#if defined(IPCT_INTENSET4_ResetValue)
        case 4:
            p_reg->INTENSET4 = mask;
            break;
#endif // IPCT_INTENSET4_ResetValue
#if defined(IPCT_INTENSET5_ResetValue)
        case 5:
            p_reg->INTENSET5 = mask;
            break;
#endif // IPCT_INTENSET5_ResetValue
#if defined(IPCT_INTENSET6_ResetValue)
        case 6:
            p_reg->INTENSET6 = mask;
            break;
#endif // IPCT_INTENSET6_ResetValue
#if defined(IPCT_INTENSET7_ResetValue)
        case 7:
            p_reg->INTENSET7 = mask;
            break;
#endif // IPCT_INTENSET7_ResetValue
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_ipct_int_disable(NRF_IPCT_Type * p_reg,
                                            uint8_t         group_idx,
                                            uint32_t        mask)
{
    switch (group_idx)
    {
#if defined(IPCT_INTENCLR0_ResetValue)
        case 0:
            p_reg->INTENCLR0 = mask;
            break;
#endif // IPCT_INTENCLR0_ResetValue
#if defined(IPCT_INTENCLR1_ResetValue)
        case 1:
            p_reg->INTENCLR1 = mask;
            break;
#endif // IPCT_INTENCLR1_ResetValue
#if defined(IPCT_INTENCLR2_ResetValue)
        case 2:
            p_reg->INTENCLR2 = mask;
            break;
#endif // IPCT_INTENCLR2_ResetValue
#if defined(IPCT_INTENCLR3_ResetValue)
        case 3:
            p_reg->INTENCLR3 = mask;
            break;
#endif // IPCT_INTENCLR3_ResetValue
#if defined(IPCT_INTENCLR4_ResetValue)
        case 4:
            p_reg->INTENCLR4 = mask;
            break;
#endif // IPCT_INTENCLR4_ResetValue
#if defined(IPCT_INTENCLR5_ResetValue)
        case 5:
            p_reg->INTENCLR5 = mask;
            break;
#endif // IPCT_INTENCLR5_ResetValue
        #if defined(IPCT_INTENCLR6_ResetValue)
        case 6:
            p_reg->INTENCLR6 = mask;
            break;
#endif // IPCT_INTENCLR6_ResetValue
#if defined(IPCT_INTENCLR7_ResetValue)
        case 7:
            p_reg->INTENCLR7 = mask;
            break;
#endif // IPCT_INTENCLR7_ResetValue
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE uint32_t nrf_ipct_int_enable_check(NRF_IPCT_Type const * p_reg,
                                                     uint8_t               group_idx,
                                                     uint32_t              mask)
{
    switch (group_idx)
    {
#if defined(IPCT_INTENSET0_ResetValue)
        case 0:
            return p_reg->INTENSET0 & mask;
#endif // IPCT_INTENSET0_ResetValue
#if defined(IPCT_INTENSET1_ResetValue)
        case 1:
            return p_reg->INTENSET1 & mask;
#endif // IPCT_INTENSET1_ResetValue
#if defined(IPCT_INTENSET2_ResetValue)
        case 2:
            return p_reg->INTENSET2 & mask;
#endif // IPCT_INTENSET2_ResetValue
#if defined(IPCT_INTENSET3_ResetValue)
        case 3:
            return p_reg->INTENSET3 & mask;
#endif // IPCT_INTENSET3_ResetValue
#if defined(IPCT_INTENSET4_ResetValue)
        case 4:
            return p_reg->INTENSET4 & mask;
#endif // IPCT_INTENSET4_ResetValue
#if defined(IPCT_INTENSET5_ResetValue)
        case 5:
            return p_reg->INTENSET5 & mask;
#endif // IPCT_INTENSET5_ResetValue
#if defined(IPCT_INTENSET6_ResetValue)
        case 6:
            return p_reg->INTENSET6 & mask;
#endif // IPCT_INTENSET6_ResetValue
#if defined(IPCT_INTENSET7_ResetValue)
        case 7:
            return p_reg->INTENSET7 & mask;
#endif // IPCT_INTENSET7_ResetValue
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE uint32_t nrf_ipct_int_pending_get(NRF_IPCT_Type const * p_reg,
                                                    uint8_t               group_idx)
{
    switch (group_idx)
    {
#if defined(IPCT_INTPEND0_ResetValue)
        case 0:
            return p_reg->INTPEND0;
#endif // IPCT_INTPEND0_ResetValue
#if defined(IPCT_INTPEND1_ResetValue)
        case 1:
            return p_reg->INTPEND1;
#endif // IPCT_INTPEND1_ResetValue
#if defined(IPCT_INTPEND2_ResetValue)
        case 2:
            return p_reg->INTPEND2;
#endif // IPCT_INTPEND2_ResetValue
#if defined(IPCT_INTPEND3_ResetValue)
        case 3:
            return p_reg->INTPEND3;
#endif // IPCT_INTPEND3_ResetValue
#if defined(IPCT_INTPEND4_ResetValue)
        case 4:
            return p_reg->INTPEND4;
#endif // IPCT_INTPEND4_ResetValue
#if defined(IPCT_INTPEND5_ResetValue)
        case 5:
            return p_reg->INTPEND5;
#endif // IPCT_INTPEND5_ResetValue
#if defined(IPCT_INTPEND6_ResetValue)
        case 6:
            return p_reg->INTPEND6;
#endif // IPCT_INTPEND6_ResetValue
#if defined(IPCT_INTPEND7_ResetValue)
        case 7:
            return p_reg->INTPEND7;
#endif // IPCT_INTPEND7_ResetValue
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_ipct_subscribe_set(NRF_IPCT_Type * p_reg,
                                             nrf_ipct_task_t task,
                                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_ipct_subscribe_clear(NRF_IPCT_Type * p_reg, nrf_ipct_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_ipct_subscribe_get(NRF_IPCT_Type const * p_reg,
                                                  nrf_ipct_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_ipct_publish_set(NRF_IPCT_Type *  p_reg,
                                           nrf_ipct_event_t event,
                                           uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_ipct_publish_clear(NRF_IPCT_Type *  p_reg, nrf_ipct_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_ipct_publish_get(NRF_IPCT_Type const * p_reg,
                                                nrf_ipct_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}

NRF_STATIC_INLINE bool nrf_ipct_overflow_send_get(NRF_IPCT_Type const * p_reg, uint8_t index)
{
    NRFX_ASSERT(index < NRF_IPCT_TASKS_SEND_COUNT);
    return (p_reg->OVERFLOW.SEND & (1uL << index));
}

NRF_STATIC_INLINE nrf_ipct_task_t nrf_ipct_send_task_get(uint8_t index)
{
    return (nrf_ipct_task_t)(NRFX_OFFSETOF(NRF_IPCT_Type, TASKS_SEND[index]));
}

NRF_STATIC_INLINE nrf_ipct_task_t nrf_ipct_ack_task_get(uint8_t index)
{
#if NRF_IPCT_HAS_ACK
    return (nrf_ipct_task_t)(NRFX_OFFSETOF(NRF_IPCT_Type, TASKS_ACK[index]));
#else
    (void)index;
    return (nrf_ipct_task_t)0;
#endif
}

NRF_STATIC_INLINE nrf_ipct_event_t nrf_ipct_receive_event_get(uint8_t index)
{
    return (nrf_ipct_event_t)(NRFX_OFFSETOF(NRF_IPCT_Type, EVENTS_RECEIVE[index]));
}

NRF_STATIC_INLINE nrf_ipct_event_t nrf_ipct_acked_event_get(uint8_t index)
{
#if NRF_IPCT_HAS_ACK
    return (nrf_ipct_event_t)(NRFX_OFFSETOF(NRF_IPCT_Type, EVENTS_ACKED[index]));
#else
    (void)index;
    return (nrf_ipct_event_t)0;
#endif
}

NRF_STATIC_INLINE void nrf_ipct_shorts_enable(NRF_IPCT_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_ipct_shorts_disable(NRF_IPCT_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE void nrf_ipct_shorts_set(NRF_IPCT_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS = mask;
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_IPCT_H__
