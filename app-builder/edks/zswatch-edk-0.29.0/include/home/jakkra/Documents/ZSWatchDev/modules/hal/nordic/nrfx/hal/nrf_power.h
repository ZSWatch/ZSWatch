/*
 * Copyright (c) 2017 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_POWER_H__
#define NRF_POWER_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_power_hal POWER HAL
 * @{
 * @ingroup nrf_power
 * @brief   Hardware access layer for managing the POWER peripheral.
 */

#if defined(POWER_TASKS_CONSTLAT_TASKS_CONSTLAT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether Constant Latency mode is present. */
#define NRF_POWER_HAS_CONST_LATENCY 1
#else
#define NRF_POWER_HAS_CONST_LATENCY 0
#endif

#if defined(POWER_TASKS_LOWPWR_TASKS_LOWPWR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether Low-Power mode is present. */
#define NRF_POWER_HAS_LOW_POWER 1
#else
#define NRF_POWER_HAS_LOW_POWER 0
#endif

#if defined(POWER_INTENSET_SLEEPENTER_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether sleep events are present. */
#define NRF_POWER_HAS_SLEEPEVT 1
#else
#define NRF_POWER_HAS_SLEEPEVT 0
#endif

#if defined(POWER_USBREGSTATUS_VBUSDETECT_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the POWER peripheral controls the USB regulator. */
#define NRF_POWER_HAS_USBREG 1
#else
#define NRF_POWER_HAS_USBREG 0
#endif

#if defined(POWER_DCDCEN0_DCDCEN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether DCDCEN for REG0 is present. */
#define NRF_POWER_HAS_DCDCEN_VDDH 1
#else
#define NRF_POWER_HAS_DCDCEN_VDDH 0
#endif

#if defined(POWER_DCDCEN_DCDCEN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether DCDCEN for REG1 is present. */
#define NRF_POWER_HAS_DCDCEN 1
#else
#define NRF_POWER_HAS_DCDCEN 0
#endif

#if defined(POWER_INTENSET_POFWARN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether power failure event is present. */
#define NRF_POWER_HAS_POFWARN 1
#else
#define NRF_POWER_HAS_POFWARN 0
#endif

#if defined(POWER_POFCON_THRESHOLD_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether power failure comparator is present. */
#define NRF_POWER_HAS_POFCON 1
#else
#define NRF_POWER_HAS_POFCON 0
#endif

#if defined(POWER_POFCON_THRESHOLDVDDH_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether power failure comparator for VDDH is present. */
#define NRF_POWER_HAS_POFCON_VDDH 1
#else
#define NRF_POWER_HAS_POFCON_VDDH 0
#endif

#if defined(POWER_RESETREAS_RESETPIN_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RESETREAS register is present in POWER */
#define NRF_POWER_HAS_RESETREAS 1
#else
#define NRF_POWER_HAS_RESETREAS 0
#endif

#if defined(POWER_RESETREAS_CTRLAP_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RESETREAS CTRLAP is present. */
#define NRF_POWER_HAS_RESETREAS_CTRLAP 1
#else
#define NRF_POWER_HAS_RESETREAS_CTRLAP 0
#endif

#if defined(POWER_RESETREAS_LPCOMP_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RESETREAS LPCOMP is present. */
#define NRF_POWER_HAS_RESETREAS_LPCOMP 1
#else
#define NRF_POWER_HAS_RESETREAS_LPCOMP 0
#endif

#if defined(POWER_RESETREAS_NFC_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RESETREAS NFC is present. */
#define NRF_POWER_HAS_RESETREAS_NFC 1
#else
#define NRF_POWER_HAS_RESETREAS_NFC 0
#endif

#if defined(POWER_RESETREAS_VBUS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether RESETREAS VBUS is present. */
#define NRF_POWER_HAS_RESETREAS_VBUS 1
#else
#define NRF_POWER_HAS_RESETREAS_VBUS 0
#endif

#if defined(POWER_MAINREGSTATUS_MAINREGSTATUS_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether MAINREGSTATUS register is present. */
#define NRF_POWER_HAS_MAINREGSTATUS 1
#else
#define NRF_POWER_HAS_MAINREGSTATUS 0
#endif

#if defined(POWER_GPREGRET_GPREGRET_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GPREGRET register is present. */
#define NRF_POWER_HAS_GPREGRET 1
#else
#define NRF_POWER_HAS_GPREGRET 0
#endif

#if defined(POWER_GPREGRET_MaxCount) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GPREGRET register is treated as an array. */
#define NRF_POWER_HAS_GPREGRET_ARRAY 1
#else
#define NRF_POWER_HAS_GPREGRET_ARRAY 0
#endif

#if NRF_POWER_HAS_GPREGRET_ARRAY || defined(__NRFX_DOXYGEN__)
/** @brief Size of GPREGRET register when defined as array. */
#define NRFX_POWER_GPREGRET_COUNT POWER_GPREGRET_MaxCount
#endif

#if defined(POWER_TASKS_SEMAPHORE_ACQUIRE_ACQUIRE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether semaphore for regulator voltage scaling procedure is present. */
#define NRF_POWER_HAS_SEMAPHORE 1
#else
#define NRF_POWER_HAS_SEMAPHORE 0
#endif

#if (defined(POWER_TASKS_REGUPDATE_TASKS_REGUPDATE_Msk) && \
     defined(POWER_EVENTS_REGUPDATED_EVENTS_REGUPDATED_Msk)) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether task and event responsible for updating voltage regulators configuration are present. */
#define NRF_POWER_HAS_VREG_UPDATE_TASK_EVENT 1
#else
#define NRF_POWER_HAS_VREG_UPDATE_TASK_EVENT 0
#endif

#if defined(POWER_REGCONFIG_VREG1V8_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether voltage regulators are configurable. */
#define NRF_POWER_HAS_VREG_CONFIG 1
#else
#define NRF_POWER_HAS_VREG_CONFIG 0
#endif

#if defined(POWER_EVENTS_ABBLOCK_EVENTS_ABBLOCK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the Adaptive Body Biasing (ABB) domains are present. */
#define NRF_POWER_HAS_ABB 1
#else
#define NRF_POWER_HAS_ABB 0
#endif

#if defined(POWER_BLOCKULPMODE_BLOCK_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the power block modes are present. */
#define NRF_POWER_HAS_BLOCK_MODES 1
#else
#define NRF_POWER_HAS_BLOCK_MODES 0
#endif

#if defined(POWER_BILSENABLE_ENABLE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the Built-in Leakage Sensors (BILS) are present. */
#define NRF_POWER_HAS_BILS 1
#else
#define NRF_POWER_HAS_BILS 0
#endif

#if defined(POWER_PMICENABLE_ENABLE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the Power Management IC (PMIC) is present. */
#define NRF_POWER_HAS_PMIC 1
#else
#define NRF_POWER_HAS_PMIC 0
#endif

#if defined(POWER_RAM_POWER_S0POWER_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether the RAM power can be controlled here */
#define NRF_POWER_HAS_RAM_POWER 1
#else
#define NRF_POWER_HAS_RAM_POWER 0
#endif

#if NRF_POWER_HAS_ABB
/** @brief Symbol specifying the maximum number of available @p ABB_LOCK events. */
#define NRF_POWER_EVENTS_ABB_LOCK_COUNT POWER_EVENTS_ABBLOCK_MaxCount
#endif

/** @brief POWER tasks. */
typedef enum
{
#if NRF_POWER_HAS_CONST_LATENCY
    NRF_POWER_TASK_CONSTLAT          = offsetof(NRF_POWER_Type, TASKS_CONSTLAT),          ///< Enable constant latency mode.
#endif
#if NRF_POWER_HAS_LOW_POWER
    NRF_POWER_TASK_LOWPWR            = offsetof(NRF_POWER_Type, TASKS_LOWPWR),            ///< Enable low-power mode (variable latency).
#endif
#if NRF_POWER_HAS_SEMAPHORE
    NRF_POWER_TASK_SEMAPHORE_ACQUIRE = offsetof(NRF_POWER_Type, TASKS_SEMAPHORE.ACQUIRE), ///< Acquire the semaphore for regulator voltage scaling procedure.
    NRF_POWER_TASK_SEMAPHORE_RELEASE = offsetof(NRF_POWER_Type, TASKS_SEMAPHORE.RELEASE), ///< Release the semaphore for regulator voltage scaling procedure.
#endif
#if NRF_POWER_HAS_VREG_UPDATE_TASK_EVENT
    NRF_POWER_TASK_REGULATOR_UPDATE  = offsetof(NRF_POWER_Type, TASKS_REGUPDATE),         ///< Update the regulator configuration.
#endif
} nrf_power_task_t;

