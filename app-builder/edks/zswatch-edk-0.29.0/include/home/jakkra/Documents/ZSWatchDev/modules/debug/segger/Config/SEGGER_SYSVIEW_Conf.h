/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2024 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* SEGGER strongly recommends to not make any changes                 *
* to or modify the source code of this software in order to stay     *
* compatible with the SystemView and RTT protocol, and J-Link.       *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: 3.58                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SYSVIEW_Conf.h
Purpose : SEGGER SystemView configuration file.
          Set defines which deviate from the defaults (see SEGGER_SYSVIEW_ConfDefaults.h) here.
Revision: $Rev: 21292 $

Additional information:
  Required defines which must be set are:
    SEGGER_SYSVIEW_GET_TIMESTAMP
    SEGGER_SYSVIEW_GET_INTERRUPT_ID
  For known compilers and cores, these might be set to good defaults
  in SEGGER_SYSVIEW_ConfDefaults.h.

  SystemView needs a (nestable) locking mechanism.
  If not defined, the RTT locking mechanism is used,
  which then needs to be properly configured.
*/

#ifndef SEGGER_SYSVIEW_CONF_H
#define SEGGER_SYSVIEW_CONF_H

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

#include <stdint.h>

#define SEGGER_SYSVIEW_GET_TIMESTAMP	sysview_get_timestamp
#define SEGGER_SYSVIEW_GET_INTERRUPT_ID sysview_get_interrupt

uint32_t sysview_get_timestamp(void);
uint32_t sysview_get_interrupt(void);

#define SEGGER_SYSVIEW_RTT_BUFFER_SIZE	CONFIG_SEGGER_SYSVIEW_RTT_BUFFER_SIZE
#define SEGGER_SYSVIEW_POST_MORTEM_MODE CONFIG_SEGGER_SYSVIEW_POST_MORTEM_MODE
#define SEGGER_SYSVIEW_RTT_CHANNEL	CONFIG_SEGGER_SYSVIEW_RTT_CHANNEL

#if defined(CONFIG_SEGGER_SYSVIEW_SECTION_DTCM)
#define SEGGER_SYSVIEW_SECTION ".dtcm_bss"
#endif

extern unsigned int zephyr_rtt_irq_lock(void);
extern void zephyr_rtt_irq_unlock(unsigned int key);

/* Lock SystemView (nestable) */
#define SEGGER_SYSVIEW_LOCK()                                                                      \
	{                                                                                          \
		unsigned int __sysview_irq_key = zephyr_rtt_irq_lock()

/* Unlock SystemView (nestable) */
#define SEGGER_SYSVIEW_UNLOCK()                                                                    \
		zephyr_rtt_irq_unlock(__sysview_irq_key);                                          \
	}

#endif  // SEGGER_SYSVIEW_CONF_H

/*************************** End of file ****************************/
