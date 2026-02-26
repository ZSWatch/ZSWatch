/*
 * Copyright (c) 2018 Open Source Foundries Limited
 * Copyright (c) 2019-2020 Arm Limited
 * Copyright (c) 2019-2020 Linaro Limited
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MCUBOOT_CONFIG_H__
#define __MCUBOOT_CONFIG_H__

#include <zephyr/devicetree.h>

#ifdef CONFIG_BOOT_SIGNATURE_TYPE_RSA
#define MCUBOOT_SIGN_RSA
#  if (CONFIG_BOOT_SIGNATURE_TYPE_RSA_LEN != 2048 && \
       CONFIG_BOOT_SIGNATURE_TYPE_RSA_LEN != 3072)
#    error "Invalid RSA key size (must be 2048 or 3072)"
#  else
#    define MCUBOOT_SIGN_RSA_LEN CONFIG_BOOT_SIGNATURE_TYPE_RSA_LEN
#  endif
#elif defined(CONFIG_BOOT_SIGNATURE_TYPE_ECDSA_P256)
#define MCUBOOT_SIGN_EC256
#elif defined(CONFIG_BOOT_SIGNATURE_TYPE_ED25519)
#define MCUBOOT_SIGN_ED25519
#endif

#if defined(CONFIG_BOOT_USE_TINYCRYPT)
#  if defined(CONFIG_MBEDTLS) || defined(CONFIG_BOOT_USE_CC310)
#     error "One crypto library implementation allowed at a time."
#  endif
#elif defined(CONFIG_MBEDTLS) && defined(CONFIG_BOOT_USE_CC310)
#     error "One crypto library implementation allowed at a time."
#endif

#if defined(CONFIG_BOOT_KEY_IMPORT_BYPASS_ASN)
#define MCUBOOT_KEY_IMPORT_BYPASS_ASN
#endif

#ifdef CONFIG_BOOT_USE_MBEDTLS
#define MCUBOOT_USE_MBED_TLS
#elif defined(CONFIG_BOOT_USE_TINYCRYPT)
#define MCUBOOT_USE_TINYCRYPT
#elif defined(CONFIG_BOOT_USE_CC310)
#define MCUBOOT_USE_CC310
#ifdef CONFIG_BOOT_USE_NRF_CC310_BL
#define MCUBOOT_USE_NRF_CC310_BL
#endif
#elif defined(CONFIG_BOOT_USE_PSA_CRYPTO)
#define MCUBOOT_USE_PSA_CRYPTO
#elif defined(CONFIG_BOOT_USE_NRF_EXTERNAL_CRYPTO)
#define MCUBOOT_USE_NRF_EXTERNAL_CRYPTO
#endif

#ifdef CONFIG_BOOT_IMG_HASH_ALG_SHA512
#define MCUBOOT_SHA512
#endif

#ifdef CONFIG_BOOT_IMG_HASH_ALG_SHA256
#define MCUBOOT_SHA256
#endif

/* Zephyr, regardless of C library used, provides snprintf */
#define MCUBOOT_USE_SNPRINTF 1

#ifdef CONFIG_BOOT_HW_KEY
#define MCUBOOT_HW_KEY
#endif

#ifdef CONFIG_NCS_BOOT_SIGNATURE_USING_ITS
#define MCUBOOT_BUILTIN_KEY
#endif

#ifdef CONFIG_BOOT_VALIDATE_SLOT0
#define MCUBOOT_VALIDATE_PRIMARY_SLOT
#endif

#ifdef CONFIG_BOOT_VALIDATE_SLOT0_ONCE
#define MCUBOOT_VALIDATE_PRIMARY_SLOT_ONCE
#endif

#ifdef CONFIG_BOOT_UPGRADE_ONLY
#define MCUBOOT_OVERWRITE_ONLY
#define MCUBOOT_OVERWRITE_ONLY_FAST
#endif

