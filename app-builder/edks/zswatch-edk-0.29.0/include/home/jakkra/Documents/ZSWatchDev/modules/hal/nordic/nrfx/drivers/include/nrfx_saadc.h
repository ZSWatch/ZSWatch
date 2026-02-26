/*
 * Copyright (c) 2015 - 2025, Nordic Semiconductor ASA
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

#ifndef NRFX_SAADC_H__
#define NRFX_SAADC_H__

#include <nrfx.h>
#include <haly/nrfy_saadc.h>
#include <helpers/nrfx_analog_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrfx_saadc SAADC driver
 * @{
 * @ingroup nrf_saadc
 * @brief   Successive Approximation Analog-to-Digital Converter (SAADC) peripheral driver.
 */

#if NRF_SAADC_HAS_ACQTIME_ENUM || defined(__NRFX_DOXYGEN__)
/** @brief Auxiliary symbol specifying default value for the SAADC acquisition time. */
#define NRFX_SAADC_DEFAULT_ACQTIME NRF_SAADC_ACQTIME_10US
#else
#define NRFX_SAADC_DEFAULT_ACQTIME 79
#endif

#if NRF_SAADC_HAS_CONVTIME || defined(__NRFX_DOXYGEN__)
/** @brief Auxiliary symbol specifying default value for the SAADC conversion time. */
#define NRFX_SAADC_DEFAULT_CONV_TIME 7
#endif

/** @brief Maximum value of the internal timer interval in microseconds. */
#define NRFX_SAADC_INTERNAL_TIMER_INTERVAL_MAX_US 128U

/** @brief Symbol specifying internal reference voltage. */
#define NRFX_SAADC_REF_INTERNAL_VALUE NRFX_ANALOG_REF_INTERNAL_VAL

/**
 * @brief SAADC channel default configuration for the single-ended mode.
 *
 * This configuration sets up single-ended SAADC channel with the following options:
 * - resistor ladder disabled
 * - gain: 1
 * - reference voltage: internal
 * - sample acquisition time: 10 us
 * - burst disabled
 *
 * @param[in] _pin_p Positive input analog pin.
 * @param[in] _index Channel index.
 *
 * @sa nrfx_saadc_channel_t
 */
#define NRFX_SAADC_DEFAULT_CHANNEL_SE(_pin_p, _index)                  \
{                                                                      \
    .channel_config =                                                  \
    {                                                                  \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_CONFIG_RES,                  \
                         (.resistor_p = NRF_SAADC_RESISTOR_DISABLED,   \
                          .resistor_n = NRF_SAADC_RESISTOR_DISABLED,), \
                         ())                                           \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_GAIN,                        \
                         (.gain = NRF_SAADC_GAIN1,),                   \
                         ())                                           \
        .reference  = NRF_SAADC_REFERENCE_INTERNAL,                    \
        .acq_time   = NRFX_SAADC_DEFAULT_ACQTIME,                      \
        .mode       = NRF_SAADC_MODE_SINGLE_ENDED,                     \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_BURST,                       \
                         (.burst = NRF_SAADC_BURST_DISABLED,),         \
                         ())                                           \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_CHOPPING,                    \
                         (.chopping = NRF_SAADC_CHOPPING_DISABLED,),   \
                         ())                                           \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_HIGHSPEED,                   \
                         (.highspeed = NRF_SAADC_HIGHSPEED_DISABLED,), \
                         ())                                           \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CONV_TIME,                      \
                         (.conv_time = NRFX_SAADC_DEFAULT_CONV_TIME,), \
                         ())                                           \
    },                                                                 \
    .pin_p          = (nrfx_analog_input_t)_pin_p,                     \
    .pin_n          = NRFX_ANALOG_INPUT_DISABLED,                      \
    .channel_index  = _index,                                          \
}

/**
 * @brief SAADC channel default configuration for the differential mode.
 *
 * This configuration sets up differential SAADC channel with the following options:
 * - resistor ladder disabled
 * - gain: 1/6
 * - reference voltage: internal
 * - sample acquisition time: 10 us
 * - burst disabled
 *
 * @param[in] _pin_p Positive input analog pin.
 * @param[in] _pin_n Negative input analog pin.
 * @param[in] _index Channel index.
 *
 * @sa nrfx_saadc_channel_t
 */
