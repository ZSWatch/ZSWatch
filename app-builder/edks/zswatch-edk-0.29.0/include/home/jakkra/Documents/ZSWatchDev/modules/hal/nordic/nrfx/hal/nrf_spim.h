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

#ifndef NRF_SPIM_H__
#define NRF_SPIM_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SPIM_CLOCKPIN_MOSI_NEEDED)
#define NRF_SPIM_CLOCKPIN_MOSI_NEEDED 1
#endif
#if defined(SPIM_CLOCKPIN_SCK_NEEDED)
#define NRF_SPIM_CLOCKPIN_SCK_NEEDED 1
#endif

/*
 * Macro for generating code blocks that allow extracting
 * the maximum prescaler value allowed for the specified SPIM instance.
 */
#define _NRF_SPIM_PRESCALER_MAX_GET(periph_name, prefix, idx, p_reg)           \
    (p_reg == NRFX_CONCAT(NRF_, periph_name, prefix, idx)) ?                   \
        (NRFX_CONCAT(periph_name, prefix, idx, _PRESCALER_DIVISOR_RANGE_MAX)) :

/*
 * Macro for generating code blocks that allow extracting
 * the minimum prescaler value allowed for the specified SPIM instance.
 */
#define _NRF_SPIM_PRESCALER_MIN_GET(periph_name, prefix, idx, p_reg)           \
    (p_reg == NRFX_CONCAT(NRF_, periph_name, prefix, idx)) ?                   \
        (NRFX_CONCAT(periph_name, prefix, idx, _PRESCALER_DIVISOR_RANGE_MIN)) :

/**
 * @defgroup nrf_spim_hal SPIM HAL
 * @{
 * @ingroup nrf_spim
 * @brief   Hardware access layer for managing the SPIM peripheral.
 */

/**
 * @brief Macro getting pointer to the structure of registers of the SPIM peripheral.
 *
 * @param[in] idx SPIM instance index.
 *
 * @return Pointer to the structure of registers of the SPIM peripheral.
 */
#define NRF_SPIM_INST_GET(idx) NRFX_CONCAT(NRF_, SPIM, idx)

#if defined(SPIM_FREQUENCY_FREQUENCY_M16) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether 16 MHz clock frequency is available. */
#define NRF_SPIM_HAS_16_MHZ_FREQ 1
#else
#define NRF_SPIM_HAS_16_MHZ_FREQ 0
#endif

#if defined(SPIM_FREQUENCY_FREQUENCY_M32) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether 32 MHz clock frequency is available. */
#define NRF_SPIM_HAS_32_MHZ_FREQ 1
#else
#define NRF_SPIM_HAS_32_MHZ_FREQ 0
#endif

#if defined(SPIM_FREQUENCY_FREQUENCY_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether frequency is used. */
#define NRF_SPIM_HAS_FREQUENCY 1
#else
#define NRF_SPIM_HAS_FREQUENCY 0
#endif

#if defined(SPIM_PRESCALER_DIVISOR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether prescaler is used. */
#define NRF_SPIM_HAS_PRESCALER 1
#else
#define NRF_SPIM_HAS_PRESCALER 0
#endif

#if defined(SPIM_TXD_LIST_LIST_ArrayList) || defined(SPIM_DMA_TX_LIST_TYPE_ArrayList) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether EasyDMA array list feature is present. */
#define NRF_SPIM_HAS_ARRAY_LIST 1
#else
#define NRF_SPIM_HAS_ARRAY_LIST 0
#endif

#if defined(SPIM_DMA_RX_PTR_PTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether dedicated DMA register is present. */
#define NRF_SPIM_HAS_DMA_REG 1
#else
#define NRF_SPIM_HAS_DMA_REG 0
#endif

#if (defined(SPIM_TASKS_DMA_RX_ENABLEMATCH_ENABLEMATCH_Msk) && \
     defined(SPIM_EVENTS_DMA_RX_END_END_Msk)) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SPIM DMA tasks and events are present. */
#define NRF_SPIM_HAS_DMA_TASKS_EVENTS 1
#else
#define NRF_SPIM_HAS_DMA_TASKS_EVENTS 0
#endif

/** @brief Macro for checking if the hardware chip select function is available. */
#if NRFX_FEATURE_PRESENT(SPIM, _FEATURE_HARDWARE_CSN_PRESENT) || defined(__NRFX_DOXYGEN__)
#define NRF_SPIM_HAS_HW_CSN 1
#else
#define NRF_SPIM_HAS_HW_CSN 0
#endif

/** @brief Macro for checking if the DCX pin control is available. */
#if NRFX_FEATURE_PRESENT(SPIM, _FEATURE_HARDWARE_DCX_PRESENT) || \
    NRFX_FEATURE_PRESENT(SPIM, _FEATURE_DCX_PRESENT) || \
    defined(__NRFX_DOXYGEN__)
#define NRF_SPIM_HAS_DCX 1
#else
#define NRF_SPIM_HAS_DCX 0
#endif

/** @brief Macro for checking if the RXDELAY function is available. */
#if NRFX_FEATURE_PRESENT(SPIM, _FEATURE_RXDELAY_PRESENT) || defined(__NRFX_DOXYGEN__)
#define NRF_SPIM_HAS_RXDELAY 1
#else
#define NRF_SPIM_HAS_RXDELAY 0
#endif

#if defined(SPIM_STALLSTAT_RX_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Macro for checking if the STALLSTAT feature is available. */
#define NRF_SPIM_HAS_STALLSTAT 1
#else
#define NRF_SPIM_HAS_STALLSTAT 0
#endif

#if NRF_SPIM_HAS_HW_CSN || NRF_SPIM_HAS_DCX || NRF_SPIM_HAS_RXDELAY
/** @brief Symbol indicating whether any of the SPIM extended features is available. */
#define NRF_SPIM_HAS_EXTENDED 1
#else
#define NRF_SPIM_HAS_EXTENDED 0
#endif

#if NRF_SPIM_HAS_DCX
/**
 * @brief This value specified in the DCX line configuration causes this line
 *        to be set low during whole transmission (all transmitted bytes are
 *        marked as command bytes). Any lower value causes the DCX line to be
 *        switched from low to high after this number of bytes is transmitted
 *        (all remaining bytes are marked as data bytes).
 */
#define NRF_SPIM_DCX_CNT_ALL_CMD 0xF
#endif

/**
 * @brief This value can be used as a parameter for the @ref nrf_spim_pins_set
 *        function to specify that a given SPI signal (SCK, MOSI, or MISO)
 *        shall not be connected to a physical pin.
 */
#define NRF_SPIM_PIN_NOT_CONNECTED UINT32_MAX 

#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
/** @brief Max number of RX patterns. */
#define NRF_SPIM_DMA_RX_PATTERN_MAX_COUNT SPIM_DMA_RX_MATCH_CANDIDATE_MaxCount
#endif

#if NRF_SPIM_HAS_DCX
#if defined(SPIM_PSEL_DCX_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying default value of DCX pin setting. */
#define NRF_SPIM_DCX_DEFAULT SPIM_PSEL_DCX_ResetValue
#endif
#endif // NRF_SPIM_HAS_DCX

#if NRF_SPIM_HAS_HW_CSN
#if defined(SPIM_PSEL_CSN_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying default value of CSN pin setting. */
#define NRF_SPIM_CSN_DEFAULT SPIM_PSEL_CSN_ResetValue
#endif

#if defined(SPIM_IFTIMING_CSNDUR_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying default value of CSN duration setting. */
#define NRF_SPIM_CSNDUR_DEFAULT SPIM_IFTIMING_CSNDUR_ResetValue
#endif

#if defined(SPIM_CSNPOL_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying default value of CSN polarity setting. */
#define NRF_SPIM_CSNPOL_DEFAULT SPIM_CSNPOL_ResetValue
#endif
#endif // NRF_SPIM_HAS_HW_CSN

#if NRF_SPIM_HAS_RXDELAY
#if defined(SPIM_IFTIMING_RXDELAY_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol specifying default value of RX delay setting. */
#define NRF_SPIM_RXDELAY_DEFAULT SPIM_IFTIMING_RXDELAY_ResetValue
#endif
#endif // NRF_SPIM_HAS_RXDELAY

/** @brief SPIM should force H0H1 drive on pins. */
#define NRF_SPIM_FORCE_H0H1 SPIM_FORCE_H0H1

/** @brief SPIM should check disable on end of transfer. */
#define NRF_SPIM_CHECK_DISABLE_ON_XFER_END SPIM_CHECK_DISABLE_ON_XFER_END

/** @brief Minimal SPIM frequency in Hz. */
#define NRF_SPIM_MIN_FREQUENCY (NRFX_KHZ_TO_HZ(125UL))

/** @brief Base frequency value 320 MHz for SPIM. */
#define NRF_SPIM_BASE_FREQUENCY_320MHZ (NRFX_MHZ_TO_HZ(320UL))

