/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef H_DECOMPRESSION_
#define H_DECOMPRESSION_

#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include "bootutil/bootutil.h"
#include "bootutil/bootutil_public.h"
#include "bootutil/image.h"
#include "../src/bootutil_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Checks if a compressed image header is valid.
 *
 * @param hdr               Image header.
 * @param fap               Flash area of the slot.
 * @param state             Bootloader state object.
 *
 * @return                  true if valid; false if invalid.
 */
bool boot_is_compressed_header_valid(const struct image_header *hdr, const struct flash_area *fap,
                                     struct boot_loader_state *state);

/**
 * Reads in compressed image data from a slot, decompresses it and writes it out to a destination
 * slot, including corresponding image headers and TLVs.
 *
 * @param state             Bootloader state object.
 * @param fap_src           Flash area of the source slot.
 * @param fap_dst           Flash area of the destination slot.
 * @param off_src           Offset of the source slot to read from (should be 0).
 * @param off_dst           Offset of the destination slot to write to (should be 0).
 * @param sz                Size of the source slot data.
 * @param buf               Temporary buffer for reading data from.
 * @param buf_size          Size of temporary buffer.
 *
 * @return                  0 on success; nonzero on failure.
 */
int boot_copy_region_decompress(struct boot_loader_state *state, const struct flash_area *fap_src,
                                const struct flash_area *fap_dst, uint32_t off_src,
                                uint32_t off_dst, uint32_t sz, uint8_t *buf, size_t buf_size);

/**
 * Gets the total data size (excluding headers and TLVs) of a compressed image when it is
 * decompressed.
 *
 * @param hdr               Image header.
 * @param fap               Flash area of the slot.
 * @param img_decomp_size   Pointer to variable that will be updated with the decompressed image
 *                          size.
 *
 * @return                  0 on success; nonzero on failure.
 */
int bootutil_get_img_decomp_size(const struct image_header *hdr, const struct flash_area *fap,
                                 uint32_t *img_decomp_size);

/**
 * Calculate MCUboot-compatible image hash of compressed image slot.
 *
 * @param state           MCUboot state.
 * @param hdr             Image header.
 * @param fap             Flash area of the slot.
 * @param tmp_buf         Temporary buffer for reading data from.
 * @param tmp_buf_sz      Size of temporary buffer.
 * @param hash_result     Pointer to a variable that will be updated with the image hash.
 * @param seed            Not currently used, set to NULL.
 * @param seed_len        Not currently used, set to 0.
 *
 * @return                0 on success; nonzero on failure.
 */
int bootutil_img_hash_decompress(struct boot_loader_state *state, struct image_header *hdr,
                                 const struct flash_area *fap, uint8_t *tmp_buf,
                                 uint32_t tmp_buf_sz, uint8_t *hash_result,
                                 uint8_t *seed, int seed_len);

/**
 * Calculates the size that the compressed image protected TLV section will occupy once the image
 * has been decompressed.
 *
 * @param hdr             Image header.
 * @param fap             Flash area of the slot.
 * @param sz              Pointer to variable that will be updated with the protected TLV size.
 *
 * @return                0 on success; nonzero on failure.
 */
int boot_size_protected_tlvs(const struct image_header *hdr, const struct flash_area *fap_src,
                             uint32_t *sz);

#ifdef __cplusplus
}
#endif

#endif /* H_DECOMPRESSION_ */
