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

#ifndef NRF_CCM_H__
#define NRF_CCM_H__

#include <nrfx.h>
#ifdef EASYVDMA_PRESENT
#include <helpers/nrf_vdma.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CCM_MODE_DATARATE_125Kbps)
#define NRF_CCM_MODE_DATARATE_125K CCM_MODE_DATARATE_125Kbps
#define NRF_CCM_MODE_DATARATE_500K CCM_MODE_DATARATE_500Kbps
#else
#define NRF_CCM_MODE_DATARATE_125K CCM_MODE_DATARATE_125Kbit
#define NRF_CCM_MODE_DATARATE_500K CCM_MODE_DATARATE_500Kbit
#endif

/**
 * @defgroup nrf_ccm_hal AES CCM HAL
 * @{
 * @ingroup nrf_ccm
 * @brief   Hardware access layer for managing the AES CCM peripheral.
 */

#if defined(CCM_TASKS_KSGEN_TASKS_KSGEN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the KSGEN task. */
#define NRF_CCM_HAS_TASK_KSGEN 1
#else
#define NRF_CCM_HAS_TASK_KSGEN 0
#endif

#if defined(CCM_TASKS_CRYPT_TASKS_CRYPT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the CRYPT task. */
#define NRF_CCM_HAS_TASK_CRYPT 1
#else
#define NRF_CCM_HAS_TASK_CRYPT 0
#endif

#if defined(CCM_TASKS_RATEOVERRIDE_TASKS_RATEOVERRIDE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the RATEOVERRIDE task. */
#define NRF_CCM_HAS_TASK_RATEOVERRIDE 1
#else
#define NRF_CCM_HAS_TASK_RATEOVERRIDE 0
#endif

#if defined(CCM_EVENTS_ENDKSGEN_EVENTS_ENDKSGEN_Msk) || defined(CCM_INTENSET_ENDKSGEN_Msk) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Presence of the ENDKSGEN event. */
#define NRF_CCM_HAS_EVENT_ENDKSGEN 1
#else
#define NRF_CCM_HAS_EVENT_ENDKSGEN 0
#endif

#if defined(CCM_EVENTS_ENDCRYPT_EVENTS_ENDCRYPT_Msk) || defined(CCM_INTENSET_ENDCRYPT_Msk) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Presence of the ENDCRYPT event. */
#define NRF_CCM_HAS_EVENT_ENDCRYPT 1
#else
#define NRF_CCM_HAS_EVENT_ENDCRYPT 0
#endif

#if defined(CCM_ADATAMASK_ADATAMASK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the ADATAMASK register. */
#define NRF_CCM_HAS_ADATAMASK 1
#else
#define NRF_CCM_HAS_ADATAMASK 0
#endif

#if defined(CCM_CNFPTR_CNFPTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the CNFPTR register. */
#define NRF_CCM_HAS_CNFPTR 1
#else
#define NRF_CCM_HAS_CNFPTR 0
#endif

#if defined(CCM_IN_AMOUNT_AMOUNT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the IN.AMOUNT register. */
#define NRF_CCM_HAS_IN_AMOUNT 1
#else
#define NRF_CCM_HAS_IN_AMOUNT 0
#endif

#if defined(CCM_OUT_AMOUNT_AMOUNT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the OUT.AMOUNT register. */
#define NRF_CCM_HAS_OUT_AMOUNT 1
#else
#define NRF_CCM_HAS_OUT_AMOUNT 0
#endif

#if defined(CCM_RATEOVERRIDE_RATEOVERRIDE_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the RATEOVERRIDE register. */
#define NRF_CCM_HAS_RATEOVERRIDE 1
#else
#define NRF_CCM_HAS_RATEOVERRIDE 0
#endif

#if defined(CCM_ERRORSTATUS_ERRORSTATUS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the ERRORSTATUS register. */
#define NRF_CCM_HAS_ERRORSTATUS 1
#else
#define NRF_CCM_HAS_ERRORSTATUS 0
#endif

#if defined(CCM_MICSTATUS_MICSTATUS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the MICSTATUS register. */
#define NRF_CCM_HAS_MICSTATUS 1
#else
#define NRF_CCM_HAS_MICSTATUS 0
#endif

#if defined(CCM_MACSTATUS_MACSTATUS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the MACSTATUS register. */
#define NRF_CCM_HAS_MACSTATUS 1
#else
#define NRF_CCM_HAS_MACSTATUS 0
#endif

#if defined(CCM_KEY_VALUE_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the KEY register. */
#define NRF_CCM_HAS_KEY 1
#else
#define NRF_CCM_HAS_KEY 0
#endif

#if defined(CCM_NONCE_VALUE_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the NONCE register. */
#define NRF_CCM_HAS_NONCE 1
#else
#define NRF_CCM_HAS_NONCE 0
#endif

#if defined(CCM_INPTR_INPTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the INPTR register. */
#define NRF_CCM_HAS_INPTR 1
#else
#define NRF_CCM_HAS_INPTR 0
#endif

#if defined(CCM_OUTPTR_OUTPTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the OUTPTR register. */
#define NRF_CCM_HAS_OUTPTR 1
#else
#define NRF_CCM_HAS_OUTPTR 0
#endif

#if defined(CCM_IN_PTR_PTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the IN.PTR register. */
#define NRF_CCM_HAS_IN_PTR 1
#else
#define NRF_CCM_HAS_IN_PTR 0
#endif

