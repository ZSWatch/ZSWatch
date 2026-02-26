/*
 * Remoteproc Virtio Framework
 *
 * Copyright(c) 2018 Xilinx Ltd.
 * Copyright(c) 2011 Texas Instruments, Inc.
 * Copyright(c) 2011 Google, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef REMOTEPROC_VIRTIO_H
#define REMOTEPROC_VIRTIO_H

#include <metal/io.h>
#include <metal/list.h>
#include <openamp/virtio.h>
#include <metal/cache.h>

#if defined __cplusplus
extern "C" {
#endif

/* maximum number of vring descriptors for a vdev limited by 16-bit data type */
#define	RPROC_MAX_VRING_DESC	USHRT_MAX

/* cache invalidation helpers for resource table */
#if defined(VIRTIO_USE_DCACHE)
#define RSC_TABLE_FLUSH(x, s)		metal_cache_flush(x, s)
#define RSC_TABLE_INVALIDATE(x, s)	metal_cache_invalidate(x, s)
#else
#define RSC_TABLE_FLUSH(x, s)		do { } while (0)
#define RSC_TABLE_INVALIDATE(x, s)	do { } while (0)
#endif /* VIRTIO_USE_DCACHE */

/* define vdev notification function user should implement */
typedef int (*rpvdev_notify_func)(void *priv, uint32_t id);

/** @brief Virtio structure for remoteproc instance */
struct remoteproc_virtio {
	/** Pointer to private data */
	void *priv;

	/** Address of vdev resource */
	void *vdev_rsc;

	/** Metal I/O region of vdev_info, can be NULL */
	struct metal_io_region *vdev_rsc_io;

	/** Notification function */
	rpvdev_notify_func notify;

	/** Virtio device */
	struct virtio_device vdev;

	/** List node */
	struct metal_list node;
};

/**
 * @brief Create rproc virtio vdev
 *
 * @param role		VIRTIO_DEV_DRIVER or VIRTIO_DEV_DEVICE
 * @param notifyid	Virtio device notification id
 * @param rsc		Pointer to the virtio device resource
 * @param rsc_io	Pointer to the virtio device resource I/O region
 * @param priv		Pointer to the private data
 * @param notify	vdev and virtqueue notification function
 * @param rst_cb	Reset virtio device callback
 *
 * @return pointer to the created virtio device for success,
 * NULL for failure.
 */
struct virtio_device *
rproc_virtio_create_vdev(unsigned int role, unsigned int notifyid,
			 void *rsc, struct metal_io_region *rsc_io,
			 void *priv,
			 rpvdev_notify_func notify,
			 virtio_dev_reset_cb rst_cb);

/**
 * @brief Remove rproc virtio vdev
 *
 * @param vdev	Pointer to the virtio device
 */
void rproc_virtio_remove_vdev(struct virtio_device *vdev);

/**
 * @brief Initialize rproc virtio vring
 *
 * @param vdev		Pointer to the virtio device
 * @param index		vring index in the virtio device
 * @param notifyid	remoteproc vring notification id
 * @param va		vring virtual address
 * @param io		Pointer to vring I/O region
 * @param num_descs	Number of descriptors
 * @param align		vring alignment
 *
 * @return 0 for success, negative value for failure.
 */
int rproc_virtio_init_vring(struct virtio_device *vdev, unsigned int index,
			    unsigned int notifyid, void *va,
			    struct metal_io_region *io,
			    unsigned int num_descs, unsigned int align);

/**
 * @brief remoteproc virtio is got notified
 *
 * @param vdev		Pointer to the virtio device
 * @param notifyid	Notify id
 *
 * @return 0 for successful, negative value for failure
 */
int rproc_virtio_notified(struct virtio_device *vdev, uint32_t notifyid);

/**
 * @brief Blocking function, waiting for the remote core is ready to start
 * communications.
 *
 * @param vdev	Pointer to the virtio device
 */
void rproc_virtio_wait_remote_ready(struct virtio_device *vdev);

#if defined __cplusplus
}
#endif

#endif /* REMOTEPROC_VIRTIO_H */
