/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RPMSG_RETARGET_H
#define RPMSG_RETARGET_H

#include <metal/mutex.h>
#include <openamp/open_amp.h>
#include <stdint.h>

#if defined __cplusplus
extern "C" {
#endif

/* File Operations System call definitions */
#define OPEN_SYSCALL_ID  0x1UL
#define CLOSE_SYSCALL_ID 0x2UL
#define WRITE_SYSCALL_ID 0x3UL
#define READ_SYSCALL_ID  0x4UL
#define ACK_STATUS_ID    0x5UL

#define TERM_SYSCALL_ID  0x6UL

#define DEFAULT_PROXY_ENDPOINT  0xFFUL

struct rpmsg_rpc_data;

typedef int (*rpmsg_rpc_poll)(void *arg);
typedef void (*rpmsg_rpc_shutdown_cb)(struct rpmsg_rpc_data *rpc);

struct rpmsg_rpc_syscall_header {
	int32_t int_field1;
	int32_t int_field2;
	uint32_t data_len;
};

struct rpmsg_rpc_syscall {
	uint32_t id;
	struct rpmsg_rpc_syscall_header args;
};

struct rpmsg_rpc_data {
	struct rpmsg_endpoint ept;
	int ept_destroyed;
	atomic_flag nacked;
	void *respbuf;
	size_t respbuf_len;
	rpmsg_rpc_poll poll;
	void *poll_arg;
	rpmsg_rpc_shutdown_cb shutdown_cb;
	metal_mutex_t lock;
	struct metal_spinlock buflock;
};

/**
 * @internal
 *
 * @brief Initialize RPMsg remote procedure call
 *
 * This function is to initialize the remote procedure call
 * global data. RPMsg RPC will send request to remote and
 * wait for callback.
 *
 * @param rpc		Pointer to the global remote procedure call data
 * @param rdev		Pointer to the rpmsg device
 * @param ept_name	Name of the endpoint used by RPC
 * @param ept_addr	Address of the endpoint used by RPC
 * @param ept_raddr	Remote address of the endpoint used by RPC
 * @param poll_arg	Pointer to poll function argument
 * @param poll		Poll function
 * @param shutdown_cb	Shutdown callback function
 *
 * @return 0 for success, and negative value for failure.
 */
int rpmsg_rpc_init(struct rpmsg_rpc_data *rpc,
		   struct rpmsg_device *rdev,
		   const char *ept_name, uint32_t ept_addr,
		   uint32_t ept_raddr,
		   void *poll_arg, rpmsg_rpc_poll poll,
		   rpmsg_rpc_shutdown_cb shutdown_cb);

/**
 * @internal
 *
 * @brief Release RPMsg remote procedure call
 *
 * This function is to release remoteproc procedure call
 * global data.
 *
 * @param rpc	Pointer to the global remote procedure call
 */
void rpmsg_rpc_release(struct rpmsg_rpc_data *rpc);

/**
 * @internal
 *
 * @brief Request RPMsg RPC call
 *
 * This function sends RPC request it will return with the length
 * of data and the response buffer.
 *
 * @param rpc		Pointer to remoteproc procedure call data struct
 * @param req		Pointer to request buffer
 * @param len		Length of the request data
 * @param resp		Pointer to where store the response buffer
 * @param resp_len	Length of the response buffer
 *
 * @return Length of the received response, negative value for failure.
 */
int rpmsg_rpc_send(struct rpmsg_rpc_data *rpc,
		   void *req, size_t len,
		   void *resp, size_t resp_len);

/**
 * @internal
 *
 * @brief Set default RPMsg RPC data
 *
 * The default RPC data is used to redirect standard C file operations
 * to RPMsg channels.
 *
 * @param rpc	Pointer to remoteproc procedure call data struct
 */
void rpmsg_set_default_rpc(struct rpmsg_rpc_data *rpc);

#if defined __cplusplus
}
#endif

#endif /* RPMSG_RETARGET_H */
