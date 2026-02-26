/*
 * Copyright (c) 2018-2020 Arm Limited
 * Copyright (c) 2020 Linaro Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BOOT_STATUS_H__
#define __BOOT_STATUS_H__

#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * The shared data between boot loader and runtime SW is TLV encoded. The
 * shared data is stored in a well known location in memory and this is a
 * contract between boot loader and runtime SW.
 *
 * The structure of shared data must be the following:
 *  - At the beginning there must be a header: struct shared_data_tlv_header
 *    This contains a magic number and a size field which covers the entire
 *    size of the shared data area including this header.
 *  - After the header there come the entries which are composed from an entry
 *    header structure: struct shared_data_tlv_entry and the data. In the entry
 *    header is a type field (tly_type) which identify the consumer of the
 *    entry in the runtime SW and specify the subtype of that data item. There
 *    is a size field (tlv_len) which covers the size of the the data. After
 *    this structure comes the actual data.
 *
 *  - Arbitrary number and size of data entry can be in the shared memory area.
 *
 * This table gives of overview about the tlv_type field in the entry header.
 * The tlv_type always composed from a major and minor number. Major number
 * identifies the addressee in runtime SW, who should process the data entry.
 * Minor number used to encode more info about the data entry. The actual
 * definition of minor number could change per major number.
 *
 * In case of boot status data, which can be processed by an attestation
 * service the minor number is split further to two part: sw_module and claim.
 * The sw_module identifies the SW component in the system which the data item
 * belongs to and the claim part identifies the exact type of the data.
 *
 * |---------------------------------------|
 * |            tlv_type (16)              |
 * |---------------------------------------|
 * | tlv_major(4)|      tlv_minor(12)      |
 * |---------------------------------------|
 * | MAJOR_IAS   | sw_module(6) | claim(6) |
 * |---------------------------------------|
 */

/* General macros to handle TLV type */
#define MAJOR_MASK 0xF     /* 4  bit */
#define MAJOR_POS  12      /* 12 bit */
#define MINOR_MASK 0xFFF   /* 12 bit */

#define SET_TLV_TYPE(major, minor) \
        (((uint16_t)((major) & MAJOR_MASK) << MAJOR_POS) \
        | ((minor) & MINOR_MASK))
#define GET_MAJOR(tlv_type) ((uint16_t)(tlv_type) >> MAJOR_POS)
#define GET_MINOR(tlv_type) ((tlv_type) & MINOR_MASK)

/* Magic value which marks the beginning of shared data area in memory */
#define SHARED_DATA_TLV_INFO_MAGIC    0x2016

/* Initial attestation specific macros */

/**
 * Major numbers (4 bit) to identify the
 * consumer of shared data in runtime SW.
 */
#define TLV_MAJOR_IAS      0x1
#define TLV_MAJOR_BLINFO   0x2

/* Initial attestation: Claim per SW components / SW modules */
/* Bits: 0-2 */
#define SW_VERSION       0x00
#define SW_SIGNER_ID     0x01
/* Reserved              0x02 */
#define SW_TYPE          0x03
/* Bits: 3-5 */
#define SW_MEASURE_VALUE 0x08
#define SW_MEASURE_TYPE  0x09
#define SW_BOOT_RECORD   0x3F

#define MODULE_POS 6               /* 6 bit */
#define CLAIM_MASK 0x3F            /* 6 bit */
#define MEASUREMENT_CLAIM_POS 3    /* 3 bit */

#define GET_IAS_MODULE(tlv_type) ((uint16_t)GET_MINOR(tlv_type) >> MODULE_POS)
#define GET_IAS_CLAIM(tlv_type)  (GET_MINOR(tlv_type) & CLAIM_MASK)
#define SET_IAS_MINOR(sw_module, claim) \
        (((uint16_t)(sw_module) << MODULE_POS) | (claim))

