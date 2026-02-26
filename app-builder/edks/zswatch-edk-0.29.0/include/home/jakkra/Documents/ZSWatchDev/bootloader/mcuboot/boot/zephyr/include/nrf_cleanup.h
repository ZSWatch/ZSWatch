/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef H_NRF_CLEANUP_
#define H_NRF_CLEANUP_

/**
 * Perform cleanup on some peripheral resources used by MCUBoot prior chainload
 * the application.
 *
 * This function disables all RTC instances and UARTE instances.
 * It Disables their interrupts signals as well.
 */
void nrf_cleanup_peripheral(void);

/**
 * Perform cleanup of non-secure RAM that may have been used by MCUBoot.
 */
void nrf_cleanup_ns_ram(void);

/**
 * Crypto key storage housekeeping. Intended to clean up key objects from
 * crypto backend and apply key policies that should take effect after
 * MCUboot no longer needs access to keys.
 */
#if defined(CONFIG_BOOT_SIGNATURE_USING_KMU)
extern void nrf_crypto_keys_housekeeping(void);
#else
#define nrf_crypto_keys_housekeeping() do {} while (0)
#endif

#endif
