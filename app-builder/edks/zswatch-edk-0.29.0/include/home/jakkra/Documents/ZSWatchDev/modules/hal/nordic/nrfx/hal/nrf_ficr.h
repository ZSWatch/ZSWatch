/*
 * Copyright (c) 2018 - 2025, Nordic Semiconductor ASA
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

#ifndef NRF_FICR_H__
#define NRF_FICR_H__

#include <nrfx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup nrf_ficr_hal FICR HAL
 * @{
 * @ingroup nrf_icr
 * @brief   Hardware access layer (HAL) for getting data from
 *          the Factory Information Configuration Registers (FICR).
 */

#if defined(FICR_CODEPAGESIZE_CODEPAGESIZE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR CODEPAGESIZE register is present. */
#define NRF_FICR_HAS_CODE_PAGE_SIZE 1
#else
#define NRF_FICR_HAS_CODE_PAGE_SIZE 0
#endif

#if defined(FICR_INFO_CODEPAGESIZE_CODEPAGESIZE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR INFO.CODEPAGESIZE register is present. */
#define NRF_FICR_HAS_INFO_CODE_PAGE_SIZE 1
#else
#define NRF_FICR_HAS_INFO_CODE_PAGE_SIZE 0
#endif

#if defined(FICR_CODESIZE_CODESIZE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR CODESIZE register is present. */
#define NRF_FICR_HAS_CODE_SIZE 1
#else
#define NRF_FICR_HAS_CODE_SIZE 0
#endif

#if defined(FICR_INFO_CODESIZE_CODESIZE_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR INFO.CODESIZE register is present. */
#define NRF_FICR_HAS_INFO_CODE_SIZE 1
#else
#define NRF_FICR_HAS_INFO_CODE_SIZE 0
#endif

#if defined(FICR_DEVICEID_DEVICEID_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR DEVICEID[n] registers are present. */
#define NRF_FICR_HAS_DEVICE_ID 1
#else
#define NRF_FICR_HAS_DEVICE_ID 0
#endif

#if defined(FICR_INFO_DEVICEID_DEVICEID_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR INFO.DEVICEID[n] registers are present. */
#define NRF_FICR_HAS_INFO_DEVICE_ID 1
#else
#define NRF_FICR_HAS_INFO_DEVICE_ID 0
#endif

#if defined(FICR_NFC_TAGHEADER0_MFGID_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR NFC.TAGHEADERn registers are present. */
#define NRF_FICR_HAS_NFC_TAGHEADER 1
#else
#define NRF_FICR_HAS_NFC_TAGHEADER 0
#endif

#if defined(FICR_NFC_TAGHEADER_UD0_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR NFC registers have tagheader array layout. */
#define NRF_FICR_HAS_NFC_TAGHEADER_ARRAY 1
#else
#define NRF_FICR_HAS_NFC_TAGHEADER_ARRAY 0
#endif

#if defined(FICR_DEVICEADDR_DEVICEADDR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR DEVICEADDR[n] registers are present. */
#define NRF_FICR_HAS_DEVICE_ADDR 1
#else
#define NRF_FICR_HAS_DEVICE_ADDR 0
#endif

#if defined(FICR_BLE_ADDR_ADDR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR BLE.ADDR[n] registers are present. */
#define NRF_FICR_HAS_BLE_ADDR 1
#else
#define NRF_FICR_HAS_BLE_ADDR 0
#endif

#if defined(FICR_ER_ER_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR ER[n] registers are present. */
#define NRF_FICR_HAS_ER 1
#else
#define NRF_FICR_HAS_ER 0
#endif

#if defined(FICR_BLE_ER_ER_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR BLE.ER[n] registers are present. */
#define NRF_FICR_HAS_BLE_ER 1
#else
#define NRF_FICR_HAS_BLE_ER 0
#endif

#if defined(FICR_IR_IR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR IR[n] registers are present. */
#define NRF_FICR_HAS_IR 1
#else
#define NRF_FICR_HAS_IR 0
#endif

#if defined(FICR_BLE_IR_IR_Msk) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR BLE.IR[n] registers are present. */
#define NRF_FICR_HAS_BLE_IR 1
#else
#define NRF_FICR_HAS_BLE_IR 0
#endif

#if defined(FICR_TRIM_GLOBAL_COMP_REFTRIM_VALUE_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR GLOBAL.COMP.REFTRIM register is present. */
#define NRF_FICR_HAS_GLOBAL_COMP_REFTRIM 1
#else
#define NRF_FICR_HAS_GLOBAL_COMP_REFTRIM 0
#endif

#if defined(FICR_TRIM_GLOBAL_SAADC_CAL_VALUE_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR GLOBAL.SAADC.CAL register is present. */
#define NRF_FICR_HAS_GLOBAL_SAADC_CAL 1
#else
#define NRF_FICR_HAS_GLOBAL_SAADC_CAL 0
#endif