#if defined(CCM_OUT_PTR_PTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the OUT.PTR register. */
#define NRF_CCM_HAS_OUT_PTR 1
#else
#define NRF_CCM_HAS_OUT_PTR 0
#endif

#if defined(CCM_SCRATCHPTR_SCRATCHPTR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the SCRATCHPTR register. */
#define NRF_CCM_HAS_SCRATCHPTR 1
#else
#define NRF_CCM_HAS_SCRATCHPTR 0
#endif

#if defined(CCM_MAXPACKETSIZE_MAXPACKETSIZE_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the MAXPACKETSIZE. */
#define NRF_CCM_HAS_MAXPACKETSIZE 1
#else
#define NRF_CCM_HAS_MAXPACKETSIZE 0
#endif

#if defined(CCM_MODE_MODE_FastDecryption) || defined(__NRFX_DOXYGEN__)
/** Presence of AES fast decrypt mode. */
#define NRF_CCM_HAS_MODE_FAST_DECRYPTION 1
#else
#define NRF_CCM_HAS_MODE_FAST_DECRYPTION 0
#endif

#if defined(CCM_MODE_PROTOCOL_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of protocol and packet format selection. */
#define NRF_CCM_HAS_MODE_PROTOCOL 1
#else
#define NRF_CCM_HAS_MODE_PROTOCOL 0
#endif

#if defined(CCM_MODE_PROTOCOL_Ble) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the BLE packet format. */
#define NRF_CCM_HAS_MODE_PROTOCOL_BLE 1
#else
#define NRF_CCM_HAS_MODE_PROTOCOL_BLE 0
#endif

#if defined(CCM_MODE_PROTOCOL_Ieee802154) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the 802.15.4 packet format. */
#define NRF_CCM_HAS_MODE_PROTOCOL_IEEE802154 1
#else
#define NRF_CCM_HAS_MODE_PROTOCOL_IEEE802154 0
#endif

#if defined(CCM_MODE_LENGTH_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the packet lengh configuration. */
#define NRF_CCM_HAS_MODE_LENGTH 1
#else
#define NRF_CCM_HAS_MODE_LENGTH 0
#endif

#if defined(CCM_MODE_DATARATE_125Kbit) || defined(CCM_MODE_DATARATE_125Kbps) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Support for 125 Kbit radio data rate. */
#define NRF_CCM_HAS_MODE_DATARATE_125K 1
#else
#define NRF_CCM_HAS_MODE_DATARATE_125K 0
#endif

#if defined(CCM_MODE_DATARATE_250Kbit) || defined(__NRFX_DOXYGEN__)
/** @brief Support for 250 Kbit radio data rate. */
#define NRF_CCM_HAS_MODE_DATARATE_250K 1
#else
#define NRF_CCM_HAS_MODE_DATARATE_250K 0
#endif

#if defined(CCM_MODE_DATARATE_500Kbit) || defined(CCM_MODE_DATARATE_500Kbps) || \
    defined(__NRFX_DOXYGEN__)
/** @brief Support for 500 Kbit radio data rate. */
#define NRF_CCM_HAS_MODE_DATARATE_500K 1
#else
#define NRF_CCM_HAS_MODE_DATARATE_500K 0
#endif

#if defined(CCM_MODE_DATARATE_4Mbit) || defined(__NRFX_DOXYGEN__)
/** @brief Support for 4 Mbit radio data rate. */
#define NRF_CCM_HAS_MODE_DATARATE_4M 1
#else
#define NRF_CCM_HAS_MODE_DATARATE_4M 0
#endif

#if defined(CCM_MODE_MACLEN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the CCM MAC length. */
#define NRF_CCM_HAS_MODE_MACLEN 1
#else
#define NRF_CCM_HAS_MODE_MACLEN 0
#endif

#if defined(CCM_MODE_DATARATE_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the radio data rate that the CCM shall run synchronous with. */
#define NRF_CCM_HAS_DATARATE 1
#else
#define NRF_CCM_HAS_DATARATE 0
#endif

#if defined(CCM_HEADERMASK_HEADERMASK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Presence of the HEADERMASK register. */
#define NRF_CCM_HAS_HEADERMASK 1
#else
#define NRF_CCM_HAS_HEADERMASK 0
#endif

#if NRF_CCM_HAS_CNFPTR
/** @brief AES key size. */
#define NRF_CCM_KEY_SIZE    16
/** @brief Initialization vector size. */
#define NRF_CCM_IV_SIZE     8
/** @brief Packet counter configuration size. */
#define NRF_CCM_PKTCTR_SIZE 9

/** @brief CCM data structure. */
typedef struct __PACKED
{
    uint8_t key[NRF_CCM_KEY_SIZE];       ///< 16-byte AES key.
    uint8_t pktctr[NRF_CCM_PKTCTR_SIZE]; ///< Packet counter configuration.
    uint8_t iv[NRF_CCM_IV_SIZE];         ///< 8-byte initialization vector (IV).
} nrf_ccm_cnf_t;
#endif

