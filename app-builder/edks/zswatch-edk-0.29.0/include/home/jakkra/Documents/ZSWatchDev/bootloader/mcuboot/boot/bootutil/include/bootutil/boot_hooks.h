/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2021 Nordic Semiconductor ASA
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

/**
 * @file
 * @brief Hooks definition implementation API
 *
 * This file contains API interface definition for hooks which can be
 * implemented to overide or to amend some of MCUboot's native routines.
 */

#ifndef H_BOOTUTIL_HOOKS
#define H_BOOTUTIL_HOOKS

#include "bootutil/bootutil.h"
#include "bootutil/fault_injection_hardening.h"

#define DO_HOOK_CALL(f, ret_default, ...) \
    f(__VA_ARGS__)

#define DO_HOOK_CALL_FIH(f, fih_ret_default, fih_rc, ...) \
    do { \
        FIH_CALL(f, fih_rc, __VA_ARGS__); \
    } while(0);

#define HOOK_CALL_NOP(f, ret_default, ...) ret_default

#define HOOK_CALL_FIH_NOP(f, fih_ret_default, fih_rc, ...) \
    do { \
        fih_rc = fih_ret_default; \
    } while(0);

#ifdef MCUBOOT_IMAGE_ACCESS_HOOKS

#define BOOT_HOOK_CALL(f, ret_default, ...) \
    DO_HOOK_CALL(f, ret_default, __VA_ARGS__)

#define BOOT_HOOK_CALL_FIH(f, fih_ret_default, fih_rc, ...) \
    DO_HOOK_CALL_FIH(f, fih_ret_default, fih_rc, __VA_ARGS__)

#else

#define BOOT_HOOK_CALL(f, ret_default, ...) \
    HOOK_CALL_NOP(f, ret_default, __VA_ARGS__)

#define BOOT_HOOK_CALL_FIH(f, fih_ret_default, fih_rc, ...) \
    HOOK_CALL_FIH_NOP(f, fih_ret_default, fih_rc, __VA_ARGS__)

#endif /* MCUBOOT_IMAGE_ACCESS_HOOKS */

#ifdef MCUBOOT_BOOT_GO_HOOKS

#define BOOT_HOOK_GO_CALL_FIH(f, fih_ret_default, fih_rc, ...) \
    DO_HOOK_CALL_FIH(f, fih_ret_default, fih_rc, __VA_ARGS__);

#else

#define BOOT_HOOK_GO_CALL_FIH(f, fih_ret_default, fih_rc, ...) \
    HOOK_CALL_FIH_NOP(f, fih_ret_default, fih_rc, __VA_ARGS__)

#endif /* MCUBOOT_BOOT_GO_HOOKS  */

#ifdef MCUBOOT_FIND_NEXT_SLOT_HOOKS

#define BOOT_HOOK_FIND_SLOT_CALL(f, ret_default, ...) \
    DO_HOOK_CALL(f, ret_default, __VA_ARGS__)

#else

#define BOOT_HOOK_FIND_SLOT_CALL(f, ret_default, ...) \
    HOOK_CALL_NOP(f, ret_default, __VA_ARGS__)

#endif /* MCUBOOT_FIND_NEXT_SLOT_HOOKS */

#ifdef MCUBOOT_FLASH_AREA_HOOKS

#define BOOT_HOOK_FLASH_AREA_CALL(f, ret_default, ...) \
    DO_HOOK_CALL(f, ret_default, __VA_ARGS__)

#else

#define BOOT_HOOK_FLASH_AREA_CALL(f, ret_default, ...) \
    HOOK_CALL_NOP(f, ret_default, __VA_ARGS__)

#endif /* MCUBOOT_FLASH_AREA_ID_HOOKS */

/** Hook for provide image header data.
 *
 * This Hook may be used to overide image header read implementation or doing
 * a custom action before.
 *
 * @param img_index the index of the image pair
 * @param slot slot number
 * @param img_head image header structure to be populated
 *
 * @retval 0: header was read/populated, skip direct header data read
 *         BOOT_HOOK_REGULAR: follow the normal execution path,
 *         otherwise an error-code value.
 */
int boot_read_image_header_hook(int img_index, int slot,
                                struct image_header *img_head);

/** Hook for Validate image hash/signature
 *
 * This Hook may be used to overide image validation procedure or doing
 * a custom action before.
 *
 * @param img_index the index of the image pair
 * @param slot slot number
 * 
 * @retval FIH_SUCCESS: image is valid, skip direct validation
 *         FIH_FAILURE: image is invalid, skip direct validation
 *         FIH_BOOT_HOOK_REGULAR: follow the normal execution path.
 */
fih_ret boot_image_check_hook(int img_index, int slot);

/** Hook for implement image update
 *
 * This hook is for for implementing an alternative mechanism of image update or
 * doing a custom action before.
 *
 * @param img_index the index of the image pair
 * @param img_head the image header of the secondary image
 * @param area the flash area of the secondary image.
 *
 * @retval 0: update was done, skip performing the update
 *         BOOT_HOOK_REGULAR: follow the normal execution path,
 *         otherwise an error-code value.
 */
