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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/crc.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/retention/bootmode.h>
#include <filesystem/zsw_rtt_flash_loader.h>
#include <SEGGER_RTT.h>

LOG_MODULE_REGISTER(zsw_rtt_flash_loader, LOG_LEVEL_DBG);

#define SPI_FLASH_SECTOR_SIZE        4096

#define RTT_RECIVE_BUFFER_SIZE SPI_FLASH_SECTOR_SIZE

#define TRANSFER_TIMEOUT_MS     5000

#define START_LOAD_SEQUENCE "LOADER_START"
#define STOP_LOAD_SEQUENCE  "LOADER_END"
#define DUMP_FLASH_SEQUENCE "DUMP_START"
#define READ_DONE_SEQUENCE  "DUMP_END"

#define RTT_CHANNEL_NAME    "FlashLoaderChannel"
#define RTT_MAGIC           0x0A0A0A0A

struct flash_partition_search_user_data {
    int     found_part_id;
    char   *search_label_name;
};

struct rtt_rx_data_header {
    uint32_t    magic;
    uint32_t    address;
    uint32_t    crc;
};

// Names must match partition manager and dts
static char *partition_map[] = {
    [FIXED_PARTITION_ID(lvgl_raw_partition)] = "lvgl_raw_partition",
    [FIXED_PARTITION_ID(littlefs_storage)] = "littlefs_storage",
};

#define DATA_BUFFER_SIZE (SPI_FLASH_SECTOR_SIZE + sizeof(struct rtt_rx_data_header))
#define UP_BUFFER_SIZE (SPI_FLASH_SECTOR_SIZE + 1 + sizeof(struct rtt_rx_data_header))
#define DOWN_BUFFER_SIZE (SPI_FLASH_SECTOR_SIZE + 1 + sizeof(struct rtt_rx_data_header))

static void rtt_load_flash_thread(void *, void *, void *);
static void rtt_dump_flash_thread(void *, void *, void *);

K_THREAD_STACK_DEFINE(rtt_work_thread_stack, 8192);
static struct k_thread rtt_work_thread;

static const struct flash_area *flash_area;

static uint8_t *data_buf;
static uint8_t *up_buffer;
static uint8_t *down_buffer;

static int loader_write_flash(int partition_id, int buf_idx, uint8_t *buf, int len)
{
    int rc;
    if (len != SPI_FLASH_SECTOR_SIZE) {
        LOG_ERR("Buflen must be same size as SPI_FLASH_SECTOR_SIZE: %d", SPI_FLASH_SECTOR_SIZE);
        return -EINVAL;
    }

#ifdef CONFIG_ERASE_PROGRESSIVELY
    rc = flash_area_erase(flash_area, buf_idx * SPI_FLASH_SECTOR_SIZE, SPI_FLASH_SECTOR_SIZE);
    if (rc != 0) {
        printk("Flash erase failed! %d", rc);
        return rc;
    }
#endif

    rc = flash_area_write(flash_area, buf_idx * SPI_FLASH_SECTOR_SIZE, buf, len);
    if (rc != 0) {
        printk("Flash write failed! %d\n", rc);
        return rc;
    }

    return 0;
}

static int loader_read_flash(int partition_id, int buf_idx, uint8_t *buf, int len)
{
    int rc;

    rc = flash_area_read(flash_area, buf_idx * SPI_FLASH_SECTOR_SIZE, buf, len);
    if (rc != 0) {
        printk("Flash read failed! %d\n", rc);
        return rc;
    }

    return 0;
}

static int find_partition_id_from_label(char *label)
{
    for (int i = 0; i < ARRAY_SIZE(partition_map); i++) {
        if (strcmp(partition_map[i], label) == 0) {
            return i;
        }
    }

    return -ENODEV;
}

static bool check_start_sequence(uint8_t *buf, uint32_t len, int *partition_id)
{
    char *partition_label;
    if (strncmp(buf, START_LOAD_SEQUENCE, strlen(START_LOAD_SEQUENCE)) == 0) {
        partition_label = strchr(buf, ':');
        if (partition_label) {
            *partition_id = find_partition_id_from_label(partition_label + 1);
            return true;
        } else {
            LOG_WRN("No partition label found");
        }
    }

    return false;
}

static bool check_end_sequence(uint8_t *buf, uint32_t len)
{
    if (memcmp(buf, STOP_LOAD_SEQUENCE, strlen(STOP_LOAD_SEQUENCE)) == 0) {
        printk("End sequence received\n");
        return true;
    }

    return false;
}

