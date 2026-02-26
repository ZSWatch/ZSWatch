/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2017-2019 Linaro LTD
 * Copyright (c) 2016-2019 JUUL Labs
 * Copyright (c) 2019-2021 Arm Limited
 *
 * Original license:
 *
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

#ifndef H_BOOTUTIL_
#define H_BOOTUTIL_

#include <inttypes.h>
#include "bootutil/fault_injection_hardening.h"
#include "bootutil/bootutil_public.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MCUBOOT_IMAGE_NUMBER
#define BOOT_IMAGE_NUMBER          MCUBOOT_IMAGE_NUMBER
#else
#define BOOT_IMAGE_NUMBER          1
#endif

_Static_assert(BOOT_IMAGE_NUMBER > 0, "Invalid value for BOOT_IMAGE_NUMBER");

struct image_header;
/**
 * A response object provided by the boot loader code; indicates where to jump
 * to execute the main image.
 */
struct boot_rsp {
    /** A pointer to the header of the image to be executed. */
    const struct image_header *br_hdr;

    /**
     * The flash offset of the image to execute.  Indicates the position of
     * the image header within its flash device.
     */
    uint8_t br_flash_dev_id;
    uint32_t br_image_off;
};

/* This is not actually used by mcuboot's code but can be used by apps
 * when attempting to read/write a trailer.
 */
struct image_trailer {
    uint8_t swap_type;
    uint8_t pad1[BOOT_MAX_ALIGN - 1];
    uint8_t copy_done;
    uint8_t pad2[BOOT_MAX_ALIGN - 1];
    uint8_t image_ok;
    uint8_t pad3[BOOT_MAX_ALIGN - 1];
#if BOOT_MAX_ALIGN > BOOT_MAGIC_SZ
    uint8_t pad4[BOOT_MAGIC_ALIGN_SIZE - BOOT_MAGIC_SZ];
#endif
    uint8_t magic[BOOT_MAGIC_SZ];
};

struct image_max_size {
	bool calculated;
	uint32_t max_size;
};

/* you must have pre-allocated all the entries within this structure */
fih_ret boot_go(struct boot_rsp *rsp);
fih_ret boot_go_for_image_id(struct boot_rsp *rsp, uint32_t image_id);

void boot_state_clear(struct boot_loader_state *state);
fih_ret context_boot_go(struct boot_loader_state *state, struct boot_rsp *rsp);

/**
 * Returns a pointer to the boot loader state structure.
 *
 * @return Pointer to the boot loader state structure.
 */
struct boot_loader_state *boot_get_loader_state(void);

#if defined(MCUBOOT_SERIAL_IMG_GRP_SLOT_INFO) || defined(MCUBOOT_DATA_SHARING)
/**
 * Returns pointer to array of image maximum sizes.
 *
 * @note This function provides a RAW access to the structure. The sizes may not be
 *       calculated yet. Use boot_get_max_app_size() to ensure the sizes are calculated.
 *
 * @return Pointer to array of image maximum sizes.
 */
struct image_max_size *boot_get_image_max_sizes(void);

/**
 * Fetches the maximum allowed size of all application images.
 *
 * @note In contrast to boot_get_image_max_sizes(), this function will fetch the sizes
 *       if they are not yet calculated.
 *
 * @return A pointer to the structure containing the maximum sizes of images.
 */
const struct image_max_size *boot_get_max_app_size(void);
#endif /* MCUBOOT_SERIAL_IMG_GRP_SLOT_INFO || MCUBOOT_DATA_SHARING */

void boot_fetch_slot_state_sizes(void);
uint32_t boot_get_state_secondary_offset(struct boot_loader_state *state,
                                         const struct flash_area *fap);

#define SPLIT_GO_OK                 (0)
#define SPLIT_GO_NON_MATCHING       (-1)
#define SPLIT_GO_ERR                (-2)

fih_ret split_go(int loader_slot, int split_slot, void **entry);

#ifdef __cplusplus
}
#endif

#endif
