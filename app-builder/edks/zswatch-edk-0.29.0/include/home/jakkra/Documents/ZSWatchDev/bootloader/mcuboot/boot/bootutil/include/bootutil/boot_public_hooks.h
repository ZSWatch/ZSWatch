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
 * implemented for overide some of MCUboot's native routines.
 */

#ifndef H_BOOTUTIL_PUBLIC_HOOKS
#define H_BOOTUTIL_PUBLIC_HOOKS

#include "bootutil/boot_hooks.h"

/** Hook for provide primary image swap state.
 *
 * @param img_index the index of the image pair
 * @param state image swap state structure to be populated
 *
 * @retval 0: header was read/populated
 *         FIH_FAILURE: image is invalid,
 *         BOOT_HOOK_REGULAR if hook not implemented for the image-slot,
 *         othervise an error-code value.
 */
int boot_read_swap_state_primary_slot_hook(int image_index,
                                           struct boot_swap_state *state);

#endif /*H_BOOTUTIL_PUBLIC_HOOKS*/
