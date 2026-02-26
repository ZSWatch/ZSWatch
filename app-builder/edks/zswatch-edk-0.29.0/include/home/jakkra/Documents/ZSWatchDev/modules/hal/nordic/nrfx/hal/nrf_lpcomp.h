/*
 * Copyright (c) 2014 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_LPCOMP_H_
#define NRF_LPCOMP_H_

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_lpcomp_hal LPCOMP HAL
 * @{
 * @ingroup nrf_lpcomp
 * @brief   Hardware access layer for managing the Low Power Comparator (LPCOMP) peripheral.
 */

#if defined(LPCOMP_PSEL_PIN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the configuration of analog input using pin number is present. */
#define NRF_LPCOMP_HAS_AIN_AS_PIN 1
#else
#define NRF_LPCOMP_HAS_AIN_AS_PIN 0
#endif

#if defined(LPCOMP_HYST_HYST_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the hysteresis is present. */
#define NRF_LPCOMP_HAS_HYST 1
#else
#define NRF_LPCOMP_HAS_HYST 0
#endif

/** @brief LPCOMP tasks. */
typedef enum
{
    NRF_LPCOMP_TASK_START  = offsetof(NRF_LPCOMP_Type, TASKS_START), ///< LPCOMP start sampling task.
    NRF_LPCOMP_TASK_STOP   = offsetof(NRF_LPCOMP_Type, TASKS_STOP),  ///< LPCOMP stop sampling task.
    NRF_LPCOMP_TASK_SAMPLE = offsetof(NRF_LPCOMP_Type, TASKS_SAMPLE) ///< Sample comparator value.
} nrf_lpcomp_task_t;

/** @brief LPCOMP events. */
typedef enum
{
    NRF_LPCOMP_EVENT_READY = offsetof(NRF_LPCOMP_Type, EVENTS_READY), ///< LPCOMP is ready and output is valid.
    NRF_LPCOMP_EVENT_DOWN  = offsetof(NRF_LPCOMP_Type, EVENTS_DOWN),  ///< Input voltage crossed the threshold going down.
    NRF_LPCOMP_EVENT_UP    = offsetof(NRF_LPCOMP_Type, EVENTS_UP),    ///< Input voltage crossed the threshold going up.
    NRF_LPCOMP_EVENT_CROSS = offsetof(NRF_LPCOMP_Type, EVENTS_CROSS)  ///< Input voltage crossed the threshold in any direction.
} nrf_lpcomp_event_t;

/** @brief LPCOMP interrupts. */
typedef enum
{
    NRF_LPCOMP_INT_READY_MASK = LPCOMP_INTENSET_READY_Msk, ///< Interrupt on READY event.
    NRF_LPCOMP_INT_DOWN_MASK  = LPCOMP_INTENSET_DOWN_Msk,  ///< Interrupt on DOWN event.
    NRF_LPCOMP_INT_UP_MASK    = LPCOMP_INTENSET_UP_Msk,    ///< Interrupt on UP event.
    NRF_LPCOMP_INT_CROSS_MASK = LPCOMP_INTENSET_CROSS_Msk  ///< Interrupt on CROSS event.
} nrf_lpcomp_int_mask_t;

/** @brief LPCOMP shortcut masks. */
typedef enum
{
    NRF_LPCOMP_SHORT_CROSS_STOP_MASK   = LPCOMP_SHORTS_CROSS_STOP_Msk,  ///< Shortcut between CROSS event and STOP task.
    NRF_LPCOMP_SHORT_UP_STOP_MASK      = LPCOMP_SHORTS_UP_STOP_Msk,     ///< Shortcut between UP event and STOP task.
    NRF_LPCOMP_SHORT_DOWN_STOP_MASK    = LPCOMP_SHORTS_DOWN_STOP_Msk,   ///< Shortcut between DOWN event and STOP task.
    NRF_LPCOMP_SHORT_READY_STOP_MASK   = LPCOMP_SHORTS_READY_STOP_Msk,  ///< Shortcut between READY event and STOP task.
    NRF_LPCOMP_SHORT_READY_SAMPLE_MASK = LPCOMP_SHORTS_READY_SAMPLE_Msk ///< Shortcut between READY event and SAMPLE task.
} nrf_lpcomp_short_mask_t;

