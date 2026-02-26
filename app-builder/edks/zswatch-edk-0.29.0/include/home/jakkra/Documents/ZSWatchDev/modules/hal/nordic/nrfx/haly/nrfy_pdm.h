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

#ifndef NRFY_PDM_H__
#define NRFY_PDM_H__

#include <nrfx.h>
#include <hal/nrf_pdm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nrfy_pdm_buffer_t nrfy_pdm_buffer_t;

NRFY_STATIC_INLINE bool __nrfy_internal_pdm_event_handle(NRF_PDM_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_pdm_event_t event,
                                                         uint32_t *      p_event_mask);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_pdm_events_process(NRF_PDM_Type *            p_reg,
                                                               uint32_t                  mask,
                                                               nrfy_pdm_buffer_t const * p_buffer);

NRFY_STATIC_INLINE void __nrfy_internal_pdm_event_enabled_clear(NRF_PDM_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_pdm_event_t event);

/**
 * @defgroup nrfy_pdm PDM HALY
 * @{
 * @ingroup nrf_pdm
 * @brief   Hardware access layer with cache and barrier support for managing the PDM peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @brief Structure describing reception buffer.*/
struct nrfy_pdm_buffer_t
{
    int16_t *  p_buff; ///< Pointer to the data buffer.
    uint16_t   length; ///< Data buffer lenght.
};

/** @brief PDM pins configuration structure. */
typedef struct
{
    uint32_t clk_pin; ///< CLK pin number.
    uint32_t din_pin; ///< DIN pin number.
} nrfy_pdm_pins_t;

/** @brief PDM configuration structure. */
typedef struct
{
    nrf_pdm_mode_t    mode;          ///< Interface operation mode.
    nrf_pdm_edge_t    edge;          ///< Sampling mode.
    nrfy_pdm_pins_t   pins;          ///< Pin configuration structure.
#if NRF_PDM_HAS_PDMCLKCTRL
    nrf_pdm_freq_t    clock_freq;    ///< Clock frequency.
#elif NRF_PDM_HAS_PRESCALER
    uint32_t          prescaler;     ///< Prescaler divisior.
#endif
    nrf_pdm_gain_t    gain_l;        ///< Left channel gain.
    nrf_pdm_gain_t    gain_r;        ///< Right channel gain.
#if NRF_PDM_HAS_RATIO_CONFIG
    nrf_pdm_ratio_t   ratio;         ///< Ratio between PDM_CLK and output sample rate.
#endif
#if NRF_PDM_HAS_SELECTABLE_CLOCK
    nrf_pdm_mclksrc_t mclksrc;       ///< Clock source selection.
#endif
    bool              skip_psel_cfg; ///< Skip pin selection configuration.
                                     /**< When set to true, the driver does not modify
                                      *   pin select registers in the peripheral.
                                      *   Those registers are supposed to be set up
                                      *   externally before the driver is initialized.
                                      *   @note When both GPIO configuration and pin
                                      *   selection are to be skipped, the structure
                                      *   fields that specify pins can be omitted,
                                      *   as they are ignored anyway. */
} nrfy_pdm_config_t;

/**
 * @brief Function for configuring the PDM.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the peripheral configuration structure.
 */
