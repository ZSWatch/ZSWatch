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

#ifndef NRFY_UARTE_H__
#define NRFY_UARTE_H__

#include <nrfx.h>
#include <hal/nrf_uarte.h>

#ifdef __cplusplus
extern "C" {
#endif

#if NRF_UARTE_HAS_FRAME_TIMEOUT || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_UARTE_HAS_FRAME_TIMEOUT} */
#define NRFY_UARTE_HAS_FRAME_TIMEOUT 1
#else
#define NRFY_UARTE_HAS_FRAME_TIMEOUT 0
#endif

#if NRF_UARTE_HAS_FRAME_SIZE || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_UARTE_HAS_FRAME_SIZE} */
#define NRFY_UARTE_HAS_FRAME_SIZE 1
#else
#define NRFY_UARTE_HAS_FRAME_SIZE 0
#endif

typedef struct nrfy_uarte_buffer_t nrfy_uarte_buffer_t;

NRFY_STATIC_INLINE void __nrfy_internal_uarte_event_enabled_clear(NRF_UARTE_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_uarte_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_uarte_event_handle(NRF_UARTE_Type *  p_reg,
                                                           uint32_t          mask,
                                                           nrf_uarte_event_t event,
                                                           uint32_t *        p_evt_mask);

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_uarte_events_process(NRF_UARTE_Type *            p_reg,
                                              uint32_t                    mask,
                                              nrfy_uarte_buffer_t const * p_xfer);

/**
 * @defgroup nrfy_uarte UARTE HALY
 * @{
 * @ingroup nrf_uarte
 * @brief   Hardware access layer with cache and barrier support for managing the UARTE peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @brief UARTE pins configuration structure. */
typedef struct
{
    uint32_t txd_pin; ///< TXD pin number.
    uint32_t rxd_pin; ///< RXD pin number.
    uint32_t rts_pin; ///< RTS pin number.
    uint32_t cts_pin; ///< CTS pin number.
} nrfy_uarte_pins_t;

/** @brief UARTE configuration structure. */
typedef struct
{
    nrfy_uarte_pins_t    pins;          ///< Pin configuration structure.
    nrf_uarte_baudrate_t baudrate;      ///< Baud rate.
    nrf_uarte_config_t   config;        ///< Peripheral configuration.
    bool                 skip_psel_cfg; ///< Skip pin selection configuration.
                                        /**< When set to true, the driver does not modify
                                         *   pin select registers in the peripheral.
                                         *   Those registers are supposed to be set up
                                         *   externally before the driver is initialized.
                                         *   @note When both GPIO configuration and pin
                                         *   selection are to be skipped, the structure
                                         *   fields that specify pins can be omitted,
                                         *   as they are ignored anyway. */
} nrfy_uarte_config_t;

/** @brief Structure describing an UARTE transfer. */
struct nrfy_uarte_buffer_t
{
    uint8_t * p_buffer; ///< Buffer address.
    size_t    length;   ///< Data length.
};

/**
 * @brief Function for configuring the UARTE.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_uarte_periph_configure(NRF_UARTE_Type *            p_reg,
                                                    nrfy_uarte_config_t const * p_config)
{
    nrf_uarte_baudrate_set(p_reg, p_config->baudrate);
    nrf_uarte_configure(p_reg, &p_config->config);
    if (!p_config->skip_psel_cfg)
    {
        nrf_uarte_txrx_pins_set(p_reg, p_config->pins.txd_pin, p_config->pins.rxd_pin);
    }

    if (p_config->config.hwfc == NRF_UARTE_HWFC_ENABLED && !p_config->skip_psel_cfg)
    {
         nrf_uarte_hwfc_pins_set(p_reg, p_config->pins.rts_pin, p_config->pins.cts_pin);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified UARTE interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts associated with the event mask are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_uarte_int_init(NRF_UARTE_Type * p_reg,
                                            uint32_t         mask,
                                            uint8_t          irq_priority,
                                            bool             enable)
{
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_CTS);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_NCTS);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_RXDRDY);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_ENDRX);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_TXDRDY);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_ENDTX);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_ERROR);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_RXTO);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_RXSTARTED);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_TXSTARTED);
    __nrfy_internal_uarte_event_enabled_clear(p_reg, mask, NRF_UARTE_EVENT_TXSTOPPED);
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_uarte_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the UARTE interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_uarte_int_uninit(NRF_UARTE_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified UARTE events.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mask   Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 * @param[in] p_xfer Pointer to the structure containing buffer associated with the last reception.
 *                   Can be NULL.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_events_process(NRF_UARTE_Type *            p_reg,
                                                      uint32_t                    mask,
                                                      nrfy_uarte_buffer_t const * p_xfer)
{
    uint32_t evt_mask = __nrfy_internal_uarte_events_process(p_reg, mask, p_xfer);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for aborting the ongoing UARTE transmission.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] wait  True if CPU should wait for peripheral to abort transmission, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_uarte_tx_abort(NRF_UARTE_Type * p_reg,
                                            bool             wait)
{
    nrf_uarte_event_clear(p_reg, NRF_UARTE_EVENT_TXSTOPPED);
    nrf_barrier_w();
    nrf_uarte_task_trigger(p_reg, NRF_UARTE_TASK_STOPTX);
    if (wait)
    {
        nrf_barrier_w();
        while (!nrf_uarte_event_check(p_reg, NRF_UARTE_EVENT_TXSTOPPED))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for stopping the UARTE transmitter and receiver.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing reception buffer. Can be NULL.
 */
NRFY_STATIC_INLINE void nrfy_uarte_stop(NRF_UARTE_Type *            p_reg,
                                        nrfy_uarte_buffer_t const * p_xfer)
{
    nrf_uarte_task_trigger(p_reg, NRF_UARTE_TASK_STOPTX);
    nrf_barrier_w();
    while (!__nrfy_internal_uarte_events_process(p_reg,
                                            NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_TXSTOPPED),
                                            NULL))
    {}
    if (p_xfer && p_xfer->length)
    {
        nrf_uarte_task_trigger(p_reg, NRF_UARTE_TASK_STOPRX);
        nrf_barrier_w();
        while (!__nrfy_internal_uarte_events_process(p_reg,
                                            NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_RXTO),
                                            p_xfer))
        {}
    }
}

