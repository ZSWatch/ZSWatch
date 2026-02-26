/*
 * Copyright (c) 2021 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFY_TWIM_H__
#define NRFY_TWIM_H__

#include <nrfx.h>
#include <hal/nrf_twim.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nrfy_twim_xfer_desc_t nrfy_twim_xfer_desc_t;

NRFY_STATIC_INLINE void __nrfy_internal_twim_event_enabled_clear(NRF_TWIM_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_twim_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_twim_event_handle(NRF_TWIM_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_twim_event_t event,
                                                          uint32_t *       p_evt_mask);

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_twim_events_process(NRF_TWIM_Type *               p_reg,
                                             uint32_t                      mask,
                                             nrfy_twim_xfer_desc_t const * p_xfer);

/**
 * @defgroup nrfy_twim TWIM HALY
 * @{
 * @ingroup nrf_twim
 * @brief   Hardware access layer with cache and barrier support for managing the TWIM peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

#if NRF_TWIM_HAS_ARRAY_LIST || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_TWIM_HAS_ARRAY_LIST} */
#define NRFY_TWIM_HAS_ARRAY_LIST 1
#else
#define NRFY_TWIM_HAS_ARRAY_LIST 0
#endif

/** @brief TWIM pins configuration structure. */
typedef struct
{
    uint32_t scl_pin; ///< SCL pin number.
    uint32_t sda_pin; ///< SDA pin number.
} nrfy_twim_pins_t;

/** @brief TWIM configuration structure. */
typedef struct
{
    nrfy_twim_pins_t     pins;          ///< TWIM pins configuation.
    nrf_twim_frequency_t frequency;     ///< TWIM frequency.
    bool                 skip_psel_cfg; ///< Skip pin selection configuration.
                                        /**< When set to true, the driver does not modify
                                         *   pin select registers in the peripheral.
                                         *   Those registers are supposed to be set up
                                         *   externally before the driver is initialized.
                                         *   @note When both GPIO configuration and pin
                                         *   selection are to be skipped, the structure
                                         *   fields that specify pins can be omitted,
                                         *   as they are ignored anyway. */
} nrfy_twim_config_t;

/** @brief Structure describing a TWIM transfer. */
struct nrfy_twim_xfer_desc_t
{
    uint8_t * p_buffer; ///< Pointer to transferred data.
    size_t    length;   ///< Number of bytes transferred.
};

/**
 * @brief Function for configuring the TWIM.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_twim_periph_configure(NRF_TWIM_Type *            p_reg,
                                                   nrfy_twim_config_t const * p_config)
{
    if (!p_config->skip_psel_cfg)
    {
        nrf_twim_pins_set(p_reg, p_config->pins.scl_pin, p_config->pins.sda_pin);
    }
    nrf_twim_frequency_set(p_reg, p_config->frequency);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified TWIM interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts associated with the event mask are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_twim_int_init(NRF_TWIM_Type * p_reg,
                                           uint32_t        mask,
                                           uint8_t         irq_priority,
                                           bool            enable)
{
    __nrfy_internal_twim_event_enabled_clear(p_reg, mask, NRF_TWIM_EVENT_TXSTARTED);
    __nrfy_internal_twim_event_enabled_clear(p_reg, mask, NRF_TWIM_EVENT_RXSTARTED);
    __nrfy_internal_twim_event_enabled_clear(p_reg, mask, NRF_TWIM_EVENT_LASTTX);
    __nrfy_internal_twim_event_enabled_clear(p_reg, mask, NRF_TWIM_EVENT_LASTRX);
    __nrfy_internal_twim_event_enabled_clear(p_reg, mask, NRF_TWIM_EVENT_STOPPED);
    __nrfy_internal_twim_event_enabled_clear(p_reg, mask, NRF_TWIM_EVENT_SUSPENDED);
    __nrfy_internal_twim_event_enabled_clear(p_reg, mask, NRF_TWIM_EVENT_ERROR);
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_twim_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the TWIM interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_twim_int_uninit(NRF_TWIM_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified TWIM events.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mask   Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK();
 * @param[in] p_xfer Pointer to the structure containing buffer associated with the last reception.
 *                   Can be NULL.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_twim_events_process(NRF_TWIM_Type *               p_reg,
                                                     uint32_t                      mask,
                                                     nrfy_twim_xfer_desc_t const * p_xfer)
{
    uint32_t evt_mask = __nrfy_internal_twim_events_process(p_reg, mask, p_xfer);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for setting the TWIM transaction buffer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing transaction buffer.
 */
