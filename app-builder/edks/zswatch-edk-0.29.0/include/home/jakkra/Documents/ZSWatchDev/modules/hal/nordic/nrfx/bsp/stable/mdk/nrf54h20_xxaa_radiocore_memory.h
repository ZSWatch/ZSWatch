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

#ifndef NRF_DEVICE_MEM_H_
#define NRF_DEVICE_MEM_H_

#ifndef __DEFAULT_STACK_SIZE
    #define __DEFAULT_STACK_SIZE 4096
#endif
#ifndef __DEFAULT_HEAP_SIZE
    #define __DEFAULT_HEAP_SIZE 4096
#endif

/* Device memory Flash: */
#define NRF_MEMORY_FLASH_BASE 0x0E120000
#define NRF_MEMORY_FLASH_SIZE 0x00080000

/* Device memory UICR: */
#define NRF_MEMORY_UICR_BASE 0x0FFFA000
#define NRF_MEMORY_UICR_SIZE 0x00000800

/* Device memory RAM0: */
#define NRF_MEMORY_RAM0_BASE 0x23000000
#define NRF_MEMORY_RAM0_SIZE 0x00010000

/* Device memory RAM1: */
#define NRF_MEMORY_RAM1_BASE 0x23010000
#define NRF_MEMORY_RAM1_SIZE 0x00008000

/* Device memory PeripheralsAPBS: */
#define NRF_MEMORY_PERIPHERALSAPBS_BASE 0x53000000
#define NRF_MEMORY_PERIPHERALSAPBS_SIZE 0x00200000

/* Device memory PeripheralsAPBNS: */
#define NRF_MEMORY_PERIPHERALSAPBNS_BASE 0x43000000
#define NRF_MEMORY_PERIPHERALSAPBNS_SIZE 0x00200000

/* Device memory PeripheralsAHB: */
#define NRF_MEMORY_PERIPHERALSAHB_BASE 0x53840000
#define NRF_MEMORY_PERIPHERALSAHB_SIZE 0x00003000

/* Device memory SystemSFR: */
#define NRF_MEMORY_SYSTEMSFR_BASE 0xE0000000
#define NRF_MEMORY_SYSTEMSFR_SIZE 0x00100000



#endif
