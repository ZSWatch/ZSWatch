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

/*
 * The QSPI driver puts the external flash in 4-byte address mode (EN4B) when
 * the flash node has address-size-32. The flash keeps that mode over a soft
 * reset, but MCUboot builds from before 32-bit addressing talk 24-bit to it.
 * They then fail to validate the image in external flash and stop in serial
 * recovery. Leave 4-byte mode right before every software reset; a power
 * cycle resets the flash on its own.
 *
 * sys_arch_reboot() is wrapped at link time (see CMakeLists.txt), so this
 * covers sys_reboot() from mcumgr, the shell, fatal errors and the UI.
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <hal/nrf_clock.h>
#include <nrfx_qspi.h>

#define QSPI_FLASH_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(nordic_qspi_nor)
#define SPI_NOR_CMD_EX4B 0xE9

void __real_sys_arch_reboot(int type);

void __wrap_sys_arch_reboot(int type)
{
    if (DT_PROP(QSPI_FLASH_NODE, address_size_32)) {
        /* Code in external flash must not run once the mode has changed. */
        (void)irq_lock();

        /* Not initialized means the driver is suspended; nothing to send then. */
        if (nrfx_qspi_init_check()) {
            /* The QSPI needs PCLK192M at 96 or 192 MHz while it is active. */
            nrf_clock_hfclk192m_div_set(NRF_CLOCK, NRF_CLOCK_HFCLK_DIV_2);
            k_busy_wait(7);

            nrf_qspi_cinstr_conf_t cmd = {
                .opcode = SPI_NOR_CMD_EX4B,
                .length = NRF_QSPI_CINSTR_LEN_1B,
                .io2_level = true,
                .io3_level = true,
            };
            /* Polls for completion, so it works with interrupts locked. */
            (void)nrfx_qspi_cinstr_xfer(&cmd, NULL, NULL);
        }
    }

    __real_sys_arch_reboot(type);
}
