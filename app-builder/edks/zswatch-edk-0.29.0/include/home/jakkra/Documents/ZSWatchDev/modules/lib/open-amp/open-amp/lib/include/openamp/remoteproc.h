/*
 * Remoteproc Framework
 *
 * Copyright(c) 2018 Xilinx Ltd.
 * Copyright(c) 2011 Texas Instruments, Inc.
 * Copyright(c) 2011 Google, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef REMOTEPROC_H
#define REMOTEPROC_H

#include <metal/io.h>
#include <metal/mutex.h>
#include <metal/compiler.h>

#if defined __cplusplus
extern "C" {
#endif

#define RSC_NOTIFY_ID_ANY 0xFFFFFFFFU

#define RPROC_MAX_NAME_LEN 32

/**
 * @brief Resource table header
 *
 * A resource table is essentially a list of system resources required
 * by the remote remoteproc. It may also include configuration entries.
 * If needed, the remote remoteproc firmware should contain this table
 * as a dedicated ".resource_table" ELF section.
 *
 * Some resource entries are mere announcements, where the host is informed
 * of specific remoteproc configurations. Other entries require the host to
 * do something (e.g. allocate a system resource). Sometimes a negotiation
 * is expected, where the firmware requests a resource, and once allocated,
 * the host should provide back its details (e.g. address of an allocated
 * memory region).
 *
 * The header of the resource table, as expressed by this structure,
 * contains a version number (should we need to change this format in the
 * future), the number of available resource entries, and their offsets
 * in the table.
 *
 * Immediately following this header are the resource entries themselves,
 * each of which begins with a resource entry header.
 */
METAL_PACKED_BEGIN
struct resource_table {
	/** Version number */
	uint32_t ver;

	/** Number of resource entries */
	uint32_t num;

	/** Reserved (must be zero) */
	uint32_t reserved[2];

	/** Array of offsets pointing at the various resource entries */
	uint32_t offset[0];
} METAL_PACKED_END;

/**
 * @brief Resource table entry header
 *
 * Every resource entry begins with this firmware resource header providing
 * its \ref type. The content of the entry itself will immediately follow
 * this header, and it should be parsed according to the resource type.
 */
METAL_PACKED_BEGIN
struct fw_rsc_hdr {
	/** Resource type matching the type field of the structure in \ref data */
	uint32_t type;

	/** Resource data */
	uint8_t data[0];
} METAL_PACKED_END;

/**
 * @brief Types of resource entries
 *
 * For more details regarding a specific resource type, please see its
 * dedicated structure below.
 *
 * Please note that these values are used as indices to the rproc_handle_rsc
 * lookup table, so please keep them sane. Moreover, \ref RSC_LAST is used to
 * check the validity of an index before the lookup table is accessed, so
 * please update it as needed.
 */
enum fw_resource_type {
	/** carveout resource
	 *
	 *  Request for allocation of a physically contiguous memory region.
	 */
	RSC_CARVEOUT = 0,
	/** device memory resource
	 *
	 *  Request to iommu_map a memory-based peripheral.
	 */
	RSC_DEVMEM = 1,
	/** trace resource
	 *
	 *  Announces the availability of a trace buffer into which the remote remoteproc will be
	 *  writing logs.
	 */
	RSC_TRACE = 2,
	/** virtio device resource
	 *
	 *  Declare support for a virtio device, and serve as its virtio header.
	 */
	RSC_VDEV = 3,
	/** end of the generic resources */
	RSC_LAST = 4,
	/** Start of the vendor specific resource types range */
	RSC_VENDOR_START = 128,
	/** End of the vendor specific resource types range */
	RSC_VENDOR_END = 512,
};

#define FW_RSC_U64_ADDR_ANY 0xFFFFFFFFFFFFFFFFUL
#define FW_RSC_U32_ADDR_ANY 0xFFFFFFFFUL

