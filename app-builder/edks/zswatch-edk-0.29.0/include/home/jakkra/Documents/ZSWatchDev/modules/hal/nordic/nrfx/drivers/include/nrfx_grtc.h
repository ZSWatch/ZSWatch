/*
 * Copyright (c) 2021 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_GRTC_H__
#define NRFX_GRTC_H__

#include <nrfx.h>
#include <haly/nrfy_grtc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_grtc GRTC driver
 * @{
 * @ingroup nrf_grtc
 * @brief   Global Real Timer Counter (GRTC) peripheral driver.
 */

/**
 * @brief GRTC driver instance compare handler type.
 *
 * @param[in] id        Channel ID.
 * @param[in] cc_value  Compare value.
 * @param[in] p_context User context.
 */
typedef void (*nrfx_grtc_cc_handler_t)(int32_t id, uint64_t cc_value, void * p_context);

#if NRFY_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)
/**
 * @brief GRTC driver instance SYSCOUNTER valid handler type.
 *
 * @param[in] p_context User context.
 */
typedef void (*nrfx_grtc_syscountervalid_handler_t)(void * p_context);
#endif //NRFY_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)

#if NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)
/**
 * @brief GRTC driver instance RTCOMPARESYNC handler type.
 *
 * @param[in] p_context User context.
 */
typedef void (*nrfx_grtc_rtcomparesync_handler_t)(void * p_context);
#endif // NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)

/** @brief GRTC capture/compare channel description structure. */
typedef struct
{
    nrfx_grtc_cc_handler_t handler;   /**< User handler. */
    void *                 p_context; /**< User context. */
    uint8_t                channel;   /**< Capture/compare channel number. */
} nrfx_grtc_channel_t;

#if NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)
/** @brief GRTC RTCOUNTER handler data structure. */
typedef struct
{
    nrfx_grtc_cc_handler_t handler;   /**< User handler. */
    void *                 p_context; /**< User context. */
} nrfx_grtc_rtcounter_handler_data_t;
#endif // NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)

#if NRFY_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)
/** @brief GRTC action types. */
typedef enum
{
    NRFX_GRTC_ACTION_START = NRF_GRTC_TASK_START, /**< Start the GRTC. */
    NRFX_GRTC_ACTION_STOP  = NRF_GRTC_TASK_STOP,  /**< Stop the GRTC. */
    NRFX_GRTC_ACTION_CLEAR = NRF_GRTC_TASK_CLEAR, /**< Clear the GRTC. */
} nrfx_grtc_action_t;

/** @brief GRTC SYSCOUNTER sleep configuration structure. */
typedef struct
{
    uint32_t timeout;   /**< Delay in LFCLK cycles after the condition allowing SYSCOUNTER to go to sleep is met. */
    uint32_t waketime;  /**< Number of LFCLK cycles to wakeup the SYSCOUNTER before the wake-up event occured. */
    bool     auto_mode; /**< Enable automatic mode, which keeps the SYSCOUNTER active when any of the local CPUs is active. */
} nrfx_grtc_sleep_config_t;

/**
 * @brief GRTC sleep default configuration.
 *
 * This configuration sets up GRTC with the following options:
 * - sleep timeout: 5 LFCLK cycles
 * - wake time: 4 LFCLK cycles
 * - automatic mode: true
 */
#define NRFX_GRTC_SLEEP_DEFAULT_CONFIG \
{                                      \
    .timeout   = 5,                    \
    .waketime  = 4,                    \
    .auto_mode = true                  \
}
#endif // NRFY_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)

/** @brief GRTC compare event relative references. */
typedef enum
{
    NRFX_GRTC_CC_RELATIVE_SYSCOUNTER = NRF_GRTC_CC_ADD_REFERENCE_SYSCOUNTER, /**< The SYSCOUNTER content will be used as the reference. */
    NRFX_GRTC_CC_RELATIVE_COMPARE    = NRF_GRTC_CC_ADD_REFERENCE_CC,         /**< The corresponding compare register content will be used as the reference. */
} nrfx_grtc_cc_relative_reference_t;

