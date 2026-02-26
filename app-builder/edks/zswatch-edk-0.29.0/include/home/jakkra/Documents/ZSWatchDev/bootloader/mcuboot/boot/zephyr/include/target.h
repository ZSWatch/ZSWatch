/*
 *  Copyright (C) 2017, Linaro Ltd
 *  Copyright (c) 2019, Arm Limited
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_TARGETS_TARGET_
#define H_TARGETS_TARGET_

#ifndef USE_PARTITION_MANAGER

#if defined(MCUBOOT_TARGET_CONFIG)
/*
 * Target-specific definitions are permitted in legacy cases that
 * don't provide the information via DTS, etc.
 */
#include MCUBOOT_TARGET_CONFIG
#else
/*
 * Otherwise, the Zephyr SoC header and the DTS provide most
 * everything we need.
 */
#include <zephyr/devicetree.h>
#include <soc.h>
#include <zephyr/storage/flash_map.h>

#define FLASH_ALIGN FLASH_WRITE_BLOCK_SIZE

#endif /* !defined(MCUBOOT_TARGET_CONFIG) */

/*
 * Sanity check the target support.
 */
#if (!defined(CONFIG_XTENSA) && !defined(CONFIG_SOC_SERIES_NRF54H) && \
    !DT_HAS_CHOSEN(zephyr_flash_controller)) || \
    (defined(CONFIG_XTENSA) && !DT_NODE_EXISTS(DT_INST(0, jedec_spi_nor)) && \
    !defined(CONFIG_SOC_FAMILY_ESPRESSIF_ESP32)) || \
    (defined(CONFIG_SOC_SERIES_NRF54H) && !DT_HAS_CHOSEN(zephyr_flash)) || \
    !defined(FLASH_ALIGN) ||                  \
    !(FIXED_PARTITION_EXISTS(slot0_partition)) || \
    !(FIXED_PARTITION_EXISTS(slot1_partition) || CONFIG_SINGLE_APPLICATION_SLOT) || \
    (defined(CONFIG_BOOT_SWAP_USING_SCRATCH) && !FIXED_PARTITION_EXISTS(scratch_partition))
#error "Target support is incomplete; cannot build mcuboot."
#endif

#if (MCUBOOT_IMAGE_NUMBER == 2) && (!(FIXED_PARTITION_EXISTS(slot2_partition)) || \
                                     !(FIXED_PARTITION_EXISTS(slot3_partition)))
#error "Target support is incomplete; cannot build mcuboot."
#endif

#endif /* ifndef USE_PARTITION_MANAGER */

#endif /* H_TARGETS_TARGET_ */
