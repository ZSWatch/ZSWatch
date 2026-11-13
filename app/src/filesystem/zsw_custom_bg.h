/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/*
 * The custom watchface background is uploaded as a file in LittleFS. Drawing it from
 * there means reading the whole image from flash for every frame, so it is also copied
 * to a partition of its own and drawn straight from the memory mapped flash, like the
 * images in the S: resource filesystem.
 */

// LVGL image source for the copy. zsw_image_decoder registers the B: drive that serves it.
#define ZSW_CUSTOM_BG_SRC "B:bg.bin"

/**
 * Copies a background image file to the background partition, unless the partition
 * already holds the same file. Blocks while the flash is erased and written.
 * The copy must not be drawn meanwhile.
 *
 * @return 0 when the copy matches the file and can be drawn, else a negative error code.
 */
int zsw_custom_bg_store(const char *path);

/**
 * Reads from the copy made by zsw_custom_bg_store() through the flash driver.
 *
 * @param len Size of the copy, set when it is not NULL.
 * @return Bytes read, 0 past the end, or a negative error code if there is no copy.
 */
int zsw_custom_bg_read(uint32_t offset, void *buf, uint32_t size, uint32_t *len);

/**
 * Gets a pointer to the copy made by zsw_custom_bg_store(). It can be used for as long
 * as a draw takes, see zsw_filesystem_get_file().
 *
 * @return Start of the image file, or NULL if there is no copy or the flash is not memory mapped.
 */
const uint8_t *zsw_custom_bg_get(uint32_t *len);