/** @brief LPCOMP reference selection. */
typedef enum
{
#if (LPCOMP_REFSEL_RESOLUTION == 8) || defined(__NRFX_DOXYGEN__)
    NRF_LPCOMP_REF_SUPPLY_1_8   = LPCOMP_REFSEL_REFSEL_SupplyOneEighthPrescaling,      ///< Use supply with a 1/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_2_8   = LPCOMP_REFSEL_REFSEL_SupplyTwoEighthsPrescaling,     ///< Use supply with a 2/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_3_8   = LPCOMP_REFSEL_REFSEL_SupplyThreeEighthsPrescaling,   ///< Use supply with a 3/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_4_8   = LPCOMP_REFSEL_REFSEL_SupplyFourEighthsPrescaling,    ///< Use supply with a 4/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_5_8   = LPCOMP_REFSEL_REFSEL_SupplyFiveEighthsPrescaling,    ///< Use supply with a 5/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_6_8   = LPCOMP_REFSEL_REFSEL_SupplySixEighthsPrescaling,     ///< Use supply with a 6/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_7_8   = LPCOMP_REFSEL_REFSEL_SupplySevenEighthsPrescaling,   ///< Use supply with a 7/8 prescaler as reference.
#elif (LPCOMP_REFSEL_RESOLUTION == 16) || defined(__NRFX_DOXYGEN__)
    NRF_LPCOMP_REF_SUPPLY_1_8   = LPCOMP_REFSEL_REFSEL_Ref1_8Vdd,                      ///< Use supply with a 1/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_2_8   = LPCOMP_REFSEL_REFSEL_Ref2_8Vdd,                      ///< Use supply with a 2/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_3_8   = LPCOMP_REFSEL_REFSEL_Ref3_8Vdd,                      ///< Use supply with a 3/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_4_8   = LPCOMP_REFSEL_REFSEL_Ref4_8Vdd,                      ///< Use supply with a 4/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_5_8   = LPCOMP_REFSEL_REFSEL_Ref5_8Vdd,                      ///< Use supply with a 5/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_6_8   = LPCOMP_REFSEL_REFSEL_Ref6_8Vdd,                      ///< Use supply with a 6/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_7_8   = LPCOMP_REFSEL_REFSEL_Ref7_8Vdd,                      ///< Use supply with a 7/8 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_1_16  = LPCOMP_REFSEL_REFSEL_Ref1_16Vdd,                     ///< Use supply with a 1/16 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_3_16  = LPCOMP_REFSEL_REFSEL_Ref3_16Vdd,                     ///< Use supply with a 3/16 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_5_16  = LPCOMP_REFSEL_REFSEL_Ref5_16Vdd,                     ///< Use supply with a 5/16 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_7_16  = LPCOMP_REFSEL_REFSEL_Ref7_16Vdd,                     ///< Use supply with a 7/16 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_9_16  = LPCOMP_REFSEL_REFSEL_Ref9_16Vdd,                     ///< Use supply with a 9/16 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_11_16 = LPCOMP_REFSEL_REFSEL_Ref11_16Vdd,                    ///< Use supply with a 11/16 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_13_16 = LPCOMP_REFSEL_REFSEL_Ref13_16Vdd,                    ///< Use supply with a 13/16 prescaler as reference.
    NRF_LPCOMP_REF_SUPPLY_15_16 = LPCOMP_REFSEL_REFSEL_Ref15_16Vdd,                    ///< Use supply with a 15/16 prescaler as reference.
#endif
    NRF_LPCOMP_REF_EXT_REF      = LPCOMP_REFSEL_REFSEL_ARef,                           ///< Use external analog reference.
} nrf_lpcomp_ref_t;

/** @brief LPCOMP external reference selection. */
#if NRF_LPCOMP_HAS_AIN_AS_PIN
typedef uint32_t nrf_lpcomp_ext_ref_t;
#else
typedef enum
{
    NRF_LPCOMP_EXT_REF_REF0 = LPCOMP_EXTREFSEL_EXTREFSEL_AnalogReference0, ///< External reference 0.
    NRF_LPCOMP_EXT_REF_REF1 = LPCOMP_EXTREFSEL_EXTREFSEL_AnalogReference1, ///< External reference 1.
} nrf_lpcomp_ext_ref_t;
#endif

