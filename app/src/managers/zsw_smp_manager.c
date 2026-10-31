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

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <string.h>
#include "drivers/zsw_display_control.h"
#include "zsw_smp_manager.h"
#include "ble/ble_comm.h"

#ifndef CONFIG_ARCH_POSIX
#include <zephyr/mgmt/mcumgr/mgmt/callbacks.h>
#endif

LOG_MODULE_REGISTER(zsw_smp_manager, LOG_LEVEL_INF);

#ifndef CONFIG_ARCH_POSIX

// The SMP service is always registered. Only the short connection interval
// follows the traffic. No SMP code may be relocated to XIP.

// Keep the short interval this long after the last SMP command.
#define SMP_TRAFFIC_HOLD_MS           5000
// Upper bound if a command never reports done.
#define SMP_TRAFFIC_MAX_COMMAND_MS    60000
// When the phone announces SMP use, keep the short interval this long for it to start.
#define SMP_ANNOUNCE_SHORT_INTERVAL_MS 5000

static K_MUTEX_DEFINE(traffic_mutex);
static int commands_active;
static int64_t last_traffic_ms;
static bool fs_write_render_blocked;

static void smp_traffic_done_work_handler(struct k_work *work);
static void unblock_fs_write_render(void);
static enum mgmt_cb_return cmd_callback(uint32_t event, enum mgmt_cb_return prev_status,
                                        int32_t *rc, uint16_t *group, bool *abort_more,
                                        void *data, size_t data_size);
static enum mgmt_cb_return fs_access_callback(uint32_t event, enum mgmt_cb_return prev_status,
                                              int32_t *rc, uint16_t *group, bool *abort_more,
                                              void *data, size_t data_size);

static K_WORK_DELAYABLE_DEFINE(smp_traffic_done_work, smp_traffic_done_work_handler);

static struct mgmt_callback cmd_callback_entry = {
    .callback = cmd_callback,
    .event_id = MGMT_EVT_OP_CMD_RECV | MGMT_EVT_OP_CMD_DONE,
};

static struct mgmt_callback fs_access_callback_entry = {
    .callback = fs_access_callback,
    .event_id = MGMT_EVT_OP_FS_MGMT_FILE_ACCESS | MGMT_EVT_OP_FS_MGMT_FILE_ACCESS_DONE,
};

static void smp_traffic_done_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    k_mutex_lock(&traffic_mutex, K_FOREVER);
    // A command may have started or finished after this was scheduled.
    int64_t elapsed = k_uptime_get() - last_traffic_ms;
    int64_t limit = commands_active > 0 ? SMP_TRAFFIC_MAX_COMMAND_MS : SMP_TRAFFIC_HOLD_MS;
    if (elapsed < limit) {
        k_work_reschedule(&smp_traffic_done_work, K_MSEC(limit - elapsed));
        k_mutex_unlock(&traffic_mutex);
        return;
    }
    commands_active = 0;
    ble_comm_short_interval_release(BLE_COMM_SHORT_INTERVAL_SMP);
    k_mutex_unlock(&traffic_mutex);
}

static enum mgmt_cb_return cmd_callback(uint32_t event, enum mgmt_cb_return prev_status,
                                        int32_t *rc, uint16_t *group, bool *abort_more,
                                        void *data, size_t data_size)
{
    ARG_UNUSED(prev_status);
    ARG_UNUSED(rc);
    ARG_UNUSED(group);
    ARG_UNUSED(abort_more);
    ARG_UNUSED(data);
    ARG_UNUSED(data_size);

    k_mutex_lock(&traffic_mutex, K_FOREVER);
    last_traffic_ms = k_uptime_get();
    if (event == MGMT_EVT_OP_CMD_RECV) {
        commands_active++;
        ble_comm_short_interval_request(BLE_COMM_SHORT_INTERVAL_SMP);
        k_work_reschedule(&smp_traffic_done_work, K_MSEC(SMP_TRAFFIC_MAX_COMMAND_MS));
    } else {
        if (commands_active > 0) {
            commands_active--;
        }
        k_work_reschedule(&smp_traffic_done_work, K_MSEC(SMP_TRAFFIC_HOLD_MS));
    }
    k_mutex_unlock(&traffic_mutex);

    return MGMT_CB_OK;
}

static void unblock_fs_write_render(void)
{
    if (fs_write_render_blocked) {
        (void)zsw_display_control_unblock_render();
        fs_write_render_blocked = false;
    }
}

static enum mgmt_cb_return fs_access_callback(uint32_t event, enum mgmt_cb_return prev_status,
                                              int32_t *rc, uint16_t *group, bool *abort_more,
                                              void *data, size_t data_size)
{
    ARG_UNUSED(prev_status);
    ARG_UNUSED(rc);
    ARG_UNUSED(group);
    ARG_UNUSED(abort_more);
    ARG_UNUSED(data_size);

    struct fs_mgmt_file_access *access = data;

    if (event == MGMT_EVT_OP_FS_MGMT_FILE_ACCESS) {
        if (access != NULL && access->access == FS_MGMT_FILE_ACCESS_WRITE) {
            if (!fs_write_render_blocked) {
                fs_write_render_blocked = zsw_display_control_block_render() == 0;
            }
            LOG_INF("MCUmgr FS write started (%s), render %s",
                    access->filename ? access->filename : "?",
                    fs_write_render_blocked ? "blocked" : "already blocked");
        }
    } else if (event == MGMT_EVT_OP_FS_MGMT_FILE_ACCESS_DONE) {
        unblock_fs_write_render();
        LOG_INF("MCUmgr FS write finished");
    }

    return MGMT_CB_OK;
}

int zsw_smp_manager_enable(bool auto_disable)
{
    ARG_UNUSED(auto_disable);

    // SMP is always available; treat this as a hint that traffic is coming.
    k_mutex_lock(&traffic_mutex, K_FOREVER);
    last_traffic_ms = k_uptime_get();
    ble_comm_short_interval_request(BLE_COMM_SHORT_INTERVAL_SMP);
    k_work_reschedule(&smp_traffic_done_work, K_MSEC(SMP_ANNOUNCE_SHORT_INTERVAL_MS));
    k_mutex_unlock(&traffic_mutex);
    return 0;
}

int zsw_smp_manager_disable(void)
{
    return 0;
}

bool zsw_smp_manager_is_enabled(void)
{
    return true;
}

void zsw_smp_manager_reset_timeout(void)
{
}

static int zsw_smp_manager_init(void)
{
    mgmt_callback_register(&cmd_callback_entry);
    mgmt_callback_register(&fs_access_callback_entry);

    return 0;
}

SYS_INIT(zsw_smp_manager_init, APPLICATION, 91);

#else /* CONFIG_ARCH_POSIX */

int zsw_smp_manager_enable(bool auto_disable)
{
    ARG_UNUSED(auto_disable);
    LOG_WRN("SMP not available on POSIX");
    return -ENOTSUP;
}

int zsw_smp_manager_disable(void)
{
    LOG_WRN("SMP not available on POSIX");
    return -ENOTSUP;
}

bool zsw_smp_manager_is_enabled(void)
{
    return false;
}

void zsw_smp_manager_reset_timeout(void)
{
}

#endif /* CONFIG_ARCH_POSIX */
