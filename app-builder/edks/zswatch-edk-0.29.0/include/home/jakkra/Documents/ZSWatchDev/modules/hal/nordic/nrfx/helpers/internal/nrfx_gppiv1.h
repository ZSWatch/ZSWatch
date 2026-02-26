/*
 * Copyright (c) 2019 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_GPPI_H
#define NRFX_GPPI_H

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_gppiv1 Generic PPI layer (legacy)
 * @{
 * @ingroup nrfx_gppi
 *
 * @brief Helper layer that provides the common functionality of PPI and DPPI drivers.
 *
 * @warning This API is deprecated.
 *
 * Use PPI and DPPI drivers directly.
 * This layer is provided only to help create generic code that can be built
 * for SoCs equipped with either of these peripherals. When using this layer,
 * take into account that there are significant differences between the PPI and DPPI
 * interfaces that affect the behavior of this layer.
 *
 * One difference is that PPI allows associating of one task or event with
 * more than one channel, whereas DPPI does not allow this. In DPPI, the second
 * association overwrites the first one. Consequently, this helper layer cannot
 * be used in applications that need to connect a task or event to multiple
 * channels.
 *
 * Another difference is that in DPPI one channel can be associated with
 * multiple tasks and multiple events, while in PPI this is not possible (with
 * the exception of the association of a second task as a fork). Because of
 * this difference, it is important to clear the previous endpoints of the channel that
 * is to be reused with some different ones. Otherwise, the behavior of this
 * helper layer will be different, depending on the actual interface used:
 * in DPPI the channel configuration will be extended with the new endpoints, and
 * in PPI the new endpoints will replace the previous ones.
 */

#if defined(PPI_PRESENT)
#include <hal/nrf_ppi.h>

#define NRFX_GPPI_GROUP_NUM              PPI_GROUP_NUM
#define NRFX_GPPI_GROUPS_USED            NRFX_PPI_GROUPS_USED
#define NRFX_GPPI_ALL_APP_GROUPS_MASK    (((1uL << PPI_GROUP_NUM) - 1) & ~(NRFX_PPI_GROUPS_USED))
#define NRFX_GPPI_ALL_APP_CHANNELS_MASK  NRFX_PPI_ALL_APP_CHANNELS_MASK
#define NRFX_GPPI_PROG_APP_CHANNELS_MASK NRFX_PPI_PROG_APP_CHANNELS_MASK

typedef enum
{
    NRFX_GPPI_CHANNEL_GROUP0 = NRF_PPI_CHANNEL_GROUP0,
    NRFX_GPPI_CHANNEL_GROUP1 = NRF_PPI_CHANNEL_GROUP1,
    NRFX_GPPI_CHANNEL_GROUP2 = NRF_PPI_CHANNEL_GROUP2,
    NRFX_GPPI_CHANNEL_GROUP3 = NRF_PPI_CHANNEL_GROUP3,
#if (PPI_GROUP_NUM > 4)
    NRFX_GPPI_CHANNEL_GROUP4 = NRF_PPI_CHANNEL_GROUP4,
    NRFX_GPPI_CHANNEL_GROUP5 = NRF_PPI_CHANNEL_GROUP5,
#endif
} nrfx_gppi_channel_group_t;

