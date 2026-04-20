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

#include "netcore_check.h"

LOG_MODULE_REGISTER(netcore_check, LOG_LEVEL_INF);

#if defined(CONFIG_SOC_NRF5340_CPUAPP)

#include <hal/nrf_ipc.h>
#include <hal/nrf_reset.h>

/* NCM protocol constants - must match nrf/subsys/net_core_monitor/common.h */
#define NCM_IPC_MEM_CNT_IDX  0
#define NCM_CNT_POS          0UL
#define NCM_CNT_MSK          (0xFFFF << NCM_CNT_POS)
#define NCM_CNT_INIT_VAL     0x0055

/* How long to wait for the net core to boot and initialize */
#define NETCORE_CHECK_BOOT_WAIT_MS 2000
/* How long to wait between counter reads */
#define NETCORE_CHECK_POLL_WAIT_MS 3000

test_result_t netcore_check_run(void)
{
    uint32_t gpmem;
    uint16_t cnt_first;
    uint16_t cnt_second;

    /*
     * GPMEM registers persist across soft/pin resets — only cleared on a
     * power-on reset.  A stale counter from a previous boot could cause a
     * false PASS.  To guarantee we read a freshly-written value:
     *
     *   1. Force the net core off.
     *   2. Clear GPMEM[0] so any old counter value is gone.
     *   3. Release the net core — its NCM init writes CNT_INIT_VAL (0x55)
     *      to GPMEM[0] at SYS_INIT PRE_KERNEL_1, *before* any workqueue
     *      scheduling or BLE controller init can block.
     *   4. Read GPMEM[0] — if >= CNT_INIT_VAL the firmware is present and
     *      the net core booted successfully.
     */

    /* Force net core off */
    nrf_reset_network_force_off(NRF_RESET, true);
    k_sleep(K_MSEC(10));

    /* Clear shared counter register */
    nrf_ipc_gpmem_set(NRF_IPC, NCM_IPC_MEM_CNT_IDX, 0);

    /* Release — net core reboots from its reset vector (B0N → ipc_radio) */
    nrf_reset_network_force_off(NRF_RESET, false);

    /* Wait for net core to boot and run NCM init */
    k_sleep(K_MSEC(NETCORE_CHECK_BOOT_WAIT_MS));

    /* First read: did NCM init write the counter? */
    gpmem = nrf_ipc_gpmem_get(NRF_IPC, NCM_IPC_MEM_CNT_IDX);
    cnt_first = (uint16_t)((gpmem & NCM_CNT_MSK) >> NCM_CNT_POS);

    if (cnt_first < NCM_CNT_INIT_VAL) {
        /* NCM hasn't initialized yet — either no firmware, firmware crashed,
         * or still booting.  Give it more time and retry once.
         */
        k_sleep(K_MSEC(NETCORE_CHECK_POLL_WAIT_MS));
        gpmem = nrf_ipc_gpmem_get(NRF_IPC, NCM_IPC_MEM_CNT_IDX);
        cnt_first = (uint16_t)((gpmem & NCM_CNT_MSK) >> NCM_CNT_POS);

        if (cnt_first < NCM_CNT_INIT_VAL) {
            LOG_ERR("Net core not responding (counter=%u, gpmem=0x%08x)",
                    cnt_first, gpmem);
            return TEST_RESULT_FAILED;
        }
    }

    /*
     * Counter >= CNT_INIT_VAL: the net core actively wrote to GPMEM after
     * we cleared it — firmware is present and booted.
     * Now check if the counter advances (workqueue alive).
     */
    k_sleep(K_MSEC(NETCORE_CHECK_POLL_WAIT_MS));
    gpmem = nrf_ipc_gpmem_get(NRF_IPC, NCM_IPC_MEM_CNT_IDX);
    cnt_second = (uint16_t)((gpmem & NCM_CNT_MSK) >> NCM_CNT_POS);

    if (cnt_second != cnt_first) {
        LOG_INF("Net core alive (counter: %u -> %u)", cnt_first, cnt_second);
        return TEST_RESULT_PASSED;
    }

    /* Counter didn't advance but was freshly written after GPMEM clear.
     * BLE controller init may still be blocking the workqueue — the net
     * core firmware is present and booted regardless.
     */
    LOG_INF("Net core booted (counter=%u, workqueue blocked)", cnt_first);
    return TEST_RESULT_PASSED;
}

#else /* !CONFIG_SOC_NRF5340_CPUAPP */

test_result_t netcore_check_run(void)
{
    /* Net core check not available on this platform */
    return TEST_RESULT_PASSED;
}

#endif
