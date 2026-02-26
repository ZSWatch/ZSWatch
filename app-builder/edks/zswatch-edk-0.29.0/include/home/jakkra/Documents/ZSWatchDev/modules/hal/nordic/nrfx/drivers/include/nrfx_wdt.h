/*
 * Copyright (c) 2014 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_WDT_H__
#define NRFX_WDT_H__

#include <nrfx.h>
#include <haly/nrfy_wdt.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_wdt WDT driver
 * @{
 * @ingroup nrf_wdt
 * @brief   Watchdog Timer (WDT) peripheral driver.
 */

#if NRF_WDT_HAS_STOP || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether watchdog stopping is supported. */
#define NRFX_WDT_HAS_STOP 1
#else
#define NRFX_WDT_HAS_STOP 0
#endif

#if !NRFX_CHECK(NRFX_WDT_CONFIG_NO_IRQ) || defined(__NRFX_DOXYGEN__)
/** @brief WDT instance interrupt priority configuration. */
    #define NRFX_WDT_IRQ_CONFIG .interrupt_priority = NRFX_WDT_DEFAULT_CONFIG_IRQ_PRIORITY
#else
    #define NRFX_WDT_IRQ_CONFIG
#endif

/**
 * @brief WDT event handler function type.
 *
 * @param[in] event_type WDT event.
 * @param[in] requests   Value of the request status register. Bits that have been set can be
 *                       used to determine which RR (Reload Request) register was the reason
 *                       for timeout event.
 *                       Valid only when @ref NRF_WDT_EVENT_TIMEOUT is passed in @p event_type.
 * @param[in] p_context  User context.
 */
typedef void (*nrfx_wdt_event_handler_t)(nrf_wdt_event_t event_type,
                                         uint32_t        requests,
                                         void *          p_context);

/** @brief WDT channel ID type. */
typedef nrf_wdt_rr_register_t nrfx_wdt_channel_id;

/** @cond Driver internal data. */
typedef struct
{
    uint8_t                  alloc_index;
    nrfx_drv_state_t         state;
#if !NRFX_CHECK(NRFX_WDT_CONFIG_NO_IRQ)
    nrfx_wdt_event_handler_t wdt_event_handler;
    void *                   p_context;
#endif
#if NRFX_WDT_HAS_STOP
    bool                     stoppable;
#endif
} wdt_control_block_t;
/** @endcond */

/** @brief Data structure of the Watchdog (WDT) driver instance. */
typedef struct
{
    NRF_WDT_Type *      p_reg; ///< Pointer to a structure with WDT registers.
    wdt_control_block_t cb;    ///< Driver internal data.
} nrfx_wdt_t;

/** @brief Macro for creating an instance of the WDT driver. */
#define NRFX_WDT_INSTANCE(reg)    \
{                                 \
    .p_reg = (NRF_WDT_Type *)reg, \
    .cb    = {0},                 \
}

/** @brief Structure for WDT initialization. */
typedef struct
{
    uint32_t behaviour;          ///< WDT behavior flags bitmask, constructed from @ref nrf_wdt_behaviour_mask_t.
    uint32_t reload_value;       ///< WDT reload value in milliseconds.
#if !NRFX_CHECK(NRFX_WDT_CONFIG_NO_IRQ) || defined(__NRFX_DOXYGEN__)
    uint8_t  interrupt_priority; ///< WDT interrupt priority.
#endif
} nrfx_wdt_config_t;

/**
 * @brief WDT driver default configuration.
 *
 * This configuration sets up WDT with the following options:
 * - run when CPU is in SLEEP mode, pause when in HALT mode
 * - reload value: 2000 ms
 */
#define NRFX_WDT_DEFAULT_CONFIG                             \
{                                                           \
    .behaviour          = NRF_WDT_BEHAVIOUR_RUN_SLEEP_MASK, \
    .reload_value       = 2000,                             \
    NRFX_WDT_IRQ_CONFIG                                     \
}

/**
 * @brief Function for initializing the WDT driver instance.
 *
 * @param[in] p_instance        Pointer to the driver instance structure.
 * @param[in] p_config          Pointer to the structure with the initial configuration.
 *                              NULL if configuration is to be skipped and will be done later
 *                              using @ref nrfx_wdt_reconfigure.
 * @param[in] wdt_event_handler Event handler provided by the user. Ignored when
 *                              @ref NRFX_WDT_CONFIG_NO_IRQ option is enabled.
 * @param[in] p_context         User context passed in event handler. Ignored when
 *                              @ref NRFX_WDT_CONFIG_NO_IRQ option is enabled.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver is already initialized.
 */