/** @brief LPCOMP input selection. */
#if NRF_LPCOMP_HAS_AIN_AS_PIN
typedef uint32_t nrf_lpcomp_input_t;
#else
typedef enum
{
    NRF_LPCOMP_INPUT_0 = LPCOMP_PSEL_PSEL_AnalogInput0, ///< Input 0.
    NRF_LPCOMP_INPUT_1 = LPCOMP_PSEL_PSEL_AnalogInput1, ///< Input 1.
    NRF_LPCOMP_INPUT_2 = LPCOMP_PSEL_PSEL_AnalogInput2, ///< Input 2.
    NRF_LPCOMP_INPUT_3 = LPCOMP_PSEL_PSEL_AnalogInput3, ///< Input 3.
    NRF_LPCOMP_INPUT_4 = LPCOMP_PSEL_PSEL_AnalogInput4, ///< Input 4.
    NRF_LPCOMP_INPUT_5 = LPCOMP_PSEL_PSEL_AnalogInput5, ///< Input 5.
    NRF_LPCOMP_INPUT_6 = LPCOMP_PSEL_PSEL_AnalogInput6, ///< Input 6.
    NRF_LPCOMP_INPUT_7 = LPCOMP_PSEL_PSEL_AnalogInput7  ///< Input 7.
} nrf_lpcomp_input_t;
#endif

/** @brief LPCOMP detection type selection. */
typedef enum
{
    NRF_LPCOMP_DETECT_CROSS = LPCOMP_ANADETECT_ANADETECT_Cross, ///< Generate ANADETEC on crossing, both upwards and downwards crossing.
    NRF_LPCOMP_DETECT_UP    = LPCOMP_ANADETECT_ANADETECT_Up,    ///< Generate ANADETEC on upwards crossing only.
    NRF_LPCOMP_DETECT_DOWN  = LPCOMP_ANADETECT_ANADETECT_Down   ///< Generate ANADETEC on downwards crossing only.
} nrf_lpcomp_detect_t;

#if NRF_LPCOMP_HAS_HYST
/** @brief LPCOMP hysteresis. */
typedef enum
{
#ifdef LPCOMP_HYST_HYST_NoHyst
    NRF_LPCOMP_HYST_NOHYST  = LPCOMP_HYST_HYST_NoHyst,   ///< Comparator hysteresis disabled.
#else
    NRF_LPCOMP_HYST_NOHYST  = LPCOMP_HYST_HYST_Disabled, ///< Comparator hysteresis disabled.
#endif
#ifdef LPCOMP_HYST_HYST_Hyst50mV
    NRF_LPCOMP_HYST_ENABLED = LPCOMP_HYST_HYST_Hyst50mV  ///< Comparator hysteresis enabled (typically 50 mV).
#else
    NRF_LPCOMP_HYST_ENABLED = LPCOMP_HYST_HYST_Enabled   ///< Comparator hysteresis enabled (typically 50 mV).
#endif
} nrf_lpcomp_hyst_t;
#endif // NRF_LPCOMP_HAS_HYST

/**
 * @brief Function for setting the specified LPCOMP task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  LPCOMP task to be set.
 */
NRF_STATIC_INLINE void nrf_lpcomp_task_trigger(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_task_t task);

/**
 * @brief Function for getting the address of the specified LPCOMP task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  LPCOMP task.
 *
 * @return The address of the specified LPCOMP task.
 */
NRF_STATIC_INLINE uint32_t nrf_lpcomp_task_address_get(NRF_LPCOMP_Type const * p_reg,
                                                       nrf_lpcomp_task_t       task);

/**
 * @brief Function for retrieving the state of the LPCOMP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_lpcomp_event_check(NRF_LPCOMP_Type const * p_reg,
                                              nrf_lpcomp_event_t      event);

/**
 * @brief Function for clearing the specified LPCOMP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event LPCOMP event to be cleared.
 */
