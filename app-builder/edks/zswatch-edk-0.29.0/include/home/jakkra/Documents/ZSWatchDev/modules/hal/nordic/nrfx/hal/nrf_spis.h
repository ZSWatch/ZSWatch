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

#ifndef NRF_SPIS_H__
#define NRF_SPIS_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SPIS_CLOCKPIN_MISO_NEEDED)
#define NRF_SPIS_CLOCKPIN_MISO_NEEDED 1
#endif
#if defined(SPIS_CLOCKPIN_SCK_NEEDED)
#define NRF_SPIS_CLOCKPIN_SCK_NEEDED 1
#endif

/**
 * @defgroup nrf_spis_hal SPIS HAL
 * @{
 * @ingroup nrf_spis
 * @brief   Hardware access layer for managing the SPIS peripheral.
 */

#if defined(SPIS_DMA_TX_PTR_PTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether dedicated DMA register is present. */
#define NRF_SPIS_HAS_DMA_REG 1
#else
#define NRF_SPIS_HAS_DMA_REG 0
#endif

#if defined(SPIS_TXD_LIST_LIST_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TX list is present. */
#define NRF_SPIS_HAS_TX_LIST 1
#else
#define NRF_SPIS_HAS_TX_LIST 0
#endif

#if defined(SPIS_RXD_LIST_LIST_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RX list is present. */
#define NRF_SPIS_HAS_RX_LIST 1
#else
#define NRF_SPIS_HAS_RX_LIST 0
#endif

#if (defined(SPIS_TASKS_DMA_RX_ENABLEMATCH_ENABLEMATCH_Msk) && \
     defined(SPIS_EVENTS_DMA_RX_END_END_Msk)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SPIS DMA tasks and events are present. */
#define NRF_SPIS_HAS_DMA_TASKS_EVENTS 1
#else
#define NRF_SPIS_HAS_DMA_TASKS_EVENTS 0
#endif

#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
/** @brief Maximum number of RX patterns. */
#define NRF_SPIS_DMA_RX_PATTERN_MAX_COUNT SPIS_DMA_RX_MATCH_CANDIDATE_MaxCount
#endif

/**
 * @brief Macro getting pointer to the structure of registers of the SPIS peripheral.
 *
 * @param[in] idx SPIS instance index.
 *
 * @return Pointer to the structure of registers of the SPIS peripheral.
 */
#define NRF_SPIS_INST_GET(idx) NRFX_CONCAT(NRF_, SPIS, idx)

/**
 * @brief This value can be used as a parameter for the @ref nrf_spis_pins_set
 *        function to specify that a given SPI signal (SCK, MOSI, or MISO)
 *        shall not be connected to a physical pin.
 */
#define NRF_SPIS_PIN_NOT_CONNECTED UINT32_MAX 

/** @brief SPIS tasks. */
typedef enum
{
    NRF_SPIS_TASK_ACQUIRE         = offsetof(NRF_SPIS_Type, TASKS_ACQUIRE),                ///< Acquire SPI semaphore.
    NRF_SPIS_TASK_RELEASE         = offsetof(NRF_SPIS_Type, TASKS_RELEASE),                ///< Release SPI semaphore, enabling the SPI slave to acquire it.
#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
    NRF_SPIS_TASK_ENABLERXMATCH0  = offsetof(NRF_SPIS_Type, TASKS_DMA.RX.ENABLEMATCH[0]),  ///< Enable SPI pattern matching functionality for pattern 0.
    NRF_SPIS_TASK_ENABLERXMATCH1  = offsetof(NRF_SPIS_Type, TASKS_DMA.RX.ENABLEMATCH[1]),  ///< Enable SPI pattern matching functionality for pattern 1.
    NRF_SPIS_TASK_ENABLERXMATCH2  = offsetof(NRF_SPIS_Type, TASKS_DMA.RX.ENABLEMATCH[2]),  ///< Enable SPI pattern matching functionality for pattern 2.
    NRF_SPIS_TASK_ENABLERXMATCH3  = offsetof(NRF_SPIS_Type, TASKS_DMA.RX.ENABLEMATCH[3]),  ///< Enable SPI pattern matching functionality for pattern 3.
    NRF_SPIS_TASK_DISABLERXMATCH0 = offsetof(NRF_SPIS_Type, TASKS_DMA.RX.DISABLEMATCH[0]), ///< Disable SPI pattern matching functionality for pattern 0.
    NRF_SPIS_TASK_DISABLERXMATCH1 = offsetof(NRF_SPIS_Type, TASKS_DMA.RX.DISABLEMATCH[1]), ///< Disable SPI pattern matching functionality for pattern 1.
    NRF_SPIS_TASK_DISABLERXMATCH2 = offsetof(NRF_SPIS_Type, TASKS_DMA.RX.DISABLEMATCH[2]), ///< Disable SPI pattern matching functionality for pattern 2.
    NRF_SPIS_TASK_DISABLERXMATCH3 = offsetof(NRF_SPIS_Type, TASKS_DMA.RX.DISABLEMATCH[3])  ///< Disable SPI pattern matching functionality for pattern 3.
#endif
} nrf_spis_task_t;

