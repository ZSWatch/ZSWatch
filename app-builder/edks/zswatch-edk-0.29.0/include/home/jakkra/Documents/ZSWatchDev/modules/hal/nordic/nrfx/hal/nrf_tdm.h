/*
 * Copyright (c) 2024 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_TDM_H__
#define NRF_TDM_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TDM_CLOCKPIN_SCK_NEEDED)
#define NRF_TDM_CLOCKPIN_SCK_NEEDED 1
#endif
#if defined(TDM_CLOCKPIN_FSYNC_NEEDED)
#define NRF_TDM_CLOCKPIN_FSYNC_NEEDED 1
#endif
#if defined(TDM_CLOCKPIN_MCK_NEEDED)
#define NRF_TDM_CLOCKPIN_MCK_NEEDED 1
#endif

/**
 * @defgroup nrf_tdm_hal TDM HAL
 * @{
 * @ingroup nrf_tdm
 * @brief   Hardware access layer for managing the Time Division Multiplexed Audio Interface
 *          (TDM) peripheral.
 */

/**
 * @brief This value can be provided as a parameter for the @ref nrf_tdm_pins_set
 *        function call to specify that the given TDM signal (SDOUT, SDIN, or MCK)
 *        shall not be connected to a physical pin.
 */
#define NRF_TDM_PIN_NOT_CONNECTED UINT32_MAX 

/** @brief TDM SCK pin selection mask. */
#define NRF_TDM_PSEL_SCK_PIN_MASK  TDM_PSEL_SCK_PIN_Msk

/** @brief TDM SCK port selection mask. */
#define NRF_TDM_PSEL_SCK_PORT_MASK TDM_PSEL_SCK_PORT_Msk

/** @brief TDM tasks. */
typedef enum
{
    NRF_TDM_TASK_START = offsetof(NRF_TDM_Type, TASKS_START), ///< Starts continuous TDM transfer. Also starts the MCK generator if this is enabled.
    NRF_TDM_TASK_STOP  = offsetof(NRF_TDM_Type, TASKS_STOP),  ///< Stops TDM transfer after completion of MAXCNT words.
    NRF_TDM_TASK_ABORT = offsetof(NRF_TDM_Type, TASKS_ABORT)  ///< Aborts TDM transfer without completing MAXCNT words.
} nrf_tdm_task_t;

/** @brief TDM events. */
typedef enum
{
    NRF_TDM_EVENT_RXPTRUPD    = offsetof(NRF_TDM_Type, EVENTS_RXPTRUPD),    ///< The RXD.PTR register has been copied to internal double buffers.
    NRF_TDM_EVENT_STOPPED     = offsetof(NRF_TDM_Type, EVENTS_STOPPED),     ///< TDM transfer stopped.
    NRF_TDM_EVENT_ABORTED     = offsetof(NRF_TDM_Type, EVENTS_ABORTED),     ///< TDM transfer aborted.
    NRF_TDM_EVENT_TXPTRUPD    = offsetof(NRF_TDM_Type, EVENTS_TXPTRUPD),    ///< The TXD.PTR register has been copied to internal double buffers.
    NRF_TDM_EVENT_MAXCNT      = offsetof(NRF_TDM_Type, EVENTS_MAXCNT),      ///< MAXCNT block event, generated on the active edge of FSYNC of every MAXCNT block.
} nrf_tdm_event_t;

/** @brief TDM interrupts. */
typedef enum
{
    NRF_TDM_INT_RXPTRUPD_MASK_MASK = TDM_INTENSET_RXPTRUPD_Msk,    ///< Interrupt on RXPTRUPD event.
    NRF_TDM_INT_STOPPED_MASK_MASK  = TDM_INTENSET_STOPPED_Msk,     ///< Interrupt on STOPPED event.
    NRF_TDM_INT_ABORTED_MASK       = TDM_INTENSET_ABORTED_Msk,     ///< Interrupt on EVENTS_ABORTED event.
    NRF_TDM_INT_TXPTRUPD_MASK_MASK = TDM_INTENSET_TXPTRUPD_Msk,    ///< Interrupt on TXPTRUPD event.
    NRF_TDM_INT_MAXCNT_MASK        = TDM_INTENSET_MAXCNT_Msk,      ///< Interrupt on EVENTS_MAXCNT event.
} nrf_tdm_int_mask_t;

/** @brief TDM modes of operation. */
typedef enum
{
    NRF_TDM_MODE_MASTER = TDM_CONFIG_MODE_MODE_Master, ///< Master mode.
    NRF_TDM_MODE_SLAVE  = TDM_CONFIG_MODE_MODE_Slave   ///< Slave mode.
} nrf_tdm_mode_t;