/** @brief POWER events. */
typedef enum
{
#if NRF_POWER_HAS_POFWARN
    NRF_POWER_EVENT_POFWARN            = offsetof(NRF_POWER_Type, EVENTS_POFWARN),            ///< Power failure warning.
#endif
#if NRF_POWER_HAS_SLEEPEVT
    NRF_POWER_EVENT_SLEEPENTER         = offsetof(NRF_POWER_Type, EVENTS_SLEEPENTER),         ///< CPU entered WFI/WFE sleep mode.
    NRF_POWER_EVENT_SLEEPEXIT          = offsetof(NRF_POWER_Type, EVENTS_SLEEPEXIT),          ///< CPU exited WFI/WFE sleep mode.
#endif
#if NRF_POWER_HAS_USBREG
    NRF_POWER_EVENT_USBDETECTED        = offsetof(NRF_POWER_Type, EVENTS_USBDETECTED),        ///< Voltage supply detected on VBUS.
    NRF_POWER_EVENT_USBREMOVED         = offsetof(NRF_POWER_Type, EVENTS_USBREMOVED),         ///< Voltage supply removed from VBUS.
    NRF_POWER_EVENT_USBPWRRDY          = offsetof(NRF_POWER_Type, EVENTS_USBPWRRDY),          ///< USB 3.3 V supply ready.
#endif
#if NRF_POWER_HAS_SEMAPHORE
    NRF_POWER_EVENT_SEMAPHORE_ACQUIRED = offsetof(NRF_POWER_Type, EVENTS_SEMAPHORE.ACQUIRED), ///< Acquired the semaphore for regulator voltage scaling procedure.
    NRF_POWER_EVENT_SEMAPHORE_RELEASED = offsetof(NRF_POWER_Type, EVENTS_SEMAPHORE.RELEASED), ///< Released the semaphore for regulator voltage scaling procedure.
#endif
#if NRF_POWER_HAS_VREG_UPDATE_TASK_EVENT
    NRF_POWER_EVENT_REGULATOR_UPDATED  = offsetof(NRF_POWER_Type, EVENTS_REGUPDATED),         ///< Updated the regulator configuration.
#endif
#if NRF_POWER_HAS_ABB
    NRF_POWER_EVENT_ABB_LOCK_0         = offsetof(NRF_POWER_Type, EVENTS_ABBLOCK[0]),         ///< ABB lock for the ABB domain 0.
    NRF_POWER_EVENT_ABB_LOCK_1         = offsetof(NRF_POWER_Type, EVENTS_ABBLOCK[1]),         ///< ABB lock for the ABB domain 1.
#endif
} nrf_power_event_t;

/** @brief POWER interrupts. */
typedef enum
{
#if NRF_POWER_HAS_POFWARN
    NRF_POWER_INT_POFWARN_MASK       = POWER_INTENSET_POFWARN_Msk,        ///< Write '1' to enable interrupt for POFWARN event.
#endif
#if NRF_POWER_HAS_SLEEPEVT
    NRF_POWER_INT_SLEEPENTER_MASK    = POWER_INTENSET_SLEEPENTER_Msk,     ///< Write '1' to enable interrupt for SLEEPENTER event.
    NRF_POWER_INT_SLEEPEXIT_MASK     = POWER_INTENSET_SLEEPEXIT_Msk,      ///< Write '1' to enable interrupt for SLEEPEXIT event.
#endif
#if NRF_POWER_HAS_USBREG
    NRF_POWER_INT_USBDETECTED_MASK   = POWER_INTENSET_USBDETECTED_Msk,    ///< Write '1' to enable interrupt for USBDETECTED event.
    NRF_POWER_INT_USBREMOVED_MASK    = POWER_INTENSET_USBREMOVED_Msk,     ///< Write '1' to enable interrupt for USBREMOVED event.
    NRF_POWER_INT_USBPWRRDY_MASK     = POWER_INTENSET_USBPWRRDY_Msk,      ///< Write '1' to enable interrupt for USBPWRRDY event.
#endif
#if NRF_POWER_HAS_SEMAPHORE
    NRF_POWER_INT_SEMAPHORE_ACQUIRED = POWER_INTEN_SEMAPHOREACQUIRED_Msk, ///< Write '1' to enable interrupt for SEMAPHORE_ACQUIRED event.
    NRF_POWER_INT_SEMAPHORE_RELEASED = POWER_INTEN_SEMAPHORERELEASED_Msk, ///< Write '1' to enable interrupt for SEMAPHORE_RELEASED event.
#endif
#if NRF_POWER_HAS_VREG_UPDATE_TASK_EVENT
    NRF_POWER_INT_REGULATOR_UPDATED  = POWER_INTEN_REGUPDATED_Msk,        ///< Write '1' to enable interrupt for REGULATOR_UPDATED event.
#endif
#if NRF_POWER_HAS_ABB
    NRF_POWER_INT_ABB_LOCK_0         = POWER_INTEN_ABBLOCK0_Msk,          ///< Write '1' to enable interrupt for ABB_LOCK_0 event.
    NRF_POWER_INT_ABB_LOCK_1         = POWER_INTEN_ABBLOCK1_Msk,          ///< Write '1' to enable interrupt for ABB_LOCK_1 event.
#endif
} nrf_power_int_mask_t;

/** @brief Reset reason. */
#if NRF_POWER_HAS_RESETREAS
typedef enum
{
    NRF_POWER_RESETREAS_RESETPIN_MASK = POWER_RESETREAS_RESETPIN_Msk, /**< Bit mask of RESETPIN field. */
    NRF_POWER_RESETREAS_DOG_MASK      = POWER_RESETREAS_DOG_Msk     , /**< Bit mask of DOG field. */
    NRF_POWER_RESETREAS_SREQ_MASK     = POWER_RESETREAS_SREQ_Msk    , /**< Bit mask of SREQ field. */
    NRF_POWER_RESETREAS_LOCKUP_MASK   = POWER_RESETREAS_LOCKUP_Msk  , /**< Bit mask of LOCKUP field. */
    NRF_POWER_RESETREAS_OFF_MASK      = POWER_RESETREAS_OFF_Msk     , /**< Bit mask of OFF field. */
#if NRF_POWER_HAS_RESETREAS_LPCOMP
    NRF_POWER_RESETREAS_LPCOMP_MASK   = POWER_RESETREAS_LPCOMP_Msk  , /**< Bit mask of LPCOMP field. */
#endif
    NRF_POWER_RESETREAS_DIF_MASK      = POWER_RESETREAS_DIF_Msk     , /**< Bit mask of DIF field. */
#if NRF_POWER_HAS_RESETREAS_NFC
    NRF_POWER_RESETREAS_NFC_MASK      = POWER_RESETREAS_NFC_Msk     , /**< Bit mask of NFC field. */
#endif
#if NRF_POWER_HAS_RESETREAS_VBUS
    NRF_POWER_RESETREAS_VBUS_MASK     = POWER_RESETREAS_VBUS_Msk    , /**< Bit mask of VBUS field. */
#endif
#if NRF_POWER_HAS_RESETREAS_CTRLAP
    NRF_POWER_RESETREAS_CTRLAP_MASK   = POWER_RESETREAS_CTRLAP_Msk  , /**< Bit mask of CTRLAP field. */
#endif
} nrf_power_resetreas_mask_t;
#endif // NRF_POWER_HAS_RESETREAS

#if NRF_POWER_HAS_USBREG
/**
 * @brief USBREGSTATUS register bit masks
 *
 * @sa nrf_power_usbregstatus_get
 */
typedef enum
{
    NRF_POWER_USBREGSTATUS_VBUSDETECT_MASK = POWER_USBREGSTATUS_VBUSDETECT_Msk, ///< USB detected or removed.
    NRF_POWER_USBREGSTATUS_OUTPUTRDY_MASK  = POWER_USBREGSTATUS_OUTPUTRDY_Msk   ///< USB 3.3 V supply ready.
} nrf_power_usbregstatus_mask_t;
#endif // NRF_POWER_HAS_USBREG

#if defined(POWER_RAMSTATUS_RAMBLOCK0_Msk) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Numbers of RAM blocks
 *
 * @sa nrf_power_ramblock_mask_t
 * @note
 * RAM blocks must be used in nRF51.
 * In newer SoCs, RAM is divided into segments and this functionality is not supported.
 * See the PS for mapping between the internal RAM and RAM blocks, because this
 * mapping is not 1:1, and functions related to old style blocks must not be used.
 */
typedef enum
{
    NRF_POWER_RAMBLOCK0 = POWER_RAMSTATUS_RAMBLOCK0_Pos,
    NRF_POWER_RAMBLOCK1 = POWER_RAMSTATUS_RAMBLOCK1_Pos,
#if defined(POWER_RAMSTATUS_RAMBLOCK2_Pos) ||  defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMBLOCK2 = POWER_RAMSTATUS_RAMBLOCK2_Pos,
#endif
#if defined(POWER_RAMSTATUS_RAMBLOCK3_Pos) ||  defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMBLOCK3 = POWER_RAMSTATUS_RAMBLOCK3_Pos
#endif
} nrf_power_ramblock_t;

/**
 * @brief Masks of RAM blocks.
 *
 * @sa nrf_power_ramblock_t
 */
typedef enum
{
    NRF_POWER_RAMBLOCK0_MASK = POWER_RAMSTATUS_RAMBLOCK0_Msk,
    NRF_POWER_RAMBLOCK1_MASK = POWER_RAMSTATUS_RAMBLOCK1_Msk,
#if defined(POWER_RAMSTATUS_RAMBLOCK2_Msk) ||  defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMBLOCK2_MASK = POWER_RAMSTATUS_RAMBLOCK2_Msk,
#endif
#if defined(POWER_RAMSTATUS_RAMBLOCK3_Msk) ||  defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMBLOCK3_MASK = POWER_RAMSTATUS_RAMBLOCK3_Msk
#endif
} nrf_power_ramblock_mask_t;
#endif // defined(POWER_RAMSTATUS_RAMBLOCK0_Msk) || defined(__NRFX_DOXYGEN__)

/**
 * @brief RAM power state position of the bits
 *
 * @sa nrf_power_onoffram_mask_t
 */
