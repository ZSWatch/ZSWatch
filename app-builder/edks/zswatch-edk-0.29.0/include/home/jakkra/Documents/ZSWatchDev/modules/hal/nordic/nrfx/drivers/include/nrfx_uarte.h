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

#ifndef NRFX_UARTE_H__
#define NRFX_UARTE_H__

#include <nrfx.h>
#include <haly/nrfy_uarte.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_uarte UARTE driver
 * @{
 * @ingroup nrf_uarte
 * @brief   UARTE peripheral driver.
 */

/** @brief Structure used internally to handle reception through cache buffers. */
typedef struct
{
    nrfy_uarte_buffer_t user[2];   ///< User buffers.
    nrfy_uarte_buffer_t cache[2];  ///< Cache buffers.
    size_t              cache_len; ///< Single cache buffer length.
    size_t              started;   ///< Used for tracking progress of the current user buffer.
    size_t              received;  ///< Amount of received data in the current user buffer.
    uint8_t             idx;       ///< Used for determining which cache buffer to use.
    bool                buf_req;   ///< Flag indicate that next user buffer should be requested.
} nrfx_uarte_rx_cache_t;

/** @brief Types of UARTE driver events. */
typedef enum
{
    NRFX_UARTE_EVT_TX_DONE,         ///< Requested TX transfer completed.
    NRFX_UARTE_EVT_RX_DONE,         ///< Requested RX transfer completed.
    NRFX_UARTE_EVT_ERROR,           ///< Error reported by UART peripheral.
    NRFX_UARTE_EVT_RX_BUF_REQUEST,  ///< Request for a RX buffer.
    NRFX_UARTE_EVT_RX_DISABLED,     ///< Receiver is disabled.
    NRFX_UARTE_EVT_RX_BUF_TOO_LATE, ///< RX buffer request handled too late.
    NRFX_UARTE_EVT_RX_BYTE,         ///< Byte was received.
    NRFX_UARTE_EVT_TRIGGER,         ///< Result of @ref nrfx_uarte_int_trigger.
} nrfx_uarte_event_type_t;

/** @brief Structure for @ref NRFX_UARTE_EVT_RX_DONE event. */
typedef struct
{
    uint8_t * p_buffer; ///< Pointer to the received data.
    size_t    length;   ///< Amount of received data.
} nrfx_uarte_rx_evt_t;

/** @brief Structure for the @ref NRFX_UARTE_EVT_TX_DONE event. */
typedef struct
{
    const uint8_t * p_buffer; ///< Pointer to the transferred data.
    size_t          length;   ///< Amount of transferred data.
    uint32_t        flags;    ///< Flags. See @ref NRFX_UARTE_TX_DONE_FLAGS.
} nrfx_uarte_tx_evt_t;

/** @brief Structure for the @ref NRFX_UARTE_EVT_ERROR. */
typedef struct
{
    nrfy_uarte_buffer_t rx;         ///< Transfer details, including number of bytes received.
    uint32_t            error_mask; ///< Mask of error flags that generated the event.
} nrfx_uarte_error_evt_t;

/** @brief Structure for the @ref NRFX_UARTE_EVT_RX_DISABLED. */
typedef struct
{
    size_t flush_cnt;  ///< Number of bytes flushed from RX FIFO.
                       /**< They will be copied to the next provided buffer if
                        *   @ref NRFX_UARTE_RX_ENABLE_KEEP_FIFO_CONTENT is set. */
} nrfx_uarte_rx_disabled_evt_t;

/** @brief Structure for UARTE event. */
typedef struct
{
    nrfx_uarte_event_type_t type;                 ///< Event type.
    union
    {
        nrfx_uarte_rx_evt_t          rx;          ///< Data for @ref NRFX_UARTE_EVT_RX_DONE.
        nrfx_uarte_tx_evt_t          tx;          ///< Data for @ref NRFX_UARTE_EVT_TX_DONE.
        nrfx_uarte_error_evt_t       error;       ///< Data for @ref NRFX_UARTE_EVT_ERROR.
        nrfx_uarte_rx_disabled_evt_t rx_disabled; ///< Data for @ref NRFX_UARTE_EVT_RX_DISABLED.
    } data;                                       ///< Union to store event data.
} nrfx_uarte_event_t;

/**
 * @brief UARTE interrupt event handler.
 *
 * @param[in] p_event   Pointer to event structure. Event is allocated on the stack so it is
 *                      available only within the context of the event handler.
 * @param[in] p_context Context passed to the interrupt handler, set on initialization.
 */
typedef void (*nrfx_uarte_event_handler_t)(nrfx_uarte_event_t const * p_event,
                                           void *                     p_context);

 /** @cond Driver internal data. */
typedef struct
{
    nrfy_uarte_buffer_t     curr;
    nrfy_uarte_buffer_t     next;
    nrfy_uarte_buffer_t     flush;
    nrfx_uarte_rx_cache_t * p_cache;
    size_t                  off;
} nrfx_uarte_rx_data_t;

typedef struct
{
    nrfy_uarte_buffer_t curr;
    nrfy_uarte_buffer_t next;
    nrfy_uarte_buffer_t cache;
    size_t              off;
    int                 amount;
} nrfx_uarte_tx_data_t;

typedef struct
{
    void                     * p_context;
    nrfx_uarte_event_handler_t handler;
    nrfx_uarte_rx_data_t       rx;
    nrfx_uarte_tx_data_t       tx;
    nrfx_drv_state_t           state;
    nrfx_atomic_t              flags;
} nrfx_uarte_control_block_t;
/** @endcond */

/** @brief Structure for the UARTE driver instance. */
typedef struct
{
    NRF_UARTE_Type *           p_reg; ///< Pointer to a structure with UARTE registers.
    nrfx_uarte_control_block_t cb;    ///< Driver internal data.
} nrfx_uarte_t;

/** @brief Macro for creating a UARTE driver instance. */
#define NRFX_UARTE_INSTANCE(reg)    \
{                                   \
    .p_reg = (NRF_UARTE_Type *)reg, \
    .cb    = {0},                   \
}

/**@defgroup NRFX_UARTE_RX_ENABLE_FLAGS Flags used for @ref nrfx_uarte_rx_enable.
 * @{ */

/**
 * @brief Flag for configuring continuous reception.
 *
 * When the flag is set, the ENDRX event is shortened with the STARTRX task.
 * The flag should not be used together with short buffers in case there is a risk that a new buffer
 * is not provided on time. If the flag is set and a new buffer is not provided on time,
 * a receiver starts to overwrite the current buffer. If not set and a new buffer has been provided
 * on time, a new transfer will be triggered from the ENDRX interrupt handler.
 * This flag is recommended to be used with longer buffers to ensure lossless reception
 * without HWFC detection.
 */
#define NRFX_UARTE_RX_ENABLE_CONT NRFX_BIT(0)

/**
 * @brief Flag indicating that receiver is stopped when new buffer is not provided.
 *
 * Setting a flag changes the behavior of the UARTE driver when receiving to the buffer is
 * completed and a new buffer is not provided. When the flag is set, stopping is initiated.
 * Since the stopping takes time, this operation can still be revoked by providing a new buffer
 * (@ref nrfx_uarte_rx_buffer_set). When the flag is not set, then the receiver is kept enabled, and
 * incoming data is put in the hardware FIFO (if HWFC is disabled) until FIFO is
 * full and bytes are dropped.
 */
#define NRFX_UARTE_RX_ENABLE_STOP_ON_END NRFX_BIT(1)

/**
 * @brief Flag indicating that FIFO content that was flushed after the last stopping shall be kept.
 *
 * When a receiver is stopped, it is possible that there is some data in the hardware FIFO
 * that was not stored in the user buffer due to the lack of capacity. As a result, that data
 * is flushed to an internal buffer. If the flag is set, the flushed data is kept and copied into
 * the user buffer.
 *
 * Using the flag makes the user to provide their buffer for flushed data during the initialization.
 */
#define NRFX_UARTE_RX_ENABLE_KEEP_FIFO_CONTENT NRFX_BIT(2)

/**@} */

/**@defgroup NRFX_UARTE_TX_FLAGS Flags used for @ref nrfx_uarte_tx.
 *
 * Flags apply only if an instance is initialized with a user handler. Otherwise flags are
 * ignored and all transfers are blocking.
 *
 * @{ */

/**
 * @brief Flag indicating blocking transfer.
 *
 * When the flag is set, the transfer is synchronous even if the driver is configured
 * to non-blocking operation. If UARTE is transmitting when the user requests blocking transfer,
 * -EBUSY is returned. A driver state is determined by hardware, thus it is
 * accepted to poll the driver and continuously request blocking transfer until it
 * is accepted. It can be done from any priority context. Blocking transfer returns
 * when the buffer is transferred or when the user buffer is no longer in use
 * (see @ref NRFX_UARTE_TX_EARLY_RETURN).
 */
#define NRFX_UARTE_TX_BLOCKING NRFX_BIT(0)

/**
 * @brief Flag indicating to return from blocking transfer not waiting for the last transmit event.
 *
 * The flag indicates a blocking transfer just like @ref NRFX_UARTE_TX_BLOCKING.
 * However, when the flag is set, if the transfer is still ongoing and if the transfer of the last
 * byte already has started, @ref nrfx_uarte_tx returns instead of waiting for the transfer to end.
 */
#define NRFX_UARTE_TX_EARLY_RETURN NRFX_BIT(1)

/**
 * @brief Flag indicating that the @ref nrfx_uarte_tx call will be linked to an active transfer.
 *
 * UARTE DMA registers are buffered which means that once transfer is started, registers with
 * transfer details (pointer and length) can be overwritten. If that is combined with the (D)PPI
 * connection between ENDTX and STARTTX events, then two transfers are linked together and
 * bytes are transferred without any gap allowing to utilize the maximum bandwidth.
 *
 * When the flag is set, it indicates that the user can set up an ENDTX-STARTTX (D)PPI connection and
 * wants to perform linked transfers. It is the user's responsibility to disable the (D)PPI connection
 * when the last transfer is started. If the user does not set up an ENDTX-STARTTX (D)PPI connection, then
 * the transfer is restarted from the context of ENDTX event handling which is earlier than context
 * of the @ref NRFX_UARTE_EVT_TX_DONE. The flag has no impact if used while there is no ongoing
 * transfer.
 *
 * For example, if a sequence consists of three transfers, then the first @ref nrfx_uarte_tx
 * can be called with or without the flag and the following two transfers must have the flag
 * set. The second @ref nrfx_uarte_tx can be called immediately after the first one and the third
 * one after the first @ref NRFX_UARTE_EVT_TX_DONE event. After the second
 * @ref NRFX_UARTE_EVT_TX_DONE event is received, the (D)PPI connection must be disabled (if it was
 * used).
 *
 * When (D)PPI connection is used, then it is critical that (D)PPI connection is disabled on time,
 * before the last transfer is completed. Otherwise, the transfer will be repeated, and unwanted data
 * will be transferred. Hence, it is recommended to use longer buffers. Time needed to
 * send the buffer must be longer than the maximum system latency.
 *
 * When the flag is used, then the driver instance must not use the ENDTX-STOPTX (D)PPI connection.
 *
 * When linked transfers are used, then blocking transfers (see @ref NRFX_UARTE_TX_BLOCKING and
 * @ref NRFX_UARTE_TX_EARLY_RETURN) cannot be performed. An error is returned when the flag is set
 * and the @ref nrfx_uarte_tx is called during ongoing blocking transfer.
 */
#define NRFX_UARTE_TX_LINK         NRFX_BIT(2)

/**@} */

/**@defgroup NRFX_UARTE_TX_DONE_FLAGS Flags used for @ref nrfx_uarte_tx_evt_t.
 * @{ */

/** @brief Flag indicating that TX transfer was aborted. */
#define NRFX_UARTE_TX_DONE_ABORTED NRFX_BIT(0)

/**@} */

/** @brief Structure for the UARTE configuration. */
typedef struct
{
    uint32_t                txd_pin;           ///< TXD pin number.
    uint32_t                rxd_pin;           ///< RXD pin number.
    uint32_t                rts_pin;           ///< RTS pin number.
    uint32_t                cts_pin;           ///< CTS pin number.
    void *                  p_context;         ///< Context passed to interrupt handler.
    nrfy_uarte_buffer_t     tx_cache;          ///< TX cache buffer.
    nrfy_uarte_buffer_t     rx_cache;          ///< RX cache buffer.
                                               /**< A buffer to store flushed RX data. The buffer is also
                                                *   used when the input RX buffer is from an address space
                                                *   that cannot be handled by the DMA.
                                                *   A buffer size must be at least 5 bytes which is the
                                                *   size of the HW FIFO, and should be bigger if
                                                *   expected to be used as cache when an input RX buffer
                                                *   cannot be used for the DMA. If not provided,
                                                *   then bytes left in the FIFO after a receiver is
                                                *   disabled will be discarded, and reception will not
                                                *   be performed if the input RX buffer cannot be used by
                                                *   the DMA. */
    nrfx_uarte_rx_cache_t * p_rx_cache_scratch; /**< Static RAM memory area used for receiving data
                                                *    through RX cache buffer. Can be NULL if RX
                                                *    caching is not used. */
    nrf_uarte_baudrate_t    baudrate;           ///< Baud rate.
    nrf_uarte_config_t      config;             ///< Peripheral configuration.
    bool                    skip_psel_cfg;      ///< Skip pin selection configuration.
                                                /**< When set to true, the driver does not modify
                                                 *   pin select registers in the peripheral.
                                                 *   Those registers are supposed to be set up
                                                 *   externally before the driver is initialized.
                                                 *   @note When both GPIO configuration and pin
                                                 *   selection are to be skipped, the structure
                                                 *   fields that specify pins can be omitted,
                                                 *   as they are ignored anyway. */
    bool                    skip_gpio_cfg;      ///< Skip GPIO configuration of pins.
                                                /**< When set to true, the driver does not modify
                                                 *   any GPIO parameters of the used pins. Those
                                                 *   parameters are supposed to be configured
                                                 *   externally before the driver is initialized. */
    bool                    tx_stop_on_end;     ///< Indicates that the STOPTX task is PPIed with ENDTX event
                                                /**< If SHORT exists, then it will be used by the driver,
                                                 *   otherwise (D)PPI connection must be setup by the user. */
    uint8_t                 interrupt_priority; ///< Interrupt priority.
} nrfx_uarte_config_t;

/**
 * @brief UARTE driver default configuration.
 *
 * This configuration sets up UARTE with the following options:
 * - hardware flow control disabled
 * - no parity bit
 * - one stop bit
 * - baudrate: 115200
 *
 * @param[in] _pin_tx TX pin.
 * @param[in] _pin_rx RX pin.
 */
#define NRFX_UARTE_DEFAULT_CONFIG(_pin_tx, _pin_rx)                             \
{                                                                               \
    .txd_pin            = _pin_tx,                                              \
    .rxd_pin            = _pin_rx,                                              \
    .rts_pin            = NRF_UARTE_PSEL_DISCONNECTED,                          \
    .cts_pin            = NRF_UARTE_PSEL_DISCONNECTED,                          \
    .p_context          = NULL,                                                 \
    .tx_cache           =                                                       \
    {                                                                           \
        .p_buffer = NULL,                                                       \
        .length = 0                                                             \
    },                                                                          \
    .rx_cache           =                                                       \
    {                                                                           \
        .p_buffer = NULL,                                                       \
        .length = 0                                                             \
    },                                                                          \
    .p_rx_cache_scratch = NULL,                                                 \
    .baudrate           = NRF_UARTE_BAUDRATE_115200,                            \
    .config             =                                                       \
    {                                                                           \
        .hwfc           = NRF_UARTE_HWFC_DISABLED,                              \
        .parity         = NRF_UARTE_PARITY_EXCLUDED,                            \
        NRFX_COND_CODE_1(NRF_UART_HAS_STOP_BITS,                                \
                (.stop = (nrf_uarte_stop_t)NRF_UARTE_STOP_ONE,), ())            \
        NRFX_COND_CODE_1(NRF_UART_HAS_PARITY_BIT,                               \
                (.paritytype = NRF_UARTE_PARITYTYPE_EVEN,), ())                 \
    },                                                                          \
    .skip_psel_cfg      = false,                                                \
    .skip_gpio_cfg      = false,                                                \
    .tx_stop_on_end     = false,                                                \
    .interrupt_priority = NRFX_UARTE_DEFAULT_CONFIG_IRQ_PRIORITY                \
}

/**
 * @brief Function for initializing the UARTE driver.
 *
 * This function configures UARTE but peripheral is kept disabled to reduce power consumption.
 *
 * @param[in] p_instance    Pointer to the driver instance structure.
 * @param[in] p_config      Pointer to the structure with the initial configuration.
 * @param[in] event_handler Event handler provided by the user. If not provided driver works in
 *                          blocking mode.
 *
 * @retval 0          Initialization was successful.
 * @retval -EALREADY  The driver is already initialized.
 * @retval -EINVAL    Invalid configuration.
 * @retval -EBUSY     Some other peripheral with the same instance ID is already in use.
 *                    This is possible only if @ref nrfx_prs module is enabled.
 * @retval -ECANCELED Peripheral was not properly deinitialized after use.
 */
int nrfx_uarte_init(nrfx_uarte_t *              p_instance,
                    nrfx_uarte_config_t const * p_config,
                    nrfx_uarte_event_handler_t  event_handler);

/**
 * @brief Function for reconfiguring the UARTE driver.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_config   Pointer to the structure with the configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EBUSY       The driver is during transfer.
 * @retval -EINPROGRESS The driver is uninitialized.
 * @retval -ECANCELED   Peripheral was not properly deinitialized after use.
 */
int nrfx_uarte_reconfigure(nrfx_uarte_t *              p_instance,
                           nrfx_uarte_config_t const * p_config);

/**
 * @brief Function for uninitializing the UARTE driver.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_uarte_uninit(nrfx_uarte_t * p_instance);

/**
 * @brief Function for checking if the UARTE driver instance is initialized.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  Instance is already initialized.
 * @retval false Instance is not initialized.
 */
bool nrfx_uarte_init_check(nrfx_uarte_t const * p_instance);

/**
 * @brief Function for getting the address of the specified UARTE task.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] task       Task.
 *
 * @return Task address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_uarte_task_address_get(nrfx_uarte_t const * p_instance,
                                                        nrf_uarte_task_t     task);

/**
 * @brief Function for getting the address of the specified UARTE event.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] event      Event.
 *
 * @return Event address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_uarte_event_address_get(nrfx_uarte_t const * p_instance,
                                                         nrf_uarte_event_t    event);

/**
 * @brief Function for sending data over UARTE.
 *
 * If an event handler is provided in nrfx_uarte_init() call, this function
 * returns immediately (unless special flags are used) and the handler is called when the transfer
 * is done. Otherwise, the transfer is performed in blocking mode, that is, this function
 * returns when the transfer is finished.
 *
 * @note Peripherals using EasyDMA (including UARTE) require the transfer buffers
 *       to be placed in the Data RAM region. If this condition is not met,
 *       this function will attempt to use the cache buffer provided in the configuration
 *       and if it is not available it will return error.
 *
 * @note To achieve the lowest power consumption, transmitter is stopped and peripheral is disabled
 *       (if receiver is not used) when transfer is completed.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_data     Pointer to data.
 * @param[in] length     Number of bytes to send. Maximum possible length is
 *                       dependent on the used SoC (see the MAXCNT register
 *                       description in the Product Specification). The driver
 *                       checks it with assertion.
 * @param[in] flags      Option flags. See @ref NRFX_UARTE_TX_FLAGS.
 *
 * @retval 0          Initialization was successful.
 * @retval -EBUSY     Driver is busy transferring the data.
 * @retval -EPERM     The transfer was aborted from a different context
                      (blocking mode only) or transfer cannot be performed due to
 *                    driver state, configuration or transfer parameters.
 * @retval -EACCES    p_data does not point to RAM buffer and cache buffer is not
 *                    provided or attempted to use non DMA buffer with linked
 *                    transfer (see @ref NRFX_UARTE_TX_LINK).
 * @retval -E2BIG     Flag @ref NRFX_UARTE_TX_EARLY_RETURN is used
 *                    but @p length exceeds internal buffer size.
 * @retval -ECANCELED Peripheral was not properly deinitialized after use.
 */
int nrfx_uarte_tx(nrfx_uarte_t *  p_instance,
                  uint8_t const * p_data,
                  size_t          length,
                  uint32_t        flags);

/**
 * @brief Function for checking if UARTE is currently transmitting.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  The UARTE is transmitting.
 * @retval false The UARTE is not transmitting.
 */
bool nrfx_uarte_tx_in_progress(nrfx_uarte_t const * p_instance);

/**
 * @brief Function for aborting any ongoing transmission.
 *
 * @note When abortion is not synchronous, the @ref NRFX_UARTE_EVT_TX_DONE event will be generated
 *       in non-blocking mode. It will contain the number of bytes sent until the abort was called.
 *       If @ref NRFX_UARTE_TX_LINK flag was used for the transfer and linked transfer have not
 *       started yet, there will be the second @ref NRFX_UARTE_EVT_TX_DONE event with length equal
 *       to 0. The event handler will be called from the UARTE interrupt context.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] sync       If true operation is synchronous. Transmitter is stopped upon
 *                       function return and no event is generated.
 *
 * @retval 0            Successfully initiated abort or when transmitter synchronously stopped.
 * @retval -EINPROGRESS Attempt to asynchronously abort when no transfer is active.
 */
int nrfx_uarte_tx_abort(nrfx_uarte_t * p_instance, bool sync);

/**
 * @brief Function for enabling the receiver.
 *
 * The event handler will be called from the caller context with
 * the @ref NRFX_UARTE_EVT_RX_BUF_REQUEST event. The user may respond and provide a buffer
 * using @ref nrfx_uarte_rx_buffer_set. An error is returned if buffer is not provided. After that,
 * the receiver is started and another @ref NRFX_UARTE_EVT_RX_BUF_REQUEST is generated.
 * If a new buffer is not provided, then the receiver is disabled once the first buffer
 * becomes full. If a new buffer is provided, then the receiver will seamlessly switch to
 * a new buffer (using a hardware shortcut).
 *
 * @note If transmitter is inactive then peripheral is disabled after receiver is stopped to achieve
 *       the lowest power consumption.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] flags      Option flags. See @ref NRFX_UARTE_RX_ENABLE_FLAGS.
 *
 * @retval 0       Receiver successfully enabled.
 * @retval -EBUSY  When receiver is already enabled.
 * @retval -ENOMEM When buffer was not provided.
 */
int nrfx_uarte_rx_enable(nrfx_uarte_t * p_instance, uint32_t flags);

/**
 * @brief Function for providing reception buffer.
 *
 * The function should be called as a response to the @ref NRFX_UARTE_EVT_RX_BUF_REQUEST event.
 * If the function is called before enabling the receiver, the first buffer is configured.
 * If the function is called and there is no active buffer but the receiver is enabled
 * but not started, it starts reception.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_data     Pointer to a buffer.
 * @param[in] length     Buffer length.
 *
 * @retval 0            Buffer successfully set.
 * @retval -EINPROGRESS Buffer provided without pending request.
 * @retval -EACCES      No cache buffer provided or blocking mode is used,
 *                      transfer cannot be handled.
 * @retval -EBUSY       Previous buffer is still in use.
 * @retval -EPERM       Provided uncached buffer after providing cached one.
 */
int nrfx_uarte_rx_buffer_set(nrfx_uarte_t * p_instance,
                             uint8_t *      p_data,
                             size_t         length);

/**
 * @brief Function for testing the receiver state in blocking mode.
 *
 * @param[in]  p_instance  Pointer to the driver instance structure.
 * @param[out] p_rx_amount Pointer to the variable to be filled with the number of received bytes.
 *                         Can be NULL.
 *
 * @retval 0         The receiving operation is completed.
 * @retval -EBUSY    The receiver did not complete the operation.
 * @retval -EALREADY The receiver is disabled.
 * @retval -EPERM    Operation is not supporting in the current configuration.
 */
int nrfx_uarte_rx_ready(nrfx_uarte_t const * p_instance, size_t * p_rx_amount);

/**
 * @brief Function for aborting any ongoing reception.
 *
 * @note @ref NRFX_UARTE_EVT_RX_DONE event will be generated in non-blocking mode.
 *       It will contain number of bytes received until the abort was called. The event
 *       handler will be called from the UARTE interrupt context.
 *
 * @warning When the double-buffering feature is used and the UARTE interrupt
 *          is processed with a delay (for example, due to a higher priority
 *          interrupt) long enough for the first buffer to be filled completely,
 *          the event handler will be supplied with the pointer to the first
 *          buffer and the number of bytes received in the second buffer.
 *          This is because from hardware perspective it is impossible to deduce
 *          the reception of which buffer has been aborted.
 *          To prevent this from happening, keep the UARTE interrupt latency low
 *          or use large enough reception buffers.
 *
 * @param[in] p_instance  Pointer to the driver instance structure.
 * @param[in] disable_all If true, UARTE is stopped. If false and there is a second RX buffer provided,
 *                        only the first transfer is stopped.
 * @param[in] sync        If true, receiver is disabled synchronously.
 *
 * @retval 0            Successfully initiate disabling or disabled (synchronous mode).
 * @retval -EINPROGRESS Receiver was not enabled.
 */
int nrfx_uarte_rx_abort(nrfx_uarte_t * p_instance, bool disable_all, bool sync);

/**
 * @brief Function for reading error source mask.
 *
 * Mask contains values from @ref nrf_uarte_error_mask_t.
 *
 * @note Function must be used in the blocking mode only. In case of non-blocking mode, an error
 *       event is generated. Function clears error sources after reading.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @return Mask of reported errors.
 */
uint32_t nrfx_uarte_errorsrc_get(nrfx_uarte_t const * p_instance);

/**
 * @brief Function for checking if there was new RX data since the last check.
 *
 * Function checks @ref NRF_UARTE_EVENT_RXDRDY event and clears it if it was set.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  At least one byte was received since the last check.
 * @retval false No new data was received since the last check.
 */
bool nrfx_uarte_rx_new_data_check(nrfx_uarte_t * p_instance);

/**
 * @brief Function for enabling @ref NRFX_UARTE_EVT_RX_BYTE event.
 *
 * The function enables the @ref NRF_UARTE_EVENT_RXDRDY hardware event which is generated whenever a byte is
 * received in RXD registers. The event indicates only that data is received, hence it must not be used yet
 * because it may not be present yet in the RAM buffer handled by the EasyDMA. The event can be used only to
 * detect a receiver activity. The event can be enabled at any time. Enabling it may increase the number of interrupts (after each received byte).
 *
 * @note If there were a receiver activity prior to enabling the @ref NRF_UARTE_EVENT_RXDRDY event,
 * the @ref NRF_UARTE_EVENT_RXDRDY event may already be set and the @ref NRFX_UARTE_EVT_RX_BYTE will be
 * triggered immediately. To avoid that, it is recommended to clear that event by calling
 * the @ref nrfx_uarte_rx_new_data_check.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
NRFX_STATIC_INLINE void nrfx_uarte_rxdrdy_enable(nrfx_uarte_t const * p_instance);

/**
 * @brief Function for disabling @ref NRFX_UARTE_EVT_RX_BYTE event.
 *
 * The function disables the RXDRDY hardware event. See the @ref nrfx_uarte_rxdrdy_enable for more details.
 * The event can be disabled at any time.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
NRFX_STATIC_INLINE void nrfx_uarte_rxdrdy_disable(nrfx_uarte_t const * p_instance);

/**
 * @brief Function for triggering UARTE interrupt.
 *
 * Function can be used to jump into UARTE interrupt context. User handler is
 * called with the event @ref NRFX_UARTE_EVT_TRIGGER.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval 0      If interrupt is successfully triggered.
 * @retval -EPERM Failure. User handler is not configured.
 */
int nrfx_uarte_int_trigger(nrfx_uarte_t * p_instance);

/**
 * @brief Driver interrupt handler.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_uarte_irq_handler(nrfx_uarte_t * p_instance);

#ifndef NRFX_DECLARE_ONLY
NRFX_STATIC_INLINE uint32_t nrfx_uarte_task_address_get(nrfx_uarte_t const * p_instance,
                                                        nrf_uarte_task_t     task)
{
    NRFX_ASSERT(p_instance);

    return nrfy_uarte_task_address_get(p_instance->p_reg, task);
}

NRFX_STATIC_INLINE uint32_t nrfx_uarte_event_address_get(nrfx_uarte_t const * p_instance,
                                                         nrf_uarte_event_t    event)
{
    NRFX_ASSERT(p_instance);

    return nrfy_uarte_event_address_get(p_instance->p_reg, event);
}

NRFX_STATIC_INLINE void nrfx_uarte_rxdrdy_enable(nrfx_uarte_t const * p_instance)
{
    NRFX_ASSERT(p_instance);

	nrfy_uarte_int_enable(p_instance->p_reg, NRF_UARTE_INT_RXDRDY_MASK);
}

NRFX_STATIC_INLINE void nrfx_uarte_rxdrdy_disable(nrfx_uarte_t const * p_instance)
{
    NRFX_ASSERT(p_instance);

	nrfy_uarte_int_disable(p_instance->p_reg, NRF_UARTE_INT_RXDRDY_MASK);
}

#endif // NRFX_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_UARTE_H__
