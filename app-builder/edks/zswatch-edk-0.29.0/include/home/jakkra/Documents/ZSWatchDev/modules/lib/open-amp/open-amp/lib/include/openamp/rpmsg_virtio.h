/*
 * rpmsg based on virtio
 *
 * Copyright (C) 2018 Linaro, Inc.
 *
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RPMSG_VIRTIO_H_
#define _RPMSG_VIRTIO_H_

#include <metal/io.h>
#include <metal/mutex.h>
#include <metal/cache.h>
#include <openamp/rpmsg.h>
#include <openamp/virtio.h>

#if defined __cplusplus
extern "C" {
#endif

/* Configurable parameters */
#ifndef RPMSG_BUFFER_SIZE
#define RPMSG_BUFFER_SIZE	(512)
#endif

/* The feature bitmap for virtio rpmsg */
#define VIRTIO_RPMSG_F_NS	0 /* RP supports name service notifications */

#if defined(VIRTIO_USE_DCACHE)
#define BUFFER_FLUSH(x, s)		metal_cache_flush(x, s)
#define BUFFER_INVALIDATE(x, s)		metal_cache_invalidate(x, s)
#else
#define BUFFER_FLUSH(x, s)		do { } while (0)
#define BUFFER_INVALIDATE(x, s)		do { } while (0)
#endif /* VIRTIO_USE_DCACHE */

/* Callback handler for rpmsg virtio service */
typedef int (*rpmsg_virtio_notify_wait_cb)(struct rpmsg_device *rdev, uint32_t id);

/** @brief Shared memory pool used for RPMsg buffers */
struct rpmsg_virtio_shm_pool {
	/** Base address of the memory pool */
	void *base;

	/** Available memory size */
	size_t avail;

	/** Total pool size */
	size_t size;
};

/**
 * @brief Configuration of RPMsg device based on virtio
 *
 * This structure is used by the RPMsg virtio host to configure the virtiio
 * layer.
 */
struct rpmsg_virtio_config {
	/** The size of the buffer used to send data from host to remote */
	uint32_t h2r_buf_size;

	/** The size of the buffer used to send data from remote to host */
	uint32_t r2h_buf_size;

	/** The flag for splitting shared memory pool to TX and RX */
	bool split_shpool;
};

/** @brief Representation of a RPMsg device based on virtio */
struct rpmsg_virtio_device {
	/** RPMsg device */
	struct rpmsg_device rdev;

	/** Structure containing virtio configuration */
	struct rpmsg_virtio_config config;

	/** Pointer to the virtio device */
	struct virtio_device *vdev;

	/** Pointer to receive virtqueue */
	struct virtqueue *rvq;

	/** Pointer to send virtqueue */
	struct virtqueue *svq;

	/** Pointer to the shared buffer I/O region */
	struct metal_io_region *shbuf_io;

	/** Pointer to the shared buffers pool */
	struct rpmsg_virtio_shm_pool *shpool;

	/**
	 * RPMsg buffer reclaimer that contains buffers released by the
	 * \ref rpmsg_virtio_release_tx_buffer function
	 */
	struct metal_list reclaimer;

	/**
	 * Callback handler for rpmsg virtio service, called when service
	 * can't get tx buffer
	 */
	rpmsg_virtio_notify_wait_cb notify_wait_cb;
};

#define RPMSG_REMOTE	VIRTIO_DEV_DEVICE
#define RPMSG_HOST	VIRTIO_DEV_DRIVER

/**
 * @brief Set the virtio callback to manage the wait for TX buffer availability.
 *
 * @param rvdev			Pointer to rpmsg virtio device.
 * @param notify_wait_cb	Callback handler to wait buffer notification.
 */
static inline void rpmsg_virtio_set_wait_cb(struct rpmsg_virtio_device *rvdev,
					    rpmsg_virtio_notify_wait_cb notify_wait_cb)
{
	rvdev->notify_wait_cb = notify_wait_cb;
}

/**
 * @brief Get rpmsg virtio device role.
 *
 * @param rvdev	Pointer to rpmsg virtio device.
 *
 * @return VIRTIO_DEV_DEVICE or VIRTIO_DEV_DRIVER
 */
static inline unsigned int
rpmsg_virtio_get_role(struct rpmsg_virtio_device *rvdev)
{
	return rvdev->vdev->role;
}

/**
 * @brief Set rpmsg virtio device status.
 *
 * Deprecated: Use virtio_set_status() instead
 *
 * @param rvdev		Pointer to rpmsg virtio device.
 * @param status	Value to be set as rpmsg virtio device status.
 */
__deprecated
static inline void rpmsg_virtio_set_status(struct rpmsg_virtio_device *rvdev,
					   uint8_t status)
{
	rvdev->vdev->func->set_status(rvdev->vdev, status);
}

