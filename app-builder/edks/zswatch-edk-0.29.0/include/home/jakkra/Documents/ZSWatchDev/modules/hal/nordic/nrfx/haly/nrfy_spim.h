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

#ifndef NRFY_SPIM_H__
#define NRFY_SPIM_H__

#include <nrfx.h>
#include <hal/nrf_spim.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nrfy_spim_xfer_desc_t nrfy_spim_xfer_desc_t;

NRFY_STATIC_INLINE bool __nrfy_internal_spim_event_handle(NRF_SPIM_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_spim_event_t event,
                                                          uint32_t *       p_evt_mask);

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_spim_events_process(NRF_SPIM_Type *               p_reg,
                                             uint32_t                      mask,
                                             nrfy_spim_xfer_desc_t const * p_xfer);

NRFY_STATIC_INLINE void __nrfy_internal_spim_event_enabled_clear(NRF_SPIM_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_spim_event_t event);

/**
 * @defgroup nrfy_spim SPIM HALY
 * @{
 * @ingroup nrf_spim
 * @brief   Hardware access layer with cache and barrier support for managing the SPIM peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

#if NRF_SPIM_HAS_HW_CSN || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SPIM_HAS_HW_CSN} */
#define NRFY_SPIM_HAS_HW_CSN 1
#else
#define NRFY_SPIM_HAS_HW_CSN 0
#endif

#if NRF_SPIM_HAS_DCX || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SPIM_HAS_DCX} */
#define NRFY_SPIM_HAS_DCX 1
#else
#define NRFY_SPIM_HAS_DCX 0
#endif

#if NRF_SPIM_HAS_RXDELAY || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SPIM_HAS_RXDELAY} */
#define NRFY_SPIM_HAS_RXDELAY 1
#else
#define NRFY_SPIM_HAS_RXDELAY 0
#endif

#if NRF_SPIM_HAS_STALLSTAT || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SPIM_HAS_STALLSTAT} */
#define NRFY_SPIM_HAS_STALLSTAT 1
#else
#define NRFY_SPIM_HAS_STALLSTAT 0
#endif

#if NRF_SPIM_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SPIM_HAS_EXTENDED} */
#define NRFY_SPIM_HAS_EXTENDED 1
#else
#define NRFY_SPIM_HAS_EXTENDED 0
#endif

#if NRF_SPIM_HAS_ARRAY_LIST || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SPIM_HAS_ARRAY_LIST} */
#define NRFY_SPIM_HAS_ARRAY_LIST 1
#else
#define NRFY_SPIM_HAS_ARRAY_LIST 0
#endif

#if NRF_SPIM_HAS_FREQUENCY || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SPIM_HAS_FREQUENCY} */
#define NRFY_SPIM_HAS_FREQUENCY 1
#else
#define NRFY_SPIM_HAS_FREQUENCY 0
#endif

#if NRF_SPIM_HAS_PRESCALER || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_SPIM_HAS_PRESCALER} */
#define NRFY_SPIM_HAS_PRESCALER 1
#else
#define NRFY_SPIM_HAS_PRESCALER 0
#endif

/** @brief Structure describing single SPIM transfer. */
struct nrfy_spim_xfer_desc_t
{
    uint8_t const * p_tx_buffer; ///< Pointer to the TX data buffer.
    size_t          tx_length;   ///< TX data buffer length.
    uint8_t *       p_rx_buffer; ///< Pointer to the RX data buffer.
    size_t          rx_length;   ///< RX data buffer length.
};

#if NRFY_SPIM_HAS_EXTENDED
/** @brief Configuration structure for SPIM pins used for extended features. */
typedef struct
{
#if NRFY_SPIM_HAS_DCX
    uint32_t dcx_pin; ///< D/CX pin number.
                      /**< Set to @ref NRF_SPIM_PIN_NOT_CONNECTED if this signal is not needed. */
#endif
#if NRFY_SPIM_HAS_HW_CSN
    uint32_t csn_pin; ///< CSN pin number.
                      /**< Set to @ref NRF_SPIM_PIN_NOT_CONNECTED if this signal is not needed. */
#endif
} nrfy_spim_ext_pins_t;