/** @brief Base frequency value 192 MHz for SPIM. */
#define NRF_SPIM_BASE_FREQUENCY_192MHZ (NRFX_MHZ_TO_HZ(192UL))

/** @brief Base frequency value 128 MHz for SPIM. */
#define NRF_SPIM_BASE_FREQUENCY_128MHZ (NRFX_MHZ_TO_HZ(128UL))

/** @brief Base frequency value 64 MHz for SPIM. */
#define NRF_SPIM_BASE_FREQUENCY_64MHZ  (NRFX_MHZ_TO_HZ(64UL))

/** @brief Base frequency value 32 MHz for SPIM. */
#define NRF_SPIM_BASE_FREQUENCY_32MHZ  (NRFX_MHZ_TO_HZ(32UL))

/** @brief Base frequency value 16 MHz for SPIM. */
#define NRF_SPIM_BASE_FREQUENCY_16MHZ  (NRFX_MHZ_TO_HZ(16UL))

#if !defined(NRF_SPIM_IS_320MHZ_SPIM)
/** @brief Macro for checking whether the base frequency for the specified SPIM instance is 320 MHz. */
#define NRF_SPIM_IS_320MHZ_SPIM(p_reg) \
    (NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(SPIM120), (p_reg == NRF_SPIM120), (false)) || \
     NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(SPIM121), (p_reg == NRF_SPIM121), (false)))
#endif

#if !defined(NRF_SPIM_IS_192MHZ_SPIM)
/** @brief Macro for checking whether the base frequency for the specified SPIM instance is 192 MHz. */
#define NRF_SPIM_IS_192MHZ_SPIM(p_reg) false
#endif

#if !defined(NRF_SPIM_IS_128MHZ_SPIM)
/** @brief Macro for checking whether the base frequency for the specified SPIM instance is 128 MHz. */
#define NRF_SPIM_IS_128MHZ_SPIM(p_reg)                                                     \
    (NRFX_COND_CODE_1(NRFX_IS_ENABLED(NRF_CPU_FREQ_IS_128MHZ),                             \
        (NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(SPIM00), (p_reg == NRF_SPIM00), (false))), \
        (false)))
#endif

#if !defined(NRF_SPIM_IS_64MHZ_SPIM)
/** @brief Macro for checking whether the base frequency for the specified SPIM instance is 64 MHz. */
#define NRF_SPIM_IS_64MHZ_SPIM(p_reg)                                                      \
    (NRFX_COND_CODE_1(NRFX_IS_ENABLED(NRF_CPU_FREQ_IS_64MHZ),                              \
        (NRFX_COND_CODE_1(NRFX_INSTANCE_PRESENT(SPIM00), (p_reg == NRF_SPIM00), (false))), \
        (false)))
#endif

#if !defined(NRF_SPIM_IS_32MHZ_SPIM)
/** @brief Macro for checking whether the base frequency for the specified SPIM instance is 32 MHz. */
#define NRF_SPIM_IS_32MHZ_SPIM(p_reg) false
#endif

/** @brief Macro for getting base frequency value in Hz for the specified SPIM instance. */
#define NRF_SPIM_BASE_FREQUENCY_GET(p_reg)                                 \
    ((NRF_SPIM_IS_320MHZ_SPIM(p_reg)) ? (NRF_SPIM_BASE_FREQUENCY_320MHZ) : \
    ((NRF_SPIM_IS_192MHZ_SPIM(p_reg)) ? (NRF_SPIM_BASE_FREQUENCY_192MHZ) : \
    ((NRF_SPIM_IS_128MHZ_SPIM(p_reg)) ? (NRF_SPIM_BASE_FREQUENCY_128MHZ) : \
    ((NRF_SPIM_IS_64MHZ_SPIM(p_reg))  ? (NRF_SPIM_BASE_FREQUENCY_64MHZ) :  \
    ((NRF_SPIM_IS_32MHZ_SPIM(p_reg))  ? (NRF_SPIM_BASE_FREQUENCY_32MHZ) :  \
    (NRF_SPIM_BASE_FREQUENCY_16MHZ))))))

#if NRF_SPIM_HAS_PRESCALER
/**
 * @brief Macro for getting the maximum value of PRESCALER register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
#define NRF_SPIM_PRESCALER_MAX_GET(p_reg)                                   \
    (NRFX_FOREACH_ENABLED(SPIM, _NRF_SPIM_PRESCALER_MAX_GET, (), (), p_reg) \
     SPIM_PRESCALER_DIVISOR_Max)

/**
 * @brief Macro for getting the minimum value of PRESCALER register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
#define NRF_SPIM_PRESCALER_MIN_GET(p_reg)                                   \
    (NRFX_FOREACH_ENABLED(SPIM, _NRF_SPIM_PRESCALER_MIN_GET, (), (), p_reg) \
     SPIM_PRESCALER_DIVISOR_Min)

/**
 * @brief Macro for computing prescaler value for a given SPIM instance and desired frequency.
 *
 * @warning Not every combination of base and desired frequency is supported.
 *          The @ref NRF_SPIM_FREQUENCY_STATIC_CHECK macro can be used to check if the desired frequency is supported.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] frequency Desired frequency value in Hz.
 */
#define NRF_SPIM_PRESCALER_CALCULATE(p_reg, frequency) \
        ((uint32_t)(NRF_SPIM_BASE_FREQUENCY_GET(p_reg)) / (uint32_t)(frequency))
#endif

/**
 * @brief Macro for checking whether specified frequency can be achieved for a given SPIM instance.
 *
 * @note This macro uses a compile-time assertion.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] frequency Desired frequency value in Hz.
 */
#define NRF_SPIM_FREQUENCY_STATIC_CHECK(p_reg, frequency)                                                    \
    NRFX_STATIC_ASSERT(                                                                                      \
    NRFX_COND_CODE_1(NRF_SPIM_HAS_PRESCALER,                                                                 \
        ((NRF_SPIM_BASE_FREQUENCY_GET(p_reg) % (uint32_t)frequency == 0) &&                                  \
        (NRFX_IS_EVEN(NRF_SPIM_PRESCALER_CALCULATE(p_reg, (uint32_t)frequency))) &&                          \
        (NRF_SPIM_PRESCALER_CALCULATE(p_reg, (uint32_t)frequency) >= (NRF_SPIM_PRESCALER_MIN_GET(p_reg))) && \
        (NRF_SPIM_PRESCALER_CALCULATE(p_reg, (uint32_t)frequency) <= (NRF_SPIM_PRESCALER_MAX_GET(p_reg))))   \
        ,                                                                                                    \
        (((uint32_t)frequency == (uint32_t)NRFX_KHZ_TO_HZ(125)) ||                                           \
         ((uint32_t)frequency == (uint32_t)NRFX_KHZ_TO_HZ(250)) ||                                           \
         ((uint32_t)frequency == (uint32_t)NRFX_KHZ_TO_HZ(500)) ||                                           \
         ((uint32_t)frequency == (uint32_t)NRFX_MHZ_TO_HZ(1))   ||                                           \
         ((uint32_t)frequency == (uint32_t)NRFX_MHZ_TO_HZ(2))   ||                                           \
         ((uint32_t)frequency == (uint32_t)NRFX_MHZ_TO_HZ(4))   ||                                           \
         ((uint32_t)frequency == (uint32_t)NRFX_MHZ_TO_HZ(8))   ||                                           \
         (((uint32_t)frequency == (uint32_t)NRFX_MHZ_TO_HZ(16)) && (NRF_SPIM_HAS_16_MHZ_FREQ)) ||            \
         (((uint32_t)frequency == (uint32_t)NRFX_MHZ_TO_HZ(32)) && (NRF_SPIM_HAS_32_MHZ_FREQ)))))

