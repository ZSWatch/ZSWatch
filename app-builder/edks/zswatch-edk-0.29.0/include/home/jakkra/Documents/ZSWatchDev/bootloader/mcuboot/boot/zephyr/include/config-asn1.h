/*
 *  Configuration of mbedTLS containing only the ASN.1 parser.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
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

#ifndef MBEDTLS_CONFIG_ASN1_H
#define MBEDTLS_CONFIG_ASN1_H

#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

/* mbed TLS modules */
#define MBEDTLS_ASN1_PARSE_C
// #define MBEDTLS_ASN1_WRITE_C
// #define MBEDTLS_BIGNUM_C
// #define MBEDTLS_MD_C
// #define MBEDTLS_OID_C
// #define MBEDTLS_SHA256_C

#endif /* MBEDTLS_CONFIG_ASN1_H */
