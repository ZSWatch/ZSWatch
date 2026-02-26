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

#ifndef NRFY_NFCT_H__
#define NRFY_NFCT_H__

#include <nrfx.h>
#include <hal/nrf_nfct.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_nfct_event_enabled_clear(NRF_NFCT_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_nfct_event_t event);

NRFY_STATIC_INLINE bool __nrfy_internal_nfct_event_handle(NRF_NFCT_Type  * p_reg,
                                                          uint32_t         mask,
                                                          nrf_nfct_event_t event,
                                                          uint32_t       * p_evt_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_nfct_events_process(NRF_NFCT_Type * p_reg,
                                                                uint32_t        mask);

/**
 * @defgroup nrfy_nfct NFCT HALY
 * @{
 * @ingroup nrf_nfct
 * @brief   Hardware access layer with cache and barrier support for managing the NFCT peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

#if NRF_NFCT_HAS_MODULATION_PSEL_REG || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_NFCT_HAS_MODULATION_PSEL_REG} */
#define NRFY_NFCT_HAS_MODULATION_PSEL_REG 1
#else
#define NRFY_NFCT_HAS_MODULATION_PSEL_REG 0
#endif

#if NRF_NFCT_HAS_MODULATION_CTRL_REG || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_NFCT_HAS_MODULATION_CTRL_REG} */
#define NRFY_NFCT_HAS_MODULATION_CTRL_REG 1
#else
#define NRFY_NFCT_HAS_MODULATION_CTRL_REG 0
#endif

#if NRF_NFCT_HAS_TAG_STATE_REG || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_NFCT_HAS_TAG_STATE_REG} */
#define NRFY_NFCT_HAS_TAG_STATE_REG 1
#else
#define NRFY_NFCT_HAS_TAG_STATE_REG 0
#endif

#if NRF_NFCT_HAS_SLEEP_STATE_REG || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_NFCT_HAS_SLEEP_STATE_REG} */
#define NRFY_NFCT_HAS_SLEEP_STATE_REG 1
#else
#define NRFY_NFCT_HAS_SLEEP_STATE_REG 0
#endif

#if NRF_NFCT_HAS_AUTOCOLRES_CONFIG_REG || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_NFCT_HAS_AUTOCOLRES_CONFIG_REG} */
#define NRFY_NFCT_HAS_AUTOCOLRES_CONFIG_REG 1
#else
#define NRFY_NFCT_HAS_AUTOCOLRES_CONFIG_REG 0
#endif

#if NRF_NFCT_HAS_PAD_CONFIG_REG || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_NFCT_HAS_PAD_CONFIG_REG} */
#define NRFY_NFCT_HAS_PAD_CONFIG_REG 1
#else
#define NRFY_NFCT_HAS_PAD_CONFIG_REG 0
#endif

#if NRF_NFCT_HAS_BIAS_CONFIG_TRIM_REG || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_NFCT_HAS_BIAS_CONFIG_TRIM_REG} */
#define NRFY_NFCT_HAS_BIAS_CONFIG_TRIM_REG 1
#else
#define NRFY_NFCT_HAS_BIAS_CONFIG_TRIM_REG 0
#endif

/** @brief NFCT parameters storage structure. */
typedef struct
{
    uint32_t                       fdmax;                                       ///< Frame delay max value.
    uint32_t                       fdmin;                                       ///< Frame delay min value.
    uint32_t                       int_enabled;                                 ///< Interrupts status.
    uint8_t                        nfcid1[NRF_NFCT_SENSRES_NFCID1_SIZE_TRIPLE]; ///< NFCID1 value.
    nrf_nfct_sensres_nfcid1_size_t nfcid1_size;                                 ///< NFCID1 size.
    nrf_nfct_selres_protocol_t     protocol;                                    ///< NFC protocol type.
} nrfy_nfct_parameters_t;

/**
 * @brief Function for initializing the specified NFCT interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupts priority.
 * @param[in] enable       True if interrupts associated with the event mask are to be enabled,
 *                         false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_nfct_int_init(NRF_NFCT_Type * p_reg,
                                           uint32_t        mask,
                                           uint8_t         irq_priority,
                                           bool            enable)
{
    IRQn_Type nfct_irq = nrfx_get_irq_number(p_reg);

    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_READY);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_FIELDDETECTED);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_FIELDLOST);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_TXFRAMESTART);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_TXFRAMEEND);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_RXFRAMESTART);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_RXFRAMEEND);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_ERROR);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_RXERROR);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_ENDRX);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_ENDTX);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_AUTOCOLRESSTARTED);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_COLLISION);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_SELECTED);
    __nrfy_internal_nfct_event_enabled_clear(p_reg, mask, NRF_NFCT_EVENT_STARTED);
    nrf_barrier_w();

    NRFY_IRQ_PENDING_CLEAR(nfct_irq);
    nrf_barrier_w();

    NRFY_IRQ_PRIORITY_SET(nfct_irq, irq_priority);
    NRFY_IRQ_ENABLE(nfct_irq);

    if (enable)
    {
        nrf_nfct_int_enable(p_reg, mask);
    }

    nrf_barrier_w();
}

/**
 * @brief Function for un-initializing the NFCT interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_nfct_int_uninit(NRF_NFCT_Type * p_reg)
{
    IRQn_Type nfct_irq = nrfx_get_irq_number(p_reg);

    NRFY_IRQ_DISABLE(nfct_irq);
    nrf_barrier_w();
    NRFY_IRQ_PENDING_CLEAR(nfct_irq);
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified NFCT events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of the events to be processed, created by the
 *                  @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of the events that were generated and processed.
 *         To be checked against the result of the @ref NRFY_EVENT_TO_INT_BITMASK().
*/
NRFY_STATIC_INLINE uint32_t nrfy_nfct_events_process(NRF_NFCT_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = __nrfy_internal_nfct_events_process(p_reg, mask);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for saving NFCT parameters before peripheral reset.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_params Pointer to the structure where the parameters will be saved.
*/
NRFY_STATIC_INLINE void nrfy_nfct_parameters_save(NRF_NFCT_Type const *    p_reg,
                                                  nrfy_nfct_parameters_t * p_params)
{
    nrf_barrier_rw();
    p_params->fdmax       = nrf_nfct_frame_delay_max_get(p_reg);
    p_params->fdmin       = nrf_nfct_frame_delay_min_get(p_reg);
    p_params->nfcid1_size = nrf_nfct_nfcid1_get(p_reg, p_params->nfcid1);
    p_params->protocol    = nrf_nfct_selres_protocol_get(p_reg);
    p_params->int_enabled = nrf_nfct_int_enable_get(p_reg);
    nrf_barrier_r();
}

/**
 * @brief Function for restoring NFCT parameters after peripheral reset. The parameters are
 *        written back to the peripheral registers.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_params Pointer to the structure holding peripheral parameters to be restored.
 */
NRFY_STATIC_INLINE void nrfy_nfct_parameters_restore(NRF_NFCT_Type *                p_reg,
                                                     nrfy_nfct_parameters_t const * p_params)
{
    nrf_nfct_frame_delay_max_set(p_reg, p_params->fdmax);
    nrf_nfct_frame_delay_min_set(p_reg, (uint16_t)p_params->fdmin);
    nrf_nfct_nfcid1_set(p_reg, p_params->nfcid1, p_params->nfcid1_size);
    nrf_nfct_selres_protocol_set(p_reg, p_params->protocol);

    nrf_barrier_w();
}

/**
 * @brief Function for setting the the NFCT RX/TX buffer (address and maximum length).
 *
 * @note Buffer for the NFC RX/TX data is used by EasyDMA and must be located in RAM.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] p_rxtx_buf   Pointer to the receive or transmit buffer.
 * @param[in] max_txrx_len Maximum receive or transmit length in bytes
 *                         (size of the RAM buffer for EasyDMA).
 * @param[in] rx           True if buffer is set for the frame reception, false if buffer is set for
 *                         the frame transmission.
 */
NRFY_STATIC_INLINE void nrfy_nfct_rxtx_buffer_set(NRF_NFCT_Type * p_reg,
                                                  uint8_t *       p_rxtx_buf,
                                                  uint16_t        max_txrx_len,
                                                  bool            rx)
{
    if (p_rxtx_buf && !rx)
    {
        NRFY_CACHE_WB(p_rxtx_buf, max_txrx_len);
    }

    nrf_nfct_rxtx_buffer_set(p_reg, p_rxtx_buf, max_txrx_len);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_task_trigger} */
NRFY_STATIC_INLINE void nrfy_nfct_task_trigger(NRF_NFCT_Type * p_reg, nrf_nfct_task_t task)
{
    nrf_nfct_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_task_address_get(NRF_NFCT_Type const * p_reg,
                                                       nrf_nfct_task_t       task)
{
    return nrf_nfct_task_address_get(p_reg, task);
}

/** @refhal{nrf_nfct_event_clear} */
NRFY_STATIC_INLINE void nrfy_nfct_event_clear(NRF_NFCT_Type * p_reg, nrf_nfct_event_t event)
{
    nrf_nfct_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_event_check} */
NRFY_STATIC_INLINE bool nrfy_nfct_event_check(NRF_NFCT_Type const * p_reg, nrf_nfct_event_t event)
{
    nrf_barrier_rw();
    bool check = nrf_nfct_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_nfct_shorts_enable} */
NRFY_STATIC_INLINE void nrfy_nfct_shorts_enable(NRF_NFCT_Type * p_reg, uint32_t short_mask)
{
    nrf_nfct_shorts_enable(p_reg, short_mask);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_shorts_disable} */
NRFY_STATIC_INLINE void nrfy_nfct_shorts_disable(NRF_NFCT_Type * p_reg, uint32_t short_mask)
{
    nrf_nfct_shorts_disable(p_reg, short_mask);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_shorts_get} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_shorts_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t shorts = nrf_nfct_shorts_get(p_reg);
    nrf_barrier_r();
    return shorts;
}

/** @refhal{nrf_nfct_shorts_set} */
NRFY_STATIC_INLINE void nrfy_nfct_shorts_set(NRF_NFCT_Type * p_reg, uint32_t short_mask)
{
    nrf_nfct_shorts_set(p_reg, short_mask);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_int_enable} */
NRFY_STATIC_INLINE void nrfy_nfct_int_enable(NRF_NFCT_Type * p_reg, uint32_t mask)
{
    nrf_nfct_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_int_enable_check(NRF_NFCT_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_nfct_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_nfct_int_enable_get} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_int_enable_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t check = nrf_nfct_int_enable_get(p_reg);
    nrf_barrier_w();
    return check;
}

/** @refhal{nrf_nfct_int_disable} */
NRFY_STATIC_INLINE void nrfy_nfct_int_disable(NRF_NFCT_Type * p_reg, uint32_t mask)
{
    nrf_nfct_int_disable(p_reg, mask);
    nrf_barrier_w();
}

#if NRFY_NFCT_HAS_MODULATION_PSEL_REG
/** @refhal{nrf_nfct_mod_ctrl_pin_set} */
NRFY_STATIC_INLINE void nrfy_nfct_mod_ctrl_pin_set(NRF_NFCT_Type * p_reg, uint32_t mod_ctrl_pin)
{
    nrf_nfct_mod_ctrl_pin_set(p_reg, mod_ctrl_pin);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_mod_ctrl_pin_get} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_mod_ctrl_pin_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t pins = nrf_nfct_mod_ctrl_pin_get(p_reg);
    nrf_barrier_r();
    return pins;
}
#endif // NRFY_NFCT_HAS_MODULATION_PSEL_REG

/** @refhal{nrf_nfct_modulation_output_set} */
#if NRFY_NFCT_HAS_MODULATION_CTRL_REG
NRFY_STATIC_INLINE void nrfy_nfct_modulation_output_set(NRF_NFCT_Type *            p_reg,
                                                        nrf_nfct_modulation_ctrl_t mod_ctrl)
{
    nrf_nfct_modulation_output_set(p_reg, mod_ctrl);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_modulation_output_get} */
NRFY_STATIC_INLINE
nrf_nfct_modulation_ctrl_t nrfy_nfct_modulation_output_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_nfct_modulation_ctrl_t modulation_ctrl = nrf_nfct_modulation_output_get(p_reg);
    nrf_barrier_r();
    return modulation_ctrl;
}
#endif // NRFY_NFCT_HAS_MODULATION_CTRL_REG