int boot_perform_update_hook(int img_index, struct image_header *img_head,
                             const struct flash_area *area);

/** Hook for implement image's post copying action
 *
 * This hook is for implement action which might be done right after image was
 * copied to the primary slot. This hook is called in MCUBOOT_OVERWRITE_ONLY
 * mode only.
 *
 * @param img_index the index of the image pair
 * @param area the flash area of the primary image.
 * @param size size of copied image.
 *
 * @retval 0: success, mcuboot will follow normal code execution flow after
 *            execution of this call.
 *         non-zero: an error, mcuboot will return from
 *         boot_copy_image() with error.
 *         Update will be undone so might be resume on the next boot.
 */
int boot_copy_region_post_hook(int img_index, const struct flash_area *area,
                               size_t size);

/** Hook for implement image's post recovery upload action
 *
 * This hook is for implement action which might be done right after image was
 * copied to the primary slot. This hook is called in serial recovery upload
 * operation.
 *
 * @param img_index the index of the image pair
 * @param area the flash area of the primary image.
 * @param size size of copied image.
 *
 * @retval 0: success, mcuboot will follow normal code execution flow after
 *            execution of this call.
 *         non-zero: an error, will be transferred as part of comand response
 *            as "rc" entry.
 */
int boot_serial_uploaded_hook(int img_index, const struct flash_area *area,
                              size_t size);

/** Hook for implement the image's slot installation status fetch operation for
 *  the MGMT custom command.
 *
 * The image's slot installation status is custom property. It's detailed
 * definition depends on user implementation. It is only defined that the status
 * will be set to 0 if this hook not provides another value.
 *
 * @param img_index the index of the image pair
 * @param slot slot number
 * @param img_install_stat the image installation status to be populated
 *
 * @retval 0: the installaton status was fetched successfully,
 *         BOOT_HOOK_REGULAR: follow the normal execution path, status will be
 *         set to 0
 *         otherwise an error-code value. Error-code is ignored, but it is up to
 *         the implementation to reflect this error in img_install_stat.
 */
int boot_img_install_stat_hook(int image_index, int slot,
                               int *img_install_stat);

/** Hook will be invoked when boot_serial requests device reset.
 *  The hook may be used to prevent device reset.
 *
 * @param force set to true when request tries to force reset.
 *
 * @retval 0 when reset should be performed;
 *         BOOT_RESET_REQUEST_HOOK_BUSY when some processing is still in
 *         progress;
 *         BOOT_RESET_REQUEST_HOOK_TIMEOUT internal process timed out;
 *         BOOT_RESET_REQUEST_HOOK_CHECK_FAILED internal code failed to
 *         obtian status;
 *         BOOT_RESET_REQUEST_HOOK_INTERNAL_ERROR unspecified internal
 *         error while checking status.
 */
int boot_reset_request_hook(bool force);

/**
 * Hook to implement custom action before boot_go() function.
 *
 * @param rsp boot response structure.
 *
 * @retval FIH_SUCCESS: boot_go() should be skipped, boot response is already
 *         filled.
 *         FIH_FAILURE: boot_go() should be skipped, boot response is already
 *         filled with error.
 *         FIH_BOOT_HOOK_REGULAR: follow the normal execution path.
 */
fih_ret boot_go_hook(struct boot_rsp *rsp);

/**
 * Hook to implement custom action before retrieving flash area ID.
 *
 * @param image_index the index of the image pair
 * @param slot slot number
 * @param area_id the flash area ID to be populated
 *
 * @retval 0 the flash area ID was fetched successfully;
 *         BOOT_HOOK_REGULAR follow the normal execution path to get the flash
 *         area ID;
 *         otherwise an error-code value.
 */
int flash_area_id_from_multi_image_slot_hook(int image_index, int slot,
                                             int *area_id);

/**
 * Hook to implement custom action before retrieving flash area device ID.
 *
 * @param fa the flash area structure
 * @param device_id the device ID to be populated
 *
 * @retval 0 the device ID was fetched successfully;
 *         BOOT_HOOK_REGULAR follow the normal execution path to get the device
 *         ID;
 *         otherwise an error-code value.
 */
int flash_area_get_device_id_hook(const struct flash_area *fa,
                                  uint8_t *device_id);

#define BOOT_RESET_REQUEST_HOOK_BUSY		1
#define BOOT_RESET_REQUEST_HOOK_TIMEOUT		2
#define BOOT_RESET_REQUEST_HOOK_CHECK_FAILED	3
#define BOOT_RESET_REQUEST_HOOK_INTERNAL_ERROR	4

/**
 * Finds the preferred slot containing the image.
 *
 * @param[in]   state        Boot loader status information.
 * @param[in]   image        Image, for which the slot should be found.
 * @param[out]  active_slot  Number of the preferred slot.
 *
 * @return 0 if a slot was requested;
 *         BOOT_HOOK_REGULAR follow the normal execution path.
 */
int boot_find_next_slot_hook(struct boot_loader_state *state, uint8_t image, enum boot_slot *active_slot);

#endif /*H_BOOTUTIL_HOOKS*/
