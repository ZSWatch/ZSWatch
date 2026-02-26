/*
 * Copyright (c) 2025, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NRFX_GPPI_ROUTES_H__
#define NRFX_GPPI_ROUTES_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <hal/nrf_ppib.h>
/**
 * @defgroup nrfx_gppi_routes GPPI Routes
 * @{
 * @ingroup nrfx_gppi
 *
 * @brief Structures and macros for handling GPPI routes in multi-domain DPPI system.
 */

/** @brief PPIB node. */
typedef struct {
    /** Pointer to channel mask. */
	nrfx_atomic_t * p_channels;
    /** Two PPIB register sets that form a bridge. */
	NRF_PPIB_Type * p_reg[2];
} nrfx_gppi_node_ppib_t;

/** @brief DPPI node. */
typedef struct {
    /** Pointer to channel mask. */
	nrfx_atomic_t * p_channels;
    /** Pointer to group mask. */
	nrfx_atomic_t * p_group_channels;
    /** DPPIC register set. */
	NRF_DPPIC_Type * p_reg;
} nrfx_gppi_node_dppi_t;

/** @brief Generic node. */
typedef struct {
    /** Pointer to channel mask. */
	nrfx_atomic_t *p_channels;
} nrfx_gppi_node_generic_t;

/** @brief Node types. */
typedef enum {
	NRFX_GPPI_NODE_DPPI,
	NRFX_GPPI_NODE_PPIB,
} nrfx_gppi_node_type_t;

/** @brief Node structure. */
typedef struct {
    /** Node type. */
	nrfx_gppi_node_type_t type;
    /** Domain ID. */
	uint8_t domain_id;
#if NRFX_CHECK(NRFX_GPPI_FIXED_CONNECTIONS)
    /** Channel offset. */
	uint8_t ch_off[2];
#endif
    /** Node specific data. */
	union {
		nrfx_gppi_node_dppi_t dppi;
		nrfx_gppi_node_ppib_t ppib;
		nrfx_gppi_node_generic_t generic;
	};
} nrfx_gppi_node_t;

/** @brief Route in a DPPI system. */
typedef struct {
    /** Array of nodes that form a route. */
	const nrfx_gppi_node_t * const * p_nodes;
    /** Number of nodes in a route. */
	uint8_t len;
} nrfx_gppi_route_t;

/** @brief Macro for defining a DPPI node.
 *
 * @param _id Node ID.
 * @param _domain_id Domain ID.
 */
#define NRFX_GPPI_DPPI_NODE_DEFINE(_id, _domain_id)                         \
[NRFX_GPPI_NODE_DPPIC##_id] = {                                             \
		.type = NRFX_GPPI_NODE_DPPI,                                        \
		.domain_id = _domain_id,                                            \
		.dppi = {                                                           \
			.p_channels = &channels[NRFX_GPPI_NODE_DPPIC##_id],             \
			.p_group_channels = &group_channels[NRFX_GPPI_NODE_DPPIC##_id], \
			.p_reg = NRF_DPPIC##_id                                         \
		}                                                                   \
	}

/** @brief Macro for defining a PPIB node.
 *
 * @param _id1 PPIB ID of the first PPIB.
 * @param _id2 PPIB ID of the second PPIB.
 */
#define NRFX_GPPI_PPIB_NODE_DEFINE(_id1, _id2)                              \
[NRFX_GPPI_NODE_PPIB##_id1##_##_id2] = {                                    \
		.type = NRFX_GPPI_NODE_PPIB,                                        \
		.domain_id = NRFX_GPPI_NODE_PPIB##_id1##_##_id2,                    \
		.ppib = {                                                           \
			.p_channels = &channels[NRFX_GPPI_NODE_PPIB##_id1##_##_id2],    \
			.p_reg = {NRF_PPIB##_id1, NRF_PPIB##_id2}                       \
		}                                                                   \
	}

/** @brief Macro for creating a route.
 *
 * @param _name Route name.
 * @param _nodes List of nodes in parenthesis.
 */
#define NRFX_GPPI_ROUTE_DEFINE(_name, _nodes)                               \
{                                                                           \
	.p_nodes = (const nrfx_gppi_node_t * const[]){ NRFX_DEBRACKET _nodes},  \
	.len = 1 + NRFX_NUM_VA_ARGS_LESS_1 _nodes,                              \
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRFX_GPPI_ROUTES_H__  */