/** @refhal{nrf_nfct_error_status_get} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_error_status_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t error_status = nrf_nfct_error_status_get(p_reg);
    nrf_barrier_r();
    return error_status;
}

/** @refhal{nrf_nfct_error_status_clear} */
NRFY_STATIC_INLINE void nrfy_nfct_error_status_clear(NRF_NFCT_Type * p_reg, uint32_t error_flag)
{
    nrf_nfct_error_status_clear(p_reg, error_flag);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_rx_frame_status_get} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_rx_frame_status_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t frame_status = nrf_nfct_rx_frame_status_get(p_reg);
    nrf_barrier_r();
    return frame_status;
}

/** @refhal{nrf_nfct_rx_frame_status_clear} */
NRFY_STATIC_INLINE void nrfy_nfct_rx_frame_status_clear(NRF_NFCT_Type * p_reg,
                                                        uint32_t        framestatus_flags)
{
    nrf_nfct_rx_frame_status_clear(p_reg, framestatus_flags);
    nrf_barrier_w();
}

#if NRFY_NFCT_HAS_TAG_STATE_REG
/** @refhal{nrf_nfct_tag_state_get} */
NRFY_STATIC_INLINE nrf_nfct_tag_state_t nrfy_nfct_tag_state_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_r();
    nrf_nfct_tag_state_t tag_state = nrf_nfct_tag_state_get(p_reg);
    nrf_barrier_r();
    return tag_state;
}
#endif // NRFY_NFCT_HAS_TAG_STATE_REG