typedef enum
{
    NRF_POWER_ONRAM0,  /**< Keep RAM block 0 ON or OFF in System ON mode.                 */
    NRF_POWER_OFFRAM0, /**< Keep retention on RAM block 0 when RAM block is switched OFF. */
    NRF_POWER_ONRAM1,  /**< Keep RAM block 1 ON or OFF in System ON mode.                 */
    NRF_POWER_OFFRAM1, /**< Keep retention on RAM block 1 when RAM block is switched OFF. */
    NRF_POWER_ONRAM2,  /**< Keep RAM block 2 ON or OFF in System ON mode.                 */
    NRF_POWER_OFFRAM2, /**< Keep retention on RAM block 2 when RAM block is switched OFF. */
    NRF_POWER_ONRAM3,  /**< Keep RAM block 3 ON or OFF in System ON mode.                 */
    NRF_POWER_OFFRAM3, /**< Keep retention on RAM block 3 when RAM block is switched OFF. */
}nrf_power_onoffram_t;

/**
 * @brief RAM power state bit masks
 *
 * @sa nrf_power_onoffram_t
 */
typedef enum
{
    NRF_POWER_ONRAM0_MASK  = 1U << NRF_POWER_ONRAM0,  /**< Keep RAM block 0 ON or OFF in System ON mode.                 */
    NRF_POWER_OFFRAM0_MASK = 1U << NRF_POWER_OFFRAM0, /**< Keep retention on RAM block 0 when RAM block is switched OFF. */
    NRF_POWER_ONRAM1_MASK  = 1U << NRF_POWER_ONRAM1,  /**< Keep RAM block 1 ON or OFF in System ON mode.                 */
    NRF_POWER_OFFRAM1_MASK = 1U << NRF_POWER_OFFRAM1, /**< Keep retention on RAM block 1 when RAM block is switched OFF. */
    NRF_POWER_ONRAM2_MASK  = 1U << NRF_POWER_ONRAM2,  /**< Keep RAM block 2 ON or OFF in System ON mode.                 */
    NRF_POWER_OFFRAM2_MASK = 1U << NRF_POWER_OFFRAM2, /**< Keep retention on RAM block 2 when RAM block is switched OFF. */
    NRF_POWER_ONRAM3_MASK  = 1U << NRF_POWER_ONRAM3,  /**< Keep RAM block 3 ON or OFF in System ON mode.                 */
    NRF_POWER_OFFRAM3_MASK = 1U << NRF_POWER_OFFRAM3, /**< Keep retention on RAM block 3 when RAM block is switched OFF. */
}nrf_power_onoffram_mask_t;

#if NRF_POWER_HAS_POFCON
/** @brief Power failure comparator thresholds. */
typedef enum
{
    NRF_POWER_POFTHR_V21 = POWER_POFCON_THRESHOLD_V21, ///< Set threshold to 2.1 V.
    NRF_POWER_POFTHR_V23 = POWER_POFCON_THRESHOLD_V23, ///< Set threshold to 2.3 V.
    NRF_POWER_POFTHR_V25 = POWER_POFCON_THRESHOLD_V25, ///< Set threshold to 2.5 V.
    NRF_POWER_POFTHR_V27 = POWER_POFCON_THRESHOLD_V27, ///< Set threshold to 2.7 V.
#if defined(POWER_POFCON_THRESHOLD_V17) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_POFTHR_V17 = POWER_POFCON_THRESHOLD_V17, ///< Set threshold to 1.7 V.
    NRF_POWER_POFTHR_V18 = POWER_POFCON_THRESHOLD_V18, ///< Set threshold to 1.8 V.
    NRF_POWER_POFTHR_V19 = POWER_POFCON_THRESHOLD_V19, ///< Set threshold to 1.9 V.
    NRF_POWER_POFTHR_V20 = POWER_POFCON_THRESHOLD_V20, ///< Set threshold to 2.0 V.
    NRF_POWER_POFTHR_V22 = POWER_POFCON_THRESHOLD_V22, ///< Set threshold to 2.2 V.
    NRF_POWER_POFTHR_V24 = POWER_POFCON_THRESHOLD_V24, ///< Set threshold to 2.4 V.
    NRF_POWER_POFTHR_V26 = POWER_POFCON_THRESHOLD_V26, ///< Set threshold to 2.6 V.
    NRF_POWER_POFTHR_V28 = POWER_POFCON_THRESHOLD_V28, ///< Set threshold to 2.8 V.
#endif // defined(POWER_POFCON_THRESHOLD_V17) || defined(__NRFX_DOXYGEN__)
} nrf_power_pof_thr_t;
#endif // NRF_POWER_HAS_POFCON

#if NRF_POWER_HAS_POFCON_VDDH
/** @brief Power failure comparator thresholds for VDDH. */
typedef enum
{
    NRF_POWER_POFTHRVDDH_V27 = POWER_POFCON_THRESHOLDVDDH_V27, ///< Set threshold to 2.7 V.
    NRF_POWER_POFTHRVDDH_V28 = POWER_POFCON_THRESHOLDVDDH_V28, ///< Set threshold to 2.8 V.
    NRF_POWER_POFTHRVDDH_V29 = POWER_POFCON_THRESHOLDVDDH_V29, ///< Set threshold to 2.9 V.
    NRF_POWER_POFTHRVDDH_V30 = POWER_POFCON_THRESHOLDVDDH_V30, ///< Set threshold to 3.0 V.
    NRF_POWER_POFTHRVDDH_V31 = POWER_POFCON_THRESHOLDVDDH_V31, ///< Set threshold to 3.1 V.
    NRF_POWER_POFTHRVDDH_V32 = POWER_POFCON_THRESHOLDVDDH_V32, ///< Set threshold to 3.2 V.
    NRF_POWER_POFTHRVDDH_V33 = POWER_POFCON_THRESHOLDVDDH_V33, ///< Set threshold to 3.3 V.
    NRF_POWER_POFTHRVDDH_V34 = POWER_POFCON_THRESHOLDVDDH_V34, ///< Set threshold to 3.4 V.
    NRF_POWER_POFTHRVDDH_V35 = POWER_POFCON_THRESHOLDVDDH_V35, ///< Set threshold to 3.5 V.
    NRF_POWER_POFTHRVDDH_V36 = POWER_POFCON_THRESHOLDVDDH_V36, ///< Set threshold to 3.6 V.
    NRF_POWER_POFTHRVDDH_V37 = POWER_POFCON_THRESHOLDVDDH_V37, ///< Set threshold to 3.7 V.
    NRF_POWER_POFTHRVDDH_V38 = POWER_POFCON_THRESHOLDVDDH_V38, ///< Set threshold to 3.8 V.
    NRF_POWER_POFTHRVDDH_V39 = POWER_POFCON_THRESHOLDVDDH_V39, ///< Set threshold to 3.9 V.
    NRF_POWER_POFTHRVDDH_V40 = POWER_POFCON_THRESHOLDVDDH_V40, ///< Set threshold to 4.0 V.
    NRF_POWER_POFTHRVDDH_V41 = POWER_POFCON_THRESHOLDVDDH_V41, ///< Set threshold to 4.1 V.
    NRF_POWER_POFTHRVDDH_V42 = POWER_POFCON_THRESHOLDVDDH_V42, ///< Set threshold to 4.2 V.
} nrf_power_pof_thrvddh_t;
#endif // NRF_POWER_HAS_POFCON_VDDH

#if NRF_POWER_HAS_MAINREGSTATUS
/** @brief Main regulator status. */
typedef enum
{
    NRF_POWER_MAINREGSTATUS_NORMAL = POWER_MAINREGSTATUS_MAINREGSTATUS_Normal, /**< Normal voltage mode. Voltage supplied on VDD. */
    NRF_POWER_MAINREGSTATUS_HIGH   = POWER_MAINREGSTATUS_MAINREGSTATUS_High    /**< High voltage mode. Voltage supplied on VDDH.  */
} nrf_power_mainregstatus_t;
#endif

#if NRF_POWER_HAS_RAM_POWER
/** @brief Position of power configuration bits for RAM section 0. */
#define NRF_POWER_RAMPOWER_S0POWER_POS POWER_RAM_POWER_S0POWER_Pos

/** @brief Position of retention configuration bits for RAM section 0. */
#define NRF_POWER_RAMPOWER_S0RETENTION_POS POWER_RAM_POWER_S0RETENTION_Pos

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
/**
 * @brief Bit masks for RAMPOWER register
 *
 * All possible bits described, even if they are not used in selected MCU.
 */
