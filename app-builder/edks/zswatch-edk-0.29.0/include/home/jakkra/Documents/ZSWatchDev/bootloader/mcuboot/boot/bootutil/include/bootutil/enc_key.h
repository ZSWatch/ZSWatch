/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2018-2019 JUUL Labs
 * Copyright (c) 2019 Arm Limited
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

#ifndef BOOTUTIL_ENC_KEY_H
#define BOOTUTIL_ENC_KEY_H

#include <stdbool.h>
#include <stdint.h>
#include <flash_map_backend/flash_map_backend.h>
#include "bootutil/crypto/aes_ctr.h"
#include "bootutil/image.h"
#include "bootutil/sign_key.h"
#include "bootutil/enc_key_public.h"

#ifdef __cplusplus
extern "C" {
#endif

struct enc_key_data {
    uint8_t valid;
    bootutil_aes_ctr_context aes_ctr;
};

/**
 * Retrieve the private key for image encryption.
 *
 * @param[out]  private_key  structure to store the private key and
 *                           its length.
 *
 * @return                   0 on success; nonzero on failure.
 *
 */
int boot_enc_retrieve_private_key(struct bootutil_key **private_key);

struct boot_status;
struct boot_loader_state;

/* Decrypt random, symmetric encryption key */
int boot_decrypt_key(const uint8_t *buf, uint8_t *enckey);

int boot_enc_init(struct enc_key_data *enc_state);
int boot_enc_drop(struct enc_key_data *enc_state);
int boot_enc_set_key(struct enc_key_data *enc_state, const uint8_t *key);
int boot_enc_load(struct boot_loader_state *state, int slot,
                  const struct image_header *hdr, const struct flash_area *fap,
                  struct boot_status *bs);
bool boot_enc_valid(const struct enc_key_data *enc_state);
void boot_enc_encrypt(struct enc_key_data *enc_state,
        uint32_t off, uint32_t sz, uint32_t blk_off, uint8_t *buf);
void boot_enc_decrypt(struct enc_key_data *enc_state,
        uint32_t off, uint32_t sz, uint32_t blk_off, uint8_t *buf);
/* Note that boot_enc_zeorize takes BOOT_CURR_ENC, not BOOT_CURR_ENC_SLOT */
void boot_enc_zeroize(struct enc_key_data *enc_state);

#ifdef __cplusplus
}
#endif

#endif /* BOOTUTIL_ENC_KEY_H */
