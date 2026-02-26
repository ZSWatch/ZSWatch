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

#ifndef NRF_REGULATORS_H__
#define NRF_REGULATORS_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_regulators_hal REGULATORS HAL
 * @{
 * @ingroup nrf_power
 * @brief   Hardware access layer for managing the REGULATORS peripheral.
 */

#if defined(REGULATORS_DCDCEN_DCDCEN_Msk) || defined(REGULATORS_VREGMAIN_DCDCEN_VAL_Msk) || \
    defined(REGULATORS_VREGMAIN_DCDCEN_DCDCEN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the main voltage regulator (VREGMAIN) is present. */
#define NRF_REGULATORS_HAS_VREG_MAIN 1
#else
#define NRF_REGULATORS_HAS_VREG_MAIN 0
#endif

#if defined(REGULATORS_VREGH_DCDCEN_DCDCEN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether high voltage regulator (VREGH) is present. */
#define NRF_REGULATORS_HAS_VREG_HIGH 1
#else
#define NRF_REGULATORS_HAS_VREG_HIGH 0
#endif

#if defined(REGULATORS_VREGM_ENABLE_ENABLE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether medium voltage regulator (VREGM) is present. */
#define NRF_REGULATORS_HAS_VREG_MEDIUM 1
#else
#define NRF_REGULATORS_HAS_VREG_MEDIUM 0
#endif

#if defined(REGULATORS_VREGRADIO_DCDCEN_DCDCEN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether radio regulator (VREGRADIO) is present. */
#define NRF_REGULATORS_HAS_VREG_RADIO 1
#else
#define NRF_REGULATORS_HAS_VREG_RADIO 0
#endif

#if NRF_REGULATORS_HAS_VREG_MAIN || NRF_REGULATORS_HAS_VREG_HIGH || \
    NRF_REGULATORS_HAS_VREG_MEDIUM || NRF_REGULATORS_HAS_VREG_RADIO
/** @brief Symbol indicating whether any regulator is present. */
#define NRF_REGULATORS_HAS_VREG_ANY 1
#else
#define NRF_REGULATORS_HAS_VREG_ANY 0
#endif

#if defined(REGULATORS_POFCON_POF_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether Power-On-Fail Comparator (POF Comparator) is present. */
#define NRF_REGULATORS_HAS_POF 1
#else
#define NRF_REGULATORS_HAS_POF 0
#endif

#if defined(REGULATORS_POFCON_THRESHOLDVDDH_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether POF Comparator for VDDH is present. */
#define NRF_REGULATORS_HAS_POF_VDDH 1
#else
#define NRF_REGULATORS_HAS_POF_VDDH 0
#endif

#if defined(REGULATORS_POFCON_EVENTDISABLE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether POF Comparator can disable POFWARN event. */
#define NRF_REGULATORS_HAS_POF_WARN_DISABLE 1
#else
#define NRF_REGULATORS_HAS_POF_WARN_DISABLE 0
#endif

#if defined(REGULATORS_POFSTAT_COMPARATOR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether POF Comparator status is present. */
#define NRF_REGULATORS_HAS_POF_STATUS 1
#else
#define NRF_REGULATORS_HAS_POF_STATUS 0
#endif

#if defined(REGULATORS_TRIM_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether TRIM register is present. */
#define NRF_REGULATORS_HAS_TRIM 1
#else
#define NRF_REGULATORS_HAS_TRIM 0
#endif

#if defined(REGULATORS_MAINREGSTATUS_VREGH_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether main supply status is present. */
#define NRF_REGULATORS_HAS_MAIN_STATUS 1
#else
#define NRF_REGULATORS_HAS_MAIN_STATUS 0
#endif

#if defined(REGULATORS_VREGMAIN_INDUCTORDET_DETECTED_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether inductor detection is present. */
#define NRF_REGULATORS_HAS_INDUCTOR_DET 1
#else
#define NRF_REGULATORS_HAS_INDUCTOR_DET 0
#endif

#if defined(REGULATORS_HIBERNATOR_SYSTEMHIBERNATE_ResetValue) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether HIBERNATOR register is present. */
#define NRF_REGULATORS_HAS_HIBERNATOR 1
#else
#define NRF_REGULATORS_HAS_HIBERNATOR 0
#endif

#if NRF_REGULATORS_HAS_VREG_ANY
/** @brief Voltage regulators. */
typedef enum
{
#if NRF_REGULATORS_HAS_VREG_MAIN
    NRF_REGULATORS_VREG_MAIN,   ///< Main voltage regulator (VREGMAIN).
#endif
#if NRF_REGULATORS_HAS_VREG_HIGH
    NRF_REGULATORS_VREG_HIGH,   ///< High voltage regulator (VREGH).
#endif
#if NRF_REGULATORS_HAS_VREG_MEDIUM
    NRF_REGULATORS_VREG_MEDIUM, ///< Medium voltage regulator (VREGM).
#endif
#if NRF_REGULATORS_HAS_VREG_RADIO
    NRF_REGULATORS_VREG_RADIO,  ///< Radio voltage regulator (VREGRADIO).
#endif
} nrf_regulators_vreg_t;
#endif

#if NRF_REGULATORS_HAS_POF
/** @brief POF Comparator thresholds. */
typedef enum
{
#if defined(REGULATORS_POFCON_THRESHOLD_V090) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_0V9  = REGULATORS_POFCON_THRESHOLD_V090, ///< Set threshold to 0.9 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V095) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_0V95 = REGULATORS_POFCON_THRESHOLD_V095, ///< Set threshold to 0.95 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V100) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V0  = REGULATORS_POFCON_THRESHOLD_V100, ///< Set threshold to 1.0 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V105) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V05 = REGULATORS_POFCON_THRESHOLD_V105, ///< Set threshold to 1.05 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V110) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V1  = REGULATORS_POFCON_THRESHOLD_V110, ///< Set threshold to 1.1 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V115) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V15 = REGULATORS_POFCON_THRESHOLD_V115, ///< Set threshold to 1.15 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V120) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V2  = REGULATORS_POFCON_THRESHOLD_V120, ///< Set threshold to 1.2 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V125) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V25 = REGULATORS_POFCON_THRESHOLD_V125, ///< Set threshold to 1.25 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V130) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V3  = REGULATORS_POFCON_THRESHOLD_V130, ///< Set threshold to 1.3 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V135) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V35 = REGULATORS_POFCON_THRESHOLD_V135, ///< Set threshold to 1.35 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V140) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V4  = REGULATORS_POFCON_THRESHOLD_V140, ///< Set threshold to 1.4 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V145) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V45 = REGULATORS_POFCON_THRESHOLD_V145, ///< Set threshold to 1.45 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V150) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V5  = REGULATORS_POFCON_THRESHOLD_V150, ///< Set threshold to 1.5 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V155) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V55 = REGULATORS_POFCON_THRESHOLD_V155, ///< Set threshold to 1.55 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V160) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V6  = REGULATORS_POFCON_THRESHOLD_V160, ///< Set threshold to 1.6 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V165) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V65 = REGULATORS_POFCON_THRESHOLD_V165, ///< Set threshold to 1.65 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V17) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V7  = REGULATORS_POFCON_THRESHOLD_V17,  ///< Set threshold to 1.7 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V18) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V8  = REGULATORS_POFCON_THRESHOLD_V18,  ///< Set threshold to 1.8 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V19) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_1V9  = REGULATORS_POFCON_THRESHOLD_V19,  ///< Set threshold to 1.9 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V20) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V0  = REGULATORS_POFCON_THRESHOLD_V20,  ///< Set threshold to 2.0 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V21) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V1  = REGULATORS_POFCON_THRESHOLD_V21,  ///< Set threshold to 2.1 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V22) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V2  = REGULATORS_POFCON_THRESHOLD_V22,  ///< Set threshold to 2.2 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V23) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V3  = REGULATORS_POFCON_THRESHOLD_V23,  ///< Set threshold to 2.3 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V24) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V4  = REGULATORS_POFCON_THRESHOLD_V24,  ///< Set threshold to 2.4 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V25) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V5  = REGULATORS_POFCON_THRESHOLD_V25,  ///< Set threshold to 2.5 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V26) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V6  = REGULATORS_POFCON_THRESHOLD_V26,  ///< Set threshold to 2.6 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V27) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V7  = REGULATORS_POFCON_THRESHOLD_V27,  ///< Set threshold to 2.7 V.
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V28) || defined(__NRFX_DOXYGEN__)
    NRF_REGULATORS_POF_THR_2V8  = REGULATORS_POFCON_THRESHOLD_V28,  ///< Set threshold to 2.8 V.
#endif
} nrf_regulators_pof_thr_t;
#endif

#if NRF_REGULATORS_HAS_POF_VDDH
/** @brief POF Comparator thresholds for VDDH. */
typedef enum
{
    NRF_REGULATORS_POF_THR_VDDH_2V7 = REGULATORS_POFCON_THRESHOLDVDDH_V27, ///< Set threshold to 2.7 V.
    NRF_REGULATORS_POF_THR_VDDH_2V8 = REGULATORS_POFCON_THRESHOLDVDDH_V28, ///< Set threshold to 2.8 V.
    NRF_REGULATORS_POF_THR_VDDH_2V9 = REGULATORS_POFCON_THRESHOLDVDDH_V29, ///< Set threshold to 2.9 V.
    NRF_REGULATORS_POF_THR_VDDH_3V0 = REGULATORS_POFCON_THRESHOLDVDDH_V30, ///< Set threshold to 3.0 V.
    NRF_REGULATORS_POF_THR_VDDH_3V1 = REGULATORS_POFCON_THRESHOLDVDDH_V31, ///< Set threshold to 3.1 V.
    NRF_REGULATORS_POF_THR_VDDH_3V2 = REGULATORS_POFCON_THRESHOLDVDDH_V32, ///< Set threshold to 3.2 V.
    NRF_REGULATORS_POF_THR_VDDH_3V3 = REGULATORS_POFCON_THRESHOLDVDDH_V33, ///< Set threshold to 3.3 V.
    NRF_REGULATORS_POF_THR_VDDH_3V4 = REGULATORS_POFCON_THRESHOLDVDDH_V34, ///< Set threshold to 3.4 V.
    NRF_REGULATORS_POF_THR_VDDH_3V5 = REGULATORS_POFCON_THRESHOLDVDDH_V35, ///< Set threshold to 3.5 V.
    NRF_REGULATORS_POF_THR_VDDH_3V6 = REGULATORS_POFCON_THRESHOLDVDDH_V36, ///< Set threshold to 3.6 V.
    NRF_REGULATORS_POF_THR_VDDH_3V7 = REGULATORS_POFCON_THRESHOLDVDDH_V37, ///< Set threshold to 3.7 V.
    NRF_REGULATORS_POF_THR_VDDH_3V8 = REGULATORS_POFCON_THRESHOLDVDDH_V38, ///< Set threshold to 3.8 V.
    NRF_REGULATORS_POF_THR_VDDH_3V9 = REGULATORS_POFCON_THRESHOLDVDDH_V39, ///< Set threshold to 3.9 V.
    NRF_REGULATORS_POF_THR_VDDH_4V0 = REGULATORS_POFCON_THRESHOLDVDDH_V40, ///< Set threshold to 4.0 V.
    NRF_REGULATORS_POF_THR_VDDH_4V1 = REGULATORS_POFCON_THRESHOLDVDDH_V41, ///< Set threshold to 4.1 V.
    NRF_REGULATORS_POF_THR_VDDH_4V2 = REGULATORS_POFCON_THRESHOLDVDDH_V42, ///< Set threshold to 4.2 V.
} nrf_regulators_pof_thr_vddh_t;
#endif

#if NRF_REGULATORS_HAS_TRIM
/** @brief Components allowed to introduce ELV mode. */
typedef enum
{
    NRF_REGULATORS_ELV_MODE_ALLOW_MASK_EXT ///< Reserved. For internal use only.
} nrf_regulators_elv_mode_allow_mask_t;
#endif // NRF_REGULATORS_HAS_TRIM

#if NRF_REGULATORS_HAS_POF
/** @brief POF Comparator configuration structure. */
typedef struct {
    bool                          enable;       ///< Enable or disable POF Comparator.
    nrf_regulators_pof_thr_t      thr;          ///< Threshold to be set for POF Comparator.
#if NRF_REGULATORS_HAS_POF_VDDH
    nrf_regulators_pof_thr_vddh_t thr_vddh;     ///< Threshold to be set for POF Comparator for VDDH.
#endif
#if NRF_REGULATORS_HAS_POF_WARN_DISABLE
    bool                          warn_disable; ///< Disable or enable POFWARN event.
#endif
} nrf_regulators_pof_config_t;
#endif

#if NRF_REGULATORS_HAS_MAIN_STATUS
/** @brief Main supply status. */
typedef enum
{
    NRF_REGULATORS_MAIN_STATUS_NORMAL = REGULATORS_MAINREGSTATUS_VREGH_Inactive, ///< Normal voltage mode. Voltage supplied on VDD and VDDH.
    NRF_REGULATORS_MAIN_STATUS_HIGH   = REGULATORS_MAINREGSTATUS_VREGH_Active    ///< High voltage mode. Voltage supplied on VDDH.
} nrf_regulators_main_status_t;
#endif

#if NRF_REGULATORS_HAS_VREG_ANY
/**
 * @brief Function for enabling or disabling the specified voltage regulator.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] regulator Regulator to be enabled or disabled.
 * @param[in] enable    True if specified voltage regulator is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_regulators_vreg_enable_set(NRF_REGULATORS_Type * p_reg,
                                                      nrf_regulators_vreg_t regulator,
                                                      bool                  enable);

/**
 * @brief Function for checking whether the specified voltage regulator is enabled.
 *
 * @param[in] p_reg     Pointer to the structure of registers of the peripheral.
 * @param[in] regulator Regulator to be checked.
 *
 * @retval true  Specified voltage regulator is enabled.
 * @retval false Specified voltage regulator is disabled.
 */
NRF_STATIC_INLINE bool nrf_regulators_vreg_enable_check(NRF_REGULATORS_Type const * p_reg,
                                                       nrf_regulators_vreg_t        regulator);
#endif // NRF_REGULATORS_HAS_VREG_ANY

/**
 * @brief Function for putting the CPU in System OFF mode.
 *
 * @note This function never returns.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_regulators_system_off(NRF_REGULATORS_Type * p_reg);

#if NRF_REGULATORS_HAS_MAIN_STATUS
/**
 * @brief Function for getting the main supply status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The current main supply status.
 */
NRF_STATIC_INLINE
nrf_regulators_main_status_t nrf_regulators_main_status_get(NRF_REGULATORS_Type const * p_reg);
#endif

#if NRF_REGULATORS_HAS_POF
/**
 * @brief Function for setting the POF Comparator configuration.
 *
 * @param[in] p_reg    Pointer to the structure of registers of the peripheral.
 * @param[in] p_config Pointer to the structure containing POF Comparator configuration.
 */
NRF_STATIC_INLINE void nrf_regulators_pof_config_set(NRF_REGULATORS_Type *               p_reg,
                                                     nrf_regulators_pof_config_t const * p_config);

/**
 * @brief Function for getting the POF Comparator configuration.
 *
 * @param[in]  p_reg    Pointer to the structure of registers of the peripheral.
 * @param[out] p_config Pointer to the structure to be filled with POF Comparator configuration.
 */
NRF_STATIC_INLINE void nrf_regulators_pof_config_get(NRF_REGULATORS_Type const *   p_reg,
                                                     nrf_regulators_pof_config_t * p_config);
#endif // NRF_REGULATORS_HAS_POF

#if NRF_REGULATORS_HAS_POF_STATUS
/**
 * @brief Function for checking if the detected voltage is below or above the threshold of VPOF (POF Comparator's threshold voltage).
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Voltage below VPOF threshold has been detected.
 * @retval false Voltage above VPOF threshold has been detected.
 */
NRF_STATIC_INLINE bool nrf_regulators_pof_below_thr_check(NRF_REGULATORS_Type const * p_reg);
#endif

#if NRF_REGULATORS_HAS_TRIM
/**
 * @brief Function for setting components that are allowed to introduce the ELV mode.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of components to be set, created using @ref nrf_regulators_elv_mode_allow_mask_t.
 */
NRF_STATIC_INLINE void nrf_regulators_elv_mode_allow_set(NRF_REGULATORS_Type * p_reg,
                                                         uint32_t              mask);

/**
 * @brief Function for geting components that are allowed to introduce the ELV mode.
 *
 * @warning This register is retained when retention is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask of components allowed to introduce ELV mode,
 *         created using @ref nrf_regulators_elv_mode_allow_mask_t.
 */
NRF_STATIC_INLINE uint32_t nrf_regulators_elv_mode_allow_get(NRF_REGULATORS_Type const * p_reg);
#endif // NRF_REGULATORS_HAS_TRIM

#if NRF_REGULATORS_HAS_INDUCTOR_DET
/**
 * @brief Function for checking whether an inductor is connected to the DCC pin.
 *
 * @note The detection can only take place if the VREG_MAIN DC/DC converter is not enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Inductor detected.
 * @retval false Inductor not detected.
 */
NRF_STATIC_INLINE bool nrf_regulators_inductor_check(NRF_REGULATORS_Type const * p_reg);
#endif

#if NRF_REGULATORS_HAS_HIBERNATOR
/**
 * @brief Function for putting the CPU in hibernation mode.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_regulators_system_hibernate(NRF_REGULATORS_Type * p_reg);

/**
 * @brief Function for enabling or disabling the GPIO retention release on wake up
 *        from hibernation mode.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if GPIO retention release is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_regulators_gpio_retention_release_set(NRF_REGULATORS_Type * p_reg,
                                                                 bool                  enable);
/**
 * @brief Function for checking the GPIO retention status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  GPIO retention is active.
 * @retval false GPIO retention is not active.
 */
NRF_STATIC_INLINE bool nrf_regulators_gpio_retention_check(NRF_REGULATORS_Type const * p_reg);
#endif

#ifndef NRF_DECLARE_ONLY

#if NRF_REGULATORS_HAS_VREG_ANY
NRF_STATIC_INLINE void nrf_regulators_vreg_enable_set(NRF_REGULATORS_Type * p_reg,
                                                      nrf_regulators_vreg_t regulator,
                                                      bool                  enable)
{
    switch (regulator)
    {
        case NRF_REGULATORS_VREG_MAIN:
#if defined(REGULATORS_DCDCEN_DCDCEN_Msk)
            p_reg->DCDCEN = (enable ? REGULATORS_DCDCEN_DCDCEN_Enabled :
                             REGULATORS_DCDCEN_DCDCEN_Disabled) << REGULATORS_DCDCEN_DCDCEN_Pos;
#elif defined(REGULATORS_VREGMAIN_DCDCEN_VAL_Msk)
            p_reg->VREGMAIN.DCDCEN = (enable ? REGULATORS_VREGMAIN_DCDCEN_VAL_Enabled :
                                      REGULATORS_VREGMAIN_DCDCEN_VAL_Disabled)
                                     << REGULATORS_VREGMAIN_DCDCEN_VAL_Pos;
#else
            p_reg->VREGMAIN.DCDCEN = (enable ? REGULATORS_VREGMAIN_DCDCEN_DCDCEN_Enabled :
                                      REGULATORS_VREGMAIN_DCDCEN_DCDCEN_Disabled)
                                     << REGULATORS_VREGMAIN_DCDCEN_DCDCEN_Pos;
#endif
            break;

#if NRF_REGULATORS_HAS_VREG_HIGH
        case NRF_REGULATORS_VREG_HIGH:
            p_reg->VREGH.DCDCEN = (enable ? REGULATORS_VREGH_DCDCEN_DCDCEN_Enabled :
                                   REGULATORS_VREGH_DCDCEN_DCDCEN_Disabled)
                                  << REGULATORS_VREGH_DCDCEN_DCDCEN_Pos;
            break;
#endif

#if NRF_REGULATORS_HAS_VREG_MEDIUM
        case NRF_REGULATORS_VREG_MEDIUM:
            p_reg->VREGM.ENABLE = (enable ? REGULATORS_VREGM_ENABLE_ENABLE_Enabled :
                                   REGULATORS_VREGM_ENABLE_ENABLE_Disabled)
                                  << REGULATORS_VREGM_ENABLE_ENABLE_Pos;
            break;
#endif

#if NRF_REGULATORS_HAS_VREG_RADIO
        case NRF_REGULATORS_VREG_RADIO:
            p_reg->VREGRADIO.DCDCEN = (enable ? REGULATORS_VREGRADIO_DCDCEN_DCDCEN_Enabled :
                                       REGULATORS_VREGRADIO_DCDCEN_DCDCEN_Disabled)
                                      << REGULATORS_VREGRADIO_DCDCEN_DCDCEN_Pos;
            break;
#endif

        default:
            NRFX_ASSERT(false);
            break;
    }
}

NRF_STATIC_INLINE bool nrf_regulators_vreg_enable_check(NRF_REGULATORS_Type const * p_reg,
                                                        nrf_regulators_vreg_t       regulator)
{
    switch (regulator)
    {
        case NRF_REGULATORS_VREG_MAIN:
#if defined(REGULATORS_DCDCEN_DCDCEN_Msk)
            return (p_reg->DCDCEN >> REGULATORS_DCDCEN_DCDCEN_Pos) ==
                   REGULATORS_DCDCEN_DCDCEN_Enabled;
#elif defined(REGULATORS_VREGMAIN_DCDCEN_VAL_Msk)
            return (p_reg->VREGMAIN.DCDCEN >> REGULATORS_VREGMAIN_DCDCEN_VAL_Pos) ==
                   REGULATORS_VREGMAIN_DCDCEN_VAL_Enabled;
#else
            return (p_reg->VREGMAIN.DCDCEN >> REGULATORS_VREGMAIN_DCDCEN_DCDCEN_Pos) ==
                    REGULATORS_VREGMAIN_DCDCEN_DCDCEN_Enabled;
#endif

#if NRF_REGULATORS_HAS_VREG_HIGH
        case NRF_REGULATORS_VREG_HIGH:
            return (p_reg->VREGH.DCDCEN >> REGULATORS_VREGH_DCDCEN_DCDCEN_Pos) ==
                   REGULATORS_VREGH_DCDCEN_DCDCEN_Enabled;
#endif

#if NRF_REGULATORS_HAS_VREG_MEDIUM
        case NRF_REGULATORS_VREG_MEDIUM:
            return (p_reg->VREGM.ENABLE >> REGULATORS_VREGM_ENABLE_ENABLE_Pos) ==
                   REGULATORS_VREGM_ENABLE_ENABLE_Enabled;
#endif

#if NRF_REGULATORS_HAS_VREG_RADIO
        case NRF_REGULATORS_VREG_RADIO:
            return (p_reg->VREGRADIO.DCDCEN >> REGULATORS_VREGRADIO_DCDCEN_DCDCEN_Pos) ==
                   REGULATORS_VREGRADIO_DCDCEN_DCDCEN_Enabled;
#endif

        default:
            NRFX_ASSERT(false);
            return false;
    }
}
#endif // NRF_REGULATORS_HAS_VREG_ANY

NRF_STATIC_INLINE void nrf_regulators_system_off(NRF_REGULATORS_Type * p_reg)
{
    p_reg->SYSTEMOFF = REGULATORS_SYSTEMOFF_SYSTEMOFF_Msk;
    __DSB();

    /* Solution for simulated System OFF in debug mode */
    while (true)
    {
        __WFE();
    }
}

#if NRF_REGULATORS_HAS_MAIN_STATUS
NRF_STATIC_INLINE
nrf_regulators_main_status_t nrf_regulators_main_status_get(NRF_REGULATORS_Type const * p_reg)
{
    return (nrf_regulators_main_status_t)p_reg->MAINREGSTATUS;
}
#endif

#if NRF_REGULATORS_HAS_POF
NRF_STATIC_INLINE void nrf_regulators_pof_config_set(NRF_REGULATORS_Type *               p_reg,
                                                     nrf_regulators_pof_config_t const * p_config)
{
    NRFX_ASSERT(p_config);

    p_reg->POFCON = ((p_config->enable ? REGULATORS_POFCON_POF_Enabled :
                      REGULATORS_POFCON_POF_Disabled) << REGULATORS_POFCON_POF_Pos) |
                    (((uint32_t)p_config->thr) << REGULATORS_POFCON_THRESHOLD_Pos) |
#if NRF_REGULATORS_HAS_POF_VDDH
                    (((uint32_t)p_config->thr_vddh) << REGULATORS_POFCON_THRESHOLDVDDH_Pos) |
#endif
#if NRF_REGULATORS_HAS_POF_WARN_DISABLE
                    ((p_config->warn_disable ? REGULATORS_POFCON_EVENTDISABLE_Disabled :
                      REGULATORS_POFCON_EVENTDISABLE_Enabled)
                     << REGULATORS_POFCON_EVENTDISABLE_Pos) |
#endif
                    0;
}

NRF_STATIC_INLINE void nrf_regulators_pof_config_get(NRF_REGULATORS_Type const *   p_reg,
                                                     nrf_regulators_pof_config_t * p_config)
{
    NRFX_ASSERT(p_config);

    p_config->enable = ((p_reg->POFCON & REGULATORS_POFCON_POF_Msk) >> REGULATORS_POFCON_POF_Pos)
                        == REGULATORS_POFCON_POF_Enabled;

    p_config->thr = (nrf_regulators_pof_thr_t)((p_reg->POFCON & REGULATORS_POFCON_THRESHOLD_Msk)
                                               >> REGULATORS_POFCON_THRESHOLD_Pos);


#if NRF_REGULATORS_HAS_POF_VDDH
    p_config->thr_vddh = (nrf_regulators_pof_thr_vddh_t)((p_reg->POFCON &
                                                          REGULATORS_POFCON_THRESHOLDVDDH_Msk)
                                                         >> REGULATORS_POFCON_THRESHOLDVDDH_Pos);
#endif

#if NRF_REGULATORS_HAS_POF_WARN_DISABLE
    p_config->warn_disable = ((p_reg->POFCON & REGULATORS_POFCON_EVENTDISABLE_Msk)
                              >> REGULATORS_POFCON_EVENTDISABLE_Pos)
                             == REGULATORS_POFCON_EVENTDISABLE_Disabled;
#endif
}
#endif // NRF_REGULATORS_HAS_POF

#if NRF_REGULATORS_HAS_POF_STATUS
NRF_STATIC_INLINE bool nrf_regulators_pof_below_thr_check(NRF_REGULATORS_Type const * p_reg)
{
    return (p_reg->POFSTAT & REGULATORS_POFSTAT_COMPARATOR_Msk) >> REGULATORS_POFSTAT_COMPARATOR_Pos
           == REGULATORS_POFSTAT_COMPARATOR_Below;
}
#endif

#if NRF_REGULATORS_HAS_TRIM
NRF_STATIC_INLINE void nrf_regulators_elv_mode_allow_set(NRF_REGULATORS_Type * p_reg,
                                                         uint32_t              mask)
{
    p_reg->TRIM = ((p_reg->TRIM & ~NRF_REGULATORS_ELV_MODE_ALL_MASK) |
                   (~mask & NRF_REGULATORS_ELV_MODE_ALL_MASK));
}

NRF_STATIC_INLINE uint32_t nrf_regulators_elv_mode_allow_get(NRF_REGULATORS_Type const * p_reg)
{
    return ~(p_reg->TRIM & NRF_REGULATORS_ELV_MODE_ALL_MASK);
}
#endif // NRF_REGULATORS_HAS_TRIM

#if NRF_REGULATORS_HAS_INDUCTOR_DET
NRF_STATIC_INLINE bool nrf_regulators_inductor_check(NRF_REGULATORS_Type const * p_reg)
{
    return (p_reg->VREGMAIN.INDUCTORDET & REGULATORS_VREGMAIN_INDUCTORDET_DETECTED_Msk)
           >> REGULATORS_VREGMAIN_INDUCTORDET_DETECTED_Pos
           == REGULATORS_VREGMAIN_INDUCTORDET_DETECTED_InductorDetected;
}
#endif

#if NRF_REGULATORS_HAS_HIBERNATOR
NRF_STATIC_INLINE void nrf_regulators_system_hibernate(NRF_REGULATORS_Type * p_reg)
{
    p_reg->HIBERNATOR.SYSTEMHIBERNATE =
        REGULATORS_HIBERNATOR_SYSTEMHIBERNATE_SYSTEMHIBERNATE_Enter;
}

NRF_STATIC_INLINE void nrf_regulators_gpio_retention_release_set(NRF_REGULATORS_Type * p_reg,
                                                                 bool                  enable)
{
    p_reg->HIBERNATOR.GPIORETENTIONRELEASE = (enable
            ? REGULATORS_HIBERNATOR_GPIORETENTIONRELEASE_GPIORETENTIONRELEASE_Release
            : REGULATORS_HIBERNATOR_GPIORETENTIONRELEASE_GPIORETENTIONRELEASE_NoRelease)
            << REGULATORS_HIBERNATOR_GPIORETENTIONRELEASE_GPIORETENTIONRELEASE_Pos;
}

NRF_STATIC_INLINE bool nrf_regulators_gpio_retention_check(NRF_REGULATORS_Type const * p_reg)
{
    return (p_reg->HIBERNATOR.GPIORETENTIONSTATUS
            & REGULATORS_HIBERNATOR_GPIORETENTIONSTATUS_GPIORETENTIONSTATUS_Msk)
            >> REGULATORS_HIBERNATOR_GPIORETENTIONSTATUS_GPIORETENTIONSTATUS_Pos
            == REGULATORS_HIBERNATOR_GPIORETENTIONSTATUS_GPIORETENTIONSTATUS_Active;
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_REGULATORS_H__
