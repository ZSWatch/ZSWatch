/*
 * Copyright (c) 2016 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_QSPI_H__
#define NRFX_QSPI_H__

#include <nrfx.h>
#include <hal/nrf_qspi.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_qspi QSPI driver
 * @{
 * @ingroup nrf_qspi
 * @brief   Quad Serial Peripheral Interface (QSPI) peripheral driver.
 */

/** @brief QSPI driver instance configuration structure. */
typedef struct
{
    uint32_t             xip_offset;    ///< Address offset into the external memory for Execute in Place operation.
    nrf_qspi_pins_t      pins;          ///< Pin configuration structure.
    nrf_qspi_prot_conf_t prot_if;       ///< Protocol layer interface configuration structure.
    nrf_qspi_phy_conf_t  phy_if;        ///< Physical layer interface configuration structure.
    uint32_t             timeout;       ///< Time in milliseconds used in timeout counter.
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
} nrfx_qspi_config_t;

/**
 * @brief QSPI instance default configuration.
 *
 * This configuration sets up QSPI with the following options:
 * - no offset for address in the external memory for Execute in Place operation
 * - single data line
 * - FAST_READ opcode for reading
 * - PP opcode for writing
 * - 24 bit addressing mode
 * - Deep Power-down disabled
 * - clock frequency: 2 MHz for nRF52 Series, 6 MHz for nRF53 Series
 * - SCK delay 5 clock ticks
 * - 500 milliseconds operation timeout
 * - mode 0 (data captured on the clock rising edge and transmitted on a falling edge. Clock base level is '0')
 *
 * @param[in] _pin_sck Pin for clock signal.
 * @param[in] _pin_csn Pin for chip select signal.
 * @param[in] _pin_io0 Pin 0 for I/O data.
 * @param[in] _pin_io1 Pin 1 for I/O data.
 * @param[in] _pin_io2 Pin 2 for I/O data.
 * @param[in] _pin_io3 Pin 3 for I/O data.
 */
#define NRFX_QSPI_DEFAULT_CONFIG(_pin_sck, _pin_csn, _pin_io0,         \
                                 _pin_io1, _pin_io2, _pin_io3)         \
{                                                                      \
    .xip_offset    = 0,                                                \
    .pins = {                                                          \
       .sck_pin    = _pin_sck,                                         \
       .csn_pin    = _pin_csn,                                         \
       .io0_pin    = _pin_io0,                                         \
       .io1_pin    = _pin_io1,                                         \
       .io2_pin    = _pin_io2,                                         \
       .io3_pin    = _pin_io3                                          \
    },                                                                 \
    .prot_if = {                                                       \
        .readoc    = NRF_QSPI_READOC_FASTREAD,                         \
        .writeoc   = NRF_QSPI_WRITEOC_PP,                              \
        .addrmode  = NRF_QSPI_ADDRMODE_24BIT,                          \
        .dpmconfig = false,                                            \
    },                                                                 \
    .phy_if = {                                                        \
        .sck_delay = 0x05,                                             \
        .dpmen     = false,                                            \
        .spi_mode  = NRF_QSPI_MODE_0,                                  \
        .sck_freq  = NRF_QSPI_FREQ_DIV16,                              \
    },                                                                 \
    .timeout       = 500,                                              \
    .irq_priority  = (uint8_t)NRFX_QSPI_DEFAULT_CONFIG_IRQ_PRIORITY,   \
}

/** @brief QSPI custom instruction helper with the default configuration. */
#define NRFX_QSPI_DEFAULT_CINSTR(opc, len) \
{                                          \
    .opcode    = (opc),                    \
    .length    = (len),                    \
    .io2_level = false,                    \
    .io3_level = false,                    \
    .wipwait   = false,                    \
    .wren      = false                     \
}

/**
 * @brief QSPI master driver event types, passed to the handler routine provided
 *        during initialization.
 */
typedef enum
{
    NRFX_QSPI_EVENT_DONE, /**< Transfer done. */
} nrfx_qspi_evt_t;

/**
 * @brief QSPI master driver extended event types,
 *        obtained using @ref nrfx_qspi_event_extended_get() function.
 */
typedef enum
{
    NRFX_QSPI_EVENT_NONE,       /**< No event occurence. */
    NRFX_QSPI_EVENT_WRITE_DONE, /**< Write done. */
    NRFX_QSPI_EVENT_READ_DONE,  /**< Read done. */
    NRFX_QSPI_EVENT_ERASE_DONE, /**< Erase done. */
} nrfx_qspi_evt_ext_type_t;