/**
 * @brief Resource table physically contiguous memory request entry
 *
 * This resource entry requests the host to allocate a physically contiguous
 * memory region.
 *
 * These request entries should precede other firmware resource entries,
 * as other entries might request placing other data objects inside
 * these memory regions (e.g. data/code segments, trace resource entries, ...).
 *
 * Allocating memory this way helps utilizing the reserved physical memory
 * (e.g. CMA) more efficiently, and also minimizes the number of TLB entries
 * needed to map it (in case rproc is using an IOMMU). Reducing the TLB
 * pressure is important; it may have a substantial impact on performance.
 *
 * If the firmware is compiled with static addresses, then \ref da should specify
 * the expected device address of this memory region. If \ref da is set to
 * FW_RSC_ADDR_ANY, then the host will dynamically allocate it, and then
 * overwrite \ref da with the dynamically allocated address.
 *
 * We will always use \ref da to negotiate the device addresses, even if it
 * isn't using an IOMMU. In that case, though, it will obviously contain
 * physical addresses.
 *
 * Some remote remoteprocs need to know the allocated physical address
 * even if they do use an IOMMU. This is needed, e.g., if they control
 * hardware accelerators which access the physical memory directly (this
 * is the case with OMAP4 for instance). In that case, the host will
 * overwrite \ref pa with the dynamically allocated physical address.
 * Generally we don't want to expose physical addresses if we don't have to
 * (remote remoteprocs are generally _not_ trusted), so we might want to
 * change this to happen _only_ when explicitly required by the hardware.
 */
METAL_PACKED_BEGIN
struct fw_rsc_carveout {
	/** Resource carveout has type 0 */
	uint32_t type;

	/** Device address */
	uint32_t da;

	/** Physical address */
	uint32_t pa;

	/** Length in bytes */
	uint32_t len;

	/** IOMMU protection flags */
	uint32_t flags;

	/** Reserved (must be zero) */
	uint32_t reserved;

	/** Optional human-readable name of the requested memory region used for debugging */
	uint8_t name[RPROC_MAX_NAME_LEN];
} METAL_PACKED_END;

/**
 * @brief Resource table IOMMU mapping request entry
 *
 * This resource entry requests the host to IOMMU map a physically contiguous
 * memory region. This is needed in case the remote remoteproc requires
 * access to certain memory-based peripherals; _never_ use it to access
 * regular memory.
 *
 * This is obviously only needed if the remote remoteproc is accessing memory
 * via an IOMMU.
 *
 * Note: at this point we just "trust" those devmem entries to contain valid
 * physical addresses, but this isn't safe and will be changed: eventually we
 * want remoteproc implementations to provide us ranges of physical addresses
 * the firmware is allowed to request, and not allow firmwares to request
 * access to physical addresses that are outside those ranges.
 */
METAL_PACKED_BEGIN
struct fw_rsc_devmem {
	/** IOMMU mapping request has type 1 */
	uint32_t type;

	/** Device address */
	uint32_t da;

	/** Physical address to map */
	uint32_t pa;

	/** Length of the mapping in bytes */
	uint32_t len;

	/** IOMMU protection flags */
	uint32_t flags;

	/** Reserved (must be zero) */
	uint32_t reserved;

	/** Optional human-readable name of the requested memory region used for debugging */
	uint8_t name[RPROC_MAX_NAME_LEN];
} METAL_PACKED_END;

/**
 * @brief Resource table trace buffer declaration entry
 *
 * This resource entry provides the host information about a trace buffer
 * into which the remote remoteproc will write log messages.
 *
 * After booting the remote remoteproc, the trace buffers are exposed to the
 * user via debugfs entries (called trace0, trace1, etc..).
 */
METAL_PACKED_BEGIN
struct fw_rsc_trace {
	/** Trace buffer entry has type 2 */
	uint32_t type;

	/** Device address of the buffer */
	uint32_t da;

