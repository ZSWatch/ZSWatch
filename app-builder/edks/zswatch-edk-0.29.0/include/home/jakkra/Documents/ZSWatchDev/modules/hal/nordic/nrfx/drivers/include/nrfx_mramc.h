/*
 * Copyright (c) 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_MRAMC_H__
#define NRFX_MRAMC_H__

#include <nrfx.h>
#include <haly/nrfy_mramc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_mramc MRAMC driver
 * @{
 * @ingroup nrf_mramc
 * @brief   Magnetoresistive Random Access Memory Controller (MRAMC) peripheral driver.
 */

/**
 * @brief Macro for mapping relative MRAMC offset to absolute address
 *
 * @param[in] offset Offset to map.
 */
#define NRFX_MRAMC_MAP_TO_ADDR(offset) (NRFY_MRAMC_MRAM_BASE_ADDRESS + (offset))

/** @brief Configuration structure of the MRAMC driver instance. */
typedef struct
{
    nrf_mramc_config_t              config;          ///< Mode of MRAMC configuration.
    nrf_mramc_readynext_timeout_t   preload_timeout; ///< Ready next timeout value for waiting for a next write.
    nrf_mramc_power_autopowerdown_t powerdown;       ///< MRAMC powerdown configuration.
    nrf_mramc_lowavgcurr_t          lowavgcurr;      /**< Preload timeout value for low average current
                                                      *   in case of read, write, and erase. */
    uint8_t                         irq_priority;    ///< Interrupt priority.
} nrfx_mramc_config_t;

#if NRF_MRAMC_HAS_CONFIG_DISABLEECC || defined(__NRFX_DOXYGEN__)
/** @brief Symbol to set DISABLEECC default configuration. */
#define NRFX_MRAMC_CONFIG_DISABLEECC .disable_ecc = true,
#else
#define NRFX_MRAMC_CONFIG_DISABLEECC
#endif

/**
 * @brief MRAMC driver default configuration.
 *
 * This configuration sets up MRAMC with the following options:
 * - Write mode disabled
 * - Erase mode disabled
 * - ECC enabled (if present)
 * - Preload timeout value: 0x0
 * - Write to the MRAM backed-to-backed on the next preload timeout in direct mode enabled
 * - Automatic power-down feature disabled
 * - Entering power-down mode when the timeout happens disabled
 * - Access timeout: 0x0
 * - Low average current read, write, and erase preload timeout values: 0x0
 */
#define NRFX_MRAMC_DEFAULT_CONFIG()                                   \
{                                                                     \
    .config                 = {                                       \
        .mode_write         = NRF_MRAMC_MODE_WRITE_DISABLE,           \
        .mode_erase         = NRF_MRAMC_MODE_ERASE_DISABLE,           \
        NRFX_MRAMC_CONFIG_DISABLEECC                                  \
    },                                                                \
    .preload_timeout        = {                                       \
        .value              = NRF_MRAMC_READYNEXTTIMEOUT_DEFAULT,     \
        .direct_write       = true,                                   \
    },                                                                \
    .powerdown              = {                                       \
        .enable             = false,                                  \
        .power_down_cfg     = false,                                  \
        .timeout_value      = NRF_MRAMC_AUTOPOWERDOWN_TIMEOUT_MIN,    \
    },                                                                \
    .lowavgcurr            = {                                        \
        .read               = NRF_MRAMC_LOWAVGCURR_READ_MIN,          \
        .write              = NRF_MRAMC_LOWAVGCURR_WRITE_MIN,         \
        .erase              = NRF_MRAMC_LOWAVGCURR_ERASE_MIN,         \
    },                                                                \
    .irq_priority           = NRFX_MRAMC_DEFAULT_CONFIG_IRQ_PRIORITY, \
}

/**
 * @brief MRAMC driver event handler type.
 *
 * @param[in] event_type MRAMC event.
 */
typedef void (* nrfx_mramc_evt_handler_t)(nrf_mramc_event_t const event_type);

/**
 * @brief Function for checking if the address is valid.
 *
 * @param[in] addr         Address to be checked.
 * @param[in] uicr_allowed If true, the UICR area is considered valid.
 *
 * @retval true Address is valid.
 * @retval false Address is invalid.
 */
bool nrfx_mramc_valid_address_check(uint32_t addr, bool uicr_allowed);

/**
 * @brief Function for checking if the address and size fit in MRAM memory.
 *
 * @param[in] addr         Address to be checked.
 * @param[in] uicr_allowed If true, the UICR area is considered valid.
 * @param[in] bytes        Number of bytes to be checked.
 *
 * @retval true Address and size fit in memory.
 * @retval false Address and size not fitting in memory.
 */
bool nrfx_mramc_fits_memory_check(uint32_t addr, bool uicr_allowed, uint32_t bytes);

