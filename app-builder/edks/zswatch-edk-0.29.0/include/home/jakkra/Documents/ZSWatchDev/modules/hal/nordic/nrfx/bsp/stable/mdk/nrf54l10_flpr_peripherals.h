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

#ifndef NRF54L10_FLPR_PERIPHERALS_H
#define NRF54L10_FLPR_PERIPHERALS_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdbool.h>
/*VPR CSR registers*/
#define VPRCSR_PRESENT 1
#define VPRCSR_COUNT 1

#define VPRCSR_HARTNUM 14                            /*!< HARTNUM: 14                                                          */
#define VPRCSR_MCLICBASERESET 0xF0000000             /*!< MCLICBASE: 0xF0000000                                                */
#define VPRCSR_MULDIV 2                              /*!< MULDIV: 2                                                            */
#define VPRCSR_HIBERNATE 1                           /*!< HIBERNATE: 1                                                         */
#define VPRCSR_DBG 1                                 /*!< DBG: 1                                                               */
#define VPRCSR_REMAP 0                               /*!< Code patching (REMAP): 0                                             */
#define VPRCSR_BUSWIDTH 64                           /*!< BUSWIDTH: 64                                                         */
#define VPRCSR_BKPT 1                                /*!< BKPT: 1                                                              */
#define VPRCSR_RETAINED 1                            /*!< VPR can be retained.                                                 */
#define VPRCSR_VIOPINS 0x0000FFFF                    /*!< CSR VIOPINS value: 0x0000FFFF                                        */
#define VPRCSR_VEVIF_NTASKS_MIN 0                    /*!< VEVIF tasks: 0..31                                                   */
#define VPRCSR_VEVIF_NTASKS_MAX 31                   /*!< VEVIF tasks: 0..31                                                   */
#define VPRCSR_VEVIF_NTASKS_SIZE 32                  /*!< VEVIF tasks: 0..31                                                   */
#define VPRCSR_VEVIF_TASKS_MASK 0xFFFFFFFF           /*!< Mask of supported VEVIF tasks: 0xFFFFFFFF                            */
#define VPRCSR_VEVIF_NDPPI_MIN 16                    /*!< VEVIF DPPI indices: 16..19                                           */
#define VPRCSR_VEVIF_NDPPI_MAX 19                    /*!< VEVIF DPPI indices: 16..19                                           */
#define VPRCSR_VEVIF_NDPPI_SIZE 20                   /*!< VEVIF DPPI indices: 16..19                                           */
#define VPRCSR_VEVIF_NEVENTS_MIN 0                   /*!< VEVIF events: 0..31                                                  */
#define VPRCSR_VEVIF_NEVENTS_MAX 31                  /*!< VEVIF events: 0..31                                                  */
#define VPRCSR_VEVIF_NEVENTS_SIZE 32                 /*!< VEVIF events: 0..31                                                  */
#define VPRCSR_BEXT 1                                /*!< Bit-Manipulation extension: 1                                        */
#define VPRCSR_CACHE_EN 1                            /*!< CACHE available.                                                     */
#define VPRCSR_CACHEEXTRATAGBUF 0                    /*!< CACHEEXTRATAGBUF: 0                                                  */
#define VPRCSR_OUTMODE_VPR1_2 1                      /*!< OUTMODE for shifting functionality available                         */
#define VPRCSR_VPR_BUS_PRIO 1                        /*!< (unspecified)                                                        */
#define VPRCSR_NMIMPID_VPR1_3_3 0                    /*!< INSTNUM field is not available withing CSR MIMPID                    */
#define VPRCSR_PERIPHERALBLOCKINGACCESS 0            /*!< VPR does not support peripheral blocking access.                     */
#define VPRCSR_BRANCHPREDECODING 0                   /*!< VPR does not support branch predecoding.                             */
#define VPRCSR_RTP_VPR_1_5 0                         /*!< (unspecified)                                                        */

/*VPR CLIC registers*/
#define CLIC_PRESENT 1
#define CLIC_COUNT 1

#define VPRCLIC_IRQ_COUNT 32
#define VPRCLIC_IRQNUM_MIN 0                         /*!< Supported interrupts (IRQNUM): 0..270                                */
#define VPRCLIC_IRQNUM_MAX 270                       /*!< Supported interrupts (IRQNUM): 0..270                                */
#define VPRCLIC_IRQNUM_SIZE 271                      /*!< Supported interrupts (IRQNUM): 0..270                                */
#define VPRCLIC_CLIC_NTASKS_MIN 0                    /*!< VEVIF tasks: 0..31                                                   */
#define VPRCLIC_CLIC_NTASKS_MAX 31                   /*!< VEVIF tasks: 0..31                                                   */
#define VPRCLIC_CLIC_NTASKS_SIZE 32                  /*!< VEVIF tasks: 0..31                                                   */
#define VPRCLIC_CLIC_TASKS_MASK 0xFFFFFFFF           /*!< Mask of supported VEVIF tasks: 0xFFFFFFFF                            */
#define VPRCLIC_COUNTER_IRQ_NUM 31                   /*!< VPR counter (CNT0) interrupt handler number (COUNTER_IRQ_NUM): 31    */
#define VPRCLIC_CLIC_VPR_1_2 1                       /*!< CLIC configuration for VPR 1.2 enabled                               */

/*Factory Information Configuration Registers*/
#define FICR_PRESENT 1
#define FICR_COUNT 1

/*User Information Configuration Registers*/
#define UICR_PRESENT 1
#define UICR_COUNT 1

/*Factory Information Configuration Registers*/
#define SICR_PRESENT 1
#define SICR_COUNT 1

/*CRACENCORE*/
#define CRACENCORE_PRESENT 1
#define CRACENCORE_COUNT 1

#define CRACENCORE_CRYPTMSTRDMAREGS 1                /*!< CRYPTMSTRDMA registers included                                      */
#define CRACENCORE_CRYPTMSTRHWREGS 1                 /*!< CRYPTMSTRHW registers included                                       */
#define CRACENCORE_RNGCONTROLREGS 1                  /*!< RNGCONTROL registers included                                        */
#define CRACENCORE_PKREGS 1                          /*!< PK registers included                                                */
#define CRACENCORE_IKGREGS 1                         /*!< IKG registers included                                               */
#define CRACENCORE_RNGDATAREGS 1                     /*!< RNGDATA registers included                                           */
#define CRACENCORE_PKDATAMEMORYREGS 1                /*!< PKDATAMEMORY registers included                                      */
#define CRACENCORE_PKUCODEREGS 1                     /*!< PKUCODE registers included                                           */
#define CRACENCORE_CRACENRESETVALUES 1               /*!< Using CRACENCORE configuration reset values                          */
#define CRACENCORE_SHA3RESETVALUES 0                 /*!< (unspecified)                                                        */
#define CRACENCORE_PKE_DATA_MEMORY 0x51808000        /*!< (unspecified)                                                        */
#define CRACENCORE_PKE_DATA_MEMORY_SIZE 16384        /*!< (unspecified)                                                        */
#define CRACENCORE_PKE_CODE_MEMORY 0x5180C000        /*!< (unspecified)                                                        */
#define CRACENCORE_PKE_CODE_MEMORY_SIZE 5120         /*!< (unspecified)                                                        */

/*System protection unit*/
#define SPU_PRESENT 1
#define SPU_COUNT 4