	/** Length of the buffer in bytes */
	uint32_t len;

	/** Reserved (must be zero) */
	uint32_t reserved;

	/** Optional human-readable name of the requested memory region used for debugging */
	uint8_t name[RPROC_MAX_NAME_LEN];
} METAL_PACKED_END;

/**
 * @brief Resource table vring descriptor entry
 *
 * This descriptor is not a resource entry by itself; it is part of the
 * \ref fw_rsc_vdev resource type.
 */
METAL_PACKED_BEGIN
struct fw_rsc_vdev_vring {
	/**
	 * The device address where the remoteproc is expecting the vring, or
	 * FW_RSC_U32_ADDR_ANY/FW_RSC_U64_ADDR_ANY to indicate that dynamic
	 * allocation of the vring's device address is supported
	 */
	uint32_t da;

	/** The alignment between the consumer and producer parts of the vring */
	uint32_t align;

	/** Number of buffers supported by this vring (must be power of two) */
	uint32_t num;

	/**
	 * A unique rproc-wide notify index for this vring. This notify index is
	 * used when kicking a remote remoteproc, to let it know that this vring
	 * is triggered
	 */
	uint32_t notifyid;

	/** Reserved (must be zero) */
	uint32_t reserved;
} METAL_PACKED_END;

/**
 * @brief Resource table virtio device entry
 *
 * This resource is a virtio device header: it provides information about
 * the vdev, and is then used by the host and its peer remote remoteprocs
 * to negotiate and share certain virtio properties.
 *
 * By providing this resource entry, the firmware essentially asks remoteproc
 * to statically allocate a vdev upon registration of the rproc (dynamic vdev
 * allocation is not yet supported).
 *
 * Note: unlike virtualization systems, the term 'host' here means
 * the Linux side which is running remoteproc to control the remote
 * remoteprocs. We use the name 'gfeatures' to comply with virtio's terms,
 * though there isn't really any virtualized guest OS here: it's the host
 * which is responsible for negotiating the final features.
 *
 * Note: immediately following this structure is the virtio config space for
 * this vdev (which is specific to the vdev; for more info, read the virtio
 * spec).
 */
METAL_PACKED_BEGIN
struct fw_rsc_vdev {
	/** Virtio device header has type 3 */
	uint32_t type;

	/** Virtio device id (as in virtio_ids.h) */
	uint32_t id;

	/**
	 * A unique rproc-wide notify index for this vdev. This notify index is
	 * used when kicking a remote remoteproc, to let it know that the
	 * status/features of this vdev have changes.
	 */
	uint32_t notifyid;

	/** The virtio device features supported by the firmware */
	uint32_t dfeatures;

	/**
	 * A place holder used by the host to write back the negotiated features
	 * that are supported by both sides
	 */
	uint32_t gfeatures;

	/**
	 * The size of the virtio config space of this vdev. The config space lies
	 * in the resource table immediate after this vdev header
	 */
	uint32_t config_len;

	/** A place holder where the host will indicate its virtio progress */
	uint8_t status;

	/** Number of vrings described in this vdev header */
	uint8_t num_of_vrings;

	/** Reserved (must be zero) */
	uint8_t reserved[2];

	/** An array of \ref num_of_vrings entries of \ref fw_rsc_vdev_vring */
	struct fw_rsc_vdev_vring vring[0];
} METAL_PACKED_END;

/**
 * @brief Resource table remote processor vendor specific entry
 *
 * This resource entry tells the host the vendor specific resource
 * required by the remote.
 *
 * These request entries should precede other shared resource entries
 * such as vdevs, vrings.
 */
METAL_PACKED_BEGIN
struct fw_rsc_vendor {
	/** Vendor specific resource type can be values 128-512 */
	uint32_t type;

	/** Length of the resource */
	uint32_t len;
} METAL_PACKED_END;

struct loader_ops;
struct image_store_ops;
struct remoteproc_ops;