#if NRFY_NFCT_HAS_SLEEP_STATE_REG
/** @refhal{nrf_nfct_sleep_state_get} */
NRFY_STATIC_INLINE nrf_nfct_sleep_state_t nrfy_nfct_sleep_state_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_r();
    nrf_nfct_sleep_state_t sleep_state = nrf_nfct_sleep_state_get(p_reg);
    nrf_barrier_r();
    return sleep_state;
}
#endif // NRFY_NFCT_HAS_SLEEP_STATE_REG

/** @refhal{nrf_nfct_field_status_get} */
NRFY_STATIC_INLINE uint8_t nrfy_nfct_field_status_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_r();
    uint8_t status = nrf_nfct_field_status_get(p_reg);
    nrf_barrier_r();
    return status;
}

/** @refhal{nrf_nfct_frame_delay_min_get} */
NRFY_STATIC_INLINE uint16_t nrfy_nfct_frame_delay_min_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint16_t frame_delay_min = nrf_nfct_frame_delay_min_get(p_reg);
    nrf_barrier_r();
    return frame_delay_min;
}

/** @refhal{nrf_nfct_frame_delay_min_set} */
NRFY_STATIC_INLINE void nrfy_nfct_frame_delay_min_set(NRF_NFCT_Type * p_reg,
                                                      uint16_t        frame_delay_min)
{
    nrf_nfct_frame_delay_min_set(p_reg, frame_delay_min);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_frame_delay_max_get} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_frame_delay_max_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t frame_delay_max = nrf_nfct_frame_delay_max_get(p_reg);
    nrf_barrier_r();
    return frame_delay_max;
}

