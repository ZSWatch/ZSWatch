/*
 * Remote processor messaging
 *
 * Copyright (C) 2011 Texas Instruments, Inc.
 * Copyright (C) 2011 Google, Inc.
 * All rights reserved.
 * Copyright (c) 2016 Freescale Semiconductor, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RPMSG_H_
#define _RPMSG_H_

#include <metal/compiler.h>
#include <metal/mutex.h>
#include <metal/list.h>
#include <metal/utilities.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

/* Configurable parameters */
#define RPMSG_NAME_SIZE			(32)
#define RPMSG_ADDR_BMP_SIZE		(128)

#define RPMSG_NS_EPT_ADDR		(0x35)
#define RPMSG_RESERVED_ADDRESSES	(1024)
#define RPMSG_ADDR_ANY			0xFFFFFFFF

/* Error macros. */
#define RPMSG_SUCCESS			0
#define RPMSG_ERROR_BASE		-2000
#define RPMSG_ERR_NO_MEM		(RPMSG_ERROR_BASE - 1)
#define RPMSG_ERR_NO_BUFF		(RPMSG_ERROR_BASE - 2)
#define RPMSG_ERR_PARAM			(RPMSG_ERROR_BASE - 3)
#define RPMSG_ERR_DEV_STATE		(RPMSG_ERROR_BASE - 4)
#define RPMSG_ERR_BUFF_SIZE		(RPMSG_ERROR_BASE - 5)
#define RPMSG_ERR_INIT			(RPMSG_ERROR_BASE - 6)
#define RPMSG_ERR_ADDR			(RPMSG_ERROR_BASE - 7)
#define RPMSG_ERR_PERM			(RPMSG_ERROR_BASE - 8)
#define RPMSG_EOPNOTSUPP		(RPMSG_ERROR_BASE - 9)

struct rpmsg_endpoint;
struct rpmsg_device;

/* Returns positive value on success or negative error value on failure */
typedef int (*rpmsg_ept_cb)(struct rpmsg_endpoint *ept, void *data,
			    size_t len, uint32_t src, void *priv);
typedef void (*rpmsg_ept_release_cb)(struct rpmsg_endpoint *ept);
typedef void (*rpmsg_ns_unbind_cb)(struct rpmsg_endpoint *ept);
typedef void (*rpmsg_ns_bind_cb)(struct rpmsg_device *rdev,
				 const char *name, uint32_t dest);

/**
 * @brief Structure that binds a local RPMsg address to its user
 *
 * In essence, an RPMsg endpoint represents a listener on the RPMsg bus, as
 * it binds an RPMsg address with an rx callback handler.
 */
struct rpmsg_endpoint {
	/** Name of the service supported */
	char name[RPMSG_NAME_SIZE];

	/** Pointer to the RPMsg device */
	struct rpmsg_device *rdev;

	/** Local address of the endpoint */
	uint32_t addr;

	/** Address of the default remote endpoint binded */
	uint32_t dest_addr;

	/** Reference count for determining whether the endpoint can be deallocated */
	uint32_t refcnt;

	/** Callback to inform the user that the endpoint allocation can be safely removed */
	rpmsg_ept_release_cb release_cb;

	/**
	 * User rx callback, return value of this callback is reserved for future
	 * use, for now, only allow RPMSG_SUCCESS as return value
	 */
	rpmsg_ept_cb cb;

	/** Endpoint service unbind callback, called when remote ept is destroyed */
	rpmsg_ns_unbind_cb ns_unbind_cb;

	/** Endpoint node */
	struct metal_list node;

	/** Private data for the driver's use */
	void *priv;
};

/** @brief RPMsg device operations */
struct rpmsg_device_ops {
	/** Send RPMsg data */
	int (*send_offchannel_raw)(struct rpmsg_device *rdev,
				   uint32_t src, uint32_t dst,
				   const void *data, int len, int wait);

	/** Hold RPMsg RX buffer */
	void (*hold_rx_buffer)(struct rpmsg_device *rdev, void *rxbuf);

	/** Release RPMsg RX buffer */
	void (*release_rx_buffer)(struct rpmsg_device *rdev, void *rxbuf);

	/** Get RPMsg TX buffer */
	void *(*get_tx_payload_buffer)(struct rpmsg_device *rdev,
				       uint32_t *len, int wait);

	/** Send RPMsg data without copy */
	int (*send_offchannel_nocopy)(struct rpmsg_device *rdev,
				      uint32_t src, uint32_t dst,
				      const void *data, int len);

