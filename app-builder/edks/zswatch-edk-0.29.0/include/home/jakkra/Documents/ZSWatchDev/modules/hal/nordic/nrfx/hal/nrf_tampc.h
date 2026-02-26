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

#ifndef NRF_TAMPC_H_
#define NRF_TAMPC_H_

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_tampc_hal TAMPC HAL
 * @{
 * @ingroup nrf_tampc
 * @brief   Hardware access layer for managing the Tamper Controller (TAMPC)
 *          peripheral.
 */

#if defined(TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Protect register write key mask. */
#define NRF_TAMPC_KEY_MASK (TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_KEY \
                            << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_KEY_Pos)
#else
#define NRF_TAMPC_KEY_MASK 0
#endif

#if defined(TAMPC_EVENTS_WRITEERROR_EVENTS_WRITEERROR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TAMPC write error event is present. */
#define NRF_TAMPC_HAS_EVENT_WRITE_ERROR 1
#else
#define NRF_TAMPC_HAS_EVENT_WRITE_ERROR 0
#endif

#if defined(TAMPC_ACTIVESHIELD_CHEN_CH0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TAMPC active shield channels are present. */
#define NRF_TAMPC_HAS_ACTIVE_SHIELD_CHANNELS 1
#else
#define NRF_TAMPC_HAS_ACTIVE_SHIELD_CHANNELS 0
#endif

#if defined(TAMPC_PROTECT_ACTIVESHIELD_CTRL_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TAMPC extended protection is present. */
#define NRF_TAMPC_HAS_EXTENDED_PROTECTORS 1
#else
#define NRF_TAMPC_HAS_EXTENDED_PROTECTORS 0
#endif

#if defined(TAMPC_PROTECT_ERASEPROTECT_CTRL_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TAMPC erase protector is present. */
#define NRF_TAMPC_HAS_ERASE_PROTECTOR 1
#else
#define NRF_TAMPC_HAS_ERASE_PROTECTOR 0
#endif

#if defined(TAMPC_PROTECT_TAMPERSWITCH_CTRL_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TAMPC external tamper switch protector is present. */
#define NRF_TAMPC_HAS_EXTERNAL_TAMPERSWITCH_PROTECTOR 1
#else
#define NRF_TAMPC_HAS_EXTERNAL_TAMPERSWITCH_PROTECTOR 0
#endif

#if defined(TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether domain secure priviliged invasive debug detector is present. */
#define NRF_TAMPC_HAS_DOMAIN_SPIDEN_PROTECTOR 1
#else
#define NRF_TAMPC_HAS_DOMAIN_SPIDEN_PROTECTOR 0
#endif

#if defined(TAMPC_PROTECT_AP_SPIDEN_CTRL_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether AP secure priviliged invasive debug detector is present. */
#define NRF_TAMPC_HAS_AP_SPIDEN_PROTECTOR 1
#else
#define NRF_TAMPC_HAS_AP_SPIDEN_PROTECTOR 0
#endif

#if defined(TAMPC_ENABLE_ACTIVESHIELD_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the availability to enable the TAMPC detectors feature is present. */
#define NRF_TAMPC_HAS_DETECTORS_ENABLE 1
#else
#define NRF_TAMPC_HAS_DETECTORS_ENABLE 0
#endif

#if defined(TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_VALUE_High) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the configuration of Coresight debugger signals protection is present. */
#define NRF_TAMPC_HAS_CORESIGHT 1
#else
#define NRF_TAMPC_HAS_CORESIGHT 0
#endif

#if defined(TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_VALUE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the configuration of warm boot protection is present. */
#define NRF_TAMPC_HAS_WARMBOOT 1
#else
#define NRF_TAMPC_HAS_WARMBOOT 0
#endif

#if defined(TAMPC_STATUS_ACTIVESHIELD_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TAMPC active shield detector is present. */
#define NRF_TAMPC_HAS_ACTIVE_SHIELD_DETECTOR 1
#else
#define NRF_TAMPC_HAS_ACTIVE_SHIELD_DETECTOR 0
#endif

#if defined(TAMPC_STATUS_TAMPERSWITCH_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TAMPC external tamper switch detector is present. */
#define NRF_TAMPC_HAS_EXTERNAL_TAMPERSWITCH_DETECTOR 1
#else
#define NRF_TAMPC_HAS_EXTERNAL_TAMPERSWITCH_DETECTOR 0
#endif

#if defined(NRF_TAMPC_DETECTOR_CRACEN) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TAMPC has CRACEN detection features. */
#define NRF_TAMPC_HAS_CRACEN 1
#else
#define NRF_TAMPC_HAS_CRACEN 0
#endif

#if NRF_TAMPC_HAS_ACTIVE_SHIELD_CHANNELS
#if defined(NRF_TAMPC_ACTIVESHIELD_CHANNEL_3_MASK) || defined(__NRFX_DOXYGEN__)
/** @brief Number of active shield channels. */
#define NRF_TAMPC_ACTIVESHIELD_CHANNEL_COUNT 4
#elif defined(NRF_TAMPC_ACTIVESHIELD_CHANNEL_2_MASK)
#define NRF_TAMPC_ACTIVESHIELD_CHANNEL_COUNT 3
#elif defined(NRF_TAMPC_ACTIVESHIELD_CHANNEL_1_MASK)
#define NRF_TAMPC_ACTIVESHIELD_CHANNEL_COUNT 2
#else
#define NRF_TAMPC_ACTIVESHIELD_CHANNEL_COUNT 1
#endif
#endif

/** @brief TAMPC events. */
typedef enum
{
    NRF_TAMPC_EVENT_TAMPER      = offsetof(NRF_TAMPC_Type, EVENTS_TAMPER),    ///< TAMPC detected an error.
#if NRF_TAMPC_HAS_EVENT_WRITE_ERROR
    NRF_TAMPC_EVENT_WRITE_ERROR = offsetof(NRF_TAMPC_Type, EVENTS_WRITEERROR) ///< Attempted to write a VALUE in PROTECT registers without clearing the WRITEPROTECT.
#endif
} nrf_tampc_event_t;

/** @brief TAMPC interrupts. */
typedef enum
{
    NRF_TAMPC_INT_TAMPER_MASK      = TAMPC_INTENSET_TAMPER_Msk,     ///< Interrupt on TAMPER event.
#if NRF_TAMPC_HAS_EVENT_WRITE_ERROR
    NRF_TAMPC_INT_WRITE_ERROR_MASK = TAMPC_INTENSET_WRITEERROR_Msk, ///< Interrupt on WRITEERROR event.
#endif
    NRF_TAMPC_ALL_INTS_MASK        = NRF_TAMPC_INT_TAMPER_MASK
#if NRF_TAMPC_HAS_EVENT_WRITE_ERROR
                                   | NRF_TAMPC_INT_WRITE_ERROR_MASK ///< All TAMPC interrupts.
#endif
} nrf_tapmc_int_mask_t;