/** @brief Memory used by the remote processor */
struct remoteproc_mem {
	/** Device memory */
	metal_phys_addr_t da;

	/** Physical memory */
	metal_phys_addr_t pa;

	/** Size of the memory */
	size_t size;

	/** Optional human-readable name of the memory region */
	char name[RPROC_MAX_NAME_LEN];

	/** Pointer to the I/O region */
	struct metal_io_region *io;

	/** List node */
	struct metal_list node;
};

/**
 * @brief A remote processor instance
 *
 * This structure is maintained by the remoteproc to represent the remote
 * processor instance. This structure acts as a prime parameter to use
 * the remoteproc APIs.
 */
struct remoteproc {
	/** Mutex lock */
	metal_mutex_t lock;

	/** Pointer to the resource table */
	void *rsc_table;

	/** Length of the resource table */
	size_t rsc_len;

	/** Metal I/O region of the resource table */
	struct metal_io_region *rsc_io;

	/** Remoteproc memories */
	struct metal_list mems;

	/** Remoteproc virtio devices */
	struct metal_list vdevs;

	/** Bitmap for notify IDs for remoteproc subdevices */
	unsigned long bitmap;

	/** Remoteproc operations */
	const struct remoteproc_ops *ops;

	/** Boot address */
	metal_phys_addr_t bootaddr;

	/** Executable loader */
	const struct loader_ops *loader;

	/** Remote processor state */
	unsigned int state;

	/** Private data */
	void *priv;
};

/**
 * @brief Remoteproc operations to manage a remoteproc instance
 *
 * Remoteproc operations need to be implemented by each remoteproc driver
 */
struct remoteproc_ops {
	/** Initialize the remoteproc instance */
	struct remoteproc *(*init)(struct remoteproc *rproc,
				   const struct remoteproc_ops *ops, void *arg);

	/** Remove the remoteproc instance */
	void (*remove)(struct remoteproc *rproc);

	/** Memory map the memory with physical address or destination address as input */
	void *(*mmap)(struct remoteproc *rproc,
		      metal_phys_addr_t *pa, metal_phys_addr_t *da,
		      size_t size, unsigned int attribute,
		      struct metal_io_region **io);

	/** Handle the vendor specific resource */
	int (*handle_rsc)(struct remoteproc *rproc, void *rsc, size_t len);

	/** Configure the remoteproc to make it ready to load and run the executable */
	int (*config)(struct remoteproc *rproc, void *data);

	/** Kick the remoteproc to run the application */
	int (*start)(struct remoteproc *rproc);

	/**
	 * Stop the remoteproc from running the application, the resource such as
	 * memory may not be off
	 */
	int (*stop)(struct remoteproc *rproc);

	/** Shutdown the remoteproc and release its resources */
	int (*shutdown)(struct remoteproc *rproc);

	/** Notify the remote */
	int (*notify)(struct remoteproc *rproc, uint32_t id);

	/**
	 * @brief Get remoteproc memory I/O region by either name, virtual
	 * address, physical address or device address.
	 *
	 * @param rproc		Pointer to remoteproc instance
	 * @param name		Memory name
	 * @param pa		Physical address
	 * @param da		Device address
	 * @param va		Virtual address
	 * @param size		Memory size
	 * @param buf		Pointer to remoteproc_mem struct object to store result
	 *
	 * @return remoteproc memory pointed by buf if success, otherwise NULL
	 */
	struct remoteproc_mem *(*get_mem)(struct remoteproc *rproc,
					  const char *name,
					  metal_phys_addr_t pa,
					  metal_phys_addr_t da,
					  void *va, size_t size,
					  struct remoteproc_mem *buf);
};