#ifdef CONFIG_SINGLE_APPLICATION_SLOT
#define MCUBOOT_SINGLE_APPLICATION_SLOT 1
#define MCUBOOT_IMAGE_NUMBER    1
#else

#ifdef CONFIG_BOOT_SWAP_USING_MOVE
#define MCUBOOT_SWAP_USING_MOVE 1
#endif

#ifdef CONFIG_BOOT_SWAP_USING_OFFSET
#define MCUBOOT_SWAP_USING_OFFSET 1
#endif

#ifdef CONFIG_BOOT_DIRECT_XIP
#define MCUBOOT_DIRECT_XIP
#endif

#ifdef CONFIG_BOOT_DIRECT_XIP_REVERT
#define MCUBOOT_DIRECT_XIP_REVERT
#endif

#ifdef CONFIG_BOOT_RAM_LOAD_REVERT
#define MCUBOOT_RAM_LOAD_REVERT
#endif

#ifdef CONFIG_BOOT_RAM_LOAD
#define MCUBOOT_RAM_LOAD 1
#define IMAGE_EXECUTABLE_RAM_START CONFIG_BOOT_IMAGE_EXECUTABLE_RAM_START
#define IMAGE_EXECUTABLE_RAM_SIZE CONFIG_BOOT_IMAGE_EXECUTABLE_RAM_SIZE
#endif

#ifdef CONFIG_BOOT_FIRMWARE_LOADER
#define MCUBOOT_FIRMWARE_LOADER
#endif

#ifdef CONFIG_UPDATEABLE_IMAGE_NUMBER
#define MCUBOOT_IMAGE_NUMBER    CONFIG_UPDATEABLE_IMAGE_NUMBER
#else
#define MCUBOOT_IMAGE_NUMBER    1
#endif

#ifdef CONFIG_BOOT_VERSION_CMP_USE_BUILD_NUMBER
#define MCUBOOT_VERSION_CMP_USE_BUILD_NUMBER
#endif

#ifdef CONFIG_BOOT_SWAP_SAVE_ENCTLV
#define MCUBOOT_SWAP_SAVE_ENCTLV 1
#endif

#endif /* CONFIG_SINGLE_APPLICATION_SLOT */

#ifdef CONFIG_SINGLE_APPLICATION_SLOT_RAM_LOAD
#define MCUBOOT_RAM_LOAD    1
#define MCUBOOT_IMAGE_NUMBER    1
#define MCUBOOT_SINGLE_APPLICATION_SLOT_RAM_LOAD    1
#define IMAGE_EXECUTABLE_RAM_START CONFIG_BOOT_IMAGE_EXECUTABLE_RAM_START
#define IMAGE_EXECUTABLE_RAM_SIZE CONFIG_BOOT_IMAGE_EXECUTABLE_RAM_SIZE
#endif

#ifdef CONFIG_MULTIPLE_EXECUTABLE_RAM_REGIONS
#define MULTIPLE_EXECUTABLE_RAM_REGIONS
#endif

#ifdef CONFIG_LOG
#define MCUBOOT_HAVE_LOGGING 1
#endif

/* Enable/disable non-protected TLV check against allow list */
#ifdef CONFIG_MCUBOOT_USE_TLV_ALLOW_LIST
#define MCUBOOT_USE_TLV_ALLOW_LIST 1
#endif

#ifdef CONFIG_BOOT_ENCRYPT_RSA
#define MCUBOOT_ENC_IMAGES
#define MCUBOOT_ENCRYPT_RSA
#endif

#ifdef CONFIG_BOOT_ENCRYPT_EC256
#define MCUBOOT_ENC_IMAGES
#define MCUBOOT_ENCRYPT_EC256
#endif

#ifdef CONFIG_BOOT_ENCRYPT_X25519
#define MCUBOOT_ENC_IMAGES
#define MCUBOOT_ENCRYPT_X25519
#endif

#ifdef CONFIG_BOOT_ENCRYPT_ALG_AES_128
#define MCUBOOT_AES_128
#endif