/** @refhal{nrf_nfct_frame_delay_max_set} */
NRFY_STATIC_INLINE void nrfy_nfct_frame_delay_max_set(NRF_NFCT_Type * p_reg,
                                                      uint32_t        frame_delay_max)
{
    nrf_nfct_frame_delay_max_set(p_reg, frame_delay_max);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_frame_delay_mode_get} */
NRFY_STATIC_INLINE
nrf_nfct_frame_delay_mode_t nrfy_nfct_frame_delay_mode_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_nfct_frame_delay_mode_t frame_delay_mode = nrf_nfct_frame_delay_mode_get(p_reg);
    nrf_barrier_r();
    return frame_delay_mode;
}

/** @refhal{nrf_nfct_frame_delay_mode_set} */
NRFY_STATIC_INLINE void nrfy_nfct_frame_delay_mode_set(NRF_NFCT_Type *             p_reg,
                                                       nrf_nfct_frame_delay_mode_t frame_delay_mode)
{
    nrf_nfct_frame_delay_mode_set(p_reg, frame_delay_mode);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_rxtx_buffer_get} */
NRFY_STATIC_INLINE uint8_t * nrfy_nfct_rxtx_buffer_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint8_t *buffer = nrf_nfct_rxtx_buffer_get(p_reg);
    nrf_barrier_r();
    return buffer;
}