NRF_STATIC_INLINE void nrf_lpcomp_event_clear(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_event_t event);

/**
 * @brief Function for getting the address of the specified LPCOMP event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event LPCOMP event.
 *
 * @return The address of the specified LPCOMP event.
 */
NRF_STATIC_INLINE uint32_t nrf_lpcomp_event_address_get(NRF_LPCOMP_Type const * p_reg,
                                                        nrf_lpcomp_event_t      event);

/**
 * @brief Function for setting LPCOMP shorts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of shortcuts.
 */
NRF_STATIC_INLINE void nrf_lpcomp_shorts_enable(NRF_LPCOMP_Type * p_reg, uint32_t mask);

/**
 * @brief Function for clearing LPCOMP shorts by mask.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of shortcuts.
 */
NRF_STATIC_INLINE void nrf_lpcomp_shorts_disable(NRF_LPCOMP_Type * p_reg, uint32_t mask);

/**
 * @brief Function for for setting the specified shortcuts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of shortcuts.
 */
NRF_STATIC_INLINE void nrf_lpcomp_shorts_set(NRF_LPCOMP_Type * p_reg, uint32_t mask);

/**
 * @brief Function for enabling interrupts from LPCOMP.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_lpcomp_int_mask_t values for bit masking.
 *
 * @sa nrf_lpcomp_int_disable
 * @sa nrf_lpcomp_int_enable_check
 */