/** @brief QSPI driver erase event data. */
typedef struct
{
    uint32_t             addr; /**< Erase start address. */
    nrf_qspi_erase_len_t len;  /**< Erase length. */
} nrfx_qspi_evt_ext_erase_t;

/** @brief QSPI driver transfer event data. */
typedef struct
{
    void *   p_buffer; /**< Pointer to the data buffer associated with transfer. */
    size_t   size;     /**< Data buffer size. */
    uint32_t addr;     /**< Transfer start address. */
} nrfx_qspi_evt_ext_xfer_t;

/** @brief QSPI driver extended event structure. */
typedef struct
{
    nrfx_qspi_evt_ext_type_t type;       ///< Extended event type.
    union
    {
        nrfx_qspi_evt_ext_xfer_t  xfer;  ///< Data for write or read transfer event.
        nrfx_qspi_evt_ext_erase_t erase; ///< Data for erase event.
    } data;                              ///< Union to store event data.
} nrfx_qspi_evt_ext_t;

/** @brief QSPI driver event handler type. */
typedef void (*nrfx_qspi_handler_t)(nrfx_qspi_evt_t event, void * p_context);

/**
 * @brief Function for initializing the QSPI driver instance.
 *
 * This function configures the peripheral and its interrupts.
 *
 * @note The function does not activate the peripheral instance. The activation is done during the first
 *       transfer after initialization or when calling @ref nrfx_qspi_activate function.
 *       The activation process starts the internal clocks, and the QSPI peripheral tries to read
 *       the status byte to check the busy bit. Reading the status byte is done in a simple poll
 *       and wait mechanism. If the busy bit is set, this indicates issues with the external memory
 *       device. As a result, transfer functions return -ETIMEDOUT.
 *
 * In case of issues:
 * - Check the connection.
 * - Make sure that the memory device does not perform other operations like erasing or writing.
 * - Check if there is a short circuit.
 *
 * @param[in] p_config  Pointer to the structure with the initial configuration.
 * @param[in] handler   Event handler provided by the user. If NULL, transfers
 *                      will be performed in blocking mode.
 * @param[in] p_context Pointer to context. Use in the interrupt handler.
 *
 * @warning On nRF5340, only the dedicated pins with @ref NRF_GPIO_PIN_SEL_PERIPHERAL configuration
 *          are supported. See the chapter <a href=@nRF5340pinAssignmentsURL>Pin assignments</a>
 *          in the Product Specification.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver is already initialized.
 * @retval -EINVAL   The pin configuration was incorrect.
 */
int nrfx_qspi_init(nrfx_qspi_config_t const * p_config,
                   nrfx_qspi_handler_t        handler,
                   void *                     p_context);

/**
 * @brief Function for reconfiguring the QSPI driver instance.
 *
 * @param[in] p_config Pointer to the structure with the configuration.
 *
 * @warning The function deactivates the peripheral instance. The activation is done during the first
 *          transfer after reconfiguration or when calling @ref nrfx_qspi_activate function.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EBUSY       The driver is during transaction.
 * @retval -ETIMEDOUT   External memory is busy or there are connection issues.
 * @retval -EINPROGRESS The driver is uninitialized.
 * @retval -EINVAL      The pin configuration was incorrect.
 */
int nrfx_qspi_reconfigure(nrfx_qspi_config_t const * p_config);

/**
 * @brief Function for uninitializing the QSPI driver instance.
 *
 * @note If a custom instruction long transfer is ongoing when the function is called,
 *       the transfer will be interrupted.
 */
void nrfx_qspi_uninit(void);

/**
 * @brief Function for activating the QSPI driver instance.
 *
 * @param[in] wait True if activation is to be in blocking mode, false otherwise.
 *
 * @retval 0          The driver instance has been activated.
 * @retval -EALREADY  The driver is already activated.
 * @retval -ETIMEDOUT External memory is busy, or there are connection issues.
 */
int nrfx_qspi_activate(bool wait);

/**
 * @brief Function for deactivating the QSPI driver instance.
 *
 * @note If a custom instruction long transfer is ongoing when the function is called,
 *       the transfer will be interrupted.
 *
 * @retval 0      The driver instance has been activated.
 * @retval -EBUSY The driver is during transaction.
 */