/** @brief TDM Reception and transmission settings. */
typedef enum
{
    NRF_TDM_RXTXEN_DUPLEX = TDM_CONFIG_RXTXEN_RXTXEN_Duplex, ///< Enable reception and transmission.
    NRF_TDM_RXTXEN_RX     = TDM_CONFIG_RXTXEN_RXTXEN_Rx,     ///< Enable reception, disable transmission.
    NRF_TDM_RXTXEN_TX     = TDM_CONFIG_RXTXEN_RXTXEN_Tx      ///< Enable transmission, disable reception.
} nrf_tdm_rxtxen_t;

/** @brief TDM master clock divider settings. */
typedef enum
{
    NRF_TDM_MCK_DIV_2   = TDM_CONFIG_MCK_DIV_DIV_CKDIV2,  ///< MCK divided by 2.
    NRF_TDM_MCK_DIV_3   = TDM_CONFIG_MCK_DIV_DIV_CKDIV3,  ///< MCK divided by 3.
    NRF_TDM_MCK_DIV_4   = TDM_CONFIG_MCK_DIV_DIV_CKDIV4,  ///< MCK divided by 4.
    NRF_TDM_MCK_DIV_5   = TDM_CONFIG_MCK_DIV_DIV_CKDIV5,  ///< MCK divided by 5.
    NRF_TDM_MCK_DIV_6   = TDM_CONFIG_MCK_DIV_DIV_CKDIV6,  ///< MCK divided by 6.
    NRF_TDM_MCK_DIV_8   = TDM_CONFIG_MCK_DIV_DIV_CKDIV8,  ///< MCK divided by 8.
    NRF_TDM_MCK_DIV_10  = TDM_CONFIG_MCK_DIV_DIV_CKDIV10, ///< MCK divided by 10.
    NRF_TDM_MCK_DIV_11  = TDM_CONFIG_MCK_DIV_DIV_CKDIV11, ///< MCK divided by 11.
    NRF_TDM_MCK_DIV_15  = TDM_CONFIG_MCK_DIV_DIV_CKDIV15, ///< MCK divided by 15.
    NRF_TDM_MCK_DIV_16  = TDM_CONFIG_MCK_DIV_DIV_CKDIV16, ///< MCK divided by 16.
    NRF_TDM_MCK_DIV_21  = TDM_CONFIG_MCK_DIV_DIV_CKDIV21, ///< MCK divided by 21.
    NRF_TDM_MCK_DIV_23  = TDM_CONFIG_MCK_DIV_DIV_CKDIV23, ///< MCK divided by 23.
    NRF_TDM_MCK_DIV_30  = TDM_CONFIG_MCK_DIV_DIV_CKDIV30, ///< MCK divided by 30.
    NRF_TDM_MCK_DIV_31  = TDM_CONFIG_MCK_DIV_DIV_CKDIV31, ///< MCK divided by 31.
    NRF_TDM_MCK_DIV_32  = TDM_CONFIG_MCK_DIV_DIV_CKDIV32, ///< MCK divided by 32.
    NRF_TDM_MCK_DIV_42  = TDM_CONFIG_MCK_DIV_DIV_CKDIV42, ///< MCK divided by 42.
    NRF_TDM_MCK_DIV_63  = TDM_CONFIG_MCK_DIV_DIV_CKDIV63, ///< MCK divided by 63.
    NRF_TDM_MCK_DIV_125 = TDM_CONFIG_MCK_DIV_DIV_CKDIV125 ///< MCK divided by 125.
} nrf_tdm_mck_div_t;

/** @brief TDM serial clock divider settings. */
typedef enum
{
    NRF_TDM_SCK_DIV_2   = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV2,  ///< SCK divided by 2.
    NRF_TDM_SCK_DIV_3   = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV3,  ///< SCK divided by 3.
    NRF_TDM_SCK_DIV_4   = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV4,  ///< SCK divided by 4.
    NRF_TDM_SCK_DIV_5   = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV5,  ///< SCK divided by 5.
    NRF_TDM_SCK_DIV_6   = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV6,  ///< SCK divided by 6.
    NRF_TDM_SCK_DIV_8   = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV8,  ///< SCK divided by 8.
    NRF_TDM_SCK_DIV_10  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV10, ///< SCK divided by 10.
    NRF_TDM_SCK_DIV_11  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV11, ///< SCK divided by 11.
    NRF_TDM_SCK_DIV_15  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV15, ///< SCK divided by 15.
    NRF_TDM_SCK_DIV_16  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV16, ///< SCK divided by 16.
    NRF_TDM_SCK_DIV_21  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV21, ///< SCK divided by 21.
    NRF_TDM_SCK_DIV_23  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV23, ///< SCK divided by 23.
    NRF_TDM_SCK_DIV_30  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV30, ///< SCK divided by 30.
    NRF_TDM_SCK_DIV_31  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV31, ///< SCK divided by 31.
    NRF_TDM_SCK_DIV_32  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV32, ///< SCK divided by 32.
    NRF_TDM_SCK_DIV_42  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV42, ///< SCK divided by 42.
    NRF_TDM_SCK_DIV_63  = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV63, ///< SCK divided by 63.
    NRF_TDM_SCK_DIV_125 = TDM_CONFIG_SCK_DIV_SCKDIV_CKDIV125 ///< SCK divided by 125.
} nrf_tdm_sck_div_t;