#if NRFY_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for configuring the SYSCOUNTER sleep feature.
 *
 * @param[in] p_sleep_cfg Pointer to the configuration sleep structure.
 */
void nrfx_grtc_sleep_configure(nrfx_grtc_sleep_config_t const * p_sleep_cfg);

/**
 * @brief Function for getting the SYSCOUNTER sleep configuration.
 *
 * @param[out] p_sleep_cfg Pointer to the structure to be filled with sleep configuration.
 */
void nrfx_grtc_sleep_configuration_get(nrfx_grtc_sleep_config_t * p_sleep_cfg);
#endif // NRFY_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for allocating the GRTC capture/compare channel.
 *
 * @note Function is thread safe as it uses @ref nrfx_flag32_alloc.
 * @note Routines that allocate and free the GRTC channels are independent
 *       from the rest of the driver. In particular, the driver does not need
 *       to be initialized when this function is called.
 *
 * @param[out] p_channel Pointer to the capture/compare channel.
 *
 * @retval 0       Allocation was successful.
 * @retval -ENOMEM No resource available.
 */
int nrfx_grtc_channel_alloc(uint8_t * p_channel);

/**
 * @brief Function for setting a callback to a channel.
 *
 * Function enables the interrupt for that channel.
 *
 * @param[in] channel   Channel.
 * @param[in] handler   User handler called when channel expires.
 * @param[in] p_context Context passed to the callback.
 */
void nrfx_grtc_channel_callback_set(uint8_t                channel,
                                    nrfx_grtc_cc_handler_t handler,
                                    void *                 p_context);

/**
 * @brief Function for freeing the GRTC capture/compare channel.
 *
 * @note Function is thread safe as it uses @ref nrfx_flag32_free.
 * @note Routines that allocate and free the GRTC channels are independent
 *       from the rest of the driver. In particular, the driver does not need
 *       to be initialized when this function is called.
 * @note This function also mark specified channel as unused by the driver.
 *
 * @param[in] channel Allocated channel to be freed.
 *
 * @retval 0       Allocation was successful.
 * @retval -EPERM  The domain is not allowed to use specified @p channel.
 * @retval -EINVAL Channel is not allocated.
 */
int nrfx_grtc_channel_free(uint8_t channel);

/**
 * @brief Function for checking whether the specified channel is used by the driver.
 *
 * @note Channels marked as used cannot be utilized by external API.
 *
 * @param[in] channel Channel to be checked.
 *
 * @retval true  Channel is used by the driver.
 * @retval false Channel is not used by the driver.
 */
bool nrfx_grtc_is_channel_used(uint8_t channel);

/**
 * @brief Function for initializing the GRTC.
 *
 * @param[in] interrupt_priority Interrupt priority.
 *
 * @retval 0          Initialization was successful.
 * @retval -EALREADY  The driver is already initialized.
 * @retval -ECANCELED No valid channel configuration provided.
 */
int nrfx_grtc_init(uint8_t interrupt_priority);

#if NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for disabling the RTCOUNTER CC channel.
 *
 * @retval 0          The procedure was successful.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is running and the operation is not allowed.
 * @retval -ETIMEDOUT RTCOUNTER compare interrupt is pending.
 */
int nrfx_grtc_rtcounter_cc_disable(void);

/**
 * @brief Function for enabling the RTCOMPARESYNC interrupt.
 *
 * @param[in] handler   Handler provided by the user. May be NULL.
 * @param[in] p_context User context.
 */
void nrfx_grtc_rtcomparesync_int_enable(nrfx_grtc_rtcomparesync_handler_t handler,
                                        void *                            p_context);

/** @brief Function for disabling the RTCOMPARESYNC interrupt. */
void nrfx_grtc_rtcomparesync_int_disable(void);

