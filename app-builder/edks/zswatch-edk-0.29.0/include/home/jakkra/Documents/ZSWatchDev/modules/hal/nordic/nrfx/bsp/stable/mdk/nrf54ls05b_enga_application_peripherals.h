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

#ifndef NRF54LS05B_ENGA_APPLICATION_PERIPHERALS_H
#define NRF54LS05B_ENGA_APPLICATION_PERIPHERALS_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdbool.h>
/*CPU control*/
#define CPUC_PRESENT 1
#define CPUC_COUNT 1

/*Cache*/
#define CACHE_PRESENT 1
#define CACHE_COUNT 1

#define ICACHE_VIRTUALCACHE 0                        /*!< (unspecified)                                                        */
#define ICACHE_FLUSH 0                               /*!< (unspecified)                                                        */
#define ICACHE_CLEAN 0                               /*!< (unspecified)                                                        */
#define ICACHE_INVALIDATELINE 0                      /*!< (unspecified)                                                        */
#define ICACHE_ERASE 0                               /*!< (unspecified)                                                        */
#define ICACHE_LINEMAINTAIN 0                        /*!< (unspecified)                                                        */
#define ICACHE_EXTENDEDPROFILING 0                   /*!< (unspecified)                                                        */
#define ICACHE_DEBUGLOCK 0                           /*!< (unspecified)                                                        */
#define ICACHE_WRITELOCK 0                           /*!< (unspecified)                                                        */
#define ICACHE_NONCACHEABLEMISS 0                    /*!< (unspecified)                                                        */
#define ICACHE_BUSWIDTH_MIN 0                        /*!< Data bus width : 0..63                                               */
#define ICACHE_BUSWIDTH_MAX 63                       /*!< Data bus width : 0..63                                               */
#define ICACHE_BUSWIDTH_SIZE 64                      /*!< Data bus width : 0..63                                               */

/*Software interrupt*/
#define SWI_PRESENT 1
#define SWI_COUNT 4

/*Factory Information Configuration Registers*/
#define FICR_PRESENT 1
#define FICR_COUNT 1

/*User Information Configuration Registers*/
#define UICR_PRESENT 1
#define UICR_COUNT 1

/*Distributed programmable peripheral interconnect controller*/
#define DPPIC_PRESENT 1
#define DPPIC_COUNT 4

#define DPPIC00_HASCHANNELGROUPS 1                   /*!< (unspecified)                                                        */
#define DPPIC00_CH_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define DPPIC00_CH_NUM_MAX 15                        /*!< (unspecified)                                                        */
#define DPPIC00_CH_NUM_SIZE 16                       /*!< (unspecified)                                                        */
#define DPPIC00_GROUP_NUM_MIN 0                      /*!< (unspecified)                                                        */
#define DPPIC00_GROUP_NUM_MAX 1                      /*!< (unspecified)                                                        */
#define DPPIC00_GROUP_NUM_SIZE 2                     /*!< (unspecified)                                                        */

#define DPPIC10_HASCHANNELGROUPS 1                   /*!< (unspecified)                                                        */
#define DPPIC10_CH_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define DPPIC10_CH_NUM_MAX 23                        /*!< (unspecified)                                                        */
#define DPPIC10_CH_NUM_SIZE 24                       /*!< (unspecified)                                                        */
#define DPPIC10_GROUP_NUM_MIN 0                      /*!< (unspecified)                                                        */
#define DPPIC10_GROUP_NUM_MAX 5                      /*!< (unspecified)                                                        */
#define DPPIC10_GROUP_NUM_SIZE 6                     /*!< (unspecified)                                                        */

#define DPPIC20_HASCHANNELGROUPS 1                   /*!< (unspecified)                                                        */
#define DPPIC20_CH_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define DPPIC20_CH_NUM_MAX 15                        /*!< (unspecified)                                                        */
#define DPPIC20_CH_NUM_SIZE 16                       /*!< (unspecified)                                                        */
#define DPPIC20_GROUP_NUM_MIN 0                      /*!< (unspecified)                                                        */
#define DPPIC20_GROUP_NUM_MAX 5                      /*!< (unspecified)                                                        */
#define DPPIC20_GROUP_NUM_SIZE 6                     /*!< (unspecified)                                                        */

#define DPPIC30_HASCHANNELGROUPS 1                   /*!< (unspecified)                                                        */
#define DPPIC30_CH_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define DPPIC30_CH_NUM_MAX 3                         /*!< (unspecified)                                                        */
#define DPPIC30_CH_NUM_SIZE 4                        /*!< (unspecified)                                                        */
#define DPPIC30_GROUP_NUM_MIN 0                      /*!< (unspecified)                                                        */
#define DPPIC30_GROUP_NUM_MAX 1                      /*!< (unspecified)                                                        */
#define DPPIC30_GROUP_NUM_SIZE 2                     /*!< (unspecified)                                                        */

/*PPIB APB registers*/
#define PPIB_PRESENT 1
#define PPIB_COUNT 8

#define PPIB00_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB00_NTASKSEVENTS_MAX 11                   /*!< (unspecified)                                                        */
#define PPIB00_NTASKSEVENTS_SIZE 12                  /*!< (unspecified)                                                        */

#define PPIB01_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB01_NTASKSEVENTS_MAX 7                    /*!< (unspecified)                                                        */
#define PPIB01_NTASKSEVENTS_SIZE 8                   /*!< (unspecified)                                                        */

#define PPIB10_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB10_NTASKSEVENTS_MAX 11                   /*!< (unspecified)                                                        */
#define PPIB10_NTASKSEVENTS_SIZE 12                  /*!< (unspecified)                                                        */

#define PPIB11_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB11_NTASKSEVENTS_MAX 15                   /*!< (unspecified)                                                        */
#define PPIB11_NTASKSEVENTS_SIZE 16                  /*!< (unspecified)                                                        */

#define PPIB20_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB20_NTASKSEVENTS_MAX 7                    /*!< (unspecified)                                                        */
#define PPIB20_NTASKSEVENTS_SIZE 8                   /*!< (unspecified)                                                        */

#define PPIB21_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB21_NTASKSEVENTS_MAX 15                   /*!< (unspecified)                                                        */
#define PPIB21_NTASKSEVENTS_SIZE 16                  /*!< (unspecified)                                                        */

#define PPIB22_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB22_NTASKSEVENTS_MAX 3                    /*!< (unspecified)                                                        */
#define PPIB22_NTASKSEVENTS_SIZE 4                   /*!< (unspecified)                                                        */

#define PPIB30_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB30_NTASKSEVENTS_MAX 3                    /*!< (unspecified)                                                        */
#define PPIB30_NTASKSEVENTS_SIZE 4                   /*!< (unspecified)                                                        */

