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

#ifndef NRF_MVDMA_H__
#define NRF_MVDMA_H__

#include <nrfx.h>
#include <helpers/nrf_vdma.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_mvdma_hal MVDMA HAL
 * @{
 * @ingroup nrf_mvdma
 * @brief   Hardware access layer for managing the Memory-to-Memory Vector DMA (MVDMA) peripheral.
 */

#if defined(MVDMA_CONFIG_AXIMODE_AXIMODE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether AXIMODE configuration is present. */
#define NRF_MVDMA_HAS_AXIMODE 1
#else
#define NRF_MVDMA_HAS_AXIMODE 0
#endif

#if (MVDMA_JOBLISTCOUNT > 1) || defined(__NRFX_DOXYGEN__)
/** @brief Macro for checking if multi-mode operation is available. */
#define NRF_MVDMA_HAS_MULTIMODE 1
#else
#define NRF_MVDMA_HAS_MULTIMODE 0
#endif

/** @brief MVDMA tasks. */
typedef enum
{
    NRF_MVDMA_TASK_PAUSE  = offsetof(NRF_MVDMA_Type, TASKS_PAUSE),    ///< Pause DMA transaction at next idle stage on memory bus.
    NRF_MVDMA_TASK_RESET  = offsetof(NRF_MVDMA_Type, TASKS_RESET),    ///< Return all registers to default state and FSMs to IDLE.
    NRF_MVDMA_TASK_START0 = offsetof(NRF_MVDMA_Type, TASKS_START[0]), ///< Start or continue processing of descriptor list 0.
    NRF_MVDMA_TASK_START1 = offsetof(NRF_MVDMA_Type, TASKS_START[1]), ///< Start or continue processing of descriptor list 1.
    NRF_MVDMA_TASK_START2 = offsetof(NRF_MVDMA_Type, TASKS_START[2]), ///< Start or continue processing of descriptor list 2.
    NRF_MVDMA_TASK_START3 = offsetof(NRF_MVDMA_Type, TASKS_START[3]), ///< Start or continue processing of descriptor list 3.
    NRF_MVDMA_TASK_START4 = offsetof(NRF_MVDMA_Type, TASKS_START[4]), ///< Start or continue processing of descriptor list 4.
    NRF_MVDMA_TASK_START5 = offsetof(NRF_MVDMA_Type, TASKS_START[5]), ///< Start or continue processing of descriptor list 5.
    NRF_MVDMA_TASK_START6 = offsetof(NRF_MVDMA_Type, TASKS_START[6]), ///< Start or continue processing of descriptor list 6.
    NRF_MVDMA_TASK_START7 = offsetof(NRF_MVDMA_Type, TASKS_START[7]), ///< Start or continue processing of descriptor list 7.
} nrf_mvdma_task_t;

/** @brief MVDMA events. */
typedef enum
{
    NRF_MVDMA_EVENT_END                 = offsetof(NRF_MVDMA_Type, EVENTS_END),                  ///< Sink data descriptor list has been completed.
    NRF_MVDMA_EVENT_STARTED             = offsetof(NRF_MVDMA_Type, EVENTS_STARTED),              ///< Data descriptor list processing has been started.
    NRF_MVDMA_EVENT_PAUSED              = offsetof(NRF_MVDMA_Type, EVENTS_PAUSED),               ///< Data transfer has been paused.
    NRF_MVDMA_EVENT_RESET               = offsetof(NRF_MVDMA_Type, EVENTS_RESET),                ///< MVDMA has been reset.
    NRF_MVDMA_EVENT_SOURCEBUSERROR      = offsetof(NRF_MVDMA_Type, EVENTS_SOURCE.BUSERROR),      ///< Bus error has been received on the source channel.
    NRF_MVDMA_EVENT_SOURCESELECTJOBDONE = offsetof(NRF_MVDMA_Type, EVENTS_SOURCE.SELECTJOBDONE), ///< Job on the source channel with event enable attribute bit active has been processed.
    NRF_MVDMA_EVENT_SINKBUSERROR        = offsetof(NRF_MVDMA_Type, EVENTS_SINK.BUSERROR),        ///< Bus error has been received on the sink channel.
    NRF_MVDMA_EVENT_SINKSELECTJOBDONE   = offsetof(NRF_MVDMA_Type, EVENTS_SINK.SELECTJOBDONE),   ///< Job on the sink channel with event enable attribute bit active has been processed.
    NRF_MVDMA_EVENT_COMPLETED0          = offsetof(NRF_MVDMA_Type, EVENTS_COMPLETED[0]),         ///< Job 0 is completed.
    NRF_MVDMA_EVENT_COMPLETED1          = offsetof(NRF_MVDMA_Type, EVENTS_COMPLETED[1]),         ///< Job 1 is completed.
    NRF_MVDMA_EVENT_COMPLETED2          = offsetof(NRF_MVDMA_Type, EVENTS_COMPLETED[2]),         ///< Job 2 is completed.
    NRF_MVDMA_EVENT_COMPLETED3          = offsetof(NRF_MVDMA_Type, EVENTS_COMPLETED[3]),         ///< Job 3 is completed.
} nrf_mvdma_event_t;

/** @brief MVDMA interrupts. */
typedef enum
{
    NRF_MVDMA_INT_END_MASK                 = MVDMA_INTENSET_END_Msk,                 ///< Interrupt on END event.
    NRF_MVDMA_INT_STARTED_MASK             = MVDMA_INTENSET_STARTED_Msk,             ///< Interrupt on STARTED event.
    NRF_MVDMA_INT_PAUSED_MASK              = MVDMA_INTENSET_PAUSED_Msk,              ///< Interrupt on PAUSED event.
    NRF_MVDMA_INT_RESET_MASK               = MVDMA_INTENSET_RESET_Msk,               ///< Interrupt on RESET event.
    NRF_MVDMA_INT_SOURCEBUSERROR_MASK      = MVDMA_INTENSET_SOURCEBUSERROR_Msk,      ///< Interrupt on SOURCEBUSERROR event.
    NRF_MVDMA_INT_SINKBUSERROR_MASK        = MVDMA_INTENSET_SINKBUSERROR_Msk,        ///< Interrupt on SINKBUSERROR event.
    NRF_MVDMA_INT_SOURCESELECTJOBDONE_MASK = MVDMA_INTENSET_SOURCESELECTJOBDONE_Msk, ///< Interrupt on SOURCESELECTJOBDONE event.
    NRF_MVDMA_INT_SINKSELECTJOBDONE_MASK   = MVDMA_INTENSET_SINKSELECTJOBDONE_Msk,   ///< Interrupt on SINKSELECTJOBDONE event.
    NRF_MVDMA_INT_COMPLETED0_MASK          = MVDMA_INTENSET_COMPLETED0_Msk,          ///< Interrupt on COMPLETED0 event.
    NRF_MVDMA_INT_COMPLETED1_MASK          = MVDMA_INTENSET_COMPLETED1_Msk,          ///< Interrupt on COMPLETED1 event.
    NRF_MVDMA_INT_COMPLETED2_MASK          = MVDMA_INTENSET_COMPLETED2_Msk,          ///< Interrupt on COMPLETED2 event.
    NRF_MVDMA_INT_COMPLETED3_MASK          = MVDMA_INTENSET_COMPLETED3_Msk,          ///< Interrupt on COMPLETED3 event.
} nrf_mvdma_int_mask_t;

/** @brief MVDMA modes of operation. */
typedef enum
{
    NRF_MVDMA_MODE_SINGLE = MVDMA_CONFIG_MODE_MODE_SingleMode, ///< Descriptor list pointers are stored in SOURCELISTPTR and SINKLISTPTR registers.
    NRF_MVDMA_MODE_MULTI  = MVDMA_CONFIG_MODE_MODE_MultiMode,  ///< Descriptor list pointers are stored in a list in the memory.
} nrf_mvdma_mode_t;

#if NRF_MVDMA_HAS_AXIMODE
/** @brief MVDMA AXI modes. */
typedef enum
{
    NRF_MVDMA_AXIMODE_AXI     = MVDMA_CONFIG_AXIMODE_AXIMODE_AXI,     ///< AXI burst transactions may be longer than one beat.
    NRF_MVDMA_AXIMODE_AXILITE = MVDMA_CONFIG_AXIMODE_AXIMODE_AXILITE, ///< All AXI transactions are one-beat accesses.
} nrf_mvdma_aximode_t;
#endif

/** @brief MVDMA FIFO status. */
typedef enum
{
    NRF_MVDMA_FIFO_STATUS_EMPTY       = MVDMA_STATUS_FIFO_FIFOSTATUS_Empty,      ///< No data in intermediate FIFO.
    NRF_MVDMA_FIFO_STATUS_NOT_EMPTY   = MVDMA_STATUS_FIFO_FIFOSTATUS_NotEmpty,   ///< Intermediate FIFO contains data.
    NRF_MVDMA_FIFO_STATUS_FULL        = MVDMA_STATUS_FIFO_FIFOSTATUS_Full,       ///< Intermediate FIFO is full.
} nrf_mvdma_fifo_status_t;

/** @brief MVDMA data source errors. */
typedef enum
{
    NRF_MVDMA_SOURCE_ERROR_NONE        = MVDMA_SOURCE_BUSERROR_BUSERROR_NoError,           ///< No error.
    NRF_MVDMA_SOURCE_ERROR_READ        = MVDMA_SOURCE_BUSERROR_BUSERROR_ReadError,         ///< Error related to memory when reading joblist or memory/register when reading data.
    NRF_MVDMA_SOURCE_ERROR_READ_DECODE = MVDMA_SOURCE_BUSERROR_BUSERROR_ReadDecodeError,   ///< Error related to the joblist address or address when reading memory/register.
} nrf_mvdma_source_error_t;

/** @brief MVDMA data sink errors. */
typedef enum
{
    NRF_MVDMA_SINK_ERROR_NONE         = MVDMA_SINK_BUSERROR_BUSERROR_NoError,          ///< No error.
    NRF_MVDMA_SINK_ERROR_READ         = MVDMA_SINK_BUSERROR_BUSERROR_ReadError,        ///< Error related to memory when reading joblist.
    NRF_MVDMA_SINK_ERROR_WRITE        = MVDMA_SINK_BUSERROR_BUSERROR_WriteError,       ///< Error related to memory/register when writing data.
    NRF_MVDMA_SINK_ERROR_DECODE_READ  = MVDMA_SINK_BUSERROR_BUSERROR_ReadDecodeError,  ///< Error related to the joblist address when reading joblist.
    NRF_MVDMA_SINK_ERROR_DECODE_WRITE = MVDMA_SINK_BUSERROR_BUSERROR_WriteDecodeError, ///< Error related to the memory/register address when writing data.
} nrf_mvdma_sink_error_t;

/**
 * @brief Function for activating the specified MVDMA task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_mvdma_task_trigger(NRF_MVDMA_Type * p_reg,
                                              nrf_mvdma_task_t task);

/**
 * @brief Function for getting the address of the specified MVDMA task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_task_address_get(NRF_MVDMA_Type const * p_reg,
                                                      nrf_mvdma_task_t       task);

/**
 * @brief Function for getting START task by its index.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of the START task.
 *
 * @return START task.
 */
NRF_STATIC_INLINE nrf_mvdma_task_t nrf_mvdma_start_task_get(NRF_MVDMA_Type const * p_reg,
                                                            uint8_t                index);

/**
 * @brief Function for clearing the specified MVDMA event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_mvdma_event_clear(NRF_MVDMA_Type *  p_reg,
                                             nrf_mvdma_event_t event);

/**
 * @brief Function for retrieving the state of the MVDMA event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_mvdma_event_check(NRF_MVDMA_Type const * p_reg,
                                             nrf_mvdma_event_t      event);

/**
 * @brief Function for getting the address of the specified MVDMA event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_event_address_get(NRF_MVDMA_Type const * p_reg,
                                                       nrf_mvdma_event_t      event);

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_mvdma_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_mvdma_int_enable(NRF_MVDMA_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_mvdma_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_mvdma_int_disable(NRF_MVDMA_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_mvdma_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_int_enable_check(NRF_MVDMA_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * @note States of pending interrupt are saved as a bitmask.
 *       One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_mvdma_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_int_pending_get(NRF_MVDMA_Type const * p_reg);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        MVDMA task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_mvdma_subscribe_set(NRF_MVDMA_Type * p_reg,
                                               nrf_mvdma_task_t task,
                                               uint8_t          channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        MVDMA task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_mvdma_subscribe_clear(NRF_MVDMA_Type * p_reg,
                                                 nrf_mvdma_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        MVDMA task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return MVDMA subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_subscribe_get(NRF_MVDMA_Type const * p_reg,
                                                   nrf_mvdma_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        MVDMA event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_mvdma_publish_set(NRF_MVDMA_Type *  p_reg,
                                             nrf_mvdma_event_t event,
                                             uint8_t           channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        MVDMA event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_mvdma_publish_clear(NRF_MVDMA_Type *  p_reg,
                                               nrf_mvdma_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        MVDMA event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return MVDMA publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_publish_get(NRF_MVDMA_Type const * p_reg,
                                                 nrf_mvdma_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for setting the MVDMA mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Desired operating mode for MVDMA.
 */
NRF_STATIC_INLINE void nrf_mvdma_mode_set(NRF_MVDMA_Type * p_reg, nrf_mvdma_mode_t mode);

#if NRF_MVDMA_HAS_AXIMODE
/**
 * @brief Function for setting the AXI mode.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] aximode Desired AXI mode for MVDMA.
 */
NRF_STATIC_INLINE void nrf_mvdma_aximode_set(NRF_MVDMA_Type * p_reg, nrf_mvdma_aximode_t aximode);
#endif

/**
 * @brief Function for setting the pointer to the source descriptor list
 *        or pointer to the list of descriptor list pointers,
 *        depending on configured @ref nrf_mvdma_mode_t mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_job Pointer to a job list.
 */
NRF_STATIC_INLINE void nrf_mvdma_source_list_ptr_set(NRF_MVDMA_Type *       p_reg,
                                                     nrf_vdma_job_t const * p_job);

/**
 * @brief Function for getting the pointer to the source descriptor list
 *        or pointer to the list of descriptor list pointers,
 *        depending on configured @ref nrf_mvdma_mode_t mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to a job list.
 */
NRF_STATIC_INLINE nrf_vdma_job_t * nrf_mvdma_source_list_ptr_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the pointer to the sink descriptor list
 *        or pointer to the list of descriptor list pointers,
 *        depending on configured @ref nrf_mvdma_mode_t mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_job Pointer to a job list.
 */
NRF_STATIC_INLINE void nrf_mvdma_sink_list_ptr_set(NRF_MVDMA_Type *       p_reg,
                                                   nrf_vdma_job_t const * p_job);

/**
 * @brief Function for getting the pointer to the sink descriptor list
 *        or pointer to the list of descriptor list pointers,
 *        depending on configured @ref nrf_mvdma_mode_t mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to a job list.
 */
NRF_STATIC_INLINE nrf_vdma_job_t * nrf_mvdma_sink_list_ptr_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the result of CRC checksum calculation.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Result of CRC checksum calculation.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_crc_result_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the status of intermediate FIFO.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Intermediate FIFO status.
 */
NRF_STATIC_INLINE nrf_mvdma_fifo_status_t nrf_mvdma_fifo_status_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for checking the MVDMA activity.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  MVDMA is processing data.
 * @retval false MVDMA is idle.
 */
NRF_STATIC_INLINE bool nrf_mvdma_activity_check(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the bus error of MVDMA data source.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bus error of data source.
 */
NRF_STATIC_INLINE nrf_mvdma_source_error_t nrf_mvdma_source_error_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the bus error of MVDMA data source.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bus error of data sink.
 */
NRF_STATIC_INLINE nrf_mvdma_sink_error_t nrf_mvdma_sink_error_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the latest address being accessed on the source AXI channel.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Latest address being accessed on the source AXI channel.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_last_source_address_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the latest address being accessed on the sink AXI channel.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Latest address being accessed on the sink AXI channel.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_last_sink_address_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the number of completed jobs in the current source descriptor list.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of completed jobs in the current source descriptor list.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_source_job_count_get(NRF_MVDMA_Type const * p_reg);

/**
 * @brief Function for getting the number of completed jobs in the current sink descriptor list.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of completed jobs in the current sink descriptor list.
 */
NRF_STATIC_INLINE uint32_t nrf_mvdma_sink_job_count_get(NRF_MVDMA_Type const * p_reg);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_mvdma_task_trigger(NRF_MVDMA_Type * p_reg,
                                              nrf_mvdma_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1uL;
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_task_address_get(NRF_MVDMA_Type const * p_reg,
                                                      nrf_mvdma_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE nrf_mvdma_task_t nrf_mvdma_start_task_get(NRF_MVDMA_Type const * p_reg,
                                                            uint8_t                index)
{
    (void)p_reg;
    return (nrf_mvdma_task_t)(NRFX_OFFSETOF(NRF_MVDMA_Type, TASKS_START[index]));
}

NRF_STATIC_INLINE void nrf_mvdma_event_clear(NRF_MVDMA_Type *  p_reg,
                                             nrf_mvdma_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0uL;
}

NRF_STATIC_INLINE bool nrf_mvdma_event_check(NRF_MVDMA_Type const * p_reg,
                                             nrf_mvdma_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_event_address_get(NRF_MVDMA_Type const * p_reg,
                                                       nrf_mvdma_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_mvdma_int_enable(NRF_MVDMA_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_mvdma_int_disable(NRF_MVDMA_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_int_enable_check(NRF_MVDMA_Type const * p_reg, uint32_t mask)
{
    return (p_reg->INTENSET & mask);
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_int_pending_get(NRF_MVDMA_Type const * p_reg)
{
    return p_reg->INTPEND;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_mvdma_subscribe_set(NRF_MVDMA_Type * p_reg,
                                               nrf_mvdma_task_t task,
                                               uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_mvdma_subscribe_clear(NRF_MVDMA_Type * p_reg,
                                                 nrf_mvdma_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_subscribe_get(NRF_MVDMA_Type const * p_reg,
                                                   nrf_mvdma_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_mvdma_publish_set(NRF_MVDMA_Type *  p_reg,
                                             nrf_mvdma_event_t event,
                                             uint8_t           channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_mvdma_publish_clear(NRF_MVDMA_Type *  p_reg,
                                               nrf_mvdma_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_publish_get(NRF_MVDMA_Type const * p_reg,
                                                 nrf_mvdma_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE void nrf_mvdma_mode_set(NRF_MVDMA_Type * p_reg, nrf_mvdma_mode_t mode)
{
    p_reg->CONFIG.MODE = ((uint32_t)mode << MVDMA_CONFIG_MODE_MODE_Pos);
}

#if NRF_MVDMA_HAS_AXIMODE
NRF_STATIC_INLINE void nrf_mvdma_aximode_set(NRF_MVDMA_Type * p_reg, nrf_mvdma_aximode_t aximode)
{
    p_reg->CONFIG.AXIMODE = ((uint32_t)aximode << MVDMA_CONFIG_AXIMODE_AXIMODE_Pos);
}
#endif

NRF_STATIC_INLINE void nrf_mvdma_source_list_ptr_set(NRF_MVDMA_Type *       p_reg,
                                                     nrf_vdma_job_t const * p_job)
{
    p_reg->SOURCE.LISTPTR = (uint32_t)p_job;
}

NRF_STATIC_INLINE nrf_vdma_job_t * nrf_mvdma_source_list_ptr_get(NRF_MVDMA_Type const * p_reg)
{
    return (nrf_vdma_job_t *)(p_reg->SOURCE.LISTPTR);
}

NRF_STATIC_INLINE void nrf_mvdma_sink_list_ptr_set(NRF_MVDMA_Type *       p_reg,
                                                   nrf_vdma_job_t const * p_job)
{
    p_reg->SINK.LISTPTR = (uint32_t)p_job;
}

NRF_STATIC_INLINE nrf_vdma_job_t * nrf_mvdma_sink_list_ptr_get(NRF_MVDMA_Type const * p_reg)
{
    return (nrf_vdma_job_t *)(p_reg->SINK.LISTPTR);
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_crc_result_get(NRF_MVDMA_Type const * p_reg)
{
    return p_reg->STATUS.CRCRESULT;
}

NRF_STATIC_INLINE nrf_mvdma_fifo_status_t nrf_mvdma_fifo_status_get(NRF_MVDMA_Type const * p_reg)
{
    return (nrf_mvdma_fifo_status_t)(p_reg->STATUS.FIFO);
}

NRF_STATIC_INLINE bool nrf_mvdma_activity_check(NRF_MVDMA_Type const * p_reg)
{
    return (p_reg->STATUS.ACTIVE ==
            (MVDMA_STATUS_ACTIVE_ACTIVE_Active << MVDMA_STATUS_ACTIVE_ACTIVE_Pos));
}

NRF_STATIC_INLINE nrf_mvdma_source_error_t nrf_mvdma_source_error_get(NRF_MVDMA_Type const * p_reg)
{
    return (nrf_mvdma_source_error_t)(p_reg->SOURCE.BUSERROR);
}

NRF_STATIC_INLINE nrf_mvdma_sink_error_t nrf_mvdma_sink_error_get(NRF_MVDMA_Type const * p_reg)
{
    return (nrf_mvdma_sink_error_t)(p_reg->SINK.BUSERROR);
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_last_source_address_get(NRF_MVDMA_Type const * p_reg)
{
    return p_reg->SOURCE.ADDRESS;
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_last_sink_address_get(NRF_MVDMA_Type const * p_reg)
{
    return p_reg->SINK.ADDRESS;
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_source_job_count_get(NRF_MVDMA_Type const * p_reg)
{
    return p_reg->SOURCE.JOBCOUNT;
}

NRF_STATIC_INLINE uint32_t nrf_mvdma_sink_job_count_get(NRF_MVDMA_Type const * p_reg)
{
    return p_reg->SINK.JOBCOUNT;
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_MVDMA_H__