/**
 * @brief Function for setting the absolute compare value for the RTCOUNTER.
 *
 * @param[in] handler_data Pointer to the handler data instance structure.
 * @param[in] val          Absolute value to be set in the compare register.
 * @param[in] enable_irq   True if interrupt is to be enabled, false otherwise.
 * @param[in] sync         True if the internal synchronization mechanism shall be used,
 *                         false otherwise.
 *
 * @retval 0          The procedure was successful.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is running and the operation is not allowed.
 */
int nrfx_grtc_rtcounter_cc_absolute_set(nrfx_grtc_rtcounter_handler_data_t * handler_data,
                                        uint64_t                             val,
                                        bool                                 enable_irq,
                                        bool                                 sync);

/**
 * @brief Function for enabling the RTCOUNTER compare interrupt.
 *
 * @param[in] sync True if the internal synchronization mechanism shall be used,
 *                 false otherwise.
 */
void nrfx_grtc_rtcounter_cc_int_enable(bool sync);

/** @brief Function for disabling the RTCOUNTER compare interrupt. */
void nrfx_grtc_rtcounter_cc_int_disable(void);
#endif // NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)

#if NRFY_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)
#if NRFY_GRTC_HAS_SYSCOUNTERVALID || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for enabling the SYSCOUNTERVALID interrupt.
 *
 * @param[in] handler   Handler provided by the user. May be NULL.
 * @param[in] p_context User context.
 */
void nrfx_grtc_syscountervalid_int_enable(nrfx_grtc_syscountervalid_handler_t handler,
                                          void *                              p_context);

/** @brief Function for disabling the SYSCOUNTERVALID interrupt. */
void nrfx_grtc_syscountervalid_int_disable(void);
#endif // NRFY_GRTC_HAS_SYSCOUNTERVALID || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for starting the 1 MHz SYSCOUNTER.
 *
 * @note This function automatically allocates and marks as used the special-purpose main
 *       capture/compare channel. It is available only for GRTC manager.
 *
 * @note Use auxiliary structure of type @ref nrfx_grtc_channel_t when working with SYSCOUNTER.
 *
 * @param[in]  busy_wait         True if wait for synchronization operation is to be performed,
 *                               false otherwise.
 * @param[out] p_main_cc_channel Pointer to the main capture/compare channel.
 *
 * @retval 0          Starting was successful.
 * @retval -ENOMEM    No resource available to allocate main channel.
 * @retval -EALREADY  The GRTC is already running.
 * @retval -ETIMEDOUT The SYSCOUNTER failed to start due to a timeout.
 */
int nrfx_grtc_syscounter_start(bool busy_wait, uint8_t * p_main_cc_channel);

/**
 * @brief Function for performing an action for the GRTC.
 *
 * @param[in] action Action to be performed.
 *
 * @retval 0          Starting was successful.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is running and the operation is not allowed.
 */
int nrfx_grtc_action_perform(nrfx_grtc_action_t action);
#endif // NRFY_GRTC_HAS_EXTENDED || defined(__NRFX_DOXYGEN__)

/**
 * @brief Function for uninitializing the GRTC.
 *
 * @note This function automatically frees all channels used by the driver.
 *       It also marks these channels as unused
*/
void nrfx_grtc_uninit(void);

/**
 * @brief Function for checking if the GRTC driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_grtc_init_check(void);

/**
 * @brief Function for disabling the SYSCOUNTER CC channel.
 *
 * @note This function marks the specified @p channel as unused.
 *
 * @param[in] channel Channel to be disabled.
 *
 * @retval 0          The procedure was successful.
 * @retval -EPERM     The domain is not allowed to use specified @p channel.
 * @retval -EINVAL    The specified @p channel is either not allocated or marked as unused.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is not running.
 * @retval -ETIMEDOUT SYSCOUNTER compare interrupt is pending on the requested channel.
 */
int nrfx_grtc_syscounter_cc_disable(uint8_t channel);

