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

#ifndef NRF_TWIM_H__
#define NRF_TWIM_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TWIM_CLOCKPIN_SDA_NEEDED)
#define NRF_TWIM_CLOCKPIN_SDA_NEEDED 1
#endif

#if defined(TWIM_CLOCKPIN_SCL_NEEDED)
#define NRF_TWIM_CLOCKPIN_SCL_NEEDED 1
#endif

/**
 * @defgroup nrf_twim_hal TWIM HAL
 * @{
 * @ingroup nrf_twim
 * @brief   Hardware access layer for managing the TWIM peripheral.
 */

/**
 * @brief Macro getting pointer to the structure of registers of the TWIM peripheral.
 *
 * @param[in] idx TWIM instance index.
 *
 * @return Pointer to the structure of registers of the TWIM peripheral.
 */
#define NRF_TWIM_INST_GET(idx) NRFX_CONCAT(NRF_, TWIM, idx)

#if defined(TWIM_FREQUENCY_FREQUENCY_K1000) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether 1000 kHz clock frequency is available. */
#define NRF_TWIM_HAS_1000_KHZ_FREQ 1
#else
#define NRF_TWIM_HAS_1000_KHZ_FREQ 0
#endif

#if defined(TWIM_TXD_LIST_LIST_ArrayList) || defined(TWIM_DMA_TX_LIST_TYPE_ArrayList) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether EasyDMA array list feature is present. */
#define NRF_TWIM_HAS_ARRAY_LIST 1
#else
#define NRF_TWIM_HAS_ARRAY_LIST 0
#endif

#if defined(TWIM_DMA_RX_PTR_PTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether dedicated DMA register is present. */
#define NRF_TWIM_HAS_DMA_REG 1
#else
#define NRF_TWIM_HAS_DMA_REG 0
#endif

#if defined(TWIM_DMA_RX_CURRENTAMOUNT_AMOUNT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether DMA CURRENTAMOUNT registers are present. */
#define NRF_TWIM_HAS_DMA_CURRENTAMOUNT_REG 1
#else
#define NRF_TWIM_HAS_DMA_CURRENTAMOUNT_REG 0
#endif

#if (defined(TWIM_TASKS_DMA_RX_START_START_Msk) && defined(TWIM_EVENTS_DMA_RX_END_END_Msk)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TWIM DMA tasks and events are present. */
#define NRF_TWIM_HAS_DMA_TASKS_EVENTS 1
#else
#define NRF_TWIM_HAS_DMA_TASKS_EVENTS 0
#endif

#if defined(TWIM_SHORTS_LASTTX_DMA_RX_START_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TWIM DMA shortcuts are present. */
#define NRF_TWIM_HAS_DMA_SHORTS 1
#else
#define NRF_TWIM_HAS_DMA_SHORTS 0
#endif

#if (defined(TWIM_EVENTS_RXBUSERROR_EVENTS_RXBUSERROR_Msk) || \
     defined(TWIM_EVENTS_DMA_RX_BUSERROR_BUSERROR_Msk)) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TWIM BUSERROR events are present. */
#define NRF_TWIM_HAS_BUS_ERROR_EVENTS 1
#else
#define NRF_TWIM_HAS_BUS_ERROR_EVENTS 0
#endif

#if defined(TWIM_1MBPS_NEEDS_E0E1)
/** @brief 1 Mbps mode requires high-speed pins to be configured with extra high drive. */
#define NRF_TWIM_1MBPS_NEEDS_E0E1
#endif

#if NRF_TWIM_HAS_DMA_REG
/** @brief Max number of RX patterns. */
#define NRF_TWIM_DMA_RX_PATTERN_MAX_COUNT TWIM_DMA_RX_MATCH_CANDIDATE_MaxCount
#endif

/** @brief TWIM tasks. */
typedef enum
{
#if NRF_TWIM_HAS_DMA_TASKS_EVENTS
    NRF_TWIM_TASK_STARTRX         = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.START),           ///< Start TWI receive operation using easyDMA to load the values.
    NRF_TWIM_TASK_STOPRX          = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.STOP),            ///< Stop TWI receive  operation using easyDMA. This does not trigger an END event.
    NRF_TWIM_TASK_STARTTX         = offsetof(NRF_TWIM_Type, TASKS_DMA.TX.START),           ///< Start TWI transmit operation using easyDMA to load the values.
    NRF_TWIM_TASK_STOPTX          = offsetof(NRF_TWIM_Type, TASKS_DMA.TX.STOP),            ///< Stop TWI transmit operation using easyDMA to load the values.
    NRF_TWIM_TASK_ENABLERXMATCH0  = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.ENABLEMATCH[0]),  ///< Enable TWI pattern matching functionality for pattern 0.
    NRF_TWIM_TASK_ENABLERXMATCH1  = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.ENABLEMATCH[1]),  ///< Enable TWI pattern matching functionality for pattern 1.
    NRF_TWIM_TASK_ENABLERXMATCH2  = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.ENABLEMATCH[2]),  ///< Enable TWI pattern matching functionality for pattern 2.
    NRF_TWIM_TASK_ENABLERXMATCH3  = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.ENABLEMATCH[3]),  ///< Enable TWI pattern matching functionality for pattern 3.
    NRF_TWIM_TASK_DISABLERXMATCH0 = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.DISABLEMATCH[0]), ///< Disable TWI pattern matching functionality for pattern 0.
    NRF_TWIM_TASK_DISABLERXMATCH1 = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.DISABLEMATCH[1]), ///< Disable TWI pattern matching functionality for pattern 1.
    NRF_TWIM_TASK_DISABLERXMATCH2 = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.DISABLEMATCH[2]), ///< Disable TWI pattern matching functionality for pattern 2.
    NRF_TWIM_TASK_DISABLERXMATCH3 = offsetof(NRF_TWIM_Type, TASKS_DMA.RX.DISABLEMATCH[3]), ///< Disable TWI pattern matching functionality for pattern 3.
#else
    NRF_TWIM_TASK_STARTRX         = offsetof(NRF_TWIM_Type, TASKS_STARTRX),                ///< Start TWI receive sequence.
    NRF_TWIM_TASK_STARTTX         = offsetof(NRF_TWIM_Type, TASKS_STARTTX),                ///< Start TWI transmit sequence.