/**
 * @brief Function for starting the UARTE transmission.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] wait  True for blocking transmission, false otherwise.
 *
 * @return Mask of events occured, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *         Always 0 for non-blocking transmission.
 */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_tx_start(NRF_UARTE_Type * p_reg,
                                                bool             wait)
{
    uint32_t evt_mask = 0;
    nrf_uarte_task_trigger(p_reg, NRF_UARTE_TASK_STARTTX);
    if (wait)
    {
        uint32_t mask = NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_ENDTX) |
                        NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_TXSTOPPED);
        nrf_barrier_w();
        do {
            evt_mask = nrfy_uarte_events_process(p_reg, mask, NULL);
        } while (!evt_mask);
    }
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for starting the UARTE reception.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing reception buffer if the
 *                   reception is to be blocking. NULL for non-blocking reception.
 *
 * @return Mask of events occured, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *         Always 0 for non-blocking reception.
 */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_rx_start(NRF_UARTE_Type *            p_reg,
                                                nrfy_uarte_buffer_t const * p_xfer)
{
    uint32_t evt_mask = 0;
    nrf_uarte_task_trigger(p_reg, NRF_UARTE_TASK_STARTRX);
    if (p_xfer)
    {
        uint32_t mask = NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_ENDRX) |
                        NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_RXTO)  |
                        NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_ERROR);
        nrf_barrier_w();
        do {
            evt_mask = nrfy_uarte_events_process(p_reg, mask, p_xfer);
        } while (!evt_mask);
    }
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for getting UARTE pins configuration.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_pins Pointer to the UARTE pin configurartion structure.
 */