/*Accelerated Address Resolver*/
#define AAR_PRESENT 1
#define AAR_COUNT 1

#define AAR00_DMAERROR 1                             /*!< (unspecified)                                                        */
#define AAR00_ERRORSTATUS 1                          /*!< (unspecified)                                                        */
#define AAR00_ERROREVENT 1                           /*!< (unspecified)                                                        */
#define AAR00_ERROREVENT_IRQ 1                       /*!< (unspecified)                                                        */
#define AAR00_PREMATUREOUTPUTPTR 1                   /*!< (unspecified)                                                        */

/*AES CCM Mode Encryption*/
#define CCM_PRESENT 1
#define CCM_COUNT 1

#define CCM00_AMOUNTREG 0                            /*!< (unspecified)                                                        */
#define CCM00_ONTHEFLYDECRYPTION 0                   /*!< Does not support on-the-fly decryption.                              */
#define CCM00_DMAERROR 1                             /*!< (unspecified)                                                        */
#define CCM00_BYPASSMODE 0                           /*!< No bypass mode available in the MODE register.                       */

/*AES ECB Mode Encryption*/
#define ECB_PRESENT 1
#define ECB_COUNT 1

#define ECB00_AMOUNTREG 0                            /*!< (unspecified)                                                        */
#define ECB00_DMAERROR 1                             /*!< (unspecified)                                                        */
#define ECB00_ERRORSTATUS 1                          /*!< (unspecified)                                                        */

/*RRAM controller GLITCH detector*/
#define RRAMC_PRESENT 1
#define RRAMC_COUNT 1

#define RRAMC_NAPMODE 1                              /*!< (unspecified)                                                        */
#define RRAMC_NRRAMWORDSIZE 64                       /*!< RRAM word size : 64 bits per wordline                                */
#define RRAMC_NWRITEBUFSIZE 32                       /*!< Maximum write buffer size : 32                                       */
#define RRAMC_LOWPOWERCONFIGOPTION2 0                /*!< (unspecified)                                                        */
#define RRAMC_REGION0ADDR_WRITABLE 0                 /*!< (unspecified)                                                        */
#define RRAMC_REGION0SIZE 4                          /*!< (unspecified)                                                        */
#define RRAMC_REGION0SIZE_WRITABLE 0                 /*!< (unspecified)                                                        */
#define RRAMC_REGION0READ 1                          /*!< (unspecified)                                                        */
#define RRAMC_REGION0READ_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION0WRITE 1                         /*!< (unspecified)                                                        */
#define RRAMC_REGION0WRITE_WRITABLE 1                /*!< (unspecified)                                                        */
#define RRAMC_REGION0EXECUTE 0                       /*!< (unspecified)                                                        */
#define RRAMC_REGION0EXECUTE_WRITABLE 0              /*!< (unspecified)                                                        */
#define RRAMC_REGION0WRITEONCE 1                     /*!< (unspecified)                                                        */
#define RRAMC_REGION0WRITEONCE_WRITABLE 0            /*!< (unspecified)                                                        */
#define RRAMC_REGION0LOCK 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION0LOCK_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION1ADDR_WRITABLE 0                 /*!< (unspecified)                                                        */
#define RRAMC_REGION1SIZE 4                          /*!< (unspecified)                                                        */
#define RRAMC_REGION1SIZE_WRITABLE 0                 /*!< (unspecified)                                                        */
#define RRAMC_REGION1READ 1                          /*!< (unspecified)                                                        */
#define RRAMC_REGION1READ_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION1WRITE 1                         /*!< (unspecified)                                                        */
#define RRAMC_REGION1WRITE_WRITABLE 1                /*!< (unspecified)                                                        */
#define RRAMC_REGION1EXECUTE 0                       /*!< (unspecified)                                                        */
#define RRAMC_REGION1EXECUTE_WRITABLE 0              /*!< (unspecified)                                                        */
#define RRAMC_REGION1WRITEONCE 1                     /*!< (unspecified)                                                        */
#define RRAMC_REGION1WRITEONCE_WRITABLE 0            /*!< (unspecified)                                                        */
#define RRAMC_REGION1LOCK 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION1LOCK_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION2ADDR_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION2SIZE 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION2SIZE_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION2READ 1                          /*!< (unspecified)                                                        */
#define RRAMC_REGION2READ_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION2WRITE 1                         /*!< (unspecified)                                                        */
#define RRAMC_REGION2WRITE_WRITABLE 1                /*!< (unspecified)                                                        */
#define RRAMC_REGION2EXECUTE 1                       /*!< (unspecified)                                                        */
#define RRAMC_REGION2EXECUTE_WRITABLE 1              /*!< (unspecified)                                                        */
#define RRAMC_REGION2WRITEONCE 0                     /*!< (unspecified)                                                        */
#define RRAMC_REGION2WRITEONCE_WRITABLE 1            /*!< (unspecified)                                                        */
#define RRAMC_REGION2LOCK 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION2LOCK_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION3ADDR_WRITABLE 0                 /*!< (unspecified)                                                        */
#define RRAMC_REGION3SIZE 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION3SIZE_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION3READ 1                          /*!< (unspecified)                                                        */
#define RRAMC_REGION3READ_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION3WRITE 1                         /*!< (unspecified)                                                        */
#define RRAMC_REGION3WRITE_WRITABLE 1                /*!< (unspecified)                                                        */
#define RRAMC_REGION3EXECUTE 1                       /*!< (unspecified)                                                        */
#define RRAMC_REGION3EXECUTE_WRITABLE 1              /*!< (unspecified)                                                        */
#define RRAMC_REGION3WRITEONCE 0                     /*!< (unspecified)                                                        */
#define RRAMC_REGION3WRITEONCE_WRITABLE 1            /*!< (unspecified)                                                        */
#define RRAMC_REGION3LOCK 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION3LOCK_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION4ADDR_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION4SIZE 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION4SIZE_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION4READ 1                          /*!< (unspecified)                                                        */
#define RRAMC_REGION4READ_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION4WRITE 1                         /*!< (unspecified)                                                        */
#define RRAMC_REGION4WRITE_WRITABLE 1                /*!< (unspecified)                                                        */
#define RRAMC_REGION4EXECUTE 1                       /*!< (unspecified)                                                        */
#define RRAMC_REGION4EXECUTE_WRITABLE 1              /*!< (unspecified)                                                        */
#define RRAMC_REGION4WRITEONCE 0                     /*!< (unspecified)                                                        */
#define RRAMC_REGION4WRITEONCE_WRITABLE 1            /*!< (unspecified)                                                        */
#define RRAMC_REGION4LOCK 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION4LOCK_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_GLITCHDETECTORS 0                      /*!< (unspecified)                                                        */