/**
 * @brief Retrieve rpmsg virtio device status.
 *
 * Deprecated: Use virtio_get_status() instead
 *
 * @param rvdev	Pointer to rpmsg virtio device.
 *
 * @return The rpmsg virtio device status.
 */
__deprecated
static inline uint8_t rpmsg_virtio_get_status(struct rpmsg_virtio_device *rvdev)
{
	return rvdev->vdev->func->get_status(rvdev->vdev);
}

/**
 * @brief Get the rpmsg virtio device features.
 *
 * Deprecated: Use virtio_get_features() instead
 *
 * @param rvdev	Pointer to the rpmsg virtio device.
 *
 * @return The features supported by both the rpmsg driver and rpmsg device.
 */
__deprecated
static inline uint32_t
rpmsg_virtio_get_features(struct rpmsg_virtio_device *rvdev)
{
	return rvdev->vdev->func->get_features(rvdev->vdev);
}

/**
 * @brief Retrieve configuration data from the rpmsg virtio device.
 *
 * Deprecated: Use virtio_read_config() instead
 *
 * @param rvdev		Pointer to the rpmsg virtio device.
 * @param offset	Offset of the data within the configuration area.
 * @param dst		Address of the buffer that will hold the data.
 * @param length	Length of the data to be retrieved.
 */
__deprecated
static inline void
rpmsg_virtio_read_config(struct rpmsg_virtio_device *rvdev,
			 uint32_t offset, void *dst, int length)
{
	rvdev->vdev->func->read_config(rvdev->vdev, offset, dst, length);
}

/**
 * @brief Write configuration data to the rpmsg virtio device.
 *
 * Deprecated: Use virtio_write_config() instead
 *
 * @param rvdev		Pointer to the rpmsg virtio device.
 * @param offset	Offset of the data within the configuration area.
 * @param src		Address of the buffer that holds the data to write.
 * @param length	Length of the data to be written.
 *
 * @return 0 on success, otherwise error code.
 */
__deprecated
static inline void
rpmsg_virtio_write_config(struct rpmsg_virtio_device *rvdev,
			 uint32_t offset, void *src, int length)
{
	rvdev->vdev->func->write_config(rvdev->vdev, offset, src, length);
}

/**
 * @brief Create the rpmsg virtio device virtqueue.
 *
 * Deprecated: Use virtio_create_virtqueues() instead
 *
 * @param rvdev		Pointer to the rpmsg virtio device.
 * @param flags		Create flag.
 * @param nvqs		The virtqueue number.
 * @param names		Virtqueue names.
 * @param callbacks	Virtqueue callback functions.
 *
 * @return 0 on success, otherwise error code.
 */
__deprecated
static inline int
rpmsg_virtio_create_virtqueues(struct rpmsg_virtio_device *rvdev,
			       int flags, unsigned int nvqs,
			       const char *names[],
			       vq_callback *callbacks)
{
	return virtio_create_virtqueues(rvdev->vdev, flags, nvqs, names,
					callbacks, NULL);
}

/**
 * @brief Delete the virtqueues created in rpmsg_virtio_create_virtqueues()
 *
 * Deprecated: Use virtio_delete_virtqueues() instead
 *
 * @param rvdev	Pointer to the rpmsg virtio device
 */
__deprecated
static inline void
rpmsg_virtio_delete_virtqueues(struct rpmsg_virtio_device *rvdev)
{
	virtio_delete_virtqueues(rvdev->vdev);
}

/**
 * @brief Get rpmsg virtio buffer size
 *
 * @param rdev	Pointer to the rpmsg device
 *
 * @return Next available buffer size for text, negative value for failure
 */
int rpmsg_virtio_get_tx_buffer_size(struct rpmsg_device *rdev);

/**
 * @brief  Get rpmsg virtio Rx buffer size
 *
 * @param rdev	Pointer to the rpmsg device
 *
 * @return Next available buffer size for text, negative value for failure
 */
int rpmsg_virtio_get_rx_buffer_size(struct rpmsg_device *rdev);

/**
 * @brief Get rpmsg virtio Tx buffer size
 *
 * This function is same as rpmsg_virtio_get_tx_buffer_size(), keep it here
 * to maintain the forward compatibility.
 *
 * @param rdev	Pointer to the rpmsg device.
 *
 * @return Next available buffer size for text, negative value for failure.
 */
static inline int rpmsg_virtio_get_buffer_size(struct rpmsg_device *rdev)
{
	return rpmsg_virtio_get_tx_buffer_size(rdev);
}