#ifdef CONFIG_BOOT_ENCRYPT_ALG_AES_256
#define MCUBOOT_AES_256
#endif

/* Support for HMAC/HKDF using SHA512; this is used in key exchange where
 * HKDF is used for key expansion and HMAC is used for key verification.
 */
#ifdef CONFIG_BOOT_HMAC_SHA512
#define MCUBOOT_HMAC_SHA512
#endif

/* Turn off check of public key hash against compiled in key
 * before attempting signature verification. When there is only
 * one key, matching is pointless, the signature may just be
 * verified with the only key that there is.
 */
#ifdef CONFIG_BOOT_BYPASS_KEY_MATCH
#define MCUBOOT_BYPASS_KEY_MATCH
#endif

#ifdef CONFIG_BOOT_DECOMPRESSION
#define MCUBOOT_DECOMPRESS_IMAGES
#endif

/* Invoke hashing functions directly on storage device. This requires the device
 * be able to map storage to address space or RAM.
 */
#ifdef CONFIG_BOOT_IMG_HASH_DIRECTLY_ON_STORAGE
#define MCUBOOT_HASH_STORAGE_DIRECTLY
#endif

#ifdef CONFIG_BOOT_SIGNATURE_TYPE_PURE
#define MCUBOOT_SIGN_PURE
#endif

#ifdef CONFIG_BOOT_BOOTSTRAP
#define MCUBOOT_BOOTSTRAP 1
#endif

#ifdef CONFIG_BOOT_USE_BENCH
#define MCUBOOT_USE_BENCH 1
#endif

#ifdef CONFIG_MCUBOOT_DOWNGRADE_PREVENTION
#define MCUBOOT_DOWNGRADE_PREVENTION 1
/* MCUBOOT_DOWNGRADE_PREVENTION_SECURITY_COUNTER is used later as bool value so it is
 * always defined, (unlike MCUBOOT_DOWNGRADE_PREVENTION which is only used in
 * preprocessor condition and my be not defined) */
#  ifdef CONFIG_MCUBOOT_DOWNGRADE_PREVENTION_SECURITY_COUNTER
#    define MCUBOOT_DOWNGRADE_PREVENTION_SECURITY_COUNTER 1
#  else
#    define MCUBOOT_DOWNGRADE_PREVENTION_SECURITY_COUNTER 0
#  endif
#endif

#ifdef CONFIG_MCUBOOT_HW_DOWNGRADE_PREVENTION
#define MCUBOOT_HW_ROLLBACK_PROT
#endif

#ifdef CONFIG_MCUBOOT_HW_DOWNGRADE_PREVENTION_COUNTER_LIMITED
#define MCUBOOT_HW_ROLLBACK_PROT_COUNTER_LIMITED
#endif

#ifdef CONFIG_MCUBOOT_HW_DOWNGRADE_PREVENTION_LOCK
#define MCUBOOT_HW_ROLLBACK_PROT_LOCK
#endif

#ifdef CONFIG_MCUBOOT_MANIFEST_UPDATES
#define MCUBOOT_MANIFEST_UPDATES

#ifdef CONFIG_MCUBOOT_MANIFEST_IMAGE_INDEX
#define MCUBOOT_MANIFEST_IMAGE_INDEX CONFIG_MCUBOOT_MANIFEST_IMAGE_INDEX
#endif /* CONFIG_MCUBOOT_MANIFEST_IMAGE_INDEX */
#endif /* CONFIG_MCUBOOT_MANIFEST_UPDATES */

#ifdef CONFIG_MCUBOOT_UUID_VID
#define MCUBOOT_UUID_VID
#endif

#ifdef CONFIG_MCUBOOT_UUID_CID
#define MCUBOOT_UUID_CID
#endif

#ifdef CONFIG_MEASURED_BOOT
#define MCUBOOT_MEASURED_BOOT
#endif

#ifdef CONFIG_BOOT_SHARE_DATA
#define MCUBOOT_DATA_SHARING
#endif