/** @refhal{nrf_nfct_max_rxtx_length_get} */
NRFY_STATIC_INLINE uint16_t nrfy_nfct_max_rxtx_length_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint16_t max_length = nrf_nfct_max_rxtx_length_get(p_reg);
    nrf_barrier_r();
    return max_length;
}

/** @refhal{nrf_nfct_tx_frame_config_get} */
NRFY_STATIC_INLINE uint8_t nrfy_nfct_tx_frame_config_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint8_t frame_config = nrf_nfct_tx_frame_config_get(p_reg);
    nrf_barrier_r();
    return frame_config;
}

/** @refhal{nrf_nfct_tx_frame_config_set} */
NRFY_STATIC_INLINE void nrfy_nfct_tx_frame_config_set(NRF_NFCT_Type * p_reg, uint8_t flags)
{
    nrf_nfct_tx_frame_config_set(p_reg, flags);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_tx_bits_get} */
NRFY_STATIC_INLINE uint16_t nrfy_nfct_tx_bits_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint16_t bits = nrf_nfct_tx_bits_get(p_reg);
    nrf_barrier_r();
    return bits;
}

/** @refhal{nrf_nfct_tx_bits_set} */
NRFY_STATIC_INLINE void nrfy_nfct_tx_bits_set(NRF_NFCT_Type * p_reg, uint16_t tx_bits)
{
    nrf_nfct_tx_bits_set(p_reg, tx_bits);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_rx_frame_config_get} */
NRFY_STATIC_INLINE uint8_t nrfy_nfct_rx_frame_config_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint8_t frame_config = nrf_nfct_rx_frame_config_get(p_reg);
    nrf_barrier_r();
    return frame_config;
}

/** @refhal{nrf_nfct_rx_frame_config_set} */
NRFY_STATIC_INLINE void nrfy_nfct_rx_frame_config_set(NRF_NFCT_Type * p_reg, uint8_t flags)
{
    nrf_nfct_rx_frame_config_set(p_reg, flags);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_rx_bits_get} */
NRFY_STATIC_INLINE uint16_t nrfy_nfct_rx_bits_get(NRF_NFCT_Type const * p_reg, bool crc_excluded)
{
    nrf_barrier_r();
    uint16_t rx_bits = nrf_nfct_rx_bits_get(p_reg, crc_excluded);
    nrf_barrier_r();
    return rx_bits;
}

/** @refhal{nrf_nfct_nfcid1_get} */
NRFY_STATIC_INLINE
nrf_nfct_sensres_nfcid1_size_t nrfy_nfct_nfcid1_get(NRF_NFCT_Type const * p_reg,
                                                    uint8_t *             p_nfcid1_buf)
{
    nrf_barrier_rw();
    nrf_nfct_sensres_nfcid1_size_t nfcid1_size = nrf_nfct_nfcid1_get(p_reg, p_nfcid1_buf);
    nrf_barrier_rw();
    return nfcid1_size;
}

/** @refhal{nrf_nfct_nfcid1_set} */
NRFY_STATIC_INLINE void nrfy_nfct_nfcid1_set(NRF_NFCT_Type *                p_reg,
                                             uint8_t const *                p_nfcid1_buf,
                                             nrf_nfct_sensres_nfcid1_size_t nfcid1_size)
{
    nrf_nfct_nfcid1_set(p_reg, p_nfcid1_buf, nfcid1_size);
    nrf_barrier_w();
}