/** @brief SPIM tasks. */
typedef enum
{
    NRF_SPIM_TASK_START           = offsetof(NRF_SPIM_Type, TASKS_START),                  ///< Start SPI transaction.
    NRF_SPIM_TASK_STOP            = offsetof(NRF_SPIM_Type, TASKS_STOP),                   ///< Stop SPI transaction.
    NRF_SPIM_TASK_SUSPEND         = offsetof(NRF_SPIM_Type, TASKS_SUSPEND),                ///< Suspend SPI transaction.
    NRF_SPIM_TASK_RESUME          = offsetof(NRF_SPIM_Type, TASKS_RESUME),                 ///< Resume SPI transaction.
#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
    NRF_SPIM_TASK_ENABLERXMATCH0  = offsetof(NRF_SPIM_Type, TASKS_DMA.RX.ENABLEMATCH[0]),  ///< Enable SPI pattern matching functionality for pattern 0.
    NRF_SPIM_TASK_ENABLERXMATCH1  = offsetof(NRF_SPIM_Type, TASKS_DMA.RX.ENABLEMATCH[1]),  ///< Enable SPI pattern matching functionality for pattern 1.
    NRF_SPIM_TASK_ENABLERXMATCH2  = offsetof(NRF_SPIM_Type, TASKS_DMA.RX.ENABLEMATCH[2]),  ///< Enable SPI pattern matching functionality for pattern 2.
    NRF_SPIM_TASK_ENABLERXMATCH3  = offsetof(NRF_SPIM_Type, TASKS_DMA.RX.ENABLEMATCH[3]),  ///< Enable SPI pattern matching functionality for pattern 3.
    NRF_SPIM_TASK_DISABLERXMATCH0 = offsetof(NRF_SPIM_Type, TASKS_DMA.RX.DISABLEMATCH[0]), ///< Disable SPI pattern matching functionality for pattern 0.
    NRF_SPIM_TASK_DISABLERXMATCH1 = offsetof(NRF_SPIM_Type, TASKS_DMA.RX.DISABLEMATCH[1]), ///< Disable SPI pattern matching functionality for pattern 1.
    NRF_SPIM_TASK_DISABLERXMATCH2 = offsetof(NRF_SPIM_Type, TASKS_DMA.RX.DISABLEMATCH[2]), ///< Disable SPI pattern matching functionality for pattern 2.
    NRF_SPIM_TASK_DISABLERXMATCH3 = offsetof(NRF_SPIM_Type, TASKS_DMA.RX.DISABLEMATCH[3])  ///< Disable SPI pattern matching functionality for pattern 3.
#endif
} nrf_spim_task_t;

/** @brief SPIM events. */
typedef enum
{
    NRF_SPIM_EVENT_STARTED    = offsetof(NRF_SPIM_Type, EVENTS_STARTED),         ///< SPI transaction has started.
    NRF_SPIM_EVENT_STOPPED    = offsetof(NRF_SPIM_Type, EVENTS_STOPPED),         ///< SPI transaction has stopped.
#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
    NRF_SPIM_EVENT_RXSTARTED  = offsetof(NRF_SPIM_Type, EVENTS_DMA.RX.READY),    ///< Receive sequence started.
    NRF_SPIM_EVENT_RXBUSERROR = offsetof(NRF_SPIM_Type, EVENTS_DMA.RX.BUSERROR), ///< Memory bus error occurred during the RX transfer.
    NRF_SPIM_EVENT_RXMATCH0   = offsetof(NRF_SPIM_Type, EVENTS_DMA.RX.MATCH[0]), ///< Pattern match for pattern 0 detected.
    NRF_SPIM_EVENT_RXMATCH1   = offsetof(NRF_SPIM_Type, EVENTS_DMA.RX.MATCH[1]), ///< Pattern match for pattern 1 detected.
    NRF_SPIM_EVENT_RXMATCH2   = offsetof(NRF_SPIM_Type, EVENTS_DMA.RX.MATCH[2]), ///< Pattern match for pattern 2 detected.
    NRF_SPIM_EVENT_RXMATCH3   = offsetof(NRF_SPIM_Type, EVENTS_DMA.RX.MATCH[3]), ///< Pattern match for pattern 3 detected.
    NRF_SPIM_EVENT_TXSTARTED  = offsetof(NRF_SPIM_Type, EVENTS_DMA.TX.READY),    ///< Transmit sequence started.
    NRF_SPIM_EVENT_TXBUSERROR = offsetof(NRF_SPIM_Type, EVENTS_DMA.TX.BUSERROR), ///< Memory bus error occurred during the TX transfer.
    NRF_SPIM_EVENT_ENDRX      = offsetof(NRF_SPIM_Type, EVENTS_DMA.RX.END),      ///< End of RXD buffer reached.
    NRF_SPIM_EVENT_ENDTX      = offsetof(NRF_SPIM_Type, EVENTS_DMA.TX.END),      ///< End of TXD buffer reached.
#else
    NRF_SPIM_EVENT_ENDRX      = offsetof(NRF_SPIM_Type, EVENTS_ENDRX),           ///< End of RXD buffer reached.
    NRF_SPIM_EVENT_ENDTX      = offsetof(NRF_SPIM_Type, EVENTS_ENDTX),           ///< End of TXD buffer reached.
#endif
    NRF_SPIM_EVENT_END        = offsetof(NRF_SPIM_Type, EVENTS_END)              ///< End of RXD buffer and TXD buffer reached.
} nrf_spim_event_t;

/** @brief SPIM shortcuts. */
typedef enum
{
    NRF_SPIM_SHORT_END_START_MASK                = SPIM_SHORTS_END_START_Msk,                          ///< Shortcut between END event and START task.
#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
    NRF_SPIM_SHORT_RXMATCH0_ENABLERXMATCH1_MASK  = SPIM_SHORTS_DMA_RX_MATCH0_DMA_RX_ENABLEMATCH1_Msk,  ///< Shortcut between DMA.RX.MATCH0 event and DMA.RX.ENABLEMATCH1 task.
    NRF_SPIM_SHORT_RXMATCH1_ENABLERXMATCH2_MASK  = SPIM_SHORTS_DMA_RX_MATCH1_DMA_RX_ENABLEMATCH2_Msk,  ///< Shortcut between DMA.RX.MATCH1 event and DMA.RX.ENABLEMATCH2 task.
    NRF_SPIM_SHORT_RXMATCH2_ENABLERXMATCH3_MASK  = SPIM_SHORTS_DMA_RX_MATCH2_DMA_RX_ENABLEMATCH3_Msk,  ///< Shortcut between DMA.RX.MATCH2 event and DMA.RX.ENABLEMATCH3 task.
    NRF_SPIM_SHORT_RXMATCH3_ENABLERXMATCH0_MASK  = SPIM_SHORTS_DMA_RX_MATCH3_DMA_RX_ENABLEMATCH0_Msk,  ///< Shortcut between DMA.RX.MATCH3 event and DMA.RX.ENABLEMATCH4 task.
    NRF_SPIM_SHORT_RXMATCH0_DISABLERXMATCH0_MASK = SPIM_SHORTS_DMA_RX_MATCH0_DMA_RX_DISABLEMATCH0_Msk, ///< Shortcut between DMA.RX.MATCH0 event and DMA.RX.DISABLEMATCH0 task.
    NRF_SPIM_SHORT_RXMATCH1_DISABLERXMATCH1_MASK = SPIM_SHORTS_DMA_RX_MATCH1_DMA_RX_DISABLEMATCH1_Msk, ///< Shortcut between DMA.RX.MATCH1 event and DMA.RX.DISABLEMATCH1 task.
    NRF_SPIM_SHORT_RXMATCH2_DISABLERXMATCH2_MASK = SPIM_SHORTS_DMA_RX_MATCH2_DMA_RX_DISABLEMATCH2_Msk, ///< Shortcut between DMA.RX.MATCH2 event and DMA.RX.DISABLEMATCH2 task.
    NRF_SPIM_SHORT_RXMATCH3_DISABLERXMATCH3_MASK = SPIM_SHORTS_DMA_RX_MATCH3_DMA_RX_DISABLEMATCH3_Msk, ///< Shortcut between DMA.RX.MATCH3 event and DMA.RX.DISABLEMATCH3 task.
#endif
    NRF_SPIM_ALL_SHORTS_MASK      = SPIM_SHORTS_END_START_Msk
#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
                                  | SPIM_SHORTS_DMA_RX_MATCH0_DMA_RX_ENABLEMATCH1_Msk
                                  | SPIM_SHORTS_DMA_RX_MATCH1_DMA_RX_ENABLEMATCH2_Msk
                                  | SPIM_SHORTS_DMA_RX_MATCH2_DMA_RX_ENABLEMATCH3_Msk
                                  | SPIM_SHORTS_DMA_RX_MATCH3_DMA_RX_ENABLEMATCH0_Msk
                                  | SPIM_SHORTS_DMA_RX_MATCH0_DMA_RX_DISABLEMATCH0_Msk
                                  | SPIM_SHORTS_DMA_RX_MATCH1_DMA_RX_DISABLEMATCH1_Msk
                                  | SPIM_SHORTS_DMA_RX_MATCH2_DMA_RX_DISABLEMATCH2_Msk
                                  | SPIM_SHORTS_DMA_RX_MATCH3_DMA_RX_DISABLEMATCH3_Msk                 ///< All SPIM shortcuts.
#endif
} nrf_spim_short_mask_t;

