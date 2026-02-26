/*
 *  Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RAMLOAD_H__
#define __RAMLOAD_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MULTIPLE_EXECUTABLE_RAM_REGIONS
/**
 * Provides information about the Executable RAM for a given image ID.
 *
 * @param image_id        Index of the image (from 0).
 * @param exec_ram_start  Pointer to store the start address of the exec RAM
 * @param exec_ram_size   Pointer to store the size of the exec RAM
 *
 * @return                0 on success; nonzero on failure.
 */
int boot_get_image_exec_ram_info(uint32_t image_id,
                                 uint32_t *exec_ram_start,
                                 uint32_t *exec_ram_size);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RAMLOAD_H__ */