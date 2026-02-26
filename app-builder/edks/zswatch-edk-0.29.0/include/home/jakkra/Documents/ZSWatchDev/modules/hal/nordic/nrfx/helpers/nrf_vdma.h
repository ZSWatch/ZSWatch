/*
 * Copyright (c) 2020 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_VDMA_H__
#define NRF_VDMA_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_vdma Generic Vector DMA layer.
 * @{
 * @ingroup nrf_mvdma
 * @brief   Helper layer that provides the common functionality for Vector DMA (VDMA).
 */

/** @brief Maximum size of VDMA buffer. */
#define NRF_VDMA_BUFFER_SIZE_MASK VDMADESCRIPTOR_CONFIG_CNT_Msk

/** @brief Structure describing VDMA job. */
typedef struct
{
    uint8_t * p_buffer; ///< Pointer to the VDMA job buffer.
    struct __PACKED
    {
        uint32_t size:24;    ///< Size of the job buffer.
        uint8_t  attributes; ///< Attributes of the job.
    };
} nrf_vdma_job_t;

/** @brief Type describing VDMA job with fixed attributes and length. */
typedef uint32_t nrf_vdma_job_reduced_t;

/**
 * @brief VDMA attributes.
 *
 * @note Only one attribute can be set for the job.
 */
typedef enum
{
    NRF_VDMA_ATTRIBUTE_PLAIN_DATA           = 0x00, ///< Data is not modified.
    NRF_VDMA_ATTRIBUTE_BYTE_SWAP            = 0x07, ///< Data bytes are swapped.
    NRF_VDMA_ATTRIBUTE_JOB_LIST             = 0x08, ///< Allows chaining of joblists.
    NRF_VDMA_ATTRIBUTE_BUFFER_FILL          = 0x09, ///< Insters zeros into sink data buffers. Sink job attribute only.
    NRF_VDMA_ATTRIBUTE_FIXED_ATTR           = 0x0A, ///< Identical job attributes and sizes for all jobs in the list.
    NRF_VDMA_ATTRIBUTE_CRC                  = 0x0B, ///< CRC checksum is calculated on all data in the source list.
    NRF_VDMA_ATTRIBUTE_STATIC_ADDR          = 0x0C, ///< Memory address is fixed for the entirety of the job.
    NRF_VDMA_ATTRIBUTE_PLAIN_DATA_BUF_WRITE = 0x0D, ///< Used to get better write performance when many short bursts are beiing sent.
} nrf_vdma_attributes_t;

/**
 * @brief VDMA extended attributes.
 *
 * @note This attributes can be combined with each other and with standard attribute
 */
typedef enum
{
    NRF_VDMA_EXT_ATTRIBUTE_PERIPHERAL_MODE = 0x40, //< Job contains a pointer to a peripheral address.
    NRF_VDMA_EXT_ATTRIBUTE_EVENT_ENABLE    = 0x80, //< Enable event on job completion.
} nrf_vdma_ext_attribute_t;

/**
 * @brief Macro for computing size of the array of reduced job structures.
 *
 * @param[in] count Number of jobs to be stored in reduced job list.
 */
#define NRF_VDMA_REDUCED_JOB_SIZE(count) (count + 2)

/**
 * @brief Macro for defining an element of a job list.
 *
 * @param[in] p_buffer  Pointer to the buffer.
 * @param[in] size      Size of the transfer.
 * @param[in] attribute Attribute mask.
 *
 * @return Two words of the job descriptor.
 */
#define NRF_VDMA_JOB_ELEMENT(p_buffer, size, attribute)                   \
        (uint32_t)p_buffer,                                               \
        (uint32_t)(((attribute) << VDMADESCRIPTOR_CONFIG_ATTRIBUTE_Pos) | \
                   ((size) & NRF_VDMA_BUFFER_SIZE_MASK))

/**
 * @brief Macro for defining initial element of reduced job list.
 *
 * @param[in] p_buffer  Job buffer.
 * @param[in] size      Size of the job buffer.
 * @param[in] attribute Attributes of the job.
 *
 * @return Two words of the job descriptor.
 */
#define NRF_VDMA_REDUCED_JOB_INIT_ELEMENT(p_buffer, size, attribute) \
    NRF_VDMA_JOB_ELEMENT(p_buffer, size, NRF_VDMA_ATTRIBUTE_FIXED_ATTR | (attribute))