typedef enum
{
    NRFX_GPPI_TASK_CHG0_EN  = NRF_PPI_TASK_CHG0_EN,
    NRFX_GPPI_TASK_CHG0_DIS = NRF_PPI_TASK_CHG0_DIS,
    NRFX_GPPI_TASK_CHG1_EN  = NRF_PPI_TASK_CHG1_EN,
    NRFX_GPPI_TASK_CHG1_DIS = NRF_PPI_TASK_CHG1_DIS,
    NRFX_GPPI_TASK_CHG2_EN  = NRF_PPI_TASK_CHG2_EN,
    NRFX_GPPI_TASK_CHG2_DIS = NRF_PPI_TASK_CHG2_DIS,
    NRFX_GPPI_TASK_CHG3_EN  = NRF_PPI_TASK_CHG3_EN,
    NRFX_GPPI_TASK_CHG3_DIS = NRF_PPI_TASK_CHG3_DIS,
#if (PPI_GROUP_NUM > 4)
    NRFX_GPPI_TASK_CHG4_EN  = NRF_PPI_TASK_CHG4_EN,
    NRFX_GPPI_TASK_CHG4_DIS = NRF_PPI_TASK_CHG4_DIS,
    NRFX_GPPI_TASK_CHG5_EN  = NRF_PPI_TASK_CHG5_EN,
    NRFX_GPPI_TASK_CHG5_DIS = NRF_PPI_TASK_CHG5_DIS
#endif
} nrfx_gppi_task_t;

#elif defined(DPPI_PRESENT)

#include <hal/nrf_dppi.h>

#define NRFX_GPPI_GROUP_NUM              NRF_DPPI_GROUP_NUM_MAX
#define NRFX_GPPI_GROUPS_USED            NRFX_DPPI_GROUPS_USED
#define NRFX_GPPI_ALL_APP_GROUPS_MASK    (NRFX_BIT_MASK(NRF_DPPI_GROUP_NUM_MAX) & \
                                         ~NRFX_DPPI_GROUPS_USED)
#define NRFX_GPPI_ALL_APP_CHANNELS_MASK  (NRFX_BIT_MASK(NRF_DPPI_CH_NUM_MAX) & \
                                         ~NRFX_DPPI_CHANNELS_USED)
#if defined(DPPI_TYPE_IPCT)
#define NRFX_GPPI_PROG_APP_CHANNELS_NUM  NRFX_BIT_SIZE(sizeof(uint32_t))
#define NRFX_GPPI_PROG_APP_CHANNELS_MASK NRFX_BIT_MASK(NRFX_GPPI_PROG_APP_CHANNELS_NUM)
#else
#define NRFX_GPPI_PROG_APP_CHANNELS_MASK NRFX_GPPI_ALL_APP_CHANNELS_MASK
#endif

typedef enum
{
    NRFX_GPPI_CHANNEL_GROUP0 = NRF_DPPI_CHANNEL_GROUP0,
    NRFX_GPPI_CHANNEL_GROUP1 = NRF_DPPI_CHANNEL_GROUP1,
#if NRFX_GPPI_GROUP_NUM > 2
    NRFX_GPPI_CHANNEL_GROUP2 = NRF_DPPI_CHANNEL_GROUP2,
    NRFX_GPPI_CHANNEL_GROUP3 = NRF_DPPI_CHANNEL_GROUP3,
#endif
#if NRFX_GPPI_GROUP_NUM > 4
    NRFX_GPPI_CHANNEL_GROUP4 = NRF_DPPI_CHANNEL_GROUP4,
    NRFX_GPPI_CHANNEL_GROUP5 = NRF_DPPI_CHANNEL_GROUP5,
#endif
} nrfx_gppi_channel_group_t;

typedef enum
{
    NRFX_GPPI_TASK_CHG0_EN  = NRF_DPPI_TASK_CHG0_EN,
    NRFX_GPPI_TASK_CHG0_DIS = NRF_DPPI_TASK_CHG0_DIS,
    NRFX_GPPI_TASK_CHG1_EN  = NRF_DPPI_TASK_CHG1_EN,
    NRFX_GPPI_TASK_CHG1_DIS = NRF_DPPI_TASK_CHG1_DIS,
#if NRFX_GPPI_GROUP_NUM > 2
    NRFX_GPPI_TASK_CHG2_EN  = NRF_DPPI_TASK_CHG2_EN,
    NRFX_GPPI_TASK_CHG2_DIS = NRF_DPPI_TASK_CHG2_DIS,
    NRFX_GPPI_TASK_CHG3_EN  = NRF_DPPI_TASK_CHG3_EN,
    NRFX_GPPI_TASK_CHG3_DIS = NRF_DPPI_TASK_CHG3_DIS,
#endif
#if NRFX_GPPI_GROUP_NUM > 4
    NRFX_GPPI_TASK_CHG4_EN  = NRF_DPPI_TASK_CHG4_EN,
    NRFX_GPPI_TASK_CHG4_DIS = NRF_DPPI_TASK_CHG4_DIS,
    NRFX_GPPI_TASK_CHG5_EN  = NRF_DPPI_TASK_CHG5_EN,
    NRFX_GPPI_TASK_CHG5_DIS = NRF_DPPI_TASK_CHG5_DIS
#endif
} nrfx_gppi_task_t;

