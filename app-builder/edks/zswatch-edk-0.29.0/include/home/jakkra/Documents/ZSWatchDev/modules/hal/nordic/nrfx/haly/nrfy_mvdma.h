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

#ifndef NRFY_MVDMA_H__
#define NRFY_MVDMA_H__

#include <nrfx.h>
#include <hal/nrf_mvdma.h>
#include <helpers/nrf_vdma.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nrfy_mvdma_list_request_t nrfy_mvdma_list_request_t;

NRFY_STATIC_INLINE bool __nrfy_internal_mvdma_event_handle(NRF_MVDMA_Type *  p_reg,
                                                           uint32_t          mask,
                                                           nrf_mvdma_event_t event,
                                                           uint32_t *        p_event_mask);

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_mvdma_events_process(NRF_MVDMA_Type *                  p_reg,
                                              uint32_t                          mask,
                                              nrfy_mvdma_list_request_t const * p_list_request);

NRFY_STATIC_INLINE void __nrfy_internal_mvdma_event_enabled_clear(NRF_MVDMA_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_mvdma_event_t event);

NRFY_STATIC_INLINE void __nrfy_internal_mvdma_source_buffers_flush(nrf_vdma_job_t * p_source_job);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_mvdma_sink_job_count_get(NRF_MVDMA_Type const * p_reg);

/**
 * @defgroup nrfy_mvdma MVDMA HALY
 * @{
 * @ingroup nrf_mvdma
 * @brief   Hardware access layer with cache and barrier support for managing the MVDMA peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

#if NRF_MVDMA_HAS_AXIMODE || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_MVDMA_HAS_AXIMODE} */
#define NRFY_MVDMA_HAS_AXIMODE 1
#else
#define NRFY_MVDMA_HAS_AXIMODE 0
#endif

/** @brief Structure describing list execution request for the MVDMA.*/
struct nrfy_mvdma_list_request_t
{
    nrf_vdma_job_t * p_source_job_list; ///< Pointer to the source job list.
    nrf_vdma_job_t * p_sink_job_list;   ///< Pointer to the sink job list.
};

#if NRF_MVDMA_HAS_MULTIMODE
/** @brief Structure describing lists of job list execution requests for the MVDMA. */
typedef struct
{
    nrf_vdma_job_t ** pp_source_job_lists; ///< Pointer to the list of the source job lists.
    nrf_vdma_job_t ** pp_sink_job_lists;   ///< Pointer to the list of the sink job lists.
    uint8_t           length;              ///< Length of the list of the sink/source job lists.
} nrfy_mvdma_multi_list_request_t;
#endif

/** @brief Auxiliary structure describing the MVDMA job list with unspecified direction. */
typedef struct
{
    nrf_vdma_job_t * p_jobs;    ///< Pointer to the job list.
    size_t           job_count; ///< Number of jobs executed, including terminating job.
    uint32_t         last_addr; ///< Last sink or source address accessed by the peripheral when the list was processed.
} nrfy_mvdma_list_desc_t;

/**
 * @brief Function for initializing the specified MVDMA interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_mvdma_int_init(NRF_MVDMA_Type * p_reg,
                                            uint32_t         mask,
                                            uint8_t          irq_priority,
                                            bool             enable)
{
    __nrfy_internal_mvdma_event_enabled_clear(p_reg, mask, NRF_MVDMA_EVENT_END);
    __nrfy_internal_mvdma_event_enabled_clear(p_reg, mask, NRF_MVDMA_EVENT_RESET);
    __nrfy_internal_mvdma_event_enabled_clear(p_reg, mask, NRF_MVDMA_EVENT_STARTED);
    __nrfy_internal_mvdma_event_enabled_clear(p_reg, mask, NRF_MVDMA_EVENT_PAUSED);
    __nrfy_internal_mvdma_event_enabled_clear(p_reg, mask, NRF_MVDMA_EVENT_SINKBUSERROR);
    __nrfy_internal_mvdma_event_enabled_clear(p_reg, mask, NRF_MVDMA_EVENT_SOURCEBUSERROR);
    __nrfy_internal_mvdma_event_enabled_clear(p_reg, mask, NRF_MVDMA_EVENT_SINKSELECTJOBDONE);
    __nrfy_internal_mvdma_event_enabled_clear(p_reg, mask, NRF_MVDMA_EVENT_SOURCESELECTJOBDONE);
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_mvdma_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the specified MVDMA interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_mvdma_int_uninit(NRF_MVDMA_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified MVDMA events.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] mask           Mask of events to be processed,
 *                           created by @ref NRFY_EVENT_TO_INT_BITMASK().
 * @param[in] p_list_request Pointer to the structure of list execution request associated with
 *                           the last operation. Can be NULL.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_events_process(NRF_MVDMA_Type *            p_reg,
                                                      uint32_t                    mask,
                                                      nrfy_mvdma_list_request_t * p_list_request)
{
    uint32_t evt_mask = __nrfy_internal_mvdma_events_process(p_reg, mask, p_list_request);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for starting the MVDMA jobs in single-mode.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] p_list_request Pointer to the structure of list execution request if the transaction
 *                           is to be blocking. NULL for non-blocking transactions.
 */