/* Remoteproc error codes */
#define RPROC_EBASE	0
#define RPROC_ENOMEM	(RPROC_EBASE + 1)
#define RPROC_EINVAL	(RPROC_EBASE + 2)
#define RPROC_ENODEV	(RPROC_EBASE + 3)
#define RPROC_EAGAIN	(RPROC_EBASE + 4)
#define RPROC_ERR_RSC_TAB_TRUNC (RPROC_EBASE + 5)
#define RPROC_ERR_RSC_TAB_VER   (RPROC_EBASE + 6)
#define RPROC_ERR_RSC_TAB_RSVD  (RPROC_EBASE + 7)
#define RPROC_ERR_RSC_TAB_VDEV_NRINGS (RPROC_EBASE + 9)
#define RPROC_ERR_RSC_TAB_NP          (RPROC_EBASE + 10)
#define RPROC_ERR_RSC_TAB_NS          (RPROC_EBASE + 11)
#define RPROC_ERR_LOADER_STATE (RPROC_EBASE + 12)
#define RPROC_EMAX	(RPROC_EBASE + 16)
#define RPROC_EPTR	(void *)(-1)
#define RPROC_EOF	(void *)(-1)

static inline long RPROC_PTR_ERR(const void *ptr)
{
	return (long)ptr;
}

static inline int RPROC_IS_ERR(const void *ptr)
{
	if ((unsigned long)ptr >= (unsigned long)(-RPROC_EMAX))
		return 1;
	else
		return 0;
}

static inline void *RPROC_ERR_PTR(long error)
{
	return (void *)error;
}

/**
 * @brief Remote processor states
 */
enum remoteproc_state {
	/** Remote is offline */
	RPROC_OFFLINE		= 0,
	/** Remote is configured */
	RPROC_CONFIGURED	= 1,
	/** Remote is ready to start */
	RPROC_READY		= 2,
	/** Remote is up and running */
	RPROC_RUNNING		= 3,
	/** Remote is suspended */
	RPROC_SUSPENDED		= 4,
	/** Remote is has error; need to recover */
	RPROC_ERROR		= 5,
	/** Remote is stopped */
	RPROC_STOPPED		= 6,
	/** Just keep this one at the end */
	RPROC_LAST		= 7,
};

/**
 * @brief Initializes remoteproc resource.
 *
 * @param rproc	Pointer to remoteproc instance
 * @param ops	Pointer to remoteproc operations
 * @param priv	Pointer to private data
 *
 * @return Created remoteproc pointer
 */
struct remoteproc *remoteproc_init(struct remoteproc *rproc,
				   const struct remoteproc_ops *ops,
				   void *priv);

/**
 * @brief Remove remoteproc resource
 *
 * @param rproc	Pointer to remoteproc instance
 *
 * @return 0 for success, negative value for failure
 */
int remoteproc_remove(struct remoteproc *rproc);

/**
 * @brief Initialize remoteproc memory
 *
 * @param mem	Pointer to remoteproc memory
 * @param name	Memory name (max string size \ref RPROC_MAX_NAME_LEN)
 * @param pa	Physical address
 * @param da	Device address
 * @param size	Memory size
 * @param io	Pointer to the I/O region
 */
void remoteproc_init_mem(struct remoteproc_mem *mem, const char *name,
			 metal_phys_addr_t pa, metal_phys_addr_t da,
			 size_t size, struct metal_io_region *io);

/**
 * @brief Add remoteproc memory
 *
 * @param rproc	Pointer to remoteproc
 * @param mem	Pointer to remoteproc memory
 */
void remoteproc_add_mem(struct remoteproc *rproc, struct remoteproc_mem *mem);

/**
 * @brief Get remoteproc memory I/O region with name
 *
 * @param rproc	Pointer to the remote processor
 * @param name	Name of the shared memory
 *
 * @return Metal I/O region pointer, NULL for failure
 */
struct metal_io_region *
remoteproc_get_io_with_name(struct remoteproc *rproc,
			    const char *name);