#if NRFY_NFCT_HAS_AUTOCOLRES_CONFIG_REG
/** @refhal{nrf_nfct_autocolres_is_enabled} */
NRFY_STATIC_INLINE bool nrfy_nfct_autocolres_is_enabled(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    bool enabled = nrf_nfct_autocolres_is_enabled(p_reg);
    nrf_barrier_r();
    return enabled;
}

/** @refhal{nrf_nfct_autocolres_enable} */
NRFY_STATIC_INLINE void nrfy_nfct_autocolres_enable(NRF_NFCT_Type * p_reg)
{
    nrf_nfct_autocolres_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_autocolres_disable} */
NRFY_STATIC_INLINE void nrfy_nfct_autocolres_disable(NRF_NFCT_Type * p_reg)
{
    nrf_nfct_autocolres_disable(p_reg);
    nrf_barrier_w();
}
#endif // NRFY_NFCT_HAS_AUTOCOLRES_CONFIG_REG

/** @refhal{nrf_nfct_sensres_nfcid1_size_get} */
NRFY_STATIC_INLINE
nrf_nfct_sensres_nfcid1_size_t nrfy_nfct_sensres_nfcid1_size_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_nfct_sensres_nfcid1_size_t nfcid1_size = nrf_nfct_sensres_nfcid1_size_get(p_reg);
    nrf_barrier_r();
    return nfcid1_size;
}

/** @refhal{nrf_nfct_sensres_nfcid1_size_set} */
NRFY_STATIC_INLINE
void nrfy_nfct_sensres_nfcid1_size_set(NRF_NFCT_Type *                p_reg,
                                       nrf_nfct_sensres_nfcid1_size_t nfcid1_size)
{
    nrf_nfct_sensres_nfcid1_size_set(p_reg, nfcid1_size);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_sensres_bit_frame_sdd_get} */
NRFY_STATIC_INLINE
nrf_nfct_sensres_bit_frame_sdd_t nrfy_nfct_sensres_bit_frame_sdd_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_nfct_sensres_bit_frame_sdd_t frame_sdd = nrf_nfct_sensres_bit_frame_sdd_get(p_reg);
    nrf_barrier_r();
    return frame_sdd;
}

/** @refhal{nrf_nfct_sensres_bit_frame_sdd_set} */
NRFY_STATIC_INLINE
void nrfy_nfct_sensres_bit_frame_sdd_set(NRF_NFCT_Type *                  p_reg,
                                         nrf_nfct_sensres_bit_frame_sdd_t bit_frame_sdd)
{
    nrf_nfct_sensres_bit_frame_sdd_set(p_reg, bit_frame_sdd);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_sensres_platform_config_get} */
NRFY_STATIC_INLINE nrf_nfct_sensres_platform_config_t
nrfy_nfct_sensres_platform_config_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_nfct_sensres_platform_config_t config = nrf_nfct_sensres_platform_config_get(p_reg);
    nrf_barrier_r();
    return config;
}

/** @refhal{nrf_nfct_sensres_platform_config_set} */
NRFY_STATIC_INLINE
void nrfy_nfct_sensres_platform_config_set(NRF_NFCT_Type *                    p_reg,
                                           nrf_nfct_sensres_platform_config_t platform_config)
{
    nrf_nfct_sensres_platform_config_set(p_reg, platform_config);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_selres_cascade_check} */
NRFY_STATIC_INLINE bool nrfy_nfct_selres_cascade_check(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_nfct_selres_cascade_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_nfct_selres_protocol_get} */
NRFY_STATIC_INLINE
nrf_nfct_selres_protocol_t nrfy_nfct_selres_protocol_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_nfct_selres_protocol_t protocol = nrf_nfct_selres_protocol_get(p_reg);
    nrf_barrier_r();
    return protocol;
}

/** @refhal{nrf_nfct_selres_protocol_set} */
NRFY_STATIC_INLINE void nrfy_nfct_selres_protocol_set(NRF_NFCT_Type *            p_reg,
                                                      nrf_nfct_selres_protocol_t sel_res_protocol)
{
    nrf_nfct_selres_protocol_set(p_reg, sel_res_protocol);
    nrf_barrier_w();
}