#define SPU00_BELLS 0                                /*!< (unspecified)                                                        */
#define SPU00_IPCT 0                                 /*!< (unspecified)                                                        */
#define SPU00_DPPI 1                                 /*!< Supports FEATURE.DPPIC[n]                                            */
#define SPU00_GPIOTE 0                               /*!< (unspecified)                                                        */
#define SPU00_GRTC 0                                 /*!< (unspecified)                                                        */
#define SPU00_GPIO 1                                 /*!< Supports FEATURE.GPIO[n]                                             */
#define SPU00_CRACEN 1                               /*!< Supports FEATURE.CRACEN                                              */
#define SPU00_MRAMC 0                                /*!< (unspecified)                                                        */
#define SPU00_COEXC 0                                /*!< (unspecified)                                                        */
#define SPU00_ANTSWC 0                               /*!< (unspecified)                                                        */
#define SPU00_TDD 0                                  /*!< (unspecified)                                                        */
#define SPU00_SLAVE_BITS 4                           /*!< SLAVE_BITS=4 (number of address bits required to represent the
                                                          peripheral slave index)*/

#define SPU10_BELLS 0                                /*!< (unspecified)                                                        */
#define SPU10_IPCT 0                                 /*!< (unspecified)                                                        */
#define SPU10_DPPI 1                                 /*!< Supports FEATURE.DPPIC[n]                                            */
#define SPU10_GPIOTE 0                               /*!< (unspecified)                                                        */
#define SPU10_GRTC 0                                 /*!< (unspecified)                                                        */
#define SPU10_GPIO 0                                 /*!< (unspecified)                                                        */
#define SPU10_CRACEN 0                               /*!< (unspecified)                                                        */
#define SPU10_MRAMC 0                                /*!< (unspecified)                                                        */
#define SPU10_COEXC 0                                /*!< (unspecified)                                                        */
#define SPU10_ANTSWC 0                               /*!< (unspecified)                                                        */
#define SPU10_TDD 0                                  /*!< (unspecified)                                                        */
#define SPU10_SLAVE_BITS 4                           /*!< SLAVE_BITS=4 (number of address bits required to represent the
                                                          peripheral slave index)*/

#define SPU20_BELLS 0                                /*!< (unspecified)                                                        */
#define SPU20_IPCT 0                                 /*!< (unspecified)                                                        */
#define SPU20_DPPI 1                                 /*!< Supports FEATURE.DPPIC[n]                                            */
#define SPU20_GPIOTE 1                               /*!< Supports FEATURE.GPIOTE[n]                                           */
#define SPU20_GRTC 1                                 /*!< Supports FEATURE.GRTC[n]                                             */
#define SPU20_GPIO 1                                 /*!< Supports FEATURE.GPIO[n]                                             */
#define SPU20_CRACEN 0                               /*!< (unspecified)                                                        */
#define SPU20_MRAMC 0                                /*!< (unspecified)                                                        */
#define SPU20_COEXC 0                                /*!< (unspecified)                                                        */
#define SPU20_ANTSWC 0                               /*!< (unspecified)                                                        */
#define SPU20_TDD 0                                  /*!< (unspecified)                                                        */
#define SPU20_SLAVE_BITS 4                           /*!< SLAVE_BITS=4 (number of address bits required to represent the
                                                          peripheral slave index)*/

#define SPU30_BELLS 0                                /*!< (unspecified)                                                        */
#define SPU30_IPCT 0                                 /*!< (unspecified)                                                        */
#define SPU30_DPPI 1                                 /*!< Supports FEATURE.DPPIC[n]                                            */
#define SPU30_GPIOTE 1                               /*!< Supports FEATURE.GPIOTE[n]                                           */
#define SPU30_GRTC 0                                 /*!< (unspecified)                                                        */
#define SPU30_GPIO 1                                 /*!< Supports FEATURE.GPIO[n]                                             */
#define SPU30_CRACEN 0                               /*!< (unspecified)                                                        */
#define SPU30_MRAMC 0                                /*!< (unspecified)                                                        */
#define SPU30_COEXC 0                                /*!< (unspecified)                                                        */
#define SPU30_ANTSWC 0                               /*!< (unspecified)                                                        */
#define SPU30_TDD 0                                  /*!< (unspecified)                                                        */
#define SPU30_SLAVE_BITS 4                           /*!< SLAVE_BITS=4 (number of address bits required to represent the
                                                          peripheral slave index)*/

/*Memory Privilege Controller*/
#define MPC_PRESENT 1
#define MPC_COUNT 1

#define MPC00_EXTEND_CLOCK_REQ 1                     /*!< (unspecified)                                                        */
#define MPC00_RTCHOKE 0                              /*!< (unspecified)                                                        */
#define MPC00_OVERRIDE_GRAN 4096                     /*!< The override region granularity is 4096 bytes                        */

/*Distributed programmable peripheral interconnect controller*/
#define DPPIC_PRESENT 1
#define DPPIC_COUNT 4

#define DPPIC00_HASCHANNELGROUPS 1                   /*!< (unspecified)                                                        */
#define DPPIC00_CH_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define DPPIC00_CH_NUM_MAX 7                         /*!< (unspecified)                                                        */
#define DPPIC00_CH_NUM_SIZE 8                        /*!< (unspecified)                                                        */
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
#define PPIB00_NTASKSEVENTS_MAX 7                    /*!< (unspecified)                                                        */
#define PPIB00_NTASKSEVENTS_SIZE 8                   /*!< (unspecified)                                                        */

#define PPIB01_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB01_NTASKSEVENTS_MAX 7                    /*!< (unspecified)                                                        */
#define PPIB01_NTASKSEVENTS_SIZE 8                   /*!< (unspecified)                                                        */

#define PPIB10_NTASKSEVENTS_MIN 0                    /*!< (unspecified)                                                        */
#define PPIB10_NTASKSEVENTS_MAX 7                    /*!< (unspecified)                                                        */
#define PPIB10_NTASKSEVENTS_SIZE 8                   /*!< (unspecified)                                                        */

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

/*Key management unit*/
#define KMU_PRESENT 1
#define KMU_COUNT 1

#define KMU_KEYSLOTNUM 250                           /*!< Number of keyslots is 250                                            */
#define KMU_KEYSLOTBITS 128                          /*!< Number of bits per keyslot is 128                                    */
#define KMU_PUSHBLOCK 1                              /*!< (unspecified)                                                        */
#define KMU_BLOCK 0                                  /*!< (unspecified)                                                        */

/*Accelerated Address Resolver*/
#define AAR_PRESENT 1
#define AAR_COUNT 1

#define AAR00_DMAERROR 1                             /*!< (unspecified)                                                        */
#define AAR00_ERRORSTATUS 1                          /*!< (unspecified)                                                        */
#define AAR00_ERROREVENT 1                           /*!< (unspecified)                                                        */
#define AAR00_ERROREVENT_IRQ 0                       /*!< (unspecified)                                                        */
#define AAR00_PREMATUREOUTPUTPTR 0                   /*!< (unspecified)                                                        */

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

/*CRACEN*/
#define CRACEN_PRESENT 1
#define CRACEN_COUNT 1

#define CRACEN_CRYPTOACCELERATOR 1                   /*!< CRACEN CRYPTOACCELERATOR specific configuration registers included   */
#define CRACEN_SEEDRAMLOCK 0                         /*!< (unspecified)                                                        */
#define CRACEN_SPLITKEYRAMLOCK 1                     /*!< (unspecified)                                                        */
#define CRACEN_SEEDALIGNED 1                         /*!< (unspecified)                                                        */
#define CRACEN_ONLYPROTECTEDRAMLOCK 0                /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SEED 0x51810000         /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SEED_SIZE 64            /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_AES_KEY0 0x51810040     /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_AES_KEY0_SIZE 32        /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_AES_KEY1 0x51810060     /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_AES_KEY1_SIZE 32        /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SM4_KEY0 0x51810080     /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SM4_KEY0_SIZE 16        /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SM4_KEY1 0x51810090     /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SM4_KEY1_SIZE 16        /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SM4_KEY2 0x518100A0     /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SM4_KEY2_SIZE 16        /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SM4_KEY3 0x518100B0     /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_SM4_KEY3_SIZE 16        /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_RESERVED 0x518100C0     /*!< (unspecified)                                                        */
#define CRACEN_PROTECTED_RAM_RESERVED_SIZE 64        /*!< (unspecified)                                                        */
#define CRACEN_PKEDATA 0x51808000                    /*!< PKE data (address 0x51808000) must be read and written using aligned
                                                          access, i.e. using an operation where a word-aligned address is used
                                                          for a word, or a halfword-aligned address is used for a halfword
                                                          access.*/