/** @brief SPIS events. */
typedef enum
{
#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
    NRF_SPIS_EVENT_RXSTARTED  = offsetof(NRF_SPIS_Type, EVENTS_DMA.RX.READY),    ///< Receive sequence started.
    NRF_SPIS_EVENT_RXBUSERROR = offsetof(NRF_SPIS_Type, EVENTS_DMA.RX.BUSERROR), ///< Memory bus error occurred during the RX transfer.
    NRF_SPIS_EVENT_RXMATCH0   = offsetof(NRF_SPIS_Type, EVENTS_DMA.RX.MATCH[0]), ///< Pattern match for pattern 0 detected.
    NRF_SPIS_EVENT_RXMATCH1   = offsetof(NRF_SPIS_Type, EVENTS_DMA.RX.MATCH[1]), ///< Pattern match for pattern 1 detected.
    NRF_SPIS_EVENT_RXMATCH2   = offsetof(NRF_SPIS_Type, EVENTS_DMA.RX.MATCH[2]), ///< Pattern match for pattern 2 detected.
    NRF_SPIS_EVENT_RXMATCH3   = offsetof(NRF_SPIS_Type, EVENTS_DMA.RX.MATCH[3]), ///< Pattern match for pattern 3 detected.
    NRF_SPIS_EVENT_TXSTARTED  = offsetof(NRF_SPIS_Type, EVENTS_DMA.TX.READY),    ///< Transmit sequence started.
    NRF_SPIS_EVENT_TXBUSERROR = offsetof(NRF_SPIS_Type, EVENTS_DMA.TX.BUSERROR), ///< Memory bus error occurred during the TX transfer.
    NRF_SPIS_EVENT_ENDRX      = offsetof(NRF_SPIS_Type, EVENTS_DMA.RX.END),      ///< End of RXD buffer reached.
    NRF_SPIS_EVENT_ENDTX      = offsetof(NRF_SPIS_Type, EVENTS_DMA.TX.END),      ///< End of TXD buffer reached.
#endif
    NRF_SPIS_EVENT_END        = offsetof(NRF_SPIS_Type, EVENTS_END),             ///< Granted transaction completed.
    NRF_SPIS_EVENT_ACQUIRED   = offsetof(NRF_SPIS_Type, EVENTS_ACQUIRED)         ///< Semaphore acquired.
} nrf_spis_event_t;

/** @brief SPIS shortcuts. */
typedef enum
{
    NRF_SPIS_SHORT_END_ACQUIRE                   = SPIS_SHORTS_END_ACQUIRE_Msk,                        ///< Shortcut between END event and ACQUIRE task.
#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
    NRF_SPIS_SHORT_RXMATCH0_ENABLERXMATCH1_MASK  = SPIS_SHORTS_DMA_RX_MATCH0_DMA_RX_ENABLEMATCH1_Msk,  ///< Shortcut between DMA.RX.MATCH0 event and DMA.RX.ENABLEMATCH1 task.
    NRF_SPIS_SHORT_RXMATCH1_ENABLERXMATCH2_MASK  = SPIS_SHORTS_DMA_RX_MATCH1_DMA_RX_ENABLEMATCH2_Msk,  ///< Shortcut between DMA.RX.MATCH1 event and DMA.RX.ENABLEMATCH2 task.
    NRF_SPIS_SHORT_RXMATCH2_ENABLERXMATCH3_MASK  = SPIS_SHORTS_DMA_RX_MATCH2_DMA_RX_ENABLEMATCH3_Msk,  ///< Shortcut between DMA.RX.MATCH2 event and DMA.RX.ENABLEMATCH3 task.
    NRF_SPIS_SHORT_RXMATCH3_ENABLERXMATCH0_MASK  = SPIS_SHORTS_DMA_RX_MATCH3_DMA_RX_ENABLEMATCH0_Msk,  ///< Shortcut between DMA.RX.MATCH3 event and DMA.RX.ENABLEMATCH0 task.
    NRF_SPIS_SHORT_RXMATCH0_DISABLERXMATCH0_MASK = SPIS_SHORTS_DMA_RX_MATCH0_DMA_RX_DISABLEMATCH0_Msk, ///< Shortcut between DMA.RX.MATCH0 event and DMA.RX.DISABLEMATCH0 task.
    NRF_SPIS_SHORT_RXMATCH1_DISABLERXMATCH1_MASK = SPIS_SHORTS_DMA_RX_MATCH1_DMA_RX_DISABLEMATCH1_Msk, ///< Shortcut between DMA.RX.MATCH1 event and DMA.RX.DISABLEMATCH1 task.
    NRF_SPIS_SHORT_RXMATCH2_DISABLERXMATCH2_MASK = SPIS_SHORTS_DMA_RX_MATCH2_DMA_RX_DISABLEMATCH2_Msk, ///< Shortcut between DMA.RX.MATCH2 event and DMA.RX.DISABLEMATCH2 task.
    NRF_SPIS_SHORT_RXMATCH3_DISABLERXMATCH3_MASK = SPIS_SHORTS_DMA_RX_MATCH3_DMA_RX_DISABLEMATCH3_Msk, ///< Shortcut between DMA.RX.MATCH3 event and DMA.RX.DISABLEMATCH3 task.
#endif
    NRF_SPIS_ALL_SHORTS_MASK      = NRF_SPIS_SHORT_END_ACQUIRE
#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
                                  | NRF_SPIS_SHORT_RXMATCH0_ENABLERXMATCH1_MASK
                                  | NRF_SPIS_SHORT_RXMATCH1_ENABLERXMATCH2_MASK
                                  | NRF_SPIS_SHORT_RXMATCH2_ENABLERXMATCH3_MASK
                                  | NRF_SPIS_SHORT_RXMATCH3_ENABLERXMATCH0_MASK
                                  | NRF_SPIS_SHORT_RXMATCH0_DISABLERXMATCH0_MASK
                                  | NRF_SPIS_SHORT_RXMATCH1_DISABLERXMATCH1_MASK
                                  | NRF_SPIS_SHORT_RXMATCH2_DISABLERXMATCH2_MASK
                                  | NRF_SPIS_SHORT_RXMATCH3_DISABLERXMATCH3_MASK                       ///< All SPIS shortcuts.
#endif
} nrf_spis_short_mask_t;