#elif defined(__NRFX_DOXYGEN__)

/** @brief Generic PPI channel groups. */
typedef enum
{
    NRFX_GPPI_CHANNEL_GROUP0, /**< Channel group 0.*/
    NRFX_GPPI_CHANNEL_GROUP1, /**< Channel group 1.*/
    NRFX_GPPI_CHANNEL_GROUP2, /**< Channel group 2.*/
    NRFX_GPPI_CHANNEL_GROUP3, /**< Channel group 3.*/
    NRFX_GPPI_CHANNEL_GROUP4, /**< Channel group 4.*/
    NRFX_GPPI_CHANNEL_GROUP5, /**< Channel group 5.*/
} nrfx_gppi_channel_group_t;

/** @brief Generic PPI tasks. */
typedef enum
{
    NRFX_GPPI_TASK_CHG0_EN,  /**< Task for enabling channel group 0 */
    NRFX_GPPI_TASK_CHG0_DIS, /**< Task for disabling channel group 0 */
    NRFX_GPPI_TASK_CHG1_EN,  /**< Task for enabling channel group 1 */
    NRFX_GPPI_TASK_CHG1_DIS, /**< Task for disabling channel group 1 */
    NRFX_GPPI_TASK_CHG2_EN,  /**< Task for enabling channel group 2 */
    NRFX_GPPI_TASK_CHG2_DIS, /**< Task for disabling channel group 2 */
    NRFX_GPPI_TASK_CHG3_EN,  /**< Task for enabling channel group 3 */
    NRFX_GPPI_TASK_CHG3_DIS, /**< Task for disabling channel group 3 */
    NRFX_GPPI_TASK_CHG4_EN,  /**< Task for enabling channel group 4 */
    NRFX_GPPI_TASK_CHG4_DIS, /**< Task for disabling channel group 4 */
    NRFX_GPPI_TASK_CHG5_EN,  /**< Task for enabling channel group 5 */
    NRFX_GPPI_TASK_CHG5_DIS, /**< Task for disabling channel group 5 */
} nrfx_gppi_task_t;
#endif // defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for checking if a given channel is enabled.
 *
 * @param[in] channel Channel to check.
 *
 * @retval true  The channel is enabled.
 * @retval false The channel is not enabled.
 */
bool nrfx_gppiv1_channel_check(uint8_t channel);

/** @brief Function for disabling all channels. */
void nrfx_gppiv1_channels_disable_all(void);

/**
 * @brief Function for enabling multiple channels.
 *
 * The bits in @c mask value correspond to particular channels. This means that
 * writing 1 to bit 0 enables channel 0, writing 1 to bit 1 enables channel 1, etc.
 *
 * @param[in] mask Channel mask.
 */
void nrfx_gppiv1_channels_enable(uint32_t mask);

/**
 * @brief Function for disabling multiple channels.
 *
 * The bits in @c mask value correspond to particular channels. This means that
 * writing 1 to bit 0 disables channel 0, writing 1 to bit 1 disables channel 1, etc.
 *
 * @param[in] mask Channel mask.
 */
void nrfx_gppiv1_channels_disable(uint32_t mask);