#define CRACEN_PKECODE 0x5180C000                    /*!< PKE code (address 0x5180C000) must be read and written using aligned
                                                          access, i.e. using an operation where a word-aligned address is used
                                                          for a word, or a halfword-aligned address is used for a halfword
                                                          access.*/

/*Serial Peripheral Interface Master with EasyDMA*/
#define SPIM_PRESENT 1
#define SPIM_COUNT 5

#define SPIM00_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */
#define SPIM00_MAX_DATARATE 32                       /*!< (unspecified)                                                        */
#define SPIM00_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM00_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIM00_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIM00_FEATURE_HARDWARE_CSN_PRESENT 1        /*!< CSN functionality is supported.                                      */
#define SPIM00_FEATURE_HARDWARE_DCX_PRESENT 1        /*!< DCX functionality is supported.                                      */
#define SPIM00_FEATURE_RXDELAY_PRESENT 1             /*!< (unspecified)                                                        */
#define SPIM00_STALL_STATUS_PRESENT 0                /*!< (unspecified)                                                        */
#define SPIM00_STALL_STATUS_TX_PRESENT 1             /*!< (unspecified)                                                        */
#define SPIM00_NUM_CHIPSELECT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM00_NUM_CHIPSELECT_MAX 0                  /*!< (unspecified)                                                        */
#define SPIM00_NUM_CHIPSELECT_SIZE 1                 /*!< (unspecified)                                                        */
#define SPIM00_CORE_FREQUENCY 128                    /*!< Peripheral core frequency is 128 MHz.                                */
#define SPIM00_PRESCALER_PRESENT 1                   /*!< (unspecified)                                                        */
#define SPIM00_PRESCALER_DIVISOR_RANGE_MIN 4         /*!< Prescaler divisor range is 4..126                                    */
#define SPIM00_PRESCALER_DIVISOR_RANGE_MAX 126       /*!< Prescaler divisor range is 4..126                                    */
#define SPIM00_PRESCALER_DIVISOR_RANGE_SIZE 127      /*!< Prescaler divisor range is 4..126                                    */
#define SPIM00_RXDELAY_VALUE_RANGE_MIN 0             /*!< (unspecified)                                                        */
#define SPIM00_RXDELAY_VALUE_RANGE_MAX 7             /*!< (unspecified)                                                        */
#define SPIM00_RXDELAY_VALUE_RANGE_SIZE 8            /*!< (unspecified)                                                        */
#define SPIM00_RXDELAY_RESET_VALUE 2                 /*!< (unspecified)                                                        */
#define SPIM00_RXDELAY_FIELD_WIDTH_MIN 0             /*!< (unspecified)                                                        */
#define SPIM00_RXDELAY_FIELD_WIDTH_MAX 2             /*!< (unspecified)                                                        */
#define SPIM00_RXDELAY_FIELD_WIDTH_SIZE 3            /*!< (unspecified)                                                        */

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

#define SPIM30_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */
#define SPIM30_MAX_DATARATE 8                        /*!< (unspecified)                                                        */
#define SPIM30_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM30_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIM30_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIM30_FEATURE_HARDWARE_CSN_PRESENT 1        /*!< CSN functionality is supported.                                      */
#define SPIM30_FEATURE_HARDWARE_DCX_PRESENT 1        /*!< DCX functionality is supported.                                      */
#define SPIM30_FEATURE_RXDELAY_PRESENT 1             /*!< (unspecified)                                                        */
#define SPIM30_STALL_STATUS_PRESENT 0                /*!< (unspecified)                                                        */
#define SPIM30_STALL_STATUS_TX_PRESENT 0             /*!< (unspecified)                                                        */
#define SPIM30_NUM_CHIPSELECT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIM30_NUM_CHIPSELECT_MAX 0                  /*!< (unspecified)                                                        */
#define SPIM30_NUM_CHIPSELECT_SIZE 1                 /*!< (unspecified)                                                        */
#define SPIM30_CORE_FREQUENCY 16                     /*!< Peripheral core frequency is 16 MHz.                                 */
#define SPIM30_PRESCALER_PRESENT 1                   /*!< (unspecified)                                                        */
#define SPIM30_PRESCALER_DIVISOR_RANGE_MIN 2         /*!< Prescaler divisor range is 2..126                                    */
#define SPIM30_PRESCALER_DIVISOR_RANGE_MAX 126       /*!< Prescaler divisor range is 2..126                                    */
#define SPIM30_PRESCALER_DIVISOR_RANGE_SIZE 127      /*!< Prescaler divisor range is 2..126                                    */
#define SPIM30_RXDELAY_VALUE_RANGE_MIN 0             /*!< (unspecified)                                                        */
#define SPIM30_RXDELAY_VALUE_RANGE_MAX 7             /*!< (unspecified)                                                        */
#define SPIM30_RXDELAY_VALUE_RANGE_SIZE 8            /*!< (unspecified)                                                        */
#define SPIM30_RXDELAY_RESET_VALUE 1                 /*!< (unspecified)                                                        */
#define SPIM30_RXDELAY_FIELD_WIDTH_MIN 0             /*!< (unspecified)                                                        */
#define SPIM30_RXDELAY_FIELD_WIDTH_MAX 2             /*!< (unspecified)                                                        */
#define SPIM30_RXDELAY_FIELD_WIDTH_SIZE 3            /*!< (unspecified)                                                        */

/*SPI Slave*/
#define SPIS_PRESENT 1
#define SPIS_COUNT 5

#define SPIS00_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIS00_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIS00_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIS00_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

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

#define SPIS30_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define SPIS30_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define SPIS30_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define SPIS30_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

/*UART with EasyDMA*/
#define UARTE_PRESENT 1
#define UARTE_COUNT 5

#define UARTE00_EASYDMA_MAXCNT_MIN 0                 /*!< (unspecified)                                                        */
#define UARTE00_EASYDMA_MAXCNT_MAX 15                /*!< (unspecified)                                                        */
#define UARTE00_EASYDMA_MAXCNT_SIZE 16               /*!< (unspecified)                                                        */
#define UARTE00_TIMEOUT_INTERRUPT 1                  /*!< Timeout interrupt is included.                                       */
#define UARTE00_CONFIGURABLE_DATA_FRAME_SIZE 1       /*!< Supports data frame sizes 4, 5, 6, 7, 8, and 9 bits.                 */
#define UARTE00_CORE_FREQUENCY 128                   /*!< Peripheral clock frequency is 128 MHz.                               */
#define UARTE00_CORE_CLOCK_128 1                     /*!< (unspecified)                                                        */
#define UARTE00_SHORTS_ENDTX_STOPTX 1                /*!< (unspecified)                                                        */
#define UARTE00_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                  */

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