/** @brief Configuration structure for SPIM extended features. */
typedef struct
{
    nrfy_spim_ext_pins_t pins;         ///< Pin configuration structure.
#if NRFY_SPIM_HAS_HW_CSN
    nrf_spim_csn_pol_t   csn_pol;      ///< Polarity of the CSN pin.
    uint8_t              csn_duration; ///< Minimum duration between the edge of CSN and the edge of SCK.
                                       /**< Also, minimum duration of CSN inactivity between transactions.
                                        *   The value is specified in number of 64 MHz clock cycles (15.625 ns). */
#endif
#if NRFY_SPIM_HAS_RXDELAY
    uint8_t              rx_delay;     ///< Sample delay for input serial data on MISO.
                                       /**< This value specifies the delay between the occurrence
                                        *   of the SCK sampling edge and actual sampling operation,
                                        *   expressed in number of 64 MHz clock cycles (15.625 ns). */
#endif
} nrfy_spim_ext_config_t;
#endif // NRFY_SPIM_HAS_EXTENDED

/** @brief SPIM pins configuration structure. */
typedef struct
{
    uint32_t sck_pin;  ///< SCK pin number.
    uint32_t mosi_pin; ///< MOSI pin number.
                       /**< Set to @ref NRF_SPIM_PIN_NOT_CONNECTED if this signal is not needed. */
    uint32_t miso_pin; ///< MISO pin number.
                       /**< Set to @ref NRF_SPIM_PIN_NOT_CONNECTED if this signal is not needed. */
} nrfy_spim_pins_t;

/** @brief SPIM configuration structure. */
typedef struct
{
    nrfy_spim_pins_t       pins;          ///< Pin configuration structure.
    uint8_t                orc;           ///< Overrun character.
                                          /**< This character is transmitted when the TX buffer gets exhausted,
                                               but the transaction continues due to RX. */
#if NRFY_SPIM_HAS_FREQUENCY
    nrf_spim_frequency_t   frequency;     ///< SPIM frequency.
#elif NRFY_SPIM_HAS_PRESCALER
    uint32_t               prescaler;     ///< SPIM prescaler value.
#endif
    nrf_spim_mode_t        mode;          ///< SPIM mode.
    nrf_spim_bit_order_t   bit_order;     ///< SPIM bit order.
#if NRFY_SPIM_HAS_EXTENDED
    nrfy_spim_ext_config_t ext_config;    ///< Extended features configuration structure.
#endif
    bool                   skip_psel_cfg; ///< Skip pin selection configuration.
                                          /**< When set to true, the driver does not modify
                                           *   pin select registers in the peripheral.
                                           *   Those registers are supposed to be set up
                                           *   externally before the driver is initialized.
                                           *   @note When both GPIO configuration and pin
                                           *   selection are to be skipped, the structure
                                           *   fields that specify pins can be omitted,
                                           *   as they are ignored anyway. */
} nrfy_spim_config_t;

/**
 * @brief Function for configuring the SPIM.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_spim_periph_configure(NRF_SPIM_Type *            p_reg,
                                                   nrfy_spim_config_t const * p_config)
{
    if (!p_config->skip_psel_cfg)
    {
        nrf_spim_pins_set(p_reg,
            p_config->pins.sck_pin, p_config->pins.mosi_pin, p_config->pins.miso_pin);
#if NRFY_SPIM_HAS_DCX
        nrf_spim_dcx_pin_set(p_reg, p_config->ext_config.pins.dcx_pin);
#endif
#if NRFY_SPIM_HAS_HW_CSN
        nrf_spim_csn_configure(p_reg,
                               p_config->ext_config.pins.csn_pin,
                               p_config->ext_config.csn_pol,
                               p_config->ext_config.csn_duration);
#endif
    }
    nrf_spim_orc_set(p_reg, p_config->orc);
#if NRFY_SPIM_HAS_FREQUENCY
    nrf_spim_frequency_set(p_reg, p_config->frequency);
#elif NRFY_SPIM_HAS_PRESCALER
    nrf_spim_prescaler_set(p_reg, p_config->prescaler);
#endif
    nrf_spim_configure(p_reg, p_config->mode, p_config->bit_order);
#if NRFY_SPIM_HAS_RXDELAY
    nrf_spim_iftiming_set(p_reg, p_config->ext_config.rx_delay);
#endif
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified SPIM interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts associated with the event mask are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_spim_int_init(NRF_SPIM_Type * p_reg,
                                           uint32_t        mask,
                                           uint8_t         irq_priority,
                                           bool            enable)
{
    __nrfy_internal_spim_event_enabled_clear(p_reg, mask, NRF_SPIM_EVENT_STOPPED);
    __nrfy_internal_spim_event_enabled_clear(p_reg, mask, NRF_SPIM_EVENT_ENDRX);
    __nrfy_internal_spim_event_enabled_clear(p_reg, mask, NRF_SPIM_EVENT_END);
    __nrfy_internal_spim_event_enabled_clear(p_reg, mask, NRF_SPIM_EVENT_ENDTX);
    __nrfy_internal_spim_event_enabled_clear(p_reg, mask, NRF_SPIM_EVENT_STARTED);

    nrf_barrier_w();
    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_spim_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the SPIM interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_spim_int_uninit(NRF_SPIM_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified SPIM events.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mask   Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK();
 * @param[in] p_xfer Pointer to the structure containing buffers associated with the last transaction.
 *                   Can be NULL.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_spim_events_process(NRF_SPIM_Type *               p_reg,
                                                     uint32_t                      mask,
                                                     nrfy_spim_xfer_desc_t const * p_xfer)
{
    uint32_t evt_mask = __nrfy_internal_spim_events_process(p_reg, mask, p_xfer);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for setting the SPIM transaction buffers.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing transaction buffers.
 */
