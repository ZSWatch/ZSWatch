/*
 * Copyright (c) 2019 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_POWER_COMPAT_H__
#define NRFX_POWER_COMPAT_H__

/**
 * POWER compatibility layer.
 *
 * The following definitions allow using of common code involving the POWER peripheral
 * driver for different SoCs, regardless of whether certain regulator registers
 * are located in the POWER peripheral or in separate peripherals like USBREG and REGULATORS.
 */

#if defined(REGULATORS_PRESENT)
#include <hal/nrf_regulators.h>

#if NRF_REGULATORS_HAS_POF

typedef nrf_regulators_pof_thr_t nrf_power_pof_thr_t;
#if defined(REGULATORS_POFCON_THRESHOLD_V090)
#define NRF_POWER_POFTHR_V09     NRF_REGULATORS_POF_THR_0V9
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V095)
#define NRF_POWER_POFTHR_V095    NRF_REGULATORS_POF_THR_0V95
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V100)
#define NRF_POWER_POFTHR_V10     NRF_REGULATORS_POF_THR_1V0
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V105)
#define NRF_POWER_POFTHR_V105    NRF_REGULATORS_POF_THR_1V05
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V110)
#define NRF_POWER_POFTHR_V11     NRF_REGULATORS_POF_THR_1V1
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V115)
#define NRF_POWER_POFTHR_V115    NRF_REGULATORS_POF_THR_1V15
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V120)
#define NRF_POWER_POFTHR_V12     NRF_REGULATORS_POF_THR_1V2
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V125)
#define NRF_POWER_POFTHR_V125    NRF_REGULATORS_POF_THR_1V25
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V130)
#define NRF_POWER_POFTHR_V13     NRF_REGULATORS_POF_THR_1V3
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V135)
#define NRF_POWER_POFTHR_V135    NRF_REGULATORS_POF_THR_1V35
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V140)
#define NRF_POWER_POFTHR_V14     NRF_REGULATORS_POF_THR_1V4
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V145)
#define NRF_POWER_POFTHR_V145    NRF_REGULATORS_POF_THR_1V45
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V150)
#define NRF_POWER_POFTHR_V15     NRF_REGULATORS_POF_THR_1V5
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V155)
#define NRF_POWER_POFTHR_V155    NRF_REGULATORS_POF_THR_1V55
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V160)
#define NRF_POWER_POFTHR_V16     NRF_REGULATORS_POF_THR_1V6
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V165)
#define NRF_POWER_POFTHR_V165    NRF_REGULATORS_POF_THR_1V65
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V17)
#define NRF_POWER_POFTHR_V17     NRF_REGULATORS_POF_THR_1V7
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V18)
#define NRF_POWER_POFTHR_V18     NRF_REGULATORS_POF_THR_1V8
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V19)
#define NRF_POWER_POFTHR_V19     NRF_REGULATORS_POF_THR_1V9
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V20)
#define NRF_POWER_POFTHR_V20     NRF_REGULATORS_POF_THR_2V0
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V21)
#define NRF_POWER_POFTHR_V21     NRF_REGULATORS_POF_THR_2V1
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V22)
#define NRF_POWER_POFTHR_V22     NRF_REGULATORS_POF_THR_2V2
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V23)
#define NRF_POWER_POFTHR_V23     NRF_REGULATORS_POF_THR_2V3
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V24)
#define NRF_POWER_POFTHR_V24     NRF_REGULATORS_POF_THR_2V4
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V25)
#define NRF_POWER_POFTHR_V25     NRF_REGULATORS_POF_THR_2V5
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V26)
#define NRF_POWER_POFTHR_V26     NRF_REGULATORS_POF_THR_2V6
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V27)
#define NRF_POWER_POFTHR_V27     NRF_REGULATORS_POF_THR_2V7
#endif
#if defined(REGULATORS_POFCON_THRESHOLD_V28)
#define NRF_POWER_POFTHR_V28     NRF_REGULATORS_POF_THR_2V8
#endif

