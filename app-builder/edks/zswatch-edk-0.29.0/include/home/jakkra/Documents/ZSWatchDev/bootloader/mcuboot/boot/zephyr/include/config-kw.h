/*
 *  Minimal configuration for using TLS in the bootloader
 *
 *  Copyright (C) 2006-2021, ARM Limited, All Rights Reserved
 *  Copyright (C) 2016, Linaro Ltd
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/*
 * Minimal configuration for using TLS in the bootloader
 *
 * - RSA or ECDSA signature verification
 */

#ifndef MCUBOOT_MBEDTLS_CONFIG_KW
#define MCUBOOT_MBEDTLS_CONFIG_KW

#ifdef CONFIG_MCUBOOT_SERIAL
/* Mcuboot uses mbedts-base64 for serial protocol encoding. */
#define MBEDTLS_BASE64_C
#endif

/* System support */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES

/* STD functions */
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

#define MBEDTLS_PLATFORM_EXIT_ALT
#define MBEDTLS_PLATFORM_PRINTF_ALT
#define MBEDTLS_PLATFORM_SNPRINTF_ALT

#define MBEDTLS_ASN1_PARSE_C

#if !defined(CONFIG_ARM)
#define MBEDTLS_HAVE_ASM
#endif

#define MBEDTLS_CIPHER_MODE_CTR

#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA256_SMALLER
#define MBEDTLS_SHA224_C
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_NIST_KW_C

#endif /* MCUBOOT_MBEDTLS_CONFIG_KW */