typedef enum
{
    NRF_POWER_RAMPOWER_S0POWER_MASK  = POWER_RAM_POWER_S0POWER_Msk,
    NRF_POWER_RAMPOWER_S1POWER_MASK  = POWER_RAM_POWER_S1POWER_Msk,
#if defined(POWER_RAM_POWER_S2POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S2POWER_MASK  = POWER_RAM_POWER_S2POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S3POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S3POWER_MASK  = POWER_RAM_POWER_S3POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S4POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S4POWER_MASK  = POWER_RAM_POWER_S4POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S5POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S5POWER_MASK  = POWER_RAM_POWER_S5POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S6POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S6POWER_MASK  = POWER_RAM_POWER_S6POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S7POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S7POWER_MASK  = POWER_RAM_POWER_S7POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S8POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S8POWER_MASK  = POWER_RAM_POWER_S8POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S9POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S9POWER_MASK  = POWER_RAM_POWER_S9POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S10POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S10POWER_MASK = POWER_RAM_POWER_S10POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S11POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S11POWER_MASK = POWER_RAM_POWER_S11POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S12POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S12POWER_MASK = POWER_RAM_POWER_S12POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S13POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S13POWER_MASK = POWER_RAM_POWER_S13POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S14POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S14POWER_MASK = POWER_RAM_POWER_S14POWER_Msk,
#endif
#if defined(POWER_RAM_POWER_S15POWER_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S15POWER_MASK = POWER_RAM_POWER_S15POWER_Msk,
#endif

    NRF_POWER_RAMPOWER_S0RETENTION_MASK  = POWER_RAM_POWER_S0RETENTION_Msk,
    NRF_POWER_RAMPOWER_S1RETENTION_MASK  = POWER_RAM_POWER_S1RETENTION_Msk,
#if defined(POWER_RAM_POWER_S2RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S2RETENTION_MASK  = POWER_RAM_POWER_S2RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S3RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S3RETENTION_MASK  = POWER_RAM_POWER_S3RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S4RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S4RETENTION_MASK  = POWER_RAM_POWER_S4RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S5RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S5RETENTION_MASK  = POWER_RAM_POWER_S5RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S6RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S6RETENTION_MASK  = POWER_RAM_POWER_S6RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S7RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S7RETENTION_MASK  = POWER_RAM_POWER_S7RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S8RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S8RETENTION_MASK  = POWER_RAM_POWER_S8RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S9RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S9RETENTION_MASK  = POWER_RAM_POWER_S9RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S10RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S10RETENTION_MASK = POWER_RAM_POWER_S10RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S11RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S11RETENTION_MASK = POWER_RAM_POWER_S11RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S12RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S12RETENTION_MASK = POWER_RAM_POWER_S12RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S13RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S13RETENTION_MASK = POWER_RAM_POWER_S13RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S14RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S14RETENTION_MASK = POWER_RAM_POWER_S14RETENTION_Msk,
#endif
#if defined(POWER_RAM_POWER_S15RETENTION_Msk) || defined(__NRFX_DOXYGEN__)
    NRF_POWER_RAMPOWER_S15RETENTION_MASK = POWER_RAM_POWER_S15RETENTION_Msk,
#endif
} nrf_power_rampower_mask_t;
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#endif // NRF_POWER_HAS_RAM_POWER

#if NRF_POWER_HAS_VREG_CONFIG
/** @brief POWER voltage regulators bit masks. */
typedef enum
{
    NRF_POWER_VREG_1V8_MASK     = POWER_REGCONFIG_VREG1V8_Msk,     ///< 1.8 V regulator.
    NRF_POWER_VREG_1V0_MASK     = POWER_REGCONFIG_VREG1V0_Msk,     ///< 1.0 V regulator.
    NRF_POWER_VREG_0V8_MASK     = POWER_REGCONFIG_VREG0V8_Msk,     ///< 0.8 V regulator.
    NRF_POWER_VREG_VS_MASK      = POWER_REGCONFIG_VREGVS_Msk,      ///< Voltage scaled regulator.
    NRF_POWER_VREG_MAIN1V8_MASK = POWER_REGCONFIG_VREGMAIN1V8_Msk, ///< 1.8 V rail at VREGMAIN regulator.
    NRF_POWER_VREG_MAIN1V0_MASK = POWER_REGCONFIG_VREGMAIN1V0_Msk, ///< 1.0 V rail at VREGMAIN regulator.
    NRF_POWER_VREG_MAINVS_MASK  = POWER_REGCONFIG_VREGMAINVS_Msk,  ///< Voltage scaled rail at VREGMAIN regulator.
    NRF_POWER_VREG_FORCE_MASK   = POWER_REGCONFIG_FORCE_Msk,       ///< Force the regulator enable configuration.
} nrf_power_vreg_mask_t;
#endif // NRF_POWER_HAS_VREG_CONFIG

#if NRF_POWER_HAS_ABB
/** @brief POWER operating points for ABB domain. */
typedef enum
{
    NRF_POWER_OP_POINT_0V4 = POWER_ABB_OPPOINT_OPPOINT_OpPoint0V4, ///< Operating point 0.4 V.
    NRF_POWER_OP_POINT_0V5 = POWER_ABB_OPPOINT_OPPOINT_OpPoint0V5, ///< Operating point 0.5 V.
    NRF_POWER_OP_POINT_0V6 = POWER_ABB_OPPOINT_OPPOINT_OpPoint0V6, ///< Operating point 0.6 V.
    NRF_POWER_OP_POINT_0V8 = POWER_ABB_OPPOINT_OPPOINT_OpPoint0V8, ///< Operating point 0.8 V.
} nrf_power_op_point_t;

/** @brief POWER operating points for ABB domain. */
typedef enum
{
    NRF_POWER_OVERRIDE_VALUE_POWER_DOWN = POWER_ABB_OPPOINT_ABBPWROVERRIDEVAL_PowerDown, ///< ABB analog macro powered down.
    NRF_POWER_OVERRIDE_VALUE_POWER_UP   = POWER_ABB_OPPOINT_ABBPWROVERRIDEVAL_PowerUp,   ///< ABB analog macro powered up.
} nrf_power_override_value_t;

/** @brief POWER operating point for ABB domain structure. */
typedef struct
{
    nrf_power_op_point_t       op_point;        ///< ABB operating point.
    nrf_power_override_value_t override_value;  ///< Override value of ABB analog macro powerup signal.
                                                /**< Value is applied only if @p override_enable is enabled. */
    bool                       override_enable; ///< True if the override of ABB analog macro signal is to be applied, false otherwise.
} nrf_power_abb_config_t;
#endif // NRF_POWER_HAS_ABB

/**
 * @brief Function for activating a specific POWER task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task.
 */
NRF_STATIC_INLINE void nrf_power_task_trigger(NRF_POWER_Type * p_reg, nrf_power_task_t task);

/**
 * @brief Function for returning the address of a specific POWER task register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task.
 *
 * @return Task address.
 */
NRF_STATIC_INLINE uint32_t nrf_power_task_address_get(NRF_POWER_Type const * p_reg,
                                                      nrf_power_task_t       task);

/**
 * @brief Function for clearing a specific event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event.
 */
NRF_STATIC_INLINE void nrf_power_event_clear(NRF_POWER_Type * p_reg, nrf_power_event_t event);

/**
 * @brief Function for retrieving the state of the POWER event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRF_STATIC_INLINE bool nrf_power_event_check(NRF_POWER_Type const * p_reg, nrf_power_event_t event);

/**
 * @brief Function for getting and clearing the state of specific event
 *
 * This function checks the state of the event and clears it.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event.
 *
 * @retval true  The event was set.
 * @retval false The event was not set.
 */
NRF_STATIC_INLINE bool nrf_power_event_get_and_clear(NRF_POWER_Type *  p_reg,
                                                     nrf_power_event_t event);

/**
 * @brief Function for returning the address of a specific POWER event register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event.
 *
 * @return Address.
 */
NRF_STATIC_INLINE uint32_t nrf_power_event_address_get(NRF_POWER_Type const * p_reg,
                                                       nrf_power_event_t      event);

/**
 * @brief Function for enabling selected interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be enabled.
 *                  Use @ref nrf_power_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_power_int_enable(NRF_POWER_Type * p_reg, uint32_t mask);

/**
 * @brief Function for checking if the specified interrupts are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be checked.
 *                  Use @ref nrf_power_int_mask_t values for bit masking.
 *
 * @return Mask of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_power_int_enable_check(NRF_POWER_Type const * p_reg, uint32_t mask);

/**
 * @brief Function for retrieving the information about enabled interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The flags of enabled interrupts.
 */
NRF_STATIC_INLINE uint32_t nrf_power_int_enable_get(NRF_POWER_Type const * p_reg);

/**
 * @brief Function for disabling selected interrupts.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of interrupts to be disabled.
 *                  Use @ref nrf_power_int_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_power_int_disable(NRF_POWER_Type * p_reg, uint32_t mask);

#if defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the subscribe configuration for a given
 *        POWER task.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] task    Task for which to set the configuration.
 * @param[in] channel Channel through which to subscribe events.
 */
NRF_STATIC_INLINE void nrf_power_subscribe_set(NRF_POWER_Type * p_reg,
                                               nrf_power_task_t task,
                                               uint8_t          channel);

/**
 * @brief Function for clearing the subscribe configuration for a given
 *        POWER task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_power_subscribe_clear(NRF_POWER_Type * p_reg, nrf_power_task_t task);

/**
 * @brief Function for getting the subscribe configuration for a given
 *        POWER task.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] task  Task for which to read the configuration.
 *
 * @return POWER subscribe configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_power_subscribe_get(NRF_POWER_Type const * p_reg,
                                                   nrf_power_task_t       task);

/**
 * @brief Function for setting the publish configuration for a given
 *        POWER event.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] event   Event for which to set the configuration.
 * @param[in] channel Channel through which to publish the event.
 */
NRF_STATIC_INLINE void nrf_power_publish_set(NRF_POWER_Type *  p_reg,
                                             nrf_power_event_t event,
                                             uint8_t           channel);

/**
 * @brief Function for clearing the publish configuration for a given
 *        POWER event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to clear the configuration.
 */
NRF_STATIC_INLINE void nrf_power_publish_clear(NRF_POWER_Type * p_reg, nrf_power_event_t event);

/**
 * @brief Function for getting the publish configuration for a given
 *        POWER event.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] event Event for which to read the configuration.
 *
 * @return POWER publish configuration.
 */
NRF_STATIC_INLINE uint32_t nrf_power_publish_get(NRF_POWER_Type const * p_reg,
                                                 nrf_power_event_t      event);
#endif // defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