	/** Release RPMsg TX buffer */
	int (*release_tx_buffer)(struct rpmsg_device *rdev, void *txbuf);

	/** Get RPMsg RX buffer size */
	int (*get_rx_buffer_size)(struct rpmsg_device *rdev);

	/** Get RPMsg TX buffer size */
	int (*get_tx_buffer_size)(struct rpmsg_device *rdev);
};

/** @brief Representation of a RPMsg device */
struct rpmsg_device {
	/** List of endpoints */
	struct metal_list endpoints;

	/** Name service endpoint */
	struct rpmsg_endpoint ns_ept;

	/** Table endpoint address allocation */
	unsigned long bitmap[metal_bitmap_longs(RPMSG_ADDR_BMP_SIZE)];
	unsigned int bitnext;

	/** Mutex lock for RPMsg management */
	metal_mutex_t lock;

	/** Callback handler for name service announcement without local epts waiting to bind */
	rpmsg_ns_bind_cb ns_bind_cb;

	/** Callback handler for name service announcement, called when remote ept is destroyed */
	rpmsg_ns_bind_cb ns_unbind_cb;

	/** RPMsg device operations */
	struct rpmsg_device_ops ops;

	/** Create/destroy namespace message */
	bool support_ns;
};

/**
 * @brief Send a message across to the remote processor,
 * specifying source and destination address.
 *
 * This function sends `data` of length `len` to the remote `dst` address from
 * the source `src` address.
 * The message will be sent to the remote processor which the channel belongs
 * to.
 *
 * @param ept	The rpmsg endpoint
 * @param src	Source endpoint address of the message
 * @param dst	Destination endpoint address of the message
 * @param data	Payload of the message
 * @param len	Length of the payload
 * @param wait	Boolean value indicating whether to wait on buffers
 *
 * @return Number of bytes it has sent or negative error value on failure.
 */
int rpmsg_send_offchannel_raw(struct rpmsg_endpoint *ept, uint32_t src,
			      uint32_t dst, const void *data, int len,
			      int wait);

/**
 * @brief Send a message across to the remote processor
 *
 * This function sends `data` of length `len` based on the `ept`.
 * The message will be sent to the remote processor which the channel belongs
 * to, using `ept`'s source and destination addresses.
 * In case there are no TX buffers available, the function will block until
 * one becomes available, or a timeout of 15 seconds elapses. When the latter
 * happens, -ERESTARTSYS is returned.
 *
 * @param ept	The rpmsg endpoint
 * @param data	Payload of the message
 * @param len	Length of the payload
 *
 * @return Number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_send(struct rpmsg_endpoint *ept, const void *data,
			     int len)
{
	if (!ept)
		return RPMSG_ERR_PARAM;

	return rpmsg_send_offchannel_raw(ept, ept->addr, ept->dest_addr, data,
					 len, true);
}

/**
 * @brief Send a message across to the remote processor, specify dst
 *
 * This function sends `data` of length `len` to the remote `dst` address.
 * The message will be sent to the remote processor which the `ept`
 * channel belongs to, using `ept`'s source address.
 * In case there are no TX buffers available, the function will block until
 * one becomes available, or a timeout of 15 seconds elapses. When the latter
 * happens, -ERESTARTSYS is returned.
 *
 * @param ept	The rpmsg endpoint
 * @param data	Payload of message
 * @param len	Length of payload
 * @param dst	Destination address
 *
 * @return Number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_sendto(struct rpmsg_endpoint *ept, const void *data,
			       int len, uint32_t dst)
{
	if (!ept)
		return RPMSG_ERR_PARAM;

	return rpmsg_send_offchannel_raw(ept, ept->addr, dst, data, len, true);
}

/**
 * @brief Send a message using explicit src/dst addresses
 *
 * This function sends `data` of length `len` to the remote `dst` address,
 * and uses `src` as the source address.
 * The message will be sent to the remote processor which the `ept`
 * channel belongs to.
 * In case there are no TX buffers available, the function will block until
 * one becomes available, or a timeout of 15 seconds elapses. When the latter
 * happens, -ERESTARTSYS is returned.
 *
 * @param ept	The rpmsg endpoint
 * @param src	Source address
 * @param dst	Destination address
 * @param data	Payload of message
 * @param len	Length of payload
 *
 * @return Number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_send_offchannel(struct rpmsg_endpoint *ept,
					uint32_t src, uint32_t dst,
					const void *data, int len)
{
	return rpmsg_send_offchannel_raw(ept, src, dst, data, len, true);
}

/**
 * @brief Send a message across to the remote processor
 *
 * This function sends `data` of length `len` on the `ept` channel.
 * The message will be sent to the remote processor which the `ept`
 * channel belongs to, using `ept`'s source and destination addresses.
 * In case there are no TX buffers available, the function will immediately
 * return -ENOMEM without waiting until one becomes available.
 *
 * @param ept	The rpmsg endpoint
 * @param data	Payload of message
 * @param len	Length of payload
 *
 * @return Number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_trysend(struct rpmsg_endpoint *ept, const void *data,
				int len)
{
	if (!ept)
		return RPMSG_ERR_PARAM;

	return rpmsg_send_offchannel_raw(ept, ept->addr, ept->dest_addr, data,
					 len, false);
}

/**
 * @brief Send a message across to the remote processor, specify dst
 *
 * This function sends `data` of length `len` to the remote `dst` address.
 * The message will be sent to the remote processor which the `ept`
 * channel belongs to, using `ept`'s source address.
 * In case there are no TX buffers available, the function will immediately
 * return -ENOMEM without waiting until one becomes available.
 *
 * @param ept	The rpmsg endpoint
 * @param data	Payload of message
 * @param len	Length of payload
 * @param dst	Destination address
 *
 * @return Number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_trysendto(struct rpmsg_endpoint *ept, const void *data,
				  int len, uint32_t dst)
{
	if (!ept)
		return RPMSG_ERR_PARAM;

	return rpmsg_send_offchannel_raw(ept, ept->addr, dst, data, len, false);
}

/**
 * @brief Send a message using explicit src/dst addresses
 *
 * This function sends `data` of length `len` to the remote `dst` address,
 * and uses `src` as the source address.
 * The message will be sent to the remote processor which the `ept`
 * channel belongs to.
 * In case there are no TX buffers available, the function will immediately
 * return -ENOMEM without waiting until one becomes available.
 *
 * @param ept	The rpmsg endpoint
 * @param src	Source address
 * @param dst	Destination address
 * @param data	Payload of message
 * @param len	Length of payload
 *
 * @return Number of bytes it has sent or negative error value on failure.
 */