/**
 * @brief Get remoteproc memory I/O region with physical address
 *
 * @param rproc	Pointer to the remote processor
 * @param pa	Physical address
 *
 * @return Metal I/O region pointer, NULL for failure
 */
struct metal_io_region *
remoteproc_get_io_with_pa(struct remoteproc *rproc,
			  metal_phys_addr_t pa);

/**
 * @brief Get remoteproc memory I/O region with device address
 *
 * @param rproc		Pointer to the remote processor
 * @param da		Device address
 * @param offset	I/O region offset of the device address
 *
 * @return Metal I/O region pointer, NULL for failure
 */
struct metal_io_region *
remoteproc_get_io_with_da(struct remoteproc *rproc,
			  metal_phys_addr_t da,
			  unsigned long *offset);

/**
 * @brief Get remoteproc memory I/O region with virtual address
 *
 * @param rproc	Pointer to the remote processor
 * @param va	Virtual address
 *
 * @return Metal I/O region pointer, NULL for failure
 */
struct metal_io_region *
remoteproc_get_io_with_va(struct remoteproc *rproc,
			  void *va);

/**
 * @brief Remoteproc mmap memory
 *
 * @param rproc		Pointer to the remote processor
 * @param pa		Physical address pointer
 * @param da		Device address pointer
 * @param size		Size of the memory
 * @param attribute	Memory attribute
 * @param io		Pointer to the I/O region
 *
 * @return Pointer to the memory
 */
void *remoteproc_mmap(struct remoteproc *rproc,
		      metal_phys_addr_t *pa, metal_phys_addr_t *da,
		      size_t size, unsigned int attribute,
		      struct metal_io_region **io);

/**
 * @brief Parse and set resource table of remoteproc
 *
 * @param rproc		Pointer to remoteproc instance
 * @param rsc_table	Pointer to resource table
 * @param rsc_size	Resource table size
 *
 * @return 0 for success and negative value for errors
 */
int remoteproc_set_rsc_table(struct remoteproc *rproc,
			     struct resource_table *rsc_table,
			     size_t rsc_size);

/**
 * @brief This function configures the remote processor to get it
 * ready to load and run executable.
 *
 * @param rproc	Pointer to remoteproc instance to start
 * @param data	Configuration data
 *
 * @return 0 for success and negative value for errors
 */
int remoteproc_config(struct remoteproc *rproc, void *data);

/**
 * @brief This function starts the remote processor.
 * It assumes the firmware is already loaded.
 *
 * @param rproc	Pointer to remoteproc instance to start
 *
 * @return 0 for success and negative value for errors
 */
int remoteproc_start(struct remoteproc *rproc);

/**
 * @brief This function stops the remote processor but it
 * will not release its resource.
 *
 * @param rproc	Pointer to remoteproc instance
 *
 * @return 0 for success and negative value for errors
 */
int remoteproc_stop(struct remoteproc *rproc);

/**
 * @brief This function shuts down the remote processor and
 * releases its resources.
 *
 * @param rproc	Pointer to remoteproc instance
 *
 * @return 0 for success and negative value for errors
 */
int remoteproc_shutdown(struct remoteproc *rproc);

/**
 * @brief Loads the executable
 *
 * Expects the user application defines how to open the executable file and how
 * to get data from the executable file and how to load data to the target
 * memory.
 *
 * @param rproc		Pointer to the remoteproc instance
 * @param path		Optional path to the image file
 * @param store		Pointer to user defined image store argument
 * @param store_ops	Pointer to image store operations
 * @param img_info	Pointer to memory which stores image information used
 *			by remoteproc loader
 *
 * @return 0 for success and negative value for failure
 */
int remoteproc_load(struct remoteproc *rproc, const char *path,
		    void *store, const struct image_store_ops *store_ops,
		    void **img_info);