#if NRF_TAMPC_HAS_ACTIVE_SHIELD_CHANNELS
/** @brief Active shield channel mask. */
typedef enum
{
    NRF_TAMPC_ACTIVESHIELD_CHANNEL_0_MASK    = TAMPC_ACTIVESHIELD_CHEN_CH0_Msk,      ///< Enable active shield channel 0.
    NRF_TAMPC_ACTIVESHIELD_CHANNEL_1_MASK    = TAMPC_ACTIVESHIELD_CHEN_CH1_Msk,      ///< Enable active shield channel 1.
#if NRF_TAMPC_ACTIVESHIELD_CHANNEL_COUNT > 2
    NRF_TAMPC_ACTIVESHIELD_CHANNEL_2_MASK    = TAMPC_ACTIVESHIELD_CHEN_CH2_Msk,      ///< Enable active shield channel 2.
    NRF_TAMPC_ACTIVESHIELD_CHANNEL_3_MASK    = TAMPC_ACTIVESHIELD_CHEN_CH3_Msk,      ///< Enable active shield channel 3.
#endif
    NRF_TAMPC_ALL_ACTIVESHIELD_CHANNELS_MASK = NRF_TAMPC_ACTIVESHIELD_CHANNEL_0_MASK
                                             | NRF_TAMPC_ACTIVESHIELD_CHANNEL_1_MASK
#if NRF_TAMPC_ACTIVESHIELD_CHANNEL_COUNT > 2
                                             | NRF_TAMPC_ACTIVESHIELD_CHANNEL_2_MASK
                                             | NRF_TAMPC_ACTIVESHIELD_CHANNEL_3_MASK ///< All TAMPC active shield channels.
#endif
} nrf_tampc_activeshield_mask_t;
#endif

/** @brief TAMPC error detectors. */
typedef enum
{
#if NRF_TAMPC_HAS_ACTIVE_SHIELD_DETECTOR
    NRF_TAMPC_DETECTOR_ACTIVE_SHIELD      = TAMPC_STATUS_ACTIVESHIELD_Msk,       ///< Active shield error detector.
#endif
#if NRF_TAMPC_HAS_EXTERNAL_TAMPERSWITCH_DETECTOR
    NRF_TAMPC_DETECTOR_TAMPER_SWITCH      = TAMPC_STATUS_TAMPERSWITCH_Msk,       ///< External tamper switch error detector.
#endif
    NRF_TAMPC_DETECTOR_PROTECTED_SIGNAL   = TAMPC_STATUS_PROTECT_Msk,            ///< Protected signals error detector.
#if NRF_TAMPC_HAS_CRACEN
    NRF_TAMPC_DETECTOR_CRACEN             = TAMPC_STATUS_CRACENTAMP_Msk,         ///< CRACEN error detector.
#endif
    NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_SLOW = TAMPC_STATUS_GLITCHSLOWDOMAIN0_Msk,  ///< Slow domain glitch error detector.
    NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_FAST = TAMPC_STATUS_GLITCHFASTDOMAIN0_Msk | ///< Fast domain glitch error detector.
                                            TAMPC_STATUS_GLITCHFASTDOMAIN1_Msk |
                                            TAMPC_STATUS_GLITCHFASTDOMAIN2_Msk |
                                            TAMPC_STATUS_GLITCHFASTDOMAIN3_Msk
} nrf_tampc_detector_t;

#if NRF_TAMPC_HAS_EXTENDED_PROTECTORS
/** @brief Signal protector registers. */
typedef enum
{
    NRF_TAMPC_PROTECT_ACTIVE_SHIELD      = offsetof(NRF_TAMPC_Type, PROTECT.ACTIVESHIELD),     ///< Control register for active shield detector enable signal.
#if NRF_TAMPC_HAS_EXTERNAL_TAMPERSWITCH_PROTECTOR
    NRF_TAMPC_PROTECT_TAMPER_SWITCH      = offsetof(NRF_TAMPC_Type, PROTECT.TAMPERSWITCH),     ///< Control register for external tamper switch enable signal.
#endif
#if NRF_TAMPC_HAS_CRACEN
    NRF_TAMPC_PROTECT_CRACEN             = offsetof(NRF_TAMPC_Type, PROTECT.CRACENTAMP),       ///< Control register for CRACEN tamper detector enable signal.
#endif
    NRF_TAMPC_PROTECT_GLITCH_DOMAIN_SLOW = offsetof(NRF_TAMPC_Type, PROTECT.GLITCHSLOWDOMAIN), ///< Control register for slow domain glitch detectors enable signal.
    NRF_TAMPC_PROTECT_GLITCH_DOMAIN_FAST = offsetof(NRF_TAMPC_Type, PROTECT.GLITCHFASTDOMAIN), ///< Control register for fast domain glitch detectors enable signal.
    NRF_TAMPC_PROTECT_RESETEN_EXT        = offsetof(NRF_TAMPC_Type, PROTECT.EXTRESETEN),       ///< Control register for external tamper reset enable signal.
    NRF_TAMPC_PROTECT_RESETEN_INT        = offsetof(NRF_TAMPC_Type, PROTECT.INTRESETEN),       ///< Control register for internal tamper reset enable signal.
#if NRF_TAMPC_HAS_ERASE_PROTECTOR
    NRF_TAMPC_PROTECT_ERASE_PROTECT      = offsetof(NRF_TAMPC_Type, PROTECT.ERASEPROTECT),     ///< Control register for erase protection.
#endif
} nrf_tampc_protect_t;
#endif

/** @brief Control register debug types. */
typedef enum
{
    NRF_TAMPC_DEBUG_TYPE_DBGEN,    ///< Invasive (halting) debug.
    NRF_TAMPC_DEBUG_TYPE_NIDEN,    ///< Non-invasive debug.
#if NRF_TAMPC_HAS_DOMAIN_SPIDEN_PROTECTOR
    NRF_TAMPC_DEBUG_TYPE_SPIDEN,   ///< Secure privileged invasive (halting) debug.
    NRF_TAMPC_DEBUG_TYPE_SPNIDEN,  ///< Secure privileged non-invasive debug.
#endif
    NRF_TAMPC_DEBUG_TYPE_DEVICEEN, ///< Domain circuitry.
} nrf_tampc_debug_type_t;

/** @brief Warm boot control register mode types. */
typedef enum
{
    NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE, ///< Unretained idle mode.
    NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF,  ///< System off mode.
} nrf_tampc_warmboot_mode_t;

/**
 * @brief Function for clearing the specified TAMPC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be cleared.
 */
NRF_STATIC_INLINE void nrf_tampc_event_clear(NRF_TAMPC_Type * p_reg, nrf_tampc_event_t event);

/**
 * @brief Function for retrieving the state of the TAMPC event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_tampc_event_check(NRF_TAMPC_Type const * p_reg, nrf_tampc_event_t event);

/**
 * @brief Function for getting the address of the specified TAMPC event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event The specified event.
 *
 * @return Address of the specified event register.
 */
NRF_STATIC_INLINE uint32_t nrf_tampc_event_address_get(NRF_TAMPC_Type const * p_reg,
                                                      nrf_tampc_event_t       event);

/**
 * @brief Function for enabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_tapmc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_tampc_int_enable(NRF_TAMPC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_tapmc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_tampc_int_disable(NRF_TAMPC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_tapmc_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_tampc_int_enable_check(NRF_TAMPC_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the state of pending interrupts.
 *
 * @note States of pending interrupt are saved as a bitmask.
 *       One set at particular position means that interrupt for event is pending.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Bitmask with information about pending interrupts.
 *         Use @ref nrf_tapmc_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE uint32_t nrf_tampc_int_pending_get(NRF_TAMPC_Type const * p_reg);

/**
 * @brief Function for getting the error detection status for given error detector.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] detector Error detector for which the error status is to be retrieved.
 *
 * @retval true  Error detected.
 * @retval false No error detected.
 */
NRF_STATIC_INLINE bool nrf_tampc_detector_status_check(NRF_TAMPC_Type const * p_reg,
                                                       nrf_tampc_detector_t   detector);

/**
 * @brief Function for clearing the error detection status for given error detector.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] detector Error detector for which the error status is to be cleared.
 */
