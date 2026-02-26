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

#ifndef NRFX_GPPI_H__
#define NRFX_GPPI_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_gppi Generic PPI layer
 * @{
 * @ingroup nrfx
 * @ingroup nrf_dppi
 * @ingroup nrf_ppi
 *
 * @brief Helper layer that provides the common functionality of PPI system.
 *
 * This is the only place where PPI system resources are managed.
 */

#if defined(DPPI_TYPE_IPCT) || defined(DPPI_TYPE_PPIB) || defined(__NRFX_DOXYGEN__)
/** @brief Flag indicating that DPPI system consists of multiple domains (instances). */
#define NRFX_GPPI_MULTI_DOMAIN 1
#endif

#if defined(DPPI_TYPE_IPCT) || defined(__NRFX_DOXYGEN__)
/** @brief Flag indicating that there are fixed connections between PPIB and DPPI channels. */
#define NRFX_GPPI_FIXED_CONNECTIONS 1
#endif

#if NRFX_CHECK(NRFX_GPPI_MULTI_DOMAIN) || defined(__NRFX_DOXYGEN__)
#include "nrfx_gppi_routes.h"
/** @brief Handle used for PPI connection. */
typedef uint32_t nrfx_gppi_handle_t;
/** @brief Handle used for PPI group resource. */
typedef uint32_t nrfx_gppi_group_handle_t;
#else
typedef uint8_t nrfx_gppi_handle_t;
typedef uint8_t nrfx_gppi_group_handle_t;
#endif

/** @brief Structure which holds resources for PPI system. */
typedef struct {
#if NRFX_CHECK(NRFX_GPPI_MULTI_DOMAIN)
    /** Array with routes in the system. */
    const nrfx_gppi_route_t *routes;
    /** Pointer to two dimensional array with map of routes. */
    const nrfx_gppi_route_t ***route_map;
    /** Array with nodes in the system. */
    const nrfx_gppi_node_t *nodes;
#else
    /** Mask of available PPI channels in a single instance (D)PPI system. */
    nrfx_atomic_t ch_mask;
    /** Mask of available PPI groups in a single instance (D)PPI system. */
    nrfx_atomic_t group_mask;
#endif
} nrfx_gppi_t;

/**
 * @brief Function for initializing GPPI with a structure with resources.
 *
 * Function shall be called as early as possible to allow resource allocation. @p p_instance
 * contains channel resources which need to be initialized before calling that function.
 *
 * @param[in] p_instance Instance.
 */
void nrfx_gppi_init(nrfx_gppi_t * p_instance);

/**
 * @brief Function for getting the domain ID from the group handle.
 *
 * @param[in] handle Group handle.
 *
 * @return Domain ID.
 */
#if NRFX_CHECK(NRFX_GPPI_MULTI_DOMAIN)
uint32_t nrfx_gppi_group_domain_id_get(nrfx_gppi_group_handle_t handle);
#else
NRFX_STATIC_INLINE uint32_t nrfx_gppi_group_domain_id_get(nrfx_gppi_group_handle_t handle);
#endif

/**
 * @brief Function for getting the domain ID from the peripheral register address.
 *
 * @param[in] addr Address.
 *
 * @return Domain ID.
 */
#if NRFX_CHECK(NRFX_GPPI_MULTI_DOMAIN)
uint32_t nrfx_gppi_domain_id_get(uint32_t addr);
#else
NRFX_STATIC_INLINE uint32_t nrfx_gppi_domain_id_get(uint32_t addr);
#endif

/**
 * @brief Function for getting the channel that is used for an endpoint.
 *
 * @param[in] ep Endpoint address (task or event register).
 *
 * @retval non-negative Configured channel.
 * @retval -EINVAL      Endpoint does not have channel.
 */
#ifdef PPI_PRESENT
int nrfx_gppi_ep_channel_get(uint32_t ep);
#else
NRFX_STATIC_INLINE int nrfx_gppi_ep_channel_get(uint32_t ep);
#endif

/**
 * @brief Function for getting one of a channels that is used in a connection.
 *
 * A connection may consists of a number of nodes which are domains with a unique DPPI or PPIB nodes.
 * @ref nrfx_gppi_domain_id_get can be used to get the domain ID for a given peripheral. Node ID for
 * PPIB is target-specific and can be found in a target-specific header file.
 *
 * @param[in] handle    Connection handle.
 * @param[in] domain_id Domain or PPIB node ID.
 *
 * @retval non-negative Configured channel.
 * @retval -EINVAL      Invalid input arguments.
 */
int nrfx_gppi_domain_channel_get(nrfx_gppi_handle_t handle, uint32_t domain_id);

/**
 * @brief Function for allocating and setup a connection between two domains.
 *
 * In case of a multi domain DPPI, function is allocating all needed resources and configures
 * PPIB bridges. For single domain cases (PPI or DPPI) only a channel is allocated.
 *
 * @param[in]  producer Domain that will produce (publish) events. Not used in single domain case.
 * @param[in]  consumer Domain that will consume (subsribe to) events. Not used in single domain case.
 * @param[out] p_handle Handle used to control the connection.
 *
 * @retval 0       On successful connection allocation.
 * @retval -ENOMEM There is not enough resources to allocate the connection.
 */
int nrfx_gppi_domain_conn_alloc(uint32_t producer, uint32_t consumer,
                                nrfx_gppi_handle_t * p_handle);

/**
 * @brief Function for attaching an endpoint to the connection.
 *
 * Endpoint can be task, event, publish or subscribe register address. In order to allow attaching
 * the endpoint it must belong to the route that connection is using. On SoC with PPI there are
 * limitations regarding how many endpoints can be attached to a handle (channel) - 1 event and
 * 2 tasks (1 if FORK is not present).
 *
 * @param[in] ep     Endpoint address (task or event register).
 * @param[in] handle Connection handle.
 *
 * @retval 0       Endpoint attached.
 * @retval -EINVAL Endpoint cannot be attached, i.e. it does not belong to the route.
 */
int nrfx_gppi_ep_attach(uint32_t ep, nrfx_gppi_handle_t handle);

/**
 * @brief Function for attaching an endpoint to the channel.
 *
 * Endpoint can be task, event, publish or subscribe register address.
 *
 * @param[in] ep      Endpoint address (task or event register).
 * @param[in] channel Channel.
 *
 * @retval 0       Endpoint attached.
 * @retval -EINVAL Endpoint cannot be attached, i.e. endpoints already configured in case of PPI.
 */
int nrfx_gppi_ep_to_ch_attach(uint32_t ep, uint8_t channel);

/**
 * @brief Function for allocating a connection between Task and Event.
 *
 * Function performs @ref nrfx_gppi_domain_conn_alloc followed by the configuration of endpoints.
 *
 * @param[in]  eep      Event endpoint address.
 * @param[in]  tep      Task endpoint address.
 * @param[out] p_handle Handle used to control the connection.
 *
 * @retval 0       Successful connection allocation.
 * @retval -ENOMEM There is not enough resources to allocate the connection.
 */
int nrfx_gppi_conn_alloc(uint32_t eep, uint32_t tep, nrfx_gppi_handle_t * p_handle);

/**
 * @brief Function for freeing a connection between domains.
 *
 * For connection within a single domain a channel is freed. For cross-domain connection
 * channels in bridges (PPIB) are cleared and all used resources are freed.. Connection shall be
 * disabled prior to clearing.
 *
 * @param[in] handle Connection handle.
 */
void nrfx_gppi_domain_conn_free(nrfx_gppi_handle_t handle);

/**
 * @brief Function for clearing an endpoint.
 *
 * Remove endpoint from DPPI channel. For PPI it searches all channel configurations and clears
 * the first configuration where the endpoint was used. For PPI it is more time consuming so in
 * PPI specific code @ref nrfx_gppi_ep_ch_clear can be used.
 *
 * @param[in] ep Endpoint address (task or event register).
 */
void nrfx_gppi_ep_clear(uint32_t ep);

/**
 * @brief Function for clearing an endpoint.
 *
 * For DPPI it is the same as @ref nrfx_gppi_ep_clear. For PPI it clears @p channel configuration.
 *
 * @param[in] ep      Endpoint address (task or event register).
 * @param[in] channel Channel.
 */
void nrfx_gppi_ep_ch_clear(uint32_t ep, uint8_t channel);

/**
 * @brief Function for enabling an endpoint.
 *
 * In case of PPI function enables the channel which is used by the endpoint. In case of DPPI,
 * function is only enabling an endpoint and does not control the channel to which the endpoint is
 * attached.
 *
 * @param[in] ep Endpoint address (task or event register).
 */
void nrfx_gppi_ep_enable(uint32_t ep);

/**
 * @brief Function for disabling an endpoint.
 *
 * In case of PPI function disables the channel which is used by the endpoint. In case of DPPI,
 * function is only disabling the endpoint and does not control the channel to which the endpoint is
 * attached.
 *
 * @param[in] ep Endpoint address (task or event register).
 */
void nrfx_gppi_ep_disable(uint32_t ep);

/**
 * @brief Function for clearing and freeing a connection.
 *
 * Function performs @ref nrfx_gppi_domain_conn_free followed by clearing of endpoints.
 *
 * @param[in] eep    Event endpoint.
 * @param[in] tep    Task endpoint.
 * @param[in] handle Connection handle.
 */
void nrfx_gppi_conn_free(uint32_t eep, uint32_t tep, nrfx_gppi_handle_t handle);

/**
 * @brief Function for enabling a connection.
 *
 * Function enables channels in every DPPI instance which belongs to the connection.
 *
 * @param[in] handle Connection handle.
 */
void nrfx_gppi_conn_enable(nrfx_gppi_handle_t handle);

/**
 * @brief Function for disabling a connection.
 *
 * Function disables channels in every DPPI instance which belongs to the connection.
 *
 * @param[in] handle Connection handle.
 */
void nrfx_gppi_conn_disable(nrfx_gppi_handle_t handle);

/**
 * @brief Function for enabling channels in a domain.
 *
 * @param[in] domain_id Domain ID. @ref nrfx_gppi_domain_id_get can be used to get @p domain_id.
 * @param[in] ch_mask   Channel mask.
 */
void nrfx_gppi_channels_enable(uint32_t domain_id, uint32_t ch_mask);

/**
 * @brief Function for disabling channels in a domain.
 *
 * @param[in] domain_id Domain ID. @ref nrfx_gppi_domain_id_get can be used to get @p domain_id.
 * @param[in] ch_mask   Channel mask.
 */
void nrfx_gppi_channels_disable(uint32_t domain_id, uint32_t ch_mask);

/**
 * @brief Function for checking if a channel is enabled.
 *
 * @param[in] domain_id Domain ID. @ref nrfx_gppi_domain_id_get can be used to get @p domain_id.
 * @param[in] ch        Channel.
 *
 * @retval true  Channel is enabled.
 * @retval false Channel is disabled.
 */
bool nrfx_gppi_chan_is_enabled(uint32_t domain_id, uint32_t ch);

/**
 * @brief Function for enabling a channel used by the endpoint.
 *
 * @param[in] ep Endpoint address (task or event register).
 *
 * @retval 0       Successful operation.
 * @retval -EINVAL Endpoint has no channel.
 */
NRFX_STATIC_INLINE int nrfx_gppi_ep_chan_enable(uint32_t ep);

/**
 * @brief Function for checking if a channel assigned to the endpoint is enabled.
 *
 * @param[in] ep Endpoint address (task or event register).
 *
 * @retval 0       Channel is enabled.
 * @retval -EINVAL Endpoint has no channel.
 * @retval -EAGAIN Channel is disabled.
 */
NRFX_STATIC_INLINE int nrfx_gppi_ep_is_enabled(uint32_t ep);

/**
 * @brief Function for disabling a channel used by the endpoint.
 *
 * @param[in] ep Endpoint address (task or event register).
 *
 * @retval 0       Successful operation.
 * @retval -EINVAL Endpoint has no channel.
 */
NRFX_STATIC_INLINE int nrfx_gppi_ep_chan_disable(uint32_t ep);

/**
 * @brief Function for allocating a group for given set of endpoints.
 *
 * Group can only enable or disable channels in a single DPPI so all endpoints must
 * belong to a single domain. Use @ref nrfx_gppi_domain_id_get with one of the endpoints that is
 * intended to be added to that group to get @p domain_id.
 *
 * @param[in] domain_id Domain ID for the group.
 * @param[out] p_handle Location where handle is written.
 *
 * @retval 0       Successful allocation of a group. @p handle can be used.
 * @retval -EINVAL Endpoints are not from the same domain.
 * @retval -ENOMEM Failed to allocate a free group channel.
 */
int nrfx_gppi_group_alloc(uint32_t domain_id, nrfx_gppi_group_handle_t * p_handle);

/**
 * @brief Function for freeing a group.
 *
 * @param[in] handle Group handle.
 */
void nrfx_gppi_group_free(nrfx_gppi_group_handle_t handle);

/**
 * @brief Function for adding a channel to a group.
 *
 * @ref nrfx_gppi_ep_channel_get can be used on a configured endpoint to retrieve the channel that
 * shall be added to the group.
 *
 * @param[in] handle  Group handle.
 * @param[in] channel Channel.
 */
void nrfx_gppi_group_ch_add(nrfx_gppi_group_handle_t handle, uint32_t channel);

/**
 * @brief Function for removing a channel from a group.
 *
 * @ref nrfx_gppi_ep_channel_get can be used on a configured endpoint to retrieve the channel that
 * shall be added to the group.
 *
 * @param[in] handle  Group handle.
 * @param[in] channel Channel.
 */
void nrfx_gppi_group_ch_remove(nrfx_gppi_group_handle_t handle, uint32_t channel);

/**
 * @brief Function for adding a configured endpoint to a group.
 *
 * Endpoint must be from the same domain as group and it must already be configured to use
 * a channel.
 *
 * @param[in] handle Group handle.
 * @param[in] ep     Endpoint address (task or event register).
 *
 * @retval 0       Successful operation.
 * @retval -EINVAL Failed to extend the group.
 */
NRFX_STATIC_INLINE int nrfx_gppi_group_ep_add(nrfx_gppi_group_handle_t handle, uint32_t ep);

/**
 * @brief Function for removing a configured endpoint from a group.
 *
 * Endpoint must be added to the group and it must be configured to use a channel.
 *
 * @param[in] handle Group handle.
 * @param[in] ep     Endpoint address (task or event register).
 *
 * @retval 0       Successful operation.
 * @retval -EINVAL Failed to remove from the group.
 */
NRFX_STATIC_INLINE int nrfx_gppi_group_ep_remove(nrfx_gppi_group_handle_t handle, uint32_t ep);

/**
 * @brief Function for getting a mask with channels which are assigned to the group.
 *
 * @param[in] handle Group handle.
 *
 * @return Mask with bits set for channels that belong to a group.
 */
uint32_t nrfx_gppi_group_channels_get(nrfx_gppi_group_handle_t handle);

/**
 * @brief Function for enabling a group.
 *
 * @param[in] handle Group handle.
 */
void nrfx_gppi_group_enable(nrfx_gppi_group_handle_t handle);

/**
 * @brief Function for disabling a group.
 *
 * @param[in] handle Group handle.
 */
void nrfx_gppi_group_disable(nrfx_gppi_group_handle_t handle);

/**
 * @brief Function for getting the enable group task address.
 *
 * @param[in] handle Group handle.
 *
 * @retval Address of the task register.
 */
uint32_t nrfx_gppi_group_task_en_addr(nrfx_gppi_group_handle_t handle);

/** @brief Function for getting the disable group task address.
 *
 * @param[in] handle Group handle.
 *
 * @retval Address of the task register.
 */
uint32_t nrfx_gppi_group_task_dis_addr(nrfx_gppi_group_handle_t handle);

/**
 * @brief Function for allocating a group channel in a specific node of the DPPI system.
 *
 * Function can be used to allocate a resource and use it outside of this driver.
 *
 * @note @p node_id parameter is used only in the system with multiple domains.
 * @ref nrfx_gppi_domain_id_get or target-specific header can be used to get the node ID.
 *
 * @param[in] node_id Target-specific identifier of the DPPI system node (DPPIC or PPIB).
 *
 * @retval non-negative Allocated channel.
 * @retval -ENOMEM      Failed to allocate a channel.
 */
int nrfx_gppi_channel_alloc(uint32_t node_id);

/**
 * @brief Function for freeing a channel in a specific node of the DPPI system.
 *
 * @p channel must be allocated earlier using @ref nrfx_gppi_channel_alloc. @p node_id must be
 * the same as the one used in @ref nrfx_gppi_channel_alloc for allocating that resource.
 *
 * @param[in] node_id Target-specific identifier of the DPPI system node (DPPIC or PPIB).
 * @param[in] channel Channel.
 */
void nrfx_gppi_channel_free(uint32_t node_id, uint8_t channel);

/**
 * @brief Function for allocating a group channel in a specific node of the DPPI system.
 *
 * Function can be used to allocate a resource and use it outside of this driver.
 *
 * @note @p domain_id parameter is used only in the system with multiple domains.
 * @ref nrfx_gppi_domain_id_get or target-specific header can be used to get the node ID.
 *
 * @param[in] domain_id Target-specific identifier of the DPPI system node.
 *
 * @retval non-negative Allocated channel.
 * @retval -ENOMEM      Failed to allocate a channel.
 */
int nrfx_gppi_group_channel_alloc(uint32_t domain_id);

/**
 * @brief Function for freeing a group channel in a specific node of the DPPI system.
 *
 * @p channel must be allocated earlier using @ref nrfx_gppi_group_channel_alloc. @p domain_id must
 * be the same as the one used in @ref nrfx_gppi_group_channel_alloc for allocating that resource.
 *
 * @param[in] domain_id Platform-specific identifier of the DPPI system node.
 * @param[in] channel   Channel.
 */
void nrfx_gppi_group_channel_free(uint32_t domain_id, uint8_t channel);

/** @brief A structure describing a DPPI resource in multi domain system. */
typedef struct {
    /** Domain ID. */
    uint16_t domain_id;
    /** Channel. */
    uint8_t  channel;
} nrfx_gppi_resource_t;

/**
 * @brief Function for allocating and setting up a connection using an external resource.
 *
 * If DPPI resource is provided then it is used in the connection and additional channel in
 * that domain is not allocated. If connection was set up with an external resource then
 * functions for enabling and disabling the connection are only changing state of channels that
 * were allocated for that connection. If %p is null then function is equivalent of
 * @ref nrfx_gppi_domain_conn_alloc.
 *
 * @param[in]  producer   Domain that will produce (publish) events.
 * @param[in]  consumer   Domain that will consume (subsribe to) events.
 * @param[out] p_handle   Handle used to control the connection.
 * @param[in]  p_resource Optional external DPPI resource.
 *
 * @retval 0        Successful connection allocation.
 * @retval -ENOMEM  There is not enough resources to allocate the connection.
 * @retval -ENOTSUP Not supported. Supported only on multi domain system.
 */
#if NRFX_CHECK(NRFX_GPPI_MULTI_DOMAIN) || defined(__NRFX_DOXYGEN__)
int nrfx_gppi_ext_conn_alloc(uint32_t producer, uint32_t consumer, nrfx_gppi_handle_t * p_handle,
                             nrfx_gppi_resource_t * p_resource);
#else
NRFX_STATIC_INLINE int nrfx_gppi_ext_conn_alloc(uint32_t producer, uint32_t consumer,
                                                nrfx_gppi_handle_t * p_handle,
                                                nrfx_gppi_resource_t * p_resource);
#endif

#if NRFX_CHECK(NRFX_GPPI_CONFIG_DPPI_PPIB_EXT_FUNC)
/**
 * @brief Function for writing to a PPIB register.
 *
 * On some platforms driver may not have a direct access to a PPIB register. It is done through
 * a function implemented outside of the driver.
 *
 * @param[in] p_addr Register address.
 * @param[in] value  Value to be written to the register.
 *
 * @retval 0       Successful write.
 * @retval -EINVAL Writing failed.
 */
int nrfx_gppi_ext_ppib_write(volatile uint32_t *p_addr, uint32_t value);
#endif

#ifndef NRFX_DECLARE_ONLY
#if !NRFX_CHECK(NRFX_GPPI_MULTI_DOMAIN)
NRFX_STATIC_INLINE uint32_t nrfx_gppi_group_domain_id_get(nrfx_gppi_group_handle_t handle)
{
    (void)handle;
    return 0;
}

NRFX_STATIC_INLINE uint32_t nrfx_gppi_domain_id_get(uint32_t addr) {
    (void)addr;
    return 0;
}
#endif // !NRFX_GPPI_MULTI_DOMAIN

#ifndef PPI_PRESENT
NRFX_STATIC_INLINE int nrfx_gppi_ep_channel_get(uint32_t ep)
{
    volatile uint32_t * p_sub_pub = (volatile uint32_t *)(ep + NRF_SUBSCRIBE_PUBLISH_OFFSET(ep));
    uint32_t val = *p_sub_pub;

    return (val & NRF_SUBSCRIBE_PUBLISH_ENABLE) ?
        (int)(val & ~NRF_SUBSCRIBE_PUBLISH_ENABLE) : -EINVAL;
}

#endif // !PPI_PRESENT

NRFX_STATIC_INLINE int nrfx_gppi_ep_chan_enable(uint32_t ep)
{
    int ch = nrfx_gppi_ep_channel_get(ep);

    if (ch < 0)
    {
        return -EINVAL;
    }
    nrfx_gppi_channels_enable(nrfx_gppi_domain_id_get(ep), NRFX_BIT((uint32_t)ch));
    return 0;
}

NRFX_STATIC_INLINE int nrfx_gppi_ep_is_enabled(uint32_t ep)
{
    int ch = nrfx_gppi_ep_channel_get(ep);

    if (ch < 0)
    {
        return -EINVAL;
    }
    if (nrfx_gppi_chan_is_enabled(nrfx_gppi_domain_id_get(ep), (uint32_t)ch))
    {
        return 0;
    }
    else
    {
        return -EAGAIN;
    }
}

NRFX_STATIC_INLINE int nrfx_gppi_ep_chan_disable(uint32_t ep)
{
    int ch = nrfx_gppi_ep_channel_get(ep);

    if (ch < 0)
    {
        return -EINVAL;
    }
    nrfx_gppi_channels_disable(nrfx_gppi_domain_id_get(ep), NRFX_BIT((uint32_t)ch));
    return 0;
}

NRFX_STATIC_INLINE int nrfx_gppi_group_ep_add(nrfx_gppi_group_handle_t handle, uint32_t ep)
{
    if (nrfx_gppi_group_domain_id_get(handle) != nrfx_gppi_domain_id_get(ep))
    {
        return -EINVAL;
    }

    int ch = nrfx_gppi_ep_channel_get(ep);
    if (ch < 0)
    {
        return -EINVAL;
    }

    nrfx_gppi_group_ch_add(handle, (uint32_t)ch);
    return 0;
}

NRFX_STATIC_INLINE int nrfx_gppi_group_ep_remove(nrfx_gppi_group_handle_t handle, uint32_t ep)
{
    if (nrfx_gppi_group_domain_id_get(handle) != nrfx_gppi_domain_id_get(ep))
    {
        return -EINVAL;
    }

    int ch = nrfx_gppi_ep_channel_get(ep);
    if (ch < 0)
    {
        return -EINVAL;
    }

    nrfx_gppi_group_ch_remove(handle, (uint32_t)ch);
    return 0;
}
#if !NRFX_CHECK(NRFX_GPPI_MULTI_DOMAIN)
NRFX_STATIC_INLINE int nrfx_gppi_ext_conn_alloc(uint32_t producer, uint32_t consumer,
                                                nrfx_gppi_handle_t * p_handle,
                                                nrfx_gppi_resource_t * p_resource)
{
    (void)producer;
    (void)consumer;
    (void)p_handle;
    (void)p_resource;
    return -ENOTSUP;
}
#endif
#endif // NRFX_DECLARE_ONLY
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NRFX_GPPI_H__ */
