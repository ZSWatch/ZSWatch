/*
 * Copyright (c) 2012 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_ECB_H__
#define NRF_ECB_H__

#include <nrfx.h>
#ifdef EASYVDMA_PRESENT
#include <helpers/nrf_vdma.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_ecb_hal AES ECB encryption HAL
 * @{
 * @ingroup nrf_ecb
 * @brief   Hardware access layer (HAL) for managing the Advanced Encryption Standard (AES) Electronic Codebook (ECB) peripheral.
 */

#if defined(ECB_TASKS_STARTECB_TASKS_STARTECB_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the STARTECB task. */
#define NRF_ECB_HAS_TASK_STARTECB 1
#else
#define NRF_ECB_HAS_TASK_STARTECB 0
#endif

#if defined(ECB_TASKS_START_TASKS_START_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the START task. */
#define NRF_ECB_HAS_TASK_START 1
#else
#define NRF_ECB_HAS_TASK_START 0
#endif

#if defined(ECB_TASKS_STOPECB_TASKS_STOPECB_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the STOPECB task. */
#define NRF_ECB_HAS_TASK_STOPECB 1
#else
#define NRF_ECB_HAS_TASK_STOPECB 0
#endif

#if defined(ECB_TASKS_STOP_TASKS_STOP_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the STOP task. */
#define NRF_ECB_HAS_TASK_STOP 1
#else
#define NRF_ECB_HAS_TASK_STOP 0
#endif

#if defined(ECB_EVENTS_ENDECB_EVENTS_ENDECB_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the ENDECB event. */
#define NRF_ECB_HAS_EVENT_ENDECB 1
#else
#define NRF_ECB_HAS_EVENT_ENDECB 0
#endif

#if defined(ECB_EVENTS_END_EVENTS_END_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the END event. */
#define NRF_ECB_HAS_EVENT_END 1
#else
#define NRF_ECB_HAS_EVENT_END 0
#endif

#if defined(ECB_EVENTS_ERRORECB_EVENTS_ERRORECB_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the ERRORECB event. */
#define NRF_ECB_HAS_EVENT_ERRORECB 1
#else
#define NRF_ECB_HAS_EVENT_ERRORECB 0
#endif

#if defined(ECB_EVENTS_ERROR_EVENTS_ERROR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the ERROR event. */
#define NRF_ECB_HAS_EVENT_ERROR 1
#else
#define NRF_ECB_HAS_EVENT_ERROR 0
#endif

#if defined(ECB_KEY_VALUE_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the KEY register. */
#define NRF_ECB_HAS_KEY 1
#else
#define NRF_ECB_HAS_KEY 0
#endif

#if defined(ECB_IN_PTR_PTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the IN.PTR register. */
#define NRF_ECB_HAS_IN_PTR 1
#else
#define NRF_ECB_HAS_IN_PTR 0
#endif

#if defined(ECB_IN_AMOUNT_AMOUNT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the IN.AMOUNT register. */
#define NRF_ECB_HAS_IN_AMOUNT 1
#else
#define NRF_ECB_HAS_IN_AMOUNT 0
#endif

#if defined(ECB_OUT_PTR_PTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the OUT.PTR register. */
#define NRF_ECB_HAS_OUT_PTR 1
#else
#define NRF_ECB_HAS_OUT_PTR 0
#endif

#if defined(ECB_OUT_AMOUNT_AMOUNT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the OUT.AMOUNT register. */
#define NRF_ECB_HAS_OUT_AMOUNT 1
#else
#define NRF_ECB_HAS_OUT_AMOUNT 0
#endif

#if defined(ECB_ECBDATAPTR_ECBDATAPTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the  ECBDATAPTR register. */
#define NRF_ECB_HAS_ECBDATAPTR 1
#else
#define NRF_ECB_HAS_ECBDATAPTR 0
#endif