/**
 * @brief Function for setting the absolute compare value for the SYSCOUNTER.
 *
 * @note This function is deprecated. Use @ref nrfx_grtc_syscounter_cc_abs_set instead.
 * @note This function marks the specified @p channel as used.
 *
 * @param[in] p_chan_data Pointer to the channel data instance structure.
 * @param[in] val         Absolute value to be set in the compare register.
 * @param[in] enable_irq  True if interrupt is to be enabled, false otherwise.
 *
 * @retval 0          The procedure was successful.
 * @retval -EPERM     The domain is not allowed to use specified @p channel.
 * @retval -EINVAL    Channel is not allocated.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is not running.
 */
int nrfx_grtc_syscounter_cc_absolute_set(nrfx_grtc_channel_t * p_chan_data,
                                         uint64_t              val,
                                         bool                  enable_irq);

/**
 * @brief Function for setting the absolute compare value for the SYSCOUNTER in an optimized way.
 *
 * Function must be called with interrupts locked. If @p safe_setting is true then
 * it means that previous CC for that channel did not yet expire and it
 * was set to a value earlier than @p val so there is a chance that it will
 * expire during setting the new value. In that case compare event may be misinterpreted.
 * Slower but safe procedure is used in that case which ensures that there will be no
 * unexpected user callback triggered. If @p safe_setting is false then function just sets
 * new CC value.
 *
 * @param[in] channel      Channel.
 * @param[in] val          Absolute value to be set in the compare register.
 * @param[in] safe_setting True if safe procedure is to be used, false otherwise.
 */
void nrfx_grtc_syscounter_cc_abs_set(uint8_t channel, uint64_t val, bool safe_setting);

/**
 * @brief Function for setting the relative compare value for the SYSCOUNTER.
 *
 * @note This function is deprecated. Use @ref nrfx_grtc_syscounter_cc_rel_set instead.
 *
 * Function has no assumptions on the current channel state so channel event is cleared and
 * interrupt can optionally be enabled for that channel.
 *
 * @note This function marks the specified @p channel as used.
 *
 * @param[in] p_chan_data Pointer to the channel data instance structure.
 * @param[in] val         Relative value to be set in the compare register.
 * @param[in] enable_irq  True if interrupt is to be enabled, false otherwise.
 * @param[in] reference   Reference type to be used.
 *
 * @retval 0          The procedure was successful.
 * @retval -EPERM     The domain is not allowed to use specified @p channel.
 * @retval -EINVAL    Channel is not allocated.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is not running.
 */
int nrfx_grtc_syscounter_cc_relative_set(nrfx_grtc_channel_t *             p_chan_data,
                                         uint32_t                          val,
                                         bool                              enable_irq,
                                         nrfx_grtc_cc_relative_reference_t reference);

/**
 * @brief Function for setting the relative compare value in an optimized way.
 *
 * Function just sets CCADD value and does not attempt to enable or disable the interrupt.
 * It assumes that expected channel configuration is done prior to that call.
 * Function assumes that previously used CC value has already expired so new value
 * can be safely set without a risk of spurious CC expiration.
 *
 * @param[in] channel   Channel.
 * @param[in] val       Relative value to be set in the CCADD register.
 * @param[in] reference Reference. Current counter value or current CC value.
 */
void nrfx_grtc_syscounter_cc_rel_set(uint8_t                           channel,
                                     uint32_t                          val,
                                     nrfx_grtc_cc_relative_reference_t reference);

/**
 * @brief Function for disabling the SYSCOUNTER compare interrupt.
 *
 * @param[in] channel Compare channel number.
 *
 * @retval 0          The procedure was successful.
 * @retval -EPERM     The domain is not allowed to use specified @p channel.
 * @retval -EINVAL    The specified @p channel is either not allocated or marked as unused.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is not running.
 */
int nrfx_grtc_syscounter_cc_int_disable(uint8_t channel);

/**
 * @brief Function for enabling the SYSCOUNTER compare interrupt.
 *
 * @note This function marks the specified @p channel as used.
 *
 * @param[in] channel Compare channel number.
 *
 * @retval 0          The procedure was successful.
 * @retval -EPERM     The domain is not allowed to use specified @p channel.
 * @retval -EINVAL    Channel is not allocated.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is not running.
 */