/** @brief CCM tasks. */
typedef enum
{
#if NRF_CCM_HAS_TASK_KSGEN
    NRF_CCM_TASK_KSGEN        = offsetof(NRF_CCM_Type, TASKS_KSGEN),        ///< Start generation of key-stream.
#endif
#if NRF_CCM_HAS_TASK_CRYPT
    NRF_CCM_TASK_START        = offsetof(NRF_CCM_Type, TASKS_CRYPT),        ///< Start encryption/decryption.
#else
    NRF_CCM_TASK_START        = offsetof(NRF_CCM_Type, TASKS_START),        ///< Start encryption/decryption.
#endif
    NRF_CCM_TASK_STOP         = offsetof(NRF_CCM_Type, TASKS_STOP),         ///< Stop encryption/decryption.
#if NRF_CCM_HAS_TASK_RATEOVERRIDE
    NRF_CCM_TASK_RATEOVERRIDE = offsetof(NRF_CCM_Type, TASKS_RATEOVERRIDE), ///< Override DATARATE setting in MODE register.
#endif
} nrf_ccm_task_t;

/** @brief CCM events. */
typedef enum
{
#if NRF_CCM_HAS_EVENT_ENDKSGEN
    NRF_CCM_EVENT_ENDKSGEN = offsetof(NRF_CCM_Type, EVENTS_ENDKSGEN),  ///< Keystream generation complete.
#endif
#if NRF_CCM_HAS_EVENT_ENDCRYPT
    NRF_CCM_EVENT_END      = offsetof(NRF_CCM_Type, EVENTS_ENDCRYPT),  ///< Encrypt/decrypt complete.
#else
    NRF_CCM_EVENT_END      = offsetof(NRF_CCM_Type, EVENTS_END),       ///< Encrypt/decrypt complete.
#endif
    NRF_CCM_EVENT_ERROR    = offsetof(NRF_CCM_Type, EVENTS_ERROR),     ///< CCM error event.
} nrf_ccm_event_t;

#if NRF_CCM_HAS_EVENT_ENDKSGEN
/** @brief Types of CCM shorts. */
typedef enum
{
    NRF_CCM_SHORT_ENDKSGEN_START_MASK = CCM_SHORTS_ENDKSGEN_CRYPT_Msk, ///< Shortcut for starting encryption/decryption when the key-stream generation is complete.
} nrf_ccm_short_mask_t;
#endif // NRF_CCM_HAS_EVENT_ENDKSGEN

/** @brief CCM interrupts. */
typedef enum
{
#if NRF_CCM_HAS_EVENT_ENDKSGEN
    NRF_CCM_INT_ENDKSGEN_MASK = CCM_INTENSET_ENDKSGEN_Msk,  ///< Interrupt on ENDKSGEN event.
#endif
#if NRF_CCM_HAS_EVENT_ENDCRYPT
    NRF_CCM_INT_END_MASK      = CCM_INTENSET_ENDCRYPT_Msk,  ///< Interrupt on encrypt/decrypt complete event.
#else
    NRF_CCM_INT_END_MASK      = CCM_INTENSET_END_Msk,       ///< Interrupt on encrypt/decrypt complete event.
#endif
    NRF_CCM_INT_ERROR_MASK    = CCM_INTENSET_ERROR_Msk,     ///< Interrupt on ERROR event.
} nrf_ccm_int_mask_t;

#if NRF_CCM_HAS_ERRORSTATUS
/** @brief CCM error status when ERROR event is generated. */
typedef enum
{
    NRF_CCM_ERROR_NO_ERROR             = CCM_ERRORSTATUS_ERRORSTATUS_NoError,            ///< No errors have occurred.
    NRF_CCM_ERROR_PREMATURE_INPTR_END  = CCM_ERRORSTATUS_ERRORSTATUS_PrematureInptrEnd,  ///< End of INPTR job list before CCM data structure was read.
    NRF_CCM_ERROR_PREMATURE_OUTPTR_END = CCM_ERRORSTATUS_ERRORSTATUS_PrematureOutptrEnd, ///< End of OUTPTR job list before CCM data structure was read.
    NRF_CCM_ERROR_ENCRYPTION_TOO_SLOW  = CCM_ERRORSTATUS_ERRORSTATUS_EncryptionTooSlow,  ///< Encryption did not complete in time.
} nrf_ccm_error_t;
#endif

/** @brief CCM modes of operation. */
typedef enum
{
    NRF_CCM_MODE_ENCRYPTION      = CCM_MODE_MODE_Encryption,     ///< Encryption mode.
    NRF_CCM_MODE_DECRYPTION      = CCM_MODE_MODE_Decryption,     ///< Decryption mode.
#if NRF_CCM_HAS_MODE_FAST_DECRYPTION
    NRF_CCM_MODE_FAST_DECRYPTION = CCM_MODE_MODE_FastDecryption, ///< Fast decryption mode.
#endif
} nrf_ccm_mode_t;

#if NRF_CCM_HAS_DATARATE
/** @brief CCM data rates. */
typedef enum
{
#if NRF_CCM_HAS_MODE_DATARATE_125K
    NRF_CCM_DATARATE_125K = NRF_CCM_MODE_DATARATE_125K, ///< 125 Kbps.
#endif
#if NRF_CCM_HAS_MODE_DATARATE_250K
    NRF_CCM_DATARATE_250K = CCM_MODE_DATARATE_250Kbit,  ///< 250 Kbps.
#endif
#if NRF_CCM_HAS_MODE_DATARATE_500K
    NRF_CCM_DATARATE_500K = NRF_CCM_MODE_DATARATE_500K, ///< 500 Kbps.
#endif
    NRF_CCM_DATARATE_1M   = CCM_MODE_DATARATE_1Mbit,    ///< 1 Mbps.
    NRF_CCM_DATARATE_2M   = CCM_MODE_DATARATE_2Mbit,    ///< 2 Mbps.
#if NRF_CCM_HAS_MODE_DATARATE_4M
    NRF_CCM_DATARATE_4M   = CCM_MODE_DATARATE_4Mbit,    ///< 4 Mbps.
#endif
} nrf_ccm_datarate_t;
#endif // NRF_CCM_HAS_DATARATE