NRFY_STATIC_INLINE void nrfy_mvdma_start(NRF_MVDMA_Type *                  p_reg,
                                         nrfy_mvdma_list_request_t const * p_list_request)
{
    nrf_mvdma_task_trigger(p_reg, NRF_MVDMA_TASK_START0);
    if (p_list_request)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_MVDMA_EVENT_END);
        while (!__nrfy_internal_mvdma_events_process(p_reg, evt_mask, p_list_request))
        {}
    }
    nrf_barrier_w();
}

#if NRF_MVDMA_HAS_MULTIMODE || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for starting the MVDMA jobs in multi-mode.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] idx            Index of the job list that will be executed.
 * @param[in] p_list_request Pointer to the structure of lists execution request if the transaction
 *                           is to be blocking. NULL for non-blocking transactions.
 */
NRFY_STATIC_INLINE
void nrfy_mvdma_multi_start(NRF_MVDMA_Type *                        p_reg,
                            uint8_t                                 idx,
                            nrfy_mvdma_multi_list_request_t const * p_list_request)
{
    nrf_mvdma_task_trigger(p_reg, nrf_mvdma_start_task_get(p_reg, idx));
    if (p_list_request)
    {
        nrfy_mvdma_list_request_t list_req =
        {
            .p_source_job_list = p_list_request->pp_source_job_lists[idx],
            .p_sink_job_list   = p_list_request->pp_sink_job_lists[idx],
        };

        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_MVDMA_EVENT_END);
        while (!__nrfy_internal_mvdma_events_process(p_reg, evt_mask, &list_req))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for setting the MVDMA job lists in multi-mode.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] p_list_request Pointer to the structure of list execution request.
 */
NRFY_STATIC_INLINE
void nrfy_mvdma_multi_job_list_set(NRF_MVDMA_Type *                        p_reg,
                                   nrfy_mvdma_multi_list_request_t const * p_list_request)
{
    for (size_t i = 0; i < p_list_request->length; i++)
    {
        __nrfy_internal_mvdma_source_buffers_flush(p_list_request->pp_source_job_lists[i]);
    }

    nrf_mvdma_source_list_ptr_set(p_reg, (nrf_vdma_job_t *)p_list_request->pp_source_job_lists);
    nrf_mvdma_sink_list_ptr_set(p_reg, (nrf_vdma_job_t *)p_list_request->pp_sink_job_lists);
}

/**
 * @brief Function for getting the MVDMA job lists in multi-mode.
 *
 * @param[in]  p_reg          Pointer to the structure of registers of the peripheral.
 * @param[out] p_list_request Pointer to the structure to be filled with list execution request.
 */
NRFY_STATIC_INLINE
void nrfy_mvdma_multi_job_list_get(NRF_MVDMA_Type const *            p_reg,
                                   nrfy_mvdma_multi_list_request_t * p_list_request)
{
    p_list_request->pp_source_job_lists = (nrf_vdma_job_t **)nrf_mvdma_source_list_ptr_get(p_reg);
    p_list_request->pp_sink_job_lists = (nrf_vdma_job_t **)nrf_mvdma_sink_list_ptr_get(p_reg);
}

#endif // NRF_MVDMA_HAS_MULTIMODE || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for setting the MVDMA jobs.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] p_list_request Pointer to the structure of list execution request.
 */
NRFY_STATIC_INLINE void nrfy_mvdma_job_list_set(NRF_MVDMA_Type *                  p_reg,
                                                nrfy_mvdma_list_request_t const * p_list_request)
{
    __nrfy_internal_mvdma_source_buffers_flush(p_list_request->p_source_job_list);

    nrf_mvdma_source_list_ptr_set(p_reg, p_list_request->p_source_job_list);
    nrf_mvdma_sink_list_ptr_set(p_reg, p_list_request->p_sink_job_list);
    nrf_barrier_w();
}