int nrfx_qspi_deactivate(void);

/**
 * @brief Function for checking if the QSPI driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_qspi_init_check(void);

/**
 * @brief Function for reading data from the QSPI memory.
 *
 * @note If that is the first operation after activation of driver initialization has been triggered,
 *       the activation process starts the internal clocks and the QSPI peripheral tries to read
 *       the status byte to check the busy bit. Reading the status byte is done in a simple poll
 *       and wait mechanism. If the busy bit is set, this indicates that the memory may not be ready yet.
 *       As a result, the function returns -ETIMEDOUT.
 *
 * Write, read, and erase operations check memory device busy state before starting the operation.
 * If the memory is busy, the resulting action depends on the mode in which the read operation is used:
 *  - blocking mode (without handler) - a delay occurs until the last operation runs and
 *    until the operation data is being read.
 *  - interrupt mode (with handler) - event emission occurs after the last operation
 *    and reading of data are finished.
 * In interrupt mode read operations can be double-buffered by calling the function again.
 * To utilize double-buffering feature, @ref NRF_QSPI_TASK_READSTART needs to be triggered
 * on @ref NRF_QSPI_EVENT_READY externally (for example by using the PPI/DPPI).
 *
 * @param[out] p_rx_buffer      Pointer to the receive buffer.
 * @param[in]  rx_buffer_length Size of the data to read.
 * @param[in]  src_address      Address in memory to read from.
 *
 * @retval 0          The operation was successful (blocking mode) or commissioned (handler mode).
 * @retval -EBUSY     The driver currently handles another operation.
 * @retval -ETIMEDOUT The external memory is busy, or there are connection issues.
 * @retval -EACCES    The provided buffer is not placed in the Data RAM region
 *                    or its address is not aligned to a 32-bit word.
 * @retval -EPERM     The operation could trigger nRF5340 anomaly 159 due to the current
 *                    configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_read(void *   p_rx_buffer,
                   size_t   rx_buffer_length,
                   uint32_t src_address);

/**
 * @brief Function for writing data to QSPI memory.
 *
 * @note Refer to the note for @ref nrfx_qspi_read.
 *
 * Write, read, and erase operations check memory device busy state before starting the operation.
 * If the memory is busy, the resulting action depends on the mode in which the write operation is used:
 *  - blocking mode (without handler) - a delay occurs until the last operation runs or
 *    until the operation data is being sent.
 *  - interrupt mode (with handler) - event emission occurs after the last operation
 *    and sending of operation data are finished.
 * To manually control operation execution in the memory device, use @ref nrfx_qspi_mem_busy_check
 * after executing the write function.
 * Remember that an incoming event signalizes only that data was sent to the memory device and the peripheral
 * before the write operation checked if memory was busy.
 * In interrupt mode write operations can be double-buffered by calling the function again.
 * To utilize double-buffering feature, @ref NRF_QSPI_TASK_WRITESTART needs to be triggered
 * on @ref NRF_QSPI_EVENT_READY externally (for example by using the PPI/DPPI).
 *
 * @param[in] p_tx_buffer      Pointer to the writing buffer.
 * @param[in] tx_buffer_length Size of the data to write.
 * @param[in] dst_address      Address in memory to write to.
 *
 * @retval 0          The operation was successful (blocking mode) or commissioned (handler mode).
 * @retval -EBUSY     The driver currently handles other operation.
 * @retval -ETIMEDOUT The external memory is busy, or there are connection issues.
 * @retval -EACCES    The provided buffer is not placed in the Data RAM region
 *                    or its address is not aligned to a 32-bit word.
 * @retval -EPERM     The operation could trigger nRF5340 anomaly 159 due to the current
 *                    configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_write(void const * p_tx_buffer,
                    size_t       tx_buffer_length,
                    uint32_t     dst_address);

/**
 * @brief Function for starting erasing of one memory block - 4KB, 64KB, or the whole chip.
 *
 * @note Refer to the note for @ref nrfx_qspi_read.
 *
 * Write, read, and erase operations check memory device busy state before starting the operation.
 * If the memory is busy, the resulting action depends on the mode in which the erase operation is used:
 *  - blocking mode (without handler) - a delay occurs until the last operation runs or
 *    until the operation data is being sent.
 *  - interrupt mode (with handler) - event emission occurs after the last operation
 *    and sending of operation data are finished.
 * To manually control operation execution in the memory device, use @ref nrfx_qspi_mem_busy_check
 * after executing the erase function.
 * Remember that an incoming event signalizes only that data was sent to the memory device and the periheral
 * before the erase operation checked if memory was busy.
 *
 * @param[in] length        Size of data to erase. See @ref nrf_qspi_erase_len_t.
 * @param[in] start_address Memory address to start erasing. If chip erase is performed, address
 *                          field is ommited.
 *
 * @retval 0          The operation was successful (blocking mode) or commissioned (handler mode).
 * @retval -EBUSY     The driver currently handles another operation.
 * @retval -ETIMEDOUT The external memory is busy, or there are connection issues.
 * @retval -EACCES    The provided start address is not aligned to a 32-bit word.
 * @retval -EPERM     The operation could trigger nRF5340 anomaly 159 due to the current
 *                    configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_erase(nrf_qspi_erase_len_t length,
                    uint32_t             start_address);

/**
 * @brief Function for starting an erase operation of the whole chip.
 *
 * @note Refer to the note for @ref nrfx_qspi_read.
 *
 * @retval 0          The operation was successful (blocking mode) or commissioned (handler mode).
 * @retval -EBUSY     The driver currently is handling another operation.
 * @retval -ETIMEDOUT The external memory is busy, or there are connection issues.
 * @retval -EPERM     The operation could trigger nRF5340 anomaly 159 due to the current
 *                    configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_chip_erase(void);

/**
 * @brief Function for getting the extended event associated with finished operation.
 *
 * @return Pointer to the extended event associated with finished operation.
 */
