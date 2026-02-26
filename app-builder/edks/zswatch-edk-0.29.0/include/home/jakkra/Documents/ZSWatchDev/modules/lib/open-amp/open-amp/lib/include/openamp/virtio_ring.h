/*
 * Copyright Rusty Russell IBM Corporation 2007.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * $FreeBSD$
 */

#ifndef VIRTIO_RING_H
#define	VIRTIO_RING_H

#include <metal/compiler.h>

#if defined __cplusplus
extern "C" {
#endif

/* This marks a buffer as continuing via the next field. */
#define VRING_DESC_F_NEXT       1
/* This marks a buffer as write-only (otherwise read-only). */
#define VRING_DESC_F_WRITE      2
/* This means the buffer contains a list of buffer descriptors. */
#define VRING_DESC_F_INDIRECT   4

/* The Host uses this in used->flags to advise the Guest: don't kick me
 * when you add a buffer.  It's unreliable, so it's simply an
 * optimization.  Guest will still kick if it's out of buffers.
 */
#define VRING_USED_F_NO_NOTIFY  1
/* The Guest uses this in avail->flags to advise the Host: don't
 * interrupt me when you consume a buffer.  It's unreliable, so it's
 * simply an optimization.
 */
#define VRING_AVAIL_F_NO_INTERRUPT      1

/**
 * @brief VirtIO ring descriptors.
 *
 * The descriptor table refers to the buffers the driver is using for the
 * device. addr is a physical address, and the buffers can be chained via \ref next.
 * Each descriptor describes a buffer which is read-only for the device
 * (“device-readable”) or write-only for the device (“device-writable”), but a
 * chain of descriptors can contain both device-readable and device-writable
 * buffers.
 */
METAL_PACKED_BEGIN
struct vring_desc {
	/** Address (guest-physical) */
	uint64_t addr;

	/** Length */
	uint32_t len;

	/** Flags relevant to the descriptors */
	uint16_t flags;

	/** We chain unused descriptors via this, too */
	uint16_t next;
} METAL_PACKED_END;

/**
 * @brief Used to offer buffers to the device.
 *
 * Each ring entry refers to the head of a descriptor chain. It is only
 * written by the driver and read by the device.
 */
METAL_PACKED_BEGIN
struct vring_avail {
	/** Flag which determines whether device notifications are required */
	uint16_t flags;

	/**
	 * Indicates where the driver puts the next descriptor entry in the
	 * ring (modulo the queue size)
	 */
	uint16_t idx;

	/** The ring of descriptors */
	uint16_t ring[0];
} METAL_PACKED_END;

/* uint32_t is used here for ids for padding reasons. */
METAL_PACKED_BEGIN
struct vring_used_elem {
	union {
		uint16_t event;
		/* Index of start of used descriptor chain. */
		uint32_t id;
	};
	/* Total length of the descriptor chain which was written to. */
	uint32_t len;
} METAL_PACKED_END;

/**
 * @brief The device returns buffers to this structure when done with them
 *
 * The structure is only written to by the device, and read by the driver.
 */
METAL_PACKED_BEGIN
struct vring_used {
	/** Flag which determines whether device notifications are required */
	uint16_t flags;

	/**
	 * Indicates where the driver puts the next descriptor entry in the
	 * ring (modulo the queue size)
	 */
	uint16_t idx;

	/** The ring of descriptors */
	struct vring_used_elem ring[0];
} METAL_PACKED_END;

/**
 * @brief The virtqueue layout structure
 *
 * Each virtqueue consists of; descriptor table, available ring, used ring,
 * where each part is physically contiguous in guest memory, referenced
 * by the pointers in this structure.
 *
 * When the driver wants to send a buffer to the device, it fills in a slot in
 * the descriptor table (or chains several together), and writes the descriptor
 * index into the available ring. It then notifies the device. When the device
 * has finished a buffer, it writes the descriptor index into the used ring,
 * and sends an interrupt.
 *
 * The standard layout for the ring, referenced by this structure, is a
 * continuous chunk of memory exemplified by this table.  The maximum number
 * of buffer descriptors, num, is a power of 2.
 *
 * |vring| definition                      | description
 * |-----|-------------------------------- |------------
 * | desc| struct vring_desc desc[num]     | All num descriptors
 * |avail| uint16_t avail_flags            | Device notification flags
 * |     | uint16_t avail_idx              | Driver's next descriptor location
 * |     | uint16_t available[num]         | The ring of descriptors
 * |     | uint16_t used_event_idx         | Free running index
 * |  pad| char pad[]                      | Padding for memory alignment
 * | used| uint16_t used_flags             | Device notification flags
 * |     | uint16_t used_idx               | Driver's next descriptor location
 * |     | struct vring_used_elem used[num]| The ring of descriptors
 * |     | uint16_t avail_event_idx        | Free running index
 *
 * Padding for alignment requirements:
 *
 * Some architectures require strict alignment for atomic operations or DMA
 * transfer. The pad item should be inserted between avail and used memory
 * sections to ensure that the used ring is aligned, as the available ring
 * size is not necessarily a multiple of 4 or 8 bytes.
 *
 * The alignment may also be necessary for CPU cache line utilization,
 * as padding can avoid crossing cache lines or memory pages in the
 * shared memory.
 *
 * NOTE: for VirtIO PCI, align is 4096.
 */
struct vring {
	/**
	 * The maximum number of buffer descriptors in the virtqueue.
	 * The value is always a power of 2.
	 */
	unsigned int num;

	/** The actual buffer descriptors, 16 bytes each */
	struct vring_desc *desc;

	/** A ring of available descriptor heads with free-running index */
	struct vring_avail *avail;

	/** A ring of used descriptor heads with free-running index */
	struct vring_used *used;
};

/*
 * We publish the used event index at the end of the available ring, and vice
 * versa. They are at the end for backwards compatibility.
 */
#define vring_used_event(vr)	((vr)->avail->ring[(vr)->num])
#define vring_avail_event(vr)	((vr)->used->ring[(vr)->num].event)

static inline int vring_size(unsigned int num, unsigned long align)
{
	int size;

	size = num * sizeof(struct vring_desc);
	size += sizeof(struct vring_avail) + (num * sizeof(uint16_t)) +
	    sizeof(uint16_t);
	size = (size + align - 1) & ~(align - 1);
	size += sizeof(struct vring_used) +
	    (num * sizeof(struct vring_used_elem)) + sizeof(uint16_t);

	return size;
}

static inline void
vring_init(struct vring *vr, unsigned int num, uint8_t *p, unsigned long align)
{
	vr->num = num;
	vr->desc = (struct vring_desc *)p;
	vr->avail = (struct vring_avail *)(p + num * sizeof(struct vring_desc));
	vr->used = (struct vring_used *)
	    (((unsigned long)&vr->avail->ring[num] + sizeof(uint16_t) +
	      align - 1) & ~(align - 1));
}

/*
 * The following is used with VIRTIO_RING_F_EVENT_IDX.
 *
 * Assuming a given event_idx value from the other size, if we have
 * just incremented index from old to new_idx, should we trigger an
 * event?
 */
static inline int
vring_need_event(uint16_t event_idx, uint16_t new_idx, uint16_t old)
{
	return (uint16_t)(new_idx - event_idx - 1) <
	    (uint16_t)(new_idx - old);
}

#if defined __cplusplus
}
#endif

#endif				/* VIRTIO_RING_H */
