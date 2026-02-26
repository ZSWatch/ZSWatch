/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __HAL_BSP_H_
#define __HAL_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

/*
 * Initializes BSP; registers flash_map with the system.
 */
void hal_bsp_init(void);

/*
 * Return pointer to flash device structure, given BSP specific
 * flash id.
 */
struct hal_flash;
const struct hal_flash *hal_bsp_flash_dev(uint8_t flash_id);

/*
 * Grows heap by given amount. XXX giving space back not implemented.
 */
void *_sbrk(int incr);

/*
 * Report which memory areas should be included inside a coredump.
 */
struct hal_bsp_mem_dump {
    void *hbmd_start;
    uint32_t hbmd_size;
};

const struct hal_bsp_mem_dump *hal_bsp_core_dump(int *area_cnt);

/*
 * Get unique HW identifier/serial number for platform.
 * Returns the number of bytes filled in.
 */
#define HAL_BSP_MAX_ID_LEN  32
int hal_bsp_hw_id(uint8_t *id, int max_len);

#define HAL_BSP_POWER_ON (1)
#define HAL_BSP_POWER_WFI (2)
#define HAL_BSP_POWER_SLEEP (3)
#define HAL_BSP_POWER_DEEP_SLEEP (4)
#define HAL_BSP_POWER_OFF (5)
#define HAL_BSP_POWER_PERUSER (128)

int hal_bsp_power_state(int state);

/* Returns priority of given interrupt number */
uint32_t hal_bsp_get_nvic_priority(int irq_num, uint32_t pri);

#ifdef __cplusplus
}
#endif

#endif
