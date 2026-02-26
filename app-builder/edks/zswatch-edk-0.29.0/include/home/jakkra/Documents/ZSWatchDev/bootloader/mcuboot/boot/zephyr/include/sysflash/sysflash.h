/*
 * Copyright (c) 2023-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#if USE_PARTITION_MANAGER
/* Blocking the rest of the file */
#define __SYSFLASH_H__
#include <sysflash/pm_sysflash.h>
#endif

#ifndef __SYSFLASH_H__
#define __SYSFLASH_H__

#include <mcuboot_config/mcuboot_config.h>
#include <zephyr/devicetree.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/util_macro.h>

#ifndef SOC_FLASH_0_ID
#define SOC_FLASH_0_ID 0
#endif

#ifndef SPI_FLASH_0_ID
#define SPI_FLASH_0_ID 1
#endif

#if !defined(CONFIG_SINGLE_APPLICATION_SLOT) && !defined(CONFIG_MCUBOOT_BOOTLOADER_MODE_SINGLE_APP)

/* Each pair of slots is separated by , and there is no terminating character */
#define FLASH_AREA_IMAGE_0_SLOTS    slot0_partition, slot1_partition
#define FLASH_AREA_IMAGE_1_SLOTS    slot2_partition, slot3_partition
#define FLASH_AREA_IMAGE_2_SLOTS    slot4_partition, slot5_partition

#if (MCUBOOT_IMAGE_NUMBER == 1)
#define ALL_AVAILABLE_SLOTS FLASH_AREA_IMAGE_0_SLOTS
#elif (MCUBOOT_IMAGE_NUMBER == 2)
#define ALL_AVAILABLE_SLOTS FLASH_AREA_IMAGE_0_SLOTS, \
                            FLASH_AREA_IMAGE_1_SLOTS
#elif (MCUBOOT_IMAGE_NUMBER == 3)
#define ALL_AVAILABLE_SLOTS FLASH_AREA_IMAGE_0_SLOTS, \
                            FLASH_AREA_IMAGE_1_SLOTS, \
                            FLASH_AREA_IMAGE_2_SLOTS
#endif

static inline uint32_t __flash_area_ids_for_slot(int img, int slot)
{
    static const int all_slots[] = {
        FOR_EACH_NONEMPTY_TERM(FIXED_PARTITION_ID, (,), ALL_AVAILABLE_SLOTS)
    };
    return all_slots[img * 2 + slot];
};

#undef FLASH_AREA_IMAGE_0_SLOTS
#undef FLASH_AREA_IMAGE_1_SLOTS
#undef FLASH_AREA_IMAGE_2_SLOTS
#undef ALL_AVAILABLE_SLOTS

#define FLASH_AREA_IMAGE_PRIMARY(x) __flash_area_ids_for_slot(x, 0)
#define FLASH_AREA_IMAGE_SECONDARY(x) __flash_area_ids_for_slot(x, 1)

#if !defined(CONFIG_BOOT_SWAP_USING_MOVE) && !defined(CONFIG_BOOT_SWAP_USING_OFFSET)
#define FLASH_AREA_IMAGE_SCRATCH    FIXED_PARTITION_ID(scratch_partition)
#endif

#else /* !CONFIG_SINGLE_APPLICATION_SLOT && !CONFIG_MCUBOOT_BOOTLOADER_MODE_SINGLE_APP */

#define FLASH_AREA_IMAGE_PRIMARY(x)	FIXED_PARTITION_ID(slot0_partition)
#define FLASH_AREA_IMAGE_SECONDARY(x)	FIXED_PARTITION_ID(slot0_partition)

#endif /* CONFIG_SINGLE_APPLICATION_SLOT */

#endif /* __SYSFLASH_H__ */
