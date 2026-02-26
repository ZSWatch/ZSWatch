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

#ifndef NRFY_MRAMC_H__
#define NRFY_MRAMC_H__

#include <nrfx.h>
#include <hal/nrf_mramc.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_mramc_event_enabled_clear(NRF_MRAMC_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_mramc_event_t event);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_mramc_event_handle(NRF_MRAMC_Type *  p_reg,
                                                               uint32_t          mask,
                                                               nrf_mramc_event_t event);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_mramc_events_process(NRF_MRAMC_Type * p_reg,
                                                                 uint32_t         mask);

NRFY_STATIC_INLINE void __nrfy_internal_mramc_config_get(NRF_MRAMC_Type const * p_reg,
                                                         nrf_mramc_config_t *   p_config);

NRFY_STATIC_INLINE void __nrfy_internal_mramc_config_set(NRF_MRAMC_Type *           p_reg,
                                                         nrf_mramc_config_t const * p_config);

NRFY_STATIC_INLINE void __nrfy_internal_mramc_word_write(uint32_t addr, uint32_t value);

/**
 * @defgroup nrfy_mramc MRAMC HALY
 * @{
 * @ingroup nrf_mramc
 * @brief   Hardware access layer with cache and barrier support for managing the MRAMC peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/** @brief Value representing how many bits in a byte. */
#define NRFY_MRAMC_BITS_IN_BYTE 8

/** @brief Value representing the number of bytes in a word. */
#define NRFY_MRAMC_BYTES_IN_WORD 4

/**
 * @brief Value representing the number of words in bus width.
 *
 * MRAM register bus width is 128-bits,
 * which means that 4 words can be written in one bus width.
 */
#define NRFY_MRAMC_WORDS_IN_BUS_SIZE \
NRF_MRAMC_BUS_SIZE / (NRFY_MRAMC_BITS_IN_BYTE * NRFY_MRAMC_BYTES_IN_WORD)

/** @brief MRAMC erase value. */
#define NRFY_MRAMC_WORD_AFTER_ERASED UINT32_MAX

/** @brief Value representing resistive random access memory (MRAM) base address. */
#define NRFY_MRAMC_MRAM_BASE_ADDRESS NRF_MEMORY_FLASH_BASE

/** @brief Default value for waiting for a next write. */
#define NRFY_MRAMC_READYNEXTTIMEOUT_DEFAULT NRF_MRAMC_READYNEXTTIMEOUT_DEFAULT

#if NRF_MRAMC_HAS_POWER_MASK || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_MRAMC_HAS_POWER_MASK} */
#define NRFY_MRAMC_HAS_POWER_MASK 1
#else
#define NRFY_MRAMC_HAS_POWER_MASK 0
#endif

#if NRF_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT} */
#define NRFY_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT 1
#else
#define NRFY_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT 0
#endif

#if NRF_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT || defined(__NRFX_DOXYGEN__)
/** @refhal{NRF_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT} */
#define NRFY_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT 1
#else
#define NRFY_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT 0
#endif

/** @brief MRAMC configuration structure. */
typedef struct
{
    nrf_mramc_config_t              config;          ///< Mode of MRAMC configuration.
    nrf_mramc_readynext_timeout_t   preload_timeout; ///< Ready next timeout value for waiting for a next write.
    nrf_mramc_power_autopowerdown_t powerdown;       ///< MRAMC powerdown configuration.
    nrf_mramc_lowavgcurr_t          lowavgcurr;      ///< Preload timeout value for low average current
                                                     ///  in case of read, write, and erase.
} nrfy_mramc_config_t;