#if defined(FICR_TRIM_GLOBAL_SAADC_CALREF_VALUE_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR GLOBAL.SAADC.CALREF register is present. */
#define NRF_FICR_HAS_GLOBAL_SAADC_CALREF 1
#else
#define NRF_FICR_HAS_GLOBAL_SAADC_CALREF 0
#endif

#if defined(FICR_TRIM_GLOBAL_SAADC_LINCALCOEFF_VALUE_Pos) || defined(__NRFX_DOXYGEN__)
/** @brief Symbol indicating whether FICR GLOBAL.SAADC.LINCALCOEFF register is present. */
#define NRF_FICR_HAS_GLOBAL_SAADC_LINCALCOEFF 1
#else
#define NRF_FICR_HAS_GLOBAL_SAADC_LINCALCOEFF 0
#endif

#if NRF_FICR_HAS_CODE_PAGE_SIZE || NRF_FICR_HAS_INFO_CODE_PAGE_SIZE
/**
 * @brief Function for getting the size of the code memory page.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Code memory page size in bytes.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_codepagesize_get(NRF_FICR_Type const * p_reg);
#endif

#if NRF_FICR_HAS_CODE_SIZE || NRF_FICR_HAS_INFO_CODE_SIZE
/**
 * @brief Function for getting the size of the code memory rendered as number of pages.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Code memory size rendered as number of pages.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_codesize_get(NRF_FICR_Type const * p_reg);
#endif

#if NRF_FICR_HAS_DEVICE_ID || NRF_FICR_HAS_INFO_DEVICE_ID
/**
 * @brief Function for getting the unique device identifier.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] reg_id Register index.
 *
 * @return Unique device identifier.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_deviceid_get(NRF_FICR_Type const * p_reg, uint32_t reg_id);
#endif

#if NRF_FICR_HAS_NFC_TAGHEADER || NRF_FICR_HAS_NFC_TAGHEADER_ARRAY
/**
 * @brief Function for getting the default header values for the NFC tag.
 *
 * @param[in] p_reg        Pointer to the structure of registers of the peripheral.
 * @param[in] tagheader_id Tag header index.
 *
 * @return The default header value of the NFC tag for the specified header index.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_nfc_tagheader_get(NRF_FICR_Type const * p_reg,
                                                      uint32_t              tagheader_id);
#endif

#if NRF_FICR_HAS_DEVICE_ADDR || NRF_FICR_HAS_BLE_ADDR
/**
 * @brief Function for getting the unique device address.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] reg_id Register index.
 *
 * @return Unique device address.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_deviceaddr_get(NRF_FICR_Type const * p_reg, uint32_t reg_id);
#endif

#if NRF_FICR_HAS_ER || NRF_FICR_HAS_BLE_ER
/**
 * @brief Function for getting the unique encryption root.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] reg_id Register index.
 *
 * @return Unique encryption root.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_er_get(NRF_FICR_Type const * p_reg, uint32_t reg_id);
#endif

#if NRF_FICR_HAS_IR || NRF_FICR_HAS_BLE_IR
/**
 * @brief Function for getting the unique identity root.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] reg_id Register index.
 *
 * @return Unique identity root.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_ir_get(NRF_FICR_Type const * p_reg, uint32_t reg_id);
#endif

#if NRF_FICR_HAS_GLOBAL_COMP_REFTRIM
/**
 * @brief Function for getting the reftrim value for comparator.
 *
 * @param[in] p_reg Pointer to the structure of registers of the peripheral.
 *
 * @return Retrim value for comparator.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_global_comp_reftrim_get(NRF_FICR_Type const * p_reg);
#endif

#if NRF_FICR_HAS_GLOBAL_SAADC_CAL
/**
 * @brief Function for getting the calibration value of a given index for SAADC.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] reg_id Register index.
 *
 * @return Calibration value.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_global_saadc_cal_get(NRF_FICR_Type const * p_reg,
                                                         uint32_t              reg_id);
#endif

#if NRF_FICR_HAS_GLOBAL_SAADC_CALREF
/**
 * @brief Function for getting the reference calibration value for SAADC.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 *
 * @return Reference calibration value.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_global_saadc_calref_get(NRF_FICR_Type const * p_reg);

#endif

#if NRF_FICR_HAS_GLOBAL_SAADC_LINCALCOEFF
/**
 * @brief Function for getting the linear calibration coefficient of a given index for SAADC.
 *
 * @param[in] p_reg  Pointer to the structure of registers of the peripheral.
 * @param[in] reg_id Register index.
 *
 * @return Linear calibration coefficient.
 */
NRF_STATIC_INLINE uint32_t nrf_ficr_global_saadc_lincalcoeff_get(NRF_FICR_Type const * p_reg,
                                                                 uint8_t               reg_id);
#endif

#ifndef NRF_DECLARE_ONLY

#if NRF_FICR_HAS_CODE_PAGE_SIZE || NRF_FICR_HAS_INFO_CODE_PAGE_SIZE
NRF_STATIC_INLINE uint32_t nrf_ficr_codepagesize_get(NRF_FICR_Type const * p_reg)
{
#if NRF_FICR_HAS_INFO_CODE_PAGE_SIZE
    return p_reg->INFO.CODEPAGESIZE;
#else
    return p_reg->CODEPAGESIZE;
#endif
}
#endif