NRF_STATIC_INLINE void nrf_tampc_detector_status_clear(NRF_TAMPC_Type *     p_reg,
                                                       nrf_tampc_detector_t detector);

#if NRF_TAMPC_HAS_ACTIVE_SHIELD_CHANNELS
/**
 * @brief Function for enabling the specified active shield detector channels.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of active shield detector channels to be enabled,
 *                  constructed from @ref nrf_tampc_activeshield_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_tampc_activeshield_channel_enable(NRF_TAMPC_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified active shield detector channels.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of active shield detector channels to be disabled,
 *                  constructed from @ref nrf_tampc_activeshield_mask_t enumerator values.
 */
NRF_STATIC_INLINE void nrf_tampc_activeshield_channel_disable(NRF_TAMPC_Type * p_reg,
                                                              uint32_t         mask);

/**
 * @brief Function for checking if the specified active shield detector channels are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of active shield detector channels to be checked,
 *                  constructed from @ref nrf_tampc_activeshield_mask_t enumerator values.
 *
 * @return Mask of enabled active shield detector channels.
 */
NRF_STATIC_INLINE uint32_t nrf_tampc_activeshield_channel_enable_check(NRF_TAMPC_Type const * p_reg,
                                                                       uint32_t               mask);
#endif

/**
 * @brief Function for setting signal value of the domain control register for
 *        given debug type and domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be modified.
 * @param[in] domain Domain for which the value is to be modified.
 * @param[in] enable True if signal is to be logic 1, false if logic 0.
 */
NRF_STATIC_INLINE void nrf_tampc_domain_ctrl_value_set(NRF_TAMPC_Type *       p_reg,
                                                       nrf_tampc_debug_type_t type,
                                                       nrf_domain_t           domain,
                                                       bool                   enable);

/**
 * @brief Function for getting the signal value of the domain control register for
 *        given debug type and domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be retrieved.
 * @param[in] domain Domain for which the value is to be retrieved.
 *
 * @retval true  Signal is logic 1.
 * @retval false Signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_tampc_domain_ctrl_value_get(NRF_TAMPC_Type const * p_reg,
                                                       nrf_tampc_debug_type_t type,
                                                       nrf_domain_t           domain);

/**
 * @brief Function for setting lock value of the domain control register for
 *        given debug type and domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be modified.
 * @param[in] domain Domain for which the value is to be modified.
 * @param[in] enable True if register is to be locked, false otherwise.
 */
NRF_STATIC_INLINE void nrf_tampc_domain_ctrl_lock_set(NRF_TAMPC_Type *       p_reg,
                                                      nrf_tampc_debug_type_t type,
                                                      nrf_domain_t           domain,
                                                      bool                   enable);

/**
 * @brief Function for getting the lock value of the domain control register for
 *        given debug type and domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be retrieved.
 * @param[in] domain Domain for which the value is to be retrieved.
 *
 * @retval true  Register is locked.
 * @retval false Register is unlocked.
 */
NRF_STATIC_INLINE bool nrf_tampc_domain_ctrl_lock_get(NRF_TAMPC_Type const * p_reg,
                                                      nrf_tampc_debug_type_t type,
                                                      nrf_domain_t           domain);

/**
 * @brief Function for setting signal value of the access port control register for
 *        given debug type and domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be modified.
 * @param[in] domain Domain for which the value is to be modified.
 * @param[in] enable True if signal is to be logic 1, false if logic 0.
 */
NRF_STATIC_INLINE void nrf_tampc_ap_ctrl_value_set(NRF_TAMPC_Type *       p_reg,
                                                   nrf_tampc_debug_type_t type,
                                                   nrf_domain_t           domain,
                                                   bool                   enable);

/**
 * @brief Function for getting the signal value of the access port control register for
 *        given debug type and domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be retrieved.
 * @param[in] domain Domain for which the value is to be retrieved.
 *
 * @retval true  Signal is logic 1.
 * @retval false Signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_tampc_ap_ctrl_value_get(NRF_TAMPC_Type const * p_reg,
                                                   nrf_tampc_debug_type_t type,
                                                   nrf_domain_t           domain);

/**
 * @brief Function for setting lock value of the access port control register for
 *        given debug type and domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be modified.
 * @param[in] domain Domain for which the value is to be modified.
 * @param[in] enable True if register is to be locked, false otherwise.
 */
NRF_STATIC_INLINE void nrf_tampc_ap_ctrl_lock_set(NRF_TAMPC_Type *       p_reg,
                                                  nrf_tampc_debug_type_t type,
                                                  nrf_domain_t           domain,
                                                  bool                   enable);

/**
 * @brief Function for getting the lock value of the access port control register for
 *        given debug type and domain.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be retrieved.
 * @param[in] domain Domain for which the value is to be retrieved.
 *
 * @retval true  Register is locked.
 * @retval false Register is unlocked.
 */
NRF_STATIC_INLINE bool nrf_tampc_ap_ctrl_lock_get(NRF_TAMPC_Type const * p_reg,
                                                  nrf_tampc_debug_type_t type,
                                                  nrf_domain_t           domain);

#if NRF_TAMPC_HAS_CORESIGHT
/**
 * @brief Function for setting signal value of the Coresight register for given debug type.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be modified.
 * @param[in] enable True if signal is to be logic 1, false if logic 0.
 */
NRF_STATIC_INLINE void nrf_tampc_coresight_ctrl_value_set(NRF_TAMPC_Type *       p_reg,
                                                          nrf_tampc_debug_type_t type,
                                                          bool                   enable);

/**
 * @brief Function for getting the signal value of the Coresight register for given debug type.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] type  Debug type to be retrieved.
 *
 * @retval true  Signal is logic 1.
 * @retval false Signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_tampc_coresight_ctrl_value_get(NRF_TAMPC_Type const * p_reg,
                                                          nrf_tampc_debug_type_t type);

/**
 * @brief Function for setting lock value of the Coresight register for given debug type.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be modified.
 * @param[in] enable True if register is to be locked, false otherwise.
 */
NRF_STATIC_INLINE void nrf_tampc_coresight_ctrl_lock_set(NRF_TAMPC_Type *       p_reg,
                                                         nrf_tampc_debug_type_t type,
                                                         bool                   enable);

/**
 * @brief Function for getting the lock value of the Coresight register for given debug type.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] type  Debug type to be retrieved.
 *
 * @retval true  Register is locked.
 * @retval false Register is unlocked.
 */
NRF_STATIC_INLINE bool nrf_tampc_coresight_ctrl_lock_get(NRF_TAMPC_Type const * p_reg,
                                                         nrf_tampc_debug_type_t type);

/**
 * @brief Function for setting fault injection of the Coresight register for given debug type.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] type   Debug type to be modified.
 * @param[in] enable True if fault is to be injected, false otherwise.
 */
NRF_STATIC_INLINE void nrf_tampc_coresight_ctrl_fault_set(NRF_TAMPC_Type *       p_reg,
                                                          nrf_tampc_debug_type_t type,
                                                          bool                   enable);

/**
 * @brief Function for getting the fault injection of the Coresight register for given debug type.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] type  Debug type to be retrieved.
 *
 * @retval true  Fault is to be injected.
 * @retval false No operation.
 */
NRF_STATIC_INLINE bool nrf_tampc_coresight_ctrl_fault_get(NRF_TAMPC_Type const * p_reg,
                                                          nrf_tampc_debug_type_t type);
#endif // NRF_TAMPC_HAS_CORESIGHT

#if NRF_TAMPC_HAS_WARMBOOT
/**
 * @brief Function for setting signal value of the warm boot register for given warm boot mode.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mode   Warm boot mode to be modified.
 * @param[in] enable True if signal is to be logic 1, false if logic 0.
 */