#endif
    NRF_TWIM_TASK_STOP            = offsetof(NRF_TWIM_Type, TASKS_STOP),                   ///< Stop TWI transaction.
    NRF_TWIM_TASK_SUSPEND         = offsetof(NRF_TWIM_Type, TASKS_SUSPEND),                ///< Suspend TWI transaction.
    NRF_TWIM_TASK_RESUME          = offsetof(NRF_TWIM_Type, TASKS_RESUME)                  ///< Resume TWI transaction.
} nrf_twim_task_t;

/** @brief TWIM events. */
typedef enum
{
    NRF_TWIM_EVENT_STOPPED    = offsetof(NRF_TWIM_Type, EVENTS_STOPPED),         ///< TWI stopped.
    NRF_TWIM_EVENT_ERROR      = offsetof(NRF_TWIM_Type, EVENTS_ERROR),           ///< TWI error.
    NRF_TWIM_EVENT_SUSPENDED  = offsetof(NRF_TWIM_Type, EVENTS_SUSPENDED),       ///< TWI suspended.
#if NRF_TWIM_HAS_DMA_TASKS_EVENTS
    NRF_TWIM_EVENT_ENDRX      = offsetof(NRF_TWIM_Type, EVENTS_DMA.RX.END),      ///< Receive sequence finished.
    NRF_TWIM_EVENT_RXSTARTED  = offsetof(NRF_TWIM_Type, EVENTS_DMA.RX.READY),    ///< Receive sequence started.
    NRF_TWIM_EVENT_RXMATCH0   = offsetof(NRF_TWIM_Type, EVENTS_DMA.RX.MATCH[0]), ///< Pattern match for pattern 0 detected.
    NRF_TWIM_EVENT_RXMATCH1   = offsetof(NRF_TWIM_Type, EVENTS_DMA.RX.MATCH[1]), ///< Pattern match for pattern 1 detected.
    NRF_TWIM_EVENT_RXMATCH2   = offsetof(NRF_TWIM_Type, EVENTS_DMA.RX.MATCH[2]), ///< Pattern match for pattern 2 detected.
    NRF_TWIM_EVENT_RXMATCH3   = offsetof(NRF_TWIM_Type, EVENTS_DMA.RX.MATCH[3]), ///< Pattern match for pattern 3 detected.
    NRF_TWIM_EVENT_ENDTX      = offsetof(NRF_TWIM_Type, EVENTS_DMA.TX.END),      ///< Transmit sequence finished.
    NRF_TWIM_EVENT_TXSTARTED  = offsetof(NRF_TWIM_Type, EVENTS_DMA.TX.READY),    ///< Transmit sequence started.
#if NRF_TWIM_HAS_BUS_ERROR_EVENTS
    NRF_TWIM_EVENT_RXBUSERROR = offsetof(NRF_TWIM_Type, EVENTS_DMA.RX.BUSERROR), ///< Memory bus error occurred during the RX transfer.
    NRF_TWIM_EVENT_TXBUSERROR = offsetof(NRF_TWIM_Type, EVENTS_DMA.TX.BUSERROR), ///< Memory bus error occurred during the TX transfer.
#endif
#else
    NRF_TWIM_EVENT_RXSTARTED  = offsetof(NRF_TWIM_Type, EVENTS_RXSTARTED),       ///< Receive sequence started.
    NRF_TWIM_EVENT_TXSTARTED  = offsetof(NRF_TWIM_Type, EVENTS_TXSTARTED),       ///< Transmit sequence started.
#if NRF_TWIM_HAS_BUS_ERROR_EVENTS
    NRF_TWIM_EVENT_RXBUSERROR = offsetof(NRF_TWIM_Type, EVENTS_RXBUSERROR),      ///< Memory bus error occurred during the RX transfer.
    NRF_TWIM_EVENT_TXBUSERROR = offsetof(NRF_TWIM_Type, EVENTS_TXBUSERROR),      ///< Memory bus error occurred during the TX transfer.
#endif
#endif // NRF_TWIM_HAS_DMA_TASKS_EVENTS
    NRF_TWIM_EVENT_LASTRX     = offsetof(NRF_TWIM_Type, EVENTS_LASTRX),          ///< Byte boundary, starting to receive the last byte.
    NRF_TWIM_EVENT_LASTTX     = offsetof(NRF_TWIM_Type, EVENTS_LASTTX)           ///< Byte boundary, starting to transmit the last byte.
} nrf_twim_event_t;