#if NRF_CCM_HAS_MODE_PROTOCOL
/** @brief CCM protocol and packet format. */
typedef enum
{
#if NRF_CCM_HAS_MODE_PROTOCOL_BLE
    NRF_CCM_MODE_PROTOCOL_BLE        = CCM_MODE_PROTOCOL_Ble,        ///< BLE packet format.
#endif
#if NRF_CCM_HAS_MODE_PROTOCOL_IEEE802154
    NRF_CCM_MODE_PROTOCOL_IEEE802154 = CCM_MODE_PROTOCOL_Ieee802154, ///< 802.15.4 packet format.
#endif
} nrf_ccm_protocol_t;
#endif // NRF_CCM_HAS_MODE_PROTOCOL

#if NRF_CCM_HAS_MODE_LENGTH
/** @brief CCM packet length options. */
typedef enum
{
    NRF_CCM_LENGTH_DEFAULT  = CCM_MODE_LENGTH_Default,  ///< Default length.
    NRF_CCM_LENGTH_EXTENDED = CCM_MODE_LENGTH_Extended, ///< Extended length.
} nrf_ccm_length_t;
#endif // NRF_CCM_HAS_MODE_LENGTH

#if NRF_CCM_HAS_MODE_MACLEN
/** @brief CCM MAC length. */
typedef enum
{
    NRF_CCM_MODE_MACLEN_M0  = CCM_MODE_MACLEN_M0,  ///< 0 bytes.
    NRF_CCM_MODE_MACLEN_M4  = CCM_MODE_MACLEN_M4,  ///< 4 bytes.
    NRF_CCM_MODE_MACLEN_M6  = CCM_MODE_MACLEN_M6,  ///< 6 bytes.
    NRF_CCM_MODE_MACLEN_M8  = CCM_MODE_MACLEN_M8,  ///< 8 bytes.
    NRF_CCM_MODE_MACLEN_M10 = CCM_MODE_MACLEN_M10, ///< 10 bytes.
    NRF_CCM_MODE_MACLEN_M12 = CCM_MODE_MACLEN_M12, ///< 12 bytes.
    NRF_CCM_MODE_MACLEN_M14 = CCM_MODE_MACLEN_M14, ///< 14 bytes.
    NRF_CCM_MODE_MACLEN_M16 = CCM_MODE_MACLEN_M16, ///< 16 bytes.
} nrf_ccm_maclen_t;
#endif // NRF_CCM_HAS_MODE_MACLEN

/** @brief CCM configuration. */
typedef struct
{
    nrf_ccm_mode_t     mode;       ///< Operation mode.
#if NRF_CCM_HAS_MODE_PROTOCOL
    nrf_ccm_protocol_t protocol;   ///< Protocol and packet format.
#endif
#if NRF_CCM_HAS_DATARATE
    nrf_ccm_datarate_t datarate;   ///< Data rate.
#endif
#if NRF_CCM_HAS_MODE_LENGTH
    nrf_ccm_length_t   length;     ///< Length of the CCM packet.
#endif
#if NRF_CCM_HAS_MODE_MACLEN
    nrf_ccm_maclen_t   mac_length; ///< Length of the CCM MAC.
#endif
} nrf_ccm_config_t;

/**
 * @brief Function for activating a specific CCM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task to be activated.
 */
NRF_STATIC_INLINE void nrf_ccm_task_trigger(NRF_CCM_Type * p_reg,
                                            nrf_ccm_task_t task);

/**
 * @brief Function for getting the address of a specific CCM task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Requested task.
 *
 * @return Address of the specified task register.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_task_address_get(NRF_CCM_Type const * p_reg,
                                                    nrf_ccm_task_t       task);

/**
 * @brief Function for clearing a specific CCM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to clear.
 */
NRF_STATIC_INLINE void nrf_ccm_event_clear(NRF_CCM_Type *  p_reg,
                                           nrf_ccm_event_t event);

/**
 * @brief Function for retrieving the state of a specific CCM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_ccm_event_check(NRF_CCM_Type const * p_reg,
                                           nrf_ccm_event_t      event);

/**
 * @brief Function for getting the address of a specific CCM event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Requested event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_event_address_get(NRF_CCM_Type const * p_reg,
                                                     nrf_ccm_event_t      event);

#if NRF_CCM_HAS_EVENT_ENDKSGEN
/**
 * @brief Function for enabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be enabled.
 */
NRF_STATIC_INLINE void nrf_ccm_shorts_enable(NRF_CCM_Type * p_reg,
                                             uint32_t       mask);

/**
 * @brief Function for disabling the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be disabled.
 */
NRF_STATIC_INLINE void nrf_ccm_shorts_disable(NRF_CCM_Type * p_reg,
                                              uint32_t       mask);

/**
 * @brief Function for setting the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Shortcuts to be set.
 */
NRF_STATIC_INLINE void nrf_ccm_shorts_set(NRF_CCM_Type * p_reg,
                                          uint32_t       mask);
#endif // NRF_CCM_HAS_EVENT_ENDKSGEN