/** @brief TDM clock source selection. */
typedef enum
{
#if defined(TDM_CONFIG_SCK_SRC_CLKSRC_PCLK) || defined(__NRFX_DOXYGEN__)
    NRF_TDM_SRC_PCLK32M = TDM_CONFIG_SCK_SRC_CLKSRC_PCLK,    ///< 32MHz peripheral clock.
#elif defined(TDM_CONFIG_SCK_SRC_CLKSRC_PCLK32M)
    NRF_TDM_SRC_PCLK32M = TDM_CONFIG_SCK_SRC_CLKSRC_PCLK32M, ///< 32MHz peripheral clock.
#endif
    NRF_TDM_SRC_ACLK    = TDM_CONFIG_SCK_SRC_CLKSRC_ACLK     ///< Audio PLL clock.
}  nrf_tdm_src_t;

/** @brief TDM sample widths. */
typedef enum
{
    NRF_TDM_SWIDTH_8BIT          = TDM_CONFIG_SWIDTH_SWIDTH_8Bit,      ///< 8 bit.
    NRF_TDM_SWIDTH_16BIT         = TDM_CONFIG_SWIDTH_SWIDTH_16Bit,     ///< 16 bit.
    NRF_TDM_SWIDTH_24BIT         = TDM_CONFIG_SWIDTH_SWIDTH_24Bit,     ///< 24 bit.
    NRF_TDM_SWIDTH_32BIT         = TDM_CONFIG_SWIDTH_SWIDTH_32Bit,     ///< 32 bit.
    NRF_TDM_SWIDTH_8BIT_IN16BIT  = TDM_CONFIG_SWIDTH_SWIDTH_8BitIn16,  ///< 8 bit sample in a 16-bit half-frame.
    NRF_TDM_SWIDTH_8BIT_IN32BIT  = TDM_CONFIG_SWIDTH_SWIDTH_8BitIn32,  ///< 8 bit sample in a 32-bit half-frame.
    NRF_TDM_SWIDTH_16BIT_IN32BIT = TDM_CONFIG_SWIDTH_SWIDTH_16BitIn32, ///< 16 bit sample in a 32-bit half-frame.
    NRF_TDM_SWIDTH_24BIT_IN32BIT = TDM_CONFIG_SWIDTH_SWIDTH_24BitIn32, ///< 24 bit sample in a 32-bit half-frame.
} nrf_tdm_swidth_t;

/** @brief TDM alignments of sample within a frame. */
typedef enum
{
    NRF_TDM_ALIGN_LEFT  = TDM_CONFIG_ALIGN_ALIGN_Left, ///< Left-aligned.
    NRF_TDM_ALIGN_RIGHT = TDM_CONFIG_ALIGN_ALIGN_Right ///< Right-aligned.
} nrf_tdm_align_t;