int nrfx_grtc_syscounter_cc_int_enable(uint8_t channel);

/**
 * @brief Function for checking whether the SYSCOUNTER compare interrupt is enabled for
 *        the specified channel.
 *
 * @param[in] channel Compare channel number.
 *
 * @retval true  The interrupt is enabled for the specified channel.
 * @retval false The interrupt is disabled for the specified channel.
 */
bool nrfx_grtc_syscounter_cc_int_enable_check(uint8_t channel);

/**
 * @brief Function for triggering the SYSCOUNTER capture task
 *
 * @note This function marks the specified @p channel as used.
 *
 * @param[in] channel Capture channel number.
 *
 * @retval 0          The procedure was successful.
 * @retval -EPERM     The domain is not allowed to use specified @p channel.
 * @retval -EINVAL    Channel is not allocated.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is not running.
 */
int nrfx_grtc_syscounter_capture(uint8_t channel);

/**
 * @brief Function for reading the GRTC capture/compare register for the specified @p channel.
 *
 * @param[in]  channel Capture channel number.
 * @param[out] p_val   Pointer to the variable where the result is to be stored.
 *
 * @retval 0          The procedure was successful.
 * @retval -EPERM     The domain is not allowed to use specified @p channel.
 * @retval -EINVAL    The specified @p channel is either not allocated or marked as unused.
 * @retval -ECANCELED The SYSCOUNTER (1 MHz) is not running.
 */
int nrfx_grtc_syscounter_cc_value_read(uint8_t channel, uint64_t * p_val);


/**
 * @brief Function for checking whether the SYSCOUNTER is in ready state.
 *
 * @note When the SYSCOUNTER is not ready its value may be corrupted.
 *
 * @retval true  The SYSCOUNTER is in ready state.
 * @retval false The SYSCOUNTER is not in ready state.
 */
bool nrfx_grtc_ready_check(void);

/**
 * @brief Function for checking the current request for the SYSCOUNTER state.
 *
 * @retval true  The SYSCOUNTER is requested to be enabled.
 * @retval false The SYSCOUNTER is requested to be disabled.
 */
bool nrfx_grtc_active_request_check(void);

/**
 * @brief Function for requesting the SYSCOUNTER state.
 *
 * @note By using this function any domain can prevent SYSCOUNTER from going to sleep state.
 *
 * @param[in] active True if SYSCOUNTER is to be always kept active, false otherwise.
 */
void nrfx_grtc_active_request_set(bool active);

/**
 * @brief Function for reading the GRTC SYSCOUNTER value.
 *
 * @return SYSCOUNTER value.
 */
uint64_t nrfx_grtc_syscounter_get(void);

/**
 * @brief Function for retrieving the address of the specified GRTC task.
 *
 * @param[in] task GRTC task.
 *
 * @return Task address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_grtc_task_address_get(nrf_grtc_task_t task);

/**
 * @brief Function for retrieving the address of the specified GRTC event.
 *
 * @param[in] event GRTC event.
 *
 * @return Event address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_grtc_event_address_get(nrf_grtc_event_t event);

/**
 * @brief Function for retrieving the address of the capture task for the specified channel.
 *
 * @param[in] channel Capture channel number.
 *
 * @return Task address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_grtc_capture_task_address_get(uint8_t channel);

/**
 * @brief Function for retrieving the address of the capture task for the specified channel.
 *
 * @param[in] channel Compare channel number.
 *
 * @return Event address.
 */
NRFX_STATIC_INLINE uint32_t nrfx_grtc_event_compare_address_get(uint8_t channel);

/**
 * @brief Function for checking whether the specified capture/compare channel is enabled.
 *
 * @param[in] channel Channel to be checked.
 *
 * @retval true  Channel is enabled.
 * @retval false Channel is disabled.
 */
NRFX_STATIC_INLINE bool nrfx_grtc_sys_counter_cc_enable_check(uint8_t channel);