#ifdef CONFIG_BOOT_SHARE_BACKEND_RETENTION
#define MCUBOOT_CUSTOM_DATA_SHARING_FUNCTION
#endif

#ifdef CONFIG_BOOT_SHARE_DATA_BOOTINFO
#define MCUBOOT_DATA_SHARING_BOOTINFO
#endif

#ifdef CONFIG_MEASURED_BOOT_MAX_CBOR_SIZE
#define MAX_BOOT_RECORD_SZ CONFIG_MEASURED_BOOT_MAX_CBOR_SIZE
#endif

#ifdef CONFIG_BOOT_FIH_PROFILE_OFF
#define MCUBOOT_FIH_PROFILE_OFF
#endif

#ifdef CONFIG_BOOT_FIH_PROFILE_LOW
#define MCUBOOT_FIH_PROFILE_LOW
#endif

#ifdef CONFIG_BOOT_FIH_PROFILE_MEDIUM
#define MCUBOOT_FIH_PROFILE_MEDIUM
#endif

#ifdef CONFIG_BOOT_FIH_PROFILE_HIGH
#define MCUBOOT_FIH_PROFILE_HIGH
#endif

#ifdef CONFIG_ENABLE_MGMT_PERUSER
#define MCUBOOT_PERUSER_MGMT_GROUP_ENABLED 1
#else
#define MCUBOOT_PERUSER_MGMT_GROUP_ENABLED 0
#endif

#ifdef CONFIG_BOOT_MGMT_ECHO
#define MCUBOOT_BOOT_MGMT_ECHO
#endif

#ifdef CONFIG_BOOT_IMAGE_ACCESS_HOOKS
#define MCUBOOT_IMAGE_ACCESS_HOOKS
#endif

#ifdef CONFIG_BOOT_GO_HOOKS
#define MCUBOOT_BOOT_GO_HOOKS
#endif

#ifdef CONFIG_BOOT_FLASH_AREA_HOOKS
#define MCUBOOT_FLASH_AREA_HOOKS
#endif

#ifdef CONFIG_FIND_NEXT_SLOT_HOOKS
#define MCUBOOT_FIND_NEXT_SLOT_HOOKS
#endif

#ifdef CONFIG_MCUBOOT_CHECK_HEADER_LOAD_ADDRESS
#define MCUBOOT_CHECK_HEADER_LOAD_ADDRESS
#endif

#ifdef CONFIG_MCUBOOT_VERIFY_IMG_ADDRESS
#define MCUBOOT_VERIFY_IMG_ADDRESS
#endif

#ifdef CONFIG_MCUBOOT_SERIAL
#define MCUBOOT_SERIAL
#endif

/*
 * The configuration option enables direct image upload with the
 * serial recovery.
 */
#ifdef CONFIG_MCUBOOT_SERIAL_DIRECT_IMAGE_UPLOAD
#define MCUBOOT_SERIAL_DIRECT_IMAGE_UPLOAD
#endif

#ifdef CONFIG_BOOT_SERIAL_WAIT_FOR_DFU
#define MCUBOOT_SERIAL_WAIT_FOR_DFU
#endif

#ifdef CONFIG_BOOT_SERIAL_IMG_GRP_HASH
#define MCUBOOT_SERIAL_IMG_GRP_HASH
#endif

#ifdef CONFIG_BOOT_SERIAL_IMG_GRP_IMAGE_STATE
#define MCUBOOT_SERIAL_IMG_GRP_IMAGE_STATE
#endif

#ifdef CONFIG_BOOT_SERIAL_IMG_GRP_SLOT_INFO
#define MCUBOOT_SERIAL_IMG_GRP_SLOT_INFO
#endif

#ifdef CONFIG_MCUBOOT_SERIAL
#define MCUBOOT_SERIAL_RECOVERY
#endif

#if (defined(CONFIG_BOOT_USB_DFU_WAIT) || \
     defined(CONFIG_BOOT_USB_DFU_GPIO))