#define NRFX_SAADC_DEFAULT_CHANNEL_DIFFERENTIAL(_pin_p, _pin_n, _index) \
{                                                                       \
    .channel_config =                                                   \
    {                                                                   \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_CONFIG_RES,                   \
                         (.resistor_p = NRF_SAADC_RESISTOR_DISABLED,    \
                          .resistor_n = NRF_SAADC_RESISTOR_DISABLED,),  \
                         ())                                            \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_GAIN,                         \
                         (.gain = NRF_SAADC_GAIN1,),                    \
                         ())                                            \
        .reference  = NRF_SAADC_REFERENCE_INTERNAL,                     \
        .acq_time   = NRFX_SAADC_DEFAULT_ACQTIME,                       \
        .mode       = NRF_SAADC_MODE_DIFFERENTIAL,                      \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_BURST,                        \
                         (.burst = NRF_SAADC_BURST_DISABLED,),          \
                         ())                                            \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_CHOPPING,                     \
                         (.chopping = NRF_SAADC_CHOPPING_DISABLED,),    \
                         ())                                            \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_HIGHSPEED,                    \
                         (.highspeed = NRF_SAADC_HIGHSPEED_DISABLED,),  \
                         ())                                            \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CONV_TIME,                       \
                         (.conv_time = NRFX_SAADC_DEFAULT_CONV_TIME,),  \
                         ())                                            \
    },                                                                  \
    .pin_p          = (nrfx_analog_input_t)_pin_p,                      \
    .pin_n          = (nrfx_analog_input_t)_pin_n,                      \
    .channel_index  = _index,                                           \
}

/**
 * @brief Macro for getting number of bytes needed to store specified number of SAADC samples.
 *
 * @param[in] _samples    Number of samples.
 *
 * @return Number of bytes needed to store specified number of samples.
 */
#define NRFX_SAADC_SAMPLES_TO_BYTES(_samples) (_samples * 2)

/**
 * @brief Macro for getting specified SAADC sample from the filled buffer.
 *
 * @param[in] _samples    Pointer to the buffer filled with SAADC samples.
 * @param[in] _index      Sample index.
 *
 * @return Specified sample.
 */
#define NRFX_SAADC_SAMPLE_GET(_samples, _index) (((int16_t *)(_samples))[(_index)])

/**
 * @brief SAADC driver advanced mode default configuration.
 *
 * This configuration sets up advanced mode of the SAADC driver with the following options:
 * - oversampling disabled
 * - burst disabled
 * - internal sampling timer disabled
 * - triggering of the START task on the END event disabled
 *
 * @param[in] _pin_p Positive input analog pin.
 * @param[in] _pin_n Negative input analog pin.
 * @param[in] _index Channel index.
 *
 * @sa nrfx_saadc_adv_config_t
 */
#define NRFX_SAADC_DEFAULT_ADV_CONFIG                                           \
{                                                                               \
    .oversampling      = NRF_SAADC_OVERSAMPLE_DISABLED,                         \
    .burst             = NRF_SAADC_BURST_DISABLED,                              \
    .internal_timer_cc = 0,                                                     \
    .start_on_end      = false,                                                 \
}

/** @brief SAADC channel configuration structure. */
typedef struct
{
    nrf_saadc_channel_config_t channel_config; ///< Channel hardware configuration.
    nrfx_analog_input_t        pin_p;          ///< Input positive pin selection.
    nrfx_analog_input_t        pin_n;          ///< Input negative pin selection.
    uint8_t                    channel_index;  ///< Channel index.
} nrfx_saadc_channel_t;

/** @brief SAADC driver advanced mode configuration structure. */
typedef struct
{
    nrf_saadc_oversample_t oversampling;      ///< Oversampling configuration.
    nrf_saadc_burst_t      burst;             ///< Burst configuration.
    uint16_t               internal_timer_cc; ///< Internal timer capture and compare value.
    bool                   start_on_end;      ///< Flag indicating if the START task is to be triggered on the END event.
} nrfx_saadc_adv_config_t;