/** @brief ECB tasks. */
typedef enum
{
#if NRF_ECB_HAS_TASK_STARTECB
    NRF_ECB_TASK_STARTECB = offsetof(NRF_ECB_Type, TASKS_STARTECB), /**< Task for starting the ECB block encryption. */
#endif
#if NRF_ECB_HAS_TASK_START
    NRF_ECB_TASK_START    = offsetof(NRF_ECB_Type, TASKS_START),    /**< Task for starting the ECB block encryption. */
#endif
#if NRF_ECB_HAS_TASK_STOPECB
    NRF_ECB_TASK_STOPECB  = offsetof(NRF_ECB_Type, TASKS_STOPECB),  /**< Task for stopping the ECB block encryption. */
#endif
#if NRF_ECB_HAS_TASK_STOP
    NRF_ECB_TASK_STOP     = offsetof(NRF_ECB_Type, TASKS_STOP),     /**< Task for stopping the ECB block encryption. */
#endif
} nrf_ecb_task_t;

/** @brief ECB events. */
typedef enum
{
#if NRF_ECB_HAS_EVENT_ENDECB
    NRF_ECB_EVENT_ENDECB   = offsetof(NRF_ECB_Type, EVENTS_ENDECB),   /**< ECB block encrypt complete. */
#endif
#if NRF_ECB_HAS_EVENT_ERRORECB
    NRF_ECB_EVENT_ERRORECB = offsetof(NRF_ECB_Type, EVENTS_ERRORECB), /**< ECB block encrypt aborted because of a STOPECB task or due to an error. */
#endif
#if NRF_ECB_HAS_EVENT_END
    NRF_ECB_EVENT_END      = offsetof(NRF_ECB_Type, EVENTS_END),      /**< ECB block encrypt complete. */
#endif
#if NRF_ECB_HAS_EVENT_ERROR
    NRF_ECB_EVENT_ERROR    = offsetof(NRF_ECB_Type, EVENTS_ERROR),    /**< ECB block encrypt aborted because of a STOPECB task or due to an error. */
#endif
} nrf_ecb_event_t;

/** @brief ECB interrupts. */
typedef enum
{
#if NRF_ECB_HAS_EVENT_ENDECB
    NRF_ECB_INT_ENDECB_MASK   = ECB_INTENSET_ENDECB_Msk,   ///< Interrupt on ENDECB event.
#endif
#if NRF_ECB_HAS_EVENT_ERRORECB
    NRF_ECB_INT_ERRORECB_MASK = ECB_INTENSET_ERRORECB_Msk, ///< Interrupt on ERRORECB event.
#endif
#if NRF_ECB_HAS_EVENT_END
    NRF_ECB_INT_END_MASK      = ECB_INTENSET_END_Msk,      ///< Interrupt on END event.
#endif
#if NRF_ECB_HAS_EVENT_ERROR
    NRF_ECB_INT_ERROR_MASK    = ECB_INTENSET_ERROR_Msk,    ///< Interrupt on ERROR event.
#endif
} nrf_ecb_int_mask_t;


/**
 * @brief Function for activating the specified ECB task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_ecb_task_trigger(NRF_ECB_Type * p_reg, nrf_ecb_task_t task);

/**
 * @brief Function for getting the address of the specified ECB task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_ecb_task_address_get(NRF_ECB_Type const * p_reg,
                                                    nrf_ecb_task_t       task);

/**
 * @brief Function for clearing the specified ECB event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_ecb_event_clear(NRF_ECB_Type * p_reg, nrf_ecb_event_t event);

/**
 * @brief Function for retrieving the state of the ECB event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_ecb_event_check(NRF_ECB_Type const * p_reg, nrf_ecb_event_t event);

/**
 * @brief Function for getting the address of the specified ECB event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_ecb_event_address_get(NRF_ECB_Type const * p_reg,
                                                     nrf_ecb_event_t      event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_ecb_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_ecb_int_enable(NRF_ECB_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_ecb_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_ecb_int_disable(NRF_ECB_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_ecb_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_ecb_int_enable_check(NRF_ECB_Type const * p_reg, uint32_t mask);

#if NRF_ECB_HAS_ECBDATAPTR
/**
 * @brief Function for setting the pointer to the ECB data buffer.
 *
 * @note The buffer has to be placed in the Data RAM region.
 *       For description of the data structure in this buffer, see the Product Specification.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the ECB data buffer.
 */