/**
 * @brief Function for associating a given channel with the specified event register.
 *
 * This function sets the DPPI publish configuration for a given event
 * or sets the PPI event endpoint register.
 *
 * @param[in] channel Channel to which to assign the event.
 * @param[in] eep     Address of the event register.
 */
void nrfx_gppiv1_event_endpoint_setup(uint8_t channel, uint32_t eep);

/**
 * @brief Function for associating a given channel with the specified task register.
 *
 * This function sets the DPPI subscribe configuration for a given task
 * or sets the PPI task endpoint register.
 *
 * @param[in] channel Channel to which to assign the task.
 * @param[in] tep     Address of the task register.
 */
void nrfx_gppiv1_task_endpoint_setup(uint8_t channel, uint32_t tep);

/**
 * @brief Function for setting up the event and task endpoints for a given channel.
 *
 * @param[in] channel Channel to which the given endpoints are assigned.
 * @param[in] eep     Address of the event register.
 * @param[in] tep     Address of the task register.
 */
void nrfx_gppiv1_channel_endpoints_setup(uint8_t channel, uint32_t eep, uint32_t tep);

/**
 * @brief Function for clearing the event and task endpoints for a given channel.
 *
 * @param[in] channel Channel to which the given endpoints are assigned.
 * @param[in] eep     Address of the event register.
 * @param[in] tep     Address of the task register.
 */
void nrfx_gppiv1_channel_endpoints_clear(uint8_t  channel, uint32_t eep, uint32_t tep);

/**
 * @brief Function for clearing the DPPI publish configuration for a given event
 * register or for clearing the PPI event endpoint register.
 *
 * @param[in] channel Channel for which to clear the event endpoint. Not used in DPPI.
 * @param[in] eep     Address of the event register. Not used in PPI.
 */
void nrfx_gppiv1_event_endpoint_clear(uint8_t channel, uint32_t eep);

/**
 * @brief Function for clearing the DPPI subscribe configuration for a given task
 * register or for clearing the PPI task endpoint register.
 *
 * @param[in] channel Channel from which to disconnect the task enpoint. Not used in DPPI.
 * @param[in] tep     Address of the task register. Not used in PPI.
 */
void nrfx_gppiv1_task_endpoint_clear(uint8_t channel, uint32_t tep);

#if defined(PPI_FEATURE_FORKS_PRESENT) || defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting up the task endpoint for a given PPI fork or for
 * associating the DPPI channel with an additional task register.
 *
 * @param[in] channel  Channel to which the given fork endpoint is assigned.
 * @param[in] fork_tep Address of the task register.
 */
void nrfx_gppiv1_fork_endpoint_setup(uint8_t channel, uint32_t fork_tep);

/**
 * @brief Function for clearing the task endpoint for a given PPI fork or for clearing
 * the DPPI subscribe register.
 *
 * @param[in] channel  Channel for which to clear the fork endpoint. Not used in DPPI.
 * @param[in] fork_tep Address of the task register. Not used in PPI.
 */
void nrfx_gppiv1_fork_endpoint_clear(uint8_t channel, uint32_t fork_tep);
#endif // defined(PPI_FEATURE_FORKS_PRESENT) || defined(DPPI_PRESENT) || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for setting multiple channels in a channel group.
 *
 * @param[in] channel_mask  Channels to be set in the group.
 * @param[in] channel_group Channel group.
 */
void nrfx_gppiv1_channels_group_set(uint32_t                  channel_mask,
                                  nrfx_gppi_channel_group_t channel_group);

/**
 * @brief Function for including multiple channels in a channel group.
 *
 * @param[in] channel_mask  Channels to be included in the group.
 * @param[in] channel_group Channel group.
 */
void nrfx_gppiv1_channels_include_in_group(uint32_t                  channel_mask,
                                         nrfx_gppi_channel_group_t channel_group);

/**
 * @brief Function for removing multiple channels from a channel group.
 *
 * @param[in] channel_mask  Channels to be removed from the group.
 * @param[in] channel_group Channel group.
 */