#define UARTE30_EASYDMA_MAXCNT_MIN 0                 /*!< (unspecified)                                                        */
#define UARTE30_EASYDMA_MAXCNT_MAX 15                /*!< (unspecified)                                                        */
#define UARTE30_EASYDMA_MAXCNT_SIZE 16               /*!< (unspecified)                                                        */
#define UARTE30_TIMEOUT_INTERRUPT 1                  /*!< Timeout interrupt is included.                                       */
#define UARTE30_CONFIGURABLE_DATA_FRAME_SIZE 1       /*!< Supports data frame sizes 4, 5, 6, 7, 8, and 9 bits.                 */
#define UARTE30_CORE_FREQUENCY 16                    /*!< Peripheral clock frequency is 16 MHz.                                */
#define UARTE30_CORE_CLOCK_16 1                      /*!< (unspecified)                                                        */
#define UARTE30_SHORTS_ENDTX_STOPTX 1                /*!< (unspecified)                                                        */
#define UARTE30_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                  */

/*Voltage glitch detectors*/
#define GLITCHDET_PRESENT 1
#define GLITCHDET_COUNT 1

/*RRAM controller GLITCH detector*/
#define RRAMC_PRESENT 1
#define RRAMC_COUNT 1

#define RRAMC_NAPMODE 1                              /*!< (unspecified)                                                        */
#define RRAMC_NRRAMWORDSIZE 128                      /*!< RRAM word size : 128 bits per wordline                               */
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
#define RRAMC_REGION0SECURE 0                        /*!< (unspecified)                                                        */
#define RRAMC_REGION0SECURE_WRITABLE 0               /*!< (unspecified)                                                        */
#define RRAMC_REGION0OWNER 0                         /*!< (unspecified)                                                        */
#define RRAMC_REGION0OWNER_WRITABLE 1                /*!< (unspecified)                                                        */
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
#define RRAMC_REGION1SECURE 1                        /*!< (unspecified)                                                        */
#define RRAMC_REGION1SECURE_WRITABLE 1               /*!< (unspecified)                                                        */
#define RRAMC_REGION1OWNER 0                         /*!< (unspecified)                                                        */
#define RRAMC_REGION1OWNER_WRITABLE 1                /*!< (unspecified)                                                        */
#define RRAMC_REGION1WRITEONCE 1                     /*!< (unspecified)                                                        */
#define RRAMC_REGION1WRITEONCE_WRITABLE 0            /*!< (unspecified)                                                        */
#define RRAMC_REGION1LOCK 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION1LOCK_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION2ADDR_WRITABLE 0                 /*!< (unspecified)                                                        */
#define RRAMC_REGION2SIZE 8                          /*!< (unspecified)                                                        */
#define RRAMC_REGION2SIZE_WRITABLE 0                 /*!< (unspecified)                                                        */
#define RRAMC_REGION2READ 1                          /*!< (unspecified)                                                        */
#define RRAMC_REGION2READ_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_REGION2WRITE 1                         /*!< (unspecified)                                                        */
#define RRAMC_REGION2WRITE_WRITABLE 1                /*!< (unspecified)                                                        */
#define RRAMC_REGION2EXECUTE 0                       /*!< (unspecified)                                                        */
#define RRAMC_REGION2EXECUTE_WRITABLE 0              /*!< (unspecified)                                                        */
#define RRAMC_REGION2SECURE 1                        /*!< (unspecified)                                                        */
#define RRAMC_REGION2SECURE_WRITABLE 0               /*!< (unspecified)                                                        */
#define RRAMC_REGION2OWNER 2                         /*!< (unspecified)                                                        */
#define RRAMC_REGION2OWNER_WRITABLE 0                /*!< (unspecified)                                                        */
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
#define RRAMC_REGION3SECURE 1                        /*!< (unspecified)                                                        */
#define RRAMC_REGION3SECURE_WRITABLE 1               /*!< (unspecified)                                                        */
#define RRAMC_REGION3OWNER 0                         /*!< (unspecified)                                                        */
#define RRAMC_REGION3OWNER_WRITABLE 1                /*!< (unspecified)                                                        */
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
#define RRAMC_REGION4SECURE 1                        /*!< (unspecified)                                                        */
#define RRAMC_REGION4SECURE_WRITABLE 1               /*!< (unspecified)                                                        */
#define RRAMC_REGION4OWNER 0                         /*!< (unspecified)                                                        */
#define RRAMC_REGION4OWNER_WRITABLE 1                /*!< (unspecified)                                                        */
#define RRAMC_REGION4WRITEONCE 0                     /*!< (unspecified)                                                        */
#define RRAMC_REGION4WRITEONCE_WRITABLE 1            /*!< (unspecified)                                                        */
#define RRAMC_REGION4LOCK 0                          /*!< (unspecified)                                                        */
#define RRAMC_REGION4LOCK_WRITABLE 1                 /*!< (unspecified)                                                        */
#define RRAMC_GLITCHDETECTORS 0                      /*!< (unspecified)                                                        */

/*VPR peripheral registers*/
#define VPR_PRESENT 1
#define VPR_COUNT 1

#define VPR00_RISCV_EXTN_E 1                         /*!< Supports RV32E (Base Integer Instruction Set embedded)               */
#define VPR00_RISCV_EXTN_M 1                         /*!< Supports M extension (Integer Multiplication and Division)           */
#define VPR00_RISCV_EXTN_C 1                         /*!< Supports C extension (compressed instructions)                       */
#define VPR00_RISCV_EXTN_ZBA 1                       /*!< Supports Zba extension (Bit Manipulation - Address generation
                                                          instructions)*/
#define VPR00_RISCV_EXTN_ZBB 1                       /*!< Supports Zbb extension (Bit Manipulation - Basic bit manipulation)   */
#define VPR00_RISCV_EXTN_ZBC 1                       /*!< Supports Zbc extension (Bit Manipulation - Carry-less multiplication)*/
#define VPR00_RISCV_EXTN_ZBS 1                       /*!< Supports Zbs extension (Bit Manipulation - Single bit instructions)  */
#define VPR00_RISCV_EXTN_ZCB 1                       /*!< Supports Zcb extension (code-size saving instructions)               */
#define VPR00_RISCV_EXTN_ZIFENCEI 0                  /*!< Does not support FENCE.I instruction (use FENCE instruction instead) */
#define VPR00_RISCV_EXTN_ZICSR 1                     /*!< Supports CSR (Control and Status Register) instructions              */
#define VPR00_RISCV_EXTN_ZICNTR 0                    /*!< Does not support CNTR (base counter) instructions                    */
#define VPR00_RISCV_EXTN_SMCLIC 1                    /*!< Supports M-mode CLIC (interrupt controller)                          */
#define VPR00_RISCV_EXTN_SMCLICCONFIG 1              /*!< Supports MCLICCFG register                                           */
#define VPR00_RISCV_EXTN_SDEXT 1                     /*!< Supports external debugger                                           */
#define VPR00_RISCV_EXTN_SDTRIG 1                    /*!< Debugger supports triggers (breakpoints)                             */
#define VPR00_INIT_PC_RESET_VALUE 0x00000000         /*!< Boot vector (INIT_PC_RESET_VALUE): 0x00000000                        */
#define VPR00_VPR_START_RESET_VALUE 0                /*!< Self-booting (VPR_START_RESET_VALUE): 0                              */
#define VPR00_RAM_BASE_ADDR 0x20000000               /*!< VPR RAM base address (RAM_BASE_ADDR): 0x20000000                     */
#define VPR00_RAM_SZ 20                              /*!< VPR RAM size (RAM_SZ): 20 (Value in bytes is computed as 2^(RAM
                                                          size))*/
