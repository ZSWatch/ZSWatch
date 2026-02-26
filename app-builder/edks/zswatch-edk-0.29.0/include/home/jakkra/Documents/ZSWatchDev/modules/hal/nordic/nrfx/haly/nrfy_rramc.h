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

#ifndef NRFY_RRAMC_H__
#define NRFY_RRAMC_H__

#include <nrfx.h>
#include <hal/nrf_rramc.h>

#ifdef __cplusplus
extern "C" {
#endif

NRFY_STATIC_INLINE void __nrfy_internal_rramc_event_enabled_clear(NRF_RRAMC_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_rramc_event_t event);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_rramc_event_handle(NRF_RRAMC_Type *  p_reg,
                                                               uint32_t          mask,
                                                               nrf_rramc_event_t event);

NRFY_STATIC_INLINE uint32_t __nrfy_internal_rramc_events_process(NRF_RRAMC_Type * p_reg,
                                                                 uint32_t         mask);

NRFY_STATIC_INLINE void __nrfy_internal_rramc_config_get(NRF_RRAMC_Type const * p_reg,
                                                         nrf_rramc_config_t *   p_config);

NRFY_STATIC_INLINE void __nrfy_internal_rramc_config_set(NRF_RRAMC_Type *           p_reg,
                                                         nrf_rramc_config_t const * p_config);

NRFY_STATIC_INLINE void __nrfy_internal_rramc_byte_write(NRF_RRAMC_Type * p_reg,
                                                         uint32_t         addr,
                                                         uint8_t          value);

NRFY_STATIC_INLINE void __nrfy_internal_rramc_word_write(NRF_RRAMC_Type * p_reg,
                                                         uint32_t         addr,
                                                         uint32_t         value);

NRFY_STATIC_INLINE bool __nrfy_internal_rramc_is_otp_word_writable(uint32_t index);

/**
 * @defgroup nrfy_rramc RRAMC HALY
 * @{
 * @ingroup nrf_rramc
 * @brief   Hardware access layer with cache and barrier support for managing the RRAMC peripheral.
 * @note    Extended Hardware Access Layer (HALY) is deprecated.
 */

/**
 * @brief Value representing the number of bytes in a word.
 *
 * It is used in loops iterating over bytes contained in a word or in word-alignment checks.
 */
#define NRFY_RRAMC_BYTES_IN_WORD 4

/**
 * @brief Value representing the number of words in a buffer line.
 *
 * It is used in loops iterating over words contained in buffer lines.
 */
#define NRFY_RRAMC_WORDS_IN_BUFER_LINE 4

/** @brief Value representing resistive random access memory (RRAM) base address. */
#define NRFY_RRAMC_RRAM_BASE_ADDRESS NRF_MEMORY_FLASH_BASE

/** @brief Default value for waiting for a next write. */
#define NRFY_RRAMC_READY_NEXT_TIMEOUT_DEFAULT RRAMC_READYNEXTTIMEOUT_ResetValue

/** @brief RRAMC configuration structure. */
typedef struct
{
    nrf_rramc_config_t             config;          ///< Mode and buffer size configuration.
    nrf_rramc_ready_next_timeout_t preload_timeout; ///< Preload timeout value for waiting for a next write.
    nrf_rramc_power_t              power;           ///< RRAMC power configuration.
} nrfy_rramc_config_t;

/**
 * @brief Function for configuring the RRAMC.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure of configuration of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_rramc_configure(NRF_RRAMC_Type *            p_reg,
                                             nrfy_rramc_config_t const * p_config)
{
    nrf_rramc_config_set(p_reg, &p_config->config);
    nrf_rramc_ready_next_timeout_set(p_reg, &p_config->preload_timeout);
    nrf_rramc_power_config_set(p_reg, &p_config->power);
    nrf_barrier_w();
    nrf_barrier_r();
    while (!nrf_rramc_write_ready_check(p_reg))
    {}
    nrf_barrier_r();
}

/**
 * @brief Function for initializing the specified RRAMC interrupts.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] mask         Mask of interrupts to be initialized.
 * @param[in] irq_priority Interrupt priority.
 * @param[in] enable       True if interrupts associated with the event mask are to be enabled, false otherwise.
 */
NRFY_STATIC_INLINE void nrfy_rramc_int_init(NRF_RRAMC_Type * p_reg,
                                            uint32_t         mask,
                                            uint8_t          irq_priority,
                                            bool             enable)
{
    __nrfy_internal_rramc_event_enabled_clear(p_reg, mask, NRF_RRAMC_EVENT_ERROR_ACCESS);
    __nrfy_internal_rramc_event_enabled_clear(p_reg, mask, NRF_RRAMC_EVENT_READY);
    __nrfy_internal_rramc_event_enabled_clear(p_reg, mask, NRF_RRAMC_EVENT_READY_NEXT);
    __nrfy_internal_rramc_event_enabled_clear(p_reg, mask, NRF_RRAMC_EVENT_WOKENUP);

    nrf_barrier_w();
    NRFX_IRQ_PRIORITY_SET(nrfx_get_irq_number(p_reg), irq_priority);
    NRFX_IRQ_ENABLE(nrfx_get_irq_number(p_reg));
    if (enable)
    {
        nrf_rramc_int_enable(p_reg, mask);
    }
    nrf_barrier_w();
}

/**
 * @brief Function for uninitializing the RRAMC interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRFY_STATIC_INLINE void nrfy_rramc_int_uninit(NRF_RRAMC_Type * p_reg)
{
    NRFX_IRQ_DISABLE(nrfx_get_irq_number(p_reg));
    nrf_barrier_w();
}

/**
 * @brief Function for processing the specified RRAMC events.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of events to be processed, created by @ref NRFY_EVENT_TO_INT_BITMASK().
 *
 * @return Mask of events that were generated and processed.
 *         To be checked against the result of @ref NRFY_EVENT_TO_INT_BITMASK().
 */
NRFY_STATIC_INLINE uint32_t nrfy_rramc_events_process(NRF_RRAMC_Type * p_reg, uint32_t mask)
{
    uint32_t evt_mask = __nrfy_internal_rramc_events_process(p_reg, mask);

    nrf_barrier_w();
    return evt_mask;
}

/**
 * @brief Function for writing a single byte to RRAM.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] address Address to where data is to be written.
 * @param[in] value   Value to be written.
 */
NRFY_STATIC_INLINE void nrfy_rramc_byte_write(NRF_RRAMC_Type * p_reg,
                                              uint32_t         address,
                                              uint8_t          value)
{
    nrf_rramc_config_t rramc_config;
    nrf_rramc_config_t prev_rramc_config;

    __nrfy_internal_rramc_config_get(p_reg, &rramc_config);
    prev_rramc_config = rramc_config;
    rramc_config.mode_write = true;

    __nrfy_internal_rramc_config_set(p_reg, &rramc_config);
    __nrfy_internal_rramc_byte_write(p_reg, address, value);
    __nrfy_internal_rramc_config_set(p_reg, &prev_rramc_config);
}

/**
 * @brief Function for writing consecutive bytes to RRAM.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] address   Address to where data is to be written.
 * @param[in] src       Pointer to data to be copied.
 * @param[in] num_bytes Number of bytes to be written.
 */
NRFY_STATIC_INLINE void nrfy_rramc_bytes_write(NRF_RRAMC_Type * p_reg,
                                               uint32_t         address,
                                               void const *     src,
                                               uint32_t         num_bytes)
{
    nrf_rramc_config_t rramc_config;
    nrf_rramc_config_t prev_rramc_config;

    __nrfy_internal_rramc_config_get(p_reg, &rramc_config);
    prev_rramc_config = rramc_config;
    rramc_config.mode_write = true;

    __nrfy_internal_rramc_config_set(p_reg, &rramc_config);
    for (uint32_t i = 0; i < num_bytes; i++)
    {
        __nrfy_internal_rramc_byte_write(p_reg, address + i, ((uint8_t const *)src)[i]);
    }
    __nrfy_internal_rramc_config_set(p_reg, &prev_rramc_config);
}

/**
 * @brief Function for reading a byte from the RRAM.
 *
 * @param[in] address Address of the byte to be read.
 *
 * @return Value read from RRAM.
 */
NRFY_STATIC_INLINE uint8_t nrfy_rramc_byte_read(uint32_t address)
{
    nrf_barrier_r();
    uint8_t byte = nrf_rramc_byte_read(address);
    nrf_barrier_r();
    return byte;
}

/**
 * @brief Function for writing a 32-bit word to RRAM.
 *
 * @note Depending on the source of the code being executed,
 *       the CPU may be halted during the operation.
 *       Refer to the Product Specification for more information.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] address Address to where data is to be written. Must be word-aligned.
 * @param[in] value   Value to be written.
 */
NRFY_STATIC_INLINE void nrfy_rramc_word_write(NRF_RRAMC_Type * p_reg,
                                              uint32_t         address,
                                              uint32_t         value)
{
    nrf_rramc_config_t rramc_config;
    nrf_rramc_config_t prev_rramc_config;

    __nrfy_internal_rramc_config_get(p_reg, &rramc_config);
    prev_rramc_config = rramc_config;
    rramc_config.mode_write = true;

    __nrfy_internal_rramc_config_set(p_reg, &rramc_config);
    __nrfy_internal_rramc_word_write(p_reg, address, value);
    __nrfy_internal_rramc_config_set(p_reg, &prev_rramc_config);
}

/**
 * @brief Function for writing consecutive 32-bit words to RRAM.
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
NRFY_STATIC_INLINE void nrfy_rramc_words_write(NRF_RRAMC_Type * p_reg,
                                               uint32_t         address,
                                               void const *     src,
                                               uint32_t         num_words)
{
    nrf_rramc_config_t rramc_config;
    nrf_rramc_config_t prev_rramc_config;

    __nrfy_internal_rramc_config_get(p_reg, &rramc_config);
    prev_rramc_config = rramc_config;
    rramc_config.mode_write = true;

    __nrfy_internal_rramc_config_set(p_reg, &rramc_config);
    for (uint32_t i = 0; i < num_words; i++)
    {
        __nrfy_internal_rramc_word_write(p_reg,
                                         address + (NRFY_RRAMC_BYTES_IN_WORD * i),
                                         ((uint32_t const *)src)[i]);
    }
    __nrfy_internal_rramc_config_set(p_reg, &prev_rramc_config);
}

/**
 * @brief Function for reading a 32-bit word from the RRAM.
 *
 * @param[in] address Address of the word to be read.
 *
 * @return Value read from RRAM.
 */
NRFY_STATIC_INLINE uint32_t nrfy_rramc_word_read(uint32_t address)
{
    nrf_barrier_r();
    uint32_t word = nrf_rramc_word_read(address);
    nrf_barrier_r();
    return word;
}

/**
 * @brief Function for reading a given number of bytes from the RRAM into the specified buffer.
 *
 * @param[out] dst       Pointer to the buffer to store the data.
 * @param[in]  address   Address of the first byte to be read.
 * @param[in]  num_bytes Number of bytes to be read.
 */
NRFY_STATIC_INLINE void nrfy_rramc_buffer_read(void *   dst,
                                               uint32_t address,
                                               uint32_t num_bytes)
{
    nrf_barrier_r();
    nrf_rramc_buffer_read(dst, address, num_bytes);
    nrf_barrier_r();
}

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
NRFY_STATIC_INLINE uint32_t nrfy_rramc_otp_word_read(uint32_t index)
{
    NRFX_ASSERT(index < UICR_OTP_MaxCount);
#if !defined(NRF_TRUSTZONE_NONSECURE)
    nrf_barrier_r();
    uint32_t val32 = NRF_UICR->OTP[index];
    nrf_barrier_r();
    return val32;
#else
    return 0xFFFFFFFF;
#endif
}

/**
 * @brief Function for writing a 32-bit word at index position to OTP region in UICR.
 *
 * The OTP can only be written once after each Erase All is performed.
 * This function checks if the value currently residing at the specified index can
 * be transformed to the desired value. If yes, then performs the write operation.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Address (index) in OTP table to which a word it to be written.
 * @param[in] value Value to be written.
 *
 * @retval true  Word can be written into the specified OTP index address.
 * @retval false Word cannot be written into the specified OTP index address.
 *               Erase UICR or change index address.
 */
NRFY_STATIC_INLINE bool nrfy_rramc_otp_word_write(NRF_RRAMC_Type * p_reg,
                                                  uint32_t         index,
                                                  uint32_t         value)
{
    NRFX_ASSERT(index < UICR_OTP_MaxCount);

#if !defined(NRF_TRUSTZONE_NONSECURE)
    if (!__nrfy_internal_rramc_is_otp_word_writable(index))
    {
        return false;
    }

    nrf_rramc_config_t rramc_config;
    nrf_rramc_config_t prev_rramc_config;

    __nrfy_internal_rramc_config_get(p_reg, &rramc_config);
    prev_rramc_config = rramc_config;
    rramc_config.mode_write = true;

    __nrfy_internal_rramc_config_set(p_reg, &rramc_config);
    NRF_UICR->OTP[index] = value;
    __nrfy_internal_rramc_config_set(p_reg, &prev_rramc_config);

    return true;
#else
    (void)index;
    (void)value;

    return false;
#endif
}

/** @refhal{nrf_rramc_task_trigger} */
NRFY_STATIC_INLINE void nrfy_rramc_task_trigger(NRF_RRAMC_Type * p_reg, nrf_rramc_task_t task)
{
    nrf_rramc_task_trigger(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_task_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_rramc_task_address_get(NRF_RRAMC_Type const * p_reg,
                                                        nrf_rramc_task_t       task)
{
    return nrf_rramc_task_address_get(p_reg, task);
}

/** @refhal{nrf_rramc_event_clear} */
NRFY_STATIC_INLINE void nrfy_rramc_event_clear(NRF_RRAMC_Type * p_reg, nrf_rramc_event_t event)
{
    nrf_rramc_event_clear(p_reg, event);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_event_check} */
NRFY_STATIC_INLINE bool nrfy_rramc_event_check(NRF_RRAMC_Type const * p_reg,
                                               nrf_rramc_event_t      event)
{
    nrf_barrier_r();
    bool check = nrf_rramc_event_check(p_reg, event);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_rramc_event_address_get} */
NRFY_STATIC_INLINE uint32_t nrfy_rramc_event_address_get(NRF_RRAMC_Type const * p_reg,
                                                         nrf_rramc_event_t      event)
{
    return nrf_rramc_event_address_get(p_reg, event);
}

/** @refhal{nrf_rramc_int_enable} */
NRFY_STATIC_INLINE void nrfy_rramc_int_enable(NRF_RRAMC_Type * p_reg, uint32_t mask)
{
    nrf_rramc_int_enable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_int_disable} */
NRFY_STATIC_INLINE void nrfy_rramc_int_disable(NRF_RRAMC_Type * p_reg, uint32_t mask)
{
    nrf_rramc_int_disable(p_reg, mask);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_int_enable_check} */
NRFY_STATIC_INLINE uint32_t nrfy_rramc_int_enable_check(NRF_RRAMC_Type const * p_reg, uint32_t mask)
{
    nrf_barrier_rw();
    uint32_t check = nrf_rramc_int_enable_check(p_reg, mask);
    nrf_barrier_r();
    return check;
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/** @refhal{nrf_rramc_subscribe_set} */
NRFY_STATIC_INLINE void nrfy_rramc_subscribe_set(NRF_RRAMC_Type * p_reg,
                                                 nrf_rramc_task_t task,
                                                 uint8_t          channel)
{
    nrf_rramc_subscribe_set(p_reg, task, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_subscribe_clear} */
NRFY_STATIC_INLINE void nrfy_rramc_subscribe_clear(NRF_RRAMC_Type * p_reg, nrf_rramc_task_t task)
{
    nrf_rramc_subscribe_clear(p_reg, task);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_publish_set} */
NRFY_STATIC_INLINE void nrfy_rramc_publish_set(NRF_RRAMC_Type *  p_reg,
                                               nrf_rramc_event_t event,
                                               uint8_t           channel)
{
    nrf_rramc_publish_set(p_reg, event, channel);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_publish_clear} */
NRFY_STATIC_INLINE void nrfy_rramc_publish_clear(NRF_RRAMC_Type *  p_reg, nrf_rramc_event_t event)
{
    nrf_rramc_publish_clear(p_reg, event);
    nrf_barrier_w();
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/** @refhal{nrf_rramc_ready_check} */
NRFY_STATIC_INLINE bool nrfy_rramc_ready_check(NRF_RRAMC_Type * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_rramc_ready_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_rramc_write_ready_check} */
NRFY_STATIC_INLINE bool nrfy_rramc_write_ready_check(NRF_RRAMC_Type * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_rramc_write_ready_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_rramc_error_access_addr_get} */
NRFY_STATIC_INLINE uint32_t nrfy_rramc_error_access_addr_get(NRF_RRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    uint32_t error_addr = nrf_rramc_error_access_addr_get(p_reg);
    nrf_barrier_r();
    return error_addr;
}

/** @refhal{nrf_rramc_empty_buffer_check} */
NRFY_STATIC_INLINE bool nrfy_rramc_empty_buffer_check(NRF_RRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_rramc_empty_buffer_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_rramc_config_get} */
NRFY_STATIC_INLINE void nrfy_rramc_config_get(NRF_RRAMC_Type const * p_reg,
                                              nrf_rramc_config_t *   p_config)
{
    __nrfy_internal_rramc_config_get(p_reg, p_config);
}

/** @refhal{nrf_rramc_config_set} */
NRFY_STATIC_INLINE void nrfy_rramc_config_set(NRF_RRAMC_Type *           p_reg,
                                              nrf_rramc_config_t const * p_config)
{
    __nrfy_internal_rramc_config_set(p_reg, p_config);
}

/** @refhal{nrf_rramc_ready_next_timeout_get} */
NRFY_STATIC_INLINE void nrfy_rramc_ready_next_timeout_get(NRF_RRAMC_Type const *           p_reg,
                                                          nrf_rramc_ready_next_timeout_t * p_config)
{
    nrf_barrier_r();
    nrf_rramc_ready_next_timeout_get(p_reg, p_config);
    nrf_barrier_r();
}

/** @refhal{nrf_rramc_ready_next_timeout_set} */
NRFY_STATIC_INLINE void
nrfy_rramc_ready_next_timeout_set(NRF_RRAMC_Type *                       p_reg,
                                  nrf_rramc_ready_next_timeout_t const * p_config)
{
    nrf_rramc_ready_next_timeout_set(p_reg, p_config);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_power_config_get} */
NRFY_STATIC_INLINE void nrfy_rramc_power_config_get(NRF_RRAMC_Type const * p_reg,
                                                    nrf_rramc_power_t *    p_config)
{
    nrf_barrier_r();
    nrf_rramc_power_config_get(p_reg, p_config);
    nrf_barrier_r();
}

/** @refhal{nrf_rramc_power_config_set} */
NRFY_STATIC_INLINE void nrfy_rramc_power_config_set(NRF_RRAMC_Type *          p_reg,
                                                    nrf_rramc_power_t const * p_config)
{
    nrf_rramc_power_config_set(p_reg, p_config);
    nrf_barrier_w();
}

/** @refhal{nrf_rramc_erase_all_check} */
NRFY_STATIC_INLINE bool nrfy_rramc_erase_all_check(NRF_RRAMC_Type const * p_reg)
{
    nrf_barrier_r();
    bool check = nrf_rramc_erase_all_check(p_reg);
    nrf_barrier_r();
    return check;
}

/** @refhal{nrf_rramc_erase_all_set} */
NRFY_STATIC_INLINE void nrfy_rramc_erase_all_set(NRF_RRAMC_Type * p_reg)
{
    nrf_rramc_config_t rramc_config;
    nrf_rramc_config_t prev_rramc_config;

    __nrfy_internal_rramc_config_get(p_reg, &rramc_config);
    prev_rramc_config = rramc_config;
    rramc_config.mode_write = true;

    __nrfy_internal_rramc_config_set(p_reg, &rramc_config);
    nrf_rramc_erase_all_set(p_reg);
    __nrfy_internal_rramc_config_set(p_reg, &prev_rramc_config);
}

/** @} */

NRFY_STATIC_INLINE void __nrfy_internal_rramc_event_enabled_clear(NRF_RRAMC_Type *  p_reg,
                                                                  uint32_t          mask,
                                                                  nrf_rramc_event_t event)
{
    if (mask & NRFY_EVENT_TO_INT_BITMASK(event))
    {
        nrf_rramc_event_clear(p_reg, event);
    }
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_rramc_event_handle(NRF_RRAMC_Type *  p_reg,
                                                               uint32_t          mask,
                                                               nrf_rramc_event_t event)
{
    if ((mask & NRFY_EVENT_TO_INT_BITMASK(event)) && nrf_rramc_event_check(p_reg, event))
    {
        nrf_rramc_event_clear(p_reg, event);
        return NRFY_EVENT_TO_INT_BITMASK(event);
    }
    return 0;
}

NRFY_STATIC_INLINE uint32_t __nrfy_internal_rramc_events_process(NRF_RRAMC_Type * p_reg,
                                                                 uint32_t         mask)
{
    uint32_t evt_mask = 0;

    nrf_barrier_r();
    evt_mask =  __nrfy_internal_rramc_event_handle(p_reg, mask, NRF_RRAMC_EVENT_ERROR_ACCESS);
    evt_mask |= __nrfy_internal_rramc_event_handle(p_reg, mask, NRF_RRAMC_EVENT_READY);
    evt_mask |= __nrfy_internal_rramc_event_handle(p_reg, mask, NRF_RRAMC_EVENT_READY_NEXT);
    evt_mask |= __nrfy_internal_rramc_event_handle(p_reg, mask, NRF_RRAMC_EVENT_WOKENUP);
    nrf_barrier_w();

    return evt_mask;
}

NRFY_STATIC_INLINE void __nrfy_internal_rramc_config_get(NRF_RRAMC_Type const * p_reg,
                                                         nrf_rramc_config_t *   p_config)
{
    nrf_barrier_r();
    nrf_rramc_config_get(p_reg, p_config);
    nrf_barrier_r();
}

NRFY_STATIC_INLINE void __nrfy_internal_rramc_config_set(NRF_RRAMC_Type *           p_reg,
                                                         nrf_rramc_config_t const * p_config)
{
    nrf_rramc_config_set(p_reg, p_config);
    nrf_barrier_w();
    nrf_barrier_r();
    while (!nrf_rramc_ready_check(p_reg))
    {}
    nrf_barrier_r();
}

NRFY_STATIC_INLINE void __nrfy_internal_rramc_byte_write(NRF_RRAMC_Type * p_reg,
                                                         uint32_t         addr,
                                                         uint8_t          value)
{
    nrf_barrier_r();
    while (!nrf_rramc_write_ready_check(p_reg))
    {}
    nrf_barrier_r();

    nrf_rramc_byte_write(addr, value);
    nrf_barrier_w();
}

NRFY_STATIC_INLINE void __nrfy_internal_rramc_word_write(NRF_RRAMC_Type * p_reg,
                                                         uint32_t         addr,
                                                         uint32_t         value)
{
    nrf_barrier_r();
    while (!nrf_rramc_write_ready_check(p_reg))
    {}
    nrf_barrier_r();

    nrf_rramc_word_write(addr, value);
    nrf_barrier_w();
}

NRFY_STATIC_INLINE bool __nrfy_internal_rramc_is_otp_word_writable(uint32_t index)
{
#if !defined(NRF_TRUSTZONE_NONSECURE)
    nrf_barrier_r();
    uint32_t val_on_addr = NRF_UICR->OTP[index];
    nrf_barrier_r();
    return (val_on_addr == 0xFFFFFFFF);
#else
    return false;
#endif
}

#ifdef __cplusplus
}
#endif

#endif // NRFY_RRAMC_H__
