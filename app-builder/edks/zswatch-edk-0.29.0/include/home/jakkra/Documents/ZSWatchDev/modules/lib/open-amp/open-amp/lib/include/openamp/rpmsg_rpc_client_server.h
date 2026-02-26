/*
 * Copyright (c) 2021, L&T Technology Services Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RPMSG_RPC_CLIENT_SERVER_H
#define RPMSG_RPC_CLIENT_SERVER_H

#include <openamp/open_amp.h>
#include <metal/compiler.h>

#if defined __cplusplus
extern "C" {
#endif

#define RPMSG_RPC_OK		0
#define RPMSG_RPC_INVALID_ID	(-1L)
#define RPMSG_RPC_SERVICE_NAME "rpmsg-rpc"

/* RPMSG_BUFFER_SIZE = 512
 * sizeof(struct rpmsg_hdr) = 16
 * RPMSG_BUFFER_SIZE - sizeof(struct rpmsg_hdr) - 1 = 495
 * Aligning to 64 bits -> 488UL
 */
#define MAX_BUF_LEN	488UL
#define MAX_FUNC_ID_LEN sizeof(uint32_t)

struct rpmsg_rpc_clt;
struct rpmsg_rpc_svr;

typedef void (*rpmsg_rpc_shutdown_cb)(struct rpmsg_rpc_clt *rpc);
typedef void (*app_cb)(struct rpmsg_rpc_clt *rpc, int status, void *data,
		       size_t len);
typedef int (*rpmsg_rpc_syscall_cb)(void *data, struct rpmsg_rpc_svr *rpcs);

/**
 * struct rpmsg_rpc_request - rpc request message
 *
 * @param id		service id
 * @param params	request params
 *
 */
struct rpmsg_rpc_request {
	uint32_t id;
	unsigned char params[MAX_BUF_LEN];
};

/** @brief RPC request message */
METAL_PACKED_BEGIN
struct rpmsg_rpc_answer {
	/** Service ID */
	uint32_t id;

	/** Status of RPC */
	int32_t status;

	/** Answer params */
	unsigned char params[MAX_BUF_LEN];
} METAL_PACKED_END;

/** @brief Table for services */
struct rpmsg_rpc_services {
	/** Service ID */
	uint32_t id;

	/** ID callback */
	rpmsg_rpc_syscall_cb cb_function;
};

/** @brief Table for client services */
struct rpmsg_rpc_client_services {
	/** Service ID */
	uint32_t id;

	/** ID callback */
	app_cb cb;
};

/**
 * @brief Server remote procedure call data
 *
 * RPMsg RPC will send request to endpoint
 */
struct rpmsg_rpc_svr {
	/** RPMsg destination endpoint structure */
	struct rpmsg_endpoint ept;

	/** Service table */
	const struct rpmsg_rpc_services *services;

	/** Number of services */
	unsigned int n_services;
};

/**
 * @brief Client remote procedure call data
 *
 * RPMsg RPC will send request to remote and
 * wait for callback.
 */
struct rpmsg_rpc_clt {
	/** RPMsg endpoint associated with the call */
	struct rpmsg_endpoint ept;

	/** Shutdown callback function */
	rpmsg_rpc_shutdown_cb shutdown_cb;

	/** Service table */
	const struct rpmsg_rpc_client_services *services;

	/** Number of services */
	unsigned int n_services;
};

/**
 * @internal
 *
 * @brief Release RPMsg remote procedure call
 *
 * This function is to release remoteproc procedure call service
 *
 * @param rpc	Pointer to the client remote procedure call data
 */
void rpmsg_rpc_client_release(struct rpmsg_rpc_clt *rpc);

/**
 * @internal
 *
 * @brief Initialize RPMsg remote procedure call
 *
 * This function is to initialize the remote procedure call
 * client data. RPMsg RPC will send request to remote and
 * wait for callback and load services to table
 *
 * @param rpc		Pointer to the client remote procedure call data
 * @param rdev		Pointer to the rpmsg device
 * @param shutdown_cb	Shutdown callback function
 * @param services	Pointer to service table
 * @param len		Length of table
 *
 * @return 0 for success, and negative value for failure
 */
int rpmsg_rpc_client_init(struct rpmsg_rpc_clt *rpc,
			  struct rpmsg_device *rdev,
			  rpmsg_rpc_shutdown_cb shutdown_cb,
			  const struct rpmsg_rpc_client_services *services,
			  int len);

/**
 * @internal
 *
 * @brief Initialize RPMsg rpc for server
 *
 * This function create endpoint and loads services into table
 *
 * @param rpcs				Pointer to the server rpc
 * @param rdev				Pointer to the rpmsg device
 * @param services			Pointer to service table
 * @param len				Length of table
 * @param rpmsg_service_server_unbind	Unbind function callback
 *
 * @return 0 for success, and negative value for failure
 */
int rpmsg_rpc_server_init(struct rpmsg_rpc_svr *rpcs, struct rpmsg_device *rdev,
			  const struct rpmsg_rpc_services *services, int len,
			  rpmsg_ns_unbind_cb rpmsg_service_server_unbind);

/**
 * @internal
 *
 * @brief Request RPMsg RPC call
 *
 * @param rpc			Pointer to client remoteproc procedure call
 *				data
 * @param rpc_id		Function id
 * @param request_param		Pointer to request buffer
 * @param req_param_size	Length of the request data
 *
 * @return Length of the received response, negative value for failure.
 */
int rpmsg_rpc_client_send(struct rpmsg_rpc_clt *rpc,
			  uint32_t rpc_id, void *request_param,
			  size_t req_param_size);

/**
 * @internal
 *
 * @brief Request RPMsg RPC call
 *
 * This function sends RPC request
 *
 * @param rpcs		Pointer to server rpc data
 * @param rpc_id	Function id
 * @param status	Status of rpc
 * @param request_param	Pointer to request buffer
 * @param param_size	Length of the request data
 *
 * @return Length of the received response, negative value for failure.
 */
int rpmsg_rpc_server_send(struct rpmsg_rpc_svr *rpcs, uint32_t rpc_id,
			  int status, void *request_param,
			  size_t param_size);

#if defined __cplusplus
}
#endif

#endif /* RPMSG_RPC_CLIENT_SERVER_H */