NRFY_STATIC_INLINE void nrfy_twim_tx_buffer_set(NRF_TWIM_Type *               p_reg,
                                                nrfy_twim_xfer_desc_t const * p_xfer)
{
    if (p_xfer->p_buffer)
    {
        NRFY_CACHE_WB(p_xfer->p_buffer, p_xfer->length);
    }
    nrf_twim_tx_buffer_set(p_reg, p_xfer->p_buffer, p_xfer->length);
    nrf_barrier_w();
}

/**
 * @brief Function for setting the TWIM reception buffer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing reception buffer.
 */
NRFY_STATIC_INLINE void nrfy_twim_rx_buffer_set(NRF_TWIM_Type *               p_reg,
                                                nrfy_twim_xfer_desc_t const * p_xfer)
{
    nrf_twim_rx_buffer_set(p_reg, p_xfer->p_buffer, p_xfer->length);
    nrf_barrier_w();
}

/**
 * @brief Function for starting TWIM transaction.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing transaction buffer.
 */
NRFY_STATIC_INLINE void nrfy_twim_tx_start(NRF_TWIM_Type *               p_reg,
                                           nrfy_twim_xfer_desc_t const * p_xfer)
{
    nrf_twim_task_trigger(p_reg, NRF_TWIM_TASK_STARTTX);
    if (p_xfer)
    {
        if (p_xfer->length == 0)
        {
            nrf_twim_task_trigger(p_reg, NRF_TWIM_TASK_STOP);
        }
        nrf_barrier_w();
        uint32_t mask = NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_SUSPENDED) |
                        NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_STOPPED) |
                        NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_ERROR);
        uint32_t evt_mask = 0;
        while (!(evt_mask & (NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_SUSPENDED) |
                             NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_STOPPED))))
        {
            evt_mask = __nrfy_internal_twim_events_process(p_reg, mask, p_xfer);
            if (evt_mask & NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_ERROR))
            {
                bool lasttx_triggered = __nrfy_internal_twim_events_process(p_reg,
                                                NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_LASTTX),
                                                NULL);
                nrf_barrier_rw();
                uint32_t shorts_mask = nrf_twim_shorts_get(p_reg);
                nrf_barrier_r();

                if (!(lasttx_triggered && (shorts_mask & NRF_TWIM_SHORT_LASTTX_STOP_MASK)))
                {
                    // Unless LASTTX event arrived and LASTTX_STOP shortcut is active,
                    // triggering of STOP task in case of error has to be done manually.
                    nrf_twim_task_trigger(p_reg, NRF_TWIM_TASK_RESUME);
                    nrf_twim_task_trigger(p_reg, NRF_TWIM_TASK_STOP);
                    nrf_barrier_w();

                    // Mark transmission as not finished yet,
                    // as STOPPED event is expected to arrive.
                    // If LASTTX_SUSPENDED shortcut is active,
                    // NACK has been received on last byte sent
                    // and SUSPENDED event happened to be checked before ERROR,
                    // transmission will be marked as finished.
                    // In such case this flag has to be overwritten.
                    evt_mask = 0;
                }

                if (lasttx_triggered && (shorts_mask & NRF_TWIM_SHORT_LASTTX_SUSPEND_MASK))
                {
                    // When STOP task was triggered just before SUSPEND task has taken effect,
                    // SUSPENDED event may not arrive.
                    // However if SUSPENDED arrives it always arrives after ERROR.
                    // Therefore SUSPENDED has to be cleared
                    // so it does not cause premature termination of busy loop
                    // waiting for STOPPED event to arrive.
                    (void)__nrfy_internal_twim_events_process(p_reg,
                                            NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_SUSPENDED),
                                            p_xfer);
                    // Mark transmission as not finished yet,
                    // for same reasons as above.
                    evt_mask = 0;
                }
            }
        }
    }
    nrf_barrier_w();
}

