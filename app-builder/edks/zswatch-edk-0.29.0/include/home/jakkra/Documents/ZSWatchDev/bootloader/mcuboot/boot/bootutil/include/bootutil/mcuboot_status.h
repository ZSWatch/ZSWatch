/*
 *  Copyright (c) 2022, Laird Connectivity
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_MCUBOOT_STATUS_
#define H_MCUBOOT_STATUS_

/* Enumeration representing the states that MCUboot can be in */
typedef enum
{
	MCUBOOT_STATUS_STARTUP = 0,
	MCUBOOT_STATUS_UPGRADING,
	MCUBOOT_STATUS_BOOTABLE_IMAGE_FOUND,
	MCUBOOT_STATUS_NO_BOOTABLE_IMAGE_FOUND,
	MCUBOOT_STATUS_BOOT_FAILED,
	MCUBOOT_STATUS_USB_DFU_WAITING,
	MCUBOOT_STATUS_USB_DFU_ENTERED,
	MCUBOOT_STATUS_USB_DFU_TIMED_OUT,
	MCUBOOT_STATUS_SERIAL_DFU_ENTERED,
} mcuboot_status_type_t;

#if defined(CONFIG_MCUBOOT_ACTION_HOOKS)
extern void mcuboot_status_change(mcuboot_status_type_t status);
#else
#define mcuboot_status_change(_status) do {} while (0)
#endif

#endif /* H_MCUBOOT_STATUS_ */