/** @brief TWIM shortcuts. */
typedef enum
{
    NRF_TWIM_SHORT_LASTTX_SUSPEND_MASK           = TWIM_SHORTS_LASTTX_SUSPEND_Msk,                     ///< Shortcut between LASTTX event and SUSPEND task.
    NRF_TWIM_SHORT_LASTTX_STOP_MASK              = TWIM_SHORTS_LASTTX_STOP_Msk,                        ///< Shortcut between LASTTX event and STOP task.
    NRF_TWIM_SHORT_LASTRX_STOP_MASK              = TWIM_SHORTS_LASTRX_STOP_Msk,                        ///< Shortcut between LASTRX event and STOP task.
#if NRF_TWIM_HAS_DMA_SHORTS
    NRF_TWIM_SHORT_LASTTX_STARTRX_MASK           = TWIM_SHORTS_LASTTX_DMA_RX_START_Msk,                ///< Shortcut between LASTTX event and STARTRX task.
    NRF_TWIM_SHORT_LASTRX_STARTTX_MASK           = TWIM_SHORTS_LASTRX_DMA_TX_START_Msk,                ///< Shortcut between LASTRX event and STARTTX task.
#else
    NRF_TWIM_SHORT_LASTTX_STARTRX_MASK           = TWIM_SHORTS_LASTTX_STARTRX_Msk,                     ///< Shortcut between LASTTX event and STARTRX task.
    NRF_TWIM_SHORT_LASTRX_STARTTX_MASK           = TWIM_SHORTS_LASTRX_STARTTX_Msk,                     ///< Shortcut between LASTRX event and STARTTX task.
#endif
#if NRF_TWIM_HAS_DMA_TASKS_EVENTS
    NRF_TWIM_SHORT_RXMATCH0_ENABLERXMATCH1_MASK  = TWIM_SHORTS_DMA_RX_MATCH0_DMA_RX_ENABLEMATCH1_Msk,  ///< Shortcut between DMA.RX.MATCH0 event and DMA.RX.ENABLEMATCH1 task.
    NRF_TWIM_SHORT_RXMATCH1_ENABLERXMATCH2_MASK  = TWIM_SHORTS_DMA_RX_MATCH1_DMA_RX_ENABLEMATCH2_Msk,  ///< Shortcut between DMA.RX.MATCH1 event and DMA.RX.ENABLEMATCH2 task.
    NRF_TWIM_SHORT_RXMATCH2_ENABLERXMATCH3_MASK  = TWIM_SHORTS_DMA_RX_MATCH2_DMA_RX_ENABLEMATCH3_Msk,  ///< Shortcut between DMA.RX.MATCH2 event and DMA.RX.ENABLEMATCH0 task.
    NRF_TWIM_SHORT_RXMATCH3_ENABLERXMATCH0_MASK  = TWIM_SHORTS_DMA_RX_MATCH3_DMA_RX_ENABLEMATCH0_Msk,  ///< Shortcut between DMA.RX.MATCH3 event and DMA.RX.ENABLEMATCH1 task.
    NRF_TWIM_SHORT_RXMATCH0_DISABLERXMATCH0_MASK = TWIM_SHORTS_DMA_RX_MATCH0_DMA_RX_DISABLEMATCH0_Msk, ///< Shortcut between DMA.RX.MATCH0 event and DMA.RX.DISABLEMATCH0 task.
    NRF_TWIM_SHORT_RXMATCH1_DISABLERXMATCH1_MASK = TWIM_SHORTS_DMA_RX_MATCH1_DMA_RX_DISABLEMATCH1_Msk, ///< Shortcut between DMA.RX.MATCH1 event and DMA.RX.DISABLEMATCH1 task.
    NRF_TWIM_SHORT_RXMATCH2_DISABLERXMATCH2_MASK = TWIM_SHORTS_DMA_RX_MATCH2_DMA_RX_DISABLEMATCH2_Msk, ///< Shortcut between DMA.RX.MATCH2 event and DMA.RX.DISABLEMATCH2 task.
    NRF_TWIM_SHORT_RXMATCH3_DISABLERXMATCH3_MASK = TWIM_SHORTS_DMA_RX_MATCH3_DMA_RX_DISABLEMATCH3_Msk, ///< Shortcut between DMA.RX.MATCH3 event and DMA.RX.DISABLEMATCH3 task.
#endif
    NRF_TWIM_ALL_SHORTS_MASK                     =
#if NRF_TWIM_HAS_DMA_SHORTS
                                                   TWIM_SHORTS_LASTTX_DMA_RX_START_Msk                |
                                                   TWIM_SHORTS_LASTRX_DMA_TX_START_Msk                |
#else
                                                   TWIM_SHORTS_LASTTX_STARTRX_Msk                     |
                                                   TWIM_SHORTS_LASTRX_STARTTX_Msk                     |
#endif
#if NRF_TWIM_HAS_DMA_TASKS_EVENTS
                                                   TWIM_SHORTS_DMA_RX_MATCH0_DMA_RX_ENABLEMATCH1_Msk  |
                                                   TWIM_SHORTS_DMA_RX_MATCH1_DMA_RX_ENABLEMATCH2_Msk  |
                                                   TWIM_SHORTS_DMA_RX_MATCH2_DMA_RX_ENABLEMATCH3_Msk  |
                                                   TWIM_SHORTS_DMA_RX_MATCH3_DMA_RX_ENABLEMATCH0_Msk  |
                                                   TWIM_SHORTS_DMA_RX_MATCH0_DMA_RX_DISABLEMATCH0_Msk |
                                                   TWIM_SHORTS_DMA_RX_MATCH1_DMA_RX_DISABLEMATCH1_Msk |
                                                   TWIM_SHORTS_DMA_RX_MATCH2_DMA_RX_DISABLEMATCH2_Msk |
                                                   TWIM_SHORTS_DMA_RX_MATCH3_DMA_RX_DISABLEMATCH3_Msk |
#endif
                                                   TWIM_SHORTS_LASTTX_SUSPEND_Msk                     |
                                                   TWIM_SHORTS_LASTTX_STOP_Msk                        |
                                                   TWIM_SHORTS_LASTRX_STOP_Msk                         ///< All TWIM shortcuts.
} nrf_twim_short_mask_t;