/*Control access port*/
#define CTRLAPPERI_PRESENT 1
#define CTRLAPPERI_COUNT 1

/*Trace and debug control*/
#define TAD_PRESENT 1
#define TAD_COUNT 1

#define TAD_TADFORCEON 0                             /*!< (unspecified)                                                        */
#define TAD_TAD_HAS_TASKS 0                          /*!< (unspecified)                                                        */
#define TAD_PDREQCLR 1                               /*!< (unspecified)                                                        */
#define TAD_TAD_HAS_DBGWAKEUPREQ 1                   /*!< (unspecified)                                                        */

/*Timer/Counter*/
#define TIMER_PRESENT 1
#define TIMER_COUNT 3

#define TIMER00_CC_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define TIMER00_CC_NUM_MAX 5                         /*!< (unspecified)                                                        */
#define TIMER00_CC_NUM_SIZE 6                        /*!< (unspecified)                                                        */
#define TIMER00_MAX_SIZE_MIN 0                       /*!< (unspecified)                                                        */
#define TIMER00_MAX_SIZE_MAX 31                      /*!< (unspecified)                                                        */
#define TIMER00_MAX_SIZE_SIZE 32                     /*!< (unspecified)                                                        */
#define TIMER00_PCLK_MHZ 128                         /*!< Peripheral clock frequency (PCLK) is 128 MHz                         */
#define TIMER00_PCLK_VARIABLE 1                      /*!< The system is able to configure the TIMER peripheral input clock
                                                          frequency (PCLK) before it reaches TIMER, and calculations of
                                                          PRESCALER value must take the actual PCLK frequency into account*/
#define TIMER00_SHUTDOWN_TASK 0                      /*!< (unspecified)                                                        */

#define TIMER10_CC_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define TIMER10_CC_NUM_MAX 7                         /*!< (unspecified)                                                        */
#define TIMER10_CC_NUM_SIZE 8                        /*!< (unspecified)                                                        */
#define TIMER10_MAX_SIZE_MIN 0                       /*!< (unspecified)                                                        */
#define TIMER10_MAX_SIZE_MAX 31                      /*!< (unspecified)                                                        */
#define TIMER10_MAX_SIZE_SIZE 32                     /*!< (unspecified)                                                        */
#define TIMER10_PCLK_MHZ 32                          /*!< Peripheral clock frequency (PCLK) is 32 MHz                          */
#define TIMER10_PCLK_VARIABLE 0                      /*!< (unspecified)                                                        */
#define TIMER10_SHUTDOWN_TASK 0                      /*!< (unspecified)                                                        */

#define TIMER20_CC_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define TIMER20_CC_NUM_MAX 5                         /*!< (unspecified)                                                        */
#define TIMER20_CC_NUM_SIZE 6                        /*!< (unspecified)                                                        */
#define TIMER20_MAX_SIZE_MIN 0                       /*!< (unspecified)                                                        */
#define TIMER20_MAX_SIZE_MAX 31                      /*!< (unspecified)                                                        */
#define TIMER20_MAX_SIZE_SIZE 32                     /*!< (unspecified)                                                        */
#define TIMER20_PCLK_MHZ 16                          /*!< Peripheral clock frequency (PCLK) is 16 MHz                          */
#define TIMER20_PCLK_VARIABLE 0                      /*!< (unspecified)                                                        */
#define TIMER20_SHUTDOWN_TASK 0                      /*!< (unspecified)                                                        */

/*Event generator unit*/
#define EGU_PRESENT 1
#define EGU_COUNT 3

#define EGU00_PEND 0                                 /*!< (unspecified)                                                        */
#define EGU00_CH_NUM_MIN 0                           /*!< (unspecified)                                                        */
#define EGU00_CH_NUM_MAX 5                           /*!< (unspecified)                                                        */
#define EGU00_CH_NUM_SIZE 6                          /*!< (unspecified)                                                        */

#define EGU10_PEND 0                                 /*!< (unspecified)                                                        */
#define EGU10_CH_NUM_MIN 0                           /*!< (unspecified)                                                        */
#define EGU10_CH_NUM_MAX 15                          /*!< (unspecified)                                                        */
#define EGU10_CH_NUM_SIZE 16                         /*!< (unspecified)                                                        */

#define EGU20_PEND 0                                 /*!< (unspecified)                                                        */
#define EGU20_CH_NUM_MIN 0                           /*!< (unspecified)                                                        */
#define EGU20_CH_NUM_MAX 5                           /*!< (unspecified)                                                        */
#define EGU20_CH_NUM_SIZE 6                          /*!< (unspecified)                                                        */

/*CRACEN TRNG*/
#define CRACEN_PRESENT 1
#define CRACEN_COUNT 1

/*CRACENCORE*/
#define CRACENCORE_PRESENT 1
#define CRACENCORE_COUNT 1

#define CRACENCORE_CRYPTMSTRDMAREGS 0                /*!< (unspecified)                                                        */
#define CRACENCORE_CRYPTMSTRHWREGS 0                 /*!< (unspecified)                                                        */
#define CRACENCORE_RNGCONTROLREGS 1                  /*!< RNGCONTROL registers included                                        */
#define CRACENCORE_PKREGS 0                          /*!< (unspecified)                                                        */
#define CRACENCORE_IKGREGS 0                         /*!< (unspecified)                                                        */
#define CRACENCORE_RNGDATAREGS 0                     /*!< (unspecified)                                                        */
#define CRACENCORE_EXTPRIVKEYSREGS 0                 /*!< (unspecified)                                                        */
#define CRACENCORE_LITESMALLRESETVALUES 0            /*!< (unspecified)                                                        */
#define CRACENCORE_LITEMEDIUMRESETVALUES 0           /*!< (unspecified)                                                        */
#define CRACENCORE_FULLRESETVALUES 0                 /*!< (unspecified)                                                        */
#define CRACENCORE_CRACENRESETVALUES 0               /*!< (unspecified)                                                        */
#define CRACENCORE_SHA3RESETVALUES 0                 /*!< (unspecified)                                                        */

/*2.4 GHz radio*/
#define RADIO_PRESENT 1
#define RADIO_COUNT 1

#define RADIO_IRQ_COUNT 2
#define RADIO_WHITENINGPOLY 1                        /*!< (unspecified)                                                        */
#define RADIO_ADPLLCOMPANION_INCLUDE_DMA 0           /*!< No internal instantiation of DmaChannelPeripheral                    */