#define VPR00_VPRSAVEDCTX_REGNAME NRF_MEMCONF->POWER[1].RET /*!< (unspecified)                                                 */
#define VPR00_VPRSAVEDCTX_REGBIT 0                   /*!< (unspecified)                                                        */
#define VPR00_RETAINED 0                             /*!< Retain registers in Deep Sleep mode: 0                               */
#define VPR00_VPRSAVEDCTX 1                          /*!< Restore VPR context at VPR reset using register
                                                          [NRF_MEMCONF->POWER1.RET].MEM[0]*/
#define VPR00_VPRSAVEADDR 0x2003FE00                 /*!< VPR context save address: 0x2003FE00                                 */
#define VPR00_VPRSAVESIZE 512                        /*!< VPR context save size: 512 bytes                                     */
#define VPR00_VPRREMAPADDRVTOB 0x00000000            /*!< VPR remap address: 0x00000000                                        */
#define VPR00_VEVIF_NTASKS_MIN 16                    /*!< VEVIF tasks: 16..22                                                  */
#define VPR00_VEVIF_NTASKS_MAX 22                    /*!< VEVIF tasks: 16..22                                                  */
#define VPR00_VEVIF_NTASKS_SIZE 23                   /*!< VEVIF tasks: 16..22                                                  */
#define VPR00_VEVIF_TASKS_MASK 0x007F0000            /*!< Mask of supported VEVIF tasks: 0x007F0000                            */
#define VPR00_VEVIF_NDPPI_MIN 16                     /*!< VEVIF DPPI indices: 16..19                                           */
#define VPR00_VEVIF_NDPPI_MAX 19                     /*!< VEVIF DPPI indices: 16..19                                           */
#define VPR00_VEVIF_NDPPI_SIZE 20                    /*!< VEVIF DPPI indices: 16..19                                           */
#define VPR00_VEVIF_DPPI_MASK 0x000F0000             /*!< Mask of supported VEVIF DPPI channels: 0x000F0000                    */
#define VPR00_VEVIF_NEVENTS_MIN 16                   /*!< VEVIF events: 16..22                                                 */
#define VPR00_VEVIF_NEVENTS_MAX 22                   /*!< VEVIF events: 16..22                                                 */
#define VPR00_VEVIF_NEVENTS_SIZE 23                  /*!< VEVIF events: 16..22                                                 */
#define VPR00_VEVIF_EVENTS_MASK 0x00100000           /*!< Mask of supported VEVIF events: 0x00100000                           */
#define VPR00_DEBUGGER_OFFSET 1024                   /*!< Debugger interface register offset: 0x5004C400                       */
#define VPR00_RTP_VPR_1_5 0                          /*!< (unspecified)                                                        */

/*GPIO high-speed pad control*/
#define GPIOHSPADCTRL_PRESENT 1
#define GPIOHSPADCTRL_COUNT 1

/*GPIO Port*/
#define GPIO_PRESENT 1
#define GPIO_COUNT 3

#define P2_CTRLSEL_MAP1 0                            /*!< (unspecified)                                                        */
#define P2_CTRLSEL_MAP2 1                            /*!< (unspecified)                                                        */
#define P2_CTRLSEL_MAP3 0                            /*!< (unspecified)                                                        */
#define P2_CTRLSEL_MAP4 0                            /*!< (unspecified)                                                        */
#define P2_CTRLSEL_MAP5 0                            /*!< (unspecified)                                                        */
#define P2_PIN_NUM_MIN 0                             /*!< (unspecified)                                                        */
#define P2_PIN_NUM_MAX 10                            /*!< (unspecified)                                                        */
#define P2_PIN_NUM_SIZE 11                           /*!< (unspecified)                                                        */
#define P2_FEATURE_PINS_PRESENT 2047                 /*!< (unspecified)                                                        */
#define P2_FEATURE_I5_LIBRARY 0                      /*!< (unspecified)                                                        */
#define P2_PIN_SENSE_MECHANISM 0                     /*!< (unspecified)                                                        */
#define P2_DRIVECTRL 0                               /*!< (unspecified)                                                        */
#define P2_WEAKPU 0                                  /*!< (unspecified)                                                        */
#define P2_RETAIN 0                                  /*!< (unspecified)                                                        */
#define P2_PWRCTRL 0                                 /*!< (unspecified)                                                        */
#define P2_SLEWMODE 0                                /*!< (unspecified)                                                        */
#define P2_PULLSTR 0                                 /*!< (unspecified)                                                        */
#define P2_SUPPORT_1V2 0                             /*!< (unspecified)                                                        */
#define P2_PIN_OWNER_SEC 0                           /*!< (unspecified)                                                        */
#define P2_BIASCTRL 0                                /*!< (unspecified)                                                        */

#define P1_CTRLSEL_MAP1 0                            /*!< (unspecified)                                                        */
#define P1_CTRLSEL_MAP2 1                            /*!< (unspecified)                                                        */
#define P1_CTRLSEL_MAP3 0                            /*!< (unspecified)                                                        */
#define P1_CTRLSEL_MAP4 0                            /*!< (unspecified)                                                        */
#define P1_CTRLSEL_MAP5 0                            /*!< (unspecified)                                                        */
#define P1_PIN_NUM_MIN 0                             /*!< (unspecified)                                                        */
#define P1_PIN_NUM_MAX 16                            /*!< (unspecified)                                                        */
#define P1_PIN_NUM_SIZE 17                           /*!< (unspecified)                                                        */
#define P1_FEATURE_PINS_PRESENT 131071               /*!< (unspecified)                                                        */
#define P1_FEATURE_I5_LIBRARY 0                      /*!< (unspecified)                                                        */
#define P1_PIN_SENSE_MECHANISM 1                     /*!< I/O pins on this port have pin sense mechanism                       */
#define P1_DRIVECTRL 0                               /*!< (unspecified)                                                        */
#define P1_WEAKPU 0                                  /*!< (unspecified)                                                        */
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
#define P0_PIN_NUM_MAX 6                             /*!< (unspecified)                                                        */
#define P0_PIN_NUM_SIZE 7                            /*!< (unspecified)                                                        */
#define P0_FEATURE_PINS_PRESENT 127                  /*!< (unspecified)                                                        */
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
#define TIMER_COUNT 7

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

#define TIMER21_CC_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define TIMER21_CC_NUM_MAX 5                         /*!< (unspecified)                                                        */
#define TIMER21_CC_NUM_SIZE 6                        /*!< (unspecified)                                                        */
#define TIMER21_MAX_SIZE_MIN 0                       /*!< (unspecified)                                                        */
#define TIMER21_MAX_SIZE_MAX 31                      /*!< (unspecified)                                                        */
#define TIMER21_MAX_SIZE_SIZE 32                     /*!< (unspecified)                                                        */
#define TIMER21_PCLK_MHZ 16                          /*!< Peripheral clock frequency (PCLK) is 16 MHz                          */
#define TIMER21_PCLK_VARIABLE 0                      /*!< (unspecified)                                                        */
#define TIMER21_SHUTDOWN_TASK 0                      /*!< (unspecified)                                                        */

#define TIMER22_CC_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define TIMER22_CC_NUM_MAX 5                         /*!< (unspecified)                                                        */
#define TIMER22_CC_NUM_SIZE 6                        /*!< (unspecified)                                                        */
#define TIMER22_MAX_SIZE_MIN 0                       /*!< (unspecified)                                                        */
#define TIMER22_MAX_SIZE_MAX 31                      /*!< (unspecified)                                                        */
#define TIMER22_MAX_SIZE_SIZE 32                     /*!< (unspecified)                                                        */
#define TIMER22_PCLK_MHZ 16                          /*!< Peripheral clock frequency (PCLK) is 16 MHz                          */
#define TIMER22_PCLK_VARIABLE 0                      /*!< (unspecified)                                                        */
#define TIMER22_SHUTDOWN_TASK 0                      /*!< (unspecified)                                                        */

