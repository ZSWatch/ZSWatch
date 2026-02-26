/*
 * Copyright (c) 2022 Wind River Systems, Inc.
 * Based on Virtio PCI driver by Anthony Liguori, copyright IBM Corp. 2007
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef OPENAMP_VIRTIO_MMIO_H
#define OPENAMP_VIRTIO_MMIO_H

#include <metal/device.h>
#include <openamp/virtio.h>
#include <openamp/virtqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Enable support for legacy devices */
#define VIRTIO_MMIO_LEGACY

/* Control registers */

/* Magic value ("virt" string) - Read Only */
#define VIRTIO_MMIO_MAGIC_VALUE		0x000

#define VIRTIO_MMIO_MAGIC_VALUE_STRING ('v' | ('i' << 8) | ('r' << 16) | ('t' << 24))

/* Virtio device version - Read Only */
#define VIRTIO_MMIO_VERSION		0x004

/* Virtio device ID - Read Only */
#define VIRTIO_MMIO_DEVICE_ID		0x008

/* Virtio vendor ID - Read Only */
#define VIRTIO_MMIO_VENDOR_ID		0x00c

/*
 * Bitmask of the features supported by the device (host)
 * (32 bits per set) - Read Only
 */
#define VIRTIO_MMIO_DEVICE_FEATURES	0x010

/* Device (host) features set selector - Write Only */
#define VIRTIO_MMIO_DEVICE_FEATURES_SEL	0x014

/*
 * Bitmask of features activated by the driver (guest)
 * (32 bits per set) - Write Only
 */
#define VIRTIO_MMIO_DRIVER_FEATURES	0x020

/* Activated features set selector - Write Only */
#define VIRTIO_MMIO_DRIVER_FEATURES_SEL	0x024

#ifndef VIRTIO_MMIO_NO_LEGACY /* LEGACY DEVICES ONLY! */
/* Guest's memory page size in bytes - Write Only */
#define VIRTIO_MMIO_GUEST_PAGE_SIZE	0x028
#endif

/* Queue selector - Write Only */
#define VIRTIO_MMIO_QUEUE_SEL		0x030

/* Maximum size of the currently selected queue - Read Only */
#define VIRTIO_MMIO_QUEUE_NUM_MAX	0x034

/* Queue size for the currently selected queue - Write Only */
#define VIRTIO_MMIO_QUEUE_NUM		0x038

#ifdef VIRTIO_MMIO_LEGACY
/* Used Ring alignment for the currently selected queue - Write Only */
#define VIRTIO_MMIO_QUEUE_ALIGN		0x03c
/* Guest's PFN for the currently selected queue - Read Write */
#define VIRTIO_MMIO_QUEUE_PFN		0x040
#endif

/* Ready bit for the currently selected queue - Read Write */
#define VIRTIO_MMIO_QUEUE_READY		0x044

/* Queue notifier - Write Only */
#define VIRTIO_MMIO_QUEUE_NOTIFY	0x050

/* Interrupt status - Read Only */
#define VIRTIO_MMIO_INTERRUPT_STATUS	0x060

/* Interrupt acknowledge - Write Only */
#define VIRTIO_MMIO_INTERRUPT_ACK	0x064

/* Device status register - Read Write */
#define VIRTIO_MMIO_STATUS		0x070

/* Selected queue's Descriptor Table address, 64 bits in two halves */
#define VIRTIO_MMIO_QUEUE_DESC_LOW	0x080
#define VIRTIO_MMIO_QUEUE_DESC_HIGH	0x084

/* Selected queue's Available Ring address, 64 bits in two halves */
#define VIRTIO_MMIO_QUEUE_AVAIL_LOW	0x090
#define VIRTIO_MMIO_QUEUE_AVAIL_HIGH	0x094

/* Selected queue's Used Ring address, 64 bits in two halves */
#define VIRTIO_MMIO_QUEUE_USED_LOW	0x0a0
#define VIRTIO_MMIO_QUEUE_USED_HIGH	0x0a4

