/*
 * Copyright (c) Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file sdc_hci_evt.h
 *
 * @defgroup sdc_hci_evt SoftDevice Controller HCI events
 * @ingroup sdc_hci
 *
 * @{
 */

#ifndef SDC_HCI_EVT_H__
#define SDC_HCI_EVT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <nrfx.h>

/**
 * @defgroup HCI_TYPES Types
 * @{
 */

/** @brief HCI Event Code values. */
enum sdc_hci_event
{
    /** @brief Number Of Completed Packets. */
    SDC_HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS = 0x13,
};

/** @brief Number Of Completed Packets array parameters. */
typedef struct __PACKED __ALIGN(1)
{
    uint16_t conn_handle;
    uint16_t num_completed_packets;
} sdc_hci_number_of_completed_packets_array_params_t;

/** @} end of HCI_TYPES */

/**
 * @defgroup HCI_EVENTS Events
 * @{
 */

/** @brief Number Of Completed Packets.
 *
 * The description below is extracted from Core_v6.2,
 * Vol 4, Part E, Section 7.7.19
 *
 * This event is used by the Controller to indicate to the Host how many HCI Data
 * packets or HCI ISO Data packets have been completed for each Handle since the
 * previous HCI_Number_Of_Completed_Packets event was sent to the Host. This means
 * that the corresponding buffer space has been freed in the Controller and is available
 * for new packets to be sent. Based on this information and the return parameters of
 * the HCI_Read_Buffer_Size and HCI_LE_Read_Buffer_Size commands, the Host can
 * determine for which Handles the following HCI packets should be sent to the Controller.
 * The HCI_Number_Of_Completed_Packets event shall not specify a given Handle
 * before the Controller has sent the event indicating that the corresponding connection
 * or BIG has been created or after it has sent the event indicating disconnection of
 * the corresponding connection or indicating that the BIG has been terminated. While
 * the Controller has HCI Data packets or HCI ISO Data packets in its buffer, it shall
 * keep sending the HCI_Number_Of_Completed_Packets event to the Host at least
 * periodically, until it finally reports that all the pending packets have been completed.
 * The rate with which this event is sent is manufacturer specific.
 *
 * Note: HCI_Number_Of_Completed_Packets events will not report on synchronous
 * Connection_Handles if synchronous Flow Control is disabled. (See Section 7.3.36 and
 * Section 7.3.37.)
 */
typedef struct __PACKED __ALIGN(1)
{
    uint8_t num_handles;
    sdc_hci_number_of_completed_packets_array_params_t handles[];
} sdc_hci_event_number_of_completed_packets_t;

/** @} end of HCI_EVENTS */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDC_HCI_EVT_H__ */