#define TIMER23_CC_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define TIMER23_CC_NUM_MAX 5                         /*!< (unspecified)                                                        */
#define TIMER23_CC_NUM_SIZE 6                        /*!< (unspecified)                                                        */
#define TIMER23_MAX_SIZE_MIN 0                       /*!< (unspecified)                                                        */
#define TIMER23_MAX_SIZE_MAX 31                      /*!< (unspecified)                                                        */
#define TIMER23_MAX_SIZE_SIZE 32                     /*!< (unspecified)                                                        */
#define TIMER23_PCLK_MHZ 16                          /*!< Peripheral clock frequency (PCLK) is 16 MHz                          */
#define TIMER23_PCLK_VARIABLE 0                      /*!< (unspecified)                                                        */
#define TIMER23_SHUTDOWN_TASK 0                      /*!< (unspecified)                                                        */

#define TIMER24_CC_NUM_MIN 0                         /*!< (unspecified)                                                        */
#define TIMER24_CC_NUM_MAX 5                         /*!< (unspecified)                                                        */
#define TIMER24_CC_NUM_SIZE 6                        /*!< (unspecified)                                                        */
#define TIMER24_MAX_SIZE_MIN 0                       /*!< (unspecified)                                                        */
#define TIMER24_MAX_SIZE_MAX 31                      /*!< (unspecified)                                                        */
#define TIMER24_MAX_SIZE_SIZE 32                     /*!< (unspecified)                                                        */
#define TIMER24_PCLK_MHZ 16                          /*!< Peripheral clock frequency (PCLK) is 16 MHz                          */
#define TIMER24_PCLK_VARIABLE 0                      /*!< (unspecified)                                                        */
#define TIMER24_SHUTDOWN_TASK 0                      /*!< (unspecified)                                                        */

/*Event generator unit*/
#define EGU_PRESENT 1
#define EGU_COUNT 2

#define EGU10_PEND 0                                 /*!< (unspecified)                                                        */
#define EGU10_CH_NUM_MIN 0                           /*!< (unspecified)                                                        */
#define EGU10_CH_NUM_MAX 15                          /*!< (unspecified)                                                        */
#define EGU10_CH_NUM_SIZE 16                         /*!< (unspecified)                                                        */

#define EGU20_PEND 0                                 /*!< (unspecified)                                                        */
#define EGU20_CH_NUM_MIN 0                           /*!< (unspecified)                                                        */
#define EGU20_CH_NUM_MAX 5                           /*!< (unspecified)                                                        */
#define EGU20_CH_NUM_SIZE 6                          /*!< (unspecified)                                                        */

/*2.4 GHz radio*/
#define RADIO_PRESENT 1
#define RADIO_COUNT 1

#define RADIO_IRQ_COUNT 2
#define RADIO_WHITENINGPOLY 1                        /*!< (unspecified)                                                        */
#define RADIO_ADPLLCOMPANION_INCLUDE_DMA 0           /*!< No internal instantiation of DmaChannelPeripheral                    */

/*I2C compatible Two-Wire Master Interface with EasyDMA*/
#define TWIM_PRESENT 1
#define TWIM_COUNT 4

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

#define TWIM30_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define TWIM30_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define TWIM30_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define TWIM30_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

/*I2C compatible Two-Wire Slave Interface with EasyDMA*/
#define TWIS_PRESENT 1
#define TWIS_COUNT 4

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

#define TWIS30_EASYDMA_MAXCNT_MIN 0                  /*!< (unspecified)                                                        */
#define TWIS30_EASYDMA_MAXCNT_MAX 15                 /*!< (unspecified)                                                        */
#define TWIS30_EASYDMA_MAXCNT_SIZE 16                /*!< (unspecified)                                                        */
#define TWIS30_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< (unspecified)                                                   */

/*Memory configuration*/
#define MEMCONF_PRESENT 1
#define MEMCONF_COUNT 1

#define MEMCONF_RETTRIM 1                            /*!< (unspecified)                                                        */
#define MEMCONF_REPAIR 0                             /*!< (unspecified)                                                        */
#define MEMCONF_POWER 1                              /*!< (unspecified)                                                        */

/*Pulse Density Modulation (Digital Microphone) Interface*/
#define PDM_PRESENT 1
#define PDM_COUNT 2

#define PDM20_SAMPLE16 0                             /*!< (unspecified)                                                        */
#define PDM20_SAMPLE48 1                             /*!< Supports 8, 16, 32, 48 kHz sample rate.                              */
#define PDM20_PRESCALER_PRESENT 1                    /*!< (unspecified)                                                        */
#define PDM20_PDMV2 0                                /*!< (unspecified)                                                        */
#define PDM20_PCLK24M 0                              /*!< (unspecified)                                                        */
#define PDM20_AUDIOPLL 0                             /*!< (unspecified)                                                        */
#define PDM20_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 1 /*!< CURRENTAMOUNT register included.                                 */

#define PDM21_SAMPLE16 0                             /*!< (unspecified)                                                        */
#define PDM21_SAMPLE48 1                             /*!< Supports 8, 16, 32, 48 kHz sample rate.                              */
#define PDM21_PRESCALER_PRESENT 1                    /*!< (unspecified)                                                        */
#define PDM21_PDMV2 0                                /*!< (unspecified)                                                        */
#define PDM21_PCLK24M 0                              /*!< (unspecified)                                                        */
#define PDM21_AUDIOPLL 0                             /*!< (unspecified)                                                        */
#define PDM21_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 1 /*!< CURRENTAMOUNT register included.                                 */

/*Pulse width modulation unit*/
#define PWM_PRESENT 1
#define PWM_COUNT 3

#define PWM20_IDLE_OUT 1                             /*!< IDLEOUT register is available.                                       */
#define PWM20_COMPARE_MATCH 1                        /*!< EVENTS_COMPAREMATCH events are available.                            */
#define PWM20_FEATURES_V2 0                          /*!< (unspecified)                                                        */
#define PWM20_NO_FEATURES_V2 1                       /*!< (unspecified)                                                        */
#define PWM20_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 1 /*!< CURRENTAMOUNT register included.                                 */

#define PWM21_IDLE_OUT 1                             /*!< IDLEOUT register is available.                                       */
#define PWM21_COMPARE_MATCH 1                        /*!< EVENTS_COMPAREMATCH events are available.                            */
#define PWM21_FEATURES_V2 0                          /*!< (unspecified)                                                        */
#define PWM21_NO_FEATURES_V2 1                       /*!< (unspecified)                                                        */
#define PWM21_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 1 /*!< CURRENTAMOUNT register included.                                 */

#define PWM22_IDLE_OUT 1                             /*!< IDLEOUT register is available.                                       */
#define PWM22_COMPARE_MATCH 1                        /*!< EVENTS_COMPAREMATCH events are available.                            */
#define PWM22_FEATURES_V2 0                          /*!< (unspecified)                                                        */
#define PWM22_NO_FEATURES_V2 1                       /*!< (unspecified)                                                        */
#define PWM22_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 1 /*!< CURRENTAMOUNT register included.                                 */

/*Analog to Digital Converter*/
#define SAADC_PRESENT 1
#define SAADC_COUNT 1