static bool check_read_sequence(uint8_t *buf, uint32_t len, int *partition_id)
{
    char *partition_label;
    if (memcmp(buf, DUMP_FLASH_SEQUENCE, strlen(DUMP_FLASH_SEQUENCE)) == 0) {
        partition_label = strchr(buf, ':');
        if (partition_label) {
            *partition_id = find_partition_id_from_label(partition_label + 1);
            return true;
        } else {
            LOG_WRN("No partition label found");
        }
    }

    return false;
}

static void rtt_load_flash_thread(void *partition_id_param, void *, void *)
{
    int ret;
    int len;
    int block_index = 0;
    int buffer_index = 0;
    int bytes_flashed = 0;
    struct rtt_rx_data_header *header;
    uint8_t partition_id = (uint8_t)((uint32_t)partition_id_param);
    uint32_t len_to_read;
    uint32_t last_activity_ms = k_uptime_get_32();
    uint32_t crc;

    ret = flash_area_open(partition_id, &flash_area);

    if (ret != 0) {
        LOG_ERR("FAIL: unable to find flash area %d: %d\n", (int)partition_id, ret);
        return;
    }

#ifndef CONFIG_ERASE_PROGRESSIVELY
    ret = flash_area_erase(flash_area, 0, flash_area->fa_size);
    LOG_WRN("Erasing flash area ... %d", ret);
#endif

    while (1) {
        len_to_read = DATA_BUFFER_SIZE - buffer_index;
        len = SEGGER_RTT_Read(CONFIG_RTT_TRANSFER_CHANNEL, data_buf, len_to_read);

        if (len <= 0) {
            if (k_uptime_get_32() - last_activity_ms > TRANSFER_TIMEOUT_MS) {
                printk("RTT Transfer timeout. Aborting.\n");
                break;
            }
            k_msleep(100);
            continue;
        }

        last_activity_ms = k_uptime_get_32();
        if (check_end_sequence(data_buf, len)) {
            printk("RTT Transfer done: %d bytes flashed\n", bytes_flashed);
            break;
        } else if (len == len_to_read) {
            header = (struct rtt_rx_data_header *)data_buf;
            if (header->magic == RTT_MAGIC) {
                if (header->address % SPI_FLASH_SECTOR_SIZE != 0) {
                    LOG_ERR("Anvalid addres, must be multiple of %d", SPI_FLASH_SECTOR_SIZE);
                    break;
                }
            } else {
                LOG_ERR("RTT: Invalid magic: %x\n", header->magic);
                break;
            }
            ret = loader_write_flash((int)partition_id, header->address / SPI_FLASH_SECTOR_SIZE,
                                     data_buf + sizeof(struct rtt_rx_data_header), DATA_BUFFER_SIZE - sizeof(struct rtt_rx_data_header));
            if (ret != 0) {
                printk("loader_write_flash failed: %dn", ret);
                break;
            }

            crc = crc32_ieee(data_buf + sizeof(struct rtt_rx_data_header), DATA_BUFFER_SIZE - sizeof(struct rtt_rx_data_header));
            if (crc != header->crc) {
                LOG_ERR("CRC mismatch, try lowering JLink RTT speed");
            }
            block_index++;
            buffer_index = 0;
            bytes_flashed += DATA_BUFFER_SIZE;
            if (block_index % 10 == 0) {
                printk("RTT: Received %d (%d)\n", bytes_flashed, block_index);
            }
        } else {
            buffer_index += len;
            if (buffer_index > DATA_BUFFER_SIZE) {
                __ASSERT(false, "Something wrong got size %d", buffer_index);
            }
        }
    }

    flash_area_close(flash_area);
}