nrfx_qspi_evt_ext_t const * nrfx_qspi_event_extended_get(void);

/**
 * @brief Function for checking whether any write or read data transfer is buffered.
 *
 * @return True if there is a transfer buffered, false otherwise.
 */
bool nrfx_qspi_xfer_buffered_check(void);

/**
 * @brief Function for getting the current driver status and status byte of memory device with
 *        testing WIP (write in progress) bit.
 *
 * @retval 0      The driver and memory are ready to handle a new operation.
 * @retval -EBUSY The driver currently is handling another operation.
 * @retval -EPERM The operation could trigger nRF5340 anomaly 159 due to the current
 *                configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_mem_busy_check(void);

/**
 * @brief Function for signaling premature operation timeout.
 *
 * The function provides a mechanism that can cause premature timeout when the driver is waiting for
 * the READY event. This allows to use external source of the timeout. If the driver is initialized
 * with a handler, it will not process the event generated for the transfer.
 */
void nrfx_qspi_timeout_signal(void);

/**
 * @brief Function for sending operation code, sending data, and receiving data from the memory device.
 *
 * @note Refer to the note for @ref nrfx_qspi_read.
 *
 * Use this function to transfer configuration data to memory and to receive data from memory.
 * Pointers can be addresses from flash memory.
 * This function is a synchronous function and should be used only if necessary.
 *
 * @note Please note that the @ref NRFX_QSPI_DEFAULT_CINSTR macro provides default values
 *       for the @p io2_level and @p io3_level fields that cause the IO2 and IO3 lines
 *       to be kept low during the custom instruction transfer. Such configuration may not
 *       be suitable in certain circumstances and memory devices can interpret such levels
 *       of those lines as active WP# and HOLD#/RESET# signals, respectively. Hence, it is
 *       safer to use a configuration that will keep the lines high during the transfer.
 *
 * @param[in]  p_config    Pointer to the structure with opcode and transfer configuration.
 * @param[in]  p_tx_buffer Pointer to the array with data to send. Can be NULL if only opcode is transmitted.
 * @param[out] p_rx_buffer Pointer to the array for data to receive. Can be NULL if there is nothing to receive.
 *
 * @retval 0          The operation was successful.
 * @retval -ETIMEDOUT The external memory is busy, or there are connection issues.
 * @retval -EBUSY     The driver currently handles other operation.
 * @retval -EPERM     The operation could trigger nRF5340 anomaly 159 due to the current
 *                    configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_cinstr_xfer(nrf_qspi_cinstr_conf_t const * p_config,
                          void const *                   p_tx_buffer,
                          void *                         p_rx_buffer);

/**
 * @brief Function for sending operation code and data to the memory device with simpler configuration.
 *
 * @note Refer to the note for @ref nrfx_qspi_read.
 *
 * Use this function to transfer configuration data to memory and to receive data from memory.
 * This function is a synchronous function and should be used only if necessary.
 *
 * @param[in] opcode      Operation code. Sending first.
 * @param[in] length      Length of the data to send and opcode. See @ref nrf_qspi_cinstr_len_t.
 * @param[in] p_tx_buffer Pointer to input data array.
 *
 * @retval 0          The operation was successful.
 * @retval -EBUSY     The driver currently handles another operation.
 * @retval -ETIMEDOUT The external memory is busy, or there are connection issues.
 * @retval -EPERM     The operation could trigger nRF5340 anomaly 159 due to the current
 *                    configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_cinstr_quick_send(uint8_t               opcode,
                                nrf_qspi_cinstr_len_t length,
                                void const *          p_tx_buffer);

/**
 * @brief Function for starting the custom instruction long frame mode.
 *
 * @note Refer to the note for @ref nrfx_qspi_read.
 *
 * The long frame mode is a mechanism that allows for arbitrary byte length custom instructions.
 * Use this function to initiate a custom transaction by sending custom instruction opcode.
 * To send and receive data, use @ref nrfx_qspi_lfm_xfer.
 *
 * @note Please note that the @ref NRFX_QSPI_DEFAULT_CINSTR macro provides default values
 *       for the @p io2_level and @p io3_level fields that cause the IO2 and IO3 lines
 *       to be kept low during the custom instruction transfer. Such configuration may not
 *       be suitable in certain circumstances and memory devices can interpret such levels
 *       of those lines as active WP# and HOLD#/RESET# signals, respectively. Hence, it is
 *       safer to use a configuration that will keep the lines high during the transfer.
 *
 * @param[in] p_config Pointer to the structure with custom instruction opcode and transfer
 *                     configuration. Transfer length must be set to @ref NRF_QSPI_CINSTR_LEN_1B.
 *
 * @retval 0          Operation was successful.
 * @retval -EBUSY     Driver currently handles other operation.
 * @retval -ETIMEDOUT The external memory is busy, or there are connection issues.
 * @retval -EPERM     The operation could trigger nRF5340 anomaly 159 due to the current
 *                    configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_lfm_start(nrf_qspi_cinstr_conf_t const * p_config);

/**
 * @brief Function for sending and receiving data in the custom instruction long frame mode.
 *
 * @note Refer to the note for @ref nrfx_qspi_read.
 *
 * Both specified buffers must be at least @p transfer_length bytes in size.
 *
 * @param[in]  p_tx_buffer     Pointer to the array with data to send.
 *                             Can be NULL if there is nothing to send.
 * @param[out] p_rx_buffer     Pointer to the array for receiving data.
 *                             Can be NULL if there is nothing to receive.
 * @param[in]  transfer_length Number of bytes to send and receive.
 * @param[in]  finalize        True if custom instruction long frame mode is to be finalized
 *                             after this transfer.
 *
 * @retval 0          Operation was successful.
 * @retval -ETIMEDOUT External memory is busy or there are connection issues.
 *                    Long frame mode becomes deactivated.
 * @retval -EPERM     The operation could trigger nRF5340 anomaly 159 due to the current
 *                    configuration of clocks. Refer to the errata document for more information.
 */
int nrfx_qspi_lfm_xfer(void const * p_tx_buffer,
                       void *       p_rx_buffer,
                       size_t       transfer_length,
                       bool         finalize);

#if NRF_QSPI_HAS_XIP_ENC || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the XIP encryption.
 *
 * @param[in] p_config XIP encryption configuration structure.
 *                     To disable encryption, pass NULL pointer as argument.
 *
 * @retval 0      Operation was successful.
 * @retval -EBUSY Driver currently handles other operation.
 */
int nrfx_qspi_xip_encrypt(nrf_qspi_encryption_t const * p_config);
#endif

#if NRF_QSPI_HAS_DMA_ENC || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the EasyDMA encryption.
 *
 * @param[in] p_config DMA encryption configuration structure.
 *                     To disable encryption, pass NULL pointer as argument.
 *
 * @retval 0      Operation was successful.
 * @retval -EBUSY Driver currently handles other operation.
 */
int nrfx_qspi_dma_encrypt(nrf_qspi_encryption_t const * p_config);
#endif

/** @} */


void nrfx_qspi_irq_handler(void);


#ifdef __cplusplus
}
#endif

#endif // NRFX_QSPI_H__