/** @brief TWIM interrupts. */
typedef enum
{
    NRF_TWIM_INT_STOPPED_MASK    = TWIM_INTENSET_STOPPED_Msk,       ///< Interrupt on STOPPED event.
    NRF_TWIM_INT_ERROR_MASK      = TWIM_INTENSET_ERROR_Msk,         ///< Interrupt on ERROR event.
    NRF_TWIM_INT_SUSPENDED_MASK  = TWIM_INTENSET_SUSPENDED_Msk,     ///< Interrupt on SUSPENDED event.
#if NRF_TWIM_HAS_DMA_TASKS_EVENTS
    NRF_TWIM_INT_RXSTARTED_MASK  = TWIM_INTENSET_DMARXREADY_Msk,    ///< Interrupt on RXSTARTED event.
    NRF_TWIM_INT_TXSTARTED_MASK  = TWIM_INTENSET_DMATXREADY_Msk,    ///< Interrupt on TXSTARTED event.
    NRF_TWIM_INT_ENDRX_MASK      = TWIM_INTENSET_DMARXEND_Msk,      ///< Interrupt on DMA.RX.END event.
    NRF_TWIM_INT_RXREADY_MASK    = TWIM_INTENSET_DMARXREADY_Msk,    ///< Interrupt on DMA.RX.READY event.
    NRF_TWIM_INT_RXBUSERROR_MASK = TWIM_INTENSET_DMARXBUSERROR_Msk, ///< Interrupt on DMA.RX.BUSERROR event.
    NRF_TWIM_INT_RXMATCH0_MASK   = TWIM_INTENSET_DMARXMATCH0_Msk,   ///< Interrupt on DMA.RX.MATCH0 event.
    NRF_TWIM_INT_RXMATCH1_MASK   = TWIM_INTENSET_DMARXMATCH1_Msk,   ///< Interrupt on DMA.RX.MATCH1 event.
    NRF_TWIM_INT_RXMATCH2_MASK   = TWIM_INTENSET_DMARXMATCH2_Msk,   ///< Interrupt on DMA.RX.MATCH2 event.
    NRF_TWIM_INT_RXMATCH3_MASK   = TWIM_INTENSET_DMARXMATCH3_Msk,   ///< Interrupt on DMA.RX.MATCH3 event.
    NRF_TWIM_INT_ENDTX_MASK      = TWIM_INTENSET_DMATXEND_Msk,      ///< Interrupt on DMA.TX.END event.
    NRF_TWIM_INT_TXREADY_MASK    = TWIM_INTENSET_DMATXREADY_Msk,    ///< Interrupt on DMA.TX.READY event.
    NRF_TWIM_INT_TXBUSERROR_MASK = TWIM_INTENSET_DMATXBUSERROR_Msk, ///< Interrupt on DMA.TX.BUSERROR event.
#else
    NRF_TWIM_INT_RXSTARTED_MASK  = TWIM_INTENSET_RXSTARTED_Msk,     ///< Interrupt on RXSTARTED event.
    NRF_TWIM_INT_TXSTARTED_MASK  = TWIM_INTENSET_TXSTARTED_Msk,     ///< Interrupt on TXSTARTED event.
#endif
    NRF_TWIM_INT_LASTRX_MASK     = TWIM_INTENSET_LASTRX_Msk,        ///< Interrupt on LASTRX event.
    NRF_TWIM_INT_LASTTX_MASK     = TWIM_INTENSET_LASTTX_Msk,        ///< Interrupt on LASTTX event.
    NRF_TWIM_ALL_INTS_MASK       = NRF_TWIM_INT_STOPPED_MASK    |
                                   NRF_TWIM_INT_ERROR_MASK      |
                                   NRF_TWIM_INT_SUSPENDED_MASK  |
                                   NRF_TWIM_INT_RXSTARTED_MASK  |
                                   NRF_TWIM_INT_TXSTARTED_MASK  |
#if NRF_TWIM_HAS_DMA_TASKS_EVENTS
                                   NRF_TWIM_INT_ENDRX_MASK      |
                                   NRF_TWIM_INT_RXREADY_MASK    |
                                   NRF_TWIM_INT_RXBUSERROR_MASK |
                                   NRF_TWIM_INT_RXMATCH0_MASK   |
                                   NRF_TWIM_INT_RXMATCH1_MASK   |
                                   NRF_TWIM_INT_RXMATCH2_MASK   |
                                   NRF_TWIM_INT_RXMATCH3_MASK   |
                                   NRF_TWIM_INT_ENDTX_MASK      |
                                   NRF_TWIM_INT_TXREADY_MASK    |
                                   NRF_TWIM_INT_TXBUSERROR_MASK |
#endif
                                   NRF_TWIM_INT_LASTRX_MASK     |
                                   NRF_TWIM_INT_LASTTX_MASK         ///< All TWIM interrupts.
} nrf_twim_int_mask_t;

/** @brief TWIM master clock frequency. */
typedef enum
{
    NRF_TWIM_FREQ_100K  = TWIM_FREQUENCY_FREQUENCY_K100, ///< 100 kbps.
    NRF_TWIM_FREQ_250K  = TWIM_FREQUENCY_FREQUENCY_K250, ///< 250 kbps.
    NRF_TWIM_FREQ_400K  = TWIM_FREQUENCY_FREQUENCY_K400, ///< 400 kbps.
#if NRF_TWIM_HAS_1000_KHZ_FREQ
    NRF_TWIM_FREQ_1000K = TWIM_FREQUENCY_FREQUENCY_K1000 ///< 1000 kbps.
#endif
} nrf_twim_frequency_t;

/** @brief TWIM error source. */
typedef enum
{
    NRF_TWIM_ERROR_ADDRESS_NACK = TWIM_ERRORSRC_ANACK_Msk,  ///< NACK received after sending the address.
    NRF_TWIM_ERROR_DATA_NACK    = TWIM_ERRORSRC_DNACK_Msk,  ///< NACK received after sending a data byte.
    NRF_TWIM_ERROR_OVERRUN      = TWIM_ERRORSRC_OVERRUN_Msk ///< Overrun error.
                                                            /**< A new byte was received before the previous byte was
                                                             *   handled by peripheral. (previous data is lost). */
} nrf_twim_error_t;


/**
 * @brief Function for activating the specified TWIM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_twim_task_trigger(NRF_TWIM_Type * p_reg,
                                             nrf_twim_task_t task);

/**
 * @brief Function for getting the address of the specified TWIM task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  The specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_task_address_get(NRF_TWIM_Type const * p_reg,
                                                     nrf_twim_task_t       task);

/**
 * @brief Function for clearing the specified TWIM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_twim_event_clear(NRF_TWIM_Type *  p_reg,
                                            nrf_twim_event_t event);

/**
 * @brief Function for retrieving the state of the TWIM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_twim_event_check(NRF_TWIM_Type const * p_reg,
                                            nrf_twim_event_t      event);

/**
 * @brief Function for getting the address of the specified TWIM event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event The specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_event_address_get(NRF_TWIM_Type const * p_reg,
                                                      nrf_twim_event_t      event);

/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_twim_shorts_enable(NRF_TWIM_Type * p_reg,
                                              uint32_t        mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_twim_shorts_disable(NRF_TWIM_Type * p_reg,
                                               uint32_t        mask);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_twim_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_twim_int_enable(NRF_TWIM_Type * p_reg,
                                           uint32_t        mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_twim_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_twim_int_disable(NRF_TWIM_Type * p_reg,
                                            uint32_t        mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_twim_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_int_enable_check(NRF_TWIM_Type const * p_reg, uint32_t mask);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        TWIM task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_twim_subscribe_set(NRF_TWIM_Type * p_reg,
                                              nrf_twim_task_t task,
                                              uint8_t         channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        TWIM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_twim_subscribe_clear(NRF_TWIM_Type * p_reg,
                                                nrf_twim_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        TWIM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return TWIM subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_subscribe_get(NRF_TWIM_Type const * p_reg,
                                                  nrf_twim_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        TWIM event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_twim_publish_set(NRF_TWIM_Type *  p_reg,
                                            nrf_twim_event_t event,
                                            uint8_t         channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        TWIM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_twim_publish_clear(NRF_TWIM_Type *  p_reg,
                                              nrf_twim_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        TWIM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return TWIM publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_publish_get(NRF_TWIM_Type const * p_reg,
                                                nrf_twim_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for enabling the TWIM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_twim_enable(NRF_TWIM_Type * p_reg);

/**
 * @brief Function for disabling the TWIM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_twim_disable(NRF_TWIM_Type * p_reg);

/**
 * @brief Function for configuring TWI pins.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] scl_pin SCL pin number.
 * @param[in] sda_pin SDA pin number.
 */
