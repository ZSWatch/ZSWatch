/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * $FreeBSD$
 */

#ifndef _VIRTIO_H_
#define _VIRTIO_H_

#include <openamp/virtqueue.h>
#include <metal/errno.h>
#include <metal/spinlock.h>

#if defined __cplusplus
extern "C" {
#endif

/* VirtIO device IDs. */
#define VIRTIO_ID_NETWORK        1UL
#define VIRTIO_ID_BLOCK          2UL
#define VIRTIO_ID_CONSOLE        3UL
#define VIRTIO_ID_ENTROPY        4UL
#define VIRTIO_ID_BALLOON        5UL
#define VIRTIO_ID_IOMEMORY       6UL
#define VIRTIO_ID_RPMSG          7UL /* remote processor messaging */
#define VIRTIO_ID_SCSI           8UL
#define VIRTIO_ID_9P             9UL
#define VIRTIO_ID_MAC80211_WLAN  10UL
#define VIRTIO_ID_RPROC_SERIAL   11UL
#define VIRTIO_ID_CAIF           12UL
#define VIRTIO_ID_MEMORY_BALLOON 13UL
#define VIRTIO_ID_GPU            16UL
#define VIRTIO_ID_CLOCK          17UL
#define VIRTIO_ID_INPUT          18UL
#define VIRTIO_ID_VSOCK          19UL
#define VIRTIO_ID_CRYPTO         20UL
#define VIRTIO_ID_SIGNAL_DIST    21UL
#define VIRTIO_ID_PSTORE         22UL
#define VIRTIO_ID_IOMMU          23UL
#define VIRTIO_ID_MEM            24UL
#define VIRTIO_ID_SOUND          25UL
#define VIRTIO_ID_FS             26UL
#define VIRTIO_ID_PMEM           27UL
#define VIRTIO_ID_RPMB           28UL
#define VIRTIO_ID_MAC80211_HWSIM 29UL
#define VIRTIO_ID_VIDEO_ENCODER  30UL
#define VIRTIO_ID_VIDEO_DECODER  31UL
#define VIRTIO_ID_SCMI           32UL
#define VIRTIO_ID_NITRO_SEC_MOD  33UL
#define VIRTIO_ID_I2C_ADAPTER    34UL
#define VIRTIO_ID_WATCHDOG       35UL
#define VIRTIO_ID_CAN            36UL
#define VIRTIO_ID_PARAM_SERV     38UL
#define VIRTIO_ID_AUDIO_POLICY   39UL
#define VIRTIO_ID_BT             40UL
#define VIRTIO_ID_GPIO           41UL
#define VIRTIO_ID_RDMA           42UL
#define VIRTIO_DEV_ANY_ID        -1UL

/* Status byte for guest to report progress. */
#define VIRTIO_CONFIG_STATUS_RESET       0x00
#define VIRTIO_CONFIG_STATUS_ACK         0x01
#define VIRTIO_CONFIG_STATUS_DRIVER      0x02
#define VIRTIO_CONFIG_STATUS_DRIVER_OK   0x04
#define VIRTIO_CONFIG_FEATURES_OK        0x08
#define VIRTIO_CONFIG_STATUS_NEEDS_RESET 0x40
#define VIRTIO_CONFIG_STATUS_FAILED      0x80

/* Virtio device role */
#define VIRTIO_DEV_DRIVER	0UL
#define VIRTIO_DEV_DEVICE	1UL

#ifdef VIRTIO_DRIVER_ONLY
#warning "VIRTIO_DRIVER_ONLY is deprecated, please use VIRTIO_DEVICE_SUPPORT=0"
#define VIRTIO_DRIVER_SUPPORT 1
#define VIRTIO_DEVICE_SUPPORT 0
#endif /* VIRTIO_DRIVER_ONLY */

#ifdef VIRTIO_DEVICE_ONLY
#warning "VIRTIO_DEVICE_ONLY is deprecated, please use VIRTIO_DRIVER_SUPPORT=0"
#define VIRTIO_DRIVER_SUPPORT 0
#define VIRTIO_DEVICE_SUPPORT 1
#endif /* VIRTIO_DEVICE_ONLY */

#define VIRTIO_ENABLED(option) (option == 1)

#ifdef VIRTIO_DRIVER_SUPPORT
#define VIRTIO_ROLE_IS_DRIVER(vdev) \
	(VIRTIO_ENABLED(VIRTIO_DRIVER_SUPPORT) && ((vdev)->role) == VIRTIO_DEV_DRIVER)
#else
/* Default definition without code size optimization */
#define VIRTIO_ROLE_IS_DRIVER(vdev) ((vdev)->role == VIRTIO_DEV_DRIVER)
#endif

#ifdef VIRTIO_DEVICE_SUPPORT
#define VIRTIO_ROLE_IS_DEVICE(vdev) \
	(VIRTIO_ENABLED(VIRTIO_DEVICE_SUPPORT) && ((vdev)->role) == VIRTIO_DEV_DEVICE)
#else
/* Default definition without code size optimization */
#define VIRTIO_ROLE_IS_DEVICE(vdev) ((vdev)->role == VIRTIO_DEV_DEVICE)
#endif

/** @brief Virtio device identifier. */
struct virtio_device_id {
	/** Virtio subsystem device ID. */
	uint32_t device;

	/** Virtio subsystem vendor ID. */
	uint32_t vendor;

	/** Virtio subsystem device version. */
	uint32_t version;
};

/*
 * Generate interrupt when the virtqueue ring is
 * completely used, even if we've suppressed them.
 */
#define VIRTIO_F_NOTIFY_ON_EMPTY (1 << 24)

/*
 * This feature indicates that the device accepts arbitrary
 * descriptor layouts.
 */
#define VIRTIO_F_ANY_LAYOUT (1 << 27)

/*
 * The guest should never negotiate this feature; it
 * is used to detect faulty drivers.
 */
#define VIRTIO_F_BAD_FEATURE (1 << 30)

/*
 * Some VirtIO feature bits (currently bits 28 through 31) are
 * reserved for the transport being used (eg. virtio_ring), the
 * rest are per-device feature bits.
 */
#define VIRTIO_TRANSPORT_F_START      28
#define VIRTIO_TRANSPORT_F_END        32

#ifdef VIRTIO_DEBUG
#include <metal/log.h>

#define VIRTIO_ASSERT(_exp, _msg) do { \
		int exp = (_exp); \
		if (!(exp)) { \
			metal_log(METAL_LOG_EMERGENCY, \
				  "FATAL: %s - " _msg, __func__); \
			metal_assert(exp); \
		} \
	} while (0)