/** @brief SPIM interrupts. */
typedef enum
{
    NRF_SPIM_INT_STARTED_MASK    = SPIM_INTENSET_STARTED_Msk,       ///< Interrupt on STARTED event.
    NRF_SPIM_INT_STOPPED_MASK    = SPIM_INTENSET_STOPPED_Msk,       ///< Interrupt on STOPPED event.
#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
    NRF_SPIM_INT_RXREADY_MASK    = SPIM_INTENSET_DMARXREADY_Msk,    ///< Interrupt on DMA.RX.READY event.
    NRF_SPIM_INT_RXBUSERROR_MASK = SPIM_INTENSET_DMARXBUSERROR_Msk, ///< Interrupt on DMA.RX.BUSERROR event.
    NRF_SPIM_INT_RXMATCH0_MASK   = SPIM_INTENSET_DMARXMATCH0_Msk,   ///< Interrupt on DMA.RX.MATCH0 event.
    NRF_SPIM_INT_RXMATCH1_MASK   = SPIM_INTENSET_DMARXMATCH1_Msk,   ///< Interrupt on DMA.RX.MATCH1 event.
    NRF_SPIM_INT_RXMATCH2_MASK   = SPIM_INTENSET_DMARXMATCH2_Msk,   ///< Interrupt on DMA.RX.MATCH2 event.
    NRF_SPIM_INT_RXMATCH3_MASK   = SPIM_INTENSET_DMARXMATCH3_Msk,   ///< Interrupt on DMA.RX.MATCH3 event.
    NRF_SPIM_INT_TXREADY_MASK    = SPIM_INTENSET_DMATXREADY_Msk,    ///< Interrupt on DMA.TX.READY event.
    NRF_SPIM_INT_TXBUSERROR_MASK = SPIM_INTENSET_DMATXBUSERROR_Msk, ///< Interrupt on DMA.TX.BUSERROR event.
    NRF_SPIM_INT_ENDRX_MASK      = SPIM_INTENSET_DMARXEND_Msk,      ///< Interrupt on ENDRX event.
    NRF_SPIM_INT_ENDTX_MASK      = SPIM_INTENSET_DMATXEND_Msk,      ///< Interrupt on ENDTX event.
#else
    NRF_SPIM_INT_ENDRX_MASK      = SPIM_INTENSET_ENDRX_Msk,         ///< Interrupt on ENDRX event.
    NRF_SPIM_INT_ENDTX_MASK      = SPIM_INTENSET_ENDTX_Msk,         ///< Interrupt on ENDTX event.
#endif
    NRF_SPIM_INT_END_MASK        = SPIM_INTENSET_END_Msk,           ///< Interrupt on END event.
    NRF_SPIM_ALL_INTS_MASK       = NRF_SPIM_INT_STARTED_MASK
                                 | NRF_SPIM_INT_STOPPED_MASK
#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
                                 | NRF_SPIM_INT_RXREADY_MASK
                                 | NRF_SPIM_INT_RXBUSERROR_MASK
                                 | NRF_SPIM_INT_RXMATCH0_MASK
                                 | NRF_SPIM_INT_RXMATCH1_MASK
                                 | NRF_SPIM_INT_RXMATCH2_MASK
                                 | NRF_SPIM_INT_RXMATCH3_MASK
                                 | NRF_SPIM_INT_TXREADY_MASK
                                 | NRF_SPIM_INT_TXBUSERROR_MASK
#endif
                                 | NRF_SPIM_INT_ENDRX_MASK
                                 | NRF_SPIM_INT_ENDTX_MASK
                                 | NRF_SPIM_INT_END_MASK            ///< All SPIM interrupts.
} nrf_spim_int_mask_t;

#if NRF_SPIM_HAS_FREQUENCY
/** @brief SPI master data rates. */
typedef enum
{
    NRF_SPIM_FREQ_125K = SPIM_FREQUENCY_FREQUENCY_K125,    ///< 125 kbps.
    NRF_SPIM_FREQ_250K = SPIM_FREQUENCY_FREQUENCY_K250,    ///< 250 kbps.
    NRF_SPIM_FREQ_500K = SPIM_FREQUENCY_FREQUENCY_K500,    ///< 500 kbps.
    NRF_SPIM_FREQ_1M   = SPIM_FREQUENCY_FREQUENCY_M1,      ///< 1 Mbps.
    NRF_SPIM_FREQ_2M   = SPIM_FREQUENCY_FREQUENCY_M2,      ///< 2 Mbps.
    NRF_SPIM_FREQ_4M   = SPIM_FREQUENCY_FREQUENCY_M4,      ///< 4 Mbps.
    // Conversion to int is needed to prevent compilers from getting an error message
    // that the provided value (0x80000000UL) is out of range for int.
    NRF_SPIM_FREQ_8M   = (int)SPIM_FREQUENCY_FREQUENCY_M8, ///< 8 Mbps.
#if NRF_SPIM_HAS_16_MHZ_FREQ
    NRF_SPIM_FREQ_16M  = SPIM_FREQUENCY_FREQUENCY_M16,     ///< 16 Mbps.
#endif
#if NRF_SPIM_HAS_32_MHZ_FREQ
    NRF_SPIM_FREQ_32M  = SPIM_FREQUENCY_FREQUENCY_M32      ///< 32 Mbps.
#endif
} nrf_spim_frequency_t;
#endif // NRF_SPIM_HAS_FREQUENCY

/** @brief SPI modes. */
typedef enum
{
    NRF_SPIM_MODE_0, ///< SCK active high, sample on leading edge of clock.
    NRF_SPIM_MODE_1, ///< SCK active high, sample on trailing edge of clock.
    NRF_SPIM_MODE_2, ///< SCK active low, sample on leading edge of clock.
    NRF_SPIM_MODE_3  ///< SCK active low, sample on trailing edge of clock.
} nrf_spim_mode_t;

/** @brief SPI bit orders. */
typedef enum
{
    NRF_SPIM_BIT_ORDER_MSB_FIRST = SPIM_CONFIG_ORDER_MsbFirst, ///< Most significant bit shifted out first.
    NRF_SPIM_BIT_ORDER_LSB_FIRST = SPIM_CONFIG_ORDER_LsbFirst  ///< Least significant bit shifted out first.
} nrf_spim_bit_order_t;

#if NRF_SPIM_HAS_HW_CSN
/** @brief SPI CSN pin polarity. */
typedef enum
{
#if defined(SPIM_CSNPOL_CSNPOL0_LOW)
    NRF_SPIM_CSN_POL_LOW  = SPIM_CSNPOL_CSNPOL0_LOW, ///< Active low (idle state high).
    NRF_SPIM_CSN_POL_HIGH = SPIM_CSNPOL_CSNPOL0_HIGH ///< Active high (idle state low).
#else
    NRF_SPIM_CSN_POL_LOW  = SPIM_CSNPOL_CSNPOL_LOW, ///< Active low (idle state high).
    NRF_SPIM_CSN_POL_HIGH = SPIM_CSNPOL_CSNPOL_HIGH ///< Active high (idle state low).
#endif
} nrf_spim_csn_pol_t;
#endif

/**
 * @brief Function for activating the specified SPIM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_spim_task_trigger(NRF_SPIM_Type * p_reg,
                                             nrf_spim_task_t task);

/**
 * @brief Function for getting the address of the specified SPIM task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  The specified task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_task_address_get(NRF_SPIM_Type const * p_reg,
                                                     nrf_spim_task_t       task);

/**
 * @brief Function for clearing the specified SPIM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_spim_event_clear(NRF_SPIM_Type *  p_reg,
                                            nrf_spim_event_t event);

/**
 * @brief Function for retrieving the state of the SPIM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_spim_event_check(NRF_SPIM_Type const * p_reg,
                                            nrf_spim_event_t      event);

/**
 * @brief Function for getting the address of the specified SPIM event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event The specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_event_address_get(NRF_SPIM_Type const * p_reg,
                                                      nrf_spim_event_t      event);

/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_spim_shorts_enable(NRF_SPIM_Type * p_reg,
                                              uint32_t        mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_spim_shorts_disable(NRF_SPIM_Type * p_reg,
                                               uint32_t        mask);

/**
 * @brief Function for getting the shortcut setting.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Current shortcut configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_shorts_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_spim_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_spim_int_enable(NRF_SPIM_Type * p_reg,
                                           uint32_t        mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_spim_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_spim_int_disable(NRF_SPIM_Type * p_reg,
                                            uint32_t        mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_spim_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_int_enable_check(NRF_SPIM_Type const * p_reg, uint32_t mask);

#if NRF_SPIM_HAS_PRESCALER
/**
 * @brief Function for setting the prescaler value.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] prescaler Prescaler value.
 */
NRF_STATIC_INLINE void nrf_spim_prescaler_set(NRF_SPIM_Type * p_reg,
                                              uint32_t        prescaler);

