/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 */

#ifndef H_BOOTUTIL_MACROS
#define H_BOOTUTIL_MACROS

#ifndef ALIGN_UP
#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(num, align)  ((num) & ~((align) - 1))
#endif

#endif