/** @brief SAADC driver event types. */
typedef enum
{
    NRFX_SAADC_EVT_DONE,          ///< Event generated when the buffer is filled with samples.
    NRFX_SAADC_EVT_LIMIT,         ///< Event generated when one of the limits is reached.
    NRFX_SAADC_EVT_CALIBRATEDONE, ///< Event generated when the calibration is complete.
    NRFX_SAADC_EVT_BUF_REQ,       ///< Event generated when the next buffer for continuous conversion is requested.
    NRFX_SAADC_EVT_READY,         ///< Event generated when the first buffer is acquired by the peripheral and sampling can be started.
    NRFX_SAADC_EVT_FINISHED,      ///< Event generated when all supplied buffers are filled with results.
} nrfx_saadc_evt_type_t;

/** @brief SAADC driver done event data. */
typedef struct
{
    nrf_saadc_value_t * p_buffer; ///< Pointer to the buffer with converted samples.
    uint16_t            size;     ///< Number of samples in the buffer.
} nrfx_saadc_done_evt_t;

/** @brief SAADC driver limit event data. */
typedef struct
{
    uint8_t           channel;    ///< Channel on which the limit was detected.
    nrf_saadc_limit_t limit_type; ///< Type of limit detected.
} nrfx_saadc_limit_evt_t;

/** @brief SAADC driver event structure. */
typedef struct
{
    nrfx_saadc_evt_type_t type; ///< Event type.
    union
    {
        nrfx_saadc_done_evt_t  done;  ///< Data for @ref NRFX_SAADC_EVT_DONE event.
        nrfx_saadc_limit_evt_t limit; ///< Data for @ref NRFX_SAADC_EVT_LIMIT event.
    } data;                           ///< Union to store event data.
} nrfx_saadc_evt_t;

/**
 * @brief SAADC driver event handler.
 *
 * When operating in the advanced mode:
 * - when the sampling is performed by the external timer, the external timer can be safely started
 *   on @ref NRFX_SAADC_EVT_READY and stopped on @ref NRFX_SAADC_EVT_FINISHED.
 * - call the @ref nrfx_saadc_buffer_set() on @ref NRFX_SAADC_EVT_BUF_REQ to achieve the continuous conversion.
 *
 * @param[in] p_event Pointer to an SAADC driver event. The event structure is allocated on
 *                    the stack, so it is valid only within the context of the event handler.
 */
typedef void (* nrfx_saadc_event_handler_t)(nrfx_saadc_evt_t const * p_event);

/**
 * @brief Function for initializing the SAADC driver.
 *
 * @param[in] interrupt_priority Interrupt priority.
 *
 * @retval 0         Initialization was successful.
 * @retval -EALREADY The driver is already initialized.
 */
int nrfx_saadc_init(uint8_t interrupt_priority);

/**
 * @brief Function for uninitializing the SAADC driver.
 *
 * This function stops all ongoing conversions and disables all channels.
 */
void nrfx_saadc_uninit(void);

/**
 * @brief Function for checking if the SAADC driver is initialized.
 *
 * @retval true  Driver is already initialized.
 * @retval false Driver is not initialized.
 */
bool nrfx_saadc_init_check(void);

/**
 * @brief Function for getting the internal timer CC value from the interval in microseconds.
 *
 * @note The internal timer runs at 16 MHz, so to convert the interval in microseconds
 *       to the internal timer CC value, we can use the formula:
 *       interval_cc = interval_us * 16 MHz
 *       where 16 MHz is the frequency of the internal timer.
 * @note The maximum value for interval_cc is 2047, which corresponds to
 *       approximately 7816 Hz ~ 128us.
 *       The minimum value for interval_cc depends on the SoC.
 *
 * @param[in] interval_us The interval in microseconds to be converted.
 *
 * @return The internal timer CC value.
 */
uint16_t nrfx_saadc_interval_to_cc(uint16_t interval_us);

/**
 * @brief Function for configuring multiple SAADC channels.
 *
 * @note The values of the @ref nrf_saadc_channel_config_t.burst fields in channel configurations
 *       are ignored. They will be overridden with the value suitable for the selected driver
 *       operation mode.
 * @note The desired mode (simple or advanced) must be set after the channels are configured.
 *
 * @warning This function overrides previous configuration done on any channel by
 *          @ref nrfx_saadc_channels_config or @ref nrfx_saadc_channel_config.
 *
 * @param[in] p_channels    Pointer to the array of channel configuration structures.
 * @param[in] channel_count Number of channels to be configured.
 *
 * @retval 0       Configuration was successful.
 * @retval -EBUSY  There is a conversion or calibration ongoing.
 * @retval -EINVAL Attempt to configure the same channel more than once or
 *                 attempt to configure an invalid analog pin.
 */