#define SAADC_CHOPPING 0                             /*!< (unspecified)                                                        */
#define SAADC_BURSTREG 0                             /*!< (unspecified)                                                        */
#define SAADC_PSEL_V2 1                              /*!< (unspecified)                                                        */
#define SAADC_TASKS_CALIBRATEGAIN 1                  /*!< (unspecified)                                                        */
#define SAADC_SAMPLERATE_CC_VALUERANGE_MIN 8         /*!< (unspecified)                                                        */
#define SAADC_SAMPLERATE_CC_VALUERANGE_MAX 2047      /*!< (unspecified)                                                        */
#define SAADC_SAMPLERATE_CC_VALUERANGE_SIZE 2048     /*!< (unspecified)                                                        */
#define SAADC_TACQ_VALUE_RANGE_MIN 1                 /*!< (unspecified)                                                        */
#define SAADC_TACQ_VALUE_RANGE_MAX 319               /*!< (unspecified)                                                        */
#define SAADC_TACQ_VALUE_RANGE_SIZE 320              /*!< (unspecified)                                                        */
#define SAADC_TCONV_VALUE_RANGE_MIN 1                /*!< (unspecified)                                                        */
#define SAADC_TCONV_VALUE_RANGE_MAX 7                /*!< (unspecified)                                                        */
#define SAADC_TCONV_VALUE_RANGE_SIZE 8               /*!< (unspecified)                                                        */
#define SAADC_EASYDMA_CURRENT_AMOUNT_REGISTER_INCLUDED 0 /*!< CURRENTAMOUNT register not included.                             */

/*NFC-A compatible radio NFC-A compatible radio*/
#define NFCT_PRESENT 1
#define NFCT_COUNT 1

#define NFCT_NFCTFIELDDETCFG_RESET 1                 /*!< Reset value of register NFCTFIELDDETCFG: 1                           */

/*Temperature Sensor*/
#define TEMP_PRESENT 1
#define TEMP_COUNT 1

/*GPIO Tasks and Events*/
#define GPIOTE_PRESENT 1
#define GPIOTE_COUNT 2

#define GPIOTE20_IRQ_COUNT 2
#define GPIOTE20_GPIOTE_NCHANNELS_MIN 0              /*!< Number of GPIOTE channels: 0..7                                      */
#define GPIOTE20_GPIOTE_NCHANNELS_MAX 7              /*!< Number of GPIOTE channels: 0..7                                      */
#define GPIOTE20_GPIOTE_NCHANNELS_SIZE 8             /*!< Number of GPIOTE channels: 0..7                                      */
#define GPIOTE20_GPIOTE_NPORTEVENTS_MIN 0            /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE20_GPIOTE_NPORTEVENTS_MAX 0            /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE20_GPIOTE_NPORTEVENTS_SIZE 1           /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE20_GPIOTE_NINTERRUPTS_MIN 0            /*!< Number of GPIOTE interrupts: 0..1                                    */
#define GPIOTE20_GPIOTE_NINTERRUPTS_MAX 1            /*!< Number of GPIOTE interrupts: 0..1                                    */
#define GPIOTE20_GPIOTE_NINTERRUPTS_SIZE 2           /*!< Number of GPIOTE interrupts: 0..1                                    */
#define GPIOTE20_HAS_PORT_EVENT 1                    /*!< (unspecified)                                                        */

#define GPIOTE30_IRQ_COUNT 2
#define GPIOTE30_GPIOTE_NCHANNELS_MIN 0              /*!< Number of GPIOTE channels: 0..3                                      */
#define GPIOTE30_GPIOTE_NCHANNELS_MAX 3              /*!< Number of GPIOTE channels: 0..3                                      */
#define GPIOTE30_GPIOTE_NCHANNELS_SIZE 4             /*!< Number of GPIOTE channels: 0..3                                      */
#define GPIOTE30_GPIOTE_NPORTEVENTS_MIN 0            /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE30_GPIOTE_NPORTEVENTS_MAX 0            /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE30_GPIOTE_NPORTEVENTS_SIZE 1           /*!< Number of GPIOTE port events: 0..0                                   */
#define GPIOTE30_GPIOTE_NINTERRUPTS_MIN 0            /*!< Number of GPIOTE interrupts: 0..1                                    */
#define GPIOTE30_GPIOTE_NINTERRUPTS_MAX 1            /*!< Number of GPIOTE interrupts: 0..1                                    */
#define GPIOTE30_GPIOTE_NINTERRUPTS_SIZE 2           /*!< Number of GPIOTE interrupts: 0..1                                    */
#define GPIOTE30_HAS_PORT_EVENT 1                    /*!< (unspecified)                                                        */

/*Tamper controller*/
#define TAMPC_PRESENT 1
#define TAMPC_COUNT 1

#define TAMPC_APSPIDEN 0                             /*!< (unspecified)                                                        */
#define TAMPC_PROTECT_INTRESETEN_CTRL_VALUE_RESET 1  /*!< Reset value of field VALUE in register PROTECT.INTRESETEN.CTRL: 1    */
#define TAMPC_TAMPERSWITCH 0                         /*!< (unspecified)                                                        */
#define TAMPC_SM4DISABLECM 1                         /*!< (unspecified)                                                        */
#define TAMPC_PROTECTRESETBEHAVIOR 0                 /*!< (unspecified)                                                        */
#define TAMPC_SPIDEN 1                               /*!< (unspecified)                                                        */
#define TAMPC_SPNIDEN 1                              /*!< (unspecified)                                                        */
#define TAMPC_ACTIVESHIELD 1                         /*!< (unspecified)                                                        */

/*Inter-IC Sound*/
#define I2S_PRESENT 1
#define I2S_COUNT 1

#define I2S20_EASYDMA_MAXCNT_SIZE_MIN 0              /*!< (unspecified)                                                        */
#define I2S20_EASYDMA_MAXCNT_SIZE_MAX 13             /*!< (unspecified)                                                        */
#define I2S20_EASYDMA_MAXCNT_SIZE_SIZE 14            /*!< (unspecified)                                                        */
#define I2S20_CLKCONFIG 0                            /*!< (unspecified)                                                        */

/*Quadrature Decoder*/
#define QDEC_PRESENT 1
#define QDEC_COUNT 2

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
#define GRTC_SYSCOUNTER_LOADED_STATUS 0              /*!< SYSCOUNTER[n].SYSCOUNTERH.LOADED status is not available             */
#define GRTC_CC_PAST_STATUS 0                        /*!< CC[n].CCEN.PASTCC status is not available                            */
#define GRTC_SYSCOUNTER_WRITEABLE 0                  /*!< (unspecified)                                                        */

/*Comparator*/
#define COMP_PRESENT 1
#define COMP_COUNT 1

/*Low-power comparator*/
#define LPCOMP_PRESENT 1
#define LPCOMP_COUNT 1

/*Watchdog Timer*/
#define WDT_PRESENT 1
#define WDT_COUNT 2

#define WDT30_ALLOW_STOP 1                           /*!< (unspecified)                                                        */
#define WDT30_HAS_INTEN 0                            /*!< (unspecified)                                                        */

#define WDT31_ALLOW_STOP 1                           /*!< (unspecified)                                                        */
#define WDT31_HAS_INTEN 0                            /*!< (unspecified)                                                        */

/*Clock management*/
#define CLOCK_PRESENT 1
#define CLOCK_COUNT 1