/*Serial Peripheral Interface Master with EasyDMA*/
#define SPIM_PRESENT 1
#define SPIM_COUNT 3

#define SPIM20_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */
#define SPIM20_MAX_DATARATE 8                        /*!< (unspecified)                                                        */
#define SPIM20_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM20_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIM20_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIM20_FEATURE_HARDWARE_CSN_PRESENT 1        /*!< CSN functionality is supported.                                      */
#define SPIM20_FEATURE_HARDWARE_DCX_PRESENT 1        /*!< DCX functionality is supported.                                      */
#define SPIM20_FEATURE_RXDELAY_PRESENT 1             /*!< (unspecified)                                                        */
#define SPIM20_STALL_STATUS_PRESENT 0                /*!< (unspecified)                                                        */
#define SPIM20_STALL_STATUS_TX_PRESENT 0             /*!< (unspecified)                                                        */
#define SPIM20_NUM_CHIPSELECT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM20_NUM_CHIPSELECT_MAX 0                  /*!< (unspecified)                                                        */
#define SPIM20_NUM_CHIPSELECT_SIZE 1                 /*!< (unspecified)                                                        */
#define SPIM20_CORE_FREQUENCY 16                     /*!< Peripheral core frequency is 16 MHz.                                 */
#define SPIM20_PRESCALER_PRESENT 1                   /*!< (unspecified)                                                        */
#define SPIM20_PRESCALER_DIVISOR_RANGE_MIN 2         /*!< Prescaler divisor range is 2..126                                    */
#define SPIM20_PRESCALER_DIVISOR_RANGE_MAX 126       /*!< Prescaler divisor range is 2..126                                    */
#define SPIM20_PRESCALER_DIVISOR_RANGE_SIZE 127      /*!< Prescaler divisor range is 2..126                                    */
#define SPIM20_RXDELAY_VALUE_RANGE_MIN 0             /*!< (unspecified)                                                        */
#define SPIM20_RXDELAY_VALUE_RANGE_MAX 7             /*!< (unspecified)                                                        */
#define SPIM20_RXDELAY_VALUE_RANGE_SIZE 8            /*!< (unspecified)                                                        */
#define SPIM20_RXDELAY_RESET_VALUE 1                 /*!< (unspecified)                                                        */
#define SPIM20_RXDELAY_FIELD_WIDTH_MIN 0             /*!< (unspecified)                                                        */
#define SPIM20_RXDELAY_FIELD_WIDTH_MAX 2             /*!< (unspecified)                                                        */
#define SPIM20_RXDELAY_FIELD_WIDTH_SIZE 3            /*!< (unspecified)                                                        */

#define SPIM21_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */
#define SPIM21_MAX_DATARATE 8                        /*!< (unspecified)                                                        */
#define SPIM21_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM21_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIM21_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIM21_FEATURE_HARDWARE_CSN_PRESENT 1        /*!< CSN functionality is supported.                                      */
#define SPIM21_FEATURE_HARDWARE_DCX_PRESENT 1        /*!< DCX functionality is supported.                                      */
#define SPIM21_FEATURE_RXDELAY_PRESENT 1             /*!< (unspecified)                                                        */
#define SPIM21_STALL_STATUS_PRESENT 0                /*!< (unspecified)                                                        */
#define SPIM21_STALL_STATUS_TX_PRESENT 0             /*!< (unspecified)                                                        */
#define SPIM21_NUM_CHIPSELECT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM21_NUM_CHIPSELECT_MAX 0                  /*!< (unspecified)                                                        */
#define SPIM21_NUM_CHIPSELECT_SIZE 1                 /*!< (unspecified)                                                        */
#define SPIM21_CORE_FREQUENCY 16                     /*!< Peripheral core frequency is 16 MHz.                                 */
#define SPIM21_PRESCALER_PRESENT 1                   /*!< (unspecified)                                                        */
#define SPIM21_PRESCALER_DIVISOR_RANGE_MIN 2         /*!< Prescaler divisor range is 2..126                                    */
#define SPIM21_PRESCALER_DIVISOR_RANGE_MAX 126       /*!< Prescaler divisor range is 2..126                                    */
#define SPIM21_PRESCALER_DIVISOR_RANGE_SIZE 127      /*!< Prescaler divisor range is 2..126                                    */
#define SPIM21_RXDELAY_VALUE_RANGE_MIN 0             /*!< (unspecified)                                                        */
#define SPIM21_RXDELAY_VALUE_RANGE_MAX 7             /*!< (unspecified)                                                        */
#define SPIM21_RXDELAY_VALUE_RANGE_SIZE 8            /*!< (unspecified)                                                        */
#define SPIM21_RXDELAY_RESET_VALUE 1                 /*!< (unspecified)                                                        */
#define SPIM21_RXDELAY_FIELD_WIDTH_MIN 0             /*!< (unspecified)                                                        */
#define SPIM21_RXDELAY_FIELD_WIDTH_MAX 2             /*!< (unspecified)                                                        */
#define SPIM21_RXDELAY_FIELD_WIDTH_SIZE 3            /*!< (unspecified)                                                        */

#define SPIM22_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */
#define SPIM22_MAX_DATARATE 8                        /*!< (unspecified)                                                        */
#define SPIM22_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM22_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIM22_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIM22_FEATURE_HARDWARE_CSN_PRESENT 1        /*!< CSN functionality is supported.                                      */
#define SPIM22_FEATURE_HARDWARE_DCX_PRESENT 1        /*!< DCX functionality is supported.                                      */
#define SPIM22_FEATURE_RXDELAY_PRESENT 1             /*!< (unspecified)                                                        */
#define SPIM22_STALL_STATUS_PRESENT 0                /*!< (unspecified)                                                        */
#define SPIM22_STALL_STATUS_TX_PRESENT 0             /*!< (unspecified)                                                        */
#define SPIM22_NUM_CHIPSELECT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM22_NUM_CHIPSELECT_MAX 0                  /*!< (unspecified)                                                        */
#define SPIM22_NUM_CHIPSELECT_SIZE 1                 /*!< (unspecified)                                                        */
#define SPIM22_CORE_FREQUENCY 16                     /*!< Peripheral core frequency is 16 MHz.                                 */
#define SPIM22_PRESCALER_PRESENT 1                   /*!< (unspecified)                                                        */
#define SPIM22_PRESCALER_DIVISOR_RANGE_MIN 2         /*!< Prescaler divisor range is 2..126                                    */
#define SPIM22_PRESCALER_DIVISOR_RANGE_MAX 126       /*!< Prescaler divisor range is 2..126                                    */
#define SPIM22_PRESCALER_DIVISOR_RANGE_SIZE 127      /*!< Prescaler divisor range is 2..126                                    */
#define SPIM22_RXDELAY_VALUE_RANGE_MIN 0             /*!< (unspecified)                                                        */
#define SPIM22_RXDELAY_VALUE_RANGE_MAX 7             /*!< (unspecified)                                                        */
#define SPIM22_RXDELAY_VALUE_RANGE_SIZE 8            /*!< (unspecified)                                                        */
#define SPIM22_RXDELAY_RESET_VALUE 1                 /*!< (unspecified)                                                        */
#define SPIM22_RXDELAY_FIELD_WIDTH_MIN 0             /*!< (unspecified)                                                        */
#define SPIM22_RXDELAY_FIELD_WIDTH_MAX 2             /*!< (unspecified)                                                        */
#define SPIM22_RXDELAY_FIELD_WIDTH_SIZE 3            /*!< (unspecified)                                                        */