/* Bootloader information */
#define BLINFO_MODE                 0x00
#define BLINFO_SIGNATURE_TYPE       0x01
#define BLINFO_RECOVERY             0x02
#define BLINFO_RUNNING_SLOT         0x03
#define BLINFO_BOOTLOADER_VERSION   0x04
#define BLINFO_MAX_APPLICATION_SIZE 0x05 /* Same as BLINFO_MAX_APPLICATION_SIZE_IMAGE_0, legacy name */
#define BLINFO_MAX_APPLICATION_SIZE_IMAGE_0 BLINFO_MAX_APPLICATION_SIZE
#define BLINFO_MAX_APPLICATION_SIZE_IMAGE_1 0x06
#define BLINFO_MAX_APPLICATION_SIZE_IMAGE_2 0x07
#define BLINFO_MAX_APPLICATION_SIZE_IMAGE_3 0x08
#define BLINFO_MAX_APPLICATION_SIZE_IMAGE_4 0x09
#define BLINFO_SECURITY_COUNTER_IMAGE_0 0x10
#define BLINFO_SECURITY_COUNTER_IMAGE_1 0x11
#define BLINFO_SECURITY_COUNTER_IMAGE_2 0x12
#define BLINFO_SECURITY_COUNTER_IMAGE_3 0x13
#define BLINFO_SECURITY_COUNTER_IMAGE_4 0x14

enum mcuboot_mode {
    MCUBOOT_MODE_SINGLE_SLOT,
    MCUBOOT_MODE_SWAP_USING_SCRATCH,
    MCUBOOT_MODE_UPGRADE_ONLY,
    MCUBOOT_MODE_SWAP_USING_MOVE,
    MCUBOOT_MODE_DIRECT_XIP,
    MCUBOOT_MODE_DIRECT_XIP_WITH_REVERT,
    MCUBOOT_MODE_RAM_LOAD,
    MCUBOOT_MODE_FIRMWARE_LOADER,
    MCUBOOT_MODE_SINGLE_SLOT_RAM_LOAD,
    MCUBOOT_MODE_SWAP_USING_OFFSET,
};

enum mcuboot_signature_type {
    MCUBOOT_SIGNATURE_TYPE_NONE,
    MCUBOOT_SIGNATURE_TYPE_RSA,
    MCUBOOT_SIGNATURE_TYPE_ECDSA_P256,
    MCUBOOT_SIGNATURE_TYPE_ED25519
};

enum mcuboot_recovery_mode {
    MCUBOOT_RECOVERY_MODE_NONE,
    MCUBOOT_RECOVERY_MODE_SERIAL_RECOVERY,
    MCUBOOT_RECOVERY_MODE_DFU,
};

/**
 * Shared data TLV header.  All fields in little endian.
 *
 *    -----------------------------------
 *    | tlv_magic(16) | tlv_tot_len(16) |
 *    -----------------------------------
 */
struct shared_data_tlv_header {
    uint16_t tlv_magic;
    uint16_t tlv_tot_len; /* size of whole TLV area (including this header) */
};

#define SHARED_DATA_HEADER_SIZE sizeof(struct shared_data_tlv_header)

/**
 * Shared data TLV entry header format. All fields in little endian.
 *
 *    -------------------------------
 *    | tlv_type(16) |  tlv_len(16) |
 *    -------------------------------
 *    |         Raw data            |
 *    -------------------------------
 */
struct shared_data_tlv_entry {
    uint16_t tlv_type;
    uint16_t tlv_len; /* TLV data length (not including this header). */
};

#define SHARED_DATA_ENTRY_HEADER_SIZE sizeof(struct shared_data_tlv_entry)
#define SHARED_DATA_ENTRY_SIZE(size) (size + SHARED_DATA_ENTRY_HEADER_SIZE)

/* Structure to store the boot data for the runtime SW. */
struct shared_boot_data {
    struct shared_data_tlv_header header;
    uint8_t data[];
};

#ifdef __cplusplus
}
#endif

#endif /* __BOOT_STATUS_H__ */