#if NRF_POWER_HAS_RESETREAS
/**
 * @brief Function for getting the reset reason bitmask.
 *
 * This function returns the reset reason bitmask.
 * Unless cleared, the RESETREAS register is cumulative.
 * A field is cleared by writing '1' to it (see @ref nrf_power_resetreas_clear).
 * If none of the reset sources is flagged,
 * the chip was reset from the on-chip reset generator,
 * which indicates a power-on-reset or a brown out reset.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The mask of reset reasons constructed with @ref nrf_power_resetreas_mask_t.
 */
NRF_STATIC_INLINE uint32_t nrf_power_resetreas_get(NRF_POWER_Type const * p_reg);

/**
 * @brief Function for clearing the selected reset reason field.
 *
 * This function clears the selected reset reason field.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  The mask constructed from @ref nrf_power_resetreas_mask_t enumerator values.
 *
 * @sa nrf_power_resetreas_get
 */
NRF_STATIC_INLINE void nrf_power_resetreas_clear(NRF_POWER_Type * p_reg, uint32_t mask);
#endif // NRF_POWER_HAS_RESETREAS

#if defined(POWER_POWERSTATUS_LTEMODEM_Msk) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for getting power status of the LTE Modem domain.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  The LTE Modem domain is powered on.
 * @retval false The LTE Modem domain is powered off.
 */
NRF_STATIC_INLINE bool nrf_power_powerstatus_get(NRF_POWER_Type const * p_reg);
#endif

#if defined(POWER_RAMSTATUS_RAMBLOCK0_Msk) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for getting the RAMSTATUS register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Value with bits set according to the masks in @ref nrf_power_ramblock_mask_t.
 */
NRF_STATIC_INLINE uint32_t nrf_power_ramstatus_get(NRF_POWER_Type const * p_reg);
#endif // defined(POWER_RAMSTATUS_RAMBLOCK0_Msk) || defined(__NRFX_DOXYGEN__)

#if defined(POWER_SYSTEMOFF_SYSTEMOFF_Enter)
/**
 * @brief Function for going into System OFF mode.
 *
 * This function puts the CPU in System OFF mode.
 * The only way to wake up the CPU is by reset.
 *
 * @note This function never returns.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 */
NRF_STATIC_INLINE void nrf_power_system_off(NRF_POWER_Type * p_reg);
#endif // defined(POWER_SYSTEMOFF_SYSTEMOFF_Enter)

#if NRF_POWER_HAS_POFCON
/**
 * @brief Function for setting the power failure comparator configuration.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if the power failure comparator is to be enabled, false otherwise.
 * @param[in] thr    Voltage threshold value.
 */
NRF_STATIC_INLINE void nrf_power_pofcon_set(NRF_POWER_Type *    p_reg,
                                            bool                enable,
                                            nrf_power_pof_thr_t thr);

/**
 * @brief Function for getting the power failure comparator configuration.
 *
 * @param[in]  p_reg     Pointer to the structure of registers of the peripheral.
 * @param[out] p_enabled Function sets this boolean variable to true
 *                       if power failure comparator is enabled.
 *                       The pointer can be NULL if we do not need this information.
 *
 * @return Threshold setting for power failure comparator.
 */
NRF_STATIC_INLINE nrf_power_pof_thr_t nrf_power_pofcon_get(NRF_POWER_Type const * p_reg,
                                                           bool *                 p_enabled);
#endif // NRF_POWER_HAS_POFCON

#if NRF_POWER_HAS_POFCON_VDDH
/**
 * @brief Function for setting the VDDH power failure comparator threshold.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] thr   Threshold to be set.
 */
NRF_STATIC_INLINE void nrf_power_pofcon_vddh_set(NRF_POWER_Type *        p_reg,
                                                 nrf_power_pof_thrvddh_t thr);

/**
 * @brief Function for getting the VDDH power failure comparator threshold.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return VDDH threshold currently configured.
 */
