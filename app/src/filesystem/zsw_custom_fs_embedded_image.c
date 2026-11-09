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

/*
 * Links the image resource filesystem into the executable for targets without
 * external flash (native_sim), so they use the same images as the watch.
 */

#include <zephyr/toolchain.h>

__asm__(".section .rodata.zsw_fs_embedded_image, \"a\"\n"
        ".balign 16\n"
        ".global zsw_fs_embedded_image\n"
        "zsw_fs_embedded_image:\n"
        ".incbin \"" ZSW_FS_EMBEDDED_IMAGE_FILE "\"\n"
        ".global zsw_fs_embedded_image_end\n"
        "zsw_fs_embedded_image_end:\n"
        ".previous\n");