/**
 * @brief Function for starting TWIM reception.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing reception buffer.
 */
NRFY_STATIC_INLINE void nrfy_twim_rx_start(NRF_TWIM_Type *               p_reg,
                                           nrfy_twim_xfer_desc_t const * p_xfer)
{
    nrf_twim_task_trigger(p_reg, NRF_TWIM_TASK_STARTRX);
    if (p_xfer)
    {
        if (p_xfer->length == 0)
        {
            nrf_twim_task_trigger(p_reg, NRF_TWIM_TASK_STOP);
        }

        nrf_barrier_w();
        uint32_t mask = NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_SUSPENDED) |
                        NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_STOPPED) |
                        NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_ERROR);
        uint32_t evt_mask = 0;
        while (!(evt_mask & (NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_SUSPENDED) |
                             NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_STOPPED))))
        {
            evt_mask = __nrfy_internal_twim_events_process(p_reg, mask, p_xfer);
            if (evt_mask & NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_ERROR))
            {
                // triggering of STOP task in case of error has to be done manually.
                nrf_twim_task_trigger(p_reg, NRF_TWIM_TASK_STOP);
                nrf_barrier_w();
            }
        }
    }
    nrf_barrier_w();
}

/**
 * @brief Function for aborting the ongoing TWIM transaction.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing reception buffer
 *                   if the abort is to be blocking. NULL for non-blocking operation.
 */
NRFY_STATIC_INLINE void nrfy_twim_abort(NRF_TWIM_Type * p_reg, nrfy_twim_xfer_desc_t const * p_xfer)
{
    nrf_twim_task_trigger(p_reg, NRF_TWIM_TASK_STOP);
    if (p_xfer)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_STOPPED);
        while (!__nrfy_internal_twim_events_process(p_reg, evt_mask, p_xfer))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for getting TWIM pins configuration.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_pins Pointer to the structure to be filled with TWIM pins configuration.
 */
NRFY_STATIC_INLINE void nrfy_twim_pins_get(NRF_TWIM_Type const * p_reg,
                                           nrfy_twim_pins_t *    p_pins)
{
    nrf_barrier_rw();
    p_pins->scl_pin = nrf_twim_scl_pin_get(p_reg);
    p_pins->sda_pin = nrf_twim_sda_pin_get(p_reg);
    nrf_barrier_r();
}

/**
 * @brief Function for disabling TWIM with all interrupts and shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_twim_stop(NRF_TWIM_Type * p_reg)
{
    nrf_twim_int_disable(p_reg, NRF_TWIM_ALL_INTS_MASK);
    nrf_twim_shorts_disable(p_reg, NRF_TWIM_ALL_SHORTS_MASK);
    if (NRF_ERRATA_DYNAMIC_CHECK(52, 89))
    {
        *(volatile uint32_t *)((uint8_t *)p_reg + 0x500UL) = 9;
    }
    nrf_twim_disable(p_reg);
    nrf_barrier_w();
}

#if NRFY_TWIM_HAS_ARRAY_LIST
/**
 * @brief Function for enabling or disabling the TX list feature.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if TX list feature is to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_twim_tx_list_set(NRF_TWIM_Type * p_reg, bool enable)
{
    if (enable)
    {
        nrf_twim_tx_list_enable(p_reg);
    }
    else
    {
        nrf_twim_tx_list_disable(p_reg);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for enabling or disabling the RX list feature.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if RX list feature is to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_twim_rx_list_set(NRF_TWIM_Type * p_reg, bool enable)
{
    if (enable)
    {
        nrf_twim_rx_list_enable(p_reg);
    }
    else
    {
        nrf_twim_rx_list_disable(p_reg);
    }
    nrf_barrier_w();
}
#endif

/**
 * @brief Function for setting the TWIM pins configuration.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_pins Pointer to the TWIM pin configurartion structure.
 */
NRFY_STATIC_INLINE void nrfy_twim_pins_set(NRF_TWIM_Type *          p_reg,
                                           nrfy_twim_pins_t const * p_pins)
{
    nrf_twim_pins_set(p_reg, p_pins->scl_pin, p_pins->sda_pin);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_task_trigger} */