int nrfx_wdt_init(nrfx_wdt_t *              p_instance,
                  nrfx_wdt_config_t const * p_config,
                  nrfx_wdt_event_handler_t  wdt_event_handler,
                  void *                    p_context);

/**
 * @brief Function for uninitializing the WDT driver instance.
 *
 * The instance can be uninitialized only when not running.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_wdt_uninit(nrfx_wdt_t * p_instance);

/**
 * @brief Function for checking if the WDT driver instance is initialized.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval true  Instance is already initialized.
 * @retval false Instance is not initialized.
 */
bool nrfx_wdt_init_check(nrfx_wdt_t const * p_instance);

/**
 * @brief Function for reconfiguring the watchdog.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] p_config   Pointer to the structure with the initial configuration.
 *
 * @retval 0            Reconfiguration was successful.
 * @retval -EBUSY       The watchdog is already active.
 * @retval -EINPROGRESS The watchdog is uninitialized.
 */
int nrfx_wdt_reconfigure(nrfx_wdt_t *              p_instance,
                         nrfx_wdt_config_t const * p_config);

/**
 * @brief Function for allocating a watchdog channel.
 *
 * @note This function can not be called after @ref nrfx_wdt_enable.
 *
 * @param[in]  p_instance   Pointer to the driver instance structure.
 * @param[out] p_channel_id ID of granted channel.
 *
 * @retval 0       The channel was successfully allocated.
 * @retval -ENOMEM There is no available channel to be used.
 */
int nrfx_wdt_channel_alloc(nrfx_wdt_t *          p_instance,
                           nrfx_wdt_channel_id * p_channel_id);

/**
 * @brief Function for deallocating all previously allocated watchdog channels.
 *
 * @note This function can be called when watchdog is stopped,
 *       that is before @ref nrfx_wdt_enable() or after @ref nrfx_wdt_stop().
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_wdt_channels_free(nrfx_wdt_t * p_instance);

/**
 * @brief Function for starting the watchdog.
 *
 * @note After calling this function the watchdog is started, so the user needs to feed
 *       all allocated watchdog channels to avoid reset. At least one watchdog channel
 *       must be allocated.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_wdt_enable(nrfx_wdt_t * p_instance);

/**
 * @brief Function for feeding the watchdog.
 *
 * @details Function feeds all allocated watchdog channels.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_wdt_feed(nrfx_wdt_t const * p_instance);

/**
 * @brief Function for feeding an individual watchdog channel.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] channel_id ID of watchdog channel.
 */
void nrfx_wdt_channel_feed(nrfx_wdt_t const * p_instance, nrfx_wdt_channel_id channel_id);

#if NRFX_WDT_HAS_STOP
/**
 * @brief Function for stopping the watchdog.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 *
 * @retval 0      Watchdog has been successfully stopped.
 * @retval -EPERM Configuration does not allow for stopping the watchdog.
 */
int nrfx_wdt_stop(nrfx_wdt_t * p_instance);
#endif

/**
 * @brief Driver interrupt handler.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 */
void nrfx_wdt_irq_handler(nrfx_wdt_t * p_instance);

/**
 * @brief Function for returning a requested task address for the WDT driver module.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] task       One of the WDT tasks.
 *
 * @return Task address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_wdt_task_address_get(nrfx_wdt_t const * p_instance,
                                                      nrf_wdt_task_t     task);

/**
 * @brief Function for returning a requested event address for the WDT driver module.
 *
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] event      One of the WDT events.
 *
 * @return Event address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_wdt_event_address_get(nrfx_wdt_t const * p_instance,
                                                       nrf_wdt_event_t    event);

#ifndef NRFX_DECLARE_ONLY
NRFX_STATIC_INLINE uint32_t nrfx_wdt_task_address_get(nrfx_wdt_t const * p_instance,
                                                      nrf_wdt_task_t     task)
{
    return nrfy_wdt_task_address_get(p_instance->p_reg, task);
}

NRFX_STATIC_INLINE uint32_t nrfx_wdt_event_address_get(nrfx_wdt_t const * p_instance,
                                                       nrf_wdt_event_t    event)
{
    return nrfy_wdt_event_address_get(p_instance->p_reg, event);
}
#endif // NRFX_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif
