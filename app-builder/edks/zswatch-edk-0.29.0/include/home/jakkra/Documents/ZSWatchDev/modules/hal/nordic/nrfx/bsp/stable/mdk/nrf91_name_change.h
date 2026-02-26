/*

Copyright (c) 2010 - 2025, Nordic Semiconductor ASA All rights reserved.

SPDX-License-Identifier: BSD-3-Clause

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of Nordic Semiconductor ASA nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef NRF91_NAME_CHANGE_H
#define NRF91_NAME_CHANGE_H

/*lint ++flb "Enter library region */

/* This file is given to prevent your SW from not compiling with the updates made to nrf91-series
 * headerfiles, like nrf9160.h and nrf9160_bitfields.h. The macros defined in this file were available
 * previously. Do not use these macros on purpose. Use the ones defined in the respective nrf91-series
 * header files.
 */

/* SAADC enums */
/* Changes to enum names in SAADC */
#define SAADC_CH_PSELP_PSELP_VDD SAADC_CH_PSELP_PSELP_VDDGPIO
#define SAADC_CH_PSELP_PSELN_VDD SAADC_CH_PSELP_PSELN_VDDGPIO

/* CTRLAP PERI Fields */
#define CTRLAPPERI_ERASEPROTECT_LOCK_ERASEPROTECTLOCK_Pos       CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Pos
#define CTRLAPPERI_ERASEPROTECT_LOCK_ERASEPROTECTLOCK_Msk       CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Msk
#define CTRLAPPERI_ERASEPROTECT_LOCK_ERASEPROTECTLOCK_Unlocked  CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Unlocked
#define CTRLAPPERI_ERASEPROTECT_LOCK_ERASEPROTECTLOCK_Locked    CTRLAPPERI_ERASEPROTECT_LOCK_LOCK_Locked

 /* DPPI */
 #define DPPI_PRESENT   DPPIC_PRESENT
 #define DPPI_COUNT     DPPIC_COUNT
 #define DPPI_CH_NUM    DPPIC_CH_NUM
 #define DPPI_GROUP_NUM DPPIC_GROUP_NUM 
 

/* The serial box interrupt ISRs were renamed. Adding old names as macros. */
#define UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQHandler   SPIM0_SPIS0_TWIM0_TWIS0_UARTE0_IRQHandler   
#define UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQn         SPIM0_SPIS0_TWIM0_TWIS0_UARTE0_IRQn         
#define UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQHandler   SPIM1_SPIS1_TWIM1_TWIS1_UARTE1_IRQHandler   
#define UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQn         SPIM1_SPIS1_TWIM1_TWIS1_UARTE1_IRQn         
#define UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQHandler   SPIM2_SPIS2_TWIM2_TWIS2_UARTE2_IRQHandler   
#define UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQn         SPIM2_SPIS2_TWIM2_TWIS2_UARTE2_IRQn         
#define UARTE3_SPIM3_SPIS3_TWIM3_TWIS3_IRQHanlder   SPIM3_SPIS3_TWIM3_TWIS3_UARTE3_IRQHandler   
#define UARTE3_SPIM3_SPIS3_TWIM3_TWIS3_IRQn         SPIM3_SPIS3_TWIM3_TWIS3_UARTE3_IRQn         

/* TAD */

#define TAD_CLOCKSTART_START_Pos    TAD_TASKS_CLOCKSTART_TASKS_CLOCKSTART_Pos
#define TAD_CLOCKSTART_START_Msk    TAD_TASKS_CLOCKSTART_TASKS_CLOCKSTART_Msk
#define TAD_CLOCKSTART_START_Start  TAD_TASKS_CLOCKSTART_TASKS_CLOCKSTART_Trigger
#define TAD_CLOCKSTOP_STOP_Pos      TAD_TASKS_CLOCKSTOP_TASKS_CLOCKSTOP_Pos 
#define TAD_CLOCKSTOP_STOP_Msk      TAD_TASKS_CLOCKSTOP_TASKS_CLOCKSTOP_Msk
#define TAD_CLOCKSTOP_STOP_Stop     TAD_TASKS_CLOCKSTOP_TASKS_CLOCKSTOP_Trigger

/*lint --flb "Leave library region" */

#endif /* NRF91_NAME_CHANGE_H */
