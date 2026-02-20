/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
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

#include "zsw_smp_manager.h"
#include "zsw_xip_manager.h"
#include "ble/ble_comm.h"

#ifndef CONFIG_ARCH_POSIX
#include <zephyr/mgmt/mcumgr/mgmt/callbacks.h>
#include <zephyr/mgmt/mcumgr/grp/img_mgmt/img_mgmt_callbacks.h>
#include <zephyr/mgmt/mcumgr/grp/fs_mgmt/fs_mgmt_callbacks.h>
#include <zephyr/mgmt/mcumgr/transport/smp_bt.h>
#endif

LOG_MODULE_REGISTER(zsw_smp_manager, LOG_LEVEL_INF);

#ifndef CONFIG_ARCH_POSIX

#define SMP_AUTO_DISABLE_TIMEOUT_SEC  180  /* 3 minutes */

static bool smp_enabled;
static bool auto_disable_active;

static void smp_auto_disable_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(smp_auto_disable_work, smp_auto_disable_work_handler);

static void smp_auto_disable_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (!smp_enabled || !auto_disable_active) {
        return;
    }

    LOG_INF("SMP auto-disable: no activity for %d s", SMP_AUTO_DISABLE_TIMEOUT_SEC);

    int rc = smp_bt_unregister();
    if (rc != 0) {
        LOG_ERR("Failed to unregister SMP BT: %d", rc);
    }

    ble_comm_set_default_adv_interval();
    ble_comm_set_default_connection_interval();
    zsw_xip_disable();

    smp_enabled = false;
    auto_disable_active = false;
}

static void reset_auto_disable_timer(void)
{
    if (smp_enabled && auto_disable_active) {
        k_work_reschedule(&smp_auto_disable_work, K_SECONDS(SMP_AUTO_DISABLE_TIMEOUT_SEC));
    }
}

/**
 * @brief MCUmgr callback for IMG management events (DFU activity).
 */
static enum mgmt_cb_return img_mgmt_callback(uint32_t event, enum mgmt_cb_return prev_status,
                                             int32_t *rc, uint16_t *group, bool *abort_more,
                                             void *data, size_t data_size)
{
    ARG_UNUSED(prev_status);
    ARG_UNUSED(rc);
    ARG_UNUSED(group);
    ARG_UNUSED(abort_more);
    ARG_UNUSED(data);
    ARG_UNUSED(data_size);

    /* Reset timeout on any IMG activity */
    reset_auto_disable_timer();

    return MGMT_CB_OK;
}

/**
 * @brief MCUmgr callback for FS management events (file access activity).
 */
static enum mgmt_cb_return fs_mgmt_callback(uint32_t event, enum mgmt_cb_return prev_status,
                                            int32_t *rc, uint16_t *group, bool *abort_more,
                                            void *data, size_t data_size)
{
    ARG_UNUSED(prev_status);
    ARG_UNUSED(rc);
    ARG_UNUSED(group);
    ARG_UNUSED(abort_more);
    ARG_UNUSED(data);
    ARG_UNUSED(data_size);

    /* Reset timeout on any FS activity */
    reset_auto_disable_timer();

    return MGMT_CB_OK;
}

static struct mgmt_callback img_callback = {
    .callback = img_mgmt_callback,
    .event_id = MGMT_EVT_OP_IMG_MGMT_DFU_CHUNK,
};

static struct mgmt_callback fs_callback = {
    .callback = fs_mgmt_callback,
    .event_id = MGMT_EVT_OP_FS_MGMT_FILE_ACCESS,
};

int zsw_smp_manager_enable(bool auto_disable)
{
    if (smp_enabled) {
        LOG_DBG("SMP already enabled");
        /* If re-enabling with auto_disable, just restart timer */
        if (auto_disable) {
            auto_disable_active = true;
            k_work_reschedule(&smp_auto_disable_work, K_SECONDS(SMP_AUTO_DISABLE_TIMEOUT_SEC));
        }
        return 0;
    }

    /* Enable XIP first - MCUmgr code resides in external flash XIP region */
    zsw_xip_enable();

    int rc = smp_bt_register();
    if (rc != 0) {
        LOG_ERR("Failed to register SMP BT: %d", rc);
        zsw_xip_disable();
        return rc;
    }

    /* Optimize BLE parameters for faster transfer */
    ble_comm_set_fast_adv_interval();
    ble_comm_set_short_connection_interval();

    smp_enabled = true;
    auto_disable_active = auto_disable;

    if (auto_disable) {
        k_work_reschedule(&smp_auto_disable_work, K_SECONDS(SMP_AUTO_DISABLE_TIMEOUT_SEC));
        LOG_INF("SMP enabled (auto-disable in %d s)", SMP_AUTO_DISABLE_TIMEOUT_SEC);
    } else {
        LOG_INF("SMP enabled (no auto-disable)");
    }

    return 0;
}

int zsw_smp_manager_disable(void)
{
    if (!smp_enabled) {
        LOG_DBG("SMP already disabled");
        return 0;
    }

    /* Cancel any pending auto-disable */
    k_work_cancel_delayable(&smp_auto_disable_work);

    int rc = smp_bt_unregister();
    if (rc != 0) {
        LOG_ERR("Failed to unregister SMP BT: %d", rc);
        return rc;
    }

    /* Restore default BLE parameters */
    ble_comm_set_default_adv_interval();
    ble_comm_set_default_connection_interval();

    /* Disable XIP after MCUmgr is disabled */
    zsw_xip_disable();

    smp_enabled = false;
    auto_disable_active = false;

    LOG_INF("SMP disabled");
    return 0;
}

bool zsw_smp_manager_is_enabled(void)
{
    return smp_enabled;
}

void zsw_smp_manager_reset_timeout(void)
{
    reset_auto_disable_timer();
}

static int zsw_smp_manager_init(void)
{
    /* Register MCUmgr callbacks for activity detection */
    mgmt_callback_register(&img_callback);
    mgmt_callback_register(&fs_callback);

    /* Ensure SMP BT is disabled at startup */
    smp_enabled = false;
    auto_disable_active = false;

    int rc = smp_bt_unregister();
    if (rc != 0) {
        LOG_WRN("SMP BT already unregistered or failed (init priority): %d", rc);
    }

    return 0;
}

/* Initialize after SMP BT transport (which runs at CONFIG_APPLICATION_INIT_PRIORITY) */
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
    /* No-op on POSIX */
}

#endif /* CONFIG_ARCH_POSIX */