NRF_STATIC_INLINE void nrf_twim_pins_set(NRF_TWIM_Type * p_reg,
                                         uint32_t        scl_pin,
                                         uint32_t        sda_pin);

/**
 * @brief Function for setting the SCL pin.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   SCL pin number.
 */
NRF_STATIC_INLINE void nrf_twim_scl_pin_set(NRF_TWIM_Type * p_reg, uint32_t pin);

/**
 * @brief Function for setting the SDA pin
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   SDA pin number.
 */
NRF_STATIC_INLINE void nrf_twim_sda_pin_set(NRF_TWIM_Type * p_reg, uint32_t pin);

/**
 * @brief Function for retrieving the SCL pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SCL pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_scl_pin_get(NRF_TWIM_Type const * p_reg);

/**
 * @brief Function for retrieving the SDA pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SDA pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_sda_pin_get(NRF_TWIM_Type const * p_reg);

/**
 * @brief Function for setting the TWI master clock frequency.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] frequency TWI frequency.
 */
NRF_STATIC_INLINE void nrf_twim_frequency_set(NRF_TWIM_Type *      p_reg,
                                              nrf_twim_frequency_t frequency);

/**
 * @brief Function for checking the TWI error source.
 *
 * The error flags are cleared after reading.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask with error source flags.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_errorsrc_get_and_clear(NRF_TWIM_Type * p_reg);

/**
 * @brief Function for setting the address to be used in TWI transfers.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] address Address to be used in transfers.
 */
NRF_STATIC_INLINE void nrf_twim_address_set(NRF_TWIM_Type * p_reg,
                                            uint8_t         address);

/**
 * @brief Function for getting the address to be used in TWI transfers.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Address to be used in TWI transfers.
 */
NRF_STATIC_INLINE uint8_t nrf_twim_address_get(NRF_TWIM_Type const * p_reg);

/**
 * @brief Function for setting the transmit buffer.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the buffer with data to send.
 * @param[in] length   Maximum number of data bytes to transmit.
 */
NRF_STATIC_INLINE void nrf_twim_tx_buffer_set(NRF_TWIM_Type * p_reg,
                                              uint8_t const * p_buffer,
                                              size_t          length);

/**
 * @brief Function for setting the receive buffer.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the buffer for received data.
 * @param[in] length   Maximum number of data bytes to receive.
 */
NRF_STATIC_INLINE void nrf_twim_rx_buffer_set(NRF_TWIM_Type * p_reg,
                                              uint8_t *       p_buffer,
                                              size_t          length);

/**
 * @brief Function for setting the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be set.
 */
NRF_STATIC_INLINE void nrf_twim_shorts_set(NRF_TWIM_Type * p_reg,
                                           uint32_t        mask);

/**
 * @brief Function for getting the shortcut setting.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Current shortcut configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_shorts_get(NRF_TWIM_Type const * p_reg);

/**
 * @brief Function for getting the amount of transmitted bytes.
 *
 * @note In case of NACK error, includes the NACK'ed byte.
 * @note Number of bytes are updated after the END event and each MATCH event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Amount of transmitted bytes.
 */
NRF_STATIC_INLINE size_t nrf_twim_txd_amount_get(NRF_TWIM_Type const * p_reg);

/**
 * @brief Function for getting the amount of received bytes in the last transaction.
 *
 * @note In case of NACK error, includes the NACK'ed byte.
 * @note Number of bytes are updated after the END event and each MATCH event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Amount of received bytes.
 */
NRF_STATIC_INLINE size_t nrf_twim_rxd_amount_get(NRF_TWIM_Type const * p_reg);

#if NRF_TWIM_HAS_DMA_CURRENTAMOUNT_REG
/**
 * @brief Function for getting the amount of transmitted bytes in the current transaction.
 *
 * @note Number of bytes is continuously updated during transmission.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Amount of transmitted bytes.
 */
NRF_STATIC_INLINE size_t nrf_twim_txd_curr_amount_get(NRF_TWIM_Type const * p_reg);

/**
 * @brief Function for getting the amount of received bytes in the current transaction.
 *
 * @note Number of bytes is continuously updated during reception.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Amount of received bytes.
 */
NRF_STATIC_INLINE size_t nrf_twim_rxd_curr_amount_get(NRF_TWIM_Type const * p_reg);
#endif

#if NRF_TWIM_HAS_ARRAY_LIST
/**
 * @brief Function for enabling the TX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_twim_tx_list_enable(NRF_TWIM_Type * p_reg);

/**
 * @brief Function for disabling the TX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_twim_tx_list_disable(NRF_TWIM_Type * p_reg);

/**
 * @brief Function for enabling the RX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_twim_rx_list_enable(NRF_TWIM_Type * p_reg);

/**
 * @brief Function for disabling the RX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_twim_rx_list_disable(NRF_TWIM_Type * p_reg);
#endif

#if NRF_TWIM_HAS_DMA_REG
/**
 * @brief Function for enabling individual pattern match filters.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Index of pattern match filter.
 * @param[in] enable True if pattern match filter is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_twim_rx_pattern_match_enable_set(NRF_TWIM_Type * p_reg,
                                                            uint8_t         index,
                                                            bool            enable);

/**
 * @brief Function for checking if the specified pattern match filter is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 *
 * @retval true  Pattern match filter is enabled.
 * @retval false Pattern match filter is disabled.
 */
NRF_STATIC_INLINE bool nrf_twim_rx_pattern_match_enable_check(NRF_TWIM_Type const * p_reg,
                                                              uint8_t               index);

/**
 * @brief Function for enabling one-shot operation for the specified match filter.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 */