NRF_STATIC_INLINE nrf_power_pof_thrvddh_t nrf_power_pofcon_vddh_get(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_POFCON_VDDH

#if NRF_POWER_HAS_GPREGRET
/**
 * @brief Function for setting the general purpose retention register.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] reg_num General purpose retention register number.
 * @param[in] val     Value to be set in the register.
 */
NRF_STATIC_INLINE void nrf_power_gpregret_set(NRF_POWER_Type * p_reg,
                                              uint32_t         reg_num,
                                              uint32_t         val);

/**
 * @brief Function for getting general purpose retention register.
 *
 * @param[in] p_reg   Pointer to the structure of registers of the peripheral.
 * @param[in] reg_num General purpose retention register number.
 *
 * @return Value from the register.
 */
NRF_STATIC_INLINE uint32_t nrf_power_gpregret_get(NRF_POWER_Type const * p_reg, uint32_t reg_num);
#endif // NRF_POWER_HAS_GPREGRET

#if NRF_POWER_HAS_DCDCEN
/**
 * @brief Enable or disable DCDC converter
 *
 * @note If the device consist of high voltage power input (VDDH), this setting
 *       will relate to the converter on low voltage side (1.3 V output).
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if DCDC converter is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_power_dcdcen_set(NRF_POWER_Type * p_reg, bool enable);

/**
 * @brief Function for getting the state of the DCDC converter.
 *
 * @note If the device consist of high voltage power input (VDDH), this setting
 *       will relate to the converter on low voltage side (1.3 V output).
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Converter is enabled.
 * @retval false Converter is disabled.
 */
NRF_STATIC_INLINE bool nrf_power_dcdcen_get(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_DCDCEN

#if NRF_POWER_HAS_RAM_POWER
/**
 * @brief Turn ON sections in the selected RAM block.
 *
 * This function turns ON several sections in one block and also block retention.
 *
 * @sa nrf_power_rampower_mask_t
 * @sa nrf_power_rampower_mask_off
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] block        RAM block index.
 * @param[in] section_mask Mask of the sections created by merging
 *                         @ref nrf_power_rampower_mask_t flags.
 */
NRF_STATIC_INLINE void nrf_power_rampower_mask_on(NRF_POWER_Type * p_reg,
                                                  uint8_t          block,
                                                  uint32_t         section_mask);

/**
 * @brief Turn ON sections in the selected RAM block.
 *
 * This function turns OFF several sections in one block and also block retention.
 *
 * @sa nrf_power_rampower_mask_t
 * @sa nrf_power_rampower_mask_off
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] block        RAM block index.
 * @param[in] section_mask Mask of the sections created by merging
 *                         @ref nrf_power_rampower_mask_t flags.
 */
NRF_STATIC_INLINE void nrf_power_rampower_mask_off(NRF_POWER_Type * p_reg,
                                                   uint8_t          block,
                                                   uint32_t         section_mask);

/**
 * @brief Function for getting the ON mask and retention sections in the selected RAM block.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] block RAM block index.
 *
 * @return Mask of sections state composed from @ref nrf_power_rampower_mask_t flags.
 */
NRF_STATIC_INLINE uint32_t nrf_power_rampower_mask_get(NRF_POWER_Type const * p_reg, uint8_t block);
#endif /* NRF_POWER_HAS_RAM_POWER */

#if NRF_POWER_HAS_DCDCEN_VDDH
/**
 * @brief Function for enabling or disabling the DCDC converter on VDDH.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if DCDC converter on VDDH is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_power_dcdcen_vddh_set(NRF_POWER_Type * p_reg, bool enable);

/**
 * @brief Function for getting the state of DCDC converter on VDDH.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Converter is enabled.
 * @retval false Converter is disabled.
 */
NRF_STATIC_INLINE bool nrf_power_dcdcen_vddh_get(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_DCDCEN_VDDH

#if NRF_POWER_HAS_MAINREGSTATUS
/**
 * @brief Function for getting the main supply status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The current main supply status.
 */
NRF_STATIC_INLINE
nrf_power_mainregstatus_t nrf_power_mainregstatus_get(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_MAINREGSTATUS

#if NRF_POWER_HAS_USBREG
/**
 * @brief Function for getting the whole USBREGSTATUS register.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return The USBREGSTATUS register value.
 *         Use @ref nrf_power_usbregstatus_mask_t values for bit masking.
 *
 * @sa nrf_power_usbregstatus_vbusdet_get
 * @sa nrf_power_usbregstatus_outrdy_get
 */
NRF_STATIC_INLINE uint32_t nrf_power_usbregstatus_get(NRF_POWER_Type const * p_reg);

/**
 * @brief Function for getting the VBUS input detection status.
 *
 * USBDETECTED and USBREMOVED events are derived from this information
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval false VBUS voltage below valid threshold.
 * @retval true  VBUS voltage above valid threshold.
 *
 * @sa nrf_power_usbregstatus_get
 */
NRF_STATIC_INLINE bool nrf_power_usbregstatus_vbusdet_get(NRF_POWER_Type const * p_reg);

/**
 * @brief Function for getting the state of the elapsed time for the USB supply output settling.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval false USBREG output settling time not elapsed.
 * @retval true  USBREG output settling time elapsed
 *               (same information as USBPWRRDY event).
 *
 * @sa nrf_power_usbregstatus_get
 */
NRF_STATIC_INLINE bool nrf_power_usbregstatus_outrdy_get(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_USBREG

#if NRF_POWER_HAS_ABB
/**
 * @brief Function for checking whether the specified ABB domain is busy.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] domain_idx Index of ABB domain.
 *
 * @retval true  The ABB is busy with applying the new operating point.
 * @retval false The ABB is ready to accept the new operating point.
 */
NRF_STATIC_INLINE bool nrf_power_abb_busy_check(NRF_POWER_Type const * p_reg, uint8_t domain_idx);

/**
 * @brief Function for setting configuration of the operating point for the specified ABB domain.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] domain_idx Index of ABB domain.
 * @param[in] p_config   Pointer to the structure with configuration to be set.
 */
NRF_STATIC_INLINE void nrf_power_abb_config_set(NRF_POWER_Type *               p_reg,
                                                uint8_t                        domain_idx,
                                                nrf_power_abb_config_t const * p_config);

/**
 * @brief Function for getting configuration of the operating point for the specified ABB domain.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] domain_idx Index of ABB domain.
 * @param[in] p_config   Pointer to the structure with configuration to be set.
 */
NRF_STATIC_INLINE void nrf_power_abb_config_get(NRF_POWER_Type const *   p_reg,
                                                uint8_t                  domain_idx,
                                                nrf_power_abb_config_t * p_config);

/**
 * @brief Function for setting the force lock for the specified ABB domain.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] domain_idx Index of ABB domain.
 * @param[in] enable     True if force lock is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_power_abb_force_lock_set(NRF_POWER_Type * p_reg,
                                                    uint8_t          domain_idx,
                                                    bool             enable);

/**
 * @brief Function for checking if the force lock for the specified ABB domain is enabled.
 *
 * @param[in] p_reg      Pointer to the structure of registers of the peripheral.
 * @param[in] domain_idx Index of ABB domain whose status is checked.
 *
 * @retval true  Force lock is enabled.
 * @retval false Force lock is disabled.
 */
NRF_STATIC_INLINE bool nrf_power_abb_force_lock_check(NRF_POWER_Type const * p_reg,
                                                      uint8_t                domain_idx);
#endif // NRF_POWER_HAS_ABB

#if NRF_POWER_HAS_VREG_CONFIG
/**
 * @brief Function for setting the enabled voltage regulators.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of enabled voltage regulators to be set.
 *                  Use @ref nrf_power_vreg_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_power_vreg_set(NRF_POWER_Type * p_reg, uint32_t mask);

/**
 * @brief Function for getting mask of enabled voltage regulators.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Mask of enabled voltage regulators.
 */
NRF_STATIC_INLINE uint32_t nrf_power_vreg_get(NRF_POWER_Type const * p_reg);

/**
 * @brief Function for enabling the specified voltage regulators.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of voltage regulators to be enabled.
 *                  Use @ref nrf_power_vreg_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_power_vreg_or_set(NRF_POWER_Type * p_reg, uint32_t mask);

/**
 * @brief Function for disabling the specified voltage regulators.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 * @param[in] mask  Mask of voltage regulators to be disabled.
 *                  Use @ref nrf_power_vreg_mask_t values for bit masking.
 */
NRF_STATIC_INLINE void nrf_power_vreg_clear_set(NRF_POWER_Type * p_reg, uint32_t mask);
#endif // NRF_POWER_HAS_VREG_CONFIG

#if NRF_POWER_HAS_BLOCK_MODES
/**
 * @brief Function for setting the Ultra Low Power (ULP) mode.
 *
 * @note Going into ULP mode is allowed only if this mode is enabled - otherwise it is blocked.
 *       If the ULV mode is blocked, the ULP mode is also blocked.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if ULP mode is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_power_ulp_mode_set(NRF_POWER_Type * p_reg, bool enable);

/**
 * @brief Function for checking if the ULP mode is enabled.
 *
 * @note Going into ULP mode is allowed only if this mode is enabled - otherwise it is blocked.
 *       If the ULV mode is blocked, the ULP mode is also blocked.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  ULP mode is enabled.
 * @retval false ULP mode is disabled.
 */
NRF_STATIC_INLINE bool nrf_power_ulp_mode_check(NRF_POWER_Type const * p_reg);

/**
 * @brief Function for setting the Ultra Low Voltage (ULV) mode.
 *
 * @note Going into ULP mode is allowed only if this mode is enabled - otherwise it is blocked.
 *       If the ULV mode is blocked, the ULP mode is also blocked.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if ULV mode is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_power_ulv_mode_set(NRF_POWER_Type * p_reg, bool enable);

/**
 * @brief Function for checking if the ULV mode is enabled.
 *
 * @note Going into ULP mode is allowed only if this mode is enabled - otherwise it is blocked.
 *       If the ULV mode is blocked, the ULP mode is also blocked.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  ULV mode is enabled.
 * @retval false ULV mode is disabled.
 */
NRF_STATIC_INLINE bool nrf_power_ulv_mode_check(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_BLOCK_MODES

#if NRF_POWER_HAS_SEMAPHORE
/**
 * @brief Function for getting the POWER semaphore status.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  Semaphore is acquired.
 * @retval false Semaphore is released.
 */
NRF_STATIC_INLINE bool nrf_power_sem_status_get(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_SEMAPHORE

#if NRF_POWER_HAS_BILS
/**
 * @brief Function for setting BILS instances.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if all configured BILS instances are to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_power_bils_set(NRF_POWER_Type * p_reg, bool enable);

/**
 * @brief Function for checking if BILS instances are enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  All configured BILS instances are enabled.
 * @retval false All BILS instances are disabled.
 */
NRF_STATIC_INLINE bool nrf_power_bils_check(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_BILS

#if NRF_POWER_HAS_PMIC
/**
 * @brief Function for setting the PMIC interface.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] enable True if PMIC interface is to be enabled, false otherwise.
 */
NRF_STATIC_INLINE void nrf_power_pmic_set(NRF_POWER_Type * p_reg, bool enable);

/**
 * @brief Function for checking if the PMIC interface is enabled.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @retval true  PMIC interface is enabled.
 * @retval false PMIC interface is disabled.
 */
NRF_STATIC_INLINE bool nrf_power_pmic_check(NRF_POWER_Type const * p_reg);
#endif // NRF_POWER_HAS_PMIC

#ifndef NRF_DECLARE_ONLY

NRF_STATIC_INLINE void nrf_power_task_trigger(NRF_POWER_Type * p_reg, nrf_power_task_t task)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)task)) = 0x1UL;
}

NRF_STATIC_INLINE uint32_t nrf_power_task_address_get(NRF_POWER_Type const * p_reg,
                                                      nrf_power_task_t       task)
{
    return ((uint32_t)p_reg + (uint32_t)task);
}

NRF_STATIC_INLINE void nrf_power_event_clear(NRF_POWER_Type * p_reg, nrf_power_event_t event)
{
    *((volatile uint32_t *)((uint8_t *)p_reg + (uint32_t)event)) = 0x0UL;
    nrf_event_readback((uint8_t *)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE bool nrf_power_event_check(NRF_POWER_Type const * p_reg, nrf_power_event_t event)
{
    return nrf_event_check(p_reg, event);
}

NRF_STATIC_INLINE bool nrf_power_event_get_and_clear(NRF_POWER_Type *  p_reg,
                                                     nrf_power_event_t event)
{
    bool ret = nrf_power_event_check(p_reg, event);
    if (ret)
    {
        nrf_power_event_clear(p_reg, event);
    }
    return ret;
}

NRF_STATIC_INLINE uint32_t nrf_power_event_address_get(NRF_POWER_Type const * p_reg,
                                                       nrf_power_event_t      event)
{
    return ((uint32_t)p_reg + (uint32_t)event);
}

NRF_STATIC_INLINE void nrf_power_int_enable(NRF_POWER_Type * p_reg, uint32_t mask)
{
    p_reg->INTENSET = mask;
}

NRF_STATIC_INLINE uint32_t nrf_power_int_enable_check(NRF_POWER_Type const * p_reg, uint32_t mask)
{
    return p_reg->INTENSET & mask;
}

NRF_STATIC_INLINE uint32_t nrf_power_int_enable_get(NRF_POWER_Type const * p_reg)
{
    return p_reg->INTENSET;
}

NRF_STATIC_INLINE void nrf_power_int_disable(NRF_POWER_Type * p_reg, uint32_t mask)
{
    p_reg->INTENCLR = mask;
}

#if defined(DPPI_PRESENT)
NRF_STATIC_INLINE void nrf_power_subscribe_set(NRF_POWER_Type * p_reg,
                                               nrf_power_task_t task,
                                               uint8_t          channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_power_subscribe_clear(NRF_POWER_Type * p_reg, nrf_power_task_t task)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) task + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_power_subscribe_get(NRF_POWER_Type const * p_reg,
                                                   nrf_power_task_t       task)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) task + 0x80uL));
}

NRF_STATIC_INLINE void nrf_power_publish_set(NRF_POWER_Type *  p_reg,
                                             nrf_power_event_t event,
                                             uint8_t           channel)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) =
            ((uint32_t)channel | NRF_SUBSCRIBE_PUBLISH_ENABLE);
}

NRF_STATIC_INLINE void nrf_power_publish_clear(NRF_POWER_Type * p_reg, nrf_power_event_t event)
{
    *((volatile uint32_t *) ((uint8_t *) p_reg + (uint32_t) event + 0x80uL)) = 0;
}

NRF_STATIC_INLINE uint32_t nrf_power_publish_get(NRF_POWER_Type const * p_reg,
                                                 nrf_power_event_t      event)
{
    return *((volatile uint32_t const *) ((uint8_t const *) p_reg + (uint32_t) event + 0x80uL));
}
#endif // defined(DPPI_PRESENT)

#if NRF_POWER_HAS_RESETREAS
NRF_STATIC_INLINE uint32_t nrf_power_resetreas_get(NRF_POWER_Type const * p_reg)
{
    return p_reg->RESETREAS;
}

NRF_STATIC_INLINE void nrf_power_resetreas_clear(NRF_POWER_Type * p_reg, uint32_t mask)
{
    p_reg->RESETREAS = mask;
}
#endif // NRF_POWER_HAS_RESETREAS