/**
 * @brief Function for enabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_ccm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_ccm_int_enable(NRF_CCM_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_ccm_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_ccm_int_disable(NRF_CCM_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_ccm_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_int_enable_check(NRF_CCM_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for enabling the CCM peripheral.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_ccm_enable(NRF_CCM_Type * p_reg);

/**
 * @brief Function for disabling the CCM peripheral.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_ccm_disable(NRF_CCM_Type * p_reg);

/**
 * @brief Function for setting the CCM peripheral configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure with configuration to be set.
 */
NRF_STATIC_INLINE void nrf_ccm_configure(NRF_CCM_Type *           p_reg,
                                         nrf_ccm_config_t const * p_config);

#if NRF_CCM_HAS_MAXPACKETSIZE
/**
 * @brief Function for setting the length of key-stream generated
 *        when the packet length is configured as extended.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] size  Maximum length of the key-stream.
 */
NRF_STATIC_INLINE void nrf_ccm_maxpacketsize_set(NRF_CCM_Type * p_reg,
                                                 uint8_t        size);
#endif // NRF_CCM_HAS_MAXPACKETSIZE

#if NRF_CCM_HAS_MICSTATUS
/**
 * @brief Function for getting the MIC check result.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The MIC check passed.
 * @retval false The MIC check failed.
 */
NRF_STATIC_INLINE bool nrf_ccm_micstatus_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_MICSTATUS

#if NRF_CCM_HAS_MACSTATUS
/**
 * @brief Function for getting the MAC check result.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The MAC check passed.
 * @retval false The MAC check failed.
 */
NRF_STATIC_INLINE bool nrf_ccm_macstatus_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_MACSTATUS

#if NRF_CCM_HAS_ERRORSTATUS
/**
 * @brief Function for getting the error status when ERROR event is generated.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval Error status when the ERROR event is generated.
 */
NRF_STATIC_INLINE nrf_ccm_error_t nrf_ccm_errorstatus_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_ERRORSTATUS

#if NRF_CCM_HAS_CNFPTR
/**
 * @brief Function for setting the pointer to the data structure
 *        holding the AES key and the CCM NONCE vector.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Pointer to the data structure.
 */
NRF_STATIC_INLINE void nrf_ccm_cnfptr_set(NRF_CCM_Type *        p_reg,
                                          nrf_ccm_cnf_t const * p_data);

/**
 * @brief Function for getting the pointer to the data structure
 *        holding the AES key and the CCM NONCE vector.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the data structure.
 */
NRF_STATIC_INLINE nrf_ccm_cnf_t * nrf_ccm_cnfptr_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_CNFPTR

#if NRF_CCM_HAS_KEY
/**
 * @brief Function for setting the AES key.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_key Pointer to the AES 128-bit key value. The key shall be stored
 *                  in big endian byte order.
 */
NRF_STATIC_INLINE void nrf_ccm_key_set(NRF_CCM_Type   * p_reg,
                                       uint32_t const * p_key);
/**
 * @brief Function for getting the AES key.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the AES 128-bit key value. The key is stored in big endian byte order.
 */
NRF_STATIC_INLINE uint32_t const volatile * nrf_ccm_key_get(NRF_CCM_Type const * p_reg);

#endif // NRF_CCM_HAS_KEY

#if NRF_CCM_HAS_NONCE
/**
 * @brief Function for setting the AES nonce.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] p_nonce Pointer to the AES 13-byte nonce value. The nonce shall be stored
 *                    in big endian byte order.
 */
NRF_STATIC_INLINE void nrf_ccm_nonce_set(NRF_CCM_Type *   p_reg,
                                         uint32_t const * p_nonce);

/**
 * @brief Function for getting the AES nonce.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the AES 13-byte nonce value. The nonce is stored in big endian byte order.
 */
NRF_STATIC_INLINE uint32_t const volatile * nrf_ccm_nonce_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_NONCE

#if NRF_CCM_HAS_IN_AMOUNT
/**
 * @brief Function for getting number of bytes read from the input data,
 *        not including the job list structure.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes read from the input data.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_in_amount_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_IN_AMOUNT

#if NRF_CCM_HAS_INPTR
/**
 * @brief Function for setting the input data pointer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Input data pointer.
 */
NRF_STATIC_INLINE void nrf_ccm_inptr_set(NRF_CCM_Type *   p_reg,
                                         uint32_t const * p_data);

/**
 * @brief Function for getting the input data pointer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Input data pointer.
 */
NRF_STATIC_INLINE uint32_t * nrf_ccm_inptr_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_INPTR

#if NRF_CCM_HAS_IN_PTR
/**
 * @brief Function for setting the pointer to a job list containing unencrypted
 *        CCM data structure in Encryption mode or encrypted CCM data structure
 *        in Decryption mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_job Pointer to a job list.
 */
NRF_STATIC_INLINE void nrf_ccm_in_ptr_set(NRF_CCM_Type *         p_reg,
                                          nrf_vdma_job_t const * p_job);

/**
 * @brief Function for getting the pointer to job list containing unencrypted
 *        CCM data structure in Encryption mode or encrypted CCM data structure
 *        in Decryption mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to a job list.
 */
NRF_STATIC_INLINE nrf_vdma_job_t * nrf_ccm_in_ptr_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_IN_PTR

#if NRF_CCM_HAS_OUTPTR
/**
 * @brief Function for setting the output data pointer.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_data Output data pointer.
 */