/**
 * @brief Function for configuring the MRAMC.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure of configuration of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_mramc_configure(NRF_MRAMC_Type *            p_reg,
                                             nrfy_mramc_config_t const * p_config)
{
    __nrfy_internal_mramc_config_set(p_reg, &p_config->config);
    nrf_mramc_readynext_timeout_set(p_reg, &p_config->preload_timeout);
    nrf_mramc_power_init_set(p_reg, NRF_MRAMC_POWER_INIT_MODE_UP);
    nrf_mramc_lowavgcurr_set(p_reg, &p_config->lowavgcurr);
    nrf_barrier_w();
}

/**
 * @brief Function for initializing the specified MRAMC interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts associated with the event mask are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_mramc_int_init(NRF_MRAMC_Type * p_reg,
                                            uint32_t         mask,
                                            uint8_t          irq_priority,
                                            bool             enable)
{
    __nrfy_internal_mramc_event_enabled_clear(p_reg, mask, NRF_MRAMC_EVENT_ACCESSERR);
    __nrfy_internal_mramc_event_enabled_clear(p_reg, mask, NRF_MRAMC_EVENT_READY);
    __nrfy_internal_mramc_event_enabled_clear(p_reg, mask, NRF_MRAMC_EVENT_READYNEXT);
    __nrfy_internal_mramc_event_enabled_clear(p_reg, mask, NRF_MRAMC_EVENT_ECCERROR);
    __nrfy_internal_mramc_event_enabled_clear(p_reg, mask, NRF_MRAMC_EVENT_ECCERRORCORR);
    __nrfy_internal_mramc_event_enabled_clear(p_reg, mask, NRF_MRAMC_EVENT_TRIMCONFIGREQ);

    nrf_barrier_w();
    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_mramc_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the MRAMC interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_mramc_int_uninit(NRF_MRAMC_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified MRAMC events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_mramc_events_process(NRF_MRAMC_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = __nrfy_internal_mramc_events_process(p_reg, mask);
    return evt_mask;
}

/**
 * @brief Function for writing a 32-bit word to MRAM.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] address Address to where data is to be written. Must be word-aligned.
 * @param[in] value   Value to be written.
 */
NRFY_STATIC_INLINE void nrfy_mramc_word_write(NRF_MRAMC_Type * p_reg,
                                              uint32_t         address,
                                              uint32_t         value)
{
    nrf_mramc_config_t mramc_config;
    nrf_mramc_config_t prev_mramc_config;

    __nrfy_internal_mramc_config_get(p_reg, &mramc_config);
    prev_mramc_config = mramc_config;
    mramc_config.mode_write = NRF_MRAMC_MODE_WRITE_DIRECT;

    __nrfy_internal_mramc_config_set(p_reg, &mramc_config);
    __nrfy_internal_mramc_word_write(address, value);

    nrf_barrier_r();
    while (!nrf_mramc_ready_get(p_reg))
    {}
    nrf_barrier_r();

    __nrfy_internal_mramc_config_set(p_reg, &prev_mramc_config);
}

/**
 * @brief Function for writing consecutive 32-bit words to MRAM.
 *
 * Function uses direct write mode to write to MRAM directly.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] address   Address to where data is to be written. Must be word-aligned.
 * @param[in] src       Pointer to data to be copied. Must be word-aligned.
 * @param[in] num_words Number of words to be written.
 */
NRFY_STATIC_INLINE void nrfy_mramc_words_write(NRF_MRAMC_Type * p_reg,
                                               uint32_t         address,
                                               void const *     src,
                                               uint32_t         num_words)
{
    nrf_mramc_config_t mramc_config;
    nrf_mramc_config_t prev_mramc_config;

    __nrfy_internal_mramc_config_get(p_reg, &mramc_config);
    prev_mramc_config = mramc_config;
    mramc_config.mode_write = NRF_MRAMC_MODE_WRITE_DIRECT;

    __nrfy_internal_mramc_config_set(p_reg, &mramc_config);
    for (uint32_t i = 0; i < num_words; i++)
    {
        __nrfy_internal_mramc_word_write(address + (NRFY_MRAMC_BYTES_IN_WORD * i),
                                         ((uint32_t const *)src)[i]);
        nrf_barrier_r();
        while (!nrf_mramc_ready_get(p_reg))
        {}
        nrf_barrier_r();
    }
    __nrfy_internal_mramc_config_set(p_reg, &prev_mramc_config);
}

/**
 * @brief Function for reading a 32-bit word from the MRAM.
 *
 * @param[in] address Address of the word to be read.
 *
 * @return Value read from MRAM.
 */
NRFY_STATIC_INLINE uint32_t nrfy_mramc_word_read(uint32_t address)
{
    nrf_barrier_r();
    uint32_t word = *(volatile uint32_t *)address;
    nrf_barrier_r();
    return word;
}

/**
 * @brief Function for reading a given number of bytes from the MRAM into the specified buffer.
 *
 * @param[out] dst       Pointer to the buffer to store the data.
 * @param[in]  address   Address of the first byte to be read.
 * @param[in]  num_bytes Number of bytes to be read.
 */
