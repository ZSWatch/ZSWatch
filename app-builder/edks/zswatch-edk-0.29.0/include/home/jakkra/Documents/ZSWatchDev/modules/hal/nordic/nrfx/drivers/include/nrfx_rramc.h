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

#ifndef NRFX_RRAMC_H__
#define NRFX_RRAMC_H__

#include <nrfx.h>
#include <haly/nrfy_rramc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_rramc RRAMC driver
 * @{
 * @ingroup nrf_rramc
 * @brief   Resistive Random Access Memory Controller (RRAMC) peripheral driver.
 */

/** @brief Configuration structure of the RRAMC driver instance. */
typedef struct
{
    bool     mode_write;             ///< True if write mode is to be enabled, false otherwise.
    uint8_t  write_buff_size;        ///< Size of write buffer. If set to 0, buffering is disabled.
    uint16_t preload_timeout;        ///< Preload value expressed in clock cycles.
    bool     preload_timeout_enable; ///< True if writing to RRAM is to be triggered on the next timeout, false otherwise.
    uint16_t access_timeout;         ///< Access timeout used either for going into standby power mode or to remain active on wake up, expressed in clock cycles.
    bool     abort_on_pof;           ///< True if the current RRAM write operation is to be aborted on the power failure, false otherwise.
    uint8_t  irq_priority;           ///< Interrupt priority.
} nrfx_rramc_config_t;

/**
 * @brief RRAMC driver default configuration.
 *
 * This configuration sets up RRAMC with the following options:
 * - Write mode disabled
 * - Preload timeout value: 0x80
 * - Write to the RRAM on the next timeout enabled
 * - Access timeout: 0x100
 * - Write operation is to be aborted on the power failure
 *
 * @param[in] _write_buff_size Size of write buffer.
 */
#define NRFX_RRAMC_DEFAULT_CONFIG(_write_buff_size)                   \
{                                                                     \
    .mode_write             = false,                                  \
    .write_buff_size        = _write_buff_size,                       \
    .preload_timeout        = 0x80,                                   \
    .preload_timeout_enable = true,                                   \
    .access_timeout         = 0x100,                                  \
    .abort_on_pof           = true,                                   \
    .irq_priority           = NRFX_RRAMC_DEFAULT_CONFIG_IRQ_PRIORITY, \
}

/**
 * @brief RRAMC driver event handler type.
 *
 * @param[in] event_type RRAMC event.
*/
typedef void (* nrfx_rramc_evt_handler_t)(nrf_rramc_event_t const event_type);

/**
 * @brief Function for erasing the whole RRAM memory.
 *
 * @note All user code and UICR will be erased.
 */
void nrfx_rramc_all_erase(void);

/**
 * @brief Function for writing a single byte to RRAM.
 *
 * To determine if the last RRAM write operation has been completed,
 * use @ref nrfx_rramc_ready_check(). The status is not updated during writes to write-buffer.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] address Address to where data is to be written.
 * @param[in] value   Value to be written.
 */
void nrfx_rramc_byte_write(uint32_t address, uint8_t value);

/**
 * @brief Function for writing consecutive bytes to RRAM.
 *
 * To determine if the last RRAM write operation has been completed,
 * use @ref nrfx_rramc_ready_check(). The status is not updated during writes to write-buffer.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] address   Address to where data is to be written.
 * @param[in] src       Pointer to data to be copied.
 * @param[in] num_bytes Number of bytes to be written.
 */
void nrfx_rramc_bytes_write(uint32_t address, void const * src, uint32_t num_bytes);

/**
 * @brief Function for writing a 32-bit word to RRAM.
 *
 * To determine if the last RRAM write operation has been completed,
 * use @ref nrfx_rramc_ready_check(). The status is not updated during writes to write-buffer.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] address Address to where data is to be written. Must be word-aligned.
 * @param[in] value   Value to be written.
 */
void nrfx_rramc_word_write(uint32_t address, uint32_t value);

/**
 * @brief Function for writing consecutive 32-bit words to RRAM.
 *
 * To determine if the last RRAM write operation has been completed,
 * use @ref nrfx_rramc_ready_check(). The status is not updated during writes to write-buffer.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] address   Address to where data is to be written. Must be word-aligned.
 * @param[in] src       Pointer to data to be copied. Must be word-aligned.
 * @param[in] num_words Number of words to be written.
 */
void nrfx_rramc_words_write(uint32_t address, void const * src, uint32_t num_words);

/**
 * @brief Function for enabling write mode and setting size of write buffer.
 *
 * @param[in] enable          True if write mode is to be enabled, false otherwise.
 * @param[in] write_buff_size Size of write buffer. If set to 0, buffering is disabled.
 */
void nrfx_rramc_write_enable_set(bool enable, uint32_t write_buff_size);

/**
 * @brief Function for checking if write mode is enabled.
 *
 * @return True if write mode is enabled, false otherwise.
 */
bool nrfx_rramc_write_enable_check(void);

/**
 * @brief Function for initializing the RRAMC driver instance.
 *
 * @param[in] p_config Pointer to the structure containing configuration.
 * @param[in] handler  Event handler provided by the user.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver has already been initialized.
 */
int nrfx_rramc_init(nrfx_rramc_config_t const * p_config,
                    nrfx_rramc_evt_handler_t    handler);

/**
 * @brief Function for reconfiguring the RRAMC driver instance.
 *
 * @param[in] p_config Pointer to the structure containing configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EINPROGRESS The driver is uninitialized.
 */
int nrfx_rramc_reconfigure(nrfx_rramc_config_t const * p_config);

/** @brief Function for uninitializing the RRAMC driver instance. */
void nrfx_rramc_uninit(void);

/**
 * @brief Function for getting the total RRAM size in bytes.
 *
 * @note The function will return @p FICR_INFO_RRAM_RRAM_Unspecified value
 *       if the total RRAM size cannot be determined based on the FICR data.
 *
 * @return RRAM total size in bytes.
 */
uint32_t nrfx_rramc_memory_size_get(void);

/** @brief Function for committing write buffer. */
void nrfx_rramc_write_buffer_commit(void);

/** @brief Function for waking up the RRAMC. */
void nrfx_rramc_wake_up(void);

/**
 * @brief Function for checking if RRAMC write buffer is empty.
 *
 * @retval true  Buffer is empty.
 * @retval false Buffer is not empty.
 */
bool nrfx_rramc_write_buffer_empty_check(void);

/**
 * @brief Function for reading a word from the OTP in UICR.
 *
 * OTP is a region of the UICR present in some chips. This function must be used
 * to read word data from this region since unaligned accesses are not
 * available on the OTP RRAM area.
 *
 * @param[in] index Address (index) in OTP table from which a word is to be read.
 *
 * @retval The contents at @p index.
 */
NRFX_STATIC_INLINE uint32_t nrfx_rramc_otp_word_read(uint32_t index);

/**
 * @brief Function for writing a 32-bit word at index position to OTP region in UICR.
 *
 * The OTP can only be written once after each Erase All is performed.
 * This function checks if the value currently residing at the specified index can
 * be transformed to the desired value. If yes, then performs the write operation.
 *
 * @param[in] index Address (index) in OTP table to which a word it to be written.
 * @param[in] value Value to be written.
 *
 * @retval true  Word can be written into the specified OTP index address.
 * @retval false Word cannot be written into the specified OTP index address.
 *               Erase UICR or change index address.
 */
NRFX_STATIC_INLINE bool nrfx_rramc_otp_word_write(uint32_t index, uint32_t value);

/**
 * @brief Function for reading a byte from the RRAM.
 *
 * Use this function in case accessing the RRAM gives the possibility
 * to run the code in an environment where the flash is simulated.
 *
 * @param[in] address Address of the byte to be read.
 *
 * @return Value read from RRAM.
 */
NRFX_STATIC_INLINE uint8_t nrfx_rramc_byte_read(uint32_t address);

/**
 * @brief Function for reading a 32-bit word from the RRAM.
 *
 * Use this function in case accessing the RRAM gives the possibility
 * to run the code in an environment where the flash is simulated.
 *
 * @param[in] address Address of the word to be read.
 *
 * @return Value read from RRAM.
 */
NRFX_STATIC_INLINE uint32_t nrfx_rramc_word_read(uint32_t address);

/**
 * @brief Function for reading a given number of bytes from the RRAM into the specified buffer.
 *
 * @param[out] dst       Pointer to the buffer to store the data.
 * @param[in]  address   Address of the first byte to be read.
 * @param[in]  num_bytes Number of bytes to be read.
 *
 */
NRFX_STATIC_INLINE void nrfx_rramc_buffer_read(void * dst, uint32_t address, uint32_t num_bytes);

/**
 * @brief Function for checking current RRAMC operation status.
 *
 * The status is updated for all RRAMC operations except during read and writes to write-buffer.
 *
 * @retval true  Current operation is completed, and RRAMC is ready.
 * @retval false RRAMC is busy.
 */
NRFX_STATIC_INLINE bool nrfx_rramc_ready_check(void);

#ifndef NRFX_DECLARE_ONLY

NRFX_STATIC_INLINE uint32_t nrfx_rramc_otp_word_read(uint32_t index)
{
    return nrfy_rramc_otp_word_read(index);
}

NRFX_STATIC_INLINE bool nrfx_rramc_otp_word_write(uint32_t index, uint32_t value)
{
    return nrfy_rramc_otp_word_write(NRF_RRAMC, index, value);
}

NRFX_STATIC_INLINE uint8_t nrfx_rramc_byte_read(uint32_t address)
{
    return nrfy_rramc_byte_read(address);
}

NRFX_STATIC_INLINE uint32_t nrfx_rramc_word_read(uint32_t address)
{
    return nrfy_rramc_word_read(address);
}

NRFX_STATIC_INLINE void nrfx_rramc_buffer_read(void * dst, uint32_t address, uint32_t num_bytes)
{
    nrfy_rramc_buffer_read(dst, address, num_bytes);
}

NRFX_STATIC_INLINE bool nrfx_rramc_ready_check(void)
{
    return nrfy_rramc_ready_check(NRF_RRAMC);
}

#endif // NRFX_DECLARE_ONLY

/** @} */

void nrfx_rramc_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif // NRFX_RRAMC_H__