NRFY_STATIC_INLINE void nrfy_uarte_pins_get(NRF_UARTE_Type const * p_reg,
                                            nrfy_uarte_pins_t *    p_pins)
{
    nrf_barrier_rw();
    p_pins->txd_pin = nrf_uarte_tx_pin_get(p_reg);
    p_pins->rxd_pin = nrf_uarte_rx_pin_get(p_reg);
    p_pins->rts_pin = nrf_uarte_rts_pin_get(p_reg);
    p_pins->cts_pin = nrf_uarte_cts_pin_get(p_reg);
    nrf_barrier_r();
}

/**
 * @brief Function for disconnecting UARTE pins.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_uarte_pins_disconnect(NRF_UARTE_Type * p_reg)
{
    nrf_uarte_txrx_pins_disconnect(p_reg);
    nrf_uarte_hwfc_pins_disconnect(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_event_clear} */
NRFY_STATIC_INLINE void nrfy_uarte_event_clear(NRF_UARTE_Type * p_reg, nrf_uarte_event_t event)
{
    nrf_uarte_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_event_check} */
NRFY_STATIC_INLINE bool nrfy_uarte_event_check(NRF_UARTE_Type const * p_reg,
                                               nrf_uarte_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_uarte_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_uarte_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_event_address_get(NRF_UARTE_Type const * p_reg,
                                                         nrf_uarte_event_t      event)
{
    return nrf_uarte_event_address_get(p_reg, event);
}

/** @refhal{nrf_uarte_shorts_set} */
NRFY_STATIC_INLINE void nrfy_uarte_shorts_set(NRF_UARTE_Type * p_reg, uint32_t mask)
{
    nrf_uarte_shorts_set(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_shorts_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_shorts_get(NRF_UARTE_Type * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t ret = nrf_uarte_shorts_get(p_reg, mask);
    nrf_barrier_r();

    return ret;
}

/** @refhal{nrf_uarte_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_uarte_shorts_enable(NRF_UARTE_Type * p_reg, uint32_t mask)
{
    nrf_uarte_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_uarte_shorts_disable(NRF_UARTE_Type * p_reg, uint32_t mask)
{
    nrf_uarte_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_int_enable} */
NRFY_STATIC_INLINE void nrfy_uarte_int_enable(NRF_UARTE_Type * p_reg, uint32_t mask)
{
    nrf_uarte_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_int_enable_check(NRF_UARTE_Type const * p_reg,
                                                        uint32_t               mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_uarte_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_uarte_int_disable} */
NRFY_STATIC_INLINE void nrfy_uarte_int_disable(NRF_UARTE_Type * p_reg, uint32_t mask)
{
    nrf_uarte_int_disable(p_reg, mask);
    nrf_barrier_w();
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_uarte_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_uarte_subscribe_set(NRF_UARTE_Type * p_reg,
                                                 nrf_uarte_task_t task,
                                                 uint8_t          channel)
{
    nrf_uarte_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_uarte_subscribe_clear(NRF_UARTE_Type * p_reg, nrf_uarte_task_t task)
{
    nrf_uarte_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_publish_set} */
NRFY_STATIC_INLINE void nrfy_uarte_publish_set(NRF_UARTE_Type *  p_reg,
                                               nrf_uarte_event_t event,
                                               uint8_t           channel)
{
    nrf_uarte_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_publish_clear} */
NRFY_STATIC_INLINE void nrfy_uarte_publish_clear(NRF_UARTE_Type *  p_reg, nrf_uarte_event_t event)
{
    nrf_uarte_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/** @refhal{nrf_uarte_errorsrc_get_and_clear} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_errorsrc_get_and_clear(NRF_UARTE_Type * p_reg)
{
    nrf_barrier_rw();
    uint32_t errorsrc = nrf_uarte_errorsrc_get(p_reg);
    nrf_barrier_rw();
    nrf_uarte_errorsrc_clear(p_reg, errorsrc);
    nrf_barrier_rw();
    return errorsrc;
}

/** @refhal{nrf_uarte_enable} */
NRFY_STATIC_INLINE void nrfy_uarte_enable(NRF_UARTE_Type * p_reg)
{
    nrf_uarte_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_disable} */
NRFY_STATIC_INLINE void nrfy_uarte_disable(NRF_UARTE_Type * p_reg)
{
    nrf_uarte_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_enable_check} */
NRFY_STATIC_INLINE bool nrfy_uarte_enable_check(NRF_UARTE_Type * p_reg)
{
    nrf_barrier_rw();
    bool ret = nrf_uarte_enable_check(p_reg);
    nrf_barrier_r();
    return ret;
}

/** @refhal{nrf_uarte_txrx_pins_set} */
NRFY_STATIC_INLINE void nrfy_uarte_txrx_pins_set(NRF_UARTE_Type * p_reg,
                                                 uint32_t         pseltxd,
                                                 uint32_t         pselrxd)
{
    nrf_uarte_txrx_pins_set(p_reg, pseltxd, pselrxd);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_txrx_pins_disconnect} */
NRFY_STATIC_INLINE void nrfy_uarte_txrx_pins_disconnect(NRF_UARTE_Type * p_reg)
{
    nrf_uarte_txrx_pins_disconnect(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_tx_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_tx_pin_get(NRF_UARTE_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_uarte_tx_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_uarte_rx_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_rx_pin_get(NRF_UARTE_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_uarte_rx_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_uarte_rts_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_rts_pin_get(NRF_UARTE_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_uarte_rts_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_uarte_cts_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_cts_pin_get(NRF_UARTE_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_uarte_cts_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_uarte_hwfc_pins_set} */
NRFY_STATIC_INLINE void nrfy_uarte_hwfc_pins_set(NRF_UARTE_Type * p_reg,
                                                 uint32_t         pselrts,
                                                 uint32_t         pselcts)
{
    nrf_uarte_hwfc_pins_set(p_reg, pselrts, pselcts);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_hwfc_pins_disconnect} */
NRFY_STATIC_INLINE void nrfy_uarte_hwfc_pins_disconnect(NRF_UARTE_Type * p_reg)
{
    nrf_uarte_hwfc_pins_disconnect(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_task_trigger} */
NRFY_STATIC_INLINE void nrfy_uarte_task_trigger(NRF_UARTE_Type * p_reg, nrf_uarte_task_t task)
{
    nrf_uarte_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_task_address_get(NRF_UARTE_Type const * p_reg,
                                                        nrf_uarte_task_t       task)
{
    return nrf_uarte_task_address_get(p_reg, task);
}

/** @refhal{nrf_uarte_configure} */
NRFY_STATIC_INLINE void nrfy_uarte_configure(NRF_UARTE_Type           * p_reg,
                                             nrf_uarte_config_t const * p_cfg)
{
    nrf_uarte_configure(p_reg, p_cfg);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_baudrate_set} */
NRFY_STATIC_INLINE void nrfy_uarte_baudrate_set(NRF_UARTE_Type *     p_reg,
                                                nrf_uarte_baudrate_t baudrate)
{
    nrf_uarte_baudrate_set(p_reg, baudrate);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_tx_buffer_set} */
NRFY_STATIC_INLINE void nrfy_uarte_tx_buffer_set(NRF_UARTE_Type * p_reg,
                                                 uint8_t  const * p_buffer,
                                                 size_t           length)
{
    if (p_buffer)
    {
        NRFY_CACHE_WB(p_buffer, length);
        nrf_uarte_tx_buffer_set(p_reg, p_buffer, length);
        nrf_barrier_w();
    }
}

/** @refhal{nrf_uarte_tx_buffer_get} */
NRFY_STATIC_INLINE uint8_t const * nrfy_uarte_tx_buffer_get(NRF_UARTE_Type * p_reg)
{
    nrf_barrier_r();
    uint8_t const * ptr = nrf_uarte_tx_buffer_get(p_reg);
    nrf_barrier_r();
    return ptr;
}

/** @refhal{nrf_uarte_tx_amount_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_tx_amount_get(NRF_UARTE_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t amount = nrf_uarte_tx_amount_get(p_reg);
    nrf_barrier_r();
    return amount;
}

/** @refhal{nrf_uarte_rx_buffer_set} */
NRFY_STATIC_INLINE void nrfy_uarte_rx_buffer_set(NRF_UARTE_Type * p_reg,
                                                 uint8_t *        p_buffer,
                                                 size_t           length)
{
    nrf_uarte_rx_buffer_set(p_reg, p_buffer, length);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_rx_amount_get} */
NRFY_STATIC_INLINE uint32_t nrfy_uarte_rx_amount_get(NRF_UARTE_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t amount = nrf_uarte_rx_amount_get(p_reg);
    nrf_barrier_r();
    return amount;
}

#if NRFX_CHECK(NRFY_UARTE_HAS_FRAME_TIMEOUT)
/** @refhal{nrf_uarte_frame_timeout_set} */
NRFY_STATIC_INLINE void nrfy_uarte_frame_timeout_set(NRF_UARTE_Type * p_reg,
                                                     uint32_t         timeout)
{
    nrf_barrier_r();
    nrf_uarte_frame_timeout_set(p_reg, timeout);
    nrf_barrier_w();
}
#endif

#if NRFX_CHECK(NRFY_UARTE_HAS_FRAME_SIZE)
/** @refhal{nrf_uarte_address_set} */
NRFY_STATIC_INLINE void nrfy_uarte_address_set(NRF_UARTE_Type * p_reg,
                                               uint8_t          address)
{
    nrf_barrier_r();
    nrf_uarte_address_set(p_reg, address);
    nrf_barrier_w();
}

/** @refhal{nrf_uarte_address_get} */
NRFY_STATIC_INLINE uint8_t nrfy_uarte_address_get(NRF_UARTE_Type const * p_reg)
{
    nrf_barrier_r();
    uint8_t address = nrf_uarte_address_get(p_reg);
    nrf_barrier_r();
    return address;
}
#endif
/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_uarte_event_enabled_clear(NRF_UARTE_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_uarte_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_uarte_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE bool __nrfy_internal_uarte_event_handle(NRF_UARTE_Type *  p_reg,
                                                           uint32_t          mask,
                                                           nrf_uarte_event_t event,
                                                           uint32_t  *       p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_uarte_event_check(p_reg, event))
    {
        nrf_uarte_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_uarte_events_process(NRF_UARTE_Type *            p_reg,
                                              uint32_t                    mask,
                                              nrfy_uarte_buffer_t const * p_xfer)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_CTS,       &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_NCTS,      &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_RXDRDY,    &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_ENDRX,     &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_TXDRDY,    &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_ENDTX,     &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_ERROR,     &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_RXTO,      &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_RXSTARTED, &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_TXSTARTED, &evt_mask);
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_TXSTOPPED, &evt_mask);
#if NRFY_UARTE_HAS_FRAME_TIMEOUT
    (void)__nrfy_internal_uarte_event_handle(p_reg, mask, NRF_UARTE_EVENT_FRAME_TIMEOUT, &evt_mask);
#endif

    if (mask & NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_ENDRX))
    {
        NRFY_CACHE_INV(p_xfer->p_buffer, p_xfer->length);
    }
    else if (mask & NRFY_EVENT_TO_INT_BITMASK(NRF_UARTE_EVENT_RXTO))
    {
        size_t size = nrf_uarte_rx_amount_get(p_reg);
        nrf_barrier_rw();
        NRFY_CACHE_INV(p_xfer->p_buffer, size);
    }

    nrf_barrier_w();
    return evt_mask;
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_UARTE_H__
