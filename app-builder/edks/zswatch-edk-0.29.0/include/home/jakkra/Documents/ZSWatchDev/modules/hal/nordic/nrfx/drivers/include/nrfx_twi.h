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

#ifndef NRFX_TWI_H__
#define NRFX_TWI_H__

#include <nrfx.h>
#include <nrfx_twi_twim.h>
#include <hal/nrf_twi.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_twi TWI driver
 * @{
 * @ingroup nrf_twi
 * @brief   Two Wire Interface master (TWI) peripheral driver.
 */

/**
 * @brief Structure for the TWI master driver instance.
 */
typedef struct
{
    NRF_TWI_Type * p_twi;        ///< Pointer to a structure with TWI registers.
    uint8_t        drv_inst_idx; ///< Index of the driver instance. For internal use only.
} nrfx_twi_t;

/** @brief Macro for creating a TWI master driver instance. */
#define NRFX_TWI_INSTANCE(id)                               \
{                                                           \
    .p_twi        = NRFX_CONCAT(NRF_, TWI, id),             \
    .drv_inst_idx = NRFX_CONCAT(NRFX_TWI, id, _INST_IDX),   \
}

#ifndef __NRFX_DOXYGEN__
enum {
    /* List all enabled driver instances (in the format NRFX_\<instance_name\>_INST_IDX). */
    NRFX_INSTANCE_ENUM_LIST(TWI)
    NRFX_TWI_ENABLED_COUNT
};
#endif

/** @brief Structure for the configuration of the TWI master driver instance. */
typedef struct
{
    uint32_t            scl;                ///< SCL pin number.
    uint32_t            sda;                ///< SDA pin number.
    nrf_twi_frequency_t frequency;          ///< TWI frequency.
    uint8_t             interrupt_priority; ///< Interrupt priority.
    bool                hold_bus_uninit;    ///< Hold pull up state on GPIO pins after uninit.
    bool                skip_gpio_cfg;      ///< Skip GPIO configuration of pins.
                                            /**< When set to true, the driver does not modify
                                             *   any GPIO parameters of the used pins. Those
                                             *   parameters are supposed to be configured
                                             *   externally before the driver is initialized. */
    bool                skip_psel_cfg;      ///< Skip pin selection configuration.
                                            /**< When set to true, the driver does not modify
                                             *   pin select registers in the peripheral.
                                             *   Those registers are supposed to be set up
                                             *   externally before the driver is initialized.
                                             *   @note When both GPIO configuration and pin
                                             *   selection are to be skipped, the structure
                                             *   fields that specify pins can be omitted,
                                             *   as they are ignored anyway. */
} nrfx_twi_config_t;

/**
 * @brief TWI master driver instance default configuration.
 *
 * This configuration sets up TWI with the following options:
 * - clock frequency: 100 kHz
 * - disable bus holding after uninit
 *
 * @param[in] _pin_scl SCL pin.
 * @param[in] _pin_sda SDA pin.
 */
#define NRFX_TWI_DEFAULT_CONFIG(_pin_scl, _pin_sda)              \
{                                                                \
    .scl                = _pin_scl,                              \
    .sda                = _pin_sda,                              \
    .frequency          = NRF_TWI_FREQ_100K,                     \
    .interrupt_priority = NRFX_TWI_DEFAULT_CONFIG_IRQ_PRIORITY,  \
    .hold_bus_uninit    = false,                                 \
}

/** @brief Flag indicating that the interrupt after each transfer will be suppressed, and the event handler will not be called. */
#define NRFX_TWI_FLAG_NO_XFER_EVT_HANDLER (1UL << 2)
/** @brief Flag indicating that the TX transfer will not end with a stop condition. */
#define NRFX_TWI_FLAG_TX_NO_STOP          (1UL << 5)
/** @brief Flag indicating that the transfer will be suspended. */
#define NRFX_TWI_FLAG_SUSPEND             (1UL << 6)

/** @brief TWI master driver event types. */
typedef enum
{
    NRFX_TWI_EVT_DONE,         ///< Transfer completed event.
    NRFX_TWI_EVT_ADDRESS_NACK, ///< Error event: NACK received after sending the address.
    NRFX_TWI_EVT_DATA_NACK,    ///< Error event: NACK received after sending a data byte.
    NRFX_TWI_EVT_OVERRUN,      ///< Error event: The unread data is replaced by new data.
    NRFX_TWI_EVT_BUS_ERROR     ///< Error event: An unexpected transition occurred on the bus.
} nrfx_twi_evt_type_t;

/** @brief TWI master driver transfer types. */
typedef enum
{
    NRFX_TWI_XFER_TX,   ///< TX transfer.
    NRFX_TWI_XFER_RX,   ///< RX transfer.
    NRFX_TWI_XFER_TXRX, ///< TX transfer followed by RX transfer with repeated start.
    NRFX_TWI_XFER_TXTX  ///< TX transfer followed by TX transfer with repeated start.
} nrfx_twi_xfer_type_t;

/** @brief Structure for a TWI transfer descriptor. */
typedef struct
{
    nrfx_twi_xfer_type_t    type;             ///< Type of transfer.
    uint8_t                 address;          ///< Slave address.
    size_t                  primary_length;   ///< Number of bytes transferred.
    size_t                  secondary_length; ///< Number of bytes transferred.
    uint8_t *               p_primary_buf;    ///< Pointer to transferred data.
    uint8_t *               p_secondary_buf;  ///< Pointer to transferred data.
} nrfx_twi_xfer_desc_t;


/** @brief Macro for setting the TX transfer descriptor. */
#define NRFX_TWI_XFER_DESC_TX(addr, p_data, length) \
{                                                   \
    .type             = NRFX_TWI_XFER_TX,           \
    .address          = (addr),                     \
    .primary_length   = (length),                   \
    .secondary_length = 0,                          \
    .p_primary_buf    = (p_data),                   \
    .p_secondary_buf  = NULL,                       \
}

/** @brief Macro for setting the RX transfer descriptor. */
#define NRFX_TWI_XFER_DESC_RX(addr, p_data, length) \
{                                                   \
    .type             = NRFX_TWI_XFER_RX,           \
    .address          = (addr),                     \
    .primary_length   = (length),                   \
    .secondary_length = 0,                          \
    .p_primary_buf    = (p_data),                   \
    .p_secondary_buf  = NULL,                       \
}

/** @brief Macro for setting the TX-RX transfer descriptor. */
#define NRFX_TWI_XFER_DESC_TXRX(addr, p_tx, tx_len, p_rx, rx_len) \
{                                                                 \
    .type             = NRFX_TWI_XFER_TXRX,                       \
    .address          = (addr),                                   \
    .primary_length   = (tx_len),                                 \
    .secondary_length = (rx_len),                                 \
    .p_primary_buf    = (p_tx),                                   \
    .p_secondary_buf  = (p_rx),                                   \
}

/** @brief Macro for setting the TX-TX transfer descriptor. */
#define NRFX_TWI_XFER_DESC_TXTX(addr, p_tx, tx_len, p_tx2, tx_len2) \
{                                                                   \
    .type             = NRFX_TWI_XFER_TXTX,                         \
    .address          = (addr),                                     \
    .primary_length   = (tx_len),                                   \
    .secondary_length = (tx_len2),                                  \
    .p_primary_buf    = (p_tx),                                     \
    .p_secondary_buf  = (p_tx2),                                    \
}

/** @brief Structure for a TWI event. */
typedef struct
{
    nrfx_twi_evt_type_t  type;      ///< Event type.
    nrfx_twi_xfer_desc_t xfer_desc; ///< Transfer details.
} nrfx_twi_evt_t;

/** @brief TWI event handler prototype. */
typedef void (* nrfx_twi_evt_handler_t)(nrfx_twi_evt_t const * p_event,
                                        void *                 p_context);

/**
 * @brief Function for initializing the TWI driver instance.
 *
 * @param[in] p_instance    Pointer to the driver instance structure.
 * @param[in] p_config      Pointer to the structure with the initial configuration.
 * @param[in] event_handler Event handler provided by the user. If NULL, blocking mode is enabled.
 * @param[in] p_context     Context passed to event handler.
 *
 * @retval 0         Initialization is successful.
 * @retval -EALREADY The driver is already initialized.
 * @retval -EBUSY    Some other peripheral with the same instance ID is already in use.
 *                   This is possible only if @ref nrfx_prs module is enabled.
 */
int nrfx_twi_init(nrfx_twi_t const *        p_instance,
                  nrfx_twi_config_t const * p_config,
                  nrfx_twi_evt_handler_t    event_handler,
                  void *                    p_context);

/**
 * @brief Function for reconfiguring the TWI instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_config   Pointer to the structure with the configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EBUSY       The driver is during transaction.
 * @retval -EINPROGRESS The driver is uninitialized.
 */
int nrfx_twi_reconfigure(nrfx_twi_t const *        p_instance,
                         nrfx_twi_config_t const * p_config);

/**
 * @brief Function for uninitializing the TWI instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_twi_uninit(nrfx_twi_t const * p_instance);

/**
 * @brief Function for checking if the TWI driver instance is initialized.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  Instance is already initialized.
 * @retval false Instance is not initialized.
 */
bool nrfx_twi_init_check(nrfx_twi_t const * p_instance);

/**
 * @brief Function for enabling the TWI instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_twi_enable(nrfx_twi_t const * p_instance);

/**
 * @brief Function for disabling the TWI instance.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_twi_disable(nrfx_twi_t const * p_instance);

/**
 * @brief Function for performing a TWI transfer.
 *
 * The following transfer types can be configured (@ref nrfx_twi_xfer_desc_t.type):
 * - @ref NRFX_TWI_XFER_TXRX - Write operation followed by a read operation (without STOP condition in between).
 * - @ref NRFX_TWI_XFER_TXTX - Write operation followed by a write operation (without STOP condition in between).
 * - @ref NRFX_TWI_XFER_TX - Write operation (with or without STOP condition).
 * - @ref NRFX_TWI_XFER_RX - Read operation  (with STOP condition).
 *
 * @note TX-RX and TX-TX transfers are supported only in non-blocking mode.
 *
 * Additional options are provided using the flags parameter:
 * - @ref NRFX_TWI_FLAG_NO_XFER_EVT_HANDLER - No user event handler after transfer completion. In most cases, this also means no interrupt at the end of the transfer.
 * - @ref NRFX_TWI_FLAG_TX_NO_STOP - No stop condition after TX transfer.
 * - @ref NRFX_TWI_FLAG_SUSPEND - Transfer will be suspended. This allows for combining multiple transfers into one transaction.
 *                                Only transactions with the same direction can be combined. To finish the transaction, call the function without this flag.
 *
 * @note
 * Some flag combinations are invalid:
 * - @ref NRFX_TWI_FLAG_TX_NO_STOP with @ref nrfx_twi_xfer_desc_t.type different than @ref NRFX_TWI_XFER_TX
 *
 * @param[in] p_instance  Pointer to the driver instance structure.
 * @param[in] p_xfer_desc Pointer to the transfer descriptor.
 * @param[in] flags       Transfer options (0 for default settings).
 *
 * @retval 0            The procedure is successful.
 * @retval -EBUSY       The driver is not ready for a new transfer.
 * @retval -ECANCELED   An unexpected transition occurred on the bus.
 * @retval -EINPROGRESS Other direction of transaction is suspended on the bus.
 * @retval -EOVERFLOW   The unread data is replaced by new data (TXRX and RX)
 * @retval -EIO         Negative acknowledgement (NACK) is received after sending
 *                      the address in polling mode.
 * @retval -EAGAIN      Negative acknowledgement (NACK) is received after sending
 *                      a data byte in polling mode.
 */
int nrfx_twi_xfer(nrfx_twi_t           const * p_instance,
                  nrfx_twi_xfer_desc_t const * p_xfer_desc,
                  uint32_t                     flags);

/**
 * @brief Function for checking the TWI driver state.
 *
 * @param[in] p_instance TWI instance.
 *
 * @retval true  The TWI driver is currently busy performing a transfer.
 * @retval false The TWI driver is ready for a new transfer.
 */
bool nrfx_twi_is_busy(nrfx_twi_t const * p_instance);

/**
 * @brief Function for getting the transferred data count.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @return Data count.
 */
size_t nrfx_twi_data_count_get(nrfx_twi_t const * p_instance);

/**
 * @brief Function for returning the address of a STOPPED TWI event.
 *
 * A STOPPED event can be used to detect the end of a transfer if the @ref NRFX_TWI_FLAG_NO_XFER_EVT_HANDLER
 * option is used.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @return STOPPED event address.
 */
uint32_t nrfx_twi_stopped_event_get(nrfx_twi_t const * p_instance);

/**
 * @brief Function for recovering the bus.
 *
 * This function checks if the bus is not stuck because of a slave holding the SDA line in the low state,
 * and if needed it performs required number of pulses on the SCL line to make the slave release the SDA line.
 * Finally, the function generates a STOP condition on the bus to put it into a known state.
 *
 * @note This function can be used only if the TWI driver is uninitialized.
 *
 * @param[in] scl_pin SCL pin number.
 * @param[in] sda_pin SDA pin number.
 *
 * @retval 0          Bus recovery was successful.
 * @retval -ECANCELED Bus recovery failed.
 */
NRFX_STATIC_INLINE int nrfx_twi_bus_recover(uint32_t scl_pin, uint32_t sda_pin);

#ifndef NRFX_DECLARE_ONLY
NRFX_STATIC_INLINE int nrfx_twi_bus_recover(uint32_t scl_pin, uint32_t sda_pin)
{
    return nrfx_twi_twim_bus_recover(scl_pin, sda_pin);
}
#endif

/**
 * @brief Macro returning TWI interrupt handler.
 *
 * param[in] idx TWI index.
 *
 * @return Interrupt handler.
 */
#define NRFX_TWI_INST_HANDLER_GET(idx) NRFX_CONCAT_3(nrfx_twi_, idx, _irq_handler)

/** @} */

/*
 * Declare interrupt handlers for all enabled driver instances in the following format:
 * nrfx_\<periph_name\>_\<idx\>_irq_handler (for example, nrfx_twi_0_irq_handler).
 *
 * A specific interrupt handler for the driver instance can be retrieved by using
 * the NRFX_TWI_INST_HANDLER_GET macro.
 *
 * Here is a sample of using the NRFX_TWI_INST_HANDLER_GET macro to map an interrupt handler
 * in a Zephyr application:
 *
 * IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TWI_INST_GET(\<instance_index\>)), \<priority\>,
 *             NRFX_TWI_INST_HANDLER_GET(\<instance_index\>), 0, 0);
 */
NRFX_INSTANCE_IRQ_HANDLERS_DECLARE(TWI, twi)

#ifdef __cplusplus
}
#endif

#endif // NRFX_TWI_H__