int nrfx_saadc_channels_config(nrfx_saadc_channel_t const * p_channels,
                               uint32_t                     channel_count);

/**
 * @brief Function for configuring single SAADC channel.
 *
 * @note The values of the @ref nrf_saadc_channel_config_t.burst fields in channel configurations
 *       are ignored. They will be overridden with the value suitable for the selected driver
 *       operation mode.
 *
 * @warning This function overrides previous configuration done on specified channel by
 *          @ref nrfx_saadc_channels_config or @ref nrfx_saadc_channel_config.
 *
 * @param[in] p_channel Pointer to the channel configuration structure.
 *
 * @retval 0       Configuration was successful.
 * @retval -EBUSY  There is a conversion or calibration ongoing.
 * @retval -EINVAL Attempt to configure an invalid analog pin.
 */
int nrfx_saadc_channel_config(nrfx_saadc_channel_t const * p_channel);

/**
 * @brief Function for getting the currently configured SAADC channels.
 *
 * @return Bitmask of configured channels.
 */
uint32_t nrfx_saadc_channels_configured_get(void);

/**
 * @brief Function for deconfiguring the specified SAADC channels.
 *
 * @warning Pins associated with the deconfigured channels will be released after
 *          next @ref nrfx_saadc_simple_mode_set() or @ref nrfx_saadc_advanced_mode_set() call.
 *
 * @param[in] channel_mask Bitmask of channels to be deconfigured.
 *
 * @retval 0      Deconfiguration was successful.
 * @retval -EBUSY There is a conversion or calibration ongoing.
 */
int nrfx_saadc_channels_deconfig(uint32_t channel_mask);

/**
 * @brief Function for setting the SAADC driver in the simple mode.
 *
 * The simple mode allows obtaining a single sample from each requested channel.
 * The conversion can be done in a blocking or non-blocking manner.
 * Sampling is initiated by calling @ref nrfx_saadc_mode_trigger() once.
 *
 * @param[in] channel_mask  Bitmask of channels to be used in the simple mode.
 * @param[in] resolution    Resolution configuration.
 * @param[in] oversampling  Oversampling configuration.
 * @param[in] event_handler Event handler provided by the user. In case of providing NULL,
 *                          the conversion will be performed in the blocking manner.
 *
 * @retval 0       Initialization was successful.
 * @retval -EBUSY  There is a conversion or calibration ongoing.
 * @retval -EINVAL Attempt to activate channel that is not configured.
 */
int nrfx_saadc_simple_mode_set(uint32_t                   channel_mask,
                               nrf_saadc_resolution_t     resolution,
                               nrf_saadc_oversample_t     oversampling,
                               nrfx_saadc_event_handler_t event_handler);

/**
 * @brief Function for setting the SAADC driver in the advanced mode.
 *
 * The advanced mode allows performing double-buffered conversions of arbitrary length.
 * The conversions can be done in a blocking or non-blocking manner. When performing conversions
 * in the non-blocking manner and @ref nrfx_saadc_adv_config_t.internal_timer_cc is set to 0,
 * sampling needs to be done by triggering @ref NRF_SAADC_TASK_SAMPLE externally
 * (for example by using the TIMER and/or the PPI/DPPI).
 * When performing conversions in the non-blocking manner and @ref nrfx_saadc_adv_config_t.start_on_end
 * is false, the @ref NRF_SAADC_TASK_START needs to be triggered on @ref NRF_SAADC_EVENT_END
 * externally (for example by using the PPI/DPPI).
 * Sampling is initiated by calling @ref nrfx_saadc_mode_trigger(). In case of performing
 * conversions in the blocking manner, @ref nrfx_saadc_mode_trigger() may need to be called several
 * times as each call sample each requested channel once.
 *
 * @note The internal timer can only be used when a single input channel is enabled.
 * @note The internal timer can only be used in the non-blocking mode.
 *
 * @param[in] channel_mask  Bitmask of channels to be used in the advanced mode.
 * @param[in] resolution    Resolution configuration.
 * @param[in] p_config      Pointer to the structure with the advanced mode configuration.
 * @param[in] event_handler Event handler provided by the user. In case of providing NULL,
 *                          the conversion will be performed in the blocking manner.
 *
 * @retval 0        Initialization was successful.
 * @retval -EBUSY   There is a conversion or calibration ongoing.
 * @retval -EINVAL  Attempt to activate channel that is not configured.
 * @retval -ENOTSUP Attempt to activate either of the following:
 *                      * internal timer in the blocking mode,
 *                      * internal timer with multiple channels enabled,
 *                      * oversampling without burst with multiple channels enabled.
 */