/** @brief SPIS interrupts. */
typedef enum
{
    NRF_SPIS_INT_END_MASK        = SPIS_INTENSET_END_Msk,           ///< Interrupt on END event.
    NRF_SPIS_INT_ACQUIRED_MASK   = SPIS_INTENSET_ACQUIRED_Msk,      ///< Interrupt on ACQUIRED event.
#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
    NRF_SPIS_INT_RXREADY_MASK    = SPIS_INTENSET_DMARXREADY_Msk,    ///< Interrupt on DMA.RX.READY event.
    NRF_SPIS_INT_RXBUSERROR_MASK = SPIS_INTENSET_DMARXBUSERROR_Msk, ///< Interrupt on DMA.RX.BUSERROR event.
    NRF_SPIS_INT_RXMATCH0_MASK   = SPIS_INTENSET_DMARXMATCH0_Msk,   ///< Interrupt on DMA.RX.MATCH0 event.
    NRF_SPIS_INT_RXMATCH1_MASK   = SPIS_INTENSET_DMARXMATCH1_Msk,   ///< Interrupt on DMA.RX.MATCH1 event.
    NRF_SPIS_INT_RXMATCH2_MASK   = SPIS_INTENSET_DMARXMATCH2_Msk,   ///< Interrupt on DMA.RX.MATCH2 event.
    NRF_SPIS_INT_RXMATCH3_MASK   = SPIS_INTENSET_DMARXMATCH3_Msk,   ///< Interrupt on DMA.RX.MATCH3 event.
    NRF_SPIS_INT_TXREADY_MASK    = SPIS_INTENSET_DMATXREADY_Msk,    ///< Interrupt on DMA.TX.READY event.
    NRF_SPIS_INT_TXBUSERROR_MASK = SPIS_INTENSET_DMATXBUSERROR_Msk, ///< Interrupt on DMA.TX.BUSERROR event.
    NRF_SPIS_INT_ENDRX_MASK      = SPIS_INTENSET_DMARXEND_Msk,      ///< Interrupt on ENDRX event.
    NRF_SPIS_INT_ENDTX_MASK      = SPIS_INTENSET_DMATXEND_Msk,      ///< Interrupt on ENDTX event.
#endif
    NRF_SPIS_ALL_INTS_MASK       = NRF_SPIS_INT_END_MASK
                                 | NRF_SPIS_INT_ACQUIRED_MASK
#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
                                 | NRF_SPIS_INT_RXREADY_MASK
                                 | NRF_SPIS_INT_RXBUSERROR_MASK
                                 | NRF_SPIS_INT_RXMATCH0_MASK
                                 | NRF_SPIS_INT_RXMATCH1_MASK
                                 | NRF_SPIS_INT_RXMATCH2_MASK
                                 | NRF_SPIS_INT_RXMATCH3_MASK
                                 | NRF_SPIS_INT_TXREADY_MASK
                                 | NRF_SPIS_INT_TXBUSERROR_MASK
                                 | NRF_SPIS_INT_ENDRX_MASK
                                 | NRF_SPIS_INT_ENDTX_MASK          ///< All SPIS interrupts.
#endif
} nrf_spis_int_mask_t;

/** @brief SPI modes. */
typedef enum
{
    NRF_SPIS_MODE_0, ///< SCK active high, sample on leading edge of clock.
    NRF_SPIS_MODE_1, ///< SCK active high, sample on trailing edge of clock.
    NRF_SPIS_MODE_2, ///< SCK active low, sample on leading edge of clock.
    NRF_SPIS_MODE_3  ///< SCK active low, sample on trailing edge of clock.
} nrf_spis_mode_t;

/** @brief SPI bit orders. */
typedef enum
{
    NRF_SPIS_BIT_ORDER_MSB_FIRST = SPIS_CONFIG_ORDER_MsbFirst, ///< Most significant bit shifted out first.
    NRF_SPIS_BIT_ORDER_LSB_FIRST = SPIS_CONFIG_ORDER_LsbFirst  ///< Least significant bit shifted out first.
} nrf_spis_bit_order_t;

/** @brief SPI semaphore status. */
typedef enum
{
    NRF_SPIS_SEMSTAT_FREE       = 0, ///< Semaphore is free.
    NRF_SPIS_SEMSTAT_CPU        = 1, ///< Semaphore is assigned to the CPU.
    NRF_SPIS_SEMSTAT_SPIS       = 2, ///< Semaphore is assigned to the SPI slave.
    NRF_SPIS_SEMSTAT_CPUPENDING = 3  ///< Semaphore is assigned to the SPI, but a handover to the CPU is pending.
} nrf_spis_semstat_t;

/** @brief SPIS status. */
typedef enum
{
    NRF_SPIS_STATUS_OVERREAD = SPIS_STATUS_OVERREAD_Msk, ///< TX buffer over-read detected and prevented.
    NRF_SPIS_STATUS_OVERFLOW = SPIS_STATUS_OVERFLOW_Msk  ///< RX buffer overflow detected and prevented.
} nrf_spis_status_mask_t;


/**
 * @brief Function for activating the specified SPIS task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_spis_task_trigger(NRF_SPIS_Type * p_reg,
                                             nrf_spis_task_t task);

/**
 * @brief Function for getting the address of the specified SPIS task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  The specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_task_address_get(NRF_SPIS_Type const * p_reg,
                                                     nrf_spis_task_t       task);

/**
 * @brief Function for clearing the specified SPIS event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_spis_event_clear(NRF_SPIS_Type *  p_reg,
                                            nrf_spis_event_t event);

/**
 * @brief Function for retrieving the state of the SPIS event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_spis_event_check(NRF_SPIS_Type const * p_reg,
                                            nrf_spis_event_t      event);

/**
 * @brief Function for getting the address of the specified SPIS event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event The specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_event_address_get(NRF_SPIS_Type const * p_reg,
                                                      nrf_spis_event_t      event);

/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_spis_shorts_enable(NRF_SPIS_Type * p_reg,
                                              uint32_t        mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_spis_shorts_disable(NRF_SPIS_Type * p_reg,
                                               uint32_t        mask);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_spis_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_spis_int_enable(NRF_SPIS_Type * p_reg,
                                           uint32_t        mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_spis_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_spis_int_disable(NRF_SPIS_Type * p_reg,
                                            uint32_t        mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_spis_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_int_enable_check(NRF_SPIS_Type const * p_reg, uint32_t mask);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        SPIS task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_spis_subscribe_set(NRF_SPIS_Type * p_reg,
                                              nrf_spis_task_t task,
                                              uint8_t         channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        SPIS task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_spis_subscribe_clear(NRF_SPIS_Type * p_reg,
                                                nrf_spis_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        SPIS task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return SPIS subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_subscribe_get(NRF_SPIS_Type const * p_reg,
                                                  nrf_spis_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        SPIS event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_spis_publish_set(NRF_SPIS_Type *  p_reg,
                                            nrf_spis_event_t event,
                                            uint8_t          channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        SPIS event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_spis_publish_clear(NRF_SPIS_Type *  p_reg,
                                              nrf_spis_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        SPIS event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return SPIS publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_publish_get(NRF_SPIS_Type const * p_reg,
                                                nrf_spis_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for enabling the SPIS peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_enable(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for disabling the SPIS peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_disable(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for checking if the SPIS peripheral is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The SPIS is enabled.
 * @retval false The SPIS is not enabled.
 */