/**
 * @brief Function for resetting the MVDMA peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] wait  True if reset is to be done in blocking mode, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_mvdma_reset(NRF_MVDMA_Type * p_reg,
                                         bool             wait)
{
    nrf_mvdma_task_trigger(p_reg, NRF_MVDMA_TASK_RESET);
    if (wait)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_MVDMA_EVENT_RESET);
        while (!__nrfy_internal_mvdma_events_process(p_reg, evt_mask, NULL))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for aborting the MVDMA transaction.
 *
 * @param[in] p_reg          Pointer to the structure of registers of the peripheral.
 * @param[in] p_list_request Pointer to the structure of list execution request.
 */
NRFY_STATIC_INLINE void nrfy_mvdma_abort(NRF_MVDMA_Type *                  p_reg,
                                         nrfy_mvdma_list_request_t const * p_list_request)
{
    nrf_mvdma_task_trigger(p_reg, NRF_MVDMA_TASK_PAUSE);

    if (p_list_request)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_MVDMA_EVENT_PAUSED);
        while (!__nrfy_internal_mvdma_events_process(p_reg, evt_mask, p_list_request))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for getting the MVDMA source job details
 *
 * @param[in]  p_reg           Pointer to the structure of registers of the peripheral.
 * @param[out] p_job_list_desc Pointer to the structure to be filled with job list description.
 */
NRFY_STATIC_INLINE
void nrfy_mvdma_source_job_description_get(NRF_MVDMA_Type const *   p_reg,
                                           nrfy_mvdma_list_desc_t * p_job_list_desc)
{
    nrf_barrier_rw();
    p_job_list_desc->p_jobs    = nrf_mvdma_source_list_ptr_get(p_reg);
    p_job_list_desc->job_count = nrf_mvdma_source_job_count_get(p_reg);
    p_job_list_desc->last_addr = nrf_mvdma_last_source_address_get(p_reg);
    nrf_barrier_r();
}

/**
 * @brief Function for getting the MVDMA sink job details
 *
 * @param[in]  p_reg           Pointer to the structure of registers of the peripheral.
 * @param[out] p_job_list_desc Pointer to the structure to be filled with job list description.
 */
NRFY_STATIC_INLINE
void nrfy_mvdma_sink_job_description_get(NRF_MVDMA_Type const *   p_reg,
                                         nrfy_mvdma_list_desc_t * p_job_list_desc)
{
    nrf_barrier_rw();
    p_job_list_desc->p_jobs    = nrf_mvdma_sink_list_ptr_get(p_reg);
    p_job_list_desc->job_count = nrf_mvdma_sink_job_count_get(p_reg);
    p_job_list_desc->last_addr = nrf_mvdma_last_sink_address_get(p_reg);
    nrf_barrier_r();
}