NRFY_STATIC_INLINE void nrfy_spim_buffers_set(NRF_SPIM_Type *               p_reg,
                                              nrfy_spim_xfer_desc_t const * p_xfer)
{
    if (p_xfer->p_tx_buffer)
    {
        NRFY_CACHE_WB(p_xfer->p_tx_buffer, p_xfer->tx_length);
    }
    nrf_spim_tx_buffer_set(p_reg, p_xfer->p_tx_buffer, p_xfer->tx_length);
    nrf_spim_rx_buffer_set(p_reg, p_xfer->p_rx_buffer, p_xfer->rx_length);
    nrf_barrier_w();
}

/**
 * @brief Function for starting the SPIM transaction.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing transaction buffers
 *                   if the transaction is to be blocking. NULL for non-blocking transactions.
 */
NRFY_STATIC_INLINE void nrfy_spim_xfer_start(NRF_SPIM_Type *               p_reg,
                                             nrfy_spim_xfer_desc_t const * p_xfer)
{
    nrf_spim_task_trigger(p_reg, NRF_SPIM_TASK_START);
    if (p_xfer)
    {
        nrf_barrier_w();
        while (!nrf_spim_event_check(p_reg, NRF_SPIM_EVENT_END))
        {}
        (void)__nrfy_internal_spim_events_process(p_reg,
                                                  NRFY_EVENT_TO_INT_BITMASK(NRF_SPIM_EVENT_END),
                                                  p_xfer);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for aborting the ongoing SPIM transaction.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_xfer Pointer to the structure containing transaction buffers
 *                   if the abort is to be blocking. NULL for non-blocking operation.
 */
NRFY_STATIC_INLINE void nrfy_spim_abort(NRF_SPIM_Type * p_reg, nrfy_spim_xfer_desc_t const * p_xfer)
{
    nrf_spim_task_trigger(p_reg, NRF_SPIM_TASK_STOP);
    if (p_xfer)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_SPIM_EVENT_STOPPED);
        while (!__nrfy_internal_spim_events_process(p_reg, evt_mask, p_xfer))
        {}
        (void)__nrfy_internal_spim_events_process(p_reg,
                                                  NRFY_EVENT_TO_INT_BITMASK(NRF_SPIM_EVENT_END),
                                                  NULL);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for getting the SPIM pins configuration.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_pins Pointer to the structure to be filled with SPIM pins configuration.
 */
NRFY_STATIC_INLINE void nrfy_spim_pins_get(NRF_SPIM_Type const * p_reg, nrfy_spim_pins_t * p_pins)
{
    p_pins->sck_pin  = nrf_spim_sck_pin_get(p_reg);
    p_pins->mosi_pin = nrf_spim_mosi_pin_get(p_reg);
    p_pins->miso_pin = nrf_spim_miso_pin_get(p_reg);
}

#if NRFY_SPIM_HAS_EXTENDED
/**
 * @brief Function for getting the configuration of the SPIM pins used for extended features.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_pins Pointer to the structure to be filled with SPIM pins configuration.
 */
NRFY_STATIC_INLINE void nrfy_spim_ext_pins_get(NRF_SPIM_Type const *  p_reg,
                                               nrfy_spim_ext_pins_t * p_pins)
{
    p_pins->dcx_pin = nrf_spim_dcx_pin_get(p_reg);
#if NRFY_SPIM_HAS_HW_CSN
    p_pins->csn_pin = nrf_spim_csn_pin_get(p_reg);
#endif
}
#endif

#if NRFY_SPIM_HAS_ARRAY_LIST
/**
 * @brief Function for enabling or disabling the TX list feature.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if TX list feature is to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_spim_tx_list_set(NRF_SPIM_Type * p_reg, bool enable)
{
    if (enable)
    {
        nrf_spim_tx_list_enable(p_reg);
    }
    else
    {
        nrf_spim_tx_list_disable(p_reg);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for enabling or disabling the RX list feature.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if RX list feature is to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_spim_rx_list_set(NRF_SPIM_Type * p_reg, bool enable)
{
    if (enable)
    {
        nrf_spim_rx_list_enable(p_reg);
    }
    else
    {
        nrf_spim_rx_list_disable(p_reg);
    }
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_spim_task_trigger} */
NRFY_STATIC_INLINE void nrfy_spim_task_trigger(NRF_SPIM_Type * p_reg,
                                               nrf_spim_task_t task)
{
    nrf_spim_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_task_address_get(NRF_SPIM_Type const * p_reg,
                                                       nrf_spim_task_t       task)
{
    return nrf_spim_task_address_get(p_reg, task);
}

/** @refhal{nrf_spim_event_clear} */
NRFY_STATIC_INLINE void nrfy_spim_event_clear(NRF_SPIM_Type * p_reg, nrf_spim_event_t event)
{
    nrf_spim_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_event_check} */
NRFY_STATIC_INLINE bool nrfy_spim_event_check(NRF_SPIM_Type const * p_reg, nrf_spim_event_t event)
{
    nrf_barrier_r();
    bool check = nrf_spim_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_spim_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_event_address_get(NRF_SPIM_Type const * p_reg,
                                                        nrf_spim_event_t      event)
{
    return nrf_spim_event_address_get(p_reg, event);
}

/** @refhal{nrf_spim_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_spim_shorts_enable(NRF_SPIM_Type * p_reg, uint32_t mask)
{
    nrf_spim_shorts_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_spim_shorts_disable(NRF_SPIM_Type * p_reg, uint32_t mask)
{
    nrf_spim_shorts_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_shorts_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_shorts_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t shorts = nrf_spim_shorts_get(p_reg);
    nrf_barrier_r();
    return shorts;
}

/** @refhal{nrf_spim_int_enable} */
NRFY_STATIC_INLINE void nrfy_spim_int_enable(NRF_SPIM_Type * p_reg, uint32_t mask)
{
    nrf_spim_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_int_disable} */
NRFY_STATIC_INLINE void nrfy_spim_int_disable(NRF_SPIM_Type * p_reg, uint32_t mask)
{
    nrf_spim_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_int_enable_check(NRF_SPIM_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_spim_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_spim_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_spim_subscribe_set(NRF_SPIM_Type * p_reg,
                                                nrf_spim_task_t task,
                                                uint8_t         channel)
{
    nrf_spim_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_spim_subscribe_clear(NRF_SPIM_Type * p_reg,
                                                  nrf_spim_task_t task)
{
    nrf_spim_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_publish_set} */
NRFY_STATIC_INLINE void nrfy_spim_publish_set(NRF_SPIM_Type *  p_reg,
                                              nrf_spim_event_t event,
                                              uint8_t          channel)
{
    nrf_spim_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_publish_clear} */
NRFY_STATIC_INLINE void nrfy_spim_publish_clear(NRF_SPIM_Type *  p_reg,
                                                nrf_spim_event_t event)
{
    nrf_spim_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/** @refhal{nrf_spim_enable} */
NRFY_STATIC_INLINE void nrfy_spim_enable(NRF_SPIM_Type * p_reg)
{
    nrf_spim_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_disable} */
NRFY_STATIC_INLINE void nrfy_spim_disable(NRF_SPIM_Type * p_reg)
{
    nrf_spim_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_enable_check} */
NRFY_STATIC_INLINE bool nrfy_spim_enable_check(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_spim_enable_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_spim_pins_set} */
NRFY_STATIC_INLINE void nrfy_spim_pins_set(NRF_SPIM_Type * p_reg,
                                           uint32_t        sck_pin,
                                           uint32_t        mosi_pin,
                                           uint32_t        miso_pin)
{
    nrf_spim_pins_set(p_reg, sck_pin, mosi_pin, miso_pin);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_sck_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_sck_pin_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_spim_sck_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_spim_mosi_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_mosi_pin_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_spim_mosi_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_spim_miso_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_miso_pin_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_spim_miso_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}


#if NRFY_SPIM_HAS_HW_CSN
/** @refhal{nrf_spim_csn_configure} */
NRFY_STATIC_INLINE void nrfy_spim_csn_configure(NRF_SPIM_Type *    p_reg,
                                                uint32_t           pin,
                                                nrf_spim_csn_pol_t polarity,
                                                uint32_t           duration)
{
    nrf_spim_csn_configure(p_reg, pin, polarity, duration);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_csn_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_csn_pin_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_spim_csn_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}
#endif

#if NRFY_SPIM_HAS_DCX
/** @refhal{nrf_spim_dcx_pin_set} */
NRFY_STATIC_INLINE void nrfy_spim_dcx_pin_set(NRF_SPIM_Type * p_reg,
                                              uint32_t        dcx_pin)
{
    nrf_spim_dcx_pin_set(p_reg, dcx_pin);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_dcx_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_dcx_pin_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pin = nrf_spim_dcx_pin_get(p_reg);
    nrf_barrier_r();
    return pin;
}

/** @refhal{nrf_spim_dcx_cnt_set} */
NRFY_STATIC_INLINE void nrfy_spim_dcx_cnt_set(NRF_SPIM_Type * p_reg, uint32_t count)
{
    nrf_spim_dcx_cnt_set(p_reg, count);
    nrf_barrier_w();
}
#endif

#if NRFY_SPIM_HAS_RXDELAY
/** @refhal{nrf_spim_iftiming_set} */
NRFY_STATIC_INLINE void nrfy_spim_iftiming_set(NRF_SPIM_Type * p_reg,
                                               uint32_t        rxdelay)
{
    nrf_spim_iftiming_set(p_reg, rxdelay);
    nrf_barrier_w();
}
#endif

#if NRFY_SPIM_HAS_STALLSTAT
/** @refhal{nrf_spim_stallstat_rx_clear} */
NRFY_STATIC_INLINE void nrfy_spim_stallstat_rx_clear(NRF_SPIM_Type * p_reg)
{
    nrf_spim_stallstat_rx_clear(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_stallstat_rx_get} */
NRFY_STATIC_INLINE bool nrfy_spim_stallstat_rx_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    bool stallstat = nrf_spim_stallstat_rx_get(p_reg);
    nrf_barrier_r();
    return stallstat;
}

/** @refhal{nrf_spim_stallstat_tx_clear} */
NRFY_STATIC_INLINE void nrfy_spim_stallstat_tx_clear(NRF_SPIM_Type * p_reg)
{
    nrf_spim_stallstat_tx_clear(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_stallstat_tx_get} */
NRFY_STATIC_INLINE bool nrfy_spim_stallstat_tx_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    bool stallstat = nrf_spim_stallstat_tx_get(p_reg);
    nrf_barrier_r();
    return stallstat;
}
#endif // NRFY_SPIM_HAS_STALLSTAT

#if NRFY_SPIM_HAS_FREQUENCY
/** @refhal{nrf_spim_frequency_set} */
NRFY_STATIC_INLINE void nrfy_spim_frequency_set(NRF_SPIM_Type *      p_reg,
                                                nrf_spim_frequency_t frequency)
{
    nrf_spim_frequency_set(p_reg, frequency);
    nrf_barrier_w();
}
#endif

#if NRFY_SPIM_HAS_PRESCALER
/** @refhal{nrf_spim_prescaler_set} */
NRFY_STATIC_INLINE void nrfy_spim_prescaler_set(NRF_SPIM_Type * p_reg, uint32_t prescaler)
{
    nrf_spim_prescaler_set(p_reg, prescaler);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_prescaler_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_prescaler_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t prescaler = nrf_spim_prescaler_get(p_reg);
    nrf_barrier_r();
    return prescaler;
}
#endif

/** @refhal{nrf_spim_tx_buffer_set} */
NRFY_STATIC_INLINE void nrfy_spim_tx_buffer_set(NRF_SPIM_Type * p_reg,
                                                uint8_t const * p_buffer,
                                                size_t          length)
{
    nrf_spim_tx_buffer_set(p_reg, p_buffer, length);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_tx_amount_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_tx_amount_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t amount = nrf_spim_tx_amount_get(p_reg);
    nrf_barrier_r();
    return amount;
}

/** @refhal{nrf_spim_tx_maxcnt_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_tx_maxcnt_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t maxcnt = nrf_spim_tx_maxcnt_get(p_reg);
    nrf_barrier_r();
    return maxcnt;
}

/** @refhal{nrf_spim_rx_buffer_set} */
NRFY_STATIC_INLINE void nrfy_spim_rx_buffer_set(NRF_SPIM_Type * p_reg,
                                                uint8_t *       p_buffer,
                                                size_t          length)
{
    nrf_spim_rx_buffer_set(p_reg, p_buffer, length);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_tx_amount_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_rx_amount_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t amount = nrf_spim_rx_amount_get(p_reg);
    nrf_barrier_r();
    return amount;
}

/** @refhal{nrf_spim_rx_maxcnt_get} */
NRFY_STATIC_INLINE uint32_t nrfy_spim_rx_maxcnt_get(NRF_SPIM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t maxcnt = nrf_spim_rx_maxcnt_get(p_reg);
    nrf_barrier_r();
    return maxcnt;
}

/** @refhal{nrf_spim_configure} */
NRFY_STATIC_INLINE void nrfy_spim_configure(NRF_SPIM_Type *      p_reg,
                                            nrf_spim_mode_t      spi_mode,
                                            nrf_spim_bit_order_t spi_bit_order)
{
    nrf_spim_configure(p_reg, spi_mode, spi_bit_order);
    nrf_barrier_w();
}

/** @refhal{nrf_spim_orc_set} */
NRFY_STATIC_INLINE void nrfy_spim_orc_set(NRF_SPIM_Type * p_reg,
                                          uint8_t         orc)
{
    nrf_spim_orc_set(p_reg, orc);
    nrf_barrier_w();
}

/** @} */

NRFY_STATIC_INLINE bool __nrfy_internal_spim_event_handle(NRF_SPIM_Type *  p_reg,
                                                          uint32_t         mask,
                                                          nrf_spim_event_t event,
                                                          uint32_t *       p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_spim_event_check(p_reg, event))
    {
        nrf_spim_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE
uint32_t __nrfy_internal_spim_events_process(NRF_SPIM_Type *               p_reg,
                                             uint32_t                      mask,
                                             nrfy_spim_xfer_desc_t const * p_xfer)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    (void)__nrfy_internal_spim_event_handle(p_reg, mask, NRF_SPIM_EVENT_STARTED, &evt_mask);

    bool stop = __nrfy_internal_spim_event_handle(p_reg, mask, NRF_SPIM_EVENT_STOPPED, &evt_mask);

    bool invalidated = false;
    if (__nrfy_internal_spim_event_handle(p_reg, mask, NRF_SPIM_EVENT_END, &evt_mask) && p_xfer)
    {
        size_t size = stop ? nrf_spim_rx_amount_get(p_reg) : p_xfer->rx_length;
        NRFY_CACHE_INV(p_xfer->p_rx_buffer, size);
        invalidated = true;
    }

    if (__nrfy_internal_spim_event_handle(p_reg, mask, NRF_SPIM_EVENT_ENDRX, &evt_mask) &&
        p_xfer && !invalidated)
    {
        size_t size = stop ? nrf_spim_rx_amount_get(p_reg) : p_xfer->rx_length;
        NRFY_CACHE_INV(p_xfer->p_rx_buffer, size);
    }

    (void)__nrfy_internal_spim_event_handle(p_reg, mask, NRF_SPIM_EVENT_ENDTX, &evt_mask);

    return evt_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_spim_event_enabled_clear(NRF_SPIM_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_spim_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_spim_event_clear(p_reg, event);
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_SPIM_H__
