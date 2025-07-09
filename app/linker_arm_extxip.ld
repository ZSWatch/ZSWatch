/*
 * Copyright (c) 2022 Carlo Caione <ccaione@baylibre.com>
 * Copyright# Copyright (c) 2024 Nordic Semiconductor
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file
 * @brief External QSPI flash Linker command/script file
 *
 * Linker script for moving desired .text and .data to the external
 * memory space.
 */

#include <zephyr/linker/sections.h>
#include <zephyr/devicetree.h>

#include <zephyr/linker/linker-defs.h>
#include <zephyr/linker/linker-tool.h>


/* Let SystemInit() be called in place of z_arm_platform_init() by default. */
PROVIDE(z_arm_platform_init = SystemInit);

/*
 * nRF5340dk and thingy53 are shipping QSPI external flashes.
 * These memories are mapped beginning from 0x1000_0000 internal SoC memory
 * address. This addressing space can be used for XIP and direct data access.
 */
MEMORY
{
#if CONFIG_NCS_IS_VARIANT_IMAGE
     /* This maps in mcuboot_secondary_1 partition defined in pm_static_no_network_core_directxip.yml
      * components for ORIGIN calculation:
      *  - 0x10000000: offset of QSPI external memory in SoC memory mapping.
      *  - 0xD4000: mcuboot_secondary_1 offset in QSPI external memory
      *  - 0x200: image header size.
      * The size of this region is size of mcuboot_secondary_1 reduced by the
      * image header size.
      */
     EXTFLASH (wx) : ORIGIN = 0x100D4200, LENGTH = 0x3FE00
#else
     /* This maps in mcuboot_primary_2 partition defined in pm_static.yml
      * components for ORIGIN calculation:
      *  - 0x10000000: offset of QSPI external memory in SoC memory mapping.
      *  - 0x120000: mcuboot_primary_2 offset in QSPI external memory
      *  - 0x200: image header size.
      * The size of this region is size of mcuboot_primary_2 reduced by the
      * image header size.
      */
     EXTFLASH (wx) : ORIGIN = 0x10120200, LENGTH = 0x7FE00
#endif
}

#include <zephyr/arch/arm/cortex_m/scripts/linker.ld>