/** @refhal{nrf_mvdma_task_trigger} */
NRFY_STATIC_INLINE void nrfy_mvdma_task_trigger(NRF_MVDMA_Type * p_reg,
                                                nrf_mvdma_task_t task)
{
    nrf_mvdma_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_task_address_get(NRF_MVDMA_Type const * p_reg,
                                                        nrf_mvdma_task_t       task)
{
    return nrf_mvdma_task_address_get(p_reg, task);
}

/** @refhal{nrf_mvdma_start_task_get} */
NRFY_STATIC_INLINE nrf_mvdma_task_t nrfy_mvdma_start_task_get(NRF_MVDMA_Type const * p_reg,
                                                              uint8_t                index)
{
    return nrf_mvdma_start_task_get(p_reg, index);
}

/** @refhal{nrf_mvdma_event_clear} */
NRFY_STATIC_INLINE void nrfy_mvdma_event_clear(NRF_MVDMA_Type *  p_reg,
                                               nrf_mvdma_event_t event)
{
    nrf_mvdma_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_event_check} */
NRFY_STATIC_INLINE bool nrfy_mvdma_event_check(NRF_MVDMA_Type const * p_reg,
                                               nrf_mvdma_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_mvdma_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_mvdma_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_event_address_get(NRF_MVDMA_Type const * p_reg,
                                                         nrf_mvdma_event_t      event)
{
    return nrf_mvdma_event_address_get(p_reg, event);
}

/** @refhal{nrf_mvdma_int_enable} */
NRFY_STATIC_INLINE void nrfy_mvdma_int_enable(NRF_MVDMA_Type * p_reg,
                                              uint32_t         mask)
{
    nrf_mvdma_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_int_disable} */
NRFY_STATIC_INLINE void nrfy_mvdma_int_disable(NRF_MVDMA_Type * p_reg,
                                               uint32_t         mask)
{
    nrf_mvdma_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_int_enable_check(NRF_MVDMA_Type const * p_reg,
                                                        uint32_t               mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_mvdma_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_mvdma_int_pending_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_int_pending_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t pending = nrf_mvdma_int_pending_get(p_reg);
    nrf_barrier_r();
    return pending;
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_mvdma_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_mvdma_subscribe_set(NRF_MVDMA_Type * p_reg,
                                                 nrf_mvdma_task_t task,
                                                 uint8_t          channel)
{
    nrf_mvdma_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_mvdma_subscribe_clear(NRF_MVDMA_Type * p_reg,
                                                   nrf_mvdma_task_t task)
{
    nrf_mvdma_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_publish_set} */
NRFY_STATIC_INLINE void nrfy_mvdma_publish_set(NRF_MVDMA_Type *  p_reg,
                                               nrf_mvdma_event_t event,
                                               uint8_t           channel)
{
    nrf_mvdma_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_publish_clear} */
NRFY_STATIC_INLINE void nrfy_mvdma_publish_clear(NRF_MVDMA_Type *  p_reg,
                                                 nrf_mvdma_event_t event)
{
    nrf_mvdma_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_mvdma_mode_set} */
NRFY_STATIC_INLINE void nrfy_mvdma_mode_set(NRF_MVDMA_Type * p_reg,
                                            nrf_mvdma_mode_t mode)
{
    nrf_mvdma_mode_set(p_reg, mode);
    nrf_barrier_w();
}

#if NRFY_MVDMA_HAS_AXIMODE
/** @refhal{nrf_mvdma_aximode_set} */
NRFY_STATIC_INLINE void nrfy_mvdma_aximode_set(NRF_MVDMA_Type *    p_reg,
                                               nrf_mvdma_aximode_t aximode)
{
    nrf_mvdma_aximode_set(p_reg, aximode);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_mvdma_source_list_ptr_set} */
NRFY_STATIC_INLINE void nrfy_mvdma_source_list_ptr_set(NRF_MVDMA_Type *       p_reg,
                                                       nrf_vdma_job_t const * p_job)
{
    nrf_mvdma_source_list_ptr_set(p_reg, p_job);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_source_list_ptr_get} */
NRFY_STATIC_INLINE nrf_vdma_job_t * nrfy_mvdma_source_list_ptr_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_vdma_job_t * p_job = nrf_mvdma_source_list_ptr_get(p_reg);
    nrf_barrier_r();
    return p_job;
}

/** @refhal{nrf_mvdma_sink_list_ptr_set} */
NRFY_STATIC_INLINE void nrfy_mvdma_sink_list_ptr_set(NRF_MVDMA_Type *       p_reg,
                                                     nrf_vdma_job_t const * p_job)
{
    nrf_mvdma_sink_list_ptr_set(p_reg, p_job);
    nrf_barrier_w();
}

/** @refhal{nrf_mvdma_sink_list_ptr_get} */
NRFY_STATIC_INLINE nrf_vdma_job_t * nrfy_mvdma_sink_list_ptr_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_vdma_job_t * p_job = nrf_mvdma_sink_list_ptr_get(p_reg);
    nrf_barrier_r();
    return p_job;
}

/** @refhal{nrf_mvdma_crc_result_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_crc_result_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t crc_result = nrf_mvdma_crc_result_get(p_reg);
    nrf_barrier_r();
    return crc_result;
}

/** @refhal{nrf_mvdma_fifo_status_get} */
NRFY_STATIC_INLINE nrf_mvdma_fifo_status_t nrfy_mvdma_fifo_status_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    nrf_mvdma_fifo_status_t fifo_status = nrf_mvdma_fifo_status_get(p_reg);
    nrf_barrier_r();
    return fifo_status;
}

/** @refhal{nrf_mvdma_activity_check} */
NRFY_STATIC_INLINE bool nrfy_mvdma_activity_check(NRF_MVDMA_Type * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_mvdma_activity_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_mvdma_source_error_get} */
NRFY_STATIC_INLINE
nrf_mvdma_source_error_t nrfy_mvdma_source_error_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    nrf_mvdma_source_error_t error = nrf_mvdma_source_error_get(p_reg);
    nrf_barrier_r();
    return error;
}

/** @refhal{nrf_mvdma_sink_error_get} */
NRFY_STATIC_INLINE nrf_mvdma_sink_error_t nrfy_mvdma_sink_error_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    nrf_mvdma_sink_error_t error = nrf_mvdma_sink_error_get(p_reg);
    nrf_barrier_r();
    return error;
}