/**
 * @brief Function for getting the prescaler value.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Prescaler value.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_prescaler_get(NRF_SPIM_Type const * p_reg);
#endif

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        SPIM task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_spim_subscribe_set(NRF_SPIM_Type * p_reg,
                                              nrf_spim_task_t task,
                                              uint8_t         channel);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        SPIM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return SPIM subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_subscribe_get(NRF_SPIM_Type const * p_reg,
                                                  nrf_spim_task_t       task);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        SPIM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_spim_subscribe_clear(NRF_SPIM_Type * p_reg,
                                                nrf_spim_task_t task);

/**
 * @brief Function for setting the publish configuration for a given
 *        SPIM event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_spim_publish_set(NRF_SPIM_Type *  p_reg,
                                            nrf_spim_event_t event,
                                            uint8_t          channel);

/**
 * @brief Function for getting the publish configuration for a given
 *        SPIM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return SPIM publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_publish_get(NRF_SPIM_Type const * p_reg,
                                                nrf_spim_event_t      event);

/**
 * @brief Function for clearing the publish configuration for a given
 *        SPIM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_spim_publish_clear(NRF_SPIM_Type *  p_reg,
                                              nrf_spim_event_t event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for enabling the SPIM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_enable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for disabling the SPIM peripheral.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_disable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for checking if the SPIM peripheral is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The SPIM is enabled.
 * @retval false The SPIM is not enabled.
 */
NRF_STATIC_INLINE bool nrf_spim_enable_check(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for configuring SPIM pins.
 *
 * If a given signal is not needed, pass the @ref NRF_SPIM_PIN_NOT_CONNECTED
 * value instead of its pin number.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] sck_pin  SCK pin number.
 * @param[in] mosi_pin MOSI pin number.
 * @param[in] miso_pin MISO pin number.
 */
NRF_STATIC_INLINE void nrf_spim_pins_set(NRF_SPIM_Type * p_reg,
                                         uint32_t        sck_pin,
                                         uint32_t        mosi_pin,
                                         uint32_t        miso_pin);

/**
 * @brief Function for setting the SCK pin.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   SCK pin number.
 */
NRF_STATIC_INLINE void nrf_spim_sck_pin_set(NRF_SPIM_Type * p_reg, uint32_t pin);

/**
 * @brief Function for setting the MOSI pin
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   MOSI pin number.
 */
NRF_STATIC_INLINE void nrf_spim_mosi_pin_set(NRF_SPIM_Type * p_reg, uint32_t pin);

/**
 * @brief Function for setting the MISO pin.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] pin   MISO pin number.
 */
NRF_STATIC_INLINE void nrf_spim_miso_pin_set(NRF_SPIM_Type * p_reg, uint32_t pin);

/**
 * @brief Function for getting the SCK pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return SCK pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_sck_pin_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for getting the MOSI pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return MOSI pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_mosi_pin_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for getting the MISO pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return MISO pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_miso_pin_get(NRF_SPIM_Type const * p_reg);

#if NRF_SPIM_HAS_HW_CSN
/**
 * @brief Function for configuring the SPIM hardware CSN pin.
 *
 * If this signal is not needed, pass the @ref NRF_SPIM_PIN_NOT_CONNECTED
 * value instead of its pin number.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] pin      CSN pin number.
 * @param[in] polarity CSN pin polarity.
 * @param[in] duration Minimum duration between the edge of CSN and the edge of SCK
 *                     and minimum duration of CSN must stay unselected between transactions.
 *                     The value is specified in number of 64 MHz clock cycles (15.625 ns).
 */
NRF_STATIC_INLINE void nrf_spim_csn_configure(NRF_SPIM_Type *    p_reg,
                                              uint32_t           pin,
                                              nrf_spim_csn_pol_t polarity,
                                              uint32_t           duration);

/**
 * @brief Function for getting the CSN pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return CSN pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_csn_pin_get(NRF_SPIM_Type const * p_reg);
#endif

#if NRF_SPIM_HAS_DCX
/**
 * @brief Function for configuring the SPIM DCX pin.
 *
 * If this signal is not needed, pass the @ref NRF_SPIM_PIN_NOT_CONNECTED
 * value instead of its pin number.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] dcx_pin DCX pin number.
 */
NRF_STATIC_INLINE void nrf_spim_dcx_pin_set(NRF_SPIM_Type * p_reg,
                                            uint32_t        dcx_pin);

/**
 * @brief Function for getting the DCX pin selection.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return DCX pin selection.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_dcx_pin_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for configuring the number of command bytes.
 *
 * Maximum value available for dividing the transmitted bytes into command
 * bytes and data bytes is @ref NRF_SPIM_DCX_CNT_ALL_CMD - 1.
 * The @ref NRF_SPIM_DCX_CNT_ALL_CMD value passed as the @c count parameter
 * causes all transmitted bytes to be marked as command bytes.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] count Number of command bytes preceding the data bytes.
 */
NRF_STATIC_INLINE void nrf_spim_dcx_cnt_set(NRF_SPIM_Type * p_reg,
                                            uint32_t        count);
#endif // NRF_SPIM_HAS_DCX

#if NRF_SPIM_HAS_RXDELAY
/**
 * @brief Function for configuring the extended SPIM interface.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] rxdelay Sample delay for input serial data on MISO,
 *                    specified in 64 MHz clock cycles (15.625 ns) from the sampling edge of SCK.
 */
NRF_STATIC_INLINE void nrf_spim_iftiming_set(NRF_SPIM_Type * p_reg,
                                             uint32_t        rxdelay);
#endif

#if NRF_SPIM_HAS_STALLSTAT
/**
 * @brief Function for clearing stall status for RX EasyDMA RAM accesses.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_stallstat_rx_clear(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for getting stall status for RX EasyDMA RAM accesses.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Stall status of RX EasyDMA RAM accesses.
 */