/** @brief TDM channel mask. */
typedef enum
{
    NRF_TDM_CHANNEL_RX0_MASK = TDM_CONFIG_CHANNEL_MASK_Rx0Enable_Msk, ///< Enable RX channel 0 data.
    NRF_TDM_CHANNEL_RX1_MASK = TDM_CONFIG_CHANNEL_MASK_Rx1Enable_Msk, ///< Enable RX channel 1 data.
    NRF_TDM_CHANNEL_RX2_MASK = TDM_CONFIG_CHANNEL_MASK_Rx2Enable_Msk, ///< Enable RX channel 2 data.
    NRF_TDM_CHANNEL_RX3_MASK = TDM_CONFIG_CHANNEL_MASK_Rx3Enable_Msk, ///< Enable RX channel 3 data.
    NRF_TDM_CHANNEL_RX4_MASK = TDM_CONFIG_CHANNEL_MASK_Rx4Enable_Msk, ///< Enable RX channel 4 data.
    NRF_TDM_CHANNEL_RX5_MASK = TDM_CONFIG_CHANNEL_MASK_Rx5Enable_Msk, ///< Enable RX channel 5 data.
    NRF_TDM_CHANNEL_RX6_MASK = TDM_CONFIG_CHANNEL_MASK_Rx6Enable_Msk, ///< Enable RX channel 6 data.
    NRF_TDM_CHANNEL_RX7_MASK = TDM_CONFIG_CHANNEL_MASK_Rx7Enable_Msk, ///< Enable RX channel 7 data.
    NRF_TDM_CHANNEL_TX0_MASK = TDM_CONFIG_CHANNEL_MASK_Tx0Enable_Msk, ///< Enable TX channel 0 data.
    NRF_TDM_CHANNEL_TX1_MASK = TDM_CONFIG_CHANNEL_MASK_Tx1Enable_Msk, ///< Enable TX channel 1 data.
    NRF_TDM_CHANNEL_TX2_MASK = TDM_CONFIG_CHANNEL_MASK_Tx2Enable_Msk, ///< Enable TX channel 2 data.
    NRF_TDM_CHANNEL_TX3_MASK = TDM_CONFIG_CHANNEL_MASK_Tx3Enable_Msk, ///< Enable TX channel 3 data.
    NRF_TDM_CHANNEL_TX4_MASK = TDM_CONFIG_CHANNEL_MASK_Tx4Enable_Msk, ///< Enable TX channel 4 data.
    NRF_TDM_CHANNEL_TX5_MASK = TDM_CONFIG_CHANNEL_MASK_Tx5Enable_Msk, ///< Enable TX channel 5 data.
    NRF_TDM_CHANNEL_TX6_MASK = TDM_CONFIG_CHANNEL_MASK_Tx6Enable_Msk, ///< Enable TX channel 6 data.
    NRF_TDM_CHANNEL_TX7_MASK = TDM_CONFIG_CHANNEL_MASK_Tx7Enable_Msk, ///< Enable TX channel 7 data.
} nrf_tdm_channel_mask_t;

/** @brief TDM number of channels. */
typedef enum
{
    NRF_TDM_CHANNELS_COUNT_1 = TDM_CONFIG_CHANNEL_NUM_NUM_Tdm1Ch, ///< 1 channel audio (mono).
    NRF_TDM_CHANNELS_COUNT_2 = TDM_CONFIG_CHANNEL_NUM_NUM_Tdm2Ch, ///< 2 channels audio (stereo).
    NRF_TDM_CHANNELS_COUNT_3 = TDM_CONFIG_CHANNEL_NUM_NUM_Tdm3Ch, ///< 3 channels audio.
    NRF_TDM_CHANNELS_COUNT_4 = TDM_CONFIG_CHANNEL_NUM_NUM_Tdm4Ch, ///< 4 channels audio.
    NRF_TDM_CHANNELS_COUNT_5 = TDM_CONFIG_CHANNEL_NUM_NUM_Tdm5Ch, ///< 5 channels audio.
    NRF_TDM_CHANNELS_COUNT_6 = TDM_CONFIG_CHANNEL_NUM_NUM_Tdm6Ch, ///< 6 channels audio.
    NRF_TDM_CHANNELS_COUNT_7 = TDM_CONFIG_CHANNEL_NUM_NUM_Tdm7Ch, ///< 7 channels audio.
    NRF_TDM_CHANNELS_COUNT_8 = TDM_CONFIG_CHANNEL_NUM_NUM_Tdm8Ch, ///< 8 channels audio.
} nrf_tdm_channels_count_t;

/** @brief TDM channel delay. */
typedef enum
{
    NRF_TDM_CHANNEL_DELAY_NONE = TDM_CONFIG_CHANNEL_DELAY_DELAY_Delay0Ck, ///< No delay. Use with DSP/Aligned format.
    NRF_TDM_CHANNEL_DELAY_1CK  = TDM_CONFIG_CHANNEL_DELAY_DELAY_Delay1Ck, ///< 1 clock pulse delay. Used with original TDM format.
    NRF_TDM_CHANNEL_DELAY_2CK  = TDM_CONFIG_CHANNEL_DELAY_DELAY_Delay2Ck, ///< 2 clock pulses delay.
} nrf_tdm_channel_delay_t;

/** @brief TDM signal polarity. */
typedef enum
{
    NRF_TDM_POLARITY_POSEDGE, ///< Synchronization at rising edge of the reference signal.
    NRF_TDM_POLARITY_NEGEDGE, ///< Synchronization at falling edge of the reference signal.
}  nrf_tdm_polarity_t;