/** @refhal{nrf_mvdma_last_source_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_last_source_address_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t address = nrf_mvdma_last_source_address_get(p_reg);
    nrf_barrier_r();
    return address;
}

/** @refhal{nrf_mvdma_last_sink_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_last_sink_address_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t address = nrf_mvdma_last_sink_address_get(p_reg);
    nrf_barrier_r();
    return address;
}

/** @refhal{nrf_mvdma_source_job_count_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_source_job_count_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t job_count = nrf_mvdma_source_job_count_get(p_reg);
    nrf_barrier_r();
    return job_count;
}

/** @refhal{nrf_mvdma_sink_job_count_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mvdma_sink_job_count_get(NRF_MVDMA_Type const * p_reg)
{
    return __nrfy_internal_mvdma_sink_job_count_get(p_reg);
}

/** @} */

NRFY_STATIC_INLINE bool __nrfy_internal_mvdma_event_handle(NRF_MVDMA_Type *  p_reg,
                                                           uint32_t          mask,
                                                           nrf_mvdma_event_t event,
                                                           uint32_t *        p_event_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_mvdma_event_check(p_reg, event))
    {
        nrf_mvdma_event_clear(p_reg, event);
        if (p_event_mask)
        {
            *p_event_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_mvdma_events_process(NRF_MVDMA_Type *                  p_reg,
                                              uint32_t                          mask,
                                              nrfy_mvdma_list_request_t const * p_list_request)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    (void)__nrfy_internal_mvdma_event_handle(p_reg, mask, NRF_MVDMA_EVENT_RESET, &evt_mask);
    (void)__nrfy_internal_mvdma_event_handle(p_reg, mask, NRF_MVDMA_EVENT_STARTED, &evt_mask);
    (void)__nrfy_internal_mvdma_event_handle(p_reg, mask, NRF_MVDMA_EVENT_SINKBUSERROR, &evt_mask);
    (void)__nrfy_internal_mvdma_event_handle(p_reg,
                                             mask,
                                             NRF_MVDMA_EVENT_SOURCEBUSERROR,
                                             &evt_mask);
    (void)__nrfy_internal_mvdma_event_handle(p_reg,
                                             mask,
                                             NRF_MVDMA_EVENT_SINKSELECTJOBDONE,
                                             &evt_mask);
    (void)__nrfy_internal_mvdma_event_handle(p_reg,
                                             mask,
                                             NRF_MVDMA_EVENT_SOURCESELECTJOBDONE,
                                             &evt_mask);

    bool invalidated = false;

    if (__nrfy_internal_mvdma_event_handle(p_reg, mask, NRF_MVDMA_EVENT_PAUSED, &evt_mask))
    {
        size_t job_count = __nrfy_internal_mvdma_sink_job_count_get(p_reg);
        for (size_t i = 0; i < job_count; i++)
        {
            NRFY_CACHE_INV(p_list_request->p_sink_job_list[i].p_buffer,
                           p_list_request->p_sink_job_list[i].size);
        }
        invalidated = true;
    }

    if (__nrfy_internal_mvdma_event_handle(p_reg, mask, NRF_MVDMA_EVENT_END, &evt_mask) &&
        !invalidated)
    {
        for (nrf_vdma_job_t * p_job = p_list_request->p_sink_job_list;
             p_job->p_buffer != NULL;
             p_job++)
        {
            NRFY_CACHE_INV(p_job->p_buffer, p_job->size);
        }
    }

    return evt_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_mvdma_event_enabled_clear(NRF_MVDMA_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_mvdma_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_mvdma_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE void __nrfy_internal_mvdma_source_buffers_flush(nrf_vdma_job_t * p_source_job)
{
    // Recognize if nrf_vdma_reduced_job_t is being used.
    if (p_source_job->attributes & NRF_VDMA_ATTRIBUTE_FIXED_ATTR)
    {
        size_t size = p_source_job->size;
        nrf_vdma_job_reduced_t * p_job_reduced = (nrf_vdma_job_reduced_t *)
                                                 (p_source_job + 1)->p_buffer;

        NRFY_CACHE_WB(p_source_job->p_buffer, size);

        for (nrf_vdma_job_reduced_t * p_buffer = p_job_reduced; p_buffer != NULL; p_buffer++)
        {
            NRFY_CACHE_WB(p_buffer, size);
        }
    }
    else
    {
        for (nrf_vdma_job_t * p_job = p_source_job; p_job->p_buffer != NULL; p_job++)
        {
            NRFY_CACHE_WB(p_job->p_buffer, p_job->size);
        }
    }
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_mvdma_sink_job_count_get(NRF_MVDMA_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t job_count = nrf_mvdma_sink_job_count_get(p_reg);
    nrf_barrier_r();
    return job_count;
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_MVDMA_H__