#else
#define VIRTIO_ASSERT(_exp, _msg) metal_assert(_exp)
#endif /* VIRTIO_DEBUG */

#define VIRTIO_MMIO_VRING_ALIGNMENT           4096

typedef void (*virtio_dev_reset_cb)(struct virtio_device *vdev);

struct virtio_dispatch;
struct virtio_memory_ops;

/** @brief Device features. */
struct virtio_feature_desc {
	/** Unique feature ID, defined in the virtio specification. */
	uint32_t vfd_val;

	/** Name of the feature (for debug). */
	const char *vfd_str;
};

/** @brief Virtio vring data structure */
struct virtio_vring_info {
	/** Virtio queue */
	struct virtqueue *vq;

	/** Vring alloc info */
	struct vring_alloc_info info;

	/** Vring notify id */
	uint32_t notifyid;

	/** Metal I/O region of the vring memory, can be NULL */
	struct metal_io_region *io;
};

/** @brief Structure definition for virtio devices for use by the applications/drivers */
struct virtio_device {
	/** Unique position on the virtio bus */
	uint32_t notifyid;

	/** The device type identification used to match it with a driver */
	struct virtio_device_id id;

	/** The features supported by both ends. */
	uint64_t features;

	/** If it is virtio backend or front end. */
	unsigned int role;

	/** User-registered device callback */
	virtio_dev_reset_cb reset_cb;

	/** Virtio dispatch table */
	const struct virtio_dispatch *func;

	/**< Virtio device memory operations */
	const struct virtio_memory_ops *mmops;

	/** Private data */
	void *priv;

	/** Number of vrings */
	unsigned int vrings_num;

	/** Pointer to the virtio vring structure */
	struct virtio_vring_info *vrings_info;
};

/*
 * Helper functions.
 */

/**
 * @brief Virtio device dispatcher functions.
 *
 * Functions for virtio device configuration as defined in Rusty Russell's paper.
 * The virtio transport layers are expected to implement these functions in their respective codes.
 */

struct virtio_dispatch {
	/** Create virtio queue instances. */
	int (*create_virtqueues)(struct virtio_device *vdev,
				 unsigned int flags,
				 unsigned int nvqs, const char *names[],
				 vq_callback callbacks[],
				 void *callback_args[]);

