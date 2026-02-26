/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**************************************************************************
 * FILE NAME
 *
 *       remoteproc_loader.h
 *
 * COMPONENT
 *
 *         OpenAMP stack.
 *
 * DESCRIPTION
 *
 *       This file provides definitions for remoteproc loader
 *
 *
 **************************************************************************/
#ifndef REMOTEPROC_LOADER_H_
#define REMOTEPROC_LOADER_H_

#include <metal/io.h>
#include <metal/list.h>
#include <metal/sys.h>
#include <openamp/remoteproc.h>

#if defined __cplusplus
extern "C" {
#endif

/* Loader feature macros */
#define SUPPORT_SEEK 1UL

/* Remoteproc loader any address */
#define RPROC_LOAD_ANYADDR ((metal_phys_addr_t)-1)

/* Remoteproc loader Executable Image Parsing States */
/* Remoteproc loader parser initial state */
#define RPROC_LOADER_NOT_READY      0x0L
/* Remoteproc loader ready to load, even it can be not finish parsing */
#define RPROC_LOADER_READY_TO_LOAD  0x10000L
/* Remoteproc loader post data load */
#define RPROC_LOADER_POST_DATA_LOAD 0x20000L
/* Remoteproc loader finished loading */
#define RPROC_LOADER_LOAD_COMPLETE  0x40000L
/* Remoteproc loader state mask */
#define RPROC_LOADER_MASK           0x00FF0000L
/* Remoteproc loader private mask */
#define RPROC_LOADER_PRIVATE_MASK   0x0000FFFFL
/* Remoteproc loader reserved mask */
#define RPROC_LOADER_RESERVED_MASK  0x0F000000L

/** @brief User-defined image store operations */
struct image_store_ops {
	/** User-defined callback to open the "firmware" to prepare loading */
	int (*open)(void *store, const char *path, const void **img_data);

	/** User-defined callback to close the "firmware" to clean up after loading */
	void (*close)(void *store);

	/** User-defined callback to load the firmware contents to target memory or local memory */
	int (*load)(void *store, size_t offset, size_t size,
		    const void **data,
		    metal_phys_addr_t pa,
		    struct metal_io_region *io, char is_blocking);

	/** Loader supported features. e.g. seek */
	unsigned int features;
};

/** @brief Loader operations */
struct loader_ops {
	/** Define how to get the executable headers */
	int (*load_header)(const void *img_data, size_t offset, size_t len,
			   void **img_info, int last_state,
			   size_t *noffset, size_t *nlen);

	/** Define how to load the target data */
	int (*load_data)(struct remoteproc *rproc,
			 const void *img_data, size_t offset, size_t len,
			 void **img_info, int last_load_state,
			 metal_phys_addr_t *da,
			 size_t *noffset, size_t *nlen,
			 unsigned char *padding, size_t *nmemsize);

	/**
	 * Define how to get the resource table target address, offset to the ELF
	 * image file and size of the resource table
	 */
	int (*locate_rsc_table)(void *img_info, metal_phys_addr_t *da,
				size_t *offset, size_t *size);

	/** Define how to release the loader */
	void (*release)(void *img_info);

	/** Get entry address */
	metal_phys_addr_t (*get_entry)(void *img_info);

	/** Get load state from the image information */
	int (*get_load_state)(void *img_info);
};

#if defined __cplusplus
}
#endif

#endif /* REMOTEPROC_LOADER_H_ */
