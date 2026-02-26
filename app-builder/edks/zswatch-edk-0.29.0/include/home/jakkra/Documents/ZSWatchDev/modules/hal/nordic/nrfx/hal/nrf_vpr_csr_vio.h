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

#ifndef NRF_VPR_CSR_VIO_H__
#define NRF_VPR_CSR_VIO_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_vpr_csr_vio_hal VPR CSR VIO HAL
 * @{
 * @ingroup nrf_vpr
 * @brief   Hardware access layer for managing the VPR RISC-V CPU Control
 *          and Status Registers for VPR IO (VPR CSR VIO).
 */

/** @brief Maximum number of frames to be shifted from buffered input before new data is required. */
#define NRF_VPR_CSR_VIO_SHIFT_CNT_IN_MAX VPRCSR_NORDIC_SHIFTCNTIN_VALUE_Max

/** @brief Maximum number of frames to be shifted from buffered output before new data is required. */
#define NRF_VPR_CSR_VIO_SHIFT_CNT_OUT_MAX VPRCSR_NORDIC_SHIFTCNTOUT_VALUE_Max

/** @brief Maximum buffered number of frames to be shifted from buffered interrupt before new data is required. */
#define NRF_VPR_CSR_VIO_SHIFT_CNT_OUT_BUFFERED_MAX VPRCSR_NORDIC_SHIFTCNTB_VALUE_Max

/** @brief Shift mode. */
typedef enum
{
    NRF_VPR_CSR_VIO_SHIFT_NONE        = VPRCSR_NORDIC_OUTMODE_MODE_NoShifting,       ///< No shifting.
    NRF_VPR_CSR_VIO_SHIFT_OUTB        = VPRCSR_NORDIC_OUTMODE_MODE_OutBBuf,          ///< OUTB used for buffering.
    NRF_VPR_CSR_VIO_SHIFT_OUTB_TOGGLE = VPRCSR_NORDIC_OUTMODE_MODE_OutBBufToggleClk, ///< OUTB used for buffering, auto-toggle clock line.
} nrf_vpr_csr_vio_shift_t;

/** @brief Output mode structure. */
typedef struct
{
    nrf_vpr_csr_vio_shift_t mode;        ///< Shift mode.
    uint16_t                frame_width; ///< Frame width in bits.
} nrf_vpr_csr_vio_mode_out_t;

/** @brief Input modes. */
typedef enum
{
    NRF_VPR_CSR_VIO_MODE_IN_CONTINUOUS = VPRCSR_NORDIC_INMODE_MODE_CONTINUOUS, ///< Continuous sampling (if CPU is not sleeping).
    NRF_VPR_CSR_VIO_MODE_IN_EVENT      = VPRCSR_NORDIC_INMODE_MODE_EVENT,      ///< Sampling on Counter 1 event.
    NRF_VPR_CSR_VIO_MODE_IN_SHIFT      = VPRCSR_NORDIC_INMODE_MODE_SHIFT,      ///< Sampling and shifting on Counter 1 event.
} nrf_vpr_csr_vio_mode_in_t;

/** @brief Shift control configuration. */
typedef struct
{
    uint8_t                   shift_count; ///< Number of frames to be shifted to OUTB or from INB before new data is required.
    nrf_vpr_csr_vio_shift_t   out_mode;    ///< Buffered output mode.
    uint8_t                   frame_width; ///< Output frame width in bits.
    nrf_vpr_csr_vio_mode_in_t in_mode;     ///< Buffered input mode.
} nrf_vpr_csr_vio_shift_ctrl_t;

/** @brief VIO configuration. */
typedef struct
{
    bool clk_polarity; ///< Clock polarity. True if high, false if low.
    bool stop_cnt;     ///< Stop counters CNT0 and CNT1 on OUTB under-run.
    bool input_sel;    ///< Input pin selection. True if sample on separate pin, false if sample on same OUT pin.
} nrf_vpr_csr_vio_config_t;

/**
 * @brief Function for getting the pin directions mask.
 *
 * @return Mask of pin directions. 0 is input, 1 is output.
 */
NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vio_dir_get(void);

/**
 * @brief Function for setting the pin directions mask.
 *
 * @param[in] value Mask of pin directions to be set. 0 is input, 1 is output.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_set(uint16_t value);

/**
 * @brief Function for setting the specified pins as outputs.
 *
 * @note Pins not set to 1 will retain their current setting.
 *       They will not be set to input.
 *
 * @param[in] value Mask of pins to be set as output.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_output_set(uint16_t value);

/**
 * @brief Function for setting the specified pins as inputs.
 *
 * @note Pins not set to 1 will retain their current setting.
 *       They will not be set to output.
 *
 * @param[in] value Mask of pins to be set as input.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_input_set(uint16_t value);

/**
 * @brief Function for getting the buffered pin directions mask.
 *
 * @return Mask of pin directions. 0 is input, 1 is output.
 */
NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vio_dir_buffered_get(void);

/**
 * @brief Function for setting the buffered pin directions mask.
 *
 * @param[in] value Mask of pin directions to be set. 0 is input, 1 is output.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_buffered_set(uint16_t value);

/**
 * @brief Function for setting the specified buffered pins as outputs.
 *
 * @note Pins not set to 1 will retain their current setting.
 *       They will not be set to input.
 *
 * @param[in] value Mask of pins to be set as output.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_buffered_output_set(uint16_t value);

/**
 * @brief Function for setting the specified buffered pins as inputs.
 *
 * @note Pins not set to 1 will retain their current setting.
 *       They will not be set to output.
 *
 * @param[in] value Mask of pins to be set as input.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_buffered_input_set(uint16_t value);

/**
 * @brief Function for retrieving the dirty status of buffered pin directions mask.
 *
 * @retval true  Buffer is dirty.
 * @retval false Buffer is clean.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_vio_dir_buffered_dirty_check(void);

/**
 * @brief Function for setting the pin directions toggle mask.
 *
 * @param[in] mask Mask of pin directions to be toggled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_toggle_set(uint16_t mask);

/**
 * @brief Function for setting the buffered pin directions toggle mask.
 *
 * @param[in] mask Mask of pin directions to be toggled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_buffered_toggle_set(uint16_t mask);

/**
 * @brief Function for getting the input values.
 *
 * @return Mask of input states. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vio_in_get(void);

/**
 * @brief Function for getting the buffered input values.
 *
 * @return Mask of input states. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_in_buffered_get(void);

/**
 * @brief Function for getting the buffered input values, reversed in each byte.
 *
 * @return Mask of input states. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_in_buffered_reversed_byte_get(void);

/**
 * @brief Function for getting the input mode.
 *
 * @return Input mode.
 */
NRF_STATIC_INLINE nrf_vpr_csr_vio_mode_in_t nrf_vpr_csr_vio_mode_in_get(void);

/**
 * @brief Function for setting the input mode.
 *
 * @param[in] mode Input mode to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_in_set(nrf_vpr_csr_vio_mode_in_t mode);

/**
 * @brief Function for getting the buffered input mode.
 *
 * @return Input mode.
 */
NRF_STATIC_INLINE nrf_vpr_csr_vio_mode_in_t nrf_vpr_csr_vio_mode_in_buffered_get(void);

/**
 * @brief Function for setting the buffered input mode.
 *
 * @param[in] mode Input mode to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_in_buffered_set(nrf_vpr_csr_vio_mode_in_t mode);

/**
 * @brief Function for getting the output values.
 *
 * @return Mask of output states. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vio_out_get(void);

/**
 * @brief Function for setting the output values.
 *
 * @param[in] value Mask of output states to be set. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_set(uint16_t value);

/**
 * @brief Function for enabling the specified output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_or_set(uint16_t value);

/**
 * @brief Function for disabling the specified output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set low.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_clear_set(uint16_t value);

/**
 * @brief Function for getting the buffered output values.
 *
 * @return Mask of output states. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_out_buffered_get(void);

/**
 * @brief Function for setting the buffered output values.
 *
 * @param[in] value Mask of output states to be set. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_set(uint32_t value);

/**
 * @brief Function for enabling the specified buffered output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_or_set(uint16_t value);

/**
 * @brief Function for disabling the specified buffered output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set low.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_clear_set(uint16_t value);

/**
 * @brief Function for getting the buffered output values, reversed in each byte.
 *
 * @return Mask of output states. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_out_buffered_reversed_byte_get(void);

/**
 * @brief Function for setting the buffered output values, reversed in each byte.
 *
 * @param[in] value Mask of output states to be set. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_byte_set(uint32_t value);

/**
 * @brief Function for enabling the specified buffered output values, reversed in each byte.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_byte_or_set(uint16_t value);

/**
 * @brief Function for disabling the specified buffered output values, reversed in each byte.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set low.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_byte_clear_set(uint16_t value);

/**
 * @brief Function for getting the buffered output values, reversed in each word.
 *
 * @return Mask of output states. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_out_buffered_reversed_word_get(void);

/**
 * @brief Function for setting the buffered output values, reversed in the whole word.
 *
 * @param[in] value Mask of output states to be set. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_word_set(uint32_t value);

/**
 * @brief Function for enabling the specified buffered output values, reversed in the whole word.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_word_or_set(uint16_t value);

/**
 * @brief Function for disabling the specified buffered output values, reversed in the whole word.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set low.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_word_clear_set(uint16_t value);

/**
 * @brief Function for retrieving the dirty status of buffered output values.
 *
 * @retval true  Buffer is dirty.
 * @retval fasle Buffer is clean.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_vio_out_buffered_dirty_check(void);

/**
 * @brief Function for setting the output toggle mask.
 *
 * @param[in] mask Mask of output to be toggled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_toggle_set(uint16_t mask);

/**
 * @brief Function for setting the buffered output toggle mask.
 *
 * @param[in] mask Mask of output to be toggled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_toggle_buffered_set(uint16_t mask);

/**
 * @brief Function for setting the combined output and buffered output values.
 *
 * @note Lower 16 bits determine the output state, while higher 16 bits determine the buffered output state.
 *
 * @param[in] value Mask of output states to be set. 0 is low, 1 is high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_combined_set(uint32_t value);

/**
 * @brief Function for enabling the specified combined output and buffered output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_combined_or_set(uint32_t value);

/**
 * @brief Function for disabling the specified combined output and buffered output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 *
 * @param[in] value Mask of output states to be set low.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_combined_clear_set(uint32_t value);

/**
 * @brief Function for setting the combined output and buffered output toggle mask.
 *
 * @note Lower 16 bits determine the output toggle, while higher 16 bits determine the buffered output toggle.
 *
 * @param[in] mask Mask of output to be toggled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_combined_toggle_set(uint32_t mask);

/**
 * @brief Function for retrieving the dirty status of combined output and buffered output values.
 *
 * @retval true  Buffer is dirty.
 * @retval fasle Buffer is clean.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_vio_out_combined_dirty_check(void);

/**
 * @brief Function for getting the configuration of output mode.
 *
 * @param[out] p_mode Pointer to the structure to be filled with output mode.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_out_get(nrf_vpr_csr_vio_mode_out_t * p_mode);

/**
 * @brief Function for setting the configuration of output mode.
 *
 * @param[in] p_mode Pointer to the structure with output mode to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_out_set(nrf_vpr_csr_vio_mode_out_t const * p_mode);

/**
 * @brief Function for getting the buffered configuration of output mode.
 *
 * @param[out] p_mode Pointer to the structure to be filled with buffered output mode.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_out_buffered_get(nrf_vpr_csr_vio_mode_out_t * p_mode);

/**
 * @brief Function for setting the buffered configuration of output mode.
 *
 * @param[in] p_mode Pointer to the structure with buffered output mode to be set.
 */
NRF_STATIC_INLINE
void nrf_vpr_csr_vio_mode_out_buffered_set(nrf_vpr_csr_vio_mode_out_t const * p_mode);

/**
 * @brief Function for setting the buffered shift control register configuration.
 *
 * @param[in] p_shift_ctrl Pointer to the structure with buffered shift control configuration to be set.
 */
NRF_STATIC_INLINE
void nrf_vpr_csr_vio_shift_ctrl_buffered_set(nrf_vpr_csr_vio_shift_ctrl_t const * p_shift_ctrl);

/**
 * @brief Function for getting the buffered shift control register configuration.
 *
 * @param[out] p_shift_ctrl Pointer to the structure to be filled with buffered shift control configuration.
 */
NRF_STATIC_INLINE
void nrf_vpr_csr_vio_shift_ctrl_buffered_get(nrf_vpr_csr_vio_shift_ctrl_t * p_shift_ctrl);

/**
 * @brief Function for getting the VIO configuration.
 *
 * @param[out] p_config Pointer to the structure to be filled with VIO configuration.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_config_get(nrf_vpr_csr_vio_config_t * p_config);

/**
 * @brief Function for setting the VIO configuration.
 *
 * @param[in] p_config Pointer to the structure with VIO configuration to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_config_set(nrf_vpr_csr_vio_config_t const * p_config);

/**
 * @brief Function for setting the VIO configuration's input pin to sample on separate pin.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_config_input_sel_enable(void);

/**
 * @brief Function for setting the VIO configuration's input pin to sample on same OUT pin.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_config_input_sel_disable(void);

/**
 * @brief Function for getting the combined pin directions mask and output values.
 *
 * @note Lower 16 bits determine the output state, while higher 16 bits determine the pin directions.
 *
 * @return Mask of pin directions and output values.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_dirout_get(void);

/**
 * @brief Function for setting the combined pin directions mask and output values.
 *
 * @note Lower 16 bits determine the output state, while higher 16 bits determine the pin directions.
 *
 * @param[in] value Mask of pin directions and output values.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_set(uint32_t value);

/**
 * @brief Function for enabling the combined pin directions mask and output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 * @note Lower 16 bits determine the output state, while higher 16 bits determine the pin directions.
 *
 * @param[in] value Mask of pins to be set as output and output states to be set high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_or_set(uint32_t value);

/**
 * @brief Function for disabling the combined pin directions mask and output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 * @note Lower 16 bits determine the output state, while higher 16 bits determine the pin directions.
 *
 * @param[in] value Mask of pins to be set as input and output states to be set low.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_clear_set(uint32_t value);

/**
 * @brief Function for getting the combined buffered pin directions mask and buffered output values.
 *
 * @note Lower 16 bits determine the buffered output state, while higher 16 bits determine the buffered pin directions.
 *
 * @return Mask of buffered pin directions and buffered output values.
 */
NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_dirout_buffered_get(void);

/**
 * @brief Function for getting the combined buffered pin directions mask and buffered output values.
 *
 * @note Lower 16 bits determine the buffered output state, while higher 16 bits determine the buffered pin directions.
 *
 * @param[in] value Mask of buffered pin directions and buffered output values.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_buffered_set(uint32_t value);

/**
 * @brief Function for enabling the combined buffered pin directions mask and buffered output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 * @note Lower 16 bits determine the buffered output state, while higher 16 bits determine the buffered pin directions.
 *
 * @param[in] value Mask of buffered pins to be set as output and buffered output states to be set high.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_buffered_or_set(uint32_t value);

/**
 * @brief Function for disabling the combined buffered pin directions mask and buffered output values.
 *
 * @note Pins not set to 1 will retain their current value.
 *       They will not be modified.
 * @note Lower 16 bits determine the buffered output state, while higher 16 bits determine the buffered pin directions.
 *
 * @param[in] value Mask of pins to be set as input and output states to be set low.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_buffered_clear_set(uint32_t value);

/**
 * @brief Function for retrieving the dirty status of combined buffered pin directions mask and buffered output values.
 *
 * @retval true  Buffer is dirty.
 * @retval false Buffer is clean.
 */
NRF_STATIC_INLINE bool nrf_vpr_csr_vio_dirout_buffered_dirty_check(void);

/**
 * @brief Function for setting the combined pin directions and output toggle masks.
 *
 * @note Lower 16 bits determine the output toggle, while higher 16 bits determine the pin directions toggle.
 *
 * @param[in] mask Mask of values to be toggled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_toggle_set(uint32_t mask);

/**
 * @brief Function for setting the combined buffered pin directions and buffered output toggle masks.
 *
 * @note Lower 16 bits determine the buffered output toggle, while higher 16 bits determine the buffered pin directions toggle.
 *
 * @param[in] mask Mask of values to be toggled.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_toggle_buffered_set(uint32_t mask);

/**
 * @brief Function for getting the number of frames to be shifted from buffered input before new data is required.
 *
 * @return Number of frames to be shifted from buffered input before new data is required.
 */
NRF_STATIC_INLINE uint8_t nrf_vpr_csr_vio_shift_cnt_in_get(void);

/**
 * @brief Function for setting the number of frames to be shifted from buffered input before new data is required.
 *
 * @param[in] cnt Number for frames to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_shift_cnt_in_set(uint8_t cnt);

/**
 * @brief Function for getting the number of frames to be shifted from buffered output before new data is required.
 *
 * @return Number of frames to be shifted from buffered output before new data is required.
 */
NRF_STATIC_INLINE uint8_t nrf_vpr_csr_vio_shift_cnt_out_get(void);

/**
 * @brief Function for setting the number of frames to be shifted from buffered output before new data is required.
 *
 * @param[in] cnt Number for frames to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_shift_cnt_out_set(uint8_t cnt);

/**
 * @brief Function for getting the buffered number of frames to be shifted from buffered output before new data is required.
 *
 * @return Buffered number of frames to be shifted from buffered output before new data is required.
 */
NRF_STATIC_INLINE uint8_t nrf_vpr_csr_vio_shift_cnt_out_buffered_get(void);

/**
 * @brief Function for setting the buffered number of frames to be shifted from buffered output before new data is required.
 *
 * @param[in] cnt Number for frames to be set.
 */
NRF_STATIC_INLINE void nrf_vpr_csr_vio_shift_cnt_out_buffered_set(uint8_t cnt);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vio_dir_get(void)
{
    return (uint16_t)nrf_csr_read(VPRCSR_NORDIC_DIR);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_set(uint16_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_DIR, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_output_set(uint16_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_DIR, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_input_set(uint16_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_DIR, value);
}

NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vio_dir_buffered_get(void)
{
    return (uint16_t)nrf_csr_read(VPRCSR_NORDIC_DIRB);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_buffered_set(uint16_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_DIRB, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_buffered_output_set(uint16_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_DIRB, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_buffered_input_set(uint16_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_DIRB, value);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_vio_dir_buffered_dirty_check(void)
{
    return ((nrf_csr_read(VPRCSR_NORDIC_DIRBS) & VPRCSR_NORDIC_DIRBS_DIRTYBIT_Msk)
            >> VPRCSR_NORDIC_DIRBS_DIRTYBIT_Pos) == VPRCSR_NORDIC_DIRBS_DIRTYBIT_DIRTY;
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_toggle_set(uint16_t mask)
{
    nrf_csr_write(VPRCSR_NORDIC_DIRTGL, mask);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dir_buffered_toggle_set(uint16_t mask)
{
    nrf_csr_write(VPRCSR_NORDIC_DIRBTGL, mask);
}

NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vio_in_get(void)
{
    return (uint16_t)nrf_csr_read(VPRCSR_NORDIC_IN);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_in_buffered_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_INB);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_in_buffered_reversed_byte_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_INBRB);
}

NRF_STATIC_INLINE nrf_vpr_csr_vio_mode_in_t nrf_vpr_csr_vio_mode_in_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_INMODE);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_in_set(nrf_vpr_csr_vio_mode_in_t mode)
{
    nrf_csr_write(VPRCSR_NORDIC_INMODE, mode);
}

NRF_STATIC_INLINE nrf_vpr_csr_vio_mode_in_t nrf_vpr_csr_vio_mode_in_buffered_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_INMODEB);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_in_buffered_set(nrf_vpr_csr_vio_mode_in_t mode)
{
    nrf_csr_write(VPRCSR_NORDIC_INMODEB, mode);
}

NRF_STATIC_INLINE uint16_t nrf_vpr_csr_vio_out_get(void)
{
    return (uint16_t)nrf_csr_read(VPRCSR_NORDIC_OUT);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_set(uint16_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_OUT, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_or_set(uint16_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_OUT, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_clear_set(uint16_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_OUT, value);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_out_buffered_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_OUTB);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_OUTB, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_or_set(uint16_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_OUTB, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_clear_set(uint16_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_OUTB, value);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_out_buffered_reversed_byte_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_OUTBRB);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_byte_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_OUTBRB, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_byte_or_set(uint16_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_OUTBRB, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_byte_clear_set(uint16_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_OUTBRB, value);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_out_buffered_reversed_word_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_OUTBRW);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_word_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_OUTBRW, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_word_or_set(uint16_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_OUTBRW, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_buffered_reversed_word_clear_set(uint16_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_OUTBRW, value);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_vio_out_buffered_dirty_check(void)
{
    return ((nrf_csr_read(VPRCSR_NORDIC_OUTBS) & VPRCSR_NORDIC_OUTBS_DIRTYBIT_Msk)
            >> VPRCSR_NORDIC_OUTBS_DIRTYBIT_Pos) == VPRCSR_NORDIC_OUTBS_DIRTYBIT_DIRTY;
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_toggle_set(uint16_t mask)
{
    nrf_csr_write(VPRCSR_NORDIC_OUTTGL, mask);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_toggle_buffered_set(uint16_t mask)
{
    nrf_csr_write(VPRCSR_NORDIC_OUTBTGL, mask);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_combined_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_OUTBD, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_combined_or_set(uint32_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_OUTBD, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_combined_clear_set(uint32_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_OUTBD, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_out_combined_toggle_set(uint32_t mask)
{
    nrf_csr_write(VPRCSR_NORDIC_OUTBDTGL, mask);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_vio_out_combined_dirty_check(void)
{
    return ((nrf_csr_read(VPRCSR_NORDIC_OUTBDS) & VPRCSR_NORDIC_OUTBDS_DIRTYBIT_Msk)
            >> VPRCSR_NORDIC_OUTBDS_DIRTYBIT_Pos) == VPRCSR_NORDIC_OUTBDS_DIRTYBIT_DIRTY;
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_out_get(nrf_vpr_csr_vio_mode_out_t * p_mode)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_OUTMODE);

    p_mode->mode = (nrf_vpr_csr_vio_shift_t)((reg & VPRCSR_NORDIC_OUTMODE_MODE_Msk)
                                             >> VPRCSR_NORDIC_OUTMODE_MODE_Pos);
    p_mode->frame_width = (reg & VPRCSR_NORDIC_OUTMODE_FRAMEWIDTH_Msk)
                          >> VPRCSR_NORDIC_OUTMODE_FRAMEWIDTH_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_out_set(nrf_vpr_csr_vio_mode_out_t const * p_mode)
{
    uint32_t reg = ((uint32_t)p_mode->mode << VPRCSR_NORDIC_OUTMODE_MODE_Pos) |
                   (((uint32_t)p_mode->frame_width << VPRCSR_NORDIC_OUTMODE_FRAMEWIDTH_Pos)
                    & VPRCSR_NORDIC_OUTMODE_FRAMEWIDTH_Msk);

    nrf_csr_write(VPRCSR_NORDIC_OUTMODE, reg);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_mode_out_buffered_get(nrf_vpr_csr_vio_mode_out_t * p_mode)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_OUTMODEB);

    p_mode->mode = (nrf_vpr_csr_vio_shift_t)((reg & VPRCSR_NORDIC_OUTMODEB_MODE_Msk)
                                             >> VPRCSR_NORDIC_OUTMODEB_MODE_Pos);
    p_mode->frame_width = (reg & VPRCSR_NORDIC_OUTMODEB_FRAMEWIDTH_Msk)
                          >> VPRCSR_NORDIC_OUTMODEB_FRAMEWIDTH_Pos;
}

NRF_STATIC_INLINE
void nrf_vpr_csr_vio_mode_out_buffered_set(nrf_vpr_csr_vio_mode_out_t const * p_mode)
{
    uint32_t reg = ((uint32_t)p_mode->mode << VPRCSR_NORDIC_OUTMODEB_MODE_Pos) |
                   (((uint32_t)p_mode->frame_width << VPRCSR_NORDIC_OUTMODEB_FRAMEWIDTH_Pos)
                    & VPRCSR_NORDIC_OUTMODEB_FRAMEWIDTH_Msk);

    nrf_csr_write(VPRCSR_NORDIC_OUTMODE, reg);
}

NRF_STATIC_INLINE
void nrf_vpr_csr_vio_shift_ctrl_buffered_set(nrf_vpr_csr_vio_shift_ctrl_t const * p_shift_ctrl)
{
    uint32_t reg = ((p_shift_ctrl->shift_count << VPRCSR_NORDIC_SHIFTCTRLB_SHIFTCNTB_VALUE_Pos)
                    & VPRCSR_NORDIC_SHIFTCTRLB_SHIFTCNTB_VALUE_Msk) | 
                   ((p_shift_ctrl->out_mode << VPRCSR_NORDIC_SHIFTCTRLB_OUTMODEB_MODE_Pos)
		    & VPRCSR_NORDIC_SHIFTCTRLB_OUTMODEB_MODE_Msk) | 
                   ((p_shift_ctrl->frame_width << VPRCSR_NORDIC_SHIFTCTRLB_OUTMODEB_FRAMEWIDTH_Pos)
		    & VPRCSR_NORDIC_SHIFTCTRLB_OUTMODEB_FRAMEWIDTH_Msk) |
                   ((p_shift_ctrl->in_mode << VPRCSR_NORDIC_SHIFTCTRLB_INMODEB_MODE_Pos)
		    & VPRCSR_NORDIC_SHIFTCTRLB_INMODEB_MODE_Msk);
    
    nrf_csr_write(VPRCSR_NORDIC_SHIFTCTRLB, reg);
}

NRF_STATIC_INLINE
void nrf_vpr_csr_vio_shift_ctrl_buffered_get(nrf_vpr_csr_vio_shift_ctrl_t * p_shift_ctrl)
{	
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_SHIFTCTRLB);
    
    p_shift_ctrl->shift_count = (reg & VPRCSR_NORDIC_SHIFTCTRLB_SHIFTCNTB_VALUE_Msk)
                                >> VPRCSR_NORDIC_SHIFTCTRLB_SHIFTCNTB_VALUE_Pos;
    p_shift_ctrl->out_mode    = (reg & VPRCSR_NORDIC_SHIFTCTRLB_OUTMODEB_MODE_Msk)
                                >> VPRCSR_NORDIC_SHIFTCTRLB_OUTMODEB_MODE_Pos;
    p_shift_ctrl->frame_width = (reg & VPRCSR_NORDIC_SHIFTCTRLB_OUTMODEB_FRAMEWIDTH_Msk)
                                >> VPRCSR_NORDIC_SHIFTCTRLB_OUTMODEB_FRAMEWIDTH_Pos;
    p_shift_ctrl->in_mode     = (reg & VPRCSR_NORDIC_SHIFTCTRLB_INMODEB_MODE_Msk)
                                >> VPRCSR_NORDIC_SHIFTCTRLB_INMODEB_MODE_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_config_get(nrf_vpr_csr_vio_config_t * p_config)
{
    uint32_t reg = nrf_csr_read(VPRCSR_NORDIC_RTPERIPHCTRL);

    p_config->clk_polarity = (reg & VPRCSR_NORDIC_RTPERIPHCTRL_CLOCKPOLARITY_Msk)
                             >> VPRCSR_NORDIC_RTPERIPHCTRL_CLOCKPOLARITY_Pos;
    p_config->stop_cnt     = (reg & VPRCSR_NORDIC_RTPERIPHCTRL_STOPCOUNTERS_Msk)
                             >> VPRCSR_NORDIC_RTPERIPHCTRL_STOPCOUNTERS_Pos;
    p_config->input_sel    = (reg & VPRCSR_NORDIC_RTPERIPHCTRL_INSEL_Msk)
                             >> VPRCSR_NORDIC_RTPERIPHCTRL_INSEL_Pos;
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_config_set(nrf_vpr_csr_vio_config_t const * p_config)
{
    uint32_t reg = (p_config->clk_polarity << VPRCSR_NORDIC_RTPERIPHCTRL_CLOCKPOLARITY_Pos) |
                   (p_config->stop_cnt << VPRCSR_NORDIC_RTPERIPHCTRL_STOPCOUNTERS_Pos) |
                   (p_config->input_sel << VPRCSR_NORDIC_RTPERIPHCTRL_INSEL_Pos);

    nrf_csr_write(VPRCSR_NORDIC_RTPERIPHCTRL, reg);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_config_input_sel_enable(void)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_RTPERIPHCTRL, 1UL << VPRCSR_NORDIC_RTPERIPHCTRL_INSEL_Pos);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_config_input_sel_disable(void)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_RTPERIPHCTRL, 1UL << VPRCSR_NORDIC_RTPERIPHCTRL_INSEL_Pos);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_dirout_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_DIROUT);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_DIROUT, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_or_set(uint32_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_DIROUT, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_clear_set(uint32_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_DIROUT, value);
}

NRF_STATIC_INLINE uint32_t nrf_vpr_csr_vio_dirout_buffered_get(void)
{
    return nrf_csr_read(VPRCSR_NORDIC_DIROUTB);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_buffered_set(uint32_t value)
{
    nrf_csr_write(VPRCSR_NORDIC_DIROUTB, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_buffered_or_set(uint32_t value)
{
    nrf_csr_set_bits(VPRCSR_NORDIC_DIROUTB, value);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_buffered_clear_set(uint32_t value)
{
    nrf_csr_clear_bits(VPRCSR_NORDIC_DIROUTB, value);
}

NRF_STATIC_INLINE bool nrf_vpr_csr_vio_dirout_buffered_dirty_check(void)
{
    return ((nrf_csr_read(VPRCSR_NORDIC_DIROUTBS) & VPRCSR_NORDIC_DIROUTBS_DIRTYBIT_Msk)
            >> VPRCSR_NORDIC_DIROUTBS_DIRTYBIT_Pos) == VPRCSR_NORDIC_DIROUTBS_DIRTYBIT_DIRTY;
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_toggle_set(uint32_t mask)
{
    nrf_csr_write(VPRCSR_NORDIC_DIROUTTGL, mask);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_dirout_toggle_buffered_set(uint32_t mask)
{
    nrf_csr_write(VPRCSR_NORDIC_DIROUTBTGL, mask);
}

NRF_STATIC_INLINE uint8_t nrf_vpr_csr_vio_shift_cnt_in_get(void)
{
    return (uint8_t)nrf_csr_read(VPRCSR_NORDIC_SHIFTCNTIN);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_shift_cnt_in_set(uint8_t cnt)
{
    NRFX_ASSERT(cnt <= NRF_VPR_CSR_VIO_SHIFT_CNT_IN_MAX);
    nrf_csr_write(VPRCSR_NORDIC_SHIFTCNTIN, cnt);
}

NRF_STATIC_INLINE uint8_t nrf_vpr_csr_vio_shift_cnt_out_get(void)
{
    return (uint8_t)nrf_csr_read(VPRCSR_NORDIC_SHIFTCNTOUT);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_shift_cnt_out_set(uint8_t cnt)
{
    NRFX_ASSERT(cnt <= NRF_VPR_CSR_VIO_SHIFT_CNT_OUT_MAX);
    nrf_csr_write(VPRCSR_NORDIC_SHIFTCNTOUT, cnt);
}

NRF_STATIC_INLINE uint8_t nrf_vpr_csr_vio_shift_cnt_out_buffered_get(void)
{
    return (uint8_t)nrf_csr_read(VPRCSR_NORDIC_SHIFTCNTB);
}

NRF_STATIC_INLINE void nrf_vpr_csr_vio_shift_cnt_out_buffered_set(uint8_t cnt)
{
    NRFX_ASSERT(cnt <= NRF_VPR_CSR_VIO_SHIFT_CNT_OUT_BUFFERED_MAX);
    nrf_csr_write(VPRCSR_NORDIC_SHIFTCNTB, cnt);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_VPR_CSR_VIO_H__