#if defined(POWER_POWERSTATUS_LTEMODEM_Msk)
NRF_STATIC_INLINE bool nrf_power_powerstatus_get(NRF_POWER_Type const * p_reg)
{
    return (p_reg->POWERSTATUS & POWER_POWERSTATUS_LTEMODEM_Msk) ==
           (POWER_POWERSTATUS_LTEMODEM_ON << POWER_POWERSTATUS_LTEMODEM_Pos);
}
#endif // (POWER_POWERSTATUS_LTEMODEM_Msk)

#if defined(POWER_RAMSTATUS_RAMBLOCK0_Msk)
NRF_STATIC_INLINE uint32_t nrf_power_ramstatus_get(NRF_POWER_Type const * p_reg)
{
    return p_reg->RAMSTATUS;
}
#endif // defined(POWER_RAMSTATUS_RAMBLOCK0_Msk)

#if defined(POWER_SYSTEMOFF_SYSTEMOFF_Enter)
NRF_STATIC_INLINE void nrf_power_system_off(NRF_POWER_Type * p_reg)
{
    p_reg->SYSTEMOFF = POWER_SYSTEMOFF_SYSTEMOFF_Enter;
    __DSB();

    /* Solution for simulated System OFF in debug mode */
    while (true)
    {
        __WFE();
    }
}
#endif // defined(POWER_SYSTEMOFF_SYSTEMOFF_Enter)

#if NRF_POWER_HAS_POFCON
NRF_STATIC_INLINE void nrf_power_pofcon_set(NRF_POWER_Type *    p_reg,
                                            bool                enable,
                                            nrf_power_pof_thr_t thr)
{
    NRFX_ASSERT(thr == (thr & (POWER_POFCON_THRESHOLD_Msk >> POWER_POFCON_THRESHOLD_Pos)));
#if NRF_POWER_HAS_POFCON_VDDH
    uint32_t pofcon = p_reg->POFCON;
    pofcon &= ~(POWER_POFCON_THRESHOLD_Msk | POWER_POFCON_POF_Msk);
    pofcon |=
#else // NRF_POWER_HAS_POFCON_VDDH
    p_reg->POFCON =
#endif
        (((uint32_t)thr) << POWER_POFCON_THRESHOLD_Pos) |
        (enable ?
        (POWER_POFCON_POF_Enabled << POWER_POFCON_POF_Pos)
        :
        (POWER_POFCON_POF_Disabled << POWER_POFCON_POF_Pos));
#if NRF_POWER_HAS_POFCON_VDDH
    p_reg->POFCON = pofcon;
#endif
}

NRF_STATIC_INLINE nrf_power_pof_thr_t nrf_power_pofcon_get(NRF_POWER_Type const * p_reg,
                                                           bool *                 p_enabled)
{
    uint32_t pofcon = p_reg->POFCON;
    if (NULL != p_enabled)
    {
        (*p_enabled) = ((pofcon & POWER_POFCON_POF_Msk) >> POWER_POFCON_POF_Pos)
            == POWER_POFCON_POF_Enabled;
    }
    return (nrf_power_pof_thr_t)((pofcon & POWER_POFCON_THRESHOLD_Msk) >>
        POWER_POFCON_THRESHOLD_Pos);
}
#endif // NRF_POWER_HAS_POFCON

#if NRF_POWER_HAS_POFCON_VDDH
NRF_STATIC_INLINE void nrf_power_pofcon_vddh_set(NRF_POWER_Type *        p_reg,
                                                 nrf_power_pof_thrvddh_t thr)
{
    NRFX_ASSERT(thr == (thr & (POWER_POFCON_THRESHOLDVDDH_Msk >> POWER_POFCON_THRESHOLDVDDH_Pos)));
    uint32_t pofcon = p_reg->POFCON;
    pofcon &= ~POWER_POFCON_THRESHOLDVDDH_Msk;
    pofcon |= (((uint32_t)thr) << POWER_POFCON_THRESHOLDVDDH_Pos);
    p_reg->POFCON = pofcon;
}

NRF_STATIC_INLINE nrf_power_pof_thrvddh_t nrf_power_pofcon_vddh_get(NRF_POWER_Type const * p_reg)
{
    return (nrf_power_pof_thrvddh_t)((p_reg->POFCON & POWER_POFCON_THRESHOLDVDDH_Msk) >>
                                     POWER_POFCON_THRESHOLDVDDH_Pos);
}
#endif // NRF_POWER_HAS_POFCON_VDDH

#if NRF_POWER_HAS_GPREGRET
NRF_STATIC_INLINE void nrf_power_gpregret_set(NRF_POWER_Type * p_reg,
                                              uint32_t         reg_num,
                                              uint32_t         val)
{
#if NRF_POWER_HAS_GPREGRET_ARRAY
    NRFX_ASSERT(reg_num < NRFX_POWER_GPREGRET_COUNT);
#elif defined(POWER_GPREGRET2_GPREGRET_Msk)
    NRFX_ASSERT(reg_num < 2);
#else
    NRFX_ASSERT(reg_num < 1);
#endif

#if NRF_POWER_HAS_GPREGRET_ARRAY
    p_reg->GPREGRET[reg_num] = (val & POWER_GPREGRET_GPREGRET_Msk) << POWER_GPREGRET_GPREGRET_Pos;
#else
    switch (reg_num)
    {
        case 0:
            p_reg->GPREGRET = (val & POWER_GPREGRET_GPREGRET_Msk) << POWER_GPREGRET_GPREGRET_Pos;
            break;
#if defined(POWER_GPREGRET2_GPREGRET_Msk)
        case 1:
            p_reg->GPREGRET2 = (val & POWER_GPREGRET2_GPREGRET_Msk) << POWER_GPREGRET2_GPREGRET_Pos;
            break;
#endif
        default:
            break;
    }
#endif
}

NRF_STATIC_INLINE uint32_t nrf_power_gpregret_get(NRF_POWER_Type const * p_reg, uint32_t reg_num)
{
#if NRF_POWER_HAS_GPREGRET_ARRAY
    NRFX_ASSERT(reg_num < NRFX_POWER_GPREGRET_COUNT);
#elif defined(POWER_GPREGRET2_GPREGRET_Msk)
    NRFX_ASSERT(reg_num < 2);
#else
    NRFX_ASSERT(reg_num < 1);
#endif

#if NRF_POWER_HAS_GPREGRET_ARRAY
    return p_reg->GPREGRET[reg_num];
#else
    switch (reg_num)
    {
        case 0:
            return p_reg->GPREGRET;
#if defined(POWER_GPREGRET2_GPREGRET_Msk)
        case 1:
            return p_reg->GPREGRET2;
#endif
        default:
            return 0;
    }
#endif
}
#endif // NRF_POWER_HAS_GPREGRET

#if NRF_POWER_HAS_DCDCEN
NRF_STATIC_INLINE void nrf_power_dcdcen_set(NRF_POWER_Type * p_reg, bool enable)
{
    p_reg->DCDCEN = (enable ? POWER_DCDCEN_DCDCEN_Enabled : POWER_DCDCEN_DCDCEN_Disabled) <<
                    POWER_DCDCEN_DCDCEN_Pos;
}

NRF_STATIC_INLINE bool nrf_power_dcdcen_get(NRF_POWER_Type const * p_reg)
{
    return (p_reg->DCDCEN & POWER_DCDCEN_DCDCEN_Msk)
            ==
           (POWER_DCDCEN_DCDCEN_Enabled << POWER_DCDCEN_DCDCEN_Pos);
}
#endif // NRF_POWER_HAS_DCDCEN

#if NRF_POWER_HAS_RAM_POWER
NRF_STATIC_INLINE void nrf_power_rampower_mask_on(NRF_POWER_Type * p_reg,
                                                  uint8_t          block,
                                                  uint32_t         section_mask)
{
    p_reg->RAM[block].POWERSET = section_mask;
}

NRF_STATIC_INLINE void nrf_power_rampower_mask_off(NRF_POWER_Type * p_reg,
                                                   uint8_t          block,
                                                   uint32_t         section_mask)
{
    p_reg->RAM[block].POWERCLR = section_mask;
}

NRF_STATIC_INLINE uint32_t nrf_power_rampower_mask_get(NRF_POWER_Type const * p_reg, uint8_t block)
{
    return p_reg->RAM[block].POWER;
}
#endif // NRF_POWER_HAS_RAM_POWER

#if NRF_POWER_HAS_DCDCEN_VDDH
NRF_STATIC_INLINE void nrf_power_dcdcen_vddh_set(NRF_POWER_Type * p_reg, bool enable)
{
    if (enable && NRF_ERRATA_DYNAMIC_CHECK(52, 197))
    {
        *(volatile uint32_t *)0x40000638ul = 1ul;
    }
    p_reg->DCDCEN0 = (enable ? POWER_DCDCEN0_DCDCEN_Enabled : POWER_DCDCEN0_DCDCEN_Disabled) <<
                     POWER_DCDCEN0_DCDCEN_Pos;
}

NRF_STATIC_INLINE bool nrf_power_dcdcen_vddh_get(NRF_POWER_Type const * p_reg)
{
    return (p_reg->DCDCEN0 & POWER_DCDCEN0_DCDCEN_Msk)
            ==
           (POWER_DCDCEN0_DCDCEN_Enabled << POWER_DCDCEN0_DCDCEN_Pos);
}
#endif // NRF_POWER_HAS_DCDCEN_VDDH

