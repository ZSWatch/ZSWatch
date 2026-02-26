/*
 * Copyright (c) 2015 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_PDM_H__
#define NRFX_PDM_H__

#include <nrfx.h>
#include <haly/nrfy_pdm.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_pdm PDM driver
 * @{
 * @ingroup nrf_pdm
 * @brief   Pulse Density Modulation (PDM) peripheral driver.
 */

/** @brief Maximum supported PDM buffer size. */
#define NRFX_PDM_MAX_BUFFER_SIZE 32767

/** @brief PDM error type. */
typedef enum
{
    NRFX_PDM_NO_ERROR = 0,      ///< No error.
    NRFX_PDM_ERROR_OVERFLOW = 1 ///< Overflow error.
} nrfx_pdm_error_t;

/** @brief PDM event structure. */
typedef struct
{
    bool             buffer_requested; ///< Buffer request flag.
    int16_t *        buffer_released;  ///< Pointer to the released buffer. Can be NULL.
    nrfx_pdm_error_t error;            ///< Error type.
} nrfx_pdm_evt_t;

/** @brief PDM prescalers structure. */
typedef struct
{
#if NRF_PDM_HAS_PDMCLKCTRL
    nrf_pdm_freq_t    clock_freq;         ///< Selectable clock frequency.
#elif NRF_PDM_HAS_PRESCALER
    uint32_t          prescaler;          ///< Prescaler divisor.
#endif
#if NRF_PDM_HAS_RATIO_CONFIG
    nrf_pdm_ratio_t   ratio;              ///< Ratio between PDM_CLK and output sample rate.
#endif
} nrfx_pdm_prescalers_t;


/** @brief PDM interface driver configuration structure. */
typedef struct
{
    nrf_pdm_mode_t        mode;               ///< Interface operation mode.
    nrf_pdm_edge_t        edge;               ///< Sampling mode.
    uint32_t              clk_pin;            ///< CLK pin number.
    uint32_t              din_pin;            ///< DIN pin number.
    nrf_pdm_gain_t        gain_l;             ///< Left channel gain.
    nrf_pdm_gain_t        gain_r;             ///< Right channel gain.
    uint8_t               interrupt_priority; ///< Interrupt priority.
    nrfx_pdm_prescalers_t prescalers;         ///< Clock prescalers.
#if NRF_PDM_HAS_SELECTABLE_CLOCK
    nrf_pdm_mclksrc_t     mclksrc;            ///< Clock source selection.
#endif
    bool                  skip_gpio_cfg;      ///< Skip GPIO configuration of pins.
                                              /**< When set to true, the driver does not modify
                                               *   any GPIO parameters of the used pins. Those
                                               *   parameters are supposed to be configured
                                               *   externally before the driver is initialized. */
    bool                  skip_psel_cfg;      ///< Skip pin selection configuration.
                                              /**< When set to true, the driver does not modify
                                               *   pin select registers in the peripheral.
                                               *   Those registers are supposed to be set up
                                               *   externally before the driver is initialized.
                                               *   @note When both GPIO configuration and pin
                                               *   selection are to be skipped, the structure
                                               *   fields that specify pins can be omitted,
                                               *   as they are ignored anyway. */
} nrfx_pdm_config_t;

/**
 * @brief PDM driver default configuration.
 *
 * This configuration sets up PDM with the following options:
 * - mono mode
 * - data sampled on the clock falling edge
 * - frequency: 1.032 MHz
 * - standard gain
 *
 * @param[in] _pin_clk CLK output pin.
 * @param[in] _pin_din DIN input pin.
 */