#if NRF_FICR_HAS_CODE_SIZE || NRF_FICR_HAS_INFO_CODE_SIZE
NRF_STATIC_INLINE uint32_t nrf_ficr_codesize_get(NRF_FICR_Type const * p_reg)
{
#if NRF_FICR_HAS_INFO_CODE_SIZE
    return p_reg->INFO.CODESIZE;
#else
    return p_reg->CODESIZE;
#endif
}
#endif

#if NRF_FICR_HAS_DEVICE_ID || NRF_FICR_HAS_INFO_DEVICE_ID
NRF_STATIC_INLINE uint32_t nrf_ficr_deviceid_get(NRF_FICR_Type const * p_reg, uint32_t reg_id)
{
#if NRF_FICR_HAS_INFO_DEVICE_ID
    return p_reg->INFO.DEVICEID[reg_id];
#else
    return p_reg->DEVICEID[reg_id];
#endif
}
#endif

#if NRF_FICR_HAS_NFC_TAGHEADER || NRF_FICR_HAS_NFC_TAGHEADER_ARRAY
NRF_STATIC_INLINE uint32_t nrf_ficr_nfc_tagheader_get(NRF_FICR_Type const * p_reg,
                                                      uint32_t              tagheader_id)
{
#if NRF_FICR_HAS_NFC_TAGHEADER_ARRAY
    switch(tagheader_id) {
        case 0:
            return p_reg->NFC.TAGHEADER[0];
        case 1:
            return p_reg->NFC.TAGHEADER[1];
        case 2:
            return p_reg->NFC.TAGHEADER[2];
        case 3:
            return p_reg->NFC.TAGHEADER[3];
        default:
            return 0;
    }
#else
    switch(tagheader_id) {
        case 0:
            return p_reg->NFC.TAGHEADER0;
        case 1:
            return p_reg->NFC.TAGHEADER1;
        case 2:
            return p_reg->NFC.TAGHEADER2;
        case 3:
            return p_reg->NFC.TAGHEADER3;
        default:
            return 0;
    }
#endif
}
#endif

#if NRF_FICR_HAS_DEVICE_ADDR || NRF_FICR_HAS_BLE_ADDR
NRF_STATIC_INLINE uint32_t nrf_ficr_deviceaddr_get(NRF_FICR_Type const * p_reg, uint32_t reg_id)
{
#if NRF_FICR_HAS_BLE_ADDR
    return p_reg->BLE.ADDR[reg_id];
#else
    return p_reg->DEVICEADDR[reg_id];
#endif
}
#endif

#if NRF_FICR_HAS_ER || NRF_FICR_HAS_BLE_ER
NRF_STATIC_INLINE uint32_t nrf_ficr_er_get(NRF_FICR_Type const * p_reg, uint32_t reg_id)
{
#if NRF_FICR_HAS_BLE_ER
    return p_reg->BLE.ER[reg_id];
#else
    return p_reg->ER[reg_id];
#endif
}
#endif

#if NRF_FICR_HAS_IR || NRF_FICR_HAS_BLE_IR
NRF_STATIC_INLINE uint32_t nrf_ficr_ir_get(NRF_FICR_Type const * p_reg, uint32_t reg_id)
{
#if NRF_FICR_HAS_BLE_IR
    return p_reg->BLE.IR[reg_id];
#else
    return p_reg->IR[reg_id];
#endif
}
#endif

#if NRF_FICR_HAS_GLOBAL_COMP_REFTRIM
NRF_STATIC_INLINE uint32_t nrf_ficr_global_comp_reftrim_get(NRF_FICR_Type const * p_reg)
{
    return p_reg->TRIM.GLOBAL.COMP.REFTRIM;
}
#endif

#if NRF_FICR_HAS_GLOBAL_SAADC_CAL
NRF_STATIC_INLINE uint32_t nrf_ficr_global_saadc_cal_get(NRF_FICR_Type const * p_reg,
                                                         uint32_t              reg_id)
{
    return p_reg->TRIM.GLOBAL.SAADC.CAL[reg_id];
}
#endif

#if NRF_FICR_HAS_GLOBAL_SAADC_CALREF
NRF_STATIC_INLINE uint32_t nrf_ficr_global_saadc_calref_get(NRF_FICR_Type const * p_reg)
{
    return p_reg->TRIM.GLOBAL.SAADC.CALREF;
}
#endif

#if NRF_FICR_HAS_GLOBAL_SAADC_LINCALCOEFF
NRF_STATIC_INLINE uint32_t nrf_ficr_global_saadc_lincalcoeff_get(NRF_FICR_Type const * p_reg,
                                                                 uint8_t               reg_id)
{
    return p_reg->TRIM.GLOBAL.SAADC.LINCALCOEFF[reg_id];
}
#endif

#endif // NRF_DECLARE_ONLY

/** @} */

#ifdef __cplusplus
}
#endif

#endif // NRF_FICR_H__
