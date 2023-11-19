/* avago_apds9306.h - Driver for Broadcom / Avago APDS9306 light sensor. */

/*
 * Copyright (c) 2023, Daniel Kampert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/** @brief Attribute to set the gain range.
*/
#define SENSOR_APDS9306_ATTR_GAIN       (SENSOR_ATTR_PRIV_START + 1)

/** @brief Attribute to set the resolution.
*/
#define SENSOR_APDS9306_ATTR_RESOLUTION (SENSOR_ATTR_PRIV_START + 2)

/** @brief APDS9306 resolution options.
*/
typedef enum {
    APDS9306_RES_20     = 0,        /**< 20 bit resolution. */
    APDS9306_RES_19,                /**< 19 bit resolution. */
    APDS9306_RES_18,                /**< 18 bit resolution (default). */
    APDS9306_RES_17,                /**< 17 bit resolution. */
    APDS9306_RES_16,                /**< 16 bit resolution. */
    APDS9306_RES_13,                /**< 13 bit resolution. */
} APDS9306_Resolution_t;

/** @brief APDS9306 measurement rate options.
*/
typedef enum {
    APDS9306_RATE_25MS  = 0,        /**< 25 ms measurement rate. */
    APDS9306_RATE_50MS,             /**< 50 ms measurement rate. */
    APDS9306_RATE_100MS,            /**< 100 ms measurement rate (default). */
    APDS9306_RATE_200MS,            /**< 200 ms measurement rate. */
    APDS9306_RATE_500MS,            /**< 500 ms measurement rate. */
    APDS9306_RATE_1S,               /**< 1 s measurement rate. */
    APDS9306_RATE_2S,               /**< 2 s measurement rate. */
} APDS9306_Rate_t;

/** @brief APDS9306 gain range options.
*/
typedef enum {
    APDS9306_GAIN_1     = 0,        /**< Measurement gain x1. */
    APDS9306_GAIN_3,                /**< Measurement gain x3 (default). */
    APDS9306_GAIN_6,                /**< Measurement gain x6. */
    APDS9306_GAIN_9,                /**< Measurement gain x9. */
    APDS9306_GAIN_18,               /**< Measurement gain x18. */
} APDS9306_Gain_t;