NRFY_STATIC_INLINE void nrfy_twim_task_trigger(NRF_TWIM_Type * p_reg,
                                               nrf_twim_task_t task)
{
    nrf_twim_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_twim_task_address_get(NRF_TWIM_Type const * p_reg,
                                                       nrf_twim_task_t       task)
{
    return nrf_twim_task_address_get(p_reg, task);
}

/** @refhal{nrf_twim_event_clear} */
NRFY_STATIC_INLINE void nrfy_twim_event_clear(NRF_TWIM_Type *  p_reg,
                                              nrf_twim_event_t event)
{
    nrf_twim_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_event_check} */
NRFY_STATIC_INLINE bool nrfy_twim_event_check(NRF_TWIM_Type const * p_reg,
                                              nrf_twim_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_twim_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_twim_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_twim_event_address_get(NRF_TWIM_Type const * p_reg,
                                                        nrf_twim_event_t      event)
{
    return nrf_twim_event_address_get(p_reg, event);
}

/** @refhal{nrf_twim_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_twim_shorts_enable(NRF_TWIM_Type * p_reg,
                                                uint32_t        mask)
{
    nrf_twim_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_twim_shorts_disable(NRF_TWIM_Type * p_reg,
                                                 uint32_t        mask)
{
    nrf_twim_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_int_enable} */
NRFY_STATIC_INLINE void nrfy_twim_int_enable(NRF_TWIM_Type * p_reg,
                                             uint32_t        mask)
{
    nrf_twim_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_int_disable} */
NRFY_STATIC_INLINE void nrfy_twim_int_disable(NRF_TWIM_Type * p_reg,
                                              uint32_t        mask)
{
    nrf_twim_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_twim_int_enable_check(NRF_TWIM_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_twim_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_twim_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_twim_subscribe_set(NRF_TWIM_Type * p_reg,
                                                nrf_twim_task_t task,
                                                uint8_t         channel)
{
    nrf_twim_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_twim_subscribe_clear(NRF_TWIM_Type * p_reg,
                                                  nrf_twim_task_t task)
{
    nrf_twim_subscribe_clear(p_reg, task);
    nrf_barrier_w();

}

/** @refhal{nrf_twim_publish_set} */
NRFY_STATIC_INLINE void nrfy_twim_publish_set(NRF_TWIM_Type *  p_reg,
                                              nrf_twim_event_t event,
                                              uint8_t          channel)
{
    nrf_twim_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_publish_clear} */
NRFY_STATIC_INLINE void nrfy_twim_publish_clear(NRF_TWIM_Type *  p_reg,
                                                nrf_twim_event_t event)
{
    nrf_twim_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/** @refhal{nrf_twim_enable} */
NRFY_STATIC_INLINE void nrfy_twim_enable(NRF_TWIM_Type * p_reg)
{
    nrf_twim_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_disable} */
NRFY_STATIC_INLINE void nrfy_twim_disable(NRF_TWIM_Type * p_reg)
{
    nrf_twim_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_scl_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_twim_scl_pin_get(NRF_TWIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t scl_pin = nrf_twim_scl_pin_get(p_reg);
    nrf_barrier_r();
    return scl_pin;
}

/** @refhal{nrf_twim_sda_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_twim_sda_pin_get(NRF_TWIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t sda_pin = nrf_twim_sda_pin_get(p_reg);
    nrf_barrier_r();
    return sda_pin;
}

/** @refhal{nrf_twim_frequency_set} */
NRFY_STATIC_INLINE void nrfy_twim_frequency_set(NRF_TWIM_Type *      p_reg,
                                                nrf_twim_frequency_t frequency)
{
    nrf_twim_frequency_set(p_reg, frequency);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_errorsrc_get_and_clear} */
NRFY_STATIC_INLINE uint32_t nrfy_twim_errorsrc_get_and_clear(NRF_TWIM_Type * p_reg)
{
    nrf_barrier_rw();
    uint32_t errorsrc = nrf_twim_errorsrc_get_and_clear(p_reg);
    nrf_barrier_rw();
    return errorsrc;
}

/** @refhal{nrf_twim_address_set} */
NRFY_STATIC_INLINE void nrfy_twim_address_set(NRF_TWIM_Type * p_reg,
                                              uint8_t         address)
{
    nrf_twim_address_set(p_reg, address);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_shorts_set} */
NRFY_STATIC_INLINE void nrfy_twim_shorts_set(NRF_TWIM_Type * p_reg,
                                             uint32_t        mask)
{
    nrf_twim_shorts_set(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_shorts_get} */
NRFY_STATIC_INLINE uint32_t nrfy_twim_shorts_get(NRF_TWIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t shorts = nrf_twim_shorts_get(p_reg);
    nrf_barrier_r();
    return shorts;
}

/** @refhal{nrf_twim_txd_amount_get} */
NRFY_STATIC_INLINE size_t nrfy_twim_txd_amount_get(NRF_TWIM_Type const * p_reg)
{
    nrf_barrier_r();
    size_t amount = nrf_twim_txd_amount_get(p_reg);
    nrf_barrier_r();
    return amount;
}

/** @refhal{nrf_twim_rxd_amount_get} */
NRFY_STATIC_INLINE size_t nrfy_twim_rxd_amount_get(NRF_TWIM_Type const * p_reg)
{
    nrf_barrier_r();
    size_t amount = nrf_twim_rxd_amount_get(p_reg);
    nrf_barrier_r();
    return amount;
}

#if NRFY_TWIM_HAS_ARRAY_LIST
/** @refhal{nrf_twim_tx_list_enable} */
NRFY_STATIC_INLINE void nrfy_twim_tx_list_enable(NRF_TWIM_Type * p_reg)
{
    nrf_twim_tx_list_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_tx_list_disable} */
NRFY_STATIC_INLINE void nrfy_twim_tx_list_disable(NRF_TWIM_Type * p_reg)
{
    nrf_twim_tx_list_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_rx_list_enable} */
NRFY_STATIC_INLINE void nrfy_twim_rx_list_enable(NRF_TWIM_Type * p_reg)
{
    nrf_twim_rx_list_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_twim_rx_list_disable} */
NRFY_STATIC_INLINE void nrfy_twim_rx_list_disable(NRF_TWIM_Type * p_reg)
{
    nrf_twim_rx_list_disable(p_reg);
    nrf_barrier_w();
}
#endif

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_twim_event_enabled_clear(NRF_TWIM_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_twim_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_twim_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE bool __nrfy_internal_twim_event_handle(NRF_TWIM_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_twim_event_t event,
                                                          uint32_t *       p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_twim_event_check(p_reg, event))
    {
        nrf_twim_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_twim_events_process(NRF_TWIM_Type *               p_reg,
                                             uint32_t                      mask,
                                             nrfy_twim_xfer_desc_t const * p_xfer)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    (void)__nrfy_internal_twim_event_handle(p_reg, mask, NRF_TWIM_EVENT_SUSPENDED, &evt_mask);
    (void)__nrfy_internal_twim_event_handle(p_reg, mask, NRF_TWIM_EVENT_STOPPED, &evt_mask);
    (void)__nrfy_internal_twim_event_handle(p_reg, mask, NRF_TWIM_EVENT_ERROR, &evt_mask);
    (void)__nrfy_internal_twim_event_handle(p_reg, mask, NRF_TWIM_EVENT_TXSTARTED, &evt_mask);
    (void)__nrfy_internal_twim_event_handle(p_reg, mask, NRF_TWIM_EVENT_RXSTARTED, &evt_mask);
    (void)__nrfy_internal_twim_event_handle(p_reg, mask, NRF_TWIM_EVENT_LASTTX, &evt_mask);
    (void)__nrfy_internal_twim_event_handle(p_reg, mask, NRF_TWIM_EVENT_LASTRX, &evt_mask);

    if (p_xfer && (mask & NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_STOPPED)))
    {
        NRFY_CACHE_INV(p_xfer->p_buffer, p_xfer->length);
    }
    else if (p_xfer && (mask & NRFY_EVENT_TO_INT_BITMASK(NRF_TWIM_EVENT_LASTRX)))
    {
        NRFY_CACHE_INV(p_xfer->p_buffer, p_xfer->length);
    }
    nrf_barrier_w();
    return evt_mask;
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_TWIM_H__
