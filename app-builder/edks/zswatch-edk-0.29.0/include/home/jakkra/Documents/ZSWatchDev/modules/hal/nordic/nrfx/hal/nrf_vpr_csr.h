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

#ifndef NRF_VPR_CSR_H__
#define NRF_VPR_CSR_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_vpr_csr_hal VPR CSR HAL
 * @{
 * @ingroup nrf_vpr
 * @brief   Hardware access layer for managing the VPR RISC-V CPU Control
 *          and Status Registers (VPR CSR).
 */

#if defined(VPRCSR_NORDIC_VPRNORDICCTRL_VPRBUSPRI_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether setting high priority for VPR RAM transactions is present. */
#define NRF_VPR_HAS_RAM_PRIO 1
#else
#define NRF_VPR_HAS_RAM_PRIO 0
#endif

/** @brief Nordic key for CSR writes. */
#define NRF_VPR_CSR_NORDIC_KEY_MASK \
    (VPRCSR_NORDIC_VPRNORDICCTRL_NORDICKEY_Enabled << VPRCSR_NORDIC_VPRNORDICCTRL_NORDICKEY_Pos)

/** @brief Interrupt threshold levels. */
typedef enum
{
    NRF_VPR_CSR_INT_THRESHOLD_DISABLED = VPRCSR_MINTTHRESH_TH_DISABLED,     ///< Threshold disabled.
    NRF_VPR_CSR_INT_THRESHOLD_LEVEL0   = VPRCSR_MINTTHRESH_TH_THRESHLEVEL0, ///< Threshold level 0.
    NRF_VPR_CSR_INT_THRESHOLD_LEVEL1   = VPRCSR_MINTTHRESH_TH_THRESHLEVEL1, ///< Threshold level 1.
    NRF_VPR_CSR_INT_THRESHOLD_LEVEL2   = VPRCSR_MINTTHRESH_TH_THRESHLEVEL2, ///< Threshold level 2.
    NRF_VPR_CSR_INT_THRESHOLD_LEVEL3   = VPRCSR_MINTTHRESH_TH_THRESHLEVEL3, ///< Threshold level 3.
} nrf_vpr_csr_int_threshold_t;

/** @brief Trap causes. */
typedef enum
{
    NRF_VPR_CSR_TRAP_CAUSE_INSTR_ADDR_MISALIGNED = VPRCSR_MCAUSE_EXCEPTIONCODE_INSTADDRMISALIGN,   ///< Instruction address misaligned.
    NRF_VPR_CSR_TRAP_CAUSE_INSTR_ACCESS_FAULT    = VPRCSR_MCAUSE_EXCEPTIONCODE_INSTACCESSFAULT,    ///< Instruction access fault.
    NRF_VPR_CSR_TRAP_CAUSE_INSTR_ILLEGAL         = VPRCSR_MCAUSE_EXCEPTIONCODE_ILLEGALINST,        ///< Illegal instruction.
    NRF_VPR_CSR_TRAP_CAUSE_BREAKPOINT            = VPRCSR_MCAUSE_EXCEPTIONCODE_BKPT,               ///< Breakpoint.
    NRF_VPR_CSR_TRAP_CAUSE_LOAD_ADDR_MISALIGNED  = VPRCSR_MCAUSE_EXCEPTIONCODE_LOADADDRMISALIGN,   ///< Load address misaligned.
    NRF_VPR_CSR_TRAP_CAUSE_LOAD_ACCESS_FAULT     = VPRCSR_MCAUSE_EXCEPTIONCODE_LOADACCESSFAULT,    ///< Load access fault.
    NRF_VPR_CSR_TRAP_CAUSE_STORE_ADDR_MISALIGNED = VPRCSR_MCAUSE_EXCEPTIONCODE_STOREADDRMISALIGN,  ///< Store/AMO address misaligned.
    NRF_VPR_CSR_TRAP_CAUSE_STORE_ACCESS_FAULT    = VPRCSR_MCAUSE_EXCEPTIONCODE_STOREACCESSFAULT,   ///< Store/AMO access misaligned.
    NRF_VPR_CSR_TRAP_CAUSE_ECALL_M               = VPRCSR_MCAUSE_EXCEPTIONCODE_ECALLMMODE,         ///< Environment call M-mode.
    NRF_VPR_CSR_TRAP_CAUSE_STACKING_BUS_FAULT    = VPRCSR_MCAUSE_EXCEPTIONCODE_BUSFAULTSTACKING,   ///< Bus fault on stacking.
    NRF_VPR_CSR_TRAP_CAUSE_STACKING_UNALIGNED    = VPRCSR_MCAUSE_EXCEPTIONCODE_MISALIGNSTACKING,   ///< Misaligned Stacking.
    NRF_VPR_CSR_TRAP_CAUSE_VECTOR_FAULT          = VPRCSR_MCAUSE_EXCEPTIONCODE_INTVECTORFAULT,     ///< Interrupt Vector Fault.
    NRF_VPR_CSR_TRAP_CAUSE_STACKING_UNALIGNED_EX = VPRCSR_MCAUSE_EXCEPTIONCODE_STACKINGEXCFAULT,   ///< Fault on Exception Stacking.
    NRF_VPR_CSR_TRAP_CAUSE_UNSTACKING_UNALIGNED  = VPRCSR_MCAUSE_EXCEPTIONCODE_MISALIGNUNSTACKING, ///< Misaligned Unstacking.
    NRF_VPR_CSR_TRAP_CAUSE_UNSTACKING_BUS_FAULT  = VPRCSR_MCAUSE_EXCEPTIONCODE_BUSFAULTUNSTACKING, ///< Bus fault on unstacking.
    NRF_VPR_CSR_TRAP_CAUSE_STORE_TIMEOUT_FAULT   = VPRCSR_MCAUSE_EXCEPTIONCODE_STORETIMEOUTFAULT,  ///< Store timeout fault.
    NRF_VPR_CSR_TRAP_CAUSE_LOAD_TIMEOUT_FAULT    = VPRCSR_MCAUSE_EXCEPTIONCODE_LOADTIMEOUTFAULT,   ///< Load timeout fault.
} nrf_vpr_csr_trap_cause_t;

/** @brief Sleep states. */
typedef enum
{
    NRF_VPR_CSR_SLEEP_STATE_WAIT       = VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_WAIT,      ///< During sleep, clock is not turned off.
    NRF_VPR_CSR_SLEEP_STATE_RESET      = VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_RESET,     ///< Sleep mode out of reset.
    NRF_VPR_CSR_SLEEP_STATE_SLEEP      = VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_SLEEP,     ///< During sleep, clock is turned off.
    NRF_VPR_CSR_SLEEP_STATE_DEEP_SLEEP = VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_DEEPSLEEP, ///< During sleep, clock and power are turned off.
    NRF_VPR_CSR_SLEEP_STATE_HIBERNATE  = VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_HIBERNATE, ///< During sleep, clock is turned off. All the registers are saved automatically. Restart by a reset.
} nrf_vpr_csr_sleep_state_t;

/** @brief Function for enabling the interrupts in machine mode. */
NRF_STATIC_INLINE void nrf_vpr_csr_machine_interrupts_enable(void);

/** @brief Function for disabling the interrupts in machine mode. */
NRF_STATIC_INLINE void nrf_vpr_csr_machine_interrupts_disable(void);

/**
 * @brief Function for checking whether interrupts are enabled in machine mode.
 *
 * @retval true  Interrupts are enabled.
 * @retval false Interrupts are disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_machine_interrupts_check(void);

/**
 * @brief Function for setting the base address of trap vector table.
 *
 * @param[in] address Machine trap vector table base address to be set.
 *                    Has to be aligned on 64-byte or greater power-of-two boundary.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_machine_trap_vector_table_addr_set(uint32_t address);

/**
 * @brief Function for getting the machine trap vector table base address.
 *
 * @return Machine trap vector table base address.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_machine_trap_vector_table_addr_get(void);

/**
 * @brief Function for getting the machine exception program counter.
 *
 * @return Virtual address of the instruction that was interrupted or that encountered the exception.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_machine_exception_pc_get(void);

/**
 * @brief Function for getting the machine trap cause exception code.
 *
 * @return Exception code.
 */
NRF_STATIC_INLINE nrf_vpr_csr_trap_cause_t nrf_vpr_csr_machine_trap_cause_code_get(void);

/**
 * @brief Function for checking the state of the interrupt bit for machine trap.
 *
 * @retval true  Trap was caused by an interrupt.
 * @retval false Trap was not caused by an interrupt.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_machine_trap_interrupt_check(void);

/**
 * @brief Function for getting the machine trap value.
 *
 * @return Exception-specific information.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_machine_trap_value_get(void);

/**
 * @brief Function for setting the machine mode interrupt level threshold.
 *
 * @param[in] th Machine mode interrupt level threshold to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_machine_interrupt_threshold_set(nrf_vpr_csr_int_threshold_t th);

/**
 * @brief Function for getting the machine mode interrupt level threshold.
 *
 * @return Machine mode interrupt level threshold.
 */
NRF_STATIC_INLINE nrf_vpr_csr_int_threshold_t nrf_vpr_csr_machine_interrupt_threshold_get(void);

/**
 * @brief Function for enabling or disabling the Cycle Counter.
 *
 * @param[in] enable True if Cycle Counter is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_machine_cycle_counter_enable_set(bool enable);

/**
 * @brief Function for checking whether the Cycle Counter is enabled.
 *
 * @retval true  Cycle counter is enabled.
 * @retval false Cycle counter is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_machine_cycle_counter_enable_check(void);

/**
 * @brief Function for getting the machine cycle counter.
 *
 * @return Number of clock cycles executed by the processor core.
 */
NRF_STATIC_INLINE uint64_t nrf_vpr_csr_machine_cycle_counter_get(void);

/**
 * @brief Function for enabling or disabling the Instruction Counter.
 *
 * @param[in] enable True if Instruction Counter is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_machine_instruction_counter_enable_set(bool enable);

/**
 * @brief Function for checking whether the Instruction Counter is enabled.
 *
 * @retval true  Instruction counter is enabled.
 * @retval false Instruction counter is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_machine_instruction_counter_enable_check(void);
/**
 * @brief Function for getting the machine instruction counter.
 *
 * @return Number of instructions exectuted by the processor.
 */
NRF_STATIC_INLINE uint64_t nrf_vpr_csr_machine_instruction_counter_get(void);

/**
 * @brief Function for enabling or disabling the Real-Time Peripherals.
 *
 * @param[in] enable True if RT Perhiperals are to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_rtperiph_enable_set(bool enable);

/**
 * @brief Function for checking whether the Real-Time Peripherals are enabled.
 *
 * @retval true  RT Peripherals are enabled.
 * @retval false RT Peripherals are disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_rtperiph_enable_check(void);

/**
 * @brief Function for enabling or disabling the remap functionality.
 *
 * @param[in] enable True if remap is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_remap_enable_set(bool enable);

/**
 * @brief Function for checking whether the remap functionality is enabled.
 *
 * @retval true  Remap is enabled.
 * @retval false Remap is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_remap_enable_check(void);
/**
 * @brief Function for enabling or disabling the generation of IRQ at position CNT_IRQ_POSITION.
 *
 * @param[in] enable True if generation of IRQ at position CNT_IRQ_POSITION is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_cnt_irq_enable_set(bool enable);

/**
 * @brief Function for checking whether the generation of IRQ at position CNT_IRQ_POSITION is enabled.
 *
 * @retval true  Generation of IRQ is enabled.
 * @retval false Generation of IRQ is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_cnt_irq_enable_check(void);

#if NRF_VPR_HAS_RAM_PRIO
/**
 * @brief Function for enabling or disabling the high priority for VPR RAM transactions on bus.
 *
 * @param[in] enable True if high priority is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_ram_prio_enable_set(bool enable);

/**
 * @brief Function for checking whether the high priority for VPR RAM transactions on bus is enabled.
 *
 * @retval true  High priority is enabled.
 * @retval false High priority is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_ram_prio_enable_check(void);
#endif

/**
 * @brief Function for setting the sleep state.
 *
 * @param[in] state Sleep state to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_sleep_state_set(nrf_vpr_csr_sleep_state_t state);

/**
 * @brief Function for getting the sleep state.
 *
 * @return Current sleep state.
 */
NRF_STATIC_INLINE nrf_vpr_csr_sleep_state_t nrf_vpr_csr_sleep_state_get(void);

/**
 * @brief Function for enabling or disabling the return to sleep functionality.
 *
 * @param[in] enable True if CPU should be forced to return to sleep when it returns in a non-handler program,
 *                   false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_return_to_sleep_set(bool enable);

/**
 * @brief Function for checking whether the return to sleep functionality is enabled.
 *
 * @retval true  Return to sleep functionality is enabled.
 * @retval false Return to sleep functionality is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_return_to_sleep_check(void);

/**
 * @brief Function for enabling or disabling the stack on sleep functionality.
 *
 * @param[in] enable True if CPU should be forced to stack the context before going to sleep (used in order to have a fast wake-up),
 *                   false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_stack_on_sleep_set(bool enable);

/**
 * @brief Function for checking whether the stack on sleep functionality is enabled.
 *
 * @retval true  Stack on sleep functionality is enabled.
 * @retval false Stack on sleep functionality is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_stack_on_sleep_check(void);

/**
 * @brief Function for enabling or disabling the CLIC round robin arbitration.
 *
 * @param[in] enable True if round robin arbitration should be used for CLIC interrupt requests,
 *                   false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_clic_round_robin_set(bool enable);

/**
 * @brief Function for checking whether the CLIC round robin arbitration is enabled.
 *
 * @retval true  CLIC round robin arbitration is enabled.
 * @retval false CLIC round robin arbitration is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_clic_round_robin_check(void);

/**
 * @brief Function for enabling or disabling the unrecoverable return functionality.
 *
 * @param[in] enable True if unrecoverable return from exception is to be forced, false otherwise.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_unrecoverable_return_set(bool enable);

/**
 * @brief Function for checking whether the unrecoverable return is enabled.
 *
 * @retval true  Unrecoverable return is enabled.
 * @retval false Unrecoverable return is disabled.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_unrecoverable_return_check(void);

#ifndef NRF_DECLARE_ONLY
NRF_STATIC_INLINE void nrf_vpr_csr_machine_interrupts_enable(void)
{
    nrf_csr_set_bits(VPRCSR_MSTATUS, VPRCSR_MSTATUS_MIE_Msk);
}

NRF_STATIC_INLINE void nrf_vpr_csr_machine_interrupts_disable(void)
{
    nrf_csr_clear_bits(VPRCSR_MSTATUS, VPRCSR_MSTATUS_MIE_Msk);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_machine_interrupts_check(void)
{
    return nrf_csr_read(VPRCSR_MSTATUS) & VPRCSR_MSTATUS_MIE_Msk;
}

NRF_STATIC_INLINE void nrf_vpr_csr_machine_trap_vector_table_addr_set(uint32_t address)
{
    NRFX_ASSERT(!(address & 0xF));

    nrf_csr_write(VPRCSR_MTVT, address);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_machine_trap_vector_table_addr_get(void)
{
    return nrf_csr_read(VPRCSR_MTVT);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_machine_exception_pc_get(void)
{
    return nrf_csr_read(VPRCSR_MEPC);
}

NRF_STATIC_INLINE nrf_vpr_csr_trap_cause_t nrf_vpr_csr_machine_trap_cause_code_get(void)
{
    return (nrf_vpr_csr_trap_cause_t)((nrf_csr_read(VPRCSR_MCAUSE)
            & VPRCSR_MCAUSE_EXCEPTIONCODE_Msk)
           >> VPRCSR_MCAUSE_EXCEPTIONCODE_Pos);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_machine_trap_interrupt_check(void)
{
    return (nrf_csr_read(VPRCSR_MCAUSE) & VPRCSR_MCAUSE_INTERRUPT_Msk) >> VPRCSR_MCAUSE_INTERRUPT_Pos;
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_machine_trap_value_get(void)
{
    return nrf_csr_read(VPRCSR_MTVAL);
}

NRF_STATIC_INLINE void nrf_vpr_csr_machine_interrupt_threshold_set(nrf_vpr_csr_int_threshold_t th)
{
    nrf_csr_write(VPRCSR_MINTTHRESH, (th << VPRCSR_MINTTHRESH_TH_Pos));
}

NRF_STATIC_INLINE nrf_vpr_csr_int_threshold_t nrf_vpr_csr_machine_interrupt_threshold_get(void)
{
    return (nrf_csr_read(VPRCSR_MINTTHRESH) & VPRCSR_MINTTHRESH_TH_Msk) >> VPRCSR_MINTTHRESH_TH_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_csr_machine_cycle_counter_enable_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_MCOUNTINHIBIT);

    reg = (reg & ~VPRCSR_MCOUNTINHIBIT_CY_Msk) | (enable ?
            (VPRCSR_MCOUNTINHIBIT_CY_INCREMENT << VPRCSR_MCOUNTINHIBIT_CY_Pos) :
            (VPRCSR_MCOUNTINHIBIT_CY_INHIBIT   << VPRCSR_MCOUNTINHIBIT_CY_Pos));

    nrf_csr_write(VPRCSR_MCOUNTINHIBIT, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_machine_cycle_counter_enable_check(void)
{
    uint32_t reg = nrf_csr_read(VPRCSR_MCOUNTINHIBIT);

    return (reg & (VPRCSR_MCOUNTINHIBIT_CY_INHIBIT << VPRCSR_MCOUNTINHIBIT_CY_Pos)) ? false : true;
}

NRF_STATIC_INLINE uint64_t nrf_vpr_csr_machine_cycle_counter_get(void)
{
    return nrf_csr_read(VPRCSR_MCYCLE) | ((uint64_t)nrf_csr_read(VPRCSR_MCYCLEH) << 32);
}

NRF_STATIC_INLINE void nrf_vpr_csr_machine_instruction_counter_enable_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_MCOUNTINHIBIT);

    reg = (reg & ~VPRCSR_MCOUNTINHIBIT_IR_Msk) | (enable ?
            (VPRCSR_MCOUNTINHIBIT_IR_INCREMENT << VPRCSR_MCOUNTINHIBIT_IR_Pos) :
            (VPRCSR_MCOUNTINHIBIT_IR_INHIBIT   << VPRCSR_MCOUNTINHIBIT_IR_Pos));

    nrf_csr_write(VPRCSR_MCOUNTINHIBIT, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_machine_instruction_counter_enable_check(void)
{
    uint32_t reg = nrf_csr_read(VPRCSR_MCOUNTINHIBIT);

    return (reg & (VPRCSR_MCOUNTINHIBIT_IR_INHIBIT << VPRCSR_MCOUNTINHIBIT_IR_Pos)) ? false : true;
}

NRF_STATIC_INLINE uint64_t nrf_vpr_csr_machine_instruction_counter_get(void)
{
    return nrf_csr_read(VPRCSR_MINSTRET) | ((uint64_t)nrf_csr_read(VPRCSR_MINSTRETH) << 32);
}

NRF_STATIC_INLINE void nrf_vpr_csr_rtperiph_enable_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICCTRL);
    reg = (reg & ~VPRCSR_NORDIC_VPRNORDICCTRL_ENABLERTPERIPH_Msk) | NRF_VPR_CSR_NORDIC_KEY_MASK;

    reg |= ((enable ? VPRCSR_NORDIC_VPRNORDICCTRL_ENABLERTPERIPH_Enabled :
                      VPRCSR_NORDIC_VPRNORDICCTRL_ENABLERTPERIPH_Disabled)
            << VPRCSR_NORDIC_VPRNORDICCTRL_ENABLERTPERIPH_Pos) | NRF_VPR_CSR_NORDIC_KEY_MASK;

    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICCTRL, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_rtperiph_enable_check(void)
{
    return (nrf_csr_read(VPRCSR_NORDIC_VPRNORDICCTRL) & VPRCSR_NORDIC_VPRNORDICCTRL_ENABLERTPERIPH_Msk)
           >> VPRCSR_NORDIC_VPRNORDICCTRL_ENABLERTPERIPH_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_csr_remap_enable_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICCTRL);
    reg = (reg & ~VPRCSR_NORDIC_VPRNORDICCTRL_ENABLEREMAP_Msk) | NRF_VPR_CSR_NORDIC_KEY_MASK;

    reg |= ((enable ? VPRCSR_NORDIC_VPRNORDICCTRL_ENABLEREMAP_Enabled :
                      VPRCSR_NORDIC_VPRNORDICCTRL_ENABLEREMAP_Disabled)
            << VPRCSR_NORDIC_VPRNORDICCTRL_ENABLEREMAP_Pos) | NRF_VPR_CSR_NORDIC_KEY_MASK;

    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICCTRL, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_remap_enable_check(void)
{
    return (nrf_csr_read(VPRCSR_NORDIC_VPRNORDICCTRL) & VPRCSR_NORDIC_VPRNORDICCTRL_ENABLEREMAP_Msk)
           >> VPRCSR_NORDIC_VPRNORDICCTRL_ENABLEREMAP_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_csr_cnt_irq_enable_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICCTRL);
    reg &= ~(VPRCSR_NORDIC_VPRNORDICCTRL_CNTIRQENABLE_Msk | NRF_VPR_CSR_NORDIC_KEY_MASK);

    reg |= ((enable ? VPRCSR_NORDIC_VPRNORDICCTRL_CNTIRQENABLE_Enabled :
                      VPRCSR_NORDIC_VPRNORDICCTRL_CNTIRQENABLE_Disabled)
            << VPRCSR_NORDIC_VPRNORDICCTRL_CNTIRQENABLE_Pos) | NRF_VPR_CSR_NORDIC_KEY_MASK;

    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICCTRL, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_cnt_irq_enable_check(void)
{
    return (nrf_csr_read(VPRCSR_NORDIC_VPRNORDICCTRL) & VPRCSR_NORDIC_VPRNORDICCTRL_CNTIRQENABLE_Msk)
           >> VPRCSR_NORDIC_VPRNORDICCTRL_CNTIRQENABLE_Pos;
}

#if NRF_VPR_HAS_RAM_PRIO
NRF_STATIC_INLINE void nrf_vpr_csr_ram_prio_enable_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICCTRL);
    reg = (reg & ~VPRCSR_NORDIC_VPRNORDICCTRL_VPRBUSPRI_Msk) | NRF_VPR_CSR_NORDIC_KEY_MASK;

    reg |= ((enable ? VPRCSR_NORDIC_VPRNORDICCTRL_VPRBUSPRI_LowPriority :
                      VPRCSR_NORDIC_VPRNORDICCTRL_VPRBUSPRI_HighPriority)
            << VPRCSR_NORDIC_VPRNORDICCTRL_VPRBUSPRI_Pos) | NRF_VPR_CSR_NORDIC_KEY_MASK;

    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICCTRL, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_ram_prio_enable_check(void)
{
    return (nrf_csr_read(VPRCSR_NORDIC_VPRNORDICCTRL) & VPRCSR_NORDIC_VPRNORDICCTRL_VPRBUSPRI_Msk)
           >> VPRCSR_NORDIC_VPRNORDICCTRL_VPRBUSPRI_Pos;
}
#endif

NRF_STATIC_INLINE void nrf_vpr_csr_sleep_state_set(nrf_vpr_csr_sleep_state_t state)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL);
    reg &= ~VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_Msk;

    reg |= (uint32_t)state << VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_Pos;
    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL, reg);
}

NRF_STATIC_INLINE nrf_vpr_csr_sleep_state_t nrf_vpr_csr_sleep_state_get(void)
{
    return (nrf_vpr_csr_sleep_state_t)((nrf_csr_read(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL)
            & VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_Msk)
           >> VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_SLEEPSTATE_Pos);
}

NRF_STATIC_INLINE void nrf_vpr_csr_return_to_sleep_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL);
    reg &= ~VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_RETURNTOSLEEP_Msk;

    reg |= (enable ? VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_RETURNTOSLEEP_Enabled :
                     VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_RETURNTOSLEEP_Disabled)
           << VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_RETURNTOSLEEP_Pos;
    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_return_to_sleep_check(void)
{
    return (nrf_csr_read(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL)
            & VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_RETURNTOSLEEP_Msk)
           >> VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_RETURNTOSLEEP_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_csr_stack_on_sleep_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL);
    reg &= ~VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_STACKONSLEEP_Msk;

    reg |= (enable ? VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_STACKONSLEEP_Enabled :
                     VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_STACKONSLEEP_Disabled)
           << VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_STACKONSLEEP_Pos;
    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_stack_on_sleep_check(void)
{
    return (nrf_csr_read(VPRCSR_NORDIC_VPRNORDICSLEEPCTRL)
            & VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_STACKONSLEEP_Msk)
           >> VPRCSR_NORDIC_VPRNORDICSLEEPCTRL_STACKONSLEEP_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_csr_clic_round_robin_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE);
    reg = (reg & ~VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_DISABLECLICROUNDROBIN_Msk) |
             NRF_VPR_CSR_NORDIC_KEY_MASK;

    reg |= (enable ? VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_DISABLECLICROUNDROBIN_Enabled :
                     VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_DISABLECLICROUNDROBIN_Disabled)
           << VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_DISABLECLICROUNDROBIN_Pos;
    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_clic_round_robin_check(void)
{
    return ((nrf_csr_read(VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE)
             & VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_DISABLECLICROUNDROBIN_Msk)
            >> VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_DISABLECLICROUNDROBIN_Pos
            == VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_DISABLECLICROUNDROBIN_Enabled);
}

NRF_STATIC_INLINE void nrf_vpr_csr_unrecoverable_return_set(bool enable)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE);
    reg = (reg & ~VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_UNRECOVRETURN_Msk) |
             NRF_VPR_CSR_NORDIC_KEY_MASK;

    reg |= (enable ? VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_UNRECOVRETURN_Enabled :
                     VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_UNRECOVRETURN_Disabled)
           << VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_UNRECOVRETURN_Pos;
    nrf_csr_write(VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE, reg);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_unrecoverable_return_check(void)
{
    return ((nrf_csr_read(VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE)
             & VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_UNRECOVRETURN_Msk)
            >> VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_UNRECOVRETURN_Pos
            == VPRCSR_NORDIC_VPRNORDICFEATURESDISABLE_UNRECOVRETURN_Enabled);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_VPR_CSR_H__
