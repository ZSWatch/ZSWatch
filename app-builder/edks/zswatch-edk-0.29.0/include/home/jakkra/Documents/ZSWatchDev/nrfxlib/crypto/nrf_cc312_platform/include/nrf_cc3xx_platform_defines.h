/*
 * Copyright (c) 2019-2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
/**@file
 * @defgroup nrf_cc3xx_platform_defines nrf_cc3xx_platform shared defines
 * @ingroup nrf_cc3xx_platform
 * @{
 * @brief nrf_cc3xx_platform shared defines and return codes.
 */
#ifndef NRF_CC3XX_PLATFORM_DEFINES_H__
#define NRF_CC3XX_PLATFORM_DEFINES_H__

#ifdef __cplusplus
extern "C"
{
#endif

/** @brief Definition of max number of entropy bits to gather for CTR_DRBG
 */
#define NRF_CC3XX_PLATFORM_ENTROPY_MAX_GATHER                   (144)

/** @brief Definition of the nonce size for EITS in bytes
 */
#define NRF_CC3XX_PLATFORM_EITS_NONCE_SIZE                      (8)

/** @brief Definition of the TF-M boot seed size in bytes
 */
#define NRF_CC3XX_PLATFORM_TFM_BOOT_SEED_SIZE                   (32)

/** @brief Definition of max count of concurrent usage
 *
 *  @note The max value will never be reached.
 */
#define NRF_CC3XX_PLATFORM_USE_COUNT_MAX                        (10)

#define NRF_CC3XX_PLATFORM_SUCCESS                              (0)
#define NRF_CC3XX_PLATFORM_TRUE                                 (1)
#define NRF_CC3XX_PLATFORM_FALSE                                (0)
#define NRF_CC3XX_PLATFORM_ERROR_PARAM_NULL                     (-0x7001)
#define NRF_CC3XX_PLATFORM_ERROR_INTERNAL                       (-0x7002)
#define NRF_CC3XX_PLATFORM_ERROR_RNG_TEST_FAILED                (-0x7003)
#define NRF_CC3XX_PLATFORM_ERROR_HW_VERSION_FAILED              (-0x7004)
#define NRF_CC3XX_PLATFORM_ERROR_PARAM_WRITE_FAILED             (-0x7005)
#define NRF_CC3XX_PLATFORM_ERROR_MUTEX_NOT_INITIALIZED          (-0x7016)
#define NRF_CC3XX_PLATFORM_ERROR_MUTEX_FAILED                   (-0x7017)
#define NRF_CC3XX_PLATFORM_ERROR_ENTROPY_NOT_INITIALIZED        (-0x7018)
#define NRF_CC3XX_PLATFORM_ERROR_ENTROPY_TRNG_TOO_LONG          (-0x7019)

#define NRF_CC3XX_PLATFORM_ERROR_KMU_INVALID_SLOT               (-0x701a)
#define NRF_CC3XX_PLATFORM_ERROR_KMU_ALREADY_FILLED             (-0x701b)
#define NRF_CC3XX_PLATFORM_ERROR_KMU_WRONG_ADDRESS              (-0x701c)
#define NRF_CC3XX_PLATFORM_ERROR_KMU_WRITE_KEY_FAILED           (-0x701d)
#define NRF_CC3XX_PLATFORM_ERROR_KMU_WRITE_INVALID_PERM         (-0x701e)
#define NRF_CC3XX_PLATFORM_ERROR_KDR_INVALID_WRITE              (-0x701f)
#define NRF_CC3XX_PLATFORM_ERROR_KDR_INVALID_PUSH               (-0x7020)
#define NRF_CC3XX_PLATFORM_ERROR_KMU_INVALID_KEY_TYPE           (-0x7021)

#define NRF_CC3XX_PLATFORM_ERROR_INVALID_PARAM                  (-0x7022)
#define NRF_CC3XX_PLATFORM_ERROR_DERIVED_KEY_CTX_INVALID_STATE  (-0x7023)

#define NRF_CC3XX_PLATFORM_ERROR_IDENTITY_KEY_INVALID_SLOT      (-0x7024)
#define NRF_CC3XX_PLATFORM_ERROR_KIDENT_ALREADY_FILLED          (-0x7025)
#define NRF_CC3XX_PLATFORM_ERROR_KIDENT_WRITE_KEY_FAILED        (-0x7026)
#define NRF_CC3XX_PLATFORM_ERROR_KIDENT_READ_KEY_FAILED         (-0x7027)
#define NRF_CC3XX_PLATFORM_ERROR_KIDENT_MKEK_MISSING            (-0x7028)
#define NRF_CC3XX_PLATFORM_ERROR_KIDENT_INVALID_STATE           (-0x7029)

#define NRF_CC3XX_PLATFORM_DERIVED_KEY_CTX_INITIALIZED          (0x10)
#define NRF_CC3XX_PLATFORM_DERIVED_KEY_DERIV_INFO_SET           (0x20)
#define NRF_CC3XX_PLATFORM_DERIVED_KEY_CIPH_INFO_SET            (0x40)
#define NRF_CC3XX_PLATFORM_DERIVED_KEY_AUTH_INFO_SET            (0x80)


#define NRF_KMU_FIRST_SLOT              (0U)    //!< First addressable key slot in KMU
#define NRF_KMU_SECOND_SLOT             (1U)    //!< Second addressable key slot in KMU
#define NRF_KMU_LAST_SLOT               (127U)  //!< Last addressable key slot in KMU.
#define NRF_KMU_LAST_IDENTTY_KEY_SLOT   (126U)	//!< Last addressable KMU slot for an identity key (Two slots used)

#define NRF_KMU_SLOT_KDR                (0U)    //!< Key slot reserved for Kdr (Also known as HUK or Root derivation key).
#define NRF_KMU_SLOT_KDR_RESERVED       (1U)    //!< Key slot reserved for Kdr (CC312: Used for last 128 bits of key material).

#define NRF_KMU_SLOT_MKEK               (2U)    //!< Key slot reserved for MKEK (Master Key Encryption Key).
#define NRF_KMU_SLOT_MKEK_RESERVED      (3U)    //!< Key slot reserved for MKEK (CC312: Used for last 128 bits of key material).

#define NRF_KMU_SLOT_MEXT               (4U)    //!< Key slot reserved for MEXT (Master External storage encryption Key).
#define NRF_KMU_SLOT_MEXT_RESERVED      (5U)    //!< Key slot reserved for MEXT (CC312: Used for last 128 bits of key material).

#define NRF_KMU_SLOT_KIDENT             (6U)    //!< Key slot reserved for the encrypted KIDENT (Identity key).
#define NRF_KMU_SLOT_KIDENT_RESERVED    (7U)    //!< Key slot reserved for the encrypted KIDENT (Identity key, second slot).

#ifdef __cplusplus
}
#endif

#endif /* NRF_CC3XX_PLATFORM_DEFINES_H__ */

/** @} */