#define MCUBOOT_USB_DFU
#endif

/*
 * The option enables code, currently in boot_serial, that attempts
 * to erase flash progressively, as update fragments are received,
 * instead of erasing whole image size of flash area after receiving
 * first frame.
 * Enabling this options prevents stalling the beginning of transfer
 * for the time needed to erase large chunk of flash.
 */
#ifdef CONFIG_BOOT_ERASE_PROGRESSIVELY
#define MCUBOOT_ERASE_PROGRESSIVELY
#endif

/*
 * Devices that do not require erase prior to write or do not support
 * erase should avoid emulation of erase by additional write.
 * The emulation is also taking time which doubles required write time
 * for such devices.
 */
#ifdef CONFIG_MCUBOOT_STORAGE_WITHOUT_ERASE
#define MCUBOOT_SUPPORT_DEV_WITHOUT_ERASE
#endif

#ifdef CONFIG_MCUBOOT_STORAGE_WITH_ERASE
#define MCUBOOT_SUPPORT_DEV_WITH_ERASE
#endif

/*
 * MCUboot often calls erase on device just to remove data or make application
 * image not recognizable. In such instances it may be faster to just remove
 * portion of data to make image unrecognizable.
 */
#ifdef CONFIG_MCUBOOT_STORAGE_MINIMAL_SCRAMBLE
#define MCUBOOT_MINIMAL_SCRAMBLE
#endif

/*
 * Enabling this option uses newer flash map APIs. This saves RAM and
 * avoids deprecated API usage.
 *
 * (This can be deleted when flash_area_to_sectors() is removed instead
 * of simply deprecated.)
 */
#define MCUBOOT_USE_FLASH_AREA_GET_SECTORS

#if (defined(CONFIG_BOOT_USB_DFU_WAIT) || \
     defined(CONFIG_BOOT_USB_DFU_GPIO))
#  ifndef CONFIG_MULTITHREADING
#    error "USB DFU Requires MULTITHREADING"
#  endif
#endif

#if defined(CONFIG_BOOT_MAX_IMG_SECTORS_AUTO) && defined(MIN_SECTOR_COUNT)

#define MCUBOOT_MAX_IMG_SECTORS       MIN_SECTOR_COUNT

#elif defined(CONFIG_BOOT_MAX_IMG_SECTORS)

#define MCUBOOT_MAX_IMG_SECTORS       CONFIG_BOOT_MAX_IMG_SECTORS

#else
#define MCUBOOT_MAX_IMG_SECTORS       128
#endif

#ifdef CONFIG_BOOT_SERIAL_MAX_RECEIVE_SIZE
#define MCUBOOT_SERIAL_MAX_RECEIVE_SIZE CONFIG_BOOT_SERIAL_MAX_RECEIVE_SIZE
#endif

#ifdef CONFIG_BOOT_SERIAL_UNALIGNED_BUFFER_SIZE
#define MCUBOOT_SERIAL_UNALIGNED_BUFFER_SIZE CONFIG_BOOT_SERIAL_UNALIGNED_BUFFER_SIZE
#endif

#if defined(MCUBOOT_DATA_SHARING) && defined(ZEPHYR_VER_INCLUDE)
#include <zephyr/app_version.h>

#define MCUBOOT_VERSION_AVAILABLE
#define MCUBOOT_VERSION_MAJOR APP_VERSION_MAJOR
#define MCUBOOT_VERSION_MINOR APP_VERSION_MINOR
#define MCUBOOT_VERSION_PATCHLEVEL APP_PATCHLEVEL
#define MCUBOOT_VERSION_TWEAK APP_TWEAK
#endif

/* Support 32-byte aligned flash sizes */
#if DT_HAS_CHOSEN(zephyr_flash)
    #if DT_PROP_OR(DT_CHOSEN(zephyr_flash), write_block_size, 0) > 8
        #define MCUBOOT_BOOT_MAX_ALIGN \
            DT_PROP(DT_CHOSEN(zephyr_flash), write_block_size)
    #endif