#define NRFX_PDM_DEFAULT_CONFIG(_pin_clk, _pin_din)             \
{                                                               \
    .mode               = NRF_PDM_MODE_MONO,                    \
    .edge               = NRF_PDM_EDGE_LEFTFALLING,             \
    .clk_pin            = _pin_clk,                             \
    .din_pin            = _pin_din,                             \
    .gain_l             = NRF_PDM_GAIN_DEFAULT,                 \
    .gain_r             = NRF_PDM_GAIN_DEFAULT,                 \
    .interrupt_priority = NRFX_PDM_DEFAULT_CONFIG_IRQ_PRIORITY, \
    .prescalers         =                                       \
    {                                                           \
        NRFX_COND_CODE_1(NRF_PDM_HAS_PDMCLKCTRL,                \
                     (.clock_freq = NRF_PDM_FREQ_1032K,), ())   \
        NRFX_COND_CODE_1(NRF_PDM_HAS_PRESCALER,                 \
                     (.prescaler = 4,), ())                     \
        NRFX_COND_CODE_1(NRF_PDM_HAS_RATIO_CONFIG,              \
                     (.ratio = NRF_PDM_RATIO_64X,), ())         \
    },                                                          \
    NRFX_COND_CODE_1(NRF_PDM_HAS_SELECTABLE_CLOCK,              \
                     (.mclksrc = NRF_PDM_MCLKSRC_PCLK32M,), ()) \
}

/** @brief PDM output frequency and sampling rate values. */
typedef struct
{
    uint32_t base_clock_freq; ///< Freqency of the PDM base clock source.
    uint32_t sampling_rate;   ///< Desired PDM sampling rate.
    uint32_t output_freq_min; ///< Minimal value of PDM output frequency.
    uint32_t output_freq_max; ///< Maximum value of PDM output frequency.
} nrfx_pdm_output_t;

/**
 * @brief Handler for the PDM interface ready events.
 *
 * This event handler is called on a buffer request, an error or when a buffer
 * is full and ready to be processed.
 *
 * @param[in] p_evt Pointer to the PDM event structure.
 */
typedef void (*nrfx_pdm_event_handler_t)(nrfx_pdm_evt_t const * p_evt);

/** @cond Driver internal data. */
typedef enum
{
    NRFX_PDM_STATE_IDLE,
    NRFX_PDM_STATE_RUNNING,
    NRFX_PDM_STATE_STARTING,
    NRFX_PDM_STATE_STOPPING
} nrfx_pdm_state_t;

typedef struct
{
    nrfx_pdm_event_handler_t  event_handler;    ///< Event handler function pointer.
    int16_t *                 buff_address[2];  ///< Sample buffers.
    uint16_t                  buff_length[2];   ///< Length of the sample buffers.
    nrfx_drv_state_t          drv_state;        ///< Driver state.
    volatile nrfx_pdm_state_t op_state;         ///< PDM peripheral operation state.
    uint8_t                   active_buffer;    ///< Number of currently active buffer.
    uint8_t                   error;            ///< Driver error flag.
    volatile uint8_t          irq_buff_request; ///< Request the next buffer in the ISR.
    bool                      skip_gpio_cfg;    ///< Do not touch GPIO configuration of used pins.
} nrfx_pdm_control_block_t;
/** @endcond */

/** @brief Data structure of the Pulse Density Modulation (PDM) driver instance. */
typedef struct
{
    NRF_PDM_Type *           p_reg; ///< Pointer to a structure with PDM instance registers.
    nrfx_pdm_control_block_t cb;    ///< Driver internal data.
} nrfx_pdm_t;

/** @brief Macro for creating an instance of the PDM driver. */
#define NRFX_PDM_INSTANCE(reg)    \
{                                 \
    .p_reg = (NRF_PDM_Type *)reg, \
    .cb = {0}                     \
}

/**
 * @brief Function for initializing the PDM driver instance.
 *
 * @param[in] p_instance    Pointer to the driver instance structure.
 * @param[in] p_config      Pointer to the structure with the initial configuration.
 * @param[in] event_handler Event handler provided by the user. Cannot be NULL.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver is already initialized.
 * @retval -EINVAL   Invalid configuration was specified.
 */
int nrfx_pdm_init(nrfx_pdm_t *              p_instance,
                  nrfx_pdm_config_t const * p_config,
                  nrfx_pdm_event_handler_t  event_handler);