/*SPI Slave*/
#define SPIS_PRESENT 1
#define SPIS_COUNT 3

#define SPIS20_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIS20_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIS20_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIS20_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

#define SPIS21_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIS21_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIS21_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIS21_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

#define SPIS22_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIS22_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIS22_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIS22_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

/*I2C compatible Two-Wire Master Interface with EasyDMA*/
#define TWIM_PRESENT 1
#define TWIM_COUNT 3

#define TWIM20_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define TWIM20_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define TWIM20_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define TWIM20_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

#define TWIM21_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define TWIM21_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define TWIM21_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define TWIM21_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

#define TWIM22_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define TWIM22_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define TWIM22_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define TWIM22_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

/*I2C compatible Two-Wire Slave Interface with EasyDMA*/
#define TWIS_PRESENT 1
#define TWIS_COUNT 3

#define TWIS20_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define TWIS20_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define TWIS20_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define TWIS20_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

#define TWIS21_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define TWIS21_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define TWIS21_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define TWIS21_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

#define TWIS22_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define TWIS22_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define TWIS22_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define TWIS22_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

/*UART with EasyDMA*/
#define UARTE_PRESENT 1
#define UARTE_COUNT 3

#define UARTE20_EASYDMA_MAXCNT_MIN 0                 /*!< (unspecified)                                                        */
#define UARTE20_EASYDMA_MAXCNT_MAX 15                /*!< (unspecified)                                                        */
#define UARTE20_EASYDMA_MAXCNT_SIZE 16               /*!< (unspecified)                                                        */
#define UARTE20_TIMEOUT_INTERRUPT 1                  /*!< Timeout interrupt is included.                                       */
#define UARTE20_CONFIGURABLE_DATA_FRAME_SIZE 1       /*!< Supports data frame sizes 4, 5, 6, 7, 8, and 9 bits.                 */
#define UARTE20_CORE_FREQUENCY 16                    /*!< Peripheral clock frequency is 16 MHz.                                */
#define UARTE20_CORE_CLOCK_16 1                      /*!< (unspecified)                                                        */
#define UARTE20_SHORTS_ENDTX_STOPTX 1                /*!< (unspecified)                                                        */
#define UARTE20_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                  */

#define UARTE21_EASYDMA_MAXCNT_MIN 0                 /*!< (unspecified)                                                        */
#define UARTE21_EASYDMA_MAXCNT_MAX 15                /*!< (unspecified)                                                        */
#define UARTE21_EASYDMA_MAXCNT_SIZE 16               /*!< (unspecified)                                                        */
#define UARTE21_TIMEOUT_INTERRUPT 1                  /*!< Timeout interrupt is included.                                       */
#define UARTE21_CONFIGURABLE_DATA_FRAME_SIZE 1       /*!< Supports data frame sizes 4, 5, 6, 7, 8, and 9 bits.                 */
#define UARTE21_CORE_FREQUENCY 16                    /*!< Peripheral clock frequency is 16 MHz.                                */
#define UARTE21_CORE_CLOCK_16 1                      /*!< (unspecified)                                                        */
#define UARTE21_SHORTS_ENDTX_STOPTX 1                /*!< (unspecified)                                                        */
#define UARTE21_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                  */

#define UARTE22_EASYDMA_MAXCNT_MIN 0                 /*!< (unspecified)                                                        */
#define UARTE22_EASYDMA_MAXCNT_MAX 15                /*!< (unspecified)                                                        */
#define UARTE22_EASYDMA_MAXCNT_SIZE 16               /*!< (unspecified)                                                        */
#define UARTE22_TIMEOUT_INTERRUPT 1                  /*!< Timeout interrupt is included.                                       */
#define UARTE22_CONFIGURABLE_DATA_FRAME_SIZE 1       /*!< Supports data frame sizes 4, 5, 6, 7, 8, and 9 bits.                 */
#define UARTE22_CORE_FREQUENCY 16                    /*!< Peripheral clock frequency is 16 MHz.                                */
#define UARTE22_CORE_CLOCK_16 1                      /*!< (unspecified)                                                        */
#define UARTE22_SHORTS_ENDTX_STOPTX 1                /*!< (unspecified)                                                        */
#define UARTE22_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                  */

/*Memory configuration*/
#define MEMCONF_PRESENT 1
#define MEMCONF_COUNT 1

#define MEMCONF_RETTRIM 1                            /*!< (unspecified)                                                        */
#define MEMCONF_REPAIR 0                             /*!< (unspecified)                                                        */
#define MEMCONF_POWER 1                              /*!< (unspecified)                                                        */

/*Pulse width modulation unit*/
#define PWM_PRESENT 1
#define PWM_COUNT 1

#define PWM20_IDLE_OUT 1                             /*!< IDLEOUT register is available.                                       */
#define PWM20_COMPARE_MATCH 1                        /*!< EVENTS_COMPAREMATCH events are available.                            */
#define PWM20_FEATURES_V2 0                          /*!< (unspecified)                                                        */
#define PWM20_NO_FEATURES_V2 1                       /*!< (unspecified)                                                        */
#define PWM20_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 1 /*!< CURRENTAMOUNT register included.                                 */

/*Analog to Digital Converter*/
#define SAADC_PRESENT 1
#define SAADC_COUNT 1

