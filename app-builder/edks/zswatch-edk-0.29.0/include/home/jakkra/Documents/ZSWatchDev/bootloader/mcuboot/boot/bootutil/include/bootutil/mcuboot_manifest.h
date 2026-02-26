/*
 *  Copyright (c) 2025 Nordic Semiconductor ASA
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MCUBOOT_MANIFEST_H__
#define __MCUBOOT_MANIFEST_H__

/**
 * @file mcuboot_manifest.h
 *
 * @note This file is only used when MCUBOOT_MANIFEST_UPDATES is enabled.
 */

#include <stdint.h>
#include "bootutil/bootutil.h"
#ifdef CONFIG_MCUBOOT
#include "bootutil/crypto/sha.h"
#elif defined(CONFIG_MCUBOOT_BOOTLOADER_USES_SHA512)
    #define IMAGE_HASH_SIZE (64)
#else
    #define IMAGE_HASH_SIZE (32)
#endif

#ifndef MCUBOOT_IMAGE_NUMBER
#ifdef CONFIG_UPDATEABLE_IMAGE_NUMBER
#define MCUBOOT_IMAGE_NUMBER CONFIG_UPDATEABLE_IMAGE_NUMBER
#else
#error "MCUBOOT_IMAGE_NUMBER must be defined when MCUBOOT_MANIFEST_UPDATES is enabled"
#endif
#endif

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Manifest structure for image updates. */
struct mcuboot_manifest {
        uint32_t format;
        uint32_t image_count;
        /* Skip a digest of the MCUBOOT_MANIFEST_IMAGE_INDEX image. */
        uint8_t image_hash[MCUBOOT_IMAGE_NUMBER - 1][IMAGE_HASH_SIZE];
} __packed;

/**
 * @brief Check if the specified manifest has the correct format.
 *
 * @param[in] manifest  The reference to the manifest structure.
 *
 * @return true on success.
 */
bool bootutil_verify_manifest(const struct mcuboot_manifest *manifest);

/**
 * @brief Get the image hash from the manifest.
 *
 * @param[in] manifest     The reference to the manifest structure.
 * @param[in] image_index  The index of the image to get the hash for.
 *                         Must be in range <0, MCUBOOT_IMAGE_NUMBER - 1>, but
 *                         must not be equal to MCUBOOT_MANIFEST_IMAGE_INDEX.
 *
 * @return true if hash matches with the manifest, false otherwise.
 */
bool bootutil_verify_manifest_image_hash(const struct mcuboot_manifest *manifest,
                                         const uint8_t *exp_hash, uint32_t image_index);

#ifdef __cplusplus
}
#endif

#endif /* __MCUBOOT_MANIFEST_H__ */