	/** Delete virtio queue instances. */
	void (*delete_virtqueues)(struct virtio_device *vdev);

	/** Get the status of the virtio device. */
	uint8_t (*get_status)(struct virtio_device *dev);

	/** Set the status of the virtio device. */
	void (*set_status)(struct virtio_device *dev, uint8_t status);

	/** Get the feature exposed by the virtio device. */
	uint32_t (*get_features)(struct virtio_device *dev);

	/** Set the supported `feature` (virtio driver only). */
	void (*set_features)(struct virtio_device *dev, uint32_t feature);

	/**
	 * Set the supported features negotiate between the `features` parameter and features
	 * supported by the device (virtio driver only).
	 */
	uint32_t (*negotiate_features)(struct virtio_device *dev,
				       uint32_t features);

	/**
	 * Read a variable amount from the device specific (ie, network)
	 * configuration region.
	 */
	void (*read_config)(struct virtio_device *dev, uint32_t offset,
			    void *dst, int length);

	/**
	 * Write a variable amount from the device specific (ie, network)
	 * configuration region.
	 */
	void (*write_config)(struct virtio_device *dev, uint32_t offset,
			     void *src, int length);

	/** Request a reset of the virtio device. */
	void (*reset_device)(struct virtio_device *dev);

	/** Notify the other side that a virtio vring as been updated. */
	void (*notify)(struct virtqueue *vq);
};

struct virtio_memory_ops {
	/** Allocate memory from the virtio device. */
	void *(*alloc)(struct virtio_device *dev, size_t size, size_t align);

	/** Free memory allocated from the virtio device. */
	void (*free)(struct virtio_device *dev, void *buf);
};

/**
 * @brief Create the virtio device virtqueue.
 *
 * @param vdev			Pointer to virtio device structure.
 * @param flags			Create flag.
 * @param nvqs			The virtqueue number.
 * @param names			Virtqueue names.
 * @param callbacks		Virtqueue callback functions.
 * @param callback_args	Virtqueue callback function arguments.
 *
 * @return 0 on success, otherwise error code.
 */
int virtio_create_virtqueues(struct virtio_device *vdev, unsigned int flags,
			     unsigned int nvqs, const char *names[],
			     vq_callback callbacks[], void *callback_args[]);

/**
 * @brief Delete the virtio device virtqueue.
 *
 * @param vdev	Pointer to virtio device structure.
 *
 */
static inline void virtio_delete_virtqueues(struct virtio_device *vdev)
{
	if (!vdev || !vdev->func || !vdev->func->delete_virtqueues)
		return;

	vdev->func->delete_virtqueues(vdev);
}

/**
 * @brief Get device ID.
 *
 * @param vdev Pointer to device structure.
 *
 * @return Device ID value.
 */
static inline uint32_t virtio_get_devid(const struct virtio_device *vdev)
{
	if (!vdev)
		return 0;
	return vdev->id.device;
}