NRF_STATIC_INLINE bool nrf_spim_stallstat_rx_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for clearing stall status for TX EasyDMA RAM accesses.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_stallstat_tx_clear(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for getting stall status for TX EasyDMA RAM accesses.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Stall status of TX EasyDMA RAM accesses.
 */
NRF_STATIC_INLINE bool nrf_spim_stallstat_tx_get(NRF_SPIM_Type const * p_reg);
#endif // NRF_SPIM_HAS_STALLSTAT

#if NRF_SPIM_HAS_FREQUENCY
/**
 * @brief Function for setting the SPI master data rate.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] frequency SPI frequency.
 */
NRF_STATIC_INLINE void nrf_spim_frequency_set(NRF_SPIM_Type *      p_reg,
                                              nrf_spim_frequency_t frequency);

/**
 * @brief Function for getting the SPI master data rate.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval SPI master frequency.
 */
NRF_STATIC_INLINE nrf_spim_frequency_t nrf_spim_frequency_get(NRF_SPIM_Type * p_reg);
#endif

/**
 * @brief Function for setting the transmit buffer.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the buffer with data to send.
 * @param[in] length   Maximum number of data bytes to transmit.
 */
NRF_STATIC_INLINE void nrf_spim_tx_buffer_set(NRF_SPIM_Type * p_reg,
                                              uint8_t const * p_buffer,
                                              size_t          length);

/**
 * @brief Function for getting number of bytes transmitted in the last transaction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Amount of bytes transmitted.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_tx_amount_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for getting number of bytes to be transmitted in the next transaction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Amount of bytes to be transmitted.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_tx_maxcnt_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for setting the receive buffer.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_buffer Pointer to the buffer for received data.
 * @param[in] length   Maximum number of data bytes to receive.
 */
NRF_STATIC_INLINE void nrf_spim_rx_buffer_set(NRF_SPIM_Type * p_reg,
                                              uint8_t *       p_buffer,
                                              size_t          length);

/**
 * @brief Function for getting number of bytes received in the last transaction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Amount of bytes received.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_rx_amount_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for getting number of bytes to be received in the next transaction.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Amount of bytes to be received.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_rx_maxcnt_get(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for setting the SPI configuration.
 *
 * @param[in] p_reg         Pointer to the structure of registers of the peripheral.
 * @param[in] spi_mode      SPI mode.
 * @param[in] spi_bit_order SPI bit order.
 */
NRF_STATIC_INLINE void nrf_spim_configure(NRF_SPIM_Type *      p_reg,
                                          nrf_spim_mode_t      spi_mode,
                                          nrf_spim_bit_order_t spi_bit_order);

/**
 * @brief Function for setting the over-read character.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] orc   Over-read character that is clocked out in case of
 *                  an over-read of the TXD buffer.
 */
NRF_STATIC_INLINE void nrf_spim_orc_set(NRF_SPIM_Type * p_reg,
                                        uint8_t         orc);

#if NRF_SPIM_HAS_ARRAY_LIST
/**
 * @brief Function for enabling the TX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_tx_list_enable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for disabling the TX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_tx_list_disable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for enabling the RX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_rx_list_enable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for disabling the RX list feature.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_rx_list_disable(NRF_SPIM_Type * p_reg);
#endif

#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
/**
 * @brief Function for enabling individual pattern match filters.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] index  Index of pattern match filter.
 * @param[in] enable True if pattern match filter is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_spim_rx_pattern_match_enable_set(NRF_SPIM_Type * p_reg,
                                                            uint8_t         index,
                                                            bool            enable);

/**
 * @brief Function for checking if the specified pattern match filter is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 *
 * @retval true  Pattern match filter is enabled.
 * @retval false Pattern match filter is disabled.
 */
NRF_STATIC_INLINE bool nrf_spim_rx_pattern_match_enable_check(NRF_SPIM_Type const * p_reg,
                                                              uint8_t               index);

/**
 * @brief Function for enabling one-shot operation for the specified match filter.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 */
NRF_STATIC_INLINE void nrf_spim_rx_pattern_match_one_shot_enable(NRF_SPIM_Type * p_reg,
                                                                 uint8_t         index);

/**
 * @brief Function for disabling one-shot operation for the specified match filter.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 */
NRF_STATIC_INLINE void nrf_spim_rx_pattern_match_one_shot_disable(NRF_SPIM_Type * p_reg,
                                                                  uint8_t         index);

/**
 * @brief Function for checking if specified pattern match filter is configured as one-shot.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 *
 * @retval true  Pattern match filter is configured as one-shot.
 * @retval false Pattern match filter is configured as continuous.
 */
NRF_STATIC_INLINE bool nrf_spim_rx_pattern_match_one_shot_check(NRF_SPIM_Type const * p_reg,
                                                                uint8_t               index);

/**
 * @brief Function for setting the pattern to be looked for by the specified match filter.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] index   Index of pattern match filter.
 * @param[in] pattern Pattern to be looked for.
 *                    Match will trigger the corresponding event, if enabled.
 */
NRF_STATIC_INLINE void nrf_spim_rx_pattern_match_candidate_set(NRF_SPIM_Type * p_reg,
                                                               uint8_t         index,
                                                               uint32_t        pattern);

/**
 * @brief Function for getting the pattern that the specified match filter is looking for.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] index Index of pattern match filter.
 *
 * @return Pattern that the specified match filter is looking for.
 */
NRF_STATIC_INLINE uint32_t nrf_spim_rx_pattern_match_candidate_get(NRF_SPIM_Type const * p_reg,
                                                                   uint8_t               index);
#endif // NRF_SPIM_HAS_DMA_TASKS_EVENTS

#if NRF_SPIM_HAS_DMA_REG
/**
 * @brief Function for enabling the termination of the RX transaction after detecting the BUSERROR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_rx_terminate_on_bus_error_enable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for disabling the termination of the RX transaction after detecting the BUSERROR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_rx_terminate_on_bus_error_disable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for checking if RX transaction termination after detecting the BUSERROR event is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  RX transaction termination after detecting a BUSERROR event is enabled.
 * @retval false RX transaction termination after detecting a BUSERROR event is disabled.
 */
NRF_STATIC_INLINE bool nrf_spim_rx_terminate_on_bus_error_check(NRF_SPIM_Type const * p_reg);

/**
 * @brief Function for enabling the termination of the TX transaction after detecting the BUSERROR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_tx_terminate_on_bus_error_enable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for disabling the termination of the TX transaction after detecting the BUSERROR event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_spim_tx_terminate_on_bus_error_disable(NRF_SPIM_Type * p_reg);

/**
 * @brief Function for checking if TX transaction termination after detecting the BUSERROR event is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  TX transaction termination after detecting a BUSERROR event is enabled.
 * @retval false TX transaction termination after detecting a BUSERROR event is disabled.
 */
NRF_STATIC_INLINE bool nrf_spim_tx_terminate_on_bus_error_check(NRF_SPIM_Type const * p_reg);
#endif // NRF_SPIM_HAS_DMA_REG

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_spim_task_trigger(NRF_SPIM_Type * p_reg,
                                             nrf_spim_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_spim_task_address_get(NRF_SPIM_Type const * p_reg,
                                                     nrf_spim_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE void nrf_spim_event_clear(NRF_SPIM_Type *  p_reg,
                                            nrf_spim_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_spim_event_check(NRF_SPIM_Type const * p_reg,
                                            nrf_spim_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_spim_event_address_get(NRF_SPIM_Type const * p_reg,
                                                      nrf_spim_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_spim_shorts_enable(NRF_SPIM_Type * p_reg,
                                              uint32_t        mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_spim_shorts_disable(NRF_SPIM_Type * p_reg,
                                               uint32_t        mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE uint32_t nrf_spim_shorts_get(NRF_SPIM_Type const * p_reg)
{
    return p_reg->SHORTS;
}

NRF_STATIC_INLINE void nrf_spim_int_enable(NRF_SPIM_Type * p_reg,
                                           uint32_t        mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_spim_int_disable(NRF_SPIM_Type * p_reg,
                                            uint32_t        mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_spim_int_enable_check(NRF_SPIM_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

#if NRF_SPIM_HAS_PRESCALER
NRF_STATIC_INLINE void nrf_spim_prescaler_set(NRF_SPIM_Type * p_reg,
                                              uint32_t        prescaler)
{
    NRFX_ASSERT(prescaler >= NRF_SPIM_PRESCALER_MIN_GET(p_reg));
    NRFX_ASSERT(prescaler <= NRF_SPIM_PRESCALER_MAX_GET(p_reg));
    NRFX_ASSERT(NRFX_IS_EVEN(prescaler));
    p_reg->PRESCALER = prescaler;
}

NRF_STATIC_INLINE uint32_t nrf_spim_prescaler_get(NRF_SPIM_Type const * p_reg)
{
    return p_reg->PRESCALER;
}
#endif

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_spim_subscribe_set(NRF_SPIM_Type * p_reg,
                                              nrf_spim_task_t task,
                                              uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_spim_subscribe_clear(NRF_SPIM_Type * p_reg,
                                                nrf_spim_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_spim_subscribe_get(NRF_SPIM_Type const * p_reg,
                                                  nrf_spim_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_spim_publish_set(NRF_SPIM_Type *  p_reg,
                                            nrf_spim_event_t event,
                                            uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_spim_publish_clear(NRF_SPIM_Type *  p_reg,
                                              nrf_spim_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_spim_publish_get(NRF_SPIM_Type const * p_reg,
                                                nrf_spim_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif

NRF_STATIC_INLINE void nrf_spim_enable(NRF_SPIM_Type * p_reg)
{
    p_reg->ENABLE = (SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_spim_disable(NRF_SPIM_Type * p_reg)
{
    if (NRF_ERRATA_DYNAMIC_CHECK(52, 89))
    {
        uint32_t temp1 = *(volatile uint32_t *)((uint8_t *)p_reg + 0x508UL);
        uint32_t temp2 = *(volatile uint32_t *)((uint8_t *)p_reg + 0x50CUL);
        *(volatile uint32_t *)((uint8_t *)p_reg + 0x500) = 0;
        *(volatile uint32_t *)((uint8_t *)p_reg + 0x508) = 0xFFFFFFFF;
        *(volatile uint32_t *)((uint8_t *)p_reg + 0x50C) = 0xFFFFFFFF;
        *(volatile uint32_t *)((uint8_t *)p_reg + 0x500) = 9;

        p_reg->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);

        *(volatile uint32_t *)((uint8_t *)p_reg + 0x508) = temp1;
        *(volatile uint32_t *)((uint8_t *)p_reg + 0x50C) = temp2;
    }
    else
    {
        p_reg->ENABLE = (SPIM_ENABLE_ENABLE_Disabled << SPIM_ENABLE_ENABLE_Pos);
    }
}

NRF_STATIC_INLINE bool nrf_spim_enable_check(NRF_SPIM_Type const * p_reg)
{
    return p_reg->ENABLE == SPIM_ENABLE_ENABLE_Enabled;
}

NRF_STATIC_INLINE void nrf_spim_pins_set(NRF_SPIM_Type * p_reg,
                                         uint32_t        sck_pin,
                                         uint32_t        mosi_pin,
                                         uint32_t        miso_pin)
{
    p_reg->PSEL.SCK  = sck_pin;
    p_reg->PSEL.MOSI = mosi_pin;
    p_reg->PSEL.MISO = miso_pin;
}

NRF_STATIC_INLINE void nrf_spim_sck_pin_set(NRF_SPIM_Type * p_reg, uint32_t pin)
{
    p_reg->PSEL.SCK = pin;
}

NRF_STATIC_INLINE void nrf_spim_mosi_pin_set(NRF_SPIM_Type * p_reg, uint32_t pin)
{
    p_reg->PSEL.MOSI = pin;
}

NRF_STATIC_INLINE void nrf_spim_miso_pin_set(NRF_SPIM_Type * p_reg, uint32_t pin)
{
    p_reg->PSEL.MISO = pin;
}

NRF_STATIC_INLINE uint32_t nrf_spim_sck_pin_get(NRF_SPIM_Type const * p_reg)
{
    return p_reg->PSEL.SCK;
}

NRF_STATIC_INLINE uint32_t nrf_spim_mosi_pin_get(NRF_SPIM_Type const * p_reg)
{
    return p_reg->PSEL.MOSI;
}

NRF_STATIC_INLINE uint32_t nrf_spim_miso_pin_get(NRF_SPIM_Type const * p_reg)
{
    return p_reg->PSEL.MISO;
}

#if NRF_SPIM_HAS_HW_CSN
NRF_STATIC_INLINE void nrf_spim_csn_configure(NRF_SPIM_Type *    p_reg,
                                              uint32_t           pin,
                                              nrf_spim_csn_pol_t polarity,
                                              uint32_t           duration)
{
#if defined(SPIM_CSNPOL_CSNPOL0_LOW) && defined(SPIM_PSEL_CSN_MaxCount)
    p_reg->PSEL.CSN[0] = pin;
#else
    p_reg->PSEL.CSN = pin;
#endif
    p_reg->CSNPOL = polarity;
    p_reg->IFTIMING.CSNDUR = duration;
}

NRF_STATIC_INLINE uint32_t nrf_spim_csn_pin_get(NRF_SPIM_Type const * p_reg)
{
#if defined(SPIM_CSNPOL_CSNPOL0_LOW) && defined(SPIM_PSEL_CSN_MaxCount)
    return p_reg->PSEL.CSN[0];
#else
    return p_reg->PSEL.CSN;
#endif
}
#endif // NRF_SPIM_HAS_HW_CSN

#if NRF_SPIM_HAS_DCX
NRF_STATIC_INLINE void nrf_spim_dcx_pin_set(NRF_SPIM_Type * p_reg, uint32_t dcx_pin)
{
#if defined(SPIM_PSEL_DCX_PIN_Msk)
    p_reg->PSEL.DCX = dcx_pin;
#else
    p_reg->PSELDCX = dcx_pin;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spim_dcx_pin_get(NRF_SPIM_Type const * p_reg)
{
#if defined(SPIM_PSEL_DCX_PIN_Msk)
    return p_reg->PSEL.DCX;
#else
    return p_reg->PSELDCX;
#endif
}

NRF_STATIC_INLINE void nrf_spim_dcx_cnt_set(NRF_SPIM_Type * p_reg, uint32_t dcx_cnt)
{
    p_reg->DCXCNT = dcx_cnt;
}
#endif // NRF_SPIM_HAS_DCX

#if NRF_SPIM_HAS_RXDELAY
NRF_STATIC_INLINE void nrf_spim_iftiming_set(NRF_SPIM_Type * p_reg,
                                             uint32_t        rxdelay)
{
    p_reg->IFTIMING.RXDELAY = rxdelay;
}
#endif

#if NRF_SPIM_HAS_STALLSTAT
NRF_STATIC_INLINE void nrf_spim_stallstat_rx_clear(NRF_SPIM_Type * p_reg)
{
    p_reg->STALLSTAT &= ~(SPIM_STALLSTAT_RX_Msk);
}

NRF_STATIC_INLINE bool nrf_spim_stallstat_rx_get(NRF_SPIM_Type const * p_reg)
{
    return (p_reg->STALLSTAT & SPIM_STALLSTAT_RX_Msk) != 0;
}

NRF_STATIC_INLINE void nrf_spim_stallstat_tx_clear(NRF_SPIM_Type * p_reg)
{
    p_reg->STALLSTAT &= ~(SPIM_STALLSTAT_TX_Msk);
}

NRF_STATIC_INLINE bool nrf_spim_stallstat_tx_get(NRF_SPIM_Type const * p_reg)
{
    return (p_reg->STALLSTAT & SPIM_STALLSTAT_TX_Msk) != 0;
}
#endif

#if NRF_SPIM_HAS_FREQUENCY
NRF_STATIC_INLINE void nrf_spim_frequency_set(NRF_SPIM_Type *      p_reg,
                                              nrf_spim_frequency_t frequency)
{
    p_reg->FREQUENCY = (uint32_t)frequency;
}

NRF_STATIC_INLINE nrf_spim_frequency_t nrf_spim_frequency_get(NRF_SPIM_Type * p_reg)
{
    return (nrf_spim_frequency_t)(p_reg->FREQUENCY);
}
#endif

NRF_STATIC_INLINE void nrf_spim_tx_buffer_set(NRF_SPIM_Type * p_reg,
                                              uint8_t const * p_buffer,
                                              size_t          length)
{
#if NRF_SPIM_HAS_DMA_REG
    p_reg->DMA.TX.PTR    = (uint32_t)p_buffer;
    p_reg->DMA.TX.MAXCNT = length;
#else
    p_reg->TXD.PTR    = (uint32_t)p_buffer;
    p_reg->TXD.MAXCNT = length;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spim_tx_amount_get(NRF_SPIM_Type const * p_reg)
{
#if NRF_SPIM_HAS_DMA_REG
    return p_reg->DMA.TX.AMOUNT;
#else
    return p_reg->TXD.AMOUNT;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spim_tx_maxcnt_get(NRF_SPIM_Type const * p_reg)
{
#if NRF_SPIM_HAS_DMA_REG
    return p_reg->DMA.TX.MAXCNT;
#else
    return p_reg->TXD.MAXCNT;
#endif
}

NRF_STATIC_INLINE void nrf_spim_rx_buffer_set(NRF_SPIM_Type * p_reg,
                                              uint8_t *       p_buffer,
                                              size_t          length)
{
#if NRF_SPIM_HAS_DMA_REG
    p_reg->DMA.RX.PTR    = (uint32_t)p_buffer;
    p_reg->DMA.RX.MAXCNT = length;
#else
    p_reg->RXD.PTR    = (uint32_t)p_buffer;
    p_reg->RXD.MAXCNT = length;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spim_rx_amount_get(NRF_SPIM_Type const * p_reg)
{
#if NRF_SPIM_HAS_DMA_REG
    return p_reg->DMA.RX.AMOUNT;
#else
    return p_reg->RXD.AMOUNT;
#endif
}

NRF_STATIC_INLINE uint32_t nrf_spim_rx_maxcnt_get(NRF_SPIM_Type const * p_reg)
{
#if NRF_SPIM_HAS_DMA_REG
    return p_reg->DMA.RX.MAXCNT;
#else
    return p_reg->RXD.MAXCNT;
#endif
}

NRF_STATIC_INLINE void nrf_spim_configure(NRF_SPIM_Type *      p_reg,
                                          nrf_spim_mode_t      spi_mode,
                                          nrf_spim_bit_order_t spi_bit_order)
{
    uint32_t config = (spi_bit_order == NRF_SPIM_BIT_ORDER_MSB_FIRST ?
        SPIM_CONFIG_ORDER_MsbFirst : SPIM_CONFIG_ORDER_LsbFirst);
    switch (spi_mode)
    {
    default:
    case NRF_SPIM_MODE_0:
        config |= (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Leading    << SPIM_CONFIG_CPHA_Pos);
        break;

    case NRF_SPIM_MODE_1:
        config |= (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos);
        break;

    case NRF_SPIM_MODE_2:
        config |= (SPIM_CONFIG_CPOL_ActiveLow  << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Leading    << SPIM_CONFIG_CPHA_Pos);
        break;

    case NRF_SPIM_MODE_3:
        config |= (SPIM_CONFIG_CPOL_ActiveLow  << SPIM_CONFIG_CPOL_Pos) |
                  (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos);
        break;
    }
    p_reg->CONFIG = config;
}

NRF_STATIC_INLINE void nrf_spim_orc_set(NRF_SPIM_Type * p_reg,
                                        uint8_t         orc)
{
    p_reg->ORC = orc;
}

#if NRF_SPIM_HAS_ARRAY_LIST
NRF_STATIC_INLINE void nrf_spim_tx_list_enable(NRF_SPIM_Type * p_reg)
{
#if NRF_SPIM_HAS_DMA_REG
    p_reg->DMA.TX.LIST = SPIM_DMA_TX_LIST_TYPE_ArrayList << SPIM_DMA_TX_LIST_TYPE_Pos;
#else
    p_reg->TXD.LIST = SPIM_TXD_LIST_LIST_ArrayList << SPIM_TXD_LIST_LIST_Pos;
#endif
}

NRF_STATIC_INLINE void nrf_spim_tx_list_disable(NRF_SPIM_Type * p_reg)
{
#if NRF_SPIM_HAS_DMA_REG
    p_reg->DMA.TX.LIST = SPIM_DMA_TX_LIST_TYPE_Disabled << SPIM_DMA_TX_LIST_TYPE_Pos;
#else
    p_reg->TXD.LIST = SPIM_TXD_LIST_LIST_Disabled << SPIM_TXD_LIST_LIST_Pos;
#endif
}

NRF_STATIC_INLINE void nrf_spim_rx_list_enable(NRF_SPIM_Type * p_reg)
{
#if NRF_SPIM_HAS_DMA_REG
    p_reg->DMA.RX.LIST = SPIM_DMA_RX_LIST_TYPE_ArrayList << SPIM_DMA_RX_LIST_TYPE_Pos;
#else
    p_reg->RXD.LIST = SPIM_RXD_LIST_LIST_ArrayList << SPIM_RXD_LIST_LIST_Pos;
#endif
}

NRF_STATIC_INLINE void nrf_spim_rx_list_disable(NRF_SPIM_Type * p_reg)
{
#if NRF_SPIM_HAS_DMA_REG
    p_reg->DMA.RX.LIST = SPIM_DMA_RX_LIST_TYPE_Disabled << SPIM_DMA_RX_LIST_TYPE_Pos;
#else
    p_reg->RXD.LIST = SPIM_RXD_LIST_LIST_Disabled << SPIM_RXD_LIST_LIST_Pos;
#endif
}
#endif

#if NRF_SPIM_HAS_DMA_TASKS_EVENTS
NRF_STATIC_INLINE void nrf_spim_rx_pattern_match_enable_set(NRF_SPIM_Type * p_reg,
                                                            uint8_t         index,
                                                            bool            enable)
{
    NRFX_ASSERT(index < NRF_SPIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                         ~SPIM_DMA_RX_MATCH_CONFIG_ENABLE0_Msk) |
                                         ((enable ?
                                           SPIM_DMA_RX_MATCH_CONFIG_ENABLE0_Enabled :
                                           SPIM_DMA_RX_MATCH_CONFIG_ENABLE0_Disabled)
                                           << SPIM_DMA_RX_MATCH_CONFIG_ENABLE0_Pos));
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                         ~SPIM_DMA_RX_MATCH_CONFIG_ENABLE1_Msk) |
                                         ((enable ?
                                           SPIM_DMA_RX_MATCH_CONFIG_ENABLE1_Enabled :
                                           SPIM_DMA_RX_MATCH_CONFIG_ENABLE1_Disabled)
                                           << SPIM_DMA_RX_MATCH_CONFIG_ENABLE1_Pos));
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                         ~SPIM_DMA_RX_MATCH_CONFIG_ENABLE2_Msk) |
                                         ((enable ?
                                           SPIM_DMA_RX_MATCH_CONFIG_ENABLE2_Enabled :
                                           SPIM_DMA_RX_MATCH_CONFIG_ENABLE2_Disabled)
                                           << SPIM_DMA_RX_MATCH_CONFIG_ENABLE2_Pos));
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG = ((p_reg->DMA.RX.MATCH.CONFIG &
                                         ~SPIM_DMA_RX_MATCH_CONFIG_ENABLE3_Msk) |
                                         ((enable ?
                                           SPIM_DMA_RX_MATCH_CONFIG_ENABLE3_Enabled :
                                           SPIM_DMA_RX_MATCH_CONFIG_ENABLE3_Disabled)
                                           << SPIM_DMA_RX_MATCH_CONFIG_ENABLE3_Pos));
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE bool nrf_spim_rx_pattern_match_enable_check(NRF_SPIM_Type const * p_reg,
                                                              uint8_t               index)
{
    NRFX_ASSERT(index < NRF_SPIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIM_DMA_RX_MATCH_CONFIG_ENABLE0_Msk)
                    >> SPIM_DMA_RX_MATCH_CONFIG_ENABLE0_Pos) ==
                    SPIM_DMA_RX_MATCH_CONFIG_ENABLE0_Enabled;
        case 1:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIM_DMA_RX_MATCH_CONFIG_ENABLE1_Msk)
                    >> SPIM_DMA_RX_MATCH_CONFIG_ENABLE1_Pos) ==
                    SPIM_DMA_RX_MATCH_CONFIG_ENABLE1_Enabled;
        case 2:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIM_DMA_RX_MATCH_CONFIG_ENABLE2_Msk)
                    >> SPIM_DMA_RX_MATCH_CONFIG_ENABLE2_Pos) ==
                    SPIM_DMA_RX_MATCH_CONFIG_ENABLE2_Enabled;
        case 3:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIM_DMA_RX_MATCH_CONFIG_ENABLE3_Msk)
                    >> SPIM_DMA_RX_MATCH_CONFIG_ENABLE3_Pos) ==
                    SPIM_DMA_RX_MATCH_CONFIG_ENABLE3_Enabled;
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_spim_rx_pattern_match_one_shot_enable(NRF_SPIM_Type * p_reg,
                                                                 uint8_t         index)
{
    NRFX_ASSERT(index < NRF_SPIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG |= SPIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk;
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG |= SPIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk;
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG |= SPIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk;
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG |= SPIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk;
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE void nrf_spim_rx_pattern_match_one_shot_disable(NRF_SPIM_Type * p_reg,
                                                                  uint8_t         index)
{
    NRFX_ASSERT(index < NRF_SPIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(SPIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk);
            break;
        case 1:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(SPIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk);
            break;
        case 2:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(SPIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk);
            break;
        case 3:
            p_reg->DMA.RX.MATCH.CONFIG &= ~(SPIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk);
            break;
        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE bool nrf_spim_rx_pattern_match_one_shot_check(NRF_SPIM_Type const * p_reg,
                                                                uint8_t               index)
{
    NRFX_ASSERT(index < NRF_SPIM_DMA_RX_PATTERN_MAX_COUNT);
    switch (index)
    {
        case 0:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Msk)
                    >> SPIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Pos) ==
                    SPIM_DMA_RX_MATCH_CONFIG_ONESHOT0_Oneshot;
        case 1:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Msk)
                    >> SPIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Pos) ==
                    SPIM_DMA_RX_MATCH_CONFIG_ONESHOT1_Oneshot;
        case 2:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Msk)
                    >> SPIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Pos) ==
                    SPIM_DMA_RX_MATCH_CONFIG_ONESHOT2_Oneshot;
        case 3:
            return ((p_reg->DMA.RX.MATCH.CONFIG & SPIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Msk)
                    >> SPIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Pos) ==
                    SPIM_DMA_RX_MATCH_CONFIG_ONESHOT3_Oneshot;
        default:
            NRFX_ASSERT(false);
            return 0;
    }
}

NRF_STATIC_INLINE void nrf_spim_rx_pattern_match_candidate_set(NRF_SPIM_Type * p_reg,
                                                               uint8_t         index,
                                                               uint32_t        pattern)
{
    NRFX_ASSERT(index < NRF_SPIM_DMA_RX_PATTERN_MAX_COUNT);
    p_reg->DMA.RX.MATCH.CANDIDATE[index] = pattern;
}

NRF_STATIC_INLINE uint32_t nrf_spim_rx_pattern_match_candidate_get(NRF_SPIM_Type const * p_reg,
                                                                   uint8_t               index)
{
    NRFX_ASSERT(index < NRF_SPIM_DMA_RX_PATTERN_MAX_COUNT);
    return p_reg->DMA.RX.MATCH.CANDIDATE[index];
}
#endif // NRF_SPIM_HAS_DMA_TASKS_EVENTS

#if NRF_SPIM_HAS_DMA_REG
NRF_STATIC_INLINE void nrf_spim_rx_terminate_on_bus_error_enable(NRF_SPIM_Type * p_reg)
{
    p_reg->DMA.RX.TERMINATEONBUSERROR |= SPIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Msk;
}

NRF_STATIC_INLINE void nrf_spim_rx_terminate_on_bus_error_disable(NRF_SPIM_Type * p_reg)
{
    p_reg->DMA.RX.TERMINATEONBUSERROR &= ~(SPIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Msk);
}

NRF_STATIC_INLINE bool nrf_spim_rx_terminate_on_bus_error_check(NRF_SPIM_Type const * p_reg)
{
    return ((p_reg->DMA.RX.TERMINATEONBUSERROR & SPIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Msk)
            >> SPIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Pos) ==
            SPIM_DMA_RX_TERMINATEONBUSERROR_ENABLE_Enabled;
}

NRF_STATIC_INLINE void nrf_spim_tx_terminate_on_bus_error_enable(NRF_SPIM_Type * p_reg)
{
    p_reg->DMA.TX.TERMINATEONBUSERROR |= SPIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Msk;
}

NRF_STATIC_INLINE void nrf_spim_tx_terminate_on_bus_error_disable(NRF_SPIM_Type * p_reg)
{
    p_reg->DMA.TX.TERMINATEONBUSERROR &= ~(SPIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Msk);
}

NRF_STATIC_INLINE bool nrf_spim_tx_terminate_on_bus_error_check(NRF_SPIM_Type const * p_reg)
{
    return ((p_reg->DMA.TX.TERMINATEONBUSERROR & SPIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Msk)
            >> SPIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Pos) ==
            SPIM_DMA_TX_TERMINATEONBUSERROR_ENABLE_Enabled;
}
#endif // NRF_SPIM_HAS_DMA_REG

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_SPIM_H__