/**
 * @brief Macro for defining a job for transfer engaging peripheral.
 *
 * @param[in] addr Starting address.
 * @param[in] size Size of a DMA job.
 *
 * @return Two words which contains address, size and the descriptor byte.
 */
#define NRFX_VDMA_PERIPH_JOB(addr, size) \
        NRF_VDMA_JOB_ELEMENT(addr, size, NRF_VDMA_EXT_ATTRIBUTE_PERIPHERAL_MODE)

/**
 * @brief Function for filling the specified structure of the job with given job parameters.
 *
 * @param[out] p_job      Pointer to the structure of the job to be filled.
 * @param[in]  p_buffer   Job buffer.
 * @param[in]  size       Size of the job buffer.
 * @param[in]  attributes Attributes of the job.
 */
__STATIC_INLINE void nrf_vdma_job_fill(nrf_vdma_job_t * p_job,
                                       void *           p_buffer,
                                       size_t           size,
                                       uint8_t          attributes)
{
    p_job->p_buffer   = (uint8_t *)p_buffer;
    p_job->size       = (uint32_t)size & NRF_VDMA_BUFFER_SIZE_MASK;
    p_job->attributes = attributes;
}

/**
 * @brief Function for initializing the specified structure of the job with fixed attributes.
 *
 * First element of reduced job list occupies space for two elements.
 *
 * @note Use @ref nrf_vdma_job_terminate() to terminate reduced job list.
 *
 * @param[out] p_job      Pointer to the reduced structure of the job to be filled.
 * @param[in]  p_buffer   Job buffer.
 * @param[in]  size       Size of the job buffer.
 * @param[in]  attributes Additional attribute of the job.
 */
__STATIC_INLINE void nrf_vdma_job_reduced_init(nrf_vdma_job_reduced_t * p_job,
                                               void *                   p_buffer,
                                               size_t                   size,
                                               uint8_t                  attributes)
{
    *p_job       = (uint32_t)p_buffer;
    *(p_job + 1) = (uint32_t)(((NRF_VDMA_ATTRIBUTE_FIXED_ATTR | attributes) <<
                                VDMADESCRIPTOR_CONFIG_ATTRIBUTE_Pos) |
                                (size & NRF_VDMA_BUFFER_SIZE_MASK));
}

/**
 * @brief Function for filling the specified reduced structure of the job with given buffer pointer.
 *
 * @param[out] p_job    Pointer to the reduced structure of the job to be filled.
 * @param[in]  p_buffer Job buffer.
 */
__STATIC_INLINE void nrf_vdma_job_reduced_fill(nrf_vdma_job_reduced_t * p_job,
                                               void *                   p_buffer)
{
    *p_job = (uint32_t)p_buffer;
}

/**
 * @brief Function for getting the pointer to the buffer associated with specified job.
 *
 * @param[in] p_job Pointer to the structure of the specified job.
 *
 * @return Pointer to the job buffer.
 */
__STATIC_INLINE void * nrf_vdma_job_buffer_get(nrf_vdma_job_t const * p_job)
{
    return (void *)p_job->p_buffer;
}

/**
 * @brief Function for getting the size of the buffer associated with specified job.
 *
 * @param[in] p_job Pointer to the structure of the specified job.
 *
 * @return Size of the job buffer.
 */
__STATIC_INLINE size_t nrf_vdma_job_size_get(nrf_vdma_job_t const * p_job)
{
    return (size_t)p_job->size;
}

/**
 * @brief Function for terminating the specified job.
 *
 * When VectorDMA encounters job that is terminated, processing of the job list stops.
 *
 * @param[out] p_job Pointer to the structure of the job to be terminated.
 */
__STATIC_INLINE void nrf_vdma_job_terminate(nrf_vdma_job_t * p_job)
{
    p_job->p_buffer = NULL;
}

/**
 * @brief Function for linking the job with another job.
 *
 * When VectorDMA encounters job that is linked to another job,
 * execution of the linked job starts.
 *
 * @param[out] p_job        Pointer to the structure of the job to become link.
 * @param[in]  p_job_linked Pointer to the structure of the job to be linked.
 */
__STATIC_INLINE void nrf_vdma_job_link(nrf_vdma_job_t * p_job,
                                       nrf_vdma_job_t * p_job_linked)
{
    p_job->p_buffer   = (uint8_t *)p_job_linked;
    p_job->attributes = NRF_VDMA_ATTRIBUTE_JOB_LIST;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_VDMA_H__