#define SAADC_PSEL_V2 1                              /*!< (unspecified)                                                        */
#define SAADC_TASKS_CALIBRATEGAIN 0                  /*!< (unspecified)                                                        */
#define SAADC_SAMPLERATE_CC_VALUERANGE_MIN 16        /*!< (unspecified)                                                        */
#define SAADC_SAMPLERATE_CC_VALUERANGE_MAX 2047      /*!< (unspecified)                                                        */
#define SAADC_SAMPLERATE_CC_VALUERANGE_SIZE 2048     /*!< (unspecified)                                                        */
#define SAADC_TACQ_VALUE_RANGE_MIN 1                 /*!< (unspecified)                                                        */
#define SAADC_TACQ_VALUE_RANGE_MAX 319               /*!< (unspecified)                                                        */
#define SAADC_TACQ_VALUE_RANGE_SIZE 320              /*!< (unspecified)                                                        */
#define SAADC_TCONV_VALUE_RANGE_MIN 1                /*!< (unspecified)                                                        */
#define SAADC_TCONV_VALUE_RANGE_MAX 7                /*!< (unspecified)                                                        */
#define SAADC_TCONV_VALUE_RANGE_SIZE 8               /*!< (unspecified)                                                        */
#define SAADC_CHOPPING 1                             /*!< (unspecified)                                                        */
#define SAADC_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< CURRENTAMOUNT register not included.                             */

/*Temperature Sensor*/
#define TEMP_PRESENT 1
#define TEMP_COUNT 1

/*GPIO Port*/
#define GPIO_PRESENT 1
#define GPIO_COUNT 2

#define P1_CTRLSEL_MAP1 0                            /*!< (unspecified)                                                        */
#define P1_CTRLSEL_MAP2 1                            /*!< (unspecified)                                                        */
#define P1_CTRLSEL_MAP3 0                            /*!< (unspecified)                                                        */
#define P1_CTRLSEL_MAP4 0                            /*!< (unspecified)                                                        */
#define P1_CTRLSEL_MAP5 0                            /*!< (unspecified)                                                        */
#define P1_PIN_NUM_MIN 0                             /*!< (unspecified)                                                        */
#define P1_PIN_NUM_MAX 31                            /*!< (unspecified)                                                        */
#define P1_PIN_NUM_SIZE 32                           /*!< (unspecified)                                                        */
#define P1_FEATURE_PINS_PRESENT 4294967295           /*!< (unspecified)                                                        */
#define P1_FEATURE_I5_LIBRARY 0                      /*!< (unspecified)                                                        */
#define P1_PIN_SENSE_MECHANISM 1                     /*!< I/O pins on this port have pin sense mechanism                       */
#define P1_DRIVECTRL 0                               /*!< (unspecified)                                                        */
#define P1_WEAKPU 1                                  /*!< This port has weak pull-ups on selected GPIO pins.                   */
#define P1_RETAIN 0                                  /*!< (unspecified)                                                        */
#define P1_PWRCTRL 0                                 /*!< (unspecified)                                                        */
#define P1_SLEWMODE 0                                /*!< (unspecified)                                                        */
#define P1_PULLSTR 0                                 /*!< (unspecified)                                                        */
#define P1_SUPPORT_1V2 0                             /*!< (unspecified)                                                        */
#define P1_PIN_OWNER_SEC 0                           /*!< (unspecified)                                                        */
#define P1_BIASCTRL 0                                /*!< (unspecified)                                                        */

#define P0_CTRLSEL_MAP1 0                            /*!< (unspecified)                                                        */
#define P0_CTRLSEL_MAP2 1                            /*!< (unspecified)                                                        */
#define P0_CTRLSEL_MAP3 0                            /*!< (unspecified)                                                        */
#define P0_CTRLSEL_MAP4 0                            /*!< (unspecified)                                                        */
#define P0_CTRLSEL_MAP5 0                            /*!< (unspecified)                                                        */
#define P0_PIN_NUM_MIN 0                             /*!< (unspecified)                                                        */
#define P0_PIN_NUM_MAX 4                             /*!< (unspecified)                                                        */
#define P0_PIN_NUM_SIZE 5                            /*!< (unspecified)                                                        */
#define P0_FEATURE_PINS_PRESENT 31                   /*!< (unspecified)                                                        */
#define P0_FEATURE_I5_LIBRARY 0                      /*!< (unspecified)                                                        */
#define P0_PIN_SENSE_MECHANISM 1                     /*!< I/O pins on this port have pin sense mechanism                       */
#define P0_DRIVECTRL 0                               /*!< (unspecified)                                                        */
#define P0_WEAKPU 0                                  /*!< (unspecified)                                                        */
#define P0_RETAIN 0                                  /*!< (unspecified)                                                        */
#define P0_PWRCTRL 0                                 /*!< (unspecified)                                                        */
#define P0_SLEWMODE 0                                /*!< (unspecified)                                                        */
#define P0_PULLSTR 0                                 /*!< (unspecified)                                                        */
#define P0_SUPPORT_1V2 0                             /*!< (unspecified)                                                        */
#define P0_PIN_OWNER_SEC 0                           /*!< (unspecified)                                                        */
#define P0_BIASCTRL 0                                /*!< (unspecified)                                                        */

/*GPIO Tasks and Events*/
#define GPIOTE_PRESENT 1
#define GPIOTE_COUNT 2

#define GPIOTE20_IRQ_COUNT 1
#define GPIOTE20_GPIOTE_NCHANNELS_MIN 0              /*!< Number of GPIOTE channels: 0..7                                      */
#define GPIOTE20_GPIOTE_NCHANNELS_MAX 7              /*!< Number of GPIOTE channels: 0..7                                      */
#define GPIOTE20_GPIOTE_NCHANNELS_SIZE 8             /*!< Number of GPIOTE channels: 0..7                                      */
#define GPIOTE20_GPIOTE_NPORTEVENTS_MIN 0            /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE20_GPIOTE_NPORTEVENTS_MAX 0            /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE20_GPIOTE_NPORTEVENTS_SIZE 1           /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE20_GPIOTE_NINTERRUPTS_MIN 0            /*!< Number of GPIOTE interrupts: 0..0                                    */
#define GPIOTE20_GPIOTE_NINTERRUPTS_MAX 0            /*!< Number of GPIOTE interrupts: 0..0                                    */
#define GPIOTE20_GPIOTE_NINTERRUPTS_SIZE 1           /*!< Number of GPIOTE interrupts: 0..0                                    */
#define GPIOTE20_HAS_PORT_EVENT 1                    /*!< (unspecified)                                                        */

#define GPIOTE30_IRQ_COUNT 1
#define GPIOTE30_GPIOTE_NCHANNELS_MIN 0              /*!< Number of GPIOTE channels: 0..3                                      */
#define GPIOTE30_GPIOTE_NCHANNELS_MAX 3              /*!< Number of GPIOTE channels: 0..3                                      */
#define GPIOTE30_GPIOTE_NCHANNELS_SIZE 4             /*!< Number of GPIOTE channels: 0..3                                      */
#define GPIOTE30_GPIOTE_NPORTEVENTS_MIN 0            /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE30_GPIOTE_NPORTEVENTS_MAX 0            /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE30_GPIOTE_NPORTEVENTS_SIZE 1           /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE30_GPIOTE_NINTERRUPTS_MIN 0            /*!< Number of GPIOTE interrupts: 0..0                                    */
#define GPIOTE30_GPIOTE_NINTERRUPTS_MAX 0            /*!< Number of GPIOTE interrupts: 0..0                                    */
#define GPIOTE30_GPIOTE_NINTERRUPTS_SIZE 1           /*!< Number of GPIOTE interrupts: 0..0                                    */
#define GPIOTE30_HAS_PORT_EVENT 1                    /*!< (unspecified)                                                        */