#if NRF_POWER_HAS_MAINREGSTATUS
NRF_STATIC_INLINE
nrf_power_mainregstatus_t nrf_power_mainregstatus_get(NRF_POWER_Type const * p_reg)
{
    return (nrf_power_mainregstatus_t)(((p_reg->MAINREGSTATUS) &
        POWER_MAINREGSTATUS_MAINREGSTATUS_Msk) >>
        POWER_MAINREGSTATUS_MAINREGSTATUS_Pos);
}
#endif // NRF_POWER_HAS_MAINREGSTATUS

#if NRF_POWER_HAS_USBREG
NRF_STATIC_INLINE uint32_t nrf_power_usbregstatus_get(NRF_POWER_Type const * p_reg)
{
    return p_reg->USBREGSTATUS;
}

NRF_STATIC_INLINE bool nrf_power_usbregstatus_vbusdet_get(NRF_POWER_Type const * p_reg)
{
    return (nrf_power_usbregstatus_get(p_reg) & NRF_POWER_USBREGSTATUS_VBUSDETECT_MASK) != 0;
}

NRF_STATIC_INLINE bool nrf_power_usbregstatus_outrdy_get(NRF_POWER_Type const * p_reg)
{
    return (nrf_power_usbregstatus_get(p_reg) & NRF_POWER_USBREGSTATUS_OUTPUTRDY_MASK) != 0;
}
#endif // NRF_POWER_HAS_USBREG

#if NRF_POWER_HAS_ABB
NRF_STATIC_INLINE bool nrf_power_abb_busy_check(NRF_POWER_Type const * p_reg, uint8_t domain_idx)
{
    return ((p_reg->ABB[domain_idx].STATUS & POWER_ABB_STATUS_STATUS_Msk) ==
            (POWER_ABB_STATUS_STATUS_Busy << POWER_ABB_STATUS_STATUS_Pos));
}

NRF_STATIC_INLINE void nrf_power_abb_config_set(NRF_POWER_Type *               p_reg,
                                                uint8_t                        domain_idx,
                                                nrf_power_abb_config_t const * p_config)
{
    p_reg->ABB[domain_idx].OPPOINT = ((p_config->op_point <<
                                       POWER_ABB_OPPOINT_OPPOINT_Pos) &
                                      POWER_ABB_OPPOINT_OPPOINT_Msk) |
                                     ((p_config->override_value <<
                                       POWER_ABB_OPPOINT_ABBPWROVERRIDEVAL_Pos) &
                                      POWER_ABB_OPPOINT_ABBPWROVERRIDEVAL_Msk) |
                                     ((p_config->override_enable ?
                                       POWER_ABB_OPPOINT_ABBPWROVERRIDEEN_Enabled :
                                       POWER_ABB_OPPOINT_ABBPWROVERRIDEEN_Disabled) <<
                                      POWER_ABB_OPPOINT_ABBPWROVERRIDEEN_Pos);
}

NRF_STATIC_INLINE void nrf_power_abb_config_get(NRF_POWER_Type const *   p_reg,
                                                uint8_t                  domain_idx,
                                                nrf_power_abb_config_t * p_config)
{
    p_config->op_point = (nrf_power_op_point_t)((p_reg->ABB[domain_idx].OPPOINT &
                                                 POWER_ABB_OPPOINT_OPPOINT_Msk) >>
                                                POWER_ABB_OPPOINT_OPPOINT_Pos);

    p_config->override_value = (nrf_power_override_value_t)
                                ((p_reg->ABB[domain_idx].OPPOINT &
                                  POWER_ABB_OPPOINT_ABBPWROVERRIDEVAL_Msk) >>
                                 POWER_ABB_OPPOINT_ABBPWROVERRIDEVAL_Pos);

    p_config->override_enable = ((p_reg->ABB[domain_idx].OPPOINT &
                                  POWER_ABB_OPPOINT_ABBPWROVERRIDEEN_Msk) ==
                                 (POWER_ABB_OPPOINT_ABBPWROVERRIDEEN_Enabled <<
                                  POWER_ABB_OPPOINT_ABBPWROVERRIDEEN_Pos));
}

NRF_STATIC_INLINE void nrf_power_abb_force_lock_set(NRF_POWER_Type * p_reg,
                                                    uint8_t          domain_idx,
                                                    bool             enable)
{
    p_reg->ABB[domain_idx].FORCELOCK = ((enable ?
                                         POWER_ABB_FORCELOCK_ENABLE_Enabled :
                                         POWER_ABB_FORCELOCK_ENABLE_Disabled) <<
                                        POWER_ABB_FORCELOCK_ENABLE_Pos);
}

NRF_STATIC_INLINE bool nrf_power_abb_force_lock_check(NRF_POWER_Type const * p_reg,
                                                      uint8_t                domain_idx)
{
    return (p_reg->ABB[domain_idx].FORCELOCK & POWER_ABB_FORCELOCK_ENABLE_Msk) ==
           (POWER_ABB_FORCELOCK_ENABLE_Enabled << POWER_ABB_FORCELOCK_ENABLE_Pos);
}
#endif // NRF_POWER_HAS_ABB

#if NRF_POWER_HAS_VREG_CONFIG
NRF_STATIC_INLINE void nrf_power_vreg_set(NRF_POWER_Type * p_reg, uint32_t mask)
{
    p_reg->REGCONFIG = mask;
}

NRF_STATIC_INLINE uint32_t nrf_power_vreg_get(NRF_POWER_Type const * p_reg)
{
    return p_reg->REGCONFIG;
}

NRF_STATIC_INLINE void nrf_power_vreg_or_set(NRF_POWER_Type * p_reg, uint32_t mask)
{
    p_reg->REGCONFIG |= mask;
}

NRF_STATIC_INLINE void nrf_power_vreg_clear_set(NRF_POWER_Type * p_reg, uint32_t mask)
{
    p_reg->REGCONFIG &= (~mask);
}

#endif // NRF_POWER_HAS_VREG_CONFIG

#if NRF_POWER_HAS_BLOCK_MODES
NRF_STATIC_INLINE void nrf_power_ulp_mode_set(NRF_POWER_Type * p_reg, bool enable)
{
    p_reg->BLOCKULPMODE = (enable ? POWER_BLOCKULPMODE_BLOCK_Allowed :
                           POWER_BLOCKULPMODE_BLOCK_Blocked) <<
                          POWER_BLOCKULPMODE_BLOCK_Pos;
}

NRF_STATIC_INLINE bool nrf_power_ulp_mode_check(NRF_POWER_Type const * p_reg)
{
    return (p_reg->BLOCKULPMODE & POWER_BLOCKULPMODE_BLOCK_Msk) ==
           (POWER_BLOCKULPMODE_BLOCK_Allowed << POWER_BLOCKULPMODE_BLOCK_Pos);
}

NRF_STATIC_INLINE void nrf_power_ulv_mode_set(NRF_POWER_Type * p_reg, bool enable)
{
    p_reg->BLOCKULVMODE = (enable ? POWER_BLOCKULVMODE_BLOCK_Allowed :
                           POWER_BLOCKULVMODE_BLOCK_Blocked) <<
                          POWER_BLOCKULVMODE_BLOCK_Pos;
}

NRF_STATIC_INLINE bool nrf_power_ulv_mode_check(NRF_POWER_Type const * p_reg)
{
    return (p_reg->BLOCKULVMODE & POWER_BLOCKULVMODE_BLOCK_Msk) ==
           (POWER_BLOCKULVMODE_BLOCK_Allowed << POWER_BLOCKULVMODE_BLOCK_Pos);
}
#endif // NRF_POWER_HAS_BLOCK_MODES

#if NRF_POWER_HAS_SEMAPHORE
NRF_STATIC_INLINE bool nrf_power_sem_status_get(NRF_POWER_Type const * p_reg)
{
    return (p_reg->SEMAPHORESTATUS & POWER_SEMAPHORESTATUS_STATUS_Msk) ==
           (POWER_SEMAPHORESTATUS_STATUS_Acquired << POWER_SEMAPHORESTATUS_STATUS_Pos);
}
#endif // NRF_POWER_HAS_SEMAPHORE

#if NRF_POWER_HAS_BILS
NRF_STATIC_INLINE void nrf_power_bils_set(NRF_POWER_Type * p_reg, bool enable)
{
    p_reg->BILSENABLE = (enable ? POWER_BILSENABLE_ENABLE_Enabled :
                         POWER_BILSENABLE_ENABLE_Disabled) <<
                        POWER_BILSENABLE_ENABLE_Pos;
}

NRF_STATIC_INLINE bool nrf_power_bils_check(NRF_POWER_Type const * p_reg)
{
    return (p_reg->BILSENABLE & POWER_BILSENABLE_ENABLE_Msk) ==
           (POWER_BILSENABLE_ENABLE_Enabled << POWER_BILSENABLE_ENABLE_Pos);
}
#endif // NRF_POWER_HAS_BILS

#if NRF_POWER_HAS_PMIC
NRF_STATIC_INLINE void nrf_power_pmic_set(NRF_POWER_Type * p_reg, bool enable)
{
    p_reg->PMICENABLE = (enable ? POWER_PMICENABLE_ENABLE_Enabled :
                         POWER_PMICENABLE_ENABLE_Disabled) <<
                        POWER_PMICENABLE_ENABLE_Pos;
}

NRF_STATIC_INLINE bool nrf_power_pmic_check(NRF_POWER_Type const * p_reg)
{
    return (p_reg->PMICENABLE & POWER_PMICENABLE_ENABLE_Msk) ==
           (POWER_PMICENABLE_ENABLE_Enabled << POWER_PMICENABLE_ENABLE_Pos);
}
#endif // NRF_POWER_HAS_PMIC

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_POWER_H__
