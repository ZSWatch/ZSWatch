/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

#define ZSW_BOOT_MODE_RTT_FLASH_LOADER  0x0A
#define ZSW_BOOT_MODE_FLASH_ERASE       0xFF

int zsw_rtt_flash_loader_start(void);

int zsw_rtt_flash_loader_erase_external(void);

int zsw_rtt_flash_loader_reboot_and_erase_flash(void);