static inline int rpmsg_trysend_offchannel(struct rpmsg_endpoint *ept,
					   uint32_t src, uint32_t dst,
					   const void *data, int len)
{
	return rpmsg_send_offchannel_raw(ept, src, dst, data, len, false);
}

/**
 * @brief Holds the rx buffer for usage outside the receive callback.
 *
 * Calling this function prevents the RPMsg receive buffer from being released
 * back to the pool of shmem buffers. This API can only be called at rx
 * callback context (rpmsg_rx_cb_t). With this API, the application doesn't
 * need to copy the message in rx callback. Instead, the rx buffer base address
 * is saved in application context and further processed in application
 * process. After the message is processed, the application can release the rx
 * buffer for future reuse in vring by calling the rpmsg_release_rx_buffer()
 * function.
 *
 * @param ept	The rpmsg endpoint
 * @param rxbuf RX buffer with message payload
 *
 * @see rpmsg_release_rx_buffer
 */
void rpmsg_hold_rx_buffer(struct rpmsg_endpoint *ept, void *rxbuf);

/**
 * @brief Releases the rx buffer for future reuse in vring.
 *
 * This API can be called at process context when the message in rx buffer is
 * processed.
 *
 * @param ept	The rpmsg endpoint
 * @param rxbuf	rx buffer with message payload
 *
 * @see rpmsg_hold_rx_buffer
 */
void rpmsg_release_rx_buffer(struct rpmsg_endpoint *ept, void *rxbuf);

/**
 * @brief Gets the tx buffer for message payload.
 *
 * This API can only be called at process context to get the tx buffer in vring.
 * By this way, the application can directly put its message into the vring tx
 * buffer without copy from an application buffer.
 * It is the application responsibility to correctly fill the allocated tx
 * buffer by data and passing correct parameters to the rpmsg_send_nocopy() or
 * rpmsg_sendto_nocopy() function to perform data no-copy-send mechanism.
 *
 * @param ept	Pointer to rpmsg endpoint
 * @param len	Pointer to store tx buffer size
 * @param wait	Boolean, wait or not for buffer to become available
 *
 * @return The tx buffer address on success and NULL on failure
 *
 * @see rpmsg_send_offchannel_nocopy
 * @see rpmsg_sendto_nocopy
 * @see rpmsg_send_nocopy
 */