/**
 * @brief Loads the executable
 *
 * Expects the caller has loaded image data to local
 * memory and passed to the this function. If the function needs more
 * image data it will return the next expected image data offset and
 * the next expected image data length. If the function requires the
 * caller to download image data to the target memory, it will also
 * return the target physical address besides the offset and length.
 * This function can be used to load firmware in stream mode. In this
 * mode, you cannot do seek to the executable file. If the executable
 * is ELF, it cannot get the resource table section before it loads
 * the full ELF file. Furthermore, application usually don't store
 * the data which is loaded to local memory in streaming mode, and
 * thus, in this mode, it will load the binary to the target memory
 * before it gets the resource table. And thus, when calling this function
 * don't put the target executable memory in the resource table, as
 * this function will parse the resource table after it loads the binary
 * to target memory.
 *
 * @param rproc		Pointer to the remoteproc instance
 * @param img_data	Pointer to image data for remoteproc loader to parse
 * @param offset	Image data offset to the beginning of the image file
 * @param len		Image data length
 * @param img_info	Pointer to memory which stores image information used
 *			by remoteproc loader
 * @param pa		Pointer to the target memory physical address. If the
 *			next expected data doesn't need to load to the target
 *			memory, the function will set it to ANY.
 * @param io		Pointer to the io region. If the next expected data
 *			doesn't need to load to the target memory, the function
 *			will set it to NULL.
 * @param noffset	Pointer to the next image data offset to the beginning
 *			of the image file needs to load to local or to the
 *			target memory.
 * @param nlen		Pointer to the next image data length needs to load to
 *			local or to the target memory.
 * @param nmlen		Pointer to the memory size. It is only used when the
 *			next expected data is going to be loaded to the target
 *			memory. E.g. in ELF, it is possible that loadable
 *			segment in memory is larger that the segment data in
 *			the ELF file. In this case, application will need to
 *			pad the rest of the memory with padding.
 * @param padding	Pointer to the padding value. It is only used when the
 *			next expected data is going to be loaded to the target
 *			memory and the target memory size is larger than the
 *			segment data in the executable file.
 *
 * @return 0 for success and negative value for failure
 */
int remoteproc_load_noblock(struct remoteproc *rproc,
			    const void *img_data, size_t offset, size_t len,
			    void **img_info,
			    metal_phys_addr_t *pa, struct metal_io_region **io,
			    size_t *noffset, size_t *nlen,
			    size_t *nmlen, unsigned char *padding);

/**
 * @brief Allocate notifyid for resource
 *
 * @param rproc	Pointer to the remoteproc instance
 * @param start	Start of the id range
 * @param end	End of the id range
 *
 * @return Allocated notify id
 */
unsigned int remoteproc_allocate_id(struct remoteproc *rproc,
				    unsigned int start,
				    unsigned int end);

/**
 * @brief Create virtio device, it returns pointer to the created virtio
 * device.
 *
 * @param rproc		Pointer to the remoteproc instance
 * @param vdev_id	virtio device ID
 * @param role		virtio device role
 * @param rst_cb	virtio device reset callback
 *
 * @return Pointer to the created virtio device, NULL for failure.
 */
struct virtio_device *
remoteproc_create_virtio(struct remoteproc *rproc,
			 int vdev_id, unsigned int role,
			 void (*rst_cb)(struct virtio_device *vdev));

/**
 * @brief Remove virtio device
 *
 * @param rproc	Pointer to the remoteproc instance
 * @param vdev	Pointer to the virtio device
 */
void remoteproc_remove_virtio(struct remoteproc *rproc,
			      struct virtio_device *vdev);

/**
 * @brief remoteproc is got notified, it will check its subdevices
 * for the notification
 *
 * @param rproc		Pointer to the remoteproc instance
 * @param notifyid	Notification id
 *
 * @return 0 for succeed, negative value for failure
 */
int remoteproc_get_notification(struct remoteproc *rproc,
				uint32_t notifyid);
#if defined __cplusplus
}
#endif

#endif /* REMOTEPROC_H_ */