#if NRF_REGULATORS_HAS_POF_VDDH

typedef nrf_regulators_pof_thr_vddh_t nrf_power_pof_thrvddh_t;
#define NRF_POWER_POFTHRVDDH_V27     NRF_REGULATORS_POF_THR_VDDH_2V7
#define NRF_POWER_POFTHRVDDH_V28     NRF_REGULATORS_POF_THR_VDDH_2V8
#define NRF_POWER_POFTHRVDDH_V29     NRF_REGULATORS_POF_THR_VDDH_2V9
#define NRF_POWER_POFTHRVDDH_V30     NRF_REGULATORS_POF_THR_VDDH_3V0
#define NRF_POWER_POFTHRVDDH_V31     NRF_REGULATORS_POF_THR_VDDH_3V1
#define NRF_POWER_POFTHRVDDH_V32     NRF_REGULATORS_POF_THR_VDDH_3V2
#define NRF_POWER_POFTHRVDDH_V33     NRF_REGULATORS_POF_THR_VDDH_3V3
#define NRF_POWER_POFTHRVDDH_V34     NRF_REGULATORS_POF_THR_VDDH_3V4
#define NRF_POWER_POFTHRVDDH_V35     NRF_REGULATORS_POF_THR_VDDH_3V5
#define NRF_POWER_POFTHRVDDH_V36     NRF_REGULATORS_POF_THR_VDDH_3V6
#define NRF_POWER_POFTHRVDDH_V37     NRF_REGULATORS_POF_THR_VDDH_3V7
#define NRF_POWER_POFTHRVDDH_V38     NRF_REGULATORS_POF_THR_VDDH_3V8
#define NRF_POWER_POFTHRVDDH_V39     NRF_REGULATORS_POF_THR_VDDH_3V9
#define NRF_POWER_POFTHRVDDH_V40     NRF_REGULATORS_POF_THR_VDDH_4V0
#define NRF_POWER_POFTHRVDDH_V41     NRF_REGULATORS_POF_THR_VDDH_4V1
#define NRF_POWER_POFTHRVDDH_V42     NRF_REGULATORS_POF_THR_VDDH_4V2

#endif // NRF_REGULATORS_HAS_POF_VDDH

#endif // NRF_REGULATORS_HAS_POF

#endif // defined(REGULATORS_PRESENT)

#if defined(USBREG_PRESENT)
#include "nrfx_usbreg.h"

typedef nrfx_usbreg_event_handler_t nrfx_power_usb_event_handler_t;
typedef nrfx_usbreg_config_t        nrfx_power_usbevt_config_t;

typedef nrfx_usbreg_evt_t           nrfx_power_usb_evt_t;
#define NRFX_POWER_USB_EVT_DETECTED NRFX_USBREG_EVT_DETECTED
#define NRFX_POWER_USB_EVT_REMOVED  NRFX_USBREG_EVT_REMOVED
#define NRFX_POWER_USB_EVT_READY    NRFX_USBREG_EVT_READY

typedef nrfx_usbreg_state_t               nrfx_power_usb_state_t;
#define NRFX_POWER_USB_STATE_DISCONNECTED NRFX_USBREG_STATE_DISCONNECTED
#define NRFX_POWER_USB_STATE_CONNECTED    NRFX_USBREG_STATE_CONNECTED
#define NRFX_POWER_USB_STATE_READY        NRFX_USBREG_STATE_READY

#define nrfx_power_usb_handler_get nrfx_usbreg_handler_get
#define nrfx_power_usbevt_init     nrfx_usbreg_init
#define nrfx_power_usbevt_enable   nrfx_usbreg_enable
#define nrfx_power_usbevt_disable  nrfx_usbreg_disable
#define nrfx_power_usbevt_uninit   nrfx_usbreg_uninit
#define nrfx_power_usbstatus_get   nrfx_usbreg_usbstatus_get

#endif // defined(USBREG_PRESENT)

#endif // NRFX_POWER_COMPAT_H__