NRFY_STATIC_INLINE void nrfy_mramc_buffer_read(void *   dst,
                                               uint32_t address,
                                               uint32_t num_bytes)
{
    nrf_barrier_r();
    memcpy(dst, (void *)address, num_bytes);
    nrf_barrier_r();
}

/**
 * @brief Function for reading a word from the OTP in UICR.
 *
 * OTP is a region of the UICR present in some chips. This function must be used
 * to read word data from this region since unaligned accesses are not
 * available on the OTP MRAM area.
 *
 * @param[in] index Address (index) in OTP table from which a word is to be read.
 *
 * @retval The contents at @p index.
 */
NRFY_STATIC_INLINE uint32_t nrfy_mramc_otp_word_read(uint32_t index)
{
    NRFX_ASSERT(index < UICR_OTP_MaxCount);
#if !defined(NRF_TRUSTZONE_NONSECURE)
    nrf_barrier_r();
    uint32_t val32 = NRF_UICR->OTP[index];
    nrf_barrier_r();
    return val32;
#else
    return UINT32_MAX;
#endif
}

/**
 * @brief Function for setting CONFIGNVR read and write permission.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True to enable write and erase permission of
 *                   CONFIGNVR.PAGE[n], false to disable.
 * @param[in] page   Page number [0...n].
 */
NRFY_STATIC_INLINE void nrfy_mramc_confignvr_perm_set(NRF_MRAMC_Type * p_reg,
                                                      bool             enable,
                                                      uint8_t          page)
{
    nrf_mramc_config_nvr_t config_nvr = {
        .wen    = enable ? NRF_MRAMC_MODE_WRITE_DIRECT : NRF_MRAMC_MODE_WRITE_DISABLE,
        .een    = enable ? NRF_MRAMC_MODE_ERASE_WORD   : NRF_MRAMC_MODE_ERASE_DISABLE,
        .lock   = false,
#if NRFY_MRAMC_HAS_CONFIGNVR_PAGE_UPPER_PROTECT
        .uren   = enable,
        .uwen   = enable,
#endif
#if NRFY_MRAMC_HAS_CONFIGNVR_PAGE_LOWER_PROTECT
        .lren   = enable,
        .lwen   = enable,
        .lrsize = (uint8_t) (enable ? NRF_MRAMC_CONFIGNVR_PAGE_LRSIZE_MAX :
                            NRF_MRAMC_CONFIGNVR_PAGE_LRSIZE_MIN),
        .lwsize = (uint8_t) (enable ? NRF_MRAMC_CONFIGNVR_PAGE_LWSIZE_MAX :
                            NRF_MRAMC_CONFIGNVR_PAGE_LWSIZE_MIN),
#endif
    };
    nrf_mramc_config_nvr_set(p_reg, &config_nvr, page);
}

/** @refhal{nrf_mramc_event_clear} */
NRFY_STATIC_INLINE void nrfy_mramc_event_clear(NRF_MRAMC_Type * p_reg, nrf_mramc_event_t event)
{
    nrf_mramc_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_event_check} */