NRF_STATIC_INLINE void nrf_lpcomp_int_enable(NRF_LPCOMP_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling interrupts from LPCOMP.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_lpcomp_int_mask_t values for bit masking.
 *
 * @sa nrf_lpcomp_int_enable
 * @sa nrf_lpcomp_int_enable_check
 */
NRF_STATIC_INLINE void nrf_lpcomp_int_disable(NRF_LPCOMP_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_lpcomp_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_lpcomp_int_enable_check(NRF_LPCOMP_Type const * p_reg,
                                                       uint32_t                mask);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting subscribe configuration for a given LPCOMP task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which the configuration is set.
 * @param[in] channel Channel through which events are subscribed.
 */
NRF_STATIC_INLINE void nrf_lpcomp_subscribe_set(NRF_LPCOMP_Type * p_reg,
                                                nrf_lpcomp_task_t task,
                                                uint8_t           channel);

/**
 * @brief Function for clearing subscribe configuration for a given LPCOMP task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which the configuration is cleared.
 */
NRF_STATIC_INLINE void nrf_lpcomp_subscribe_clear(NRF_LPCOMP_Type * p_reg,
                                                  nrf_lpcomp_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        LPCOMP task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return LPCOMP subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_lpcomp_subscribe_get(NRF_LPCOMP_Type const * p_reg,
                                                    nrf_lpcomp_task_t       task);

/**
 * @brief Function for setting publish configuration for a given LPCOMP event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which the configuration is set.
 * @param[in] channel Channel through which the event is published.
 */
NRF_STATIC_INLINE void nrf_lpcomp_publish_set(NRF_LPCOMP_Type *  p_reg,
                                              nrf_lpcomp_event_t event,
                                              uint8_t            channel);

/**
 * @brief Function for clearing publish configuration for a given LPCOMP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which the configuration is cleared.
 */
NRF_STATIC_INLINE void nrf_lpcomp_publish_clear(NRF_LPCOMP_Type *  p_reg,
                                                nrf_lpcomp_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        LPCOMP event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return LPCOMP publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_lpcomp_publish_get(NRF_LPCOMP_Type const * p_reg,
                                                  nrf_lpcomp_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for setting the reference source.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] reference LPCOMP reference selection.
 */
NRF_STATIC_INLINE void nrf_lpcomp_ref_set(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_ref_t reference);

/**
 * @brief Function for setting the external analog reference source.
 *
 * To use external reference first call @ref nrf_lpcomp_ref_set with NRF_LPCOMP_REF_EXT_REF argument.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] ext_ref LPCOMP external analog reference selection.
 */
NRF_STATIC_INLINE void nrf_lpcomp_ext_ref_set(NRF_LPCOMP_Type *    p_reg,
                                              nrf_lpcomp_ext_ref_t ext_ref);

/**
 * @brief Function for selecting an active LPCOMP input.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] input Input to be selected.
 */
NRF_STATIC_INLINE void nrf_lpcomp_input_select(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_input_t input);

/**
 * @brief Function for setting the detection type.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] detection LPCOMP detection type.
 */
NRF_STATIC_INLINE void nrf_lpcomp_detection_set(NRF_LPCOMP_Type *   p_reg,
                                                nrf_lpcomp_detect_t detection);

#if NRF_LPCOMP_HAS_HYST
/**
 * @brief Function for setting the hysteresis.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] hyst  LPCOMP comparator hysteresis.
 */
NRF_STATIC_INLINE void nrf_lpcomp_hysteresis_set(NRF_LPCOMP_Type * p_reg,
                                                 nrf_lpcomp_hyst_t hyst);
#endif

/**
 * @brief Function for enabling the LPCOMP.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_lpcomp_enable(NRF_LPCOMP_Type * p_reg);

/**
 * @brief Function for disabling the LPCOMP.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_lpcomp_disable(NRF_LPCOMP_Type * p_reg);

/**
 * @brief Function for checking if the LPCOMP peripheral is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The LPCOMP peripheral is enabled.
 * @retval false The LPCOMP peripheral is not enabled.
 */
NRF_STATIC_INLINE bool nrf_lpcomp_enable_check(NRF_LPCOMP_Type * p_reg);

/**
 * @brief Function for getting the last LPCOMP compare result.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The last compare result. If 0, then VIN+ < VIN-. If 1, then VIN- < VIN+.
 */
NRF_STATIC_INLINE uint32_t nrf_lpcomp_result_get(NRF_LPCOMP_Type const * p_reg);

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_lpcomp_task_trigger(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_task_t task)
{
    *( (volatile uint32_t *)( (uint8_t *)p_reg + (uint32_t)task) ) = 1;
}

NRF_STATIC_INLINE uint32_t nrf_lpcomp_task_address_get(NRF_LPCOMP_Type const * p_reg,
                                                       nrf_lpcomp_task_t       task)
{
    return nrf_task_event_address_get(p_reg, task);
}

NRF_STATIC_INLINE bool nrf_lpcomp_event_check(NRF_LPCOMP_Type const * p_reg,
                                              nrf_lpcomp_event_t      event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE void nrf_lpcomp_event_clear(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_event_t event)
{
    *( (volatile uint32_t *)( (uint8_t *)p_reg + (uint32_t)event) ) = 0;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE uint32_t nrf_lpcomp_event_address_get(NRF_LPCOMP_Type const * p_reg,
                                                        nrf_lpcomp_event_t      event)
{
    return nrf_task_event_address_get(p_reg, event);
}

NRF_STATIC_INLINE void nrf_lpcomp_shorts_enable(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS |= mask;
}

NRF_STATIC_INLINE void nrf_lpcomp_shorts_disable(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS &= ~mask;
}

NRF_STATIC_INLINE void nrf_lpcomp_shorts_set(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    p_reg->SHORTS = mask;
}

NRF_STATIC_INLINE void nrf_lpcomp_int_enable(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE void nrf_lpcomp_int_disable(NRF_LPCOMP_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

NRF_STATIC_INLINE uint32_t nrf_lpcomp_int_enable_check(NRF_LPCOMP_Type const * p_reg,
                                                       uint32_t                mask)
{
    return p_reg->INTENSET & mask; // when read this register will return the value of INTEN.
}

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
NRF_STATIC_INLINE void nrf_lpcomp_subscribe_set(NRF_LPCOMP_Type * p_reg,
                                                nrf_lpcomp_task_t task,
                                                uint8_t           channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_lpcomp_subscribe_clear(NRF_LPCOMP_Type * p_reg,
                                                  nrf_lpcomp_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_lpcomp_subscribe_get(NRF_LPCOMP_Type const * p_reg,
                                                    nrf_lpcomp_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_lpcomp_publish_set(NRF_LPCOMP_Type *  p_reg,
                                              nrf_lpcomp_event_t event,
                                              uint8_t            channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_lpcomp_publish_clear(NRF_LPCOMP_Type *  p_reg,
                                                nrf_lpcomp_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_lpcomp_publish_get(NRF_LPCOMP_Type const * p_reg,
                                                  nrf_lpcomp_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

NRF_STATIC_INLINE void nrf_lpcomp_ref_set(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_ref_t reference)
{
    p_reg->REFSEL = (reference << LPCOMP_REFSEL_REFSEL_Pos) & LPCOMP_REFSEL_REFSEL_Msk;
}

NRF_STATIC_INLINE void nrf_lpcomp_ext_ref_set(NRF_LPCOMP_Type *    p_reg,
                                              nrf_lpcomp_ext_ref_t ext_ref)
{
#if NRF_LPCOMP_HAS_AIN_AS_PIN
    p_reg->EXTREFSEL = ((NRF_PIN_NUMBER_TO_PIN(ext_ref) << LPCOMP_EXTREFSEL_PIN_Pos) &
                        LPCOMP_EXTREFSEL_PIN_Msk)
                        | ((NRF_PIN_NUMBER_TO_PORT(ext_ref) << LPCOMP_EXTREFSEL_PORT_Pos) &
                        LPCOMP_EXTREFSEL_PORT_Msk);
#else
    p_reg->EXTREFSEL = (ext_ref << LPCOMP_EXTREFSEL_EXTREFSEL_Pos) &
                       LPCOMP_EXTREFSEL_EXTREFSEL_Msk;
#endif
}

NRF_STATIC_INLINE void nrf_lpcomp_input_select(NRF_LPCOMP_Type * p_reg, nrf_lpcomp_input_t input)
{
#if NRF_LPCOMP_HAS_AIN_AS_PIN
    p_reg->PSEL = (NRF_PIN_NUMBER_TO_PORT(input) << LPCOMP_PSEL_PORT_Pos) |
                  (NRF_PIN_NUMBER_TO_PIN(input) << LPCOMP_PSEL_PIN_Pos) |
                  (p_reg->PSEL & ~(LPCOMP_PSEL_PORT_Msk | LPCOMP_PSEL_PIN_Msk));
#else
    p_reg->PSEL = ((uint32_t)input << LPCOMP_PSEL_PSEL_Pos) | (p_reg->PSEL & ~LPCOMP_PSEL_PSEL_Msk);
#endif
}

NRF_STATIC_INLINE void nrf_lpcomp_detection_set(NRF_LPCOMP_Type *   p_reg,
                                                nrf_lpcomp_detect_t detection)
{
    p_reg->ANADETECT = (detection << LPCOMP_ANADETECT_ANADETECT_Pos) &
                       LPCOMP_ANADETECT_ANADETECT_Msk;
}

#if NRF_LPCOMP_HAS_HYST
NRF_STATIC_INLINE void nrf_lpcomp_hysteresis_set(NRF_LPCOMP_Type * p_reg,
                                                 nrf_lpcomp_hyst_t hyst)
{
    p_reg->HYST = ((hyst) << LPCOMP_HYST_HYST_Pos) & LPCOMP_HYST_HYST_Msk;
}
#endif

NRF_STATIC_INLINE void nrf_lpcomp_enable(NRF_LPCOMP_Type * p_reg)
{
    p_reg->ENABLE = LPCOMP_ENABLE_ENABLE_Enabled << LPCOMP_ENABLE_ENABLE_Pos;
}

NRF_STATIC_INLINE void nrf_lpcomp_disable(NRF_LPCOMP_Type * p_reg)
{
    p_reg->ENABLE = LPCOMP_ENABLE_ENABLE_Disabled << LPCOMP_ENABLE_ENABLE_Pos;
}

NRF_STATIC_INLINE bool nrf_lpcomp_enable_check(NRF_LPCOMP_Type * p_reg)
{
    return ((p_reg->ENABLE) & LPCOMP_ENABLE_ENABLE_Enabled) >> LPCOMP_ENABLE_ENABLE_Pos;
}

NRF_STATIC_INLINE uint32_t nrf_lpcomp_result_get(NRF_LPCOMP_Type const * p_reg)
{
    return (uint32_t)p_reg->RESULT;
}

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_LPCOMP_H_
