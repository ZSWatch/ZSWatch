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

#include <zsw_cpu_freq.h>
#include <zephyr/kernel.h>
#ifndef CONFIG_ARCH_POSIX
#include <nrfx_clock.h>

//#define CPU_CLOCK_PROFILING

void zsw_cpu_set_freq(zsw_cpu_freq_t freq, bool wait)
{
#ifdef CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
    int ret;
#ifdef CPU_CLOCK_PROFILING
    uint32_t start_time;
    uint32_t stop_time;
    uint32_t cycles_spent;
    uint32_t nanoseconds_spent;
    start_time = k_cycle_get_32();
#endif

    // This configures the clock for CPU APP Core
    ret = nrfx_clock_divider_set(NRF_CLOCK_DOMAIN_HFCLK,
                                 freq == ZSW_CPU_FREQ_FAST ? NRF_CLOCK_HFCLK_DIV_1 : NRF_CLOCK_HFCLK_DIV_2);

    ret -= NRFX_ERROR_BASE_NUM;
    if (ret) {
        return;
    }

    // Wait if requested for the clock to start/stop
    if (freq == ZSW_CPU_FREQ_FAST && wait) {
        nrfx_clock_start(NRF_CLOCK_DOMAIN_HFCLK);
        while (!nrfx_clock_is_running(NRF_CLOCK_DOMAIN_HFCLK, NULL)) {
        }
    } else if (freq == ZSW_CPU_FREQ_DEFAULT && wait) {
        nrfx_clock_stop(NRF_CLOCK_DOMAIN_HFCLK);
        while (nrfx_clock_is_running(NRF_CLOCK_DOMAIN_HFCLK, NULL)) {
        }
    }
#ifdef CPU_CLOCK_PROFILING
    stop_time = k_cycle_get_32();
    cycles_spent = stop_time - start_time;
    nanoseconds_spent = k_cyc_to_ns_ceil32(cycles_spent);
    if (freq == ZSW_CPU_FREQ_FAST) {
        printk("Start HFCLK took: %dns\n", nanoseconds_spent);
    } else {
        printk("Stop HFCLK took: %dns\n", nanoseconds_spent);
    }
#endif
#endif
}

zsw_cpu_freq_t zsw_cpu_get_freq(void)
{
#ifdef CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
    if (nrf_clock_hfclk_div_get(NRF_CLOCK) == NRF_CLOCK_HFCLK_DIV_1) {
        return ZSW_CPU_FREQ_FAST;
    } else {
        return ZSW_CPU_FREQ_DEFAULT;
    }
#else
    return ZSW_CPU_FREQ_DEFAULT;
#endif
}
#else
void zsw_cpu_set_freq(zsw_cpu_freq_t freq, bool wait) {}

zsw_cpu_freq_t zsw_cpu_get_freq(void)
{
    return ZSW_CPU_FREQ_DEFAULT;
}
#endif
