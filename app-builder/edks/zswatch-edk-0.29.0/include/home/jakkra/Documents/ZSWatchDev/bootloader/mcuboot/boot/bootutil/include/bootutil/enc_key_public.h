/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2018-2019 JUUL Labs
 * Copyright (c) 2019-2021 Arm Limited
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

#ifndef BOOTUTIL_ENC_KEY_PUBLIC_H
#define BOOTUTIL_ENC_KEY_PUBLIC_H
#include <mcuboot_config/mcuboot_config.h>
#include <bootutil/bootutil_macros.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The unit provides following system wide definitions:
 *   BOOT_ENC_TLV_SIZE  -- is the complete size of TLV with encryption data.
 *   BOOT_ENC_TLV       -- is the encryption TLV type, should be given value
 *                         of one of IMAGE_TVL_ENC_ identifiers.
 *   BOOT_ENC_KEY_SIZE  -- is the encryption key size; this includes portion
 *                         of TLV data stream taken by key.
 *
 * For ECIES based key exchange there is additionally provided:
 *   EC_PUBK_LEN        -- is the length, in bytes, of a public key; depends
 *                         selected key exchange.
 *   EC_PRIVK_LEN       -- is the length, in bytes, of a private key; depends
 *                         on selected key exchange.
 *   EC_SHARED_LEN      -- is the length, in bytes, of a shared key resulting
 *                         from processing of private and public key; depends
 *                         on selected key exchange parameters.
 *
 * ECIES TLV processing uses following TLVs, from this header:
 *   EC_TAG_INDEX       -- is the HMAC tag of encryption key index within TLV data
 *                         stream.
 *   EC_TAG_LEN         -- is the HMAC tag length.
 *   EC_PUBK_INDEX      -- is the index of shared public key within TLV data stream;
 *                         EC_PUBK_LEN represents length in bytes.
 *   EC_CIPHERKEY_INDEX -- is the encryption key index within TLV data stream.
 *   EC_CIPHERKEY_LEN   -- is the length of an encryption key; depends on selected
 *                         encryption.
 *
 * Note that in case of ECIES, the BOOT_ENC_TLV_SIZE will be defined as
 * a sum of EC_*_LEN TLV components, defined for selected key exchange.
 */

#ifdef MCUBOOT_AES_256
#   define BOOT_ENC_KEY_SIZE   32
#else
#   define BOOT_ENC_KEY_SIZE   16
#endif

#ifdef MCUBOOT_HMAC_SHA512
#   define BOOT_HMAC_SIZE      64
#else
#   define BOOT_HMAC_SIZE      32
#endif

#if defined(MCUBOOT_ENCRYPT_RSA)
#   define BOOT_ENC_TLV_SIZE   (256)
#   define BOOT_ENC_TLV        IMAGE_TLV_ENC_RSA2048
#elif defined(MCUBOOT_ENCRYPT_EC256)
#   if defined(MCUBOOT_HMAC_SHA512)
#       error "ECIES-P256 does not support HMAC-SHA512"
#   endif
#   define EC_PUBK_LEN         (65)
#   define EC_PRIVK_LEN        (32)
#   define EC_SHARED_LEN       (32)
#   define BOOT_ENC_TLV        IMAGE_TLV_ENC_EC256
#elif defined(MCUBOOT_ENCRYPT_X25519)
#   define EC_PUBK_LEN         (32)
#   define EC_PRIVK_LEN        (32)
#   define EC_SHARED_LEN       (32)
#   if !defined(MCUBOOT_HMAC_SHA512)
#       define BOOT_ENC_TLV     IMAGE_TLV_ENC_X25519
#   else
#       define BOOT_ENC_TLV     IMAGE_TLV_ENC_X25519_SHA512
#   endif
#elif defined(MCUBOOT_ENCRYPT_KW)
#   define BOOT_ENC_TLV_SIZE   (BOOT_ENC_KEY_SIZE + 8)
#   define BOOT_ENC_TLV        IMAGE_TLV_ENC_KW
#endif

/* Common ECIES definitions */
#if defined(EC_PUBK_LEN)
#   define EC_PUBK_INDEX       (0)
#   define EC_TAG_LEN          (BOOT_HMAC_SIZE)
#   define EC_TAG_INDEX        (EC_PUBK_INDEX + EC_PUBK_LEN)
#   define EC_CIPHERKEY_INDEX  (EC_TAG_INDEX + EC_TAG_LEN)
#   define EC_CIPHERKEY_LEN    BOOT_ENC_KEY_SIZE
#   define EC_SHARED_KEY_LEN   (32)
#   define BOOT_ENC_TLV_SIZE   (EC_PUBK_LEN + EC_TAG_LEN + EC_CIPHERKEY_LEN)
#endif

#define BOOT_ENC_KEY_ALIGN_SIZE ALIGN_UP(BOOT_ENC_KEY_SIZE, BOOT_MAX_ALIGN)
#define BOOT_ENC_TLV_ALIGN_SIZE ALIGN_UP(BOOT_ENC_TLV_SIZE, BOOT_MAX_ALIGN)

#ifdef __cplusplus
}
#endif

#endif /* BOOTUTIL_ENC_KEY_PUBLIC_H */
