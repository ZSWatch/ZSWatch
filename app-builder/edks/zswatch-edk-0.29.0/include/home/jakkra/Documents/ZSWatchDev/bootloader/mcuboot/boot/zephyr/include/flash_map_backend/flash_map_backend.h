/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __FLASH_MAP_BACKEND_H__
#define __FLASH_MAP_BACKEND_H__

#include <zephyr/storage/flash_map.h> // the zephyr flash_map
#include <zephyr/drivers/flash.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Provides abstraction of flash regions for type of use.
 * I.e. dude where's my image?
 *
 * System will contain a map which contains flash areas. Every
 * region will contain flash identifier, offset within flash and length.
 *
 * 1. This system map could be in a file within filesystem (Initializer
 * must know/figure out where the filesystem is at).
 * 2. Map could be at fixed location for project (compiled to code)
 * 3. Map could be at specific place in flash (put in place at mfg time).
 *
 * Note that the map you use must be valid for BSP it's for,
 * match the linker scripts when platform executes from flash,
 * and match the target offset specified in download script.
 */
#include <inttypes.h>
#include <sys/types.h>

/*
 * Retrieve a memory-mapped flash device's base address.
 *
 * On success, the address will be stored in the value pointed to by
 * ret.
 *
 * Returns 0 on success, or an error code on failure.
 */
int flash_device_base(uint8_t fd_id, uintptr_t *ret);

int flash_area_id_from_image_slot(int slot);
int flash_area_id_from_multi_image_slot(int image_index, int slot);

/**
 * Converts the specified flash area ID and image index (in multi-image setup)
 * to an image slot index.
 *
 * Returns image slot index (0 or 1), or -1 if ID doesn't correspond to an image
 * slot.
 */
int flash_area_id_to_multi_image_slot(int image_index, int area_id);

/* Retrieve the flash sector a given offset belongs to.
 *
 * Returns 0 on success, or an error code on failure.
 */
int flash_area_sector_from_off(off_t off, struct flash_sector *sector);

static inline uint32_t flash_area_get_off(const struct flash_area *fa)
{
	return (uint32_t)fa->fa_off;
}

static inline uint32_t flash_area_get_size(const struct flash_area *fa)
{
	return (uint32_t)fa->fa_size;
}

static inline uint8_t flash_area_get_id(const struct flash_area *fa)
{
	return fa->fa_id;
}

uint8_t flash_area_get_device_id(const struct flash_area *fa);

/*
 * Returns the value expected to be read when accessing any erased
 * flash byte.
 */
uint8_t flash_area_erased_val(const struct flash_area *fap);

static inline uint32_t flash_sector_get_off(const struct flash_sector *fs)
{
	return fs->fs_off;
}

static inline uint32_t flash_sector_get_size(const struct flash_sector *fs)
{
	return fs->fs_size;
}

/* Retrieve the flash sector withing given flash area, at a given offset.
 *
 * @param fa        flash area where the sector is taken from.
 * @param off       offset within flash area.
 * @param sector    structure of sector information.
 * Returns 0 on success, -ERANGE if @p off is beyond flash area size,
 *         other negative errno code on failure.
 */
int flash_area_get_sector(const struct flash_area *fa, off_t off,
                          struct flash_sector *fs);


#if defined(CONFIG_MCUBOOT)
static inline bool flash_area_erase_required(const struct flash_area *fa)
{
#if defined(CONFIG_FLASH_HAS_EXPLICIT_ERASE) && defined(CONFIG_FLASH_HAS_NO_EXPLICIT_ERASE)
    return flash_params_get_erase_cap(flash_get_parameters(flash_area_get_device(fa))) &
            FLASH_ERASE_C_EXPLICIT;
#elif defined(CONFIG_FLASH_HAS_EXPLICIT_ERASE)
    (void)fa;

    return true;
#else
    (void)fa;

    return false;
#endif
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_MAP_BACKEND_H__ */