#endif

#ifdef CONFIG_MCUBOOT_BOOTUTIL_LIB_FOR_DIRECT_XIP
#define MCUBOOT_BOOTUTIL_LIB_FOR_DIRECT_XIP 1
#endif

#if CONFIG_BOOT_WATCHDOG_FEED
#if CONFIG_BOOT_WATCHDOG_FEED_NRFX_WDT
#include <nrfx_wdt.h>

#define FEED_WDT_INST(inst)                                  \
    do {                                                     \
        for (uint8_t i = 0; i < NRF_WDT_CHANNEL_NUMBER; i++) \
        {                                                    \
            nrf_wdt_reload_request_set(inst,                 \
                (nrf_wdt_rr_register_t)(NRF_WDT_RR0 + i));   \
        }                                                    \
    } while (0)
#if defined(NRF_WDT0) && defined(NRF_WDT1)
#define MCUBOOT_WATCHDOG_FEED()  \
    do {                         \
        FEED_WDT_INST(NRF_WDT0); \
        FEED_WDT_INST(NRF_WDT1); \
    } while (0)
#elif defined(NRF_WDT0)
#define MCUBOOT_WATCHDOG_FEED() \
    FEED_WDT_INST(NRF_WDT0);
#elif defined(NRF_WDT30) && defined(NRF_WDT31)
#define MCUBOOT_WATCHDOG_FEED()   \
    do {                          \
        FEED_WDT_INST(NRF_WDT30); \
        FEED_WDT_INST(NRF_WDT31); \
    } while (0)
#elif defined(NRF_WDT30)
#define MCUBOOT_WATCHDOG_FEED() \
    FEED_WDT_INST(NRF_WDT30);
#elif defined(NRF_WDT31)
#define MCUBOOT_WATCHDOG_FEED() \
    FEED_WDT_INST(NRF_WDT31);
#elif defined(NRF_WDT010)
#define MCUBOOT_WATCHDOG_FEED() \
    FEED_WDT_INST(NRF_WDT010);
#else
#error "No NRFX WDT instances enabled"
#endif

#elif DT_NODE_HAS_STATUS(DT_ALIAS(watchdog0), okay) /* CONFIG_BOOT_WATCHDOG_FEED_NRFX_WDT */
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>

#define MCUBOOT_WATCHDOG_SETUP()                              \
    do {                                                      \
        const struct device* wdt =                            \
            DEVICE_DT_GET(DT_ALIAS(watchdog0));               \
        if (device_is_ready(wdt)) {                           \
            wdt_setup(wdt, 0);                                \
        }                                                     \
    } while (0)

#define MCUBOOT_WATCHDOG_FEED()                               \
    do {                                                      \
        const struct device* wdt =                            \
            DEVICE_DT_GET(DT_ALIAS(watchdog0));               \
        if (device_is_ready(wdt)) {                           \
            wdt_feed(wdt, 0);                                 \
        }                                                     \
    } while (0)
#else /* DT_NODE_HAS_STATUS(DT_ALIAS(watchdog0), okay) */
/* No vendor implementation, no-op for historical reasons */
#define MCUBOOT_WATCHDOG_FEED()         \
    do {                                \
    } while (0)
#endif
#else  /* CONFIG_BOOT_WATCHDOG_FEED */
/* Not enabled, no feed activity */
#define MCUBOOT_WATCHDOG_FEED()         \
    do {                                \
    } while (0)

#endif /* CONFIG_BOOT_WATCHDOG_FEED */

#ifndef MCUBOOT_WATCHDOG_SETUP
#define MCUBOOT_WATCHDOG_SETUP()
#endif

#define MCUBOOT_CPU_IDLE() \
  if (!IS_ENABLED(CONFIG_MULTITHREADING)) { \
    k_cpu_idle(); \
  }

#endif /* __MCUBOOT_CONFIG_H__ */