NRF_STATIC_INLINE void nrf_tampc_warmboot_ctrl_value_set(NRF_TAMPC_Type *          p_reg,
                                                         nrf_tampc_warmboot_mode_t mode,
                                                         bool                      enable);

/**
 * @brief Function for getting the signal value of the warm boot register for given warm boot mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Warm boot mode to be retrieved.
 *
 * @retval true  Signal is logic 1.
 * @retval false Signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_tampc_warmboot_ctrl_value_get(NRF_TAMPC_Type const *    p_reg,
                                                         nrf_tampc_warmboot_mode_t mode);

/**
 * @brief Function for setting lock value of the warm boot register for given warm boot mode.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mode   Warm boot mode to be modified.
 * @param[in] enable True if register is to be locked, false otherwise.
 */
NRF_STATIC_INLINE void nrf_tampc_warmboot_ctrl_lock_set(NRF_TAMPC_Type *          p_reg,
                                                        nrf_tampc_warmboot_mode_t mode,
                                                        bool                      enable);

/**
 * @brief Function for getting the lock value of the warm boot register for given warm boot mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Warm boot mode to be retrieved.
 *
 * @retval true  Register is locked.
 * @retval false Register is unlocked.
 */
NRF_STATIC_INLINE bool nrf_tampc_warmboot_ctrl_lock_get(NRF_TAMPC_Type const *    p_reg,
                                                        nrf_tampc_warmboot_mode_t mode);

/**
 * @brief Function for setting fault injection of the warm boot register for given warm boot mode.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] mode   Warm boot mode to be modified.
 * @param[in] enable True if fault is to be injected, false otherwise.
 */
NRF_STATIC_INLINE void nrf_tampc_warmboot_ctrl_fault_set(NRF_TAMPC_Type *          p_reg,
                                                         nrf_tampc_warmboot_mode_t mode,
                                                         bool                      enable);

/**
 * @brief Function for getting the fault injection of the warm boot register for given warm boot mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Warm boot mode to be retrieved.
 *
 * @retval true  Fault is to be injected.
 * @retval false No operation.
 */
NRF_STATIC_INLINE bool nrf_tampc_warmboot_ctrl_fault_get(NRF_TAMPC_Type const *    p_reg,
                                                         nrf_tampc_warmboot_mode_t mode);

/**
 * @brief Function for checking the error detection status for given warm boot mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Warm boot mode for which to retrieve the error status.
 *
 * @retval true  Error detected.
 * @retval false No error detected.
 */
NRF_STATIC_INLINE bool nrf_tampc_warmboot_status_check(NRF_TAMPC_Type const *    p_reg,
                                                       nrf_tampc_warmboot_mode_t mode);

/**
 * @brief Function for clearing the error detection status for given warm boot mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mode  Warm boot mode for which the error status is to be cleared.
 */
NRF_STATIC_INLINE void nrf_tampc_warmboot_status_clear(NRF_TAMPC_Type *          p_reg,
                                                       nrf_tampc_warmboot_mode_t mode);
#endif // NRF_TAMPC_HAS_WARMBOOT

#if NRF_TAMPC_HAS_EXTENDED_PROTECTORS
/**
 * @brief Function for setting signal value of the given signal protector register.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] ctrl   Signal protector control register to be modified.
 * @param[in] enable True if signal is to be logic 1, false if logic 0.
 */
NRF_STATIC_INLINE void nrf_tampc_protector_ctrl_value_set(NRF_TAMPC_Type *    p_reg,
                                                          nrf_tampc_protect_t ctrl,
                                                          bool                enable);

/**
 * @brief Function for getting the signal value of the given signal protector register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] ctrl  Signal protector control register to be retrieved.
 *
 * @retval true  Signal is logic 1.
 * @retval false Signal is logic 0.
 */
NRF_STATIC_INLINE bool nrf_tampc_protector_ctrl_value_get(NRF_TAMPC_Type const * p_reg,
                                                          nrf_tampc_protect_t    ctrl);

/**
 * @brief Function for setting lock value of the given signal protector register.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] ctrl   Signal protector control register to be modified.
 * @param[in] enable True if register is to be locked, false otherwise.
 */
NRF_STATIC_INLINE void nrf_tampc_protector_ctrl_lock_set(NRF_TAMPC_Type *    p_reg,
                                                         nrf_tampc_protect_t ctrl,
                                                         bool                enable);

/**
 * @brief Function for getting the lock value of the given signal protector register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] ctrl  Signal protector control register to be retrieved.
 *
 * @retval true  Register is locked.
 * @retval false Register is unlocked.
 */
NRF_STATIC_INLINE bool nrf_tampc_protector_ctrl_lock_get(NRF_TAMPC_Type const * p_reg,
                                                         nrf_tampc_protect_t    ctrl);

/**
 * @brief Function for checking the error detection status for given signal protector status register.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] status Signal protector status register for which to retrieve the error status.
 *
 * @retval true  Error detected.
 * @retval false No error detected.
 */
NRF_STATIC_INLINE bool nrf_tampc_protector_status_check(NRF_TAMPC_Type const * p_reg,
                                                        nrf_tampc_protect_t    status);

/**
 * @brief Function for clearing the error detection status for given signal protector status register.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] status Signal protector status register for which the error status is to be cleared.
 */
NRF_STATIC_INLINE void nrf_tampc_protector_status_clear(NRF_TAMPC_Type *    p_reg,
                                                        nrf_tampc_protect_t status);