/**
 * @brief Function for erasing the whole MRAM memory.
 *
 * @note All user code and UICR will be erased.
 */
void nrfx_mramc_all_erase(void);

/**
 * @brief Function for writing a 32-bit word to MRAM.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] address Address to where data is to be written. Must be word-aligned.
 * @param[in] value   Value to be written.
 */
void nrfx_mramc_word_write(uint32_t address, uint32_t value);

/**
 * @brief Function for writing consecutive 32-bit words to MRAM.
 *
 * Function uses direct write mode to write to MRAM directly.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] address   Address to where data is to be written. Must be word-aligned.
 * @param[in] src       Pointer to data to be copied. Must be word-aligned.
 * @param[in] num_words Number of words to be written.
 */
void nrfx_mramc_words_write(uint32_t address, void const * src, uint32_t num_words);

/**
 * @brief Function for reading consecutive bytes from MRAM.
 *
 * @param[out] dst       Pointer to the buffer to store the data.
 * @param[in]  address   Address of the first byte to be read. Must be word-aligned.
 * @param[in]  num_bytes Number of bytes to be read.
 */
void nrfx_mramc_buffer_read(void * dst, uint32_t address, uint32_t num_bytes);

/**
 * @brief Function for setting the MRAMC write mode.
 *
 * @param[in] write_mode One of the enum in @ref nrf_mramc_mode_write_t.
 */
void nrfx_mramc_config_write_mode_set(nrf_mramc_mode_write_t write_mode);

/**
 * @brief Function for setting the MRAMC erase mode.
 *
 * @param[in] erase_mode One of the enum in @ref nrf_mramc_mode_erase_t.
 */
void nrfx_mramc_config_erase_mode_set(nrf_mramc_mode_erase_t erase_mode);

/**
 * @brief Function for setting the MRAMC NVR page write and erase permission,
 *
 * @note: Setting enable allows @ref nrf_mramc_readynext_timeout_t diret_write
 *        and preload_timeout to minimum, @ref nrf_mramc_config_nvr_t PAGE[n] to
 *        be writtable and erasable.
 *
 * @param[in] enable True to enable write and erase permission of
 *                   CONFIGNVR.PAGE[n], false to disable.
 * @param[in] page   Page number of CONFIGNVR.PAGE[0...n].
 */
void nrfx_mramc_confignvr_perm_set(bool enable, uint8_t page);

/**
 * @brief Function for initializing the MRAMC driver instance.
 *
 * @param[in] p_config Pointer to the structure containing configuration.
 * @param[in] handler  Event handler provided by the user.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver has already been initialized.
 */
int nrfx_mramc_init(nrfx_mramc_config_t const * p_config,
                    nrfx_mramc_evt_handler_t    handler);

/**
 * @brief Function for reconfiguring the MRAMC driver instance.
 *
 * @param[in] p_config Pointer to the structure containing configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EINPROGRESS The driver is uninitialized.
 */
int nrfx_mramc_reconfigure(nrfx_mramc_config_t const * p_config);

/** @brief Function for uninitializing the MRAMC driver instance. */
void nrfx_mramc_uninit(void);

/**
 * @brief Function for getting the total MRAM size in bytes.
 *
 * @note The function will return @p FICR_INFO_MRAM_MRAM_Unspecified value
 *       if the total MRAM size cannot be determined based on the FICR data.
 *
 * @return MRAM total size in bytes.
 */
uint32_t nrfx_mramc_memory_size_get(void);

/**
 * @brief Function for erasing area in MRAM.
 *
 * @param[in] address Address to be erased.
 * @param[in] size    Size to erase in number of words.
 */
void nrfx_mramc_area_erase(uint32_t address, uint32_t size);

/**
 * @brief Function for reading a 32-bit word from the MRAM.
 *
 * Use this function in case accessing the MRAM gives the possibility
 * to run the code in an environment where the MRAM is simulated.
 *
 * @param[in] address Address of the word to be read.
 *
 * @return Value read from MRAM.
 */
NRFX_STATIC_INLINE uint32_t nrfx_mramc_word_read(uint32_t address);

/**
 * @brief Function for checking current MRAMC operation status.
 *
 * The status is updated for all MRAMC operations.
 *
 * @retval true  Current operation is completed, and MRAMC is ready.
 * @retval false MRAMC is busy.
 */
NRFX_STATIC_INLINE bool nrfx_mramc_ready_check(void);

#ifndef NRFX_DECLARE_ONLY

NRFX_STATIC_INLINE uint32_t nrfx_mramc_word_read(uint32_t address)
{
    return nrfy_mramc_word_read(address);
}

NRFX_STATIC_INLINE bool nrfx_mramc_ready_check(void)
{
    return nrfy_mramc_ready_get(NRF_MRAMC);
}

#endif // NRFX_DECLARE_ONLY

/** @} */

void nrfx_mramc_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif // NRFX_MRAMC_H__