/** @brief TDM frame synchronization pulse duration. */
typedef enum
{
    NRF_TDM_FSYNC_DURATION_SCK     = TDM_CONFIG_FSYNC_DURATION_DURATION_Sck,    ///< FSYNC is active for the duration of one SCK pulse.
    NRF_TDM_FSYNC_DURATION_CHANNEL = TDM_CONFIG_FSYNC_DURATION_DURATION_Channel ///< FSYNC is active for the duration of channel transmission.
}  nrf_tdm_fsync_duration_t;

/** @brief TDM configuration. */
typedef struct
{
    nrf_tdm_mode_t           mode;            /**< Mode of operation (master or slave). */
    nrf_tdm_align_t          alignment;       /**< Alignment of sample within a frame. */
    nrf_tdm_swidth_t         sample_width;    /**< Sample width. */
    nrf_tdm_channel_mask_t   channels;        /**< Enabled channels. */
    nrf_tdm_channels_count_t num_of_channels; /**< Nnumber of channels within a frame. */
    nrf_tdm_channel_delay_t  channel_delay;   /**< Channel delay settings. */
    nrf_tdm_mck_div_t        mck_setup;       /**< Master clock divider setup. */
    nrf_tdm_sck_div_t        sck_setup;       /**< Serial clock divider setup. */
    nrf_tdm_polarity_t       sck_polarity;    /**< Serial clock polarity. */
    nrf_tdm_polarity_t       fsync_polarity;  /**< Frame Synchronization pulse polarity. */
    nrf_tdm_fsync_duration_t fsync_duration;  /**< Frame Synchronization pulse duration. */
} nrf_tdm_config_t;

/** @brief TDM pins. */
typedef struct
{
    uint32_t sck_pin;   ///< SCK pin number.
    uint32_t fsync_pin; ///< FSYNC pin number.
    uint32_t mck_pin;   ///< MCK pin number.
                        /**< Optional. Use @ref NRF_TDM_PIN_NOT_CONNECTED
                         *   if this signal is not needed. */
    uint32_t sdout_pin; ///< SDOUT pin number.
                        /**< Optional. Use @ref NRF_TDM_PIN_NOT_CONNECTED
                         *   if this signal is not needed. */
    uint32_t sdin_pin;  ///< SDIN pin number.
                        /**< Optional. Use @ref NRF_TDM_PIN_NOT_CONNECTED
                         *   if this signal is not needed. */
} nrf_tdm_pins_t;

/**
 * @brief Function for activating the specified TDM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_tdm_task_trigger(NRF_TDM_Type * p_reg,
                                            nrf_tdm_task_t task);

/**
 * @brief Function for getting the address of the specified TDM task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_task_address_get(NRF_TDM_Type const * p_reg,
                                                    nrf_tdm_task_t       task);

/**
 * @brief Function for clearing the specified TDM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_tdm_event_clear(NRF_TDM_Type *  p_reg,
                                           nrf_tdm_event_t event);

/**
 * @brief Function for retrieving the state of the TDM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_tdm_event_check(NRF_TDM_Type const * p_reg,
                                           nrf_tdm_event_t      event);

/**
 * @brief Function for getting the address of the specified TDM event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_event_address_get(NRF_TDM_Type const * p_reg,
                                                     nrf_tdm_event_t      event);

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_tdm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_tdm_int_enable(NRF_TDM_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_tdm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_tdm_int_disable(NRF_TDM_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_tdm_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_int_enable_check(NRF_TDM_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for enabling the TDM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_tdm_enable(NRF_TDM_Type * p_reg);

/**
 * @brief Function for disabling the TDM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_tdm_disable(NRF_TDM_Type * p_reg);

/**
 * @brief Function for checking if the TDM peripheral is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The TDM is enabled.
 * @retval false The TDM is not enabled.
 */