NRF_STATIC_INLINE void nrf_twim_rx_pattern_match_one_shot_enable(NRF_TWIM_Type * p_reg,
                                                                 uint8_t         index);

/**
 * @brief Function for disabling one-shot operation for the specified match filter.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 */
NRF_STATIC_INLINE void nrf_twim_rx_pattern_match_one_shot_disable(NRF_TWIM_Type * p_reg,
                                                                  uint8_t         index);

/**
 * @brief Function for checking if specified pattern match filter is configured as one-shot.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 *
 * @retval true  Pattern match filter is configured as one-shot.
 * @retval false Pattern match filter is configured as continuous.
 */
NRF_STATIC_INLINE bool nrf_twim_rx_pattern_match_one_shot_check(NRF_TWIM_Type const * p_reg,
                                                                uint8_t               index);

/**
 * @brief Function for setting the pattern to be looked for by the specified match filter.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] index   Index of pattern match filter.
 * @param[in] pattern Pattern to be looked for.
 *                    Match will trigger the corresponding event, if enabled.
 */
NRF_STATIC_INLINE void nrf_twim_rx_pattern_match_candidate_set(NRF_TWIM_Type * p_reg,
                                                               uint8_t         index,
                                                               uint32_t        pattern);

/**
 * @brief Function for getting the pattern that the specified match filter is looking for.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 *
 * @return Pattern that the specified match filter is looking for.
 */
NRF_STATIC_INLINE uint32_t nrf_twim_rx_pattern_match_candidate_get(NRF_TWIM_Type const * p_reg,
                                                                   uint8_t               index);
#endif // NRF_TWIM_HAS_DMA_REG

#if NRF_TWIM_HAS_BUS_ERROR_EVENTS
/**
 * @brief Function for enabling RX transaction termination on the detection of a BUSERROR event.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the RX transaction termination is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_twim_rx_terminate_on_bus_error_enable_set(NRF_TWIM_Type * p_reg,
                                                                     bool            enable);

/**
 * @brief Function for checking if the RX transaction termination on the detection
 *        of a BUSERROR event is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Transaction termination on BUSERROR event is enabled.
 * @retval false Transaction termination on BUSERROR event is disabled.
 */
NRF_STATIC_INLINE bool nrf_twim_rx_terminate_on_bus_error_check(NRF_TWIM_Type const * p_reg);

/**
 * @brief Function for enabling TX transaction termination on the detection of a BUSERROR event.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the TX transaction termination is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_twim_tx_terminate_on_bus_error_enable_set(NRF_TWIM_Type * p_reg,
                                                                     bool            enable);

/**
 * @brief Function for checking if the TX transaction termination on the detection
 *        of a BUSERROR event is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Transaction termination on BUSERROR event is enabled.
 * @retval false Transaction termination on BUSERROR event is disabled.
 */