/** @refhal{nrf_nfct_selres_get} */
NRFY_STATIC_INLINE uint32_t nrfy_nfct_selres_get(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t selres = nrf_nfct_selres_get(p_reg);
    nrf_barrier_r();
    return selres;
}

/** @refhal{nrf_nfct_selres_set} */
NRFY_STATIC_INLINE void nrfy_nfct_selres_set(NRF_NFCT_Type * p_reg, uint32_t selres)
{
    nrf_nfct_selres_set(p_reg, selres);
    nrf_barrier_w();
}

#if NRFY_NFCT_HAS_PAD_CONFIG_REG
/** @refhal{nrf_nfct_pad_config_enable_check} */
NRFY_STATIC_INLINE bool nrfy_nfct_pad_config_enable_check(NRF_NFCT_Type const * p_reg)
{
    nrf_barrier_rw();
    bool enabled = nrf_nfct_pad_config_enable_check(p_reg);
    nrf_barrier_w();
    return enabled;
}

/** @refhal{nrf_nfct_pad_config_enable_set} */
NRFY_STATIC_INLINE void nrfy_nfct_pad_config_enable_set(NRF_NFCT_Type * p_reg, bool enable)
{
    nrf_nfct_pad_config_enable_set(p_reg, enable);
    nrf_barrier_w();
}
#endif // NRFY_NFCT_HAS_PAD_CONFIG_REG

#if NRFY_NFCT_HAS_BIAS_CONFIG_TRIM_REG
/** @refhal{nrf_nfct_bias_config_get} */
NRFY_STATIC_INLINE void nrfy_nfct_bias_config_get(NRF_NFCT_Type const *    p_reg,
                                                  nrf_nfct_bias_config_t * p_bias_config)
{
    nrf_barrier_rw();
    nrf_nfct_bias_config_get(p_reg, p_bias_config);
    nrf_barrier_r();
}

/** @refhal{nrf_nfct_bias_config_set} */
NRFY_STATIC_INLINE void nrfy_nfct_bias_config_set(NRF_NFCT_Type *                p_reg,
                                                  nrf_nfct_bias_config_t const * p_bias_config)
{
    nrf_nfct_bias_config_set(p_reg, p_bias_config);
    nrf_barrier_w();
}
#endif // NRFY_NFCT_HAS_BIAS_CONFIG_TRIM_REG

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_nfct_event_enabled_clear(NRF_NFCT_Type *  p_reg,
                                                                 uint32_t         mask,
                                                                 nrf_nfct_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_nfct_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE bool __nrfy_internal_nfct_event_handle(NRF_NFCT_Type  * p_reg,
                                                          uint32_t         mask,
                                                          nrf_nfct_event_t event,
                                                          uint32_t       * p_evt_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_nfct_event_check(p_reg, event))
    {
        nrf_nfct_event_clear(p_reg, event);
        if (p_evt_mask)
        {
            *p_evt_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }

    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_nfct_events_process(NRF_NFCT_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_READY, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_FIELDDETECTED, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_FIELDLOST, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_TXFRAMESTART, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_TXFRAMEEND, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_RXFRAMESTART, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_RXFRAMEEND, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_ERROR, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_RXERROR, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_ENDRX, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_ENDTX, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_AUTOCOLRESSTARTED, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_COLLISION, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_SELECTED, &evt_mask);
    __nrfy_internal_nfct_event_handle(p_reg, mask, NRF_NFCT_EVENT_STARTED, &evt_mask);

    if (evt_mask & NRFY_EVENT_TO_INT_BITMASK(NRF_NFCT_EVENT_RXFRAMEEND))
    {
        uint8_t *buffer = nrf_nfct_rxtx_buffer_get(p_reg);
        uint16_t bytes = (nrf_nfct_rx_bits_get(p_reg, true) >> 3U);

        nrf_barrier_rw();
        NRFY_CACHE_INV(buffer, bytes);
    }

    nrf_barrier_w();

    return evt_mask;
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_NFCT_H__
