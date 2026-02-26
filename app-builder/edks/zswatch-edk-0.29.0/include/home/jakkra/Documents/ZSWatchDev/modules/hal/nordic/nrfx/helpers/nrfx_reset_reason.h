/*
 * Copyright (c) 2020 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_RESET_REASON_H
#define NRFX_RESET_REASON_H

#include <nrfx.h>

#if defined(NRF_POWER)
#include <hal/nrf_power.h>

#if !NRF_POWER_HAS_RESETREAS
#include <hal/nrf_reset.h>
#endif

#elif defined(NRF_RESETINFO)
#include <hal/nrf_resetinfo.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_reset_reason Generic Reset Reason layer
 * @{
 * @ingroup nrfx
 * @ingroup nrf_power
 * @ingroup nrf_reset
 * @ingroup nrf_resetinfo
 *
 * @brief Helper layer that provides a uniform way of checking the reset reason.
 */

#if defined(NRF_RESETINFO) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Symbol specifying the offset of local reset reasons in the reset reason bitmask.
 *
 * @details Local reset reasons occupy highest bits.
 */
#define NRFX_RESET_REASON_LOCAL_OFFSET (31UL - RESETINFO_RESETREAS_LOCAL_UNRETAINEDWAKE_Pos)
#endif

#if (defined(NRF_POWER) && NRF_POWER_HAS_RESETREAS_CTRLAP) || \
    (defined(NRF_RESET) && NRF_RESET_HAS_CTRLAP_RESET) ||     \
    (defined(NRF_RESETINFO)) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CTRAP reset reason is present. */
#define NRFX_RESET_REASON_HAS_CTRLAP 1
#else
#define NRFX_RESET_REASON_HAS_CTRLAP 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_LSREQ_RESET) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether LSREQ reset reason is present. */
#define NRFX_RESET_REASON_HAS_LSREQ 1
#else
#define NRFX_RESET_REASON_HAS_LSREQ 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_LCTRLAP_RESET) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether LCTRLAP reset reason is present. */
#define NRFX_RESET_REASON_HAS_LCTRLAP 1
#else
#define NRFX_RESET_REASON_HAS_LCTRLAP 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_LDOG_RESET) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether LDOG reset reason is present. */
#define NRFX_RESET_REASON_HAS_LDOG 1
#else
#define NRFX_RESET_REASON_HAS_LDOG 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_LLOCKUP_RESET) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether LLOCKUP reset reason is present. */
#define NRFX_RESET_REASON_HAS_LLOCKUP 1
#else
#define NRFX_RESET_REASON_HAS_LLOCKUP 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_MFORCEOFF_RESET) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether MFORCEOFF reset reason is present. */
#define NRFX_RESET_REASON_HAS_MFORCEOFF 1
#else
#define NRFX_RESET_REASON_HAS_MFORCEOFF 0
#endif

#if (defined(NRF_POWER) && NRF_POWER_HAS_RESETREAS_LPCOMP) ||      \
    (defined(NRF_RESET) && defined(RESET_RESETREAS_LPCOMP_Msk)) || \
    (defined(NRF_RESETINFO)) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether LPCOMP reset reason is present. */
#define NRFX_RESET_REASON_HAS_LPCOMP 1
#else
#define NRFX_RESET_REASON_HAS_LPCOMP 0
#endif

#if (defined(NRF_POWER) && NRF_POWER_HAS_RESETREAS_NFC) ||      \
    (defined(NRF_RESET) && NRF_RESET_HAS_NFC_RESET) || \
    (defined(NRF_RESETINFO)) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether NFC reset reason is present. */
#define NRFX_RESET_REASON_HAS_NFC 1
#else
#define NRFX_RESET_REASON_HAS_NFC 0
#endif

#if (defined(NRF_POWER) && NRF_POWER_HAS_RESETREAS_VBUS) || \
    (defined(NRF_RESET) && NRF_RESET_HAS_VBUS_RESET) ||     \
    (defined(NRF_RESETINFO)) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether VBUS reset reason is present. */
#define NRFX_RESET_REASON_HAS_VBUS 1
#else
#define NRFX_RESET_REASON_HAS_VBUS 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_CTRLAPSOFT_RESET) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CTRL-AP reset reason is present. */
#define NRFX_RESET_REASON_HAS_CTRLAPSOFT 1
#else
#define NRFX_RESET_REASON_HAS_CTRLAPSOFT 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_CTRLAPHARD_RESET) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CTRL-AP hard reset reason is present. */
#define NRFX_RESET_REASON_HAS_CTRLAPHARD 1
#else
#define NRFX_RESET_REASON_HAS_CTRLAPHARD 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_CTRLAPPIN_RESET) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether CTRL-AP pin reset reason is present. */
#define NRFX_RESET_REASON_HAS_CTRLAPPIN 1
#else
#define NRFX_RESET_REASON_HAS_CTRLAPPIN 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_GRTC_RESET) || \
    (defined(NRF_RESETINFO)) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether GRTC reset reason is present. */