NRFY_STATIC_INLINE bool nrfy_mramc_event_check(NRF_MRAMC_Type const * p_reg,
                                               nrf_mramc_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_mramc_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_mramc_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mramc_event_address_get(NRF_MRAMC_Type const * p_reg,
                                                         nrf_mramc_event_t      event)
{
    return nrf_mramc_event_address_get(p_reg, event);
}

/** @refhal{nrf_mramc_int_enable} */
NRFY_STATIC_INLINE void nrfy_mramc_int_enable(NRF_MRAMC_Type * p_reg, uint32_t mask)
{
    nrf_mramc_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_int_disable} */
NRFY_STATIC_INLINE void nrfy_mramc_int_disable(NRF_MRAMC_Type * p_reg, uint32_t mask)
{
    nrf_mramc_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_mramc_int_enable_check(NRF_MRAMC_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_mramc_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_mramc_int_pending_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mramc_int_pending_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t pending = nrf_mramc_int_pending_get(p_reg);
    nrf_barrier_r();
    return pending;
}

/** @refhal{nrf_mramc_ready_get} */
NRFY_STATIC_INLINE bool nrfy_mramc_ready_get(NRF_MRAMC_Type * p_reg)
{
    nrf_barrier_r();
    bool ready = nrf_mramc_ready_get(p_reg);
    nrf_barrier_r();
    return ready;
}

/** @refhal{nrf_mramc_readynext_get} */
NRFY_STATIC_INLINE bool nrfy_mramc_readynext_get(NRF_MRAMC_Type * p_reg)
{
    nrf_barrier_r();
    bool readynext = nrf_mramc_readynext_get(p_reg);
    nrf_barrier_r();
    return readynext;
}

/** @refhal{nrf_mramc_ecc_get} */
NRFY_STATIC_INLINE void nrfy_mramc_ecc_get(NRF_MRAMC_Type const * p_reg,
                                           nrf_mramc_ecc_t *      p_data)
{
    nrf_barrier_r();
    nrf_mramc_ecc_get(p_reg, p_data);
    nrf_barrier_r();
}

/** @refhal{nrf_mramc_ecc_error_clear} */
NRFY_STATIC_INLINE void nrfy_mramc_ecc_error_clear(NRF_MRAMC_Type * p_reg)
{
    nrf_mramc_ecc_error_clear(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_ecc_corr_clear} */
NRFY_STATIC_INLINE void nrfy_mramc_ecc_corr_clear(NRF_MRAMC_Type * p_reg)
{
    nrf_mramc_ecc_corr_clear(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_config_set} */
NRFY_STATIC_INLINE void nrfy_mramc_config_set(NRF_MRAMC_Type *           p_reg,
                                              nrf_mramc_config_t const * p_config)
{
    __nrfy_internal_mramc_config_set(p_reg, p_config);
}

/** @refhal{nrf_mramc_config_get} */
NRFY_STATIC_INLINE void nrfy_mramc_config_get(NRF_MRAMC_Type const * p_reg,
                                              nrf_mramc_config_t *   p_config)
{
    __nrfy_internal_mramc_config_get(p_reg, p_config);
}

/** @refhal{nrf_mramc_autoreadmode_set} */
NRFY_STATIC_INLINE void nrfy_mramc_autoreadmode_set(NRF_MRAMC_Type * p_reg,
                                                   uint16_t         timeout)
{
    nrf_mramc_autoreadmode_set(p_reg, timeout);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_autoreadmode_get} */
NRFY_STATIC_INLINE uint16_t nrfy_mramc_autoreadmode_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    uint16_t auto_read_mode = nrf_mramc_autoreadmode_get(p_reg);
    nrf_barrier_r();
    return auto_read_mode;
}

/** @refhal{nrf_mramc_waitstates_set} */
NRFY_STATIC_INLINE void nrfy_mramc_waitstates_set(NRF_MRAMC_Type *               p_reg,
                                                  nrf_mramc_waitstates_t const * p_data)
{
    nrf_mramc_waitstates_set(p_reg, p_data);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_waitstates_get} */
NRFY_STATIC_INLINE void nrfy_mramc_waitstates_get(NRF_MRAMC_Type const *   p_reg,
                                                  nrf_mramc_waitstates_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_waitstates_get(p_reg, p_data);
    nrf_barrier_r();
}

/** @refhal{nrf_mramc_readynext_timeout_set} */
NRFY_STATIC_INLINE void nrfy_mramc_readynext_timeout_set(NRF_MRAMC_Type *                       p_reg,
                                                          nrf_mramc_readynext_timeout_t const * p_data)
{
    nrf_mramc_readynext_timeout_set(p_reg, p_data);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_readynext_timeout_get} */
NRFY_STATIC_INLINE void nrfy_mramc_readynext_timeout_get(NRF_MRAMC_Type const *           p_reg,
                                                          nrf_mramc_readynext_timeout_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_readynext_timeout_get(p_reg, p_data);
    nrf_barrier_r();
}

/** @refhal{nrf_mramc_lowavgcurr_set} */
NRFY_STATIC_INLINE void nrfy_mramc_lowavgcurr_set(NRF_MRAMC_Type *               p_reg,
                                                  nrf_mramc_lowavgcurr_t const * p_data)
{
    nrf_mramc_lowavgcurr_set(p_reg, p_data);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_lowavgcurr_get} */
NRFY_STATIC_INLINE void nrfy_mramc_lowavgcurr_get(NRF_MRAMC_Type const *   p_reg,
                                                  nrf_mramc_lowavgcurr_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_lowavgcurr_get(p_reg, p_data);
    nrf_barrier_r();
}

/** @refhal{nrf_mramc_power_init_set} */
NRFY_STATIC_INLINE void nrfy_mramc_power_init_set(NRF_MRAMC_Type *       p_reg,
                                                  nrf_mramc_power_init_t mode)
{
    nrf_mramc_power_init_set(p_reg, mode);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_power_init_get} */
NRFY_STATIC_INLINE nrf_mramc_power_init_t nrfy_mramc_power_init_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    nrf_mramc_power_init_t mode = nrf_mramc_power_init_get(p_reg);
    nrf_barrier_r();
    return mode;
}

/** @refhal{nrf_mramc_power_autopowerdown_set} */
NRFY_STATIC_INLINE void nrfy_mramc_power_autopowerdown_set(NRF_MRAMC_Type *                        p_reg,
                                                           nrf_mramc_power_autopowerdown_t const * p_data)
{
    nrf_mramc_power_autopowerdown_set(p_reg, p_data);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_power_autopowerdown_get} */
NRFY_STATIC_INLINE void nrfy_mramc_power_autopowerdown_get(NRF_MRAMC_Type const *            p_reg,
                                                           nrf_mramc_power_autopowerdown_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_power_autopowerdown_get(p_reg, p_data);
    nrf_barrier_r();
}

#if NRFY_MRAMC_HAS_POWER_MASK
/** @refhal{nrf_mramc_power_mask_set} */
NRFY_STATIC_INLINE void nrfy_mramc_power_mask_set(NRF_MRAMC_Type *               p_reg,
                                                  nrf_mramc_power_conf_t const * p_data)
{
    nrf_mramc_power_mask_set(p_reg, p_data);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_power_mask_get} */
NRFY_STATIC_INLINE void nrfy_mramc_power_mask_get(NRF_MRAMC_Type const *        p_reg,
                                                       nrf_mramc_power_conf_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_power_mask_get(p_reg, p_data);
    nrf_barrier_r();
}
#endif // NRFY_MRAMC_HAS_POWER_MASK

/** @refhal{nrf_mramc_power_status_get} */
NRFY_STATIC_INLINE nrf_mramc_power_status_t nrfy_mramc_power_status_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    nrf_mramc_power_status_t status = nrf_mramc_power_status_get(p_reg);
    nrf_barrier_r();
    return status;
}

#if NRFY_MRAMC_HAS_POWER_MASK
/** @refhal{nrf_mramc_powerup_ack_get} */
NRFY_STATIC_INLINE void nrfy_mramc_powerup_ack_get(NRF_MRAMC_Type const *   p_reg,
                                                   nrf_mramc_power_conf_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_powerup_ack_get(p_reg, p_data);
    nrf_barrier_r();
}

/** @refhal{nrf_mramc_powerdown_ack_get} */
NRFY_STATIC_INLINE void nrfy_mramc_powerdown_ack_get(NRF_MRAMC_Type const *   p_reg,
                                                     nrf_mramc_power_conf_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_powerdown_ack_get(p_reg, p_data);
    nrf_barrier_r();
}

/** @refhal{nrf_mramc_power_force_on_set} */
NRFY_STATIC_INLINE void nrfy_mramc_power_force_on_set(NRF_MRAMC_Type *               p_reg,
                                                     nrf_mramc_power_conf_t const * p_data)
{
    nrf_mramc_power_force_on_set(p_reg, p_data);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_power_force_on_get} */
NRFY_STATIC_INLINE void nrfy_mramc_power_force_on_get(NRF_MRAMC_Type const *   p_reg,
                                                     nrf_mramc_power_conf_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_power_force_on_get(p_reg, p_data);
    nrf_barrier_r();
}

/** @refhal{nrf_mramc_power_force_off_set} */
NRFY_STATIC_INLINE void nrfy_mramc_power_force_off_set(NRF_MRAMC_Type *               p_reg,
                                                     nrf_mramc_power_conf_t const * p_data)
{
    nrf_mramc_power_force_off_set(p_reg, p_data);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_power_force_off_get} */
NRFY_STATIC_INLINE void nrfy_mramc_power_force_off_get(NRF_MRAMC_Type const *   p_reg,
                                                     nrf_mramc_power_conf_t * p_data)
{
    nrf_barrier_r();
    nrf_mramc_power_force_off_get(p_reg, p_data);
    nrf_barrier_r();
}
#endif // NRFY_MRAMC_HAS_POWER_MASK

/** @refhal{nrf_mramc_trim_datain_set} */
NRFY_STATIC_INLINE void nrfy_mramc_trim_datain_set(NRF_MRAMC_Type * p_reg,
                                                   uint32_t         data)
{
    nrf_mramc_trim_datain_set(p_reg, data);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_trim_datain_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mramc_trim_datain_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t data_in = nrf_mramc_trim_datain_get(p_reg);
    nrf_barrier_r();
    return data_in;
}

/** @refhal{nrf_mramc_trim_dataout_get} */
NRFY_STATIC_INLINE uint32_t nrfy_mramc_trim_dataout_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t data_out = nrf_mramc_trim_dataout_get(p_reg);
    nrf_barrier_r();
    return data_out;
}

