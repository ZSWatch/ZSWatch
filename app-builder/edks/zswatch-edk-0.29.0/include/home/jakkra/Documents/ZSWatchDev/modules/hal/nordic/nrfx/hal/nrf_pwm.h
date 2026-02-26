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

#ifndef NRF_PWM_H__
#define NRF_PWM_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_pwm_hal PWM HAL
 * @{
 * @ingroup nrf_pwm
 * @brief   Hardware access layer for managing the Pulse Width Modulation (PWM) peripheral.
 */

#if defined(PWM_DMA_SEQ_PTR_PTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether dedicated DMA register is present. */
#define NRF_PWM_HAS_DMA_REG 1
#else
#define NRF_PWM_HAS_DMA_REG 0
#endif

#if defined(PWM_SHORTS_LOOPSDONE_SEQSTART0_Msk) || \
    defined(PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether shorting SEQSTART task with LOOPSDONE event is available. */
#define NRF_PWM_HAS_SHORT_LOOPSDONE_SEQSTART 1
#else
#define NRF_PWM_HAS_SHORT_LOOPSDONE_SEQSTART 0
#endif

#if (defined(PWM_TASKS_DMA_SEQ_START_START_Msk) && defined(PWM_EVENTS_DMA_SEQ_END_END_Msk)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PWM DMA tasks and events are present. */
#define NRF_PWM_HAS_DMA_TASKS_EVENTS 1
#else
#define NRF_PWM_HAS_DMA_TASKS_EVENTS 0
#endif

#if defined(PWM_SEQ_CNT_CNT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether setting the number of duty cycle values for a sequence is available. */
#define NRF_PWM_HAS_SEQ_CNT 1
#else
#define NRF_PWM_HAS_SEQ_CNT 0
#endif

#if defined(PWM_IDLEOUT_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether PWM has IDLEOUT register. */
#define NRF_PWM_HAS_IDLEOUT 1
#else
#define NRF_PWM_HAS_IDLEOUT 0
#endif

/**
 * @brief Macro getting pointer to the structure of registers of the PWM peripheral.
 *
 * @param[in] idx PWM instance index.
 *
 * @return Pointer to the structure of registers of the PWM peripheral.
 */
 #define NRF_PWM_INST_GET(idx) NRFX_CONCAT(NRF_, PWM, idx)

/**
 * @brief This value can be provided as a parameter for the @ref nrf_pwm_pins_set
 *        function call to specify that a given output channel shall not be
 *        connected to a physical pin.
 */
#define NRF_PWM_PIN_NOT_CONNECTED  0xFFFFFFFF

/** @brief Number of channels in each PWM instance. */
#define NRF_PWM_CHANNEL_COUNT  4

/**
 * @brief Helper macro for calculating the number of 16-bit values in the specified
 *        array of duty cycle values.
 */
#define NRF_PWM_VALUES_LENGTH(array)  (sizeof(array) / 2UL)


/** @brief PWM tasks. */
typedef enum
{
    NRF_PWM_TASK_STOP      = offsetof(NRF_PWM_Type, TASKS_STOP),             ///< Stops PWM pulse generation on all channels at the end of the current PWM period, and stops the sequence playback.
#if NRF_PWM_HAS_DMA_TASKS_EVENTS
    NRF_PWM_TASK_SEQSTART0 = offsetof(NRF_PWM_Type, TASKS_DMA.SEQ[0].START), ///< Starts playback of sequence 0.
    NRF_PWM_TASK_SEQSTART1 = offsetof(NRF_PWM_Type, TASKS_DMA.SEQ[1].START), ///< Starts playback of sequence 1.
#else
    NRF_PWM_TASK_SEQSTART0 = offsetof(NRF_PWM_Type, TASKS_SEQSTART[0]),      ///< Starts playback of sequence 0.
    NRF_PWM_TASK_SEQSTART1 = offsetof(NRF_PWM_Type, TASKS_SEQSTART[1]),      ///< Starts playback of sequence 1.
#endif
    NRF_PWM_TASK_NEXTSTEP  = offsetof(NRF_PWM_Type, TASKS_NEXTSTEP)          ///< Steps by one value in the current sequence if the decoder is set to @ref NRF_PWM_STEP_TRIGGERED mode.
} nrf_pwm_task_t;

/** @brief PWM events. */
typedef enum
{
    NRF_PWM_EVENT_STOPPED      = offsetof(NRF_PWM_Type, EVENTS_STOPPED),       ///< Response to STOP task, emitted when PWM pulses are no longer generated.
    NRF_PWM_EVENT_SEQSTARTED0  = offsetof(NRF_PWM_Type, EVENTS_SEQSTARTED[0]), ///< First PWM period started on sequence 0.
    NRF_PWM_EVENT_SEQSTARTED1  = offsetof(NRF_PWM_Type, EVENTS_SEQSTARTED[1]), ///< First PWM period started on sequence 1.
    NRF_PWM_EVENT_SEQEND0      = offsetof(NRF_PWM_Type, EVENTS_SEQEND[0]),     ///< Emitted at the end of every sequence 0 when its last value has been read from RAM.
    NRF_PWM_EVENT_SEQEND1      = offsetof(NRF_PWM_Type, EVENTS_SEQEND[1]),     ///< Emitted at the end of every sequence 1 when its last value has been read from RAM.
    NRF_PWM_EVENT_PWMPERIODEND = offsetof(NRF_PWM_Type, EVENTS_PWMPERIODEND),  ///< Emitted at the end of each PWM period.
    NRF_PWM_EVENT_LOOPSDONE    = offsetof(NRF_PWM_Type, EVENTS_LOOPSDONE)      ///< Concatenated sequences have been played the specified number of times.
} nrf_pwm_event_t;

/** @brief PWM interrupts. */
typedef enum
{
    NRF_PWM_INT_STOPPED_MASK      = PWM_INTENSET_STOPPED_Msk,      ///< Interrupt on STOPPED event.
    NRF_PWM_INT_SEQSTARTED0_MASK  = PWM_INTENSET_SEQSTARTED0_Msk,  ///< Interrupt on SEQSTARTED[0] event.
    NRF_PWM_INT_SEQSTARTED1_MASK  = PWM_INTENSET_SEQSTARTED1_Msk,  ///< Interrupt on SEQSTARTED[1] event.
    NRF_PWM_INT_SEQEND0_MASK      = PWM_INTENSET_SEQEND0_Msk,      ///< Interrupt on SEQEND[0] event.
    NRF_PWM_INT_SEQEND1_MASK      = PWM_INTENSET_SEQEND1_Msk,      ///< Interrupt on SEQEND[1] event.
    NRF_PWM_INT_PWMPERIODEND_MASK = PWM_INTENSET_PWMPERIODEND_Msk, ///< Interrupt on PWMPERIODEND event.
    NRF_PWM_INT_LOOPSDONE_MASK    = PWM_INTENSET_LOOPSDONE_Msk     ///< Interrupt on LOOPSDONE event.
} nrf_pwm_int_mask_t;

/** @brief PWM shortcuts. */
typedef enum
{
    NRF_PWM_SHORT_SEQEND0_STOP_MASK        = PWM_SHORTS_SEQEND0_STOP_Msk,             ///< Shortcut between SEQEND[0] event and STOP task.
    NRF_PWM_SHORT_SEQEND1_STOP_MASK        = PWM_SHORTS_SEQEND1_STOP_Msk,             ///< Shortcut between SEQEND[1] event and STOP task.
#if defined(PWM_SHORTS_LOOPSDONE_SEQSTART0_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_PWM_SHORT_LOOPSDONE_SEQSTART0_MASK = PWM_SHORTS_LOOPSDONE_SEQSTART0_Msk,      ///< Shortcut between LOOPSDONE event and SEQSTART[0] task.
    NRF_PWM_SHORT_LOOPSDONE_SEQSTART1_MASK = PWM_SHORTS_LOOPSDONE_SEQSTART1_Msk,      ///< Shortcut between LOOPSDONE event and SEQSTART[1] task.
#else
    NRF_PWM_SHORT_LOOPSDONE_SEQSTART0_MASK = PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Msk, ///< Shortcut between LOOPSDONE event and DMA.SEQ[0].START task.
    NRF_PWM_SHORT_LOOPSDONE_SEQSTART1_MASK = PWM_SHORTS_LOOPSDONE_DMA_SEQ1_START_Msk, ///< Shortcut between LOOPSDONE event and DMA.SEQ[1].START task.
#endif
    NRF_PWM_SHORT_LOOPSDONE_STOP_MASK      = PWM_SHORTS_LOOPSDONE_STOP_Msk            ///< Shortcut between LOOPSDONE event and STOP task.
} nrf_pwm_short_mask_t;

/** @brief PWM modes of operation. */
typedef enum
{
    NRF_PWM_MODE_UP          = PWM_MODE_UPDOWN_Up,        ///< Up counter (edge-aligned PWM duty cycle).
    NRF_PWM_MODE_UP_AND_DOWN = PWM_MODE_UPDOWN_UpAndDown, ///< Up and down counter (center-aligned PWM duty cycle).
} nrf_pwm_mode_t;

/** @brief PWM base clock frequencies. */
typedef enum
{
    NRF_PWM_CLK_16MHz  = PWM_PRESCALER_PRESCALER_DIV_1,  ///< 16 MHz / 1 = 16 MHz.
    NRF_PWM_CLK_8MHz   = PWM_PRESCALER_PRESCALER_DIV_2,  ///< 16 MHz / 2 = 8 MHz.
    NRF_PWM_CLK_4MHz   = PWM_PRESCALER_PRESCALER_DIV_4,  ///< 16 MHz / 4 = 4 MHz.
    NRF_PWM_CLK_2MHz   = PWM_PRESCALER_PRESCALER_DIV_8,  ///< 16 MHz / 8 = 2 MHz.
    NRF_PWM_CLK_1MHz   = PWM_PRESCALER_PRESCALER_DIV_16, ///< 16 MHz / 16 = 1 MHz.
    NRF_PWM_CLK_500kHz = PWM_PRESCALER_PRESCALER_DIV_32, ///< 16 MHz / 32 = 500 kHz.
    NRF_PWM_CLK_250kHz = PWM_PRESCALER_PRESCALER_DIV_64, ///< 16 MHz / 64 = 250 kHz.
    NRF_PWM_CLK_125kHz = PWM_PRESCALER_PRESCALER_DIV_128 ///< 16 MHz / 128 = 125 kHz.
} nrf_pwm_clk_t;

/**
 * @brief PWM decoder load modes.
 *
 * The selected mode determines how the sequence data is read from RAM and
 * spread to the compare registers.
 */
typedef enum
{
    NRF_PWM_LOAD_COMMON     = PWM_DECODER_LOAD_Common,     ///< 1st half word (16-bit) used in all PWM channels (0-3).
    NRF_PWM_LOAD_GROUPED    = PWM_DECODER_LOAD_Grouped,    ///< 1st half word (16-bit) used in channels 0 and 1; 2nd word in channels 2 and 3.
    NRF_PWM_LOAD_INDIVIDUAL = PWM_DECODER_LOAD_Individual, ///< 1st half word (16-bit) used in channel 0; 2nd in channel 1; 3rd in channel 2; 4th in channel 3.
    NRF_PWM_LOAD_WAVE_FORM  = PWM_DECODER_LOAD_WaveForm    ///< 1st half word (16-bit) used in channel 0; 2nd in channel 1; ... ; 4th as the top value for the pulse generator counter.
} nrf_pwm_dec_load_t;

/**
 * @brief PWM decoder next step modes.
 *
 * The selected mode determines when the next value from the active sequence
 * is loaded.
 */
typedef enum
{
    NRF_PWM_STEP_AUTO      = PWM_DECODER_MODE_RefreshCount, ///< Automatically after the current value is played and repeated the requested number of times.
    NRF_PWM_STEP_TRIGGERED = PWM_DECODER_MODE_NextStep      ///< When the @ref NRF_PWM_TASK_NEXTSTEP task is triggered.
} nrf_pwm_dec_step_t;

/**
 * @brief Type used for defining duty cycle values for a sequence
 *        loaded in @ref NRF_PWM_LOAD_COMMON mode.
 */
typedef uint16_t nrf_pwm_values_common_t;

/**
 * @brief Structure for defining duty cycle values for a sequence
 *        loaded in @ref NRF_PWM_LOAD_GROUPED mode.
 */
typedef struct {
    uint16_t group_0; ///< Duty cycle value for group 0 (channels 0 and 1).
    uint16_t group_1; ///< Duty cycle value for group 1 (channels 2 and 3).
} nrf_pwm_values_grouped_t;

/**
 * @brief Structure for defining duty cycle values for a sequence
 *        loaded in @ref NRF_PWM_LOAD_INDIVIDUAL mode.
 */
typedef struct
{
    uint16_t channel_0; ///< Duty cycle value for channel 0.
    uint16_t channel_1; ///< Duty cycle value for channel 1.
    uint16_t channel_2; ///< Duty cycle value for channel 2.
    uint16_t channel_3; ///< Duty cycle value for channel 3.
} nrf_pwm_values_individual_t;

/**
 * @brief Structure for defining duty cycle values for a sequence
 *        loaded in @ref NRF_PWM_LOAD_WAVE_FORM mode.
 */
typedef struct {
    uint16_t channel_0;   ///< Duty cycle value for channel 0.
    uint16_t channel_1;   ///< Duty cycle value for channel 1.
    uint16_t channel_2;   ///< Duty cycle value for channel 2.
    uint16_t counter_top; ///< Top value for the pulse generator counter.
} nrf_pwm_values_wave_form_t;

/**
 * @brief Union grouping pointers to arrays of duty cycle values applicable to
 *        various loading modes.
 */
typedef union {
    nrf_pwm_values_common_t     const * p_common;     ///< Pointer to be used in @ref NRF_PWM_LOAD_COMMON mode.
    nrf_pwm_values_grouped_t    const * p_grouped;    ///< Pointer to be used in @ref NRF_PWM_LOAD_GROUPED mode.
    nrf_pwm_values_individual_t const * p_individual; ///< Pointer to be used in @ref NRF_PWM_LOAD_INDIVIDUAL mode.
    nrf_pwm_values_wave_form_t  const * p_wave_form;  ///< Pointer to be used in @ref NRF_PWM_LOAD_WAVE_FORM mode.
    uint16_t                    const * p_raw;        ///< Pointer providing raw access to the values.
} nrf_pwm_values_t;

/**
 * @brief Structure for defining a sequence of PWM duty cycles.
 *
 * When the sequence is set (by a call to @ref nrf_pwm_sequence_set), the
 * provided duty cycle values are not copied. The @p values pointer is stored
 * in the internal register of the peripheral, and the values are loaded from RAM
 * during the sequence playback. Therefore, you must ensure that the values
 * do not change before and during the sequence playback (for example,
 * the values cannot be placed in a local variable that is allocated on stack).
 * If the sequence is played in a loop and the values are to be updated
 * before the next iteration, it is safe to modify them when the corresponding
 * event signaling the end of sequence occurs (@ref NRF_PWM_EVENT_SEQEND0
 * or @ref NRF_PWM_EVENT_SEQEND1, respectively).
 *
 * @note The @p repeats and @p end_delay values (which are written to the
 *       SEQ[n].REFRESH and SEQ[n].ENDDELAY registers in the peripheral,
 *       respectively) are ignored at the end of a complex sequence
 *       playback, indicated by the LOOPSDONE event.
 *       See the Product Specification for more information.
 */
typedef struct
{
    nrf_pwm_values_t values; ///< Pointer to an array with duty cycle values. This array must be in Data RAM.
                             /**< This field is defined as an union of pointers
                              *   to provide a convenient way to define duty
                              *   cycle values in various loading modes
                              *   (see @ref nrf_pwm_dec_load_t).
                              *   In each value, the most significant bit (15)
                              *   determines the polarity of the output and the
                              *   others (14-0) compose the 15-bit value to be
                              *   compared with the pulse generator counter. */
    uint16_t length;    ///< Number of 16-bit values in the array pointed by @p values.
    uint32_t repeats;   ///< Number of times that each duty cycle is to be repeated (after being played once). Ignored in @ref NRF_PWM_STEP_TRIGGERED mode.
    uint32_t end_delay; ///< Additional time (in PWM periods) that the last duty cycle is to be kept after the sequence is played. Ignored in @ref NRF_PWM_STEP_TRIGGERED mode.
} nrf_pwm_sequence_t;


/**
 * @brief Function for activating the specified PWM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_pwm_task_trigger(NRF_PWM_Type * p_reg,
                                            nrf_pwm_task_t task);

/**
 * @brief Function for getting the address of the specified PWM task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  PWM task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_pwm_task_address_get(NRF_PWM_Type const * p_reg,
                                                    nrf_pwm_task_t       task);

/**
 * @brief Function for clearing the specified PWM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_pwm_event_clear(NRF_PWM_Type *  p_reg,
                                           nrf_pwm_event_t event);

/**
 * @brief Function for retrieving the state of the PWM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_pwm_event_check(NRF_PWM_Type const * p_reg,
                                           nrf_pwm_event_t      event);

/**
 * @brief Function for getting the address of the specified PWM event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event PWM event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_pwm_event_address_get(NRF_PWM_Type const * p_reg,
                                                     nrf_pwm_event_t      event);

/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_pwm_shorts_enable(NRF_PWM_Type * p_reg,
                                             uint32_t       mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_pwm_shorts_disable(NRF_PWM_Type * p_reg,
                                              uint32_t       mask);

/**
 * @brief Function for setting the configuration of PWM shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts configuration to be set.
 */
NRF_STATIC_INLINE void nrf_pwm_shorts_set(NRF_PWM_Type * p_reg,
                                          uint32_t       mask);

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_pwm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_pwm_int_enable(NRF_PWM_Type * p_reg,
                                          uint32_t       mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_pwm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_pwm_int_disable(NRF_PWM_Type * p_reg,
                                           uint32_t       mask);

/**
 * @brief Function for setting the configuration of PWM interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be set.
 *                  Use @ref nrf_pwm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_pwm_int_set(NRF_PWM_Type * p_reg,
                                       uint32_t       mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_pwm_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_pwm_int_enable_check(NRF_PWM_Type const * p_reg, uint32_t mask);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        PWM task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_pwm_subscribe_set(NRF_PWM_Type * p_reg,
                                             nrf_pwm_task_t task,
                                             uint8_t        channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        PWM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_pwm_subscribe_clear(NRF_PWM_Type * p_reg,
                                               nrf_pwm_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        PWM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return PWM subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_pwm_subscribe_get(NRF_PWM_Type const * p_reg,
                                                 nrf_pwm_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        PWM event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_pwm_publish_set(NRF_PWM_Type *  p_reg,
                                           nrf_pwm_event_t event,
                                           uint8_t         channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        PWM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_pwm_publish_clear(NRF_PWM_Type *  p_reg,
                                             nrf_pwm_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        PWM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return PWM publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_pwm_publish_get(NRF_PWM_Type const * p_reg,
                                               nrf_pwm_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for enabling the PWM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_pwm_enable(NRF_PWM_Type * p_reg);

/**
 * @brief Function for disabling the PWM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_pwm_disable(NRF_PWM_Type * p_reg);

/**
 * @brief Function for checking if the PWM peripheral is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The PWM is enabled.
 * @retval false The PWM is not enabled.
 */
NRF_STATIC_INLINE bool nrf_pwm_enable_check(NRF_PWM_Type const * p_reg);

/**
 * @brief Function for assigning pins to PWM output channels.
 *
 * Usage of all PWM output channels is optional. If a given channel is not
 * needed, pass the @ref NRF_PWM_PIN_NOT_CONNECTED value instead of its pin
 * number.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] out_pins Array with pin numbers for individual PWM output channels.
 */
NRF_STATIC_INLINE void nrf_pwm_pins_set(NRF_PWM_Type * p_reg,
                                        uint32_t const out_pins[NRF_PWM_CHANNEL_COUNT]);

/**
 * @brief Function for setting pin associated with specified PWM output channel.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel PWM output channel.
 * @param[in] pin     PWM output pin associated with specified PWM output channel.
 */
NRF_STATIC_INLINE void nrf_pwm_pin_set(NRF_PWM_Type * p_reg, uint8_t channel, uint32_t pin);

/**
 * @brief Function for getting pin selection associated with specified PWM output channel.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel PWM output channel.
 *
 * @return Pin selection associated with specified PWM output channel.
 */
NRF_STATIC_INLINE uint32_t nrf_pwm_pin_get(NRF_PWM_Type const * p_reg, uint8_t channel);

/**
 * @brief Function for configuring the PWM peripheral.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] base_clock Base clock frequency.
 * @param[in] mode       Operating mode of the pulse generator counter.
 * @param[in] top_value  Value up to which the pulse generator counter counts.
 */
NRF_STATIC_INLINE void nrf_pwm_configure(NRF_PWM_Type * p_reg,
                                         nrf_pwm_clk_t  base_clock,
                                         nrf_pwm_mode_t mode,
                                         uint16_t       top_value);

/**
 * @brief Function for defining a sequence of PWM duty cycles.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] seq_id Identifier of the sequence (0 or 1).
 * @param[in] p_seq  Pointer to the sequence definition.
 */
NRF_STATIC_INLINE void nrf_pwm_sequence_set(NRF_PWM_Type *             p_reg,
                                            uint8_t                    seq_id,
                                            nrf_pwm_sequence_t const * p_seq);

/**
 * @brief Function for modifying the pointer to the duty cycle values
 *        in the specified sequence.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] seq_id   Identifier of the sequence (0 or 1).
 * @param[in] p_values Pointer to an array with duty cycle values.
 */
NRF_STATIC_INLINE void nrf_pwm_seq_ptr_set(NRF_PWM_Type *   p_reg,
                                           uint8_t          seq_id,
                                           uint16_t const * p_values);

/**
 * @brief Function for modifying the total number of duty cycle values
 *        in the specified sequence.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] seq_id Identifier of the sequence (0 or 1).
 * @param[in] length Number of duty cycle values (in 16-bit half words).
 */
NRF_STATIC_INLINE void nrf_pwm_seq_cnt_set(NRF_PWM_Type * p_reg,
                                           uint8_t        seq_id,
                                           uint16_t       length);

/**
 * @brief Function for modifying the additional number of PWM periods spent
 *        on each duty cycle value in the specified sequence.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] seq_id  Identifier of the sequence (0 or 1).
 * @param[in] refresh Number of additional PWM periods for each duty cycle value.
 */
NRF_STATIC_INLINE void nrf_pwm_seq_refresh_set(NRF_PWM_Type * p_reg,
                                               uint8_t        seq_id,
                                               uint32_t       refresh);

/**
 * @brief Function for modifying the additional time added after the sequence
 *        is played.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] seq_id    Identifier of the sequence (0 or 1).
 * @param[in] end_delay Number of PWM periods added at the end of the sequence.
 */
NRF_STATIC_INLINE void nrf_pwm_seq_end_delay_set(NRF_PWM_Type * p_reg,
                                                 uint8_t        seq_id,
                                                 uint32_t       end_delay);

/**
 * @brief Function for setting the mode of loading sequence data from RAM
 *        and advancing the sequence.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] dec_load Mode of loading sequence data from RAM.
 * @param[in] dec_step Mode of advancing the active sequence.
 */
NRF_STATIC_INLINE void nrf_pwm_decoder_set(NRF_PWM_Type *     p_reg,
                                           nrf_pwm_dec_load_t dec_load,
                                           nrf_pwm_dec_step_t dec_step);

/**
 * @brief Function for setting the number of times the sequence playback
 *        should be performed.
 *
 * This function applies to two-sequence playback (concatenated sequence 0 and 1).
 * A single sequence can be played back only once.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] loop_count Number of times to perform the sequence playback.
 */
NRF_STATIC_INLINE void nrf_pwm_loop_set(NRF_PWM_Type * p_reg, uint16_t loop_count);

/**
 * @brief Function for getting the specified PWM SEQSTART task.
 *
 * @param[in] seq_id Sequence index.
 *
 * @return The specified PWM SEQSTART task.
 */
NRF_STATIC_INLINE nrf_pwm_task_t nrf_pwm_seqstart_task_get(uint8_t seq_id);

/**
 * @brief Function for getting the specified PWM SEQEND event.
 *
 * @param[in] seq_id Sequence index.
 *
 * @return The specified PWM SEQEND event.
 */
NRF_STATIC_INLINE nrf_pwm_event_t nrf_pwm_seqend_event_get(uint8_t seq_id);

#if NRF_PWM_HAS_IDLEOUT
/**
 * @brief Function for setting the output value of specified channel while
 *        PDM is idle.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel PWM output channel.
 * @param[in] value   If true, idle output will be high. If false, it will be low.
 */
NRF_STATIC_INLINE void nrf_pwm_channel_idle_set(NRF_PWM_Type * p_reg,
                                                uint8_t        channel,
                                                bool           value);

/**
 * @brief Function for getting the output value of specified channel while
 *        PDM is idle.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] channel PWM output channel.
 *
 * @retval true  Idle output value is high.
 * @retval false Idle output value is low.
 */
NRF_STATIC_INLINE bool nrf_pwm_channel_idle_get(NRF_PWM_Type const * p_reg,
                                                uint8_t              channel);
#endif // NRF_PWM_HAS_IDLEOUT

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_pwm_task_trigger(NRF_PWM_Type * p_reg,
                                            nrf_pwm_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_pwm_task_address_get(NRF_PWM_Type const * p_reg,
                                                    nrf_pwm_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE void nrf_pwm_event_clear(NRF_PWM_Type *  p_reg,
                                           nrf_pwm_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_pwm_event_check(NRF_PWM_Type const * p_reg,
                                           nrf_pwm_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_pwm_event_address_get(NRF_PWM_Type const * p_reg,
                                                     nrf_pwm_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_pwm_shorts_enable(NRF_PWM_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_pwm_shorts_disable(NRF_PWM_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE void nrf_pwm_shorts_set(NRF_PWM_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS = mask;
}

NRF_STATIC_INLINE void nrf_pwm_int_enable(NRF_PWM_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_pwm_int_disable(NRF_PWM_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE void nrf_pwm_int_set(NRF_PWM_Type * p_reg, uint32_t mask)
{
    p_reg->INTEN = mask;
}

NRF_STATIC_INLINE uint32_t nrf_pwm_int_enable_check(NRF_PWM_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_pwm_subscribe_set(NRF_PWM_Type * p_reg,
                                             nrf_pwm_task_t task,
                                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_pwm_subscribe_clear(NRF_PWM_Type * p_reg,
                                               nrf_pwm_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_pwm_subscribe_get(NRF_PWM_Type const * p_reg,
                                                 nrf_pwm_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_pwm_publish_set(NRF_PWM_Type *  p_reg,
                                           nrf_pwm_event_t event,
                                           uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_pwm_publish_clear(NRF_PWM_Type *  p_reg,
                                             nrf_pwm_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_pwm_publish_get(NRF_PWM_Type const * p_reg,
                                               nrf_pwm_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE void nrf_pwm_enable(NRF_PWM_Type * p_reg)
{
    p_reg->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_pwm_disable(NRF_PWM_Type * p_reg)
{
    p_reg->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE bool nrf_pwm_enable_check(NRF_PWM_Type const * p_reg)
{
    return p_reg->ENABLE == PWM_ENABLE_ENABLE_Enabled;
}

NRF_STATIC_INLINE void nrf_pwm_pins_set(NRF_PWM_Type * p_reg,
                                        uint32_t const out_pins[NRF_PWM_CHANNEL_COUNT])
{
    uint8_t i;
    for (i = 0; i < NRF_PWM_CHANNEL_COUNT; ++i)
    {
        p_reg->PSEL.OUT[i] = out_pins[i];
    }
}

NRF_STATIC_INLINE void nrf_pwm_pin_set(NRF_PWM_Type * p_reg, uint8_t channel, uint32_t pin)
{
    NRFX_ASSERT(channel < NRF_PWM_CHANNEL_COUNT);
    p_reg->PSEL.OUT[channel] = pin;
}

NRF_STATIC_INLINE uint32_t nrf_pwm_pin_get(NRF_PWM_Type const * p_reg, uint8_t channel)
{
    NRFX_ASSERT(channel < NRF_PWM_CHANNEL_COUNT);
    return p_reg->PSEL.OUT[channel];
}

NRF_STATIC_INLINE void nrf_pwm_configure(NRF_PWM_Type * p_reg,
                                         nrf_pwm_clk_t  base_clock,
                                         nrf_pwm_mode_t mode,
                                         uint16_t       top_value)
{
    NRFX_ASSERT(top_value <= PWM_COUNTERTOP_COUNTERTOP_Msk);

    p_reg->PRESCALER  = base_clock;
    p_reg->MODE       = mode;
    p_reg->COUNTERTOP = top_value;
}

NRF_STATIC_INLINE void nrf_pwm_sequence_set(NRF_PWM_Type *             p_reg,
                                            uint8_t                    seq_id,
                                            nrf_pwm_sequence_t const * p_seq)
{
    NRFX_ASSERT(p_seq != NULL);

    nrf_pwm_seq_ptr_set(      p_reg, seq_id, p_seq->values.p_raw);
    nrf_pwm_seq_cnt_set(      p_reg, seq_id, p_seq->length);
    nrf_pwm_seq_refresh_set(  p_reg, seq_id, p_seq->repeats);
    nrf_pwm_seq_end_delay_set(p_reg, seq_id, p_seq->end_delay);
}

NRF_STATIC_INLINE void nrf_pwm_seq_ptr_set(NRF_PWM_Type *   p_reg,
                                           uint8_t          seq_id,
                                           uint16_t const * p_values)
{
    NRFX_ASSERT(seq_id <= 1);
    NRFX_ASSERT(p_values != NULL);
#if NRF_PWM_HAS_DMA_REG
    p_reg->DMA.SEQ[seq_id].PTR = (uint32_t)p_values;
#else
    p_reg->SEQ[seq_id].PTR = (uint32_t)p_values;
#endif
}

NRF_STATIC_INLINE void nrf_pwm_seq_cnt_set(NRF_PWM_Type * p_reg,
                                           uint8_t        seq_id,
                                           uint16_t       length)
{
    NRFX_ASSERT(seq_id <= 1);
    NRFX_ASSERT(length != 0);
#if NRF_PWM_HAS_DMA_REG
    NRFX_ASSERT(length * sizeof(uint16_t) <= PWM_DMA_SEQ_MAXCNT_MAXCNT_Msk);
    p_reg->DMA.SEQ[seq_id].MAXCNT = length * sizeof(uint16_t);
#else
    NRFX_ASSERT(length <= PWM_SEQ_CNT_CNT_Msk);
    p_reg->SEQ[seq_id].CNT = length;
#endif
}

NRF_STATIC_INLINE void nrf_pwm_seq_refresh_set(NRF_PWM_Type * p_reg,
                                               uint8_t        seq_id,
                                               uint32_t       refresh)
{
    NRFX_ASSERT(seq_id <= 1);
    NRFX_ASSERT(refresh <= PWM_SEQ_REFRESH_CNT_Msk);
    p_reg->SEQ[seq_id].REFRESH  = refresh;
}

NRF_STATIC_INLINE void nrf_pwm_seq_end_delay_set(NRF_PWM_Type * p_reg,
                                                 uint8_t        seq_id,
                                                 uint32_t       end_delay)
{
    NRFX_ASSERT(seq_id <= 1);
    NRFX_ASSERT(end_delay <= PWM_SEQ_ENDDELAY_CNT_Msk);
    p_reg->SEQ[seq_id].ENDDELAY = end_delay;
}

NRF_STATIC_INLINE void nrf_pwm_decoder_set(NRF_PWM_Type *     p_reg,
                                           nrf_pwm_dec_load_t dec_load,
                                           nrf_pwm_dec_step_t dec_step)
{
    p_reg->DECODER = ((uint32_t)dec_load << PWM_DECODER_LOAD_Pos) |
                     ((uint32_t)dec_step << PWM_DECODER_MODE_Pos);
}

NRF_STATIC_INLINE void nrf_pwm_loop_set(NRF_PWM_Type * p_reg,
                                        uint16_t       loop_count)
{
    p_reg->LOOP = loop_count;
}

NRF_STATIC_INLINE nrf_pwm_task_t nrf_pwm_seqstart_task_get(uint8_t seq_id)
{
    NRFX_ASSERT(seq_id <= 1);
#if NRF_PWM_HAS_DMA_TASKS_EVENTS
    return (nrf_pwm_task_t)NRFX_OFFSETOF(NRF_PWM_Type, TASKS_DMA.SEQ[seq_id].START);
#else
    return (nrf_pwm_task_t)NRFX_OFFSETOF(NRF_PWM_Type, TASKS_SEQSTART[seq_id]);
#endif
}

NRF_STATIC_INLINE nrf_pwm_event_t nrf_pwm_seqend_event_get(uint8_t seq_id)
{
    NRFX_ASSERT(seq_id <= 1);
#if NRF_PWM_HAS_DMA_TASKS_EVENTS
    return (nrf_pwm_event_t)NRFX_OFFSETOF(NRF_PWM_Type, EVENTS_DMA.SEQ[seq_id].END);
#else
    return (nrf_pwm_event_t)NRFX_OFFSETOF(NRF_PWM_Type, EVENTS_SEQEND[seq_id]);
#endif
}

#if NRF_PWM_HAS_IDLEOUT
NRF_STATIC_INLINE void nrf_pwm_channel_idle_set(NRF_PWM_Type * p_reg,
                                                uint8_t        channel,
                                                bool           value)
{
    if (value)
    {
        p_reg->IDLEOUT |= (1UL << channel);
    }
    else
    {
        p_reg->IDLEOUT &= ~(1UL << channel);
    }
}

NRF_STATIC_INLINE bool nrf_pwm_channel_idle_get(NRF_PWM_Type const * p_reg,
                                                uint8_t              channel)
{
    return ((p_reg->IDLEOUT >> channel) & 1UL);
}
#endif // NRF_PWM_HAS_IDLEOUT

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_PWM_H__