#define NRFX_RESET_REASON_HAS_GRTC 1
#else
#define NRFX_RESET_REASON_HAS_GRTC 0
#endif

#if (defined(NRF_RESET) && NRF_RESET_HAS_SECTAMPER_RESET) || \
    (defined(NRF_RESETINFO)) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether SECTAMPER reset reason is present. */
#define NRFX_RESET_REASON_HAS_SECTAMPER 1
#else
#define NRFX_RESET_REASON_HAS_SECTAMPER 0
#endif

/** @brief Reset reason bit masks. */
typedef enum
{
#if defined(NRF_RESET) || defined(__NRFX_DOXYGEN__)
    /**< Reset from pin-reset detected. */
    NRFX_RESET_REASON_RESETPIN_MASK     = NRF_RESET_RESETREAS_RESETPIN_MASK,
    /**< Reset from watchdog/application watchdog timer 0 detected. */
    NRFX_RESET_REASON_DOG0_MASK         = NRF_RESET_RESETREAS_DOG0_MASK,
    /**< Reset from watchdog/application watchdog timer 0 detected. */
    NRFX_RESET_REASON_DOG_MASK          = NRF_RESET_RESETREAS_DOG0_MASK,
#if NRFX_RESET_REASON_HAS_CTRLAP
    /**< Reset from application CTRL-AP detected. */
    NRFX_RESET_REASON_CTRLAP_MASK       = NRF_RESET_RESETREAS_CTRLAP_MASK,
#endif
    /**< Reset from soft reset/application soft reset detected. */
    NRFX_RESET_REASON_SREQ_MASK         = NRF_RESET_RESETREAS_SREQ_MASK,
    /**< Reset from CPU lockup/application CPU lockup detected. */
    NRFX_RESET_REASON_LOCKUP_MASK       = NRF_RESET_RESETREAS_LOCKUP_MASK,
    /**< Reset due to wakeup from System OFF mode when wakeup is triggered by DETECT signal from
     *   GPIO. */
    NRFX_RESET_REASON_OFF_MASK          = NRF_RESET_RESETREAS_OFF_MASK,
    /**< Reset due to wakeup from System OFF mode when wakeup is triggered by ANADETECT signal from
     *   LPCOMP. */
    NRFX_RESET_REASON_LPCOMP_MASK       = NRF_RESET_RESETREAS_LPCOMP_MASK,
    /**< Reset due to wakeup from System OFF mode when wakeup is triggered by entering the debug
     *   interface mode. */
    NRFX_RESET_REASON_DIF_MASK          = NRF_RESET_RESETREAS_DIF_MASK,
#if NRFX_RESET_REASON_HAS_LSREQ
    /**< Reset from network soft reset detected. */
    NRFX_RESET_REASON_LSREQ_MASK        = NRF_RESET_RESETREAS_LSREQ_MASK,
#endif
#if NRFX_RESET_REASON_HAS_LLOCKUP
    /**< Reset from network CPU lockup detected. */
    NRFX_RESET_REASON_LLOCKUP_MASK      = NRF_RESET_RESETREAS_LLOCKUP_MASK,
#endif
#if NRFX_RESET_REASON_HAS_LDOG
    /**< Reset from network watchdog timer detected. */
    NRFX_RESET_REASON_LDOG_MASK         = NRF_RESET_RESETREAS_LDOG_MASK,
#endif
#if NRFX_RESET_REASON_HAS_MFORCEOFF
    /**< Force off reset from application core detected. */
    NRFX_RESET_REASON_MFORCEOFF_MASK    = NRF_RESET_RESETREAS_MFORCEOFF_MASK,
#endif
#if NRFX_RESET_REASON_HAS_NFC
    /**< Reset after wakeup from System OFF mode due to NFC field being detected. */
    NRFX_RESET_REASON_NFC_MASK          = NRF_RESET_RESETREAS_NFC_MASK,
#endif
    /**< Reset from application watchdog timer 1 detected. */
    NRFX_RESET_REASON_DOG1_MASK         = NRF_RESET_RESETREAS_DOG1_MASK,
#if NRFX_RESET_REASON_HAS_VBUS
    /**< Reset after wakeup from System OFF mode due to VBUS rising into valid range. */
    NRFX_RESET_REASON_VBUS_MASK         = NRF_RESET_RESETREAS_VBUS_MASK,
#endif
#if NRFX_RESET_REASON_HAS_LCTRLAP
    /**< Reset from network CTRL-AP detected. */
    NRFX_RESET_REASON_LCTRLAP_MASK      = NRF_RESET_RESETREAS_LCTRLAP_MASK,
#endif
#if NRF_RESET_HAS_CTRLAPSOFT_RESET
    /**< Soft reset from CTRL-AP detected. */
    NRFX_RESET_REASON_CTRLAPSOFT_MASK   = NRF_RESET_RESETREAS_CTRLAPSOFT_MASK,
#endif
#if NRFX_RESET_REASON_HAS_CTRLAPSOFT
    /**< Reset due to CTRL-AP hard reset. */
    NRFX_RESET_REASON_CTRLAPHARD_MASK   = NRF_RESET_RESETREAS_CTRLAPHARD_MASK,
#endif
#if NRFX_RESET_REASON_HAS_CTRLAPPIN
    /**< Reset due to CTRL-AP pin reset. */
    NRFX_RESET_REASON_CTRLAPPIN_MASK    = NRF_RESET_RESETREAS_CTRLAPPIN_MASK,
#endif
#if NRFX_RESET_REASON_HAS_GRTC
    /**< Reset due to wakeup from GRTC. */
    NRFX_RESET_REASON_GRTC_MASK         = NRF_RESET_RESETREAS_GRTC_MASK,
#endif
#if NRFX_RESET_REASON_HAS_SECTAMPER
    /**< Reset due to illegal tampering of the device. */
    NRFX_RESET_REASON_SECTAMPER_MASK    = NRF_RESET_RESETREAS_SECTAMPER_MASK,
#endif
#elif defined(NRF_RESETINFO)
    NRFX_RESET_REASON_POR_MASK              = NRF_RESETINFO_RESETREAS_GLOBAL_POR_MASK,
    NRFX_RESET_REASON_RESETPIN_MASK         = NRF_RESETINFO_RESETREAS_GLOBAL_PIN_MASK,
    NRFX_RESET_REASON_DOG_MASK              = NRF_RESETINFO_RESETREAS_GLOBAL_DOG_MASK,
    NRFX_RESET_REASON_CTRLAP_MASK           = NRF_RESETINFO_RESETREAS_GLOBAL_CTRLAP_MASK,
    NRFX_RESET_REASON_SREQ_MASK             = NRF_RESETINFO_RESETREAS_GLOBAL_SECSREQ_MASK,
    NRFX_RESET_REASON_SECWDT0_MASK          = NRF_RESETINFO_RESETREAS_GLOBAL_SECWDT0_MASK,
    NRFX_RESET_REASON_SECWDT1_MASK          = NRF_RESETINFO_RESETREAS_GLOBAL_SECWDT1_MASK,
    NRFX_RESET_REASON_LOCKUP_MASK           = NRF_RESETINFO_RESETREAS_GLOBAL_SECLOCKUP_MASK,
    NRFX_RESET_REASON_SECTAMPER_MASK        = NRF_RESETINFO_RESETREAS_GLOBAL_SECTAMPER_MASK,
    NRFX_RESET_REASON_OFF_MASK              = NRF_RESETINFO_RESETREAS_GLOBAL_GPIO_MASK,
    NRFX_RESET_REASON_LPCOMP_MASK           = NRF_RESETINFO_RESETREAS_GLOBAL_LPCOMP_MASK,
    NRFX_RESET_REASON_DIF_MASK              = NRF_RESETINFO_RESETREAS_GLOBAL_DIF_MASK,
    NRFX_RESET_REASON_GRTC_MASK             = NRF_RESETINFO_RESETREAS_GLOBAL_GRTC_MASK,
    NRFX_RESET_REASON_NFC_MASK              = NRF_RESETINFO_RESETREAS_GLOBAL_NFC_MASK,
    NRFX_RESET_REASON_VBUS_MASK             = NRF_RESETINFO_RESETREAS_GLOBAL_VUSB_MASK,
    NRFX_RESET_REASON_LOCAL_DOG0_MASK       = NRF_RESETINFO_RESETREAS_LOCAL_WDT0_MASK
                                              << NRFX_RESET_REASON_LOCAL_OFFSET,
    NRFX_RESET_REASON_LOCAL_DOG1_MASK       = NRF_RESETINFO_RESETREAS_LOCAL_WDT1_MASK
                                              << NRFX_RESET_REASON_LOCAL_OFFSET,
    NRFX_RESET_REASON_LOCAL_SREQ_MASK       = NRF_RESETINFO_RESETREAS_LOCAL_SREQ_MASK
                                              << NRFX_RESET_REASON_LOCAL_OFFSET,
    NRFX_RESET_REASON_LOCAL_LOCKUP_MASK     = NRF_RESETINFO_RESETREAS_LOCAL_LOCKUP_MASK
                                              << NRFX_RESET_REASON_LOCAL_OFFSET,
    NRFX_RESET_REASON_LOCAL_UNRETAINED_MASK = NRF_RESETINFO_RESETREAS_LOCAL_UNRETAINED_MASK
                                              << NRFX_RESET_REASON_LOCAL_OFFSET,
#elif defined(NRF_POWER)
    NRFX_RESET_REASON_RESETPIN_MASK     = NRF_POWER_RESETREAS_RESETPIN_MASK,
    NRFX_RESET_REASON_DOG_MASK          = NRF_POWER_RESETREAS_DOG_MASK,
    NRFX_RESET_REASON_SREQ_MASK         = NRF_POWER_RESETREAS_SREQ_MASK,
    NRFX_RESET_REASON_LOCKUP_MASK       = NRF_POWER_RESETREAS_LOCKUP_MASK,
    NRFX_RESET_REASON_OFF_MASK          = NRF_POWER_RESETREAS_OFF_MASK,
#if NRFX_RESET_REASON_HAS_CTRLAP
    NRFX_RESET_REASON_CTRLAP_MASK       = NRF_POWER_RESETREAS_CTRLAP_MASK,
#endif
#if NRFX_RESET_REASON_HAS_LPCOMP
    NRFX_RESET_REASON_LPCOMP_MASK       = NRF_POWER_RESETREAS_LPCOMP_MASK,
#endif
    NRFX_RESET_REASON_DIF_MASK          = NRF_POWER_RESETREAS_DIF_MASK,
#if NRFX_RESET_REASON_HAS_NFC
    NRFX_RESET_REASON_NFC_MASK          = NRF_POWER_RESETREAS_NFC_MASK,
#endif
#if NRFX_RESET_REASON_HAS_VBUS
    NRFX_RESET_REASON_VBUS_MASK         = NRF_POWER_RESETREAS_VBUS_MASK,
#endif
#else
#error "Unsupported device"
#endif
} nrfx_reset_reason_mask_t;

