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

#ifndef HALTIUM_INTERIM_H__
#define HALTIUM_INTERIM_H__

#if defined(HALTIUM_XXAA)


    #define NRF_DOMAIN_COUNT    NRF_DOMAIN_GLOBAL + 1
    #define NRF_PROCESSOR_COUNT NRF_PROCESSOR_FLPR + 1


    #define ADDRESS_REGION_Pos        (29UL)
    #define ADDRESS_REGION_Msk        (0x7UL << ADDRESS_REGION_Pos)
    #define ADDRESS_SECURITY_Pos      (28UL)
    #define ADDRESS_SECURITY_Msk      (0x1UL << ADDRESS_SECURITY_Pos)
    #define ADDRESS_DOMAIN_Pos        (24UL)
    #define ADDRESS_DOMAIN_Msk        (0xFUL << ADDRESS_DOMAIN_Pos)
    #define ADDRESS_BUS_Pos           (16UL)
    #define ADDRESS_BUS_Msk           (0xFFUL << ADDRESS_BUS_Pos)
    #define ADDRESS_SLAVE_Pos         (12UL)
    #define ADDRESS_SLAVE_Msk         (0xFUL << ADDRESS_SLAVE_Pos)
    #define ADDRESS_PERIPHID_Pos      (12UL)
    #define ADDRESS_PERIPHID_Msk      (0x7FFUL << ADDRESS_PERIPHID_Pos)

    typedef enum
    {
        NRF_REGION_PROGRAM      = 0,
        NRF_REGION_DATA         = 1,
        NRF_REGION_PERIPHERALS  = 2,
        NRF_REGION_EXTMEM       = 3,
        NRF_REGION_EXTMEM_ENC   = 4,
        NRF_REGION_STM          = 5,
        NRF_REGION_CPU_INTERNAL = 7,
    } nrf_region_t;

    #define GPIOTE_INT_COUNT 7

    #define GPIOTE_CH_NUM   8
    #define GPIOTE130_CH_NUM (GPIOTE130_GPIOTE_NCHANNELS_MAX + 1UL)
    #define GPIOTE130_AVAILABLE_GPIO_PORTS 0x207UL
    #define GPIOTE_PORT_NUM GPIOTE_EVENTS_PORT_MaxCount
    #define GPIOTE_FEATURE_SET_PRESENT
    #define GPIOTE_FEATURE_CLR_PRESENT

    #define VPR_CLIC_PRIO_COUNT 4
    #define VPR_VEVIF_EVENT_MaxCount 32

    #define GLOBAL_IRQN_START (96)
    #define GLOBAL_IRQN_MAX   (480)

    #define SPU000_PERIPH_COUNT 16
    #define SPU010_PERIPH_COUNT 16
    #define SPU020_PERIPH_COUNT 16
    #define SPU030_PERIPH_COUNT 16

    #define SAADC_CH_NUM 8
    #define SAADC_EASYDMA_MAXCNT_SIZE 15

    #define LPCOMP_REFSEL_RESOLUTION 16

    #if !defined(DPPI_PRESENT)
        #define DPPI_PRESENT
    #endif
    #define DPPI_GROUP_NUM 2
    #if defined(NRF_RADIOCORE)
        #define DPPI020_CH_NUM 16
        #define DPPI020_GROUP_NUM 0
        #define DPPI030_CH_NUM 9
        #define DPPI030_GROUP_NUM 1
    #endif

    #define GLOBAL_IPCT_CH_NUM 8
    #if defined(NRF_RADIOCORE)
        #define LOCAL_IPCT_NUM 8
    #elif defined(NRF_APPLICATION)
        #define LOCAL_IPCT_NUM 4
    #endif

    #if defined(NRF_PPR) || defined(NRF_FLPR)
        #define MVDMA_JOBLISTCOUNT 4
    #else
        #define MVDMA_JOBLISTCOUNT 1
    #endif

    #define MVDMA_AXI_BUS_WIDTH 8


    #define MPC_MASTER_PORTS_MaxCount (32UL)
#endif
#endif // HALTIUM_INTERIM_H__