/*Quadrature Decoder*/
#define QDEC_PRESENT 1
#define QDEC_COUNT 1

/*Global Real-time counter*/
#define GRTC_PRESENT 1
#define GRTC_COUNT 1

#define GRTC_IRQ_COUNT 4
#define GRTC_MSBWIDTH_MIN 0                          /*!< Width of the RTCOUNTERH, RTCOMPAREH and RTCOMPARESYNCH registers :
                                                          0..14*/
#define GRTC_MSBWIDTH_MAX 14                         /*!< Width of the RTCOUNTERH, RTCOMPAREH and RTCOMPARESYNCH registers :
                                                          0..14*/
#define GRTC_MSBWIDTH_SIZE 15                        /*!< Width of the RTCOUNTERH, RTCOMPAREH and RTCOMPARESYNCH registers :
                                                          0..14*/
#define GRTC_NCC_MIN 0                               /*!< Number of compare/capture registers : 0..11                          */
#define GRTC_NCC_MAX 11                              /*!< Number of compare/capture registers : 0..11                          */
#define GRTC_NCC_SIZE 12                             /*!< Number of compare/capture registers : 0..11                          */
#define GRTC_NTIMEOUT_MIN 0                          /*!< Width of the TIMEOUT register : 0..15                                */
#define GRTC_NTIMEOUT_MAX 15                         /*!< Width of the TIMEOUT register : 0..15                                */
#define GRTC_NTIMEOUT_SIZE 16                        /*!< Width of the TIMEOUT register : 0..15                                */
#define GRTC_GRTC_NINTERRUPTS_MIN 0                  /*!< Number of GRTC interrupts : 0..3                                     */
#define GRTC_GRTC_NINTERRUPTS_MAX 3                  /*!< Number of GRTC interrupts : 0..3                                     */
#define GRTC_GRTC_NINTERRUPTS_SIZE 4                 /*!< Number of GRTC interrupts : 0..3                                     */
#define GRTC_PWMREGS 1                               /*!< The PWM registers are available.                                     */
#define GRTC_CLKOUTREG 1                             /*!< The CLKOUT register is available.                                    */
#define GRTC_CLKSELREG 1                             /*!< The CLKCFG.CLKSEL register is available.                             */
#define GRTC_CLKSELLFLPRC 1                          /*!< The CLKCFG.CLKSEL register supports LFLPRC.                          */
#define GRTC_CCADD_WRITE_ONLY 1                      /*!< The CC[n].CCADD register has write access only.                      */
#define GRTC_READY_STATUS_AND_EVENTS 1               /*!< The ready status and events are available.                           */
#define GRTC_MULTIINTERVAL 0                         /*!< (unspecified)                                                        */
#define GRTC_STATUSDEBUGREG 0                        /*!< (unspecified)                                                        */
#define GRTC_SYSCOUNTER_LOADED_STATUS 1              /*!< SYSCOUNTER[n].SYSCOUNTERH.LOADED status is available                 */
#define GRTC_CC_PAST_STATUS 1                        /*!< CC[n].CCEN.PASTCC status is available                                */
#define GRTC_SYSCOUNTER_WRITEABLE 1                  /*!< (unspecified)                                                        */

/*Tamper controller*/
#define TAMPC_PRESENT 1
#define TAMPC_COUNT 1

#define TAMPC_APSPIDEN 0                             /*!< (unspecified)                                                        */
#define TAMPC_PROTECT_INTRESETEN_CTRL_VALUE_RESET 1  /*!< Reset value of field VALUE in register PROTECT.INTRESETEN.CTRL: 1    */
#define TAMPC_TAMPERSWITCH 0                         /*!< (unspecified)                                                        */
#define TAMPC_SM4DISABLECM 0                         /*!< (unspecified)                                                        */
#define TAMPC_PROTECTRESETBEHAVIOR 0                 /*!< (unspecified)                                                        */
#define TAMPC_SPIDEN 0                               /*!< (unspecified)                                                        */
#define TAMPC_SPNIDEN 0                              /*!< (unspecified)                                                        */
#define TAMPC_ACTIVESHIELD 0                         /*!< (unspecified)                                                        */

/*Watchdog Timer*/
#define WDT_PRESENT 1
#define WDT_COUNT 1

#define WDT30_ALLOW_STOP 1                           /*!< (unspecified)                                                        */
#define WDT30_HAS_INTEN 0                            /*!< (unspecified)                                                        */

/*Clock management*/
#define CLOCK_PRESENT 1
#define CLOCK_COUNT 1

#define CLOCK_XOTUNE 1                               /*!< (unspecified)                                                        */
#define CLOCK_PLL24M 0                               /*!< (unspecified)                                                        */
#define CLOCK_VAR1_NXT 0                             /*!< (unspecified)                                                        */

/*Power control*/
#define POWER_PRESENT 1
#define POWER_COUNT 1

#define POWER_CONSTLATSTAT 1                         /*!< (unspecified)                                                        */

/*Reset control*/
#define RESET_PRESENT 1
#define RESET_COUNT 1

/*Oscillator control*/
#define OSCILLATORS_PRESENT 1
#define OSCILLATORS_COUNT 1

/*Voltage regulators*/
#define REGULATORS_PRESENT 1
#define REGULATORS_COUNT 1

#define REGULATORS_POF 1                             /*!< (unspecified)                                                        */

/* ==================================================== Baudrate settings ==================================================== */
/**
  * @brief UARTE.BAUDRATE register values for combinations of baudrate and core frequency
  */