/**
 * @brief Function for getting the reset reason bitmask.
 *
 * Unless cleared, the RESETREAS register is cumulative.
 * If none of the reset sources is flagged, the chip was reset from the on-chip reset generator.
 * This indicates a power-on-reset or a brown out reset.
 *
 * @return Mask of reset reasons constructed from @ref nrfx_reset_reason_mask_t values.
 */
__STATIC_INLINE uint32_t nrfx_reset_reason_get(void)
{
#if defined(NRF_RESETINFO)
    return (nrf_resetinfo_resetreas_global_get(NRF_RESETINFO)) |
           (nrf_resetinfo_resetreas_local_get(NRF_RESETINFO) << NRFX_RESET_REASON_LOCAL_OFFSET);
#elif defined(NRF_RESET)
    return nrf_reset_resetreas_get(NRF_RESET);
#elif defined(NRF_POWER)
    return nrf_power_resetreas_get(NRF_POWER);
#endif
}

/**
 * @brief Function for clearing the selected reset reason fields.
 *
 * @param[in] mask Mask constructed from @ref nrfx_reset_reason_mask_t values.
 */
__STATIC_INLINE void nrfx_reset_reason_clear(uint32_t mask)
{
#if defined(NRF_RESETINFO)
    uint32_t resetreas;

    // Contrary to NRF_RESET clearing is done by writing expected value to the register and
    // not by writing '1'.
    // Check if there are any global reasons to clear.
    resetreas = mask & NRFX_BIT_MASK(NRFX_RESET_REASON_LOCAL_OFFSET);
    if (resetreas) {
        resetreas = ~resetreas & nrf_resetinfo_resetreas_global_get(NRF_RESETINFO);
        nrf_resetinfo_resetreas_global_set(NRF_RESETINFO, resetreas);
    }

    // Check if there are any local reasons to clear.
    resetreas = mask >> NRFX_RESET_REASON_LOCAL_OFFSET;
    if (resetreas) {
        resetreas = ~resetreas & nrf_resetinfo_resetreas_local_get(NRF_RESETINFO);
        nrf_resetinfo_resetreas_local_set(NRF_RESETINFO, resetreas);
    }

#elif defined(NRF_RESET)
    nrf_reset_resetreas_clear(NRF_RESET, mask);
#elif defined(NRF_POWER)
    nrf_power_resetreas_clear(NRF_POWER, mask);
#endif
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_RESET_REASON_H