/** @refhal{nrf_mramc_trim_count_set} */
NRFY_STATIC_INLINE void nrfy_mramc_trim_count_set(NRF_MRAMC_Type * p_reg,
                                                  uint32_t         bits)
{
    nrf_mramc_trim_count_set(p_reg, bits);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_trim_count_get} */
NRF_STATIC_INLINE uint32_t nrfy_mramc_trim_count_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t bits = nrf_mramc_trim_count_get(p_reg);
    nrf_barrier_r();
    return bits;
}

/** @refhal{nrf_mramc_trim_start} */
NRFY_STATIC_INLINE void nrfy_mramc_trim_start(NRF_MRAMC_Type * p_reg)
{
    nrf_mramc_trim_start(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_trim_start_get} */
NRFY_STATIC_INLINE bool nrfy_mramc_trim_start_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    bool start = nrf_mramc_trim_start_get(p_reg);
    nrf_barrier_r();
    return start;
}

/** @refhal{nrf_mramc_trim_ready_get} */
NRFY_STATIC_INLINE bool nrfy_mramc_trim_ready_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    bool ready = nrf_mramc_trim_ready_get(p_reg);
    nrf_barrier_r();
    return ready;
}

/** @refhal{nrf_mramc_trim_done_set} */
NRFY_STATIC_INLINE void nrfy_mramc_trim_done_set(NRF_MRAMC_Type * p_reg)
{
    nrf_mramc_trim_done_set(p_reg);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_trim_done_get} */