NRF_STATIC_INLINE bool nrf_spis_enable_check(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for retrieving the SPIS semaphore status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Current semaphore status.
 */
NRF_STATIC_INLINE nrf_spis_semstat_t nrf_spis_semaphore_status_get(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for retrieving the SPIS status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Current SPIS status.
 */
NRF_STATIC_INLINE nrf_spis_status_mask_t nrf_spis_status_get(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for configuring SPIS pins.
 *
 * If a given signal is not needed, pass the @ref NRF_SPIS_PIN_NOT_CONNECTED
 * value instead of its pin number.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] sck_pin  SCK pin number.
 * @param[in] mosi_pin MOSI pin number.
 * @param[in] miso_pin MISO pin number.
 * @param[in] csn_pin  CSN pin number.
 */
NRF_STATIC_INLINE void nrf_spis_pins_set(NRF_SPIS_Type * p_reg,
                                         uint32_t        sck_pin,
                                         uint32_t        mosi_pin,
                                         uint32_t        miso_pin,
                                         uint32_t        csn_pin);

/**
 * @brief Function for setting the SCK pin.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   SCK pin number.
 */
NRF_STATIC_INLINE void nrf_spis_sck_pin_set(NRF_SPIS_Type * p_reg, uint32_t pin);

/**
 * @brief Function for setting the MOSI pin
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   MOSI pin number.
 */
NRF_STATIC_INLINE void nrf_spis_mosi_pin_set(NRF_SPIS_Type * p_reg, uint32_t pin);

/**
 * @brief Function for setting the MISO pin.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   MISO pin number.
 */
NRF_STATIC_INLINE void nrf_spis_miso_pin_set(NRF_SPIS_Type * p_reg, uint32_t pin);

/**
 * @brief Function for setting the CSN pin.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   CSN pin number.
 */
NRF_STATIC_INLINE void nrf_spis_csn_pin_set(NRF_SPIS_Type * p_reg, uint32_t pin);

/**
 * @brief Function for getting the SCK pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SCK pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_sck_pin_get(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for getting the MOSI pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return MOSI pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_mosi_pin_get(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for getting the MISO pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return MISO pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_miso_pin_get(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for getting the CSN pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return CSN pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spis_csn_pin_get(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for setting the transmit buffer.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the buffer that contains the data to send.
 * @param[in] length   Maximum number of data bytes to transmit.
 */
NRF_STATIC_INLINE void nrf_spis_tx_buffer_set(NRF_SPIS_Type * p_reg,
                                              uint8_t const * p_buffer,
                                              size_t          length);

/**
 * @brief Function for setting the receive buffer.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the buffer for received data.
 * @param[in] length   Maximum number of data bytes to receive.
 */
NRF_STATIC_INLINE void nrf_spis_rx_buffer_set(NRF_SPIS_Type * p_reg,
                                              uint8_t *       p_buffer,
                                              size_t          length);

/**
 * @brief Function for getting the transmit buffer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The transmit buffer pointer.
 */
NRF_STATIC_INLINE uint8_t * nrf_spis_tx_buffer_get(NRF_SPIS_Type * p_reg);

/**
* @brief Function for getting the receive buffer.
*
* @param[in] p_reg Pointer to the structure of registers of the peripheral.
*
* @return The receive buffer pointer.
*/
NRF_STATIC_INLINE uint8_t * nrf_spis_rx_buffer_get(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for getting the transmit buffer size.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The transmit buffer size.
 */
NRF_STATIC_INLINE size_t nrf_spis_tx_maxcnt_get(NRF_SPIS_Type * p_reg);

/**
* @brief Function for getting the receive buffer size.
*
* @param[in] p_reg Pointer to the structure of registers of the peripheral.
*
* @return The receive buffer size.
*/
NRF_STATIC_INLINE size_t nrf_spis_rx_maxcnt_get(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for getting the number of bytes transmitted
 *        in the last granted transaction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes transmitted.
 */
NRF_STATIC_INLINE size_t nrf_spis_tx_amount_get(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for getting the number of bytes received
 *        in the last granted transaction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes received.
 */
NRF_STATIC_INLINE size_t nrf_spis_rx_amount_get(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for setting the SPI configuration.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] spi_mode      SPI mode.
 * @param[in] spi_bit_order SPI bit order.
 */
NRF_STATIC_INLINE void nrf_spis_configure(NRF_SPIS_Type *      p_reg,
                                          nrf_spis_mode_t      spi_mode,
                                          nrf_spis_bit_order_t spi_bit_order);

/**
 * @brief Function for setting the default character.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] def   Default character that is clocked out in case of
 *                  an overflow of the RXD buffer.
 */
NRF_STATIC_INLINE void nrf_spis_def_set(NRF_SPIS_Type * p_reg,
                                        uint8_t         def);

/**
 * @brief Function for setting the over-read character.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] orc   Over-read character that is clocked out in case of
 *                  an over-read of the TXD buffer.
 */
NRF_STATIC_INLINE void nrf_spis_orc_set(NRF_SPIS_Type * p_reg,
                                        uint8_t         orc);

#if NRF_SPIS_HAS_TX_LIST
/**
 * @brief Function for enabling the TX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_tx_list_enable(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for disabling the TX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_tx_list_disable(NRF_SPIS_Type * p_reg);
#endif // NRF_SPIS_HAS_TX_LIST

#if NRF_SPIS_HAS_RX_LIST
/**
 * @brief Function for enabling the RX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_rx_list_enable(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for disabling the RX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_rx_list_disable(NRF_SPIS_Type * p_reg);
#endif // NRF_SPIS_HAS_RX_LIST

#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
/**
 * @brief Function for enabling individual pattern match filters.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Index of pattern match filter.
 * @param[in] enable True if pattern match filter is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_spis_rx_pattern_match_enable_set(NRF_SPIS_Type * p_reg,
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
NRF_STATIC_INLINE bool nrf_spis_rx_pattern_match_enable_check(NRF_SPIS_Type const * p_reg,
                                                              uint8_t               index);

/**
 * @brief Function for enabling one-shot operation for the specified match filter.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 */
NRF_STATIC_INLINE void nrf_spis_rx_pattern_match_one_shot_enable(NRF_SPIS_Type * p_reg,
                                                                 uint8_t         index);

/**
 * @brief Function for disabling one-shot operation for the specified match filter.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 */
NRF_STATIC_INLINE void nrf_spis_rx_pattern_match_one_shot_disable(NRF_SPIS_Type * p_reg,
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
NRF_STATIC_INLINE bool nrf_spis_rx_pattern_match_one_shot_check(NRF_SPIS_Type const * p_reg,
                                                                uint8_t               index);

/**
 * @brief Function for setting the pattern to be looked for by the specified match filter.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] index   Index of pattern match filter.
 * @param[in] pattern Pattern to be looked for.
 *                    Match will trigger the corresponding event, if enabled.
 */
NRF_STATIC_INLINE void nrf_spis_rx_pattern_match_candidate_set(NRF_SPIS_Type * p_reg,
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
NRF_STATIC_INLINE uint32_t nrf_spis_rx_pattern_match_candidate_get(NRF_SPIS_Type const * p_reg,
                                                                   uint8_t               index);
#endif // NRF_SPIS_HAS_DMA_TASKS_EVENTS

#if NRF_SPIS_HAS_DMA_REG
/**
 * @brief Function for enabling the termination of the RX transaction after detecting the BUSERROR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_rx_terminate_on_bus_error_enable(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for disabling the termination of the RX transaction after detecting the BUSERROR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_rx_terminate_on_bus_error_disable(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for checking if RX transaction termination after detecting the BUSERROR event is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  RX transaction termination after detecting a BUSERROR event is enabled.
 * @retval false RX transaction termination after detecting a BUSERROR event is disabled.
 */
NRF_STATIC_INLINE bool nrf_spis_rx_terminate_on_bus_error_check(NRF_SPIS_Type const * p_reg);

/**
 * @brief Function for enabling the termination of the TX transaction after detecting the BUSERROR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_tx_terminate_on_bus_error_enable(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for disabling the termination of the TX transaction after detecting the BUSERROR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spis_tx_terminate_on_bus_error_disable(NRF_SPIS_Type * p_reg);

/**
 * @brief Function for checking if TX transaction termination after detecting the BUSERROR event is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  TX transaction termination after detecting a BUSERROR event is enabled.
 * @retval false TX transaction termination after detecting a BUSERROR event is disabled.
 */
NRF_STATIC_INLINE bool nrf_spis_tx_terminate_on_bus_error_check(NRF_SPIS_Type const * p_reg);
#endif // NRF_SPIS_HAS_DMA_REG

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_spis_task_trigger(NRF_SPIS_Type * p_reg,
                                             nrf_spis_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_spis_task_address_get(NRF_SPIS_Type const * p_reg,
                                                     nrf_spis_task_t       task)
{
    return (uint32_t)p_reg + (uint32_t)task;
}

NRF_STATIC_INLINE void nrf_spis_event_clear(NRF_SPIS_Type *  p_reg,
                                            nrf_spis_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_spis_event_check(NRF_SPIS_Type const * p_reg,
                                            nrf_spis_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_spis_event_address_get(NRF_SPIS_Type const * p_reg,
                                                      nrf_spis_event_t      event)
{
    return (uint32_t)p_reg + (uint32_t)event;
}

NRF_STATIC_INLINE void nrf_spis_shorts_enable(NRF_SPIS_Type * p_reg,
                                              uint32_t        mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_spis_shorts_disable(NRF_SPIS_Type * p_reg,
                                               uint32_t        mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE void nrf_spis_int_enable(NRF_SPIS_Type * p_reg,
                                           uint32_t        mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_spis_int_disable(NRF_SPIS_Type * p_reg,
                                            uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_spis_int_enable_check(NRF_SPIS_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_spis_subscribe_set(NRF_SPIS_Type * p_reg,
                                              nrf_spis_task_t task,
                                              uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_spis_subscribe_clear(NRF_SPIS_Type * p_reg,
                                                nrf_spis_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_spis_subscribe_get(NRF_SPIS_Type const * p_reg,
                                                  nrf_spis_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_spis_publish_set(NRF_SPIS_Type *  p_reg,
                                            nrf_spis_event_t event,
                                            uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_spis_publish_clear(NRF_SPIS_Type *  p_reg,
                                              nrf_spis_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_spis_publish_get(NRF_SPIS_Type const * p_reg,
                                                nrf_spis_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE void nrf_spis_enable(NRF_SPIS_Type * p_reg)
{
    p_reg->ENABLE = (SPIS_ENABLE_ENABLE_Enabled << SPIS_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_spis_disable(NRF_SPIS_Type * p_reg)
{
    p_reg->ENABLE = (SPIS_ENABLE_ENABLE_Disabled << SPIS_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE bool nrf_spis_enable_check(NRF_SPIS_Type const * p_reg)
{
    return p_reg->ENABLE == SPIS_ENABLE_ENABLE_Enabled;
}

NRF_STATIC_INLINE nrf_spis_semstat_t nrf_spis_semaphore_status_get(NRF_SPIS_Type const * p_reg)
{
    return (nrf_spis_semstat_t) ((p_reg->SEMSTAT & SPIS_SEMSTAT_SEMSTAT_Msk)
                                 >> SPIS_SEMSTAT_SEMSTAT_Pos);
}

NRF_STATIC_INLINE nrf_spis_status_mask_t nrf_spis_status_get(NRF_SPIS_Type const * p_reg)
{
    return (nrf_spis_status_mask_t) p_reg->STATUS;
}

NRF_STATIC_INLINE void nrf_spis_pins_set(NRF_SPIS_Type * p_reg,
                                         uint32_t        sck_pin,
                                         uint32_t        mosi_pin,
                                         uint32_t        miso_pin,
                                         uint32_t        csn_pin)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    p_reg->PSELSCK  = sck_pin;
    p_reg->PSELMOSI = mosi_pin;
    p_reg->PSELMISO = miso_pin;
    p_reg->PSELCSN  = csn_pin;
#else
    p_reg->PSEL.SCK  = sck_pin;
    p_reg->PSEL.MOSI = mosi_pin;
    p_reg->PSEL.MISO = miso_pin;
    p_reg->PSEL.CSN  = csn_pin;
#endif
}

NRF_STATIC_INLINE void nrf_spis_sck_pin_set(NRF_SPIS_Type * p_reg, uint32_t pin)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    p_reg->PSELSCK = pin;
#else
    p_reg->PSEL.SCK = pin;
#endif
}

NRF_STATIC_INLINE void nrf_spis_mosi_pin_set(NRF_SPIS_Type * p_reg, uint32_t pin)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    p_reg->PSELMOSI = pin;
#else
    p_reg->PSEL.MOSI = pin;
#endif
}

NRF_STATIC_INLINE void nrf_spis_miso_pin_set(NRF_SPIS_Type * p_reg, uint32_t pin)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    p_reg->PSELMISO = pin;
#else
    p_reg->PSEL.MISO = pin;
#endif
}

NRF_STATIC_INLINE void nrf_spis_csn_pin_set(NRF_SPIS_Type * p_reg, uint32_t pin)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    p_reg->PSELCSN = pin;
#else
    p_reg->PSEL.CSN = pin;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spis_sck_pin_get(NRF_SPIS_Type const * p_reg)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    return p_reg->PSELSCK;
#else
    return p_reg->PSEL.SCK;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spis_mosi_pin_get(NRF_SPIS_Type const * p_reg)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    return p_reg->PSELMOSI;
#else
    return p_reg->PSEL.MOSI;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spis_miso_pin_get(NRF_SPIS_Type const * p_reg)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    return p_reg->PSELMISO;
#else
    return p_reg->PSEL.MISO;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spis_csn_pin_get(NRF_SPIS_Type const * p_reg)
{
#if !defined(SPIS_PSEL_SCK_CONNECT_Msk)
    return p_reg->PSELCSN;
#else
    return p_reg->PSEL.CSN;
#endif
}

NRF_STATIC_INLINE void nrf_spis_tx_buffer_set(NRF_SPIS_Type * p_reg,
                                              uint8_t const * p_buffer,
                                              size_t          length)
{
#if NRF_SPIS_HAS_DMA_REG
    p_reg->DMA.TX.PTR    = (uint32_t)p_buffer;
    p_reg->DMA.TX.MAXCNT = length;
#elif !defined(SPIS_TXD_PTR_PTR_Msk)
    p_reg->TXDPTR = (uint32_t)p_buffer;
    p_reg->MAXTX  = length;
#else
    p_reg->TXD.PTR    = (uint32_t)p_buffer;
    p_reg->TXD.MAXCNT = length;
#endif
}

NRF_STATIC_INLINE void nrf_spis_rx_buffer_set(NRF_SPIS_Type * p_reg,
                                              uint8_t *       p_buffer,
                                              size_t          length)
{
#if NRF_SPIS_HAS_DMA_REG
    p_reg->DMA.RX.PTR    = (uint32_t)p_buffer;
    p_reg->DMA.RX.MAXCNT = length;
#elif !defined(SPIS_RXD_PTR_PTR_Msk)
    p_reg->RXDPTR = (uint32_t)p_buffer;
    p_reg->MAXRX  = length;
#else
    p_reg->RXD.PTR    = (uint32_t)p_buffer;
    p_reg->RXD.MAXCNT = length;
#endif
}

NRF_STATIC_INLINE uint8_t * nrf_spis_tx_buffer_get(NRF_SPIS_Type * p_reg)
{
#if NRF_SPIS_HAS_DMA_REG
    return (uint8_t *)p_reg->DMA.TX.PTR;
#elif !defined(SPIS_TXD_PTR_PTR_Msk)
    return (uint8_t *)p_reg->TXDPTR;
#else
    return (uint8_t *)p_reg->TXD.PTR;
#endif
}

NRF_STATIC_INLINE uint8_t * nrf_spis_rx_buffer_get(NRF_SPIS_Type * p_reg)
{
#if NRF_SPIS_HAS_DMA_REG
    return (uint8_t *)p_reg->DMA.RX.PTR;
#elif !defined(SPIS_RXD_PTR_PTR_Msk)
    return (uint8_t *)p_reg->RXDPTR;
#else
    return (uint8_t *)p_reg->RXD.PTR;
#endif
}

NRF_STATIC_INLINE size_t nrf_spis_tx_maxcnt_get(NRF_SPIS_Type * p_reg)
{
#if NRF_SPIS_HAS_DMA_REG
    return (size_t)p_reg->DMA.TX.MAXCNT;
#elif !defined(SPIS_TXD_MAXCNT_MAXCNT_Msk)
    return (size_t)p_reg->MAXTX;
#else
    return (size_t)p_reg->TXD.MAXCNT;
#endif
}

NRF_STATIC_INLINE size_t nrf_spis_rx_maxcnt_get(NRF_SPIS_Type * p_reg)
{
#if NRF_SPIS_HAS_DMA_REG
    return (size_t)p_reg->DMA.RX.MAXCNT;
#elif !defined(SPIS_RXD_MAXCNT_MAXCNT_Msk)
    return (size_t)p_reg->MAXRX;
#else
    return (size_t)p_reg->RXD.MAXCNT;
#endif
}

NRF_STATIC_INLINE size_t nrf_spis_tx_amount_get(NRF_SPIS_Type const * p_reg)
{
#if NRF_SPIS_HAS_DMA_REG
    return p_reg->DMA.TX.AMOUNT;
#elif !defined(SPIS_TXD_AMOUNT_AMOUNT_Msk)
    return p_reg->AMOUNTTX;
#else
    return p_reg->TXD.AMOUNT;
#endif
}

NRF_STATIC_INLINE size_t nrf_spis_rx_amount_get(NRF_SPIS_Type const * p_reg)
{
#if NRF_SPIS_HAS_DMA_REG
    return p_reg->DMA.RX.AMOUNT;
#elif !defined(SPIS_RXD_AMOUNT_AMOUNT_Msk)
    return p_reg->AMOUNTRX;
#else
    return p_reg->RXD.AMOUNT;
#endif
}

NRF_STATIC_INLINE void nrf_spis_configure(NRF_SPIS_Type *      p_reg,
                                          nrf_spis_mode_t      spi_mode,
                                          nrf_spis_bit_order_t spi_bit_order)
{
    uint32_t config = (spi_bit_order == NRF_SPIS_BIT_ORDER_MSB_FIRST ?
        SPIS_CONFIG_ORDER_MsbFirst : SPIS_CONFIG_ORDER_LsbFirst);

    switch (spi_mode)
    {
    default:
    case NRF_SPIS_MODE_0:
        config |= (SPIS_CONFIG_CPOL_ActiveHigh << SPIS_CONFIG_CPOL_Pos) |
                  (SPIS_CONFIG_CPHA_Leading    << SPIS_CONFIG_CPHA_Pos);
        break;

    case NRF_SPIS_MODE_1:
        config |= (SPIS_CONFIG_CPOL_ActiveHigh << SPIS_CONFIG_CPOL_Pos) |
                  (SPIS_CONFIG_CPHA_Trailing   << SPIS_CONFIG_CPHA_Pos);
        break;

    case NRF_SPIS_MODE_2:
        config |= (SPIS_CONFIG_CPOL_ActiveLow  << SPIS_CONFIG_CPOL_Pos) |
                  (SPIS_CONFIG_CPHA_Leading    << SPIS_CONFIG_CPHA_Pos);
        break;

    case NRF_SPIS_MODE_3:
        config |= (SPIS_CONFIG_CPOL_ActiveLow  << SPIS_CONFIG_CPOL_Pos) |
                  (SPIS_CONFIG_CPHA_Trailing   << SPIS_CONFIG_CPHA_Pos);
        break;
    }
    p_reg->CONFIG = config;
}

NRF_STATIC_INLINE void nrf_spis_orc_set(NRF_SPIS_Type * p_reg,
                                        uint8_t         orc)
{
    p_reg->ORC = orc;
}

NRF_STATIC_INLINE void nrf_spis_def_set(NRF_SPIS_Type * p_reg,
                                        uint8_t         def)
{
    p_reg->DEF = def;
}

#if NRF_SPIS_HAS_TX_LIST
NRF_STATIC_INLINE void nrf_spis_tx_list_enable(NRF_SPIS_Type * p_reg)
{
    p_reg->TXD.LIST = SPIS_TXD_LIST_LIST_ArrayList << SPIS_TXD_LIST_LIST_Pos;
}

NRF_STATIC_INLINE void nrf_spis_tx_list_disable(NRF_SPIS_Type * p_reg)
{
    p_reg->TXD.LIST = SPIS_TXD_LIST_LIST_Disabled << SPIS_TXD_LIST_LIST_Pos;
}
#endif // NRF_SPIS_HAS_TX_LIST

#if NRF_SPIS_HAS_RX_LIST
NRF_STATIC_INLINE void nrf_spis_rx_list_enable(NRF_SPIS_Type * p_reg)
{
    p_reg->RXD.LIST = SPIS_RXD_LIST_LIST_ArrayList << SPIS_RXD_LIST_LIST_Pos;
}

NRF_STATIC_INLINE void nrf_spis_rx_list_disable(NRF_SPIS_Type * p_reg)
{
    p_reg->RXD.LIST = SPIS_RXD_LIST_LIST_Disabled << SPIS_RXD_LIST_LIST_Pos;
}
#endif // NRF_SPIS_HAS_RX_LIST

#if NRF_SPIS_HAS_DMA_TASKS_EVENTS
NRF_STATIC_INLINE void nrf_spis_rx_pattern_match_enable_set(NRF_SPIS_Type * p_reg,
                                                            uint8_t         index,
                                                            bool            enable)
{
    NRFX_ASSERT(index < NRF_SPIS_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                           ~SPIS_DMA_RX_MATCH_CONFIG_ENABLE0_Msk) |
                                          ((enable ?
                                            SPIS_DMA_RX_MATCH_CONFIG_ENABLE0_Enabled :
                                            SPIS_DMA_RX_MATCH_CONFIG_ENABLE0_Disabled)
                                           << SPIS_DMA_RX_MATCH_CONFIG_ENABLE0_Pos));
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                           ~SPIS_DMA_RX_MATCH_CONFIG_ENABLE1_Msk) |
                                          ((enable ?
                                            SPIS_DMA_RX_MATCH_CONFIG_ENABLE1_Enabled :
                                            SPIS_DMA_RX_MATCH_CONFIG_ENABLE1_Disabled)
                                           << SPIS_DMA_RX_MATCH_CONFIG_ENABLE1_Pos));
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                           ~SPIS_DMA_RX_MATCH_CONFIG_ENABLE2_Msk) |
                                          ((enable ?
                                            SPIS_DMA_RX_MATCH_CONFIG_ENABLE2_Enabled :
                                            SPIS_DMA_RX_MATCH_CONFIG_ENABLE2_Disabled)
                                           << SPIS_DMA_RX_MATCH_CONFIG_ENABLE2_Pos));
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                           ~SPIS_DMA_RX_MATCH_CONFIG_ENABLE3_Msk) |
                                          ((enable ?
                                            SPIS_DMA_RX_MATCH_CONFIG_ENABLE3_Enabled :
                                            SPIS_DMA_RX_MATCH_CONFIG_ENABLE3_Disabled)
                                           << SPIS_DMA_RX_MATCH_CONFIG_ENABLE3_Pos));
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE bool nrf_spis_rx_pattern_match_enable_check(NRF_SPIS_Type const * p_reg,
                                                              uint8_t               index)
{
    NRFX_ASSERT(index < NRF_SPIS_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIS_DMA_RX_MATCH_CONFIG_ENABLE0_Msk)
                    >> SPIS_DMA_RX_MATCH_CONFIG_ENABLE0_Pos) ==
                   SPIS_DMA_RX_MATCH_CONFIG_ENABLE0_Enabled;
        case 1:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIS_DMA_RX_MATCH_CONFIG_ENABLE1_Msk)
                    >> SPIS_DMA_RX_MATCH_CONFIG_ENABLE1_Pos) ==
                   SPIS_DMA_RX_MATCH_CONFIG_ENABLE1_Enabled;
        case 2:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIS_DMA_RX_MATCH_CONFIG_ENABLE2_Msk)
                    >> SPIS_DMA_RX_MATCH_CONFIG_ENABLE2_Pos) ==
                   SPIS_DMA_RX_MATCH_CONFIG_ENABLE2_Enabled;
        case 3:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIS_DMA_RX_MATCH_CONFIG_ENABLE3_Msk)
                    >> SPIS_DMA_RX_MATCH_CONFIG_ENABLE3_Pos) ==
                   SPIS_DMA_RX_MATCH_CONFIG_ENABLE3_Enabled;
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_spis_rx_pattern_match_one_shot_enable(NRF_SPIS_Type * p_reg,
                                                                 uint8_t         index)
{
    NRFX_ASSERT(index < NRF_SPIS_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG |= SPIS_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk;
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG |= SPIS_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk;
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG |= SPIS_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk;
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG |= SPIS_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk;
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_spis_rx_pattern_match_one_shot_disable(NRF_SPIS_Type * p_reg,
                                                                  uint8_t         index)
{
    NRFX_ASSERT(index < NRF_SPIS_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(SPIS_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk);
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(SPIS_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk);
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(SPIS_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk);
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(SPIS_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE bool nrf_spis_rx_pattern_match_one_shot_check(NRF_SPIS_Type const * p_reg,
                                                                uint8_t               index)
{
    NRFX_ASSERT(index < NRF_SPIS_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIS_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk)
                    >> SPIS_DMA_RX_MATCH_CONFIG_ONESHOT0_Pos) ==
                   SPIS_DMA_RX_MATCH_CONFIG_ONESHOT0_Oneshot;
        case 1:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIS_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk)
                    >> SPIS_DMA_RX_MATCH_CONFIG_ONESHOT1_Pos) ==
                   SPIS_DMA_RX_MATCH_CONFIG_ONESHOT1_Oneshot;
        case 2:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIS_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk)
                    >> SPIS_DMA_RX_MATCH_CONFIG_ONESHOT2_Pos) ==
                   SPIS_DMA_RX_MATCH_CONFIG_ONESHOT2_Oneshot;
        case 3:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIS_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk)
                    >> SPIS_DMA_RX_MATCH_CONFIG_ONESHOT3_Pos) ==
                   SPIS_DMA_RX_MATCH_CONFIG_ONESHOT3_Oneshot;
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_spis_rx_pattern_match_candidate_set(NRF_SPIS_Type * p_reg,
                                                               uint8_t         index,
                                                               uint32_t        pattern)
{
    NRFX_ASSERT(index < NRF_SPIS_DMA_RX_PATTERN_MAX_COUNT);
    p_reg->DMA.RX.MATCH.CANDIDATE[index] = pattern;
}

NRF_STATIC_INLINE uint32_t nrf_spis_rx_pattern_match_candidate_get(NRF_SPIS_Type const * p_reg,
                                                                   uint8_t               index)
{
    NRFX_ASSERT(index < NRF_SPIS_DMA_RX_PATTERN_MAX_COUNT);
    return p_reg->DMA.RX.MATCH.CANDIDATE[index];
}
#endif // NRF_SPIS_HAS_DMA_TASKS_EVENTS

#if NRF_SPIS_HAS_DMA_REG
NRF_STATIC_INLINE void nrf_spis_rx_terminate_on_bus_error_enable(NRF_SPIS_Type * p_reg)
{
    p_reg->DMA.RX.TERMINATEONBUSERROR |= SPIS_DMA_RX_TERMINATEONBUSERROR_ENABLE_Msk;
}

NRF_STATIC_INLINE void nrf_spis_rx_terminate_on_bus_error_disable(NRF_SPIS_Type * p_reg)
{
    p_reg->DMA.RX.TERMINATEONBUSERROR &= ~(SPIS_DMA_RX_TERMINATEONBUSERROR_ENABLE_Msk);
}

NRF_STATIC_INLINE bool nrf_spis_rx_terminate_on_bus_error_check(NRF_SPIS_Type const * p_reg)
{
    return ((p_reg->DMA.RX.TERMINATEONBUSERROR & SPIS_DMA_RX_TERMINATEONBUSERROR_ENABLE_Msk)
            >> SPIS_DMA_RX_TERMINATEONBUSERROR_ENABLE_Pos) ==
           SPIS_DMA_RX_TERMINATEONBUSERROR_ENABLE_Enabled;
}

NRF_STATIC_INLINE void nrf_spis_tx_terminate_on_bus_error_enable(NRF_SPIS_Type * p_reg)
{
    p_reg->DMA.TX.TERMINATEONBUSERROR |= SPIS_DMA_TX_TERMINATEONBUSERROR_ENABLE_Msk;
}

NRF_STATIC_INLINE void nrf_spis_tx_terminate_on_bus_error_disable(NRF_SPIS_Type * p_reg)
{
    p_reg->DMA.TX.TERMINATEONBUSERROR &= ~(SPIS_DMA_TX_TERMINATEONBUSERROR_ENABLE_Msk);
}

NRF_STATIC_INLINE bool nrf_spis_tx_terminate_on_bus_error_check(NRF_SPIS_Type const * p_reg)
{
    return ((p_reg->DMA.TX.TERMINATEONBUSERROR & SPIS_DMA_TX_TERMINATEONBUSERROR_ENABLE_Msk)
            >> SPIS_DMA_TX_TERMINATEONBUSERROR_ENABLE_Pos) ==
           SPIS_DMA_TX_TERMINATEONBUSERROR_ENABLE_Enabled;
}
#endif // NRF_SPIS_HAS_DMA_REG

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_SPIS_H__