NRF_STATIC_INLINE void nrf_ccm_outptr_set(NRF_CCM_Type *   p_reg,
                                          uint32_t const * p_data);

/**
 * @brief Function for getting the output data pointer.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Output data pointer.
 */
NRF_STATIC_INLINE uint32_t * nrf_ccm_outptr_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_OUTPTR

#if NRF_CCM_HAS_OUT_PTR
/**
 * @brief Function for setting the pointer to a job list containing encrypted
 *        CCM data structure in Encryption mode or decrypted CCM data structure
 *        in Decryption mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] p_job Pointer to a job list.
 */
NRF_STATIC_INLINE void nrf_ccm_out_ptr_set(NRF_CCM_Type *         p_reg,
                                           nrf_vdma_job_t const * p_job);

/**
 * @brief Function for getting the pointer to a job list containing encrypted
 *        CCM data structure in Encryption mode or decrypted CCM data structure
 *        in Decryption mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the job list.
 */
NRF_STATIC_INLINE nrf_vdma_job_t * nrf_ccm_out_ptr_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_OUT_PTR

#if NRF_CCM_HAS_OUT_AMOUNT
/**
 * @brief Function for getting number of bytes available in the output data,
 *        not including the job list structure.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Number of bytes available in the output data.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_out_amount_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_OUT_AMOUNT

#if NRF_CCM_HAS_SCRATCHPTR
/**
 * @brief Function for setting the pointer to the scratch area used for
 *        temporary storage.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] p_area Pointer to the scratch area.
 */
NRF_STATIC_INLINE void nrf_ccm_scratchptr_set(NRF_CCM_Type *   p_reg,
                                              uint32_t const * p_area);

/**
 * @brief Function for getting the pointer to the scratch area.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Pointer to the scratch area.
 */
NRF_STATIC_INLINE uint32_t * nrf_ccm_scratchptr_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_SCRATCHPTR

#if NRF_CCM_HAS_RATEOVERRIDE
/**
 * @brief Function for setting the data rate override value.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] datarate Override value to be applied when the RATEOVERRIDE task
 *                     is triggered.
 */
NRF_STATIC_INLINE void nrf_ccm_datarate_override_set(NRF_CCM_Type *     p_reg,
                                                     nrf_ccm_datarate_t datarate);

/**
 * @brief Function for getting data override setting.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Data override setting.
 */
NRF_STATIC_INLINE nrf_ccm_datarate_t nrf_ccm_datarate_override_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_RATEOVERRIDE

#if NRF_CCM_HAS_ADATAMASK
/**
 * @brief Function for setting the CCM adata mask.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] adata_msk CCM adata mask.
 */
NRF_STATIC_INLINE void nrf_ccm_adatamask_set(NRF_CCM_Type * p_reg,
                                             uint8_t        adata_msk);

/**
 * @brief Function for getting bitmask for the first adata byte.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return CCM adata mask.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_adatamask_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_ADATAMASK

#if NRF_CCM_HAS_HEADERMASK
/**
 * @brief Function for setting the CCM header mask.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] header_msk CCM header mask.
 */
NRF_STATIC_INLINE void nrf_ccm_headermask_set(NRF_CCM_Type * p_reg,
                                              uint8_t        header_msk);