NRFY_STATIC_INLINE nrf_mramc_trim_t nrfy_mramc_trim_done_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    nrf_mramc_trim_t trim = nrf_mramc_trim_done_get(p_reg);
    nrf_barrier_r();
    return trim;
}

/** @refhal{nrf_mramc_erase_word_set} */
NRFY_STATIC_INLINE void nrfy_mramc_erase_word_set(NRF_MRAMC_Type * p_reg, uint32_t address)
{
    nrf_mramc_erase_word_set(p_reg, address);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_erase_area_set} */
NRFY_STATIC_INLINE void nrfy_mramc_erase_area_set(NRF_MRAMC_Type * p_reg,
                                                  uint32_t         address,
                                                  uint32_t         size)
{
    nrf_mramc_erase_area_set(p_reg, address, size);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_erase_area_get} */
NRFY_STATIC_INLINE void nrfy_mramc_erase_area_get(NRF_MRAMC_Type * p_reg,
                                                  uint32_t *       p_address,
                                                  uint32_t *       p_size)
{
    nrf_barrier_r();
    nrf_mramc_erase_area_get(p_reg, p_address, p_size);
    nrf_barrier_r();
}

/** @refhal{nrf_mramc_erase_all} */
NRFY_STATIC_INLINE void nrfy_mramc_erase_all(NRF_MRAMC_Type * p_reg)
{
    nrf_mramc_config_t mramc_config;
    nrf_mramc_config_t prev_mramc_config;

    __nrfy_internal_mramc_config_get(p_reg, &mramc_config);
    prev_mramc_config = mramc_config;
    mramc_config.mode_erase = NRF_MRAMC_MODE_ERASE_PAGE;

    __nrfy_internal_mramc_config_set(p_reg, &mramc_config);
    nrf_mramc_erase_all(p_reg);
    __nrfy_internal_mramc_config_set(p_reg, &prev_mramc_config);
}

/** @refhal{nrf_mramc_erase_all_get} */
NRFY_STATIC_INLINE bool nrfy_mramc_erase_all_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_mramc_erase_all_get(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_mramc_erase_word_lock_set} */
NRFY_STATIC_INLINE void nrfy_mramc_erase_word_lock_set(NRF_MRAMC_Type * p_reg, bool lock)
{
    nrf_mramc_erase_word_lock_set(p_reg, lock);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_erase_word_lock_get} */