static void rtt_dump_flash_thread(void *partition_id_param, void *, void *)
{
    int ret;
    int len;
    int buffer_index = 0;
    int bytes_sent = 0;
    uint8_t partition_id = (uint8_t)((uint32_t)partition_id_param);
    uint32_t len_to_send;
    uint32_t last_activity_ms = k_uptime_get_32();

    ret = flash_area_open(partition_id, &flash_area);

    if (ret != 0) {
        LOG_ERR("FAIL: unable to find flash area %d: %d\n", (int)partition_id, ret);
        return;
    }

#ifndef CONFIG_ERASE_PROGRESSIVELY
    ret = flash_area_erase(flash_area, 0, flash_area->fa_size);
    LOG_WRN("Erasing flash area ... %d", ret);
#endif

    while (buffer_index < flash_area->fa_size / SPI_FLASH_SECTOR_SIZE) {
        ret = loader_read_flash((int)partition_id, buffer_index, data_buf, SPI_FLASH_SECTOR_SIZE);
        if (ret != 0) {
            printk("loader_read_flash failed: %dn", ret);
            break;
        }
        len_to_send = SPI_FLASH_SECTOR_SIZE;
        while (len_to_send > 0) {
            len = SEGGER_RTT_Write(CONFIG_RTT_TRANSFER_CHANNEL, data_buf, SPI_FLASH_SECTOR_SIZE);
            bytes_sent += len;
            len_to_send -= len;
            if (len > 0) {
                last_activity_ms = k_uptime_get_32();
            } else {
                if (k_uptime_get_32() - last_activity_ms > TRANSFER_TIMEOUT_MS) {
                    printk("RTT Transfer timeout. Aborting.\n");
                }
            }
            if (buffer_index % 10 == 0) {
                printk("RTT: Sent %d (%d)\n", bytes_sent, buffer_index);
            }
            k_msleep(1);
        }
        buffer_index++;
    }
    printk("Done sending %d bytes\n", bytes_sent);

    // Sleep a bit so python code can read the last bytes
    k_msleep(2000);
    SEGGER_RTT_Write(CONFIG_RTT_TRANSFER_CHANNEL, READ_DONE_SEQUENCE, strlen(READ_DONE_SEQUENCE));
    k_msleep(2000);

    flash_area_close(flash_area);
}

int zsw_rtt_flash_loader_start(void)
{
    int partition_id;

    bootmode_clear();

    data_buf = k_malloc(DATA_BUFFER_SIZE);
    up_buffer = k_malloc(UP_BUFFER_SIZE);
    down_buffer = k_malloc(DOWN_BUFFER_SIZE);

    __ASSERT(data_buf && up_buffer && down_buffer, "Failed to allocate buffers for RTT file tarnsfer");

    SEGGER_RTT_ConfigUpBuffer(CONFIG_RTT_TRANSFER_CHANNEL, RTT_CHANNEL_NAME,
                              up_buffer, UP_BUFFER_SIZE,
                              SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    SEGGER_RTT_ConfigDownBuffer(CONFIG_RTT_TRANSFER_CHANNEL, RTT_CHANNEL_NAME,
                                down_buffer, DOWN_BUFFER_SIZE,
                                SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);

    while (1) {
        int len = SEGGER_RTT_Read(CONFIG_RTT_TRANSFER_CHANNEL, data_buf, DATA_BUFFER_SIZE);

        if (len <= 0) {
            k_msleep(100);
            //SEGGER_RTT_Write(CONFIG_RTT_TRANSFER_CHANNEL, "READY", strlen("READY"));
            continue;
        }

        if (check_start_sequence(data_buf, len, &partition_id)) {
            printk("Load sequence received: %s partition ID: %d\n", data_buf, partition_id);
            k_tid_t tid = k_thread_create(&rtt_work_thread, rtt_work_thread_stack, K_KERNEL_STACK_SIZEOF(rtt_work_thread_stack),
                                          rtt_load_flash_thread, (void *)partition_id, NULL, NULL, CONFIG_NUM_COOP_PRIORITIES - 2, 0, K_NO_WAIT);
            k_thread_join(tid, K_FOREVER);
            printk("Load thread done\n");
        } else if (check_read_sequence(data_buf, len, &partition_id)) {
            printk("Read sequence received: %s partition ID: %d\n", data_buf, partition_id);
            k_tid_t tid = k_thread_create(&rtt_work_thread, rtt_work_thread_stack, K_KERNEL_STACK_SIZEOF(rtt_work_thread_stack),
                                          rtt_dump_flash_thread, (void *)partition_id, NULL, NULL, CONFIG_NUM_COOP_PRIORITIES - 2, 0, K_NO_WAIT);
            k_thread_join(tid, K_FOREVER);
            printk("Read thread done\n");
        } else {
            printk("Unknown sequence received\n");
        }
    }

    return 0;
}

int zsw_rtt_flash_loader_erase_external(void)
{
    struct flash_pages_info flash_get_page;
    const struct device *flash_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(nordic_pm_ext_flash));

    bootmode_clear();

    if (flash_dev) {
        flash_get_page_info_by_idx(flash_dev, 0, &flash_get_page);
        flash_erase(flash_dev, 0, flash_get_page_count(flash_dev) * flash_get_page.size);
        sys_reboot(SYS_REBOOT_COLD);
        return 0;
    } else {
        return -ENODEV;
    }
}

int zsw_rtt_flash_loader_reboot_and_erase_flash(void)
{
    const struct device *flash_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(nordic_pm_ext_flash));
    if (flash_dev) {
        bootmode_set(ZSW_BOOT_MODE_FLASH_ERASE);
        sys_reboot(SYS_REBOOT_COLD);
    } else {
        return -ENODEV;
    }

    return 0;
}