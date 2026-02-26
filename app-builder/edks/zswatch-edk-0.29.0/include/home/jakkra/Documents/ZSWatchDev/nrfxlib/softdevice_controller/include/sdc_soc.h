/*
 * Copyright (c) Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


/**
 * @file sdc_soc.h
 * @defgroup sdc_soc SoftDevice Controller SoC Interface
 * @ingroup sdc
 *
 * The SoftDevice Controller SoC interface provides APIs for block encryption and random numbers.
 * While the SoftDevice Controller is enabled, the application should only use the provided APIs to
 * access NRF_ECB. Not doing so will lead to undefined behavior.
 * @{
 */


#ifndef SDC_SOC_H__
#define SDC_SOC_H__


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include "mpsl_hwres.h"

#if defined(NRF52_SERIES)
  /* PPI channels 17 - 31, for the nRF52 Series */
  #define SDC_PPI_CHANNELS_USED_MASK      (0xfffe0000)
#elif defined(NRF53_SERIES)
  /* PPI channels 3 - 12, for the nRF53 Series */
  #define SDC_DPPI_CHANNELS_USED_MASK     (0x00001ff8)
#elif defined(LUMOS_XXAA)
  #define SDC_DPPIC10_CHANNELS_USED_MASK  (0x00000ffe)
  #define SDC_DPPIC00_CHANNELS_USED_MASK  (0x0000000a)
  #define SDC_PPIB00_CHANNELS_USED_MASK   (0x0000000f)
  #define SDC_PPIB10_CHANNELS_USED_MASK   (0x0000000f)
#elif defined(NRF54H_SERIES) || defined(GRTC_PRESENT)
  #define SDC_DPPIC020_CHANNELS_USED_MASK (0x00000ffe)
  #define SDC_DPPIC030_CHANNELS_USED_MASK (0x0000000a)
  #define SDC_PPIB020_CHANNELS_USED_MASK  (0x0000000f)
  #define SDC_PPIB030_CHANNELS_USED_MASK  (0x0000000f)
#endif

/**
 * @brief Functions used by the SoftDevice Controller to obtain random numbers.
 *
 * These functions are used for security-related purposes, so they must conform to:
 * BLUETOOTH CORE SPECIFICATION Version 5.2 | Vol 2, Part H, Section 2.
 */
typedef struct
{
    /** @brief Function used by the controller to obtain random numbers.
     *  This function must block until length bytes of random numbers were written to p_buff.
     *  The function will be executed in the same context as mpsl_low_priority_process.
     *
     *  @param[out]  p_buff  The destination buffer for the random numbers
     *  @param[in]   length  The requested number of random bytes and the maximum length of the destination buffer
     */
    void (*rand_poll)(uint8_t *p_buff, uint8_t length);
} sdc_rand_source_t;

/** @brief Pass a source of randomness to the SoftDevice Controller
 *
 * The SoftDevice Controller will use the function pointers provided in this function call to get random numbers.
 *
 * @param[in] rand_source  A table of function pointers to obtain random numbers
 *
 * @retval 0 Success
 */
int32_t sdc_rand_source_register(const sdc_rand_source_t *rand_source);


#ifdef __cplusplus
}
#endif

/** @} end of sdc_soc */

#endif /* SDC_SOC_H__ */
