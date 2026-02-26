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

#ifndef NRFX_I2S_H__
#define NRFX_I2S_H__

#include <nrfx.h>
#include <haly/nrfy_i2s.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_i2s I2S driver
 * @{
 * @ingroup nrf_i2s
 * @brief   Inter-IC Sound (I2S) peripheral driver.
 */

/** @brief I2S prescalers structure. */
typedef struct
{
    nrf_i2s_mck_t   mck_setup;     ///< Master clock generator setup.
    nrf_i2s_ratio_t ratio;         ///< MCK/LRCK ratio.
#if NRF_I2S_HAS_CLKCONFIG
    bool            enable_bypass; /**< Bypass clock generator.
                                    *   MCK will be equal to source input. */
#endif
} nrfx_i2s_prescalers_t;

/** @brief Parameters use to describe I2S clock prescaling. */
typedef struct
{
    uint32_t         base_clock_freq; ///< Freqency of the I2S base clock source.
    uint32_t         transfer_rate;   ///< Desired I2S data transfer rate.
    nrf_i2s_swidth_t swidth;          ///< I2S sample width.
    bool             allow_bypass;    ///< Bypass clock generator if possible.
} nrfx_i2s_clk_params_t;

/** @brief I2S driver configuration structure. */
typedef struct
{
    uint32_t              sck_pin;       ///< SCK pin number.
    uint32_t              lrck_pin;      ///< LRCK pin number.
    uint32_t              mck_pin;       ///< MCK pin number.
                                         /**< Optional. Use @ref NRF_I2S_PIN_NOT_CONNECTED
                                          *   if this signal is not needed. */
    uint32_t              sdout_pin;     ///< SDOUT pin number.
                                         /**< Optional. Use @ref NRF_I2S_PIN_NOT_CONNECTED
                                          *   if this signal is not needed. */
    uint32_t              sdin_pin;      ///< SDIN pin number.
                                         /**< Optional. Use @ref NRF_I2S_PIN_NOT_CONNECTED
                                          *   if this signal is not needed. */
    uint8_t               irq_priority;  ///< Interrupt priority.
    nrf_i2s_mode_t        mode;          ///< Mode of operation (master or slave).
    nrf_i2s_format_t      format;        ///< I2S frame format.
    nrf_i2s_align_t       alignment;     ///< Alignment of sample within a frame.
    nrf_i2s_swidth_t      sample_width;  ///< Sample width.
    nrf_i2s_channels_t    channels;      ///< Enabled channels.
    nrfx_i2s_prescalers_t prescalers;    ///< Clock prescalers.
#if NRF_I2S_HAS_CLKCONFIG
    nrf_i2s_clksrc_t      clksrc;        ///< Clock source selection.
#endif
    bool                  skip_gpio_cfg; ///< Skip GPIO configuration of pins.
                                         /**< When set to true, the driver does not modify
                                          *   any GPIO parameters of the used pins. Those
                                          *   parameters are supposed to be configured
                                          *   externally before the driver is initialized. */
    bool                  skip_psel_cfg; ///< Skip pin selection configuration.
                                         /**< When set to true, the driver does not modify
                                          *   pin select registers in the peripheral.
                                          *   Those registers are supposed to be set up
                                          *   externally before the driver is initialized.
                                          *   @note When both GPIO configuration and pin
                                          *   selection are to be skipped, the structure
                                          *   fields that specify pins can be omitted,
                                          *   as they are ignored anyway. */
} nrfx_i2s_config_t;

/** @brief I2S driver buffers structure. */
typedef nrfy_i2s_xfer_desc_t nrfx_i2s_buffers_t;

/**
 * @brief I2S driver default configuration.
 *
 * This configuration sets up I2S with the following options:
 * - master mode
 * - i2s data format
 * - left alignment
 * - sample width 16 bit
 * - left channel enabled
 * - MCK frequency 4 MHz
 * - LRCK frequency 125 kHz
 *
 * @param[in] _pin_sck   SCK pin number.
 * @param[in] _pin_lrck  LRCK pin number.
 * @param[in] _pin_mck   MCK pin number.
 * @param[in] _pin_sdout SDOUT pin number.
 * @param[in] _pin_sdin  SDIN pin number.
 */
#define NRFX_I2S_DEFAULT_CONFIG(_pin_sck, _pin_lrck, _pin_mck, _pin_sdout, _pin_sdin) \
{                                                                                     \
    .sck_pin      = _pin_sck,                                                         \
    .lrck_pin     = _pin_lrck,                                                        \
    .mck_pin      = _pin_mck,                                                         \
    .sdout_pin    = _pin_sdout,                                                       \
    .sdin_pin     = _pin_sdin,                                                        \
    .irq_priority = NRFX_I2S_DEFAULT_CONFIG_IRQ_PRIORITY,                             \
    .mode         = NRF_I2S_MODE_MASTER,                                              \
    .format       = NRF_I2S_FORMAT_I2S,                                               \
    .alignment    = NRF_I2S_ALIGN_LEFT,                                               \
    .sample_width = NRF_I2S_SWIDTH_16BIT,                                             \
    .channels     = NRF_I2S_CHANNELS_LEFT,                                            \
    .prescalers   =                                                                   \
    {                                                                                 \
        .mck_setup    = NRF_I2S_MCK_32MDIV8,                                          \
        .ratio        = NRF_I2S_RATIO_32X,                                            \
        NRFX_COND_CODE_1(NRF_I2S_HAS_CLKCONFIG,                                       \
                        (.enable_bypass = false),                                     \
                        ())                                                           \
    },                                                                                \
    NRFX_COND_CODE_1(NRF_I2S_HAS_CLKCONFIG,                                           \
                     (.clksrc = NRF_I2S_CLKSRC_PCLK32M),                              \
                     ())                                                              \
}

#define NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED (1UL << 0)
    /**< The application must provide buffers that are to be used in the next
     *   part of the transfer. A call to @ref nrfx_i2s_next_buffers_set must
     *   be done before the currently used buffers are completely processed
     *   (that is, the time remaining for supplying the next buffers depends on
     *   the used size of the buffers). */

#define NRFX_I2S_STATUS_TRANSFER_STOPPED    (1UL << 1)
    /**< The I2S peripheral has been stopped and all buffers that were passed
     *   to the driver have been released. */

/**
 * @brief I2S driver data handler type.
 *
 * A data handling function of this type must be specified during the initialization
 * of the driver. The driver will call this function when it finishes using
 * buffers passed to it by the application, and when it needs to be provided
 * with buffers for the next part of the transfer.
 *
 * @note The @c p_released pointer passed to this function is temporary and
 *       will be invalid after the function returns, hence it cannot be stored
 *       and used later. If needed, the pointed content (that is, buffers pointers)
 *       must be copied instead.
 *
 * @param[in] p_released  Pointer to a structure with pointers to buffers
 *                        passed previously to the driver that will no longer
 *                        be accessed by it (they can be now safely released or
 *                        used for another purpose, in particular for a next
 *                        part of the transfer).
 *                        This pointer will be NULL if the application did not
 *                        supply the buffers for the next part of the transfer
 *                        (via a call to @ref nrfx_i2s_next_buffers_set) since
 *                        the previous time the data handler signaled such need.
 *                        This means that data corruption occurred (the previous
 *                        buffers are used for the second time) and no buffers
 *                        can be released at the moment.
 *                        Both pointers in this structure are NULL when the
 *                        handler is called for the first time after a transfer
 *                        is started, because no data has been transferred yet
 *                        at this point. In all successive calls, the pointers
 *                        specify what has been sent (TX) and what has been
 *                        received (RX) in the part of the transfer that has
 *                        just been completed (provided that a given direction
 *                        is enabled, see @ref nrfx_i2s_start).
 *                        @note Since the peripheral is stopped asynchronously,
 *                              buffers that are released after the call to
 *                              @ref nrfx_i2s_stop are not used entirely.
 *                              In this case, only a part (if any) of the TX
 *                              buffer has been actually transmitted and only
 *                              a part (if any) of the RX buffer is filled with
 *                              received data.
 * @param[in] status  Bit field describing the current status of the transfer.
 *                    It can be 0 or a combination of the following flags:
 *                    - @ref NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED
 *                    - @ref NRFX_I2S_STATUS_TRANSFER_STOPPED
 */
typedef void (* nrfx_i2s_data_handler_t)(nrfx_i2s_buffers_t const * p_released,
                                         uint32_t                   status);

/** @cond Driver internal data. */
typedef struct
{
    nrfx_i2s_data_handler_t handler;
    nrfx_drv_state_t        state;

    bool use_rx         : 1;
    bool use_tx         : 1;
    bool rx_ready       : 1;
    bool tx_ready       : 1;
    bool buffers_needed : 1;
    bool buffers_reused : 1;
    bool skip_gpio_cfg  : 1;
    bool skip_psel_cfg  : 1;

    nrfx_i2s_buffers_t  next_buffers;
    nrfx_i2s_buffers_t  current_buffers;
} nrfx_i2s_control_block_t;
/** @endcond */

/** @brief I2S driver instance data structure. */
typedef struct
{
    NRF_I2S_Type *           p_reg; ///< Pointer to a structure with I2S instance registers.
    nrfx_i2s_control_block_t cb;    ///< Driver internal data.
} nrfx_i2s_t;

/** @brief Macro for creating an I2S driver instance. */
#define NRFX_I2S_INSTANCE(reg)    \
{                                 \
    .p_reg = (NRF_I2S_Type *)reg, \
    .cb = {0},                    \
}

/**
 * @brief Function for initializing the I2S driver.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_config   Pointer to the structure with the initial configuration.
 * @param[in] handler    Data handler provided by the user. Must not be NULL.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver is already initialized.
 * @retval -EINVAL   The requested combination of configuration
 *                   options is not allowed by the I2S peripheral.
 */
int nrfx_i2s_init(nrfx_i2s_t *              p_instance,
                  nrfx_i2s_config_t const * p_config,
                  nrfx_i2s_data_handler_t   handler);

/**
 * @brief Function for uninitializing the I2S driver.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_i2s_uninit(nrfx_i2s_t * p_instance);

/**
 * @brief Function for checking if the I2S driver instance is initialized.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  Instance is already initialized.
 * @retval false Instance is not initialized.
 */
bool nrfx_i2s_init_check(nrfx_i2s_t const * p_instance);

/**
 * @brief Function for starting the continuous I2S transfer.
 *
 * The I2S data transfer can be performed in one of three modes: RX (reception)
 * only, TX (transmission) only, or in both directions simultaneously.
 * The mode is selected by specifying a proper buffer for a given direction
 * in the call to this function or by passing NULL instead if this direction
 * is to be disabled.
 *
 * The length of the buffer (which is a common value for RX and TX if both
 * directions are enabled) is specified in 32-bit words. One 32-bit memory
 * word can either contain four 8-bit samples, two 16-bit samples, or one
 * right-aligned 24-bit sample sign-extended to a 32-bit value.
 * For a detailed memory mapping for different supported configurations,
 * see the Product Specification.
 *
 * @note Peripherals using EasyDMA (including I2S) require the transfer buffers
 *       to be placed in the Data RAM region. If this condition is not met,
 *       this function will fail with the error code -EACCES.
 *
 * @param[in] p_instance        Pointer to the driver instance structure.
 * @param[in] p_initial_buffers Pointer to a structure specifying the buffers
 *                              to be used in the initial part of the transfer
 *                              (buffers for all consecutive parts are provided
 *                              through the data handler).
 * @param[in] flags             Transfer options (0 for default settings).
 *                              Currently, no additional flags are available.
 *
 * @retval 0            The operation was successful.
 * @retval -EINPROGRESS Transfer was already started or the driver has not been initialized.
 * @retval -EACCES      The provided buffers are not placed in the Data RAM region.
 */
int nrfx_i2s_start(nrfx_i2s_t *               p_instance,
                   nrfx_i2s_buffers_t const * p_initial_buffers,
                   uint8_t                    flags);

/**
 * @brief Function for supplying the buffers to be used in the next part of
 *        the transfer.
 *
 * The application must call this function when the data handler receives
 * @ref NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED in the @c status parameter.
 * The call can be done immediately from the data handler function or later,
 * but it has to be done before the I2S peripheral finishes processing the
 * buffers supplied previously. Otherwise, data corruption will occur.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_buffers  Pointer to a structure specifying the buffers
 *                       to be used in the upcoming part of the transfer.
 *
 * @retval 0            If the operation was successful.
 * @retval -EINPROGRESS If the buffers were already supplied or
 *                      the peripheral is currently being stopped.
 * @retval -EACCES      The provided buffers are not placed in the Data RAM region.
 *
 * @sa nrfx_i2s_data_handler_t
 */
int nrfx_i2s_next_buffers_set(nrfx_i2s_t *               p_instance,
                              nrfx_i2s_buffers_t const * p_buffers);

/**
 * @brief Function for stopping the I2S transfer.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_i2s_stop(nrfx_i2s_t * p_instance);

/**
 * @brief Function for calculating I2S clock prescaler values.
 *
 * Call this function to find suitable value for prescalers in
 * @ref nrfx_i2s_config_t structure.
 *
 * @param[in]  clk_params Parameters used to describe clock prescaling.
 * @param[out] prescalers Prescaler structure pointer to be filled with prescaler values.
 *
 * @retval 0        Suitable prescaler values were found.
 * @retval -EINVAL  No suitable prescaler values were found.
 * @retval -ENOTSUP Requested configuration is not supported.
 */
int nrfx_i2s_prescalers_calc(nrfx_i2s_clk_params_t const * clk_params,
                             nrfx_i2s_prescalers_t *       prescalers);

/**
 * @brief Driver interrupt handler.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_i2s_irq_handler(nrfx_i2s_t * p_instance);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_I2S_H__
