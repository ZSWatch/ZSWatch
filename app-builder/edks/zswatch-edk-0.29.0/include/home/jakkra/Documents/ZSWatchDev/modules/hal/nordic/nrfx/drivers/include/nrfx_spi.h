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

#ifndef NRFX_SPI_H__
#define NRFX_SPI_H__

#include <nrfx.h>
#include <hal/nrf_spi.h>
#include <hal/nrf_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_spi SPI driver
 * @{
 * @ingroup nrf_spi
 * @brief   Serial Peripheral Interface master (SPI) driver.
 */

/** @brief Data structure of the Serial Peripheral Interface master (SPI) driver instance. */
typedef struct
{
    NRF_SPI_Type * p_reg;        ///< Pointer to a structure with SPI registers.
    uint8_t        drv_inst_idx; ///< Index of the driver instance. For internal use only.
} nrfx_spi_t;

#ifndef __NRFX_DOXYGEN__
enum {
    /* List all enabled driver instances (in the format NRFX_\<instance_name\>_INST_IDX). */
    NRFX_INSTANCE_ENUM_LIST(SPI)
    NRFX_SPI_ENABLED_COUNT
};
#endif

/** @brief Macro for creating an instance of the SPI master driver. */
#define NRFX_SPI_INSTANCE(id)                               \
{                                                           \
    .p_reg        = NRFX_CONCAT(NRF_, SPI, id),             \
    .drv_inst_idx = NRFX_CONCAT(NRFX_SPI, id, _INST_IDX),   \
}

/**
 * @brief This value can be provided instead of a pin number for signals MOSI,
 *        MISO, and Slave Select to specify that the given signal is not used and
 *        therefore does not need to be connected to a pin.
 */
#define NRFX_SPI_PIN_NOT_USED  0xFF

/** @brief Configuration structure of the SPI master driver instance. */
typedef struct
{
    uint8_t             sck_pin;       ///< SCK pin number.
    uint8_t             mosi_pin;      ///< MOSI pin number (optional).
                                       /**< Set to @ref NRFX_SPI_PIN_NOT_USED
                                        *   if this signal is not needed. */
    uint8_t             miso_pin;      ///< MISO pin number (optional).
                                       /**< Set to @ref NRFX_SPI_PIN_NOT_USED
                                        *   if this signal is not needed. */
    uint8_t             ss_pin;        ///< Slave Select pin number (optional).
                                       /**< Set to @ref NRFX_SPI_PIN_NOT_USED
                                        *   if this signal is not needed. The driver
                                        *   supports only active low for this signal.
                                        *   If the signal must be active high,
                                        *   it must be controlled externally.
                                        *   @note Unlike the other fields that specify
                                        *   pin numbers, this one cannot be omitted
                                        *   when both GPIO configuration and pin
                                        *   selection are to be skipped, as the driver
                                        *   must control the signal as a regular GPIO. */
    uint8_t             irq_priority;  ///< Interrupt priority.
    uint8_t             orc;           ///< Overrun character.
                                       /**< This character is used when all bytes from the TX buffer are sent,
                                        *   but the transfer continues due to RX. */
    nrf_spi_frequency_t frequency;     ///< SPI frequency.
    nrf_spi_mode_t      mode;          ///< SPI mode.
    nrf_spi_bit_order_t bit_order;     ///< SPI bit order.
    nrf_gpio_pin_pull_t miso_pull;     ///< MISO pull up configuration.
    bool                skip_gpio_cfg; ///< Skip GPIO configuration of pins.
                                       /**< When set to true, the driver does not modify
                                        *   any GPIO parameters of the used pins. Those
                                        *   parameters are supposed to be configured
                                        *   externally before the driver is initialized. */
    bool                skip_psel_cfg; ///< Skip pin selection configuration.
                                       /**< When set to true, the driver does not modify
                                        *   pin select registers in the peripheral.
                                        *   Those registers are supposed to be set up
                                        *   externally before the driver is initialized.
                                        *   @note When both GPIO configuration and pin
                                        *   selection are to be skipped, the structure
                                        *   fields that specify pins can be omitted,
                                        *   as they are ignored anyway. This does not
                                        *   apply to the @p ss_pin field. */
} nrfx_spi_config_t;

/**
 * @brief SPI master instance default configuration.
 * This configuration sets up SPI with the following options:
 * - over-run character set to 0xFF
 * - clock frequency 4 MHz
 * - mode 0 enabled (SCK active high, sample on leading edge of clock)
 * - MSB shifted out first
 * - MISO pull-up disabled
 *
 * @param[in] _pin_sck  SCK pin.
 * @param[in] _pin_mosi MOSI pin.
 * @param[in] _pin_miso MISO pin.
 * @param[in] _pin_ss   SS pin.
 */
#define NRFX_SPI_DEFAULT_CONFIG(_pin_sck, _pin_mosi, _pin_miso, _pin_ss)    \
{                                                                           \
    .sck_pin      = _pin_sck,                                               \
    .mosi_pin     = _pin_mosi,                                              \
    .miso_pin     = _pin_miso,                                              \
    .ss_pin       = _pin_ss,                                                \
    .irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,                   \
    .orc          = 0xFF,                                                   \
    .frequency    = NRF_SPI_FREQ_4M,                                        \
    .mode         = NRF_SPI_MODE_0,                                         \
    .bit_order    = NRF_SPI_BIT_ORDER_MSB_FIRST,                            \
    .miso_pull    = NRF_GPIO_PIN_NOPULL,                                    \
}

/** @brief Single transfer descriptor structure. */
typedef struct
{
    uint8_t const * p_tx_buffer; ///< Pointer to TX buffer.
    size_t          tx_length;   ///< TX buffer length.
    uint8_t       * p_rx_buffer; ///< Pointer to RX buffer.
    size_t          rx_length;   ///< RX buffer length.
}nrfx_spi_xfer_desc_t;

/**
 * @brief Macro for setting up single transfer descriptor.
 *
 * This macro is for internal use only.
 */
#define NRFX_SPI_SINGLE_XFER(p_tx, tx_len, p_rx, rx_len) \
{                                                        \
    .p_tx_buffer = (uint8_t const *)(p_tx),              \
    .tx_length = (tx_len),                               \
    .p_rx_buffer = (p_rx),                               \
    .rx_length = (rx_len),                               \
}

/** @brief Macro for setting the duplex TX RX transfer. */
#define NRFX_SPI_XFER_TRX(p_tx_buf, tx_length, p_rx_buf, rx_length) \
        NRFX_SPI_SINGLE_XFER(p_tx_buf, tx_length, p_rx_buf, rx_length)

/** @brief Macro for setting the TX transfer. */
#define NRFX_SPI_XFER_TX(p_buf, length) \
        NRFX_SPI_SINGLE_XFER(p_buf, length, NULL, 0)

/** @brief Macro for setting the RX transfer. */
#define NRFX_SPI_XFER_RX(p_buf, length) \
        NRFX_SPI_SINGLE_XFER(NULL, 0, p_buf, length)

/**
 * @brief SPI master driver event types, passed to the handler routine provided
 *        during initialization.
 */
typedef enum
{
    NRFX_SPI_EVENT_DONE, ///< Transfer done.
} nrfx_spi_evt_type_t;

/** @brief SPI master event description with transmission details. */
typedef struct
{
    nrfx_spi_evt_type_t  type;      ///< Event type.
    nrfx_spi_xfer_desc_t xfer_desc; ///< Transfer details.
} nrfx_spi_evt_t;

/** @brief SPI master driver event handler type. */
typedef void (* nrfx_spi_evt_handler_t)(nrfx_spi_evt_t const * p_event,
                                        void *                 p_context);

/**
 * @brief Function for initializing the SPI master driver instance.
 *
 * This function configures and enables the specified peripheral.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_config   Pointer to the structure with the initial configuration.
 * @param[in] handler    Event handler provided by the user. If NULL, transfers
 *                       will be performed in blocking mode.
 * @param[in] p_context  Context passed to the event handler.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver is already initialized.
 * @retval -EBUSY    Some other peripheral with the same instance ID is already in use.
 *                   This is possible only if @ref nrfx_prs module is enabled.
 */
int nrfx_spi_init(nrfx_spi_t const *        p_instance,
                  nrfx_spi_config_t const * p_config,
                  nrfx_spi_evt_handler_t    handler,
                  void *                    p_context);

/**
 * @brief Function for reconfiguring the SPI master driver instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_config   Pointer to the structure with the configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EBUSY       The driver is during transfer.
 * @retval -EINPROGRESS The driver is uninitialized.
 */
int nrfx_spi_reconfigure(nrfx_spi_t const *        p_instance,
                         nrfx_spi_config_t const * p_config);

/**
 * @brief Function for uninitializing the SPI master driver instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_spi_uninit(nrfx_spi_t const * p_instance);

/**
 * @brief Function for checking if the SPI driver instance is initialized.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  Instance is already initialized.
 * @retval false Instance is not initialized.
 */
bool nrfx_spi_init_check(nrfx_spi_t const * p_instance);

/**
 * @brief Function for starting the SPI data transfer.
 *
 * If an event handler was provided in the @ref nrfx_spi_init call, this function
 * returns immediately and the handler is called when the transfer is done.
 * Otherwise, the transfer is performed in blocking mode, which means that this function
 * returns when the transfer is finished.
 *
 * @param p_instance  Pointer to the driver instance structure.
 * @param p_xfer_desc Pointer to the transfer descriptor.
 * @param flags       Transfer options (0 for default settings).
 *                    Currently, no additional flags are available.
 *
 * @retval 0        The procedure is successful.
 * @retval -EBUSY   The driver is not ready for a new transfer.
 * @retval -ENOTSUP The provided parameters are not supported.
 */
int nrfx_spi_xfer(nrfx_spi_t const *           p_instance,
                  nrfx_spi_xfer_desc_t const * p_xfer_desc,
                  uint32_t                     flags);

/**
 * @brief Function for aborting the ongoing transfer.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_spi_abort(nrfx_spi_t const * p_instance);

/**
 * @brief Macro returning SPI interrupt handler.
 *
 * param[in] idx SPI index.
 *
 * @return Interrupt handler.
 */
#define NRFX_SPI_INST_HANDLER_GET(idx) NRFX_CONCAT_3(nrfx_spi_, idx, _irq_handler)

/** @} */

/*
 * Declare interrupt handlers for all enabled driver instances in the following format:
 * nrfx_\<periph_name\>_\<idx\>_irq_handler (for example, nrfx_spi_0_irq_handler).
 *
 * A specific interrupt handler for the driver instance can be retrieved by using
 * the NRFX_SPI_INST_HANDLER_GET macro.
 *
 * Here is a sample of using the NRFX_SPI_INST_HANDLER_GET macro to map an interrupt handler
 * in a Zephyr application:
 *
 * IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_SPI_INST_GET(\<instance_index\>)), \<priority\>,
 *             NRFX_SPI_INST_HANDLER_GET(\<instance_index\>), 0, 0);
 */
NRFX_INSTANCE_IRQ_HANDLERS_DECLARE(SPI, spi)


#ifdef __cplusplus
}
#endif

#endif // NRFX_SPI_H__
