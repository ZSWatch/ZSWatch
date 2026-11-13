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

#include <stdint.h>

#define ZSW_USER_LFS_MOUNT_POINT "/user"

#define ZSW_USER_LFS_CACHE_SIZE  512

int zsw_filesytem_get_num_rawfs_files(void);

int zsw_filesytem_get_total_size(void);

int zsw_filesytem_erase(void);

/**
 * @brief Get a pointer to the content of a file in the image resource filesystem (S:).
 *
 * Only works while the external flash is memory mapped (XIP on). The display keeps
 * XIP on while LVGL renders, so the pointer can be used for as long as a draw takes.
 * Do not keep it longer than that.
 *
 * @param name File name without drive letter, for example "icon.bin".
 * @param len Set to the length of the file.
 *
 * @return Pointer to the file content, NULL if not memory mapped or the file does not exist.
 */
const uint8_t *zsw_filesystem_get_file(const char *name, uint32_t *len);