/**
 * @brief Function for getting the bitmask for packet header (S0) before MIC
 *        generation/authentication.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return CCM header mask.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_headermask_get(NRF_CCM_Type const * p_reg);
#endif // NRF_CCM_HAS_HEADERMASK

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        CCM task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_ccm_subscribe_set(NRF_CCM_Type * p_reg,
                                             nrf_ccm_task_t task,
                                             uint8_t        channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        CCM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_ccm_subscribe_clear(NRF_CCM_Type * p_reg,
                                               nrf_ccm_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        CCM task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return CCM subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_subscribe_get(NRF_CCM_Type const * p_reg,
                                                 nrf_ccm_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        CCM event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_ccm_publish_set(NRF_CCM_Type *  p_reg,
                                           nrf_ccm_event_t event,
                                           uint8_t         channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        CCM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_ccm_publish_clear(NRF_CCM_Type *  p_reg,
                                             nrf_ccm_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        CCM event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return CCM publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_ccm_publish_get(NRF_CCM_Type const * p_reg,
                                               nrf_ccm_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_ccm_task_trigger(NRF_CCM_Type * p_reg,
                                            nrf_ccm_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_ccm_task_address_get(NRF_CCM_Type const * p_reg,
                                                    nrf_ccm_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE void nrf_ccm_event_clear(NRF_CCM_Type *  p_reg,
                                           nrf_ccm_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_ccm_event_check(NRF_CCM_Type const * p_reg,
                                           nrf_ccm_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_ccm_event_address_get(NRF_CCM_Type const * p_reg,
                                                     nrf_ccm_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

#if NRF_CCM_HAS_EVENT_ENDKSGEN
NRF_STATIC_INLINE void nrf_ccm_shorts_enable(NRF_CCM_Type * p_reg,
                                             uint32_t       mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_ccm_shorts_disable(NRF_CCM_Type * p_reg,
                                              uint32_t       mask)
{
    p_reg->SHORTS &= ~(mask);
}

NRF_STATIC_INLINE void nrf_ccm_shorts_set(NRF_CCM_Type * p_reg,
                                          uint32_t       mask)
{
    p_reg->SHORTS = mask;
}
#endif // NRF_CCM_HAS_EVENT_ENDKSGEN

NRF_STATIC_INLINE void nrf_ccm_int_enable(NRF_CCM_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_ccm_int_disable(NRF_CCM_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_ccm_int_enable_check(NRF_CCM_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE void nrf_ccm_enable(NRF_CCM_Type * p_reg)
{
    p_reg->ENABLE = (CCM_ENABLE_ENABLE_Enabled << CCM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_ccm_disable(NRF_CCM_Type * p_reg)
{
    p_reg->ENABLE = (CCM_ENABLE_ENABLE_Disabled << CCM_ENABLE_ENABLE_Pos);
}

NRF_STATIC_INLINE void nrf_ccm_configure(NRF_CCM_Type *           p_reg,
                                         nrf_ccm_config_t const * p_config)
{
    p_reg->MODE = (((uint32_t)p_config->mode       << CCM_MODE_MODE_Pos)     |
#if NRF_CCM_HAS_MODE_PROTOCOL
                   ((uint32_t)p_config->protocol   << CCM_MODE_PROTOCOL_Pos) |
#endif
#if NRF_CCM_HAS_DATARATE
                   ((uint32_t)p_config->datarate   << CCM_MODE_DATARATE_Pos) |
#endif
#if NRF_CCM_HAS_MODE_LENGTH
                   ((uint32_t)p_config->length     << CCM_MODE_LENGTH_Pos)   |
#endif
#if NRF_CCM_HAS_MODE_MACLEN
                   ((uint32_t)p_config->mac_length << CCM_MODE_MACLEN_Pos)   |
#endif
                   0);
}

#if NRF_CCM_HAS_MAXPACKETSIZE
NRF_STATIC_INLINE void nrf_ccm_maxpacketsize_set(NRF_CCM_Type * p_reg,
                                                 uint8_t        size)
{
    NRFX_ASSERT((size >= 0x1B) && (size <= 0xFB));

    p_reg->MAXPACKETSIZE = size;
}
#endif // defined(NRF_CCM_HAS_MAXPACKETSIZE)

#if NRF_CCM_HAS_MICSTATUS
NRF_STATIC_INLINE bool nrf_ccm_micstatus_get(NRF_CCM_Type const * p_reg)
{
    return (bool)(p_reg->MICSTATUS);
}
#endif // NRF_CCM_HAS_MICSTATUS

#if NRF_CCM_HAS_MACSTATUS
NRF_STATIC_INLINE bool nrf_ccm_macstatus_get(NRF_CCM_Type const * p_reg)
{
    return (bool)(p_reg->MACSTATUS);
}
#endif // NRF_CCM_HAS_MACSTATUS

#if NRF_CCM_HAS_ERRORSTATUS
NRF_STATIC_INLINE nrf_ccm_error_t nrf_ccm_errorstatus_get(NRF_CCM_Type const * p_reg)
{
    return (nrf_ccm_error_t)(p_reg->ERRORSTATUS);
}
#endif // NRF_CCM_HAS_ERRORSTATUS

#if NRF_CCM_HAS_CNFPTR
NRF_STATIC_INLINE void nrf_ccm_cnfptr_set(NRF_CCM_Type *        p_reg,
                                          nrf_ccm_cnf_t const * p_data)
{
    p_reg->CNFPTR = (uint32_t)p_data;
}

NRF_STATIC_INLINE nrf_ccm_cnf_t * nrf_ccm_cnfptr_get(NRF_CCM_Type const * p_reg)
{
    return (nrf_ccm_cnf_t *)(p_reg->CNFPTR);
}
#endif // NRF_CCM_HAS_CNFPTR

#if NRF_CCM_HAS_KEY
NRF_STATIC_INLINE void nrf_ccm_key_set(NRF_CCM_Type   * p_reg,
                                       uint32_t const * p_key)
{
    NRFX_ASSERT(p_key);
    for (uint8_t i = 0; i < CCM_KEY_VALUE_MaxCount; i++)
    {
        p_reg->KEY.VALUE[i] = p_key[i];
    }
}

NRF_STATIC_INLINE uint32_t const volatile * nrf_ccm_key_get(NRF_CCM_Type const * p_reg)
{
    return (uint32_t const volatile *)(p_reg->KEY.VALUE);
}
#endif // NRF_CCM_HAS_KEY

#if NRF_CCM_HAS_NONCE
NRF_STATIC_INLINE void nrf_ccm_nonce_set(NRF_CCM_Type *   p_reg,
                                         uint32_t const * p_nonce)
{
    NRFX_ASSERT(p_nonce);
    for (uint8_t i = 0; i < CCM_NONCE_VALUE_MaxCount; i++)
    {
        p_reg->NONCE.VALUE[i] = p_nonce[i];
    }
}

NRF_STATIC_INLINE uint32_t const volatile * nrf_ccm_nonce_get(NRF_CCM_Type const * p_reg)
{
    return (uint32_t const volatile *)(p_reg->NONCE.VALUE);
}
#endif // NRF_CCM_HAS_NONCE

#if NRF_CCM_HAS_IN_AMOUNT
NRF_STATIC_INLINE uint32_t nrf_ccm_in_amount_get(NRF_CCM_Type const * p_reg)
{
    return p_reg->IN.AMOUNT;
}
#endif // NRF_CCM_HAS_IN_AMOUNT

#if NRF_CCM_HAS_OUT_AMOUNT
NRF_STATIC_INLINE uint32_t nrf_ccm_out_amount_get(NRF_CCM_Type const * p_reg)
{
    return p_reg->OUT.AMOUNT;
}
#endif // NRF_CCM_HAS_OUT_AMOUNT

#if NRF_CCM_HAS_INPTR
NRF_STATIC_INLINE void nrf_ccm_inptr_set(NRF_CCM_Type *   p_reg,
                                         uint32_t const * p_data)
{
    p_reg->INPTR = (uint32_t)p_data;
}

NRF_STATIC_INLINE uint32_t * nrf_ccm_inptr_get(NRF_CCM_Type const * p_reg)
{
    return (uint32_t *)(p_reg->INPTR);
}
#endif // NRF_CCM_HAS_INPTR

#if NRF_CCM_HAS_IN_PTR
NRF_STATIC_INLINE void nrf_ccm_in_ptr_set(NRF_CCM_Type *         p_reg,
                                          nrf_vdma_job_t const * p_job)
{
    p_reg->IN.PTR = (uint32_t)p_job;
}

NRF_STATIC_INLINE nrf_vdma_job_t * nrf_ccm_in_ptr_get(NRF_CCM_Type const * p_reg)
{
    return (nrf_vdma_job_t *)(p_reg->IN.PTR);
}
#endif // NRF_CCM_HAS_IN_PTR

#if NRF_CCM_HAS_OUTPTR
NRF_STATIC_INLINE void nrf_ccm_outptr_set(NRF_CCM_Type *   p_reg,
                                          uint32_t const * p_data)
{
    p_reg->OUTPTR = (uint32_t)p_data;
}

NRF_STATIC_INLINE uint32_t * nrf_ccm_outptr_get(NRF_CCM_Type const * p_reg)
{
    return (uint32_t *)(p_reg->OUTPTR);
}
#endif // NRF_CCM_HAS_OUTPTR

#if NRF_CCM_HAS_OUT_PTR
NRF_STATIC_INLINE void nrf_ccm_out_ptr_set(NRF_CCM_Type *         p_reg,
                                           nrf_vdma_job_t const * p_job)
{
    p_reg->OUT.PTR = (uint32_t)p_job;
}

NRF_STATIC_INLINE nrf_vdma_job_t * nrf_ccm_out_ptr_get(NRF_CCM_Type const * p_reg)
{
    return (nrf_vdma_job_t *)(p_reg->OUT.PTR);
}
#endif // NRF_CCM_HAS_OUT_PTR

#if NRF_CCM_HAS_SCRATCHPTR
NRF_STATIC_INLINE void nrf_ccm_scratchptr_set(NRF_CCM_Type *   p_reg,
                                              uint32_t const * p_area)
{
    p_reg->SCRATCHPTR = (uint32_t)p_area;
}

NRF_STATIC_INLINE uint32_t * nrf_ccm_scratchptr_get(NRF_CCM_Type const * p_reg)
{
    return (uint32_t *)(p_reg->SCRATCHPTR);
}
#endif // NRF_CCM_HAS_SCRATCHPTR

#if NRF_CCM_HAS_RATEOVERRIDE
NRF_STATIC_INLINE void nrf_ccm_datarate_override_set(NRF_CCM_Type *     p_reg,
                                                     nrf_ccm_datarate_t datarate)
{
    p_reg->RATEOVERRIDE = ((uint32_t)datarate << CCM_RATEOVERRIDE_RATEOVERRIDE_Pos);
}

NRF_STATIC_INLINE nrf_ccm_datarate_t nrf_ccm_datarate_override_get(NRF_CCM_Type const * p_reg)
{
    return (nrf_ccm_datarate_t)(p_reg->RATEOVERRIDE);
}
#endif // NRF_CCM_HAS_RATEOVERRIDE

#if NRF_CCM_HAS_ADATAMASK
NRF_STATIC_INLINE void nrf_ccm_adatamask_set(NRF_CCM_Type * p_reg,
                                             uint8_t        adata_msk)
{
    p_reg->ADATAMASK = (uint32_t)adata_msk;
}

NRF_STATIC_INLINE uint32_t nrf_ccm_adatamask_get(NRF_CCM_Type const * p_reg)
{
    return (uint32_t)(p_reg->ADATAMASK);
}
#endif // NRF_CCM_HAS_ADATAMASK

#if NRF_CCM_HAS_HEADERMASK
NRF_STATIC_INLINE void nrf_ccm_headermask_set(NRF_CCM_Type * p_reg,
                                              uint8_t        header_msk)
{
    p_reg->HEADERMASK = (uint32_t)header_msk;
}

NRF_STATIC_INLINE uint32_t nrf_ccm_headermask_get(NRF_CCM_Type const * p_reg)
{
    return (uint32_t)(p_reg->HEADERMASK);
}
#endif

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_ccm_subscribe_set(NRF_CCM_Type * p_reg,
                                             nrf_ccm_task_t task,
                                             uint8_t        channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_ccm_subscribe_clear(NRF_CCM_Type * p_reg,
                                               nrf_ccm_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_ccm_subscribe_get(NRF_CCM_Type const * p_reg,
                                                 nrf_ccm_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_ccm_publish_set(NRF_CCM_Type *  p_reg,
                                           nrf_ccm_event_t event,
                                           uint8_t         channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_ccm_publish_clear(NRF_CCM_Type *  p_reg,
                                             nrf_ccm_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_ccm_publish_get(NRF_CCM_Type const * p_reg,
                                               nrf_ccm_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif  // NRF_CCM_H__