int nrfx_saadc_advanced_mode_set(uint32_t                        channel_mask,
                                 nrf_saadc_resolution_t          resolution,
                                 nrfx_saadc_adv_config_t const * p_config,
                                 nrfx_saadc_event_handler_t      event_handler);

/**
 * @brief Function for supplying the buffer to be used in the next part of
 *        the conversion.
 *
 * @param[in] p_buffer Pointer to the buffer to be filled with conversion results.
 * @param[in] size     Number of samples in the buffer.
 *
 * @retval 0            Buffer was supplied successfully.
 * @retval -EACCES      The provided buffer is not in the Data RAM region.
 * @retval -E2BIG       The provided buffer is not aligned to the number of activated channels
 *                      or is too long for the EasyDMA to handle.
 * @retval -EINPROGRESS The driver is in the idle mode.
 * @retval -EALREADY    Both buffers for double-buffered conversions are already set.
 */
int nrfx_saadc_buffer_set(nrf_saadc_value_t * p_buffer, uint16_t size);

/**
 * @brief Function for triggering the conversion in the configured mode.
 *
 * @retval 0            Operation finished successfully in the blocking manner or started
 *                      successfully in the non-blocking manner.
 * @retval -EBUSY       The driver is performing the conversion in the advanced blocking mode.
 *                      Call the function again to continue the conversion.
 * @retval -ENOMEM      There is no buffer provided.
 *                      Supply the buffer using @ref nrfx_saadc_buffer_set() and try again.
 * @retval -EINPROGRESS There is an ongoing conversion or calibration being performed
 *                      in the non-blocking manner or the driver is in the idle mode.
 */
int nrfx_saadc_mode_trigger(void);

/**
 * @brief Function for aborting the ongoing and buffered conversions.
 *
 * @warning Aborting blocking conversion or calibration from different context is not supported.
 *          Perform the operation in non-blocking manner instead.
 *
 * @note @ref NRFX_SAADC_EVT_DONE event will be generated if there is a conversion in progress.
 *       Event will contain number of words in the sample buffer.
 */
void nrfx_saadc_abort(void);

/**
 * @brief Function for setting the SAADC channel limits.
 *
 * When limits are enabled and the conversion result exceeds the defined bounds,
 * the handler function is called with the corresponding event as parameter.
 *
 * @note Before the limits are set, the driver operation mode (simple or advanced) has
 *       to be configured. Only non-blocking conversions can be monitored.
 *
 * @note Changing of the driver operation mode disables all configured limits.
 *
 * @param[in] channel    Channel index.
 * @param[in] limit_low  Limit low value to generate interrupt. Use @c INT16_MIN
 *                       to disable interrupt generation.
 * @param[in] limit_high Limit high value to generate interrupt. Use @c INT16_MAX
 *                       to disable interrupt generation.
 *
 * @retval 0            Requested channel limits were set.
 * @retval -EINVAL      Attempt to activate the limits on disabled channel.
 * @retval -EPERM       Attempt to activate the limits for blocking conversions.
 * @retval -EINPROGRESS Attempt to activate the limits without configured mode.
 */
int nrfx_saadc_limits_set(uint8_t channel, int16_t limit_low, int16_t limit_high);

/**
 * @brief Function for starting the SAADC offset calibration.
 *
 * @param[in] calib_event_handler Calibration event handler provided by the user. In case of providing NULL,
 *                                the calibration will be performed in the blocking manner.
 *
 * @retval 0      Calibration finished successfully in the blocking manner
 *                or started successfully in the non-blocking manner.
 * @retval -EBUSY There is a conversion or calibration ongoing.
 */
int nrfx_saadc_offset_calibrate(nrfx_saadc_event_handler_t calib_event_handler);

/** @} */

void nrfx_saadc_irq_handler(void);

#ifdef __cplusplus
}
#endif

#endif // NRFX_SAADC_H__