typedef enum {
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud1200Core16M = 319488, /*!< 1200 baud (actual rate: 1190, -0.8 percent error), 16 MHz core
                                                               frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud2400Core16M = 643072, /*!< 2400 baud (actual rate: 2395, -0.2 percent error), 16 MHz core
                                                               frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud4800Core16M = 1286144, /*!< 4800 baud (actual rate: 4791, -0.2 percent error), 16 MHz core
                                                                frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud9600Core16M = 2576384, /*!< 9600 baud (actual rate: 9598, -0.0 percent error), 16 MHz core
                                                                frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud14400Core16M = 3862528, /*!< 14400 baud (actual rate: 14401, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud19200Core16M = 5152768, /*!< 19200 baud (actual rate: 19207, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud28800Core16M = 7720960, /*!< 28800 baud (actual rate: 28776, -0.1 percent error), 16 MHz
                                                                 core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud31250Core16M = 8388608, /*!< 31250 baud (actual rate: 31250, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud38400Core16M = 10297344, /*!< 38400 baud (actual rate: 38369, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud56000Core16M = 15015936, /*!< 56000 baud (actual rate: 55944, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud57600Core16M = 15446016, /*!< 57600 baud (actual rate: 57553, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud76800Core16M = 20647936, /*!< 76800 baud (actual rate: 76923, 0.2 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud115200Core16M = 30896128, /*!< 115200 baud (actual rate: 115107, -0.1 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud230400Core16M = 62242816, /*!< 230400 baud (actual rate: 231884, 0.6 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud250000Core16M = 67108864, /*!< 250000 baud (actual rate: 250000, 0.0 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud460800Core16M = 122712064, /*!< 460800 baud (actual rate: 457142, -0.8 percent error), 16
                                                                    MHz core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud921600Core16M = 252641280, /*!< 921600 baud (actual rate: 941176, 2.1 percent error), 16 MHz
                                                                    core frequency*/
  NRF_UARTE20_BAUDRATE_BAUDRATE_Baud1000000Core16M = 268435456, /*!< 1000000 baud (actual rate: 1000000, 0.0 percent error), 16
                                                                     MHz core frequency*/
} NRF_UARTE20_BAUDRATE_BAUDRATE_ENUM_t;

/* ==================================================== Baudrate settings ==================================================== */
/**
  * @brief UARTE.BAUDRATE register values for combinations of baudrate and core frequency
  */
typedef enum {
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud1200Core16M = 319488, /*!< 1200 baud (actual rate: 1190, -0.8 percent error), 16 MHz core
                                                               frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud2400Core16M = 643072, /*!< 2400 baud (actual rate: 2395, -0.2 percent error), 16 MHz core
                                                               frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud4800Core16M = 1286144, /*!< 4800 baud (actual rate: 4791, -0.2 percent error), 16 MHz core
                                                                frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud9600Core16M = 2576384, /*!< 9600 baud (actual rate: 9598, -0.0 percent error), 16 MHz core
                                                                frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud14400Core16M = 3862528, /*!< 14400 baud (actual rate: 14401, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud19200Core16M = 5152768, /*!< 19200 baud (actual rate: 19207, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud28800Core16M = 7720960, /*!< 28800 baud (actual rate: 28776, -0.1 percent error), 16 MHz
                                                                 core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud31250Core16M = 8388608, /*!< 31250 baud (actual rate: 31250, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud38400Core16M = 10297344, /*!< 38400 baud (actual rate: 38369, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud56000Core16M = 15015936, /*!< 56000 baud (actual rate: 55944, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud57600Core16M = 15446016, /*!< 57600 baud (actual rate: 57553, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud76800Core16M = 20647936, /*!< 76800 baud (actual rate: 76923, 0.2 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud115200Core16M = 30896128, /*!< 115200 baud (actual rate: 115107, -0.1 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud230400Core16M = 62242816, /*!< 230400 baud (actual rate: 231884, 0.6 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud250000Core16M = 67108864, /*!< 250000 baud (actual rate: 250000, 0.0 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud460800Core16M = 122712064, /*!< 460800 baud (actual rate: 457142, -0.8 percent error), 16
                                                                    MHz core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud921600Core16M = 252641280, /*!< 921600 baud (actual rate: 941176, 2.1 percent error), 16 MHz
                                                                    core frequency*/
  NRF_UARTE21_BAUDRATE_BAUDRATE_Baud1000000Core16M = 268435456, /*!< 1000000 baud (actual rate: 1000000, 0.0 percent error), 16
                                                                     MHz core frequency*/
} NRF_UARTE21_BAUDRATE_BAUDRATE_ENUM_t;

/* ==================================================== Baudrate settings ==================================================== */
/**
  * @brief UARTE.BAUDRATE register values for combinations of baudrate and core frequency
  */
typedef enum {
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud1200Core16M = 319488, /*!< 1200 baud (actual rate: 1190, -0.8 percent error), 16 MHz core
                                                               frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud2400Core16M = 643072, /*!< 2400 baud (actual rate: 2395, -0.2 percent error), 16 MHz core
                                                               frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud4800Core16M = 1286144, /*!< 4800 baud (actual rate: 4791, -0.2 percent error), 16 MHz core
                                                                frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud9600Core16M = 2576384, /*!< 9600 baud (actual rate: 9598, -0.0 percent error), 16 MHz core
                                                                frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud14400Core16M = 3862528, /*!< 14400 baud (actual rate: 14401, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud19200Core16M = 5152768, /*!< 19200 baud (actual rate: 19207, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud28800Core16M = 7720960, /*!< 28800 baud (actual rate: 28776, -0.1 percent error), 16 MHz
                                                                 core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud31250Core16M = 8388608, /*!< 31250 baud (actual rate: 31250, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud38400Core16M = 10297344, /*!< 38400 baud (actual rate: 38369, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud56000Core16M = 15015936, /*!< 56000 baud (actual rate: 55944, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud57600Core16M = 15446016, /*!< 57600 baud (actual rate: 57553, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud76800Core16M = 20647936, /*!< 76800 baud (actual rate: 76923, 0.2 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud115200Core16M = 30896128, /*!< 115200 baud (actual rate: 115107, -0.1 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud230400Core16M = 62242816, /*!< 230400 baud (actual rate: 231884, 0.6 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud250000Core16M = 67108864, /*!< 250000 baud (actual rate: 250000, 0.0 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud460800Core16M = 122712064, /*!< 460800 baud (actual rate: 457142, -0.8 percent error), 16
                                                                    MHz core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud921600Core16M = 252641280, /*!< 921600 baud (actual rate: 941176, 2.1 percent error), 16 MHz
                                                                    core frequency*/
  NRF_UARTE22_BAUDRATE_BAUDRATE_Baud1000000Core16M = 268435456, /*!< 1000000 baud (actual rate: 1000000, 0.0 percent error), 16
                                                                     MHz core frequency*/
} NRF_UARTE22_BAUDRATE_BAUDRATE_ENUM_t;


#ifdef __cplusplus
}
#endif
#endif /* NRF54LS05B_ENGA_APPLICATION_PERIPHERALS_H */