/**
 * @brief Initialize rpmsg virtio device
 *
 * Host side:
 * Initialize RPMsg virtio queues and shared buffers, the address of shm can be
 * ANY. In this case, function will get shared memory from system shared memory
 * pools. If the vdev has the RPMsg name service feature, this API will create
 * a name service endpoint.
 *
 * Remote side:
 * This API will not return until the driver ready is set by the host side.
 *
 * @param rvdev		Pointer to the rpmsg virtio device
 * @param vdev		Pointer to the virtio device
 * @param ns_bind_cb	Callback handler for name service announcement without
 *                      local endpoints waiting to bind.
 * @param shm_io	Pointer to the share memory I/O region.
 * @param shpool	Pointer to shared memory pool.
 *			rpmsg_virtio_init_shm_pool has to be called first to
 *			fill this structure.
 *
 * @return Status of function execution
 */
int rpmsg_init_vdev(struct rpmsg_virtio_device *rvdev,
		    struct virtio_device *vdev,
		    rpmsg_ns_bind_cb ns_bind_cb,
		    struct metal_io_region *shm_io,
		    struct rpmsg_virtio_shm_pool *shpool);

/**
 * @brief Initialize rpmsg virtio device with config
 *
 * Host side:
 * Initialize RPMsg virtio queues and shared buffers, the address of shm can be
 * ANY. In this case, function will get shared memory from system shared memory
 * pools. If the vdev has the RPMsg name service feature, this API will create
 * a name service endpoint.
 * Sizes of virtio data buffers used by the initialized RPMsg instance are set
 * to values read from the passed configuration structure.
 *
 * Remote side:
 * This API will not return until the driver ready is set by the host side.
 * Sizes of virtio data buffers are set by the host side. Values passed in the
 * configuration structure have no effect.
 *
 * @param rvdev		Pointer to the rpmsg virtio device
 * @param vdev		Pointer to the virtio device
 * @param ns_bind_cb	Callback handler for name service announcement without
 *                      local endpoints waiting to bind.
 * @param shm_io	Pointer to the share memory I/O region.
 * @param shpool	Pointer to shared memory pool array.
 *			If the config->split_shpool is turn on, the array will
 *			contain two elements, the shpool of txshpool and
 *			rxshpool, Otherwise, the array has only one element,
 *			and txshpool rxshpool shares a shpool.
 *			And rpmsg_virtio_init_shm_pool has to be called first
 *			to fill each shpool in this array.
 * @param config	Pointer to configuration structure
 *
 * @return Status of function execution
 */
int rpmsg_init_vdev_with_config(struct rpmsg_virtio_device *rvdev,
				struct virtio_device *vdev,
				rpmsg_ns_bind_cb ns_bind_cb,
				struct metal_io_region *shm_io,
				struct rpmsg_virtio_shm_pool *shpool,
				const struct rpmsg_virtio_config *config);

/**
 * @brief Deinitialize rpmsg virtio device
 *
 * @param rvdev	Pointer to the rpmsg virtio device
 */
void rpmsg_deinit_vdev(struct rpmsg_virtio_device *rvdev);

/**
 * @brief Initialize default shared buffers pool
 *
 * RPMsg virtio has default shared buffers pool implementation.
 * The memory assigned to this pool will be dedicated to the RPMsg
 * virtio. This function has to be called before calling rpmsg_init_vdev,
 * to initialize the rpmsg_virtio_shm_pool structure.
 *
 * @param shpool	Pointer to the shared buffers pool structure
 * @param shbuf		Pointer to the beginning of shared buffers
 * @param size		Shared buffers total size
 */
void rpmsg_virtio_init_shm_pool(struct rpmsg_virtio_shm_pool *shpool,
				void *shbuf, size_t size);

/**
 * @brief Get RPMsg device from RPMsg virtio device
 *
 * @param rvdev	Pointer to RPMsg virtio device
 *
 * @return RPMsg device pointed by RPMsg virtio device
 */
static inline struct rpmsg_device *
rpmsg_virtio_get_rpmsg_device(struct rpmsg_virtio_device *rvdev)
{
	if (!rvdev)
		return NULL;

	return &rvdev->rdev;
}

/**
 * @brief Get buffer in the shared memory pool
 *
 * RPMsg virtio has default shared buffers pool implementation.
 * The memory assigned to this pool will be dedicated to the RPMsg
 * virtio. If you prefer to have other shared buffers allocation,
 * you can implement your rpmsg_virtio_shm_pool_get_buffer function.
 *
 * @param shpool	Pointer to the shared buffers pool
 * @param size		Shared buffers total size
 *
 * @return Buffer pointer if free buffer is available, NULL otherwise.
 */
metal_weak void *
rpmsg_virtio_shm_pool_get_buffer(struct rpmsg_virtio_shm_pool *shpool,
				 size_t size);

#if defined __cplusplus
}
#endif

#endif	/* _RPMSG_VIRTIO_H_ */