NRFY_STATIC_INLINE void nrfy_pdm_periph_configure(NRF_PDM_Type *            p_reg,
                                                  nrfy_pdm_config_t const * p_config)
{
#if NRF_PDM_HAS_RATIO_CONFIG
    nrf_pdm_ratio_set(p_reg, p_config->ratio);
#endif

#if NRF_PDM_HAS_SELECTABLE_CLOCK
    nrf_pdm_mclksrc_configure(p_reg, p_config->mclksrc);
#endif
#if NRF_PDM_HAS_PDMCLKCTRL
    nrf_pdm_clock_set(p_reg, p_config->clock_freq);
#elif NRF_PDM_HAS_PRESCALER
    nrf_pdm_prescaler_set(p_reg, p_config->prescaler);
#endif
    nrf_pdm_mode_set(p_reg, p_config->mode, p_config->edge);
    nrf_pdm_gain_set(p_reg, p_config->gain_l, p_config->gain_r);
    if (!p_config->skip_psel_cfg)
    {
        nrf_pdm_psel_connect(p_reg, p_config->pins.clk_pin, p_config->pins.din_pin);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified PDM interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if the interrupts are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_pdm_int_init(NRF_PDM_Type * p_reg,
                                          uint32_t       mask,
                                          uint8_t        irq_priority,
                                          bool           enable)
{
    __nrfy_internal_pdm_event_enabled_clear(p_reg, mask, NRF_PDM_EVENT_STARTED);
    __nrfy_internal_pdm_event_enabled_clear(p_reg, mask, NRF_PDM_EVENT_END);
    __nrfy_internal_pdm_event_enabled_clear(p_reg, mask, NRF_PDM_EVENT_STOPPED);
    nrf_barrier_w();

    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_pdm_int_enable(p_reg, mask);
    }

    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the PDM interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_pdm_int_uninit(NRF_PDM_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified PDM events.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] mask     Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 * @param[in] p_buffer Pointer to the structure containing buffer associated with the last reception. Can be NULL.
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_pdm_events_process(NRF_PDM_Type *      p_reg,
                                                    uint32_t            mask,
                                                    nrfy_pdm_buffer_t * p_buffer)
{
    uint32_t evt_mask = __nrfy_internal_pdm_events_process(p_reg, mask, p_buffer);
    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for setting the PDM sampling buffer.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the structure containing reception buffer.
 */
NRFY_STATIC_INLINE void nrfy_pdm_buffer_set(NRF_PDM_Type *            p_reg,
                                            nrfy_pdm_buffer_t const * p_buffer)
{
    nrf_pdm_buffer_set(p_reg, (uint32_t *)(p_buffer->p_buff), p_buffer->length);
    nrf_barrier_w();
}

/**
 * @brief Function for starting the PDM sampling.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the structure containing reception buffer if the reception
 *                     is to be blocking. NULL for non-blocking receptions.
 */
NRFY_STATIC_INLINE void nrfy_pdm_start(NRF_PDM_Type *            p_reg,
                                       nrfy_pdm_buffer_t const * p_buffer)
{
    nrf_pdm_task_trigger(p_reg, NRF_PDM_TASK_START);
    if (p_buffer)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_PDM_EVENT_END);
        while (!__nrfy_internal_pdm_events_process(p_reg, evt_mask, p_buffer))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for aborting PDM transaction.
 *
 * @param[in] p_reg    Pointer to thr structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the structure containing reception buffer if the reception
 *                     is to be blocking. NULL for non-blocking receptions.
 */
NRFY_STATIC_INLINE void nrfy_pdm_abort(NRF_PDM_Type *            p_reg,
                                       nrfy_pdm_buffer_t const * p_buffer)
{
    nrf_pdm_task_trigger(p_reg, NRF_PDM_TASK_STOP);
    if (p_buffer)
    {
        nrf_barrier_w();
        uint32_t evt_mask = NRFY_EVENT_TO_INT_BITMASK(NRF_PDM_EVENT_STOPPED) |
                            NRFY_EVENT_TO_INT_BITMASK(NRF_PDM_EVENT_END);
        while (!__nrfy_internal_pdm_events_process(p_reg, evt_mask, p_buffer))
        {}
    }
    nrf_barrier_w();
}

/**
 * @brief Function for setting the PDM pins configuration.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_pins Pointer to the PDM pin configurartion structure.
 */
NRFY_STATIC_INLINE void nrfy_pdm_pins_set(NRF_PDM_Type * p_reg, nrfy_pdm_pins_t * p_pins)
{
    nrf_pdm_psel_connect(p_reg, p_pins->clk_pin, p_pins->din_pin);
    nrf_barrier_w();
}

/**
 * @brief Function for getting the PDM pins configuration.
 *
 * @param[in]  p_reg  Pointer to the structure of registers of the peripheral.
 * @param[out] p_pins Pointer to the structure to be filled with PDM pins configuration.
 */
NRFY_STATIC_INLINE void nrfy_pdm_pins_get(NRF_PDM_Type const * p_reg, nrfy_pdm_pins_t * p_pins)
{
    nrf_barrier_rw();
    p_pins->clk_pin = nrf_pdm_clk_pin_get(p_reg);
    p_pins->din_pin = nrf_pdm_din_pin_get(p_reg);
    nrf_barrier_r();
}

/** @refhal{nrf_pdm_task_trigger} */
NRFY_STATIC_INLINE void nrfy_pdm_task_trigger(NRF_PDM_Type * p_reg,
                                              nrf_pdm_task_t task)
{
    nrf_pdm_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_pdm_task_address_get(NRF_PDM_Type const * p_reg,
                                                      nrf_pdm_task_t       task)
{
    return nrf_pdm_task_address_get(p_reg, task);
}

/** @refhal{nrf_pdm_event_check} */
NRFY_STATIC_INLINE bool nrfy_pdm_event_check(NRF_PDM_Type const * p_reg,
                                             nrf_pdm_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_pdm_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_pdm_event_clear} */
NRFY_STATIC_INLINE void nrfy_pdm_event_clear(NRF_PDM_Type *  p_reg,
                                             nrf_pdm_event_t event)
{
    nrf_pdm_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_pdm_event_address_get(NRF_PDM_Type const * p_reg,
                                                       nrf_pdm_event_t      event)
{
    return nrf_pdm_event_address_get(p_reg, event);
}

/** @refhal{nrf_pdm_int_enable} */
NRFY_STATIC_INLINE void nrfy_pdm_int_enable(NRF_PDM_Type * p_reg,
                                            uint32_t       mask)
{
    nrf_pdm_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_pdm_int_enable_check(NRF_PDM_Type const * p_reg,
                                                      uint32_t             mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_pdm_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_pdm_int_disable} */
NRFY_STATIC_INLINE void nrfy_pdm_int_disable(NRF_PDM_Type * p_reg,
                                             uint32_t       mask)
{
    nrf_pdm_int_disable(p_reg, mask);
    nrf_barrier_w();
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_pdm_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_pdm_subscribe_set(NRF_PDM_Type * p_reg,
                                               nrf_pdm_task_t task,
                                               uint8_t        channel)
{
    nrf_pdm_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_pdm_subscribe_clear(NRF_PDM_Type * p_reg,
                                                 nrf_pdm_task_t task)
{
    nrf_pdm_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_publish_set} */
NRFY_STATIC_INLINE void nrfy_pdm_publish_set(NRF_PDM_Type *  p_reg,
                                             nrf_pdm_event_t event,
                                             uint8_t         channel)
{
    nrf_pdm_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_publish_clear} */
NRFY_STATIC_INLINE void nrfy_pdm_publish_clear(NRF_PDM_Type *  p_reg,
                                               nrf_pdm_event_t event)
{
    nrf_pdm_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif

/** @refhal{nrf_pdm_enable} */
NRFY_STATIC_INLINE void nrfy_pdm_enable(NRF_PDM_Type * p_reg)
{
    nrf_pdm_enable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_disable} */
NRFY_STATIC_INLINE void nrfy_pdm_disable(NRF_PDM_Type * p_reg)
{
    nrf_pdm_disable(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_enable_check} */
NRFY_STATIC_INLINE bool nrfy_pdm_enable_check(NRF_PDM_Type const * p_reg)
{
    nrf_barrier_rw();
    bool check = nrf_pdm_enable_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_pdm_mode_set} */
NRFY_STATIC_INLINE void nrfy_pdm_mode_set(NRF_PDM_Type * p_reg,
                                          nrf_pdm_mode_t pdm_mode,
                                          nrf_pdm_edge_t pdm_edge)
{
    nrf_pdm_mode_set(p_reg, pdm_mode, pdm_edge);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_mode_get} */
NRFY_STATIC_INLINE void nrfy_pdm_mode_get(NRF_PDM_Type const * p_reg,
                                          nrf_pdm_mode_t *     p_pdm_mode,
                                          nrf_pdm_edge_t *     p_pdm_edge)
{
    nrf_barrier_rw();
    nrf_pdm_mode_get(p_reg, p_pdm_mode, p_pdm_edge);
    nrf_barrier_r();
}

#if NRF_PDM_HAS_PDMCLKCTRL
/** @refhal{nrf_pdm_clock_set} */
NRFY_STATIC_INLINE void nrfy_pdm_clock_set(NRF_PDM_Type * p_reg,
                                           nrf_pdm_freq_t pdm_freq)
{
    nrf_pdm_clock_set(p_reg, pdm_freq);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_clock_get} */
NRFY_STATIC_INLINE nrf_pdm_freq_t nrfy_pdm_clock_get(NRF_PDM_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_pdm_freq_t clock = nrf_pdm_clock_get(p_reg);
    nrf_barrier_r();
    return clock;
}
#endif

#if NRF_PDM_HAS_PRESCALER
/** @refhal{nrf_pdm_prescaler_set} */
NRFY_STATIC_INLINE void nrfy_pdm_prescaler_set(NRF_PDM_Type * p_reg, uint32_t prescaler)
{
    nrf_pdm_prescaler_set(p_reg, prescaler);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_prescaler_get} */
NRFY_STATIC_INLINE uint32_t nrfy_pdm_prescaler_get(NRF_PDM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t prescaler = nrf_pdm_prescaler_get(p_reg);
    nrf_barrier_r();
    return prescaler;
}
#endif

/** @refhal{nrf_pdm_psel_disconnect} */
NRFY_STATIC_INLINE void nrfy_pdm_pin_disconnect(NRF_PDM_Type * p_reg)
{
    nrf_pdm_psel_disconnect(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_gain_set} */
NRFY_STATIC_INLINE void nrfy_pdm_gain_set(NRF_PDM_Type * p_reg,
                                          nrf_pdm_gain_t gain_l,
                                          nrf_pdm_gain_t gain_r)
{
    nrf_pdm_gain_set(p_reg, gain_l, gain_r);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_gain_get} */
NRFY_STATIC_INLINE void nrfy_pdm_gain_get(NRF_PDM_Type const * p_reg,
                                          nrf_pdm_gain_t *     p_gain_l,
                                          nrf_pdm_gain_t *     p_gain_r)
{
    nrf_barrier_rw();
    nrf_pdm_gain_get(p_reg, p_gain_l, p_gain_r);
    nrf_barrier_r();
}

/** @refhal{nrf_pdm_buffer_get} */
NRFY_STATIC_INLINE uint32_t * nrfy_pdm_buffer_get(NRF_PDM_Type const * p_reg)
{
    nrf_barrier_rw();
    uint32_t * p_buffer = nrf_pdm_buffer_get(p_reg);
    nrf_barrier_r();
    return p_buffer;
}

#if NRF_PDM_HAS_RATIO_CONFIG
/** @refhal{nrf_pdm_ratio_set} */
NRFY_STATIC_INLINE void nrfy_pdm_ratio_set(NRF_PDM_Type *  p_reg,
                                           nrf_pdm_ratio_t ratio)
{
    nrf_pdm_ratio_set(p_reg, ratio);
    nrf_barrier_w();
}
#if NRF_PDM_HAS_CUSTOM_RATIO
/**
 * @brief Function for setting custom ratio between PDM_CLK and output sample rate.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] ratio Custom ratio between PDM_CLK and output sample rate.
 */
NRFY_STATIC_INLINE void nrfy_pdm_custom_ratio_set(NRF_PDM_Type *  p_reg,
                                                  uint8_t         ratio)
{
    NRFX_ASSERT(ratio % 2 == 0);
    nrf_pdm_custom_ratio_set(p_reg, (ratio / 2) - 1);
    nrf_barrier_w();
    nrf_pdm_ratio_set(p_reg, NRF_PDM_RATIO_CUSTOM);
    nrf_barrier_w();
}
#endif
#endif // NRF_PDM_HAS_RATIO_CONFIG

#if NRF_PDM_HAS_SELECTABLE_CLOCK
/** @refhal{nrf_pdm_mclksrc_configure} */
NRFY_STATIC_INLINE void nrfy_pdm_mclksrc_configure(NRF_PDM_Type *    p_reg,
                                                   nrf_pdm_mclksrc_t mclksrc)
{
    nrf_pdm_mclksrc_configure(p_reg, mclksrc);
    nrf_barrier_w();
}
#endif

#if NRF_PDM_HAS_FILTER
/** @refhal{nrf_pdm_filter_ctrl_set} */
NRFY_STATIC_INLINE
void nrfy_pdm_filter_ctrl_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_ctrl_t const * fctrl)
{
    nrf_pdm_filter_ctrl_set(p_reg, fctrl);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_filter_ctrl_get} */
NRFY_STATIC_INLINE
void nrfy_pdm_filter_ctrl_get(NRF_PDM_Type const * p_reg, nrf_pdm_filter_ctrl_t * fctrl)
{
    nrf_barrier_rw();
    nrf_pdm_filter_ctrl_get(p_reg, fctrl);
    nrf_barrier_r();
}

/** @refhal{nrf_pdm_filter_hp_pole_enable} */
NRFY_STATIC_INLINE
void nrfy_pdm_filter_hp_pole_enable(NRF_PDM_Type * p_reg, bool enable)
{
    nrf_pdm_filter_hp_pole_enable(p_reg, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_filter_hp_pole_set} */
NRFY_STATIC_INLINE
void nrfy_pdm_filter_hp_pole_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_hp_pole_t hppole)
{
    nrf_pdm_filter_hp_pole_set(p_reg, hppole);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_filter_hp_pole_get} */
NRFY_STATIC_INLINE nrf_pdm_filter_hp_pole_t nrfy_pdm_filter_hp_pole_get(NRF_PDM_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_pdm_filter_hp_pole_t hppole = nrf_pdm_filter_hp_pole_get(p_reg);
    nrf_barrier_r();
    return hppole;
}

/** @refhal{nrf_pdm_filter_softmute_enable} */
NRFY_STATIC_INLINE void nrfy_pdm_filter_softmute_enable(NRF_PDM_Type * p_reg, bool enable)
{
    nrf_pdm_filter_softmute_enable(p_reg, enable);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_filter_softcycles_set} */
NRFY_STATIC_INLINE
void nrfy_pdm_filter_softcycles_set(NRF_PDM_Type * p_reg, nrf_pdm_filter_softcycles_t cycles)
{
    nrf_pdm_filter_softcycles_set(p_reg, cycles);
    nrf_barrier_w();
}

/** @refhal{nrf_pdm_filter_softcycles_get} */
NRFY_STATIC_INLINE
nrf_pdm_filter_softcycles_t nrfy_pdm_filter_softcycles_get(NRF_PDM_Type const * p_reg)
{
    nrf_barrier_rw();
    nrf_pdm_filter_softcycles_t cycles = nrf_pdm_filter_softcycles_get(p_reg);
    nrf_barrier_r();
    return cycles;
}

/** @refhal{nrf_pdm_filter_sampledelay_enable} */
NRFY_STATIC_INLINE void nrfy_pdm_filter_sampledelay_enable(NRF_PDM_Type * p_reg, bool enable)
{
    nrf_pdm_filter_sampledelay_enable(p_reg, enable);
    nrf_barrier_w();
}
#endif

/** @} */

NRFY_STATIC_INLINE bool __nrfy_internal_pdm_event_handle(NRF_PDM_Type *  p_reg,
                                                         uint32_t        mask,
                                                         nrf_pdm_event_t event,
                                                         uint32_t *      p_event_mask)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_pdm_event_check(p_reg, event))
    {
        nrf_pdm_event_clear(p_reg, event);
        if (p_event_mask)
        {
            *p_event_mask |= NRFY_EVENT_TO_INT_BITMASK(event);
        }
        return true;
    }
    return false;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_pdm_events_process(NRF_PDM_Type *            p_reg,
                                                               uint32_t                  mask,
                                                               nrfy_pdm_buffer_t const * p_buffer)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    (void)__nrfy_internal_pdm_event_handle(p_reg, mask, NRF_PDM_EVENT_STARTED, &evt_mask);

    if (__nrfy_internal_pdm_event_handle(p_reg, mask, NRF_PDM_EVENT_STOPPED, &evt_mask) &&
        p_buffer->p_buff)
    {
        NRFY_CACHE_INV(p_buffer->p_buff, p_buffer->length);
    }

    (void)__nrfy_internal_pdm_event_handle(p_reg, mask, NRF_PDM_EVENT_END, &evt_mask);
    return evt_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_pdm_event_enabled_clear(NRF_PDM_Type *  p_reg,
                                                                uint32_t        mask,
                                                                nrf_pdm_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_pdm_event_clear(p_reg, event);
    }
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_PDM_H__