void *rpmsg_get_tx_payload_buffer(struct rpmsg_endpoint *ept,
				  uint32_t *len, int wait);

/**
 * @brief Releases unused buffer.
 *
 * This API can be called when the Tx buffer reserved by
 * rpmsg_get_tx_payload_buffer needs to be released without having been sent to
 * the remote side.
 *
 * Note that the rpmsg virtio is not able to detect if a buffer has already
 * been released. The user must prevent a double release (e.g. by resetting its
 * buffer pointer to zero after the release).
 *
 * @param ept	The rpmsg endpoint
 * @param txbuf	tx buffer with message payload
 *
 * @return
 *   - RPMSG_SUCCESS on success
 *   - RPMSG_ERR_PARAM on invalid parameter
 *   - RPMSG_ERR_PERM if service not implemented
 *
 * @see rpmsg_get_tx_payload_buffer
 */
int rpmsg_release_tx_buffer(struct rpmsg_endpoint *ept, void *txbuf);

/**
 * @brief Get RPMsg Tx buffer size
 *
 * @param ept	The rpmsg endpoint
 *
 * @return
 *   - Next available Tx buffer size on success
 *   - RPMSG_ERR_PARAM on invalid parameter
 *   - RPMSG_ERR_PERM if service not implemented
 */
int rpmsg_get_tx_buffer_size(struct rpmsg_endpoint *ept);

/**
 * @brief Get RPMsg Rx buffer size
 *
 * @param ept	The rpmsg endpoint
 *
 * @return
 *   - Next available Rx buffer size on success
 *   - RPMSG_ERR_PARAM on invalid parameter
 *   - RPMSG_ERR_PERM if service not implemented
 */
int rpmsg_get_rx_buffer_size(struct rpmsg_endpoint *ept);

/**
 * @brief Send a message in tx buffer reserved by
 * rpmsg_get_tx_payload_buffer() across to the remote processor.
 *
 * This function sends buf of length len to the remote dst address,
 * and uses src as the source address.
 * The message will be sent to the remote processor which the ept
 * endpoint belongs to.
 * The application has to take the responsibility for:
 *  1. tx buffer reserved (rpmsg_get_tx_payload_buffer() )
 *  2. filling the data to be sent into the pre-allocated tx buffer
 *  3. not exceeding the buffer size when filling the data
 *  4. data cache coherency
 *
 * After the rpmsg_send_offchannel_nocopy() function is issued the tx buffer is
 * no more owned by the sending task and must not be touched anymore unless the
 * rpmsg_send_offchannel_nocopy() function fails and returns an error. In that
 * case application should try to re-issue the rpmsg_send_offchannel_nocopy()
 * again.
 *
 * @param ept	The rpmsg endpoint
 * @param src	The rpmsg endpoint local address
 * @param dst	The rpmsg endpoint remote address
 * @param data	TX buffer with message filled
 * @param len	Length of payload
 *
 * @return Number of bytes it has sent or negative error value on failure.
 *
 * @see rpmsg_get_tx_payload_buffer
 * @see rpmsg_sendto_nocopy
 * @see rpmsg_send_nocopy
 */
int rpmsg_send_offchannel_nocopy(struct rpmsg_endpoint *ept, uint32_t src,
				 uint32_t dst, const void *data, int len);

/**
 * @brief Sends a message in tx buffer allocated by
 * rpmsg_get_tx_payload_buffer() across to the remote processor, specify dst.
 *
 * This function sends buf of length len to the remote dst address.
 * The message will be sent to the remote processor which the ept
 * endpoint belongs to, using ept's source address.
 * The application has to take the responsibility for:
 *  1. tx buffer allocation (rpmsg_get_tx_payload_buffer() )
 *  2. filling the data to be sent into the pre-allocated tx buffer
 *  3. not exceeding the buffer size when filling the data
 *  4. data cache coherency
 *
 * After the rpmsg_sendto_nocopy() function is issued the tx buffer is no more
 * owned by the sending task and must not be touched anymore unless the
 * rpmsg_sendto_nocopy() function fails and returns an error. In that case the
 * application should try to re-issue the rpmsg_sendto_nocopy() again.
 *
 * @param ept	The rpmsg endpoint
 * @param data	TX buffer with message filled
 * @param len	Length of payload
 * @param dst	Destination address
 *
 * @return Number of bytes it has sent or negative error value on failure.
 *
 * @see rpmsg_get_tx_payload_buffer
 * @see rpmsg_send_offchannel_nocopy
 * @see rpmsg_send_nocopy
 */