/**
 * @brief Function for reconfiguring the PDM driver instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_config   Pointer to the structure with the configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EBUSY       There is ongoing sampling and driver cannot be reconfigured.
 * @retval -EINPROGRESS The driver is not initialized.
 * @retval -EINVAL      Invalid configuration was specified.
 */
int nrfx_pdm_reconfigure(nrfx_pdm_t *              p_instance,
                         nrfx_pdm_config_t const * p_config);

/**
 * @brief Function for uninitializing the PDM driver instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * This function stops PDM sampling, if it is in progress.
 */
void nrfx_pdm_uninit(nrfx_pdm_t * p_instance);

/**
 * @brief Function for checking if the PDM driver instance is initialized.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  Interface is already initialized.
 * @retval false Interface is not initialized.
 */
bool nrfx_pdm_init_check(nrfx_pdm_t const * p_instance);

/**
 * @brief Function for getting the address of a PDM driver instance task.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] task Task.
 *
 * @return Task address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_pdm_task_address_get(nrfx_pdm_t const * p_instance,
                                                      nrf_pdm_task_t     task);

/**
 * @brief Function for getting the state of the PDM drivers instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  The PDM interface is enabled.
 * @retval false The PDM interface is disabled.
 */
NRFX_STATIC_INLINE bool nrfx_pdm_enable_check(nrfx_pdm_t const * p_instance);

/**
 * @brief Function for starting the PDM sampling.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval 0      Sampling was started successfully or was already in progress.
 * @retval -EBUSY Previous start/stop operation is in progress.
 */
int nrfx_pdm_start(nrfx_pdm_t * p_instance);

/**
 * @brief Function for stopping the PDM sampling.
 *
 * When this function is called, the PDM driver instance is stopped after finishing
 * the current frame.
 * The event handler function might be called once more after calling this function.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval 0      Sampling was stopped successfully or was already stopped before.
 * @retval -EBUSY Previous start/stop operation is in progress.
 */
int nrfx_pdm_stop(nrfx_pdm_t * p_instance);

/**
 * @brief Function for supplying the sample buffer.
 *
 * Call this function after every buffer request event.
 *
 * @param[in] p_instance    Pointer to the driver instance structure.
 * @param[in] buffer        Pointer to the receive buffer. Cannot be NULL.
 * @param[in] buffer_length Length of the receive buffer in 16-bit words.
 *
 * @retval 0            The buffer was applied successfully.
 * @retval -EBUSY       The buffer was already supplied or the peripheral is currently being stopped.
 * @retval -EINPROGRESS The driver was not initialized.
 * @retval -EINVAL      Invalid parameters were provided.
 */
int nrfx_pdm_buffer_set(nrfx_pdm_t * p_instance,
                        int16_t *    buffer,
                        uint16_t     buffer_length);

/**
 * @brief Function for calculating PDM clock prescaler values.
 *
 * Call this function to find suitable value for prescalers in
 * @ref nrfx_pdm_config_t structure.
 *
 * @param[in]  output_config Expected output frequencies.
 * @param[out] prescalers    Prescaler structure pointer to be filled with prescaler values.
 *
 * @retval 0       Suitable prescaler values were found.
 * @retval -EINVAL No suitable prescaler values were found.
 */
int nrfx_pdm_prescalers_calc(nrfx_pdm_output_t const * output_config,
                             nrfx_pdm_prescalers_t *   prescalers);

#ifndef NRFX_DECLARE_ONLY
NRFX_STATIC_INLINE uint32_t nrfx_pdm_task_address_get(nrfx_pdm_t const * p_instance,
                                                      nrf_pdm_task_t     task)
{
    return nrf_pdm_task_address_get(p_instance->p_reg, task);
}

NRFX_STATIC_INLINE bool nrfx_pdm_enable_check(nrfx_pdm_t const * p_instance)
{
    return nrf_pdm_enable_check(p_instance->p_reg);
}
#endif // NRFX_DECLARE_ONLY

/**
 * @brief Driver interrupt handler.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_pdm_irq_handler(nrfx_pdm_t * p_instance);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_PDM_H__