NRF_STATIC_INLINE void nrf_ecb_data_pointer_set(NRF_ECB_Type * p_reg, void const * p_buffer);

/**
 * @brief Function for getting the pointer to the ECB data buffer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the ECB data buffer.
 */
NRF_STATIC_INLINE void * nrf_ecb_data_pointer_get(NRF_ECB_Type const * p_reg);
#endif // NRF_ECB_HAS_ECBDATAPTR

#if NRF_ECB_HAS_KEY
/**
 * @brief Function for setting the AES key.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_key Pointer to the AES 128-bit key value. The key shall be stored
 *                  in big endian byte order.
 */
NRF_STATIC_INLINE void nrf_ecb_key_set(NRF_ECB_Type   * p_reg,
                                       uint32_t const * p_key);
#endif // NRF_ECB_HAS_KEY

#if NRF_ECB_HAS_IN_PTR
/**
 * @brief Function for setting the pointer to a job list containing unencrypted
 *        ECB data structure in Encryption mode or encrypted ECB data structure
 *        in Decryption mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_job Pointer to a job list.
 */
NRF_STATIC_INLINE void nrf_ecb_in_ptr_set(NRF_ECB_Type *         p_reg,
                                          nrf_vdma_job_t const * p_job);

/**
 * @brief Function for getting the pointer to job list containing unencrypted
 *        ECB data structure in Encryption mode or encrypted ECB data structure
 *        in Decryption mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to a job list.
 */
NRF_STATIC_INLINE nrf_vdma_job_t * nrf_ecb_in_ptr_get(NRF_ECB_Type const * p_reg);
#endif // NRF_ECB_HAS_IN_PTR

#if NRF_ECB_HAS_IN_AMOUNT
/**
 * @brief Function for getting number of bytes read from the input data,
 *        not including the job list structure.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes read from the input data.
 */
NRF_STATIC_INLINE uint32_t nrf_ecb_in_amount_get(NRF_ECB_Type const * p_reg);
#endif // NRF_ECB_HAS_IN_AMOUNT

#if NRF_ECB_HAS_OUT_PTR
/**
 * @brief Function for setting the pointer to a job list containing encrypted
 *        ECB data structure in Encryption mode or decrypted ECB data structure
 *        in Decryption mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_job Pointer to a job list.
 */
NRF_STATIC_INLINE void nrf_ecb_out_ptr_set(NRF_ECB_Type *         p_reg,
                                           nrf_vdma_job_t const * p_job);

/**
 * @brief Function for getting the pointer to a job list containing encrypted
 *        ECB data structure in Encryption mode or decrypted ECB data structure
 *        in Decryption mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the job list.
 */
NRF_STATIC_INLINE nrf_vdma_job_t * nrf_ecb_out_ptr_get(NRF_ECB_Type const * p_reg);
#endif // NRF_ECB_HAS_OUT_PTR

#if NRF_ECB_HAS_OUT_AMOUNT
/**
 * @brief Function for getting number of bytes available in the output data,
 *        not including the job list structure.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes available in the output data.
 */