static inline int rpmsg_sendto_nocopy(struct rpmsg_endpoint *ept,
				      const void *data, int len, uint32_t dst)
{
	if (!ept)
		return RPMSG_ERR_PARAM;

	return rpmsg_send_offchannel_nocopy(ept, ept->addr, dst, data, len);
}

/**
 * @brief Send a message in tx buffer reserved by
 * rpmsg_get_tx_payload_buffer() across to the remote processor.
 *
 * This function sends buf of length len on the ept endpoint.
 * The message will be sent to the remote processor which the ept
 * endpoint belongs to, using ept's source and destination addresses.
 * The application has to take the responsibility for:
 *  1. tx buffer reserved (rpmsg_get_tx_payload_buffer() )
 *  2. filling the data to be sent into the pre-allocated tx buffer
 *  3. not exceeding the buffer size when filling the data
 *  4. data cache coherency
 *
 * After the rpmsg_send_nocopy() function is issued the tx buffer is no more
 * owned by the sending task and must not be touched anymore unless the
 * rpmsg_send_nocopy() function fails and returns an error. In that case the
 * application should try to re-issue the rpmsg_send_nocopy() again.
 *
 * @param ept	The rpmsg endpoint
 * @param data	TX buffer with message filled
 * @param len	Length of payload
 *
 * @return Number of bytes it has sent or negative error value on failure.
 *
 * @see rpmsg_get_tx_payload_buffer
 * @see rpmsg_send_offchannel_nocopy
 * @see rpmsg_sendto_nocopy
 */
static inline int rpmsg_send_nocopy(struct rpmsg_endpoint *ept,
				    const void *data, int len)
{
	if (!ept)
		return RPMSG_ERR_PARAM;

	return rpmsg_send_offchannel_nocopy(ept, ept->addr,
					    ept->dest_addr, data, len);
}

/**
 * @brief Create rpmsg endpoint and register it to rpmsg device
 *
 * Create a RPMsg endpoint, initialize it with a name, source address,
 * remoteproc address, endpoint callback, and destroy endpoint callback,
 * and register it to the RPMsg device.
 *
 * In essence, an rpmsg endpoint represents a listener on the rpmsg bus, as
 * it binds an rpmsg address with an rx callback handler.
 *
 * Rpmsg client should create an endpoint to discuss with remote. rpmsg client
 * provide at least a channel name, a callback for message notification and by
 * default endpoint source address should be set to RPMSG_ADDR_ANY.
 *
 * As an option Some rpmsg clients can specify an endpoint with a specific
 * source address.
 *
 * @param ept		Pointer to rpmsg endpoint
 * @param rdev		RPMsg device associated with the endpoint
 * @param name		Service name associated to the endpoint (maximum size \ref RPMSG_NAME_SIZE)
 * @param src		Local address of the endpoint
 * @param dest		Target address of the endpoint
 * @param cb		Endpoint callback
 * @param ns_unbind_cb	Endpoint service unbind callback, called when remote
 *			ept is destroyed.
 *
 * @return 0 on success, or negative error value on failure.
 */
int rpmsg_create_ept(struct rpmsg_endpoint *ept, struct rpmsg_device *rdev,
		     const char *name, uint32_t src, uint32_t dest,
		     rpmsg_ept_cb cb, rpmsg_ns_unbind_cb ns_unbind_cb);

/**
 * @brief Destroy rpmsg endpoint and unregister it from rpmsg device
 *
 * It unregisters the rpmsg endpoint from the rpmsg device and calls the
 * destroy endpoint callback if it is provided.
 *
 * @param ept	Pointer to the rpmsg endpoint
 */
void rpmsg_destroy_ept(struct rpmsg_endpoint *ept);

/**
 * @brief Check if the rpmsg endpoint ready to send
 *
 * @param ept	Pointer to rpmsg endpoint
 *
 * @return 1 if the rpmsg endpoint has both local addr and destination
 * addr set, 0 otherwise
 */
static inline unsigned int is_rpmsg_ept_ready(struct rpmsg_endpoint *ept)
{
	return ept && ept->rdev && ept->dest_addr != RPMSG_ADDR_ANY;
}

#if defined __cplusplus
}
#endif

#endif				/* _RPMSG_H_ */
