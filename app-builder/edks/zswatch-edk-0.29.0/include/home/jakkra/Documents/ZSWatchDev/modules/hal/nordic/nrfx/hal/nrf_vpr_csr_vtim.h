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

#ifndef NRF_VPR_CSR_VTIM_H__
#define NRF_VPR_CSR_VTIM_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_vpr_csr_vtim_hal VPR CSR VTIM HAL
 * @{
 * @ingroup nrf_vpr
 * @brief   Hardware access layer for managing the VPR RISC-V CPU Control
 *          and Status Registers for VPR Timer (VPR CSR VTIM).
 */

/** @brief Counter modes. */
typedef enum
{
    NRF_VPR_CSR_VTIM_COUNT_STOP             = VPRCSR_NORDIC_CNTMODE0_CNTMODE0_STOP,     ///< Counter stops at 0.
    NRF_VPR_CSR_VTIM_COUNT_WRAP             = VPRCSR_NORDIC_CNTMODE0_CNTMODE0_WRAP,     ///< Counter will continue counting from 0xFFF.
    NRF_VPR_CSR_VTIM_COUNT_RELOAD           = VPRCSR_NORDIC_CNTMODE0_CNTMODE0_RELOAD,   ///< Counter will continue counting from the value in counter top.
    NRF_VPR_CSR_VTIM_COUNT_TRIGGER_COMBINED = VPRCSR_NORDIC_CNTMODE0_CNTMODE0_TRIGCOMB, ///< Trigger (counter 0) or combined (counter 1) mode.
                                                                                        /**< Trigger (applies to counter 0): Counter stops at 0.
                                                                                         *   Counting will restart when a VIO event happens.
                                                                                         *   Combined (applies to counter 1): Counter 1 acts as an extension of counter 0.
                                                                                         *   (16 most significant bits of a 32-bit counter.) */
} nrf_vpr_csr_vtim_count_t;

/**
 * @brief Macro for generating 32-bit counter value.
 *
 * @note Use this macro when running two separate counters in trigger mode to write
 *       the values simultaneously using @ref nrf_vpr_csr_vtim_combined_counter_set
 *
 * @param[in] cnt0 Counter 0 value.
 * @param[in] cnt1 Counter 1 value.
 */
#define NRF_VPR_CSR_VTIM_COUNTER_VAL(cnt0, cnt1)                           \
    (((cnt0 << VPRCSR_NORDIC_CNT_CNT0_Pos) & VPRCSR_NORDIC_CNT_CNT0_Msk) | \
     ((cnt1 << VPRCSR_NORDIC_CNT_CNT1_Pos) & VPRCSR_NORDIC_CNT_CNT1_Msk))

/**
 * @brief Function for getting the counter mode.
 *
 * @param[in] counter Index of the counter.
 *
 * @return Counter mode.
 */
NRF_STATIC_INLINE nrf_vpr_csr_vtim_count_t nrf_vpr_cst_vtim_count_mode_get(uint8_t counter);

/**
 * @brief Function for setting the counter mode.
 *
 * @param[in] counter Index of the counter.
 * @param[in] mode    Counter mode to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_count_mode_set(uint8_t                  counter,
                                                       nrf_vpr_csr_vtim_count_t mode);

/**
 * @brief Function for getting the counter value.
 *
 * @param[in] counter Index of the counter.
 *
 * @return Counter value.
 */
NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vtim_simple_counter_get(uint8_t counter);

/**
 * @brief Function for setting the counter value.
 *
 * @param[in] counter Index of the counter.
 * @param[in] value   Value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_simple_counter_set(uint8_t counter, uint16_t value);

/**
 * @brief Function for getting the counter top.
 *
 * @param[in] counter Index of the counter.
 *
 * @return Counter top.
 */
NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vtim_simple_counter_top_get(uint8_t counter);

/**
 * @brief Function for setting the counter top.
 *
 * @param[in] counter Index of the counter.
 * @param[in] value   Top value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_simple_counter_top_set(uint8_t counter, uint16_t value);

/**
 * @brief Function for setting the counter add.
 *
 * @param[in] counter Index of the counter.
 * @param[in] value   Add value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_simple_counter_add_set(uint8_t counter, uint16_t value);

/**
 * @brief Function for setting the wait register.
 *
 * Writing to this register will stall the CPU until counter reaches 0.
 *
 * @param[in] counter Index of the counter.
 * @param[in] write   True if @p value is to be written to the counter value before starting the wait.
 *                    False otherwise.
 * @param[in] value   Value to be written to the counter if @p write is true.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_simple_wait_set(uint8_t  counter,
                                                        bool     write,
                                                        uint16_t value);

/**
 * @brief Function for getting the combined counter value.
 *
 * @note Lower 16 bits represent counter 0, while higher 16 bits represent counter 1.
 *
 * @return Counter value.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vtim_combined_counter_get(void);

/**
 * @brief Function for setting the combined counter value.
 *
 * @note Lower 16 bits represent counter 0, while higher 16 bits represent counter 1.
 * @note This function can also be used in trigger mode to make sure the counter values are
 *       written simultaneously. Use @ref NRF_VPR_CSR_VTIM_COUNTER_VAL macro to generate
 *       value to be set.
 *
 * @param[in] value Value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_combined_counter_set(uint32_t value);

/**
 * @brief Function for getting the combined counter top.
 *
 * @note Lower 16 bits represent counter 0, while higher 16 bits represent counter 1.
 *
 * @return Counter top.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vtim_combined_counter_top_get(void);

/**
 * @brief Function for setting the combined counter top.
 *
 * @note Lower 16 bits represent counter 0, while higher 16 bits represent counter 1.
 *
 * @param[in] value Top value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_combined_counter_top_set(uint32_t value);

/**
 * @brief Function for setting the combined counter add.
 *
 * @note This function should be used in 32-bit counter mode.
 *
 * @param[in] value Add value to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_combined_counter_add_set(uint32_t value);

/**
 * @brief Function for triggering the wait.
 *
 * Writing to this register will stall the CPU until 32-bit counter reaches 0.
 *
 * @note This function should be used in 32-bit counter mode.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vtim_combined_wait_trigger(void);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE nrf_vpr_csr_vtim_count_t nrf_vpr_cst_vtim_count_mode_get(uint8_t counter)
{
    switch (counter)
    {
        case 0:
            return nrf_csr_read(VPRCSR_NORDIC_CNTMODE0);
        case 1:
            return nrf_csr_read(VPRCSR_NORDIC_CNTMODE1);
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_count_mode_set(uint8_t                  counter,
                                                       nrf_vpr_csr_vtim_count_t mode)
{
    switch (counter)
    {
        case 0:
            nrf_csr_write(VPRCSR_NORDIC_CNTMODE0, mode);
            break;
        case 1:
            nrf_csr_write(VPRCSR_NORDIC_CNTMODE1, mode);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vtim_simple_counter_get(uint8_t counter)
{
    switch (counter)
    {
        case 0:
            return (uint16_t)nrf_csr_read(VPRCSR_NORDIC_CNT0);
        case 1:
            return (uint16_t)nrf_csr_read(VPRCSR_NORDIC_CNT1);
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_simple_counter_set(uint8_t counter, uint16_t value)
{
    switch (counter)
    {
        case 0:
            nrf_csr_write(VPRCSR_NORDIC_CNT0, (uint32_t)value);
            break;
        case 1:
            nrf_csr_write(VPRCSR_NORDIC_CNT1, (uint32_t)value);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vtim_simple_counter_top_get(uint8_t counter)
{
    switch (counter)
    {
        case 0:
            return (nrf_csr_read(VPRCSR_NORDIC_CNTTOP) & VPRCSR_NORDIC_CNTTOP_CNT0RELOAD_Msk)
                   >> VPRCSR_NORDIC_CNTTOP_CNT0RELOAD_Pos;
        case 1:
            return (nrf_csr_read(VPRCSR_NORDIC_CNTTOP) & VPRCSR_NORDIC_CNTTOP_CNT1RELOAD_Msk)
                   >> VPRCSR_NORDIC_CNTTOP_CNT1RELOAD_Pos;
        default:
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_simple_counter_top_set(uint8_t counter, uint16_t value)
{
    uint32_t reg;

    switch (counter)
    {
        case 0:
            reg = nrf_csr_read(VPRCSR_NORDIC_CNTTOP);
            reg &= ~VPRCSR_NORDIC_CNTTOP_CNT0RELOAD_Msk;
            reg |= value << VPRCSR_NORDIC_CNTTOP_CNT0RELOAD_Pos;
            nrf_csr_write(VPRCSR_NORDIC_CNTTOP, reg);
            break;
        case 1:
            reg = nrf_csr_read(VPRCSR_NORDIC_CNTTOP);
            reg &= ~VPRCSR_NORDIC_CNTTOP_CNT1RELOAD_Msk;
            reg |= value << VPRCSR_NORDIC_CNTTOP_CNT1RELOAD_Pos;
            nrf_csr_write(VPRCSR_NORDIC_CNTTOP, reg);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_simple_counter_add_set(uint8_t counter, uint16_t value)
{
    switch (counter)
    {
        case 0:
            nrf_csr_write(VPRCSR_NORDIC_CNTADD0, value);
            break;
        case 1:
            nrf_csr_write(VPRCSR_NORDIC_CNTADD1, value);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_simple_wait_set(uint8_t counter, bool write, uint16_t value)
{
    switch (counter)
    {
        case 0:
            nrf_csr_write(VPRCSR_NORDIC_WAIT0,
                      ((write ? VPRCSR_NORDIC_WAIT0_WRITEDATA_WRITE :
                                VPRCSR_NORDIC_WAIT0_WRITEDATA_WAIT)
                       << VPRCSR_NORDIC_WAIT0_WRITEDATA_Pos) |
                      (value << VPRCSR_NORDIC_WAIT0_DATA_Pos));
            break;
        case 1:
            nrf_csr_write(VPRCSR_NORDIC_WAIT1,
                      ((write ? VPRCSR_NORDIC_WAIT1_WRITEDATA_WRITE :
                                VPRCSR_NORDIC_WAIT1_WRITEDATA_WAIT)
                       << VPRCSR_NORDIC_WAIT1_WRITEDATA_Pos) |
                      (value << VPRCSR_NORDIC_WAIT1_DATA_Pos));
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vtim_combined_counter_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_CNT);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_combined_counter_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_CNT, value);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vtim_combined_counter_top_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_CNTTOP);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_combined_counter_top_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_CNTTOP, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_combined_counter_add_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_CNTADD, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vtim_combined_wait_trigger(void)
{
    /* Writing any value will trigger wait. */
    nrf_csr_write(VPRCSR_NORDIC_WAIT, VPRCSR_NORDIC_WAIT_VAL_Msk);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_VPR_CSR_VTIM_H__
