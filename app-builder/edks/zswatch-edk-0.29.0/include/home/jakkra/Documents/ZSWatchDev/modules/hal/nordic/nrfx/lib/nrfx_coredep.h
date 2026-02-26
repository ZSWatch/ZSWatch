/*
 * Copyright (c) 2018 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_COREDEP_H__
#define NRFX_COREDEP_H__

#include <nrfx.h>

#if NRFX_CHECK(ISA_RISCV)
#include <hal/nrf_vpr_csr.h>
#include <hal/nrf_vpr_csr_vtim.h>
#endif

/**
 * @defgroup nrfx_coredep Core-dependent functionality
 * @{
 * @ingroup nrfx
 * @brief Module containing functions with core-dependent implementation, like delay.
 */

/** @brief Core frequency (in MHz). */
#if !defined(NRFX_COREDEP_DELAY_CPU_FREQ_MHZ) || defined(__NRFX_DOXYGEN__)
#define NRFX_COREDEP_DELAY_CPU_FREQ_MHZ (SystemCoreClock / 1000000)
#endif

/** @brief Availability of Data Watchpoint and Trace (DWT) unit in the given SoC. */
#if !defined(NRFX_COREDEP_DELAY_DWT_PRESENT) || defined(__NRFX_DOXYGEN__)
#if defined(DWT_MISSING)
#define NRFX_COREDEP_DELAY_DWT_PRESENT 0
#else
#define NRFX_COREDEP_DELAY_DWT_PRESENT 1
#endif
#endif

/**
 * @brief Number of cycles consumed by one iteration of the internal loop
 *        in the function @ref nrfx_coredep_delay_us.
 *
 * This value can be specified externally (for example, when the SoC is emulated).
 */
#if !defined(NRFX_COREDEP_DELAY_US_LOOP_CYCLES) || defined(__NRFX_DOXYGEN__)
#if defined(DELAY_CUSTOM_CYCLES)
#define NRFX_COREDEP_DELAY_US_LOOP_CYCLES DELAY_CUSTOM_CYCLES
#else
#define NRFX_COREDEP_DELAY_US_LOOP_CYCLES 3
#endif
#endif

/** @brief RISC-V delay factor. */
#if !defined(NRFX_DELAY_RISCV_SLOWDOWN) || defined(__NRFX_DOXYGEN__)
#if defined(DELAY_RISCV_SLOWDOWN)
#define NRFX_DELAY_RISCV_SLOWDOWN DELAY_RISCV_SLOWDOWN
#else
#define NRFX_DELAY_RISCV_SLOWDOWN 50
#endif
#endif

/**
 * @brief Function for delaying execution for a number of microseconds.
 *
 * The value of @p time_us is multiplied by the CPU frequency in MHz. Therefore, the delay
 * is limited to the maximum value of the uint32_t type divided by the frequency.
 * @sa NRFX_COREDEP_DELAY_US_LOOP_CYCLES
 *
 * @param time_us Number of microseconds to wait.
 */
NRF_STATIC_INLINE void nrfx_coredep_delay_us(uint32_t time_us);

/** @} */

#ifndef NRF_DECLARE_ONLY

#if NRFX_CHECK(NRFX_DELAY_DWT_BASED)

#if !NRFX_COREDEP_DELAY_DWT_PRESENT
#error "DWT unit not present in the SoC that is used."
#endif

NRF_STATIC_INLINE void nrfx_coredep_delay_us(uint32_t time_us)
{
    if (time_us == 0)
    {
        return;
    }
    uint32_t time_cycles = time_us * NRFX_DELAY_CPU_FREQ_MHZ;

    // Save the current state of the DEMCR register to be able to restore it before exiting
    // this function. Enable the trace and debug blocks (including DWT).
    uint32_t core_debug = CoreDebug->DEMCR;
    CoreDebug->DEMCR = core_debug | CoreDebug_DEMCR_TRCENA_Msk;

    // Save the current state of the CTRL register in the DWT block. Make sure
    // that the cycle counter is enabled.
    uint32_t dwt_ctrl = DWT->CTRL;
    DWT->CTRL = dwt_ctrl | DWT_CTRL_CYCCNTENA_Msk;

    // Store start value of the cycle counter.
    uint32_t cyccnt_initial = DWT->CYCCNT;

    // Delay required time.
    while ((DWT->CYCCNT - cyccnt_initial) < time_cycles)
    {}

    // Restore preserved registers.
    DWT->CTRL = dwt_ctrl;
    CoreDebug->DEMCR = core_debug;
}

#else // NRFX_CHECK(NRFX_DELAY_DWT_BASED)

NRF_STATIC_INLINE void nrfx_coredep_delay_us(uint32_t time_us)
{
    if (time_us == 0)
    {
        return;
    }

#if NRFX_CHECK(ISA_ARM)
    // Align the machine code, so that it can be cached properly and no extra
    // wait states appear.
    __ALIGN(16)
    static const uint16_t delay_machine_code[] = {
        0x3800 + NRFX_COREDEP_DELAY_US_LOOP_CYCLES, // SUBS r0, #loop_cycles
        0xd8fd, // BHI .-2
        0x4770  // BX LR
    };

    typedef void (* delay_func_t)(uint32_t);
    const delay_func_t delay_cycles =
        // Set LSB to 1 to execute the code in the Thumb mode.
        (delay_func_t)((((uint32_t)delay_machine_code) | 1));
    uint32_t cycles = time_us * NRFX_COREDEP_DELAY_CPU_FREQ_MHZ;
    delay_cycles(cycles);
#elif NRFX_CHECK(ISA_RISCV)
#if !NRFX_CHECK(NRFX_COREDEP_VPR_LEGACY)
    nrf_vpr_csr_vtim_count_mode_set(1, NRF_VPR_CSR_VTIM_COUNT_TRIGGER_COMBINED);
    nrf_vpr_csr_vtim_combined_counter_set(time_us * NRFX_COREDEP_DELAY_CPU_FREQ_MHZ);
    nrf_vpr_csr_vtim_combined_wait_trigger();
#else

    for (volatile uint32_t i = 0;
             i < ((NRFX_COREDEP_DELAY_CPU_FREQ_MHZ * time_us) / NRFX_DELAY_RISCV_SLOWDOWN);
             i++)
        {}

#endif // !NRFX_CHECK(NRFX_CONFIG_COREDEP_VPR_LEGACY)
#endif // NRFX_CHECK(ISA_ARM)
}

#endif // !NRFX_CHECK(NRFX_DELAY_DWT_BASED_DELAY)

#endif // NRF_DECLARE_ONLY

#endif // NRFX_COREDEP_H__