NRFY_STATIC_INLINE bool nrfy_mramc_erase_word_lock_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    bool lock = nrf_mramc_erase_word_lock_get(p_reg);
    nrf_barrier_r();
    return lock;
}

/** @refhal{nrf_mramc_erase_area_lock_set} */
NRFY_STATIC_INLINE void nrfy_mramc_erase_area_lock_set(NRF_MRAMC_Type * p_reg, bool lock)
{
    nrf_mramc_erase_area_lock_set(p_reg, lock);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_erase_area_lock_get} */
NRFY_STATIC_INLINE bool nrfy_mramc_erase_area_lock_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    bool lock = nrf_mramc_erase_area_lock_get(p_reg);
    nrf_barrier_r();
    return lock;
}

/** @refhal{nrf_mramc_erase_all_lock_set} */
NRFY_STATIC_INLINE void nrfy_mramc_erase_all_lock_set(NRF_MRAMC_Type * p_reg, bool lock)
{
    nrf_mramc_erase_all_lock_set(p_reg, lock);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_erase_all_lock_get} */
NRFY_STATIC_INLINE bool nrfy_mramc_erase_all_lock_get(NRF_MRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    bool lock = nrf_mramc_erase_all_lock_get(p_reg);
    nrf_barrier_r();
    return lock;
}

/** @refhal{nrf_mramc_config_nvr_set} */
NRFY_STATIC_INLINE void nrfy_mramc_config_nvr_set(NRF_MRAMC_Type *               p_reg,
                                                  nrf_mramc_config_nvr_t const * p_data,
                                                  uint8_t                        page)
{
    nrf_mramc_config_nvr_set(p_reg, p_data, page);
    nrf_barrier_w();
}

/** @refhal{nrf_mramc_config_nvr_get} */
NRF_STATIC_INLINE void nrfy_mramc_config_nvr_get(NRF_MRAMC_Type const *   p_reg,
                                                nrf_mramc_config_nvr_t * p_data,
                                                uint8_t                  page)
{
    nrf_barrier_r();
    nrf_mramc_config_nvr_get(p_reg, p_data, page);
    nrf_barrier_r();
}

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_mramc_event_enabled_clear(NRF_MRAMC_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_mramc_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_mramc_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_mramc_event_handle(NRF_MRAMC_Type *  p_reg,
                                                               uint32_t          mask,
                                                               nrf_mramc_event_t event)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_mramc_event_check(p_reg, event))
    {
        nrf_mramc_event_clear(p_reg, event);
        return NRFY_EVENT_TO_INT_BITMASK(event);
    }
    return 0;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_mramc_events_process(NRF_MRAMC_Type * p_reg,
                                                                 uint32_t         mask)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    evt_mask =  __nrfy_internal_mramc_event_handle(p_reg, mask, NRF_MRAMC_EVENT_ACCESSERR);
    evt_mask |= __nrfy_internal_mramc_event_handle(p_reg, mask, NRF_MRAMC_EVENT_READY);
    evt_mask |= __nrfy_internal_mramc_event_handle(p_reg, mask, NRF_MRAMC_EVENT_READYNEXT);
    evt_mask |= __nrfy_internal_mramc_event_handle(p_reg, mask, NRF_MRAMC_EVENT_ECCERROR);
    evt_mask |= __nrfy_internal_mramc_event_handle(p_reg, mask, NRF_MRAMC_EVENT_ECCERRORCORR);
    evt_mask |= __nrfy_internal_mramc_event_handle(p_reg, mask, NRF_MRAMC_EVENT_TRIMCONFIGREQ);
    nrf_barrier_w();

    return evt_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_mramc_config_get(NRF_MRAMC_Type const * p_reg,
                                                         nrf_mramc_config_t *   p_config)
{
    nrf_barrier_r();
    nrf_mramc_config_get(p_reg, p_config);
    nrf_barrier_r();
}

NRFY_STATIC_INLINE void __nrfy_internal_mramc_config_set(NRF_MRAMC_Type *           p_reg,
                                                         nrf_mramc_config_t const * p_config)
{
    nrf_mramc_config_set(p_reg, p_config);
    nrf_barrier_w();
}

NRFY_STATIC_INLINE void __nrfy_internal_mramc_word_write(uint32_t addr, uint32_t value)
{
    *(volatile uint32_t *)addr = value;
    nrf_barrier_w();
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_MRAMC_H__
