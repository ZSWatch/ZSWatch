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

#ifndef NRFY_I2S_H__
#define NRFY_I2S_H__

#include <nrfx.h>
#include <hal/nrf_i2s.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nrfy_i2s_xfer_desc_t nrfy_i2s_xfer_desc_t;

NRFY_STATIC_INLINE bool __nrfy_internal_i2s_event_handle(NRF_I2S_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_i2s_event_t event,
                                                         uint32_t *      p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_i2s_events_process(NRF_I2S_Type *               p_reg,
                                                               uint32_t                     mask,
                                                               nrfy_i2s_xfer_desc_t const * p_xfer);

NRFY_STATIC_INLINE void __nrfy_internal_i2s_event_enabled_clear(NRF_I2S_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_i2s_event_t event);


/**
 * @defgroup nrfy_i2s I2S HALY
 * @{
 * @ingroup nrf_i2s
 * @brief   Hardware access layer with cache and barrier support for managing the I2S peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @brief Structure for I2S configuration. */
typedef struct
{
    nrf_i2s_config_t config;        ///< Peripheral configuration.
    nrf_i2s_pins_t   pins;          ///< Pins to be used.
#if NRF_I2S_HAS_CLKCONFIG
    nrf_i2s_clksrc_t clksrc;        ///< Clock source selection.
    bool             enable_bypass; ///< Bypass clock generator. MCK will be equal to source input.
#endif
    bool             skip_psel_cfg; ///< Skip pin selection configuration.
                                    /**< When set to true, the driver does not modify
                                     *   pin select registers in the peripheral.
                                     *   Those registers are supposed to be set up
                                     *   externally before the driver is initialized.
                                     *   @note When both GPIO configuration and pin
                                     *   selection are to be skipped, the structure
                                     *   fields that specify pins can be omitted,
                                     *   as they are ignored anyway. */
} nrfy_i2s_config_t;

/** @brief Structure describing single I2S transfer. */
struct nrfy_i2s_xfer_desc_t
{
    uint32_t       * p_rx_buffer; ///< Pointer to the buffer for received data.
    uint32_t const * p_tx_buffer; ///< Pointer to the buffer with data to be sent.
    uint16_t         buffer_size; ///< Size of buffers.
};

/**
 * @brief Function for configuring the I2S.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_i2s_periph_configure(NRF_I2S_Type *            p_reg,
                                                  nrfy_i2s_config_t const * p_config)
{
    nrf_i2s_configure(p_reg, &p_config->config);

#if NRF_I2S_HAS_CLKCONFIG
    nrf_i2s_clk_configure(p_reg, p_config->clksrc, p_config->enable_bypass);
#endif
    if (!p_config->skip_psel_cfg)
    {
        nrf_i2s_pins_set(p_reg, &p_config->pins);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified I2S interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_i2s_int_init(NRF_I2S_Type * p_reg,
                                          uint32_t       mask,
                                          uint8_t        irq_priority,
                                          bool           enable)
{
    __nrfy_internal_i2s_event_enabled_clear(p_reg, mask, NRF_I2S_EVENT_RXPTRUPD);
    __nrfy_internal_i2s_event_enabled_clear(p_reg, mask, NRF_I2S_EVENT_TXPTRUPD);
    __nrfy_internal_i2s_event_enabled_clear(p_reg, mask, NRF_I2S_EVENT_STOPPED);
#if NRF_I2S_HAS_FRAMESTART
    __nrfy_internal_i2s_event_enabled_clear(p_reg, mask, NRF_I2S_EVENT_FRAMESTART);
#endif
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));

    if (enable)
    {
        nrf_i2s_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the I2S interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_i2s_int_uninit(NRF_I2S_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified I2S events.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mask   Mask of events to be processed,
 *                   created by @ref NRFY_EVENT_TO_INT_BITMASK().
 * @param[in] p_xfer Pointer to the structure containing buffers associated with the last transaction.
 *                   Can be NULL.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_events_process(NRF_I2S_Type *         p_reg,
                                                    uint32_t               mask,
                                                    nrfy_i2s_xfer_desc_t * p_xfer)
{
    uint32_t evt_mask = __nrfy_internal_i2s_events_process(p_reg, mask, p_xfer);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for setting the I2S transaction buffers.
 *
 * If the transfer in a given direction is not required, pass NULL instead of the pointer
 * to the corresponding buffer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing transaction buffers.
 */
NRFY_STATIC_INLINE void nrfy_i2s_buffers_set(NRF_I2S_Type *               p_reg,
                                             nrfy_i2s_xfer_desc_t const * p_xfer)
{
    if (p_xfer->p_tx_buffer != NULL)
    {
        NRFY_CACHE_WB(p_xfer->p_tx_buffer, (p_xfer->buffer_size * sizeof(uint32_t)));
    }

    nrf_i2s_transfer_set(p_reg,
                         p_xfer->buffer_size,
                         p_xfer->p_rx_buffer,
                         p_xfer->p_tx_buffer);

    nrf_barrier_w();
}

/**
 * @brief Function for starting the I2S transaction.
 *
 * If the transfer in a given direction is not required, pass NULL instead of the pointer
 * to the corresponding buffer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing transaction buffers
 *                   if the transaction is to be blocking. NULL for non-blocking transactions.
 */
NRFY_STATIC_INLINE void nrfy_i2s_xfer_start(NRF_I2S_Type *               p_reg,
                                            nrfy_i2s_xfer_desc_t const * p_xfer)
{

    nrf_i2s_task_trigger(p_reg, NRF_I2S_TASK_START);

    if (p_xfer)
    {
        nrf_barrier_w();

        bool tx_done = false, rx_done = false;

        while (!((tx_done || (p_xfer->p_tx_buffer == NULL)) &&
                 (rx_done || (p_xfer->p_rx_buffer == NULL))))
        {
            if (__nrfy_internal_i2s_events_process(p_reg,
                    NRFY_EVENT_TO_INT_BITMASK(NRF_I2S_EVENT_TXPTRUPD),
                    p_xfer))
            {
                tx_done = true;
            }
            if (__nrfy_internal_i2s_events_process(p_reg,
                    NRFY_EVENT_TO_INT_BITMASK(NRF_I2S_EVENT_RXPTRUPD),
                    p_xfer))
            {
                rx_done = true;
            }
        }
    }

    nrf_barrier_w();
}

/**
 * @brief Function for aborting the ongoing I2S transaction.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing transaction buffers
 *                   if the abort is to be blocking. NULL for non-blocking operation.
 */
NRFY_STATIC_INLINE void nrfy_i2s_abort(NRF_I2S_Type * p_reg, nrfy_i2s_xfer_desc_t const * p_xfer)
{
    nrf_i2s_task_trigger(p_reg, NRF_I2S_TASK_STOP);
    if (p_xfer)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_I2S_EVENT_STOPPED);
        while (!__nrfy_internal_i2s_events_process(p_reg, evt_mask, p_xfer))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for getting the pins selection.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_pins Pointer to the I2S pin configuration structure.
 *
 */
NRFY_STATIC_INLINE void nrfy_i2s_pins_get(NRF_I2S_Type const * p_reg,
                                          nrf_i2s_pins_t *     p_pins)
{
    nrf_barrier_rw();

    p_pins->sck_pin   = nrf_i2s_sck_pin_get(p_reg),
    p_pins->lrck_pin  = nrf_i2s_lrck_pin_get(p_reg),
    p_pins->mck_pin   = nrf_i2s_mck_pin_get(p_reg),
    p_pins->sdout_pin = nrf_i2s_sdout_pin_get(p_reg),
    p_pins->sdin_pin  = nrf_i2s_sdin_pin_get(p_reg),

    nrf_barrier_r();
}

/** @refhal{nrf_i2s_task_trigger} */
NRFY_STATIC_INLINE void nrfy_i2s_task_trigger(NRF_I2S_Type * p_reg, nrf_i2s_task_t task)
{
    nrf_i2s_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_task_address_get(NRF_I2S_Type const * p_reg,
                                                      nrf_i2s_task_t       task)
{
    return nrf_i2s_task_address_get(p_reg, task);
}

/** @refhal{nrf_i2s_event_clear} */
NRFY_STATIC_INLINE void nrfy_i2s_event_clear(NRF_I2S_Type *  p_reg,
                                             nrf_i2s_event_t event)
{
    nrf_i2s_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_event_check} */
NRFY_STATIC_INLINE bool nrfy_i2s_event_check(NRF_I2S_Type const * p_reg,
                                             nrf_i2s_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_i2s_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_i2s_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_event_address_get(NRF_I2S_Type const * p_reg,
                                                       nrf_i2s_event_t      event)
{
    return nrf_i2s_event_address_get(p_reg, event);
}

/** @refhal{nrf_i2s_int_enable} */
NRFY_STATIC_INLINE void nrfy_i2s_int_enable(NRF_I2S_Type * p_reg, uint32_t mask)
{
    nrf_i2s_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_int_disable} */
NRFY_STATIC_INLINE void nrfy_i2s_int_disable(NRF_I2S_Type * p_reg, uint32_t mask)
{
    nrf_i2s_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_int_enable_check(NRF_I2S_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_i2s_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_i2s_enable} */
NRFY_STATIC_INLINE void nrfy_i2s_enable(NRF_I2S_Type * p_reg)
{
    nrf_i2s_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_disable} */
NRFY_STATIC_INLINE void nrfy_i2s_disable(NRF_I2S_Type * p_reg)
{
    nrf_i2s_disable(p_reg);
    nrf_barrier_w();
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_i2s_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_i2s_subscribe_set(NRF_I2S_Type * p_reg,
                                               nrf_i2s_task_t task,
                                               uint8_t        channel)
{
    nrf_i2s_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_i2s_subscribe_clear(NRF_I2S_Type * p_reg,
                                                 nrf_i2s_task_t task)
{
    nrf_i2s_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_publish_set} */
NRFY_STATIC_INLINE void nrfy_i2s_publish_set(NRF_I2S_Type *  p_reg,
                                             nrf_i2s_event_t event,
                                             uint8_t         channel)
{
    nrf_i2s_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_publish_clear} */
NRFY_STATIC_INLINE void nrfy_i2s_publish_clear(NRF_I2S_Type *  p_reg,
                                               nrf_i2s_event_t event)
{
    nrf_i2s_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_i2s_pins_set} */
NRFY_STATIC_INLINE void nrfy_i2s_pins_set(NRF_I2S_Type * p_reg, nrf_i2s_pins_t const * p_pins)
{
    nrf_i2s_pins_set(p_reg, p_pins);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_sck_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_sck_pin_get(NRF_I2S_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_i2s_sck_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_i2s_lrck_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_lrck_pin_get(NRF_I2S_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_i2s_lrck_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_i2s_mck_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_mck_pin_get(NRF_I2S_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_i2s_mck_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_i2s_sdout_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_sdout_pin_get(NRF_I2S_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_i2s_sdout_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_i2s_sdin_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_i2s_sdin_pin_get(NRF_I2S_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_i2s_sdin_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_i2s_configure} */
NRFY_STATIC_INLINE void nrfy_i2s_configure(NRF_I2S_Type * p_reg, nrf_i2s_config_t const * p_config)
{
    nrf_i2s_configure(p_reg, p_config);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_transfer_set} */
NRFY_STATIC_INLINE void nrfy_i2s_transfer_set(NRF_I2S_Type *   p_reg,
                                              uint16_t         size,
                                              uint32_t *       p_rx_buffer,
                                              uint32_t const * p_tx_buffer)
{
    nrf_i2s_transfer_set(p_reg, size, p_rx_buffer, p_tx_buffer);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_rx_buffer_set} */
NRFY_STATIC_INLINE void nrfy_i2s_rx_buffer_set(NRF_I2S_Type * p_reg,
                                               uint32_t *     p_buffer)
{
    nrf_i2s_rx_buffer_set(p_reg, p_buffer);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_rx_buffer_get} */
NRFY_STATIC_INLINE uint32_t * nrfy_i2s_rx_buffer_get(NRF_I2S_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t * p_buffer = nrf_i2s_rx_buffer_get(p_reg);
    nrf_barrier_r();
    return p_buffer;
}

/** @refhal{nrf_i2s_tx_buffer_set} */
NRFY_STATIC_INLINE void nrfy_i2s_tx_buffer_set(NRF_I2S_Type *   p_reg,
                                               uint32_t const * p_buffer)
{
    nrf_i2s_tx_buffer_set(p_reg, p_buffer);
    nrf_barrier_w();
}

/** @refhal{nrf_i2s_tx_buffer_get} */
NRFY_STATIC_INLINE uint32_t * nrfy_i2s_tx_buffer_get(NRF_I2S_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t * p_buffer = nrf_i2s_tx_buffer_get(p_reg);
    nrf_barrier_r();
    return p_buffer;
}

/** @} */

NRFY_STATIC_INLINE bool __nrfy_internal_i2s_event_handle(NRF_I2S_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_i2s_event_t event,
                                                         uint32_t *      p_event_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_i2s_event_check(p_reg, event))
    {
        nrf_i2s_event_clear(p_reg, event);
        if (p_event_mask)
        {
            *p_event_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_i2s_events_process(NRF_I2S_Type *               p_reg,
                                                               uint32_t                     mask,
                                                               nrfy_i2s_xfer_desc_t const * p_xfer)
{
    uint32_t event_mask = 0;
    bool invalidated = false;

    nrf_barrier_r();
#if NRF_I2S_HAS_FRAMESTART
    (void)__nrfy_internal_i2s_event_handle(p_reg, mask, NRF_I2S_EVENT_FRAMESTART, &event_mask);
#endif

    (void)__nrfy_internal_i2s_event_handle(p_reg, mask, NRF_I2S_EVENT_TXPTRUPD, &event_mask);

    if (__nrfy_internal_i2s_event_handle(p_reg, mask, NRF_I2S_EVENT_RXPTRUPD, &event_mask) &&
        p_xfer)
    {
        NRFY_CACHE_INV(p_xfer->p_rx_buffer,
                       (p_xfer->buffer_size * sizeof(uint32_t)));
        invalidated = true;
    }

    if (__nrfy_internal_i2s_event_handle(p_reg, mask, NRF_I2S_EVENT_STOPPED, &event_mask) &&
        p_xfer && !invalidated)
    {
        NRFY_CACHE_INV(p_xfer->p_rx_buffer,
                       (p_xfer->buffer_size * sizeof(uint32_t)));
    }

    return event_mask;
}


NRFY_STATIC_INLINE void __nrfy_internal_i2s_event_enabled_clear(NRF_I2S_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_i2s_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_i2s_event_clear(p_reg, event);
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_I2S_H__