/**
 * @brief Retrieve device status.
 *
 * @param vdev		Pointer to device structure.
 * @param status	Pointer to the virtio device status.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_get_status(struct virtio_device *vdev, uint8_t *status)
{
	if (!vdev || !status)
		return -EINVAL;

	if (!vdev->func || !vdev->func->get_status)
		return -ENXIO;

	*status = vdev->func->get_status(vdev);
	return 0;
}

/**
 * @brief Set device status.
 *
 * @param vdev		Pointer to device structure.
 * @param status	Value to be set as device status.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_set_status(struct virtio_device *vdev, uint8_t status)
{
	if (!vdev)
		return -EINVAL;

	if (!vdev->func || !vdev->func->set_status)
		return -ENXIO;

	vdev->func->set_status(vdev, status);
	return 0;
}

/**
 * @brief Retrieve configuration data from the device.
 *
 * @param vdev		Pointer to device structure.
 * @param offset	Offset of the data within the configuration area.
 * @param dst		Address of the buffer that will hold the data.
 * @param len		Length of the data to be retrieved.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_read_config(struct virtio_device *vdev,
				     uint32_t offset, void *dst, int len)
{
	if (!vdev || !dst)
		return -EINVAL;

	if (!vdev->func || !vdev->func->read_config)
		return -ENXIO;

	vdev->func->read_config(vdev, offset, dst, len);
	return 0;
}

/**
 * @brief Write configuration data to the device.
 *
 * @param vdev		Pointer to device structure.
 * @param offset	Offset of the data within the configuration area.
 * @param src		Address of the buffer that holds the data to write.
 * @param len		Length of the data to be written.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_write_config(struct virtio_device *vdev,
				      uint32_t offset, void *src, int len)
{
	if (!vdev || !src)
		return -EINVAL;

	if (!vdev->func || !vdev->func->write_config)
		return -ENXIO;

	vdev->func->write_config(vdev, offset, src, len);
	return 0;
}

/**
 * @brief Get the virtio device features.
 *
 * @param vdev		Pointer to device structure.
 * @param features	Pointer to features supported by both the driver and
 *			the device as a bitfield.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_get_features(struct virtio_device *vdev,
				      uint32_t *features)
{
	if (!vdev || !features)
		return -EINVAL;

	if (!vdev->func || !vdev->func->get_features)
		return -ENXIO;

	*features = vdev->func->get_features(vdev);
	if (VIRTIO_ROLE_IS_DEVICE(vdev))
		vdev->features = *features;

	return 0;
}

/**
 * @brief Set features supported by the VIRTIO driver.
 *
 * @param vdev		Pointer to device structure.
 * @param features	Features supported by the driver as a bitfield.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_set_features(struct virtio_device *vdev,
				      uint32_t features)
{
	if (!vdev)
		return -EINVAL;

	if (!vdev->func || !vdev->func->set_features)
		return -ENXIO;

	vdev->func->set_features(vdev, features);
	return 0;
}

/**
 * @brief Negotiate features between virtio device and driver.
 *
 * @param vdev			Pointer to device structure.
 * @param features		Supported features.
 * @param final_features	Pointer to the final features after negotiate.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_negotiate_features(struct virtio_device *vdev,
					    uint32_t features,
					    uint32_t *final_features)
{
	if (!vdev)
		return -EINVAL;

	if (!vdev->func || !vdev->func->negotiate_features)
		return -ENXIO;

	vdev->features = vdev->func->negotiate_features(vdev, features);
	if (final_features)
		*final_features = vdev->features;
	return 0;
}

/**
 * @brief Reset virtio device.
 *
 * @param vdev	Pointer to virtio_device structure.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_reset_device(struct virtio_device *vdev)
{
	if (!vdev)
		return -EINVAL;

	if (!vdev->func || !vdev->func->reset_device)
		return -ENXIO;

	vdev->func->reset_device(vdev);
	return 0;
}

/**
 * @brief Allocate buffer from the virtio device
 *
 * @param vdev	Pointer to virtio device structure.
 * @param buf	Pointer to the allocated buffer (virtual address).
 * @param size	Allocated buffer size.
 * @param align	Allocated buffer alignment.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_alloc_buf(struct virtio_device *vdev, void **buf,
				   size_t size, size_t align)
{
	if (!vdev || !buf)
		return -EINVAL;

	if (!vdev->mmops || !vdev->mmops->alloc)
		return -ENXIO;

	*buf = vdev->mmops->alloc(vdev, size, align);
	if (!*buf)
		return -ENOMEM;

	return 0;
}

/**
 * @brief Free the buffer allocated by \ref virtio_alloc_buf from the virtio
 * device.
 *
 * @param vdev	Pointer to virtio device structure.
 * @param buf	Buffer need to be freed.
 *
 * @return 0 on success, otherwise error code.
 */
static inline int virtio_free_buf(struct virtio_device *vdev, void *buf)
{
	if (!vdev)
		return -EINVAL;

	if (!vdev->mmops || !vdev->mmops->free)
		return -ENXIO;

	vdev->mmops->free(vdev, buf);

	return 0;
}

/**
 * @brief Check if the virtio device support a specific feature.
 *
 * @param vdev		Pointer to device structure.
 * @param feature_bit	Feature bit to check.
 *
 * @return true if the feature is supported, otherwise false.
 */
static inline bool virtio_has_feature(struct virtio_device *vdev,
				      unsigned int feature_bit)
{
	uint32_t features;

	if (!vdev && feature_bit >= sizeof(features) * 8)
		return false;

	if (!vdev->features)
		virtio_get_features(vdev, &features);

	return (vdev->features & (1UL << feature_bit)) != 0;
}

#if defined __cplusplus
}
#endif

#endif				/* _VIRTIO_H_ */