#endif // NRF_TAMPC_HAS_EXTENDED_PROTECTORS

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_tampc_event_clear(NRF_TAMPC_Type * p_reg, nrf_tampc_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_tampc_event_check(NRF_TAMPC_Type const * p_reg, nrf_tampc_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE uint32_t nrf_tampc_event_address_get(NRF_TAMPC_Type const * p_reg,
                                                      nrf_tampc_event_t       event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_tampc_int_enable(NRF_TAMPC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_tampc_int_disable(NRF_TAMPC_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_tampc_int_enable_check(NRF_TAMPC_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_tampc_int_pending_get(NRF_TAMPC_Type const * p_reg)
{
    return p_reg->INTPEND;
}

NRF_STATIC_INLINE bool nrf_tampc_detector_status_check(NRF_TAMPC_Type const * p_reg,
                                                       nrf_tampc_detector_t   detector)
{
    return ((p_reg->STATUS & detector) != 0);
}

NRF_STATIC_INLINE void nrf_tampc_detector_status_clear(NRF_TAMPC_Type *     p_reg,
                                                       nrf_tampc_detector_t detector)
{
#if NRF_TAMPC_HAS_EXTENDED_PROTECTORS
    switch (detector)
    {
        case NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_SLOW:
            nrf_tampc_protector_ctrl_value_set(p_reg, NRF_TAMPC_PROTECT_GLITCH_DOMAIN_SLOW, false);
            break;
        case NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_FAST:
            nrf_tampc_protector_ctrl_value_set(p_reg, NRF_TAMPC_PROTECT_GLITCH_DOMAIN_FAST, false);
            break;
        default:
            break;
    }
#endif

#if NRF_TAMPC_HAS_DETECTORS_ENABLE
    switch (detector)
    {
        case NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_SLOW:
            p_reg->ENABLE = ((p_reg->ENABLE & ~TAMPC_ENABLE_GLITCHSLOWDOMAIN_Msk) |
                (TAMPC_ENABLE_GLITCHSLOWDOMAIN_Disabled << TAMPC_ENABLE_GLITCHSLOWDOMAIN_Pos));
            break;
        case NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_FAST:
            p_reg->ENABLE = ((p_reg->ENABLE & ~TAMPC_ENABLE_GLITCHFASTDOMAIN_Msk) |
                (TAMPC_ENABLE_GLITCHFASTDOMAIN_Disabled << TAMPC_ENABLE_GLITCHFASTDOMAIN_Pos));
            break;
        default:
            break;
    }
#endif

    p_reg->STATUS = detector;

#if NRF_TAMPC_HAS_DETECTORS_ENABLE
    switch (detector)
    {
        case NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_SLOW:
            p_reg->ENABLE = ((p_reg->ENABLE & ~TAMPC_ENABLE_GLITCHSLOWDOMAIN_Msk) |
                (TAMPC_ENABLE_GLITCHSLOWDOMAIN_Enabled << TAMPC_ENABLE_GLITCHSLOWDOMAIN_Pos));
            break;
        case NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_FAST:
            p_reg->ENABLE = ((p_reg->ENABLE & ~TAMPC_ENABLE_GLITCHFASTDOMAIN_Msk) |
                (TAMPC_ENABLE_GLITCHFASTDOMAIN_Enabled << TAMPC_ENABLE_GLITCHFASTDOMAIN_Pos));
            break;
        default:
            break;
    }
#endif

#if NRF_TAMPC_HAS_EXTENDED_PROTECTORS
    switch (detector)
    {
        case NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_SLOW:
            nrf_tampc_protector_ctrl_value_set(p_reg, NRF_TAMPC_PROTECT_GLITCH_DOMAIN_SLOW, true);
            break;
        case NRF_TAMPC_DETECTOR_GLITCH_DOMAIN_FAST:
            nrf_tampc_protector_ctrl_value_set(p_reg, NRF_TAMPC_PROTECT_GLITCH_DOMAIN_FAST, true);
            break;
        default:
            break;
    }
#endif
}

#if NRF_TAMPC_HAS_ACTIVE_SHIELD_CHANNELS
NRF_STATIC_INLINE void nrf_tampc_activeshield_channel_enable(NRF_TAMPC_Type * p_reg, uint32_t mask)
{
    p_reg->ACTIVESHIELD.CHEN |= mask;
}

NRF_STATIC_INLINE void nrf_tampc_activeshield_channel_disable(NRF_TAMPC_Type * p_reg,
                                                              uint32_t         mask)
{
    p_reg->ACTIVESHIELD.CHEN &= ~mask;
}

NRF_STATIC_INLINE uint32_t nrf_tampc_activeshield_channel_enable_check(NRF_TAMPC_Type const * p_reg,
                                                                       uint32_t               mask)
{
    return p_reg->ACTIVESHIELD.CHEN & mask;
}
#endif

NRF_STATIC_INLINE void nrf_tampc_domain_ctrl_value_set(NRF_TAMPC_Type *       p_reg,
                                                       nrf_tampc_debug_type_t type,
                                                       nrf_domain_t           domain,
                                                       bool                   enable)
{
    NRFX_ASSERT(domain > 0);
    NRFX_ASSERT(domain < NRF_DOMAIN_COUNT);

    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.DOMAIN[domain].DBGEN.CTRL =
                (TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.DOMAIN[domain].DBGEN.CTRL =
                ((p_reg->PROTECT.DOMAIN[domain].DBGEN.CTRL &
                ~TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Msk) |
                ((enable ? TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_High
                : TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Low)
                << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.DOMAIN[domain].NIDEN.CTRL =
                (TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.DOMAIN[domain].NIDEN.CTRL =
                ((p_reg->PROTECT.DOMAIN[domain].NIDEN.CTRL &
                ~TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_VALUE_Msk) |
                ((enable ? TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_VALUE_High
                : TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_VALUE_Low)
                << TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
#if NRF_TAMPC_HAS_DOMAIN_SPIDEN_PROTECTOR
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.DOMAIN[domain].SPIDEN.CTRL =
                (TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.DOMAIN[domain].SPIDEN.CTRL =
                ((p_reg->PROTECT.DOMAIN[domain].SPIDEN.CTRL &
                ~TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_VALUE_Msk) |
                ((enable ? TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_VALUE_High
                : TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_VALUE_Low)
                << TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.DOMAIN[domain].SPNIDEN.CTRL =
                (TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.DOMAIN[domain].SPNIDEN.CTRL =
                ((p_reg->PROTECT.DOMAIN[domain].SPNIDEN.CTRL &
                ~TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_VALUE_Msk) |
                ((enable ? TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_VALUE_High
                : TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_VALUE_Low)
                << TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
#endif
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_domain_ctrl_value_get(NRF_TAMPC_Type const * p_reg,
                                                       nrf_tampc_debug_type_t type,
                                                       nrf_domain_t           domain)
{
    NRFX_ASSERT(domain > 0);
    NRFX_ASSERT(domain < NRF_DOMAIN_COUNT);

    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
            return ((p_reg->PROTECT.DOMAIN[domain].DBGEN.CTRL
                     & TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_VALUE_Pos);
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
            return ((p_reg->PROTECT.DOMAIN[domain].NIDEN.CTRL
                     & TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_VALUE_Pos);
#if NRF_TAMPC_HAS_DOMAIN_SPIDEN_PROTECTOR
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
            return ((p_reg->PROTECT.DOMAIN[domain].SPIDEN.CTRL
                     & TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_VALUE_Pos);
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
            return ((p_reg->PROTECT.DOMAIN[domain].SPNIDEN.CTRL
                     & TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_VALUE_Pos);
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE void nrf_tampc_domain_ctrl_lock_set(NRF_TAMPC_Type *       p_reg,
                                                      nrf_tampc_debug_type_t type,
                                                      nrf_domain_t           domain,
                                                      bool                   enable)
{
    NRFX_ASSERT(domain > 0);
    NRFX_ASSERT(domain < NRF_DOMAIN_COUNT);

    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.DOMAIN[domain].DBGEN.CTRL =
                (TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.DOMAIN[domain].DBGEN.CTRL =
                ((p_reg->PROTECT.DOMAIN[domain].DBGEN.CTRL &
                ~TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Msk) |
                ((enable ? TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Enabled
                : TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Disabled)
                << TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.DOMAIN[domain].NIDEN.CTRL =
                (TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.DOMAIN[domain].NIDEN.CTRL =
                ((p_reg->PROTECT.DOMAIN[domain].NIDEN.CTRL &
                ~TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_LOCK_Msk) |
                ((enable ? TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_LOCK_Enabled
                : TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_LOCK_Disabled)
                << TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
#if NRF_TAMPC_HAS_DOMAIN_SPIDEN_PROTECTOR
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.DOMAIN[domain].SPIDEN.CTRL =
                (TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.DOMAIN[domain].SPIDEN.CTRL =
                ((p_reg->PROTECT.DOMAIN[domain].SPIDEN.CTRL &
                ~TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_LOCK_Msk) |
                ((enable ? TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_LOCK_Enabled
                : TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_LOCK_Disabled)
                << TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.DOMAIN[domain].SPNIDEN.CTRL =
                (TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.DOMAIN[domain].SPNIDEN.CTRL =
                ((p_reg->PROTECT.DOMAIN[domain].SPNIDEN.CTRL &
                ~TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_LOCK_Msk) |
                ((enable ? TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_LOCK_Enabled
                : TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_LOCK_Disabled)
                << TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
#endif
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_domain_ctrl_lock_get(NRF_TAMPC_Type const * p_reg,
                                                      nrf_tampc_debug_type_t type,
                                                      nrf_domain_t           domain)
{
    NRFX_ASSERT(domain > 0);
    NRFX_ASSERT(domain < NRF_DOMAIN_COUNT);

    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
            return ((p_reg->PROTECT.DOMAIN[domain].DBGEN.CTRL
                     & TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_DOMAIN_DBGEN_CTRL_LOCK_Pos);
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
            return ((p_reg->PROTECT.DOMAIN[domain].NIDEN.CTRL
                     & TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_DOMAIN_NIDEN_CTRL_LOCK_Pos);
#if NRF_TAMPC_HAS_DOMAIN_SPIDEN_PROTECTOR
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
            return ((p_reg->PROTECT.DOMAIN[domain].SPIDEN.CTRL
                     & TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_DOMAIN_SPIDEN_CTRL_LOCK_Pos);
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
            return ((p_reg->PROTECT.DOMAIN[domain].SPNIDEN.CTRL
                     & TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_DOMAIN_SPNIDEN_CTRL_LOCK_Pos);
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE void nrf_tampc_ap_ctrl_value_set(NRF_TAMPC_Type *       p_reg,
                                                   nrf_tampc_debug_type_t type,
                                                   nrf_domain_t           domain,
                                                   bool                   enable)
{
    NRFX_ASSERT(domain > 0);
    NRFX_ASSERT(domain < NRF_DOMAIN_COUNT);

    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.AP[domain].DBGEN.CTRL =
                (TAMPC_PROTECT_AP_DBGEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_AP_DBGEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.AP[domain].DBGEN.CTRL =
                ((p_reg->PROTECT.AP[domain].DBGEN.CTRL &
                ~TAMPC_PROTECT_AP_DBGEN_CTRL_VALUE_Msk) |
                ((enable ? TAMPC_PROTECT_AP_DBGEN_CTRL_VALUE_High :
                TAMPC_PROTECT_AP_DBGEN_CTRL_VALUE_Low)
                << TAMPC_PROTECT_AP_DBGEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
#if NRF_TAMPC_HAS_AP_SPIDEN_PROTECTOR
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.AP[domain].SPIDEN.CTRL =
                (TAMPC_PROTECT_AP_SPIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_AP_SPIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.AP[domain].SPIDEN.CTRL =
                ((p_reg->PROTECT.AP[domain].SPIDEN.CTRL &
                ~TAMPC_PROTECT_AP_SPIDEN_CTRL_VALUE_Msk) |
                ((enable ? TAMPC_PROTECT_AP_SPIDEN_CTRL_VALUE_High :
                TAMPC_PROTECT_AP_SPIDEN_CTRL_VALUE_Low)
                << TAMPC_PROTECT_AP_SPIDEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
#endif
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_ap_ctrl_value_get(NRF_TAMPC_Type const * p_reg,
                                                   nrf_tampc_debug_type_t type,
                                                   nrf_domain_t           domain)
{
    NRFX_ASSERT(domain > 0);
    NRFX_ASSERT(domain < NRF_DOMAIN_COUNT);

    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
            return ((p_reg->PROTECT.AP[domain].DBGEN.CTRL
                     & TAMPC_PROTECT_AP_DBGEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_AP_DBGEN_CTRL_VALUE_Pos);
#if NRF_TAMPC_HAS_AP_SPIDEN_PROTECTOR
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
            return ((p_reg->PROTECT.AP[domain].SPIDEN.CTRL
                     & TAMPC_PROTECT_AP_SPIDEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_AP_SPIDEN_CTRL_VALUE_Pos);
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE void nrf_tampc_ap_ctrl_lock_set(NRF_TAMPC_Type *       p_reg,
                                                  nrf_tampc_debug_type_t type,
                                                  nrf_domain_t           domain,
                                                  bool                   enable)
{
    NRFX_ASSERT(domain > 0);
    NRFX_ASSERT(domain < NRF_DOMAIN_COUNT);

    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.AP[domain].DBGEN.CTRL =
                (TAMPC_PROTECT_AP_DBGEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_AP_DBGEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.AP[domain].DBGEN.CTRL =
                ((p_reg->PROTECT.AP[domain].DBGEN.CTRL &
                ~TAMPC_PROTECT_AP_DBGEN_CTRL_LOCK_Msk) |
                ((enable ? TAMPC_PROTECT_AP_DBGEN_CTRL_LOCK_Enabled :
                TAMPC_PROTECT_AP_DBGEN_CTRL_LOCK_Disabled)
                << TAMPC_PROTECT_AP_DBGEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
#if NRF_TAMPC_HAS_AP_SPIDEN_PROTECTOR
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.AP[domain].SPIDEN.CTRL =
                (TAMPC_PROTECT_AP_SPIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_AP_SPIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.AP[domain].SPIDEN.CTRL =
                ((p_reg->PROTECT.AP[domain].SPIDEN.CTRL &
                ~TAMPC_PROTECT_AP_SPIDEN_CTRL_LOCK_Msk) |
                ((enable ? TAMPC_PROTECT_AP_SPIDEN_CTRL_LOCK_Enabled :
                TAMPC_PROTECT_AP_SPIDEN_CTRL_LOCK_Disabled)
                << TAMPC_PROTECT_AP_SPIDEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
#endif
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_ap_ctrl_lock_get(NRF_TAMPC_Type const * p_reg,
                                                  nrf_tampc_debug_type_t type,
                                                  nrf_domain_t           domain)
{
    NRFX_ASSERT(domain > 0);
    NRFX_ASSERT(domain < NRF_DOMAIN_COUNT);

    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
            return ((p_reg->PROTECT.AP[domain].DBGEN.CTRL
                     & TAMPC_PROTECT_AP_DBGEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_AP_DBGEN_CTRL_LOCK_Pos);
#if NRF_TAMPC_HAS_AP_SPIDEN_PROTECTOR
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
            return ((p_reg->PROTECT.AP[domain].SPIDEN.CTRL
                     & TAMPC_PROTECT_AP_SPIDEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_AP_SPIDEN_CTRL_LOCK_Pos);
#endif
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

#if NRF_TAMPC_HAS_CORESIGHT
NRF_STATIC_INLINE void nrf_tampc_coresight_ctrl_value_set(NRF_TAMPC_Type *       p_reg,
                                                          nrf_tampc_debug_type_t type,
                                                          bool                   enable)
{
    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DEVICEEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_VALUE_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_VALUE_High :
                   TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_VALUE_Low)
                  << TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.DBGEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.DBGEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.DBGEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_VALUE_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_VALUE_High :
                   TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_VALUE_Low)
                  << TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.NIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.NIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.NIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_VALUE_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_VALUE_High :
                   TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_VALUE_Low)
                  << TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_VALUE_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_VALUE_High :
                   TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_VALUE_Low)
                  << TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_VALUE_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_VALUE_High :
                   TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_VALUE_Low)
                  << TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_coresight_ctrl_value_get(NRF_TAMPC_Type const * p_reg,
                                                          nrf_tampc_debug_type_t type)
{
    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DEVICEEN:
            return ((p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_VALUE_Pos);
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
            return ((p_reg->PROTECT.CORESIGHT.DBGEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_VALUE_Pos);
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
            return ((p_reg->PROTECT.CORESIGHT.NIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_VALUE_Pos);
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
            return ((p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_VALUE_Pos);
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
            return ((p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_VALUE_Pos);
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE void nrf_tampc_coresight_ctrl_lock_set(NRF_TAMPC_Type *       p_reg,
                                                         nrf_tampc_debug_type_t type,
                                                         bool                   enable)
{
    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DEVICEEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_LOCK_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_LOCK_Enabled :
                   TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_LOCK_Disabled)
                  << TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.DBGEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.DBGEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.DBGEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_LOCK_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_LOCK_Enabled :
                   TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_LOCK_Disabled)
                  << TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.NIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.NIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.NIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_LOCK_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_LOCK_Enabled :
                   TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_LOCK_Disabled)
                  << TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_LOCK_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_LOCK_Enabled :
                   TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_LOCK_Disabled)
                  << TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_LOCK_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_LOCK_Enabled :
                   TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_LOCK_Disabled)
                  << TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_coresight_ctrl_lock_get(NRF_TAMPC_Type const * p_reg,
                                                         nrf_tampc_debug_type_t type)
{
    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DEVICEEN:
            return ((p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_LOCK_Pos);
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
            return ((p_reg->PROTECT.CORESIGHT.DBGEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_LOCK_Pos);
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
            return ((p_reg->PROTECT.CORESIGHT.NIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_LOCK_Pos);
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
            return ((p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_LOCK_Pos);
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
            return ((p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_LOCK_Pos);
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE void nrf_tampc_coresight_ctrl_fault_set(NRF_TAMPC_Type *       p_reg,
                                                          nrf_tampc_debug_type_t type,
                                                          bool                   enable)
{
    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DEVICEEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_FAULTTEST_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_FAULTTEST_Trigger :
                   TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_FAULTTEST_NoOperation)
                  << TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_FAULTTEST_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.DBGEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.DBGEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.DBGEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_FAULTTEST_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_FAULTTEST_Trigger :
                   TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_FAULTTEST_NoOperation)
                  << TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_FAULTTEST_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.NIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.NIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.NIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_FAULTTEST_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_FAULTTEST_Trigger :
                   TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_FAULTTEST_NoOperation)
                  << TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_FAULTTEST_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_FAULTTEST_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_FAULTTEST_Trigger :
                   TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_FAULTTEST_NoOperation)
                  << TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_FAULTTEST_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL =
                (TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL =
                ((p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL &
                  ~TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_FAULTTEST_Msk) |
                 ((enable ? TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_FAULTTEST_Trigger :
                   TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_FAULTTEST_NoOperation)
                  << TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_FAULTTEST_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_coresight_ctrl_fault_get(NRF_TAMPC_Type const * p_reg,
                                                          nrf_tampc_debug_type_t type)
{
    switch (type)
    {
        case NRF_TAMPC_DEBUG_TYPE_DEVICEEN:
            return ((p_reg->PROTECT.CORESIGHT.DEVICEEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_FAULTTEST_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_DEVICEEN_CTRL_FAULTTEST_Pos);
        case NRF_TAMPC_DEBUG_TYPE_DBGEN:
            return ((p_reg->PROTECT.CORESIGHT.DBGEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_FAULTTEST_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_DBGEN_CTRL_FAULTTEST_Pos);
        case NRF_TAMPC_DEBUG_TYPE_NIDEN:
            return ((p_reg->PROTECT.CORESIGHT.NIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_FAULTTEST_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_NIDEN_CTRL_FAULTTEST_Pos);
        case NRF_TAMPC_DEBUG_TYPE_SPIDEN:
            return ((p_reg->PROTECT.CORESIGHT.SPIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_FAULTTEST_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_SPIDEN_CTRL_FAULTTEST_Pos);
        case NRF_TAMPC_DEBUG_TYPE_SPNIDEN:
            return ((p_reg->PROTECT.CORESIGHT.SPNIDEN.CTRL &
                     TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_FAULTTEST_Msk)
                    >> TAMPC_PROTECT_CORESIGHT_SPNIDEN_CTRL_FAULTTEST_Pos);
        default:
            NRFX_ASSERT(0);
            return false;
    }
}
#endif // NRF_TAMPC_HAS_CORESIGHT

#if NRF_TAMPC_HAS_WARMBOOT
NRF_STATIC_INLINE void nrf_tampc_warmboot_ctrl_value_set(NRF_TAMPC_Type *          p_reg,
                                                         nrf_tampc_warmboot_mode_t mode,
                                                         bool                      enable)
{
    switch (mode)
    {
        case NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL =
                (TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL =
                ((p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL &
                  ~TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_VALUE_Msk) |
                 ((enable ? TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_VALUE_High :
                   TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_VALUE_Low)
                  << TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL =
                (TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL =
                ((p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL &
                  ~TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_VALUE_Msk) |
                 ((enable ? TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_VALUE_High :
                   TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_VALUE_Low)
                  << TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_VALUE_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_warmboot_ctrl_value_get(NRF_TAMPC_Type const *    p_reg,
                                                         nrf_tampc_warmboot_mode_t mode)
{
    switch (mode)
    {
        case NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE:
            return ((p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL &
                     TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_VALUE_Pos);
        case NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF:
            return ((p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL &
                     TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_VALUE_Msk)
                    >> TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_VALUE_Pos);
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE void nrf_tampc_warmboot_ctrl_lock_set(NRF_TAMPC_Type *          p_reg,
                                                        nrf_tampc_warmboot_mode_t mode,
                                                        bool                      enable)
{
    switch (mode)
    {
        case NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL =
                (TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL =
                ((p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL &
                  ~TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_LOCK_Msk) |
                 ((enable ? TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_LOCK_Enabled :
                   TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_LOCK_Disabled)
                  << TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL =
                (TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL =
                ((p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL &
                  ~TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_LOCK_Msk) |
                 ((enable ? TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_LOCK_Enabled :
                   TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_LOCK_Disabled)
                  << TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_LOCK_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_warmboot_ctrl_lock_get(NRF_TAMPC_Type const *    p_reg,
                                                        nrf_tampc_warmboot_mode_t mode)
{
    switch (mode)
    {
        case NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE:
            return ((p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL &
                     TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_LOCK_Pos);
        case NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF:
            return ((p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL &
                     TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_LOCK_Msk)
                    >> TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_LOCK_Pos);
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE void nrf_tampc_warmboot_ctrl_fault_set(NRF_TAMPC_Type *          p_reg,
                                                         nrf_tampc_warmboot_mode_t mode,
                                                         bool                      enable)
{
    switch (mode)
    {
        case NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL =
                (TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL =
                ((p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL &
                  ~TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_FAULTTEST_Msk) |
                 ((enable ? TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_FAULTTEST_Trigger :
                   TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_FAULTTEST_NoOperation)
                  << TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_FAULTTEST_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        case NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF:
#if NRF_TAMPC_KEY_MASK
            p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL =
                (TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_WRITEPROTECTION_Clear
                 << TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
            p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL =
                ((p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL &
                  ~TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_FAULTTEST_Msk) |
                 ((enable ? TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_FAULTTEST_Trigger :
                   TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_FAULTTEST_NoOperation)
                  << TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_FAULTTEST_Pos))
                | NRF_TAMPC_KEY_MASK;
            break;
        default:
            NRFX_ASSERT(0);
    }
}

NRF_STATIC_INLINE bool nrf_tampc_warmboot_ctrl_fault_get(NRF_TAMPC_Type const *    p_reg,
                                                         nrf_tampc_warmboot_mode_t mode)
{
    switch (mode)
    {
        case NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE:
            return ((p_reg->PROTECT.WARMBOOT.UNRETIDLE.CTRL &
                     TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_FAULTTEST_Msk)
                    >> TAMPC_PROTECT_WARMBOOT_UNRETIDLE_CTRL_FAULTTEST_Pos);
        case NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF:
            return ((p_reg->PROTECT.WARMBOOT.SYSTEMOFF.CTRL &
                     TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_FAULTTEST_Msk)
                    >> TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_CTRL_FAULTTEST_Pos);
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE bool nrf_tampc_warmboot_status_check(NRF_TAMPC_Type const *    p_reg,
                                                       nrf_tampc_warmboot_mode_t mode)
{
    switch (mode)
    {
        case NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE:
            return ((p_reg->PROTECT.WARMBOOT.UNRETIDLE.STATUS &
                    TAMPC_PROTECT_WARMBOOT_UNRETIDLE_STATUS_ERROR_Msk)
                    >> TAMPC_PROTECT_WARMBOOT_UNRETIDLE_STATUS_ERROR_Pos) ==
                    TAMPC_PROTECT_WARMBOOT_UNRETIDLE_STATUS_ERROR_Error;
        case NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF:
            return ((p_reg->PROTECT.WARMBOOT.SYSTEMOFF.STATUS &
                    TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_STATUS_ERROR_Msk)
                    >> TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_STATUS_ERROR_Pos) ==
                    TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_STATUS_ERROR_Error;
        default:
            NRFX_ASSERT(0);
            return false;
    }
}

NRF_STATIC_INLINE void nrf_tampc_warmboot_status_clear(NRF_TAMPC_Type *          p_reg,
                                                       nrf_tampc_warmboot_mode_t mode)
{
    switch (mode)
    {
        case NRF_TAMPC_WARMBOOT_MODE_UNRET_IDLE:
            p_reg->PROTECT.WARMBOOT.UNRETIDLE.STATUS =
                TAMPC_PROTECT_WARMBOOT_UNRETIDLE_STATUS_ERROR_Msk;
            break;
        case NRF_TAMPC_WARMBOOT_MODE_SYSTEMOFF:
            p_reg->PROTECT.WARMBOOT.SYSTEMOFF.STATUS =
                TAMPC_PROTECT_WARMBOOT_SYSTEMOFF_STATUS_ERROR_Msk;
            break;
        default:
            NRFX_ASSERT(0);
    }
}
#endif // NRF_TAMPC_HAS_WARMBOOT

#if NRF_TAMPC_HAS_EXTENDED_PROTECTORS
NRF_STATIC_INLINE void nrf_tampc_protector_ctrl_value_set(NRF_TAMPC_Type *    p_reg,
                                                          nrf_tampc_protect_t ctrl,
                                                          bool                enable)
{
    NRF_TAMPC_PROTECT_ACTIVESHIELD_Type * reg =
        ((NRF_TAMPC_PROTECT_ACTIVESHIELD_Type *)((uint8_t *)p_reg + (uint32_t)ctrl));

#if NRF_TAMPC_KEY_MASK
    reg->CTRL = (TAMPC_PROTECT_ACTIVESHIELD_CTRL_WRITEPROTECTION_Clear
              << TAMPC_PROTECT_ACTIVESHIELD_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
    reg->CTRL = ((reg->CTRL & ~TAMPC_PROTECT_ACTIVESHIELD_CTRL_VALUE_Msk) |
                    ((enable ? TAMPC_PROTECT_ACTIVESHIELD_CTRL_VALUE_High :
                               TAMPC_PROTECT_ACTIVESHIELD_CTRL_VALUE_Low)
                            << TAMPC_PROTECT_ACTIVESHIELD_CTRL_VALUE_Pos)) |
                               NRF_TAMPC_KEY_MASK;
}

NRF_STATIC_INLINE bool nrf_tampc_protector_ctrl_value_get(NRF_TAMPC_Type const * p_reg,
                                                          nrf_tampc_protect_t    ctrl)
{
    NRF_TAMPC_PROTECT_ACTIVESHIELD_Type const * reg =
        ((NRF_TAMPC_PROTECT_ACTIVESHIELD_Type const *)((uint8_t const *)p_reg + (uint32_t)ctrl));

    return ((reg->CTRL & TAMPC_PROTECT_ACTIVESHIELD_CTRL_VALUE_Msk)
                      >> TAMPC_PROTECT_ACTIVESHIELD_CTRL_VALUE_Pos)
                      == TAMPC_PROTECT_ACTIVESHIELD_CTRL_VALUE_High;
}

NRF_STATIC_INLINE void nrf_tampc_protector_ctrl_lock_set(NRF_TAMPC_Type *    p_reg,
                                                         nrf_tampc_protect_t ctrl,
                                                         bool                enable)
{
    NRF_TAMPC_PROTECT_ACTIVESHIELD_Type * reg =
        ((NRF_TAMPC_PROTECT_ACTIVESHIELD_Type *)((uint8_t *)p_reg + (uint32_t)ctrl));

#if NRF_TAMPC_KEY_MASK
    reg->CTRL = (TAMPC_PROTECT_ACTIVESHIELD_CTRL_WRITEPROTECTION_Clear
              << TAMPC_PROTECT_ACTIVESHIELD_CTRL_WRITEPROTECTION_Pos) | NRF_TAMPC_KEY_MASK;
#endif
    reg->CTRL = ((reg->CTRL & ~TAMPC_PROTECT_ACTIVESHIELD_CTRL_LOCK_Msk) |
                    ((enable ? TAMPC_PROTECT_ACTIVESHIELD_CTRL_LOCK_Enabled :
                               TAMPC_PROTECT_ACTIVESHIELD_CTRL_LOCK_Disabled)
                            << TAMPC_PROTECT_ACTIVESHIELD_CTRL_LOCK_Pos)) |
                               NRF_TAMPC_KEY_MASK;
}

NRF_STATIC_INLINE bool nrf_tampc_protector_ctrl_lock_get(NRF_TAMPC_Type const * p_reg,
                                                         nrf_tampc_protect_t    ctrl)
{
    NRF_TAMPC_PROTECT_ACTIVESHIELD_Type const * reg =
        ((NRF_TAMPC_PROTECT_ACTIVESHIELD_Type const *)((uint8_t const *)p_reg + (uint32_t)ctrl));

    return ((reg->CTRL & TAMPC_PROTECT_ACTIVESHIELD_CTRL_LOCK_Msk)
                      >> TAMPC_PROTECT_ACTIVESHIELD_CTRL_LOCK_Pos)
                      == TAMPC_PROTECT_ACTIVESHIELD_CTRL_LOCK_Enabled;
}

NRF_STATIC_INLINE bool nrf_tampc_protector_status_check(NRF_TAMPC_Type const * p_reg,
                                                        nrf_tampc_protect_t    status)
{
    NRF_TAMPC_PROTECT_ACTIVESHIELD_Type const * reg =
        ((NRF_TAMPC_PROTECT_ACTIVESHIELD_Type const *)((uint8_t const *)p_reg + (uint32_t)status));

    return ((reg->STATUS & TAMPC_PROTECT_ACTIVESHIELD_STATUS_ERROR_Msk)
                        >> TAMPC_PROTECT_ACTIVESHIELD_STATUS_ERROR_Pos)
                        == TAMPC_PROTECT_ACTIVESHIELD_STATUS_ERROR_Error;
}

NRF_STATIC_INLINE void nrf_tampc_protector_status_clear(NRF_TAMPC_Type *    p_reg,
                                                        nrf_tampc_protect_t status)
{
    NRF_TAMPC_PROTECT_ACTIVESHIELD_Type * reg =
        ((NRF_TAMPC_PROTECT_ACTIVESHIELD_Type *)((uint8_t *)p_reg + (uint32_t)status));

    reg->STATUS = TAMPC_PROTECT_ACTIVESHIELD_STATUS_ERROR_Msk;
}
#endif // NRF_TAMPC_HAS_EXTENDED_PROTECTORS

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRF_TAMPC_H_ */