/* Shared memory region id */
#define VIRTIO_MMIO_SHM_SEL             0x0ac

/* Shared memory region length, 64 bits in two halves */
#define VIRTIO_MMIO_SHM_LEN_LOW         0x0b0
#define VIRTIO_MMIO_SHM_LEN_HIGH        0x0b4

/* Shared memory region base address, 64 bits in two halves */
#define VIRTIO_MMIO_SHM_BASE_LOW        0x0b8
#define VIRTIO_MMIO_SHM_BASE_HIGH       0x0bc

/* Configuration atomicity value */
#define VIRTIO_MMIO_CONFIG_GENERATION	0x0fc

/*
 * The config space is defined by each driver as
 * the per-driver configuration space - Read Write
 */
#define VIRTIO_MMIO_CONFIG		0x100

/* Interrupt flags (re: interrupt status & acknowledge registers) */
#define VIRTIO_MMIO_INT_VRING		(1 << 0)
#define VIRTIO_MMIO_INT_CONFIG		(1 << 1)

/* Data buffer size for preallocated buffers before vring */
#define VIRTIO_MMIO_MAX_DATA_SIZE 128

/** @brief VIRTIO MMIO memory area */
struct virtio_mmio_dev_mem {
	/** Memory region physical address */
	void *base;

	/** Memory region size */
	size_t size;
};

/** @brief A VIRTIO MMIO device */
struct virtio_mmio_device {
	/** Base virtio device structure */
	struct virtio_device vdev;

	/** Device configuration space metal_io_region */
	struct metal_io_region cfg_io;

	/** Pre-shared memory space metal_io_region */
	struct metal_io_region shm_io;

	/** VIRTIO device configuration space */
	struct virtio_mmio_dev_mem cfg_mem;

	/** VIRTIO device pre-shared memory */
	struct virtio_mmio_dev_mem shm_mem;

	/** VIRTIO_DEV_DRIVER or VIRTIO_DEV_DEVICE */
	unsigned int device_mode;

	/** Interrupt number */
	unsigned int irq;

	/** Custom user data */
	void *user_data;
};

/**
 * @brief Register a VIRTIO device with the VIRTIO stack.
 *
 * @param vdev		Pointer to device structure.
 * @param vq_num	Number of virtqueues the device uses.
 * @param vqs		Array of pointers to vthe virtqueues used by the device.
 */
void virtio_mmio_register_device(struct virtio_device *vdev, int vq_num, struct virtqueue **vqs);

/**
 * @brief Setup a virtqueue structure.
 *
 * @param vdev		Pointer to device structure.
 * @param idx		Index of the virtqueue.
 * @param vq		Pointer to virtqueue structure.
 * @param cb		Pointer to virtqueue callback. Can be NULL.
 * @param cb_arg	Argument for the virtqueue callback.
 * @param vq_name	Name of the virtqueue.
 *
 * @return pointer to virtqueue structure.
 */
struct virtqueue *virtio_mmio_setup_virtqueue(struct virtio_device *vdev,
					      unsigned int idx,
					      struct virtqueue *vq,
					      void (*cb)(void *),
					      void *cb_arg,
					      const char *vq_name);

/**
 * @brief VIRTIO MMIO device initialization.
 *
 * @param vmdev		Pointer to virtio_mmio_device structure.
 * @param virt_mem_ptr	Guest virtio (shared) memory base address (virtual).
 * @param cfg_mem_ptr	Virtio device configuration memory base address (virtual).
 * @param user_data	Pointer to custom user data.
 *
 * @return int 0 for success.
 */
int virtio_mmio_device_init(struct virtio_mmio_device *vmdev, uintptr_t virt_mem_ptr,
			    uintptr_t cfg_mem_ptr, void *user_data);

/**
 * @brief VIRTIO MMIO interrupt service routine.
 *
 * @param vdev Pointer to virtio_device structure.
 */
void virtio_mmio_isr(struct virtio_device *vdev);

#ifdef __cplusplus
}
#endif

#endif /* OPENAMP_VIRTIO_MMIO_H */
