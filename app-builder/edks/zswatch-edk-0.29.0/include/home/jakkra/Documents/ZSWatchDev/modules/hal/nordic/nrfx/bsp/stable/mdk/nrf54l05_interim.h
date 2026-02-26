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

#ifndef NRF54L05_INTERIM_H__
#define NRF54L05_INTERIM_H__

#if defined(NRF54L05_XXAA)


    #define NRF_DOMAIN_COUNT NRF_DOMAIN_NONE + 1

    #define ADDRESS_BUS_Pos (18UL)
    #define ADDRESS_BUS_Msk (0x3FUL << ADDRESS_BUS_Pos)


    #define PPIB00_CH_NUM 8
    #define PPIB10_CH_NUM 8
    #define PPIB11_CH_NUM 16
    #define PPIB21_CH_NUM 16
    #define PPIB22_CH_NUM 4
    #define PPIB30_CH_NUM 4
    #define PPIB20_CH_NUM 8
    #define PPIB01_CH_NUM 8

    typedef enum
    {
        NRF_APB_INDEX_MCU   = 1,
        NRF_APB_INDEX_RADIO = 2,
        NRF_APB_INDEX_PERI  = 3,
        NRF_APB_INDEX_LP    = 4
    } nrf_apb_index_t;

    #if defined(NRF_FLPR)
        #define GRTC_IRQ_GROUP   0
        #define GPIOTE_IRQ_GROUP 0
    #elif defined(NRF_APPLICATION)
        #if defined(NRF_TRUSTZONE_NONSECURE)
            #define GPIOTE_IRQ_GROUP       0
            #define GRTC_IRQ_GROUP         1 
        #else
            #define GPIOTE_IRQ_GROUP       1
            #define GRTC_IRQ_GROUP         2
        #endif
    #endif

    #define EASYVDMA_PRESENT

    #define SAADC_CH_NUM SAADC_CH_MaxCount
    #define SAADC_EASYDMA_MAXCNT_SIZE 15

    #define LPCOMP_REFSEL_RESOLUTION 16

    #define MPC_MASTER_PORTS_MaxCount (15UL) /*!< Max number of master ports. */

    #define GPIOTE20_CH_NUM GPIOTE20_GPIOTE_NCHANNELS_SIZE
    #define GPIOTE30_CH_NUM GPIOTE30_GPIOTE_NCHANNELS_SIZE

    #define GPIOTE_CH_NUM   8
    #define GPIOTE20_AVAILABLE_GPIO_PORTS 0x2UL
    #define GPIOTE30_AVAILABLE_GPIO_PORTS 0x1UL
    #define GPIOTE_FEATURE_SET_PRESENT
    #define GPIOTE_FEATURE_CLR_PRESENT
    #define GPIOTE_PORT_NUM GPIOTE_EVENTS_PORT_MaxCount

    #define DPPI_PRESENT DPPIC_PRESENT

    #define DPPIC00_CH_NUM DPPIC00_CH_NUM_SIZE
    #define DPPIC10_CH_NUM DPPIC10_CH_NUM_SIZE
    #define DPPIC20_CH_NUM DPPIC20_CH_NUM_SIZE
    #define DPPIC30_CH_NUM DPPIC30_CH_NUM_SIZE
    
    #define DPPIC00_GROUP_NUM DPPIC00_GROUP_NUM_SIZE
    #define DPPIC10_GROUP_NUM DPPIC10_GROUP_NUM_SIZE
    #define DPPIC20_GROUP_NUM DPPIC20_GROUP_NUM_SIZE
    #define DPPIC30_GROUP_NUM DPPIC30_GROUP_NUM_SIZE

    #define PPIB_CHANNEL_MAX_COUNT 24UL

    #define P0_PIN_NUM P0_PIN_NUM_SIZE
    #define P1_PIN_NUM P1_PIN_NUM_SIZE
    #define P2_PIN_NUM P2_PIN_NUM_SIZE


    #define TIMER00_CC_NUM TIMER00_CC_NUM_SIZE
    #define TIMER10_CC_NUM TIMER10_CC_NUM_SIZE
    #define TIMER20_CC_NUM TIMER20_CC_NUM_SIZE
    #define TIMER21_CC_NUM TIMER21_CC_NUM_SIZE
    #define TIMER22_CC_NUM TIMER22_CC_NUM_SIZE
    #define TIMER23_CC_NUM TIMER23_CC_NUM_SIZE
    #define TIMER24_CC_NUM TIMER24_CC_NUM_SIZE

    #define TIMER00_MAX_SIZE TIMER00_MAX_SIZE_SIZE
    #define TIMER10_MAX_SIZE TIMER10_MAX_SIZE_SIZE
    #define TIMER20_MAX_SIZE TIMER20_MAX_SIZE_SIZE
    #define TIMER21_MAX_SIZE TIMER21_MAX_SIZE_SIZE
    #define TIMER22_MAX_SIZE TIMER22_MAX_SIZE_SIZE
    #define TIMER23_MAX_SIZE TIMER23_MAX_SIZE_SIZE
    #define TIMER24_MAX_SIZE TIMER24_MAX_SIZE_SIZE

    #define EGU10_CH_NUM EGU10_CH_NUM_SIZE
    #define EGU20_CH_NUM EGU20_CH_NUM_SIZE


    #define RTC10_CC_NUM RTC10_CC_NUM_SIZE
    #define RTC30_CC_NUM RTC30_CC_NUM_SIZE

    #define I2S20_EASYDMA_MAXCNT_SIZE I2S20_EASYDMA_MAXCNT_SIZE_SIZE

    #define VPR_VEVIF_EVENT_MaxCount  32
    #define VPR_CLIC_PRIO_COUNT       4
    #define ADDRESS_SLAVE_Pos         (12UL)
    #define ADDRESS_SLAVE_Msk         (0x3FUL << ADDRESS_SLAVE_Pos)

    #define TWIM_FREQUENCY_FREQUENCY_K1000 (0x0FF00000UL)


#endif

#endif