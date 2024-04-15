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

#include <zephyr/kernel.h>
#include <zsw_retained_ram_storage.h>
#ifndef CONFIG_BOARD_NATIVE_POSIX
#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/retention/retention.h>

static const struct device *retention_area = DEVICE_DT_GET(DT_NODELABEL(retention0));

struct retained_data retained;

void zsw_retained_ram_update(void)
{
    uint64_t now = k_uptime_get();
    char *timezone = getenv("TZ");

    retained.uptime_sum += (now - retained.uptime_latest);
    retained.uptime_latest = now;
    strncpy(retained.timezone, timezone, sizeof(retained.timezone) - 1);
    retention_write(retention_area, 0, (uint8_t *)&retained, sizeof(retained));
}

void zsw_retained_ram_reset(void)
{
    retention_clear(retention_area);
    zsw_retained_ram_update();
}

static int zsw_retained_init(void)
{
    if (!retention_is_valid(retention_area)) {
        retention_clear(retention_area);
    }
    retention_read(retention_area, 0, (uint8_t *)&retained, sizeof(retained));
    retained.uptime_latest = 0;
    retained.boots++;
    retention_write(retention_area, 0, (uint8_t *)&retained, sizeof(retained));
    return 0;
}

SYS_INIT(zsw_retained_init, POST_KERNEL, 1);
#else
struct retained_data retained;

void zsw_retained_ram_update(void)
{

}

void zsw_retained_ram_reset(void)
{
}
#endif