NRF_STATIC_INLINE bool nrf_twim_tx_terminate_on_bus_error_check(NRF_TWIM_Type const * p_reg);
#endif

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_twim_task_trigger(NRF_TWIM_Type * p_reg,
                                             nrf_twim_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_twim_task_address_get(NRF_TWIM_Type const * p_reg,
                                                     nrf_twim_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_twim_event_clear(NRF_TWIM_Type * p_reg,
                                            nrf_twim_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_twim_event_check(NRF_TWIM_Type const * p_reg,
                                            nrf_twim_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_twim_event_address_get(NRF_TWIM_Type const * p_reg,
                                                      nrf_twim_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_twim_shorts_enable(NRF_TWIM_Type * p_reg,
                                              uint32_t mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_twim_shorts_disable(NRF_TWIM_Type * p_reg,
                                               uint32_t mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE void nrf_twim_int_enable(NRF_TWIM_Type * p_reg,
                                           uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_twim_int_disable(NRF_TWIM_Type * p_reg,
                                            uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_twim_int_enable_check(NRF_TWIM_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_twim_subscribe_set(NRF_TWIM_Type * p_reg,
                                              nrf_twim_task_t task,
                                              uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_twim_subscribe_clear(NRF_TWIM_Type * p_reg,
                                                nrf_twim_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_twim_subscribe_get(NRF_TWIM_Type const * p_reg,
                                                  nrf_twim_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_twim_publish_set(NRF_TWIM_Type *  p_reg,
                                            nrf_twim_event_t event,
                                            uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_twim_publish_clear(NRF_TWIM_Type *  p_reg,
                                              nrf_twim_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_twim_publish_get(NRF_TWIM_Type const * p_reg,
                                                nrf_twim_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE void nrf_twim_enable(NRF_TWIM_Type * p_reg)
{
    p_reg->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_twim_disable(NRF_TWIM_Type * p_reg)
{
    p_reg->ENABLE = (TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_twim_pins_set(NRF_TWIM_Type * p_reg,
                                         uint32_t scl_pin,
                                         uint32_t sda_pin)
{
    p_reg->PSEL.SCL = scl_pin;
    p_reg->PSEL.SDA = sda_pin;
}

NRF_STATIC_INLINE void nrf_twim_scl_pin_set(NRF_TWIM_Type * p_reg, uint32_t pin)
{
    p_reg->PSEL.SCL = pin;
}

NRF_STATIC_INLINE void nrf_twim_sda_pin_set(NRF_TWIM_Type * p_reg, uint32_t pin)
{
    p_reg->PSEL.SDA = pin;
}

NRF_STATIC_INLINE uint32_t nrf_twim_scl_pin_get(NRF_TWIM_Type const * p_reg)
{
    return p_reg->PSEL.SCL;
}

NRF_STATIC_INLINE uint32_t nrf_twim_sda_pin_get(NRF_TWIM_Type const * p_reg)
{
    return p_reg->PSEL.SDA;
}

NRF_STATIC_INLINE void nrf_twim_frequency_set(NRF_TWIM_Type * p_reg,
                                              nrf_twim_frequency_t frequency)
{
    p_reg->FREQUENCY = frequency;
}

NRF_STATIC_INLINE uint32_t nrf_twim_errorsrc_get_and_clear(NRF_TWIM_Type * p_reg)
{
    uint32_t error_source = p_reg->ERRORSRC;

    // [error flags are cleared by writing '1' on their position]
    p_reg->ERRORSRC = error_source;

    return error_source;
}

NRF_STATIC_INLINE void nrf_twim_address_set(NRF_TWIM_Type * p_reg,
                                            uint8_t address)
{
    p_reg->ADDRESS = address;
}

NRF_STATIC_INLINE uint8_t nrf_twim_address_get(NRF_TWIM_Type const * p_reg)
{
    return (uint8_t)(p_reg->ADDRESS & TWIM_ADDRESS_ADDRESS_Msk);
}

NRF_STATIC_INLINE void nrf_twim_tx_buffer_set(NRF_TWIM_Type * p_reg,
                                              uint8_t const * p_buffer,
                                              size_t          length)
{
#if NRF_TWIM_HAS_DMA_REG
    p_reg->DMA.TX.PTR    = (uint32_t)p_buffer;
    p_reg->DMA.TX.MAXCNT = length;
#else
    p_reg->TXD.PTR    = (uint32_t)p_buffer;
    p_reg->TXD.MAXCNT = length;
#endif
}

NRF_STATIC_INLINE void nrf_twim_rx_buffer_set(NRF_TWIM_Type * p_reg,
                                              uint8_t * p_buffer,
                                              size_t    length)
{
#if NRF_TWIM_HAS_DMA_REG
    p_reg->DMA.RX.PTR    = (uint32_t)p_buffer;
    p_reg->DMA.RX.MAXCNT = length;
#else
    p_reg->RXD.PTR    = (uint32_t)p_buffer;
    p_reg->RXD.MAXCNT = length;
#endif
}

NRF_STATIC_INLINE void nrf_twim_shorts_set(NRF_TWIM_Type * p_reg,
                                           uint32_t mask)
{
    p_reg->SHORTS = mask;
}

NRF_STATIC_INLINE uint32_t nrf_twim_shorts_get(NRF_TWIM_Type const * p_reg)
{
    return p_reg->SHORTS;
}

NRF_STATIC_INLINE size_t nrf_twim_txd_amount_get(NRF_TWIM_Type const * p_reg)
{
#if NRF_TWIM_HAS_DMA_REG
    return p_reg->DMA.TX.AMOUNT;
#else
    return p_reg->TXD.AMOUNT;
#endif
}

NRF_STATIC_INLINE size_t nrf_twim_rxd_amount_get(NRF_TWIM_Type const * p_reg)
{
#if NRF_TWIM_HAS_DMA_REG
    return p_reg->DMA.RX.AMOUNT;
#else
    return p_reg->RXD.AMOUNT;
#endif
}

#if NRF_TWIM_HAS_DMA_CURRENTAMOUNT_REG
NRF_STATIC_INLINE size_t nrf_twim_txd_curr_amount_get(NRF_TWIM_Type const * p_reg)
{
    return p_reg->DMA.TX.CURRENTAMOUNT;
}

NRF_STATIC_INLINE size_t nrf_twim_rxd_curr_amount_get(NRF_TWIM_Type const * p_reg)
{
    return p_reg->DMA.RX.CURRENTAMOUNT;
}
#endif

#if NRF_TWIM_HAS_ARRAY_LIST
NRF_STATIC_INLINE void nrf_twim_tx_list_enable(NRF_TWIM_Type * p_reg)
{
#if NRF_TWIM_HAS_DMA_REG
    p_reg->DMA.TX.LIST = TWIM_DMA_TX_LIST_TYPE_ArrayList << TWIM_DMA_TX_LIST_TYPE_Pos;
#else
    p_reg->TXD.LIST = TWIM_TXD_LIST_LIST_ArrayList << TWIM_TXD_LIST_LIST_Pos;
#endif
}

NRF_STATIC_INLINE void nrf_twim_tx_list_disable(NRF_TWIM_Type * p_reg)
{
#if NRF_TWIM_HAS_DMA_REG
    p_reg->DMA.TX.LIST = TWIM_DMA_TX_LIST_TYPE_Disabled << TWIM_DMA_TX_LIST_TYPE_Pos;
#else
    p_reg->TXD.LIST = TWIM_TXD_LIST_LIST_Disabled << TWIM_TXD_LIST_LIST_Pos;
#endif
}

NRF_STATIC_INLINE void nrf_twim_rx_list_enable(NRF_TWIM_Type * p_reg)
{
#if NRF_TWIM_HAS_DMA_REG
    p_reg->DMA.RX.LIST = TWIM_DMA_RX_LIST_TYPE_ArrayList << TWIM_DMA_RX_LIST_TYPE_Pos;
#else
    p_reg->RXD.LIST = TWIM_RXD_LIST_LIST_ArrayList << TWIM_RXD_LIST_LIST_Pos;
#endif
}

NRF_STATIC_INLINE void nrf_twim_rx_list_disable(NRF_TWIM_Type * p_reg)
{
#if NRF_TWIM_HAS_DMA_REG
    p_reg->DMA.RX.LIST = TWIM_DMA_RX_LIST_TYPE_Disabled << TWIM_DMA_RX_LIST_TYPE_Pos;
#else
    p_reg->RXD.LIST = TWIM_RXD_LIST_LIST_Disabled << TWIM_RXD_LIST_LIST_Pos;
#endif
}
#endif

#if NRF_TWIM_HAS_DMA_REG
NRF_STATIC_INLINE void nrf_twim_rx_pattern_match_enable_set(NRF_TWIM_Type * p_reg,
                                                            uint8_t         index,
                                                            bool            enable)
{
    NRFX_ASSERT(index < NRF_TWIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                         ~TWIM_DMA_RX_MATCH_CONFIG_ENABLE0_Msk) |
                                         ((enable ?
                                           TWIM_DMA_RX_MATCH_CONFIG_ENABLE0_Enabled :
                                           TWIM_DMA_RX_MATCH_CONFIG_ENABLE0_Disabled)
                                           << TWIM_DMA_RX_MATCH_CONFIG_ENABLE0_Pos));
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                         ~TWIM_DMA_RX_MATCH_CONFIG_ENABLE1_Msk) |
                                         ((enable ?
                                           TWIM_DMA_RX_MATCH_CONFIG_ENABLE1_Enabled :
                                           TWIM_DMA_RX_MATCH_CONFIG_ENABLE1_Disabled)
                                           << TWIM_DMA_RX_MATCH_CONFIG_ENABLE1_Pos));
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                         ~TWIM_DMA_RX_MATCH_CONFIG_ENABLE2_Msk) |
                                         ((enable ?
                                           TWIM_DMA_RX_MATCH_CONFIG_ENABLE2_Enabled :
                                           TWIM_DMA_RX_MATCH_CONFIG_ENABLE2_Disabled)
                                           << TWIM_DMA_RX_MATCH_CONFIG_ENABLE2_Pos));
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                         ~TWIM_DMA_RX_MATCH_CONFIG_ENABLE3_Msk) |
                                         ((enable ?
                                           TWIM_DMA_RX_MATCH_CONFIG_ENABLE3_Enabled :
                                           TWIM_DMA_RX_MATCH_CONFIG_ENABLE3_Disabled)
                                           << TWIM_DMA_RX_MATCH_CONFIG_ENABLE3_Pos));
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE bool nrf_twim_rx_pattern_match_enable_check(NRF_TWIM_Type const * p_reg,
                                                              uint8_t               index)
{
    NRFX_ASSERT(index < NRF_TWIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            return ((p_reg->DMA.RX.MATCH.CONFIG & TWIM_DMA_RX_MATCH_CONFIG_ENABLE0_Msk)
                    >> TWIM_DMA_RX_MATCH_CONFIG_ENABLE0_Pos) ==
                    TWIM_DMA_RX_MATCH_CONFIG_ENABLE0_Enabled;
        case 1:
            return ((p_reg->DMA.RX.MATCH.CONFIG & TWIM_DMA_RX_MATCH_CONFIG_ENABLE1_Msk)
                    >> TWIM_DMA_RX_MATCH_CONFIG_ENABLE1_Pos) ==
                    TWIM_DMA_RX_MATCH_CONFIG_ENABLE1_Enabled;
        case 2:
            return ((p_reg->DMA.RX.MATCH.CONFIG & TWIM_DMA_RX_MATCH_CONFIG_ENABLE2_Msk)
                    >> TWIM_DMA_RX_MATCH_CONFIG_ENABLE2_Pos) ==
                    TWIM_DMA_RX_MATCH_CONFIG_ENABLE2_Enabled;
        case 3:
            return ((p_reg->DMA.RX.MATCH.CONFIG & TWIM_DMA_RX_MATCH_CONFIG_ENABLE3_Msk)
                    >> TWIM_DMA_RX_MATCH_CONFIG_ENABLE3_Pos) ==
                    TWIM_DMA_RX_MATCH_CONFIG_ENABLE3_Enabled;
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_twim_rx_pattern_match_one_shot_enable(NRF_TWIM_Type * p_reg,
                                                                 uint8_t         index)
{
    NRFX_ASSERT(index < NRF_TWIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG |= TWIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk;
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG |= TWIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk;
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG |= TWIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk;
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG |= TWIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk;
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_twim_rx_pattern_match_one_shot_disable(NRF_TWIM_Type * p_reg,
                                                                  uint8_t         index)
{
    NRFX_ASSERT(index < NRF_TWIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(TWIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk);
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(TWIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk);
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(TWIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk);
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(TWIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE bool nrf_twim_rx_pattern_match_one_shot_check(NRF_TWIM_Type const * p_reg,
                                                                uint8_t               index)
{
    NRFX_ASSERT(index < NRF_TWIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            return ((p_reg->DMA.RX.MATCH.CONFIG & TWIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk)
                    >> TWIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Pos) ==
                    TWIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Oneshot;
        case 1:
            return ((p_reg->DMA.RX.MATCH.CONFIG & TWIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk)
                    >> TWIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Pos) ==
                    TWIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Oneshot;
        case 2:
            return ((p_reg->DMA.RX.MATCH.CONFIG & TWIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk)
                    >> TWIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Pos) ==
                    TWIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Oneshot;
        case 3:
            return ((p_reg->DMA.RX.MATCH.CONFIG & TWIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk)
                    >> TWIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Pos) ==
                    TWIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Oneshot;
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_twim_rx_pattern_match_candidate_set(NRF_TWIM_Type * p_reg,
                                                               uint8_t         index,
                                                               uint32_t        pattern)
{
    NRFX_ASSERT(index < NRF_TWIM_DMA_RX_PATTERN_MAX_COUNT);
    p_reg->DMA.RX.MATCH.CANDIDATE[index] = pattern;
}

NRF_STATIC_INLINE uint32_t nrf_twim_rx_pattern_match_candidate_get(NRF_TWIM_Type const * p_reg,
                                                                   uint8_t               index)
{
    NRFX_ASSERT(index < NRF_TWIM_DMA_RX_PATTERN_MAX_COUNT);
    return p_reg->DMA.RX.MATCH.CANDIDATE[index];
}
#endif // NRF_TWIM_HAS_DMA_REG

#if NRF_TWIM_HAS_BUS_ERROR_EVENTS
NRF_STATIC_INLINE void nrf_twim_rx_terminate_on_bus_error_enable_set(NRF_TWIM_Type * p_reg,
                                                                     bool            enable)
{
    p_reg->DMA.RX.TERMINATEONBUSERROR =  (enable ? TWIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Enabled :
                                                   TWIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Disabled)
                                         << TWIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Pos;
}

NRF_STATIC_INLINE bool nrf_twim_rx_terminate_on_bus_error_check(NRF_TWIM_Type const * p_reg)
{
    return ((p_reg->DMA.RX.TERMINATEONBUSERROR & TWIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Msk)
            >> TWIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Pos) ==
           TWIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Enabled;
}

NRF_STATIC_INLINE void nrf_twim_tx_terminate_on_bus_error_enable_set(NRF_TWIM_Type * p_reg,
                                                                     bool            enable)
{
    p_reg->DMA.TX.TERMINATEONBUSERROR =  (enable ? TWIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Enabled :
                                                   TWIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Disabled)
                                         << TWIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Pos;
}

NRF_STATIC_INLINE bool nrf_twim_tx_terminate_on_bus_error_check(NRF_TWIM_Type const * p_reg)
{
    return ((p_reg->DMA.TX.TERMINATEONBUSERROR & TWIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Msk)
            >> TWIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Pos) ==
           TWIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Enabled;
}
#endif // NRF_TWIM_HAS_BUS_ERROR_EVENTS

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_TWIM_H__