NRF_STATIC_INLINE bool nrf_tdm_enable_check(NRF_TDM_Type * p_reg);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        TDM task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_tdm_subscribe_set(NRF_TDM_Type * p_reg,
                                             nrf_tdm_task_t task,
                                             uint8_t        channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        TDM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_tdm_subscribe_clear(NRF_TDM_Type * p_reg,
                                               nrf_tdm_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        TDM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return TDM subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_subscribe_get(NRF_TDM_Type const * p_reg,
                                                 nrf_tdm_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        TDM event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_tdm_publish_set(NRF_TDM_Type *  p_reg,
                                           nrf_tdm_event_t event,
                                           uint8_t         channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        TDM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_tdm_publish_clear(NRF_TDM_Type *  p_reg,
                                             nrf_tdm_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        TDM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return TDM publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_publish_get(NRF_TDM_Type const * p_reg,
                                               nrf_tdm_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for configuring TDM pins.
 *
 * Usage of the SDOUT, SDIN, and MCK signals is optional.
 * If a given signal is not needed, pass the @ref NRF_TDM_PIN_NOT_CONNECTED
 * value instead of its pin number.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_pins Pointer to the structure with pins selection.
 */
NRF_STATIC_INLINE void nrf_tdm_pins_set(NRF_TDM_Type * p_reg, nrf_tdm_pins_t const * p_pins);

/**
 * @brief Function for getting the SCK pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SCK pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_sck_pin_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for getting the FSYNC pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return FSYNC pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_fsync_pin_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for getting the MCK pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return MCK pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_mck_pin_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for getting the SDOUT pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SDOUT pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_sdout_pin_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for getting the SDIN pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SDIN pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_sdin_pin_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for setting the TDM peripheral configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure with configuration.
 */
NRF_STATIC_INLINE void nrf_tdm_configure(NRF_TDM_Type * p_reg, nrf_tdm_config_t const * p_config);

/**
 * @brief Function for setting the master clock generator.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the master clock generator is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_tdm_mck_set(NRF_TDM_Type * p_reg, bool enable);

/**
 * @brief Function for setting up the TDM RX transfer length.
 *
 * @note This function sets up the RX buffer size. At least one of
 *       @ref nrf_tdm_rx_count_set or @ref nrf_tdm_tx_count_set functions must be called
 *       before starting the transmission. Also @ref nrf_tdm_transfer_direction_set and
 *       @ref nrf_tdm_rx_buffer_set should be called before starting the transmission.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] size     Size of the buffer (in 32-bit words).
 */
NRF_STATIC_INLINE void nrf_tdm_rx_count_set(NRF_TDM_Type * p_reg,
                                            uint16_t       size);

/**
 * @brief Function for setting up the TDM TX transfer length.
 *
 * @note This function sets up the RX buffer size. At least one of
 *       @ref nrf_tdm_rx_count_set or @ref nrf_tdm_tx_count_set functions must be called
 *       before starting the transmission. Also @ref nrf_tdm_transfer_direction_set and
 *       @ref nrf_tdm_tx_buffer_set should be called before starting the transmission.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] size     Size of the buffer (in 32-bit words).
 */
NRF_STATIC_INLINE void nrf_tdm_tx_count_set(NRF_TDM_Type * p_reg,
                                            uint16_t       size);

/**
 * @brief Function for setting up the direction of the TDM transfer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] dir   Transmission direction.
 */
NRF_STATIC_INLINE void nrf_tdm_transfer_direction_set(NRF_TDM_Type *   p_reg,
                                                      nrf_tdm_rxtxen_t dir);

/**
 * @brief Function for setting the pointer to the receive buffer.
 *
 * @note The size of the buffer can be set only by calling
 *       @ref nrf_tdm_rx_count_set.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the receive buffer.
 */
NRF_STATIC_INLINE void nrf_tdm_rx_buffer_set(NRF_TDM_Type * p_reg,
                                             uint32_t *     p_buffer);

/**
 * @brief Function for getting the pointer to the receive buffer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the receive buffer.
 */
NRF_STATIC_INLINE uint32_t * nrf_tdm_rx_buffer_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for setting the pointer to the transmit buffer.
 *
 * @note The size of the buffer can be set only by calling
 *       @ref nrf_tdm_tx_count_set.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the transmit buffer.
 */
NRF_STATIC_INLINE void nrf_tdm_tx_buffer_set(NRF_TDM_Type *   p_reg,
                                             uint32_t const * p_buffer);

/**
 * @brief Function for getting the number of bytes transferred in the current transaction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes transferred.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_current_tx_transfer_amount_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for getting the number of bytes transferred in the last transaction.
 *        The value has been updated after the END event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes transferred.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_last_tx_transfer_amount_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for getting the number of bytes received in the current transaction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes received.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_current_rx_transfer_amount_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for getting the number of bytes received in the last transaction.
 *        The value has been updated after the END event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes received.
 */
NRF_STATIC_INLINE uint32_t nrf_tdm_last_rx_transfer_amount_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for getting the pointer to the transmit buffer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the transmit buffer.
 */
NRF_STATIC_INLINE uint32_t * nrf_tdm_tx_buffer_get(NRF_TDM_Type const * p_reg);

/**
 * @brief Function for configuring TDM master Clock.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] clksrc        TDM master clock source selection.
 * @param[in] enable_bypass Bypass clock generator. MCK will be equal to source input.
 *                          If bypass is enabled the MCK.DIV setting has no effect.
 */
NRF_STATIC_INLINE void nrf_tdm_mck_configure(NRF_TDM_Type * p_reg,
                                             nrf_tdm_src_t  clksrc,
                                             bool           enable_bypass);

/**
 * @brief Function for configuring TDM serial Clock.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] clksrc        TDM serial clock source selection.
 * @param[in] enable_bypass Bypass clock generator. SCK will be equal to source input.
 *                          If bypass is enabled the SCK.DIV setting has no effect.
 */
NRF_STATIC_INLINE void nrf_tdm_sck_configure(NRF_TDM_Type * p_reg,
                                             nrf_tdm_src_t  clksrc,
                                             bool           enable_bypass);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_tdm_task_trigger(NRF_TDM_Type * p_reg,
                                            nrf_tdm_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_task_address_get(NRF_TDM_Type const * p_reg,
                                                    nrf_tdm_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_tdm_event_clear(NRF_TDM_Type *  p_reg,
                                           nrf_tdm_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_tdm_event_check(NRF_TDM_Type const * p_reg,
                                           nrf_tdm_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_tdm_event_address_get(NRF_TDM_Type const * p_reg,
                                                     nrf_tdm_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_tdm_int_enable(NRF_TDM_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_tdm_int_disable(NRF_TDM_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_int_enable_check(NRF_TDM_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE void nrf_tdm_enable(NRF_TDM_Type * p_reg)
{
    p_reg->ENABLE = (TDM_ENABLE_ENABLE_Enabled << TDM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_tdm_disable(NRF_TDM_Type * p_reg)
{
    p_reg->ENABLE = (TDM_ENABLE_ENABLE_Disabled << TDM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE bool nrf_tdm_enable_check(NRF_TDM_Type * p_reg)
{
    return p_reg->ENABLE == (TDM_ENABLE_ENABLE_Enabled << TDM_ENABLE_ENABLE_Pos);
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_tdm_subscribe_set(NRF_TDM_Type * p_reg,
                                             nrf_tdm_task_t task,
                                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_tdm_subscribe_clear(NRF_TDM_Type * p_reg,
                                               nrf_tdm_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}


NRF_STATIC_INLINE uint32_t nrf_tdm_subscribe_get(NRF_TDM_Type const * p_reg,
                                                 nrf_tdm_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}


NRF_STATIC_INLINE void nrf_tdm_publish_set(NRF_TDM_Type *  p_reg,
                                           nrf_tdm_event_t event,
                                           uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_tdm_publish_clear(NRF_TDM_Type *  p_reg,
                                             nrf_tdm_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_publish_get(NRF_TDM_Type const * p_reg,
                                               nrf_tdm_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

NRF_STATIC_INLINE void nrf_tdm_pins_set(NRF_TDM_Type * p_reg, nrf_tdm_pins_t const * p_pins)
{
    p_reg->PSEL.SCK   = p_pins->sck_pin;
    p_reg->PSEL.FSYNC  = p_pins->fsync_pin;
    p_reg->PSEL.MCK   = p_pins->mck_pin;
    p_reg->PSEL.SDOUT = p_pins->sdout_pin;
    p_reg->PSEL.SDIN  = p_pins->sdin_pin;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_sck_pin_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->PSEL.SCK;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_fsync_pin_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->PSEL.FSYNC;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_mck_pin_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->PSEL.MCK;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_sdout_pin_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->PSEL.SDOUT;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_sdin_pin_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->PSEL.SDIN;
}

NRF_STATIC_INLINE void nrf_tdm_configure(NRF_TDM_Type * p_reg, nrf_tdm_config_t const * p_config)
{
    p_reg->CONFIG.MODE           = p_config->mode;
    p_reg->CONFIG.ALIGN          = p_config->alignment;
    p_reg->CONFIG.SWIDTH         = p_config->sample_width;
    p_reg->CONFIG.CHANNEL.MASK   = p_config->channels;
    p_reg->CONFIG.CHANNEL.NUM    = p_config->num_of_channels;
    p_reg->CONFIG.CHANNEL.DELAY  = p_config->channel_delay;
    p_reg->CONFIG.SCK.DIV        = p_config->sck_setup;
    p_reg->CONFIG.FSYNC.DURATION = p_config->fsync_duration;
    p_reg->CONFIG.MCK.DIV        = p_config->mck_setup;
    p_reg->CONFIG.SCK.POLARITY   = (p_config->sck_polarity == NRF_TDM_POLARITY_POSEDGE) ?
                                   (TDM_CONFIG_SCK_POLARITY_SCKPOLARITY_PosEdge) :
                                   (TDM_CONFIG_SCK_POLARITY_SCKPOLARITY_NegEdge);
    p_reg->CONFIG.FSYNC.POLARITY = (p_config->fsync_polarity == NRF_TDM_POLARITY_POSEDGE) ?
                                   (TDM_CONFIG_FSYNC_POLARITY_POLARITY_PosEdge) :
                                   (TDM_CONFIG_FSYNC_POLARITY_POLARITY_NegEdge);
}

NRF_STATIC_INLINE void nrf_tdm_mck_set(NRF_TDM_Type * p_reg, bool enable)
{
    p_reg->CONFIG.MCK.EN = ((p_reg->CONFIG.MCK.EN & ~TDM_CONFIG_MCK_EN_MCKEN_Msk) |
                            ((enable ? TDM_CONFIG_MCK_EN_MCKEN_Enabled :
                             TDM_CONFIG_MCK_EN_MCKEN_Disabled) << TDM_CONFIG_MCK_EN_MCKEN_Pos));
}

NRF_STATIC_INLINE void nrf_tdm_rx_count_set(NRF_TDM_Type * p_reg,
                                            uint16_t       size)
{
#if defined(DMA_BUFFER_UNIFIED_BYTE_ACCESS)
    p_reg->RXD.MAXCNT = size * sizeof(uint32_t);
#else
    p_reg->RXD.MAXCNT = (uint32_t)size;
#endif
}

NRF_STATIC_INLINE void nrf_tdm_tx_count_set(NRF_TDM_Type * p_reg,
                                            uint16_t       size)
{
#if defined(DMA_BUFFER_UNIFIED_BYTE_ACCESS)
    p_reg->TXD.MAXCNT = size * sizeof(uint32_t);
#else
    p_reg->TXD.MAXCNT = (uint32_t)size;
#endif
}

NRF_STATIC_INLINE void nrf_tdm_transfer_direction_set(NRF_TDM_Type *   p_reg,
                                                      nrf_tdm_rxtxen_t dir)
{
    p_reg->CONFIG.RXTXEN = (dir << TDM_CONFIG_RXTXEN_RXTXEN_Pos);
}

NRF_STATIC_INLINE void nrf_tdm_rx_buffer_set(NRF_TDM_Type * p_reg,
                                             uint32_t *     p_buffer)
{
    p_reg->RXD.PTR = (uint32_t)p_buffer;
}

NRF_STATIC_INLINE uint32_t * nrf_tdm_rx_buffer_get(NRF_TDM_Type const * p_reg)
{
    return (uint32_t *)(p_reg->RXD.PTR);
}

NRF_STATIC_INLINE void nrf_tdm_tx_buffer_set(NRF_TDM_Type *   p_reg,
                                             uint32_t const * p_buffer)
{
    p_reg->TXD.PTR = (uint32_t)p_buffer;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_current_tx_transfer_amount_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->TXD.CURRENTAMOUNT;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_last_tx_transfer_amount_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->TXD.AMOUNT;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_current_rx_transfer_amount_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->RXD.CURRENTAMOUNT;
}

NRF_STATIC_INLINE uint32_t nrf_tdm_last_rx_transfer_amount_get(NRF_TDM_Type const * p_reg)
{
    return p_reg->RXD.AMOUNT;
}

NRF_STATIC_INLINE uint32_t * nrf_tdm_tx_buffer_get(NRF_TDM_Type const * p_reg)
{
    return (uint32_t *)(p_reg->TXD.PTR);
}

NRF_STATIC_INLINE void nrf_tdm_mck_configure(NRF_TDM_Type * p_reg,
                                             nrf_tdm_src_t  clksrc,
                                             bool           enable_bypass)
{
    p_reg->CONFIG.MCK.SRC = (((uint32_t) clksrc << TDM_CONFIG_MCK_SRC_CLKSRC_Pos) &
                             TDM_CONFIG_MCK_SRC_CLKSRC_Msk) |
                            (((uint32_t) enable_bypass << TDM_CONFIG_MCK_SRC_BYPASS_Pos) &
                             TDM_CONFIG_MCK_SRC_BYPASS_Msk);
}

NRF_STATIC_INLINE void nrf_tdm_sck_configure(NRF_TDM_Type * p_reg,
                                             nrf_tdm_src_t  clksrc,
                                             bool           enable_bypass)
{
    p_reg->CONFIG.SCK.SRC = ((uint32_t) clksrc << TDM_CONFIG_SCK_SRC_CLKSRC_Pos) |
                            ((uint32_t) enable_bypass << TDM_CONFIG_SCK_SRC_BYPASS_Pos);
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_TDM_H__
