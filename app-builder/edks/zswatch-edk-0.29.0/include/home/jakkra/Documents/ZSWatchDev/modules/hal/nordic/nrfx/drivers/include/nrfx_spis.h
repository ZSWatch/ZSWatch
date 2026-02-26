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

#ifndef NRFX_SPIS_H__
#define NRFX_SPIS_H__

#include <nrfx.h>
#include <hal/nrf_spis.h>
#include <haly/nrfy_gpio.h>
#if NRF_ERRATA_STATIC_CHECK(52, 109)
#include <nrfx_gpiote.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_spis SPIS driver
 * @{
 * @ingroup nrf_spis
 * @brief   Serial Peripheral Interface Slave with EasyDMA (SPIS) driver.
 */

/** @brief SPI slave driver event types. */
typedef enum
{
    NRFX_SPIS_BUFFERS_SET_DONE, //!< Memory buffer set event. Memory buffers have been set successfully to the SPI slave device, and SPI transaction can be done.
    NRFX_SPIS_XFER_DONE,        //!< SPI transaction event. SPI transaction has been completed.
    NRFX_SPIS_EVT_TYPE_MAX      //!< Enumeration upper bound.
} nrfx_spis_event_type_t;

/** @brief SPI slave driver event structure. */
typedef struct
{
    nrfx_spis_event_type_t evt_type;    //!< Type of the event.
    size_t                 rx_amount;   //!< Number of bytes received in the last transaction. This parameter is only valid for @ref NRFX_SPIS_XFER_DONE events.
    size_t                 tx_amount;   //!< Number of bytes transmitted in the last transaction. This parameter is only valid for @ref NRFX_SPIS_XFER_DONE events.
    void *                 p_tx_buf;    //!< Pointer to the TX buffer used in the last transaction. This parameter is only valid for @ref NRFX_SPIS_XFER_DONE events.
    void *                 p_rx_buf;    //!< Pointer to the RX buffer used in the last transaction. This parameter is only valid for @ref NRFX_SPIS_XFER_DONE events.
    size_t                 tx_buf_size; //!< Size of the TX buffer used int the last transaction. This parameter is only valid for @ref NRFX_SPIS_XFER_DONE events.
    size_t                 rx_buf_size; //!< Size of the RX buffer used int the last transaction. This parameter is only valid for @ref NRFX_SPIS_XFER_DONE events.
} nrfx_spis_event_t;

/**
 * @brief SPI slave driver event handler type.
 *
 * @param[in] p_event   Pointer to the event structure. The structure is
 *                      allocated on the stack so it is valid only until
 *                      the event handler returns.
 * @param[in] p_context Context set on initialization.
 */
typedef void (*nrfx_spis_event_handler_t)(nrfx_spis_event_t const * p_event,
                                          void *                    p_context);

/** @cond Driver internal data. */
typedef enum
{
    SPIS_STATE_INIT,
    SPIS_BUFFER_RESOURCE_REQUESTED,
    SPIS_BUFFER_RESOURCE_CONFIGURED,
    SPIS_XFER_COMPLETED
} nrfx_spis_state_t;

typedef struct
{
    volatile uint32_t          tx_buffer_size;
    volatile uint32_t          rx_buffer_size;
    nrfx_spis_event_handler_t  handler;
    volatile const uint8_t *   tx_buffer;
    volatile uint8_t *         rx_buffer;
    void *                     p_context;
    nrfx_drv_state_t           state;
    volatile nrfx_spis_state_t spi_state;
    bool                       skip_gpio_cfg;
#if NRF_ERRATA_STATIC_CHECK(52, 109)
    uint8_t                    gpiote_ch;
    nrfx_gpiote_t *            p_gpiote_inst;
    uint32_t                   csn_pin;
#endif
} nrfx_spis_control_block_t;
/** @endcond */

/** @brief Data structure for the Serial Peripheral Interface Slave with EasyDMA (SPIS) driver instance. */
typedef struct
{
    NRF_SPIS_Type *           p_reg; //!< Pointer to the structure of registers of the peripheral.
    nrfx_spis_control_block_t cb;    //!< Driver internal data.
} nrfx_spis_t;

/** @brief Macro for creating an instance of the SPI slave driver. */
#define NRFX_SPIS_INSTANCE(reg)    \
{                                  \
    .p_reg = (NRF_SPIS_Type *)reg, \
    .cb    = {0},                  \
}

/**
 * @brief SPIS driver default configuration.
 *
 * This configuration sets up SPIS with the following options:
 * - mode: 0 (SCK active high, sample on leading edge of the clock signal)
 * - MSB shifted out first
 * - CSN pull-up disabled
 * - MISO pin drive set to standard '0' and standard '1'
 * - default character set to 0xFF
 * - over-read character set to 0xFE
 *
 * @param[in] _pin_sck  SCK pin.
 * @param[in] _pin_mosi MOSI pin.
 * @param[in] _pin_miso MISO pin.
 * @param[in] _pin_csn  CSN pin.
 */
#define NRFX_SPIS_DEFAULT_CONFIG(_pin_sck, _pin_mosi, _pin_miso, _pin_csn)   \
{                                                                            \
    .miso_pin      = _pin_miso,                                              \
    .mosi_pin      = _pin_mosi,                                              \
    .sck_pin       = _pin_sck,                                               \
    .csn_pin       = _pin_csn,                                               \
    .mode          = NRF_SPIS_MODE_0,                                        \
    .bit_order     = NRF_SPIS_BIT_ORDER_MSB_FIRST,                           \
    .csn_pullup    = NRF_GPIO_PIN_NOPULL,                                    \
    .miso_drive    = NRF_GPIO_PIN_S0S1,                                      \
    .def           = 0xFF,                                                   \
    .orc           = 0xFE,                                                   \
    .irq_priority  = NRFX_SPIS_DEFAULT_CONFIG_IRQ_PRIORITY,                  \
    .skip_gpio_cfg = false,                                                  \
    .skip_psel_cfg = false,                                                  \
}

/** @brief SPI peripheral device configuration data. */
typedef struct
{
    uint32_t             miso_pin;      ///< SPI MISO pin (optional).
                                        /**< Set @ref NRF_SPIS_PIN_NOT_CONNECTED
                                         *   if this signal is not needed. */
    uint32_t             mosi_pin;      ///< SPI MOSI pin (optional).
                                        /**< Set @ref NRF_SPIS_PIN_NOT_CONNECTED
                                         *   if this signal is not needed. */
    uint32_t             sck_pin;       ///< SPI SCK pin.
    uint32_t             csn_pin;       ///< SPI CSN pin.
    nrf_spis_mode_t      mode;          ///< SPI mode.
    nrf_spis_bit_order_t bit_order;     ///< SPI transaction bit order.
    nrf_gpio_pin_pull_t  csn_pullup;    ///< CSN pin pull-up configuration.
    nrf_gpio_pin_drive_t miso_drive;    ///< MISO pin drive configuration.
    uint8_t              def;           ///< Character clocked out in case of an ignored transaction.
    uint8_t              orc;           ///< Character clocked out after an over-read of the transmit buffer.
    uint8_t              irq_priority;  ///< Interrupt priority.
    bool                 skip_gpio_cfg; ///< Skip GPIO configuration of pins.
                                        /**< When set to true, the driver does not modify
                                         *   any GPIO parameters of the used pins. Those
                                         *   parameters are supposed to be configured
                                         *   externally before the driver is initialized. */
    bool                 skip_psel_cfg; ///< Skip pin selection configuration.
                                        /**< When set to true, the driver does not modify
                                         *   pin select registers in the peripheral.
                                         *   Those registers are supposed to be set up
                                         *   externally before the driver is initialized.
                                         *   @note When both GPIO configuration and pin
                                         *   selection are to be skipped, the structure
                                         *   fields that specify pins can be omitted,
                                         *   as they are ignored anyway. */
} nrfx_spis_config_t;

/**
 * @brief Function for initializing the SPI slave driver instance.
 *
 * @note When the nRF52 Anomaly 109 workaround for SPIS is enabled, this function
 *       initializes the GPIOTE driver instance provided via @ref nrfx_spis_nrf52_anomaly_109_init
 *       as well, and uses one of GPIOTE channels to detect falling edges on CSN pin.
 *
 * @param[in] p_instance    Pointer to the driver instance structure.
 * @param[in] p_config      Pointer to the structure with the initial configuration.
 * @param[in] event_handler Function to be called by the SPI slave driver upon event.
 *                          Must not be NULL.
 * @param[in] p_context     Context passed to the event handler.
 *
 * @retval 0          The initialization was successful.
 * @retval -EALREADY  The driver is already initialized.
 * @retval -EINVAL    Invalid parameter is supplied.
 * @retval -EBUSY     Some other peripheral with the same instance ID is already in use.
 *                    This is possible only if @ref nrfx_prs module is enabled.
 * @retval -ECANCELED GPIOTE channel for detecting falling edges on CSN pin cannot
 *                    be initialized. Possible only when using nRF52 Anomaly 109 workaround.
 */
int nrfx_spis_init(nrfx_spis_t *              p_instance,
                   nrfx_spis_config_t const * p_config,
                   nrfx_spis_event_handler_t  event_handler,
                   void *                     p_context);

/**
 * @brief Function for reconfiguring the SPI slave driver instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_config   Pointer to the structure with the configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EBUSY       The driver is during transfer.
 * @retval -EINPROGRESS The driver is uninitialized.
 */
int nrfx_spis_reconfigure(nrfx_spis_t *              p_instance,
                          nrfx_spis_config_t const * p_config);

/**
 * @brief Function for uninitializing the SPI slave driver instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_spis_uninit(nrfx_spis_t * p_instance);

/**
 * @brief Function for checking if the SPIS driver instance is initialized.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  Instance is already initialized.
 * @retval false Instance is not initialized.
 */
bool nrfx_spis_init_check(nrfx_spis_t const * p_instance);

/**
 * @brief Function for preparing the SPI slave instance for a single SPI transaction.
 *
 * This function prepares the SPI slave device to be ready for a single SPI transaction. It configures
 * the SPI slave device to use the memory supplied with the function call in SPI transactions.
 *
 * When either the memory buffer configuration or the SPI transaction has been
 * completed, the event callback function will be called with the appropriate event
 * @ref nrfx_spis_event_type_t. The callback function can be called before returning from
 * this function, because it is called from the SPI slave interrupt context.
 *
 * @note This function can be called from the callback function context.
 *
 * @note Client applications must call this function after every @ref NRFX_SPIS_XFER_DONE event if
 * the SPI slave driver must be prepared for a possible new SPI transaction.
 *
 * @note Peripherals using EasyDMA (including SPIS) require the transfer buffers
 *       to be placed in the Data RAM region. If this condition is not met,
 *       this function will fail with the error code -EACCES.
 *
 * @param[in] p_instance       Pointer to the driver instance structure.
 * @param[in] p_tx_buffer      Pointer to the TX buffer. Can be NULL when the buffer length is zero.
 * @param[in] p_rx_buffer      Pointer to the RX buffer. Can be NULL when the buffer length is zero.
 * @param[in] tx_buffer_length Length of the TX buffer in bytes.
 * @param[in] rx_buffer_length Length of the RX buffer in bytes.
 *
 * @retval 0            The operation was successful.
 * @retval -EINPROGRESS The operation failed because the SPI slave device is in an incorrect state.
 * @retval -EACCES      The provided buffers are not placed in the Data RAM region.
 * @retval -E2BIG       Provided lengths exceed the EasyDMA limits for the peripheral.
 * @retval -ECANCELED   The operation failed because of an internal error.
 */
int nrfx_spis_buffers_set(nrfx_spis_t *   p_instance,
                          uint8_t const * p_tx_buffer,
                          size_t          tx_buffer_length,
                          uint8_t *       p_rx_buffer,
                          size_t          rx_buffer_length);

/**
 * @brief Driver interrupt handler.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_spis_irq_handler(nrfx_spis_t * p_instance);

#if NRF_ERRATA_STATIC_CHECK(52, 109) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for providing GPIOTE driver instance for the nRF52 Anomaly 109 workaround.
 *
 * @warning Must be called before @ref nrfx_spis_init.
 *
 * @param[in] p_instance    Pointer to the driver instance structure.
 * @param[in] p_gpiote_inst Pointer to the GPIOTE driver instance structure.
 */
void nrfx_spis_nrf52_anomaly_109_init(nrfx_spis_t * p_instance, nrfx_gpiote_t * p_gpiote_inst);
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_SPIS_H__