void nrfx_gppiv1_channels_remove_from_group(uint32_t                  channel_mask,
                                          nrfx_gppi_channel_group_t channel_group);

/**
 * @brief Function for removing all channels from a channel group.
 *
 * @param[in] channel_group Channel group.
 */
void nrfx_gppiv1_group_clear(nrfx_gppi_channel_group_t channel_group);

/**
 * @brief Function for enabling a channel group.
 *
 * @param[in] channel_group Channel group.
 */
void nrfx_gppiv1_group_enable(nrfx_gppi_channel_group_t channel_group);

/**
 * @brief Function for disabling a group.
 *
 * @param[in] channel_group Channel group.
 */
void nrfx_gppiv1_group_disable(nrfx_gppi_channel_group_t channel_group);

/**
 * @brief Function for activating a task.
 *
 * @param[in] task Task to be activated.
 */
void nrfx_gppiv1_task_trigger(nrfx_gppi_task_t task);

/**
 * @brief Function for returning the address of a specific task register.
 *
 * @param[in] task PPI or DPPI task.
 *
 * @return Address of the requested task register.
 */
uint32_t nrfx_gppiv1_task_address_get(nrfx_gppi_task_t task);

/**
 * @brief Function for returning the address of a channel group disable task.
 *
 * @param[in] group Channel group.
 *
 * @return Disable task address of the specified group.
 */
nrfx_gppi_task_t nrfx_gppiv1_group_disable_task_get(nrfx_gppi_channel_group_t group);

/**
 * @brief Function for returning the address of a channel group enable task.
 *
 * @param[in] group Channel group.
 *
 * @return Enable task address of the specified group.
 */
nrfx_gppi_task_t nrfx_gppiv1_group_enable_task_get(nrfx_gppi_channel_group_t group);

/**
 * @brief Function for allocating a channel.
 *
 * @param[out] p_channel After successful allocation, index of the allocated channel.
 *
 * @retval NRFX_SUCCESS             Channel was successfully allocated.
 * @retval NRFX_ERROR_NO_MEM        There is no available channel to be used.
 * @retval NRFX_ERROR_NOT_SUPPORTED Driver is not enabled.
 */
nrfx_err_t nrfx_gppiv1_channel_alloc(uint8_t * p_channel);

/**
 * @brief Function for freeing a channel.
 *
 * @param[in] channel (D)PPI channel to be freed.
 *
 * @retval NRFX_SUCCESS             The channel was successfully freed.
 * @retval NRFX_ERROR_INVALID_PARAM The specified channel is not allocated or
 *                                  is not user-configurable.
 * @retval NRFX_ERROR_NOT_SUPPORTED Driver is not enabled.
 */
nrfx_err_t nrfx_gppiv1_channel_free(uint8_t channel);

/**
 * @brief Function for allocating a channel group.
 *
 * @param[out] p_group Pointer to the (D)PPI channel group that has been allocated.
 *
 * @retval NRFX_SUCCESS             The channel group was successfully allocated.
 * @retval NRFX_ERROR_NO_MEM        There is no available channel group to be used.
 * @retval NRFX_ERROR_NOT_SUPPORTED Driver is not enabled.
 */
nrfx_err_t nrfx_gppiv1_group_alloc(nrfx_gppi_channel_group_t * p_group);

/**
 * @brief Function for freeing a channel group.
 *
 * @param[in] group (D)PPI channel group to be freed.
 *
 * @retval NRFX_SUCCESS             The channel was successfully freed.
 * @retval NRFX_ERROR_INVALID_PARAM The specified channel is not allocated or
 *                                  is not user-configurable.
 * @retval NRFX_ERROR_NOT_SUPPORTED Driver is not enabled.
 */
nrfx_err_t nrfx_gppiv1_group_free(nrfx_gppi_channel_group_t group);
/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRFX_GPPI_H
