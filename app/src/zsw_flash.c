/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zsw_flash, LOG_LEVEL_DBG);

#define SPI_FLASH_TEST_REGION_OFFSET 0xff000
#define SPI_FLASH_SECTOR_SIZE        4096

// Just to verify the HW. TODO use properly.
static void single_sector_test(const struct device *flash_dev)
{
	const uint8_t expected[] = { 0x55, 0xaa, 0x66, 0x99 };
	const size_t len = sizeof(expected);
	uint8_t buf[sizeof(expected)];
	int rc;

	/* Write protection needs to be disabled before each write or
	 * erase, since the flash component turns on write protection
	 * automatically after completion of write and erase
	 * operations.
	 */
	LOG_DBG("Test 1: Flash erase");

	/* Full flash erase if SPI_FLASH_TEST_REGION_OFFSET = 0 and
	 * SPI_FLASH_SECTOR_SIZE = flash size
	 */
	rc = flash_erase(flash_dev, SPI_FLASH_TEST_REGION_OFFSET,
			 SPI_FLASH_SECTOR_SIZE);
	if (rc != 0) {
		LOG_DBG("Flash erase failed! %d", rc);
	} else {
		LOG_DBG("Flash erase succeeded!");
	}

	LOG_DBG("Attempting to write %zu bytes", len);
	rc = flash_write(flash_dev, SPI_FLASH_TEST_REGION_OFFSET, expected, len);
	if (rc != 0) {
		LOG_DBG("Flash write failed! %d", rc);
		return;
	}

	memset(buf, 0, len);
	rc = flash_read(flash_dev, SPI_FLASH_TEST_REGION_OFFSET, buf, len);
	if (rc != 0) {
		LOG_DBG("Flash read failed! %d", rc);
		return;
	}

	if (memcmp(expected, buf, len) == 0) {
		LOG_DBG("Data read matches data written. Good!!");
	} else {
		const uint8_t *wp = expected;
		const uint8_t *rp = buf;
		const uint8_t *rpe = rp + len;

		LOG_DBG("Data read does not match data written!!");
		while (rp < rpe) {
			LOG_DBG("%08x wrote %02x read %02x %s",
			       (uint32_t)(SPI_FLASH_TEST_REGION_OFFSET + (rp - buf)),
			       *wp, *rp, (*rp == *wp) ? "match" : "MISMATCH");
			++rp;
			++wp;
		}
	}
}

static int zsw_flash_test(void)
{
	const struct device *flash_dev = DEVICE_DT_GET(DT_ALIAS(spi_flash0));

	if (!device_is_ready(flash_dev)) {
		LOG_ERR("No external flash present: %s", flash_dev->name);
		return 0;
	}

	single_sector_test(flash_dev);
	return 0;
}

SYS_INIT(zsw_flash_test, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