#define CLOCK_XOTUNE 1                               /*!< (unspecified)                                                        */
#define CLOCK_PLL24M 0                               /*!< (unspecified)                                                        */

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
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud1200Core64M = 77824, /*!< 1200 baud (actual rate: 1159, -3.4 percent error), 64 MHz core
                                                              frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud2400Core64M = 159744, /*!< 2400 baud (actual rate: 2380, -0.8 percent error), 64 MHz core
                                                               frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud4800Core64M = 319488, /*!< 4800 baud (actual rate: 4760, -0.8 percent error), 64 MHz core
                                                               frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud9600Core64M = 643072, /*!< 9600 baud (actual rate: 9583, -0.2 percent error), 64 MHz core
                                                               frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud14400Core64M = 962560, /*!< 14400 baud (actual rate: 14343, -0.4 percent error), 64 MHz core
                                                                frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud19200Core64M = 1286144, /*!< 19200 baud (actual rate: 19167, -0.2 percent error), 64 MHz
                                                                 core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud28800Core64M = 1929216, /*!< 28800 baud (actual rate: 28751, -0.2 percent error), 64 MHz
                                                                 core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud31250Core64M = 2097152, /*!< 31250 baud (actual rate: 31250, 0.0 percent error), 64 MHz core
                                                                 frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud38400Core64M = 2576384, /*!< 38400 baud (actual rate: 38392, -0.0 percent error), 64 MHz
                                                                 core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud56000Core64M = 3756032, /*!< 56000 baud (actual rate: 55993, -0.0 percent error), 64 MHz
                                                                 core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud57600Core64M = 3862528, /*!< 57600 baud (actual rate: 57605, 0.0 percent error), 64 MHz core
                                                                 frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud76800Core64M = 5152768, /*!< 76800 baud (actual rate: 76830, 0.0 percent error), 64 MHz core
                                                                 frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud115200Core64M = 7720960, /*!< 115200 baud (actual rate: 115107, -0.1 percent error), 64 MHz
                                                                  core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud230400Core64M = 15446016, /*!< 230400 baud (actual rate: 230215, -0.1 percent error), 64 MHz
                                                                   core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud250000Core64M = 16777216, /*!< 250000 baud (actual rate: 250000, 0.0 percent error), 64 MHz
                                                                   core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud460800Core64M = 30896128, /*!< 460800 baud (actual rate: 460431, -0.1 percent error), 64 MHz
                                                                   core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud921600Core64M = 62242816, /*!< 921600 baud (actual rate: 927536, 0.6 percent error), 64 MHz
                                                                   core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud1000000Core64M = 67108864, /*!< 1000000 baud (actual rate: 1000000, 0.0 percent error), 64
                                                                    MHz core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud1200Core128M = 36864, /*!< 1200 baud (actual rate: 1098, -8.4 percent error), 128 MHz core
                                                               frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud2400Core128M = 77824, /*!< 2400 baud (actual rate: 2319, -3.4 percent error), 128 MHz core
                                                               frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud4800Core128M = 159744, /*!< 4800 baud (actual rate: 4760, -0.8 percent error), 128 MHz core
                                                                frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud9600Core128M = 319488, /*!< 9600 baud (actual rate: 9521, -0.8 percent error), 128 MHz core
                                                                frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud14400Core128M = 479232, /*!< 14400 baud (actual rate: 14282, -0.8 percent error), 128 MHz
                                                                 core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud19200Core128M = 643072, /*!< 19200 baud (actual rate: 19167, -0.2 percent error), 128 MHz
                                                                 core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud28800Core128M = 962560, /*!< 28800 baud (actual rate: 28686, -0.4 percent error), 128 MHz
                                                                 core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud31250Core128M = 1048576, /*!< 31250 baud (actual rate: 31250, 0.0 percent error), 128 MHz
                                                                  core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud38400Core128M = 1286144, /*!< 38400 baud (actual rate: 38334, -0.2 percent error), 128 MHz
                                                                  core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud56000Core128M = 1875968, /*!< 56000 baud (actual rate: 55919, -0.1 percent error), 128 MHz
                                                                  core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud57600Core128M = 1929216, /*!< 57600 baud (actual rate: 57502, -0.2 percent error), 128 MHz
                                                                  core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud76800Core128M = 2576384, /*!< 76800 baud (actual rate: 76784, -0.0 percent error), 128 MHz
                                                                  core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud115200Core128M = 3862528, /*!< 115200 baud (actual rate: 115211, 0.0 percent error), 128 MHz
                                                                   core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud230400Core128M = 7720960, /*!< 230400 baud (actual rate: 230215, -0.1 percent error), 128
                                                                   MHz core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud250000Core128M = 8388608, /*!< 250000 baud (actual rate: 250000, 0.0 percent error), 128 MHz
                                                                   core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud460800Core128M = 15446016, /*!< 460800 baud (actual rate: 460431, -0.1 percent error), 128
                                                                    MHz core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud921600Core128M = 30896128, /*!< 921600 baud (actual rate: 920863, -0.1 percent error), 128
                                                                    MHz core frequency*/
  NRF_UARTE00_BAUDRATE_BAUDRATE_Baud1000000Core128M = 33554432, /*!< 1000000 baud (actual rate: 1000000, 0.0 percent error), 128
                                                                     MHz core frequency*/
} NRF_UARTE00_BAUDRATE_BAUDRATE_ENUM_t;

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

/* ==================================================== Baudrate settings ==================================================== */
/**
  * @brief UARTE.BAUDRATE register values for combinations of baudrate and core frequency
  */
typedef enum {
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud1200Core16M = 319488, /*!< 1200 baud (actual rate: 1190, -0.8 percent error), 16 MHz core
                                                               frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud2400Core16M = 643072, /*!< 2400 baud (actual rate: 2395, -0.2 percent error), 16 MHz core
                                                               frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud4800Core16M = 1286144, /*!< 4800 baud (actual rate: 4791, -0.2 percent error), 16 MHz core
                                                                frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud9600Core16M = 2576384, /*!< 9600 baud (actual rate: 9598, -0.0 percent error), 16 MHz core
                                                                frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud14400Core16M = 3862528, /*!< 14400 baud (actual rate: 14401, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud19200Core16M = 5152768, /*!< 19200 baud (actual rate: 19207, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud28800Core16M = 7720960, /*!< 28800 baud (actual rate: 28776, -0.1 percent error), 16 MHz
                                                                 core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud31250Core16M = 8388608, /*!< 31250 baud (actual rate: 31250, 0.0 percent error), 16 MHz core
                                                                 frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud38400Core16M = 10297344, /*!< 38400 baud (actual rate: 38369, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud56000Core16M = 15015936, /*!< 56000 baud (actual rate: 55944, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud57600Core16M = 15446016, /*!< 57600 baud (actual rate: 57553, -0.1 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud76800Core16M = 20647936, /*!< 76800 baud (actual rate: 76923, 0.2 percent error), 16 MHz
                                                                  core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud115200Core16M = 30896128, /*!< 115200 baud (actual rate: 115107, -0.1 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud230400Core16M = 62242816, /*!< 230400 baud (actual rate: 231884, 0.6 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud250000Core16M = 67108864, /*!< 250000 baud (actual rate: 250000, 0.0 percent error), 16 MHz
                                                                   core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud460800Core16M = 122712064, /*!< 460800 baud (actual rate: 457142, -0.8 percent error), 16
                                                                    MHz core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud921600Core16M = 252641280, /*!< 921600 baud (actual rate: 941176, 2.1 percent error), 16 MHz
                                                                    core frequency*/
  NRF_UARTE30_BAUDRATE_BAUDRATE_Baud1000000Core16M = 268435456, /*!< 1000000 baud (actual rate: 1000000, 0.0 percent error), 16
                                                                     MHz core frequency*/
} NRF_UARTE30_BAUDRATE_BAUDRATE_ENUM_t;


#ifdef __cplusplus
}
#endif
#endif /* NRF54L10_FLPR_PERIPHERALS_H */