NRF_STATIC_INLINE uint32_t nrf_ecb_out_amount_get(NRF_ECB_Type const * p_reg);
#endif // NRF_ECB_HAS_OUT_AMOUNT

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        ECB task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_ecb_subscribe_set(NRF_ECB_Type * p_reg,
                                             nrf_ecb_task_t task,
                                             uint8_t        channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        ECB task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_ecb_subscribe_clear(NRF_ECB_Type * p_reg,
                                               nrf_ecb_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        ECB task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return ECB subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_ecb_subscribe_get(NRF_ECB_Type const * p_reg,
                                                 nrf_ecb_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        ECB event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_ecb_publish_set(NRF_ECB_Type *  p_reg,
                                           nrf_ecb_event_t event,
                                           uint8_t         channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        ECB event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_ecb_publish_clear(NRF_ECB_Type *  p_reg,
                                             nrf_ecb_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        ECB event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return ECB publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_ecb_publish_get(NRF_ECB_Type const * p_reg,
                                               nrf_ecb_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_ecb_task_trigger(NRF_ECB_Type * p_reg, nrf_ecb_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_ecb_task_address_get(NRF_ECB_Type const * p_reg,
                                                    nrf_ecb_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE void nrf_ecb_event_clear(NRF_ECB_Type * p_reg, nrf_ecb_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_ecb_event_check(NRF_ECB_Type const * p_reg, nrf_ecb_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_ecb_event_address_get(NRF_ECB_Type const * p_reg,
                                                     nrf_ecb_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_ecb_int_enable(NRF_ECB_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_ecb_int_disable(NRF_ECB_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_ecb_int_enable_check(NRF_ECB_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

#if NRF_ECB_HAS_ECBDATAPTR
NRF_STATIC_INLINE void nrf_ecb_data_pointer_set(NRF_ECB_Type * p_reg, void const * p_buffer)
{
    p_reg->ECBDATAPTR = (uint32_t)p_buffer;
}

NRF_STATIC_INLINE void * nrf_ecb_data_pointer_get(NRF_ECB_Type const * p_reg)
{
    return (void *)(p_reg->ECBDATAPTR);
}
#endif // NRF_ECB_HAS_ECBDATAPTR

#if NRF_ECB_HAS_KEY
NRF_STATIC_INLINE void nrf_ecb_key_set(NRF_ECB_Type   * p_reg,
                                       uint32_t const * p_key)
{
    for (uint8_t i = 0; i < ECB_KEY_VALUE_MaxCount; i++)
    {
        p_reg->KEY.VALUE[i] = p_key[i];
    }
}
#endif // NRF_ECB_HAS_KEY

#if NRF_ECB_HAS_IN_PTR
NRF_STATIC_INLINE void nrf_ecb_in_ptr_set(NRF_ECB_Type *         p_reg,
                                          nrf_vdma_job_t const * p_job)
{
    p_reg->IN.PTR = (uint32_t)p_job;
}

NRF_STATIC_INLINE nrf_vdma_job_t * nrf_ecb_in_ptr_get(NRF_ECB_Type const * p_reg)
{
    return (nrf_vdma_job_t *)(p_reg->IN.PTR);
}
#endif // NRF_ECB_HAS_IN_PTR

#if NRF_ECB_HAS_IN_AMOUNT
NRF_STATIC_INLINE uint32_t nrf_ecb_in_amount_get(NRF_ECB_Type const * p_reg)
{
    return p_reg->IN.AMOUNT;
}
#endif // NRF_ECB_HAS_IN_AMOUNT

#if NRF_ECB_HAS_OUT_PTR
NRF_STATIC_INLINE void nrf_ecb_out_ptr_set(NRF_ECB_Type *         p_reg,
                                           nrf_vdma_job_t const * p_job)
{
    p_reg->OUT.PTR = (uint32_t)p_job;
}

NRF_STATIC_INLINE nrf_vdma_job_t * nrf_ecb_out_ptr_get(NRF_ECB_Type const * p_reg)
{
    return (nrf_vdma_job_t *)(p_reg->OUT.PTR);
}
#endif // NRF_ECB_HAS_OUT_PTR

#if NRF_ECB_HAS_OUT_AMOUNT
NRF_STATIC_INLINE uint32_t nrf_ecb_out_amount_get(NRF_ECB_Type const * p_reg)
{
    return p_reg->OUT.AMOUNT;
}
#endif // NRF_ECB_HAS_OUT_AMOUNT

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_ecb_subscribe_set(NRF_ECB_Type * p_reg,
                                             nrf_ecb_task_t task,
                                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_ecb_subscribe_clear(NRF_ECB_Type * p_reg,
                                               nrf_ecb_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_ecb_subscribe_get(NRF_ECB_Type const * p_reg,
                                                 nrf_ecb_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_ecb_publish_set(NRF_ECB_Type *  p_reg,
                                           nrf_ecb_event_t event,
                                           uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_ecb_publish_clear(NRF_ECB_Type *  p_reg,
                                             nrf_ecb_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_ecb_publish_get(NRF_ECB_Type const * p_reg,
                                               nrf_ecb_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // NRF_ECB_H__