/**
 * @brief Function for retrieving the state of the compare GRTC event.
 *
 * @param[in] channel Compare channel of the corresponding event to be checked.
 *
 * @retval true  The event has been generated.
 * @retval false The event has not been generated.
 */
NRFX_STATIC_INLINE bool nrfx_grtc_syscounter_compare_event_check(uint8_t channel);

/**
 * @brief Function for retrieving CC value.
 *
 * @param[in] channel Compare channel.
 *
 * @return Value read from CC register.
 */
NRFX_STATIC_INLINE uint64_t nrfx_grtc_sys_counter_cc_get(uint8_t channel);

#if NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for reading the GRTC RTCOUNTER value.
 *
 * @return RTCOUNTER (32 kHz) value.
 */
NRFX_STATIC_INLINE uint64_t nrfx_grtc_rtcounter_get(void);
#endif // NRF_GRTC_HAS_RTCOUNTER || defined(__NRFX_DOXYGEN__)

#if NRFY_GRTC_HAS_CLKSEL || defined(__NRFX_DOXYGEN__)
/**
 * @brief Function for setting the clock source for the GRTC low-frequency clock.
 *
 * @param[in] clk_src Selected clock source.
 */
NRFX_STATIC_INLINE void nrfx_grtc_clock_source_set(nrf_grtc_clksel_t clk_src);
#endif // NRFY_GRTC_HAS_CLKSEL

#ifndef NRFX_DECLARE_ONLY

NRFX_STATIC_INLINE uint32_t nrfx_grtc_task_address_get(nrf_grtc_task_t task)
{
    return nrfy_grtc_task_address_get(NRF_GRTC, task);
}

NRFX_STATIC_INLINE uint32_t nrfx_grtc_event_address_get(nrf_grtc_event_t event)
{
    return nrfy_grtc_event_address_get(NRF_GRTC, event);
}

NRFX_STATIC_INLINE uint32_t nrfx_grtc_capture_task_address_get(uint8_t channel)
{
    return nrfy_grtc_task_address_get(NRF_GRTC, nrfy_grtc_sys_counter_capture_task_get(channel));
}

NRFX_STATIC_INLINE uint32_t nrfx_grtc_event_compare_address_get(uint8_t channel)
{
    return nrfy_grtc_event_address_get(NRF_GRTC, nrfy_grtc_sys_counter_compare_event_get(channel));
}

NRFX_STATIC_INLINE bool nrfx_grtc_sys_counter_cc_enable_check(uint8_t channel)
{
    NRFX_ASSERT(channel < NRF_GRTC_SYSCOUNTER_CC_COUNT);
    return nrfy_grtc_sys_counter_cc_enable_check(NRF_GRTC, channel);
}

NRFX_STATIC_INLINE bool nrfx_grtc_syscounter_compare_event_check(uint8_t channel)
{
    return nrfy_grtc_sys_counter_compare_event_check(NRF_GRTC, channel);
}

NRFX_STATIC_INLINE uint64_t nrfx_grtc_sys_counter_cc_get(uint8_t channel)
{
    return nrfy_grtc_sys_counter_cc_get(NRF_GRTC, channel);
}

#if NRF_GRTC_HAS_RTCOUNTER
NRFX_STATIC_INLINE uint64_t nrfx_grtc_rtcounter_get(void)
{
    return nrfy_grtc_rt_counter_get(NRF_GRTC);
}
#endif // NRF_GRTC_HAS_RTCOUNTER

#if NRFY_GRTC_HAS_CLKSEL
NRFX_STATIC_INLINE void nrfx_grtc_clock_source_set(nrf_grtc_clksel_t  clk_src)
{
    nrfy_grtc_clksel_set(NRF_GRTC, clk_src);
}
#endif // NRFY_GRTC_HAS_CLKSEL
#endif // NRFX_DECLARE_ONLY

/** @} */

void nrfx_grtc_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif // NRFX_GRTC_H__
